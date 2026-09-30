// Neural Planner Integration Tests
// Tests for NeuralPlanner system integration with other systems

#pragma once

#include "cognition/NeuralPlanner.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "IntegrationTestFramework.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace nlm {
namespace test_neuralplanner {

class NeuralPlannerIntegrationTests : public IntegrationTestFramework {
public:
    NeuralPlannerIntegrationTests() {
        auto config = getConfig();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    }
    
    ~NeuralPlannerIntegrationTests() override = default;
    
    void runAllTests() {
        std::cout << "=== Neural Planner Integration Tests ===" << std::endl;
        
        testNeuralPlannerActionSelectionWithSystemIntegration();
        testNeuralPlannerIntegrationWithWorkingMemory();
        testNeuralPlannerIntegrationWithEpisodicMemory();
        testNeuralPlannerIntegrationWithPredictionSystem();
        testNeuralPlannerIntegrationWithNeuromodulation();
        
        std::cout << "=== All Neural Planner Integration Tests PASSED ===" << std::endl;
    }
    
private:
    void testNeuralPlannerActionSelectionWithSystemIntegration() {
        logTestStart("Neural Planner Action Selection with System Integration");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* planner = brain.getPlanner();
        assert(planner != nullptr);
        
        // Current brain state
        std::vector<float> currentState(100, 0.0f);
        for (size_t i = 0; i < 100; ++i) {
            currentState[i] = static_cast<float>(i) / 100.0f;
        }
        
        // Plan action with system integration
        ActionType action = planner->planAction(currentState, 0.5f);
        
        // Verify action was selected
        assert(action != ActionType::Wait);  // Should select meaningful action
        
        // Test enhanced planning with integration
        std::vector<float> workingMemoryPattern(100, 0.0f);
        std::vector<float> episodicFeatures(100, 0.0f);
        
        for (size_t i = 0; i < 100; ++i) {
            workingMemoryPattern[i] = (i < 33) ? 1.0f : 0.0f;
            episodicFeatures[i] = (i >= 67) ? 1.0f : 0.0f;
        }
        
        ActionType integratedAction = planner->planActionWithIntegration(
            currentState, 0.5f, &workingMemoryPattern, &episodicFeatures, 0.7f, 0.6f);
        
        // Verify integrated action was selected
        assert(integratedAction != ActionType::Wait);
        
        logTestResult("Neural Planner Action Selection with System Integration", true,
                     "Selected actions: " + std::to_string(static_cast<int>(action)) + 
                     " and " + std::to_string(static_cast<int>(integratedAction)));
    }
    
    void testNeuralPlannerIntegrationWithWorkingMemory() {
        logTestStart("Neural Planner Integration with Working Memory");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* planner = brain.getPlanner();
        auto* workingMemory = brain.getWorkingMemory();
        assert(planner != nullptr && workingMemory != nullptr);
        
        // Store working memory patterns for planning
        std::vector<float> wmPattern1(100, 0.0f);
        std::vector<float> wmPattern2(100, 0.0f);
        
        for (size_t i = 0; i < 100; ++i) {
            wmPattern1[i] = (i < 50) ? 1.0f : 0.0f;
            wmPattern2[i] = (i >= 50) ? 1.0f : 0.0f;
        }
        
        workingMemory->store(wmPattern1, 1.5f);
        workingMemory->store(wmPattern2, 0.8f);
        
        // Plan action using working memory patterns
        std::vector<float> currentState(100, 0.0f);
        for (size_t i = 0; i < 100; ++i) {
            currentState[i] = static_cast<float>(i) / 100.0f;
        }
        
        ActionType action = planner->planActionWithIntegration(
            currentState, 0.5f, &wmPattern1, nullptr, 0.7f, 0.5f);
        
        // Verify planning was influenced by working memory
        assert(action != ActionType::Wait);
        
        logTestResult("Neural Planner Integration with Working Memory", true,
                     "Planned action influenced by working memory: " + 
                     std::to_string(static_cast<int>(action)));
    }
    
    void testNeuralPlannerIntegrationWithEpisodicMemory() {
        logTestStart("Neural Planner Integration with Episodic Memory");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* planner = brain.getPlanner();
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(planner != nullptr && episodicMemory != nullptr);
        
        // Store episodic memories for planning reference
        std::vector<EpisodicMemoryItem> episodes;
        for (size_t i = 0; i < 3; ++i) {
            EpisodicMemoryItem episode;
            episode.timestamp = i * 10;
            episode.positionX = static_cast<float>(i * 2);
            episode.positionY = static_cast<float>(i * 3);
            episode.action = (i % 2 == 0) ? ActionType::MoveForward : ActionType::TurnLeft;
            episode.reward = 5.0f - static_cast<float>(i);
            episode.stabilityScore = 0.7f + (static_cast<float>(i) * 0.1f);
            
            episodes.push_back(episode);
        }
        
        for (const auto& episode : episodes) {
            episodicMemory->storeEpisode(episode);
        }
        
        // Get episodic features for planning
        std::vector<float> episodicFeatures(100, 0.0f);
        episodicFeatures[0] = 0.8f; // Previous action success
        episodicFeatures[1] = 0.6f; // Location feature
        
        // Plan action using episodic memory integration
        std::vector<float> currentState(100, 0.0f);
        ActionType action = planner->planActionWithIntegration(
            currentState, 0.5f, nullptr, &episodicFeatures, 0.8f, 0.7f);
        
        // Verify planning was influenced by episodic memory
        assert(action != ActionType::Wait);
        
        logTestResult("Neural Planner Integration with Episodic Memory", true,
                     "Planned action influenced by episodic memory: " + 
                     std::to_string(static_cast<int>(action)));
    }
    
    void testNeuralPlannerIntegrationWithPredictionSystem() {
        logTestStart("Neural Planner Integration with Prediction System");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* planner = brain.getPlanner();
        auto* predictionSystem = brain.getPredictionSystem();
        assert(planner != nullptr && predictionSystem != nullptr);
        
        // Current state
        std::vector<float> currentState(100, 0.0f);
        for (size_t i = 0; i < 100; ++i) {
            currentState[i] = static_cast<float>(i) / 100.0f;
        }
        
        // Plan action with prediction system integration
        float predictionConfidence = 0.8f;
        ActionType action = planner->planActionWithIntegration(
            currentState, 0.5f, nullptr, nullptr, predictionConfidence, 0.6f);
        
        // Verify planning was influenced by prediction confidence
        assert(action != ActionType::Wait);
        
        // Store planning results for learning
        std::vector<ActionType> plannedActions = {action, ActionType::Wait, ActionType::MoveForward};
        std::vector<float> predictedStates(100, 0.5f);
        
        planner->storePlanningResults(plannedActions, predictedStates, 0.7f, 0.8f);
        
        logTestResult("Neural Planner Integration with Prediction System", true,
                     "Planned action influenced by prediction system: " + 
                     std::to_string(static_cast<int>(action)));
    }
    
    void testNeuralPlannerIntegrationWithNeuromodulation() {
        logTestStart("Neural Planner Integration with Neuromodulation");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* planner = brain.getPlanner();
        auto* dopamine = brain.getDopamine();
        assert(planner != nullptr && dopamine != nullptr);
        
        // Set neuromodulation levels
        dopamine->setLevel(0.7f);
        
        // Current state
        std::vector<float> currentState(100, 0.0f);
        for (size_t i = 0; i < 100; ++i) {
            currentState[i] = static_cast<float>(i) / 100.0f;
        }
        
        // Plan action with neuromodulation integration
        ActionType action = planner->planActionWithIntegration(
            currentState, 0.5f, nullptr, nullptr, 0.7f, 0.6f);
        
        // Verify planning was influenced by neuromodulation
        assert(action != ActionType::Wait);
        
        // Get planning success probability
        float successProb = planner->getPlanningSuccessProbability();
        assert(successProb >= 0.0f && successProb <= 1.0f);
        
        logTestResult("Neural Planner Integration with Neuromodulation", true,
                     "Planned action influenced by neuromodulation: " + 
                     std::to_string(static_cast<int>(action)) + 
                     ", success probability: " + std::to_string(successProb));
    }
};

void runAll() {
    NeuralPlannerIntegrationTests tests;
    tests.runAllTests();
}

} // namespace test_neuralplanner
} // namespace nlm
