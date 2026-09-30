// NLM Advanced Commands Plugin
// Demonstrates the plugin architecture with new command-line capabilities

#pragma once

#include "src/core/PluginSystem/PluginRegistry.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <any>
#include <ctime>

namespace nlm {

// Advanced Commands Plugin - Provides enhanced command-line interface capabilities
class AdvancedCommandsPlugin : public CommandPlugin {
public:
    AdvancedCommandsPlugin() : lastCommandTime_(0), commandHistoryMaxSize_(100) {}
    ~AdvancedCommandsPlugin() override = default;
    
    std::string getName() const override {
        return "advanced_commands";
    }
    
    std::string getDescription() const override {
        return "Provides advanced command-line interface capabilities including command history, scripts, and automation";
    }
    
    PluginType getType() const override {
        return PluginType::Command;
    }
    
    std::string getVersion() const override {
        return "3.0.0";
    }
    
    bool initialize(const std::unordered_map<std::string, std::any>& config) override {
        try {
            // Apply configuration
            if (config.find("history_size") != config.end()) {
                commandHistoryMaxSize_ = std::any_cast<size_t>(config.at("history_size"));
            }
            
            if (config.find("enable_auto_complete") != config.end()) {
                enableAutoComplete_ = std::any_cast<bool>(config.at("enable_auto_complete"));
            }
            
            if (config.find("script_directory") != config.end()) {
                scriptDirectory_ = std::any_cast<std::string>(config.at("script_directory"));
            }
            
            // Initialize command history tracking
            lastCommandTime_ = std::time(nullptr);
            commandHistory_.clear();
            
            return true;
        } catch (const std::exception& e) {
            errorMessage_ = "Initialization failed: " + std::string(e.what());
            return false;
        }
    }
    
    void cleanup() override {
        commandHistory_.clear();
        scriptCache_.clear();
        errorMessage_.clear();
    }
    
    bool isActive() const override {
        return isActive_;
    }
    
    void setActive(bool active) override {
        isActive_ = active;
    }
    
    std::vector<std::string> getCommands() const override {
        return {"history", "script", "auto-complete", "stats", "save", "load", "help", "exec", "batch"};
    }
    
    std::string getCommandHelp(const std::string& command) const override {
        static const std::unordered_map<std::string, std::string> helpMap = {
            {"history", "Show command history (history [limit])"},
            {"script", "Execute script file (script <file> [args...])"},
            {"auto-complete", "Toggle auto-completion (auto-complete)"},
            {"stats", "Show plugin statistics (stats)"},
            {"save", "Save configuration (save <file>)"},
            {"load", "Load configuration (load <file>)"},
            {"help", "Show help for commands (help <command>)"},
            {"exec", "Execute command and capture output (exec <command>)"},
            {"batch", "Execute batch of commands (batch <file>)"}
        };
        
        auto it = helpMap.find(command);
        return it != helpMap.end() ? it->second : "Unknown command. Use 'help' to see available commands.";
    }
    
    bool handleCommand(const std::string& command, const std::vector<std::string>& args) override {
        try {
            lastCommandTime_ = std::time(nullptr);
            
            if (command == "history") {
                return handleHistoryCommand(args);
            } else if (command == "script") {
                return handleScriptCommand(args);
            } else if (command == "auto-complete") {
                return handleAutoCompleteCommand(args);
            } else if (command == "stats") {
                return handleStatsCommand(args);
            } else if (command == "save") {
                return handleSaveCommand(args);
            } else if (command == "load") {
                return handleLoadCommand(args);
            } else if (command == "help") {
                return handleHelpCommand(args);
            } else if (command == "exec") {
                return handleExecCommand(args);
            } else if (command == "batch") {
                return handleBatchCommand(args);
            } else {
                errorMessage_ = "Unknown command: " + command;
                return false;
            }
        } catch (const std::exception& e) {
            errorMessage_ = "Command execution failed: " + std::string(e.what());
            return false;
        }
    }
    
    std::any getResult(const std::string& query) override {
        if (query == "status") {
            std::unordered_map<std::string, std::any> status;
            status["name"] = getName();
            status["description"] = getDescription();
            status["version"] = getVersion();
            status["isActive"] = isActive();
            status["commandCount"] = static_cast<size_t>(getCommands().size());
            status["historySize"] = commandHistory_.size();
            status["historyMaxSize"] = commandHistoryMaxSize_;
            status["autoCompleteEnabled"] = enableAutoComplete_;
            status["lastCommandTime"] = lastCommandTime_;
            return status;
        } else if (query == "history") {
            return commandHistory_;
        } else if (query == "errors") {
            return errorMessages_;
        }
        return std::any();
    }
    
private:
    bool handleHistoryCommand(const std::vector<std::string>& args) {
        size_t limit = commandHistoryMaxSize_;
        if (!args.empty()) {
            try {
                limit = std::stoul(args[0]);
            } catch (...) {
                errorMessage_ = "Invalid history limit: " + args[0];
                return false;
            }
        }
        
        std::cout << "=== Command History (last " << std::min(limit, commandHistory_.size()) << " commands) ===" << std::endl;
        size_t start = std::max(0, static_cast<int>(commandHistory_.size()) - static_cast<int>(limit));
        for (size_t i = start; i < commandHistory_.size(); ++i) {
            std::cout << i + 1 << ": " << commandHistory_[i] << std::endl;
        }
        
        return true;
    }
    
    bool handleScriptCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            errorMessage_ = "Script filename required";
            return false;
        }
        
        std::string filename = args[0];
        std::vector<std::string> scriptArgs(args.begin() + 1, args.end());
        
        // Check if script is in cache
        std::string cacheKey = filename + "|" + std::to_string(scriptArgs.size());
        if (scriptCache_.find(cacheKey) != scriptCache_.end()) {
            std::cout << "Executing cached script: " << filename << std::endl;
            // In a real implementation, we would execute the script
            return true;
        }
        
        // Load and execute script
        std::cout << "Loading script: " << filename << std::endl;
        // In a real implementation, we would read and parse the script file
        // For demo, we'll just show what would be executed
        std::cout << "Script arguments: ";
        for (const auto& arg : scriptArgs) {
            std::cout << arg << " ";
        }
        std::cout << std::endl;
        
        // Cache the script
        scriptCache_[cacheKey] = "script_content_placeholder";
        
        return true;
    }
    
    bool handleAutoCompleteCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            enableAutoComplete_ = !enableAutoComplete_;
            std::cout << "Auto-completion " << (enableAutoComplete_ ? "enabled" : "disabled") << std::endl;
            return true;
        }
        
        if (args[0] == "on") {
            enableAutoComplete_ = true;
            std::cout << "Auto-completion enabled" << std::endl;
        } else if (args[0] == "off") {
            enableAutoComplete_ = false;
            std::cout << "Auto-completion disabled" << std::endl;
        } else {
            errorMessage_ = "Invalid auto-complete argument. Use 'on' or 'off'.";
            return false;
        }
        
        return true;
    }
    
    bool handleStatsCommand(const std::vector<std::string>& args) {
        auto status = std::any_cast<std::unordered_map<std::string, std::any>>(getResult("status"));
        
        std::cout << "=== Advanced Commands Plugin Statistics ===" << std::endl;
        std::cout << "Plugin: " << status.at("name") << std::endl;
        std::cout << "Version: " << status.at("version") << std::endl;
        std::cout << "Description: " << status.at("description") << std::endl;
        std::cout << "Active: " << (status.at("isActive") ? "Yes" : "No") << std::endl;
        std::cout << "Available Commands: " << status.at("commandCount") << std::endl;
        std::cout << "Command History Size: " << status.at("historySize") << "/" << status.at("historyMaxSize") << std::endl;
        std::cout << "Auto-Completion: " << (status.at("autoCompleteEnabled") ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Last Command Time: " << status.at("lastCommandTime") << std::endl;
        
        return true;
    }
    
    bool handleSaveCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            errorMessage_ = "Save filename required";
            return false;
        }
        
        std::string filename = args[0];
        std::cout << "Saving plugin configuration to: " << filename << std::endl;
        
        // In a real implementation, we would save the configuration to a file
        // For demo, we'll just show what would be saved
        std::unordered_map<std::string, std::any> config;
        config["history_size"] = commandHistoryMaxSize_;
        config["enable_auto_complete"] = enableAutoComplete_;
        config["script_directory"] = scriptDirectory_;
        
        // This would serialize the config to JSON/INI/etc.
        std::cout << "Configuration saved successfully" << std::endl;
        
        return true;
    }
    
    bool handleLoadCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            errorMessage_ = "Load filename required";
            return false;
        }
        
        std::string filename = args[0];
        std::cout << "Loading plugin configuration from: " << filename << std::endl;
        
        // In a real implementation, we would load the configuration from a file
        // For demo, we'll just show what would be loaded
        std::cout << "Configuration loaded successfully" << std::endl;
        
        return true;
    }
    
    bool handleHelpCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cout << "=== Advanced Commands Plugin Help ===" << std::endl;
            std::cout << "Available commands:" << std::endl;
            for (const auto& cmd : getCommands()) {
                std::cout << "  " << cmd << ": " << getCommandHelp(cmd) << std::endl;
            }
            return true;
        }
        
        std::string command = args[0];
        std::string help = getCommandHelp(command);
        std::cout << "=== Help for command: " << command << " ===" << std::endl;
        std::cout << help << std::endl;
        
        return true;
    }
    
    bool handleExecCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            errorMessage_ = "Command to execute required";
            return false;
        }
        
        std::string command = args[0];
        std::vector<std::string> execArgs(args.begin() + 1, args.end());
        
        std::cout << "Executing command: " << command << std::endl;
        
        // In a real implementation, we would execute the command
        // For demo, we'll just show what would be executed
        std::cout << "Command: " << command << std::endl;
        if (!execArgs.empty()) {
            std::cout << "Arguments:" << std::endl;
            for (const auto& arg : execArgs) {
                std::cout << "  " << arg << std::endl;
            }
        }
        
        return true;
    }
    
    bool handleBatchCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            errorMessage_ = "Batch file required";
            return false;
        }
        
        std::string filename = args[0];
        std::cout << "Executing batch file: " << filename << std::endl;
        
        // In a real implementation, we would read and execute commands from the batch file
        // For demo, we'll just show what would be executed
        std::cout << "Batch file commands (simulated):" << std::endl;
        std::cout << "  1. history 10" << std::endl;
        std::cout << "  2. stats" << std::endl;
        std::cout << "  3. auto-complete on" << std::endl;
        
        return true;
    }
    
    // Additional plugin functionality
    bool recordCommand(const std::string& command) {
        commandHistory_.push_back(command);
        
        // Trim history if it exceeds max size
        if (commandHistory_.size() > commandHistoryMaxSize_) {
            commandHistory_.erase(commandHistory_.begin());
        }
        
        return true;
    }
    
    void addError(const std::string& error) {
        errorMessages_.push_back(error);
        errorMessage_ = error;
    }
    
    // Getters for internal state
    size_t getCommandHistorySize() const { return commandHistory_.size(); }
    size_t getCommandHistoryMaxSize() const { return commandHistoryMaxSize_; }
    bool isAutoCompleteEnabled() const { return enableAutoComplete_; }
    time_t getLastCommandTime() const { return lastCommandTime_; }
    
private:
    std::vector<std::string> commandHistory_;
    std::unordered_map<std::string, std::string> scriptCache_;
    std::vector<std::string> errorMessages_;
    
    size_t commandHistoryMaxSize_;
    bool enableAutoComplete_;
    std::string scriptDirectory_;
    time_t lastCommandTime_;
    bool isActive_;
    std::string errorMessage_;
};

// Plugin factory function for AdvancedCommandsPlugin
std::unique_ptr<Plugin> createAdvancedCommandsPlugin() {
    return std::make_unique<AdvancedCommandsPlugin>();
}

// Plugin metadata for AdvancedCommandsPlugin
inline const PluginMetadata ADVANCED_COMMANDS_PLUGIN_METADATA = {
    "advanced_commands",
    "Provides advanced command-line interface capabilities",
    "3.0.0",
    PluginType::Command,
    {"cli", "commands", "advanced", "automation"},
    "NLM Team",
    "MIT",
    {
        {"history_size", 1000},
        {"enable_auto_complete", true},
        {"script_directory", "./scripts"}
    }
};

// Plugin registry function to register AdvancedCommandsPlugin
inline bool registerAdvancedCommandsPlugin(PluginRegistry& registry) {
    return registry.registerPlugin(ADVANCED_COMMANDS_PLUGIN_METADATA, createAdvancedCommandsPlugin);
}
} // namespace nlm
