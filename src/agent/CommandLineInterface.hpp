// NLM Command-Line Interface
// Real-time control and monitoring tool for NLM brain simulation

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <chrono>

namespace nlm {

class Brain;
class AgentBrain;
class Config;

class Command {
public:
    virtual ~Command() = default;
    virtual std::string execute(const std::vector<std::string>& args) = 0;
    virtual std::string getHelp() const = 0;
    virtual std::string getName() const = 0;
};

class CommandLineInterface {
public:
    CommandLineInterface();
    ~CommandLineInterface();
    
    // Initialize with brain
    void initialize(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agentBrain);
    
    // Parse and execute command
    std::string executeCommand(const std::string& commandLine);
    
    // Command registration
    void registerCommand(std::unique_ptr<Command> cmd);
    
    // Show help for all commands
    std::string showHelp() const;
    
    // Get available commands
    std::vector<std::string> getCommandNames() const;
    
    // Check if command exists
    bool hasCommand(const std::string& name) const;
    
    // Brain control commands
    std::string startCommand(const std::vector<std::string>& args);
    std::string stopCommand(const std::vector<std::string>& args);
    std::string resetCommand(const std::vector<std::string>& args);
    std::string stepCommand(const std::vector<std::string>& args);
    
    // Configuration commands
    std::string configViewCommand(const std::vector<std::string>& args);
    std::string configModifyCommand(const std::vector<std::string>& args);
    std::string configSaveCommand(const std::vector<std::string>& args);
    
    // Memory inspection commands
    std::string memoryWorkingCommand(const std::vector<std::string>& args);
    std::string memoryEpisodicCommand(const std::vector<std::string>& args);
    
    // State control commands
    std::string saveStateCommand(const std::vector<std::string>& args);
    std::string loadStateCommand(const std::vector<std::string>& args);
    
    // Statistics and monitoring
    std::string statsCommand(const std::vector<std::string>& args);
    std::string monitoringCommand(const std::vector<std::string>& args);
    
    // Experiment control
    std::string experimentRunCommand(const std::vector<std::string>& args);
    std::string experimentStatusCommand(const std::vector<std::string>& args);
    
    // Advanced features
    std::string batchCommand(const std::vector<std::string>& args);
    std::string scriptCommand(const std::vector<std::string>& args);
    std::string eventCommand(const std::vector<std::string>& args);
    
    // Get brain reference
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    std::shared_ptr<AgentBrain> getAgentBrain() const { return agentBrain_; }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    
    std::unordered_map<std::string, std::unique_ptr<Command>> commands_;
    
    // Command parsing utilities
    std::vector<std::string> tokenize(const std::string& commandLine) const;
    std::string trim(const std::string& str) const;
    
    // Error handling
    std::string getErrorMessage(const std::string& command, const std::string& error) const;
};

} // namespace nlm