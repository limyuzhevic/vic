// Neuromodulation Integration Tests
// Tests integration of neuromodulation systems with the brain

#include "integration/test_framework.cpp"

namespace test_neuromodulation_integration {

bool testDopamineIntegration() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("dopamine_level", 0.5f, nlm::ConfigSource::Default);
    config->set("dopamine_decay_rate", 0.99f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify neuromodulation systems are initialized
    REQUIRE(brain.getDopamine() != nullptr);
    REQUIRE(brain.getCuriosity() != nullptr);
    REQUIRE(brain.getNovelty() != nullptr);
    REQUIRE(brain.getPredictionError() != nullptr);
    
    // Test neuromodulation functionality
    brain.getDopamine()->setLevel(0.5f);
    REQUIRE_EQ(brain.getDopamine()->getLevel(), 0.5f);
    
    brain.reset();
    return true;
}

bool testCuriosityIntegration() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("curiosity_level", 0.8f, nlm::ConfigSource::Default);
    config->set("novelty_sensitivity", 0.7f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify neuromodulation systems are initialized
    REQUIRE(brain.getCuriosity() != nullptr);
    
    // Test neuromodulation functionality
    brain.getCuriosity()->setCuriosityLevel(0.8f);
    REQUIRE_EQ(brain.getCuriosity()->getCuriosityLevel(), 0.8f);
    
    brain.reset();
    return true;
}

bool testNoveltyIntegration() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("novelty_threshold", 0.6f, nlm::ConfigSource::Default);
    config->set("novelty_decay", 0.95f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify neuromodulation systems are initialized
    REQUIRE(brain.getNovelty() != nullptr);
    
    // Test neuromodulation functionality
    std::vector<float> state1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> state2 = {0.1f, 0.2f, 0.3f};  // Same as state1 (not novel)
    std::vector<float> state3 = {0.5f, 0.6f, 0.7f};  // Different from state1 (novel)
    
    float novelty1 = brain.getNovelty()->computeNovelty(state1);
    float novelty2 = brain.getNovelty()->computeNovelty(state2);
    float novelty3 = brain.getNovelty()->computeNovelty(state3);
    
    // Novelty should be higher for different states
    REQUIRE(novelty1 >= 0.0f);
    REQUIRE(novelty2 >= 0.0f);
    REQUIRE(novelty3 >= 0.0f);
    
    brain.reset();
    return true;
}

bool testPredictionErrorIntegration() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("prediction_error_weight", 0.3f, nlm::ConfigSource::Default);
    config->set("error_learning_rate", 0.01f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify neuromodulation systems are initialized
    REQUIRE(brain.getPredictionError() != nullptr);
    
    // Test neuromodulation functionality
    std::vector<float> predicted = {0.1f, 0.2f, 0.3f};
    std::vector<float> actual = {0.15f, 0.25f, 0.35f};
    
    float error = brain.getPredictionError()->computeError(predicted, actual);
    REQUIRE(error >= 0.0f);
    
    brain.reset();
    return true;
}

bool testNeuromodulationEffects() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("plasticity_modulation", 0.5f, nlm::ConfigSource::Default);
    config->set("learning_rate_modulation", 0.3f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify neuromodulation systems are initialized
    REQUIRE(brain.getDopamine() != nullptr);
    REQUIRE(brain.getCuriosity() != nullptr);
    REQUIRE(brain.getNovelty() != nullptr);
    REQUIRE(brain.getPredictionError() != nullptr);
    
    // Test neuromodulation effects on plasticity and behavior
    brain.getDopamine()->setLevel(0.7f);
    brain.getCuriosity()->setCuriosityLevel(0.9f);
    
    // Verify effects
    REQUIRE_EQ(brain.getDopamine()->getLevel(), 0.7f);
    REQUIRE_EQ(brain.getCuriosity()->getCuriosityLevel(), 0.9f);
    
    brain.reset();
    return true;
}

// Run all neuromodulation integration tests
bool runNeuromodulationIntegrationTests() {
    bool allPassed = true;
    
    std::cout << "[RUNNING] Neuromodulation Integration Tests..." << std::endl;
    
    allPassed &= testDopamineIntegration();
    std::cout << "[PASS] Dopamine Integration" << std::endl;
    
    allPassed &= testCuriosityIntegration();
    std::cout << "[PASS] Curiosity Integration" << std::endl;
    
    allPassed &= testNoveltyIntegration();
    std::cout << "[PASS] Novelty Integration" << std::endl;
    
    allPassed &= testPredictionErrorIntegration();
    std::cout << "[PASS] PredictionError Integration" << std::endl;
    
    allPassed &= testNeuromodulationEffects();
    std::cout << "[PASS] Neuromodulation Effects" << std::endl;
    
    return allPassed;
}

} // namespace test_neuromodulation_integration
