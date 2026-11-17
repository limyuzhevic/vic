#pragma once

#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class CommandLineInterface {
public:
    struct CLIOptions {
        std::string configFile;
        std::string logLevel;
        bool verbose;
        bool quiet;
        size_t numTrials;
        std::string outputFile;
        bool help;
        bool version;
        std::vector<std::string> extraArgs;
    };
    
    static CLIOptions parseArgs(int argc, char* argv[]);
    static void printUsage(const char* programName);
    static void printVersion();
    static std::shared_ptr<Config> createConfigFromOptions(const CLIOptions& options);
    static void setupLogging(const CLIOptions& options);
    static void saveResultsToFile(const std::string& filename, const std::string& results);
};

void printBanner(const std::string& title = "NLM - Neural Learning Machine");
void printHeader(const std::string& title);
void printSubHeader(const std::string& title);
void printSection(const std::string& title);
void printResult(const std::string& name, float value, float max = 1.0f);
void printResult(const std::string& name, double value, double max = 1.0f);
void printResult(const std::string& name, size_t value, size_t max = 1);
void printResult(const std::string& name, bool value);
void printSuccess(const std::string& message);
void printWarning(const std::string& message);
void printError(const std::string& message);
void printInfo(const std::string& message);
std::string formatFloat(float value, int precision = 3);
std::string formatPercentage(float value, float max = 1.0f);
std::string getTimestamp();
}  // namespace nlm
