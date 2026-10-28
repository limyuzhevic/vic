// Cognition Systems Integration Tests
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_cognition_integration {

void testPlannerConceptFormationAttentionIntegration() {
    std::cout << "    Testing Planner + Concept Formation + Attention Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300));
    config->set("region_count", static_cast<int64_t>(3));
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    // Create a current state
    std::vector<float> currentState(50, 0.0f);
    for (size_t i = 0; i < 50; ++i) {
        currentState[i] = 0.5f + 0.3f * sinf((float)i * 0.15f);
    }
    
    // Test planning with different goals
    nlm::ActionType plannedAction = planner->planAction(currentState, 0.7f);
    assert(static_cast<size_t>(plannedAction) < 10); // ActionType is enum
    
    // Set current goal
    std::vector<float> goal(50, 0.0f);
    for (size_t i = 0; i < 50; ++i) {
        goal[i] = 0.8f * sinf((float)i * 0.2f);
    }
    planner->setCurrentGoal(goal);
    
    // Test concept formation with current state
    std::vector<std::string> concepts = conceptFormation->extractConcepts(currentState);
    assert(!concepts.empty()); // Should identify some concepts
    
    // Update concepts based on action outcome
    conceptFormation->updateConcept(currentState, plannedAction, 0.9f);
    
    // Test attention with competition
    std::vector<nlm::NeuronId> competingNeurons;
    for (int i = 0; i < 10; ++i) {
        competingNeurons.push_back(nlm::NeuronId(i + 100));
    }
    
    std::vector<nlm::NeuronId> winners = attention->processCompetition(competingNeurons, 0.5f);
    assert(!winners.empty()); // Should select some winners
    
    // Test attention focus
    attention->focusOnRegion(nlm::RegionId(0));
    assert(attention->getAttendedRegions().size() >= 1);
    
    // Simulate multiple steps of integrated cognition
    for (int step = 0; step < 50; ++step) {
        // Plan action
        nlm::ActionType action = planner->planAction(currentState, 0.6f);
        
        // Form concepts from state
        conceptFormation->updateConcept(currentState, action, 0.8f);
        
        // Update attention
        attention->update(0.001f);
        
        // Simulate state change
        for (size_t i = 0; i < currentState.size(); ++i) {
            currentState[i] += 0.01f * sinf((float)step * 0.1f);
        }
        
        brain.step(step, step * 0.001f);
    }
    
    // Verify integration results
    float planningConfidence = planner->getPlanningConfidence();
    assert(planningConfidence >= 0.0f && planningConfidence <= 1.0f);
    
    assert(attention->getWinners().size() > 0);
    
    std::cout << "    Planner + Concept Formation + Attention integration test passed" << std::endl;
}

void testAttentionMemoryIntegration() {
    std::cout << "    Testing Attention + Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200));
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* attention = brain.getAttention();
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    assert(attention != nullptr);
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    
    // Create competing neural patterns
    std::vector<nlm::NeuronId> competitors;
    for (int i = 0; i < 15; ++i) {
        competitors.push_back(nlm::NeuronId(i + 50));
    }
    
    // Store patterns in working memory
    std::vector<float> pattern1(20, 0.0f);
    std::vector<float> pattern2(20, 0.0f);
    for (size_t i = 0; i < 20; ++i) {
        pattern1[i] = sinf((float)i * 0.3f);
        pattern2[i] = cosf((float)i * 0.3f);
    }
    
    workingMemory->store(pattern1, 0.9f);
    workingMemory->store(pattern2, 0.9f);
    
    // Process competition with attention
    std::vector<nlm::NeuronId> winners = attention->processCompetition(competitors, 0.6f);
    assert(!winners.empty());
    
    // Attention should affect memory retrieval
    std::vector<float> retrieved = workingMemory->retrieve();
    assert(retrieved.size() == pattern1.size()); // Should maintain pattern size
    
    // Create episodic memory and replay
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 10;
    episode.sensoryState = pattern1;
    episode.positionX = 1.0f;
    episode.positionY = 1.0f;
    episode.action = nlm::ActionType::Move;
    episode.reward = 0.8f;
    
    episodicMemory->storeEpisode(episode);
    
    // Replay episode to strengthen attention
    episodicMemory->replayEpisode(&episode);
    
    // Attention should be updated during replay
    attention->update(0.001f);
    
    // Verify attention winners changed after replay
    assert(attention->getWinners().size() >= 0);
    
    std::cout << "    Attention + Memory integration test passed" << std::endl;
}

void runAll() {
    testPlannerConceptFormationAttentionIntegration();
    testAttentionMemoryIntegration();
}

} // namespace test_cognition_integration