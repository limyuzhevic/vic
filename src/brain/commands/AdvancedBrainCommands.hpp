#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <unordered_map>
#include "../core/Config/Config.hpp"
#include "ExtensionSystem.hpp"
#include "ExtensionManager.hpp"
#include "factory/ComponentFactory.hpp"

namespace nlm {

// Forward declarations
class Brain;
class Neuromodulator;
class MemorySystem;
class CognitiveFunction;
class CommandRegistry;
class ExtensionSystem;

// Advanced brain commands for component creation, extension management, and customization
template<typename ComponentType>
class AdvancedBrainCommand {
public:
    virtual ~AdvancedBrainCommand() = default;
    
    // Get command name
    virtual std::string getName() const = 0;
    
    // Get command description
    virtual std::string getDescription() const = 0;
    
    // Execute command
    virtual bool execute(const std::vector<std::string>& args, 
                        std::shared_ptr<Config> config,
                        std::shared_ptr<Brain> brain = nullptr) = 0;
    
    // Get command parameters
    virtual std::vector<std::string> getParameters() const = 0;
    
    // Check if command requires brain
    virtual bool requiresBrain() const { return false; }
    
    // Get command category
    virtual std::string getCategory() const { return "general"; }
    
    // Check if command is administrative
    virtual bool isAdmin() const { return false; }
};

// Command categories
namespace CommandCategories {
    const std::string COMPONENT = "component";
    const std::string EXTENSION = "extension";
    const std::string PLUGIN = "plugin";
    const std::string BRAIN = "brain";
    const std::string SYSTEM = "system";
    const std::string CONFIGURATION = "configuration";
}

// Brain commands for component creation
class CreateNeuromodulatorCommand : public AdvancedBrainCommand<Neuromodulator> {
public:
    std::string getName() const override { return "create_neuromodulator"; }
    std::string getDescription() const override { 
        return "Create a custom neuromodulator component"; 
    }
    bool execute(const std::vector<std::string>& args, 
                std::shared_ptr<Config> config,
                std::shared_ptr<Brain> brain = nullptr) override;
    std::vector<std::string> getParameters() const override {
        return {"name", "type", "level", "decay_rate"};
    }
    bool requiresBrain() const override { return true; }
    std::string getCategory() const override { return CommandCategories::COMPONENT; }
};

class CreateMemorySystemCommand : public AdvancedBrainCommand<MemorySystem> {
public:
    std::string getName() const override { return "create_memory_system"; }
    std::string getDescription() const override { 
        return "Create a custom memory system component"; 
    }
    bool execute(const std::vector<std::string>& args, 
                std::shared_ptr<Config> config,
                std::shared_ptr<Brain> brain = nullptr) override;
    std::vector<std::string> getParameters() const override {
        return {"name", "type", "capacity", "max_episodes"};
    }
    bool requiresBrain() const override { return true; }
    std::string getCategory() const override { return CommandCategories::COMPONENT; }
};

class CreateCognitiveFunctionCommand : public AdvancedBrainCommand<CognitiveFunction> {
public:
    std::string getName() const override { return "create_cognitive_function"; }
    std::string getDescription() const override { 
        return "Create a custom cognitive function component"; 
    }
    bool execute(const std::vector<std::string>& args, 
                std::shared_ptr<Config> config,
                std::shared_ptr<Brain> brain = nullptr) override;
    std::vector<std::string> getParameters() const override {
        return {"name", "type", "planning_depth", "inhibition_strength"};
    }
    bool requiresBrain() const override { return true; }
    std::string getCategory() const override { return CommandCategories::COMPONENT; }
};

// Brain commands for extension management
class LoadExtensionCommand : public AdvancedBrainCommand<void> {
public:
    std::string getName() const override { return "load_extension"; }
    std::string getDescription() const override { 
        return "Load an extension from a module or file"; 
    }
    bool execute(const std::vector<std::string>& args, 
                std::shared_ptr<Config> config,
                std::shared_ptr<Brain> brain = nullptr) override;
    std::vector<std::string> getParameters() const override {
        return {"name", "module_path", "auto_init", "parameters"};
    }
    bool requiresBrain() const override { return true; }
    std::string getCategory() const override { return CommandCategories::EXTENSION; }
};

class UnloadExtensionCommand : public AdvancedBrainCommand<void> {
public:
    std::string getName() const override { return "unload_extension"; }
    std::string getDescription() const override { 
        return "Unload an extension"; 
    }
    bool execute(const std::vector<std::string>& args, 
                std::shared_ptr<Config> config,
                std::shared_ptr<Brain> brain = nullptr) override;
    std::vector<std::string> getParameters() const override {
        return {"name"};
    }
    bool requiresBrain() const override { return true; }
    std::string getCategory() const override { return CommandCategories::EXTENSION; }
};

class ListExtensionsCommand : public AdvancedBrainCommand<void> {
public:
    std::string getName() const override { return "list_extensions"; }
    std::string getDescription() const override { 
        return "List loaded extensions"; 
    }
    bool execute(const std::vector<std::string>& args, 
                std::shared_ptr<Config> config,
                std::shared_ptr<Brain> brain = nullptr) override;
    std::vector<std::string> getParameters() const override {
        return {"category", "state"};
    }
    std::string getCategory() const override { return CommandCategories::EXTENSION; }
};

// Brain commands for system management
class SystemStatusCommand : public AdvancedBrainCommand<void> {
public:
    std::string getName() const override { return "system_status"; }
    std::string getDescription() const override { 
        return "Get system status and statistics"; 
    }
    bool execute(const std::vector<std::string>& args, 
                std::shared_ptr<Config> config,
                std::shared_ptr<Brain> brain = nullptr) override;
    std::vector<std::string> getParameters() const override {
        return {"detailed"};
    }
    std::string getCategory() const override { return CommandCategories::SYSTEM; }
};

class ExtensionPointStatusCommand : public AdvancedBrainCommand<void> {
public:
    std::string getName() const override { return "extension_point_status"; }
    std::string getDescription() const override { 
        return "Get extension point status and handlers"; 
    }
    bool execute(const std::vector<std::string>& args, 
                std::shared_ptr<Config> config,
                std::shared_ptr<Brain> brain = nullptr) override;
    std::vector<std::string> getParameters() const override {
        return {"point_name"};
    }
    std::string getCategory() const override { return CommandCategories::SYSTEM; }
};

class EventSystemStatusCommand : public AdvancedBrainCommand<void> {
public:
    std::string getName() const override { return "event_system_status"; }
    std::string getDescription() const override { 
        return "Get event system status and listeners"; 
    }
    bool execute(const std::vector<std::string>& args, 
                std::shared_ptr<Config> config,
                std::shared_ptr<Brain> brain = nullptr) override;
    std::string getCategory() const override { return CommandCategories::SYSTEM; }
};

// Brain commands for configuration
class ConfigurationCommand : public AdvancedBrainCommand<void> {
public:
    std::string getName() const override { return "configuration"; }
    std::string getDescription() const override { 
        return "Configuration management for extensions"; 
    }
    bool execute(const std::vector<std::string>& args, 
                std::shared_ptr<Config> config,
                std::shared_ptr<Brain> brain = nullptr) override;
    std::vector<std::string> getParameters() const override {
        return {"operation", "name", "value"};
    }
    std::string getCategory() const override { return CommandCategories::CONFIGURATION; }
};

class PluginManagementCommand : public AdvancedBrainCommand<void> {
public:
    std::string getName() const override { return "plugin_management"; }
    std::string getDescription() const override { 
        return "Plugin management commands"; 
    }
    bool execute(const std::vector<std::string>& args, 
                std::shared_ptr<Config> config,
                std::shared_ptr<Brain> brain = nullptr) override;
    std::vector<std::string> getParameters() const override {
        return {"operation", "plugin_name", "parameters"};
    }
    std::string getCategory() const override { return CommandCategories::PLUGIN; }
};

// Advanced brain command registry
class AdvancedBrainCommandRegistry {
public:
    // Register a command
    bool registerCommand(std::unique_ptr<AdvancedBrainCommand<void>> command) {
        if (!command) {
            return false;
        }
        
        std::string name = command->getName();
        if (commands_.find(name) != commands_.end()) {
            NLM_LOG_WARNING("Command already registered: " + name);
            return false;
        }
        
        commands_[name] = std::move(command);
        NLM_LOG_INFO("Registered advanced brain command: " + name);
        return true;
    }
    
    // Get command
    AdvancedBrainCommand<void>* getCommand(const std::string& name) {
        auto it = commands_.find(name);
        if (it != commands_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    // Execute command
    bool executeCommand(const std::string& name,
                       const std::vector<std::string>& args,
                       std::shared_ptr<Config> config,
                       std::shared_ptr<Brain> brain = nullptr) {
        auto it = commands_.find(name);
        if (it == commands_.end()) {
            NLM_LOG_ERROR("Command not found: " + name);
            return false;
        }
        
        const auto& command = it->second;
        
        if (command->requiresBrain() && !brain) {
            NLM_LOG_ERROR("Command requires brain: " + name);
            return false;
        }
        
        try {
            bool result = command->execute(args, config, brain);
            
            if (result) {
                NLM_LOG_INFO("Executed advanced brain command: " + name);
            } else {
                NLM_LOG_ERROR("Advanced brain command failed: " + name);
            }
            
            return result;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in command " + name + ": " + e.what()));
            return false;
        }
    }
    
    // Get all commands
    std::vector<std::string> getCommandNames() const {
        std::vector<std::string> names;
        names.reserve(commands_.size());
        for (const auto& pair : commands_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get commands by category
    std::vector<std::string> getCommandsByCategory(const std::string& category) const {
        std::vector<std::string> names;
        for (const auto& pair : commands_) {
            if (pair.second->getCategory() == category) {
                names.push_back(pair.first);
            }
        }
        return names;
    }
    
    // Remove command
    bool removeCommand(const std::string& name) {
        auto it = commands_.find(name);
        if (it != commands_.end()) {
            commands_.erase(it);
            NLM_LOG_INFO("Removed command: " + name);
            return true;
        }
        return false;
    }
    
    // Clear all commands
    void clear() {
        commands_.clear();
        NLM_LOG_INFO("Cleared all commands");
    }
    
    // Get command count
    size_t getCommandCount() const {
        return commands_.size();
    }
    
    // Check if command exists
    bool hasCommand(const std::string& name) const {
        return commands_.find(name) != commands_.end();
    }
    
private:
    std::unordered_map<std::string, std::unique_ptr<AdvancedBrainCommand<void>>> commands_;
};

} // namespace nlm
