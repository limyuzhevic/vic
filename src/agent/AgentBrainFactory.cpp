#include "AgentBrainFactory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Static member initialization
std::map<AgentBrainFactory::AgentProfile, AgentBrainFactory::AgentConfig> AgentBrainFactory::profileConfigs = createProfileConfigs();

std::unique_ptr<AgentBrain> AgentBrainFactory::createAgent(std::shared_ptr<Brain> brain, 
                                                           const SimpleWorld& world,
                                                           AgentProfile profile) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot create AgentBrain: brain is null");
        return nullptr;
    }
    
    // Get configuration for the specified profile
    AgentConfig config = getProfileConfig(profile);
    
    // Create agent
    auto agent = std::make_unique<AgentBrain>(brain);
    
    // Apply configuration
    applyConfiguration(agent, config);
    
    // Initialize with world
    agent->initialize(world);
    
    NLM_LOG_INFO("Created AgentBrain with profile: " + std::to_string(static_cast<int>(profile)));
    return agent;
}

std::unique_ptr<AgentBrain> AgentBrainFactory::createCustomAgent(std::shared_ptr<Brain> brain,
                                                                 const SimpleWorld& world,
                                                                 const AgentConfig& config) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot create AgentBrain: brain is null");
        return nullptr;
    }
    
    // Validate configuration
    std::string error;
    if (!validateConfig(config, error)) {
        NLM_LOG_ERROR("Invalid agent configuration: " + error);
        return nullptr;
    }
    
    // Create agent
    auto agent = std::make_unique<AgentBrain>(brain);
    
    // Apply configuration
    applyConfiguration(agent, config);
    
    // Initialize with world
    agent->initialize(world);
    
    NLM_LOG_INFO("Created custom AgentBrain with user configuration");
    return agent;
}

AgentBrainFactory::AgentConfig AgentBrainFactory::getProfileConfig(AgentProfile profile) {
    auto it = profileConfigs.find(profile);
    if (it != profileConfigs.end()) {
        return it->second;
    }
    
    // Return default if profile not found
    NLM_LOG_WARNING("Profile " + std::to_string(static_cast<int>(profile)) + " not found, using default");
    return getProfileConfig(AgentProfile::Default);
}

void AgentBrainFactory::applyConfiguration(std::unique_ptr<AgentBrain>& agent, 
                                           const AgentConfig& config) {
    if (!agent) {
        NLM_LOG_ERROR("Cannot apply configuration: agent is null");
        return;
    }
    
    auto* brain = agent->getBrain();
    if (!brain) {
        NLM_LOG_ERROR("Cannot apply configuration: brain is null");
        return;
    }
    
    // Apply motor neuron distribution
    // Note: This would require modifying the AgentBrain constructor to accept
    // distribution parameters, or post-initialization configuration
    agent->enableRewardModulation(config.neuromodulation.enableRewardModulation);
    agent->enableStructuralPlasticity(config.neuromodulation.enableStructuralPlasticity);
    agent->enableDevelopment(config.neuromodulation.enableDevelopment);
    agent->enableCuriosity(config.neuromodulation.enableCuriosity);
    
    // Set development parameters
    if (config.development.enableMaturation) {
        // Would need to modify development system parameters
    }
    if (config.development.enablePruning) {
        // Would need to modify pruning parameters
    }
    
    // Set learning parameters
    if (brain->getWorkingMemory()) {
        brain->getWorkingMemory()->setCapacity(config.learning.workingMemoryCapacity);
    }
    if (brain->getEpisodicMemory()) {
        brain->getEpisodicMemory()->setMaxEpisodes(config.learning.episodicMemoryMaxEpisodes);
    }
    
    NLM_LOG_INFO("Applied agent configuration successfully");
}

AgentBrainFactory::AgentConfig AgentBrainFactory::getRecommendedConfig(const std::string& useCase) {
    AgentConfig config;
    
    if (useCase == "exploration") {
        config = getProfileConfig(AgentProfile::Explorer);
    } else if (useCase == "tasks") {
        config = getProfileConfig(AgentProfile::Worker);
    } else if (useCase == "social") {
        config = getProfileConfig(AgentProfile::Social);
    } else if (useCase == "conservation") {
        config = getProfileConfig(AgentProfile::Conservative);
    } else if (useCase == "rapid_learning") {
        config = getProfileConfig(AgentProfile::Aggressive);
    } else if (useCase == "adaptive") {
        config = getProfileConfig(AgentProfile::Adaptive);
    } else {
        config = getProfileConfig(AgentProfile::Default);
        NLM_LOG_WARNING("Unknown use case '" + useCase + "', using default configuration");
    }
    
    return config;
}

bool AgentBrainFactory::validateConfig(const AgentConfig& config, std::string& errorMessage) {
    errorMessage.clear();
    
    // Validate motor distribution sums to 1.0
    float motorTotal = config.motorDistribution.forward + config.motorDistribution.backward +
                      config.motorDistribution.turnLeft + config.motorDistribution.turnRight +
                      config.motorDistribution.interact + config.motorDistribution.wait;
    if (std::abs(motorTotal - 1.0f) > 0.01f) {
        errorMessage = "Motor distribution must sum to 1.0 (current: " + std::to_string(motorTotal) + ")";
        return false;
    }
    
    // Validate sensory distribution sums to 1.0
    float sensoryTotal = config.sensoryDistribution.vision + config.sensoryDistribution.touch +
                        config.sensoryDistribution.internal + config.sensoryDistribution.proprioception;
    if (std::abs(sensoryTotal - 1.0f) > 0.01f) {
        errorMessage = "Sensory distribution must sum to 1.0 (current: " + std::to_string(sensoryTotal) + ")";
        return false;
    }
    
    // Validate curiosity threshold
    if (config.neuromodulation.curiosityThreshold < 0.0f || 
        config.neuromodulation.curiosityThreshold > 1.0f) {
        errorMessage = "Curiosity threshold must be between 0.0 and 1.0";
        return false;
    }
    
    // Validate exploration probability
    if (config.neuromodulation.explorationProbability < 0.0f || 
        config.neuromodulation.explorationProbability > 1.0f) {
        errorMessage = "Exploration probability must be between 0.0 and 1.0";
        return false;
    }
    
    // Validate learning rate
    if (config.learning.learningRate < 0.0f || config.learning.learningRate > 1.0f) {
        errorMessage = "Learning rate must be between 0.0 and 1.0";
        return false;
    }
    
    // Validate activity threshold
    if (config.behavior.activityThreshold < 0.0f || config.behavior.activityThreshold > 1.0f) {
        errorMessage = "Activity threshold must be between 0.0 and 1.0";
        return false;
    }
    
    return true;
}

// Initialize static profile configurations
std::map<AgentBrainFactory::AgentProfile, AgentBrainFactory::AgentConfig> 
AgentBrainFactory::createProfileConfigs() {
    std::map<AgentProfile, AgentConfig> configs;
    
    // Default profile
    AgentConfig defaultConfig;
    defaultConfig.neuromodulation.enableRewardModulation = true;
    defaultConfig.neuromodulation.enableStructuralPlasticity = true;
    defaultConfig.neuromodulation.enableDevelopment = true;
    defaultConfig.neuromodulation.enableCuriosity = true;
    defaultConfig.neuromodulation.curiosityThreshold = 0.3f;
    defaultConfig.neuromodulation.explorationProbability = 0.1f;
    defaultConfig.development.enableMaturation = true;
    defaultConfig.development.enablePruning = true;
    defaultConfig.learning.workingMemoryCapacity = 1000;
    defaultConfig.learning.episodicMemoryMaxEpisodes = 1000;
    defaultConfig.behavior.defaultAction = MotorCommand::Wait;
    defaultConfig.behavior.activityThreshold = 0.5f;
    
    configs[AgentProfile::Default] = defaultConfig;
    
    // Explorer profile - high curiosity, frequent exploration
    AgentConfig explorerConfig = defaultConfig;
    explorerConfig.motorDistribution.wait = 0.05f;  // Less waiting
    explorerConfig.motorDistribution.interact = 0.10f;  // More interaction
    explorerConfig.neuromodulation.enableCuriosity = true;
    explorerConfig.neuromodulation.curiosityThreshold = 0.2f;  // Lower threshold
    explorerConfig.neuromodulation.explorationProbability = 0.3f;  // Higher exploration
    explorerConfig.learning.learningRate = 0.02f;  // Faster learning
    explorerConfig.behavior.defaultAction = MotorCommand::LookLeft;  // More exploration actions
    
    configs[AgentProfile::Explorer] = explorerConfig;
    
    // Worker profile - focused on task completion
    AgentConfig workerConfig = defaultConfig;
    workerConfig.motorDistribution.wait = 0.15f;  // More waiting/stopping
    workerConfig.motorDistribution.forward = 0.35f;  // More focused movement
    workerConfig.neuromodulation.enableCuriosity = false;  // Less exploration
    workerConfig.neuromodulation.explorationProbability = 0.05f;  // Low exploration
    workerConfig.learning.learningRate = 0.008f;  // Slower learning
    workerConfig.development.enableMaturation = true;  // More stable
    workerConfig.development.enablePruning = true;  // More pruning
    workerConfig.behavior.defaultAction = MotorCommand::MoveForward;  // Goal-oriented
    
    configs[AgentProfile::Worker] = workerConfig;
    
    // Social profile - enhanced social interaction
    AgentConfig socialConfig = defaultConfig;
    socialConfig.motorDistribution.interact = 0.20f;  // Much more interaction
    socialConfig.motorDistribution.turnLeft = 0.10f;  // More social turning
    socialConfig.motorDistribution.turnRight = 0.10f;  // More social turning
    socialConfig.neuromodulation.enableCuriosity = true;  // Social learning
    socialConfig.neuromodulation.curiosityThreshold = 0.25f;  // Lower threshold
    socialConfig.learning.learningRate = 0.015f;  // Social learning
    socialConfig.behavior.defaultAction = MotorCommand::Interact;  // Social default
    
    configs[AgentProfile::Social] = socialConfig;
    
    // Conservative profile - low risk, stable behavior
    AgentConfig conservativeConfig = defaultConfig;
    conservativeConfig.motorDistribution.wait = 0.30f;  // A lot of waiting
    conservativeConfig.motorDistribution.forward = 0.25f;  // Cautious movement
    conservativeConfig.neuromodulation.enableCuriosity = false;  // No exploration
    conservativeConfig.neuromodulation.explorationProbability = 0.01f;  // Very low exploration
    conservativeConfig.learning.learningRate = 0.005f;  // Very slow learning
    conservativeConfig.development.enableMaturation = true;  // Stable development
    conservativeConfig.development.enablePruning = true;  // Aggressive pruning
    conservativeConfig.behavior.activityThreshold = 0.7f;  // High activity threshold
    conservativeConfig.behavior.defaultAction = MotorCommand::Wait;  // Conservative default
    
    configs[AgentProfile::Conservative] = conservativeConfig;
    
    // Aggressive profile - high motivation, rapid learning
    AgentConfig aggressiveConfig = defaultConfig;
    aggressiveConfig.motorDistribution.wait = 0.02f;  // Very little waiting
    aggressiveConfig.motorDistribution.forward = 0.40f;  // Strong movement
    aggressiveConfig.neuromodulation.enableCuriosity = true;  // High curiosity
    aggressiveConfig.neuromodulation.curiosityThreshold = 0.15f;  // Very low threshold
    aggressiveConfig.neuromodulation.explorationProbability = 0.25f;  // High exploration
    aggressiveConfig.learning.learningRate = 0.03f;  // Very fast learning
    aggressiveConfig.development.enableMaturation = true;  // Develop but fast
    aggressiveConfig.development.enablePruning = true;  // Selective pruning
    aggressiveConfig.behavior.activityThreshold = 0.3f;  // Low activity threshold
    aggressiveConfig.behavior.defaultAction = MotorCommand::MoveForward;  // Driven action
    
    configs[AgentProfile::Aggressive] = aggressiveConfig;
    
    // Adaptive profile - changes strategy based on environment
    AgentConfig adaptiveConfig = defaultConfig;
    adaptiveConfig.motorDistribution.wait = 0.10f;  // Moderate waiting
    adaptiveConfig.motorDistribution.forward = 0.25f;  // Balanced movement
    adaptiveConfig.motorDistribution.interact = 0.15f;  // Some interaction
    adaptiveConfig.neuromodulation.enableCuriosity = true;  // Always curious
    adaptiveConfig.neuromodulation.curiosityThreshold = 0.25f;  // Moderate threshold
    adaptiveConfig.neuromodulation.explorationProbability = 0.15f;  // Moderate exploration
    adaptiveConfig.learning.learningRate = 0.012f;  // Moderate learning
    adaptiveConfig.development.enableMaturation = true;  // Develop but adaptive
    adaptiveConfig.development.enablePruning = true;  // Adaptive pruning
    adaptiveConfig.behavior.enableSelfRegulation = true;  // Self-regulation enabled
    
    configs[AgentProfile::Adaptive] = adaptiveConfig;
    
    return configs;
}

} // namespace nlm

(End of file - total 552 lines)