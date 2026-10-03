// NLM Command Line Interface
// Main CLI class for the enhanced NLM command-line interface

#pragma once

#include "CLICommands.hpp"
#include "CLIUtils.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>

namespace nlm {
namespace cli {

class NLMCommandLine {
public:
    NLMCommandLine();
    ~NLMCommandLine();
    
    // Disable copying, enable moving
    NLMCommandLine(const NLMCommandLine&) = delete;
    NLMCommandLine& operator=(const NLMCommandLine&) = delete;
    NLMCommandLine(NLMCommandLine&&) noexcept;
    NLMCommandLine& operator=(NLMCommandLine&&) noexcept;
    
    // Initialize CLI system
    bool initialize();
    
    // Main CLI entry point
    void start();
    
    // Shutdown CLI system
    void shutdown();
    
    // Configuration access
    std::string getConfigValue(const std::string& key, const std::string& defaultValue = "") const;
    void setConfigValue(const std::string& key, const std::string& value);
    
    // Command execution
    bool executeCommand(const std::string& commandName, const std::vector<std::string>& args = {});
    
    // Help system
    void printBanner() const;
    void printHelp() const;
    void printCommandHelp(const std::string& commandName) const;
    
    // Version information
    void printVersion() const;
    
    // Logging
    void logMessage(const std::string& message, LogLevel level = LogLevel::Info);
    
    // Check if CLI is running
    bool isRunning() const { return running; }
    bool isInitialized() const { return initialized; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool initialized;
    bool running;
    LogLevel currentLogLevel;
    std::ofstream logFile;
    std::unordered_map<std::string, std::string> config;
    std::vector<std::string> commandLineArgs;
    std::unordered_map<std::string, std::string> helpSystemInfo;
    
    // Private helper methods
    void setupSignalHandlers();
    void initializeDefaultConfig();
    bool loadConfig();
    void setupLogging();
    void initializeCommands();
    void setupHelpSystem();
    void parseCommandLineArgs();
    void processNextCommand();
};

} // namespace cli
} // namespace nlm