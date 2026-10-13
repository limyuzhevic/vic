// Performance Benchmarks and Stress Tests
// Tests computational efficiency, scalability, and edge cases for all NLM systems

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <random>

namespace test_performance_benchmarks {

void testWorkingMemoryPerformance() {
    NLM_LOG_INFO("=== Testing Working Memory Performance ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 200);
    cfg->set("region_count", 2);
    cfg->set("memory_capacity", 5000);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* wm = brain->getWorkingMemory();
    assert(wm != nullptr);
    
    // Time memory operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Store many patterns
    std::vector<std::vector<float>> patterns;
    for (int i = 0; i < 1000; ++i) {
        std::vector<float> pattern(20, 0.5f + static_cast<float>(i) * 0.001f);
        patterns.push_back(pattern);
        wm->store(pattern, 1.0f);
    }
    
    // Retrieve patterns
    for (int i = 0; i < 1000; ++i) {
        std::vector<float> retrieved = wm->retrieve();
        assert(retrieved.size() == 20);
    }
    
    // Update memory
    for (int i = 0; i < 500; ++i) {
        wm->update(0.001f);
    }
    
    // Competition operations
    for (int i = 0; i < 100; ++i) {
        std::vector<nlm::NeuronId> competitors;
        for (int j = 0; j < 10; ++j) {
            competitors.push_back(nlm::NeuronId(200000 + j));
        }
        wm->runCompetition();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double operationsPerSecond = (5000.0 / duration.count()) * 1000.0;
    
    NLM_LOG_INFO("Working memory operations: " + std::to_string(operationsPerSecond) + " ops/second");
    
    // Should be efficient
    assert(operationsPerSecond > 1000.0);
    
    NLM_LOG_INFO("[PASS] Working memory performance is acceptable");
    
    std::cout << "    testWorkingMemoryPerformance passed" << std::endl;
}

void testEpisodicMemoryPerformance() {
    NLM_LOG_INFO("=== Testing Episodic Memory Performance ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* em = brain->getEpisodicMemory();
    assert(em != nullptr);
    
    // Time episodic memory operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Store many episodes
    for (int i = 0; i < 500; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = std::vector<float>(50, static_cast<float>(i) * 0.01f);
        episode.positionX = static_cast<float>(i % 10);
        episode.positionY = static_cast<float>((i / 10) % 10);
        episode.reward = static_cast<float>(i % 5) * 0.2f;
        em->storeEpisode(episode);
    }
    
    // Similar retrieval
    for (int i = 0; i < 200; ++i) {
        std::vector<float> query = std::vector<float>(50, 0.5f);
        auto results = em->retrieveSimilar(query, 5);
        assert(!results.empty());
    }
    
    // Temporal retrieval
    for (int i = 0; i < 100; ++i) {
        auto results = em->retrieveTemporal(i * 10, i * 10 + 50, 10);
    }
    
    // Location retrieval
    for (int i = 0; i < 100; ++i) {
        float x = static_cast<float>(i) * 2.0f;
        float y = static_cast<float>(i) * 2.0f;
        auto results = em->retrieveByLocation(x, y, 5.0f, 5);
    }
    
    // Replay operations
    for (int i = 0; i < 50; ++i) {
        auto episodes = em->getEpisodesForReplay(3);
        for (const auto* episode : episodes) {
            if (episode) em->replayEpisode(episode);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double operationsPerSecond = (1500.0 / duration.count()) * 1000.0;
    
    NLM_LOG_INFO("Episodic memory operations: " + std::to_string(operationsPerSecond) + " ops/second");
    
    // Should be efficient
    assert(operationsPerSecond > 500.0);
    
    NLM_LOG_INFO("[PASS] Episodic memory performance is acceptable");
    
    std::cout << "    testEpisodicMemoryPerformance passed" << std::endl;
}

void testConceptFormationPerformance() {
    NLM_LOG_INFO("=== Testing Concept Formation Performance ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* cf = brain->getConceptFormation();
    assert(cf != nullptr);
    cf->initialize(brain.get());
    
    // Time concept formation operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Present many experiences
    for (int i = 0; i < 1000; ++i) {
        std::vector<float> pattern(30, 0.5f + static_cast<float>(i) * 0.001f);
        std::vector<float> features = {0.5f};
        cf->presentExperience(pattern, features, 0.5f + static_cast<float>(i % 10) * 0.05f, i);
    }
    
    // Query concepts
    for (int i = 0; i < 500; ++i) {
        std::vector<float> query = std::vector<float>(30, 0.5f);
        size_t conceptId = cf->getMatchingConcept(query, 0.5f);
        if (conceptId > 0) {
            float stability = cf->getConceptStability(conceptId);
            float generalization = cf->getGeneralizationAbility(conceptId);
            assert(stability >= 0.0f && stability <= 1.0f);
            assert(generalization >= 0.0f && generalization <= 1.0f);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double operationsPerSecond = (2000.0 / duration.count()) * 1000.0;
    
    NLM_LOG_INFO("Concept formation operations: " + std::to_string(operationsPerSecond) + " ops/second");
    
    // Should be efficient
    assert(operationsPerSecond > 500.0);
    
    NLM_LOG_INFO("[PASS] Concept formation performance is acceptable");
    
    std::cout << "    testConceptFormationPerformance passed" << std::endl;
}

void testPlanningPerformance() {
    NLM_LOG_INFO("=== Testing Planning Performance ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* planner = brain->getPlanner();
    assert(planner != nullptr);
    planner->initialize(brain.get());
    
    // Set up planning
    planner->setPlanningDepth(5);
    planner->setActionQuality(nlm::ActionType::MoveForward, 0.7f);
    planner->setActionQuality(nlm::ActionType::Interact, 0.8f);
    planner->setActionQuality(nlm::ActionType::Wait, 0.3f);
    
    // Time planning operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<float> state = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    
    // Plan actions
    for (int i = 0; i < 500; ++i) {
        nlm::ActionType action = planner->planAction(state, 0.5f);
        assert(action != nlm::ActionType::ActionTypeCount); // Valid action
        
        // Update plan quality
        std::vector<nlm::ActionType> planned = {action};
        std::vector<nlm::ActionType> actual = {action};
        planner->updatePlanQuality(planned, actual, 0.5f);
    }
    
    // Evaluate sequences
    std::vector<nlm::ActionType> actions = {nlm::ActionType::MoveForward, nlm::ActionType::Wait};
    for (int i = 0; i < 200; ++i) {
        nlm::PlanningCandidate candidate = planner->evaluateSequence(actions, state);
        assert(candidate.actions.size() == actions.size());
        assert(candidate.expectedReward >= 0.0f);
        assert(candidate.confidence >= 0.0f);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double operationsPerSecond = (1000.0 / duration.count()) * 1000.0;
    
    NLM_LOG_INFO("Planning operations: " + std::to_string(operationsPerSecond) + " ops/second");
    
    // Should be efficient
    assert(operationsPerSecond > 200.0);
    
    NLM_LOG_INFO("[PASS] Planning performance is acceptable");
    
    std::cout << "    testPlanningPerformance passed" << std::endl;
}

void testAttentionPerformance() {
    NLM_LOG_INFO("=== Testing Attention Performance ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* attention = brain->getAttention();
    assert(attention != nullptr);
    attention->initialize(brain.get());
    
    // Time attention operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<nlm::NeuronId> competitors;
    for (int i = 0; i < 20; ++i) {
        competitors.push_back(nlm::NeuronId(300000 + i));
    }
    
    // Process competition
    for (int i = 0; i < 200; ++i) {
        // Vary biases
        for (size_t j = 0; j < competitors.size(); ++j) {
            attention->applyTopDownBias(competitors[j], 0.5f + static_cast<float>(i % 5) * 0.1f);
            attention->applyBottomUpSalience(competitors[j], 0.3f + static_cast<float>(i % 3) * 0.1f);
        }
        
        std::vector<nlm::NeuronId> winners = attention->processCompetition(competitors);
        assert(!winners.empty());
        
        attention->update(0.001f);
    }
    
    // Focus/release operations
    for (int i = 0; i < 50; ++i) {
        attention->focusOnRegion(nlm::RegionId(1));
        attention->releaseAttention();
        attention->reset();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double operationsPerSecond = (500.0 / duration.count()) * 1000.0;
    
    NLM_LOG_INFO("Attention operations: " + std::to_string(operationsPerSecond) + " ops/second");
    
    // Should be efficient
    assert(operationsPerSecond > 200.0);
    
    NLM_LOG_INFO("[PASS] Attention performance is acceptable");
    
    std::cout << "    testAttentionPerformance passed" << std::endl;
}

void testSelfModelPerformance() {
    NLM_LOG_INFO("=== Testing Self-Model Performance ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* selfModel = brain->getSelfModel();
    assert(selfModel != nullptr);
    selfModel->initialize(brain.get());
    
    // Time self-model operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<float> beforeState = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    std::vector<float> afterState = {0.6f, 0.7f, 0.8f, 0.7f, 0.6f};
    
    // Record many action-effects
    for (int i = 0; i < 200; ++i) {
        nlm::ActionType action = static_cast<nlm::ActionType>(i % 6);
        selfModel->recordSelfAction(action, beforeState, afterState);
        
        // Predict consequences
        std::vector<float> prediction = selfModel->predictActionConsequence(action, beforeState);
        assert(prediction.size() == afterState.size());
        
        // Compute self-likeness
        float likeness = selfModel->computeSelfGeneratedLikeness(beforeState, afterState, action);
        assert(likeness >= 0.0f && likeness <= 1.0f);
        
        // Get confidence
        float confidence = selfModel->getSelfModelConfidence(action);
        assert(confidence >= 0.0f && confidence <= 1.0f);
        
        // Update based on prediction error
        selfModel->updateSelfModel(prediction, afterState, action);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double operationsPerSecond = (1000.0 / duration.count()) * 1000.0;
    
    NLM_LOG_INFO("Self-model operations: " + std::to_string(operationsPerSecond) + " ops/second");
    
    // Should be efficient
    assert(operationsPerSecond > 200.0);
    
    NLM_LOG_INFO("[PASS] Self-model performance is acceptable");
    
    std::cout << "    testSelfModelPerformance passed" << std::endl;
}

void testSocialLearningPerformance() {
    NLM_LOG_INFO("=== Testing Social Learning Performance ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* socialLearning = brain->getSocialLearning();
    assert(socialLearning != nullptr);
    socialLearning->initialize(brain.get());
    
    // Time social learning operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Observe many actions
    for (int i = 0; i < 200; ++i) {
        nlm::ActionType action = static_cast<nlm::ActionType>(i % 6);
        std::vector<float> state(5, 0.5f + static_cast<float>(i) * 0.01f);
        std::vector<float> result(5, 0.6f + static_cast<float>(i) * 0.01f);
        
        socialLearning->observeAgentAction(action, state, result);
        
        // Check imitation capability
        bool canImitate = socialLearning->canImitate(action);
        assert(canImitate); // Should be able to imitate after observation
        
        // Get imitation action
        nlm::ActionType imitation = socialLearning->getImitationAction(state);
        assert(imitation != nlm::ActionType::ActionTypeCount);
        
        // Learn communication signal
        if (i % 20 == 0) {
            std::vector<float> signal(5, 0.1f + static_cast<float>(i) * 0.01f);
            socialLearning->learnCommunicationSignal(signal, 0.7f);
        }
        
        // Update social knowledge
        socialLearning->updateSocialKnowledge(0.5f + static_cast<float>(i % 5) * 0.1f);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double operationsPerSecond = (1500.0 / duration.count()) * 1000.0;
    
    NLM_LOG_INFO("Social learning operations: " + std::to_string(operationsPerSecond) + " ops/second");
    
    // Should be efficient
    assert(operationsPerSecond > 300.0);
    
    NLM_LOG_INFO("[PASS] Social learning performance is acceptable");
    
    std::cout << "    testSocialLearningPerformance passed" << std::endl;
}

void testBrainIntegrationPerformance() {
    NLM_LOG_INFO("=== Testing Brain Integration Performance ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 500);
    cfg->set("region_count", 4);
    cfg->set("connection_probability", 0.05f);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    // Time brain integration performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run simulation steps
    for (uint64_t step = 0; step < 500; ++step) {
        // Inject sensory input
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 2.0f);
        
        // Run brain step
        brain->step(step, step * 0.001);
        
        // Perform various operations
        brain->getAverageFiringRate();
        brain->getTotalNeuronCount();
        brain->getTotalSynapseCount();
        
        if (step % 100 == 0) {
            brain->logStatus();
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double stepsPerSecond = (500.0 / duration.count()) * 1000.0;
    
    NLM_LOG_INFO("Brain integration performance: " + std::to_string(stepsPerSecond) + " steps/second");
    
    // Should be efficient
    assert(stepsPerSecond > 50.0);
    
    NLM_LOG_INFO("[PASS] Brain integration performance is acceptable");
    
    std::cout << "    testBrainIntegrationPerformance passed" << std::endl;
}

void testMemoryStressTest() {
    NLM_LOG_INFO("=== Testing Memory Stress Test ===");
    
    // Create configuration with limited memory
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    cfg->set("memory_capacity", 500);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* wm = brain->getWorkingMemory();
    auto* em = brain->getEpisodicMemory();
    assert(wm && em);
    
    // Stress test working memory with many patterns
    for (int i = 0; i < 100; ++i) {
        std::vector<float> pattern(50, 0.5f + static_cast<float>(i) * 0.001f);
        wm->store(pattern, 1.0f);
        
        // Update to cause decay
        wm->update(0.001f);
        
        // Run competition
        wm->runCompetition();
    }
    
    // Stress test episodic memory with many episodes
    for (int i = 0; i < 100; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = std::vector<float>(100, 0.5f + static_cast<float>(i) * 0.001f);
        episode.reward = static_cast<float>(i % 5) * 0.2f;
        em->storeEpisode(episode);
        
        // Periodically consolidate
        if (i % 20 == 0) {
            em->consolidate(0.3f);
        }
    }
    
    // Verify systems still work
    size_t wmTraces = wm->getActiveTraces();
    size_t emEpisodes = em->getEpisodeCount();
    
    NLM_LOG_INFO("Stress test results: WM traces = " + std::to_string(wmTraces) +
                ", EM episodes = " + std::to_string(emEpisodes));
    
    // Systems should not crash
    assert(wmTraces >= 0);
    assert(emEpisodes >= 0);
    
    NLM_LOG_INFO("[PASS] Memory stress test passed");
    
    std::cout << "    testMemoryStressTest passed" << std::endl;
}

void runAll() {
    testWorkingMemoryPerformance();
    testEpisodicMemoryPerformance();
    testConceptFormationPerformance();
    testPlanningPerformance();
    testAttentionPerformance();
    testSelfModelPerformance();
    testSocialLearningPerformance();
    testBrainIntegrationPerformance();
    testMemoryStressTest();
}

} // namespace test_performance_benchmarks
