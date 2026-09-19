// ComponentFactory.cpp
// Implementation of the ComponentFactory class for brain component creation

#include "ComponentFactory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <sstream>

namespace nlm {

// Component factory exception implementations
// (Constructor implementations are in the header since they're simple)

// ComponentFactory implementation

// Constructor and destructor implementations
ComponentFactory::ComponentFactory() = default;

ComponentFactory::~ComponentFactory() {
    cleanup();
}

// Factory initialization
bool ComponentFactory::initialize(const std::shared_ptr<Config>& config) {
    config_ = config;
    
    // Initialize all plugins
    for (auto& plugin : pluginsList) {
        try {
            if (!plugin->isCompatible(config)) {
                continue;
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Plugin compatibility check failed: " + std::string(e.what()));
        }
    }
    
    return true;
}

// Cleanup factory resources
void ComponentFactory::cleanup() {
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
void ComponentFactory::registerComponentCreator(const std::string& componentType, CreatorFunc creator) {
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
void ComponentFactory::registerPlugin(std::unique_ptr<IComponentPlugin> plugin) {
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
std::unique_ptr<ComponentType> ComponentFactory::getComponent(const std::string& componentType,
                                                           const std::shared_ptr<Config>& config,
                                                           const std::shared_ptr<RandomGenerator>& rng,
                                                           const std::map<std::string, std::unique_ptr<void>>& dependencies) {
    auto component = createComponent(componentType, config, rng, dependencies);
    return std::unique_ptr<ComponentType>(static_cast<ComponentType*>(component.release()));
}

// Create all standard components at once in dependency order
std::map<std::string, std::unique_ptr<void>> ComponentFactory::createAllComponents(
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
void ComponentFactory::registerBuiltInComponents() {
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
void ComponentFactory::registerComponentMetadata(const std::string& name, const std::string& typeName,
                               const std::string& description, const std::set<std::string>& dependencies,
                               bool requiresConfig, bool isCore, int initializationOrder) {
    componentMetadata[name] = ComponentMetadata(name, typeName, description, dependencies);
    componentMetadata[name].requiresConfig = requiresConfig;
    componentMetadata[name].isCore = isCore;
    componentMetadata[name].initializationOrder = initializationOrder;
}

// Get all supported component types
std::vector<std::string> ComponentFactory::getSupportedComponentTypes() const {
    std::vector<std::string> types;
    for (const auto& pair : componentMetadata) {
        if (supportsComponent(pair.first)) {
            types.push_back(pair.first);
        }
    }
    return types;
}

// Get initialization order for component type
int ComponentFactory::getInitializationOrder(const std::string& componentType) const {
    auto it = componentMetadata.find(componentType);
    if (it != componentMetadata.end()) {
        return it->second.initializationOrder;
    }
    return 999;
}

// Get all registered component metadata
std::map<std::string, ComponentMetadata> ComponentFactory::getAllComponentMetadata() const {
    return componentMetadata;
}

// Check if factory has any plugins
bool ComponentFactory::hasPlugins() const {
    return !pluginsList.empty();
}

// Get plugin count
size_t ComponentFactory::getPluginCount() const {
    return pluginsList.size();
}

// Create factory configuration from Brain parameters
std::map<std::string, std::string> ComponentFactory::createDefaultConfig() {
    std::map<std::string, std::string> config;
    
    // Core parameters
    config["neuron_count"] = "1000";
    config["region_count"] = "1";
    config["connection_probability"] = "0.1";
    \n    // Plasticity parameters\n    config[\"stdp_ltp_weight\"] = \"0.01\";\n    config[\"stdp_ltd_weight\"] = \"0.012\";\n    config[\"stdp_tau\"] = \"20.0\";\n    config[\"synaptogenesis_rate\"] = \"0.001\";\n    config[\"pruning_rate\"] = \"0.0005\";\n    \n    // Memory parameters\n    config[\"working_memory_capacity\"] = \"100\";\n    config[\"episodic_memory_max_episodes\"] = \"1000\";\n    config[\"associative_memory_capacity\"] = \"500\";\n    \n    // Neuromodulation parameters\n    config[\"dopamine_baseline\"] = \"0.5\";\n    config[\"curiosity_rate\"] = \"0.1\";\n    config[\"novelty_threshold\"] = \"0.7\";\n    \n    // Development parameters\n    config[\"plasticity_modifier\"] = \"1.0\";\n    config[\"replay_interval\"] = \"100\";\n    config[\"consolidation_interval\"] = \"1000\";\n    \n    // Checkpoint parameters\n    config[\"checkpoint_dir\"] = \"./checkpoints\";\n    config[\"checkpoint_interval\"] = \"10000\";\n    \n    return config;
}

// Get factory status information
std::string ComponentFactory::getStatus() const {
    std::ostringstream oss;
    oss << "ComponentFactory Status:" << std::endl;
    oss << "  Registered component types: " << componentMetadata.size() << std::endl;
    oss << "  Active plugins: " << pluginsList.size() << std::endl;
    oss << "  Default creators: " << creators.size() << std::endl;
    oss << "  Has configuration: " << (config_ ? "Yes" : "No") << std::endl;
    oss << "  Supported components:" << std::endl;
    \n    for (const auto& type : getSupportedComponentTypes()) {
        oss << "    - " << type << " (order: " << getInitializationOrder(type) << ")" << std::endl;
    }
    \n    return oss.str();
}

// GlobalComponentFactory implementation

GlobalComponentFactory& GlobalComponentFactory::getInstance() {
    static GlobalComponentFactory instance;
    return instance;
}

void GlobalComponentFactory::setFactory(std::unique_ptr<IComponentFactory> factory) {
    defaultFactory_ = std::move(factory);
}

IComponentFactory& GlobalComponentFactory::getFactory() {
    if (!defaultFactory_) {
        defaultFactory_ = std::make_unique<ComponentFactory>();
        static_cast<ComponentFactory&>(*defaultFactory_).registerBuiltInComponents();
    }
    return *defaultFactory_;
}

template<typename ComponentType, typename CreatorFunc>
void GlobalComponentFactory::registerComponentCreator(const std::string& componentType, CreatorFunc creator) {
    getFactory().registerComponentCreator<ComponentType>(componentType, creator);
}

void GlobalComponentFactory::registerPlugin(std::unique_ptr<IComponentPlugin> plugin) {
    getFactory().registerPlugin(std::move(plugin));
}

void GlobalComponentFactory::initializeWithDefaults() {
    auto& factory = getFactory();
    auto config = std::make_shared<Config>();
    \n    // Add default configuration values
    auto defaultConfig = ComponentFactory::createDefaultConfig();
    for (const auto& pair : defaultConfig) {
        config->set(pair.first, pair.second, ConfigSource::Default);
    }
    \n    factory.initialize(config);
}

void GlobalComponentFactory::cleanup() {
    if (defaultFactory_) {
        defaultFactory_->cleanup();
    }
    defaultFactory_.reset();
}

// Convenience functions for common usage with Brain integration
// (These are implemented in the header file)

// Brain integration helper for factory-based initialization
bool initializeBrainComponentsWithFactory(Brain::Impl& impl, const std::shared_ptr<Config>& config) {
    try {
        auto& factory = GlobalComponentFactory::getInstance().getFactory();
        \n        // Initialize factory with config
        if (!factory.initialize(config)) {
            NLM_LOG_ERROR("Failed to initialize component factory");
            return false;
        }
        \n        // Configure STDP with parameters from config
        auto stdpComponent = factory.getComponent<STDP>("STDP", config, nullptr);
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", nlm::DEFAULT_STDP_LTP_WEIGHT);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", nlm::DEFAULT_STDP_LTD_WEIGHT);
        float tau = config->getOr<float>("stdp_tau", nlm::DEFAULT_STDP_TAU);
        stdpComponent->configure(ltpWeight, ltdWeight, tau);
        \n        // Create the actual components using the configured ones
        impl.spikeSystem = std::make_unique<SpikeSystem>();
        impl.spikeSystem->configure(config->getOr<float>("simulation_timestep", nlm::SIMULATION_TIMESTEP_SECONDS));
        \n        impl.stdp = std::move(stdpComponent);
        impl.hebbian = std::make_unique<Hebbian>();
        impl.structuralPlasticity = std::make_unique<StructuralPlasticity>();
        \n        // Configure structural plasticity
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", nlm::DEFAULT_SYNAPTOGENESIS_RATE);
        float pruningRate = config->getOr<float>("pruning_rate", nlm::DEFAULT_PRUNING_RATE);
        impl.structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        impl.structuralPlasticity->setPruningRate(pruningRate);
        \n        // Create memory systems
        impl.workingMemory = std::make_unique<NeuralWorkingMemory>();
        impl.workingMemory->setCapacity(config->getOr<size_t>("working_memory_capacity", 100));
        \n        impl.episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        impl.episodicMemory->setMaxEpisodes(config->getOr<size_t>("episodic_memory_max_episodes", 1000));
        \n        impl.associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        \n        // Create neuromodulation systems
        impl.dopamine = std::make_unique<Dopamine>();
        impl.curiosity = std::make_unique<Curiosity>();
        impl.predictionError = std::make_unique<PredictionError>();
        impl.novelty = std::make_unique<Novelty>();
        \n        // Create other systems
        impl.predictionSystem = std::make_unique<PredictionSystem>();
        impl.planner = std::make_unique<NeuralPlanner>();
        impl.conceptFormation = std::make_unique<ConceptFormation>();
        impl.attention = std::make_unique<AttentionalSelection>();
        impl.developmentSystem = std::make_unique<DevelopmentSystem>();
        impl.checkpointManager = std::make_unique<CheckpointManager>();
        \n        // Configure checkpoint manager
        std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        impl.checkpointManager->configure(checkpointDir, 10000, 5, true);
        \n        NLM_LOG_INFO("Brain components initialized successfully using ComponentFactory");
        return true;
        \n    } catch (const ComponentCreationException& e) {
        NLM_LOG_ERROR("Component creation failed in Brain initialization: " + std::string(e.what()));
        return false;
    } catch (const ComponentRegistrationException& e) {
        NLM_LOG_ERROR("Component registration failed in Brain initialization: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error during Brain component initialization: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm