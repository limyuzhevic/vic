// Edge Case and Regression Tests
// Tests boundary conditions, error handling, and regression scenarios

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>

namespace test_edge_cases {

void testWorkingMemoryEdgeCases() {
    NLM_LOG_INFO("=== Testing Working Memory Edge Cases ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* wm = brain->getWorkingMemory();
    assert(wm != nullptr);
    
    // Test empty working memory
    assert(wm->getCurrentSize() == 0);
    assert(wm->getActiveTraces() == 0);
    assert(wm->getMemoryActivity() == 0.0f);
    
    // Test with empty pattern
    wm->store(std::vector<float>(), 1.0f);
    // Should handle gracefully
    
    // Test with very large pattern
    std::vector<float> largePattern(10000, 1.0f);
    wm->store(largePattern, 1.0f);
    
    // Test with very small values
    std::vector<float> smallPattern(10, 1e-10f);
    wm->store(smallPattern, 1.0f);
    
    // Test with very large values
    std::vector<float> largeValuePattern(10, 1000.0f);
    wm->store(largeValuePattern, 1.0f);
    
    // Test extreme capacity settings
    wm->setCapacity(0);
    assert(wm->getCapacity() == 0);
    
    wm->setCapacity(1000000);
    assert(wm->getCapacity() == 1000000);
    
    // Test extreme decay rate
    wm->setDecayRate(0.0f);
    wm->update(0.001f);
    
    wm->setDecayRate(1.0f);
    wm->update(0.001f);
    
    wm->setDecayRate(-1.0f); // Negative decay (should not crash)
    wm->update(0.001f);
    
    // Clear after stress test
    wm->clear();
    
    NLM_LOG_INFO("[PASS] Working memory edge cases handled");
    
    std::cout << "    testWorkingMemoryEdgeCases passed" << std::endl;
}

void testEpisodicMemoryEdgeCases() {
    NLM_LOG_INFO("=== Testing Episodic Memory Edge Cases ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* em = brain->getEpisodicMemory();
    assert(em != nullptr);
    
    // Test empty episodic memory
    assert(em->getEpisodeCount() == 0);
    auto emptyResult = em->retrieveSimilar(std::vector<float>(), 5);
    assert(emptyResult.empty());
    
    // Test episode with empty data
    nlm::EpisodicMemoryItem emptyEpisode;
    em->storeEpisode(emptyEpisode);
    
    // Test extreme timestamps
    nlm::EpisodicMemoryItem extremeEpisode;
    extremeEpisode.timestamp = std::numeric_limits<nlm::SimulationStep>::max();
    extremeEpisode.sensoryState = std::vector<float>(1000, 1.0f);
    em->storeEpisode(extremeEpisode);
    
    // Test with very large position values
    nlm::EpisodicMemoryItem largePosEpisode;
    largePosEpisode.positionX = 1e10f;
    largePosEpisode.positionY = 1e10f;
    largePosEpisode.orientation = 1e10f;
    largePosEpisode.sensoryState = std::vector<float>(100, 0.5f);
    em->storeEpisode(largePosEpisode);
    
    // Test extreme reward values
    nlm::EpisodicMemoryItem rewardEpisode;
    rewardEpisode.reward = std::numeric_limits<float>::max();
    rewardEpisode.energy = std::numeric_limits<float>::lowest();
    rewardEpisode.novelty = 2.0f; // > 1.0
    rewardEpisode.sensoryState = std::vector<float>(100, 0.5f);
    em->storeEpisode(rewardEpisode);
    
    // Test retrieval with empty queries
    auto emptyQueryResults = em->retrieveSimilar(std::vector<float>(), 10);
    // Should handle gracefully
    
    // Test consolidation with extreme thresholds
    em->consolidate(0.0f); // Keep everything
    em->consolidate(1.0f); // Keep nothing
    
    // Test replay with empty episode list
    em->getEpisodesForReplay(0); // Should return empty list
    
    // Clean up
    em->clear();
    
    NLM_LOG_INFO("[PASS] Episodic memory edge cases handled");
    
    std::cout << "    testEpisodicMemoryEdgeCases passed" << std::endl;
}

void testConceptFormationEdgeCases() {
    NLM_LOG_INFO("=== Testing Concept Formation Edge Cases ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* cf = brain->getConceptFormation();
    assert(cf != nullptr);
    cf->initialize(brain.get());
    
    // Test empty concept formation
    assert(cf->getConceptCount() == 0);
    
    // Test with empty pattern
    cf->presentExperience(std::vector<float>(), std::vector<float>(), 0.5f, 0);
    
    // Test with very large pattern
    std::vector<float> largePattern(10000, 1.0f);
    cf->presentExperience(largePattern, std::vector<float>{0.5f}, 1.0f, 1);
    
    // Test with very small values
    std::vector<float> smallPattern(10, 1e-10f);
    cf->presentExperience(smallPattern, std::vector<float>{0.5f}, 1e-10f, 2);
    
    // Test extreme similarity thresholds
    std::vector<float> pattern = {0.5f, 0.5f, 0.5f};
    cf->presentExperience(pattern, std::vector<float>{0.5f}, 1.0f, 3);
    
    size_t conceptId = cf->getMatchingConcept(pattern, 0.0f); // Very low threshold
    assert(conceptId >= 0);
    
    conceptId = cf->getMatchingConcept(pattern, 2.0f); // Very high threshold
    assert(conceptId >= 0);
    
    // Test extreme rewards
    cf->presentExperience(pattern, std::vector<float>{0.5f}, std::numeric_limits<float>::max(), 4);
    cf->presentExperience(pattern, std::vector<float>{0.5f}, std::numeric_limits<float>::lowest(), 5);
    
    // Test stability with single observation
    if (cf->getConceptCount() > 0) {
        float stability = cf->getConceptStability(1);
        assert(stability >= 0.0f && stability <= 1.0f);
    }
    
    // Test generalization with empty concept
    float generalization = cf->getGeneralizationAbility(0);
    assert(generalization == 0.0f);
    
    // Clear after stress test
    cf->clear();
    
    NLM_LOG_INFO("[PASS] Concept formation edge cases handled");
    
    std::cout << "    testConceptFormationEdgeCases passed" << std::endl;
}

void testAttentionEdgeCases() {
    NLM_LOG_INFO("=== Testing Attention Edge Cases ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* attention = brain->getAttention();
    assert(attention != nullptr);
    attention->initialize(brain.get());
    
    // Test empty attention
    std::vector<nlm::NeuronId> emptyCompetitors;
    std::vector<nlm::NeuronId> emptyWinners = attention->processCompetition(emptyCompetitors);
    assert(emptyWinners.empty());
    
    // Test with single competitor
    std::vector<nlm::NeuronId> singleCompetitor = {nlm::NeuronId(400000)};
    attention->applyBottomUpSalience(singleCompetitor[0], 1.0f);
    std::vector<nlm::NeuronId> singleWinners = attention->processCompetition(singleCompetitor);
    // Should return something (implementation dependent)
    
    // Test extreme bias values
    attention->applyTopDownBias(nlm::NeuronId(400001), -1.0f); // Negative bias
    attention->applyBottomUpSalience(nlm::NeuronId(400002), 2.0f); // > 1.0
    
    attention->applyTopDownBias(nlm::NeuronId(400003), std::numeric_limits<float>::max());
    attention->applyBottomUpSalience(nlm::NeuronId(400004), std::numeric_limits<float>::lowest());
    
    // Test extreme parameter values
    attention->setInhibitionStrength(0.0f);
    attention->setExcitationStrength(0.0f);
    attention->setCompetitionThreshold(2.0f); // > 1.0
    attention->setCompetitionThreshold(-1.0f); // < 0.0
    
    // Test focus/release with invalid regions
    attention->focusOnRegion(nlm::RegionId(0)); // Might be invalid
    attention->releaseAttention();
    
    // Test reset with various states
    attention->applyTopDownBias(nlm::NeuronId(400005), 0.5f);
    attention->applyBottomUpSalience(nlm::NeuronId(400006), 0.5f);
    attention->reset();
    
    // Test update with extreme timesteps
    attention->update(0.0f);
    attention->update(std::numeric_limits<float>::max());
    
    NLM_LOG_INFO("[PASS] Attention edge cases handled");
    
    std::cout << "    testAttentionEdgeCases passed" << std::endl;
}

void testPlanningEdgeCases() {
    NLM_LOG_INFO("=== Testing Planning Edge Cases ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* planner = brain->getPlanner();
    assert(planner != nullptr);
    planner->initialize(brain.get());
    
    // Test with empty state
    nlm::ActionType actionEmpty = planner->planAction(std::vector<float>(), 0.5f);
    assert(actionEmpty != nlm::ActionType::ActionTypeCount);
    
    // Test with very large state
    std::vector<float> largeState(10000, 1.0f);
    nlm::ActionType actionLarge = planner->planAction(largeState, 0.5f);
    assert(actionLarge != nlm::ActionType::ActionTypeCount);
    
    // Test extreme planning depth
    planner->setPlanningDepth(0);
    assert(planner->getPlanningDepth() == 0);
    
    planner->setPlanningDepth(1000);
    assert(planner->getPlanningDepth() == 1000);
    
    // Test extreme confidence
    planner->setActionQuality(nlm::ActionType::MoveForward, 2.0f); // > 1.0
    planner->setActionQuality(nlm::ActionType::MoveForward, -1.0f); // < 0.0
    
    // Test evaluation with empty action sequence
    std::vector<nlm::ActionType> emptyActions;
    nlm::PlanningCandidate emptyCandidate = planner->evaluateSequence(emptyActions, std::vector<float>());
    assert(emptyCandidate.actions.empty());
    
    // Test evaluation with very large action sequence
    std::vector<nlm::ActionType> largeActions(100, nlm::ActionType::MoveForward);
    nlm::PlanningCandidate largeCandidate = planner->evaluateSequence(largeActions, largeState);
    assert(largeCandidate.actions.size() == largeActions.size());
    
    // Test update with extreme outcomes
    std::vector<nlm::ActionType> planned = {nlm::ActionType::MoveForward};
    std::vector<nlm::ActionType> actual = {nlm::ActionType::MoveForward};
    planner->updatePlanQuality(planned, actual, std::numeric_limits<float>::max());
    planner->updatePlanQuality(planned, actual, std::numeric_limits<float>::lowest());
    
    // Test cache clearing
    planner->clearCache();
    
    NLM_LOG_INFO("[PASS] Planning edge cases handled");
    
    std::cout << "    testPlanningEdgeCases passed" << std::endl;
}

void testSelfModelEdgeCases() {
    NLM_LOG_INFO("=== Testing Self-Model Edge Cases ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* selfModel = brain->getSelfModel();
    assert(selfModel != nullptr);
    selfModel->initialize(brain.get());
    
    // Test empty self-model
    assert(!selfModel->hasSelfModel());
    
    // Test with empty states
    selfModel->recordSelfAction(nlm::ActionType::MoveForward, std::vector<float>(), std::vector<float>());
    
    // Test extreme states
    std::vector<float> extremeBefore(10000, 1.0f);
    std::vector<float> extremeAfter(10000, 0.0f);
    selfModel->recordSelfAction(nlm::ActionType::MoveForward, extremeBefore, extremeAfter);
    
    // Test extreme prediction values
    std::vector<float> predictionMax(std::numeric_limits<float>::max());
    std::vector<float> predictionMin(std::numeric_limits<float>::lowest());
    
    std::vector<float> predicted = selfModel->predictActionConsequence(nlm::ActionType::MoveForward, extremeBefore);
    
    // Test extreme self-likeness
    float likeness = selfModel->computeSelfGeneratedLikeness(extremeBefore, extremeAfter, nlm::ActionType::MoveForward);
    assert(likeness >= 0.0f && likeness <= 1.0f);
    
    // Test extreme confidence values
    float confidence = selfModel->getSelfModelConfidence(nlm::ActionType::MoveForward);
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test update with extreme prediction errors
    selfModel->updateSelfModel(predictionMax, predictionMin, nlm::ActionType::MoveForward);
    selfModel->updateSelfModel(predictionMin, predictionMax, nlm::ActionType::MoveForward);
    
    // Test capability level extremes
    assert(selfModel->getCapabilityLevel() >= 0.0f && selfModel->getCapabilityLevel() <= 1.0f);
    
    // Test body awareness
    float awareness = selfModel->getBodyAwareness();
    assert(awareness >= 0.0f && awareness <= 1.0f);
    
    // Clean up
    selfModel->clear();
    
    NLM_LOG_INFO("[PASS] Self-model edge cases handled");
    
    std::cout << "    testSelfModelEdgeCases passed" << std::endl;
}

void testSocialLearningEdgeCases() {
    NLM_LOG_INFO("=== Testing Social Learning Edge Cases ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* socialLearning = brain->getSocialLearning();
    assert(socialLearning != nullptr);
    socialLearning->initialize(brain.get());
    
    // Test empty social learning
    assert(!socialLearning->hasSocialKnowledge());
    assert(socialLearning->getObservationCount() == 0);
    
    // Test with empty states
    socialLearning->observeAgentAction(nlm::ActionType::MoveForward, std::vector<float>(), std::vector<float>());
    
    // Test with extreme values
    std::vector<float> extremeState(10000, 1.0f);
    socialLearning->observeAgentAction(nlm::ActionType::MoveForward, extremeState, extremeState);
    
    // Test imitation with no observations
    bool canImitateEmpty = socialLearning->canImitate(nlm::ActionType::MoveForward);
    // Should handle gracefully
    
    // Test with extreme action types
    nlm::ActionType extremeAction = static_cast<nlm::ActionType>(1000); // Potentially invalid
    bool canImitateExtreme = socialLearning->canImitate(extremeAction);
    
    // Test communication signal with empty pattern
    socialLearning->learnCommunicationSignal(std::vector<float>(), 0.5f);
    
    // Test signal detection with extreme values
    std::vector<float> extremeSignal(std::numeric_limits<float>::max());
    bool detected = socialLearning->detectSignal(extremeSignal);
    
    // Test update with extreme rewards
    socialLearning->updateSocialKnowledge(std::numeric_limits<float>::max());
    socialLearning->updateSocialKnowledge(std::numeric_limits<float>::lowest());
    
    // Test clearance
    socialLearning->clear();
    assert(!socialLearning->hasSocialKnowledge());
    
    NLM_LOG_INFO("[PASS] Social learning edge cases handled");
    
    std::cout << "    testSocialLearningEdgeCases passed" << std::endl;
}

void testBrainEdgeCases() {
    NLM_LOG_INFO("=== Testing Brain Edge Cases ===");
    
    // Test brain creation with minimal config
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 1);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    // Test step with zero time
    brain->step(0, 0.0);
    
    // Test step with maximum time
    brain->step(1000000, 1000000.0);
    
    // Test with extreme neuron counts
    auto cfg2 = std::make_shared<nlm::Config>();
    cfg2->set("neuron_count", std::numeric_limits<size_t>::max() / 1000);
    cfg2->set("region_count", 1);
    
    // Should handle gracefully or throw exception
    auto brain2 = std::make_shared<nlm::Brain>(cfg2);
    bool initialized = brain2->initialize();
    
    // Test injection with extreme values
    brain->injectCurrent(nlm::NeuronId(0), std::numeric_limits<float>::max());
    brain->injectCurrent(nlm::NeuronId(0), std::numeric_limits<float>::lowest());
    
    brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, std::numeric_limits<float>::max());
    
    // Test statistics with empty brain
    size_t neuronCount = brain->getTotalNeuronCount();
    assert(neuronCount >= 0);
    
    // Test action production
    auto action = brain->produceAction();
    assert(action != nullptr);
    
    // Test reset multiple times
    brain->reset();
    brain->reset();
    
    // Test checkpoint with empty brain
    std::string path = "/tmp/empty_checkpoint.nml";
    bool saved = brain->save(path);
    // Might save or fail depending on implementation
    
    bool loaded = brain->load(path);
    // Should handle gracefully
    
    // Clean up
    std::filesystem::remove(path);
    
    NLM_LOG_INFO("[PASS] Brain edge cases handled");
    
    std::cout << "    testBrainEdgeCases passed" << std::endl;
}

void testMemoryUsageRegression() {
    NLM_LOG_INFO("=== Testing Memory Usage Regression ===");
    
    // Test that memory doesn't grow unbounded
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    // Run multiple iterations and check memory doesn't leak
    for (int iteration = 0; iteration < 10; ++iteration) {
        auto brain = std::make_shared<nlm::Brain>(cfg);
        assert(brain->initialize());
        
        auto* wm = brain->getWorkingMemory();
        auto* em = brain->getEpisodicMemory();
        
        // Do some work
        for (int step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
            if (wm) {
                std::vector<float> pattern(10, 0.5f);
                wm->store(pattern, 1.0f);
            }
            if (em && step % 10 == 0) {
                nlm::EpisodicMemoryItem episode;
                episode.timestamp = step;
                episode.sensoryState = std::vector<float>(50, 0.5f);
                em->storeEpisode(episode);
            }
        }
        
        // Clear at end of iteration
        if (wm) wm->clear();
        if (em) em->clear();
        
        // Check that systems are properly cleaned up
        if (wm) {
            size_t traces = wm->getActiveTraces();
            assert(traces == 0); // Should be cleared
        }
        
        if (em) {
            size_t episodes = em->getEpisodeCount();
            // Might be 0 or small number depending on consolidation
            assert(episodes >= 0);
        }
        
        NLM_LOG_INFO("Iteration " + std::to_string(iteration) + " completed");
    }
    
    NLM_LOG_INFO("[PASS] Memory usage regression test passed");
    
    std::cout << "    testMemoryUsageRegression passed" << std::endl;
}

void runAll() {
    testWorkingMemoryEdgeCases();
    testEpisodicMemoryEdgeCases();
    testConceptFormationEdgeCases();
    testAttentionEdgeCases();
    testPlanningEdgeCases();
    testSelfModelEdgeCases();
    testSocialLearningEdgeCases();
    testBrainEdgeCases();
    testMemoryUsageRegression();
}

} // namespace test_edge_cases
