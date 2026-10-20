// PredictionError Tests
#include "neuromodulation/PredictionError.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_prediction_error {

void testPredictionErrorCreation() {
    nlm::PredictionError error;
    
    assert(error.getError() == 0.0f);
    assert(error.getHistory().empty());
    assert(error.getMagnitude() == 0.0f);
    
    std::cout << "    testPredictionErrorCreation passed" << std::endl;
}

void testPredictionErrorInitialization() {
    nlm::PredictionError error;
    
    // Test initialization (just ensure no crash)
    error.initialize(nullptr);
    
    std::cout << "    testPredictionErrorInitialization passed" << std::endl;
}

void testPredictionErrorValue() {
    nlm::PredictionError error;
    
    // Test initial error
    assert(error.getError() == 0.0f);
    assert(error.getMagnitude() == 0.0f);
    
    // Compute error
    error.computeError(0.5f, 0.8f);  // predicted=0.5, actual=0.8
    
    float computedError = error.getError();
    assert(computedError >= 0.0f);
    
    // Update prediction
    error.updatePrediction(0.7f);
    
    // Error magnitude should reflect the difference
    float magnitude = error.getMagnitude();
    assert(magnitude >= 0.0f);
    
    std::cout << "    testPredictionErrorValue passed" << std::endl;
}

void testPredictionErrorHistory() {
    nlm::PredictionError error;
    
    assert(error.getHistory().empty());
    
    // Compute some errors
    error.computeError(0.1f, 0.2f);
    assert(error.getHistory().size() == 1);
    
    error.computeError(0.3f, 0.6f);
    assert(error.getHistory().size() == 2);
    
    // Clear history
    error.clearHistory();
    assert(error.getHistory().empty());
    
    std::cout << "    testPredictionErrorHistory passed" << std::endl;
}

void testPredictionErrorMagnitude() {
    nlm::PredictionError error;
    
    // Test with identical predicted and actual (error should be 0)
    error.computeError(0.5f, 0.5f);
    assert(error.getMagnitude() == 0.0f);
    
    // Test with different values
    error.computeError(0.0f, 1.0f);
    float magnitude = error.getMagnitude();
    assert(magnitude >= 0.0f);
    assert(magnitude <= 1.0f);  // Should be bounded
    
    std::cout << "    testPredictionErrorMagnitude passed" << std::endl;
}

void testPredictionErrorIntegration() {
    nlm::PredictionError error;
    
    // Test multiple error computations
    std::vector<std::pair<float, float>> testCases = {
        {0.0f, 0.1f},
        {0.5f, 0.5f},
        {1.0f, 0.0f},
        {0.2f, 0.8f},
        {0.9f, 0.1f}
    };
    
    for (const auto& testCase : testCases) {
        error.computeError(testCase.first, testCase.second);
        
        float errorValue = error.getError();
        float magnitude = error.getMagnitude();
        
        assert(errorValue >= 0.0f);
        assert(magnitude >= 0.0f);
    }
    
    // Update prediction with each computed error
    for (float err : error.getHistory()) {
        error.updatePrediction(err);
    }
    
    std::cout << "    testPredictionErrorIntegration passed" << std::endl;
}

void testPredictionErrorZeroError() {
    nlm::PredictionError error;
    
    // Compute zero error
    error.computeError(0.5f, 0.5f);
    assert(error.getError() == 0.0f);
    assert(error.getMagnitude() == 0.0f);
    
    // Update prediction with zero error
    error.updatePrediction(0.0f);
    
    // Should still have zero magnitude
    assert(error.getMagnitude() == 0.0f);
    
    std::cout << "    testPredictionErrorZeroError passed" << std::endl;
}

void testPredictionErrorNegativePrediction() {
    nlm::PredictionError error;
    
    // Test with potentially negative values
    error.computeError(-0.5f, 0.5f);
    
    float computedError = error.getError();
    float magnitude = error.getMagnitude();
    
    assert(computedError >= 0.0f);
    assert(magnitude >= 0.0f);
    
    std::cout << "    testPredictionErrorNegativePrediction passed" << std::endl;
}

void runAll() {
    testPredictionErrorCreation();
    testPredictionErrorInitialization();
    testPredictionErrorValue();
    testPredictionErrorHistory();
    testPredictionErrorMagnitude();
    testPredictionErrorIntegration();
    testPredictionErrorZeroError();
    testPredictionErrorNegativePrediction();
}

} // namespace test_prediction_error
