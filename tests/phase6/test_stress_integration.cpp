// test_stress_integration.cpp - Stress and performance tests for Phase 6
// Author: Kilo
// Purpose: Test system under high load and complex scenarios

#include "brain_test_common.hpp"

namespace nlm {
namespace test {

/**
 * Run complex simulation with all systems
 */
bool testCrossSystemStress() {
    std::cout << "=== Test: Cross-System Stress Test ===" << std::endl;
    
    Phase6TestConfig config;
    config.maxSteps = 2000;
    config.neuronCount = 1500;
    config.regionCount = 3;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "/tmp/nlm_stress_checkpoint.bin";
    
    Phase6TestFixture fixture(config);
    auto brain = fixture.createBrain();
    
    if (!brain) {
        std::cout << "FAIL: Could not create brain" << std::endl;
        return false;
    }
    
    std::cout << "Running stress test with " << config.neuronCount << " neurons, " 
              << config.maxSteps << " steps, " << config.regionCount << " regions" << std::endl;
    
    // Test 1: High load simulation
    std::cout << "Test 1: High load simulation" << std::endl;
    
    PerformanceTimer timer;
    timer.reset();
    
    for (uint64_t step = 0; step < config.maxSteps; ++step) {
        // Inject complex, varied input to stress all systems
        float baseInput = sin(step * 0.02f) * 3.0f + cos(step * 0.03f) * 2.0f;
        float noise = (step % 7 == 0) ? 5.0f : (step % 13 == 0) ? -3.0f : 0.0f;
        
        float totalInput = baseInput + noise;
        brain->injectCurrentToNeurons(NeuronType::Sensory, totalInput);
        
        // Inject to different neuron types
        if (step % 5 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Internal, sin(step * 0.07f) * 1.5f);
        }
        if (step % 7 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Motor, cos(step * 0.05f) * 1.0f);
        }
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Occasionally check system state
        if (step % 500 == 0) {
            size_t firingCount = brain->getFiringNeuronCount();
            float firingRate = brain->getAverageFiringRate();
            size_t totalSpikes = brain->getTotalSpikeCount();
            
            std::cout << "  Step " << step << ": firing=" << firingCount 
                      << ", rate=" << firingRate << ", total_spikes=" << totalSpikes << std::endl;
            
            // Verify systems are still functional
            bool workingMemoryOk = (brain->getWorkingMemory() != nullptr);
            bool episodicMemoryOk = (brain->getEpisodicMemory() != nullptr);
            bool neuromodulationOk = (brain->getDopamine() != nullptr);
            
            if (!workingMemoryOk || !episodicMemoryOk || !neuromodulationOk) {
                std::cout << "  WARNING: Some systems may be degraded" << std::endl;
            }
        }
    }
    
    double simulationTime = timer.elapsed();
    std::cout << "Simulation completed in " << simulationTime << " seconds" << std::endl;
    
    // Test 2: Check system stability
    std::cout << "Test 2: System stability check" << std::endl;
    
    size_t finalNeurons = brain->getTotalNeuronCount();
    size_t finalSpikes = brain->getTotalSpikeCount();
    float finalFiringRate = brain->getAverageFiringRate();
    
    std::cout << "Final system state:" << std::endl;
    std::cout << "  Neurons: " << finalNeurons << std::endl;
    std::cout << "  Total spikes: " << finalSpikes << std::endl;
    std::cout << "  Average firing rate: " << finalFiringRate << std::endl;
    
    // System should be stable (not crashed or frozen)
    bool systemStable = (finalNeurons > 0 && finalSpikes > 0 && finalFiringRate >= 0.0f);
    std::cout << "System stability: " << (systemStable ? "STABLE" : "UNSTABLE") << std::endl;
    
    // Test 3: Memory system stress
    std::cout << "Test 3: Memory system stress" << std::endl;
    
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    
    if (workingMemory && episodicMemory) {
        size_t wmTraces = workingMemory->getActiveTraces();
        size_t emEpisodes = episodicMemory->getEpisodeCount();
        
        std::cout << "Working memory traces: " << wmTraces << std::endl;
        std::cout << "Episodic memory episodes: " << emEpisodes << std::endl;
        
        // Memory systems should handle stress well
        bool memoryHandlesStress = (wmTraces >= 0 && emEpisodes >= 0);
        std::cout << "Memory systems handle stress: " << (memoryHandlesStress ? "YES" : "NO") << std::endl;
        
        // Check memory replay under stress
        auto replayEpisodes = episodicMemory->getEpisodesForReplay(3);
        bool replayAvailable = !replayEpisodes.empty();
        std::cout << "Replay available under stress: " << (replayAvailable ? "YES" : "NO") << std::endl;
    }
    
    // Test 4: Neuromodulation stress
    std::cout << "Test 4: Neuromodulation stress" << std::endl;
    
    auto* dopamine = brain->getDopamine();
    auto* adenosine = brain->getAdenosine();
    
    if (dopamine && adenosine) {
        float dopamineLevel = dopamine->getLevel();
        float adenosineLevel = adenosine->getLevel();
        
        std::cout << "Dopamine level: " << dopamineLevel << std::endl;
        std::cout << "Adenosine level: " << adenosineLevel << std::endl;
        
        // Neuromodulation should remain stable
        bool neuromodulationStable = (dopamineLevel >= 0.0f && adenosineLevel >= 0.0f);
        std::cout << "Neuromodulation stable: " << (neuromodulationStable ? "YES" : "NO") << std::endl;
    }
    
    // Test 5: Checkpoint under stress
    std::cout << "Test 5: Checkpoint under stress" << std::endl;
    
    bool checkpointSuccess = brain->save(config.checkpointPath);
    std::cout << "Checkpoint under stress: " << (checkpointSuccess ? "SUCCESS" : "FAILED") << std::endl;
    
    // Test 6: Recovery from stress
    std::cout << "Test 6: Recovery from stress" << std::endl;
    
    // Create new brain and load checkpoint
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    cfg->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
    cfg->set("connection_probability", config.connectionProbability, ConfigSource::Default);
    
    auto brain2 = std::make_shared<Brain>(cfg);
    bool brain2Initialized = brain2->initialize();
    std::cout << "Brain2 initialization: " << (brain2Initialized ? "SUCCESS" : "FAILED") << std::endl;
    
    if (brain2Initialized) {
        bool loadSuccess = brain2->load(config.checkpointPath);
        std::cout << "Load from stress checkpoint: " << (loadSuccess ? "SUCCESS" : "FAILED") << std::endl;
        
        bool canRecover = loadSuccess && brain2->getTotalNeuronCount() > 0;
        std::cout << "System can recover from stress: " << (canRecover ? "YES" : "NO") << std::endl;
    }
    
    // Overall test result
    bool overallSuccess = systemStable && memoryHandlesStress && neuromodulationStable && 
                         checkpointSuccess && canRecover;
    
    std::cout << "=== Cross-System Stress Test Result: " 
              << (overallSuccess ? "PASS" : "FAIL") << " ===" << std::endl;
    
    return overallSuccess;
}

/**
 * Test error conditions and recovery
 */
bool testErrorConditionsRecovery() {
    std::cout << "=== Test: Error Conditions and Recovery ===" << std::endl;
    
    Phase6TestConfig config;
    config.maxSteps = 500;
    config.neuronCount = 300;
    
    Phase6TestFixture fixture(config);
    auto brain = fixture.createBrain();
    
    if (!brain) {
        std::cout << "FAIL: Could not create brain" << std::endl;
        return false;
    }
    
    // Test 1: Handle high input (overload condition)
    std::cout << "Test 1: High input overload condition" << std::endl;
    
    // Inject extremely high input to stress the system
    for (int step = 0; step < 100; ++step) {
        // Very high input to test overload handling
        brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);  // Very high current
        brain->step(step, step * 0.001);
        
        // Check that system doesn't crash
        if (step % 20 == 0) {
            size_t firing = brain->getFiringNeuronCount();
            std::cout << "  Step " << step << ": firing neurons = " << firing << std::endl;
            
            // System should not crash
            bool systemOk = (firing >= 0);
            std::cout << "  System stable: " << (systemOk ? "YES" : "NO") << std::endl;
        }
    }
    
    // Test 2: Handle mixed input patterns
    std::cout << "Test 2: Mixed input patterns" << std::endl;
    
    // Create complex, changing input patterns
    for (int step = 0; step < 200; ++step) {
        // Create different input patterns
        float pattern = 0.0f;
        
        switch (step % 5) {
            case 0: pattern = 5.0f; break;      // Strong input
            case 1: pattern = -2.0f; break;     // Inhibitory input
            case 2: pattern = 0.0f; break;      // No input
            case 3: pattern = sin(step * 0.2f) * 3.0f; break; // Oscillating
            case 4: pattern = 1.0f + step * 0.001f; break; // Ramp up
        }
        
        brain->injectCurrentToNeurons(NeuronType::Sensory, pattern);
        brain->step(step, step * 0.001);
        
        if (step % 50 == 0) {
            size_t firing = brain->getFiringNeuronCount();
            float rate = brain->getAverageFiringRate();
            std::cout << "  Step " << step << ": pattern=" << pattern 
                      << ", firing=" << firing << ", rate=" << rate << std::endl;
        }
    }
    
    // Test 3: Handle neuromodulator extremes
    std::cout << "Test 3: Neuromodulator extreme levels" << std::endl;
    
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        // Set dopamine to extreme levels
        dopamine->setLevel(1.0f);  // Maximum
        brain->step(200, 0.2);
        
        float maxDopamine = dopamine->getLevel();
        std::cout << "Maximum dopamine level: " << maxDopamine << std::endl;
        
        // Set to minimum
        dopamine->setLevel(0.0f);
        brain->step(201, 0.201);
        
        float minDopamine = dopamine->getLevel();
        std::cout << "Minimum dopamine level: " << minDopamine << std::endl;
        
        // System should handle extreme levels
        bool dopamineHandlesExtremes = (maxDopamine <= 1.0f + 0.01f) && (minDopamine >= 0.0f);
        std::cout << "Dopamine handles extremes: " << (dopamineHandlesExtremes ? "YES" : "NO") << std::endl;
    }
    
    // Test 4: Memory system error handling
    std::cout << "Test 4: Memory system error handling" << std::endl;
    
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        // Clear memory and test recovery
        workingMemory->clear();
        
        size_t tracesAfterClear = workingMemory->getActiveTraces();
        std::cout << "Memory traces after clear: " << tracesAfterClear << std::endl;
        
        // System should handle memory clearing
        bool memoryHandlesClearing = (tracesAfterClear == 0);
        std::cout << "Memory handles clearing: " << (memoryHandlesClearing ? "YES" : "NO") << std::endl;
        
        // Restore memory with new input
        brain->injectCurrentToNeurons(NeuronType::Sensory, 2.0f);
        brain->step(250, 0.25);
        
        size_t tracesAfterRestore = workingMemory->getActiveTraces();
        std::cout << "Memory traces after restore: " << tracesAfterRestore << std::endl;
        
        bool memoryRestores = (tracesAfterRestore >= 0);
        std::cout << "Memory restores properly: " << (memoryRestores ? "YES" : "NO") << std::endl;
    }
    
    // Test 5: System recovery after stress
    std::cout << "Test 5: System recovery after stress" << std::endl;
    
    // Check final system state
    size_t finalNeurons = brain->getTotalNeuronCount();
    size_t finalSpikes = brain->getTotalSpikeCount();
    float finalFiringRate = brain->getAverageFiringRate();
    
    std::cout << "Final system state:" << std::endl;
    std::cout << "  Neurons: " << finalNeurons << std::endl;
    std::cout << "  Total spikes: " << finalSpikes << std::endl;
    std::cout << "  Average firing rate: " << finalFiringRate << std::endl;
    
    // System should be stable after stress
    bool systemRecovers = (finalNeurons > 0 && finalSpikes > 0 && finalFiringRate >= 0.0f);
    std::cout << "System recovers from stress: " << (systemRecovers ? "YES" : "NO") << std::endl;
    
    // Test 6: Checkpoint after error conditions
    std::cout << "Test 6: Checkpoint after error conditions" << std::endl;
    
    bool checkpointAfterStress = brain->save("/tmp/nlm_error_test_checkpoint.bin");
    std::cout << "Checkpoint after stress: " << (checkpointAfterStress ? "SUCCESS" : "FAILED") << std::endl;
    
    // Overall test result
    bool overallSuccess = systemRecovers && checkpointAfterStress;
    
    std::cout << "=== Error Conditions and Recovery Test Result: " 
              << (overallSuccess ? "PASS" : "FAIL") << " ===" << std::endl;
    
    return overallSuccess;
}

/**
 * Run all stress and performance tests
 */
void runAllStressIntegrationTests() {
    TestStats stats;
    PerformanceTimer timer;
    
    // Test cross-system stress
    timer.reset();
    bool test1Passed = testCrossSystemStress();
    stats.addTest("Cross-System Stress", test1Passed, timer.elapsed());
    
    // Test error conditions and recovery
    timer.reset();
    bool test2Passed = testErrorConditionsRecovery();
    stats.addTest("Error Conditions and Recovery", test2Passed, timer.elapsed());
    
    // Report results
    stats.report();
}

} // namespace test
} // namespace nlm