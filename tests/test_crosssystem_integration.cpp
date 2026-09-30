// Cross-System Integration Tests
// Tests for all systems working together in Brain::step()

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
namespace test_crosssystem {

class CrossSystemIntegrationTests : public IntegrationTestFramework {
public:
    CrossSystemIntegrationTests() {
        auto config = getConfig();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
    }
    
    ~CrossSystemIntegrationTests() override = default;
    
    void runAllTests() {
        std::cout << "=== Cross-System Integration Tests ===" << std::endl;
        
        testAllSystemsWorkingTogetherInBrainStep();
        testMemoryConsolidationAndReplayCycles();
        testNeuromodulationEffectsOnMemoryAndLearning();
        testPredictionErrorsAndRewardProcessing();
        testAttentionAndWorkingMemoryCompetition();
        
        std::cout << "=== All Cross-System Integration Tests PASSED ===" << std::endl;
    }
    
private:
    void testAllSystemsWorkingTogetherInBrainStep() {
        logTestStart("All Systems Working Together in Brain::step()");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        // Store initial sensory input
        std::vector<float> sensoryInput(100, 0.0f);
        for (size_t i = 0; i < 50; ++i) {
            sensoryInput[i] = static_cast<float>(i) / 50.0f;
        }
        
        brain.receiveSensoryInput(sensoryInput);
        
        // Run multiple Brain::step() iterations
        size_t stepsToRun = 100;
        for (nlm::SimulationStep step = 0; step < stepsToRun; ++step) {
            brain.step(step);
        }
        
        // Verify all systems were integrated and functional
        assert(brain.getTotalNeuronCount() > 0);
        assert(brain.getTotalSynapseCount() > 0);
        assert(brain.getWorkingMemory() != nullptr);
        assert(brain.getEpisodicMemory() != nullptr);
        assert(brain.getPredictionSystem() != nullptr);
        assert(brain.getPlanner() != nullptr);
        assert(brain.getConceptFormation() != nullptr);
        assert(brain.getAttention() != nullptr);
        assert(brain.getDopamine() != nullptr);
        
        // Verify working memory was used
        auto* workingMemory = brain.getWorkingMemory();
        assert(workingMemory->getActiveTraces() > 0);
        
        // Verify episodic memory was updated
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory->getEpisodeCount() > 0);
        
        logTestResult("All Systems Working Together in Brain::step()", true,
                     "All " + std::to_string(stepsToRun) + " Brain::step() iterations completed with full system integration");
    }
    
    void testMemoryConsolidationAndReplayCycles() {
        logTestStart("Memory Consolidation and Replay Cycles");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* episodicMemory = brain.getEpisodicMemory();
        auto* workingMemory = brain.getWorkingMemory();
        auto* dopamine = brain.getDopamine();
        assert(episodicMemory != nullptr && workingMemory != nullptr && dopamine != nullptr);
        
        // Store initial episodes
        for (size_t i = 0; i < 10; ++i) {
            EpisodicMemoryItem episode;
            episode.timestamp = i * 10;
            episode.positionX = static_cast<float>(i);
            episode.positionY = static_cast<float>(i * 2);
            episode.action = ActionType::MoveForward;
            episode.reward = static_cast<float>(i % 5);
            episode.stabilityScore = 0.1f + (static_cast<float>(i) / 50.0f);
            
            episodicMemory->storeEpisode(episode);
        }
        
        // Run Brain::step() iterations to simulate consolidation cycles
        size_t consolidationSteps = 200;
        for (nlm::SimulationStep step = 0; step < consolidationSteps; ++step) {
            brain.step(step);
        }
        
        // During consolidation, episodic memory should have been updated
        assert(episodicMemory->getEpisodeCount() > 0);
        
        // Working memory should have been consolidated
        assert(workingMemory->getActiveTraces() > 0);
        
        // Neuromodulators should have been updated
        assert(dopamine->getLevel() >= 0.0f && dopamine->getLevel() <= 1.0f);
        
        logTestResult("Memory Consolidation and Replay Cycles", true,
                     "Memory consolidation cycles completed over " + 
                     std::to_string(consolidationSteps) + " steps");
    }
    
    void testNeuromodulationEffectsOnMemoryAndLearning() {
        logTestStart("Neuromodulation Effects on Memory and Learning");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* dopamine = brain.getDopamine();
        auto* acetylcholine = brain.getAcetylcholine();
        auto* episodicMemory = brain.getEpisodicMemory();
        auto* workingMemory = brain.getWorkingMemory();
        
        assert(dopamine != nullptr && acetylcholine != nullptr && 
               episodicMemory != nullptr && workingMemory != nullptr);
        
        // Set neuromodulation levels
        dopamine->setLevel(0.7f);
        acetylcholine->setLevel(0.8f);
        
        // Store episodes for memory testing
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
        
        // Store working memory patterns
        std::vector<float> wmPattern(100, 0.0f);
        for (size_t i = 0; i < 100; ++i) {
            wmPattern[i] = (i % 20) / 20.0f;
        }
        
        workingMemory->store(wmPattern, 1.5f);
        
        // Run Brain::step() iterations
        size_t neuromodulationSteps = 150;
        for (nlm::SimulationStep step = 0; step < neuromodulationSteps; ++step) {
            brain.step(step);
        }
        
        // Verify neuromodulation effects on memory and learning
        assert(dopamine->getLevel() == 0.7f); // Dopamine level maintained
        assert(acetylcholine->getLevel() == 0.8f); // Acetylcholine level maintained
        assert(episodicMemory->getEpisodeCount() == 5); // Episodes retained
        assert(workingMemory->getActiveTraces() > 0); // Working memory active
        
        logTestResult("Neuromodulation Effects on Memory and Learning", true,
                     "Neuromodulation effects tested over " + 
                     std::to_string(neuromodulationSteps) + " steps");
    }
    
    void testPredictionErrorsAndRewardProcessing() {
        logTestStart("Prediction Errors and Reward Processing");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* predictionError = brain.getPredictionErrorSignal();
        auto* dopamine = brain.getDopamine();
        auto* episodicMemory = brain.getEpisodicMemory();
        
        assert(predictionError != nullptr && dopamine != nullptr && episodicMemory != nullptr);
        
        // Set neuromodulation levels
        dopamine->setLevel(0.6f);
        
        // Store episodes
        for (size_t i = 0; i < 5; ++i) {
            EpisodicMemoryItem episode;
            episode.timestamp = i * 10;
            episode.positionX = static_cast<float>(i * 2);
            episode.positionY = static_cast<float>(i * 3);
            episode.action = ActionType::MoveForward;
            episode.reward = 5.0f - static_cast<float>(i);
            episode.stabilityScore = 0.7f + (static_cast<float>(i) * 0.1f);
            
            // Add prediction error data
            episode.predictionError = (static_cast<float>(i) / 5.0f - 0.5f) * 0.2f;
            episode.predictionConfidence = 0.8f - (static_cast<float>(i) / 10.0f);
            
            episodicMemory->storeEpisode(episode);
        }
        
        // Run Brain::step() iterations
        size_t predictionSteps = 100;
        for (nlm::SimulationStep step = 0; step < predictionSteps; ++step) {
            brain.step(step);
        }
        
        // Verify prediction error and reward processing
        assert(predictionError->getLevel() >= 0.0f && predictionError->getLevel() <= 1.0f);
        assert(dopamine->getLevel() == 0.6f);
        assert(episodicMemory->getEpisodeCount() == 5);
        
        logTestResult("Prediction Errors and Reward Processing", true,
                     "Prediction errors and reward processing tested over " + 
                     std::to_string(predictionSteps) + " steps");
    }
    
    void testAttentionAndWorkingMemoryCompetition() {
        logTestStart("Attention and Working Memory Competition");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* attention = brain.getAttention();
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(attention != nullptr && workingMemory != nullptr && episodicMemory != nullptr);
        
        // Store working memory patterns
        std::vector<float> wmPattern1(100, 0.0f);
        std::vector<float> wmPattern2(100, 0.0f);
        
        for (size_t i = 0; i < 100; ++i) {
            wmPattern1[i] = (i < 50) ? 1.0f : 0.0f;
            wmPattern2[i] = (i >= 50) ? 1.0f : 0.0f;
        }
        
        workingMemory->store(wmPattern1, 1.5f);
        workingMemory->store(wmPattern2, 0.8f);
        
        // Store episodes
        for (size_t i = 0; i < 3; ++i) {
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
        size_t competitionSteps = 80;
        for (nlm::SimulationStep step = 0; step < competitionSteps; ++step) {
            brain.step(step);
        }
        
        // Verify attention and working memory competition
        assert(attention->getWinners().size() > 0);
        assert(workingMemory->getActiveTraces() > 0);
        assert(episodicMemory->getEpisodeCount() == 3);
        
        logTestResult("Attention and Working Memory Competition", true,
                     "Attention and working memory competition tested over " + 
                     std::to_string(competitionSteps) + " steps");
    }
};

void runAll() {
    CrossSystemIntegrationTests tests;
    tests.runAllTests();
}

} // namespace test_crosssystem
} // namespace nlm
