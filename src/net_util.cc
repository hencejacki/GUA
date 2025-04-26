#include "net_util.hpp"

NetUtil::NetUtil() : domain_(""), port_(0), timeout_(0), max_retry_(0), ssl_ctx_(nullptr), status_(ParseStatus::kParseStatusLine)
{
}

NetUtil::~NetUtil()
{
    if (ssl_ctx_) {
        SSL_CTX_free(ssl_ctx_);
        ssl_ctx_ = nullptr;
    }
    EVP_cleanup();
}

NetUtil &NetUtil::GetInstance()
{
    static NetUtil ins;
    return ins;
}

void NetUtil::Init(const char *domain, uint16_t port, int timeout, int max_retry)
{
    domain_    = domain;
    port_      = port;
    timeout_   = timeout;
    max_retry_ = max_retry;

    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    
    ssl_ctx_ = SSL_CTX_new(TLS_client_method());
    if (!ssl_ctx_) {
        ERR_print_errors_fp(stderr);
    }
}

void NetUtil::constructGetRequest(const char* endpoint, std::string &req)
{
    char buffer[1024] = {0};
    snprintf(buffer, 1024, "GET %s HTTP/1.1\r\n", endpoint);
    req.append(buffer);
    memset(buffer, 0, 1024);
    snprintf(buffer, 1024, "Host: %s\r\nAccept: application/vnd.github+json\r\nUser-Agent: github-activity\r\nConnection: close\r\n\r\n", domain_.c_str());
    req.append(buffer);
#ifdef _DEBUG
    fprintf(stdout, req.c_str());
#endif
}

void NetUtil::parseHttpResponse(const std::string& resp, std::string &content)
{
    size_t resp_len = resp.size();
    const char* p = resp.c_str();
    int parsed_bytes = 0;
    const char CRLF[] = "\r\n";
    while (status_ != ParseStatus::kParseFinish) {
        const char* line_end = std::search(p + parsed_bytes, p + resp_len, CRLF, CRLF + 2);
        std::string line(p + parsed_bytes, line_end);
        switch (status_)
        {
        case ParseStatus::kParseStatusLine:
            parseStatusLine(line);
            break;
        case ParseStatus::kParseHeaderField:
            parseHeaderField(line);
            if (resp_len - parsed_bytes <= 2) {
                status_ = ParseStatus::kParseFinish;
            }
            break;
        case ParseStatus::kParseMessageBody:
            parseMessageBody(line);
            break;
        default:
            break;
        }
        parsed_bytes += (line_end + 2 - (p + parsed_bytes));
    }
    if (http_resp_.status_code == "200") {
        content = http_resp_.body;
#ifdef _DEBUG
        fprintf(stderr, "Response body: [%s].\n", content.c_str());
#endif // _DEBUG
    } else {
        fprintf(stderr, "Got error response, status code: [%s], msg: [%s].\n", http_resp_.status_code.c_str(), http_resp_.status_msg.c_str());
    }
}

void NetUtil::parseStatusLine(const std::string &line)
{
    std::regex pattern("^HTTP/([^ ]*) ([^ ]*) ([^ ]*)$", std::regex_constants::optimize);
    std::smatch match;
    if (std::regex_match(line, match, pattern)) {
        http_resp_.http_version = match[1];
        http_resp_.status_code  = match[2];
        http_resp_.status_msg   = match[3];
        status_ = ParseStatus::kParseHeaderField;
    } else {
        status_ = ParseStatus::kParseFinish;
#ifdef _DEBUG
        fprintf(stderr, "Failed to parse status line: [%s].\n", line.c_str());
#endif // _DEBUG
    }
}

void NetUtil::parseHeaderField(const std::string &line)
{
    std::regex pattern("^([^ ]*): ?(.*)$", std::regex_constants::optimize);
    std::smatch match;
    if (std::regex_match(line, match, pattern)) {
        http_resp_.header[match[1]] = match[2];
    } else {
        status_ = ParseStatus::kParseMessageBody;
    }
}

void NetUtil::parseMessageBody(const std::string &line)
{
    http_resp_.body = line;
    status_ = ParseStatus::kParseFinish;
}

int NetUtil::Get(const char *endpoint, std::string& resp) {
    int sock = -1;
    SSL* ssl = nullptr;
    try {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            throw std::runtime_error("Create socket failed");
        }
        
        struct timeval tv;
        tv.tv_sec = timeout_;
        tv.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
        
        struct sockaddr_in addr = {0};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        
        
        struct hostent *he = gethostbyname(domain_.c_str());
        if (!he) {
            close(sock);
            throw std::runtime_error("DNS resolution failed");
        }
        memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
        
        if (connect(sock, (sockaddr*)&addr, sizeof(addr))) {
            close(sock);
            throw std::runtime_error("Connection failed");
        }

        // SSL connect
        ssl = SSL_new(ssl_ctx_);
        SSL_set_fd(ssl, sock);

        if (SSL_connect(ssl) != 1) {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("SSL handshake failed");
        }

        std::string request;
        constructGetRequest(endpoint, request);
        
        int written = SSL_write(ssl, request.c_str(), request.size());
        if (written <= 0) {
            throw std::runtime_error("SSL write failed");
        }

        char buffer[4096];
        std::string tmp;
        int bytesRead;
        while ((bytesRead = SSL_read(ssl, buffer, sizeof(buffer))) > 0) {
            tmp.append(buffer, bytesRead);
        }
#ifdef _DEBUG
        fprintf(stderr, "%s\n", tmp.c_str());
#endif // _DEBUG
        parseHttpResponse(tmp, resp);
        SSL_shutdown(ssl);
        close(sock);
        SSL_free(ssl);
        return 0;
    } catch (std::exception& e) {
        if (sock != -1) close(sock);
        fprintf(stderr, e.what());
        return -1;
    }
}
