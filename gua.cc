#include "activity_analyze.hpp"
#include <iostream>

int main(int argc, char const *argv[])
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <username>" << std::endl;
        return 1;
    }
    const char* user_name = argv[1];
    ActivityAnalyze::GetInstance().Start(user_name);
    return 0;
}
