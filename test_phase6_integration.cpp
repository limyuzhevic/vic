// Phase 6 Integration Test
// This test verifies that the Phase 6 integration of all systems works

#include "Phase6IntegratedExperiment.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>

int main() {
    NLM_LOG_INFO("=== Phase 6 Integration Test ===");
    
    // Test 1: Verification of integration
    Phase6IntegratedExperiment experiment;
    bool integrationVerified = experiment.verifyIntegration();
    
    if (integrationVerified) {
        NLM_LOG_INFO("[PASS] Phase 6 integration verified successfully");
    } else {
        NLM_LOG_ERROR("[FAIL] Phase 6 integration verification failed");
        return 1;
    }
    
    // Test 2: Memory integration test
    bool memoryTest = experiment.testMemoryIntegration();
    
    if (memoryTest) {
        NLM_LOG_INFO("[PASS] Memory integration test passed");
    } else {
        NLM_LOG_ERROR("[FAIL] Memory integration test failed");
        return 1;
    }
    
    // Test 3: Neuromodulation integration test
    bool neuromodulationTest = experiment.testNeuromodulationIntegration();
    
    if (neuromodulationTest) {
        NLM_LOG_INFO("[PASS] Neuromodulation integration test passed");
    } else {
        NLM_LOG_ERROR("[FAIL] Neuromodulation integration test failed");
        return 1;
    }
    
    // Test 4: Checkpointing test
    bool checkpointTest = experiment.testCheckpointing();
    
    if (checkpointTest) {
        NLM_LOG_INFO("[PASS] Checkpointing test passed");
    } else {
        NLM_LOG_ERROR("[FAIL] Checkpointing test failed");
        return 1;
    }
    
    // Test 5: Replay test
    bool replayTest = experiment.testReplay();
    
    if (replayTest) {
        NLM_LOG_INFO("[PASS] Replay test passed");
    } else {
        NLM_LOG_ERROR("[FAIL] Replay test failed");
        return 1;
    }
    
    NLM_LOG_INFO("=== All Phase 6 Integration Tests Passed ===");
    return 0;
}
