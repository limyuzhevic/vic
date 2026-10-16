// BrainCognitiveManager Comprehensive Tests
// Tests NeuralPlanner, ConceptFormation, AttentionalSelection

#include "brain/BrainCognitiveManager.hpp"
#include "brain/BrainNeuralCore.hpp"
#include "core/Config/Config.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace test_brain_cognitive_manager {

void testBrainCognitiveManagerCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::BrainCognitiveManager cognitive(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    
    assert(cognitive.initialize(&neuralCore));
    
    std::cout << "    testBrainCognitiveManagerCreation passed" << std::endl;
}

void testNeuralPlannerIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainCognitiveManager cognitive(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(cognitive.initialize(&neuralCore));
    
    // Get NeuralPlanner component
    nlm::NeuralPlanner* planner = cognitive.getPlanner();
    assert(planner != nullptr);
    
    // Test planning depth configuration
    assert(cognitive.getPlanningDepth() > 0);
    
    // Configure planning depth
    cognitive.setPlanningDepth(3);
    assert(cognitive.getPlanningDepth() == 3);
    
    std::cout << "    testNeuralPlannerIntegration passed" << std::endl;
}

void testConceptFormationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainCognitiveManager cognitive(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(cognitive.initialize(&neuralCore));
    
    // Get ConceptFormation component
    nlm::ConceptFormation* conceptFormation = cognitive.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    std::cout << "    testConceptFormationIntegration passed" << std::endl;
}

void testAttentionalSelectionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainCognitiveManager cognitive(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(cognitive.initialize(&neuralCore));
    
    // Get AttentionalSelection component
    nlm::AttentionalSelection* attentional = cognitive.getAttention();
    assert(attentional != nullptr);
    
    // Test attentional strength configuration
    assert(cognitive.getInhibitionStrength() > 0.0f);
    assert(cognitive.getExcitationStrength() > 0.0f);
    
    // Configure attentional strength
    cognitive.setInhibitionStrength(0.8f);
    cognitive.setExcitationStrength(1.2f);
    
    assert(cognitive.getInhibitionStrength() == 0.8f);
    assert(cognitive.getExcitationStrength() == 1.2f);
    
    std::cout << "    testAttentionalSelectionIntegration passed" << std::endl;
}

void testCognitiveCompetitionProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainCognitiveManager cognitive(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(cognitive.initialize(&neuralCore));
    
    // Create competitor neurons
    std::vector<nlm::NeuronId> competitors = {1, 2, 3, 4, 5};
    
    // Process competition
    cognitive.processCompetition(competitors);
    
    std::cout << "    testCognitiveCompetitionProcessing passed" << std::endl;
}

void testCognitiveCurrentActivityProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainCognitiveManager cognitive(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(cognitive.initialize(&neuralCore));
    
    // Create active neurons
    std::vector<nlm::NeuronId> activeNeurons = {1, 2, 3, 4, 5};
    
    // Process current activity
    cognitive.processCurrentActivity(activeNeurons);
    
    std::cout << "    testCognitiveCurrentActivityProcessing passed" << std::endl;
}

void testConceptFormationProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainCognitiveManager cognitive(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(cognitive.initialize(&neuralCore));
    
    // Create active neurons for concept formation
    std::vector<nlm::NeuronId> activeNeurons = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Form concepts
    cognitive.formConcepts(activeNeurons);
    
    std::cout << "    testConceptFormationProcessing passed" << std::endl;
}

void testCognitiveReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainCognitiveManager cognitive(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(cognitive.initialize(&neuralCore));
    
    // Apply some cognitive processing
    std::vector<nlm::NeuronId> competitors = {1, 2, 3};
    cognitive.processCompetition(competitors);
    
    // Reset cognitive state
    cognitive.reset();
    
    std::cout << "    testCognitiveReset passed" << std::endl;
}

void testCognitiveStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainCognitiveManager cognitive(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(cognitive.initialize(&neuralCore));
    
    // Test configuration access
    std::shared_ptr<const nlm::Config> cfg = cognitive.getConfig();
    assert(cfg != nullptr);
    
    // Test status logging
    cognitive.logStatus();
    
    std::cout << "    testCognitiveStatus passed" << std::endl;
}

void runAll() {
    std::cout << "Running BrainCognitiveManager tests..." << std::endl;
    
    testBrainCognitiveManagerCreation();
    testNeuralPlannerIntegration();
    testConceptFormationIntegration();
    testAttentionalSelectionIntegration();
    testCognitiveCompetitionProcessing();
    testCognitiveCurrentActivityProcessing();
    testConceptFormationProcessing();
    testCognitiveReset();
    testCognitiveStatus();
    
    std::cout << "\nAll BrainCognitiveManager tests passed!" << std::endl;
}

} // namespace test_brain_cognitive_manager
