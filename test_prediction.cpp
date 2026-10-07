#include "src/prediction/PredictionSystem.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/environment/Observation.hpp"
#include <iostream>
#include <vector>

void testNeuralPrediction() {
    std::cout << "Testing Neural Prediction System Implementation" << std::endl;
    std::cout << "=" << std::endl;
    
    // Create prediction system
    nlm::PredictionSystem predSys;
    
    // Configure pattern sizes
    predSys.setCurrentSensorySize(50);
    predSys.setVisionPatternSize(16);
    predSys.setTouchPatternSize(8);
    predSys.setInternalPatternSize(4);
    
    std::cout << "Prediction system created" << std::endl;
    
    // Create initial sensory input
    auto sensory1 = std::make_unique<nlm::SensoryInput>();
    
    // Fill with test data
    std::vector<float> visionData(16, 0.5f);
    std::vector<float> touchData(8, 0.3f);
    std::vector<float> internalData(4, 0.7f);
    
    sensory1->setVision(visionData);
    sensory1->setTouch(touchData);
    sensory1->setInternal(internalData);
    
    std::cout << "Created initial sensory input" << std::endl;
    
    // Test 1: Initial prediction (should return enhanced current state)
    auto prediction1 = predSys.predictNextState(*sensory1);
    std::cout << "Test 1: Initial prediction created" << std::endl;
    
    // Test 2: Train the system with the sensory observation
    predSys.train(*sensory1);
    std::cout << "Test 2: Training completed" << std::endl;
    
    // Test 3: Generate prediction after training
    auto prediction2 = predSys.predictNextState(*sensory1);
    std::cout << "Test 3: Prediction after training created" << std::endl;
    
    // Test 4: Update with predicted vs actual (simulated)
    auto prediction3 = predSys.predictNextState(*sensory1);
    predSys.updatePredictions(*prediction3, *sensory1);
    std::cout << "Test 4: Prediction updated with actual" << std::endl;
    
    // Test 5: Check error and confidence
    float error = predSys.getPredictionError();
    float confidence = predSys.getConfidence();
    
    std::cout << std::endl;
    std::cout << "Results:" << std::endl;
    std::cout << "  Prediction error: " << error << std::endl;
    std::cout << "  Prediction confidence: " << confidence << std::endl;
    
    // Test 6: Get error history
    const auto& history = predSys.getErrorHistory();
    std::cout << "  Error history size: " << history.size() << std::endl;
    
    // Test 7: Clear history
    predSys.clearHistory();
    std::cout << "Test 7: History cleared" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Neural Prediction System test completed successfully!" << std::endl;
    std::cout << "=" << std::endl;
}

int main() {
    testNeuralPrediction();
    return 0;
}