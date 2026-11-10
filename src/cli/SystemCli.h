// System CLI Tools
#ifndef NLM_SYSTEM_CLI_H
#define NLM_SYSTEM_CLI_H

#include <map>
#include <string>

class SystemCli {
public:
    static void monitorSystem(const std::map<std::string, std::string>& options);
    static void profileSystem(const std::map<std::string, std::string>& options);
    static void analyzeSystem(const std::map<std::string, std::string>& options);
    static void optimizeSystem(const std::map<std::string, std::string>& options);
    static void runSystemBenchmark(const std::map<std::string, std::string>& options);
    static void compareSystems(const std::map<std::string, std::string>& options);
    static void generateSystemReport(const std::map<std::string, std::string>& options);
};

#endif // NLM_SYSTEM_CLI_H
