// Command-line interface (CLI) for NLM (Neural Learning Machine)
// Provides comprehensive simulation control and management functionality

#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <memory>
#include <chrono>
#include <fstream>

namespace nlm {

// Command result from processing arguments
CommandResult processArguments(int argc, char** argv);

// Standard CLI command handler type
using CLICommandHandler = std::function<CommandResult(int, char**)>;
class CLIParser {
public:
    CLIParser();
    ~CLIParser();
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Get command name
    const std::string& getCommand() const { return currentCommand_; }
    
    // Get argument values
    const std::string& getArg(size_t index) const;
    std::string getFlagValue(const std::string& flag) const;
    
    // Check if flag is present
    bool hasFlag(const std::string& flag) const;
    
    // Get verbose mode
    bool isVerbose() const { return verboseMode_; }
    
    // Get configuration file
    const std::string& getConfigFile() const { return configFile_; }
    
    // Get current command
    const Command* getCurrentCommand() const { return currentCommand_.empty() ? nullptr : findCommand(currentCommand_); }
    
    // Show help
    void showHelp() const;
    
    // Show version
    void showVersion() const;
    
    // Get help text for command
    std::string getCommandHelp(const std::string& commandName) const;
    
private:
    bool findCommand(const std::string& name) const;
    
    std::string currentCommand_;
    std::vector<std::string> args_;
    std::unordered_map<std::string, std::string> flagValues_;
    bool verboseMode_;
    std::string configFile_;
    std::vector<Command> commands_;
    
    // Version information
    static const std::string version_;
    static const std::string buildInfo_;
};

} // namespace nlm