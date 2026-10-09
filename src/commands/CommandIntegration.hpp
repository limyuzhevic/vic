// Command configuration and plugin architecture for NLM
#pragma once

#include "CommandBase.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace nlm {

class CommandConfig {
public:
    struct PluginInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string path;
        bool enabled = true;
        std::map<std::string, std::string> metadata;
    };
    
    // Load command configuration from file
    static bool loadFromFile(const std::string& filepath) {
        // TODO: Implement file loading (JSON/YAML/INI)
        return false;
    }
    
    // Save command configuration to file
    static bool saveToFile(const std::string& filepath) const {
        // TODO: Implement file saving (JSON/YAML/INI)
        return false;
    }
    
    // Get default command configuration
    static std::map<std::string, std::string> getDefaultConfig() {
        return {
            {"command_timeout", "30.0"},
            {"max_history_size", "1000"},
            {"enable_plugins", "true"},
            {"auto_complete", "true"},
            {"verbose_logging", "false"},
            {"json_output", "false"}
        };
    }
};

class PluginLoader {
public:
    // Load plugins from directory
    static std::vector<CommandConfig::PluginInfo> loadPlugins(const std::string& pluginDir) {
        std::vector<CommandConfig::PluginInfo> plugins;
        
        // TODO: Implement directory scanning and plugin loading
        // For now, return empty list
        return plugins;
    }
    
    // Load plugin from file
    static std::shared_ptr<CommandFactory> loadPlugin(const std::string& pluginPath) {
        // TODO: Implement dynamic library loading
        return nullptr;
    }
    
    // Check if file is a valid plugin
    static bool isPluginFile(const std::string& filepath) {
        // TODO: Check file extension and header
        return false;
    }
};

class AdvancedCommandFactory : public CommandFactory {
private:
    std::map<std::string, std::function<std::shared_ptr<Command>()>> commandCreators_;
    std::map<std::string, CommandConfig::PluginInfo> pluginInfos_;
    bool pluginsEnabled_ = true;
    
public:
    AdvancedCommandFactory() {
        initializeDefaultCommands();
    }
    
    // Register command factory function
    void registerCommand(const std::string& name, 
                        std::function<std::shared_ptr<Command>()> creator) {
        commandCreators_[name] = creator;
    }
    
    // Register plugin information
    void registerPlugin(const std::string& name, const CommandConfig::PluginInfo& info) {
        pluginInfos_[name] = info;
    }
    
    // Enable/disable plugins
    void setPluginsEnabled(bool enabled) {
        pluginsEnabled_ = enabled;
    }
    
    // Get list of available plugins
    std::vector<std::string> getAvailablePlugins() const {
        std::vector<std::string> plugins;
        for (const auto& pair : pluginInfos_) {
            if (pair.second.enabled) {
                plugins.push_back(pair.first);
            }
        }
        return plugins;
    }
    
    // Get plugin information
    std::optional<CommandConfig::PluginInfo> getPluginInfo(const std::string& name) const {
        auto it = pluginInfos_.find(name);
        if (it != pluginInfos_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    std::shared_ptr<Command> createCommand(const std::string& name) override {
        // Check if it's a built-in command
        auto it = commandCreators_.find(name);
        if (it != commandCreators_.end()) {
            return it->second();
        }
        
        // Check if it's a plugin command (when plugins enabled)
        if (pluginsEnabled_) {
            // TODO: Dynamically load and create plugin commands
        }
        
        return nullptr;
    }
    
    bool hasCommand(const std::string& name) const override {
        return commandCreators_.find(name) != commandCreators_.end();
    }
    
    // Get all registered command names
    std::vector<std::string> getAllCommandNames() const {
        std::vector<std::string> names;
        for (const auto& pair : commandCreators_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
private:
    void initializeDefaultCommands() {
        // Register all command groups and commands
        // This would be expanded with all the actual command classes
        
        // For now, register placeholder commands
        registerCommand("help", []() { return std::make_shared<Command>(); });
        registerCommand("exit", []() { return std::make_shared<Command>(); });
        registerCommand("quit", []() { return std::make_shared<Command>(); });
        registerCommand("clear", []() { return std::make_shared<Command>(); });
    }
};

class ScriptEngine {
public:
    // Execute command script from file
    static CommandResult executeScript(const std::string& scriptPath, 
                                      const CommandContext& context) {
        // TODO: Implement script parsing and execution
        // Supports bash-like syntax, Python-like syntax, or custom NLM script format
        return "Script execution not yet implemented";
    }
    
    // Execute command script from string
    static CommandResult executeScript(const std::string& script,
                                      const CommandContext& context) {
        // TODO: Implement inline script execution
        return "Inline script execution not yet implemented";
    }
    
    // Get supported script languages
    static std::vector<std::string> getSupportedLanguages() {
        return {"bash", "nlmscript", "python"}; // Examples
    }
};

class CommandStreamProcessor {
public:
    // Process streaming command input
    static std::string processStream(const std::string& input,
                                    std::function<void(const std::string&)> callback) {
        // TODO: Implement streaming input processing
        // For real-time command execution and output streaming
        return "Stream processing not yet implemented";
    }
    
    // Start command stream processor
    static bool startStreamProcessor() {
        // TODO: Implement stream processor startup
        return true;
    }
    
    // Stop command stream processor
    static bool stopStreamProcessor() {
        // TODO: Implement stream processor shutdown
        return true;
    }
};

} // namespace nlm
