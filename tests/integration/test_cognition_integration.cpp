// Cognition System Integration Tests
// Tests integration of cognition systems with the brain

#include "integration/test_framework.cpp"

namespace test_cognition_integration {

bool testNeuralPlannerIntegration() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("planning_depth", 5, nlm::ConfigSource::Default);
    config->set("max_actions", 10, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify cognition systems are initialized
    REQUIRE(brain.getPlanner() != nullptr);
    REQUIRE(brain.getConceptFormation() != nullptr);
    REQUIRE(brain.getAttention() != nullptr);
    
    // Test cognition system configuration
    brain.getPlanner()->setPlanningDepth(5);
    brain.getConceptFormation()->setConceptCapacity(100);
    brain.getAttention()->setInhibitionStrength(0.5f);
    
    // Verify configuration
    REQUIRE_EQ(brain.getPlanner()->getPlanningDepth(), 5);
    
    brain.reset();
    return true;
}

bool testConceptFormationIntegration() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("concept_capacity", 50, nlm::ConfigSource::Default);
    config->set("similarity_threshold", 0.7f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify cognition systems are initialized
    REQUIRE(brain.getConceptFormation() != nullptr);
    
    // Test concept formation
    std::vector<float> concept1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> concept2 = {0.15f, 0.25f, 0.35f};
    
    brain.getConceptFormation()->addConcept(concept1, 1);
    brain.getConceptFormation()->addConcept(concept2, 2);
    
    // Test concept retrieval
    auto similarConcepts = brain.getConceptFormation()->findSimilarConcepts(concept1, 0.5f);
    REQUIRE_EQ(similarConcepts.size(), 2);  // Both concepts should be similar enough
    
    brain.reset();
    return true;
}

bool testAttentionalSelectionIntegration() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("attention_strength", 0.8f, nlm::ConfigSource::Default);
    config->set("inhibition_strength", 0.5f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify cognition systems are initialized
    REQUIRE(brain.getAttention() != nullptr);
    
    // Test attentional selection
    std::vector<float> inputs = {0.1f, 0.5f, 0.9f, 0.2f, 0.8f};
    brain.getAttention()->processInputs(inputs);
    
    auto attendedItems = brain.getAttention()->getAttendedItems(3);
    REQUIRE_EQ(attendedItems.size(), 3);
    
    brain.reset();
    return true;
}

bool testCognitionSystemInitialization() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("planner_depth", 3, nlm::ConfigSource::Default);
    config->set("concept_capacity", 200, nlm::ConfigSource::Default);
    config->set("attention_strength", 0.6f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify cognition systems are initialized
    REQUIRE(brain.getPlanner() != nullptr);
    REQUIRE(brain.getConceptFormation() != nullptr);
    REQUIRE(brain.getAttention() != nullptr);
    
    // Test cognitive planning and concept formation
    brain.getPlanner()->setPlanningDepth(3);
    brain.getConceptFormation()->setConceptCapacity(200);
    brain.getAttention()->setInhibitionStrength(0.6f);
    
    // Verify configuration
    REQUIRE_EQ(brain.getPlanner()->getPlanningDepth(), 3);
    
    brain.reset();
    return true;
}

// Run all cognition integration tests
bool runCognitionIntegrationTests() {
    bool allPassed = true;
    
    std::cout << "[RUNNING] Cognition Integration Tests..." << std::endl;
    
    allPassed &= testNeuralPlannerIntegration();
    std::cout << "[PASS] NeuralPlanner Integration" << std::endl;
    
    allPassed &= testConceptFormationIntegration();
    std::cout << "[PASS] ConceptFormation Integration" << std::endl;
    
    allPassed &= testAttentionalSelectionIntegration();
    std::cout << "[PASS] AttentionalSelection Integration" << std::endl;
    
    allPassed &= testCognitionSystemInitialization();
    std::cout << "[PASS] CognitionSystem Initialization" << std::endl;
    
    return allPassed;
}

} // namespace test_cognition_integration
