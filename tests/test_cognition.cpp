// Cognition System Tests
// Tests for planner, concept formation, attention systems

#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_cognition {

void testNeuralPlanner() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::NeuralPlanner planner(config);
    
    // Test basic operations
    assert(planner.getHorizon() > 0);
    assert(planner.getExplorationRate() >= 0.0f && planner.getExplorationRate() <= 1.0f);
    
    // Test action planning
    std::vector<float> state{0.5f, 0.3f, 0.8f};
    std::vector<float> action = planner.planAction(state);
    
    assert(action.size() > 0);
    
    std::cout << "    testNeuralPlanner passed" << std::endl;
}

void testConceptFormation() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::ConceptFormation concepts(config);
    
    // Test basic operations
    assert(concepts.getFormationRate() > 0.0f);
    
    // Test concept learning
    std::vector<float> pattern{0.1f, 0.2f, 0.3f};
    concepts.learnConcept(pattern);
    
    assert(concepts.getConceptCount() > 0);
    
    std::cout << "    testConceptFormation passed" << std::endl;
}

void testAttentionalSelection() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::AttentionalSelection attention(config);
    
    // Test basic operations
    assert(attention.getCompetitionStrength() >= 0.0f && attention.getCompetitionStrength() <= 1.0f);
    assert(attention.getSelectionThreshold() >= 0.0f);
    
    // Test attention allocation
    std::vector<float> stimuli{0.5f, 0.8f, 0.3f, 0.9f};
    size_t selected = attention.selectFocus(stimuli);
    
    assert(selected < stimuli.size());
    
    std::cout << "    testAttentionalSelection passed" << std::endl;
}

void runAll() {
    testNeuralPlanner();
    testConceptFormation();
    testAttentionalSelection();
}

} // namespace test_cognition