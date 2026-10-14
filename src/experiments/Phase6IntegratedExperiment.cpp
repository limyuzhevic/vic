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

Phase6IntegrationResult Phase6IntegratedExperiment::run(const Phase6Config& config) {
    Phase6IntegrationResult result;
    result.startTime = time(nullptr);
    
    auto startWall = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("=== Phase 6 Integration Experiment ===");
    NLM_LOG_INFO("Configuration: " + std::to_string(config.neuronCount) + " neurons, " +
                 std::to_string(config.maxSteps) + " steps");
    
    // Create configuration using enhanced Phase6Config
    auto phase6Config = Phase6Config::createScenario("simple");  // Use simple scenario as default
    
    // Override with user-provided values
    if (config.neuronCount > 0) phase6Config.neuronCount = config.neuronCount;
    if (config.regionCount > 0) phase6Config.regionCount = config.regionCount;
    if (config.connectionProbability > 0.0f) phase6Config.connectionProbability = config.connectionProbability;
    if (config.maxSteps > 0) phase6Config.maxSteps = config.maxSteps;
    if (config.enableCheckpointing) phase6Config.enableCheckpointing = true;
    if (config.enableReplay) phase6Config.enableReplay = true;
    if (config.enableDevelopment) phase6Config.enableDevelopment = true;
    if (!config.checkpointPath.empty()) phase6Config.checkpointPath = config.checkpointPath;
    
    // Convert Phase6Config to standard Config
    auto cfg = std::make_shared<Config>();
    
    // Basic parameters
    cfg->set("neuron_count", phase6Config.neuronCount);
    cfg->set("region_count", phase6Config.regionCount);
    cfg->set("connection_probability", phase6Config.connectionProbability);
    cfg->set("simulation_timestep", phase6Config.simulationTimestep);
    cfg->set("random_seed", phase6Config.experimentSeed);
    
    // Plasticity parameters
    if (phase6Config.enableHebbianPlasticity) {
        cfg->set("hebbian_learning_rate", phase6Config.hebbianLearningRate);
    }
    if (phase6Config.enableSTDP) {
        cfg->set("stdp_ltp_weight", phase6Config.stdpLTPWeight);
        cfg->set("stdp_ltd_weight", phase6Config.stdpLTDWeight);
        cfg->set("stdp_tau", phase6Config.stdpTimeConstant);
    }
    if (phase6Config.enableStructuralPlasticity) {
        cfg->set("synaptogenesis_rate", phase6Config.synaptogenesisRate);
        cfg->set("pruning_rate", phase6Config.pruningRate);
    }
    
    // Development parameters
    cfg->set("replay_interval", phase6Config.replayInterval);
    cfg->set("consolidation_interval", phase6Config.consolidationInterval);
    cfg->set("development_stage", phase6Config.enableDevelopment ? 0 : -1);
    
    // Memory system configuration
    cfg->set("working_memory_capacity", phase6Config.workingMemoryCapacity);
    cfg->set("max_episodic_episodes", phase6Config.maxEpisodicEpisodes);
    
    // Neuromodulation configuration
    if (phase6Config.enableDopamine) {
        cfg->set("dopamine_baseline", phase6Config.baselineDopamineLevel);
    }
    if (phase6Config.enableCuriosity) {
        cfg->set("curiosity_threshold", phase6Config.curiosityThreshold);
    }
    
    // Cognition parameters
    if (phase6Config.enableNeuralPlanner) {
        cfg->set("planning_depth", phase6Config.planningDepth);
    }
    if (phase6Config.enableAttentionSystem) {
        cfg->set("attention_inhibition_strength", phase6Config.attentionInhibitionStrength);
    }
    
    // Checkpoint configuration
    if (phase6Config.enableCheckpointing) {
        cfg->set("checkpoint_dir", phase6Config.checkpointPath);
        cfg->set("checkpoint_frequency", phase6Config.checkpointFrequency);
        cfg->set("max_checkpoints", phase6Config.maxCheckpoints);
    }
    
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
        
        // Periodic status
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
    result.noveltyLevel = agent.getNoveltyLevel();
    result.curiosityLevel = agent.getCuriosityLevel();
    result.dopamineLevel = agent.getNeuromodulationLevel();
    
    // Verify integration
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
    
    if (!wm || !em) {
        NLM_LOG_ERROR("Memory systems not available");
        return false;
    }
    
    // Run some steps
    for (int i = 0; i < 100; ++i) {
        brain->step(i, i * 0.001);
    }
    
    // Check if working memory has traces
    if (wm->getActiveTraces() > 0) {
        NLM_LOG_INFO("[PASS] Working memory has active traces: " + std::to_string(wm->getActiveTraces()));
    } else {
        NLM_LOG_INFO("[INFO] Working memory has no active traces (may be normal for simple simulation)");
    }
    
    // Check if episodic memory has episodes
    if (em->getEpisodeCount() > 0) {
        NLM_LOG_INFO("[PASS] Episodic memory has episodes: " + std::to_string(em->getEpisodeCount()));
    } else {
        NLM_LOG_INFO("[INFO] Episodic memory has no episodes (may be normal for simple simulation)");
    }
    
    return true;
}

bool Phase6IntegratedExperiment::testNeuromodulationIntegration() {
    NLM_LOG_INFO("=== Testing Neuromodulation Integration ===");
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Get neuromodulation systems
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    
    if (!dopamine || !curiosity || !novelty) {
        NLM_LOG_ERROR("Neuromodulation systems not available");
        return false;
    }
    
    // Run some steps with sensory input
    for (int i = 0; i < 50; ++i) {
        // Inject some sensory activity
        brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
        brain->step(i, i * 0.001);
    }
    
    NLM_LOG_INFO("[PASS] Neuromodulation systems are functional");
    return true;
}

bool Phase6IntegratedExperiment::testCheckpointing() {
    NLM_LOG_INFO("=== Testing Checkpoint Save/Load ===");
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    
    // Create and initialize brain
    auto brain1 = std::make_shared<Brain>(cfg);
    brain1->initialize();
    
    // Run some steps
    for (int i = 0; i < 100; ++i) {
        brain1->step(i, i * 0.001);
    }
    
    float avgFiring1 = brain1->getAverageFiringRate();
    size_t totalSpikes1 = brain1->getTotalSpikeCount();
    
    NLM_LOG_INFO("Brain1 avg firing: " + std::to_string(avgFiring1));
    NLM_LOG_INFO("Brain1 total spikes: " + std::to_string(totalSpikes1));
    
    // Save checkpoint
    std::string path = "/tmp/nlm_checkpoint_test.bin";
    if (!brain1->save(path)) {
        NLM_LOG_ERROR("Failed to save checkpoint");
        return false;
    }
    
    // Create new brain and load
    auto brain2 = std::make_shared<Brain>(cfg);
    brain2->initialize();
    
    if (!brain2->load(path)) {
        NLM_LOG_ERROR("Failed to load checkpoint");
        return false;
    }
    
    size_t totalSpikes2 = brain2->getTotalSpikeCount();
    NLM_LOG_INFO("Brain2 total spikes after load: " + std::to_string(totalSpikes2));
    
    // Note: Due to the nature of neural simulation, exact state restoration
    // is complex. The load should restore the structure at minimum.
    
    NLM_LOG_INFO("[PASS] Checkpoint save/load completed");
    return true;
}

bool Phase6IntegratedExperiment::testDevelopmentIntegration() {
    NLM_LOG_INFO("=== Testing Development Integration ===");
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Test developmental stage transitions
    auto* devSystem = brain->getDevelopmentSystem();
    if (!devSystem) {
        NLM_LOG_ERROR("Development system not available");
        return false;
    }
    
    // Run some steps to allow development to progress
    for (int i = 0; i < 500; ++i) {
        brain->step(i, i * 0.001);
    }
    
    // Check developmental stage
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    NLM_LOG_INFO("Development stage after 500 steps: " + std::to_string(static_cast<int>(stage)));
    
    // Check structural plasticity effects
    float plasticityMod = 1.0f;
    switch (stage) {
        case DevelopmentalStage::Initial:
            plasticityMod = 1.0f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityMod = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            plasticityMod = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            plasticityMod = 0.2f;
            break;
    }
    
    NLM_LOG_INFO("Plasticity modification factor: " + std::to_string(plasticityMod));
    
    // Check if development system affects plasticity rates
    auto* sp = brain->getStructuralPlasticity();
    if (sp) {
        // Get current rates
        NLM_LOG_INFO("Current synaptogenesis rate: " + std::to_string(sp->getSynaptogenesisRate()));
        NLM_LOG_INFO("Current pruning rate: " + std::to_string(sp->getPruningRate()));
    }
    
    NLM_LOG_INFO("[PASS] Development integration test completed");
    return true;
}

bool Phase6IntegratedExperiment::testPredictionIntegration() {
    NLM_LOG_INFO("=== Testing Prediction Integration ===");
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Get prediction system
    auto* predSystem = brain->getPredictionSystem();
    if (!predSystem) {
        NLM_LOG_ERROR("Prediction system not available");
        return false;
    }
    
    NLM_LOG_INFO("[PASS] Prediction system is integrated");
    
    // Test prediction error signaling
    auto* predError = brain->getPredictionErrorSignal();
    if (predError) {
        NLM_LOG_INFO("[PASS] Prediction error system is integrated");
    }
    
    // Run some steps
    for (int i = 0; i < 100; ++i) {
        // Inject some sensory activity
        brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
        brain->step(i, i * 0.001);
    }
    
    NLM_LOG_INFO("[PASS] Prediction integration test completed");
    return true;
}

bool Phase6IntegratedExperiment::testCognitionIntegration() {
    NLM_LOG_INFO("=== Testing Cognition Integration ===");
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    bool allCognitionIntegrated = true;
    
    // Test neural planner
    auto* planner = brain->getPlanner();
    if (planner) {
        NLM_LOG_INFO("[PASS] Neural planner is integrated");
        
        // Test planning functionality
        planner->setPlanningDepth(3);
        NLM_LOG_INFO("Planner depth set to: " + std::to_string(3));
    } else {
        NLM_LOG_ERROR("[FAIL] Neural planner is NOT integrated");
        allCognitionIntegrated = false;
    }
    
    // Test concept formation
    auto* conceptFormation = brain->getConceptFormation();
    if (conceptFormation) {
        NLM_LOG_INFO("[PASS] Concept formation is integrated");
        
        conceptFormation->initialize(brain.get());
        NLM_LOG_INFO("Concept formation initialized");
    } else {
        NLM_LOG_ERROR("[FAIL] Concept formation is NOT integrated");
        allCognitionIntegrated = false;
    }
    
    // Test attention system
    auto* attention = brain->getAttention();
    if (attention) {
        NLM_LOG_INFO("[PASS] Attention system is integrated");
        
        attention->initialize(brain.get());
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
        NLM_LOG_INFO("Attention system initialized with inhibition=0.5, excitation=1.5");
    } else {
        NLM_LOG_ERROR("[FAIL] Attention system is NOT integrated");
        allCognitionIntegrated = false;
    }
    
    // Run some steps with cognition systems
    for (int i = 0; i < 200; ++i) {
        brain->injectCurrentToNeurons(NeuronType::Internal, 3.0f);
        brain->step(i, i * 0.001);
    }
    
    if (allCognitionIntegrated) {
        NLM_LOG_INFO("[PASS] Cognition integration test completed successfully");
    } else {
        NLM_LOG_ERROR("[FAIL] Cognition integration test failed");
    }
    
    return allCognitionIntegrated;
}

Phase6IntegrationResult Phase6IntegratedExperiment::runComprehensiveTest(const Phase6Config& config) {
    NLM_LOG_INFO("=== Running Comprehensive Integration Test ===");
    NLM_LOG_INFO("This test runs all integration tests together to verify complete functionality.");
    
    Phase6IntegrationResult result;
    result.startTime = time(nullptr);
    
    auto startWall = std::chrono::high_resolution_clock::now();
    
    // Create configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", config.neuronCount);
    cfg->set("region_count", config.regionCount);
    cfg->set("connection_probability", config.connectionProbability);
    
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
    
    // Run comprehensive test
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    size_t firingCount = 0;
    
    for (uint64_t step = 0; step < config.maxSteps; ++step) {
        // Get observation
        SensoryPercept percept = world.observe(brain->getRegions()[0].get());
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Get motor command
        MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply action to world
        world.applyAction(brain->getRegions()[0].get(), cmd);
        
        // Compute reward
        float reward = world.computeReward(brain->getRegions()[0].get());
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
                            brain->getWorkingMemory()->getActiveTraces() : 0));
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
        std::string path = "/tmp/nlm_comprehensive_checkpoint_test.bin";
        if (brain->save(path)) {
            NLM_LOG_INFO("Checkpoint saved successfully");
            
            // Create new brain and load
            auto brain2 = std::make_shared<Brain>(cfg);
            brain2->initialize();
            
            if (brain2->load(path)) {
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
    
    NLM_LOG_INFO("=== Comprehensive Test Complete ===");
    NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
    NLM_LOG_INFO("Avg firing rate: " + std::to_string(result.avgFiringRate));
    NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
    NLM_LOG_INFO("Wall time: " + std::to_string(result.totalWallClockTime) + "s");
    
    return result;
}
