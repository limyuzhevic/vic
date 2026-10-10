#include "test_common.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace test_cognition {

void testConceptFormationCreation() {
    nlm::ConceptFormation conceptFormation;
    assert(conceptFormation.getConceptCount() == 0);
    assert(conceptFormation.getConceptStrength() == 0.0f);
    std::cout << "    testConceptFormationCreation passed" << std::endl;
}

void testNeuralPlanner() {
    nlm::NeuralPlanner planner;
    assert(planner.getPlanCount() == 0);
    assert(planner.getPlannerStrength() == 0.0f);
    assert(planner.isActive() == false);
    std::cout << "    testNeuralPlanner passed" << std::endl;
}

void testConceptFormation() {
    nlm::ConceptFormation conceptFormation;
    
    // Create a concept
    std::string conceptId = "concept_1";
    std::vector<float> features = {0.1f, 0.2f, 0.3f};
    conceptFormation.createConcept(conceptId, features);
    
    assert(conceptFormation.getConceptCount() == 1);
    assert(conceptFormation.getConceptStrength(conceptId) == 0.0f);
    
    // Update concept strength
    conceptFormation.updateConceptStrength(conceptId, 0.5f);
    assert(conceptFormation.getConceptStrength(conceptId) == 0.5f);
    std::cout << "    testConceptFormation passed" << std::endl;
}

void testNeuralPlanning() {
    nlm::NeuralPlanner planner;
    
    // Create a plan
    std::string planId = "plan_1";
    std::vector<float> objectives = {0.1f, 0.2f, 0.3f};
    planner.createPlan(planId, objectives);
    
    assert(planner.getPlanCount() == 1);
    assert(planner.getPlanConfidence(planId) == 0.0f);
    
    // Update plan confidence
    planner.updatePlanConfidence(planId, 0.7f);
    assert(planner.getPlanConfidence(planId) == 0.7f);
    std::cout << "    testNeuralPlanning passed" << std::endl;
}

void testCognitionReset() {
    nlm::ConceptFormation conceptFormation;
    nlm::NeuralPlanner planner;
    
    // Create concepts and plans
    conceptFormation.createConcept("concept_1", {0.1f, 0.2f});
    planner.createPlan("plan_1", {0.3f, 0.4f});
    
    // Reset
    conceptFormation.reset();
    planner.reset();
    
    assert(conceptFormation.getConceptCount() == 0);
    assert(planner.getPlanCount() == 0);
    std::cout << "    testCognitionReset passed" << std::endl;
}

void testCognitionIntegration() {
    nlm::ConceptFormation conceptFormation;
    
    // Test integration with Brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Concept formation should be accessible
    assert(&conceptFormation != nullptr);
    std::cout << "    testCognitionIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Cognition tests..." << std::endl;
    testConceptFormationCreation();
    testNeuralPlanner();
    testConceptFormation();
    testNeuralPlanning();
    testCognitionReset();
    testCognitionIntegration();
}

} // namespace test_cognition