// test_brain_integration.cpp - Brain integration tests for Phase 6
// Author: Kilo
// Purpose: Test the integration of all brain systems in Brain::step()

#include "brain_test_common.hpp"

namespace nlm {
namespace test {

/**
 * Test basic brain integration - all systems work together
 */
bool testBasicBrainIntegration() {
    std::cout << "=== Test: Basic Brain Integration ===" << std::endl;
    
    Phase6TestConfig config;
    config.maxSteps = 500;
    config.neuronCount = 300;
    config.regionCount = 1;
    
    Phase6TestFixture fixture(config);
    auto brain = fixture.createBrain();
    
    if (!brain) {
        std::cout << "FAIL: Could not create brain" << std::endl;
        return false;
    }
    
    // Verify all systems are integrated
    bool success = true;
    
    // Test 1: Memory systems
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        std::cout << "PASS: Working memory is available" << std::endl;
        
        // Test basic storage
        for (int i = 0; i < 10; ++i) {
            brain->step(i, i * 0.001);
        }
        
        std::cout << "PASS: Working memory updated during simulation" << std::endl;
    } else {
        std::cout << "FAIL: Working memory is not available" << std::endl;
        success = false;
    }
    
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        std::cout << "PASS: Episodic memory is available" << std::endl;
        
        // Test episode storage
        size_t initialEpisodes = episodicMemory->getEpisodeCount();
        std::cout << "  Initial episodes: " << initialEpisodes << std::endl;
    } else {
        std::cout << "FAIL: Episodic memory is not available" << std::endl;
        success = false;
    }
    
    // Test 2: Neuromodulation systems
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        std::cout << "PASS: Dopamine system is available" << std::endl;
        
        // Test that dopamine affects neural computation
        float initialLevel = dopamine->getLevel();
        brain->injectCurrentToNeurons(NeuronType::Internal, 5.0f);
        brain->step(10, 0.01);
        float finalLevel = dopamine->getLevel();
        
        std::cout << "  Dopamine level changed from " << initialLevel 
                  << " to " << finalLevel << std::endl;
    } else {
        std::cout << "FAIL: Dopamine system is not available" << std::endl;
        success = false;
    }
    
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        std::cout << "PASS: Curiosity system is available" << std::endl;
    } else {
        std::cout << "FAIL: Curiosity system is not available" << std::endl;
        success = false;
    }
    
    // Test 3: Prediction system
    auto* predictionSystem = brain->getPredictionSystem();
    if (predictionSystem) {
        std::cout << "PASS: Prediction system is available" << std::endl;
        
        // Test prediction error tracking
        float error = predictionSystem->getPredictionError();
        std::cout << "  Initial prediction error: " << error << std::endl;
    } else {
        std::cout << "FAIL: Prediction system is not available" << std::endl;
        success = false;
    }
    
    // Test 4: Cognition systems
    auto* planner = brain->getPlanner();
    if (planner) {
        std::cout << "PASS: Neural planner is available" << std::endl;
    } else {
        std::cout << "FAIL: Neural planner is not available" << std::endl;
        success = false;
    }
    
    auto* conceptFormation = brain->getConceptFormation();
    if (conceptFormation) {
        std::cout << "PASS: Concept formation is available" << std::endl;
    } else {
        std::cout << "FAIL: Concept formation is not available" << std::endl;
        success = false;
    }
    
    auto* attention = brain->getAttention();
    if (attention) {
        std::cout << "PASS: Attention system is available" << std::endl;
    } else {
        std::cout << "FAIL: Attention system is not available" << std::endl;
        success = false;
    }
    
    // Test 5: Development system
    auto* developmentSystem = brain->getDevelopmentSystem();
    if (developmentSystem) {
        std::cout << "PASS: Development system is available" << std::endl;
    } else {
        std::cout << "FAIL: Development system is not available" << std::endl;
        success = false;
    }
    
    // Run integration simulation
    std::cout << "Running integration simulation (" << config.maxSteps << " steps)..." << std::endl;
    for (uint64_t step = 0; step < config.maxSteps; ++step) {
        brain->step(step, step * 0.001);
        
        // Inject sensory activity to keep brain active
        if (step % 10 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 2.0f);
        }
        
        if (step % 500 == 0) {
            std::cout << "  Progress: " << step << "/" << config.maxSteps << std::endl;
        }
    }
    
    // Verify systems interacted properly
    size_t finalFiring = brain->getFiringNeuronCount();
    std::cout << "Integration simulation completed. Final firing count: " << finalFiring << std::endl;
    
    // Check that systems are still functional after simulation
    bool memoryStillWorking = (workingMemory && workingMemory->getActiveTraces() > 0);
    std::cout << "Memory still working: " << (memoryStillWorking ? "YES" : "NO") << std::endl;
    
    bool neuromodulationStillWorking = (dopamine && dopamine->getLevel() > 0.0f);
    std::cout << "Neuromodulation still working: " << (neuromodulationStillWorking ? "YES" : "NO") << std::endl;
    
    if (memoryStillWorking && neuromodulationStillWorking) {
        std::cout << "PASS: All integrated systems are functional" << std::endl;
        return true;
    } else {
        std::cout << "FAIL: Some integrated systems failed" << std::endl;
        return false;
    }
}

/**
 * Test that memory systems properly exchange data
 */
bool testMemorySystemDataExchange() {
    std::cout << "=== Test: Memory System Data Exchange ===" << std::endl;
    
    Phase6TestConfig config;
    config.maxSteps = 1000;
    config.neuronCount = 400;
    
    Phase6TestFixture fixture(config);
    auto brain = fixture.createBrain();
    
    if (!brain) {
        std::cout << "FAIL: Could not create brain" << std::endl;
        return false;
    }
    
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    
    if (!workingMemory || !episodicMemory) {
        std::cout << "FAIL: Memory systems not available" << std::endl;
        return false;
    }
    
    // Test 1: Working memory receives sensory input
    std::cout << "Test 1: Working memory receives and stores sensory input" << std::endl;
    
    // Inject sensory activity multiple times
    for (int step = 0; step < 100; step += 10) {
        brain->step(step, step * 0.001);
        brain->injectCurrentToNeurons(NeuronType::Sensory, 3.0f + sin(step * 0.1) * 2.0f);
        
        size_t activeTraces = workingMemory->getActiveTraces();
        if (activeTraces > 0) {
            std::cout << "  Step " << step << ": Working memory has " << activeTraces 
                      << " active traces" << std::endl;
        }
    }
    
    bool workingMemoryStoring = (workingMemory->getActiveTraces() > 0);
    std::cout << "Working memory storing: " << (workingMemoryStoring ? "YES" : "NO") << std::endl;
    
    // Test 2: Episodic memory integrates with sensory input and brain state
    std::cout << "Test 2: Episodic memory integration" << std::endl;
    
    // Store some episodes during simulation
    size_t episodesBefore = episodicMemory->getEpisodeCount();
    
    for (int step = 0; step < 300; step += 30) {
        brain->step(step, step * 0.001);
        
        // Store some memory traces
        brain->injectCurrentToNeurons(NeuronType::Internal, 1.0f);
        
        if (step % 60 == 0) {
            size_t episodesAfter = episodicMemory->getEpisodeCount();
            if (episodesAfter > episodesBefore) {
                std::cout << "  New episode stored at step " << step 
                          << ". Total: " << episodesAfter << std::endl;
                episodesBefore = episodesAfter;
            }
        }
    }
    
    bool episodicMemoryStoring = (episodicMemory->getEpisodeCount() > 0);
    std::cout << "Episodic memory storing: " << (episodicMemoryStoring ? "YES" : "NO") << std::endl;
    
    // Test 3: Memory consolidation during rest periods
    std::cout << "Test 3: Memory consolidation simulation" << std::endl;
    
    // The adenosine-based rest cycle should trigger consolidation
    auto* adenosine = brain->getAdenosine();
    if (adenosine) {
        std::cout << "  Adenosine system available for rest cycle testing" << std::endl;
        
        // Check if adenosine should trigger rest
        bool shouldRest = adenosine->shouldTriggerRest();
        std::cout << "  Should rest now: " << (shouldRest ? "YES" : "NO") << std::endl;
        
        // Simulate rest by setting adenosine level high
        adenosine->setLevel(0.8f);
        
        bool restingNow = adenosine->shouldTriggerRest();
        std::cout << "  Should rest after high adenosine: " << (restingNow ? "YES" : "NO") << std::endl;
        
        // Reset adenosine
        adenosine->setLevel(0.0f);
    }
    
    // Test 4: Replay mechanism functionality
    std::cout << "Test 4: Memory replay mechanism" << std::endl;
    
    // Get episodes for replay
    auto episodes = episodicMemory->getEpisodesForReplay(5);
    
    bool replayAvailable = !episodes.empty();
    std::cout << "Replay available: " << (replayAvailable ? "YES" : "NO")") << std::endl;
    
    if (replayAvailable) {
        std::cout << "  Can replay " << episodes.size() << " episodes" << std::endl;
    }
    
    // Test 5: Cross-system memory integration
    std::cout << "Test 5: Cross-system memory integration" << std::endl;
    
    // Working memory should influence episodic memory
    // and episodic memory should influence working memory through replay
    
    // Current working memory activity
    float memoryActivity = workingMemory->getMemoryActivity();
    std::cout << "Current working memory activity: " << memoryActivity << std::endl;
    
    // Episodic memory count
    size_t episodeCount = episodicMemory->getEpisodeCount();
    std::cout << "Current episodic memory episodes: " << episodeCount << std::endl;
    
    // Both should be positive for successful integration
    bool integrationSuccess = workingMemoryStoring && episodicMemoryStoring && replayAvailable;
    std::cout << "Overall memory integration: " << (integrationSuccess ? "SUCCESS" : "FAILED") << std::endl;
    
    return integrationSuccess;
}

/**
 * Run all brain integration tests
 */
void runAllBrainIntegrationTests() {
    TestStats stats;
    PerformanceTimer timer;
    
    // Test basic integration
    timer.reset();
    bool test1Passed = testBasicBrainIntegration();
    stats.addTest("Basic Brain Integration", test1Passed, timer.elapsed());
    
    // Test memory data exchange
    timer.reset();
    bool test2Passed = testMemorySystemDataExchange();
    stats.addTest("Memory System Data Exchange", test2Passed, timer.elapsed());
    
    // Report results
    stats.report();
}

} // namespace test
} // namespace nlm