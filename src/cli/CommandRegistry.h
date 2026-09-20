/*
 * CommandRegistry.h - Runtime command system for NLM
 * Provides a registry for runtime commands that can be executed
 * during simulation for advanced control and manipulation.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <iostream>
#include <mutex>
#include <chrono>
#include <ctime>

namespace nlm {

// Command execution status
enum class CommandStatus {
    Pending,
    Executing,
    Completed,
    Failed,
    Cancelled
};

// Command execution result
struct CommandResult {
    bool success;
    std::string message;
    double executionTime;  // seconds
    std::map<std::string, std::string> metadata;
    
    CommandResult() : success(false), executionTime(0.0) {}
    CommandResult(bool s, const std::string& msg) : success(s), message(msg), executionTime(0.0) {}
};

// Runtime command base class
class CLICommandBase {
public:
    virtual ~CLICommandBase() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::vector<std::string> getArguments() const = 0;
    virtual bool execute(const std::vector<std::string>& args) = 0;
    virtual std::string getHelp() const = 0;
};

// Typed command implementation
template<typename T>
class TypedCLICommand : public CLICommandBase {
public:
    using CommandHandler = std::function<CommandResult(const std::vector<std::string>&, T&)>;
    
    TypedCLICommand(const std::string& name,
                   const std::string& description,
                   const std::vector<std::string>& argumentNames,
                   const std::vector<std::string>& argumentDescriptions,
                   CommandHandler handler)
        : name_(name), description_(description),
          argumentNames_(argumentNames), argumentDescriptions_(argumentDescriptions),
          handler_(handler) {}
    
    std::string getName() const override { return name_; }
    std::string getDescription() const override { return description_; }
    std::vector<std::string> getArguments() const override { return argumentNames_; }
    std::string getHelp() const override {
        std::ostringstream oss;
        oss << name_ << " - " << description_ << "\n";
        if (!argumentNames_.empty()) {
            oss << "Arguments:\n";
            for (size_t i = 0; i < argumentNames_.size(); ++i) {
                oss << "  " << argumentNames_[i];
                if (i < argumentDescriptions_.size() && !argumentDescriptions_[i].empty()) {
                    oss << " - " << argumentDescriptions_[i];
                }
                oss << "\n";
            }
        }
        return oss.str();
    }
    
    bool execute(const std::vector<std::string>& args) override {
        throw std::runtime_error("TypedCLICommand::execute should not be called directly");
    }
    
    CommandResult execute(T& context, const std::vector<std::string>& args) {
        return handler_(args, context);
    }
    
private:
    std::string name_;
    std::string description_;
    std::vector<std::string> argumentNames_;
    std::vector<std::string> argumentDescriptions_;
    CommandHandler handler_;
};

// Command execution context for runtime commands
class CommandExecutionContext {
public:
    CommandExecutionContext(std::shared_ptr<Brain> brain,
                           std::shared_ptr<Config> config,
                           std::shared_ptr<PowerUserAPI> powerUserAPI,
                           std::shared_ptr<AdvancedSimulationController> controller,
                           std::shared_ptr<AdvancedDebugger> debugger)
        : brain_(brain), config_(config), powerUserAPI_(powerUserAPI),
          controller_(controller), debugger_(debugger), 
          startTime_(std::chrono::steady_clock::now()) {}
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    std::shared_ptr<Config> getConfig() const { return config_; }
    std::shared_ptr<PowerUserAPI> getPowerUserAPI() const { return powerUserAPI_; }
    std::shared_ptr<AdvancedSimulationController> getController() const { return controller_; }
    std::shared_ptr<AdvancedDebugger> getDebugger() const { return debugger_; }
    
    double getElapsedTime() const {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
        return duration.count() / 1000.0;
    }
    
    std::string getCurrentStep() const {
        return std::to_string(config_->getOr<int64_t>("current_step", 0));
    }
    
    std::string getCurrentTime() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
        return ss.str();
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<PowerUserAPI> powerUserAPI_;
    std::shared_ptr<AdvancedSimulationController> controller_;
    std::shared_ptr<AdvancedDebugger> debugger_;
    std::chrono::steady_clock::time_point startTime_;
};

// Runtime command registry and executor
class CommandRegistry {
public:
    CommandRegistry();
    ~CommandRegistry();
    
    // Register a command
    template<typename T>
    void registerCommand(std::shared_ptr<TypedCLICommand<T>> command);
    
    // Execute a command by name
    CommandResult executeCommand(const std::string& commandName,
                                const std::vector<std::string>& args,
                                CommandExecutionContext& context);
    
    // Get all registered commands
    std::vector<std::string> getCommandNames() const;
    
    // Get command help
    std::string getCommandHelp(const std::string& commandName) const;
    
    // Check if command exists
    bool hasCommand(const std::string& commandName) const;
    
    // Get all commands
    std::vector<std::shared_ptr<CLICommandBase>> getAllCommands() const;
    
    // Set output stream
    void setOutputStream(std::ostream* outputStream);
    
    // Set error stream
    void setErrorStream(std::ostream* errorStream);
    
    // Cleanup
    void cleanup();
    
    // Get command execution history
    std::vector<CommandResult> getCommandHistory(const std::string& commandName, size_t maxCount = 10) const;
    
    // Clear command history
    void clearHistory(const std::string& commandName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm