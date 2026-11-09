#include "Phase5Experiment.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <chrono>
#include "nlm_experiment_utils.h"

namespace nlm {

Phase5Experiment::Phase5Experiment() {}

Phase5Experiment::~Phase5Experiment() = default;

Phase5ExperimentResult Phase5Experiment::run(const Phase5Config& config) {
    Phase5ExperimentResult result;
    result.startTime = time(nullptr);
    
    auto startWall = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("=== Phase 5 Enhanced Experiment ===");
    NLM_LOG_INFO("Configuration: " + std::to_string(config.neuronCount) + " neurons, " +
                 std::to_string(config.maxSteps) + " steps");
    
    // Create enhanced configuration using the factory
    auto cfg = ConfigBuilder::createExtendedConfig();
    cfg->set("neuron_count", static_cast<int64_t>(config.neuronCount));
    cfg->set("region_count", static_cast<int64_t>(config.regionCount));
    cfg->set("connection_probability", config.connectionProbability);
    
    // Create brain using the factory
    auto brain = ExperimentFactory::createBrain(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        result.endTime = time(nullptr);
        return result;
    }
    
    // Create world using the factory
    auto world = ExperimentFactory::createWorld(config.environmentWidth, config.environmentHeight,
                                                config.gridWidth, config.gridHeight);
    
    // Create agent using the factory
    auto agent = ExperimentFactory::createAgentBrain(brain);
    agent->initialize(*world);
    
    // Configure agent features
    agent->enableRewardModulation(config.enableRewardModulation);
    agent->enableCuriosity(config.enableCuriosity);
    agent->enableDevelopment(config.enableDevelopment);
    agent->enableStructuralPlasticity(config.enableDevelopment);
    
    NLM_LOG_INFO("Brain and agent initialized successfully");
    
    // Run simulation using the enhanced simulation runner
    SimulationRunner::runAgentSimulation(*brain, *world, *agent, 
                                         config.maxSteps, config.enableDevelopment, 
                                         config.enableCuriosity);
    
    // Collect metrics using the metrics collector
    MetricsCollector::SimulationMetrics metrics;
    MetricsCollector::collectSimulationMetrics(*brain, *agent, config.maxSteps, metrics);
    
    // Convert to result structure
    result.totalReward = metrics.totalReward;
    result.avgFiringRate = metrics.avgFiringRate;
    result.memoryEpisodesStored = metrics.memoryEpisodesStored;
    result.noveltyLevel = metrics.noveltyLevel;
    result.curiosityLevel = metrics.curiosityLevel;
    result.dopamineLevel = metrics.dopamineLevel;
    result.endTime = time(nullptr);
    
    // Calculate wall clock time
    auto endWall = std::chrono::high_resolution_clock::now();
    result.totalWallClockTime = std::chrono::duration<double>(endWall - startWall).count();
    
    NLM_LOG_INFO("=== Experiment Complete ===");
    NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
    NLM_LOG_INFO("Avg firing rate: " + std::to_string(result.avgFiringRate));
    NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
    NLM_LOG_INFO("Wall time: " + std::to_string(result.totalWallClockTime) + "s");
    
    return result;
}

} // namespace nlm