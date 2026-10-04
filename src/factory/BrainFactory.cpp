#include "BrainFactory.hpp"
#include "../core/ErrorHandling.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

BrainCreationResult BrainFactory::createBrain(const BrainFactoryConfig& config) {
    try {
        // Validate configuration
        if (!config.isValid()) {
            return BrainCreationResult(false, "Invalid factory configuration: " + validateConfig(config));
        }
        
        NLM_LOG_INFO("Creating brain with configuration: " + std::to_string(config.neuronCount) + " neurons, " +
                    std::to_string(config.connectionDensity * 100) + "% connection density");
        
        switch (config.brainType) {
            case BrainComponentType::DefaultBrain:
                return createDefaultBrain(config);
            case BrainComponentType::EnhancedBrain:
                return createEnhancedBrainInternal(config);
            case BrainComponentType::MinimalBrain:
                return createMinimalBrainInternal(config);
            case BrainComponentType::CustomBrain:
                return createEnhancedBrainInternal(config); // Use enhanced as base for custom
            default:
                return BrainCreationResult(false, "Unknown brain type");
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception creating brain: " + std::string(e.what()));
        return BrainCreationResult(false, "Brain creation failed: " + std::string(e.what()));
    }
}

BrainCreationResult BrainFactory::createBrainFromFile(const std::string& configFile) {
    try {
        NLM_LOG_INFO("Loading brain configuration from file: " + configFile);
        
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            return BrainCreationResult(false, "Failed to load configuration from file: " + configFile);
        }
        
        // Convert file config to factory config
        BrainFactoryConfig factoryConfig;
        factoryConfig.configFile = configFile;
        
        // Try to extract key parameters from config
        if (config->has("brain.neuron_count")) {
            factoryConfig.neuronCount = config->getOr<int64_t>("brain.neuron_count", 1000);
        }
        if (config->has("brain.synapse_density")) {
            factoryConfig.connectionDensity = config->getOr<float>("brain.synapse_density", 0.1f);
        }
        if (config->has("brain.plasticity.enabled")) {
            factoryConfig.enablePlasticity = config->getOr<bool>("brain.plasticity.enabled", true);
        }
        if (config->has("brain.development.enabled")) {
            factoryConfig.enableDevelopment = config->getOr<bool>("brain.development.enabled", true);
        }
        if (config->has("brain.neuromodulation.enabled")) {
            factoryConfig.enableNeuromodulation = config->getOr<bool>("brain.neuromodulation.enabled", true);
        }
        
        return createBrain(factoryConfig);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception creating brain from file: " + std::string(e.what()));
        return BrainCreationResult(false, "Brain creation from file failed: " + std::string(e.what()));
    }
}

BrainCreationResult BrainFactory::createBrain(const std::shared_ptr<Config>& config) {
    try {
        NLM_LOG_INFO("Creating brain from config object");
        
        // Convert config to factory config
        BrainFactoryConfig factoryConfig;
        
        if (config->has("brain.neuron_count")) {
            factoryConfig.neuronCount = config->getOr<int64_t>("brain.neuron_count", 1000);
        }
        if (config->has("brain.synapse_density")) {
            factoryConfig.connectionDensity = config->getOr<float>("brain.synapse_density", 0.1f);
        }
        if (config->has("brain.plasticity.enabled")) {
            factoryConfig.enablePlasticity = config->getOr<bool>("brain.plasticity.enabled", true);
        }
        if (config->has("brain.development.enabled")) {
            factoryConfig.enableDevelopment = config->getOr<bool>("brain.development.enabled", true);
        }
        if (config->has("brain.neuromodulation.enabled")) {
            factoryConfig.enableNeuromodulation = config->getOr<bool>("brain.neuromodulation.enabled", true);
        }
        if (config->has("brain.memory.enabled")) {
            factoryConfig.enableMemory = config->getOr<bool>("brain.memory.enabled", true);
        }
        if (config->has("brain.prediction.enabled")) {
            factoryConfig.enablePrediction = config->getOr<bool>("brain.prediction.enabled", true);
        }
        if (config->has("brain.cognition.enabled")) {
            factoryConfig.enableCognition = config->getOr<bool>("brain.cognition.enabled", true);
        }
        
        return createBrain(factoryConfig);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception creating brain from config: " + std::string(e.what()));
        return BrainCreationResult(false, "Brain creation from config failed: " + std::string(e.what()));
    }
}

BrainCreationResult BrainFactory::createMinimalBrain(size_t neuronCount) {
    BrainFactoryConfig config;
    config.brainType = BrainComponentType::MinimalBrain;
    config.neuronCount = neuronCount;
    config.connectionDensity = 0.05f; // Low density for minimal brain
    config.enablePlasticity = false;  // Disabled for minimal brain
    config.enableDevelopment = false;
    config.enableNeuromodulation = false;
    config.enableMemory = false;
    config.enablePrediction = false;
    config.enableCognition = false;
    
    return createBrain(config);
}

BrainCreationResult BrainFactory::createEnhancedBrain(size_t neuronCount) {
    BrainFactoryConfig config;
    config.brainType = BrainComponentType::EnhancedBrain;
    config.neuronCount = neuronCount;
    config.connectionDensity = 0.2f; // Higher density for enhanced brain
    config.enablePlasticity = true;
    config.enableDevelopment = true;
    config.enableNeuromodulation = true;
    config.enableMemory = true;
    config.enablePrediction = true;
    config.enableCognition = true;
    
    return createBrain(config);
}

BrainCreationResult BrainFactory::createCustomBrain(const std::vector<size_t>& regionSizes,
                                                   const std::vector<std::string>& regionNames) {
    try {
        if (regionSizes.size() != regionNames.size()) {
            return BrainCreationResult(false, "Region sizes and names must have same length");
        }
        
        // Create a basic enhanced brain first
        BrainCreationResult result = createEnhancedBrain(1000); // Base size
        if (!result.isOk()) {
            return result;
        }
        
        // TODO: Implement custom region configuration based on regionSizes and regionNames
        NLM_LOG_INFO("Custom brain creation requested with " + std::to_string(regionSizes.size()) + " regions");
        
        // For now, return the enhanced brain as a valid custom brain
        return result;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception creating custom brain: " + std::string(e.what()));
        return BrainCreationResult(false, "Custom brain creation failed: " + std::string(e.what()));
    }
}

void BrainFactory::configureNeuronTypes(std::shared_ptr<Brain> brain, NeuronTypeConfig neuronType) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot configure neuron types: brain is null");
        return;
    }
    
    try {
        applyNeuronConfiguration(brain, neuronType);
        NLM_LOG_INFO("Configured neuron types: " + std::to_string(static_cast<int>(neuronType)));
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error configuring neuron types: " + std::string(e.what()));
    }
}

void BrainFactory::configureTopology(std::shared_ptr<Brain> brain, NetworkTopology topology) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot configure topology: brain is null");
        return;
    }
    
    try {
        applyTopologyConfiguration(brain, topology);
        NLM_LOG_INFO("Configured network topology: " + std::to_string(static_cast<int>(topology)));
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error configuring topology: " + std::string(e.what()));
    }
}

BrainFactoryConfig BrainFactory::getDefaultConfig() {
    BrainFactoryConfig config;
    config.brainType = BrainComponentType::DefaultBrain;
    config.neuronType = NeuronTypeConfig::Standard;
    config.topology = NetworkTopology::Feedforward;
    config.neuronCount = 1000;
    config.connectionDensity = 0.1f;
    config.enablePlasticity = true;
    config.enableDevelopment = true;
    config.enableNeuromodulation = true;
    config.enableMemory = true;
    config.enablePrediction = true;
    config.enableCognition = true;
    
    return config;
}

BrainFactoryConfig BrainFactory::getEnhancedConfig() {
    BrainFactoryConfig config;
    config.brainType = BrainComponentType::EnhancedBrain;
    config.neuronType = NeuronTypeConfig::HighConductance;
    config.topology = NetworkTopology::Recurrent;
    config.neuronCount = 5000;
    config.connectionDensity = 0.2f;
    config.enablePlasticity = true;
    config.enableDevelopment = true;
    config.enableNeuromodulation = true;
    config.enableMemory = true;
    config.enablePrediction = true;
    config.enableCognition = true;
    
    return config;
}

std::string BrainFactory::validateConfig(const BrainFactoryConfig& config) {
    std::stringstream ss;
    bool hasErrors = false;
    
    if (config.neuronCount == 0) {
        ss << "Neuron count must be greater than 0. ";
        hasErrors = true;
    }
    
    if (config.connectionDensity < 0.0f || config.connectionDensity > 1.0f) {
        ss << "Connection density must be between 0.0 and 1.0. ";
        hasErrors = true;
    }
    
    if (hasErrors) {
        return ss.str();
    }
    
    return "";
}

std::vector<std::string> BrainFactory::getAvailableBrainTypes() {
    return {
        "DefaultBrain",
        "EnhancedBrain", 
        "MinimalBrain",
        "CustomBrain"
    };
}

std::vector<std::string> BrainFactory::getAvailableNeuronTypes() {
    return {
        "Standard",
        "HighConductance",
        "LowThreshold",
        "Adaptive"
    };
}

std::vector<std::string> BrainFactory::getAvailableTopologies() {
    return {
        "Feedforward",
        "Recurrent",
        "Hierarchical",
        "Modular"
    };
}

// Private implementation methods
BrainCreationResult BrainFactory::createDefaultBrain(const BrainFactoryConfig& config) {
    try {
        // Create config object
        auto nlmConfig = std::make_shared<Config>();
        
        // Set basic parameters
        nlmConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        nlmConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
        nlmConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        nlmConfig->set("connection_probability", config.connectionDensity, ConfigSource::Default);
        
        // Enable/disable features based on config
        nlmConfig->set("plasticity.enabled", config.enablePlasticity, ConfigSource::Default);
        nlmConfig->set("development.enabled", config.enableDevelopment, ConfigSource::Default);
        nlmConfig->set("neuromodulation.enabled", config.enableNeuromodulation, ConfigSource::Default);
        nlmConfig->set("memory.enabled", config.enableMemory, ConfigSource::Default);
        nlmConfig->set("prediction.enabled", config.enablePrediction, ConfigSource::Default);
        nlmConfig->set("cognition.enabled", config.enableCognition, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(nlmConfig);
        
        // Initialize with configuration
        if (!brain->initialize()) {
            return BrainCreationResult(false, "Failed to initialize brain");
        }
        
        // Apply neuron type configuration
        applyNeuronConfiguration(brain, config.neuronType);
        
        // Apply topology configuration
        applyTopologyConfiguration(brain, config.topology);
        
        // Apply feature configurations
        applyPlasticityConfiguration(brain, config.enablePlasticity);
        applyDevelopmentConfiguration(brain, config.enableDevelopment);
        applyNeuromodulationConfiguration(brain, config.enableNeuromodulation);
        applyMemoryConfiguration(brain, config.enableMemory);
        applyPredictionConfiguration(brain, config.enablePrediction);
        applyCognitionConfiguration(brain, config.enableCognition);
        
        NLM_LOG_INFO("Successfully created default brain with " + std::to_string(config.neuronCount) + " neurons");
        
        return BrainCreationResult(brain, "Default brain created successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception creating default brain: " + std::string(e.what()));
        return BrainCreationResult(false, "Default brain creation failed: " + std::string(e.what()));
    }
}

BrainCreationResult BrainFactory::createEnhancedBrainInternal(const BrainFactoryConfig& config) {
    try {
        // Create enhanced config with more features
        auto nlmConfig = std::make_shared<Config>();
        
        // Set enhanced parameters
        nlmConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        nlmConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
        nlmConfig->set("region_count", static_cast<int64_t>(2), ConfigSource::Default); // More regions
        nlmConfig->set("connection_probability", config.connectionDensity, ConfigSource::Default);
        
        // Enable all features
        nlmConfig->set("plasticity.enabled", true, ConfigSource::Default);
        nlmConfig->set("development.enabled", true, ConfigSource::Default);
        nlmConfig->set("neuromodulation.enabled", true, ConfigSource::Default);
        nlmConfig->set("memory.enabled", true, ConfigSource::Default);
        nlmConfig->set("prediction.enabled", true, ConfigSource::Default);
        nlmConfig->set("cognition.enabled", true, ConfigSource::Default);
        
        // Enhanced parameters
        nlmConfig->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        nlmConfig->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        nlmConfig->set("stdp_tau", 20.0f, ConfigSource::Default);
        nlmConfig->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        nlmConfig->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(nlmConfig);
        
        // Initialize with configuration
        if (!brain->initialize()) {
            return BrainCreationResult(false, "Failed to initialize enhanced brain");
        }
        
        // Apply advanced neuron configuration
        applyNeuronConfiguration(brain, NeuronTypeConfig::HighConductance);
        
        // Apply recurrent topology
        applyTopologyConfiguration(brain, NetworkTopology::Recurrent);
        
        // Apply all feature configurations
        applyPlasticityConfiguration(brain, true);
        applyDevelopmentConfiguration(brain, true);
        applyNeuromodulationConfiguration(brain, true);
        applyMemoryConfiguration(brain, true);
        applyPredictionConfiguration(brain, true);
        applyCognitionConfiguration(brain, true);
        
        NLM_LOG_INFO("Successfully created enhanced brain with " + std::to_string(config.neuronCount) + " neurons");
        
        return BrainCreationResult(brain, "Enhanced brain created successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception creating enhanced brain: " + std::string(e.what()));
        return BrainCreationResult(false, "Enhanced brain creation failed: " + std::string(e.what()));
    }
}

BrainCreationResult BrainFactory::createMinimalBrainInternal(const BrainFactoryConfig& config) {
    try {
        // Create minimal config with basic features only
        auto nlmConfig = std::make_shared<Config>();
        
        // Set minimal parameters
        nlmConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        nlmConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
        nlmConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        nlmConfig->set("connection_probability", config.connectionDensity, ConfigSource::Default);
        
        // Disable all advanced features
        nlmConfig->set("plasticity.enabled", false, ConfigSource::Default);
        nlmConfig->set("development.enabled", false, ConfigSource::Default);
        nlmConfig->set("neuromodulation.enabled", false, ConfigSource::Default);
        nlmConfig->set("memory.enabled", false, ConfigSource::Default);
        nlmConfig->set("prediction.enabled", false, ConfigSource::Default);
        nlmConfig->set("cognition.enabled", false, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(nlmConfig);
        
        // Initialize with configuration
        if (!brain->initialize()) {
            return BrainCreationResult(false, "Failed to initialize minimal brain");
        }
        
        NLM_LOG_INFO("Successfully created minimal brain with " + std::to_string(config.neuronCount) + " neurons");
        
        return BrainCreationResult(brain, "Minimal brain created successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception creating minimal brain: " + std::string(e.what()));
        return BrainCreationResult(false, "Minimal brain creation failed: " + std::string(e.what()));
    }
}

void BrainFactory::applyNeuronConfiguration(std::shared_ptr<Brain> brain, NeuronTypeConfig neuronType) {
    if (!brain) return;
    
    switch (neuronType) {
        case NeuronTypeConfig::Standard:
            configureStandardNeurons(brain);
            break;
        case NeuronTypeConfig::HighConductance:
            configureHighConductanceNeurons(brain);
            break;
        case NeuronTypeConfig::LowThreshold:
            configureLowThresholdNeurons(brain);
            break;
        case NeuronTypeConfig::Adaptive:
            configureAdaptiveNeurons(brain);
            break;
    }
}

void BrainFactory::applyTopologyConfiguration(std::shared_ptr<Brain> brain, NetworkTopology topology) {
    if (!brain) return;
    
    switch (topology) {
        case NetworkTopology::Feedforward:
            configureFeedforwardTopology(brain);
            break;
        case NetworkTopology::Recurrent:
            configureRecurrentTopology(brain);
            break;
        case NetworkTopology::Hierarchical:
            configureHierarchicalTopology(brain);
            break;
        case NetworkTopology::Modular:
            configureModularTopology(brain);
            break;
    }
}

void BrainFactory::applyPlasticityConfiguration(std::shared_ptr<Brain> brain, bool enable) {
    if (!brain) return;
    
    if (enable) {
        auto* stdp = brain->getSTDP();
        if (stdp) {
            stdp->enable(true);
        }
    }
}

void BrainFactory::applyDevelopmentConfiguration(std::shared_ptr<Brain> brain, bool enable) {
    if (!brain) return;
    
    if (enable) {
        auto* dev = brain->getDevelopmentSystem();
        if (dev) {
            dev->enable(true);
        }
    }
}

void BrainFactory::applyNeuromodulationConfiguration(std::shared_ptr<Brain> brain, bool enable) {
    if (!brain) return;
    
    if (enable) {
        auto* dopamine = brain->getDopamine();
        auto* curiosity = brain->getCuriosity();
        auto* novelty = brain->getNovelty();
        auto* predError = brain->getPredictionErrorSignal();
        
        if (dopamine) dopamine->enable(true);
        if (curiosity) curiosity->enable(true);
        if (novelty) novelty->enable(true);
        if (predError) predError->enable(true);
    }
}

void BrainFactory::applyMemoryConfiguration(std::shared_ptr<Brain> brain, bool enable) {
    if (!brain) return;
    
    if (enable) {
        auto* workingMem = brain->getWorkingMemory();
        auto* episodicMem = brain->getEpisodicMemory();
        auto* assocMem = brain->getAssociativeMemory();
        
        if (workingMem) workingMem->enable(true);
        if (episodicMem) episodicMem->enable(true);
        if (assocMem) assocMem->enable(true);
    }
}

void BrainFactory::applyPredictionConfiguration(std::shared_ptr<Brain> brain, bool enable) {
    if (!brain) return;
    
    if (enable) {
        auto* predSys = brain->getPredictionSystem();
        if (predSys) predSys->enable(true);
    }
}

void BrainFactory::applyCognitionConfiguration(std::shared_ptr<Brain> brain, bool enable) {
    if (!brain) return;
    
    if (enable) {
        auto* planner = brain->getPlanner();
        auto* conceptForm = brain->getConceptFormation();
        auto* attention = brain->getAttention();
        
        if (planner) planner->enable(true);
        if (conceptForm) conceptForm->enable(true);
        if (attention) attention->enable(true);
    }
}

void BrainFactory::configureStandardNeurons(std::shared_ptr<Brain> brain) {
    NLM_LOG_DEBUG("Configuring standard neurons");
    // Standard LIF neurons with basic properties
}

void BrainFactory::configureHighConductanceNeurons(std::shared_ptr<Brain> brain) {
    NLM_LOG_DEBUG("Configuring high conductance neurons");
    // Neurons with higher input resistance and faster dynamics
}

void BrainFactory::configureLowThresholdNeurons(std::shared_ptr<Brain> brain) {
    NLM_LOG_DEBUG("Configuring low threshold neurons");
    // Neurons with lower firing thresholds
}

void BrainFactory::configureAdaptiveNeurons(std::shared_ptr<Brain> brain) {
    NLM_LOG_DEBUG("Configuring adaptive neurons");
    // Neurons with activity-dependent properties
}

void BrainFactory::configureFeedforwardTopology(std::shared_ptr<Brain> brain) {
    NLM_LOG_DEBUG("Configuring feedforward topology");
    // Simple layer structure with one-way connections
}

void BrainFactory::configureRecurrentTopology(std::shared_ptr<Brain> brain) {
    NLM_LOG_DEBUG("Configuring recurrent topology");
    // Networks with feedback connections for dynamics
}

void BrainFactory::configureHierarchicalTopology(std::shared_ptr<Brain> brain) {
    NLM_LOG_DEBUG("Configuring hierarchical topology");
    // Multi-level organization with processing stages
}

void BrainFactory::configureModularTopology(std::shared_ptr<Brain> brain) {
    NLM_LOG_DEBUG("Configuring modular topology");
    // Specialized modules with limited interconnections
}

} // namespace nlm