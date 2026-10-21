// Planner Integration Tests
// Tests action planning and selection

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "motor/Action.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_planner_integration {

void testNeuralPlannerInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Planner should be accessible
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Initial state
    // Note: Planner initialization is in Brain::Impl constructor, not in NeuralPlanner::initialize
    
    std::cout << "    testNeuralPlannerInitialization passed" << std::endl;
}

void testActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Produce action
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Action should have type
    assert(action->getType() != nlm::ActionType::Unknown);
    
    std::cout << "    testActionProduction passed" << std::endl;
}

void testPlannerSequenceGeneration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Step a few times to generate actions
    std::vector<std::unique_ptr<nlm::Action>> actions;
    for (nlm::SimulationStep step = 1; step < 10; ++step) {
        brain.step(step);
        auto action = brain.produceAction();
        if (action) {
            actions.push_back(std::move(action));
        }
    }
    
    // Should have produced some actions
    assert(!actions.empty());
    
    // Actions should have valid types
    for (const auto& action : actions) {
        assert(action->getType() != nlm::ActionType::Unknown);
    }
    
    std::cout << "    testPlannerSequenceGeneration passed" << std::endl;
}

void testPlannerActionSelectionQuality() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Generate multiple action sequences
    std::vector<std::vector<nlm::ActionType>> actionSequences;
    
    for (int seq = 0; seq < 5; ++seq) {
        std::vector<nlm::ActionType> sequence;
        for (nlm::SimulationStep step = 1; step < 5; ++step) {
            brain.step(step);
            auto action = brain.produceAction();
            if (action) {
                sequence.push_back(action->getType());
            }
        }
        actionSequences.push_back(sequence);
    }
    
    // All sequences should be non-empty
    for (const auto& sequence : actionSequences) {
        assert(!sequence.empty());
    }
    
    // Sequences should have reasonable length (not crashing)
    for (const auto& sequence : actionSequences) {
        assert(sequence.size() >= 1 && sequence.size() <= 5);
    }
    
    std::cout << "    testPlannerActionSelectionQuality passed" << std::endl;
}

void testPlannerWithMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Store pattern in working memory
    std::vector<float> pattern = {0.8f, 0.6f, 0.4f, 0.2f};
    brain.getWorkingMemory()->store(pattern, 1.0f);
    
    // Create episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.sensoryState = pattern;
    episode.timestamp = 1;
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Planner should still work with memory
    for (nlm::SimulationStep step = 1; step < 15; ++step) {
        brain.step(step);
        auto action = brain.produceAction();
        assert(action != nullptr);
        
        // Memory should persist
        assert(brain.getWorkingMemory()->getActiveTraces() > 0);
        assert(brain.getEpisodicMemory()->getEpisodeCount() > 0);
    }
    
    std::cout << "    testPlannerWithMemoryIntegration passed" << std::endl;
}

void testPlannerWithPredictionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create some patterns for prediction
    std::vector<float> pattern1 = {0.5f, 0.5f, 0.5f};
    std::vector<float> pattern2 = {0.6f, 0.6f, 0.6f};
    
    // Train prediction system
    nlm::InternalSignals input1(pattern1);
    nlm::InternalSignals input2(pattern2);
    brain.getPredictionSystem()->train(input1);
    brain.getPredictionSystem()->updatePredictions(input1, input2);
    
    // Planner should still work with prediction
    for (nlm::SimulationStep step = 1; step < 10; ++step) {
        brain.step(step);
        auto action = brain.produceAction();
        assert(action != nullptr);
        
        // Prediction system should be active
        assert(brain.getPredictionSystem()->getConfidence() >= 0.0f);
    }
    
    std::cout << "    testPlannerWithPredictionIntegration passed" << std::endl;
}

void testPlannerWithAttentionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(220), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Set attention parameters
    brain.getAttention()->setInhibitionStrength(0.5f);
    brain.getAttention()->setExcitationStrength(1.5f);
    
    // Planner should work with attention
    for (nlm::SimulationStep step = 1; step < 12; ++step) {
        brain.step(step);
        auto action = brain.produceAction();
        assert(action != nullptr);
        
        // Attention should be active
        assert(brain.getAttention()->getInhibitionFor(1) >= 0.0f);
    }
    
    std::cout << "    testPlannerWithAttentionIntegration passed" << std::endl;
}

void testPlannerWithNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Apply neuromodulation
    nlm::Neuromodulator dopamine;
    dopamine.setLevel(0.7f);
    brain.applyNeuromodulation(dopamine);
    
    // Planner should work with neuromodulation
    for (nlm::SimulationStep step = 1; step < 10; ++step) {
        brain.step(step);
        auto action = brain.produceAction();
        assert(action != nullptr);
        
        // Neuromodulation should have effect
        assert(brain.getDopamine()->getLevel() >= 0.0f);
    }
    
    std::cout << "    testPlannerWithNeuromodulation passed" << std::endl;
}

void runAll() {
    testNeuralPlannerInitialization();
    testActionProduction();
    testPlannerSequenceGeneration();
    testPlannerActionSelectionQuality();
    testPlannerWithMemoryIntegration();
    testPlannerWithPredictionIntegration();
    testPlannerWithAttentionIntegration();
    testPlannerWithNeuromodulation();
}

} // namespace test_planner_integration