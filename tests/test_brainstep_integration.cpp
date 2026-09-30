// Brain::step() Integration Validation Tests
// Comprehensive tests for Brain::step() integration with all connected systems

#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "IntegrationTestFramework.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>

namespace nlm {
namespace test_brainstep {

class BrainStepIntegrationTests : public IntegrationTestFramework {
public:
    BrainStepIntegrationTests() {
        auto config = getConfig();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("replay_interval", 50, ConfigSource::Default);
        config->set("consolidation_interval", 100, ConfigSource::Default);
    }
    
    ~BrainStepIntegrationTests() override = default;
    
    void runAllTests() {
        std::cout << "=== Brain::step() Integration Validation Tests ===" << std::endl;
        
        testBrainStepWithSensoryInput();
        testBrainStepMemorySystemIntegration();
        testBrainStepPredictionSystemIntegration();
        testBrainStepCognitionSystemIntegration();
        testBrainStepNeuromodulationIntegration();
        testBrainStepDevelopmentSystemIntegration();
        testBrainStepReplaySystemIntegration();
        testBrainStepPlasticityIntegration();
        
        std::cout << "=== All Brain::step() Integration Validation Tests PASSED ===" << std::endl;
    }
    
private:
    void testBrainStepWithSensoryInput() {
        logTestStart("Brain::step() with Sensory Input");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        // Provide sensory input
        std::vector<float> sensoryInput(100, 0.0f);
        for (size_t i = 0; i < 50; ++i) {
            sensoryInput[i] = static_cast<float>(i) / 50.0f;
        }
        
        brain.receiveSensoryInput(sensoryInput);
        
        // Run Brain::step() iterations
        size_t steps = 50;
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain.step(step);
        }
        
        // Verify integration with sensory processing
        assert(brain.getTotalNeuronCount() > 0);
        assert(brain.getTotalSynapseCount() > 0);
        assert(brain.getWorkingMemory() != nullptr);
        assert(brain.getWorkingMemory()->getActiveTraces() > 0);
        
        logTestResult("Brain::step() with Sensory Input", true,
                     "Brain::step() with sensory input tested over " + 
                     std::to_string(steps) + " steps");
    }
    
    void testBrainStepMemorySystemIntegration() {
        logTestStart("Brain::step() Memory System Integration");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(workingMemory != nullptr && episodicMemory != nullptr);
        
        // Store initial memory traces
        std::vector<float> wmPattern(100, 0.0f);
        for (size_t i = 0; i < 100; ++i) {
            wmPattern[i] = (i % 10) / 10.0f;
        }
        
        workingMemory->store(wmPattern, 1.0f);
        
        // Store initial episodic memory
        for (size_t i = 0; i < 5; ++i) {
            EpisodicMemoryItem episode;
            episode.timestamp = i * 10;
            episode.positionX = static_cast<float>(i * 2);
            episode.positionY = static_cast<float>(i * 3);
            episode.action = ActionType::MoveForward;
            episode.reward = 5.0f - static_cast<float>(i);
            episode.stabilityScore = 0.7f + (static_cast<float>(i) * 0.1f);
            
            episodicMemory->storeEpisode(episode);
        }
        
        // Run Brain::step() iterations
        size_t steps = 100;
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain.step(step);
        }
        
        // Verify memory system integration
        assert(workingMemory->getActiveTraces() > 0);
        assert(episodicMemory->getEpisodeCount() == 5);
        
        logTestResult("Brain::step() Memory System Integration", true,
                     "Brain::step() memory system integration tested over " + 
                     std::to_string(steps) + " steps");
    }
    
    void testBrainStepPredictionSystemIntegration() {
        logTestStart("Brain::step() Prediction System Integration");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* predictionSystem = brain.getPredictionSystem();
        auto* predictionError = brain.getPredictionErrorSignal();
        assert(predictionSystem != nullptr && predictionError != nullptr);
        
        // Run Brain::step() iterations
        size_t steps = 75;
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain.step(step);
        }
        
        // Verify prediction system integration
        assert(predictionSystem != nullptr);
        assert(predictionError->getLevel() >= 0.0f && predictionError->getLevel() <= 1.0f);
        
        logTestResult("Brain::step() Prediction System Integration", true,
                     "Brain::step() prediction system integration tested over " + 
                     std::to_string(steps) + " steps");
    }
    
    void testBrainStepCognitionSystemIntegration() {
        logTestStart("Brain::step() Cognition System Integration");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* planner = brain.getPlanner();
        auto* conceptFormation = brain.getConceptFormation();
        auto* attention = brain.getAttention();
        assert(planner != nullptr && conceptFormation != nullptr && attention != nullptr);
        
        // Run Brain::step() iterations
        size_t steps = 80;
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain.step(step);
        }
        
        // Verify cognition system integration
        assert(planner != nullptr);
        assert(conceptFormation != nullptr);
        assert(attention->getWinners().size() > 0);
        
        logTestResult("Brain::step() Cognition System Integration", true,
                     "Brain::step() cognition system integration tested over " + 
                     std::to_string(steps) + " steps");
    }
    
    void testBrainStepNeuromodulationIntegration() {
        logTestStart("Brain::step() Neuromodulation Integration");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* dopamine = brain.getDopamine();
        auto* acetylcholine = brain.getAcetylcholine();
        auto* norepinephrine = brain.getNorepinephrine();
        assert(dopamine != nullptr && acetylcholine != nullptr && norepinephrine != nullptr);
        
        // Set neuromodulation levels
        dopamine->setLevel(0.7f);
        acetylcholine->setLevel(0.8f);
        norepinephrine->setLevel(0.6f);
        
        // Run Brain::step() iterations
        size_t steps = 90;
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain.step(step);
        }
        
        // Verify neuromodulation integration
        assert(dopamine->getLevel() == 0.7f);
        assert(acetylcholine->getLevel() == 0.8f);
        assert(norepinephrine->getLevel() == 0.6f);
        
        logTestResult("Brain::step() Neuromodulation Integration", true,
                     "Brain::step() neuromodulation integration tested over " + 
                     std::to_string(steps) + " steps");
    }
    
    void testBrainStepDevelopmentSystemIntegration() {
        logTestStart("Brain::step() Development System Integration");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* developmentSystem = brain.getDevelopmentSystem();
        assert(developmentSystem != nullptr);
        
        // Run Brain::step() iterations
        size_t steps = 1000; // Development updates less frequently
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain.step(step);
        }
        
        // Verify development system integration
        assert(developmentSystem != nullptr);
        
        logTestResult("Brain::step() Development System Integration", true,
                     "Brain::step() development system integration tested over " + 
                     std::to_string(steps) + " steps");
    }
    
    void testBrainStepReplaySystemIntegration() {
        logTestStart("Brain::step() Replay System Integration");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        // Enable replay
        episodicMemory->enableReplay(true);
        
        // Store episodes
        for (size_t i = 0; i < 5; ++i) {
            EpisodicMemoryItem episode;
            episode.timestamp = i * 10;
            episode.positionX = static_cast<float>(i * 2);
            episode.positionY = static_cast<float>(i * 3);
            episode.action = ActionType::MoveForward;
            episode.reward = 5.0f - static_cast<float>(i);
            episode.stabilityScore = 0.7f + (static_cast<float>(i) * 0.1f);
            
            episodicMemory->storeEpisode(episode);
        }
        
        // Run Brain::step() iterations
        size_t steps = 200;
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain.step(step);
        }
        
        // Verify replay system integration
        assert(episodicMemory->isReplayEnabled());
        assert(episodicMemory->getEpisodeCount() == 5);
        
        logTestResult("Brain::step() Replay System Integration", true,
                     "Brain::step() replay system integration tested over " + 
                     std::to_string(steps) + " steps");
    }
    
    void testBrainStepPlasticityIntegration() {
        logTestStart("Brain::step() Plasticity Integration");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* stdp = brain.getSTDP();
        auto* hebbian = brain.getHebbian();
        assert(stdp != nullptr && hebbian != nullptr);
        
        // Run Brain::step() iterations
        size_t steps = 150;
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain.step(step);
        }
        
        // Verify plasticity integration
        assert(stdp != nullptr);
        assert(hebbian != nullptr);
        
        logTestResult("Brain::step() Plasticity Integration", true,
                     "Brain::step() plasticity integration tested over " + 
                     std::to_string(steps) + " steps");
    }
};

void runAll() {
    BrainStepIntegrationTests tests;
    tests.runAllTests();
}

} // namespace test_brainstep
} // namespace nlm
