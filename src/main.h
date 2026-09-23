#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

// Command line interface options
struct CLIOptions {
    std::string configFile = "configs/default.cfg";
    bool showHelp = false;
    bool showVersion = false;
    bool quiet = false;
    bool verbose = false;
    std::string logLevel = "INFO";
    std::string logFile = "nlm.log";
    bool logToFile = false;
    std::string runMode = "standard"; // "standard", "benchmark", "test"
    int benchmarkSteps = 1000;
    bool enablePerformanceLogging = false;
    bool exportResults = false;
    std::string resultFile = "results.json";
};

// Command line parser
class CLIParser {
public:
    static CLIOptions parse(int argc, char** argv);
    static void printHelp();
    static void printVersion();
    static bool validateOptions(const CLIOptions& options);
    static std::string getLogLevelString(int level);
    static int getLogLevelValue(const std::string& level);
    
private:
    static std::unordered_map<std::string, std::string> parseConfigFile(const std::string& filepath);
    static bool loadConfigFromFile(const std::string& filepath, std::unordered_map<std::string, std::string>& config);
    static bool saveConfigToFile(const std::string& filepath, const std::unordered_map<std::string, std::string>& config);
    static void mergeConfigFiles(const std::string& baseConfig, const std::string& overrideConfig);
};

} // namespace nlm