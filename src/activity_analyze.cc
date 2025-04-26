#include "activity_analyze.hpp"

static std::unordered_map<std::string, EventType> event_type_map = {
    {"CreateEvent", EventType::kCreateEvent},
    {"PushEvent", EventType::kPushEvent},
};

ActivityAnalyze::ActivityAnalyze() {
    NetUtil::GetInstance().Init("api.github.com", 443);
}

ActivityAnalyze::~ActivityAnalyze() {

}

ActivityAnalyze& ActivityAnalyze::GetInstance() {
    static ActivityAnalyze ins;
    return ins;
}

void ActivityAnalyze::parseEvents(const HJson *event_array)
{
    HJson* p = event_array->child;
    HJson* actor = nullptr;
    HJson* repo = nullptr;
    HJson* payload = nullptr;
    HJson* commits = nullptr;
    HJson* child = nullptr;
    HJson* author = nullptr;
    while (p) {
        GitHubEvent event{};
        std::string event_type = p->get("type")->ToCString();
        auto iter = event_type_map.find(event_type);
        if (iter != event_type_map.end()) {
            event.type = event_type_map[event_type];
        }
        event.type_literal = event_type;
        event.id = p->get("id")->ToCString();
        actor = p->get("actor");
        if (actor) {
            event.actor.id = actor->get("id")->ToInt();
            event.actor.login = actor->get("login")->ToCString();
            event.actor.display_login = actor->get("display_login")->ToCString();
            event.actor.gravatar_id = actor->get("gravatar_id")->ToCString();
            event.actor.url = actor->get("url")->ToCString();
            event.actor.avatar_url = actor->get("avatar_url")->ToCString();
        }
        repo = p->get("repo");
        if (repo) {
            event.repo.id = repo->get("id")->ToInt();
            event.repo.name = repo->get("name")->ToCString();
            event.repo.url = repo->get("url")->ToCString();
        }
        payload = p->get("payload");
        if (payload) {
            switch (event.type)
            {
            case EventType::kCreateEvent:
                event.create_payload.ref = payload->get("ref")->ToCString();
                event.create_payload.ref_type = payload->get("ref_type")->ToCString();
                event.create_payload.master_branch = payload->get("master_branch")->ToCString();
                event.create_payload.description = payload->get("description")->ToCString();
                event.create_payload.pusher_type = payload->get("pusher_type")->ToCString();
                break;
            case EventType::kPushEvent:
                event.push_payload.repository_id = payload->get("repository_id")->ToInt();
                event.push_payload.push_id = payload->get("push_id")->ToInt();
                event.push_payload.size = payload->get("size")->ToInt();
                event.push_payload.distinct_size = payload->get("distinct_size")->ToInt();
                event.push_payload.ref = payload->get("ref")->ToCString();
                event.push_payload.head = payload->get("head")->ToCString();
                event.push_payload.before = payload->get("before")->ToCString();
                commits = payload->get("commits");
                child = commits->child;
                while (child) {
                    Commit commit{};
                    commit.sha = child->get("sha")->ToCString();
                    author = child->get("author");
                    if (author) {
                        commit.author.name = author->get("name")->ToCString();
                        commit.author.email = author->get("email")->ToCString();    
                    }
                    commit.message = child->get("message")->ToCString();
                    commit.distinct = child->get("distinct")->ToBoolean();
                    commit.url = child->get("url")->ToCString();    
                    event.push_payload.commits.push_back(commit);
                    child = child->next;
                }
                break;
            default:
                fprintf(stderr, "Not implement event type yet.\n");
                break;
            }
        }
        event.is_public = p->get("public")->ToBoolean();
        event.created_at = p->get("created_at")->ToCString();
        activity_.events.push_back(event);
        p = p->next;
    }
}

void ActivityAnalyze::analyzeActivities()
{
    printf("Analyze result: \n");
    for (auto iter = activity_.events.begin()
        ; iter != activity_.events.end()
        ; ++iter) 
    {
        // Pretty print
        switch (iter->type)
        {
        case EventType::kCreateEvent:
            fprintf(stdout, "- Create an repository named %s.\n", iter->repo.name.c_str());
            break;
        case EventType::kPushEvent:
            fprintf(stdout, "- Push %ld commits to repo %s.\n", iter->push_payload.commits.size(), iter->repo.name.c_str());
            break;
        default:
        fprintf(stderr, "Not implement event type yet.\n");
            break;
        }
    }
}

void ActivityAnalyze::Start(const char* user_name) {
    // Do http request
    char endpoint[64] = {0};
    snprintf(endpoint, 256, "/users/%s/events", user_name);
    std::string resp = "";
    NetUtil::GetInstance().Get(endpoint, resp);
    // Parse response
    HJson* event_array = HJson_parse(resp.c_str());
    parseEvents(event_array);
    // Statistic result
    analyzeActivities();
    HJson_delete(event_array);
}
