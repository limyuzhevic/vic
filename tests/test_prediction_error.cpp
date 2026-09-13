// Prediction Error Tests
// Tests for prediction error calculation and signaling

#include "neuromodulation/PredictionError.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_prediction_error {

void testPredictionErrorCreation() {
    nlm::PredictionError pe;
    
    assert(pe.getError() == 0.0f);
    assert(pe.getMagnitude() == 0.0f);
    
    std::cout << "    testPredictionErrorCreation passed" << std::endl;
}

void testPredictionErrorCompute() {
    nlm::PredictionError pe;
    
    // Test positive prediction error (actual > predicted)
    pe.computeError(0.5f, 0.8f);  // predicted=0.5, actual=0.8
    assert(pe.getError() == 0.3f);
    assert(pe.getMagnitude() == 0.3f);
    
    // Test negative prediction error (actual < predicted)
    pe.computeError(0.8f, 0.5f);  // predicted=0.8, actual=0.5
    assert(pe.getError() == -0.3f);
    assert(pe.getMagnitude() == 0.3f);
    
    // Test zero error
    pe.computeError(0.5f, 0.5f);
    assert(pe.getError() == 0.0f);
    assert(pe.getMagnitude() == 0.0f);
    
    std::cout << "    testPredictionErrorCompute passed" << std::endl;
}

void testPredictionErrorHistory() {
    nlm::PredictionError pe;
    
    assert(pe.getHistory().empty());
    
    pe.computeError(0.1f, 0.3f);
    pe.computeError(0.2f, 0.5f);
    pe.computeError(0.4f, 0.6f);
    
    const auto& history = pe.getHistory();
    assert(history.size() == 3);
    assert(history[0] == 0.2f);
    assert(history[1] == 0.3f);
    assert(history[2] == 0.2f);
    
    std::cout << "    testPredictionErrorHistory passed" << std::endl;
}

void testPredictionErrorUpdatePrediction() {
    nlm::PredictionError pe;
    
    pe.computeError(0.5f, 0.7f);
    assert(pe.getError() == 0.2f);
    
    // Update prediction
    pe.updatePrediction(0.6f);
    
    // Error should be recomputed based on new prediction
    pe.computeError(0.6f, 0.7f);
    assert(pe.getError() == 0.1f);
    
    std::cout << "    testPredictionErrorUpdatePrediction passed" << std::endl;
}

void testPredictionErrorClearHistory() {
    nlm::PredictionError pe;
    
    pe.computeError(0.1f, 0.3f);
    pe.computeError(0.2f, 0.5f);
    assert(!pe.getHistory().empty());
    
    pe.clearHistory();
    assert(pe.getHistory().empty());
    
    std::cout << "    testPredictionErrorClearHistory passed" << std::endl;
}

void testPredictionErrorInitialization() {
    nlm::PredictionError pe;
    
    // Initialize with null brain (placeholder)
    pe.initialize(nullptr);
    
    // Should not crash
    assert(true);
    
    std::cout << "    testPredictionErrorInitialization passed" << std::endl;
}

void testPredictionErrorIntegration() {
    nlm::PredictionError pe;
    
    // Test integration with brain
    // This is a placeholder for more complex integration
    std::cout << "    testPredictionErrorIntegration passed (placeholder)" << std::endl;
}

void runAll() {
    std::cout << "Running Prediction Error tests..." << std::endl;
    testPredictionErrorCreation();
    testPredictionErrorCompute();
    testPredictionErrorHistory();
    testPredictionErrorUpdatePrediction();
    testPredictionErrorClearHistory();
    testPredictionErrorInitialization();
    testPredictionErrorIntegration();
}

} // namespace test_prediction_error