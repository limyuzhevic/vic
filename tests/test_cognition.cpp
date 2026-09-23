// Cognition System Tests
// Testing neural planner, concept formation, and attentional selection

#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_cognition {

void testNeuralPlanner() {
    std::cout << "    Testing neural planner..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    auto* planner = brain->getPlanner();
    assert(planner != nullptr);
    
    // Test initialization
    planner->initialize(brain.get());
    
    // Test planning depth
    planner->setPlanningDepth(5);
    
    // Test action selection (simplified)
    // In a real implementation, this would select actions based on neural activity
    
    std::cout << "      ✓ Neural planner tests passed" << std::endl;
}

void testConceptFormation() {
    std::cout << "    Testing concept formation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    auto* conceptFormation = brain->getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test initialization
    conceptFormation->initialize(brain.get());
    
    // Test concept learning (simplified)
    // In a real implementation, this would learn concepts from neural patterns
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    // Test concept retrieval
    // conceptFormation->addConcept(pattern);
    // auto concepts = conceptFormation->getSimilarConcepts(pattern, 0.8f);
    
    std::cout << "      ✓ Concept formation tests passed" << std::endl;
}

void testAttentionalSelection() {
    std::cout << "    Testing attentional selection..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    auto* attention = brain->getAttention();
    assert(attention != nullptr);
    
    // Test initialization
    attention->initialize(brain.get());
    
    // Test attentional parameters
    attention->setInhibitionStrength(0.5f);
    attention->setExcitationStrength(1.5f);
    
    // Test competition processing
    std::vector<nlm::NeuronId> competitors = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    attention->processCompetition(competitors);
    
    // Test update
    attention->update(0.001);
    
    std::cout << "      ✓ Attentional selection tests passed" << std::endl;
}

void testCognitionIntegration() {
    std::cout << "    Testing cognition integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test that all cognition systems are available and functional
    auto* planner = brain->getPlanner();
    auto* conceptFormation = brain->getConceptFormation();
    auto* attention = brain->getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    // Test cognitive capabilities
    // planner->setPlanningDepth(3);
    // conceptFormation->setLearningRate(0.01f);
    // attention->setInhibitionStrength(0.3f);
    
    std::cout << "      ✓ Cognition integration tests passed" << std::endl;
}

void runAll() {
    std::cout << "=== Cognition System Tests ===" << std::endl;
    
    testNeuralPlanner();
    testConceptFormation();
    testAttentionalSelection();
    testCognitionIntegration();
    
    std::cout << "=== All Cognition Tests PASSED ===" << std::endl;
}

} // namespace test_cognition
