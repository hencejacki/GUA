#ifndef ACTIVITY_ANALYZE_HPP
#define ACTIVITY_ANALYZE_HPP

#include <cstdio>

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

private:

};

#endif // ACTIVITY_ANALYZE_HPP