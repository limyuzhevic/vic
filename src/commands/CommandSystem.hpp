// Main command system implementation for NLM
#pragma once

#include "CommandBase.hpp"
#include "CommandParsing.hpp"
#include "CommandIntegration.hpp"
#include "MonitoringCommands.hpp"
#include "NetworkExplorationCommands.hpp"
#include "LearningControlCommands.hpp"
#include "SimulationControlCommands.hpp"
#include "ExperimentControlCommands.hpp"
#include "VisualizationControlCommands.hpp"
#include <memory>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>

namespace nlm {

class CommandSystem {
private:
    std::shared_ptr<AdvancedCommandFactory> commandFactory_;
    std::shared_ptr<CommandHistory> commandHistory_;
    std::shared_ptr<CommandRegistry> commandRegistry_;
    std::shared_ptr<Logger> logger_;
    
    std::atomic<bool> running_ = false;
    std::thread inputThread_;
    std::thread outputThread_;
    
    std::mutex outputMutex_;
    std::condition_variable outputCV_;
    std::queue<std::string> outputQueue_;
    
    std::function<void(const std::string&)> outputCallback_;
    std::function<void(const std::string&)> errorCallback_;
    
public:
    CommandSystem() {
        commandFactory_ = std::make_shared<AdvancedCommandFactory>();
        commandHistory_ = std::make_shared<CommandHistory>();
        commandRegistry_ = std::make_shared<DefaultCommandRegistry>(commandFactory_);
        logger_ = Logger::getGlobal();
        
        initializeCommands();
    }
    
    ~CommandSystem() {
        stop();
    }
    
    // Initialize all commands and command groups
    void initializeCommands() {
        // Register all command groups
        auto monitoringGroup = std::make_shared<MonitoringGroup>();
        auto networkExploreGroup = std::make_shared<NetworkExplorationGroup>();
        auto learningControlGroup = std::make_shared<LearningControlGroup>();
        auto simulationControlGroup = std::make_shared<SimulationControlGroup>();
        auto experimentControlGroup = std::make_shared<ExperimentControlGroup>();
        auto visualizationControlGroup = std::make_shared<VisualizationControlGroup>();
        
        if (auto defaultRegistry = std::dynamic_pointer_cast<DefaultCommandRegistry>(commandRegistry_)) {
            defaultRegistry->registerGroup(monitoringGroup);
            defaultRegistry->registerGroup(networkExploreGroup);
            defaultRegistry->registerGroup(learningControlGroup);
            defaultRegistry->registerGroup(simulationControlGroup);
            defaultRegistry->registerGroup(experimentControlGroup);
            defaultRegistry->registerGroup(visualizationControlGroup);
        }
        
        // Register special commands
        registerSpecialCommands();
    }
    
    // Register special commands (help, exit, etc.)
    void registerSpecialCommands() {
        // Help command
        auto helpCommand = std::make_shared<Command>();
        helpCommand->setName("help");
        helpCommand->setDescription("Show help information about commands");
        helpCommand->setUsage("help [command_name]");
        helpCommand->setExecute([this](const CommandContext& context, 
                                       const std::map<std::string, CommandArgument>& args) {
            return showHelp(args);
        });
        
        // Exit command
        auto exitCommand = std::make_shared<Command>();
        exitCommand->setName("exit");
        exitCommand->setDescription("Exit the command system");
        exitCommand->setExecute([this](const CommandContext& context,
                                       const std::map<std::string, CommandArgument>& args) {
            stop();
            return "Exiting command system...";
        });
        
        if (auto defaultRegistry = std::dynamic_pointer_cast<DefaultCommandRegistry>(commandRegistry_)) {
            defaultRegistry->registerCommand(helpCommand);
            defaultRegistry->registerCommand(exitCommand);
        }
    }
    
    // Execute command string
    CommandResult executeCommand(const std::string& commandLine, const std::string& user = "system") {
        try {
            // Parse command line
            auto parsed = CommandParser::parse(commandLine);
            
            // Get command
            auto command = commandRegistry_->getCommand(parsed.commandName);
            if (!command) {
                std::string error = "Unknown command: " + parsed.commandName;
                errorCallback_(error);
                commandHistory_->add(commandLine, error, false);
                return error;
            }
            
            // Validate arguments
            if (!CommandValidator::validate(command, parsed.arguments)) {
                std::string error = "Invalid arguments for command: " + parsed.commandName;
                errorCallback_(error);
                commandHistory_->add(commandLine, error, false);
                return error;
            }
            
            // Execute command
            CommandContext context;
            context.user = user;
            
            CommandResult result = command->execute(context, parsed.arguments);
            
            // Add to history
            std::string resultStr = std::visit([](const auto& val) -> std::string {
                if constexpr (std::is_same_v<decltype(val), std::string>) {
                    return val;
                } else if constexpr (std::is_same_v<decltype(val), int>) {
                    return std::to_string(val);
                } else if constexpr (std::is_same_v<decltype(val), double>) {
                    return std::to_string(val);
                } else if constexpr (std::is_same_v<decltype(val), bool>) {
                    return val ? "true" : "false";
                } else {
                    return "unknown_result_type";
                }
            }, result);
            
            commandHistory_->add(commandLine, resultStr, true);
            outputCallback_("> " + commandLine + "\n");
            outputCallback_("> " + resultStr + "\n");
            
            return result;
            
        } catch (const std::exception& e) {
            std::string error = "Error executing command: " + std::string(e.what());
            errorCallback_(error);
            commandHistory_->add(commandLine, error, false);
            return error;
        }
    }
    
    // Show help information
    std::string showHelp(const std::map<std::string, CommandArgument>& args) const {
        std::stringstream help;
        help << "=== NLM Command System Help ===\n\n";
        
        if (args.find("command_name") != args.end()) {
            // Show help for specific command
            std::string commandName = std::get<std::string>(args.at("command_name"));
            auto command = commandRegistry_->getCommand(commandName);
            if (command) {
                help << "Command: " << command->getName() << "\n";
                help << "Description: " << command->getDescription() << "\n";
                help << "Usage: " << command->getUsage() << "\n";
                help << "Arguments: ";
                auto argNames = command->getArguments();
                for (size_t i = 0; i < argNames.size(); ++i) {
                    if (i > 0) help << ", ";
                    help << argNames[i];
                }
                help << "\n";
            } else {
                help << "Command not found: " << commandName << "\n";
            }
        } else {
            // Show all commands grouped by category
            auto groups = commandRegistry_->getGroups();
            for (const auto& group : groups) {
                help << group->getName() << ": " << group->getDescription() << "\n";
                auto commands = group->getCommands();
                for (const auto& command : commands) {
                    help << "  " << command->getName() << " - " << command->getDescription() << "\n";
                }
                help << "\n";
            }
        }
        
        return help.str();
    }
    
    // Start command system
    void start() {
        running_ = true;
        
        // Start output thread
        outputThread_ = std::thread(&CommandSystem::outputThreadFunc, this);
        
        // Start input thread (if interactive mode)
        // inputThread_ = std::thread(&CommandSystem::inputThreadFunc, this);
        
        if (logger_) {
            logger_->info("Command system started");
        }
    }
    
    // Stop command system
    void stop() {
        running_ = false;
        
        if (outputThread_.joinable()) {
            outputCV_.notify_all();
            outputThread_.join();
        }
        
        if (inputThread_.joinable()) {
            inputThread_.join();
        }
        
        if (logger_) {
            logger_->info("Command system stopped");
        }
    }
    
    // Set output callback
    void setOutputCallback(std::function<void(const std::string&)> callback) {
        outputCallback_ = callback;
    }
    
    // Set error callback
    void setErrorCallback(std::function<void(const std::string&)> callback) {
        errorCallback_ = callback;
    }
    
    // Get command history
    std::shared_ptr<CommandHistory> getHistory() const {
        return commandHistory_;
    }
    
    // Get all commands
    std::vector<std::string> getAllCommands() const {
        return commandRegistry_->getCommandNames();
    }
    
private:
    // Output thread function
    void outputThreadFunc() {
        while (running_) {
            std::unique_lock<std::mutex> lock(outputMutex_);
            
            if (outputQueue_.empty()) {
                outputCV_.wait_for(lock, std::chrono::milliseconds(100));
                continue;
            }
            
            std::string output = outputQueue_.front();
            outputQueue_.pop();
            
            lock.unlock();
            
            if (outputCallback_) {
                outputCallback_(output);
            }
        }
    }
    
    // Input thread function (for interactive mode)
    void inputThreadFunc() {
        std::string line;
        while (running_) {
            if (std::getline(std::cin, line)) {
                CommandResult result = executeCommand(line, "user");
                // Queue output for processing
                std::lock_guard<std::mutex> lock(outputMutex_);
                outputQueue_.push(std::visit([](const auto& val) -> std::string {
                    if constexpr (std::is_same_v<decltype(val), std::string>) {
                        return val;
                    } else {
                        return "result_type_not_handled";
                    }
                }, result));
            }
        }
    }
};

// Default command registry implementation
class DefaultCommandRegistry : public CommandRegistry {
private:
    std::shared_ptr<AdvancedCommandFactory> commandFactory_;
    std::vector<std::shared_ptr<CommandGroup>> groups_;
    std::map<std::string, std::shared_ptr<Command>> commands_;
    
public:
    DefaultCommandRegistry(std::shared_ptr<AdvancedCommandFactory> factory) : commandFactory_(factory) {}
    
    void registerCommand(std::shared_ptr<Command> command) override {
        commands_[command->getName()] = command;
    }
    
    void registerGroup(std::shared_ptr<CommandGroup> group) override {
        groups_.push_back(group);
        
        // Register all commands in the group
        auto groupCommands = group->getCommands();
        for (const auto& command : groupCommands) {
            commands_[command->getName()] = command;
        }
    }
    
    std::shared_ptr<Command> getCommand(const std::string& name) override {
        auto it = commands_.find(name);
        if (it != commands_.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    std::vector<std::shared_ptr<CommandGroup>> getGroups() override {
        return groups_;
    }
    
    std::vector<std::shared_ptr<Command>> getAllCommands() override {
        std::vector<std::shared_ptr<Command>> allCommands;
        for (const auto& pair : commands_) {
            allCommands.push_back(pair.second);
        }
        return allCommands;
    }
    
    std::vector<std::string> getCommandNames() override {
        std::vector<std::string> names;
        for (const auto& pair : commands_) {
            names.push_back(pair.first);
        }
        return names;
    }
};

} // namespace nlm
