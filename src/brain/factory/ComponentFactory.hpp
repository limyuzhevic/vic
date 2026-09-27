#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <type_traits>
#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Forward declarations
class Brain;
class Neuromodulator;
class MemorySystem;
class CognitiveFunction;

// Component registration and lifecycle management
template<typename ComponentType>
class ComponentRegistry {
public:
    using CreatorFunction = std::function<std::shared_ptr<ComponentType>(std::shared_ptr<Config>, std::shared_ptr<Brain>)>;
    using DestructorFunction = std::function<void(std::shared_ptr<ComponentType>)>;
    
    static ComponentRegistry& getInstance() {
        static ComponentRegistry instance;
        return instance;
    }
    
    // Register a component type
    void registerComponent(const std::string& name, 
                          CreatorFunction creator,
                          DestructorFunction destructor = nullptr) {
        creators_[name] = creator;
        destructors_[name] = destructor ? destructor : 
            [](std::shared_ptr<ComponentType> comp) {};
    }
    
    // Create a component instance
    std::shared_ptr<ComponentType> create(const std::string& name,
                                         std::shared_ptr<Config> config,
                                         std::shared_ptr<Brain> brain) {
        auto it = creators_.find(name);
        if (it != creators_.end()) {
            return it->second(config, brain);
        }
        return nullptr;
    }
    
    // Check if component type is registered
    bool isRegistered(const std::string& name) const {
        return creators_.find(name) != creators_.end();
    }
    
    // Get all registered component names
    std::vector<std::string> getRegisteredComponents() const {
        std::vector<std::string> names;
        names.reserve(creators_.size());
        for (const auto& pair : creators_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Unregister a component type
    void unregisterComponent(const std::string& name) {
        creators_.erase(name);
        destructors_.erase(name);
    }
    
private:
    ComponentRegistry() = default;
    ComponentRegistry(const ComponentRegistry&) = delete;
    ComponentRegistry& operator=(const ComponentRegistry&) = delete;
    
    std::unordered_map<std::string, CreatorFunction> creators_;
    std::unordered_map<std::string, DestructorFunction> destructors_;
};

// Component factory for creating custom brain components
template<typename ComponentType>
class ComponentFactory {
public:
    // Create component with configuration
    static std::shared_ptr<ComponentType> create(
        const std::string& componentName,
        std::shared_ptr<Config> config,
        std::shared_ptr<Brain> brain = nullptr,
        const std::unordered_map<std::string, std::string>& params = {}) {
        
        auto registry = ComponentRegistry<ComponentType>::getInstance();
        
        if (!registry.isRegistered(componentName)) {
            // Try to create using componentName as type name
            std::shared_ptr<ComponentType> component;
            
            // Create based on component type
            if constexpr (std::is_same<ComponentType, Neuromodulator>::value) {
                component = createNeuromodulator(componentName, config, brain, params);
            } else if constexpr (std::is_same<ComponentType, MemorySystem>::value) {
                component = createMemorySystem(componentName, config, brain, params);
            } else if constexpr (std::is_same<ComponentType, CognitiveFunction>::value) {
                component = createCognitiveFunction(componentName, config, brain, params);
            } else {
                // Generic component creation
                component = createGenericComponent(componentName, config, brain, params);
            }
            
            if (component) {
                registry.registerComponent(componentName, 
                    [component](std::shared_ptr<Config>, std::shared_ptr<Brain>) -> std::shared_ptr<ComponentType> {
                        return component;
                    });
            }
            
            return component;
        }
        
        return registry.create(componentName, config, brain);
    }
    
private:
    // Create neuromodulator components
    static std::shared_ptr<Neuromodulator> createNeuromodulator(
        const std::string& type,
        std::shared_ptr<Config> config,
        std::shared_ptr<Brain> brain,
        const std::unordered_map<std::string, std::string>& params);
    
    // Create memory system components
    static std::shared_ptr<MemorySystem> createMemorySystem(
        const std::string& type,
        std::shared_ptr<Config> config,
        std::shared_ptr<Brain> brain,
        const std::unordered_map<std::string, std::string>& params);
    
    // Create cognitive function components
    static std::shared_ptr<CognitiveFunction> createCognitiveFunction(
        const std::string& type,
        std::shared_ptr<Config> config,
        std::shared_ptr<Brain> brain,
        const std::unordered_map<std::string, std::string>& params);
    
    // Create generic components
    static std::shared_ptr<ComponentType> createGenericComponent(
        const std::string& type,
        std::shared_ptr<Config> config,
        std::shared_ptr<Brain> brain,
        const std::unordered_map<std::string, std::string>& params);
};

// Extension point hooks for custom integration and behavior modification
template<typename ComponentType>
class ExtensionPoint {
public:
    using HandlerFunction = std::function<bool(std::shared_ptr<ComponentType>, const std::unordered_map<std::string, std::any>&)>;
    
    // Add an extension point handler
    void addHandler(const std::string& pointName, HandlerFunction handler) {
        handlers_[pointName].push_back(handler);
    }
    
    // Execute all handlers for an extension point
    std::vector<bool> execute(const std::string& pointName, 
                             std::shared_ptr<ComponentType> component,
                             const std::unordered_map<std::string, std::any>& data = {}) {
        std::vector<bool> results;
        auto it = handlers_.find(pointName);
        if (it != handlers_.end()) {
            for (const auto& handler : it->second) {
                results.push_back(handler(component, data));
            }
        }
        return results;
    }
    
    // Get all extension point names
    std::vector<std::string> getExtensionPoints() const {
        std::vector<std::string> names;
        names.reserve(handlers_.size());
        for (const auto& pair : handlers_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Remove a handler from an extension point
    void removeHandler(const std::string& pointName, size_t index) {
        auto it = handlers_.find(pointName);
        if (it != handlers_.end() && index < it->second.size()) {
            it->second.erase(it->second.begin() + index);
        }
    }
    
private:
    std::unordered_map<std::string, std::vector<HandlerFunction>> handlers_;
};

// Event system for extending brain behavior
template<typename EventType>
class EventSystem {
public:
    using EventHandler = std::function<void(const EventType&)>;
    
    // Add event handler
    void addEventHandler(const std::string& eventName, EventHandler handler) {
        handlers_[eventName].push_back(handler);
    }
    
    // Emit an event
    void emitEvent(const std::string& eventName, const EventType& event) {
        auto it = handlers_.find(eventName);
        if (it != handlers_.end()) {
            for (const auto& handler : it->second) {
                handler(event);
            }
        }
    }
    
    // Get all event names
    std::vector<std::string> getEventNames() const {
        std::vector<std::string> names;
        names.reserve(handlers_.size());
        for (const auto& pair : handlers_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
private:
    std::unordered_map<std::string, std::vector<EventHandler>> handlers_;
};

// Plugin architecture with dynamic loading and unloading
class PluginManager {
public:
    // Plugin interface
    class PluginInterface {
    public:
        virtual ~PluginInterface() = default;
        virtual std::string getName() const = 0;
        virtual std::string getVersion() const = 0;
        virtual bool initialize(std::shared_ptr<Config>, std::shared_ptr<Brain>) = 0;
        virtual void shutdown() = 0;
        virtual std::vector<std::string> getComponentTypes() const = 0;
    };
    
    // Register a plugin
    void registerPlugin(std::unique_ptr<PluginInterface> plugin) {
        plugins_.push_back(std::move(plugin));
    }
    
    // Initialize all plugins
    bool initializePlugins(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) {
        bool success = true;
        for (auto& plugin : plugins_) {
            if (!plugin->initialize(config, brain)) {
                NLM_LOG_ERROR("Failed to initialize plugin: " + plugin->getName());
                success = false;
            } else {
                NLM_LOG_INFO("Initialized plugin: " + plugin->getName() + " v" + plugin->getVersion());
            }
        }
        return success;
    }
    
    // Shutdown all plugins
    void shutdownPlugins() {
        for (auto& plugin : plugins_) {
            NLM_LOG_INFO("Shutting down plugin: " + plugin->getName());
            plugin->shutdown();
        }
    }
    
    // Get plugin by name
    PluginInterface* getPlugin(const std::string& name) {
        for (auto& plugin : plugins_) {
            if (plugin->getName() == name) {
                return plugin.get();
            }
        }
        return nullptr;
    }
    
    // Get all plugin names
    std::vector<std::string> getPluginNames() const {
        std::vector<std::string> names;
        names.reserve(plugins_.size());
        for (const auto& plugin : plugins_) {
            names.push_back(plugin->getName());
        }
        return names;
    }
    
private:
    std::vector<std::unique_ptr<PluginInterface>> plugins_;
};

// Command system for registering and managing custom commands
class CommandRegistry {
public:
    // Command function type
    using CommandFunction = std::function<bool(const std::vector<std::string>&, std::shared_ptr<Config>)>;
    
    // Command metadata
    struct CommandMetadata {
        std::string name;
        std::string description;
        std::vector<std::string> parameters;
        bool requiresBrain = false;
        bool isAdmin = false;
        std::string category;
    };
    
    // Register a command
    void registerCommand(const std::string& name,
                        CommandFunction command,
                        const std::string& description = "",
                        const std::vector<std::string>& parameters = {},
                        bool requiresBrain = false,
                        bool isAdmin = false,
                        const std::string& category = "general") {
        CommandMetadata metadata;
        metadata.name = name;
        metadata.description = description;
        metadata.parameters = parameters;
        metadata.requiresBrain = requiresBrain;
        metadata.isAdmin = isAdmin;
        metadata.category = category;
        
        commands_[name] = {metadata, command};
    }
    
    // Execute a command
    bool executeCommand(const std::string& name,
                       const std::vector<std::string>& args,
                       std::shared_ptr<Config> config,
                       std::shared_ptr<Brain> brain = nullptr) {
        auto it = commands_.find(name);
        if (it == commands_.end()) {
            NLM_LOG_ERROR("Unknown command: " + name);
            return false;
        }
        
        const auto& command = it->second;
        
        // Check if brain is required
        if (command.metadata.requiresBrain && !brain) {
            NLM_LOG_ERROR("Command requires brain: " + name);
            return false;
        }
        
        // Execute command
        bool success = command.function(args, config);
        
        if (success) {
            NLM_LOG_INFO("Executed command: " + name);
        } else {
            NLM_LOG_ERROR("Command failed: " + name);
        }
        
        return success;
    }
    
    // Get command metadata
    CommandMetadata* getCommand(const std::string& name) {
        auto it = commands_.find(name);
        if (it != commands_.end()) {
            return &it->second.metadata;
        }
        return nullptr;
    }
    
    // Get all command names
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
            if (pair.second.metadata.category == category) {
                names.push_back(pair.first);
            }
        }
        return names;
    }
    
private:
    struct CommandEntry {
        CommandMetadata metadata;
        CommandFunction function;
    };
    
    std::unordered_map<std::string, CommandEntry> commands_;
};

// Advanced configuration system for extensions
class ExtensionConfiguration {
public:
    // Load configuration from file
    static bool loadFromFile(const std::string& filepath, std::shared_ptr<Config>& config) {
        config = std::make_shared<Config>();
        return config->loadFromFile(filepath);
    }
    
    // Save configuration to file
    static bool saveToFile(const std::string& filepath, std::shared_ptr<Config> config) {
        return config->saveToFile(filepath);
    }
    
    // Apply extension-specific configuration
    static bool applyExtensionConfig(std::shared_ptr<Config> config,
                                    const std::string& extensionName,
                                    const std::unordered_map<std::string, std::any>& extensionConfig) {
        try {
            // Validate extension configuration
            for (const auto& pair : extensionConfig) {
                std::string key = extensionName + "_" + pair.first;
                // Convert any value to appropriate type and set
                if (std::holds_alternative<int>(pair.second)) {
                    config->set(key, std::get<int>(pair.second));
                } else if (std::holds_alternative<double>(pair.second)) {
                    config->set(key, std::get<double>(pair.second));
                } else if (std::holds_alternative<bool>(pair.second)) {
                    config->set(key, std::get<bool>(pair.second));
                } else if (std::holds_alternative<std::string>(pair.second)) {
                    config->set(key, std::get<std::string>(pair.second));
                }
            }
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error applying extension config: ") + e.what());
            return false;
        }
    }
    
    // Get extension configuration
    static std::unordered_map<std::string, std::any> getExtensionConfig(std::shared_ptr<Config> config,
                                                                       const std::string& extensionName) {
        std::unordered_map<std::string, std::any> result;
        
        for (const auto& key : config->getKeys()) {
            if (key.find(extensionName + "_") == 0) {
                std::string suffix = key.substr(extensionName.length() + 1);
                std::any value = config->get<ConfigValue>(key).value_or(ConfigValue());
                result[suffix] = value;
            }
        }
        
        return result;
    }
};

} // namespace nlm
