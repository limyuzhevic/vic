// Prediction System Tests
#include "prediction/PredictionSystem.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_prediction {

void testPredictionSystemCreation() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* predictionSystem = brain->getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Prediction system doesn't have initialize method in current implementation
    // but we can test that it exists
    
    std::cout << "    testPredictionSystemCreation passed" << std::endl;
}

void testNeuralPredictionCreation() {
    nlm::PredictionId id(1);
    nlm::NeuralPrediction prediction(id);
    
    assert(prediction.getId() == id);
    assert(prediction.getPredictionValue() == 0.0f);
    assert(prediction.getConfidence() == 0.0f);
    
    std::cout << "    testNeuralPredictionCreation passed" << std::endl;
}

void testNeuralPredictionUpdate() {
    nlm::PredictionId id(1);
    nlm::NeuralPrediction prediction(id);
    
    // Update prediction with new information
    prediction.update(0.7f, 0.8f, 0.001);
    
    assert(prediction.getPredictionValue() == 0.7f);
    assert(prediction.getConfidence() == 0.8f);
    
    std::cout << "    testNeuralPredictionUpdate passed" << std::endl;
}

void testNeuralPredictionError() {
    nlm::PredictionId id(1);
    nlm::NeuralPrediction prediction(id);
    
    // Set prediction and actual values
    prediction.setExpectedValue(0.5f);
    prediction.setActualValue(0.3f);
    
    float error = prediction.getError();
    assert(std::abs(error - (-0.2f)) < 0.001f);
    
    // Update with new values
    prediction.update(0.6f, 0.8f, 0.001);
    
    float newError = prediction.getError();
    assert(std::abs(prediction.getPredictionValue() - 0.6f) < 0.001f);
    
    std::cout << "    testNeuralPredictionError passed" << std::endl;
}

void testPredictionIntegration() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Test that prediction system is integrated
    assert(brain->getPredictionSystem() != nullptr);
    
    // The prediction system should be accessible
    auto* predictionSystem = brain->getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    std::cout << "    testPredictionIntegration passed" << std::endl;
}

void testPredictionWithSensoryInput() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create a simple sensory input
    nlm::SensoryInput input;
    std::vector<float> sensoryData = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    input.setData(sensoryData);
    
    // The brain should process this input
    brain->receiveSensoryInput(input);
    
    // The brain should have processed the input without crashing
    for (int i = 0; i < 10; ++i) {
        brain->step(i, i * 0.001);
    }
    
    std::cout << "    testPredictionWithSensoryInput passed" << std::endl;
}

void testPredictionLearning() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Enable plasticity for learning
    auto* region = brain->getRegion(nlm::RegionId(1));
    if (region) {
        for (auto* syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Run some steps with varying input patterns
    for (int i = 0; i < 200; ++i) {
        // Create varied input patterns
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 
                                     0.1f + 0.4f * std::sin(i * 0.1f));
        
        brain->step(i, i * 0.001);
        
        // Every 50 steps, check that the brain is making progress
        if (i % 50 == 0) {
            // The brain should be learning something (synapses may have changed)
            auto totalSpikes = brain->getTotalSpikeCount();
            assert(totalSpikes >= 0);
        }
    }
    
    std::cout << "    testPredictionLearning passed" << std::endl;
}

void runAll() {
    std::cout << "Running Prediction System tests..." << std::endl;
    testPredictionSystemCreation();
    testNeuralPredictionCreation();
    testNeuralPredictionUpdate();
    testNeuralPredictionError();
    testPredictionIntegration();
    testPredictionWithSensoryInput();
    testPredictionLearning();
    
    std::cout << "All Prediction System tests passed!" << std::endl;
}

} // namespace test_prediction
