#pragma once

#include "Action.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

namespace nlm {

// Command base class for action selection
class Command {
public:
    Command() = default;
    virtual ~Command() = default;
    
    // Execute the command
    virtual std::unique_ptr<Action> execute() = 0;
    
    // Get command name
    virtual std::string getName() const = 0;
    
    // Get command description
    virtual std::string getDescription() const = 0;
    
    // Clone command
    virtual std::unique_ptr<Command> clone() const = 0;
    
    // Check if command is enabled
    virtual bool isEnabled() const = 0;
    
    // Enable/disable command
    virtual void setEnabled(bool enabled) = 0;
    
    // Get command priority (higher = executed first)
    virtual float getPriority() const = 0;
    
    // Set command priority
    virtual void setPriority(float priority) = 0;
};

// Simple command implementation - wrapper around Action
class SimpleCommand : public Command {
public:
    SimpleCommand(std::unique_ptr<Action> action, float priority = 1.0f)
        : action_(std::move(action)), priority_(priority), enabled_(true) {}
    
    std::unique_ptr<Action> execute() override {
        if (!enabled_) {
            return std::make_unique<Action>(ActionType::Wait);
        }
        return action_->clone();
    }
    
    std::string getName() const override {
        return action_->getName();
    }
    
    std::string getDescription() const override {
        return "Simple action command";
    }
    
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<SimpleCommand>(*this);
    }
    
    bool isEnabled() const override {
        return enabled_;
    }
    
    void setEnabled(bool enabled) override {
        enabled_ = enabled;
    }
    
    float getPriority() const override {
        return priority_;
    }
    
    void setPriority(float priority) override {
        priority_ = priority;
    }
    
    // Get the underlying action
    const Action& getAction() const {
        return *action_;
    }
    
private:
    std::unique_ptr<Action> action_;
    float priority_;
    bool enabled_;
};

// Conditional command - only executes if condition is met
class ConditionalCommand : public Command {
public:
    using ConditionFunc = std::function<bool(const Command&, std::shared_ptr<Brain>)>;
    using ActionGeneratorFunc = std::function<std::unique_ptr<Action>(std::shared_ptr<Brain>)>;
    
    ConditionalCommand(std::unique_ptr<Command> condition, 
                      ActionGeneratorFunc actionGenerator,
                      float priority = 1.0f)
        : condition_(std::move(condition)), 
          actionGenerator_(std::move(actionGenerator)),
          priority_(priority), enabled_(true) {}
    
    std::unique_ptr<Action> execute() override {
        if (!enabled_) {
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        if (condition_) {
            // This is a simplified version - in a full implementation,
            // the condition would have access to the brain and state
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        return actionGenerator_(nullptr);
    }
    
    std::string getName() const override {
        return "ConditionalCommand";
    }
    
    std::string getDescription() const override {
        return "Command that executes only if condition is met";
    }
    
    std::unique_ptr<Command> clone() const override {
        auto cloned = std::make_unique<ConditionalCommand>(
            condition_->clone(), actionGenerator_, priority_);
        cloned->enabled_ = this->enabled_;
        return cloned;
    }
    
    bool isEnabled() const override {
        return enabled_;
    }
    
    void setEnabled(bool enabled) override {
        enabled_ = enabled;
    }
    
    float getPriority() const override {
        return priority_;
    }
    
    void setPriority(float priority) override {
        priority_ = priority;
    }
    
private:
    std::unique_ptr<Command> condition_;
    ActionGeneratorFunc actionGenerator_;
    float priority_;
    bool enabled_;
};

// Command manager - manages and executes commands
class CommandManager {
public:
    CommandManager() = default;
    ~CommandManager() = default;
    
    // Add a command to the manager
    void addCommand(std::unique_ptr<Command> command) {
        commands_.push_back(std::move(command));
    }
    
    // Remove a command by name
    bool removeCommand(const std::string& name);
    
    // Execute all commands and return the best action
    std::unique_ptr<Action> executeCommands(std::shared_ptr<Brain> brain = nullptr);
    
    // Get all commands
    const std::vector<std::unique_ptr<Command>>& getCommands() const {
        return commands_;
    }
    
    // Clear all commands
    void clearCommands() {
        commands_.clear();
    }
    
    // Get command by name
    Command* getCommand(const std::string& name);
    
    // Enable/disable a command
    bool setCommandEnabled(const std::string& name, bool enabled);
    
    // Set command priority
    bool setCommandPriority(const std::string& name, float priority);
    
    // Get enabled commands
    std::vector<Command*> getEnabledCommands() const;
    
    // Sort commands by priority
    void sortCommandsByPriority() {
        std::sort(commands_.begin(), commands_.end(),
                  [](const std::unique_ptr<Command>& a, const std::unique_ptr<Command>& b) {
                      return a->getPriority() > b->getPriority();
                  });
    }
    
    // Get number of commands
    size_t getCommandCount() const {
        return commands_.size();
    }
    
private:
    std::vector<std::unique_ptr<Command>> commands_;
};

} // namespace nlm