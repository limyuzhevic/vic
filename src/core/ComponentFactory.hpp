// ComponentFactory.hpp
// Abstract factory interface for brain component creation with configuration support

#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <stdexcept>
#include <vector>
#include <set>
#include <algorithm>
#include <sstream>

namespace nlm {

// Component factory exception hierarchy
class ComponentFactoryException : public std::runtime_error {
public:
    explicit ComponentFactoryException(const std::string& message)
        : std::runtime_error(message) {}
};

class ComponentCreationException : public ComponentFactoryException {
public:
    explicit ComponentCreationException(const std::string& message)
        : ComponentFactoryException(message) {}
};

class ComponentRegistrationException : public ComponentFactoryException {
public:
    explicit ComponentRegistrationException(const std::string& message)
        : ComponentFactoryException(message) {}
};

// Component metadata for type information and dependency management
struct ComponentMetadata {
    std::string name;
    std::string typeName;
    std::string description;
    std::set<std::string> dependencies;  // Components that must be created first
    bool requiresConfig = false;
    bool isCore = false;
    int initializationOrder = 0;
    
    ComponentMetadata() : name(), typeName(), description(), dependencies(), 
                          requiresConfig(false), isCore(false), initializationOrder(0) {}
    
    ComponentMetadata(const std::string& n, const std::string& type, const std::string& desc = "",
                      const std::set<std::string>& deps = {})
        : name(n), typeName(type), description(desc), dependencies(deps),
          requiresConfig(false), isCore(false), initializationOrder(0) {}
};

// Abstract factory interface - defines the contract for component creation
class IComponentFactory {
public:
    virtual ~IComponentFactory() = default;
    
    // Create a component of the specified type (basic creation)
    virtual std::unique_ptr<void> createComponent(const std::string& componentType,
                                                 const std::shared_ptr<Config>& config,
                                                 const std::shared_ptr<RandomGenerator>& rng) = 0;
    
    // Create a component with dependency injection (advanced creation)
    virtual std::unique_ptr<void> createComponent(const std::string& componentType,
                                                 const std::shared_ptr<Config>& config,
                                                 const std::shared_ptr<RandomGenerator>& rng,
                                                 const std::map<std::string, std::unique_ptr<void>>& dependencies) = 0;
    
    // Check if component type is supported by this factory
    virtual bool supportsComponent(const std::string& componentType) const = 0;
    
    // Get metadata about a supported component
    virtual ComponentMetadata getComponentMetadata(const std::string& componentType) const = 0;
    
    // Get the factory type name
    virtual std::string getComponentTypeName() const = 0;
    
    // Initialize factory with configuration
    virtual bool initialize(const std::shared_ptr<Config>& config) = 0;
    
    // Cleanup factory resources
    virtual void cleanup() = 0;
    
    // Get all supported component types
    virtual std::vector<std::string> getSupportedComponentTypes() const = 0;
    
    // Get initialization order for component type (used for dependency management)
    virtual int getInitializationOrder(const std::string& componentType) const = 0;
};

// Component creator function type - function signature for creating components
using ComponentCreator = std::function<std::unique_ptr<void>(const std::shared_ptr<Config>& config,
                                                             const std::shared_ptr<RandomGenerator>& rng,
                                                             const std::map<std::string, std::unique_ptr<void>>& dependencies,
                                                             const ComponentMetadata& metadata)>;

// Plugin interface for extending factory functionality
class IComponentPlugin {
public:
    virtual ~IComponentPlugin() = default;
    
    // Return the component type this plugin handles
    virtual std::string getComponentType() const = 0;
    
    // Create a component instance - plugins override this to provide custom creation logic
    virtual std::unique_ptr<void> createComponent(const std::shared_ptr<Config>& config,
                                                  const std::shared_ptr<RandomGenerator>& rng,
                                                  const std::map<std::string, std::unique_ptr<void>>& dependencies,
                                                  const ComponentMetadata& metadata) = 0;
    
    // Check if plugin is compatible with given configuration
    virtual bool isCompatible(const std::shared_ptr<Config>& config) const = 0;
    
    // Cleanup plugin resources
    virtual void cleanup() {}
    
    // Get plugin information
    virtual std::string getPluginName() const { return "BasePlugin"; }
    virtual std::string getPluginVersion() const { return "1.0.0"; }
};

// Default component factory implementation with comprehensive configuration support
class ComponentFactory : public IComponentFactory {
public:
    ComponentFactory() = default;
    ~ComponentFactory() override {
        cleanup();
    }
    
    // IComponentFactory interface implementation
    std::unique_ptr<void> createComponent(const std::string& componentType,
                                          const std::shared_ptr<Config>& config,
                                          const std::shared_ptr<RandomGenerator>& rng) override {
        return createComponent(componentType, config, rng, std::map<std::string, std::unique_ptr<void>>());
    }
    
    std::unique_ptr<void> createComponent(const std::string& componentType,
                                          const std::shared_ptr<Config>& config,
                                          const std::shared_ptr<RandomGenerator>& rng,
                                          const std::map<std::string, std::unique_ptr<void>>& dependencies) override {
        // Get component metadata
        auto it = componentMetadata.find(componentType);
        if (it == componentMetadata.end()) {
            throw ComponentCreationException("Unsupported component type: " + componentType);
        }
        
        const ComponentMetadata& metadata = it->second;
        
        // Check if plugin handles this component type
        auto pluginIt = plugins.find(componentType);
        if (pluginIt != plugins.end()) {
            return pluginIt->second(config, rng, dependencies, metadata);
        }
        
        // Try default creators
        auto creatorIt = creators.find(componentType);
        if (creatorIt != creators.end()) {
            return creatorIt->second(config, rng, dependencies, metadata);
        }
        
        throw ComponentCreationException("Unsupported component type: " + componentType);
    }
    
    bool supportsComponent(const std::string& componentType) const override {
        return componentMetadata.find(componentType) != componentMetadata.end() &&
               (plugins.find(componentType) != plugins.end() || 
                creators.find(componentType) != creators.end());
    }
    
    ComponentMetadata getComponentMetadata(const std::string& componentType) const override {
        auto it = componentMetadata.find(componentType);
        if (it != componentMetadata.end()) {
            return it->second;
        }
        // Return empty metadata for unsupported types
        return ComponentMetadata();
    }
    
    std::string getComponentTypeName() const override {
        return "DefaultComponentFactory";
    }
    
    bool initialize(const std::shared_ptr<Config>& config) override {
        config_ = config;
        
        // Initialize all plugins
        for (auto& plugin : pluginsList) {
            try {
                if (!plugin->isCompatible(config)) {
                    continue;
                }
            } catch (const std::exception& e) {
                // Log error
            }
        }
        
        return true;
    }
    
    void cleanup() override {
        // Cleanup plugins
        for (auto& plugin : pluginsList) {
            try {
                plugin->cleanup();
            } catch (...) {
                // Ignore exceptions during cleanup
            }
        }
        pluginsList.clear();
        plugins.clear();
        creators.clear();
        componentMetadata.clear();
        config_.reset();
    }
    
    // Register a default component creator
    template<typename ComponentType, typename CreatorFunc>
    void registerComponentCreator(const std::string& componentType, CreatorFunc creator) {
        if (componentMetadata.find(componentType) == componentMetadata.end()) {
            throw ComponentRegistrationException("Component type not in registry: " + componentType);
        }
        if (creators.find(componentType) != creators.end()) {
            throw ComponentRegistrationException("Component type already registered: " + componentType);
        }
        if (plugins.find(componentType) != plugins.end()) {
            throw ComponentRegistrationException("Plugin already registered for component type: " + componentType);
        }
        creators[componentType] = std::function<std::unique_ptr<void>(const std::shared_ptr<Config>&,
                                                                     const std::shared_ptr<RandomGenerator>&,
                                                                     const std::map<std::string, std::unique_ptr<void>>&,
                                                                     const ComponentMetadata&)>(creator);
    }
    
    // Register a plugin
    void registerPlugin(std::unique_ptr<IComponentPlugin> plugin) {
        std::string componentType = plugin->getComponentType();
        
        // Check if type is registered
        if (componentMetadata.find(componentType) == componentMetadata.end()) {
            throw ComponentRegistrationException("Component type not registered: " + componentType);
        }
        
        // Check for duplicates
        if (plugins.find(componentType) != plugins.end()) {
            throw ComponentRegistrationException("Plugin already registered for component type: " + componentType);
        }
        
        plugins[componentType] = [plugin = plugin.get()](const std::shared_ptr<Config>& config,
                                                       const std::shared_ptr<RandomGenerator>& rng,
                                                       const std::map<std::string, std::unique_ptr<void>>& dependencies,
                                                       const ComponentMetadata& metadata) {
            return plugin->createComponent(config, rng, dependencies, metadata);
        };
        
        pluginsList.push_back(std::move(plugin));
    }
    
    // Get a component by name (type-erased)
    template<typename ComponentType>
    std::unique_ptr<ComponentType> getComponent(const std::string& componentType,
                                               const std::shared_ptr<Config>& config,
                                               const std::shared_ptr<RandomGenerator>& rng,
                                               const std::map<std::string, std::unique_ptr<void>>& dependencies = {}) {
        auto component = createComponent(componentType, config, rng, dependencies);
        return std::unique_ptr<ComponentType>(static_cast<ComponentType*>(component.release()));
    }
    
    // Create all standard components at once in dependency order
    std::map<std::string, std::unique_ptr<void>> createAllComponents(
        const std::shared_ptr<Config>& config,
        const std::shared_ptr<RandomGenerator>& rng) {
        std::map<std::string, std::unique_ptr<void>> components;
        
        // Get all component types sorted by initialization order
        std::vector<std::pair<std::string, int>> componentTypes;
        for (const auto& pair : componentMetadata) {
            componentTypes.emplace_back(pair.first, pair.second.initializationOrder);
        }
        
        std::sort(componentTypes.begin(), componentTypes.end(),
                 [](const auto& a, const auto& b) { return a.second < b.second; });
        
        // Create components in order, respecting dependencies
        for (const auto& componentType : componentTypes) {
            const std::string& type = componentType.first;
            
            // Check if all dependencies are available
            auto it = componentMetadata.find(type);
            if (it == componentMetadata.end()) continue;
            
            bool dependenciesMet = true;
            std::map<std::string, std::unique_ptr<void>> dependencyMap;
            
            for (const auto& dep : it->second.dependencies) {
                auto depIt = components.find(dep);
                if (depIt == components.end()) {
                    dependenciesMet = false;
                    break;
                }
                // Copy dependency pointer
                dependencyMap[dep] = std::move(depIt->second);
            }
            
            if (dependenciesMet) {
                try {
                    components[type] = createComponent(type, config, rng, dependencyMap);
                } catch (const ComponentCreationException& e) {
                    // Continue with other components if this is not critical
                }
            } else {
                // Skip component if dependencies not met
            }
        }
        
        return components;
    }
    
    // Register built-in component types
    void registerBuiltInComponents() {
        // Core system components (initialization order 0-99)
        registerComponentMetadata("SpikeSystem", "SpikeSystem", "Neural spike generation system", {}, true, true, 0);
        registerComponentMetadata("DevelopmentSystem", "DevelopmentSystem", "Brain development and maturation", {}, true, true, 1);
        registerComponentMetadata("Neuromodulator", "Neuromodulator", "Base neuromodulator class", {}, true, true, 2);
        registerComponentMetadata("Dopamine", "Dopamine", "Reward and reinforcement signal", {"Neuromodulator"}, true, false, 3);
        registerComponentMetadata("Curiosity", "Curiosity", "Exploration motivation signal", {"Dopamine"}, true, false, 4);
        registerComponentMetadata("PredictionError", "PredictionError", "Prediction error signal", {}, true, false, 5);
        registerComponentMetadata("Novelty", "Novelty", "Novelty detection signal", {}, true, false, 6);
        
        // Memory systems
        registerComponentMetadata("WorkingMemory", "WorkingMemory", "Transient active information storage", {}, true, true, 10);
        registerComponentMetadata("EpisodicMemory", "EpisodicMemory", "Experience storage and replay", {}, true, true, 11);
        registerComponentMetadata("AssociativeMemory", "AssociativeMemory", "Pattern association memory", {}, true, true, 12);
        
        // Plasticity systems (depend on neuromodulators)
        registerComponentMetadata("STDP", "STDP", "Spike-timing dependent plasticity", {"Dopamine"}, true, true, 20);
        registerComponentMetadata("Hebbian", "Hebbian", "Hebbian learning rule", {"Dopamine"}, true, true, 21);
        registerComponentMetadata("StructuralPlasticity", "StructuralPlasticity", "Synaptic structure changes", {"STDP"}, true, true, 22);
        
        // Cognitive and prediction systems
        registerComponentMetadata("PredictionSystem", "PredictionSystem", "Sensory prediction and error", {}, true, true, 30);
        registerComponentMetadata("NeuralPlanner", "NeuralPlanner", "Action planning system", {}, true, true, 40);
        registerComponentMetadata("ConceptFormation", "ConceptFormation", "Pattern concept formation", {}, true, true, 50);
        registerComponentMetadata("AttentionalSelection", "AttentionalSelection", "Attention selection mechanism", {}, true, true, 60);
        
        // Development system (already registered)
        registerComponentMetadata("DevelopmentSystem", "DevelopmentSystem", "Brain development and maturation", {}, true, true, 70);
        
        // Infrastructure
        registerComponentMetadata("CheckpointManager", "CheckpointManager", "State checkpointing system", {}, true, true, 90);
    }
    
    // Register component metadata
    void registerComponentMetadata(const std::string& name, const std::string& typeName,
                                   const std::string& description, const std::set<std::string>& dependencies = {},
                                   bool requiresConfig = false, bool isCore = false, int initializationOrder = 0) {
        componentMetadata[name] = ComponentMetadata(name, typeName, description, dependencies);
        componentMetadata[name].requiresConfig = requiresConfig;
        componentMetadata[name].isCore = isCore;
        componentMetadata[name].initializationOrder = initializationOrder;
    }
    
    // Get all supported component types
    std::vector<std::string> getSupportedComponentTypes() const override {
        std::vector<std::string> types;
        for (const auto& pair : componentMetadata) {
            if (supportsComponent(pair.first)) {
                types.push_back(pair.first);
            }
        }
        return types;
    }
    
    // Get initialization order for component type
    int getInitializationOrder(const std::string& componentType) const override {
        auto it = componentMetadata.find(componentType);
        if (it != componentMetadata.end()) {
            return it->second.initializationOrder;
        }
        return 999;
    }
    
    // Get all registered component metadata
    std::map<std::string, ComponentMetadata> getAllComponentMetadata() const {
        return componentMetadata;
    }
    
    // Check if factory has any plugins
    bool hasPlugins() const {
        return !pluginsList.empty();
    }
    
    // Get plugin count
    size_t getPluginCount() const {
        return pluginsList.size();
    }
    
    // Create factory configuration from Brain parameters
    static std::map<std::string, std::string> createDefaultConfig() {
        std::map<std::string, std::string> config;
        
        // Core parameters
        config["neuron_count"] = "1000";
        config["region_count"] = "1";
        config["connection_probability"] = "0.1";
        \n        // Plasticity parameters\n        config[\"stdp_ltp_weight\"] = \"0.01\";\n        config[\"stdp_ltd_weight\"] = \"0.012\";\n        config[\"stdp_tau\"] = \"20.0\";\n        config[\"synaptogenesis_rate\"] = \"0.001\";\n        config[\"pruning_rate\"] = \"0.0005\";\n        \n        // Memory parameters\n        config[\"working_memory_capacity\"] = \"100\";\n        config[\"episodic_memory_max_episodes\"] = \"1000\";\n        config[\"associative_memory_capacity\"] = \"500\";\n        \n        // Neuromodulation parameters\n        config[\"dopamine_baseline\"] = \"0.5\";\n        config[\"curiosity_rate\"] = \"0.1\";\n        config[\"novelty_threshold\"] = \"0.7\";\n        \n        // Development parameters\n        config[\"plasticity_modifier\"] = \"1.0\";\n        config[\"replay_interval\"] = \"100\";\n        config[\"consolidation_interval\"] = \"1000\";\n        \n        // Checkpoint parameters\n        config[\"checkpoint_dir\"] = \"./checkpoints\";\n        config[\"checkpoint_interval\"] = \"10000\";\n        \n        return config;\n    }\n    \n    // Get factory status information\n    std::string getStatus() const {\n        std::ostringstream oss;\n        oss << \"ComponentFactory Status:\" << std::endl;\n        oss << \"  Registered component types: \" << componentMetadata.size() << std::endl;\n        oss << \"  Active plugins: \" << pluginsList.size() << std::endl;\n        oss << \"  Default creators: \" << creators.size() << std::endl;\n        oss << \"  Has configuration: \" << (config_ ? \"Yes\" : \"No\") << std::endl;\n        oss << \"  Supported components:\" << std::endl;\n        \n        for (const auto& type : getSupportedComponentTypes()) {\n            oss << \"    - \" << type << \" (order: \" << getInitializationOrder(type) << \")\" << std::endl;\n        }\n        \n        return oss.str();\n    }\n\nprivate:\n    std::shared_ptr<Config> config_;\n    std::map<std::string, ComponentMetadata> componentMetadata;\n    std::map<std::string, ComponentCreator> plugins;\n    std::vector<std::unique_ptr<IComponentPlugin>> pluginsList;\n    std::map<std::string, ComponentCreator> creators;\n};\n\n// Global factory registry for plugin management\nclass GlobalComponentFactory {\npublic:\n    static GlobalComponentFactory& getInstance() {\n        static GlobalComponentFactory instance;\n        return instance;\n    }\n    \n    // Set the default factory instance\n    void setFactory(std::unique_ptr<IComponentFactory> factory) {\n        defaultFactory_ = std::move(factory);\n    }\n    \n    // Get the default factory instance\n    IComponentFactory& getFactory() {\n        if (!defaultFactory_) {\n            defaultFactory_ = std::make_unique<ComponentFactory>();\n            static_cast<ComponentFactory&>(*defaultFactory_).registerBuiltInComponents();\n        }\n        return *defaultFactory_;\n    }\n    \n    // Register a component creator globally\n    template<typename ComponentType, typename CreatorFunc>\n    void registerComponentCreator(const std::string& componentType, CreatorFunc creator) {\n        getFactory().registerComponentCreator<ComponentType>(componentType, creator);\n    }\n    \n    // Register a plugin globally\n    void registerPlugin(std::unique_ptr<IComponentPlugin> plugin) {\n        getFactory().registerPlugin(std::move(plugin));\n    }\n    \n    // Initialize with default configuration\n    void initializeWithDefaults() {\n        auto& factory = getFactory();\n        auto config = std::make_shared<Config>();\n        \n        // Add default configuration values\n        auto defaultConfig = ComponentFactory::createDefaultConfig();\n        for (const auto& pair : defaultConfig) {\n            config->set(pair.first, pair.second, ConfigSource::Default);\n        }\n        \n        factory.initialize(config);\n    }\n    \n    // Cleanup\n    void cleanup() {\n        if (defaultFactory_) {\n            defaultFactory_->cleanup();\n        }\n        defaultFactory_.reset();\n    }\n    \nprivate:\n    GlobalComponentFactory() = default;\n    ~GlobalComponentFactory() = default;\n    \n    std::unique_ptr<IComponentFactory> defaultFactory_;\n};\n\n// Convenience functions for common usage with Brain integration\ninline std::unique_ptr<SpikeSystem> createSpikeSystem(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<SpikeSystem>(\"SpikeSystem\", config, nullptr);\n}\n\ninline std::unique_ptr<STDP> createSTDP(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<STDP>(\"STDP\", config, nullptr);\n}\n\ninline std::unique_ptr<Hebbian> createHebbian(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<Hebbian>(\"Hebbian\", config, nullptr);\n}\n\ninline std::unique_ptr<StructuralPlasticity> createStructuralPlasticity(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<StructuralPlasticity>(\"StructuralPlasticity\", config, nullptr);\n}\n\ninline std::unique_ptr<NeuralWorkingMemory> createWorkingMemory(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<NeuralWorkingMemory>(\"WorkingMemory\", config, nullptr);\n}\n\ninline std::unique_ptr<NeuralEpisodicMemory> createEpisodicMemory(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<NeuralEpisodicMemory>(\"EpisodicMemory\", config, nullptr);\n}\n\ninline std::unique_ptr<NeuralAssociativeMemory> createAssociativeMemory(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<NeuralAssociativeMemory>(\"AssociativeMemory\", config, nullptr);\n}\n\ninline std::unique_ptr<PredictionSystem> createPredictionSystem(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<PredictionSystem>(\"PredictionSystem\", config, nullptr);\n}\n\ninline std::unique_ptr<NeuralPlanner> createNeuralPlanner(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<NeuralPlanner>(\"NeuralPlanner\", config, nullptr);\n}\n\ninline std::unique_ptr<ConceptFormation> createConceptFormation(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<ConceptFormation>(\"ConceptFormation\", config, nullptr);\n}\n\ninline std::unique_ptr<AttentionalSelection> createAttentionalSelection(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<AttentionalSelection>(\"AttentionalSelection\", config, nullptr);\n}\n\ninline std::unique_ptr<DevelopmentSystem> createDevelopmentSystem(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<DevelopmentSystem>(\"DevelopmentSystem\", config, nullptr);\n}\n\ninline std::unique_ptr<Dopamine> createDopamine(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<Dopamine>(\"Dopamine\", config, nullptr);\n}\n\ninline std::unique_ptr<Curiosity> createCuriosity(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<Curiosity>(\"Curiosity\", config, nullptr);\n}\n\ninline std::unique_ptr<PredictionError> createPredictionError(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<PredictionError>(\"PredictionError\", config, nullptr);\n}\n\ninline std::unique_ptr<Novelty> createNovelty(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<Novelty>(\"Novelty\", config, nullptr);\n}\n\ninline std::unique_ptr<CheckpointManager> createCheckpointManager(const std::shared_ptr<Config>& config) {\n    auto& factory = GlobalComponentFactory::getInstance().getFactory();\n    return factory.getComponent<CheckpointManager>(\"CheckpointManager\", config, nullptr);\n}\n\n// Brain integration helper for factory-based initialization\ninline bool initializeBrainComponentsWithFactory(Brain::Impl& impl, const std::shared_ptr<Config>& config) {\n    try {\n        auto& factory = GlobalComponentFactory::getInstance().getFactory();\n        \n        // Initialize factory with config\n        if (!factory.initialize(config)) {\n            NLM_LOG_ERROR(\"Failed to initialize component factory\");\n            return false;\n        }\n        \n        // Configure STDP with parameters from config\n        auto stdpComponent = factory.getComponent<STDP>(\"STDP\", config, nullptr);\n        float ltpWeight = config->getOr<float>(\"stdp_ltp_weight\", nlm::DEFAULT_STDP_LTP_WEIGHT);\n        float ltdWeight = config->getOr<float>(\"stdp_ltd_weight\", nlm::DEFAULT_STDP_LTD_WEIGHT);\n        float tau = config->getOr<float>(\"stdp_tau\", nlm::DEFAULT_STDP_TAU);\n        stdpComponent->configure(ltpWeight, ltdWeight, tau);\n        \n        // Create the actual components using the configured ones\n        impl.spikeSystem = std::make_unique<SpikeSystem>();\n        impl.spikeSystem->configure(config->getOr<float>(\"simulation_timestep\", nlm::SIMULATION_TIMESTEP_SECONDS));\n        \n        impl.stdp = std::move(stdpComponent);\n        impl.hebbian = std::make_unique<Hebbian>();\n        impl.structuralPlasticity = std::make_unique<StructuralPlasticity>();\n        \n        // Configure structural plasticity\n        float synaptogenesisRate = config->getOr<float>(\"synaptogenesis_rate\", nlm::DEFAULT_SYNAPTOGENESIS_RATE);\n        float pruningRate = config->getOr<float>(\"pruning_rate\", nlm::DEFAULT_PRUNING_RATE);\n        impl.structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);\n        impl.structuralPlasticity->setPruningRate(pruningRate);\n        \n        // Create memory systems\n        impl.workingMemory = std::make_unique<NeuralWorkingMemory>();\n        impl.workingMemory->setCapacity(config->getOr<size_t>(\"working_memory_capacity\", 100));\n        \n        impl.episodicMemory = std::make_unique<NeuralEpisodicMemory>();\n        impl.episodicMemory->setMaxEpisodes(config->getOr<size_t>(\"episodic_memory_max_episodes\", 1000));\n        \n        impl.associativeMemory = std::make_unique<NeuralAssociativeMemory>();\n        \n        // Create neuromodulation systems\n        impl.dopamine = std::make_unique<Dopamine>();\n        impl.curiosity = std::make_unique<Curiosity>();\n        impl.predictionError = std::make_unique<PredictionError>();\n        impl.novelty = std::make_unique<Novelty>();\n        \n        // Create other systems\n        impl.predictionSystem = std::make_unique<PredictionSystem>();\n        impl.planner = std::make_unique<NeuralPlanner>();\n        impl.conceptFormation = std::make_unique<ConceptFormation>();\n        impl.attention = std::make_unique<AttentionalSelection>();\n        impl.developmentSystem = std::make_unique<DevelopmentSystem>();\n        impl.checkpointManager = std::make_unique<CheckpointManager>();\n        \n        // Configure checkpoint manager\n        std::string checkpointDir = config->getOr<std::string>(\"checkpoint_dir\", \"./checkpoints\");\n        impl.checkpointManager->configure(checkpointDir, 10000, 5, true);\n        \n        NLM_LOG_INFO(\"Brain components initialized successfully using ComponentFactory\");\n        return true;\n        \n    } catch (const ComponentCreationException& e) {\n        NLM_LOG_ERROR(\"Component creation failed in Brain initialization: \" + std::string(e.what()));\n        return false;\n    } catch (const ComponentRegistrationException& e) {\n        NLM_LOG_ERROR(\"Component registration failed in Brain initialization: \" + std::string(e.what()));\n        return false;\n    } catch (const std::exception& e) {\n        NLM_LOG_ERROR(\"Unexpected error during Brain component initialization: \" + std::string(e.what()));\n        return false;\n    }\n}\n\n} // namespace nlm