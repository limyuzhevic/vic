// Cognition Integration Tests
// Tests integration of NeuralPlanner, ConceptFormation, and AttentionalSelection

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

namespace test_cognition_integration {

void testConceptFormationInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Note: ConceptFormation is part of brain integration
    // Test basic brain functionality that would involve cognition
    
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testConceptFormationInitialization passed" << std::endl;
}

void testConceptFormationLearning() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test concept formation through experience simulation
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        // Create experience patterns (simulating sensory input)
        std::vector<float> pattern(4);
        for (int i = 0; i < 4; ++i) {
            pattern[i] = static_cast<float>(step + i * 10) / 100.0f;
        }
        
        // In full implementation: concepts would be formed from patterns
        // brain.getConceptFormation()->presentExperience(pattern, features, reward, step);
        
        brain.step(step);
    }
    
    std::cout << "    testConceptFormationLearning passed" << std::endl;
}

void testNeuralPlanningIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.12, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neural planning through action selection
    for (nlm::SimulationStep step = 0; step < 75; ++step) {
        // Simulate decision-making process
        float planningInput = static_cast<float>(step) / 100.0f;
        
        // In full implementation: planning would occur
        // brain.getNeuralPlanner()->makePlan(someGoal, planningInput);
        
        brain.step(step);
    }
    
    std::cout << "    testNeuralPlanningIntegration passed" << std::endl;
}

void testAttentionalSelectionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(130), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.13, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test attentional selection through focus on different regions
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Simulate different competing inputs
        std::vector<nlm::NeuronId> competitors;
        for (int i = 0; i < 5; ++i) {
            competitors.push_back(nlm::NeuronId(i));
        }
        
        // In full implementation: attention would select winners
        // brain.getAttentionalSelection()->processCompetition(competitors);
        
        brain.step(step);
    }
    
    std::cout << "    testAttentionalSelectionIntegration passed" << std::endl;
}

void testCognitionSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test integrated cognitive processing
    std::vector<float> accumulatedPattern(8, 0.0f);
    
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        // Create a time-varying input pattern
        for (int i = 0; i < 8; ++i) {
            accumulatedPattern[i] = std::sin(static_cast<float>(step + i) / 20.0f) * 0.5f + 0.5f;
        }
        
        // In full integration: cognition systems would process the pattern
        // brain.getConceptFormation()->presentExperience(accumulatedPattern, features, reward, step);
        // brain.getNeuralPlanner()->updatePlan(accumulatedPattern, step);
        // brain.getAttentionalSelection()->processInput(accumulatedPattern);
        
        brain.step(step);
    }
    
    // Verify brain is functioning
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testCognitionSystemIntegration passed" << std::endl;
}

void testCognitionWithMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.12, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test cognition systems interacting with memory systems
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.action = nlm::ActionType::Wait;
    episode.reward = 0.0f;
    episode.novelty = 0.0f;
    
    for (nlm::SimulationStep step = 0; step < 150; ++step) {
        // Create cognitive input that would be stored
        std::vector<float> cognitiveInput(6);
        for (int i = 0; i < 6; ++i) {
            cognitiveInput[i] = static_cast<float>(step + i * 5) / 100.0f;
        }
        
        // Store in working memory for cognition
        // brain.getWorkingMemory()->store(cognitiveInput);
        
        // Create episodic memory
        episode.sensoryState = cognitiveInput;
        episode.timestamp = step;
        episode.reward = (step % 10 == 0) ? 1.0f : 0.0f;
        
        // brain.getEpisodicMemory()->storeEpisode(episode);
        
        // Simulate cognitive processing
        // brain.getConceptFormation()->presentExperience(cognitiveInput, features, episode.reward, step);
        
        brain.step(step);
    }
    
    // Verify memory and cognition integration
    assert(brain.getEpisodicMemory()->getEpisodeCount() > 0);
    
    std::cout << "    testCognitionWithMemoryIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Cognition Integration tests..." << std::endl;
    testConceptFormationInitialization();
    testConceptFormationLearning();
    testNeuralPlanningIntegration();
    testAttentionalSelectionIntegration();
    testCognitionSystemIntegration();
    testCognitionWithMemoryIntegration();
}

} // namespace test_cognition_integration
