#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "Command.hpp"

namespace nlm {

class CommandLineParser {
public:
    struct CommandInfo {
        std::string name;
        std::string description;
        std::string usage;
        std::string help;
        std::function<bool(const std::vector<std::string>&)> advancedParser;
        std::function<bool(const Config&)> configExecutor;
        bool isAdvanced = false;
        std::vector<std::string> examples;
        std::unordered_map<std::string, std::string> parameters;
    };

    CommandLineParser();
    ~CommandLineParser();

    // Add a command
    void addCommand(std::unique_ptr<Command> command);
    
    // Parse command line arguments
    bool parse(int argc, char** argv, Config& config);
    
    // Get command by name
    Command* getCommand(const std::string& name);
    
    // Get help text
    std::string getGlobalHelp() const;
    std::string getCommandHelp(const std::string& commandName) const;
    
    // List all commands
    void listCommands() const;
    
    // Check if help flag is present
    bool hasHelpFlag(int argc, char**argv) const;
    
    // Check if version flag is present
    bool hasVersionFlag(int argc, char**argv) const;


private:
    // Parse advanced arguments (key=value, --help, --version, etc.)
    bool parseAdvancedArgs(int argc, char**argv, Config& config);
    
    // Parse command arguments
    bool parseCommandArgs(const std::string& commandName, int argc, char**argv, Config& config);
    
    // Build help text
    std::string buildCommandHelp(const CommandInfo& cmdInfo) const;
    
    // Build global help
    std::string buildGlobalHelp() const;
    
    // Display error message
    void displayError(const std::string& message) const;
    
private:
    std::vector<std::unique_ptr<Command>> commands_;
    std::unordered_map<std::string, CommandInfo> commandRegistry_;
};

} // namespace nlm