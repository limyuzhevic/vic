#pragma once

#include "AdvancedAgent.hpp"
#include "AgentBrain.hpp"
#include <memory>
#include <string>

namespace nlm {

// Enhanced AgentBrain with advanced command capabilities
class AdvancedAgentBrain {
public:
    AdvancedAgentBrain(std::shared_ptr<AgentBrain> agent);
    ~AdvancedAgentBrain();
    
    // Command execution interface
    CommandResult execute(const std::string& commandLine);
    CommandResult execute(ComplexAction action);
    
    // Script management
    bool loadScript(const std::string& filename);
    bool executeScript(const std::string& scriptName);
    bool scheduleScript(const std::string& scriptName, double delay = 0.0);
    
    // Command history and completion
    void addToHistory(const std::string& command);
    std::vector<std::string> getCommandHistory(size_t limit = 10) const;
    std::vector<std::string> getCommandCompletions(const std::string& partial) const;
    
    // Command registration
    void registerCommand(const std::string& name, std::unique_ptr<Command> command);
    bool hasCommand(const std::string& name) const;
    
    // State management
    void reset();
    void saveState(const std::string& filename) const;
    bool loadState(const std::string& filename);
    
    // Configuration
    void setVerbose(bool verbose) { commandManager_->setVerbose(verbose); }
    void setMaxHistorySize(size_t size) { commandManager_->setMaxHistorySize(size); }
    
    // Script variables and context
    void setVariable(const std::string& name, float value);
    float getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    
    void setContextVariable(const std::string& name, const std::string& value);
    std::string getContextVariable(const std::string& name) const;
    
    // Agent access (for advanced operations)
    std::shared_ptr<AgentBrain> getAgent() const { return agent_; }
    
    // Statistics
    size_t getCommandCount() const { return commandManager_->getCommandCount(); }
    double getAverageExecutionTime() const { return commandManager_->getAverageExecutionTime(); }
    
    // Advanced operations
    void setExplorationMode(bool explore) { explorationMode_ = explore; }
    bool isExplorationMode() const { return explorationMode_; }
    
    void setLearningMode(bool learn) { learningMode_ = learn; }
    bool isLearningMode() const { return learningMode_; }
    
    // Complex action sequences
    CommandResult executeActionSequence(const std::vector<ComplexAction>& actions);
    CommandResult createAndExecuteMacro(const std::string& macroName, 
                                        const std::vector<std::string>& commands);
    
private:
    std::shared_ptr<AgentBrain> agent_;
    std::unique_ptr<CommandManager> commandManager_;
    
    bool explorationMode_;
    bool learningMode_;
    
    // Macro management
    std::map<std::string, std::vector<std::string>> macros_;
};

} // namespace nlm
