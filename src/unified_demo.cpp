// NLM - Unified Demo System
// Provides consistent interfaces for running brain demonstrations

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace nlm {

// Demo configuration structure
struct DemoConfig {
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    std::string worldType;
    std::string demoType;  // "basic", "plasticity", "integration", "advanced"
    bool enableCheckpointing;
    bool enableDevelopment;
    bool enableCuriosity;
    bool enableReward;
    std::string checkpointPath;
    std::string outputPrefix;
    bool verbose;
    
    DemoConfig() : maxSteps(1000), neuronCount(500), regionCount(1), 
                   connectionProbability(0.15f), worldType("simple"), 
                   demoType("basic"), enableCheckpointing(false), 
                   enableDevelopment(false), enableCuriosity(false), 
                   enableReward(false), verbose(true) {}
};

// Unified demo runner
class UnifiedDemo {
public:
    UnifiedDemo() = default;
    ~UnifiedDemo() = default;
    
    bool run(const DemoConfig& config) {
        NLM_LOG_INFO("=== NLM Unified Demo ===");
        NLM_LOG_INFO("Demo type: " + config.demoType);
        NLM_LOG_INFO("Configuration: " + std::to_string(config.neuronCount) + " neurons, " +
                     std::to_string(config.maxSteps) + " steps");
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(config.neuronCount));
        cfg->set("region_count", static_cast<int64_t>(config.regionCount));
        cfg->set("connection_probability", config.connectionProbability);
        cfg->set("random_seed", static_cast<int64_t>(42));
        cfg->set("simulation_timestep", 0.001);
        
        // Set demo-specific parameters
        setupDemoConfig(cfg, config);
        
        // Create brain
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        // Create world
        SimpleWorld world;
        world.configure(16, 16, 16, 16);
        world.reset();
        
        // Create agent
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(config.enableReward);
        agent.enableStructuralPlasticity(config.enableDevelopment);
        agent.enableDevelopment(config.enableDevelopment);
        agent.enableCuriosity(config.enableCuriosity);
        
        NLM_LOG_INFO("Brain and agent initialized successfully");
        
        // Run demo based on type
        if (config.demoType == "basic") {
            return runBasicDemo(brain, agent, world, config);
        } else if (config.demoType == "plasticity") {
            return runPlasticityDemo(brain, agent, world, config);
        } else if (config.demoType == "integration") {
            return runIntegrationDemo(brain, agent, world, config);
        } else if (config.demoType == "advanced") {
            return runAdvancedDemo(brain, agent, world, config);
        } else {
            NLM_LOG_ERROR("Unknown demo type: " + config.demoType);
            return false;
        }
    }
    
private:
    void setupDemoConfig(std::shared_ptr<Config> cfg, const DemoConfig& config) {
        // Configure based on demo type
        if (config.demoType == "plasticity") {
            cfg->set("stdp_ltp_weight", 0.02f);
            cfg->set("stdp_ltd_weight", 0.015f);
            cfg->set("synaptogenesis_rate", 0.0001f);
            cfg->set("pruning_rate", 0.00001f);
        } else if (config.demoType == "integration") {
            cfg->set("stdp_ltp_weight", 0.01f);
            cfg->set("stdp_ltd_weight", 0.012f);
            cfg->set("synaptogenesis_rate", 0.00005f);
            cfg->set("pruning_rate", 0.000005f);
        } else if (config.demoType == "advanced") {
            cfg->set("stdp_ltp_weight", 0.015f);
            cfg->set("stdp_ltd_weight", 0.018f);
            cfg->set("synaptogenesis_rate", 0.0002f);
            cfg->set("pruning_rate", 0.00002f);
        }
    }
    
    bool runBasicDemo(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                      std::shared_ptr<SimpleWorld> world, const DemoConfig& config) {
        NLM_LOG_INFO("Running Basic Demo (Phase 2 functionality)");
        
        float totalReward = 0.0f;
        size_t firingCount = 0;
        
        for (uint64_t step = 0; step < config.maxSteps; ++step) {
            // Update world
            world->update(0.001);
            
            // Get sensory input
            SensoryPercept percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get motor command
            MotorCommand cmd = agent->decodeMotorCommand();
            
            // Apply action
            ActionResult actionResult = world->applyMotorCommand(cmd, step * 0.001);
            
            // Record metrics
            totalReward += actionResult.reward;
            if (brain->getFiringNeuronCount() > 0) firingCount++;
            
            // Periodic status
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                             " | Reward: " + std::to_string(totalReward / (step + 1)) +
                             " | Firing: " + std::to_string(brain->getAverageFiringRate()) +
                             " | WorkingMem: " + std::to_string(brain->getWorkingMemory() ? 
                                 brain->getWorkingMemory()->getActiveTraces() : 0));
            }
        }
        
        NLM_LOG_INFO("=== Basic Demo Complete ===");
        NLM_LOG_INFO("Average reward: " + std::to_string(totalReward / config.maxSteps));
        NLM_LOG_INFO("Firing neurons: " + std::to_string(firingCount));
        return true;
    }
    
    bool runPlasticityDemo(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                           std::shared_ptr<SimpleWorld> world, const DemoConfig& config) {
        NLM_LOG_INFO("Running Plasticity Demo (STDP and Hebbian learning)");
        
        // Enable plasticity
        if (brain->getRegion(RegionId(1))) {
            for (auto& syn : brain->getRegion(RegionId(1))->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // STDP and Hebbian
            }
        }
        
        // Record initial weights
        std::vector<float> initialWeights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        // Apply learning stimulus
        for (uint64_t step = 0; step < config.maxSteps; ++step) {
            world->update(0.001);
            
            // Inject correlated pre-post activity for STDP
            if (step % 2 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            } else {
                brain->injectCurrentToNeurons(NeuronType::Internal, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Get sensory input and action
            SensoryPercept percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            MotorCommand cmd = agent->decodeMotorCommand();
            world->applyMotorCommand(cmd, step * 0.001);
            
            if (step % 200 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                             " | Firing: " + std::to_string(brain->getAverageFiringRate()) +
                             " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
            }
        }
        
        // Record final weights and compute changes
        std::vector<float> finalWeights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        if (!initialWeights.empty() && !finalWeights.empty()) {
            size_t minSize = std::min(initialWeights.size(), finalWeights.size());
            size_t strengthened = 0, weakened = 0, unchanged = 0;
            
            for (size_t i = 0; i < minSize; ++i) {
                float delta = finalWeights[i] - initialWeights[i];
                if (delta > 0.01f) strengthened++;
                else if (delta < -0.01f) weakened++;
                else unchanged++;
            }
            
            NLM_LOG_INFO("=== Plasticity Results ===");
            NLM_LOG_INFO("Strengthened synapses: " + std::to_string(strengthened));
            NLM_LOG_INFO("Weakened synapses: " + std::to_string(weakened));
            NLM_LOG_INFO("Unchanged synapses: " + std::to_string(unchanged));
            
            if (strengthened > 0 || weakened > 0) {
                NLM_LOG_INFO("✓ Learning detected: Synaptic weights changed through experience");
                return true;
            }
        }
        
        NLM_LOG_INFO("✗ No significant learning detected");
        return true;  // Still return success even if no learning occurred
    }
    
    bool runIntegrationDemo(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                            std::shared_ptr<SimpleWorld> world, const DemoConfig& config) {
        NLM_LOG_INFO("Running Integration Demo (Phase 6 functionality)");
        
        // Use the existing Phase 6 integration experiment
        Phase6Config phase6Config;
        phase6Config.neuronCount = config.neuronCount;
        phase6Config.maxSteps = config.maxSteps;
        phase6Config.enableCheckpointing = config.enableCheckpointing;
        phase6Config.enableReplay = false;  // Disable for demo
        phase6Config.enableDevelopment = config.enableDevelopment;
        
        Phase6IntegratedExperiment experiment;
        Phase6IntegrationResult result = experiment.run(phase6Config);
        
        // Display results
        NLM_LOG_INFO("=== Integration Demo Results ===");
        NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
        NLM_LOG_INFO("Avg firing rate: " + std::to_string(result.avgFiringRate));
        NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
        NLM_LOG_INFO("Dopamine level: " + std::to_string(result.dopamineLevel));
        
        NLM_LOG_INFO("=== System Integration Status ===");
        NLM_LOG_INFO("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Prediction: " + std::string(result.predictionIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Development: " + std::string(result.developmentIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Checkpointing: " + std::string(result.checkpointingWorks ? "WORKING" : "NOT WORKING"));
        
        return result.totalReward > 0 || result.memoryEpisodesStored > 0;
    }
    
    bool runAdvancedDemo(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                         std::shared_ptr<SimpleWorld> world, const DemoConfig& config) {
        NLM_LOG_INFO("Running Advanced Demo (All features enabled)");
        
        // Enable all features
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        // Run for a longer period
        uint64_t demoSteps = std::min(config.maxSteps, static_cast<uint64_t>(5000));
        
        float totalReward = 0.0f;
        size_t episodeCount = 0;
        size_t developmentStageChanges = 0;
        
        for (uint64_t step = 0; step < demoSteps; ++step) {
            // Update world
            world->update(0.001);
            
            // Get sensory input
            SensoryPercept percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get motor command
            MotorCommand cmd = agent->decodeMotorCommand();
            
            // Apply action
            ActionResult actionResult = world->applyMotorCommand(cmd, step * 0.001);
            
            // Apply reward modulation
            agent->applyRewardModulation(actionResult.reward, 0.0f);
            
            // Update development periodically
            if (step % 100 == 0) {
                agent->updateDevelopment(0.001);
                
                // Track development stage changes
                DevelopmentStage currentStage = brain->getDevelopmentSystem() ? 
                    brain->getDevelopmentSystem()->getStage() : DevelopmentStage::Initial;
                if (currentStage != DevelopmentStage::Initial) {
                    developmentStageChanges++;
                }
            }
            
            // Track episodes
            if (brain->getEpisodicMemory()) {
                episodeCount = brain->getEpisodicMemory()->getEpisodeCount();
            }
            
            totalReward += actionResult.reward;
            
            // Log progress
            if (step % 500 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                             " | Reward: " + std::to_string(totalReward / (step + 1)) +
                             " | Episodes: " + std::to_string(episodeCount) +
                             " | DevStage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())) +
                             " | Curiosity: " + std::to_string(agent->getCuriosityLevel()));
            }
        }
        
        NLM_LOG_INFO("=== Advanced Demo Complete ===");
        NLM_LOG_INFO("Total reward: " + std::to_string(totalReward));
        NLM_LOG_INFO("Average reward: " + std::to_string(totalReward / demoSteps));
        NLM_LOG_INFO("Episodes stored: " + std::to_string(episodeCount));
        NLM_LOG_INFO("Development stage changes: " + std::to_string(developmentStageChanges));
        NLM_LOG_INFO("Final curiosity level: " + std::to_string(agent->getCuriosityLevel()));
        
        return true;
    }
};

// Convenience function for easy usage
bool runUnifiedDemo(const std::string& demoType, uint64_t steps = 1000) {
    UnifiedDemo demo;
    DemoConfig config;
    config.demoType = demoType;
    config.maxSteps = steps;
    return demo.run(config);
}

} // namespace nlm
