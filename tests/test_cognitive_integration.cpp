// Cognitive Systems Integration Tests
#include "brain/Brain.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_cognitive_integration {

void testNeuralPlannerIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get neural planner
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Create test state
    std::vector<float> testState(50, 0.5f);
    
    // Test planning
    nlm::ActionType action = planner->planAction(testState, 0.5f);
    assert(action != nlm::ActionType::Custom); // Should be a valid action
    
    // Test action quality update
    planner->setActionQuality(nlm::ActionType::MoveForward, 0.8f);
    
    // Test evaluation
    std::vector<nlm::ActionType> actions = {
        nlm::ActionType::MoveForward, nlm::ActionType::MoveBackward
    };
    auto candidate = planner->evaluateSequence(actions, testState);
    assert(candidate.actions.size() > 0);
    assert(candidate.confidence >= 0.0f);
    
    std::cout << "    testNeuralPlannerIntegration passed" << std::endl;
}

void testConceptFormationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get concept formation
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test concept creation with sensory-like pattern
    std::vector<float> pattern1(20, 0.1f);
    std::vector<float> features1(10, 0.2f);
    
    size_t conceptId1 = conceptFormation->presentExperience(pattern1, features1, 0.5f, 0);
    // Should create concept (id > 0) or return 0 if not yet formed
    if (conceptId1 > 0) {
        // Verify concept was created
        auto* concept = conceptFormation->getConcept(conceptId1);
        assert(concept != nullptr);
        assert(concept->prototype.size() == pattern1.size());
        assert(concept->avgStability >= 0.0f);
    }
    
    // Test concept recognition with similar pattern
    std::vector<float> pattern2 = pattern1;
    for (float& val : pattern2) val += 0.05f; // Similar but not identical
    
    std::vector<float> features2 = features1;
    size_t conceptId2 = conceptFormation->presentExperience(pattern2, features2, 0.3f, 1);
    // May match existing concept or create new one
    
    // Test similarity computation
    float similarity = conceptFormation->computeSimilarity(pattern1, pattern2);
    assert(similarity >= 0.0f && similarity <= 1.0f);
    
    // Test novelty detection
    std::vector<float> novelPattern(20, 0.0f);
    bool isNovel = conceptFormation->isNovel(novelPattern, 0.5f);
    assert(isNovel == true); // Should be novel (doesn't match any concepts)
    
    std::cout << "    testConceptFormationIntegration passed" << std::endl;
}

void testAttentionalSelectionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get attention system
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    // Create test neurons
    std::vector<nlm::NeuronId> neurons;
    for (size_t i = 0; i < 10; ++i) {
        neurons.push_back(nlm::NeuronId(i));
    }
    
    // Test competition processing
    auto winners = attention->processCompetition(neurons, 1.0f);
    // May have zero or more winners
    for (const auto& winner : winners) {
        assert(std::find(neurons.begin(), neurons.end(), winner) != neurons.end());
    }
    
    // Test attention weighting
    attention->applyTopDownBias(neurons[0], 0.5f);
    attention->applyBottomUpSalience(neurons[1], 0.8f);
    
    // Test excitation/inhibition values
    float inhibition = attention->getInhibitionFor(neurons[0]);
    float excitation = attention->getExcitationFor(neurons[1]);
    assert(inhibition >= 0.0f);
    assert(excitation >= 0.0f);
    
    // Test attention update
    attention->update(0.01f);
    
    // Test attended check
    bool isAttended = attention->isAttended(neurons[0]);
    assert(isAttended == (std::find(winners.begin(), winners.end(), neurons[0]) != winners.end()));
    
    std::cout << "    testAttentionalSelectionIntegration passed" << std::endl;
}

void testBrainCognitiveSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that all cognitive systems are accessible
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    // Test that cognitive systems are used in brain loop
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    // Verify brain operations completed
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainCognitiveSystemIntegration passed" << std::endl;
}

void runAll() {
    testNeuralPlannerIntegration();
    testConceptFormationIntegration();
    testAttentionalSelectionIntegration();
    testBrainCognitiveSystemIntegration();
}

} // namespace test_cognitive_integration

int main() {
    std::cout << "Running Cognitive Systems Integration Tests..." << std::endl;
    test_cognitive_integration::runAll();
    std::cout << "All Cognitive Systems Integration Tests passed!" << std::endl;
    return 0;
}
