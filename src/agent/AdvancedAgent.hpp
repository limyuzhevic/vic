#pragma once

#include "AgentBrain.hpp"
#include "SensoryPercept.hpp"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <set>
#include <chrono>

namespace nlm {

// Advanced motor command groups for complex behaviors
enum class ComplexAction {
    // Basic movements
    MoveForward = 0,
    MoveBackward = 1,
    TurnLeft = 2,
    TurnRight = 3,
    
    // Advanced combinations
    MoveForwardThenTurnLeft = 4,
    MoveForwardThenTurnRight = 5,
    MoveBackwardThenTurnLeft = 6,
    MoveBackwardThenTurnRight = 7,
    
    // Environmental interactions
    ExploreArea = 8,
    FollowPath = 9,
    AvoidObstacles = 10,
    ReturnToStart = 11,
    
    // Communication commands
    SignalOther = 12,
    ShareInformation = 13,
    FormGroup = 14,
    BreakGroup = 15,
    
    // Special actions
    Wait = 16,
    Rest = 17,
    Sleep = 18,
    
    // Developmental commands
    LearnFromExperience = 19,
    TransferLearning = 20,
    MetaLearning = 21
};

// Command execution result structure
struct CommandResult {
    bool success;
    std::string message;
    float executionTime;
    std::vector<float> resultingState;
    std::map<std::string, float> metrics;
    
    CommandResult() : success(false), executionTime(0.0f) {}
    CommandResult(bool s, const std::string& msg) : success(s), message(msg), executionTime(0.0f) {}
};

// Script command for advanced scripting
struct ScriptCommand {
    std::string name;
    std::vector<std::string> arguments;
    std::chrono::steady_clock::time_point scheduledTime;
    bool executed;
    bool conditional;
    
    ScriptCommand(const std::string& cmdName, const std::vector<std::string>& args = {})
        : name(cmdName), arguments(args), executed(false), conditional(false) {
        scheduledTime = std::chrono::steady_clock::now();
    }
};

// Command pattern for scripting
class Command {
public:
    virtual ~Command() = default;
    virtual CommandResult execute(AgentBrain& agent, const std::map<std::string, std::string>& context) = 0;
    virtual std::string getDescription() const = 0;
};

// Concrete command implementations
class MoveCommand : public Command {
public:
    MoveCommand(float distance, float angle) : distance_(distance), angle_(angle) {}
    
    CommandResult execute(AgentBrain& agent, const std::map<std::string, std::string>& context) override;
    std::string getDescription() const override {
        return "Move " + std::to_string(distance_) + " units at " + std::to_string(angle_) + " degrees";
    }
    
private:
    float distance_;
    float angle_;
};

class WaitCommand : public Command {
public:
    WaitCommand(double duration) : duration_(duration) {}
    
    CommandResult execute(AgentBrain& agent, const std::map<std::string, std::string>& context) override;
    std::string getDescription() const override {
        return "Wait for " + std::to_string(duration_) + " seconds";
    }
    
private:
    double duration_;
};

class ExploreCommand : public Command {
public:
    CommandResult execute(AgentBrain& agent, const std::map<std::string, std::string>& context) override;
    std::string getDescription() const override {
        return "Explore the environment";
    }
};

class LearnCommand : public Command {
public:
    CommandResult execute(AgentBrain& agent, const std::map<std::string, std::string>& context) override;
    std::string getDescription() const override {
        return "Learn from current experience";
    }
};

// Command manager for advanced command handling
class CommandManager {
public:
    CommandManager(std::shared_ptr<AgentBrain> agent);
    ~CommandManager();
    
    // Command execution
    CommandResult executeCommand(const std::string& commandLine);
    CommandResult executeComplexAction(ComplexAction action);
    
    // Script management
    bool loadScript(const std::string& filename);
    bool executeScript(const std::string& scriptName);
    bool scheduleScript(const std::string& scriptName, double delay = 0.0);
    
    // Command history and completion
    void addToHistory(const std::string& command);
    std::vector<std::string> getCommandHistory(size_t limit = 10) const;
    std::vector<std::string> getCommandCompletions(const std::string& partial) const;
    
    // Command definition
    void registerCommand(const std::string& name, std::unique_ptr<Command> command);
    bool hasCommand(const std::string& name) const;
    
    // State management
    void reset();
    void saveState(const std::string& filename) const;
    bool loadState(const std::string& filename);
    
    // Configuration
    void setVerbose(bool verbose) { verbose_ = verbose; }
    void setMaxHistorySize(size_t size) { maxHistorySize_ = size; }
    
    // Script variables and context
    void setVariable(const std::string& name, float value);
    float getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    
    // Context management for script execution
    void setContextVariable(const std::string& name, const std::string& value);
    std::string getContextVariable(const std::string& name) const;
    
    // Statistics
    size_t getCommandCount() const { return commandCount_; }
    double getAverageExecutionTime() const { return totalExecutionTime_ / std::max(commandCount_, 1ul); }
    
private:
    std::shared_ptr<AgentBrain> agent_;
    std::map<std::string, std::unique_ptr<Command>> commands_;
    std::vector<std::string> commandHistory_;
    std::map<std::string, float> variables_;
    std::map<std::string, std::string> contextVariables_;
    
    bool verbose_;
    size_t maxHistorySize_;
    size_t commandCount_;
    double totalExecutionTime_;
    
    // Script management
    std::map<std::string, std::vector<ScriptCommand>> scheduledScripts_;
    std::map<std::string, std::string> loadedScripts_;
    
    // Helper methods
    std::string parseCommand(const std::string& commandLine) const;
    std::vector<std::string> parseArguments(const std::string& args) const;
    std::string generateCompletion(const std::string& partial) const;
};

} // namespace nlm
