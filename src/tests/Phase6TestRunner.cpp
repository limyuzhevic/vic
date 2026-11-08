// Phase6TestRunner.cpp
// Main test runner for all Phase 6 integration tests

#include "Phase6IntegrationTest.hpp"
#include "RewardPredictionTest.hpp"
#include "CheckpointingTest.hpp"
#include "ConfigTest.hpp"
#include <iostream>
#include <fstream>

int main() {
    std::cout << "=== Phase 6 Comprehensive Integration Test Suite ===" << std::endl;
    std::cout << std::endl;
    
    try {
        // Run Phase 6 integration tests
        test_phase6::runAllPhase6Tests();
        std::cout << std::endl;
        
        // Run reward prediction tests
        test_reward_prediction::runAllRewardPredictionTests();
        std::cout << std::endl;
        
        // Run checkpointing tests
        test_checkpointing::runAllCheckpointingTests();
        std::cout << std::endl;
        
        // Run configuration tests
        test_config::runAllConfigTests();
        std::cout << std::endl;
        
        std::cout << "=== ALL PHASE 6 TESTS COMPLETED SUCCESSFULLY ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Summary of tested fixes:" << std::endl;
        std::cout << "1. ✓ Reward Prediction System - Fixed zero prediction bug" << std::endl;
        std::cout << "2. ✓ Checkpointing - Fixed stub implementations" << std::endl;
        std::cout << "3. ✓ Configuration - Fixed duplicate dopamine_baseline" << std::endl;
        std::cout << "4. ✓ Learning Effectiveness - Fixed saturation issues" << std::endl;
        std::cout << "5. ✓ Backward Compatibility - Existing functionality preserved" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Test suite failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Test suite failed with unknown exception" << std::endl;
        return 1;
    }
}