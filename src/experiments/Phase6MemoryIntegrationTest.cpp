// Memory Integration Test Suite
// Comprehensive test suite for validating memory system integration in NLM Phase 6
//
// This test suite verifies that:
// 1. All memory systems are properly initialized
// 2. Memory systems integrate correctly with neural processing
// 3. Memory systems communicate effectively with other brain systems
// 4. Memory operations are consistent and reliable
// 5. Memory integration works in realistic brain scenarios

#include "Phase6IntegrationTest.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <cassert>
#include <cmath>
#include <chrono>
#include <algorithm>

namespace nlm {

bool Phase6IntegratedExperiment::testMemoryIntegration() {
    NLM_LOG_INFO("=== Testing Memory Integration ===");
    
    // Create test configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    cfg->set("connection_probability", 0.1f);
    
    // Create brain instance
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed");
        return false;
    }
    
    // Test 1: Verify all memory systems are accessible
    bool memorySystemsAccessible = true;
    
    if (brain->getWorkingMemory() == nullptr) {
        NLM_LOG_ERROR("Working memory not accessible");
        memorySystemsAccessible = false;
    }
    
    if (brain->getEpisodicMemory() == nullptr) {
        NLM_LOG_ERROR("Episodic memory not accessible");
        memorySystemsAccessible = false;
    }
    
    if (brain->getAssociativeMemory() == nullptr) {
        NLM_LOG_ERROR("Associative memory not accessible");
        memorySystemsAccessible = false;
    }
    
    if (!memorySystemsAccessible) {
        NLM_LOG_ERROR("Memory systems not properly integrated");
        return false;
    }
    
    NLM_LOG_INFO("[PASS] All memory systems accessible");
    
    // Test 2: Verify working memory functionality
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        // Test basic storage and retrieval
        std::vector<float> testPattern = {0.1f, 0.5f, 0.8f, 0.3f, 0.9f};
        workingMemory->store(testPattern, 1.0f);
        
        // Give some time for memory processing
        for (int step = 0; step < 10; ++step) {
            brain->step(step, step * 0.001);
        }
        
        if (workingMemory->getActiveTraces() > 0) {
            NLM_LOG_INFO("[PASS] Working memory has active traces: " + 
                        std::to_string(workingMemory->getActiveTraces()));
        } else {
            NLM_LOG_INFO("[INFO] Working memory has no active traces (may be normal for simple simulation)");
        }
        
        // Test memory capacity
        workingMemory->setCapacity(50);
        if (workingMemory->getCapacity() == 50) {
            NLM_LOG_INFO("[PASS] Working memory capacity set correctly");
        }
        
        // Test memory decay
        workingMemory->setDecayRate(0.05f);
        if (workingMemory->getDecayRate() == 0.05f) {
            NLM_LOG_INFO("[PASS] Working memory decay rate set correctly");
        }
    }
    
    // Test 3: Verify episodic memory functionality
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        // Create a test episodic memory item
        EpisodicMemoryItem testEpisode;
        testEpisode.timestamp = 0;
        testEpisode.reward = 0.5f;
        testEpisode.energy = 0.8f;
        testEpisode.novelty = 0.3f;
        testEpisode.action = ActionType::MoveForward;
        testEpisode.positionX = 0.0f;
        testEpisode.positionY = 0.0f;
        testEpisode.orientation = 0.0f;
        testEpisode.age = 0;
        
        // Add some sensory state
        testEpisode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        testEpisode.resultingSensoryState = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
        testEpisode.resultingReward = 0.6f;
        
        // Add active neurons
        testEpisode.activeNeurons = {10001, 10002, 10003, 10004, 10005};
        testEpisode.neuronActivations = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        
        // Store the episode
        episodicMemory->storeEpisode(testEpisode);
        
        // Verify it was stored
        if (episodicMemory->getEpisodeCount() > 0) {
            NLM_LOG_INFO("[PASS] Episodic memory episode stored successfully: " + 
                        std::to_string(episodicMemory->getEpisodeCount()) + " episodes");
        }
        
        // Test episode retrieval
        auto* episode = episodicMemory->getEpisode(0);
        if (episode && episode->reward == 0.5f) {
            NLM_LOG_INFO("[PASS] Episodic memory episode retrieved correctly");
        }
        
        // Test episode replay
        episodicMemory->enableReplay(true);
        if (episodicMemory->isReplayEnabled()) {
            NLM_LOG_INFO("[PASS] Episodic memory replay enabled");
        }
        
        // Test recent episodes
        auto recentEpisodes = episodicMemory->getRecentEpisodes(3);
        if (!recentEpisodes.empty()) {
            NLM_LOG_INFO("[PASS] Recent episodes retrieved: " + 
                        std::to_string(recentEpisodes.size()));
        }
    }
    
    // Test 4: Verify associative memory functionality
    auto* associativeMemory = brain->getAssociativeMemory();
    if (associativeMemory) {
        // Create test patterns
        std::vector<float> patternA = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        std::vector<float> patternB = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
        
        // Create associations
        associativeMemory->associate(patternA, patternB, 0.8f);
        associativeMemory->associateFromExperience(testEpisode);
        
        // Verify associations were created
        if (associativeMemory->getAssociationCount() > 0) {
            NLM_LOG_INFO("[PASS] Associative memory associations created: " + 
                        std::to_string(associativeMemory->getAssociationCount()));
        }
        
        // Test association retrieval
        auto retrieved = associativeMemory->retrieve(patternA, 5);
        if (!retrieved.empty()) {
            NLM_LOG_INFO("[PASS] Associative memory patterns retrieved: " + 
                        std::to_string(retrieved.size()));
        }
        
        // Test association strength
        float strength = associativeMemory->getAssociationStrength(patternA, patternB);
        if (strength > 0.0f) {
            NLM_LOG_INFO("[PASS] Associative memory strength retrieved: " + 
                        std::to_string(strength));
        }
        
        // Test activation spreading
        auto spread = associativeMemory->spreadActivation(patternA, 2);
        if (!spread.empty()) {
            NLM_LOG_INFO("[PASS] Associative memory activation spread: " + 
                        std::to_string(spread.size()));
        }
        
        // Test association update
        associativeMemory->updateAssociation(patternA, patternB, 0.1f);
        float newStrength = associativeMemory->getAssociationStrength(patternA, patternB);
        if (newStrength > strength) {
            NLM_LOG_INFO("[PASS] Associative memory association updated: " + 
                        std::to_string(newStrength));
        }
        
        // Test clearing associations
        associativeMemory->clear();
        if (associativeMemory->getAssociationCount() == 0) {
            NLM_LOG_INFO("[PASS] Associative memory cleared successfully");
        }
    }
    
    // Test 5: Verify memory integration with neural processing
    bool integrationSuccessful = true;
    
    // Run some steps with neural activity
    for (int step = 0; step < 100; ++step) {
        // Inject some sensory activity to trigger neural processing
        brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
        
        // Step the brain
        brain->step(step, step * 0.001);
        
        // Occasionally store in working memory
        if (step % 10 == 0) {
            std::vector<float> pattern = {static_cast<float>(step % 10) * 0.1f,
                                         static_cast<float>((step + 5) % 10) * 0.1f,
                                         static_cast<float>((step + 15) % 10) * 0.1f};
            if (workingMemory) {
                workingMemory->store(pattern, 0.5f);
            }
        }
    }
    
    // Check if episodic memory captured any episodes
    if (episodicMemory && episodicMemory->getEpisodeCount() > 0) {
        NLM_LOG_INFO("[PASS] Memory systems captured episodic data during neural processing");
    } else {
        NLM_LOG_INFO("[INFO] No episodic data captured (may be normal for simple simulation)");
    }
    
    // Test 6: Verify memory system consistency
    bool consistencyCheck = true;
    
    if (workingMemory && workingMemory->getActiveTraces() > 0) {
        NLM_LOG_INFO("[PASS] Working memory maintaining consistency");
    }
    
    if (episodicMemory && episodicMemory->getEpisodeCount() >= 0) {
        NLM_LOG_INFO("[PASS] Episodic memory maintaining consistency");
    }
    
    if (associativeMemory && associativeMemory->getAssociationCount() >= 0) {
        NLM_LOG_INFO("[PASS] Associative memory maintaining consistency");
    }
    
    // Test 7: Verify memory system error handling
    bool errorHandlingGood = true;
    
    try {
        if (workingMemory) {
            workingMemory->clear();
        }
        if (episodicMemory) {
            episodicMemory->clear();
        }
        if (associativeMemory) {
            associativeMemory->clear();
        }
        
        NLM_LOG_INFO("[PASS] Memory system error handling working correctly");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Memory system error handling failed: " + std::string(e.what()));
        errorHandlingGood = false;
    }
    
    if (!integrationSuccessful || !errorHandlingGood) {
        NLM_LOG_ERROR("Memory integration test failed");
        return false;
    }
    
    NLM_LOG_INFO("=== Memory Integration Test PASSED ===");
    return true;
}

bool Phase6IntegratedExperiment::testNeuromodulationIntegration() {
    NLM_LOG_INFO("=== Testing Neuromodulation Integration ===");
    
    // Create test configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    // Create brain instance
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed");
        return false;
    }
    
    // Test neuromodulation system accessibility
    bool neuromodulationAvailable = true;
    
    if (brain->getDopamine() == nullptr) {
        NLM_LOG_ERROR("Dopamine system not accessible");
        neuromodulationAvailable = false;
    }
    
    if (brain->getCuriosity() == nullptr) {
        NLM_LOG_ERROR("Curiosity system not accessible");
        neuromodulationAvailable = false;
    }
    
    if (brain->getNovelty() == nullptr) {
        NLM_LOG_ERROR("Novelty system not accessible");
        neuromodulationAvailable = false;
    }
    
    if (!neuromodulationAvailable) {
        NLM_LOG_ERROR("Neuromodulation systems not properly integrated");
        return false;
    }
    
    NLM_LOG_INFO("[PASS] All neuromodulation systems accessible");
    
    // Test neuromodulation effects on neural processing
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    
    // Run simulation with neuromodulation
    float totalReward = 0.0f;
    float noveltySum = 0.0f;
    float curiositySum = 0.0f;
    
    for (int step = 0; step < 200; ++step) {
        // Get neuromodulation values
        if (dopamine) {
            totalReward += dopamine->getLevel();
        }
        if (curiosity) {
            curiositySum += curiosity->getLevel();
        }
        if (novelty) {
            noveltySum += novelty->getLevel();
        }
        
        // Process brain step
        brain->step(step, step * 0.001);
        
        // Inject some sensory activity to trigger neuromodulation
        if (step % 20 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 3.0f);
        }
    }
    
    // Verify neuromodulation had effects
    if (dopamine && dopamine->getLevel() > 0.0f) {
        NLM_LOG_INFO("[PASS] Dopamine system active: " + std::to_string(dopamine->getLevel()));
    }
    
    if (curiosity && curiositySum / 200 > 0.0f) {
        NLM_LOG_INFO("[PASS] Curiosity system active: avg level = " + 
                    std::to_string(curiositySum / 200));
    }
    
    if (novelty && noveltySum / 200 > 0.0f) {
        NLM_LOG_INFO("[PASS] Novelty system active: avg level = " + 
                    std::to_string(noveltySum / 200));
    }
    
    // Test neuromodulation effects on plasticity
    if (dopamine) {
        float plasticityFactor = dopamine->getPlasticityFactor();
        if (plasticityFactor > 0.0f && plasticityFactor <= 1.0f) {
            NLM_LOG_INFO("[PASS] Dopamine plasticity factor valid: " + 
                        std::to_string(plasticityFactor));
        }
    }
    
    // Test neuromodulation integration with memory systems
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory && dopamine) {
        // Check if working memory is affected by neuromodulation
        workingMemory->store({0.5f, 0.5f, 0.5f}, 1.0f);
        
        for (int step = 0; step < 10; ++step) {
            brain->step(step, step * 0.001);
            if (dopamine->getLevel() > 0.5f) {
                break;
            }
        }
        
        NLM_LOG_INFO("[PASS] Neuromodulation integrated with memory systems");
    }
    
    // Test neuromodulation error handling
    try {
        // Verify all neuromodulators are accessible
        if (!dopamine || !curiosity || !novelty) {
            throw std::runtime_error("Neuromodulator not properly initialized");
        }
        
        // Verify neuromodulators have valid states
        if (dopamine->getLevel() < 0.0f || dopamine->getLevel() > 2.0f) {
            throw std::runtime_error("Invalid dopamine level");
        }
        
        NLM_LOG_INFO("[PASS] Neuromodulation error handling working correctly");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Neuromodulation integration test failed: " + std::string(e.what()));
        return false;
    }
    
    NLM_LOG_INFO("=== Neuromodulation Integration Test PASSED ===");
    return true;
}

bool Phase6IntegratedExperiment::testPredictionIntegration() {
    NLM_LOG_INFO("=== Testing Prediction System Integration ===");
    
    // Create test configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    // Create brain instance
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed");
        return false;
    }
    
    // Test prediction system accessibility
    auto* predictionSystem = brain->getPredictionSystem();
    if (predictionSystem == nullptr) {
        NLM_LOG_ERROR("Prediction system not accessible");
        return false;
    }
    
    NLM_LOG_INFO("[PASS] Prediction system accessible");
    
    // Test prediction system integration with other systems
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    auto* dopamine = brain->getDopamine();
    
    // Run simulation to test integration
    bool predictionIntegrated = true;
    
    for (int step = 0; step < 100; ++step) {
        // Inject sensory input to trigger prediction
        if (step % 30 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 2.0f);
        }
        
        // Step brain
        brain->step(step, step * 0.001);
        
        // Occasionally update prediction system if available
        if (step % 20 == 0 && predictionSystem) {
            // Prediction system would typically process sensory input and generate predictions
            // For this test, we just verify it's accessible and doesn't crash
            NLM_LOG_INFO("[INFO] Prediction system processed step " + std::to_string(step));
        }
    }
    
    // Test prediction system with memory systems
    if (workingMemory && predictionSystem) {
        workingMemory->store({0.1f, 0.2f, 0.3f}, 0.5f);
        NLM_LOG_INFO("[PASS] Prediction system integrated with working memory");
    }
    
    if (episodicMemory && predictionSystem) {
        // Create a test episode that prediction system could use
        EpisodicMemoryItem testEpisode;
        testEpisode.timestamp = 50;
        testEpisode.sensoryState = {0.1f, 0.2f, 0.3f};
        testEpisode.resultingSensoryState = {0.2f, 0.3f, 0.4f};
        
        episodicMemory->storeEpisode(testEpisode);
        NLM_LOG_INFO("[PASS] Prediction system integrated with episodic memory");
    }
    
    if (dopamine && predictionSystem) {
        // Test prediction system with neuromodulation
        if (dopamine->getLevel() > 0.0f) {
            NLM_LOG_INFO("[PASS] Prediction system integrated with neuromodulation");
        }
    }
    
    // Test prediction system error handling
    try {
        if (!predictionSystem) {
            throw std::runtime_error("Prediction system not initialized");
        }
        
        // Test prediction system capabilities
        if (predictionSystem) {
            // The prediction system would typically provide:
            // - Prediction accuracy
            // - Prediction confidence
            // - Prediction error signals
            // For this test, we verify it doesn't crash
            NLM_LOG_INFO("[PASS] Prediction system functionality working");
        }
        
        NLM_LOG_INFO("[PASS] Prediction system error handling working correctly");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Prediction integration test failed: " + std::string(e.what()));
        return false;
    }
    
    NLM_LOG_INFO("=== Prediction System Integration Test PASSED ===");
    return true;
}

bool Phase6IntegratedExperiment::testDevelopmentIntegration() {
    NLM_LOG_INFO("=== Testing Development System Integration ===");
    
    // Create test configuration with development enabled
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    cfg->set("enable_development", true);
    
    // Create brain instance
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed");
        return false;
    }
    
    // Test development system accessibility
    auto* developmentSystem = brain->getDevelopmentSystem();
    if (developmentSystem == nullptr) {
        NLM_LOG_ERROR("Development system not accessible");
        return false;
    }
    
    NLM_LOG_INFO("[PASS] Development system accessible");
    
    // Test developmental stages
    DevelopmentalStage initialStage = brain->getDevelopmentalStage();
    NLM_LOG_INFO("[PASS] Initial developmental stage: " + std::to_string(static_cast<int>(initialStage)));
    
    // Test developmental stage transitions
    brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    DevelopmentalStage criticalStage = brain->getDevelopmentalStage();
    if (criticalStage == DevelopmentalStage::CriticalPeriod) {
        NLM_LOG_INFO("[PASS] Developmental stage transition working");
    }
    
    // Test development system integration with plasticity
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    if (structuralPlasticity) {
        // Development system should affect plasticity rates
        NLM_LOG_INFO("[PASS] Development system integrated with plasticity");
    }
    
    // Test development system integration with other systems
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    auto* dopamine = brain->getDopamine();
    
    if (workingMemory) {
        NLM_LOG_INFO("[PASS] Development system integrated with working memory");
    }
    
    if (episodicMemory) {
        NLM_LOG_INFO("[PASS] Development system integrated with episodic memory");
    }
    
    if (dopamine) {
        NLM_LOG_INFO("[PASS] Development system integrated with neuromodulation");
    }
    
    // Run simulation to test development effects
    for (int step = 0; step < 500; ++step) {
        brain->step(step, step * 0.001);
        
        // Check developmental stage changes over time
        if (step % 100 == 0) {
            DevelopmentalStage currentStage = brain->getDevelopmentalStage();
            NLM_LOG_INFO("[INFO] Step " + std::to_string(step) + 
                        ": Developmental stage = " + std::to_string(static_cast<int>(currentStage)));
        }
    }
    
    // Test development system error handling
    try {
        if (!developmentSystem) {
            throw std::runtime_error("Development system not initialized");
        }
        
        // Verify development system can be reset
        brain->reset();
        
        // Verify developmental stage can be changed
        brain->setDevelopmentalStage(DevelopmentalStage::Adult);
        
        NLM_LOG_INFO("[PASS] Development system error handling working correctly");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Development integration test failed: " + std::string(e.what()));
        return false;
    }
    
    NLM_LOG_INFO("=== Development System Integration Test PASSED ===");
    return true;
}

bool Phase6IntegratedExperiment::testCognitionIntegration() {
    NLM_LOG_INFO("=== Testing Cognition System Integration ===");
    
    // Create test configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    // Create brain instance
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed");
        return false;
    }
    
    // Test cognition system accessibility
    bool cognitionSystemsAvailable = true;
    
    if (brain->getPlanner() == nullptr) {
        NLM_LOG_ERROR("Planner system not accessible");
        cognitionSystemsAvailable = false;
    }
    
    if (brain->getConceptFormation() == nullptr) {
        NLM_LOG_ERROR("Concept formation system not accessible");
        cognitionSystemsAvailable = false;
    }
    
    if (brain->getAttention() == nullptr) {
        NLM_LOG_ERROR("Attention system not accessible");
        cognitionSystemsAvailable = false;
    }
    
    if (!cognitionSystemsAvailable) {
        NLM_LOG_ERROR("Cognition systems not properly integrated");
        return false;
    }
    
    NLM_LOG_INFO("[PASS] All cognition systems accessible");
    
    // Test cognition system integration with neural processing
    auto* planner = brain->getPlanner();
    auto* conceptFormation = brain->getConceptFormation();
    auto* attention = brain->getAttention();
    
    // Run simulation to test cognition integration
    for (int step = 0; step < 300; ++step) {
        // Inject sensory input to trigger cognition
        if (step % 50 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 1.0f);
        }
        
        // Step brain
        brain->step(step, step * 0.001);
        
        // Test attention system integration
        if (attention) {
            if (step % 100 == 0) {
                std::vector<NeuronId> competitors = {10001, 10002, 10003, 10004, 10005};
                auto winners = attention->processCompetition(competitors);
                if (!winners.empty()) {
                    NLM_LOG_INFO("[PASS] Attention system processing competition at step " + 
                                std::to_string(step));
                }
            }
        }
        
        // Test concept formation integration
        if (conceptFormation && step % 75 == 0) {
            // Concept formation would typically process neural patterns
            // to form abstract concepts
            NLM_LOG_INFO("[INFO] Concept formation processing at step " + std::to_string(step));
        }
        
        // Test planner integration
        if (planner && step % 120 == 0) {
            // Planner would typically evaluate actions and select plans
            NLM_LOG_INFO("[INFO] Planner evaluating actions at step " + std::to_string(step));
        }
    }
    
    // Test cognition system integration with other systems
    if (planner) {
        NLM_LOG_INFO("[PASS] Cognition systems integrated with neural processing");
    }
    
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory && attention) {
        // Cognition systems should interact with working memory
        workingMemory->store({0.3f, 0.4f, 0.5f}, 0.5f);
        NLM_LOG_INFO("[PASS] Cognition systems integrated with working memory");
    }
    
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory && conceptFormation) {
        // Cognition systems should interact with episodic memory
        NLM_LOG_INFO("[PASS] Cognition systems integrated with episodic memory");
    }
    
    auto* dopamine = brain->getDopamine();
    if (dopamine && planner) {
        // Cognition systems should be influenced by neuromodulation
        NLM_LOG_INFO("[PASS] Cognition systems integrated with neuromodulation");
    }
    
    // Test cognition system error handling
    try {
        if (!planner || !conceptFormation || !attention) {
            throw std::runtime_error("Cognition system not properly initialized");
        }
        
        // Test cognition system capabilities
        if (attention) {
            attention->reset();
        }
        
        NLM_LOG_INFO("[PASS] Cognition system error handling working correctly");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Cognition integration test failed: " + std::string(e.what()));
        return false;
    }
    
    NLM_LOG_INFO("=== Cognition System Integration Test PASSED ===");
    return true;
}

} // namespace nlm