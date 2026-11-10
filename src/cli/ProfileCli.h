// Profile CLI Tools
#ifndef NLM_PROFILE_CLI_H
#define NLM_PROFILE_CLI_H

#include <map>
#include <string>

class ProfileCli {
public:
    static void startProfiling(const std::map<std::string, std::string>& options);
    static void stopProfiling(const std::map<std::string, std::string>& options);
    static void analyzeProfile(const std::map<std::string, std::string>& options);
    static void runBenchmark(const std::map<std::string, std::string>& options);
    static void optimizePerformance(const std::map<std::string, std::string>& options);
    static void monitorPerformance(const std::map<std::string, std::string>& options);
    static void compareProfiles(const std::map<std::string, std::string>& options);
    static void exportProfile(const std::map<std::string, std::string>& options);
};

#endif // NLM_PROFILE_CLI_H
