#pragma once

#include <string>
#include <vector>
#include <map>

namespace nlm {

class CommandExecutionFramework {
public:
    CommandExecutionFramework(std::shared_ptr<Logger> logger);
    
    // Command execution
    bool executeCommand(const std::string& command, const std::vector<std::string>& args);
    
    // Command help
    void printHelp(const std::string& command = "");
    void printHelpGroup(const std::string& group);
    
    // Command completion
    std::vector<std::string> getCommandCompletions(const std::string& partialCommand);
    
    // Command history
    void addCommandToHistory(const std::string& command);
    std::vector<std::string> getCommandHistory();
    
    // Error handling
    bool handleCommandError(const std::string& command, const std::string& error);
    
    // Command registration
    void registerCommand(const std::string& command, const std::string& description, 
                        const std::string& helpText, const std::string& group);
    
    void registerCommandGroup(const std::string& group, const std::string& description);
    
private:
    std::shared_ptr<Logger> logger;
    std::map<std::string, std::string> commandDescriptions;
    std::map<std::string, std::string> commandHelpTexts;
    std::map<std::string, std::string> commandGroups;
    std::vector<std::string> commandHistory;
};
