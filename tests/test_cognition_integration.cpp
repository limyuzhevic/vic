// Cognition Integration Tests for Phase 6 NLM Brain
// Tests attention, planning, and concept formation integration

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_cognition_integration {

void testAttentionSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get attention system
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    // Test attention system updates
    attention->update(0.01f);
    
    // Test attention parameters
    attention->setInhibitionStrength(0.5f);
    attention->setExcitationStrength(1.5f);
    
    assert(attention->getInhibitionStrength() == 0.5f);
    assert(attention->getExcitationStrength() == 1.5f);
    
    // Test attention processing
    std::vector<nlm::NeuronId> competitors = {1, 2, 3, 4, 5};
    attention->processCompetition(competitors);
    
    std::cout << "    testAttentionSystemIntegration passed" << std::endl;
}

void testPlanningSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get planning system
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Test planning system initialization
    planner->setPlanningDepth(3);
    
    assert(planner->getPlanningDepth() == 3);
    
    // Test planning with action selection
    std::vector<nlm::ActionType> actions = {
        nlm::ActionType::Wait,
        nlm::ActionType::MoveForward,
        nlm::ActionType::TurnLeft,
        nlm::ActionType::TurnRight
    };
    
    // The planner would select an action based on neural activity
    // For now, just verify the system exists and has basic functionality
    
    std::cout << "    testPlanningSystemIntegration passed" << std::endl;
}

void testConceptFormationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get concept formation system
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test concept formation with neural activity patterns
    std::vector<float> activityPattern = {0.1f, 0.5f, 0.9f, 0.3f, 0.7f, 0.2f, 0.8f, 0.4f};
    
    // Update concepts based on activity
    conceptFormation->updateConcepts(activityPattern);
    
    // Test concept formation with attention
    std::vector<nlm::NeuronId> attentionNeurons = {1, 3, 5};
    conceptFormation->updateAttention(attentionNeurons, activityPattern);
    
    std::cout << "    testConceptFormationIntegration passed" << std::endl;
}

void testCognitionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get all cognition systems
    auto* attention = brain.getAttention();
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    
    assert(attention != nullptr);
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    
    // Test integrated cognition
    // Attention influences planning
    attention->update(0.01f);
    
    // Planning influences concept formation
    planner->setPlanningDepth(5);
    
    // Concept formation integrates attention and activity
    std::vector<float> activity = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
    conceptFormation->updateConcepts(activity);
    
    std::cout << "    testCognitionIntegration passed" << std::endl;
}

} // namespace test_cognition_integration
