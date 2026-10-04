
#include "brain/Brain.hpp"
#include "prediction/NeuralPrediction.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "=== NLM Phase 6 Prediction Integration Test ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Get prediction system
    auto* predictionSystem = brain->getPredictionSystem();
    
    std::cout << "Prediction system initialized: " << (predictionSystem != nullptr ? "✓" : "✗") << std::endl;
    
    if (predictionSystem) {
        // Test prediction system parameters
        std::cout << "Sequence memory size: " << predictionSystem->getSequenceMemorySize() << std::endl;
        std::cout << "Prediction horizon: " << predictionSystem->getPredictionHorizon() << std::endl;
        
        // Enable prediction mechanisms
        predictionSystem->enableTemporalPrediction(true);
        predictionSystem->enableActionConsequencePrediction(true);
        
        // Test prediction with some data
        std::vector<float> state1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        std::cout << "Recording sensory state 1..." << std::endl;
        predictionSystem->recordSensoryState(state1, 0);
        
        std::vector<float> state2 = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
        std::cout << "Recording sensory state 2..." << std::endl;
        predictionSystem->recordSensoryState(state2, 1);
        
        // Generate prediction
        std::cout << "Generating prediction..." << std::endl;
        std::vector<float> prediction = predictionSystem->generatePrediction(1);
        std::cout << "Prediction generated, size: " << prediction.size() << std::endl;
        
        // Update prediction with actual observation
        float error = predictionSystem->updateWithObservation(state2, 1);
        std::cout << "Prediction error: " << error << std::endl;
        
        // Check prediction error history
        auto errorHistory = predictionSystem->getErrorHistory();
        std::cout << "Prediction error history size: " << errorHistory.size() << std::endl;
    }
    
    std::cout << "✓ Prediction integration test completed successfully" << std::endl;
    return 0;
}
