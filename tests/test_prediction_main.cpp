#include "prediction/PredictionSystem.hpp"
#include <iostream>

int main() {
    std::cout << "Testing NLM Predictive Coding System" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Create prediction system
    nlm::PredictionSystem predSys;
    
    // Configure prediction system
    predSys.setPredictionHorizon(10);
    predSys.setSequenceMemorySize(100);
    predSys.setPrecisionWeighting(true);
    predSys.setActiveInference(true);
    
    // Enable all prediction modalities
    predSys.enableSensoryPrediction(true);
    predSys.enableMotorPrediction(true);
    predSys.enableInternalPrediction(true);
    
    std::cout << "PredictionSystem initialized and configured" << std::endl;
    std::cout << "Prediction horizon: " << predSys.getPredictionHorizon() << std::endl;
    std::cout << "Sequence memory size: " << predSys.getSequenceMemorySize() << std::endl;
    
    // Test error history (should be empty initially)
    auto errorHistory = predSys.getErrorHistory();
    std::cout << "Initial error history size: " << errorHistory.size() << std::endl;
    
    // Clear history
    predSys.clearHistory();
    std::cout << "Error history cleared" << std::endl;
    
    // Test with a simple vision input
    nlm::Vision testVision(20, 20, 3);
    std::vector<float> testData(20 * 20 * 3, 0.5f);
    testVision.setData(testData);
    
    std::cout << "Created test vision input: " << testVision.getType() 
              << " (" << testVision.getWidth() << "x" << testVision.getHeight() 
              << "x" << testVision.getChannels() << ")" << std::endl;
    
    // Make prediction
    auto prediction = predSys.predictNextState(testVision, 0.1);
    std::cout << "Made prediction for next state" << std::endl;
    
    // Get prediction errors and confidences
    auto predictionErrors = predSys.getPredictionErrors();
    auto predictionConfidences = predSys.getPredictionConfidences();
    
    std::cout << "Prediction errors: ";
    for (size_t i = 0; i < predictionErrors.size(); ++i) {
        std::cout << predictionErrors[i];
        if (i < predictionErrors.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    
    std::cout << "Prediction confidences: ";
    for (size_t i = 0; i < predictionConfidences.size(); ++i) {
        std::cout << predictionConfidences[i];
        if (i < predictionConfidences.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    
    // Update predictions with actual observation
    predSys.updatePredictions(testVision, 0.1);
    std::cout << "Updated predictions with actual observation" << std::endl;
    
    // Get hierarchical state
    auto hierarchicalState = predSys.getHierarchicalState();
    std::cout << "Hierarchical state levels: " << hierarchicalState.size() << std::endl;
    
    // Test action selection
    std::vector<nlm::ActionType> availableActions = {
        nlm::ActionType::MOVE_LEFT,
        nlm::ActionType::MOVE_RIGHT,
        nlm::ActionType::MOVE_UP,
        nlm::ActionType::MOVE_DOWN,
        nlm::ActionType::NO_ACTION
    };
    
    nlm::ActionType selectedAction = predSys.selectAction(availableActions, testVision);
    std::cout << "Selected action: " << static_cast<int>(selectedAction) << std::endl;
    
    // Train prediction system
    predSys.train(testVision);
    std::cout << "Trained prediction system" << std::endl;
    
    std::cout << "\nPredictionSystem test completed successfully!" << std::endl;
    return 0;
}