#include  "activity_analyze.hpp"

ActivityAnalyze::ActivityAnalyze() {

}

ActivityAnalyze::~ActivityAnalyze() {

}

ActivityAnalyze& ActivityAnalyze::GetInstance() {
    static ActivityAnalyze ins;
    return ins;
}

void ActivityAnalyze::Start(const char* user_name) {
    // TODO: Do http request

    // TODO: Parse response

    // TODO: Statistic result

    // TODO: Pretty print
}
