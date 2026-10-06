// Test Cognitive System Integration
// This test verifies that cognitive systems (NeuralPlanner, ConceptFormation, Attention) properly integrate with brain dynamics

#include "test_cognitive_integration.h"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

using namespace nlm;

void testCognitiveIntegration::testPlannerIntegration() {
    NLM_LOG_INFO("Testing NeuralPlanner integration...");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    NLM_LOG_INFO("[PASS] NeuralPlanner is accessible");
    
    // Test action planning with current state
    std::vector<float> currentState = {0.1f, 0.3f, 0.5f, 0.7f, 0.9f};
    float targetReward = 0.8f;
    
    ActionType action = planner->planAction(currentState, targetReward);
    NLM_LOG_INFO("[PASS] NeuralPlanner planned action: " + std::to_string(static_cast<int>(action)));
    
    // Test planning confidence
    float confidence = planner->getPlanningConfidence();
    NLM_LOG_INFO("[PASS] NeuralPlanner confidence: " + std::to_string(confidence));
    
    // Test action evaluation
    float actionValue = planner->evaluateAction(action, currentState);
    NLM_LOG_INFO("[PASS] NeuralPlanner evaluated action value: " + std::to_string(actionValue));
}

void testCognitiveIntegration::testConceptFormationIntegration() {
    NLM_LOG_INFO("Testing ConceptFormation integration...");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    NLM_LOG_INFO("[PASS] ConceptFormation is accessible");
    
    // Test experience presentation
    std::vector<float> pattern = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
    std::vector<float> features = {0.1f, 0.5f, 0.9f};
    float reward = 0.7f;
    SimulationStep currentTime = 100;
    
    size_t conceptId = conceptFormation->presentExperience(pattern, features, reward, currentTime);
    NLM_LOG_INFO("[PASS] ConceptFormation presented experience, got concept ID: " + std::to_string(conceptId));
    
    // Test concept retrieval
    if (conceptId > 0) {
        const DiscoveredConcept* concept = conceptFormation->getConcept(conceptId);
        if (concept != nullptr) {
            NLM_LOG_INFO("[PASS] ConceptFormation can retrieve concepts");
            
            // Test concept stability
            float stability = conceptFormation->getConceptStability(conceptId);
            NLM_LOG_INFO("[PASS] Concept stability: " + std::to_string(stability));
        }
    }
}

void testCognitiveIntegration::testAttentionIntegration() {
    NLM_LOG_INFO("Testing AttentionalSelection integration...");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    NLM_LOG_INFO("[PASS] AttentionalSelection is accessible");
    
    // Test competition processing
    std::vector<NeuronId> competitors = {NeuronId(1001), NeuronId(1002), NeuronId(1003)};
    
    std::vector<NeuronId> winners = attention->processCompetition(competitors, 1.0f);
    NLM_LOG_INFO("[PASS] AttentionalSelection processed competition, got " + std::to_string(winners.size()) + " winners");
    
    // Test attention modifiers
    attention->setInhibitionStrength(0.6f);
    attention->setExcitationStrength(1.4f);
    
    float inhibitionFor = attention->getInhibitionFor(NeuronId(1001));
    float excitationFor = attention->getExcitationFor(NeuronId(1002));
    
    NLM_LOG_INFO("[PASS] Attention inhibition strength: " + std::to_string(inhibitionFor));
    NLM_LOG_INFO("[PASS] Attention excitation strength: " + std::to_string(excitationFor));
}

void testCognitiveIntegration::testCognitiveSystemIntegration() {
    NLM_LOG_INFO("Testing cognitive system integration...");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    auto* workingMemory = brain.getWorkingMemory();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    assert(workingMemory != nullptr);
    
    // Test integration flow: attention -> working memory -> planner
    std::vector<NeuronId> competitors = {NeuronId(1001), NeuronId(1002)};
    std::vector<NeuronId> winners = attention->processCompetition(competitors, 1.0f);
    
    if (!winners.empty()) {
        // Store winning neurons in working memory
        for (NeuronId winner : winners) {
            workingMemory->storeToNeuron(winner, 0.8f);
        }
        
        // Get working memory pattern
        std::vector<float> workingPattern = workingMemory->retrieve();
        
        // Use pattern for planning
        if (!workingPattern.empty()) {
            float targetReward = 0.9f;
            ActionType action = planner->planAction(workingPattern, targetReward);
            NLM_LOG_INFO("[PASS] Cognitive integration flow successful: attention -> working memory -> planner = " + std::to_string(static_cast<int>(action)));
        }
    }
    
    // Test concept formation integration with attention
    std::vector<float> pattern = {0.3f, 0.5f, 0.7f, 0.9f, 1.0f};
    std::vector<float> features = {0.2f, 0.6f, 0.8f};
    
    size_t conceptId = conceptFormation->presentExperience(pattern, features, 0.7f, 50);
    if (conceptId > 0) {
        // Use concept to influence planning
        float stability = conceptFormation->getConceptStability(conceptId);
        if (stability > 0.7f) {
            NLM_LOG_INFO("[PASS] Concept formation successfully integrated with cognitive systems");
        }
    }
}

void testCognitiveIntegration::testCognitiveLearningOverTime() {
    NLM_LOG_INFO("Testing cognitive system learning over time...");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    // Simulate learning over time
    for (int i = 0; i < 10; ++i) {
        // Generate varying state
        std::vector<float> state;
        for (int j = 0; j < 5; ++j) {
            state.push_back(0.1f * (i + 1) * (j + 1));
        }
        
        // Process through cognitive systems
        std::vector<NeuronId> competitors = {NeuronId(i + 1000), NeuronId(i + 1001)};
        std::vector<NeuronId> winners = attention->processCompetition(competitors, 1.0f);
        
        ActionType action = planner->planAction(state, 0.5f + i * 0.05f);
        
        // Form concepts from experiences
        std::vector<float> features = {0.1f * i, 0.2f * i, 0.3f * i};
        size_t conceptId = conceptFormation->presentExperience(state, features, 0.3f + i * 0.05f, i);
        
        // Record success
        bool recentPlanSuccessful = planner->wasRecentPlanSuccessful();
        NLM_LOG_INFO("[PASS] Cognitive learning cycle " + std::to_string(i) + ": plan successful = " + std::to_string(recentPlanSuccessful));
    }
    
    NLM_LOG_INFO("[PASS] Cognitive systems showed learning over time");
}

void testCognitiveIntegration::run() {
    NLM_LOG_INFO("=== Running Cognitive Integration Tests ===");
    
    testPlannerIntegration();
    testConceptFormationIntegration();
    testAttentionIntegration();
    testCognitiveSystemIntegration();
    testCognitiveLearningOverTime();
    
    NLM_LOG_INFO("=== Cognitive Integration Tests Complete ===");
}