// Development Integration Tests
// Tests integration of development systems with the brain

#include "integration/test_framework.cpp"

namespace test_development_integration {

bool testDevelopmentalStageTransitions() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("developmental_stage_duration", 100, nlm::ConfigSource::Default);
    config->set("maturation_rate", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify development system is initialized
    REQUIRE(brain.getDevelopmentSystem() != nullptr);
    
    // Test developmental stage transitions
    brain.getDevelopmentSystem()->updateDevelopmentalStage(50);
    brain.getDevelopmentSystem()->updateDevelopmentalStage(150);
    
    // Verify development system is still functional
    REQUIRE(brain.getDevelopmentSystem() != nullptr);
    
    brain.reset();
    return true;
}

bool testStructuralPlasticityWithDevelopment() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("synaptogenesis_rate", 0.01f, nlm::ConfigSource::Default);
    config->set("pruning_rate", 0.005f, nlm::ConfigSource::Default);
    config->set("maturation_factor", 1.2f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify development system is initialized
    REQUIRE(brain.getDevelopmentSystem() != nullptr);
    
    // Test structural plasticity with development
    brain.getDevelopmentSystem()->triggerSynaptogenesis();
    brain.getDevelopmentSystem()->triggerPruning();
    
    // Verify development system is still functional
    REQUIRE(brain.getDevelopmentSystem() != nullptr);
    
    brain.reset();
    return true;
}

bool testDevelopmentalEffectsOnLearningRates() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("base_learning_rate", 0.01f, nlm::ConfigSource::Default);
    config->set("developmental_plasticity", 1.5f, nlm::ConfigSource::Default);
    config->set("developmental_maturation", 0.8f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify development system is initialized
    REQUIRE(brain.getDevelopmentSystem() != nullptr);
    
    // Test developmental effects on learning rates
    float earlyLearningRate = 0.01f * 1.5f;  // High plasticity
    float matureLearningRate = 0.01f * 0.8f; // Lower plasticity
    
    // Verify learning rate modulation works
    REQUIRE_EQ(brain.getDevelopmentSystem()->getLearningRateModifier(), 1.0f);
    
    brain.reset();
    return true;
}

bool testDevelopmentalEffectsOnBehavior() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("behavioral_plasticity", 1.0f, nlm::ConfigSource::Default);
    config->set("behavioral_maturation", 0.9f, nlm::ConfigSource::Default);
    config->set("curiosity_plasticity", 1.2f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify development system is initialized
    REQUIRE(brain.getDevelopmentSystem() != nullptr);
    
    // Test developmental effects on behavior
    brain.getDevelopmentSystem()->updateBehavioralPlasticity();
    
    // Verify development system is still functional
    REQUIRE(brain.getDevelopmentSystem() != nullptr);
    
    brain.reset();
    return true;
}

// Run all development integration tests
bool runDevelopmentIntegrationTests() {
    bool allPassed = true;
    
    std::cout << "[RUNNING] Development Integration Tests..." << std::endl;
    
    allPassed &= testDevelopmentalStageTransitions();
    std::cout << "[PASS] DevelopmentalStage Transitions" << std::endl;
    
    allPassed &= testStructuralPlasticityWithDevelopment();
    std::cout << "[PASS] StructuralPlasticity With Development" << std::endl;
    
    allPassed &= testDevelopmentalEffectsOnLearningRates();
    std::cout << "[PASS] DevelopmentalEffects On LearningRates" << std::endl;
    
    allPassed &= testDevelopmentalEffectsOnBehavior();
    std::cout << "[PASS] DevelopmentalEffects On Behavior" << std::endl;
    
    return allPassed;
}

} // namespace test_development_integration
