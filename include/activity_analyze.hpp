#ifndef ACTIVITY_ANALYZE_HPP
#define ACTIVITY_ANALYZE_HPP

#include <cstdio>
#include <string>
#include <vector>
#include "net_util.hpp"

enum class EventType {
    kUnknownEvent = -1,
    kCommitCommentEvent,
    kCreateEvent,
    kDeleteEvent,
    kForkEvent,
    kGollumEvent,
    kIssueCommentEvent,
    kIssuesEvent,
    kMemberEvent,
    kPublicEvent,
    kPullRequestEvent,
    kPullRequestReviewEvent,
    kPullRequestReviewCommentEvent,
    kPullRequestReviewThreadEvent,
    kPushEvent,
    kReleaseEvent,
    kSponsorshipEvent,
    kWatchEvent
};

struct GitHubUser {
    int64_t id;
    std::string login;
    std::string display_login;
    std::string gravatar_id;
    std::string url;
    std::string avatar_url;
};

struct GitHubRepo {
    int64_t id;
    std::string name;
    std::string url;
};

struct CommitAuthor {
    std::string email;
    std::string name;
};

struct Commit {
    std::string sha;
    CommitAuthor author;
    std::string message;
    bool distinct;
    std::string url;
};

struct PushEventPayload {
    int64_t repository_id;
    int64_t push_id;
    int size;
    int distinct_size;
    std::string ref;
    std::string head;
    std::string before;
    std::vector<Commit> commits;
};

struct CreateEventPayload {
    std::string ref; // nullable (represented as empty string)
    std::string ref_type;
    std::string master_branch;
    std::string description;
    std::string pusher_type;
};

struct GitHubEvent {
    std::string id;
    EventType type = EventType::kUnknownEvent;
    std::string type_literal;
    
    GitHubUser actor;
    GitHubRepo repo;
    
    // Union-like payload (use based on event type)
    PushEventPayload push_payload;
    CreateEventPayload create_payload;
    
    bool is_public;
    std::string created_at;
    
    // Helper methods to check event type
    bool isPushEvent() const { return type == EventType::kPushEvent; }
    bool isCreateEvent() const { return type == EventType::kCreateEvent; }
};

// Main container for the response
struct GitHubActivity {
    std::vector<GitHubEvent> events;
};

struct ActivityBaseInfo {
    EventType event_type;
    int nums;
    std::string repo_name;
};

struct StatisticResult {
    std::vector<ActivityBaseInfo> infos;
};


class ActivityAnalyze {
public:
    ActivityAnalyze(const ActivityAnalyze&) = delete;
    ActivityAnalyze(const ActivityAnalyze&&) = delete;
    ActivityAnalyze& operator=(const ActivityAnalyze&) = delete;
    ActivityAnalyze& operator=(const ActivityAnalyze&&) = delete;
    ~ActivityAnalyze();

    static ActivityAnalyze& GetInstance();

    void Start(const char* /*user_name*/);

private:
    ActivityAnalyze();

    void parseEvents(const HJson* /*node*/);

    void analyzeActivities();

private:
    GitHubActivity activity_;
    StatisticResult statistic_res_;
};

#endif // ACTIVITY_ANALYZE_HPP