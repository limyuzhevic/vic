// nlm/BrainFactory.cpp - Implementation of BrainFactory class
#include "BrainFactory.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>

namespace nlm {

std::shared_ptr<Brain> BrainFactory::createStandardBrain(const BrainFactoryConfig& config) {
    NLM_LOG_INFO("Creating standard NLM brain with " + std::to_string(config.neuronCount) + " neurons");
    
    auto brain = std::make_shared<Brain>(createConfigFromFactoryConfig(config));
    applyConfigToBrain(brain, config);
    
    NLM_LOG_INFO("Standard brain created successfully");
    return brain;
}

std::shared_ptr<Brain> BrainFactory::createMinimalBrain() {
    NLM_LOG_INFO("Creating minimal NLM brain for testing");
    
    BrainFactoryConfig config;
    config.neuronCount = constants::MIN_NEURON_COUNT;
    config.regionCount = 1;
    config.enableSTDP = false;
    config.enableHebbian = false;
    config.enableStructuralPlasticity = false;
    config.enableWorkingMemory = false;
    config.enableDevelopment = false;
    config.maxSteps = 10;
    
    return createStandardBrain(config);
}

std::shared_ptr<Brain> BrainFactory::createResearchBrain() {
    NLM_LOG_INFO("Creating research-grade NLM brain");
    
    BrainFactoryConfig config;
    config.neuronCount = 5000;
    config.regionCount = 8;
    config.connectionProbability = 0.08f;
    config.enableSTDP = true;
    config.enableHebbian = true;
    config.enableStructuralPlasticity = true;
    config.enableWorkingMemory = true;
    config.enableEpisodicMemory = true;
    config.enablePredictionSystem = true;
    config.enableDevelopment = true;
    config.enableOptimization = true;
    config.enableSIMD = true;
    config.enableParallel = true;
    config.maxSteps = 5000;
    
    return createStandardBrain(config);
}

std::shared_ptr<Brain> BrainFactory::createDemoBrain(const std::string& demoType) {
    NLM_LOG_INFO("Creating demo brain of type: " + demoType);
    
    BrainFactoryConfig config;
    if (demoType == "performance") {
        config = Presets::performanceTest();
    } else if (demoType == "learning") {
        config = Presets::learningDemo();
    } else if (demoType == "integration") {
        config = Presets::integrationTest();
    } else {
        config = Presets::quickTest();
    }
    
    return createStandardBrain(config);
}

std::shared_ptr<Brain> BrainFactory::createBrainWithDefaults() {
    NLM_LOG_INFO("Creating brain with comprehensive defaults");
    return createStandardBrain();
}

std::shared_ptr<Brain> BrainFactory::createBrainFromConfigFile(const std::string& configPath) {
    NLM_LOG_INFO("Creating brain from config file: " + configPath);
    
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(configPath)) {
        NLM_LOG_ERROR("Failed to load config file: " + configPath);
        return nullptr;
    }
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain from config file");
        return nullptr;
    }
    
    NLM_LOG_INFO("Brain created successfully from config file: " + configPath);
    return brain;
}

std::shared_ptr<Brain> BrainFactory::createBrainWithProfile(const std::string& profileName) {
    NLM_LOG_INFO("Creating brain with profile: " + profileName);
    
    BrainFactoryConfig config = getConfigFromProfile(profileName);
    return createStandardBrain(config);
}

void BrainFactory::applyConfigToBrain(std::shared_ptr<Brain> brain, const BrainFactoryConfig& config) {
    if (!brain || !validateConfig(config)) {
        NLM_LOG_ERROR("Invalid configuration for brain application");
        return;
    }
    
    // Apply brain configuration
    auto brainConfig = brain->getConfig();
    
    // Basic settings
    brainConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount));
    brainConfig->set("region_count", static_cast<int64_t>(config.regionCount));
    brainConfig->set("connection_probability", config.connectionProbability);
    brainConfig->set("random_seed", static_cast<int64_t>(config.seed));
    brainConfig->set("simulation_timestep", config.timestep);
    
    // Plasticity settings
    brainConfig->set("stdp_enable", config.enableSTDP);
    brainConfig->set("hebbian_enable", config.enableHebbian);
    brainConfig->set("structural_plasticity_enable", config.enableStructuralPlasticity);
    brainConfig->set("stdp_ltp_weight", config.stdpLTPWeight);
    brainConfig->set("stdp_ltd_weight", config.stdpLTDWeight);
    brainConfig->set("stdp_tau", config.stdpTau);
    brainConfig->set("hebbian_weight", config.hebbianWeight);
    brainConfig->set("synaptogenesis_rate", config.synaptogenesisRate);
    brainConfig->set("pruning_rate", config.pruningRate);
    
    // Neuromodulation settings
    brainConfig->set("dopamine_enable", config.enableDopamine);
    brainConfig->set("curiosity_enable", config.enableCuriosity);
    brainConfig->set("novelty_enable", config.enableNovelty);
    brainConfig->set("prediction_error_enable", config.enablePredictionError);
    
    // Memory settings
    brainConfig->set("working_memory_enable", config.enableWorkingMemory);
    brainConfig->set("working_memory_size", static_cast<int64_t>(config.workingMemorySize));
    brainConfig->set("episodic_memory_enable", config.enableEpisodicMemory);
    brainConfig->set("associative_memory_enable", config.enableAssociativeMemory);
    
    // Cognitive settings
    brainConfig->set("prediction_system_enable", config.enablePredictionSystem);
    brainConfig->set("neural_planner_enable", config.enablePlanning);
    brainConfig->set("concept_formation_enable", config.enableConceptFormation);
    brainConfig->set("attention_enable", config.enableAttention);
    
    // Development settings
    brainConfig->set("development_enable", config.enableDevelopment);
    brainConfig->set("development_start_time", config.developmentStartTime);
    brainConfig->set("critical_period_end", config.criticalPeriodEnd);
    
    // Performance settings
    brainConfig->set("optimization_enable", config.enableOptimization);
    brainConfig->set("simd_enable", config.enableSIMD);
    brainConfig->set("parallel_enable", config.enableParallel);
    
    // Custom settings
    for (const auto& custom : config.customSettings) {
        brainConfig->set(custom.first, custom.second);
    }
    
    // Reinitialize brain with new config
    brain->reset();
    brain->initialize();
    
    NLM_LOG_INFO("Configuration applied to brain successfully");
}

BrainFactoryConfig BrainFactory::getDefaultConfigForDemo(const std::string& demoType) {
    if (demoType == "performance") {
        return Presets::performanceTest();
    } else if (demoType == "learning") {
        return Presets::learningDemo();
    } else if (demoType == "integration") {
        return Presets::integrationTest();
    } else {
        return Presets::quickTest();
    }
}

BrainFactoryConfig BrainFactory::getConfigFromProfile(const std::string& profileName) {
    // Built-in profiles
    if (profileName == "research") {
        return Presets::performanceTest();
    } else if (profileName == "demo") {
        return Presets::learningDemo();
    } else if (profileName == "integration") {
        return Presets::integrationTest();
    } else if (profileName == "quick") {
        return Presets::quickTest();
    } else if (profileName == "production") {
        BrainFactoryConfig config;
        config.neuronCount = 2000;
        config.regionCount = 4;
        config.connectionProbability = 0.08f;
        config.enableOptimization = true;
        config.enableSIMD = true;
        config.maxSteps = 10000;
        return config;
    } else {
        NLM_LOG_WARNING("Unknown profile name: " + profileName + ", using defaults");
        return BrainFactoryConfig{};
    }
}

bool BrainFactory::validateConfig(const BrainFactoryConfig& config) {
    if (!isConfigValid(config)) {
        NLM_LOG_ERROR("Invalid brain factory configuration");
        return false;
    }
    
    // Validate ranges
    if (config.neuronCount < constants::MIN_NEURON_COUNT || 
        config.neuronCount > constants::MAX_NEURON_COUNT) {
        NLM_LOG_ERROR("Invalid neuron count: " + std::to_string(config.neuronCount));
        return false;
    }
    
    if (config.regionCount < 1 || config.regionCount > 20) {
        NLM_LOG_ERROR("Invalid region count: " + std::to_string(config.regionCount));
        return false;
    }
    
    if (config.connectionProbability < constants::MIN_CONNECTION_PROBABILITY || 
        config.connectionProbability > constants::MAX_CONNECTION_PROBABILITY) {
        NLM_LOG_ERROR("Invalid connection probability: " + std::to_string(config.connectionProbability));
        return false;
    }
    
    if (config.maxSteps == 0) {
        NLM_LOG_ERROR("Invalid max steps: must be > 0");
        return false;
    }
    
    if (config.workingMemorySize < constants::MIN_WORKING_MEMORY_SIZE || 
        config.workingMemorySize > constants::MAX_WORKING_MEMORY_SIZE) {
        NLM_LOG_ERROR("Invalid working memory size: " + std::to_string(config.workingMemorySize));
        return false;
    }
    
    if (config.timestep <= 0.0) {
        NLM_LOG_ERROR("Invalid timestep: must be > 0");
        return false;
    }
    
    return true;
}

void BrainFactory::printConfigInfo(const BrainFactoryConfig& config) {
    NLM_LOG_INFO("=== Brain Factory Configuration ===");
    NLM_LOG_INFO("Neurons: " + std::to_string(config.neuronCount));
    NLM_LOG_INFO("Regions: " + std::to_string(config.regionCount));
    NLM_LOG_INFO("Connection probability: " + std::to_string(config.connectionProbability));
    NLM_LOG_INFO("STDP enabled: " + std::string(config.enableSTDP ? "yes" : "no"));
    NLM_LOG_INFO("Hebbian enabled: " + std::string(config.enableHebbian ? "yes" : "no"));
    NLM_LOG_INFO("Structural plasticity: " + std::string(config.enableStructuralPlasticity ? "yes" : "no"));
    NLM_LOG_INFO("Working memory: " + std::string(config.enableWorkingMemory ? "yes" : "no"));
    NLM_LOG_INFO("Development enabled: " + std::string(config.enableDevelopment ? "yes" : "no"));
    NLM_LOG_INFO("Max steps: " + std::to_string(config.maxSteps));
    NLM_LOG_INFO("Timestep: " + std::to_string(config.timestep));
    NLM_LOG_INFO("Seed: " + std::to_string(config.seed));
    NLM_LOG_INFO("=====================================");
}

std::vector<std::string> BrainFactory::getAvailableProfiles() {
    return {"quick", "demo", "integration", "performance", "research", "production"};
}

bool BrainFactory::saveProfile(const std::string& profileName, const BrainFactoryConfig& config) {
    if (!validateConfig(config)) {
        NLM_LOG_ERROR("Cannot save invalid profile: " + profileName);
        return false;
    }
    
    // TODO: Implement profile saving to file
    NLM_LOG_INFO("Profile saving to file not yet implemented (profile: " + profileName + ")");
    
    return true;
}

bool BrainFactory::loadProfile(const std::string& profileName, BrainFactoryConfig& config) {
    // Try to load from built-in profiles first
    config = getConfigFromProfile(profileName);
    
    // TODO: Implement loading from file profiles
    if (profileName.find('/') != std::string::npos || 
        profileName.find('\\') != std::string::npos) {
        // Likely a file path
        NLM_LOG_INFO("Loading profile from file not yet implemented (file: " + profileName + ")");
        return false;
    }
    
    return true;
}

std::shared_ptr<Config> BrainFactory::createConfigFromFactoryConfig(const BrainFactoryConfig& config) {
    auto brainConfig = std::make_shared<Config>();
    applyDefaultSettings(brainConfig, config);
    return brainConfig;
}

void BrainFactory::applyDefaultSettings(std::shared_ptr<Config> config, const BrainFactoryConfig& factoryConfig) {
    // Apply default configuration
    config->set("random_seed", static_cast<int64_t>(factoryConfig.seed), ConfigSource::Default);
    config->set("simulation_timestep", factoryConfig.timestep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(factoryConfig.neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(factoryConfig.regionCount), ConfigSource::Default);
    config->set("connection_probability", factoryConfig.connectionProbability, ConfigSource::Default);
    
    // Enable/disable systems
    config->set("stdp_enable", factoryConfig.enableSTDP, ConfigSource::Default);
    config->set("hebbian_enable", factoryConfig.enableHebbian, ConfigSource::Default);
    config->set("structural_plasticity_enable", factoryConfig.enableStructuralPlasticity, ConfigSource::Default);
    
    config->set("dopamine_enable", factoryConfig.enableDopamine, ConfigSource::Default);
    config->set("curiosity_enable", factoryConfig.enableCuriosity, ConfigSource::Default);
    config->set("novelty_enable", factoryConfig.enableNovelty, ConfigSource::Default);
    config->set("prediction_error_enable", factoryConfig.enablePredictionError, ConfigSource::Default);
    
    config->set("working_memory_enable", factoryConfig.enableWorkingMemory, ConfigSource::Default);
    config->set("episodic_memory_enable", factoryConfig.enableEpisodicMemory, ConfigSource::Default);
    config->set("associative_memory_enable", factoryConfig.enableAssociativeMemory, ConfigSource::Default);
    
    config->set("prediction_system_enable", factoryConfig.enablePredictionSystem, ConfigSource::Default);
    config->set("neural_planner_enable", factoryConfig.enablePlanning, ConfigSource::Default);
    config->set("concept_formation_enable", factoryConfig.enableConceptFormation, ConfigSource::Default);
    config->set("attention_enable", factoryConfig.enableAttention, ConfigSource::Default);
    
    config->set("development_enable", factoryConfig.enableDevelopment, ConfigSource::Default);
    
    config->set("optimization_enable", factoryConfig.enableOptimization, ConfigSource::Default);
    config->set("simd_enable", factoryConfig.enableSIMD, ConfigSource::Default);
    config->set("parallel_enable", factoryConfig.enableParallel, ConfigSource::Default);
    
    // Plasticity weights
    config->set("stdp_ltp_weight", factoryConfig.stdpLTPWeight, ConfigSource::Default);
    config->set("stdp_ltd_weight", factoryConfig.stdpLTDWeight, ConfigSource::Default);
    config->set("stdp_tau", factoryConfig.stdpTau, ConfigSource::Default);
    config->set("hebbian_weight", factoryConfig.hebbianWeight, ConfigSource::Default);
    config->set("synaptogenesis_rate", factoryConfig.synaptogenesisRate, ConfigSource::Default);
    config->set("pruning_rate", factoryConfig.pruningRate, ConfigSource::Default);
    
    // Memory sizes
    config->set("working_memory_size", static_cast<int64_t>(factoryConfig.workingMemorySize), ConfigSource::Default);
    
    // Development parameters
    config->set("development_start_time", factoryConfig.developmentStartTime, ConfigSource::Default);
    config->set("critical_period_end", factoryConfig.criticalPeriodEnd, ConfigSource::Default);
}

bool BrainFactory::isConfigValid(const BrainFactoryConfig& config) {
    if (config.neuronCount <= 0) return false;
    if (config.regionCount <= 0) return false;
    if (config.connectionProbability < 0.0f || config.connectionProbability > 1.0f) return false;
    if (config.maxSteps <= 0) return false;
    if (config.workingMemorySize < 0) return false;
    if (config.timestep <= 0.0) return false;
    if (config.seed < 0) return false;
    
    return true;
}

void BrainFactory::logFactoryConfig(const BrainFactoryConfig& config) {
    printConfigInfo(config);
}

} // namespace nlm
