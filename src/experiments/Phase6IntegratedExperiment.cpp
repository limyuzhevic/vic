#include "Phase6IntegratedExperiment.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <chrono>
#include <cmath>
#include "nlm_experiment_utils.h"

namespace nlm {

Phase6IntegratedExperiment::Phase6IntegratedExperiment() {}

Phase6IntegratedExperiment::~Phase6IntegratedExperiment() = default;

Phase6IntegrationResult Phase6IntegratedExperiment::run(const Phase6Config& config) {
    Phase6IntegrationResult result;
    result.startTime = time(nullptr);
    
    auto startWall = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("=== Phase 6 Integration Experiment ===");
    NLM_LOG_INFO("Configuration: " + std::to_string(config.neuronCount) + " neurons, " +
                 std::to_string(config.maxSteps) + " steps");
    
    // Create configuration using the enhanced config builder
    auto cfg = ConfigBuilder::createDefaultConfig();
    cfg->set("neuron_count", static_cast<int64_t>(config.neuronCount));
    cfg->set("region_count", static_cast<int64_t>(config.regionCount));
    cfg->set("connection_probability", config.connectionProbability);
    cfg->set("development_synaptogenesis_rate", 0.0001f);
    cfg->set("development_pruning_rate", 0.00001f);
    cfg->set("dopamine_baseline", 0.1f);
    
    // Create brain
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        result.endTime = time(nullptr);
        return result;
    }
    
    // Create simple world using the factory
    SimpleWorld world;
    world.configure(config.environmentWidth, config.environmentHeight, 
                   config.environmentWidth, config.environmentHeight);
    world.reset();
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(config.enableDevelopment);
    agent.enableDevelopment(config.enableDevelopment);
    agent.enableCuriosity(true);
    
    NLM_LOG_INFO("Brain and agent initialized successfully");
    
    // Run simulation using the enhanced simulation runner
    // This consolidates the common simulation loop pattern
    SimulationRunner::runAgentSimulation(brain, world, agent, config.maxSteps, 
                                         config.enableDevelopment, true);
    
    // Collect final metrics using the metrics collector
    MetricsCollector::SimulationMetrics metrics;
    MetricsCollector::collectSimulationMetrics(brain, agent, config.maxSteps, metrics);
    
    // Convert to result structure
    result.totalReward = metrics.totalReward;
    result.avgFiringRate = metrics.avgFiringRate;
    result.avgSynapticWeight = brain->getAverageSynapticWeight();
    result.memoryEpisodesStored = metrics.memoryEpisodesStored;
    result.noveltyLevel = metrics.noveltyLevel;
    result.curiosityLevel = metrics.curiosityLevel;
    result.dopamineLevel = metrics.dopamineLevel;
    result.endTime = time(nullptr);
    
    // Verify integration using the standardized verification pattern
    result.memoryWorkingMemoryIntegrated = metrics.memoryWorkingMemoryIntegrated;
    result.memoryEpisodicMemoryIntegrated = metrics.memoryEpisodicMemoryIntegrated;
    result.neuromodulationIntegrated = (brain->getDopamine() != nullptr);
    result.predictionIntegrated = (brain->getPredictionSystem() != nullptr);
    result.developmentIntegrated = (brain->getDevelopmentSystem() != nullptr);
    
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