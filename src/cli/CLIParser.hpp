#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace nlm {

class CLICommand;

class CLIParser {
public:
    CLIParser(const std::string& name, const std::string& description);
    ~CLIParser();
    
    // Add a command to the parser
    void addCommand(std::unique_ptr<CLICommand> command);
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Get parsed command name
    std::string getCommandName() const { return currentCommandName; }
    
    // Get parsed arguments
    std::vector<std::string> getArguments() const { return parsedArguments; }
    
    // Get parser name
    std::string getName() const { return name; }
    
    // Get parser description
    std::string getDescription() const { return description; }
    
    // Show help for a specific command
    void showHelpForCommand(const std::string& commandName);
    
    // Show general help
    void showHelp();
    
    // Execute the current command
    int executeCommand();
    
private:
    std::string name;
    std::string description;
    std::string currentCommandName;
    std::vector<std::string> parsedArguments;
    std::unordered_map<std::string, std::unique_ptr<CLICommand>> commands;
    
    // Helper methods
    bool findCommand(const std::string& commandName);
    std::string findCommandDescription(const std::string& commandName) const;
};

} // namespace nlm