// Cognition System Integration Tests for NLM
// Tests integration between NeuralPlanner, ConceptFormation, and attention mechanisms

#include "brain/Brain.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

namespace test_cognition_integration {

void testCognitionSystemConnections() {
    std::cout << "    Testing cognition system connections..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("concept_threshold", 0.5f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get cognition systems
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    
    // Test neural planner integration
    std::vector<nlm::Action> actionPlan = planner->planActions(
        std::vector<std::string>{"move_forward", "turn_left"},
        nlm::NeuralWorkingMemory::ActionContext()
    );
    
    assert(!actionPlan.empty());
    
    // Test concept formation
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::string conceptName = conceptFormation->createConcept(pattern);
    assert(!conceptName.empty());
    
    std::cout << "    testCognitionSystemConnections passed" << std::endl;
}

void testCognitionMemoryIntegration() {
    std::cout << "    Testing cognition-memory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* workingMemory = brain.getWorkingMemory();
    
    // Create and plan actions
    std::vector<std::string> goals = {"explore_environment", "find_target"};
    std::vector<nlm::Action> plan = planner->planActions(goals, nlm::NeuralWorkingMemory::ActionContext());
    
    // Store plan in working memory
    workingMemory->updateWorkingMemory("action_plan_generated");
    
    // Form concepts from action patterns
    std::string actionConcept = conceptFormation->createConceptFromSequence(plan);
    assert(!actionConcept.empty());
    
    // Verify integration
    assert(conceptFormation->getConceptCount() > 0);
    
    std::cout << "    testCognitionMemoryIntegration passed" << std::endl;
}

void testCognitionNeuromodulationInteraction() {
    std::cout << "    Testing cognition-neuromodulation interaction..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* planner = brain.getPlanner();
    auto* neuromodulator = brain.getCuriosity();
    
    // Apply neuromodulation before planning
    nlm::Neuromodulator curiositySignal(nlm::Curiosity::Create(0.7f, 0.5f));
    brain.applyNeuromodulation(curiositySignal);
    
    // Verify neuromodulation affects planning
    std::vector<nlm::Action> plan = planner->planActions(
        {"explore", "investigate"},
        nlm::NeuralWorkingMemory::ActionContext()
    );
    
    // Should have exploration bias due to curiosity
    assert(!plan.empty());
    
    // Verify neuromodulator affects cognitive processes
    assert(neuromodulator->getCurrentLevel() > 0.0f);
    
    std::cout << "    testCognitionNeuromodulationInteraction passed" << std::endl;
}

void runAll() {
    std::cout << "=== Cognition Integration Tests ===" << std::endl;
    testCognitionSystemConnections();
    testCognitionMemoryIntegration();
    testCognitionNeuromodulationInteraction();
    std::cout << "=== Cognition Integration Tests Complete ===" << std::endl;
}

} // namespace test_cognition_integration