// PredictionError Tests
// Comprehensive testing of prediction error neuromodulation system

#include "neuromodulation/PredictionError.hpp"
#include "brain/Brain.hpp"
#include "core/Random/Random.hpp"
#include "core/Config/Config.hpp"
#include "core/Exception.hpp"
#include <iostream>
#include <cmath>

namespace test_prediction_error {

void testPredictionErrorCreation() {
    nlm::PredictionError predictionError;
    
    const char* name = predictionError.getName();
    if (!name || std::string(name) != "PredictionError") {
        throw nlm::NLMTestException("PredictionError creation failed: name should be 'PredictionError'");
    }
    
    std::cout << "    testPredictionErrorCreation passed" << std::endl;
}

void testPredictionErrorName() {
    nlm::PredictionError predictionError;
    
    const char* name = predictionError.getName();
    if (!name) {
        throw nlm::NLMTestException("PredictionError name failed: name is null");
    }
    if (std::string(name) != "PredictionError") {
        throw nlm::NLMTestException("PredictionError name failed: expected 'PredictionError', got '" + 
                                   std::string(name) + "'");
    }
    
    std::cout << "    testPredictionErrorName passed" << std::endl;
}

void testPredictionErrorUpdate() {
    nlm::PredictionError predictionError;
    
    float initialLevel = predictionError.getLevel();
    
    // Update with positive prediction error
    predictionError.update(0.5f);
    
    float newLevel = predictionError.getLevel();
    
    // Level should have increased
    if (newLevel <= initialLevel) {
        throw nlm::NLMTestException("PredictionError update test failed: level should increase with positive error, " + 
                                   "initial: " + std::to_string(initialLevel) + ", new: " + 
                                   std::to_string(newLevel));
    }
    
    std::cout << "    testPredictionErrorUpdate passed (level: " << initialLevel << " -> " << newLevel << ")" << std::endl;
}

void testPredictionErrorZeroInput() {
    nlm::PredictionError predictionError;
    
    float initialLevel = predictionError.getLevel();
    predictionError.update(0.0f); // Zero prediction error
    float newLevel = predictionError.getLevel();
    
    // Level should have decayed towards zero
    if (newLevel > initialLevel * 1.1f) {
        throw nlm::NLMTestException("PredictionError zero input test failed: level should decay with zero error, " + 
                                   "initial: " + std::to_string(initialLevel) + ", new: " + 
                                   std::to_string(newLevel));
    }
    
    std::cout << "    testPredictionErrorZeroInput passed (level: " << initialLevel << " -> " << newLevel << ")" << std::endl;
}

void testPredictionErrorReset() {
    nlm::PredictionError predictionError;
    
    // Set some level
    predictionError.update(0.8f);
    
    float nonZeroLevel = predictionError.getLevel();
    if (nonZeroLevel <= 0.0f) {
        throw nlm::NLMTestException("PredictionError reset test failed: level should be non-zero before reset");
    }
    
    // Reset
    predictionError.reset();
    
    float resetLevel = predictionError.getLevel();
    if (resetLevel != 0.0f) {
        throw nlm::NLMTestException("PredictionError reset test failed: level should be 0.0f after reset, got " + 
                                   std::to_string(resetLevel));
    }
    
    std::cout << "    testPredictionErrorReset passed (level reset from " << nonZeroLevel << " to 0.0f)" << std::endl;
}

void testPredictionErrorDecay() {
    nlm::PredictionError predictionError;
    
    // Set high level
    predictionError.update(1.0f);
    
    float initialLevel = predictionError.getLevel();
    
    // Wait (decay over time)
    predictionError.update(0.0f); // Zero input - should decay
    float levelAfterOneStep = predictionError.getLevel();
    
    // Level should have decayed
    if (levelAfterOneStep >= initialLevel) {
        throw nlm::NLMTestException("PredictionError decay test failed: level should decay over time, " + 
                                   "initial: " + std::to_string(initialLevel) + ", after step: " + 
                                   std::to_string(levelAfterOneStep));
    }
    
    std::cout << "    testPredictionErrorDecay passed (level: " << initialLevel << " -> " << levelAfterOneStep << ")" << std::endl;
}

void testPredictionErrorTimeDependency() {
    nlm::PredictionError predictionError;
    
    // Reset for clean test
    predictionError.reset();
    
    float initialLevel = predictionError.getLevel();
    
    // Update with input
    predictionError.update(0.5f);
    float levelAfterUpdate = predictionError.getLevel();
    
    // Reset and update with input (should be the same without decay in between)
    predictionError.reset();
    predictionError.update(0.5f);
    float levelAfterReset = predictionError.getLevel();
    
    // Levels should be very close (decay is small)
    if (std::abs(levelAfterUpdate - levelAfterReset) > 0.01f) {
        throw nlm::NLMTestException("PredictionError time dependency test failed: levels should be close when reset between updates, " + 
                                   "update: " + std::to_string(levelAfterUpdate) + ", reset: " + 
                                   std::to_string(levelAfterReset));
    }
    
    std::cout << "    testPredictionErrorTimeDependency passed (update: " << levelAfterUpdate << ", reset: " << levelAfterReset << ")" << std::endl;
}

void testPredictionErrorMaxLevel() {
    nlm::PredictionError predictionError;
    
    // Set very high input
    predictionError.update(1.0f);
    
    float level = predictionError.getLevel();
    
    // Level should be reasonable (not infinite)
    if (level > 1.0f) {
        throw nlm::NLMTestException("PredictionError max level test failed: level should not exceed 1.0, got " + 
                                   std::to_string(level));
    }
    
    std::cout << "    testPredictionErrorMaxLevel passed (level: " << level << ")" << std::endl;
}

void testPredictionErrorIntegration() {
    // Test integration with Curiosity system conceptually
    nlm::PredictionError predictionError;
    
    // Get level for integration check
    float level = predictionError.getLevel();
    
    // Update
    predictionError.update(0.3f);
    
    float newLevel = predictionError.getLevel();
    
    if (newLevel <= level) {
        throw nlm::NLMTestException("PredictionError integration test failed: level should increase with error, " + 
                                   "initial: " + std::to_string(level) + ", new: " + 
                                   std::to_string(newLevel));
    }
    
    std::cout << "    testPredictionErrorIntegration passed (level: " << level << " -> " << newLevel << ")" << std::endl;
}

void testPredictionErrorNegativeInput() {
    nlm::PredictionError predictionError;
    
    // Test with negative input (should handle gracefully)
    predictionError.update(-0.5f);
    
    float level = predictionError.getLevel();
    
    // Level might be negative or zero, but shouldn't crash
    if (level > 0.0f) {
        throw nlm::NLMTestException("PredictionError negative input test failed: level should be <= 0.0 with negative input, got " + 
                                   std::to_string(level));
    }
    
    std::cout << "    testPredictionErrorNegativeInput passed (level: " << level << ")" << std::endl;
}

void runAll() {
    std::cout << "Running PredictionError tests..." << std::endl;
    testPredictionErrorCreation();
    testPredictionErrorName();
    testPredictionErrorUpdate();
    testPredictionErrorZeroInput();
    testPredictionErrorReset();
    testPredictionErrorDecay();
    testPredictionErrorTimeDependency();
    testPredictionErrorMaxLevel();
    testPredictionErrorIntegration();
    testPredictionErrorNegativeInput();
}

} // namespace test_prediction_error
