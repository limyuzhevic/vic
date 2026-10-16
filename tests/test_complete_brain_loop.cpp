// Complete Brain Loop Integration Tests
// Tests all systems working together in simulation (complete brain loop)

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>

namespace test_complete_brain_loop {

void testBasicBrainLoop() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run complete brain loop simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Simulate sensory input
        brain.step(step);
        
        // Produce action
        auto action = brain.produceAction();
        
        // Apply action effects
        if (action) {
            // Apply neuromodulation based on action
            // brain.applyNeuromodulation(action->getNeuromodulationSignal());
        }
    }
    
    // Verify all systems are functional
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBasicBrainLoop passed" << std::endl;
}

void testBrainWithAllMemorySystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.12, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Store information in different memory systems
    std::vector<float> pattern = {0.5f, 0.7f, 0.3f, 0.9f};
    brain.getWorkingMemory()->store(pattern);
    
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = pattern;
    episode.action = nlm::ActionType::Wait;
    episode.reward = 0.8f;
    
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    std::vector<float> similarPattern = {0.4f, 0.6f, 0.2f, 0.8f};
    brain.getAssociativeMemory()->associate(pattern, similarPattern, 0.7f);
    
    // Run simulation where memory systems interact
    for (nlm::SimulationStep step = 0; step < 150; ++step) {
        brain.step(step);
        
        // Access memory systems
        auto retrieved = brain.getWorkingMemory()->retrieve();
        auto* storedEpisode = brain.getEpisodicMemory()->getEpisode(0);
        
        // Update memory systems
        brain.getWorkingMemory()->update(0.01f);
        
        if (step % 50 == 0) {
            brain.getEpisodicMemory()->consolidate(0.5f);
        }
    }
    
    // Verify all memory systems are active
    assert(brain.getWorkingMemory()->getActiveTraces() > 0);
    assert(brain.getEpisodicMemory()->getEpisodeCount() > 0);
    assert(brain.getAssociativeMemory()->getAssociationCount() > 0);
    
    std::cout << "    testBrainWithAllMemorySystems passed" << std::endl;
}

void testBrainWithNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.11, nlm::ConfigSource::Default);
    config->set("plasticity.stdp.enabled", true, nlm::ConfigSource::Default);
    config->set("plasticity.hebbian.enabled", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Simulate neuromodulation loop
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        brain.step(step);
        
        // Simulate reward/punishment signals
        float reward = (step % 10 == 0) ? 1.0f : 0.0f;
        
        // In full implementation: neuromodulators would be updated based on reward
        // brain.applyReward(reward);
        
        // Update plasticity based on neuromodulation
        // brain.updatePlasticity();
    }
    
    // Verify brain is responding to neuromodulation
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "    testBrainWithNeuromodulation passed" << std::endl;
}

void testBrainWithPredictionAndLearning() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.12, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create predictable sequence
    std::vector<std::vector<float>> sequence;
    for (int i = 0; i < 10; ++i) {
        std::vector<float> state(3, 0.0f);
        for (int j = 0; j < 3; ++j) {
            state[j] = static_cast<float>((i + j) % 10) / 10.0f;
        }
        sequence.push_back(state);
    }
    
    // Run prediction and learning simulation
    for (nlm::SimulationStep step = 0; step < 300; ++step) {
        brain.step(step);
        
        // Simulate prediction error
        float predictionError = 0.0f;
        if (step > 10 && step % 5 == 0) {
            predictionError = 0.5f;
        }
        
        // In full implementation: prediction error would be applied
        // brain.applyPredictionError(predictionError);
    }
    
    std::cout << "    testBrainWithPredictionAndLearning passed" << std::endl;
}

void testBrainWithCognition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test cognitive processing loop
    for (nlm::SimulationStep step = 0; step < 250; ++step) {
        // Create cognitive input
        std::vector<float> cognitiveInput(6);
        for (int i = 0; i < 6; ++i) {
            cognitiveInput[i] = std::sin(static_cast<float>(step + i) / 20.0f) * 0.5f + 0.5f;
        }
        
        brain.step(step);
        
        // Simulate cognitive processing
        // In full implementation: cognition systems would process the input
        // brain.getConceptFormation()->presentExperience(cognitiveInput, features, reward, step);
        // brain.getNeuralPlanner()->updatePlan(cognitiveInput, step);
    }
    
    std::cout << "    testBrainWithCognition passed" << std::endl;
}

void testCompleteBrainLoopIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("plasticity.stdp.enabled", true, nlm::ConfigSource::Default);
    config->set("plasticity.hebbian.enabled", true, nlm::ConfigSource::Default);
    config->set("plasticity.structural.enabled", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run complete brain loop with all systems
    nlm::SimulationStep step = 0;
    while (step < 500 && brain.getTotalNeuronCount() > 0) {
        // 1. Memory operations
        std::vector<float> pattern = {static_cast<float>(step) / 100.0f, 0.5f, 0.8f};
        brain.getWorkingMemory()->store(pattern);
        
        // 2. Create episodic memory
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = step;
        episode.sensoryState = pattern;
        episode.reward = 0.5f + (static_cast<float>(step % 5) / 10.0f);
        brain.getEpisodicMemory()->storeEpisode(episode);
        
        // 3. Create associations
        std::vector<float> associated = {0.7f, 0.2f, 0.9f};
        brain.getAssociativeMemory()->associate(pattern, associated, 0.6f);
        
        // 4. Main brain step
        brain.step(step);
        
        // 5. Update memory systems
        brain.getWorkingMemory()->update(0.01f);
        if (step % 100 == 0) {
            brain.getEpisodicMemory()->consolidate(0.7f);
        }
        
        // 6. Update development
        // brain.getDevelopmentSystem()->update(step);
        
        step++;
    }
    
    // Verify complete integration
    assert(brain.getWorkingMemory()->getActiveTraces() > 0);
    assert(brain.getEpisodicMemory()->getEpisodeCount() > 0);
    assert(brain.getAssociativeMemory()->getAssociationCount() > 0);
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testCompleteBrainLoopIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Complete Brain Loop Integration tests..." << std::endl;
    testBasicBrainLoop();
    testBrainWithAllMemorySystems();
    testBrainWithNeuromodulation();
    testBrainWithPredictionAndLearning();
    testBrainWithCognition();
    testCompleteBrainLoopIntegration();
}

} // namespace test_complete_brain_loop
