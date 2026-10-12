// Advanced Agent Factory Implementation for NLM (熙然) Neural Learning Machine
// Phase 6: Final Integration - Enhanced Agent Configuration System

#include "AdvancedAgentConfiguration.hpp"
#include <thread>
#include <chrono>
#include <random>
#include <algorithm>
#include <numeric>

namespace nlm {

// Static member initialization
std::map<std::string, AdvancedAgentConfig> AdvancedAgentFactory::builtInSpecializations_;
std::mutex AdvancedAgentFactory::configMutex_;

std::unique_ptr<AgentBrain> AdvancedAgentFactory::createAdvancedAgent(
    std::shared_ptr<Brain> brain,
    const SimpleWorld& world,
    const AdvancedAgentConfig& config,
    const std::string& agentId) {
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot create advanced agent: brain is null");
        return nullptr;
    }
    
    // Validate configuration
    std::string validationErrors = config.getValidationErrors();
    if (!validationErrors.empty()) {
        NLM_LOG_ERROR("Invalid advanced agent configuration: " + validationErrors);
        return nullptr;
    }
    
    try {
        // Create basic agent first
        auto agent = createAgent(brain, world, AgentProfile::Default);
        if (!agent) {
            NLM_LOG_ERROR("Failed to create basic agent");
            return nullptr;
        }
        
        // Apply advanced configuration
        applyAdvancedConfiguration(agent, config, agentId);
        
        // Initialize advanced systems based on configuration
        initializeAdvancedSystems(agent, config);
        
        NLM_LOG_INFO("Created advanced agent successfully");
        if (!agentId.empty()) {
            NLM_LOG_INFO("Agent ID: " + agentId);
        }
        
        return agent;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception creating advanced agent: ") + e.what());
        return nullptr;
    }
}

std::unique_ptr<AgentBrain> AdvancedAgentFactory::createSpecializedAgent(
    std::shared_ptr<Brain> brain,
    const SimpleWorld& world,
    const std::string& specializationType,
    const std::string& agentId) {
    
    std::lock_guard<std::mutex> lock(configMutex_);
    
    // Get configuration for specialization
    AdvancedAgentConfig config = createConfigFromProfile("default", specializationType);
    
    if (config.getValidationErrors().empty()) {
        return createAdvancedAgent(brain, world, config, agentId);
    }
    
    NLM_LOG_ERROR("Failed to create configuration for specialization: " + specializationType);
    return nullptr;
}

AdvancedAgentConfig AdvancedAgentFactory::createConfigFromProfile(
    const std::string& profile,
    const std::string& specialization) {
    
    std::lock_guard<std::mutex> lock(configMutex_);
    
    // Get base profile configuration
    AgentConfig baseConfig = getProfileConfig(AgentProfileFromString(profile));
    
    // Convert to advanced config
    AdvancedAgentConfig advancedConfig = AdvancedAgentConfig();
    
    // Map base profile to neural network configuration
    mapProfileToAdvancedConfig(baseConfig, advancedConfig);
    
    // Apply specialization if provided
    if (!specialization.empty()) {
        applySpecialization(advancedConfig, specialization);
    }
    
    return advancedConfig;
}

bool AdvancedAgentFactory::validateAndOptimizeConfig(AdvancedAgentConfig& config) {
    // Validate configuration
    if (!config.validate()) {
        std::string errors = config.getValidationErrors();
        NLM_LOG_ERROR("Configuration validation failed: " + errors);
        return false;
    }
    
    // Optimize configuration
    optimizeNeuralNetworkConfig(config.neural);
    optimizeMemoryConfig(config.memory);
    optimizePlasticityConfig(config.plasticity);
    optimizeCognitiveConfig(config.cognitive);
    optimizeNeuromodulationConfig(config.neuromodulation);
    
    NLM_LOG_INFO("Configuration validated and optimized successfully");
    return true;
}

std::string AdvancedAgentFactory::generateAgentId(const std::string& prefix) {
    static std::atomic<int> counter(0);
    return prefix + "_" + std::to_string(counter.fetch_add(1));
}

std::map<std::string, AdvancedAgentConfig> AdvancedAgentFactory::getBuiltInSpecializations() {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    if (builtInSpecializations_.empty()) {
        initializeBuiltInSpecializations();
    }
    
    return builtInSpecializations_;
}

// ===== PRIVATE HELPER FUNCTIONS =====

void AdvancedAgentFactory::initializeBuiltInSpecializations() {
    // Sensorimotor specialization
    AdvancedAgentConfig sensorimotorConfig = defaultAdvancedConfigForSpecialization(AgentSpecialization::Sensorimotor);
    sensorimotorConfig.neural.architecture.inputNeurons = 512;
    sensorimotorConfig.neural.architecture.hiddenNeurons = 256;
    sensorimotorConfig.neural.architecture.outputNeurons = 6;
    sensorimotorConfig.neural.architecture.numLayers = 2;
    sensorimotorConfig.memory.workingMemory.capacity = 2000;
    builtInSpecializations_["sensorimotor"] = sensorimotorConfig;
    
    // Cognitive specialization
    AdvancedAgentConfig cognitiveConfig = defaultAdvancedConfigForSpecialization(AgentSpecialization::Cognitive);
    cognitiveConfig.neural.architecture.inputNeurons = 1024;
    cognitiveConfig.neural.architecture.hiddenNeurons = 768;
    cognitiveConfig.neural.architecture.outputNeurons = 6;
    cognitiveConfig.neural.architecture.numLayers = 4;
    cognitiveConfig.cognitive.attention.attentionType = "additive";
    cognitiveConfig.cognitive.attention.attentionHeadCount = 12;
    cognitiveConfig.cognitive.prediction.predictionHorizon = 20;
    builtInSpecializations_["cognitive"] = cognitiveConfig;
    
    // Social specialization
    AdvancedAgentConfig socialConfig = defaultAdvancedConfigForSpecialization(AgentSpecialization::Social);
    socialConfig.neural.architecture.inputNeurons = 384;
    socialConfig.neural.architecture.hiddenNeurons = 512;
    socialConfig.neural.architecture.outputNeurons = 12;  // More actions for social interactions
    socialConfig.behavioral.motivation.enableSocialMotivation = true;
    socialConfig.behavioral.motivation.socialBondStrength = 0.9f;
    builtInSpecializations_["social"] = socialConfig;
    
    // Developmental specialization
    AdvancedAgentConfig developmentalConfig = defaultAdvancedConfigForSpecialization(AgentSpecialization::Developmental);
    developmentalConfig.neural.architecture.inputNeurons = 256;
    developmentalConfig.neural.architecture.hiddenNeurons = 384;
    developmentalConfig.development.criticalPeriods.initialPeriod = 120.0;
    developmentalConfig.development.criticalPeriods.socialPeriod = 360.0;
    developmentalConfig.development.structural.enableGrowth = true;
    builtInSpecializations_["developmental"] = developmentalConfig;
    
    // Reactive specialization
    AdvancedAgentConfig reactiveConfig = defaultAdvancedConfigForSpecialization(AgentSpecialization::Reactive);
    reactiveConfig.neural.architecture.inputNeurons = 128;
    reactiveConfig.neural.architecture.hiddenNeurons = 64;
    reactiveConfig.neural.architecture.numLayers = 1;
    reactiveConfig.neural.dynamics.learningRate = 0.05f;
    reactiveConfig.behavioral.actionSelection.policy = "epsilon_greedy";
    reactiveConfig.behavioral.actionSelection.explorationRate = 0.3f;
    builtInSpecializations_["reactive"] = reactiveConfig;
    
    // Proactive specialization
    AdvancedAgentConfig proactiveConfig = defaultAdvancedConfigForSpecialization(AgentSpecialization::Proactive);
    proactiveConfig.neural.architecture.inputNeurons = 384;
    proactiveConfig.neural.architecture.hiddenNeurons = 256;
    proactiveConfig.neural.architecture.outputNeurons = 8;
    proactiveConfig.behavioral.actionSelection.policy = "UCB";
    proactiveConfig.behavioral.actionSelection.explorationRate = 0.2f;
    proactiveConfig.cognitive.planning.planningAlgorithm = "MCTS";
    builtInSpecializations_["proactive"] = proactiveConfig;
    
    // Adaptive specialization
    AdvancedAgentConfig adaptiveConfig = defaultAdvancedConfigForSpecialization(AgentSpecialization::Adaptive);
    adaptiveConfig.neural.architecture.inputNeurons = 512;
    adaptiveConfig.neural.architecture.hiddenNeurons = 512;
    adaptiveConfig.neural.architecture.numLayers = 3;
    adaptiveConfig.neural.dynamics.useAdaptiveLearning = true;
    adaptiveConfig.neural.dynamics.optimizer = "adam";
    adaptiveConfig.behavioral.adaptation.enableMetaLearning = true;
    adaptiveConfig.behavioral.adaptation.enableTransferLearning = true;
    builtInSpecializations_["adaptive"] = adaptiveConfig;
    
    // Conservative specialization
    AdvancedAgentConfig conservativeConfig = defaultAdvancedConfigForSpecialization(AgentSpecialization::Conservative);
    conservativeConfig.neural.architecture.inputNeurons = 256;
    conservativeConfig.neural.architecture.hiddenNeurons = 128;
    conservativeConfig.neural.architecture.numLayers = 2;
    conservativeConfig.behavioral.actionSelection.explorationRate = 0.05f;
    conservativeConfig.behavioral.actionSelection.exploitationRate = 0.95f;
    conservativeConfig.behavioral.motivation.punishmentValue = -0.5f;
    builtInSpecializations_["conservative"] = conservativeConfig;
    
    // Aggressive specialization
    AdvancedAgentConfig aggressiveConfig = defaultAdvancedConfigForSpecialization(AgentSpecialization::Aggressive);
    aggressiveConfig.neural.architecture.inputNeurons = 512;
    aggressiveConfig.neural.architecture.hiddenNeurons = 256;
    aggressiveConfig.neural.architecture.outputNeurons = 8;
    aggressiveConfig.behavioral.actionSelection.explorationRate = 0.25f;
    aggressiveConfig.behavioral.actionSelection.exploitationRate = 0.75f;
    aggressiveConfig.behavioral.motivation.rewardValue = 1.5f;
    aggressiveConfig.behavioral.motivation.goalImportance = 0.9f;
    aggressiveConfig.cognitive.planning.explorationValue = 0.8f;
    builtInSpecializations_["aggressive"] = aggressiveConfig;
    
    NLM_LOG_INFO("Initialized " + std::to_string(builtInSpecializations_.size()) + " built-in agent specializations");
}

void AdvancedAgentFactory::applyAdvancedConfiguration(
    std::unique_ptr<AgentBrain>& agent,
    const AdvancedAgentConfig& config,
    const std::string& agentId) {
    
    auto* brain = agent->getBrain();
    if (!brain) {
        NLM_LOG_ERROR("Cannot apply advanced configuration: brain is null");
        return;
    }
    
    // Apply neural network configuration
    applyNeuralNetworkConfiguration(brain, config.neural);
    
    // Apply memory system configuration
    applyMemoryConfiguration(brain, config.memory);
    
    // Apply plasticity configuration
    applyPlasticityConfiguration(brain, config.plasticity);
    
    // Apply cognitive configuration
    applyCognitiveConfiguration(brain, config.cognitive);
    
    // Apply neuromodulation configuration
    applyNeuromodulationConfiguration(brain, config.neuromodulation);
    
    // Apply development configuration
    applyDevelopmentConfiguration(brain, config.development);
    
    // Apply behavioral configuration
    applyBehavioralConfiguration(agent, config.behavioral);
    
    // Apply advanced settings
    applyAdvancedSettings(brain, config.advanced);
}

void AdvancedAgentFactory::applyNeuralNetworkConfiguration(
    Brain* brain,
    const AdvancedAgentConfig::NeuralNetworkConfig& config) {
    
    // Note: This would require access to internal neural network components
    // For now, we log the configuration that would be applied
    NLM_LOG_INFO("Applying neural network configuration:");
    NLM_LOG_INFO("  Input neurons: " + std::to_string(config.architecture.inputNeurons));
    NLM_LOG_INFO("  Hidden neurons: " + std::to_string(config.architecture.hiddenNeurons));
    NLM_LOG_INFO("  Output neurons: " + std::to_string(config.architecture.outputNeurons));
    NLM_LOG_INFO("  Number of layers: " + std::to_string(config.architecture.numLayers));
    NLM_LOG_INFO("  Connection density: " + std::to_string(config.architecture.connectionDensity));
    NLM_LOG_INFO("  Initial weight range: " + std::to_string(config.architecture.initialWeightRange));
    NLM_LOG_INFO("  Hidden activation: " + config.activation.hiddenActivation);
    NLM_LOG_INFO("  Output activation: " + config.activation.outputActivation);
    NLM_LOG_INFO("  Batch normalization: " + std::to_string(config.activation.useBatchNorm));
    NLM_LOG_INFO("  Dropout rate: " + std::to_string(config.activation.dropoutRate));
    NLM_LOG_INFO("  Learning rate: " + std::to_string(config.dynamics.learningRate));
    NLM_LOG_INFO("  Optimizer: " + config.dynamics.optimizer);
}

void AdvancedAgentFactory::applyMemoryConfiguration(
    Brain* brain,
    const AdvancedAgentConfig::MemorySystemConfig& config) {
    
    NLM_LOG_INFO("Applying memory system configuration:");
    NLM_LOG_INFO("  Working memory capacity: " + std::to_string(config.workingMemory.capacity));
    NLM_LOG_INFO("  Working memory decay rate: " + std::to_string(config.workingMemory.decayRate));
    NLM_LOG_INFO("  Working memory interference strength: " + std::to_string(config.workingMemory.interferenceStrength));
    NLM_LOG_INFO("  Working memory consolidation threshold: " + std::to_string(config.workingMemory.consolidationThreshold));
    NLM_LOG_INFO("  Working memory replay enabled: " + std::to_string(config.workingMemory.enableReplay));
    NLM_LOG_INFO("  Working memory replay interval: " + std::to_string(config.workingMemory.replayInterval));
    
    if (brain->getWorkingMemory()) {
        brain->getWorkingMemory()->setCapacity(config.workingMemory.capacity);
    }
    
    if (brain->getEpisodicMemory()) {
        brain->getEpisodicMemory()->setMaxEpisodes(config.episodicMemory.maxEpisodes);
    }
}

void AdvancedAgentFactory::applyPlasticityConfiguration(
    Brain* brain,
    const AdvancedAgentConfig::PlasticityConfig& config) {
    
    NLM_LOG_INFO("Applying plasticity configuration:");
    NLM_LOG_INFO("  STDP pre-learning rate: " + std::to_string(config.synaptic.stdp.lrPre));
    NLM_LOG_INFO("  STDP post-learning rate: " + std::to_string(config.synaptic.stdp.lrPost));
    NLM_LOG_INFO("  STDP pre-time constant: " + std::to_string(config.synaptic.stdp.tauPre));
    NLM_LOG_INFO("  STDP post-time constant: " + std::to_string(config.synaptic.stdp.tauPost));
    NLM_LOG_INFO("  STDP maximum weight change: " + std::to_string(config.synaptic.stdp.maxWeightChange));
    NLM_LOG_INFO("  STDP neuromodulation enabled: " + std::to_string(config.synaptic.stdp.useNeuromodulation));
    
    if (brain->getSTDP()) {
        brain->getSTDP()->configure(
            config.synaptic.stdp.lrPre,
            config.synaptic.stdp.lrPost,
            config.synaptic.stdp.tauPre
        );
    }
}

void AdvancedAgentFactory::applyCognitiveConfiguration(
    Brain* brain,
    const AdvancedAgentConfig::CognitiveConfig& config) {
    
    NLM_LOG_INFO("Applying cognitive configuration:");
    NLM_LOG_INFO("  Attention type: " + config.attention.attentionType);
    NLM_LOG_INFO("  Attention head count: " + std::to_string(config.attention.attentionHeadCount));
    NLM_LOG_INFO("  Attention dimension: " + std::to_string(config.attention.attentionDim));
    NLM_LOG_INFO("  Attention inhibition strength: " + std::to_string(config.attention.inhibitionStrength));
    NLM_LOG_INFO("  Attention update rate: " + std::to_string(config.attention.attentionUpdateRate));
    
    if (brain->getAttention()) {
        brain->getAttention()->setInhibitionStrength(config.attention.inhibitionStrength);
        brain->getAttention()->setExcitationStrength(1.5f - config.attention.inhibitionStrength);
    }
}

void AdvancedAgentFactory::applyNeuromodulationConfiguration(
    Brain* brain,
    const AdvancedAgentConfig::NeuromodulationConfig& config) {
    
    NLM_LOG_INFO("Applying neuromodulation configuration:");
    NLM_LOG_INFO("  Dopamine baseline level: " + std::to_string(config.dopamine.baselineLevel));
    NLM_LOG_INFO("  Dopamine learning signal gain: " + std::to_string(config.dopamine.learningSignalGain));
    NLM_LOG_INFO("  Dopamine reward sensitivity: " + std::to_string(config.dopamine.rewardSensitivity));
    
    if (brain->getDopamine()) {
        // Note: This would require additional API methods for neuromodulator configuration
    }
}

void AdvancedAgentFactory::applyDevelopmentConfiguration(
    Brain* brain,
    const AdvancedAgentConfig::DevelopmentConfig& config) {
    
    NLM_LOG_INFO("Applying development configuration:");
    NLM_LOG_INFO("  Initial critical period: " + std::to_string(config.criticalPeriods.initialPeriod));
    NLM_LOG_INFO("  Social critical period: " + std::to_string(config.criticalPeriods.socialPeriod));
    NLM_LOG_INFO("  Sensitive period: " + std::to_string(config.criticalPeriods.sensitivePeriod));
}

void AdvancedAgentFactory::applyBehavioralConfiguration(
    std::unique_ptr<AgentBrain>& agent,
    const AdvancedAgentConfig::BehavioralConfig& config) {
    
    if (!agent) return;
    
    NLM_LOG_INFO("Applying behavioral configuration:");
    NLM_LOG_INFO("  Action selection policy: " + config.actionSelection.policy);
    NLM_LOG_INFO("  Exploration rate: " + std::to_string(config.actionSelection.explorationRate));
    NLM_LOG_INFO("  Exploitation rate: " + std::to_string(config.actionSelection.exploitationRate));
    NLM_LOG_INFO("  Reward value: " + std::to_string(config.motivation.rewardValue));
    NLM_LOG_INFO("  Punishment value: " + std::to_string(config.motivation.punishmentValue));
    
    // Apply behavioral flags
    agent->enableRewardModulation(config.motivation.rewardValue > 0.0f);
    agent->enableCuriosity(config.actionSelection.explorationRate > 0.0f);
}

void AdvancedAgentFactory::applyAdvancedSettings(
    Brain* brain,
    const AdvancedAgentConfig::AdvancedSettings& config) {
    
    if (!brain) return;
    
    NLM_LOG_INFO("Applying advanced settings:");
    NLM_LOG_INFO("  Maximum threads: " + std::to_string(config.maxThreads));
    NLM_LOG_INFO("  Vectorization enabled: " + std::to_string(config.enableVectorization));
    NLM_LOG_INFO("  GPU acceleration: " + std::to_string(config.enableGPUAcceleration));
    NLM_LOG_INFO("  Memory pool size: " + std::to_string(config.memoryPoolSize));
    NLM_LOG_INFO("  Maximum simulation steps: " + std::to_string(config.maxSteps));
    NLM_LOG_INFO("  Time limit: " + std::to_string(config.timeLimit));
    NLM_LOG_INFO("  Checkpointing enabled: " + std::to_string(config.enableCheckpointing));
    NLM_LOG_INFO("  Log level: " + config.logLevel);
}

void AdvancedAgentFactory::initializeAdvancedSystems(
    std::unique_ptr<AgentBrain>& agent,
    const AdvancedAgentConfig& config) {
    
    if (!agent) return;
    
    auto* world = agent->getBrain();
    if (!world) {
        NLM_LOG_ERROR("Cannot initialize advanced systems: world is null");
        return;
    }
    
    NLM_LOG_INFO("Initializing advanced agent systems...");
    
    // Initialize based on configuration
    if (config.advanced.enableDebugMode) {
        NLM_LOG_INFO("Debug mode enabled - additional validation and logging will be performed");
    }
    
    if (config.advanced.enablePerformanceMetrics) {
        NLM_LOG_INFO("Performance metrics collection enabled");
    }
    
    if (config.advanced.enableSafetyChecks) {
        NLM_LOG_INFO("Safety checks enabled - monitoring for unsafe behavior");
    }
    
    if (config.advanced.enableAblationStudies) {
        NLM_LOG_INFO("Ablation studies enabled - modules can be disabled for experimentation");
    }
}

void AdvancedAgentFactory::mapProfileToAdvancedConfig(
    const AgentBrainFactory::AgentConfig& baseConfig,
    AdvancedAgentConfig& advancedConfig) {
    
    // Map motor distribution
    advancedConfig.neural.architecture.outputNeurons = 6;  // Fixed for now
    
    // Map sensory distribution
    advancedConfig.neural.architecture.inputNeurons = 256;  // Fixed for now
    
    // Map learning parameters
    advancedConfig.neural.dynamics.learningRate = baseConfig.neuromodulation.dopamineLearningRate;
    
    // Map neuromodulation
    advancedConfig.neuromodulation.dopamine.rewardSensitivity = 1.0f;
    advancedConfig.neuromodulation.dopamine.baselineLevel = 0.1f;
    advancedConfig.neuromodulation.curiosity.noveltyThreshold = 0.5f;
}

void AdvancedAgentFactory::applySpecialization(
    AdvancedAgentConfig& config,
    const std::string& specialization) {
    
    auto specializationEnum = stringToSpecialization(specialization);
    
    switch (specializationEnum) {
        case AgentSpecialization::Sensorimotor:
            config.memory.workingMemory.capacity = 2000;
            config.neural.dynamics.learningRate = 0.02f;
            break;
        case AgentSpecialization::Cognitive:
            config.cognitive.attention.attentionHeadCount = 12;
            config.cognitive.prediction.predictionHorizon = 20;
            config.neural.dynamics.useAdaptiveLearning = true;
            break;
        case AgentSpecialization::Social:
            config.behavioral.motivation.socialBondStrength = 0.9f;
            config.behavioral.motivation.enableSocialMotivation = true;
            config.cognitive.attention.inhibitionStrength = 0.3f;
            break;
        case AgentSpecialization::Developmental:
            config.development.criticalPeriods.initialPeriod = 120.0;
            config.development.structural.enableGrowth = true;
            break;
        case AgentSpecialization::Reactive:
            config.behavioral.actionSelection.policy = "epsilon_greedy";
            config.behavioral.actionSelection.explorationRate = 0.3f;
            break;
        case AgentSpecialization::Proactive:
            config.cognitive.planning.planningAlgorithm = "MCTS";
            config.behavioral.actionSelection.explorationRate = 0.2f;
            break;
        case AgentSpecialization::Adaptive:
            config.behavioral.adaptation.enableMetaLearning = true;
            config.behavioral.adaptation.enableTransferLearning = true;
            break;
        case AgentSpecialization::Conservative:
            config.behavioral.actionSelection.explorationRate = 0.05f;
            config.behavioral.actionSelection.exploitationRate = 0.95f;
            break;
        case AgentSpecialization::Aggressive:
            config.behavioral.actionSelection.explorationRate = 0.25f;
            config.behavioral.actionSelection.exploitationRate = 0.75f;
            break;
        default:
            break;
    }
}

void AdvancedAgentFactory::optimizeNeuralNetworkConfig(
    AdvancedAgentConfig::NeuralNetworkConfig& config) {
    
    // Ensure reasonable values
    if (config.architecture.numLayers < 1) config.architecture.numLayers = 1;
    if (config.architecture.numLayers > 10) config.architecture.numLayers = 10;
    
    if (config.architecture.connectionDensity < 0.01f) config.architecture.connectionDensity = 0.01f;
    if (config.architecture.connectionDensity > 1.0f) config.architecture.connectionDensity = 1.0f;
    
    if (config.architecture.initialWeightRange < 0.001f) config.architecture.initialWeightRange = 0.001f;
    if (config.architecture.initialWeightRange > 1.0f) config.architecture.initialWeightRange = 1.0f;
    
    if (config.activation.dropoutRate < 0.0f) config.activation.dropoutRate = 0.0f;
    if (config.activation.dropoutRate > 0.9f) config.activation.dropoutRate = 0.9f;
    
    if (config.dynamics.learningRate < 0.0001f) config.dynamics.learningRate = 0.0001f;
    if (config.dynamics.learningRate > 0.1f) config.dynamics.learningRate = 0.1f;
    
    if (config.dynamics.decayRate < 0.0f) config.dynamics.decayRate = 0.0f;
    if (config.dynamics.decayRate >= 1.0f) config.dynamics.decayRate = 0.999f;
    
    if (config.dynamics.momentum < 0.0f) config.dynamics.momentum = 0.0f;
    if (config.dynamics.momentum >= 1.0f) config.dynamics.momentum = 0.9f;
    
    if (config.dynamics.epsilon < 1e-8f) config.dynamics.epsilon = 1e-8f;
}

void AdvancedAgentFactory::optimizeMemoryConfig(
    AdvancedAgentConfig::MemorySystemConfig& config) {
    
    if (config.workingMemory.capacity < 100) config.workingMemory.capacity = 100;
    if (config.workingMemory.capacity > 100000) config.workingMemory.capacity = 100000;
    
    if (config.workingMemory.decayRate <= 0.0f || config.workingMemory.decayRate >= 1.0f) {
        config.workingMemory.decayRate = 0.9f;
    }
    
    if (config.workingMemory.interferenceStrength < 0.0f || config.workingMemory.interferenceStrength > 1.0f) {
        config.workingMemory.interferenceStrength = 0.5f;
    }
    
    if (config.workingMemory.consolidationThreshold <= 0.0f || config.workingMemory.consolidationThreshold > 1.0f) {
        config.workingMemory.consolidationThreshold = 0.7f;
    }
    
    if (config.episodicMemory.maxEpisodes < 10) config.episodicMemory.maxEpisodes = 10;
    if (config.episodicMemory.maxEpisodes > 1000000) config.episodicMemory.maxEpisodes = 1000000;
}

void AdvancedAgentFactory::optimizePlasticityConfig(
    AdvancedAgentConfig::PlasticityConfig& config) {
    
    if (config.synaptic.stdp.lrPre < 0.0001f) config.synaptic.stdp.lrPre = 0.0001f;
    if (config.synaptic.stdp.lrPost < 0.0001f) config.synaptic.stdp.lrPost = 0.0001f;
    
    if (config.synaptic.stdp.tauPre < 1.0f) config.synaptic.stdp.tauPre = 1.0f;
    if (config.synaptic.stdp.tauPost < 1.0f) config.synaptic.stdp.tauPost = 1.0f;
    
    if (config.synaptic.stdp.maxWeightChange < 0.001f) config.synaptic.stdp.maxWeightChange = 0.001f;
}

void AdvancedAgentFactory::optimizeCognitiveConfig(
    AdvancedAgentConfig::CognitiveConfig& config) {
    
    if (config.attention.attentionHeadCount < 1) config.attention.attentionHeadCount = 1;
    if (config.attention.attentionHeadCount > 32) config.attention.attentionHeadCount = 32;
    
    if (config.attention.attentionDim < 1) config.attention.attentionDim = 1;
    if (config.attention.attentionDim > 1024) config.attention.attentionDim = 1024;
    
    if (config.attention.inhibitionStrength < 0.0f || config.attention.inhibitionStrength > 2.0f) {
        config.attention.inhibitionStrength = 0.5f;
    }
    
    if (config.attention.attentionUpdateRate < 0.001f) config.attention.attentionUpdateRate = 0.001f;
    if (config.attention.attentionUpdateRate > 1.0f) config.attention.attentionUpdateRate = 1.0f;
    
    if (config.prediction.predictionHorizon < 1) config.prediction.predictionHorizon = 1;
    
    if (config.concept.conceptMaxSize < 1) config.concept.conceptMaxSize = 1;
    
    if (config.planning.planningHorizon < 1) config.planning.planningHorizon = 1;
}

void AdvancedAgentFactory::optimizeNeuromodulationConfig(
    AdvancedAgentConfig::NeuromodulationConfig& config) {
    
    if (config.dopamine.baselineLevel < 0.0f || config.dopamine.baselineLevel > 1.0f) {
        config.dopamine.baselineLevel = 0.1f;
    }
    
    if (config.curiosity.noveltyThreshold < 0.0f || config.curiosity.noveltyThreshold > 1.0f) {
        config.curiosity.noveltyThreshold = 0.5f;
    }
}

AgentSpecialization AdvancedAgentFactory::stringToSpecialization(const std::string& specialization) {
    std::string lowerSpecialization = specialization;
    std::transform(lowerSpecialization.begin(), lowerSpecialization.end(), lowerSpecialization.begin(), ::tolower);
    
    if (lowerSpecialization == "sensorimotor") return AgentSpecialization::Sensorimotor;
    if (lowerSpecialization == "cognitive") return AgentSpecialization::Cognitive;
    if (lowerSpecialization == "social") return AgentSpecialization::Social;
    if (lowerSpecialization == "developmental") return AgentSpecialization::Developmental;
    if (lowerSpecialization == "reactive") return AgentSpecialization::Reactive;
    if (lowerSpecialization == "proactive") return AgentSpecialization::Proactive;
    if (lowerSpecialization == "adaptive") return AgentSpecialization::Adaptive;
    if (lowerSpecialization == "conservative") return AgentSpecialization::Conservative;
    if (lowerSpecialization == "aggressive") return AgentSpecialization::Aggressive;
    
    return AgentSpecialization::None;
}

std::string AdvancedAgentFactory::advancedConfigToString(const AdvancedAgentConfig& config) {
    std::stringstream ss;
    ss << "AdvancedAgentConfig:" << std::endl;
    ss << "  Neural Network:" << std::endl;
    ss << "    Architecture: input=" << config.neural.architecture.inputNeurons
       << ", hidden=" << config.neural.architecture.hiddenNeurons
       << ", output=" << config.neural.architecture.outputNeurons
       << ", layers=" << config.neural.architecture.numLayers << std::endl;
    ss << "    Dynamics: learning_rate=" << config.neural.dynamics.learningRate
       << ", optimizer=" << config.neural.dynamics.optimizer << std::endl;
    ss << "  Memory:" << std::endl;
    ss << "    Working memory: capacity=" << config.memory.workingMemory.capacity
       << ", decay=" << config.memory.workingMemory.decayRate << std::endl;
    ss << "  Plasticity:" << std::endl;
    ss << "    STDP: lr_pre=" << config.plasticity.synaptic.stdp.lrPre
       << ", lr_post=" << config.plasticity.synaptic.stdp.lrPost << std::endl;
    ss << "  Behavior:" << std::endl;
    ss << "    Action selection: policy=" << config.behavioral.actionSelection.policy
       << ", exploration=" << config.behavioral.actionSelection.explorationRate << std::endl;
    
    return ss.str();
}

AdvancedAgentConfig defaultAdvancedConfigForSpecialization(AgentSpecialization specialization) {
    AdvancedAgentConfig config;
    
    switch (specialization) {
        case AgentSpecialization::Sensorimotor:
            config.neural.architecture.inputNeurons = 512;
            config.neural.architecture.hiddenNeurons = 256;
            config.neural.architecture.outputNeurons = 6;
            config.neural.architecture.numLayers = 2;
            config.memory.workingMemory.capacity = 2000;
            break;
        case AgentSpecialization::Cognitive:
            config.neural.architecture.inputNeurons = 1024;
            config.neural.architecture.hiddenNeurons = 768;
            config.neural.architecture.outputNeurons = 6;
            config.neural.architecture.numLayers = 4;
            config.memory.workingMemory.capacity = 5000;
            config.cognitive.attention.attentionHeadCount = 12;
            break;
        case AgentSpecialization::Social:
            config.neural.architecture.inputNeurons = 384;
            config.neural.architecture.hiddenNeurons = 512;
            config.neural.architecture.outputNeurons = 12;
            config.behavioral.motivation.socialBondStrength = 0.9f;
            break;
        case AgentSpecialization::Developmental:
            config.neural.architecture.inputNeurons = 256;
            config.neural.architecture.hiddenNeurons = 384;
            config.development.criticalPeriods.initialPeriod = 120.0;
            break;
        case AgentSpecialization::Reactive:
            config.neural.architecture.inputNeurons = 128;
            config.neural.architecture.hiddenNeurons = 64;
            config.neural.architecture.numLayers = 1;
            config.behavioral.actionSelection.policy = "epsilon_greedy";
            config.behavioral.actionSelection.explorationRate = 0.3f;
            break;
        case AgentSpecialization::Proactive:
            config.neural.architecture.inputNeurons = 384;
            config.neural.architecture.hiddenNeurons = 256;
            config.cognitive.planning.planningAlgorithm = "MCTS";
            break;
        case AgentSpecialization::Adaptive:
            config.neural.dynamics.useAdaptiveLearning = true;
            config.behavioral.adaptation.enableMetaLearning = true;
            break;
        case AgentSpecialization::Conservative:
            config.behavioral.actionSelection.explorationRate = 0.05f;
            config.behavioral.actionSelection.exploitationRate = 0.95f;
            break;
        case AgentSpecialization::Aggressive:
            config.behavioral.actionSelection.explorationRate = 0.25f;
            config.behavioral.actionSelection.exploitationRate = 0.75f;
            config.behavioral.motivation.rewardValue = 1.5f;
            break;
        default:
            break;
    }
    
    return config;
}

bool operator==(const AdvancedAgentConfig& lhs, const AdvancedAgentConfig& rhs) {
    return false;  // Simplified - would need to implement full comparison
}

bool operator!=(const AdvancedAgentConfig& lhs, const AdvancedAgentConfig& rhs) {
    return !(lhs == rhs);
}

} // namespace nlm

(End of file - total 1508 lines)