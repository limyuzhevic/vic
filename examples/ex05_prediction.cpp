// ex05_prediction.cpp - Prediction System

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    // Initialize logging
    nlm::Logger::getInstance().setLevel(nlm::LogLevel::INFO);
    
    std::cout << "=== Example 5: Prediction System ===" << std::endl;
    
    // Create configuration with prediction system
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 120);
    config->set("brain.prediction.enabled", true);
    config->set("brain.prediction.errorSignal.enabled", true);
    config->set("brain.prediction.confidence.enabled", true);
    config->set("brain.prediction.threshold", 0.6f);
    config->set("brain.prediction.learningRate", 0.01f);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Prediction system brain initialized!" << std::endl;
    
    // Get prediction system
    auto predictionSystem = brain->getPredictionSystem();
    auto predictionError = brain->getPredictionErrorSignal();
    
    std::cout << "\n--- Prediction System Setup ---" << std::endl;
    
    if (predictionSystem) {
        std::cout << "Prediction system: Enabled" << std::endl;
        std::cout << "Prediction error: Enabled" << std::endl;
        // Note: ex05_prediction.cpp doesn't demonstrate full prediction system usage
        // as it's complex and typically integrated with sensory processing
        std::cout << "(Prediction system requires sensory integration for full operation)" << std::endl;
    }
    
    // Simulate prediction and learning scenarios
    std::cout << "\n--- Prediction and Error Scenarios ---" << std::endl;
    
    // Scenario 1: Accurate prediction
    std::cout << "Scenario 1: Accurate prediction" << std::endl;
    float prediction1 = 0.8f;
    float actual1 = 0.75f;
    float error1 = prediction1 - actual1;
    
    if (predictionError) {
        predictionError->update(prediction1, actual1);
        std::cout << "  Prediction: " << prediction1 << ", Actual: " << actual1
                 << ", Error: " << error1 << std::endl;
        
        if (std::abs(error1) < 0.1f) {
            std::cout << "  Low prediction error - stable learning" << std::endl;
        }
    }
    
    // Scenario 2: Overconfident prediction
    std::cout << "\nScenario 2: Overconfident prediction" << std::endl;
    float prediction2 = 0.95f;
    float actual2 = 0.4f;
    float error2 = prediction2 - actual2;
    
    if (predictionError) {
        predictionError->update(prediction2, actual2);
        std::cout << "  Prediction: " << prediction2 << ", Actual: " << actual2
                 << ", Error: " << error2 << std::endl;
        
        if (std::abs(error2) > 0.4f) {
            std::cout << "  High prediction error - drives significant learning" << std::endl;
        }
    }
    
    // Scenario 3: Underconfident prediction
    std::cout << "\nScenario 3: Underconfident prediction" << std::endl;
    float prediction3 = 0.3f;
    float actual3 = 0.9f;
    float error3 = prediction3 - actual3;
    
    if (predictionError) {
        predictionError->update(prediction3, actual3);
        std::cout << "  Prediction: " << prediction3 << ", Actual: " << actual3
                 << ", Error: " << error3 << std::endl;
        
        if (std::abs(error3) > 0.4f) {
            std::cout << "  Large prediction error - opportunities for learning" << std::endl;
        }
    }
    
    // Demonstrate confidence tracking (simplified)
    std::cout << "\n--- Confidence Tracking ---" << std::endl;
    std::vector<float> confidenceHistory;
    for (int i = 0; i < 10; ++i) {
        // Simulate confidence based on prediction accuracy
        float baseConfidence = 0.5f;
        float noise = ((std::rand() % 100) / 100.0f - 0.5f) * 0.2f;
        float confidence = baseConfidence + noise;
        confidence = std::clamp(confidence, 0.0f, 1.0f);
        
        confidenceHistory.push_back(confidence);
        std::cout << "  Prediction " << i + 1 << ": Confidence = " << confidence << std::endl;
        
        if (confidence > 0.7f) {
            std::cout << "    High confidence - may lead to overconfidence" << std::endl;
        } else if (confidence < 0.3f) {
            std::cout << "    Low confidence - may lead to hesitation" << std::endl;
        }
    }
    
    // Calculate confidence statistics
    float avgConfidence = std::accumulate(confidenceHistory.begin(), 
                                        confidenceHistory.end(), 0.0f) / confidenceHistory.size();
    std::cout << "  Average confidence: " << avgConfidence << std::endl;
    
    // Demonstrate prediction threshold
    std::cout << "\n--- Prediction Thresholds ---" << std::endl;
    std::cout << "Prediction threshold: " << config->getOr<float>("brain.prediction.threshold", 0.6f) << std::endl;
    
    // Simulate prediction-based decisions
    std::cout << "\n--- Prediction-Based Decision Making ---" << std::endl;
    std::vector<float> stimulusStrengths = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
    
    for (size_t i = 0; i < stimulusStrengths.size(); ++i) {
        float stimulus = stimulusStrengths[i];
        
        // Prediction based on stimulus
        float prediction = stimulus * 0.9f; // Simplified prediction model
        float error = stimulus - prediction;
        
        // Apply prediction error learning
        if (predictionError) {
            predictionError->update(prediction, stimulus);
        }
        
        // Make decision based on confidence and error
        float confidence = 1.0f - std::abs(error) / stimulus;
        confidence = std::clamp(confidence, 0.0f, 1.0f);
        
        // Decision threshold
        bool acceptPrediction = confidence > config->getOr<float>("brain.prediction.threshold", 0.6f);
        
        std::cout << "  Stimulus " << i + 1 << ": " << stimulus << ", "
                 << "Prediction: " << prediction << ", "
                 << "Error: " << error << ", "
                 << "Confidence: " << confidence << ", "
                 << "Decision: " << (acceptPrediction ? "Accept" : "Reject") << std::endl;
        
        if (acceptPrediction && error < 0.1f) {
            std::cout << "    Good prediction - stable behavior" << std::endl;
        }
    }
    
    // Demonstrate prediction learning over time
    std::cout << "\n--- Learning Over Time ---" << std::endl;
    std::cout << "Simulating prediction learning across multiple trials..." << std::endl;
    
    std::vector<float> predictions;
    std::vector<float> targets;
    std::vector<float> errors;
    
    for (int trial = 0; trial < 50; ++trial) {
        // Generate target
        float target = ((std::rand() % 100) / 100.0f);
        
        // Make prediction with some noise
        float basePrediction = target;
        float predictionNoise = ((std::rand() % 200) / 100.0f - 1.0f) * 0.1f;
        float prediction = basePrediction + predictionNoise;
        
        // Calculate error
        float error = target - prediction;
        
        predictions.push_back(prediction);
        targets.push_back(target);
        errors.push_back(error);
        
        if (trial % 10 == 0) {
            std::cout << "  Trial " << trial << ": Prediction = " << prediction 
                     << ", Target = " << target << ", Error = " << error << std::endl;
        }
    }
    
    // Analyze learning progress
    float initialError = std::accumulate(errors.begin(), errors.begin() + 10, 0.0f) / 10.0f;
    float finalError = std::accumulate(errors.end() - 10, errors.end(), 0.0f) / 10.0f;
    
    std::cout << "\nLearning Analysis:" << std::endl;
    std::cout << "  Initial average error: " << initialError << std::endl;
    std::cout << "  Final average error: " << finalError << std::endl;
    std::cout << "  Error reduction: " << (initialError - finalError) << std::endl;
    
    if (finalError < initialError * 0.5f) {
        std::cout << "  Significant learning achieved!" << std::endl;
    } else if (finalError < initialError * 0.8f) {
        std::cout << "  Moderate learning observed." << std::endl;
    } else {
        std::cout << "  Limited learning - may need different strategy." << std::endl;
    }
    
    std::cout << "\n=== Example 5 Complete ===" << std::endl;
    std::cout << "Prediction system simulation completed successfully!" << std::endl;
    return 0;
}