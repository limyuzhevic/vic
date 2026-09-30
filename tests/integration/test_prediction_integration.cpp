// Prediction System Integration Tests
// Tests integration of prediction systems with the brain

#include "integration/test_framework.cpp"

namespace test_prediction_integration {

bool testNeuralPredictionIntegration() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("prediction_horizon", 3, nlm::ConfigSource::Default);
    config->set("prediction_accuracy_threshold", 0.8f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify prediction system is initialized
    REQUIRE(brain.getPredictionSystem() != nullptr);
    
    // Test prediction system functionality
    std::vector<float> testState = {0.1f, 0.2f, 0.3f};
    brain.getPredictionSystem()->recordSensoryState(testState, 1);
    
    auto prediction = brain.getPredictionSystem()->generatePrediction(2);
    REQUIRE(!prediction.empty());
    
    // Test prediction error computation
    float error = brain.getPredictionSystem()->updateWithObservation(testState, 3);
    REQUIRE(error >= 0.0f);
    
    brain.reset();
    return true;
}

bool testPredictionSystemInitialization() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("prediction_horizon", 5, nlm::ConfigSource::Default);
    config->set("prediction_accuracy", 0.9f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify prediction system is initialized
    REQUIRE(brain.getPredictionSystem() != nullptr);
    
    // Test prediction generation and updating
    std::vector<float> state1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> state2 = {0.2f, 0.3f, 0.4f};
    std::vector<float> state3 = {0.3f, 0.4f, 0.5f};
    
    brain.getPredictionSystem()->recordSensoryState(state1, 1);
    brain.getPredictionSystem()->recordSensoryState(state2, 2);
    
    auto pred1 = brain.getPredictionSystem()->generatePrediction(1);
    auto pred2 = brain.getPredictionSystem()->generatePrediction(2);
    
    REQUIRE(!pred1.empty());
    REQUIRE(!pred2.empty());
    
    // Test prediction error computation
    float error1 = brain.getPredictionSystem()->updateWithObservation(state2, 2);
    float error2 = brain.getPredictionSystem()->updateWithObservation(state3, 3);
    
    REQUIRE(error1 >= 0.0f);
    REQUIRE(error2 >= 0.0f);
    
    brain.reset();
    return true;
}

// Run all prediction integration tests
bool runPredictionIntegrationTests() {
    bool allPassed = true;
    
    std::cout << "[RUNNING] Prediction Integration Tests..." << std::endl;
    
    allPassed &= testNeuralPredictionIntegration();
    std::cout << "[PASS] NeuralPrediction Integration" << std::endl;
    
    allPassed &= testPredictionSystemInitialization();
    std::cout << "[PASS] PredictionSystem Initialization" << std::endl;
    
    return allPassed;
}

} // namespace test_prediction_integration
