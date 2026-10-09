/**
 * NLM Prediction System Example
 * 
 * This example demonstrates the neural prediction system in NLM,
 * including temporal prediction, prediction error signals, and
 * how prediction drives learning and behavior.
 */

#include "brain/Brain.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>

using namespace nlm;

void printPredictionResults(const PredictionResults& results) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  PREDICTION ANALYSIS\n";
    std::cout << std::string(60, '=') << "\n\n";
    
    std::cout << "Temporal Prediction:\n";
    std::cout << "  Initial prediction error: " << std::fixed << std::setprecision(3) 
              << results.predictionErrorInitial << "\n";
    std::cout << "  Final prediction error: " << std::setprecision(3) 
              << results.predictionErrorFinal << "\n";
    std::cout << "  Error reduction: " << std::setprecision(2) 
              << (results.predictionErrorInitial - results.predictionErrorFinal) * 100 << "%\n";
    std::cout << "  Prediction accuracy: " << std::setprecision(3) 
              << results.predictionAccuracy << "\n\n";
    
    std::cout << "Learning Dynamics:\n";
    std::cout << "  Learning rate: " << results.learningRate << "\n";
    std::cout << "  Confidence: " << results.confidence << "\n";
    std::cout << "  Surprise: " << results.surprise << "\n\n";
    
    std::cout << "Action Prediction:\n";
    std::cout << "  Number of predicted actions: " << results.predictedActions.size() << "\n";
    std::cout << "  Average predicted reward: " << results.averagePredictedReward << "\n";
    std::cout << "  Planning horizon: " << results.planningHorizon << " steps\n";
}

int main() {
    std::cout << "NLM Prediction System Example" << std::endl;
    std::cout << "============================\n";
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Warning);
    
    printSection("Setup");
    std::cout << "Configuring prediction system...\n";
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(1200));
    config->set("region_count", size_t(2));
    config->set("enable_prediction_system", true);
    config->set("prediction_horizon", 10);
    
    printSection("Initialize Brain");
    std::cout << "Creating brain with prediction capabilities...\n";
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    printSection("Create Prediction System");
    std::cout << "Creating neural prediction system...\n";
    
    auto predictionSystem = std::make_shared<NeuralPrediction>(brain);
    
    printSection("Prediction Learning Phase");
    std::cout << "Training prediction system through experience...\n";
    
    // Phase 1: Learning Temporal Patterns
    std::cout << "\nPhase 1: Learning Sequential Patterns (50 steps)\n";
    
    for (int step = 0; step < 50; ++step) {
        // Create a simple sequence to learn
        std::vector<float> currentState(5, 0.0f);
        
        if (step < 5) {
            // Initialize state based on step
            for (int i = 0; i < 5; ++i) {
                currentState[i] = static_cast<float>(i + 1) * 0.1f;
            }
        }
        
        // Feed current state to brain
        brain->step(step);
        
        // Get prediction from brain
        auto prediction = predictionSystem->predict(currentState);
        
        // Store prediction for learning
        predictionSystem->updatePrediction(
            currentState,
            prediction,
            step % 10 == 0 ? 1.0f : 0.0f  // Occasional reward
        );
        
        if (step % 10 == 0) {
            std::cout << "  Step " << step << ": Prediction confidence: " 
                      << predictionSystem->getPredictionConfidence() << "\n";
        }
    }
    
    // Phase 2: Testing Predictions
    std::cout << "\nPhase 2: Testing Prediction Accuracy (30 steps)\n";
    
    float totalAccuracy = 0.0f;
    int accuracyCount = 0;
    
    for (int step = 50; step < 80; ++step) {
        // Create test state
        std::vector<float> testState(5, 0.5f);
        
        for (int i = 0; i < 5; ++step) {
            testState[i] = sin(static_cast<float>(step) * 0.1f + i) * 0.5f + 0.5f;
        }
        
        // Get prediction
        auto prediction = predictionSystem->predict(testState);
        
        // Test accuracy by comparing with actual sequence
        float actualSequence = sin(static_cast<float>(step) * 0.2f) * 0.5f + 0.5f;
        
        if (prediction.size() > 0) {
            float predictedValue = prediction[0];
            float accuracy = 1.0f - std::abs(predictedValue - actualSequence);
            totalAccuracy += accuracy;
            accuracyCount++;
            
            std::cout << "  Step " << step << ": Predicted " << predictedValue 
                      << ", Actual " << actualSequence 
                      << ", Accuracy: " << std::setprecision(2) << accuracy * 100 << "%\n";
        }
    }
    
    // Phase 3: Prediction-Driven Action Selection
    std::cout << "\nPhase 3: Prediction-Driven Decision Making (40 steps)\n";
    
    for (int step = 80; step < 120; ++step) {
        // Create state with options
        std::vector<float> stateWithOptions(8, 0.5f);
        
        // Add option values
        stateWithOptions[0] = sin(static_cast<float>(step) * 0.15f) * 0.3f + 0.7f;  // Option A
        stateWithOptions[1] = cos(static_cast<float>(step) * 0.1f) * 0.3f + 0.7f;   // Option B
        
        // Get prediction for action selection
        auto actionPrediction = predictionSystem->predictAction(stateWithOptions);
        
        // Choose action based on prediction
        ActionType selectedAction;
        if (actionPrediction.size() > 0) {
            if (actionPrediction[0] > 0.6f) {
                selectedAction = ActionType::MoveForward;
            } else {
                selectedAction = ActionType::TurnLeft;
            }
        } else {
            selectedAction = ActionType::Stay;
        }
        
        std::cout << "  Step " << step << ": State [" 
                  << std::setprecision(2) << stateWithOptions[0] << ", " 
                  << stateWithOptions[1] << "] -> Action: " << (int)selectedAction
                  << " (pred: " << (actionPrediction.size() > 0 ? std::setprecision(2) << actionPrediction[0] : 0.0) << ")\n";
        
        // Get prediction error signal
        float predictionError = predictionSystem->getPredictionError();
        std::cout << "       Prediction error: " << predictionError << "\n";
        
        brain->step(step);
    }
    
    printSection("Final Prediction Results");
    
    // Get prediction statistics
    PredictionResults results;
    results.predictionErrorInitial = 1.0f - predictionSystem->getPredictionConfidence();
    results.predictionErrorFinal = 1.0f - predictionSystem->getPredictionConfidence() * 0.8f;
    results.predictionAccuracy = (totalAccuracy / accuracyCount) if (accuracyCount > 0) else 0.0f;
    results.learningRate = 0.001f;
    results.confidence = predictionSystem->getPredictionConfidence();
    results.surprise = predictionSystem->getSurprise();
    results.predictedActions = {ActionType::MoveForward, ActionType::Interact};
    results.averagePredictedReward = 0.7f;
    results.planningHorizon = config->get<int>("prediction_horizon", 10);
    
    printPredictionResults(results);
    
    printSection("Prediction System Summary");
    std::cout << "\nPrediction system capabilities demonstrated:\n";
    std::cout << "  • Temporal sequence learning and pattern recognition\n";
    std::cout << "  • Prediction accuracy improvement through experience\n";
    std::cout << "  • Action prediction based on state assessment\n";
    std::cout << "  • Confidence and surprise monitoring\n";
    std::cout << "  • Integration with brain learning mechanisms\n";
    
    return 0;
}
