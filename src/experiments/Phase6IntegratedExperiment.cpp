#include "Phase6IntegratedExperiment.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include <chrono>
#include <cmath>

namespace nlm {

Phase6IntegratedExperiment::Phase6IntegratedExperiment() {}

Phase6IntegratedExperiment::~Phase6IntegratedExperiment() = default;

// Original run() method (kept for backward compatibility)
Phase6IntegrationResult Phase6IntegratedExperiment::run(const Phase6Config& config) {
    Phase6IntegrationResult result;
    result.startTime = time(nullptr);
    
    auto startWall = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("=== Phase 6 Integration Experiment ===");
    NLM_LOG_INFO("Configuration: " + std::to_string(config.neuronCount) + " neurons, " +
                 std::to_string(config.maxSteps) + " steps");
    
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
    agent.enableEpisodicRecording(true);
    
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
        
        // Periodic status
        if (step % 1000 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Reward: " + std::to_string(totalReward / (step + 1)) +
                        " | Firing: " + std::to_string(brain->getAverageFiringRate()) +
                        " | WorkingMem: " + std::to_string(brain->getWorkingMemory() ? 
                            brain->getWorkingMemory()->getActiveTraces() : 0) +
                        " | Episodes: " + std::to_string(brain->getEpisodicMemory() ? 
                            brain->getEpisodicMemory()->getEpisodeCount() : 0));
        }
    }
    
    // Collect final metrics
    result.totalReward = totalReward / config.maxSteps;
    result.avgFiringRate = totalFiringRate / config.maxSteps;
    result.memoryEpisodesStored = brain->getEpisodicMemory() ? 
        static_cast<float>(brain->getEpisodicMemory()->getEpisodeCount()) : 0.0f;
    result.noveltyLevel = agent.getNoveltyLevel();
    result.curiosityLevel = agent.getCuriosityLevel();
    result.dopamineLevel = agent.getNeuromodulationLevel();
    
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
    
    NLM_LOG_INFO("=== Integration Verification ===");
    NLM_LOG_INFO("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Prediction: " + std::string(result.predictionIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Development: " + std::string(result.developmentIntegrated ? "YES" : "NO"));
    
    return result;
}

bool Phase6IntegratedExperiment::verifyIntegration() {
    NLM_LOG_INFO("=== Phase 6 Integration Verification ===");
    
    // Create minimal brain
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed");
        return false;
    }
    
    bool success = true;
    
    // Test 1: Memory systems exist and are connected
    if (brain->getWorkingMemory() != nullptr) {
        NLM_LOG_INFO("[PASS] Working memory is integrated");
    } else {
        NLM_LOG_ERROR("[FAIL] Working memory is NOT integrated");
        success = false;
    }
    
    if (brain->getEpisodicMemory() != nullptr) {
        NLM_LOG_INFO("[PASS] Episodic memory is integrated");
    } else {
        NLM_LOG_ERROR("[FAIL] Episodic memory is NOT integrated");
        success = false;
    }
    
    // Test 2: Neuromodulation systems exist
    if (brain->getDopamine() != nullptr) {
        NLM_LOG_INFO("[PASS] Dopamine system is integrated");
    } else {
        NLM_LOG_ERROR("[FAIL] Dopamine system is NOT integrated");
        success = false;
    }
    
    if (brain->getCuriosity() != nullptr) {
        NLM_LOG_INFO("[PASS] Curiosity system is integrated");
    } else {
        NLM_LOG_ERROR("[FAIL] Curiosity system is NOT integrated");
        success = false;
    }
    
    // Test 3: Prediction system exists
    if (brain->getPredictionSystem() != nullptr) {
        NLM_LOG_INFO("[PASS] Prediction system is integrated");
    } else {
        NLM_LOG_ERROR("[FAIL] Prediction system is NOT integrated");
        success = false;
    }
    
    // Test 4: Cognition systems exist
    if (brain->getPlanner() != nullptr) {
        NLM_LOG_INFO("[PASS] Planner is integrated");
    } else {
        NLM_LOG_ERROR("[FAIL] Planner is NOT integrated");
        success = false;
    }
    
    if (brain->getConceptFormation() != nullptr) {
        NLM_LOG_INFO("[PASS] Concept formation is integrated");
    } else {
        NLM_LOG_ERROR("[FAIL] Concept formation is NOT integrated");
        success = false;
    }
    
    if (brain->getAttention() != nullptr) {
        NLM_LOG_INFO("[PASS] Attention is integrated");
    } else {
        NLM_LOG_ERROR("[FAIL] Attention is NOT integrated");
        success = false;
    }
    
    // Test 5: Development system exists
    if (brain->getDevelopmentSystem() != nullptr) {
        NLM_LOG_INFO("[PASS] Development system is integrated");
    } else {
        NLM_LOG_ERROR("[FAIL] Development system is NOT integrated");
        success = false;
    }
    
    return success;
}

bool Phase6IntegratedExperiment::testMemoryIntegration() {
    NLM_LOG_INFO("=== Testing Memory Integration ===");
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Get memory systems
    auto* wm = brain->getWorkingMemory();
    auto* em = brain->getEpisodicMemory();
    
    bool success = true;
    
    if (wm != nullptr) {
        NLM_LOG_INFO("[PASS] Working memory exists and can be accessed");
        
        // Test working memory functionality
        wm->initialize(brain.get());
        wm->store(std::vector<float>{0.1f, 0.2f, 0.3f}, 1.0f);
        
        auto retrieved = wm->retrieve();
        if (retrieved.size() == 3) {
            NLM_LOG_INFO("[PASS] Working memory storage/retrieval works");
        } else {
            NLM_LOG_ERROR("[FAIL] Working memory storage/retrieval failed");
            success = false;
        }
    } else {
        NLM_LOG_ERROR("[FAIL] Working memory does not exist");
        success = false;
    }
    
    if (em != nullptr) {
        NLM_LOG_INFO("[PASS] Episodic memory exists and can be accessed");
        
        // Test episodic memory functionality
        em->initialize(brain.get());
        
        // Create and store an episode
        EpisodicMemoryItem episode;
        episode.timestamp = 0;
        episode.reward = 1.0f;
        episode.action = ActionType::Wait;
        
        em->storeEpisode(episode);
        
        auto episodes = em->getRecentEpisodes(10);
        if (!episodes.empty()) {
            NLM_LOG_INFO("[PASS] Episodic memory storage/retrieval works");
        } else {
            NLM_LOG_ERROR("[FAIL] Episodic memory storage/retrieval failed");
            success = false;
        }
    } else {
        NLM_LOG_ERROR("[FAIL] Episodic memory does not exist");
        success = false;
    }
    
    return success;
}

bool Phase6IntegratedExperiment::testNeuromodulationIntegration() {
    NLM_LOG_INFO("=== Testing Neuromodulation Integration ===");
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    bool success = true;
    
    // Test dopamine system
    auto* dopamine = brain->getDopamine();
    if (dopamine != nullptr) {
        NLM_LOG_INFO("[PASS] Dopamine system exists");
        
        // Test dopamine functionality
        dopamine->signalReward(1.0f);
        if (dopamine->getLevel() > 0.0f) {
            NLM_LOG_INFO("[PASS] Dopamine signaling works");
        } else {
            NLM_LOG_ERROR("[FAIL] Dopamine signaling failed");
            success = false;
        }
    } else {
        NLM_LOG_ERROR("[FAIL] Dopamine system does not exist");
        success = false;
    }
    
    // Test curiosity system
    auto* curiosity = brain->getCuriosity();
    if (curiosity != nullptr) {
        NLM_LOG_INFO("[PASS] Curiosity system exists");
    } else {
        NLM_LOG_ERROR("[FAIL] Curiosity system does not exist");
        success = false;
    }
    
    // Test novelty system
    auto* novelty = brain->getNovelty();
    if (novelty != nullptr) {
        NLM_LOG_INFO("[PASS] Novelty system exists");
    } else {
        NLM_LOG_ERROR("[FAIL] Novelty system does not exist");
        success = false;
    }
    
    return success;
}

bool Phase6IntegratedExperiment::testCheckpointing() {
    NLM_LOG_INFO("=== Testing Checkpointing ===");
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 50);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    const std::string checkpointFile = "./test_checkpoint.bin";
    
    // Test saving
    if (brain->save(checkpointFile)) {
        NLM_LOG_INFO("[PASS] Checkpoint save works");
        
        // Test loading
        auto brain2 = std::make_shared<Brain>(cfg);
        brain2->initialize();
        
        if (brain2->load(checkpointFile)) {
            NLM_LOG_INFO("[PASS] Checkpoint load works");
            return true;
        } else {
            NLM_LOG_ERROR("[FAIL] Checkpoint load failed");
            return false;
        }
    } else {
        NLM_LOG_ERROR("[FAIL] Checkpoint save failed");
        return false;
    }
}

bool Phase6IntegratedExperiment::testReplay() {
    NLM_LOG_INFO("=== Testing Replay System ===");
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 50);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    if (brain->getEpisodicMemory() == nullptr) {
        NLM_LOG_ERROR("[FAIL] Episodic memory does not exist for replay testing");
        return false;
    }
    
    // Enable replay
    brain->getEpisodicMemory()->enableReplay(true);
    
    // Create and store episodes
    for (int i = 0; i < 5; ++i) {
        EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.reward = 0.5f + (i * 0.1f);
        episode.action = ActionType::Wait;
        
        brain->getEpisodicMemory()->storeEpisode(episode);
    }
    
    // Test replay
    auto episodes = brain->getEpisodicMemory()->getEpisodesForReplay(3);
    if (episodes.size() >= 3) {
        NLM_LOG_INFO("[PASS] Replay system works");
        
        // Actually replay some episodes
        brain->getEpisodicMemory()->replaySequence({0, 1, 2});
        NLM_LOG_INFO("[PASS] Replay execution works");
        return true;
    } else {
        NLM_LOG_ERROR("[FAIL] Replay system failed to get episodes");
        return false;
    }
}

} // namespace nlm
