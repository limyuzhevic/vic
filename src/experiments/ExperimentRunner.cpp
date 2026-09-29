#include "ExperimentRunner.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>

namespace nlm {

bool ExperimentRunner::createSimulationComponents(std::shared_ptr<Brain>& brain,
                                                 std::shared_ptr<SimpleWorld>& world,
                                                 std::shared_ptr<AgentBrain>& agent,
                                                 std::shared_ptr<Config>& config,
                                                 const Phase6Config& phaseConfig) {
    try {
        // Create configuration
        config = std::make_shared<Config>();
        config->set("neuron_count", phaseConfig.neuronCount);
        config->set("region_count", phaseConfig.regionCount);
        config->set("connection_probability", phaseConfig.connectionProbability);
        config->set("stdp_ltp_weight", ExperimentConstants::DEFAULT_STDP_LTP_WEIGHT);
        config->set("stdp_ltd_weight", ExperimentConstants::DEFAULT_STDP_LTD_WEIGHT);
        config->set("synaptogenesis_rate", ExperimentConstants::DEFAULT_SYNAPTOGENESIS_RATE);
        config->set("pruning_rate", ExperimentConstants::DEFAULT_PRUNING_RATE);
        
        // Create brain
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        // Create world
        world = std::make_shared<SimpleWorld>();
        world->initialize(16, 16);
        
        // Create agent
        agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(phaseConfig.enableDevelopment);
        agent->enableDevelopment(phaseConfig.enableDevelopment);
        agent->enableCuriosity(true);
        
        NLM_LOG_INFO("Simulation components created successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to create simulation components: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentRunner::runSimulation(std::shared_ptr<Brain> brain,
                                     std::shared_ptr<SimpleWorld> world,
                                     std::shared_ptr<AgentBrain> agent,
                                     const Phase6Config& config,
                                     float& totalReward,
                                     float& totalFiringRate,
                                     size_t& firingCount) {
    if (!brain || !world || !agent) {
        NLM_LOG_ERROR("Invalid simulation components");
        return false;
    }
    
    totalReward = 0.0f;
    totalFiringRate = 0.0f;
    firingCount = 0;
    
    NLM_LOG_INFO("Starting simulation with " + std::to_string(config.maxSteps) + " steps");
    
    for (uint64_t step = 0; step < config.maxSteps; ++step) {
        if (!runSimulationStep(brain, world, agent, step, config, 
                              totalReward, totalFiringRate)) {
            NLM_LOG_ERROR("Simulation failed at step " + std::to_string(step));
            return false;
        }
        
        // Update development if enabled
        if (config.enableDevelopment) {
            updateDevelopment(agent, ExperimentConstants::DEFAULT_DEVELOPMENT_TIME_STEP);
        }
        
        // Periodic status update
        if (step % ExperimentConstants::STATUS_REPORT_INTERVAL == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Reward: " + std::to_string(totalReward / (step + 1)) +
                        " | Firing: " + std::to_string(totalFiringRate / (step + 1)) +
                        " | WorkingMem: " + std::to_string(brain->getWorkingMemory() ? 
                            brain->getWorkingMemory()->getActiveTraces() : 0));
        }
    }
    
    // Update firing count based on final state
    if (brain->getFiringNeuronCount() > 0) firingCount++;
    
    NLM_LOG_INFO("Simulation completed successfully");
    return true;
}

bool ExperimentRunner::runSimulationStep(std::shared_ptr<Brain> brain,
                                         std::shared_ptr<SimpleWorld> world,
                                         std::shared_ptr<AgentBrain> agent,
                                         uint64_t step,
                                         const Phase6Config& config,
                                         float& totalReward,
                                         float& totalFiringRate) {
    if (!brain || !world || !agent) return false;
    
    float stepReward = 0.0f;
    float firingRate = 0.0f;
    
    try {
        // Get observation
        SensoryPercept percept = world->observe(agent->getBrain()->getRegions()[0].get());
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Get motor command
        MotorCommand cmd = agent->decodeMotorCommand();
        
        // Apply action to world
        world->applyAction(agent->getBrain()->getRegions()[0].get(), cmd);
        
        // Compute reward
        stepReward = world->computeReward(agent->getBrain()->getRegions()[0].get());
        
        // Apply reward modulation
        applyRewardModulation(agent, stepReward, 0.0f);
        
        // Update metrics
        totalReward += stepReward;
        firingRate = brain->getAverageFiringRate();
        totalFiringRate += firingRate;
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Simulation step failed at step " + std::to_string(step) + ": " + std::string(e.what()));
        return false;
    }
}

bool ExperimentRunner::updateDevelopment(std::shared_ptr<AgentBrain> agent, float timeStep) {
    if (!agent) return false;
    
    try {
        agent->updateDevelopment(timeStep);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Development update failed: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentRunner::applyRewardModulation(std::shared_ptr<AgentBrain> agent, float reward, float novelty) {
    if (!agent) return false;
    
    try {
        agent->applyRewardModulation(reward, novelty);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Reward modulation failed: " + std::string(e.what()));
        return false;
    }
}

void ExperimentRunner::collectMetrics(std::shared_ptr<Brain> brain,
                                       std::shared_ptr<AgentBrain> agent,
                                       float totalReward,
                                       float totalFiringRate,
                                       size_t totalSteps,
                                       float& avgFiringRate,
                                       float& avgSynapticWeight,
                                       float& memoryEpisodesStored,
                                       float& noveltyLevel,
                                       float& curiosityLevel,
                                       float& dopamineLevel) {
    if (!brain || !agent) {
        NLM_LOG_ERROR("Cannot collect metrics: invalid components");
        return;
    }
    
    // Calculate average metrics
    avgFiringRate = (totalSteps > 0) ? (totalFiringRate / totalSteps) : 0.0f;
    
    // Note: synaptic weight calculation depends on Brain implementation
    // For now, we'll set a placeholder value
    avgSynapticWeight = 0.0f; // TODO: Implement actual synaptic weight calculation
    
    // Memory metrics
    memoryEpisodesStored = brain->getEpisodicMemory() ? 
        static_cast<float>(brain->getEpisodicMemory()->getEpisodeCount()) : 0.0f;
    
    // Neuromodulation metrics
    noveltyLevel = agent->getNoveltyLevel();
    curiosityLevel = agent->getCuriosityLevel();
    dopamineLevel = agent->getNeuromodulationLevel();
}

} // namespace nlm
