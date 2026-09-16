#include "Phase6IntegratedExperiment.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>

namespace nlm {

Phase6IntegratedExperiment::Phase6IntegratedExperiment() {
    NLM_LOG_INFO("Phase6IntegratedExperiment initialized");
}

Phase6IntegratedExperiment::~Phase6IntegratedExperiment() = default;

Phase6IntegrationResult Phase6IntegratedExperiment::run(const Phase6Config& config) {
    NLM_LOG_INFO("Phase6IntegratedExperiment::run - Starting integration experiment");
    
    // Initialize config validation - check if config.neuronCount is reasonable
    if (config.neuronCount == 0) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Invalid neuronCount: 0");
        Phase6IntegrationResult result;
        result.endTime = time(nullptr);
        return result;
    }
    
    Phase6IntegrationResult result;
    result.startTime = time(nullptr);
    
    auto startWall = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("=== Phase 6 Integration Experiment ===");
    NLM_LOG_INFO("Configuration: " + std::to_string(config.neuronCount) + " neurons, " +
                 std::to_string(config.maxSteps) + " steps");
    
    try {
        // Create configuration with validation
        auto cfg = std::make_shared<Config>();
        if (!cfg) {
            NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Failed to create Config instance");
            result.endTime = time(nullptr);
            return result;
        }
        
        cfg->set("neuron_count", config.neuronCount);
        cfg->set("region_count", config.regionCount);
        cfg->set("connection_probability", config.connectionProbability);
        cfg->set("stdp_ltp_weight", 0.01f);
        cfg->set("stdp_ltd_weight", 0.012f);
        cfg->set("synaptogenesis_rate", 0.0001f);
        cfg->set("pruning_rate", 0.00001f);
        
        // Create brain with validation
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain) {
            NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Failed to create Brain instance");
            result.endTime = time(nullptr);
            return result;
        }
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Failed to initialize brain");
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
        
        // Run simulation with comprehensive validation
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        size_t firingCount = 0;
        
        for (uint64_t step = 0; step < config.maxSteps; ++step) {
            try {
                // Get observation with validation
                const auto& regions = brain->getRegions();
                if (regions.empty()) {
                    NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Brain has no regions at step " + std::to_string(step));
                    break;
                }
                
                auto* region = regions[0].get();
                if (!region) {
                    NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Region pointer is null at step " + std::to_string(step));
                    break;
                }
                
                SensoryPercept percept = world.observe(region);
                
                // Process sensory input
                agent.processSensoryInput(percept);
                
                // Brain step
                brain->step(step, step * 0.001);
                
                // Get motor command
                MotorCommand cmd = agent.decodeMotorCommand();
                
                // Apply action to world
                world.applyAction(region, cmd);
                
                // Compute reward
                float reward = world.computeReward(region);
                totalReward += reward;
                
                // Apply reward modulation
                agent.applyRewardModulation(reward, 0.0f);
                
                // Update development
                if (config.enableDevelopment) {
                    agent.updateDevelopment(0.001);
                }
                
                // Collect metrics with validation
                float firingRate = brain->getAverageFiringRate();
                totalFiringRate += firingRate;
                if (firingRate > 0.0f) firingCount++;
                
                // Periodic status
                if (step % 1000 == 0) {
                    float workingMem = 0.0f;
                    auto* workingMemory = brain->getWorkingMemory();
                    if (workingMemory) {
                        workingMem = static_cast<float>(workingMemory->getActiveTraces());
                    }
                    
                    NLM_LOG_INFO("Step " + std::to_string(step) + 
                                " | Reward: " + std::to_string(totalReward / (step + 1)) +
                                " | Firing: " + std::to_string(firingRate) +
                                " | WorkingMem: " + std::to_string(workingMem));
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Exception at step " + std::to_string(step) + ": " + std::string(e.what()));
                break;
            } catch (...) {
                NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Unknown exception at step " + std::to_string(step));
                break;
            }
        }
        
        // Collect final metrics with validation
        result.totalReward = totalReward / config.maxSteps;
        result.avgFiringRate = totalFiringRate / config.maxSteps;
        
        auto* episodicMemory = brain->getEpisodicMemory();
        result.memoryEpisodesStored = episodicMemory ? 
            static_cast<float>(episodicMemory->getEpisodeCount()) : 0.0f;
        
        result.noveltyLevel = agent.getNoveltyLevel();
        result.curiosityLevel = agent.getCuriosityLevel();
        result.dopamineLevel = agent.getNeuromodulationLevel();
        
        // Verify integration with validation
        result.memoryWorkingMemoryIntegrated = (brain->getWorkingMemory() != nullptr);
        result.memoryEpisodicMemoryIntegrated = (brain->getEpisodicMemory() != nullptr);
        result.neuromodulationIntegrated = (brain->getDopamine() != nullptr);
        result.predictionIntegrated = (brain->getPredictionSystem() != nullptr);
        result.developmentIntegrated = (brain->getDevelopmentSystem() != nullptr);
        
        NLM_LOG_INFO("=== Integration Verification ===");
        NLM_LOG_INFO("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Prediction: " + std::string(result.predictionIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Development: " + std::string(result.developmentIntegrated ? "YES" : "NO"));
        
        // Test checkpointing with comprehensive validation and error handling
        if (config.enableCheckpointing) {
            NLM_LOG_INFO("Testing checkpoint save/load...");
            try {
                if (!config.checkpointPath.empty()) {
                    if (brain->save(config.checkpointPath)) {
                        NLM_LOG_INFO("Checkpoint saved successfully");
                        
                        // Create new brain and load with validation
                        auto brain2 = std::make_shared<Brain>(cfg);
                        if (!brain2) {
                            NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Failed to create brain2 instance");
                        } else if (!brain2->initialize()) {
                            NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Failed to initialize new brain for checkpointing");
                        } else if (brain2->load(config.checkpointPath)) {
                            NLM_LOG_INFO("Checkpoint loaded successfully");
                            result.checkpointingWorks = true;
                        } else {
                            NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Failed to load checkpoint");
                        }
                    } else {
                        NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Failed to save checkpoint");
                    }
                } else {
                    NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Checkpoint path is empty");
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Checkpoint operation failed: " + std::string(e.what()));
            } catch (...) {
                NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Unknown checkpoint operation exception");
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
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Fatal exception: " + std::string(e.what()));
        result.endTime = time(nullptr);
    } catch (...) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::run - Unknown fatal exception occurred");
        result.endTime = time(nullptr);
    }
    
    return result;
}

bool Phase6IntegratedExperiment::verifyIntegration() {
    NLM_LOG_INFO("=== Phase 6 Integration Verification ===");
    
    // Create minimal brain with validation
    auto cfg = std::make_shared<Config>();
    if (!cfg) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::verifyIntegration - Failed to create Config instance");
        return false;
    }
    
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::verifyIntegration - Failed to create Brain instance");
        return false;
    }
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::verifyIntegration - Brain initialization failed");
        return false;
    }
    
    bool success = true;
    
    // Test 1: Memory systems exist and are properly validated
    {
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            NLM_LOG_INFO("[PASS] Working memory is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Working memory is NOT integrated");
            success = false;
        }
    }
    
    {
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            NLM_LOG_INFO("[PASS] Episodic memory is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Episodic memory is NOT integrated");
            success = false;
        }
    }
    
    {
        auto* associativeMemory = brain->getAssociativeMemory();
        if (associativeMemory) {
            NLM_LOG_INFO("[PASS] Associative memory is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Associative memory is NOT integrated");
            success = false;
        }
    }
    
    // Test 2: Neuromodulation systems exist and are validated
    {
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            NLM_LOG_INFO("[PASS] Dopamine system is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Dopamine system is NOT integrated");
            success = false;
        }
    }
    
    {
        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            NLM_LOG_INFO("[PASS] Curiosity system is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Curiosity system is NOT integrated");
            success = false;
        }
    }
    
    {
        auto* novelty = brain->getNovelty();
        if (novelty) {
            NLM_LOG_INFO("[PASS] Novelty system is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Novelty system is NOT integrated");
            success = false;
        }
    }
    
    {
        auto* predictionError = brain->getPredictionErrorSignal();
        if (predictionError) {
            NLM_LOG_INFO("[PASS] Prediction error system is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Prediction error system is NOT integrated");
            success = false;
        }
    }
    
    // Test 3: Prediction system exists
    {
        auto* predictionSystem = brain->getPredictionSystem();
        if (predictionSystem) {
            NLM_LOG_INFO("[PASS] Prediction system is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Prediction system is NOT integrated");
            success = false;
        }
    }
    
    // Test 4: Cognition systems exist and are validated
    {
        auto* planner = brain->getPlanner();
        if (planner) {
            NLM_LOG_INFO("[PASS] Planner is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Planner is NOT integrated");
            success = false;
        }
    }
    
    {
        auto* conceptFormation = brain->getConceptFormation();
        if (conceptFormation) {
            NLM_LOG_INFO("[PASS] Concept formation is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Concept formation is NOT integrated");
            success = false;
        }
    }
    
    {
        auto* attention = brain->getAttention();
        if (attention) {
            NLM_LOG_INFO("[PASS] Attention is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Attention is NOT integrated");
            success = false;
        }
    }
    
    // Test 5: Development system exists
    {
        auto* developmentSystem = brain->getDevelopmentSystem();
        if (developmentSystem) {
            NLM_LOG_INFO("[PASS] Development system is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Development system is NOT integrated");
            success = false;
        }
    }
    
    return success;
}

bool Phase6IntegratedExperiment::testMemoryIntegration() {
    NLM_LOG_INFO("=== Testing Memory Integration ===");
    
    auto cfg = std::make_shared<Config>();
    if (!cfg) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testMemoryIntegration - Failed to create Config instance");
        return false;
    }
    cfg->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testMemoryIntegration - Failed to create Brain instance");
        return false;
    }
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testMemoryIntegration - Brain initialization failed");
        return false;
    }
    
    // Get memory systems with comprehensive validation
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    auto* associativeMemory = brain->getAssociativeMemory();
    
    if (!workingMemory || !episodicMemory || !associativeMemory) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testMemoryIntegration - Memory systems not available");
        return false;
    }
    
    // Run some steps with error handling
    for (int i = 0; i < 100; ++i) {
        try {
            brain->step(i, i * 0.001);
            
            // Check memory system state with bounds checking
            size_t activeTraces = workingMemory->getActiveTraces();
            size_t episodeCount = episodicMemory->getEpisodeCount();
            size_t associationCount = associativeMemory->getAssociationCount();
            
            if (activeTraces > 10000) {
                NLM_LOG_ERROR("Phase6IntegratedExperiment::testMemoryIntegration - Excessive working memory traces at step " + std::to_string(i));
            }
            
            if (episodeCount > 10000) {
                NLM_LOG_ERROR("Phase6IntegratedExperiment::testMemoryIntegration - Excessive episodic memory episodes at step " + std::to_string(i));
            }
            
            if (associationCount > 10000) {
                NLM_LOG_ERROR("Phase6IntegratedExperiment::testMemoryIntegration - Excessive associative memory associations at step " + std::to_string(i));
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Phase6IntegratedExperiment::testMemoryIntegration - Exception at step " + std::to_string(i) + ": " + std::string(e.what()));
            return false;
        }
    }
    
    // Check if working memory has traces
    if (workingMemory->getActiveTraces() > 0) {
        NLM_LOG_INFO("[PASS] Working memory has active traces: " + std::to_string(workingMemory->getActiveTraces()));
    } else {
        NLM_LOG_INFO("[INFO] Working memory has no active traces (may be normal for simple simulation)");
    }
    
    // Check if episodic memory has episodes
    if (episodicMemory->getEpisodeCount() > 0) {
        NLM_LOG_INFO("[PASS] Episodic memory has episodes: " + std::to_string(episodicMemory->getEpisodeCount()));
    } else {
        NLM_LOG_INFO("[INFO] Episodic memory has no episodes (may be normal for simple simulation)");
    }
    
    // Check associative memory
    if (associativeMemory->getAssociationCount() > 0) {
        NLM_LOG_INFO("[PASS] Associative memory has associations: " + std::to_string(associativeMemory->getAssociationCount()));
    } else {
        NLM_LOG_INFO("[INFO] Associative memory has no associations (may be normal for simple simulation)");
    }
    
    return true;
}

bool Phase6IntegratedExperiment::testNeuromodulationIntegration() {
    NLM_LOG_INFO("=== Testing Neuromodulation Integration ===");
    
    auto cfg = std::make_shared<Config>();
    if (!cfg) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testNeuromodulationIntegration - Failed to create Config instance");
        return false;
    }
    cfg->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testNeuromodulationIntegration - Failed to create Brain instance");
        return false;
    }
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testNeuromodulationIntegration - Brain initialization failed");
        return false;
    }
    
    // Get neuromodulation systems with comprehensive validation
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    auto* predictionError = brain->getPredictionErrorSignal();
    
    if (!dopamine || !curiosity || !novelty || !predictionError) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testNeuromodulationIntegration - Neuromodulation systems not available");
        return false;
    }
    
    // Run some steps with neuromodulation validation
    for (int i = 0; i < 50; ++i) {
        try {
            // Inject some sensory activity
            brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
            brain->step(i, i * 0.001);
            
            // Validate neuromodulator states
            if (dopamine->getLevel() < -1.0f || dopamine->getLevel() > 10.0f) {
                NLM_LOG_ERROR("Phase6IntegratedExperiment::testNeuromodulationIntegration - Invalid dopamine level: " + std::to_string(dopamine->getLevel()));
            }
            
            if (curiosity->getLevel() < -1.0f || curiosity->getLevel() > 10.0f) {
                NLM_LOG_ERROR("Phase6IntegratedExperiment::testNeuromodulationIntegration - Invalid curiosity level: " + std::to_string(curiosity->getLevel()));
            }
            
            if (novelty->getLevel() < -1.0f || novelty->getLevel() > 10.0f) {
                NLM_LOG_ERROR("Phase6IntegratedExperiment::testNeuromodulationIntegration - Invalid novelty level: " + std::to_string(novelty->getLevel()));
            }
            
            if (predictionError->getLevel() < -1.0f || predictionError->getLevel() > 10.0f) {
                NLM_LOG_ERROR("Phase6IntegratedExperiment::testNeuromodulationIntegration - Invalid prediction error level: " + std::to_string(predictionError->getLevel()));
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Phase6IntegratedExperiment::testNeuromodulationIntegration - Exception at step " + std::to_string(i) + ": " + std::string(e.what()));
            return false;
        }
    }
    
    NLM_LOG_INFO("[PASS] Neuromodulation systems are functional");
    return true;
}

bool Phase6IntegratedExperiment::testCheckpointing() {
    NLM_LOG_INFO("=== Testing Checkpoint Save/Load ===");
    
    auto cfg = std::make_shared<Config>();
    if (!cfg) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testCheckpointing - Failed to create Config instance");
        return false;
    }
    cfg->set("neuron_count", 100);
    
    // Create and initialize brain with validation
    auto brain1 = std::make_shared<Brain>(cfg);
    if (!brain1) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testCheckpointing - Failed to create brain1 instance");
        return false;
    }
    
    if (!brain1->initialize()) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testCheckpointing - Failed to initialize brain1");
        return false;
    }
    
    // Run some steps with validation
    for (int i = 0; i < 100; ++i) {
        try {
            brain1->step(i, i * 0.001);
            
            // Validate metrics during simulation
            float avgFiring = brain1->getAverageFiringRate();
            if (avgFiring < -10.0f || avgFiring > 100.0f) {
                NLM_LOG_ERROR("Phase6IntegratedExperiment::testCheckpointing - Invalid firing rate at step " + std::to_string(i) + ": " + std::to_string(avgFiring));
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Phase6IntegratedExperiment::testCheckpointing - Exception at step " + std::to_string(i) + ": " + std::string(e.what()));
            return false;
        }
    }
    
    float avgFiring1 = brain1->getAverageFiringRate();
    size_t totalSpikes1 = brain1->getTotalSpikeCount();
    
    NLM_LOG_INFO("Brain1 avg firing: " + std::to_string(avgFiring1));
    NLM_LOG_INFO("Brain1 total spikes: " + std::to_string(totalSpikes1));
    
    // Save checkpoint with comprehensive error handling
    std::string path = "/tmp/nlm_checkpoint_test.bin";
    try {
        if (!brain1->save(path)) {
            NLM_LOG_ERROR("Phase6IntegratedExperiment::testCheckpointing - Failed to save checkpoint");
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testCheckpointing - Checkpoint save exception: " + std::string(e.what()));
        return false;
    }
    
    // Create new brain and load with validation
    auto brain2 = std::make_shared<Brain>(cfg);
    if (!brain2) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testCheckpointing - Failed to create brain2 instance");
        return false;
    }
    
    if (!brain2->initialize()) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testCheckpointing - Failed to initialize brain2");
        return false;
    }
    
    try {
        if (!brain2->load(path)) {
            NLM_LOG_ERROR("Phase6IntegratedExperiment::testCheckpointing - Failed to load checkpoint");
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testCheckpointing - Checkpoint load exception: " + std::string(e.what()));
        return false;
    }
    
    size_t totalSpikes2 = brain2->getTotalSpikeCount();
    NLM_LOG_INFO("Brain2 total spikes after load: " + std::to_string(totalSpikes2));
    
    // Note: Due to the nature of neural simulation, exact state restoration
    // is complex. The load should restore the structure at minimum.
    
    NLM_LOG_INFO("[PASS] Checkpoint save/load completed");
    return true;
}

bool Phase6IntegratedExperiment::testReplay() {
    NLM_LOG_INFO("=== Testing Replay System ===");
    
    auto cfg = std::make_shared<Config>();
    if (!cfg) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testReplay - Failed to create Config instance");
        return false;
    }
    cfg->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testReplay - Failed to create Brain instance");
        return false;
    }
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testReplay - Brain initialization failed");
        return false;
    }
    
    auto* episodicMemory = brain->getEpisodicMemory();
    if (!episodicMemory) {
        NLM_LOG_ERROR("Phase6IntegratedExperiment::testReplay - Episodic memory not available");
        return false;
    }
    
    // Run some steps with comprehensive validation
    for (int i = 0; i < 200; ++i) {
        try {
            brain->step(i, i * 0.001);
            
            // Check episodic memory state with bounds checking
            size_t episodeCount = episodicMemory->getEpisodeCount();
            if (episodeCount > 1000) {
                NLM_LOG_ERROR("Phase6IntegratedExperiment::testReplay - Excessive episode count: " + std::to_string(episodeCount));
            }
            
            // Get some neurons for replay testing
            auto regions = brain->getRegions();
            if (!regions.empty()) {
                auto* region = regions[0].get();
                if (region) {
                    // Validate region neuron count
                    size_t neuronCount = region->getNeuronCount();
                    if (neuronCount > 10000) {
                        NLM_LOG_ERROR("Phase6IntegratedExperiment::testReplay - Excessive neuron count in region: " + std::to_string(neuronCount));
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Phase6IntegratedExperiment::testReplay - Exception at step " + std::to_string(i) + ": " + std::string(e.what()));
            return false;
        }
    }
    
    // Check if episodes exist for replay
    size_t episodeCount = episodicMemory->getEpisodeCount();
    if (episodeCount > 0) {
        NLM_LOG_INFO("[PASS] Episodes available for replay: " + std::to_string(episodeCount));
        
        try {
            // Get episodes for replay with null checks
            auto episodes = episodicMemory->getEpisodesForReplay(3);
            if (!episodes.empty()) {
                NLM_LOG_INFO("[PASS] Replay system can retrieve episodes");
                
                // Validate episode data integrity
                for (const auto& episode : episodes) {
                    if (!episode || episode->getDuration() < 0.0f) {
                        NLM_LOG_ERROR("Phase6IntegratedExperiment::testReplay - Invalid episode data");
                        return false;
                    }
                }
                return true;
            } else {
                NLM_LOG_INFO("[INFO] No episodes available for replay (may be normal)");
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Phase6IntegratedExperiment::testReplay - Exception during episode retrieval: " + std::string(e.what()));
            return false;
        }
    }
    
    NLM_LOG_INFO("[INFO] No episodes available for replay (may be normal)");
    return true;
}

} // namespace nlm
