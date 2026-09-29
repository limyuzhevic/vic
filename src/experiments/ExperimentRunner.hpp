#pragma once

#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "ExperimentConstants.hpp"
#include <memory>

namespace nlm {

/**
 * Experiment runner and simulation executor
 * Handles the core simulation loop and execution
 */
class ExperimentRunner {
public:
    ExperimentRunner() = default;
    ~ExperimentRunner() = default;
    
    /**
     * Run the main simulation loop
     */
    bool runSimulation(std::shared_ptr<Brain> brain,
                      std::shared_ptr<SimpleWorld> world,
                      std::shared_ptr<AgentBrain> agent,
                      const Phase6Config& config,
                      float& totalReward,
                      float& totalFiringRate,
                      size_t& firingCount);
    
    /**
     * Run a single step of the simulation
     */
    bool runSimulationStep(std::shared_ptr<Brain> brain,
                          std::shared_ptr<SimpleWorld> world,
                          std::shared_ptr<AgentBrain> agent,
                          uint64_t step,
                          const Phase6Config& config,
                          float& stepReward,
                          float& firingRate);
    
    /**
     * Update development system if enabled
     */
    bool updateDevelopment(std::shared_ptr<AgentBrain> agent, float timeStep);
    
    /**
     * Apply reward modulation
     */
    bool applyRewardModulation(std::shared_ptr<AgentBrain> agent, float reward, float novelty);
    
    /**
     * Collect metrics from brain and agent
     */
    void collectMetrics(std::shared_ptr<Brain> brain,
                       std::shared_ptr<AgentBrain> agent,
                       float totalReward,
                       float totalFiringRate,
                       size_t totalSteps,
                       float& avgFiringRate,
                       float& avgSynapticWeight,
                       float& memoryEpisodesStored,
                       float& noveltyLevel,
                       float& curiosityLevel,
                       float& dopamineLevel);
    
    /**
     * Create simulation components
     */
    bool createSimulationComponents(std::shared_ptr<Brain>& brain,
                                   std::shared_ptr<SimpleWorld>& world,
                                   std::shared_ptr<AgentBrain>& agent,
                                   std::shared_ptr<Config>& config,
                                   const Phase6Config& phaseConfig);
};

} // namespace nlm
