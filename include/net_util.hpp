#ifndef NET_UTIL_HPP
#define NET_UTIL_HPP

#include <string>
#include <cstring>
#include <stdexcept>
#include <unordered_map>
#include <algorithm>
#include <regex>
#include "hjson.hpp"
#include "sys/socket.h"
#include "arpa/inet.h"
#include "netinet/in.h"
#include "unistd.h"
#include "sys/select.h"
#include "netdb.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

enum class ParseStatus {
    kParseStatusLine,
    kParseHeaderField,
    kParseMessageBody,
    kParseFinish
};

struct HttpResponse {
    std::string http_version;
    std::string status_code;
    std::string status_msg;
    std::unordered_map<std::string, std::string> header;
    std::string body;
};

class NetUtil {
public:
    NetUtil(const NetUtil&) = delete;
    NetUtil(const NetUtil&&) = delete;
    NetUtil& operator=(const NetUtil&) = delete;
    NetUtil& operator=(const NetUtil&&) = delete;
    ~NetUtil();

    static NetUtil& GetInstance();

    void Init(const char* domain, uint16_t port, int timeout = 3, int max_retry = 3);

    int Get(const char* endpoint, std::string& resp);

private:
    NetUtil();

    void constructGetRequest(const char* endpoint, std::string& req);

    void parseHttpResponse(const std::string& resp, std::string& content);

    void parseStatusLine(const std::string& line);

    void parseHeaderField(const std::string& line);

    void parseMessageBody(const std::string& line);

private:
    std::string domain_;
    uint16_t port_;
    int timeout_;
    int max_retry_;
    SSL_CTX* ssl_ctx_;
    HttpResponse http_resp_;
    ParseStatus status_;
};

#endif // NET_UTIL_HPP