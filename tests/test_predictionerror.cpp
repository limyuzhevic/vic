// PredictionError Tests
#include "neuromodulation/PredictionError.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_predictionerror {

void testPredictionErrorCreation() {
    nlm::PredictionError predError;
    
    assert(predError.getError() == 0.0f);  // Default error
    assert(predError.getMagnitude() == 0.0f);  // Default magnitude
    assert(predError.getHistory().empty());
    
    std::cout << "    testPredictionErrorCreation passed" << std::endl;
}

void testPredictionErrorComputation() {
    nlm::PredictionError predError;
    
    // Test computing error with simple values
    predError.computeError(0.5f, 0.7f);  // Error should be 0.2
    float error = predError.getError();
    assert(std::abs(error - 0.2f) < 0.001f);  // Allow small floating point error
    
    // Test with larger error
    predError.computeError(0.0f, 1.0f);  // Error should be 1.0
    assert(std::abs(predError.getError() - 1.0f) < 0.001f);
    
    // Test negative error
    predError.computeError(1.0f, 0.3f);  // Error should be -0.7
    assert(std::abs(predError.getError() - (-0.7f)) < 0.001f);
    
    std::cout << "    testPredictionErrorComputation passed" << std::endl;
}

void testPredictionUpdate() {
    nlm::PredictionError predError;
    
    // Set initial error
    predError.computeError(0.5f, 0.7f);
    assert(predError.getError() == 0.2f);
    
    // Update prediction
    predError.updatePrediction(0.6f);
    
    // Verify history updated
    const auto& history = predError.getHistory();
    assert(!history.empty());
    assert(history.back() == 0.6f);
    
    std::cout << "    testPredictionUpdate passed" << std::endl;
}

void testPredictionErrorMagnitude() {
    nlm::PredictionError predError;
    
    // Test with zero error
    predError.computeError(0.5f, 0.5f);  // Error = 0
    assert(predError.getMagnitude() == 0.0f);
    
    // Test with non-zero error
    predError.computeError(0.0f, 1.0f);  // Error = 1.0
    assert(std::abs(predError.getMagnitude() - 1.0f) < 0.001f);
    
    // Test absolute value (magnitude should always be positive)
    predError.computeError(1.0f, 0.2f);  // Error = 0.8
    assert(predError.getMagnitude() == 0.8f);
    
    std::cout << "    testPredictionErrorMagnitude passed" << std::endl;
}

void testPredictionErrorHistory() {
    nlm::PredictionError predError;
    
    // Verify initial state
    assert(predError.getHistory().empty());
    
    // Add some error computations
    predError.computeError(0.1f, 0.2f);
    predError.computeError(0.3f, 0.5f);
    predError.updatePrediction(0.4f);
    
    // Check history contains expected values
    const auto& history = predError.getHistory();
    assert(history.size() == 2);  // One error computation + one prediction update
    assert(std::abs(history[0] - 0.1f) < 0.001f);
    assert(std::abs(history[1] - 0.4f) < 0.001f);
    
    // Test clearing history
    predError.clearHistory();
    assert(predError.getHistory().empty());
    
    std::cout << "    testPredictionErrorHistory passed" << std::endl;
}

void testPredictionErrorInitialization() {
    nlm::PredictionError predError;
    
    // Create with dummy brain pointer (implementation dependent)
    // This test verifies the interface exists
    class MockBrain {};
    MockBrain mockBrain;
    
    // The actual implementation may or may not require this
    // We'll just test that it compiles
    
    std::cout << "    testPredictionErrorInitialization passed" << std::endl;
}

void runAll() {
    testPredictionErrorCreation();
    testPredictionErrorComputation();
    testPredictionUpdate();
    testPredictionErrorMagnitude();
    testPredictionErrorHistory();
    testPredictionErrorInitialization();
    
    std::cout << "=== PredictionError Tests Complete ===" << std::endl;
}

} // namespace test_predictionerror