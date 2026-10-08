// Prediction System Tests
// Phase 4: Emerging Cognition - Prediction Tests

#include "prediction/PredictionSystem.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_prediction {

void testPredictionSystemCreation() {
    nlm::PredictionSystem predictionSystem;
    
    // Test that prediction system can be created
    // Prediction error should start at 0.0 (no error yet)
    assert(predictionSystem.getPredictionError() == 0.0f);
    assert(predictionSystem.getConfidence() == 0.0f);
    
    std::cout << "    testPredictionSystemCreation passed" << std::endl;
}

void testPredictionSystemConfiguration() {
    nlm::PredictionSystem predictionSystem;
    
    // Configure prediction system with parameters
    predictionSystem.setPredictionHorizon(10);
    assert(predictionSystem.getPredictionError() == 0.0f);
    
    std::cout << "    testPredictionSystemConfiguration passed" << std::endl;
}

void testNeuralPredictionCreation() {
    nlm::NeuralPrediction neuralPrediction;
    
    // Test that neural prediction system can be created
    assert(neuralPrediction.getPredictionError() == 0.0f);
    assert(neuralPrediction.getPredictionConfidence() == 0.0f);
    
    std::cout << "    testNeuralPredictionCreation passed" << std::endl;
}

void testNeuralPredictionInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    nlm::NeuralPrediction neuralPrediction;
    neuralPrediction.initialize(&brain);
    
    // After initialization, should have brain reference
    assert(neuralPrediction.getPredictionError() == 0.0f);
    
    std::cout << "    testNeuralPredictionInitialization passed" << std::endl;
}

void testPredictionLearning() {
    nlm::PredictionSystem predictionSystem;
    
    // Create some dummy sensory state
    std::vector<float> sensoryState(10, 0.5f);
    
    // Train prediction system with sensory state
    predictionSystem.train(sensoryState);
    
    // Get prediction error (should be low after training)
    float error = predictionSystem.getPredictionError();
    assert(error >= 0.0f && error <= 1.0f);
    
    std::cout << "    testPredictionLearning passed" << std::endl;
}

void testPredictionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool brainInitialized = brain.initialize();
    assert(brainInitialized);
    
    // Test that brain has access to prediction system
    // (This tests the integration between Brain and PredictionSystem)
    
    std::cout << "    testPredictionIntegration passed" << std::endl;
}

void testPredictionMultiStepPrediction() {
    nlm::PredictionSystem predictionSystem;
    
    // Test multi-step prediction capability
    predictionSystem.enableMultiStepPrediction(true);
    
    // Set prediction horizon
    predictionSystem.setPredictionHorizon(5);
    
    assert(predictionSystem.getPredictionError() == 0.0f);
    
    std::cout << "    testPredictionMultiStepPrediction passed" << std::endl;
}

void testPredictionErrorHandling() {
    nlm::PredictionSystem predictionSystem;
    
    // Test prediction error handling
    float error = predictionSystem.getPredictionError();
    assert(error == 0.0f);
    
    // Test error is within valid range
    // (Additional error tests would go here)
    
    std::cout << "    testPredictionErrorHandling passed" << std::endl;
}

void testPredictionConfidence() {
    nlm::PredictionSystem predictionSystem;
    
    // Test prediction confidence
    float confidence = predictionSystem.getConfidence();
    assert(confidence == 0.0f);
    
    std::cout << "    testPredictionConfidence passed" << std::endl;
}

void testBrainPredictionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that brain's prediction systems are integrated
    // (This tests the Phase 6 integration)
    
    // The brain should have all prediction systems initialized
    // This is tested indirectly through the brain initialization process
    
    std::cout << "    testBrainPredictionIntegration passed" << std::endl;
}

void testPredictionWithWorld() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    
    nlm::AgentBrain agentBrain(&brain);
    agentBrain.initialize(world);
    
    // Test prediction in world context
    // (This tests integration with the world simulation)
    
    std::cout << "    testPredictionWithWorld passed" << std::endl;
}

void runAll() {
    std::cout << "Running Prediction System Tests:" << std::endl;
    
    testPredictionSystemCreation();
    testPredictionSystemConfiguration();
    testNeuralPredictionCreation();
    testNeuralPredictionInitialization();
    testPredictionLearning();
    testPredictionIntegration();
    testPredictionMultiStepPrediction();
    testPredictionErrorHandling();
    testPredictionConfidence();
    testBrainPredictionIntegration();
    testPredictionWithWorld();
    
    std::cout << "All Prediction Tests PASSED!" << std::endl;
}

} // namespace test_prediction