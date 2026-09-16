// Component factory implementation for NLM
// This file implements factory patterns and component management for the NLM system

#pragma once

#include "Brain.hpp"
#include "Config.hpp"
#include "Random.hpp"
#include "Logger.hpp"
#include "SimulationClock.hpp"
#include <memory>
#include <map>
#include <functional>
#include <string>
#include <typeinfo>
#include <stdexcept>

namespace nlm {

// Component factory interface
class IComponentFactory {
public:
    virtual ~IComponentFactory() = default;
    virtual std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config) = 0;
    virtual std::shared_ptr<RandomGenerator> createRandomGenerator(const std::shared_ptr<Config>& config) = 0;
    virtual std::shared_ptr<Logger> createLogger(const std::shared_ptr<Config>& config) = 0;
    virtual std::shared_ptr<SimulationClock> createClock(const std::shared_ptr<Config>& config) = 0;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
};

// Default component factory implementation
class DefaultComponentFactory : public IComponentFactory {
public:
    DefaultComponentFactory() = default;
    
    std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config) override {
        return std::make_shared<Brain>(config);
    }
    
    std::shared_ptr<RandomGenerator> createRandomGenerator(const std::shared_ptr<Config>& config) override {
        uint64_t seed = config->getOr<int64_t>("random_seed", 42);
        return std::make_shared<RandomGenerator>(seed);
    }
    
    std::shared_ptr<Logger> createLogger(const std::shared_ptr<Config>& config) override {
        auto logger = std::make_shared<Logger>();
        std::string logLevel = config->getOr<std::string>("log_level", "Info");
        auto consoleLogger = std::make_shared<ConsoleLogger>(getLogLevelFromString(logLevel));
        logger->addLogger(consoleLogger);
        return logger;
    }
    
    std::shared_ptr<SimulationClock> createClock(const std::shared_ptr<Config>& config) override {
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        return std::make_shared<SimulationClock>(timestep);
    }
    
    bool initialize() override {
        return true;
    }
    
    void shutdown() override {
        // Cleanup resources
    }
    
private:
    LogLevel getLogLevelFromString(const std::string& level) {
        if (level == "Debug") return LogLevel::Debug;
        if (level == "Info") return LogLevel::Info;
        if (level == "Warning") return LogLevel::Warning;
        if (level == "Error") return LogLevel::Error;
        if (level == "Critical") return LogLevel::Critical;
        return LogLevel::Info; // Default
    }
};

// Component registry for managing factories
class ComponentRegistry {
public:
    static ComponentRegistry& getInstance() {
        static ComponentRegistry instance;
        return instance;
    }
    
    // Register a factory for a specific component type
    template<typename T>
    void registerFactory(const std::string& componentName,
                        std::shared_ptr<IComponentFactory> factory) {
        factories_[componentName] = std::move(factory);
    }
    
    // Get a factory for a component type
    std::shared_ptr<IComponentFactory> getFactory(const std::string& componentName) {
        auto it = factories_.find(componentName);
        if (it != factories_.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    // Check if a factory is registered
    bool hasFactory(const std::string& componentName) const {
        return factories_.find(componentName) != factories_.end();
    }
    
    // List all registered factories
    std::vector<std::string> getRegisteredFactories() const {
        std::vector<std::string> names;
        for (const auto& pair : factories_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Unregister a factory
    void unregisterFactory(const std::string& componentName) {
        factories_.erase(componentName);
    }
    
    // Create a component using a factory
    template<typename T>
    std::shared_ptr<T> createComponent(const std::string& componentName,
                                       const std::shared_ptr<Config>& config) {
        auto factory = getFactory(componentName);
        if (!factory) {
            throw std::runtime_error("No factory registered for component: " + componentName);
        }
        
        // This is a simplified type check - in a real implementation, we'd use
        // a more sophisticated type system
        auto brain = factory->createBrain(config);
        if (std::dynamic_pointer_cast<T>(brain)) {
            return std::dynamic_pointer_cast<T>(brain);
        }
        
        throw std::runtime_error("Component type mismatch for: " + componentName);
    }
    
    // Create a default brain using the registered brain factory
    std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config) {
        return getFactory("brain")->createBrain(config);
    }
    
    // Create a default random generator
    std::shared_ptr<RandomGenerator> createRandomGenerator(const std::shared_ptr<Config>& config) {
        return getFactory("random_generator")->createRandomGenerator(config);
    }
    
    // Create a default logger
    std::shared_ptr<Logger> createLogger(const std::shared_ptr<Config>& config) {
        return getFactory("logger")->createLogger(config);
    }
    
    // Create a default clock
    std::shared_ptr<SimulationClock> createClock(const std::shared_ptr<Config>& config) {
        return getFactory("clock")->createClock(config);
    }
    
    // Initialize all registered factories
    bool initializeAll() {
        bool success = true;
        for (const auto& pair : factories_) {
            if (!pair.second->initialize()) {
                NLM_LOG_ERROR("Failed to initialize factory: " + pair.first);
                success = false;
            }
        }
        return success;
    }
    
    // Shutdown all registered factories
    void shutdownAll() {
        for (const auto& pair : factories_) {
            pair.second->shutdown();
        }
    }
    
private:
    ComponentRegistry() = default;
    ComponentRegistry(const ComponentRegistry&) = delete;
    ComponentRegistry& operator=(const ComponentRegistry&) = delete;
    
    std::map<std::string, std::shared_ptr<IComponentFactory>> factories_;
};

// Component configuration builder
class ComponentConfigBuilder {
public:
    static ComponentConfigBuilder& getInstance() {
        static ComponentConfigBuilder instance;
        return instance;
    }
    
    // Add configuration for a component
    ComponentConfigBuilder& addComponentConfig(const std::string& componentName,
                                               const std::string& key,
                                               const ConfigValue& value,
                                               ConfigSource source = ConfigSource::Runtime) {
        componentConfigs_[componentName][key] = std::make_pair(value, source);
        return *this;
    }
    
    // Get configuration for a component
    std::map<std::string, std::pair<ConfigValue, ConfigSource>> getComponentConfig(const std::string& componentName) const {
        auto it = componentConfigs_.find(componentName);
        if (it != componentConfigs_.end()) {
            return it->second;
        }
        return std::map<std::string, std::pair<ConfigValue, ConfigSource>>();
    }
    
    // Check if configuration exists for a component
    bool hasComponentConfig(const std::string& componentName) const {
        return componentConfigs_.find(componentName) != componentConfigs_.end();
    }
    
    // Remove configuration for a component
    void removeComponentConfig(const std::string& componentName) {
        componentConfigs_.erase(componentName);
    }
    
    // Apply component configuration to a config object
    bool applyToConfig(const std::string& componentName, std::shared_ptr<Config> config) const {
        auto it = componentConfigs_.find(componentName);
        if (it == componentConfigs_.end()) {
            return false;
        }
        
        for (const auto& pair : it->second) {
            config->set(pair.first, pair.second.first, pair.second.second);
        }
        
        return true;
    }
    
    // Get all component names
    std::vector<std::string> getComponentNames() const {
        std::vector<std::string> names;
        for (const auto& pair : componentConfigs_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
private:
    ComponentConfigBuilder() = default;
    ComponentConfigBuilder(const ComponentConfigBuilder&) = delete;
    ComponentConfigBuilder& operator=(const ComponentConfigBuilder&) = delete;
    
    std::map<std::string, std::map<std::string, std::pair<ConfigValue, ConfigSource>>> componentConfigs_;
};

// Component lifecycle manager
class ComponentLifecycleManager {
public:
    static ComponentLifecycleManager& getInstance() {
        static ComponentLifecycleManager instance;
        return instance;
    }
    
    // Register a component lifecycle callback
    void registerLifecycleCallback(const std::string& componentName,
                                   std::function<void()> initCallback,
                                   std::function<void()> shutdownCallback) {
        lifecycleCallbacks_[componentName] = std::make_pair(initCallback, shutdownCallback);
    }
    
    // Unregister a component lifecycle callback
    void unregisterLifecycleCallback(const std::string& componentName) {
        lifecycleCallbacks_.erase(componentName);
    }
    
    // Initialize a component
    bool initializeComponent(const std::string& componentName) {
        auto it = lifecycleCallbacks_.find(componentName);
        if (it != lifecycleCallbacks_.end()) {
            it->second.first();
            return true;
        }
        return false;
    }
    
    // Shutdown a component
    bool shutdownComponent(const std::string& componentName) {
        auto it = lifecycleCallbacks_.find(componentName);
        if (it != lifecycleCallbacks_.end()) {
            it->second.second();
            return true;
        }
        return false;
    }
    
    // Check if a component is initialized
    bool isComponentInitialized(const std::string& componentName) const {
        return lifecycleCallbacks_.find(componentName) != lifecycleCallbacks_.end();
    }
    
    // List all registered components
    std::vector<std::string> getRegisteredComponents() const {
        std::vector<std::string> names;
        for (const auto& pair : lifecycleCallbacks_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
private:
    ComponentLifecycleManager() = default;
    ComponentLifecycleManager(const ComponentLifecycleManager&) = delete;
    ComponentLifecycleManager& operator=(const ComponentLifecycleManager&) = delete;
    
    std::map<std::string, std::pair<std::function<void()>, std::function<void()>>> lifecycleCallbacks_;
};

// Convenience functions for factory usage
namespace Factory {

// Initialize the factory system
inline bool initialize() {
    return ComponentRegistry::getInstance().initializeAll();
}

// Shutdown the factory system
inline void shutdown() {
    ComponentRegistry::getInstance().shutdownAll();
}

// Register default factories
inline void registerDefaults() {
    ComponentRegistry& registry = ComponentRegistry::getInstance();
    
    // Register the default component factory
    auto defaultFactory = std::make_shared<DefaultComponentFactory>();
    registry.registerFactory("brain", defaultFactory);
    registry.registerFactory("random_generator", defaultFactory);
    registry.registerFactory("logger", defaultFactory);
    registry.registerFactory("clock", defaultFactory);
    
    // Register lifecycle callbacks
    ComponentLifecycleManager& lifecycleManager = ComponentLifecycleManager::getInstance();
    lifecycleManager.registerLifecycleCallback("brain", 
        []() { NLM_LOG_INFO("Brain component initialized"); },
        []() { NLM_LOG_INFO("Brain component shutdown"); });
    
    lifecycleManager.registerLifecycleCallback("random_generator",
        []() { NLM_LOG_INFO("Random generator component initialized"); },
        []() { NLM_LOG_INFO("Random generator component shutdown"); });
    
    lifecycleManager.registerLifecycleCallback("logger",
        []() { NLM_LOG_INFO("Logger component initialized"); },
        []() { NLM_LOG_INFO("Logger component shutdown"); });
    
    lifecycleManager.registerLifecycleCallback("clock",
        []() { NLM_LOG_INFO("Clock component initialized"); },
        []() { NLM_LOG_INFO("Clock component shutdown"); });
}

// Create a brain using the factory
inline std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config) {
    return ComponentRegistry::getInstance().createBrain(config);
}

// Create a random generator using the factory
inline std::shared_ptr<RandomGenerator> createRandomGenerator(const std::shared_ptr<Config>& config) {
    return ComponentRegistry::getInstance().createRandomGenerator(config);
}

// Create a logger using the factory
inline std::shared_ptr<Logger> createLogger(const std::shared_ptr<Config>& config) {
    return ComponentRegistry::getInstance().createLogger(config);
}

// Create a clock using the factory
inline std::shared_ptr<SimulationClock> createClock(const std::shared_ptr<Config>& config) {
    return ComponentRegistry::getInstance().createClock(config);
}

// Get component registry
inline ComponentRegistry& getComponentRegistry() {
    return ComponentRegistry::getInstance();
}

// Get component config builder
inline ComponentConfigBuilder& getComponentConfigBuilder() {
    return ComponentConfigBuilder::getInstance();
}

// Get component lifecycle manager
inline ComponentLifecycleManager& getComponentLifecycleManager() {
    return ComponentLifecycleManager::getInstance();
}

// Check if the factory system is initialized
inline bool isInitialized() {
    return !ComponentRegistry::getInstance().getRegisteredFactories().empty();
}

} // namespace Factory

// Configuration template system
class ConfigTemplate {
public:
    ConfigTemplate(const std::string& name, const std::string& description)
        : name_(name), description_(description) {}
    
    void addEntry(const std::string& key, const ConfigValue& value,
                  const std::string& description = "",
                  ConfigSource source = ConfigSource::Default) {
        entries_[key] = ConfigEntry{key, value, source, description};
    }
    
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        for (const auto& pair : entries_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    std::string getValueAsString(const std::string& key) const {
        auto it = entries_.find(key);
        if (it != entries_.end()) {
            return getConfigValueAsString(it->second.value);
        }
        return "";
    }
    
    bool hasKey(const std::string& key) const {
        return entries_.find(key) != entries_.end();
    }
    
    ConfigEntry getEntry(const std::string& key) const {
        auto it = entries_.find(key);
        if (it != entries_.end()) {
            return it->second;
        }
        return ConfigEntry();
    }
    
private:
    std::string getConfigValueAsString(const ConfigValue& value) const {
        return std::visit([](auto&& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T, int>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << arg[i];
                }
                ss << "]";
                return ss.str();
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << arg[i];
                }
                ss << "]";
                return ss.str();
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << "\"" << arg[i] << "\"";
                }
                ss << "]";
                return ss.str();
            } else {
                return "\"UNKNOWN_TYPE\"";
            }
        }, value);
    }
    
    std::string name_;
    std::string description_;
    std::map<std::string, ConfigEntry> entries_;
};

// Configuration template registry
class ConfigTemplateRegistry {
public:
    static ConfigTemplateRegistry& getInstance() {
        static ConfigTemplateRegistry instance;
        return instance;
    }
    
    void registerTemplate(std::shared_ptr<ConfigTemplate> template) {
        templates_[template->getName()] = template;
    }
    
    std::shared_ptr<ConfigTemplate> getTemplate(const std::string& name) const {
        auto it = templates_.find(name);
        if (it != templates_.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    bool hasTemplate(const std::string& name) const {
        return templates_.find(name) != templates_.end();
    }
    
    void unregisterTemplate(const std::string& name) {
        templates_.erase(name);
    }
    
    std::vector<std::string> getTemplateNames() const {
        std::vector<std::string> names;
        for (const auto& pair : templates_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    bool applyTemplate(const std::string& name, std::shared_ptr<Config> config) {
        auto template = getTemplate(name);
        if (!template) {
            return false;
        }
        
        for (const auto& key : template->getKeys()) {
            std::string value = template->getValueAsString(key);
            config->set(key, value, ConfigSource::File);
        }
        
        return true;
    }
    
private:
    ConfigTemplateRegistry() = default;
    ConfigTemplateRegistry(const ConfigTemplateRegistry&) = delete;
    ConfigTemplateRegistry& operator=(const ConfigTemplateRegistry&) = delete;
    
    std::map<std::string, std::shared_ptr<ConfigTemplate>> templates_;
};

// Factory utilities namespace
namespace FactoryUtils {

// Create a neural network configuration template
inline std::shared_ptr<ConfigTemplate> createNeuralNetworkTemplate(const std::string& name,
                                                                const std::string& description) {
    auto template = std::make_shared<ConfigTemplate>(name, description);
    
    // Add neural network parameters
    template->addEntry("neuron_count", static_cast<int64_t>(100),
                      "Number of neurons in the network");
    template->addEntry("region_count", static_cast<int64_t>(1),
                      "Number of brain regions");
    template->addEntry("connection_probability", 0.15f,
                      "Probability of connection between neurons");
    template->addEntry("simulation_timestep", 0.001,
                      "Time step for simulation (ms)");
    
    return template;
}

// Create a learning configuration template
inline std::shared_ptr<ConfigTemplate> createLearningTemplate(const std::string& name,
                                                             const std::string& description) {
    auto template = std::make_shared<ConfigTemplate>(name, description);
    
    // Add learning parameters
    template->addEntry("stdp_ltp_weight", 0.02f,
                      "STDP LTP weight");
    template->addEntry("stdp_ltd_weight", 0.015f,
                      "STDP LTD weight");
    template->addEntry("stdp_time_constant", 20.0f,
                      "STDP time constant (ms)");
    template->addEntry("hebbian_learning_rate", 0.01f,
                      "Hebbian learning rate");
    
    return template;
}

// Create a structural plasticity configuration template
inline std::shared_ptr<ConfigTemplate> createStructuralPlasticityTemplate(const std::string& name,
                                                                      const std::string& description) {
    auto template = std::make_shared<ConfigTemplate>(name, description);
    
    // Add structural plasticity parameters
    template->addEntry("synaptogenesis_rate", 0.0001f,
                      "Synaptogenesis rate");
    template->addEntry("pruning_rate", 0.00001f,
                      "Pruning rate");
    
    return template;
}

// Create an experiment configuration template
inline std::shared_ptr<ConfigTemplate> createExperimentTemplate(const std::string& name,
                                                               const std::string& description) {
    auto template = std::make_shared<ConfigTemplate>(name, description);
    
    // Add experiment parameters
    template->addEntry("neuron_count", static_cast<int64_t>(500),
                      "Number of neurons for experiments");
    template->addEntry("simulation_timestep", 0.001,
                      "Time step for simulation");
    template->addEntry("connection_probability", 0.15f,
                      "Connection probability");
    
    return template;
}

} // namespace FactoryUtils

} // namespace nlm