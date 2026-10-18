// CLI Engine for Command Execution
//
// This file provides the CLI execution engine that manages command registration,
// execution, and error handling for NLM command-line tools.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <chrono>

#include "CLICommand.hpp"
#include "core/Config/Config.hpp"
#include "core/ArgumentParser.hpp"

namespace nlm {

class CLIEngine {
public:
    CLIEngine();
    ~CLIEngine() = default;
    
    // Command registration
    void registerCommand(std::unique_ptr<CLICommand> command);
    void registerCommandsFromDirectory(const std::string& directory);
    
    // Command execution
    bool executeCommand(const std::string& commandName, 
                       const std::vector<std::string>& args,
                       Config& config);
    
    // Command discovery
    std::vector<std::string> getAvailableCommands() const;
    std::vector<std::string> getCommandsByType(CLICommand::CommandType type) const;
    std::vector<std::string> getCommandsByCategory(const std::string& category) const;
    
    // Help system
    void displayHelp(const std::string& commandName = "") const;
    void displayCommandList(CLICommand::CommandType type) const;
    void displayCommandCategories() const;
    
    // Configuration management
    bool loadConfiguration(const std::string& configFile, Config& config);
    bool saveConfiguration(const std::string& configFile, const Config& config);
    
    // Error handling and logging
    void setVerbose(bool verbose);
    void setQuiet(bool quiet);
    bool isVerbose() const;
    bool isQuiet() const;
    
    void log(const std::string& message, bool force = false) const;
    void logError(const std::string& error) const;
    void logWarning(const std::string& warning) const;
    
    // Performance monitoring
    void startTiming();
    double stopTiming() const;
    
    // Batch processing
    bool executeBatch(const std::vector<std::string>& commands,
                     Config& config);
    
    // Parallel execution
    bool executeParallel(const std::string& command,
                        const std::vector<std::vector<std::string>>& argSets,
                        Config& config);
    
private:
    // Command storage
    std::map<std::string, std::unique_ptr<CLICommand>> commands_;
    
    // Configuration
    bool verbose_;
    bool quiet_;
    
    // Performance monitoring
    std::chrono::steady_clock::time_point startTime_;
    
    // Command categories
    std::map<CLICommand::CommandType, std::vector<std::string>> commandsByType_;
    std::map<std::string, std::vector<std::string>> commandsByCategory_;
    
    // Internal helper methods
    void categorizeCommand(const std::string& commandName, 
                          CLICommand::CommandType type,
                          const std::string& category);
    
    std::unique_ptr<CLICommand> createCommand(const std::string& commandType) const;
};

} // namespace nlm