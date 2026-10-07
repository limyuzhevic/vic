#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include "CommandBase.hpp"

namespace nlm {

class CommandManager {
public:
    CommandManager();
    ~CommandManager();
    
    // Command registration
    bool registerCommand(std::unique_ptr<CommandBase> command);
    bool unregisterCommand(const std::string& name);
    bool hasCommand(const std::string& name) const;
    
    // Command execution
    CommandResult executeCommand(const std::string& name);
    CommandResult executeCommand(const std::string& name, const std::vector<std::string>& arguments);
    
    // Command management
    std::vector<std::string> getCommandNames() const;
    std::vector<std::string> getCommandNames(CommandBase::Category category) const;
    
    CommandBase* getCommand(const std::string& name);
    const CommandBase* getCommand(const std::string& name) const;
    
    // Command information
    std::string getCommandHelp(const std::string& name) const;
    std::string getCommandUsage(const std::string& name) const;
    std::string getCommandDescription(const std::string& name) const;
    CommandBase::Category getCommandCategory(const std::string& name) const;
    
    // Batch operations
    std::vector<CommandResult> executeCommands(const std::vector<std::string>& commandNames);
    std::vector<CommandResult> executeCommands(const std::vector<std::string>& commandNames, 
                                              const std::vector<std::vector<std::string>>& arguments);
    
    // Command history
    std::vector<std::string> getCommandHistory() const;
    void clearCommandHistory();
    
    // Progress monitoring
    std::vector<std::pair<std::string, double>> getActiveCommandProgress() const;
    void setGlobalProgressCallback(const CommandBase::ProgressCallback& callback);
    
    // Command templates
    bool createCommandTemplate(const std::string& name, const std::string& commandName,
                              const std::vector<std::string>& arguments);
    bool removeCommandTemplate(const std::string& name);
    CommandResult executeCommandTemplate(const std::string& name);
    
    // Command search
    std::vector<std::string> searchCommands(const std::string& pattern) const;
    std::vector<std::string> getCommandsByCategory(CommandBase::Category category) const;
    
    // Command execution monitoring
    size_t getTotalCommandsExecuted() const;
    size_t getSuccessfulCommandsExecuted() const;
    double getCommandSuccessRate() const;
    
    // Configuration management
    void setAutoExecuteOnLoad(bool autoExecute);
    void setMaxConcurrentCommands(size_t maxConcurrent);
    
    // Statistics and reporting
    std::string generateCommandReport() const;
    json exportCommandStats() const;
    
    // Command validation
    bool validateCommandArguments(const std::string& name, const std::vector<std::string>& arguments) const;
    
    // Cleanup
    void cleanupCompletedCommands();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm