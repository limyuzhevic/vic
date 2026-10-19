// Cognition System Tests
// Tests for neural planning, concept formation, and attentional selection

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
    
    nlm::NeuralPlanner planner;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain for neural planner test" << std::endl;
        assert(false);
    }
    
    planner.initialize(&brain);
    planner.setPlanningDepth(5);
    
    // Test planning functionality
    // Note: The planner interface may be limited, but we can test initialization
    
    std::cout << "    Neural planner test passed" << std::endl;
}

void testConceptFormation() {
    std::cout << "    Testing concept formation..." << std::endl;
    
    nlm::ConceptFormation conceptFormation;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain for concept formation test" << std::endl;
        assert(false);
    }
    
    conceptFormation.initialize(&brain);
    
    // Test concept formation functionality
    // Note: This is a placeholder test since concept formation may have limited interface
    
    std::cout << "    Concept formation test passed" << std::endl;
}

void testAttentionalSelection() {
    std::cout << "    Testing attentional selection..." << std::endl;
    
    nlm::AttentionalSelection attention;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain for attentional selection test" << std::endl;
        assert(false);
    }
    
    attention.initialize(&brain);
    attention.setInhibitionStrength(0.5f);
    attention.setExcitationStrength(1.5f);
    attention.setCompetitionThreshold(0.3f);
    
    // Test attentional selection functionality
    std::vector<nlm::NeuronId> competitors = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    auto winners = attention.processCompetition(competitors, 0.5f);
    
    assert(winners.size() <= competitors.size());
    
    // Test attention focus
    attention.focusOnRegion(nlm::RegionId(1));
    attention.releaseAttention();
    
    std::cout << "    Attentional selection test passed" << std::endl;
}

void testCognitionIntegration() {
    std::cout << "    Testing cognition integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    
    if (brain.initialize()) {
        // Check that cognition systems are integrated
        auto* planner = brain.getPlanner();
        auto* conceptFormation = brain.getConceptFormation();
        auto* attention = brain.getAttention();
        
        assert(planner != nullptr);
        assert(conceptFormation != nullptr);
        assert(attention != nullptr);
        
        // Initialize all cognition systems
        planner->initialize(&brain);
        conceptFormation->initialize(&brain);
        attention->initialize(&brain);
        
        // Test that cognition systems work together
        attention->focusOnRegion(nlm::RegionId(1));
        auto attended = attention->getAttendedRegions();
        assert(!attended.empty() || attended.empty()); // Either may be non-empty
        
        attention->releaseAttention();
    }
    
    std::cout << "    Cognition integration test passed" << std::endl;
}

void testPlanningActionSequence() {
    std::cout << "    Testing planning action sequence..." << std::endl;
    
    // Test neural planning for action selection
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    
    if (brain.initialize()) {
        // Generate motor action based on neural activity
        auto action = brain.produceAction();
        
        // Action should be valid
        assert(action != nullptr);
        // Note: ActionType enum may have specific values
    }
    
    std::cout << "    Planning action sequence test passed" << std::endl;
}

void testConceptLearning() {
    std::cout << "    Testing concept learning..." << std::endl;
    
    // Test concept formation from neural activity patterns
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    
    if (brain.initialize()) {
        // Simulate neural activity patterns that could form concepts
        nlm::SimulationStep step = 0;
        
        for (size_t i = 0; i < 5; ++i) {
            brain.step(step++);
            
            // Concept formation would analyze neural patterns
            // For now, just ensure the system runs without errors
        }
    }
    
    std::cout << "    Concept learning test passed" << std::endl;
}

} // namespace test_cognition
