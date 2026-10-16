// NLM Command Line Interface
// Command-line tools and utilities for NLM (Neural Learning Machine)

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>

namespace nlm {

// Forward declarations
class Brain;
class Config;

class NLMCommandLine {
public:
    static void printBanner();
    static void printHelp();
    static void printVersion();
    static void printError(const std::string& message);
    static void printWarning(const std::string& message);
    static void printInfo(const std::string& message);
    
    static std::string getGitCommit();
    static std::string getTimestamp();
    static void printFormattedTime(const std::chrono::duration<double>& duration);
    static std::string formatNumber(size_t num);
    static std::string formatFloat(float value);
    
    static bool runSimulation(const std::map<std::string, std::string>& options);
    static bool createBrain(const std::map<std::string, std::string>& options);
    static bool exportBrain(const std::map<std::string, std::string>& options);
    static bool importBrain(const std::map<std::string, std::string>& options);
    static bool runConfigCommand(const std::map<std::string, std::string>& options);
    static void printSimulationResults(std::shared_ptr<Brain> brain);
    static bool runTests();
    
    static std::map<std::string, std::string> parseOptions(int argc, char** argv);
    static bool runCommand(const std::string& command, const std::map<std::string, std::string>& options);
};

} // namespace nlm