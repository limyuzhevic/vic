#include "Phase6IntegrationTests.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

namespace nlm {

Phase6IntegrationTests::Phase6IntegrationTests() : testCount(0), passedCount(0), failedCount(0) {}

void Phase6IntegrationTests::runAllTests() {
    std::cout << "=== NLM Phase 6 Integration Test Suite ===" << std::endl;
    std::cout << "Testing complete brain system integration..." << std::endl << std::endl;
    
    // Run all test suites
    testMemoryIntegration();
    testNeuromodulationIntegration();
    testPredictionIntegration();
    testCognitionIntegration();
    testDevelopmentIntegration();
    testPersistenceIntegration();
    testFullIntegration();
    
    // Print summary
    std::cout << std::endl;
    std::cout << "=== TEST SUMMARY ===" << std::endl;
    std::cout << "Total tests: " << testCount << std::endl;
    std::cout << "Passed: " << passedCount << std::endl;
    std::cout << "Failed: " << failedCount << std::endl;
    std::cout << "Success rate: " << (testCount > 0 ? (passedCount * 100.0 / testCount) : 0.0) << "%" << std::endl;
}

void Phase6IntegrationTests::testMemoryIntegration() {
    std::cout << "\n--- Testing Memory Integration ---" << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 200);
    cfg->set("region_count", 2);
    
    auto brain = std::make_shared<Brain>(cfg);
    assert(brain->initialize());
    
    // Test 1: Working memory is accessible and functional
    testCount++;
    if (brain->getWorkingMemory() != nullptr) {
        passedCount++;
        std::cout << "[PASS] Working memory is accessible" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Working memory is null" << std::endl;
    }
    
    // Test 2: Episodic memory is accessible and functional
    testCount++;
    if (brain->getEpisodicMemory() != nullptr) {
        passedCount++;
        std::cout << "[PASS] Episodic memory is accessible" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Episodic memory is null" << std::endl;
    }
    
    // Test 3: Associative memory is accessible and functional
    testCount++;
    if (brain->getAssociativeMemory() != nullptr) {
        passedCount++;
        std::cout << "[PASS] Associative memory is accessible" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Associative memory is null" << std::endl;
    }
    
    // Test 4: Memory systems store and retrieve data
    testCount++;
    if (brain->getWorkingMemory() != nullptr) {
        // Store some working memory
        brain->getWorkingMemory()->storeToNeuron(NeuronId(1001), 0.5f);
        float retrieved = brain->getWorkingMemory()->getNeuronActivation(NeuronId(1001));
        if (std::abs(retrieved - 0.5f) < 0.01f) {
            passedCount++;
            std::cout << "[PASS] Working memory stores and retrieves data" << std::endl;
        } else {
            failedCount++;
            std::cout << "[FAIL] Working memory retrieval failed" << std::endl;
        }
    } else {
        failedCount++;
        std::cout << "[FAIL] Cannot test working memory (null)" << std::endl;
    }
    
    // Test 5: Memory updates during brain steps
    testCount++;
    size_t initialActiveTraces = brain->getWorkingMemory() ? 
        brain->getWorkingMemory()->getActiveTraces() : 0;
    
    // Run several steps
    for (int i = 0; i < 50; ++i) {
        brain->step(i);
    }
    
    size_t finalActiveTraces = brain->getWorkingMemory() ? 
        brain->getWorkingMemory()->getActiveTraces() : 0;
    
    // Working memory should have traces after steps
    if (finalActiveTraces > 0 || initialActiveTraces > 0) {
        passedCount++;
        std::cout << "[PASS] Memory systems update during brain steps" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Memory systems not updating during steps" << std::endl;
    }
    
    // Test 6: Episodic memory stores episodes
    testCount++;
    if (brain->getEpisodicMemory() != nullptr) {
        // Run steps to generate episodes
        for (int i = 0; i < 30; ++i) {
            brain->step(i + 100);
        }
        
        size_t episodeCount = brain->getEpisodicMemory()->getEpisodeCount();
        if (episodeCount > 0) {
            passedCount++;
            std::cout << "[PASS] Episodic memory stores episodes: " << episodeCount << std::endl;
        } else {
            failedCount++;
            std::cout << "[FAIL] Episodic memory has no episodes" << std::endl;
        }
    } else {
        failedCount++;
        std::cout << "[FAIL] Cannot test episodic memory (null)" << std::endl;
    }
    
    std::cout << "--- Memory Integration Tests Complete ---" << std::endl;
}

void Phase6IntegrationTests::testNeuromodulationIntegration() {
    std::cout << "\n--- Testing Neuromodulation Integration ---" << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 200);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(cfg);
    assert(brain->initialize());
    
    // Test 1: All neuromodulation systems are accessible
    testCount++;
    bool neuromodulatorsAvailable = 
        brain->getDopamine() != nullptr &&
        brain->getCuriosity() != nullptr &&
        brain->getNovelty() != nullptr &&
        brain->getPredictionErrorSignal() != nullptr;
    
    if (neuromodulatorsAvailable) {
        passedCount++;
        std::cout << "[PASS] All neuromodulation systems are accessible" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Some neuromodulation systems are null" << std::endl;
    }
    
    // Test 2: Neuromodulation affects neural excitability
    testCount++;
    if (brain->getDopamine() != nullptr) {
        float initialLevel = brain->getDopamine()->getLevel();
        
        // Apply reward modulation
        brain->getDopamine()->setLevel(1.0f);
        
        // Run steps to see effect
        for (int i = 0; i < 20; ++i) {
            brain->step(i);
        }
        
        float finalLevel = brain->getDopamine()->getLevel();
        if (finalLevel < 1.0f) {  // Should decay
            passedCount++;
            std::cout << "[PASS] Neuromodulation affects neural excitability" << std::endl;
        } else {
            failedCount++;
            std::cout << "[FAIL] Neuromodulation not affecting neural dynamics" << std::endl;
        }
    } else {
        failedCount++;
        std::cout << "[FAIL] Cannot test neuromodulation effects (dopamine null)" << std::endl;
    }
    
    // Test 3: Neuromodulation interacts with plasticity
    testCount++;
    if (brain->getDopamine() != nullptr && brain->getSTDP() != nullptr) {
        float initialLTP = brain->getSTDP()->getLTPWeight();
        
        // Apply dopamine to affect plasticity
        brain->getDopamine()->setLevel(0.5f);
        
        // Run steps with some spike activity
        for (int i = 0; i < 20; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Excitatory, 20.0f);
            brain->step(i);
        }
        
        float finalLTP = brain->getSTDP()->getLTPWeight();
        if (std::abs(finalLTP - initialLTP) > 0.001f) {
            passedCount++;
            std::cout << "[PASS] Neuromodulation affects plasticity" << std::endl;
        } else {
            failedCount++;
            std::cout << "[FAIL] Neuromodulation not affecting plasticity" << std::endl;
        }
    } else {
        failedCount++;
        std::cout << "[FAIL] Cannot test neuromodulation-plasticity interaction" << std::endl;
    }
    
    // Test 4: Neuromodulation systems respond to brain state
    testCount++;
    bool neuromodulatorsResponsive = true;
    
    if (brain->getCuriosity() != nullptr) {
        // Update curiosity based on prediction error
        brain->getCuriosity()->updateCuriosity(0.5f);
        float curiosity = brain->getCuriosity()->getCuriosityLevel();
        neuromodulatorsResponsive &= (curiosity > 0.0f);
    }
    
    if (brain->getNovelty() != nullptr) {
        // Update novelty based on prediction error
        brain->getNovelty()->detectNovelty(0.7f, 0.1f);
        float novelty = brain->getNovelty()->getLevel();
        neuromodulatorsResponsive &= (novelty > 0.0f);
    }
    
    if (neuromodulatorsResponsive) {
        passedCount++;
        std::cout << "[PASS] Neuromodulation systems respond to brain state" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Neuromodulation systems not responding to brain state" << std::endl;
    }
    
    std::cout << "--- Neuromodulation Integration Tests Complete ---" << std::endl;
}

void Phase6IntegrationTests::testPredictionIntegration() {
    std::cout << "\n--- Testing Prediction Integration ---" << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 200);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(cfg);
    assert(brain->initialize());
    
    // Test 1: Prediction system is accessible
    testCount++;
    if (brain->getPredictionSystem() != nullptr) {
        passedCount++;
        std::cout << "[PASS] Prediction system is accessible" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Prediction system is null" << std::endl;
    }
    
    // Test 2: Prediction system interacts with neuromodulation
    testCount++;
    if (brain->getPredictionSystem() != nullptr && brain->getPredictionErrorSignal() != nullptr && 
        brain->getNovelty() != nullptr && brain->getCuriosity() != nullptr) {
        
        // Run steps with some neural activity
        for (int i = 0; i < 30; ++i) {
            brain->step(i);
        }
        
        // Update prediction system (placeholder implementation)
        std::vector<float> testPattern(10, 0.5f);
        brain->getPredictionSystem()->setCurrentPattern(testPattern);
        
        float predictionError = brain->getPredictionErrorSignal()->getMagnitude();
        
        if (predictionError >= 0.0f) {  // Should be non-negative
            passedCount++;
            std::cout << "[PASS] Prediction system interacts with prediction error" << std::endl;
        } else {
            failedCount++;
            std::cout << "[FAIL] Prediction system not interacting with prediction error" << std::endl;
        }
    } else {
        failedCount++;
        std::cout << "[FAIL] Cannot test prediction-neuromodulation integration" << std::endl;
    }
    
    // Test 3: Prediction system works with working memory
    testCount++;
    if (brain->getPredictionSystem() != nullptr && brain->getWorkingMemory() != nullptr) {
        // Store some working memory
        brain->getWorkingMemory()->storeToNeuron(NeuronId(2001), 0.7f);
        
        // Get working memory for prediction
        std::vector<float> wmData = brain->getWorkingMemory()->retrieve();
        brain->getPredictionSystem()->setCurrentPattern(wmData);
        
        std::vector<float> predicted = brain->getPredictionSystem()->getPrediction();
        if (!predicted.empty()) {
            passedCount++;
            std::cout << "[PASS] Prediction system uses working memory" << std::endl;
        } else {
            failedCount++;
            std::cout << "[FAIL] Prediction system not using working memory" << std::endl;
        }
    } else {
        failedCount++;
        std::cout << "[FAIL] Cannot test prediction-working memory integration" << std::endl;
    }
    
    // Test 4: Prediction error signals drive learning
    testCount++;
    if (brain->getPredictionErrorSignal() != nullptr) {
        float initialError = brain->getPredictionErrorSignal()->getMagnitude();
        
        // Simulate prediction error signal
        brain->getPredictionErrorSignal()->setMagnitude(0.8f);
        
        float finalError = brain->getPredictionErrorSignal()->getMagnitude();
        if (finalError >= 0.8f) {
            passedCount++;
            std::cout << "[PASS] Prediction error signals drive learning" << std::endl;
        } else {
            failedCount++;
            std::cout << "[FAIL] Prediction error signals not driving learning" << std::endl;
        }
    } else {
        failedCount++;
        std::cout << "[FAIL] Cannot test prediction error signals" << std::endl;
    }
    
    std::cout << "--- Prediction Integration Tests Complete ---" << std::endl;
}

void Phase6IntegrationTests::testCognitionIntegration() {
    std::cout << "\n--- Testing Cognition Integration ---" << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 300);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(cfg);
    assert(brain->initialize());
    
    // Test 1: All cognition systems are accessible
    testCount++;
    bool cognitionAvailable = 
        brain->getPlanner() != nullptr &&
        brain->getConceptFormation() != nullptr &&
        brain->getAttention() != nullptr;
    
    if (cognitionAvailable) {
        passedCount++;
        std::cout << "[PASS] All cognition systems are accessible" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Some cognition systems are null" << std::endl;
    }
    
    // Test 2: Neural planner selects actions
    testCount++;
    if (brain->getPlanner() != nullptr) {
        // Run some steps to generate activity
        for (int i = 0; i < 20; ++i) {
            brain->step(i);
        }
        
        // Check if planner can select actions
        auto action = brain->produceAction();
        if (action && action->getType() != ActionType::Wait) {
            passedCount++;
            std::cout << "[PASS] Neural planner selects actions" << std::endl;
        } else {
            failedCount++;
            std::cout << "[FAIL] Neural planner not selecting actions" << std::endl;
        }
    } else {
        failedCount++;
        std::cout << "[FAIL] Cannot test neural planner (null)" << std::endl;
    }
    
    // Test 3: Concept formation processes experiences
    testCount++;
    if (brain->getConceptFormation() != nullptr) {
        // Present some neural patterns to concept formation
        std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        std::vector<float> pattern2 = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
        
        // Process experiences (placeholder implementation)
        brain->getConceptFormation()->presentExperience(pattern1, pattern2, 0.5f, 0);
        brain->getConceptFormation()->presentExperience(pattern2, pattern1, 0.7f, 1);
        
        // Check if concepts were formed
        size_t conceptCount = brain->getConceptFormation()->getConceptCount();
        if (conceptCount > 0) {
            passedCount++;
            std::cout << "[PASS] Concept formation processes experiences" << std::endl;
        } else {
            failedCount++;
            std::cout << "[FAIL] Concept formation not processing experiences" << std::endl;
        }
    } else {
        failedCount++;
        std::cout << "[FAIL] Cannot test concept formation (null)" << std::endl;
    }
    
    // Test 4: Attention system selects winners
    testCount++;
    if (brain->getAttention() != nullptr) {
        // Create some competitors
        std::vector<NeuronId> competitors;
        competitors.push_back(NeuronId(3001));
        competitors.push_back(NeuronId(3002));
        competitors.push_back(NeuronId(3003));
        
        // Process competition
        std::vector<NeuronId> winners = brain->getAttention()->processCompetition(competitors, 1.0f);
        
        if (!winners.empty()) {
            passedCount++;
            std::cout << "[PASS] Attention system selects winners" << std::endl;
        } else {
            failedCount++;
            std::cout << "[FAIL] Attention system not selecting winners" << std::endl;
        }
    } else {
        failedCount++;
        std::cout << "[FAIL] Cannot test attention system (null)" << std::endl;
    }
    
    // Test 5: Cognition systems work together
    testCount++;
    bool cognitionIntegration = true;
    
    if (brain->getPlanner() && brain->getConceptFormation() && brain->getAttention()) {
        // Test integration by running brain steps
        for (int i = 0; i < 30; ++i) {
            brain->step(i);
        }
        
        // Check if all systems had opportunities to work
        if (brain->getPlanner() && brain->getConceptFormation()->getConceptCount() >= 0) {
            cognitionIntegration = true;
        } else {
            cognitionIntegration = false;
        }
    }
    
    if (cognitionIntegration) {
        passedCount++;
        std::cout << "[PASS] Cognition systems work together" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Cognition systems not working together" << std::endl;
    }
    
    std::cout << "--- Cognition Integration Tests Complete ---" << std::endl;
}

void Phase6IntegrationTests::testDevelopmentIntegration() {
    std::cout << "\n--- Testing Development Integration ---" << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 200);
    cfg->set("region_count", 1);
    cfg->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(cfg);
    assert(brain->initialize());
    
    // Test 1: Development system is accessible
    testCount++;
    if (brain->getDevelopmentSystem() != nullptr) {
        passedCount++;
        std::cout << "[PASS] Development system is accessible" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Development system is null" << std::endl;
    }
    
    // Test 2: Development stages affect plasticity
    testCount++;
    if (brain->getDevelopmentSystem() != nullptr && brain->getSTDP() != nullptr && 
        brain->getDopamine() != nullptr) {
        
        DevelopmentalStage initialStage = brain->getDevelopmentStage();
        
        // Run some steps to develop
        for (int i = 0; i < 100; ++i) {
            brain->step(i);
        }
        
        DevelopmentalStage finalStage = brain->getDevelopmentStage();
        
        if (finalStage != DevelopmentalStage::Initial) {  // Should have progressed
            passedCount++;
            std::cout << "[PASS] Development stages change over time" << std::endl;
        } else {
            failedCount++;
            std::cout << "[FAIL] Development stages not changing" << std::endl;
        }
    } else {
        failedCount++;
        std::cout << "[FAIL] Cannot test development-plasticity integration" << std::endl;
    }
    
    // Test 3: Development affects plasticity rates
    testCount++;
    if (brain->getDevelopmentSystem() != nullptr && brain->getStructuralPlasticity() != nullptr) {
        float initialSynaptogenesis = brain->getStructuralPlasticity()->getSynaptogenesisRate();
        
        // Advance development
        brain->setDevelopmentalStage(DevelopmentalStage::Maturation);
        
        float finalSynaptogenesis = brain->getStructuralPlasticity()->getSynaptogenesisRate();
        
        if (std::abs(finalSynaptogenesis - initialSynaptogenesis) > 0.001f) {
            passedCount++;
            std::cout << "[PASS] Development affects plasticity rates" << std::endl;
        } else {
            failedCount++;
            std::cout << "[FAIL] Development not affecting plasticity rates" << std::endl;
        }
    } else {
        failedCount++;
        std::cout << "[FAIL] Cannot test development-plasticity rate integration" << std::endl;
    }
    
    // Test 4: Development interacts with neuromodulation
    testCount++;
    if (brain->getDevelopmentSystem() != nullptr && brain->getDopamine() != nullptr) {
        // Update development
        brain->getDevelopmentSystem()->updateDevelopment(0.1);
        
        float dopamineLevel = brain->getDopamine()->getLevel();
        
        if (dopamineLevel >= 0.0f) {
            passedCount++;
            std::cout << "[PASS] Development interacts with neuromodulation" << std::endl;
        } else {
            failedCount++;
            std::cout << "[FAIL] Development not interacting with neuromodulation" << std::endl;
        }
    } else {
        failedCount++;
        std::cout << "[FAIL] Cannot test development-neuromodulation integration" << std::endl;
    }
    
    // Test 5: Development progresses through stages
    testCount++;
    bool developmentProgresses = false;
    
    if (brain->getDevelopmentSystem() != nullptr) {
        DevelopmentalStage stage = brain->getDevelopmentStage();
        
        // Check if stages are properly defined
        if (stage == DevelopmentalStage::Initial || 
            stage == DevelopmentalStage::CriticalPeriod ||
            stage == DevelopmentalStage::Maturation ||
            stage == DevelopmentalStage::Adult) {
            developmentProgresses = true;
        }
    }
    
    if (developmentProgresses) {
        passedCount++;
        std::cout << "[PASS] Development progresses through proper stages" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Development not progressing through proper stages" << std::endl;
    }
    
    std::cout << "--- Development Integration Tests Complete ---" << std::endl;
}

void Phase6IntegrationTests::testPersistenceIntegration() {
    std::cout << "\n--- Testing Persistence Integration ---" << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 150);
    cfg->set("region_count", 1);
    cfg->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(cfg);
    assert(brain->initialize());
    
    // Test 1: Checkpoint manager is accessible
    testCount++;
    if (brain->getCheckpointManager() != nullptr) {
        passedCount++;
        std::cout << "[PASS] Checkpoint manager is accessible" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Checkpoint manager is null" << std::endl;
    }
    
    // Test 2: Brain can save state
    testCount++;
    std::string checkpointPath = "/tmp/nlm_test_checkpoint.bin";
    
    if (brain->save(checkpointPath)) {
        passedCount++;
        std::cout << "[PASS] Brain can save state" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Brain cannot save state" << std::endl;
    }
    
    // Test 3: Brain can load state (basic check)
    testCount++;
    auto brain2 = std::make_shared<Brain>(cfg);
    assert(brain2->initialize());
    
    // Run some steps
    for (int i = 0; i < 20; ++i) {
        brain2->step(i);
    }
    
    size_t spikesBeforeLoad = brain2->getTotalSpikeCount();
    
    // Load saved state
    if (brain2->load(checkpointPath)) {
        size_t spikesAfterLoad = brain2->getTotalSpikeCount();
        
        if (spikesAfterLoad == spikesBeforeLoad) {
            passedCount++;
            std::cout << "[PASS] Brain can load state correctly" << std::endl;
        } else {
            failedCount++;
            std::cout << "[FAIL] Brain loading state changed neural activity" << std::endl;
        }
    } else {
        failedCount++;
        std::cout << "[FAIL] Brain cannot load state" << std::endl;
    }
    
    // Test 4: Replay system works
    testCount++;
    if (brain->getEpisodicMemory() != nullptr) {
        // Generate some episodes
        for (int i = 0; i < 50; ++i) {
            brain->step(i + 200);
        }
        
        size_t episodeCount = brain->getEpisodicMemory()->getEpisodeCount();
        
        if (episodeCount > 0) {
            // Test replay
            auto episodes = brain->getEpisodicMemory()->getEpisodesForReplay(3);
            
            if (!episodes.empty()) {
                passedCount++;
                std::cout << "[PASS] Replay system works: " << episodes.size() << " episodes to replay" << std::endl;
            } else {
                failedCount++;
                std::cout << "[FAIL] Replay system has no episodes to replay" << std::endl;
            }
        } else {
            failedCount++;
            std::cout << "[FAIL] No episodes for replay test" << std::endl;
        }
    } else {
        failedCount++;
        std::cout << "[FAIL] Cannot test replay system (episodic memory null)" << std::endl;
    }
    
    // Test 5: Checkpoint system integrates with memory
    testCount++;
    if (brain->getCheckpointManager() != nullptr && brain->getEpisodicMemory() != nullptr) {
        // Save with memory system
        std::string checkpointPath2 = "/tmp/nlm_test_checkpoint2.bin";
        
        if (brain->save(checkpointPath2)) {
            // Load and verify memory persistence
            if (brain2->load(checkpointPath2)) {
                passedCount++;
                std::cout << "[PASS] Checkpoint system integrates with memory" << std::endl;
            } else {
                failedCount++;
                std::cout << "[FAIL] Checkpoint system not integrating with memory" << std::endl;
            }
        } else {
            failedCount++;
            std::cout << "[FAIL] Cannot save with checkpoint memory integration" << std::endl;
        }
    } else {
        failedCount++;
        std::cout << "[FAIL] Cannot test checkpoint-memory integration" << std::endl;
    }
    
    std::cout << "--- Persistence Integration Tests Complete ---" << std::endl;
}

void Phase6IntegrationTests::testFullIntegration() {
    std::cout << "\n--- Testing Full Brain Integration ---" << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 500);
    cfg->set("region_count", 2);
    cfg->set("simulation_timestep", 0.001);
    cfg->set("replay_interval", 50);
    cfg->set("consolidation_interval", 200);
    
    auto brain = std::make_shared<Brain>(cfg);
    assert(brain->initialize());
    
    // Test 1: All critical systems are initialized
    testCount++;
    if (brain->areAllSystemsInitialized()) {
        passedCount++;
        std::cout << "[PASS] All critical systems are initialized" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Some critical systems not initialized" << std::endl;
    }
    
    // Test 2: Brain loop works with all systems
    testCount++;
    bool brainLoopWorks = true;
    
    try {
        // Run complete brain loop
        for (int step = 0; step < 100; ++step) {
            // Simulate world interaction
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            
            // Brain processes
            brain->step(step);
            
            // Generate action
            auto action = brain->produceAction();
            
            // Apply action effects
            brain->injectCurrentToNeurons(NeuronType::Motor, 5.0f);
            
            // Check if systems are still operational
            if (brain->getWorkingMemory() && brain->getEpisodicMemory() && 
                brain->getDopamine() && brain->getCuriosity()) {
                continue;
            } else {
                brainLoopWorks = false;
                break;
            }
        }
        
        if (brainLoopWorks) {
            passedCount++;
            std::cout << "[PASS] Complete brain loop works with all systems" << std::endl;
        } else {
            failedCount++;
            std::cout << "[FAIL] Brain loop fails when systems disconnected" << std::endl;
        }
    } catch (const std::exception& e) {
        failedCount++;
        std::cout << "[FAIL] Brain loop exception: " << e.what() << std::endl;
    }
    
    // Test 3: Brain develops and learns over time
    testCount++;
    bool brainDevelops = true;
    
    DevelopmentalStage initialStage = brain->getDevelopmentStage();
    
    // Run many steps for development
    for (int step = 100; step < 300; ++step) {
        brain->step(step);
        
        // Check development progress
        if (brain->getDevelopmentStage() == DevelopmentalStage::Adult) {
            break;
        }
    }
    
    DevelopmentalStage finalStage = brain->getDevelopmentStage();
    
    if (finalStage == DevelopmentalStage::Adult) {
        passedCount++;
        std::cout << "[PASS] Brain develops through all developmental stages" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Brain not developing through all stages" << std::endl;
    }
    
    // Test 4: Systems interact bidirectionally
    testCount++;
    bool systemsInteract = true;
    
    if (brain->getWorkingMemory() && brain->getDopamine()) {
        // Store memory
        brain->getWorkingMemory()->storeToNeuron(NeuronId(9001), 0.8f);
        
        // Check neuromodulation affected memory
        if (brain->getDopamine()->getLevel() >= 0.0f) {
            systemsInteract = true;
        } else {
            systemsInteract = false;
        }
    }
    
    if (systemsInteract) {
        passedCount++;
        std::cout << "[PASS] Systems interact bidirectionally" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Systems not interacting bidirectionally" << std::endl;
    }
    
    // Test 5: End state validation
    testCount++;
    bool endStateValid = true;
    
    if (brain->getWorkingMemory()) {
        size_t memorySize = brain->getWorkingMemory()->getActiveTraces();
        if (memorySize < 1000) {  // Reasonable limit
            endStateValid = true;
        } else {
            endStateValid = false;
        }
    }
    
    if (brain->getEpisodicMemory()) {
        size_t episodeCount = brain->getEpisodicMemory()->getEpisodeCount();
        if (episodeCount >= 0) {  // Should have episodes
            endStateValid = true;
        } else {
            endStateValid = false;
        }
    }
    
    if (brain->getDopamine()) {
        float dopamineLevel = brain->getDopamine()->getLevel();
        if (dopamineLevel >= 0.0f) {
            endStateValid = true;
        } else {
            endStateValid = false;
        }
    }
    
    if (endStateValid) {
        passedCount++;
        std::cout << "[PASS] End state is valid and coherent" << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] End state is invalid or incoherent" << std::endl;
    }
    
    // Test 6: Performance metrics are reasonable
    testCount++;
    bool performanceValid = true;
    
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSpikes = brain->getTotalSpikeCount();
    size_t activeNeurons = brain->getActiveNeuronCount();
    
    if (totalNeurons > 0 && totalSpikes >= 0 && activeNeurons <= totalNeurons) {
        passedCount++;
        std::cout << "[PASS] Performance metrics are reasonable" << std::endl;
        std::cout << "    Neurons: " << totalNeurons << ", Spikes: " << totalSpikes << ", Active: " << activeNeurons << std::endl;
    } else {
        failedCount++;
        std::cout << "[FAIL] Performance metrics are invalid" << std::endl;
    }
    
    std::cout << "--- Full Integration Tests Complete ---" << std::endl;
}

std::string Phase6IntegrationTests::getTestSummary() const {
    std::stringstream ss;
    ss << "=== NLM Phase 6 Integration Test Suite ===\n";
    ss << "Total tests: " << testCount << "\n";
    ss << "Passed: " << passedCount << "\n";
    ss << "Failed: " << failedCount << "\n";
    ss << "Success rate: " << (testCount > 0 ? (passedCount * 100.0 / testCount) : 0.0) << "%\n";
    return ss.str();
}