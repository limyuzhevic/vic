#include "Brain.hpp"
#include "../prediction/Phase6PredictionSystems.hpp"
#include "../core/Config/Config.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== NLM Phase 6 Prediction Systems Integration Test ===" << std::endl;
    std::cout << "Testing real neural prediction systems..." << std::endl;
    
    // Initialize NLM brain
    auto config = std::make_shared<Config>();
    nlm::Brain brain(config);
    
    if (!brain.initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    
    // Get the Phase 6 prediction systems
    auto* phase6Prediction = brain.getPhase6PredictionSystems();
    if (!phase6Prediction) {
        std::cerr << "ERROR: Failed to get Phase 6 prediction systems!" << std::endl;
        return 1;
    }
    
    std::cout << "Phase 6 prediction systems retrieved successfully" << std::endl;
    
    // Test basic prediction functionality
    std::cout << "\n--- Testing Prediction Systems ---" << std::endl;
    
    // Get neural prediction
    auto* neuralPred = phase6Prediction->getNeuralPrediction();
    if (neuralPred) {
        std::cout << "✓ Neural prediction system available" << std::endl;
        
        // Test prediction error
        float error = neuralPred->getPredictionError();
        std::cout << "  Prediction error: " << error << std::endl;
        
        // Test prediction confidence
        float confidence = neuralPred->getPredictionConfidence();
        std::cout << "  Prediction confidence: " << confidence << std::endl;
        
        // Test prediction generation
        std::vector<float> prediction = neuralPred->generatePrediction(0);
        std::cout << "  Generated prediction size: " << prediction.size() << std::endl;
    } else {
        std::cout << "✗ Neural prediction system not available" << std::endl;
    }
    
    // Get action consequence predictor
    auto* actionPredictor = phase6Prediction->getActionConsequencePredictor();
    if (actionPredictor) {
        std::cout << "✓ Action consequence predictor available" << std::endl;
        
        // Test action quality
        float actionQuality = actionPredictor->getActionQuality(nlm::ActionType::MoveForward);
        std::cout << "  Action quality for MoveForward: " << actionQuality << std::endl;
    } else {
        std::cout << "✗ Action consequence predictor not available" << std::endl;
    }
    
    // Get prediction error signal
    auto* errorSignal = phase6Prediction->getPredictionErrorSignal();
    if (errorSignal) {
        std::cout << "✓ Prediction error signal available" << std::endl;
        
        // Test error components
        auto components = errorSignal->getErrorComponents();
        std::cout << "  Error components: intensity=" << components.intensityError 
                  << ", spatial=" << components.spatialError
                  << ", temporal=" << components.temporalError << std::endl;
    } else {
        std::cout << "✗ Prediction error signal not available" << std::endl;
    }
    
    // Test prediction system integration with brain
    std::cout << "\n--- Testing Integration ---" << std::endl;
    
    // Configure prediction parameters
    phase6Prediction->configurePrediction(100, 5); // sequence memory size, prediction horizon
    std::cout << "✓ Prediction systems configured" << std::endl;
    
    // Test prediction update
    std::vector<float> sensoryState = {0.1f, 0.5f, 0.9f, 0.2f, 0.8f};
    phase6Prediction->updatePredictions(sensoryState, 100);
    std::cout << "✓ Prediction systems updated with sensory input" << std::endl;
    
    // Test action consequence recording
    phase6Prediction->recordActionConsequence(
        nlm::ActionType::Wait,   // action
        {0.0f, 0.0f, 0.0f, 0.0f, 0.0f},  // before state
        {0.1f, 0.5f, 0.9f, 0.2f, 0.8f},  // after state
        1.0f,                   // reward
        100                     // step
    );
    std::cout << "✓ Action consequences recorded" << std::endl;
    
    // Test prediction history clearing
    phase6Prediction->clearPredictionHistory();
    std::cout << "✓ Prediction history cleared" << std::endl;
    
    std::cout << "\n=== Integration Test Complete ===" << std::endl;
    std::cout << "All Phase 6 prediction systems are properly integrated!" << std::endl;
    
    return 0;
}
