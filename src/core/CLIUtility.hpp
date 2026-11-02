#ifndef NLM_CLI_UTILITY_H
#define NLM_CLI_UTILITY_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include "core/Config/Config.hpp"

namespace nlm {

enum class LogLevel { Error, Warning, Info, Debug };

enum class OutputFormat { Simple, Detailed, JSON, CSV, YAML };

struct CLIOptions {
    std::string configFile = "configs/default.cfg";
    LogLevel logLevel = LogLevel::Info;
    OutputFormat outputFormat = OutputFormat::Detailed;
    bool verbose = false;
    bool debug = false;
    bool help = false;
    std::string outputFile;
    std::vector<std::string> experimentArgs;
};

class CLIUtility {
public:
    static bool parseArguments(int argc, char** argv, CLIOptions& options);
    static void printHelp(const std::string& programName);
    static void printVersion();
    static LogLevel stringToLogLevel(const std::string& levelStr);
    static std::string logLevelToString(LogLevel level);
    static OutputFormat stringToOutputFormat(const std::string& formatStr);
    static std::string outputFormatToString(OutputFormat format);
    static std::shared_ptr<Config> loadConfig(const std::string& configFile, 
                                            const std::vector<std::string>& experimentArgs);
    static void setupLogging(LogLevel level, bool verbose, bool debug);
    static void printOutput(const std::string& data, OutputFormat format, 
                           const std::string& outputFile = "");
    static bool validateOptions(const CLIOptions& options);
    static std::string generateHelpText(const std::string& programName);
};

// Helper function to run NLM Phase 2 demonstration
void runNLMPhase2Demonstration(const std::shared_ptr<Config>& config);

// Main demonstration function for enhanced CLI
void runDemonstration(const CLIOptions& options);

} // namespace nlm

#endif // NLM_CLI_UTILITY_H