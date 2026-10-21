// Phase6IntegratedExperiment - Refactored Integration Test Framework
// Provides comprehensive integration testing for Phase 6 neuromorphic brain systems
// Refactored into smaller, testable components for improved maintainability

#pragma once

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase6IntegratedExperimentIntegration.hpp"
#include <chrono>
#include <cmath>

namespace nlm {

// ========== PHASE 6 INTEGRATION RESULT ==========

// Result structure for Phase 6 integration experiments
struct Phase6IntegrationResult {
    time_t startTime;
    time_t endTime;
    double totalWallClockTime;
    
    float totalReward;
    float avgFiringRate;
    float memoryEpisodesStored;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
    
    Phase6IntegrationResult() : startTime(0), endTime(0), totalWallClockTime(0.0),
                                totalReward(0.0f), avgFiringRate(0.0f), 
                                memoryEpisodesStored(0.0f), noveltyLevel(0.0f),
                                curiosityLevel(0.0f), dopamineLevel(0.0f),
                                memoryWorkingMemoryIntegrated(false),
                                memoryEpisodicMemoryIntegrated(false),
                                neuromodulationIntegrated(false),
                                predictionIntegrated(false),
                                developmentIntegrated(false),
                                checkpointingWorks(false) {}
};

// ========== PHASE 6 CONFIGURATION ==========

// Configuration structure for Phase 6 integration experiments
struct Phase6Config {
    size_t neuronCount;
    size_t maxSteps;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    
    Phase6Config() : neuronCount(500), maxSteps(2000), regionCount(1),
                     connectionProbability(0.1f), enableCheckpointing(true),
                     enableReplay(true), enableDevelopment(true),
                     checkpointPath("/tmp/nlm_checkpoint_test.bin") {}
};

// ========== PHASE 6 INTEGRATED EXPERIMENT (REFACTORED) ==========

// Phase 6 integrated experiment - refactored to use component-based testing
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment() : integrationTestRunner() {}
    
    ~Phase6IntegratedExperiment() = default;
    
    // Run the complete integration experiment
    Phase6IntegrationResult run(const Phase6Config& config) {
        NLM_LOG_INFO("=== Phase 6 Integration Experiment (Refactored) ===");
        NLM_LOG_INFO("Configuration: " + std::to_string(config.neuronCount) + " neurons, " +
                     std::to_string(config.maxSteps) + " steps");
        
        Phase6IntegrationResult result;
        result.startTime = time(nullptr);
        
        auto startWall = std::chrono::high_resolution_clock::now();
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", config.neuronCount);
        cfg->set("region_count", config.regionCount);
        cfg->set("connection_probability", config.connectionProbability);
        cfg->set("stdp_ltp_weight", 0.01f);
        cfg->set("stdp_ltd_weight", 0.012f);
        cfg->set("synaptogenesis_rate", 0.0001f);
        cfg->set("pruning_rate", 0.00001f);
        
        // Create brain
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            result.endTime = time(nullptr);
            return result;
        }
        
        // Create simple world
        SimpleWorld world;
        world.initialize(16, 16);
        
        // Create agent
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(config.enableDevelopment);
        agent.enableDevelopment(config.enableDevelopment);
        agent.enableCuriosity(true);
        
        NLM_LOG_INFO("Brain and agent initialized successfully");
        
        // Run simulation
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        size_t firingCount = 0;
        
        for (uint64_t step = 0; step < config.maxSteps; ++step) {
            // Get observation
            SensoryPercept percept = world.observe(agent.getBrain()->getRegions()[0].get());
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get motor command
            MotorCommand cmd = agent.decodeMotorCommand();
            
            // Apply action to world
            world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd);
            
            // Compute reward
            float reward = world.computeReward(agent.getBrain()->getRegions()[0].get());
            totalReward += reward;
            
            // Apply reward modulation
            agent.applyRewardModulation(reward, 0.0f);
            
            // Update development
            if (config.enableDevelopment) {
                agent.updateDevelopment(0.001);
            }
            
            // Collect metrics
            totalFiringRate += brain->getAverageFiringRate();
            if (brain->getFiringNeuronCount() > 0) firingCount++;
            
            // Periodic status (less frequent for cleaner output)
            if (step % 1000 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                            " | Reward: " + std::to_string(totalReward / (step + 1)) +
                            " | Firing: " + std::to_string(brain->getAverageFiringRate()) +
                            " | WorkingMem: " + std::to_string(brain->getWorkingMemory() ? 
                                brain->getWorkingMemory()->getActiveTraces() : 0));
            }
        }
        
        // Collect final metrics
        result.totalReward = totalReward / config.maxSteps;
        result.avgFiringRate = totalFiringRate / config.maxSteps;
        result.memoryEpisodesStored = brain->getEpisodicMemory() ? 
            static_cast<float>(brain->getEpisodicMemory()->getEpisodeCount()) : 0.0f;
        result.noveltyLevel = 0.5f; // Placeholder
        result.curiosityLevel = 0.5f; // Placeholder
        result.dopamineLevel = 0.5f; // Placeholder
        
        // Verify integration
        result.memoryWorkingMemoryIntegrated = (brain->getWorkingMemory() != nullptr);
        result.memoryEpisodicMemoryIntegrated = (brain->getEpisodicMemory() != nullptr);
        result.neuromodulationIntegrated = (brain->getDopamine() != nullptr);
        result.predictionIntegrated = (brain->getPredictionSystem() != nullptr);
        result.developmentIntegrated = (brain->getDevelopmentSystem() != nullptr);
        
        // Test checkpointing
        if (config.enableCheckpointing) {
            NLM_LOG_INFO("Testing checkpoint save/load...");
            if (brain->save(config.checkpointPath)) {
                NLM_LOG_INFO("Checkpoint saved successfully");
                
                // Create new brain and load
                auto brain2 = std::make_shared<Brain>(cfg);
                brain2->initialize();
                
                if (brain2->load(config.checkpointPath)) {
                    NLM_LOG_INFO("Checkpoint loaded successfully");
                    result.checkpointingWorks = true;
                } else {
                    NLM_LOG_ERROR("Failed to load checkpoint");
                }
            } else {
                NLM_LOG_ERROR("Failed to save checkpoint");
            }
        }
        
        result.endTime = time(nullptr);
        auto endWall = std::chrono::high_resolution_clock::now();
        result.totalWallClockTime = std::chrono::duration<double>(endWall - startWall).count();
        
        NLM_LOG_INFO("=== Experiment Complete ===");
        NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
        NLM_LOG_INFO("Avg firing rate: " + std::to_string(result.avgFiringRate));
        NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
        NLM_LOG_INFO("Wall time: " + std::to_string(result.totalWallClockTime) + "s");
        
        return result;
    }
    
    // Run quick integration verification
    bool verifyIntegration() {
        IntegrationTestRunner runner;
        return runner.runAllTests();
    }
    
    // Run the enhanced integration test suite
    bool runEnhancedTests() {
        IntegrationTestRunner runner;
        return runner.runAllTests();
    }
    
    // Test individual systems
    bool testMemoryIntegration() {
        return BrainIntegrationTests::testBrainMemorySystems();
    }
    
    bool testNeuromodulationIntegration() {
        return NeuromodulationIntegrationTests::testDopamineSystem() &&
               NeuromodulationIntegrationTests::testCuriositySystem() &&
               NeuromodulationIntegrationTests::testPredictionErrorSystem() &&
               NeuromodulationIntegrationTests::testNoveltyDetection();
    }
    
    bool testCheckpointing() {
        return CheckpointIntegrationTests::testCheckpointSaveLoad();
    }
    
    bool testReplay() {
        return ReplayIntegrationTests::testMemoryReplay();
    }
    
private:
    IntegrationTestRunner integrationTestRunner;
};

} // namespace nlm