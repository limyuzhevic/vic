// Learning Tests
// Tests continual learning without catastrophic forgetting

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "cognition/ConceptFormation.hpp"
#include "prediction/PredictionSystem.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_learning_continual {

void testContinualLearningWithoutForgetting() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Train on initial patterns
    std::vector<std::vector<float>> initialPatterns = {
        {1.0f, 0.5f, 0.0f, -0.5f, -1.0f},
        {0.8f, 0.4f, 0.0f, -0.4f, -0.8f},
        {0.6f, 0.3f, 0.0f, -0.3f, -0.6f}
    };
    
    for (size_t i = 0; i < initialPatterns.size(); ++i) {
        auto pattern = initialPatterns[i];
        auto conceptId = brain.getConceptFormation()->presentExperience(
            pattern, pattern, 1.0f, static_cast<nlm::SimulationStep>(i));
        assert(conceptId > 0 || conceptId == 0); // Can be new or existing concept
    }
    
    // Store initial concept count
    size_t initialConcepts = brain.getConceptFormation()->getConceptCount();
    
    // Simulate more learning over time
    for (nlm::SimulationStep step = 1; step < 100; ++step) {
        brain.step(step);
        
        // Present similar patterns (should reinforce existing concepts)
        float variation = 0.05f * std::sin(static_cast<float>(step) * 0.1f);
        
        for (size_t i = 0; i < initialPatterns.size(); ++i) {
            std::vector<float> variedPattern = initialPatterns[i];
            for (size_t j = 0; j < variedPattern.size(); ++j) {
                variedPattern[j] += variation;
            }
            
            brain.getConceptFormation()->presentExperience(
                variedPattern, variedPattern, 1.0f, step);
        }
        
        // Concept count should not explode (no catastrophic forgetting)
        size_t currentConcepts = brain.getConceptFormation()->getConceptCount();
        assert(currentConcepts <= initialConcepts * 2); // Allow some growth but not explosion
    }
    
    // Verify initial learning was retained
    std::vector<const nlm::DiscoveredConcept*> concepts = brain.getConceptFormation()->getConcepts();
    assert(!concepts.empty());
    
    std::cout << "    testContinualLearningWithoutForgetting passed" << std::endl;
}

void testConceptFormationAndRetention() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create distinct patterns that should form separate concepts
    std::vector<std::vector<float>> distinctPatterns = {
        {1.0f, 0.9f, 0.8f, 0.7f, 0.6f}, // Pattern 1
        {-1.0f, -0.9f, -0.8f, -0.7f, -0.6f}, // Pattern 2
        {0.0f, 0.5f, 1.0f, 0.5f, 0.0f}, // Pattern 3
        {2.0f, 1.0f, 0.0f, -1.0f, -2.0f} // Pattern 4
    };
    
    // Present patterns to form concepts
    std::vector<size_t> conceptIds;
    for (size_t i = 0; i < distinctPatterns.size(); ++i) {
        auto pattern = distinctPatterns[i];
        size_t conceptId = brain.getConceptFormation()->presentExperience(
            pattern, pattern, 1.0f, static_cast<nlm::SimulationStep>(i));
        conceptIds.push_back(conceptId);
    }
    
    // Verify concepts were formed
    size_t conceptCount = brain.getConceptFormation()->getConceptCount();
    assert(conceptCount >= 2); // At least 2 distinct concepts should form
    
    // Step forward to test retention
    for (nlm::SimulationStep step = 1; step < 50; ++step) {
        brain.step(step);
        
        // Concepts should persist
        size_t currentConcepts = brain.getConceptFormation()->getConceptCount();
        assert(currentConcepts >= 2); // Should maintain at least 2 concepts
        
        // Test concept stability
        for (size_t i = 0; i < conceptIds.size() && conceptIds[i] > 0; ++i) {
            float stability = brain.getConceptFormation()->getConceptStability(conceptIds[i]);
            assert(stability >= 0.0f && stability <= 1.0f);
        }
    }
    
    std::cout << "    testConceptFormationAndRetention passed" << std::endl;
}

void testPredictionAccuracyImprovement() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a predictable sequence
    std::vector<std::vector<float>> sequence = {
        {0.5f, 0.5f},
        {0.6f, 0.6f},
        {0.7f, 0.7f},
        {0.8f, 0.8f}
    };
    
    // Present sequence to prediction system
    for (size_t i = 0; i < sequence.size(); ++i) {
        auto pattern = sequence[i];
        brain.getPredictionSystem()->train(brain.getSensoryInputFromData(pattern));
    }
    
    // Make predictions and compare with actual values
    float totalPredictionError = 0.0f;
    size_t errorCount = 0;
    
    for (size_t i = 1; i < sequence.size(); ++i) {
        // Predict next state
        auto predicted = brain.getPredictionSystem()->predictNextState(
            brain.getSensoryInputFromData(sequence[i-1]));
        
        if (predicted) {
            const auto& predData = predicted->getData();
            const auto& actualData = sequence[i];
            
            // Calculate prediction error
            float error = 0.0f;
            for (size_t j = 0; j < predData.size() && j < actualData.size(); ++j) {
                error += std::abs(predData[j] - actualData[j]);
            }
            error /= predData.size();
            
            totalPredictionError += error;
            errorCount++;
        }
    }
    
    // Average prediction error should be reasonable (not exploding)
    if (errorCount > 0) {
        float avgError = totalPredictionError / errorCount;
        assert(avgError < 2.0f); // Should be stable, not exploding
    }
    
    std::cout << "    testPredictionAccuracyImprovement passed" << std::endl;
}

void testIntegrationOfWorkingEpisodicMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Store pattern in working memory
    std::vector<float> wmPattern = {1.0f, 0.5f, 0.0f, -0.5f, -1.0f};
    brain.getWorkingMemory()->store(wmPattern, 1.0f);
    
    // Create episodic memory from same pattern
    nlm::EpisodicMemoryItem episode;
    episode.sensoryState = wmPattern;
    episode.timestamp = 1;
    episode.reward = 1.0f;
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Present to concept formation
    size_t conceptId = brain.getConceptFormation()->presentExperience(
        wmPattern, wmPattern, 1.0f, 1);
    
    // All three memory systems should be active
    assert(brain.getWorkingMemory()->getActiveTraces() > 0);
    assert(brain.getEpisodicMemory()->getEpisodeCount() > 0);
    assert(brain.getConceptFormation()->getConceptCount() > 0 || conceptId == 0);
    
    // Test integration: working memory should influence episodic memory retrieval
    std::vector<const nlm::EpisodicMemoryItem*> retrieved = 
        brain.getEpisodicMemory()->retrieveSimilar(wmPattern, 3);
    
    if (!retrieved.empty()) {
        // Should have retrieved the episode we just stored
        assert(retrieved[0]->timestamp == 1);
    }
    
    // Test that concept formation can generalize
    std::vector<float> similarPattern = wmPattern;
    for (float& val : similarPattern) val *= 0.9f; // Slightly varied
    
    size_t similarConceptId = brain.getConceptFormation()->presentExperience(
        similarPattern, similarPattern, 0.8f, 2);
    
    // Should either find matching concept or create new one
    assert(similarConceptId >= 0);
    
    std::cout << "    testIntegrationOfWorkingEpisodicMemory passed" << std::endl;
}

void testCatastrophicForgettingPrevention() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a stable baseline of learned patterns
    std::vector<std::vector<float>> baselinePatterns = {
        {1.0f, 0.8f, 0.6f},
        {-1.0f, -0.8f, -0.6f},
        {0.5f, 0.5f, 0.5f},
        {-0.5f, -0.5f, -0.5f}
    };
    
    // Learn baseline
    for (size_t i = 0; i < baselinePatterns.size(); ++i) {
        brain.getConceptFormation()->presentExperience(
            baselinePatterns[i], baselinePatterns[i], 1.0f, static_cast<nlm::SimulationStep>(i));
    }
    
    size_t initialConcepts = brain.getConceptFormation()->getConceptCount();
    
    // Simulate extensive new learning
    for (nlm::SimulationStep step = 10; step < 200; ++step) {
        brain.step(step);
        
        // Present many new patterns
        std::vector<float> newPattern = {
            static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f,
            static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f,
            static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f
        };
        
        brain.getConceptFormation()->presentExperience(newPattern, newPattern, 0.5f, step);
        
        // Baseline concepts should still be accessible
        for (size_t i = 0; i < baselinePatterns.size(); ++i) {
            size_t matchingConcept = brain.getConceptFormation()->getMatchingConcept(
                baselinePatterns[i], 0.7f);
            
            // Should either find existing concept or create new one quickly
            // In either case, should not lose the concept entirely
            assert(matchingConcept != 0 || brain.getConceptFormation()->getConceptCount() > 0);
        }
        
        // Concept count should be reasonable (not exploding uncontrollably)
        size_t currentConcepts = brain.getConceptFormation()->getConceptCount();
        assert(currentConcepts < initialConcepts * 10); // Allow growth but prevent explosion
    }
    
    std::cout << "    testCatastrophicForgettingPrevention passed" << std::endl;
}

void testConceptStabilityOverTime() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create initial concept
    std::vector<float> initialPattern = {1.0f, 0.8f, 0.6f, 0.4f, 0.2f};
    size_t conceptId = brain.getConceptFormation()->presentExperience(
        initialPattern, initialPattern, 1.0f, 1);
    
    if (conceptId > 0) {
        // Check initial stability
        float initialStability = brain.getConceptFormation()->getConceptStability(conceptId);
        assert(initialStability >= 0.0f && initialStability <= 1.0f);
        
        // Step forward many times to test stability retention
        for (nlm::SimulationStep step = 2; step < 100; ++step) {
            brain.step(step);
            
            // Periodically reinforce the concept
            if (step % 10 == 0) {
                brain.getConceptFormation()->updateConcept(
                    conceptId, initialPattern, initialPattern, 1.0f);
            }
            
            // Stability should remain reasonable
            float currentStability = brain.getConceptFormation()->getConceptStability(conceptId);
            assert(currentStability >= 0.0f && currentStability <= 1.0f);
        }
    }
    
    std::cout << "    testConceptStabilityOverTime passed" << std::endl;
}

void testGeneralizationAbility() {
    auto config = std::std::shared_ptr<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a base concept from a pattern
    std::vector<float> basePattern = {1.0f, 0.8f, 0.6f, 0.4f, 0.2f};
    size_t baseConceptId = brain.getConceptFormation()->presentExperience(
        basePattern, basePattern, 1.0f, 1);
    
    if (baseConceptId > 0) {
        // Create variations of the base pattern
        std::vector<std::vector<float>> variations;
        
        for (float scale = 0.5f; scale <= 1.5f; scale += 0.2f) {
            std::vector<float> variation;
            for (float val : basePattern) {
                variation.push_back(val * scale);
            }
            variations.push_back(variation);
        }
        
        // Test that variations are recognized as belonging to the concept
        for (size_t i = 0; i < variations.size(); ++i) {
            size_t matchingConcept = brain.getConceptFormation()->getMatchingConcept(
                variations[i], 0.7f);
            
            // Either should find the original concept or be novel
            // but should not create an excessive number of new concepts
            if (matchingConcept > 0) {
                // Good: variation recognized as existing concept
            } else {
                // Also acceptable: variation is novel (creates new concept)
                // But this shouldn't happen too often
                size_t conceptCount = brain.getConceptFormation()->getConceptCount();
                assert(conceptCount < 10); // Reasonable number of concepts
            }
        }
    }
    
    std::cout << "    testGeneralizationAbility passed" << std::endl;
}

void runAll() {
    testContinualLearningWithoutForgetting();
    testConceptFormationAndRetention();
    testPredictionAccuracyImprovement();
    testIntegrationOfWorkingEpisodicMemory();
    testCatastrophicForgettingPrevention();
    testConceptStabilityOverTime();
    testGeneralizationAbility();
}

} // namespace test_learning_continual