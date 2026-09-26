#pragma once

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <memory>
#include <string>

namespace nlm {

// Utility functions for common initialization patterns

// Configure standard brain parameters for experiments
inline void configureStandardBrain(std::shared_ptr<Config> config, 
                                  size_t neuronCount = 500,
                                  size_t regionCount = 1,
                                  float connectionProb = 0.1f,
                                  uint64_t randomSeed = 42u) {
    if (!config) {
        NLM_LOG_ERROR("Config is null, cannot configure brain");
        return;
    }
    
    config->set("neuron_count", static_cast<int64_t>(neuronCount));
    config->set("region_count", static_cast<int64_t>(regionCount));
    config->set("connection_probability", connectionProb);
    config->set("random_seed", static_cast<int64_t>(randomSeed));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    
    NLM_LOG_INFO("Configured brain with standard parameters");
}

// Configure simple environment parameters
inline void configureSimpleEnvironment(std::shared_ptr<Config> config,
                                       float width = 20.0f,
                                       float height = 20.0f,
                                       int visionWidth = 16,
                                       int visionHeight = 16) {
    if (!config) {
        NLM_LOG_ERROR("Config is null, cannot configure environment");
        return;
    }
    
    config->set("environment_width", width);
    config->set("environment_height", height);
    config->set("vision_width", visionWidth);
    config->set("vision_height", visionHeight);
    
    NLM_LOG_INFO("Configured environment with standard parameters");
}

// Configure agent brain with common subsystems
inline std::shared_ptr<AgentBrain> createAgentWithStandardSubsystems(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    bool enableRewardModulation = true,
    bool enableStructuralPlasticity = true,
    bool enableDevelopment = true,
    bool enableCuriosity = true) {
    
    if (!brain || !world) {
        NLM_LOG_ERROR("Brain or world is null, cannot create agent");
        return nullptr;
    }
    
    auto agentBrain = std::make_shared<AgentBrain>(brain);
    agentBrain->initialize(world);
    
    // Enable common subsystems
    if (enableRewardModulation) {
        agentBrain->enableRewardModulation(true);
    }
    if (enableStructuralPlasticity) {
        agentBrain->enableStructuralPlasticity(true);
    }
    if (enableDevelopment) {
        agentBrain->enableDevelopment(true);
    }
    if (enableCuriosity) {
        agentBrain->enableCuriosity(true);
    }
    
    NLM_LOG_INFO("Created agent with standard subsystems enabled");
    return agentBrain;
}

// Complete experiment setup helper
inline struct ExperimentSetup {
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    
    bool isValid() const {
        return config && brain && agent && world;
    }
    
    void reset() {
        if (world) world->reset();
        if (agent) agent->reset();
        if (brain) brain->reset();
        brain->initialize();
        agent->initialize(*world);
    }
} createStandardExperimentSetup(
    const std::string& configName = "standard",
    size_t neuronCount = 500,
    size_t regionCount = 1,
    size_t worldWidth = 20,
    size_t worldHeight = 20,
    int visionWidth = 16,
    int visionHeight = 16,
    uint64_t randomSeed = 42u) {
    
    ExperimentSetup setup;
    
    // Create configuration
    setup.config = std::make_shared<Config>();
    configureStandardBrain(setup.config, neuronCount, regionCount, 0.1f, randomSeed);
    configureSimpleEnvironment(setup.config, static_cast<float>(worldWidth), 
                               static_cast<float>(worldHeight), visionWidth, visionHeight);
    
    // Create world
    setup.world = std::make_shared<SimpleWorld>();
    setup.world->configure(static_cast<float>(worldWidth), static_cast<float>(worldHeight), 
                          visionWidth, visionHeight);
    setup.world->setRandomSeed(randomSeed);
    
    // Create brain
    setup.brain = std::make_shared<Brain>(setup.config);
    if (!setup.brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain in standard experiment setup");
        return setup;
    }
    
    // Create agent with standard subsystems
    setup.agent = createAgentWithStandardSubsystems(setup.brain, setup.world);
    
    if (!setup.isValid()) {
        NLM_LOG_ERROR("Standard experiment setup failed - invalid components");
    } else {
        NLM_LOG_INFO("Standard experiment setup completed successfully");
    }
    
    return setup;
}

// Agent subsystem enable helper
inline void enableAgentSubsystems(std::shared_ptr<AgentBrain> agent,
                                 const std::vector<std::string>& subsystems) {
    if (!agent) {
        NLM_LOG_ERROR("Agent is null, cannot enable subsystems");
        return;
    }
    
    for (const auto& subsystem : subsystems) {
        if (subsystem == "reward_modulation" || subsystem == "reward") {
            agent->enableRewardModulation(true);
        } else if (subsystem == "structural_plasticity" || subsystem == "plasticity") {
            agent->enableStructuralPlasticity(true);
        } else if (subsystem == "development" || subsystem == "developmental") {
            agent->enableDevelopment(true);
        } else if (subsystem == "curiosity" || subsystem == "exploration") {
            agent->enableCuriosity(true);
        } else if (subsystem == "prediction" || subsystem == "prediction_error") {
            agent->enablePredictionError(true);
        } else if (subsystem == "novelty") {
            agent->enableNovelty(true);
        } else {
            NLM_LOG_WARNING("Unknown subsystem: " + subsystem);
        }
    }
    
    NLM_LOG_INFO("Enabled agent subsystems: " + std::to_string(subsystems.size()));
}

// Experiment metrics helper
inline struct ExperimentMetrics {
    double totalReward = 0.0;
    size_t totalSteps = 0;
    size_t totalSpikes = 0;
    size_t actionCounts[6] = {0};  // MoveForward, MoveBackward, TurnLeft, TurnRight, Interact, Wait
    double finalNovelty = 0.0;
    double finalCuriosity = 0.0;
    double finalEnergy = 0.0;
    double predictionError = 0.0;
    std::string developmentalStage = "Unknown";
    
    void reset() {
        totalReward = 0.0;
        totalSteps = 0;
        totalSpikes = 0;
        std::fill(std::begin(actionCounts), std::end(actionCounts), 0);
        finalNovelty = 0.0;
        finalCuriosity = 0.0;
        finalEnergy = 0.0;
        predictionError = 0.0;
        developmentalStage = "Unknown";
    }
    
    void update(const ExperimentMetrics& other) {
        totalReward += other.totalReward;
        totalSteps += other.totalSteps;
        totalSpikes += other.totalSpikes;
        for (int i = 0; i < 6; ++i) {
            actionCounts[i] += other.actionCounts[i];
        }
        // Keep the most recent values
        finalNovelty = other.finalNovelty;
        finalCuriosity = other.finalCuriosity;
        finalEnergy = other.finalEnergy;
        predictionError = other.predictionError;
        developmentalStage = other.developmentalStage;
    }
} recordExperimentMetrics(const std::shared_ptr<Brain>& brain,
                        const std::shared_ptr<AgentBrain>& agent,
                        const std::shared_ptr<SimpleWorld>& world,
                        const std::string& stage = "Unknown") {
    ExperimentMetrics metrics;
    
    if (brain) {
        metrics.totalSpikes = brain->getTotalSpikeCount();
        metrics.finalEnergy = world ? world->getAgentBody().energy : 0.0;
        metrics.developmentalStage = stage;
        metrics.predictionError = agent ? agent->getPredictionError() : 0.0;
    }
    
    if (agent) {
        metrics.finalNovelty = agent->getNoveltyLevel();
        metrics.finalCuriosity = agent->getCuriosityLevel();
        metrics.developmentalStage = stage;
        metrics.predictionError = agent->getPredictionError();
    }
    
    if (world) {
        metrics.finalEnergy = world->getAgentBody().energy;
    }
    
    return metrics;
}

// Safe brain initialization with error handling
inline bool safeBrainInitialize(std::shared_ptr<Brain> brain, const std::string& context = "unknown") {
    if (!brain) {
        NLM_LOG_ERROR("Brain is null, cannot initialize");
        return false;
    }
    
    bool success = brain->initialize();
    if (!success) {
        NLM_LOG_ERROR("Brain initialization failed in context: " + context);
    } else {
        NLM_LOG_INFO("Brain initialized successfully in context: " + context);
    }
    
    return success;
}

// Configuration file loading with fallback
inline bool loadConfigurationWithFallback(const std::string& configFile, std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Config is null, cannot load configuration");
        return false;
    }
    
    bool loaded = config->loadFromFile(configFile);
    if (!loaded) {
        NLM_LOG_WARNING("Could not load config from " + configFile + ", using defaults");
        // Set reasonable defaults
        configureStandardBrain(config, 500, 1, 0.1f, 42u);
        configureSimpleEnvironment(config, 20.0f, 20.0f, 16, 16);
    }
    
    return true;
}

// Experiment runner helper
inline void runExperimentStep(
    const std::shared_ptr<Brain>& brain,
    const std::shared_ptr<AgentBrain>& agent,
    const std::shared_ptr<SimpleWorld>& world,
    bool logProgress = false,
    size_t logInterval = 100) {
    
    if (!brain || !agent || !world) {
        NLM_LOG_ERROR("Invalid components for experiment step");
        return;
    }
    
    // Update world
    world->update(0.1);
    
    // Process sensory input
    const SensoryPercept& percept = world->getSensoryPercept();
    agent->processSensoryInput(percept);
    
    // Run brain steps
    for (int i = 0; i < 10; ++i) {
        brain->step(i);
    }
    
    // Decode and apply motor command
    MotorCommand cmd = agent->decodeMotorCommand();
    world->applyMotorCommand(cmd, world->getSimulationTime());
    
    // Apply neuromodulation
    agent->applyRewardModulation(world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0, 0.0);
    
    // Update development
    agent->updateDevelopment(0.1);
    
    if (logProgress && brain->getTotalSpikeCount() % logInterval == 0) {
        NLM_LOG_INFO("Experiment step completed - Spikes: " + std::to_string(brain->getTotalSpikeCount()));
    }
}

} // namespace nlm
