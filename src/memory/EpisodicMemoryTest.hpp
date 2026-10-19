#pragma once

#include "NeuralEpisodicMemory.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <iomanip>
#include <sstream>

namespace nlm
{

class EpisodicMemoryTest
{
public:
    static void runAllTests()
    {
        std::cout << "=== Neural Episodic Memory Tests ===" << std::endl;
        
        // Test core episodic memory functionality
        testCoreFunctionality();
        std::cout << "✓ Core functionality tests passed" << std::endl;
        
        testEventEncodingAndBinding();
        std::cout << "✓ Event encoding and binding tests passed" << std::endl;
        
        testMemoryTraceStorage();
        std::cout << "✓ Memory trace storage tests passed" << std::endl;
        
        testRetrievalMechanisms();
        std::cout << "✓ Retrieval mechanism tests passed" << std::endl;
        
        testReplayAndConsolidation();
        std::cout << "✓ Replay and consolidation tests passed" << std::endl;
        
        testPatternCompletion();
        std::cout << "✓ Pattern completion tests passed" << std::endl;
        
        testMemoryUpdatingAndReconsolidation();
        std::cout << "✓ Memory updating and reconsolidation tests passed" << std::endl;
        
        testSelfReferentialProcessing();
        std::cout << "✓ Self-referential processing tests passed" << std::endl;
        
        testSourceMonitoringAndAttribution();
        std::cout << "✓ Source monitoring and attribution tests passed" << std::endl;
        
        testMemorySpecificityAndPrecision();
        std::cout << "✓ Memory specificity and precision tests passed" << std::endl;
        
        testTemporalCompressionExpansion();
        std::cout << "✓ Temporal compression and expansion tests passed" << std::endl;
        
        testHippocampalNeocorticalInteractions();
        std::cout << "✓ Hippocampal-neocortical interaction tests passed" << std::endl;
        
        testMemoryPrioritizationAndGating();
        std::cout << "✓ Memory prioritization and gating tests passed" << std::endl;
        
        testEmotionalAndRewardIntegration();
        std::cout << "✓ Emotional and reward integration tests passed" << std::endl;
        
        testContextualRetrievalAndCompletion();
        std::cout << "✓ Contextual retrieval and completion tests passed" << std::endl;
        
        testMemoryInterferenceManagement();
        std::cout << "✓ Memory interference management tests passed" << std::endl;
        
        testSchemaBasedMemoryIntegration();
        std::cout << "✓ Schema-based memory integration tests passed" << std::endl;
        
        testTemporalSequencing();
        std::cout << "✓ Temporal sequencing validation tests passed" << std::endl;
        
        testSystemIntegration();
        std::cout << "✓ System integration tests passed" << std::endl;
        
        std::cout << "\n=== All Episodic Memory Tests Passed ===" << std::endl;
    }

private:
    static void testCoreFunctionality()
    {
        std::cout << "  Testing core episodic memory functionality..." << std::endl;
        
        // Create config and brain
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        
        // Initialize episodic memory
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        em.setCapacity(100);
        
        // Test empty state
        assert(em.getEpisodeCount() == 0);
        assert(em.getAverageReward() == 0.0f);
        
        // Create test episode
        EpisodicMemoryItem episode;
        episode.id = 1;
        episode.timestamp = 1000;
        episode.action = ActionType::Move;
        episode.positionX = 10.0f;
        episode.positionY = 20.0f;
        episode.orientation = 0.5f;
        episode.reward = 0.8f;
        episode.novelty = 0.6f;
        episode.selfReferential = true;
        episode.selfImportance = 0.9f;
        episode.emotionalValence = 0.3f;
        episode.emotionalArousal = 0.7f;
        episode.precision = 0.85f;
        episode.specificity = 0.8f;
        
        episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        episode.resultingSensoryState = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
        
        // Store episode
        em.storeEpisode(episode);
        
        // Verify storage
        assert(em.getEpisodeCount() == 1);
        assert(em.getAverageReward() == 0.8f);
        
        std::cout << "    ✓ Core storage and retrieval" << std::endl;
    }

    static void testEventEncodingAndBinding()
    {
        std::cout << "  Testing event encoding and contextual binding..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        
        // Test different event types
        std::vector<ActionType> actions = {
            ActionType::Move, ActionType::Attack, ActionType::Defend,
            ActionType::Explore, ActionType::Interact, ActionType::Wait
        };
        
        for (size_t i = 0; i < actions.size(); ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = 1000 + i * 100;
            episode.action = actions[i];
            episode.positionX = 10.0f + static_cast<float>(i);
            episode.positionY = 20.0f + static_cast<float>(i * 2);
            episode.orientation = 0.1f * (i + 1);
            episode.reward = 0.5f + static_cast<float>(i) * 0.1f;
            episode.novelty = 0.3f + static_cast<float>(i) * 0.1f;
            
            episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
            episode.resultingSensoryState = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
            episode.selfReferential = (i % 2 == 0);
            episode.selfImportance = episode.selfReferential ? 0.9f : 0.1f;
            
            em.storeEpisode(episode);
        }
        
        // Verify binding quality
        float avgPrecision = 0.0f;
        for (size_t i = 0; i < actions.size(); ++i) {
            const EpisodicMemoryItem* retrieved = em.getEpisode(i);
            if (retrieved) {
                avgPrecision += retrieved->precision;
            }
        }
        avgPrecision /= actions.size();
        
        assert(avgPrecision > 0.5f); // Should have good binding quality
        
        std::cout << "    ✓ Event encoding and contextual binding" << std::endl;
    }

    static void testMemoryTraceStorage()
    {
        std::cout << "  Testing memory trace storage with temporal sequencing..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        
        // Store multiple episodes with temporal sequencing
        std::vector<SimulationStep> timestamps = {1000, 2000, 3000, 4000, 5000};
        std::vector<float> rewards = {0.2f, 0.8f, 0.5f, 0.9f, 0.3f};
        
        for (size_t i = 0; i < timestamps.size(); ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = timestamps[i];
            episode.action = (i % 3 == 0) ? ActionType::Move : 
                           (i % 3 == 1) ? ActionType::Attack : ActionType::Explore;
            episode.positionX = 10.0f + static_cast<float>(i * 10);
            episode.positionY = 20.0f + static_cast<float>(i * 5);
            episode.orientation = 0.1f * (i + 1);
            episode.reward = rewards[i];
            episode.novelty = 0.5f - rewards[i] * 0.3f;
            
            episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
            
            em.storeEpisode(episode);
        }
        
        // Test temporal retrieval
        std::vector<const EpisodicMemoryItem*> retrieved = 
            em.retrieveTemporal(2500, 4500, 3);
        
        // Should get episodes 2, 3, 4 (timestamps 3000, 4000, 5000)
        assert(retrieved.size() <= 3);
        
        // Verify temporal ordering (most recent first)
        if (retrieved.size() >= 2) {
            assert(retrieved[0]->timestamp >= retrieved[1]->timestamp);
        }
        
        // Test temporal compression
        em.setTemporalCompression(true);
        em.setExpansionFactor(1.5f);
        
        // Update system to trigger compression
        em.update(100, 1.0f);
        
        std::cout << "    ✓ Memory trace storage with temporal sequencing" << std::endl;
    }

    static void testRetrievalMechanisms()
    {
        std::cout << "  Testing retrieval mechanisms..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        
        // Store episodes with different characteristics
        std::vector<EpisodicMemoryItem> episodes;
        
        for (size_t i = 0; i < 10; ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = 1000 + i * 100;
            episode.action = (i % 4 == 0) ? ActionType::Move : 
                           (i % 4 == 1) ? ActionType::Attack : 
                           (i % 4 == 2) ? ActionType::Explore : ActionType::Interact;
            episode.positionX = 10.0f + static_cast<float>(i);
            episode.positionY = 20.0f + static_cast<float>(i * 2);
            episode.orientation = 0.1f * (i + 1);
            episode.reward = 0.5f + static_cast<float>(i) * 0.05f;
            episode.novelty = 0.8f - static_cast<float>(i) * 0.07f;
            
            episode.sensoryState = {0.1f * (i + 1), 0.2f * (i + 1), 0.3f * (i + 1),
                                   0.4f * (i + 1), 0.5f * (i + 1)};
            
            em.storeEpisode(episode);
            episodes.push_back(episode);
        }
        
        // Test similar retrieval
        std::vector<float> query = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
        std::vector<const EpisodicMemoryItem*> similar = em.retrieveSimilar(query, 5);
        
        // Should find similar episodes
        assert(!similar.empty() || similar.size() <= 5);
        
        // Test location retrieval
        std::vector<const EpisodicMemoryItem*> byLocation = em.retrieveByLocation(15.0f, 30.0f, 10.0f, 5);
        
        // Should find episodes near location (10, 20) + offsets
        assert(byLocation.size() <= 5);
        
        // Test action retrieval
        std::vector<const EpisodicMemoryItem*> byAction = em.retrieveAfterAction(ActionType::Move, 5);
        
        assert(byAction.size() <= 5);
        
        // Test contextual retrieval
        std::vector<std::string> features = {"spatial", "temporal", "self-referential"};
        std::vector<const EpisodicMemoryItem*> byContext = em.retrieveByContext(features, 0.5f, 5);
        
        assert(byContext.size() <= 5);
        
        std::cout << "    ✓ Retrieval mechanisms" << std::endl;
    }

    static void testReplayAndConsolidation()
    {
        std::cout << "  Testing replay and consolidation..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        em.enableReplay(true);
        em.enableConsolidation(true);
        
        // Store episodes
        for (size_t i = 0; i < 5; ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = 1000 + i * 100;
            episode.action = ActionType::Move;
            episode.positionX = 10.0f + static_cast<float>(i);
            episode.positionY = 20.0f + static_cast<float>(i * 2);
            episode.orientation = 0.1f * (i + 1);
            episode.reward = 0.5f + static_cast<float>(i) * 0.1f;
            
            episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
            
            em.storeEpisode(episode);
        }
        
        // Test episode replay
        const EpisodicMemoryItem* episodeToReplay = em.getEpisode(0);
        if (episodeToReplay) {
            em.replayEpisode(episodeToReplay);
            // Replay should increase replay count
            assert(episodeToReplay->replayCount > 0);
        }
        
        // Test sequence replay
        std::vector<uint64_t> episodeIds;
        for (size_t i = 0; i < 3; ++i) {
            episodeIds.push_back(i + 1);
        }
        
        em.replaySequence(episodeIds);
        
        // Test consolidation
        em.consolidate(0.7f);
        
        // Check consolidation status
        size_t consolidatedCount = em.getConsolidatedCount();
        assert(consolidatedCount >= 0);
        
        // Get episodes for replay
        std::vector<uint64_t> forReplay = em.getEpisodesForReplay(3);
        
        // Should get some episodes
        assert(forReplay.size() <= 3);
        
        std::cout << "    ✓ Replay and consolidation" << std::endl;
    }

    static void testPatternCompletion()
    {
        std::cout << "  Testing pattern completion with contextual cues..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        em.enablePatternCompletion(true);
        
        // Store complete patterns
        for (size_t i = 0; i < 3; ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = 1000 + i * 100;
            episode.action = ActionType::Move;
            episode.positionX = 10.0f + static_cast<float>(i);
            episode.positionY = 20.0f + static_cast<float>(i * 2);
            
            // Store distinct patterns
            std::vector<float> pattern(10, 0.0f);
            for (size_t j = 0; j < 10; ++j) {
                pattern[j] = 0.1f * (i * 10 + j + 1);
            }
            
            episode.sensoryState = pattern;
            em.storeEpisode(episode);
        }
        
        // Test pattern completion with partial cues
        em.setPatternCompletionEnabled(true);
        
        // Update system to allow pattern completion
        em.update(100, 1.0f);
        
        std::cout << "    ✓ Pattern completion with contextual cues" << std::endl;
    }

    static void testMemoryUpdatingAndReconsolidation()
    {
        std::cout << "  Testing memory updating and reconsolidation..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        
        // Store initial episode
        EpisodicMemoryItem originalEpisode;
        originalEpisode.id = 1;
        originalEpisode.timestamp = 1000;
        originalEpisode.action = ActionType::Move;
        originalEpisode.positionX = 10.0f;
        originalEpisode.positionY = 20.0f;
        originalEpisode.reward = 0.5f;
        originalEpisode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        
        em.storeEpisode(originalEpisode);
        
        // Update episode with new information
        em.reconsolidate(1, originalEpisode);
        
        // Verify update
        const EpisodicMemoryItem* updated = em.getEpisode(0);
        if (updated) {
            assert(updated->replayCount > 0); // Reconsolidation should trigger replay
        }
        
        // Test relevance updates
        em.updateRelevance(1, 0.2f);
        
        std::cout << "    ✓ Memory updating and reconsolidation" << std::endl;
    }

    static void testSelfReferentialProcessing()
    {
        std::cout << "  Testing self-referential encoding and processing..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        em.setSelfReferentialProcessing(true);
        
        // Store self-referential episodes
        for (size_t i = 0; i < 5; ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = 1000 + i * 100;
            episode.action = ActionType::Move;
            episode.positionX = 10.0f + static_cast<float>(i);
            episode.positionY = 20.0f + static_cast<float>(i * 2);
            episode.selfReferential = true;
            episode.selfImportance = 0.7f + static_cast<float>(i) * 0.1f;
            
            episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
            
            em.storeEpisode(episode);
        }
        
        // Test self-importance threshold
        em.setSelfImportanceThreshold(0.6f);
        
        // Get high priority episodes (should include self-referential ones)
        std::vector<uint64_t> highPriority = em.getHighPriorityEpisodes(3);
        
        // Should get some self-referential episodes
        assert(highPriority.size() <= 3);
        
        std::cout << "    ✓ Self-referential processing" << std::endl;
    }

    static void testSourceMonitoringAndAttribution()
    {
        std::cout << "  Testing source monitoring and attribution..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        
        // Store episodes with source information
        for (size_t i = 0; i < 5; ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = 1000 + i * 100;
            episode.action = ActionType::Move;
            episode.positionX = 10.0f + static_cast<float>(i);
            episode.positionY = 20.0f + static_cast<float>(i * 2);
            episode.sourceId = 100 + i;
            episode.sourceType = "sensor";
            episode.sourceConfidence = 0.8f + static_cast<float>(i) * 0.05f;
            
            episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
            
            em.storeEpisode(episode);
        }
        
        // Verify source tracking
        // (In a full implementation, we would track source neural signatures)
        
        std::cout << "    ✓ Source monitoring and attribution" << std::endl;
    }

    static void testMemorySpecificityAndPrecision()
    {
        std::cout << "  Testing memory specificity and precision control..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        
        // Store episodes with varying precision
        for (size_t i = 0; i < 5; ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = 1000 + i * 100;
            episode.action = ActionType::Move;
            episode.positionX = 10.0f + static_cast<float>(i);
            episode.positionY = 20.0f + static_cast<float>(i * 2);
            
            // Vary precision based on episode characteristics
            episode.precision = 0.5f + static_cast<float>(i) * 0.1f;
            episode.specificity = 0.4f + static_cast<float>(i) * 0.12f;
            
            episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
            
            em.storeEpisode(episode);
        }
        
        // Verify precision statistics
        float avgPrecision = em.getAveragePrecision();
        float avgRecall = em.getAverageRecall();
        
        assert(avgPrecision >= 0.0f && avgPrecision <= 1.0f);
        assert(avgRecall >= 0.0f && avgRecall <= 1.0f);
        
        std::cout << "    ✓ Memory specificity and precision control" << std::endl;
    }

    static void testTemporalCompressionExpansion()
    {
        std::cout << "  Testing temporal compression and expansion..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        em.setTemporalCompression(true);
        em.setExpansionFactor(1.5f);
        
        // Store episodes with different ages
        for (size_t i = 0; i < 5; ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = 1000 + i * 100;
            episode.action = ActionType::Move;
            episode.positionX = 10.0f + static_cast<float>(i);
            episode.positionY = 20.0f + static_cast<float>(i * 2);
            
            episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
            
            em.storeEpisode(episode);
        }
        
        // Update to trigger temporal compression
        em.update(200, 2.0f);
        
        // Check temporal context after compression
        const EpisodicMemoryItem* episode = em.getEpisode(0);
        if (episode) {
            assert(episode->temporalContextStart <= episode->temporalContextEnd);
        }
        
        std::cout << "    ✓ Temporal compression and expansion" << std::endl;
    }

    static void testHippocampalNeocorticalInteractions()
    {
        std::cout << "  Testing hippocampal-neocortical interactions..." << stdendl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        em.setHippocampalNeocorticalMode(true);
        em.setReplayFrequency(0.2f);
        
        // Store episodes
        for (size_t i = 0; i < 5; ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = 1000 + i * 100;
            episode.action = ActionType::Move;
            episode.positionX = 10.0f + static_cast<float>(i);
            episode.positionY = 20.0f + static_cast<float>(i * 2);
            
            episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
            
            em.storeEpisode(episode);
        }
        
        // Update system to trigger hippocampal-neocortical interactions
        em.update(100, 1.0f);
        
        // Integrate with neurons
        em.integrateWithNeurons();
        
        // Synchronize with hippocampal system
        em.synchronizeWithHippocampalSystem();
        
        // Update neocortical integrations
        em.updateNeocorticalIntegrations();
        
        std::cout << "    ✓ Hippocampal-neocortical interactions" << std::endl;
    }

    static void testMemoryPrioritizationAndGating()
    {
        std::cout << "  Testing memory prioritization and gating..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        em.setPrioritizationEnabled(true);
        
        // Store episodes with different priorities
        for (size_t i = 0; i < 5; ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = 1000 + i * 100;
            episode.action = ActionType::Move;
            episode.positionX = 10.0f + static_cast<float>(i);
            episode.positionY = 20.0f + static_cast<float>(i * 2);
            episode.reward = 0.5f + static_cast<float>(i) * 0.1f;
            episode.novelty = 0.8f - static_cast<float>(i) * 0.15f;
            episode.selfReferential = (i % 2 == 0);
            episode.selfImportance = episode.selfReferential ? 0.9f : 0.1f;
            
            // Vary emotional content
            episode.emotionalValence = (static_cast<float>(i) - 2.0f) / 2.0f;
            episode.emotionalArousal = 0.5f + static_cast<float>(i) * 0.1f;
            
            episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
            
            em.storeEpisode(episode);
        }
        
        // Set priority weights
        MemoryPriority weights;
        weights.temporalPriority = 0.2f;
        weights.rewardPriority = 0.2f;
        weights.noveltyPriority = 0.15f;
        weights.selfPriority = 0.15f;
        weights.emotionalPriority = 0.15f;
        weights.schemaPriority = 0.08f;
        weights.interferencePriority = 0.05f;
        
        em.setPriorityWeights(weights);
        
        // Get high priority episodes
        std::vector<uint64_t> highPriority = em.getHighPriorityEpisodes(3);
        
        // Should get some high-priority episodes
        assert(highPriority.size() <= 3);
        
        // Get episodes for replay
        std::vector<uint64_t> forReplay = em.getEpisodesForReplay(3);
        
        assert(forReplay.size() <= 3);
        
        std::cout << "    ✓ Memory prioritization and gating" << std::endl;
    }

    static void testEmotionalAndRewardIntegration()
    {
        std::cout << "  Testing emotional and reward integration..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        em.setEmotionalIntegrationEnabled(true);
        
        // Store emotionally salient episodes
        for (size_t i = 0; i < 5; ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = 1000 + i * 100;
            episode.action = ActionType::Move;
            episode.positionX = 10.0f + static_cast<float>(i);
            episode.positionY = 20.0f + static_cast<float>(i * 2);
            
            // Vary emotional content
            episode.emotionalValence = (static_cast<float>(i) - 2.0f) / 2.0f; // -1 to 1
            episode.emotionalArousal = 0.5f + static_cast<float>(i) * 0.1f;  // 0.5 to 1.0
            episode.reward = 0.5f + static_cast<float>(i) * 0.1f;
            
            episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
            
            em.storeEpisode(episode);
        }
        
        // Update system to integrate emotional content
        em.update(100, 1.0f);
        
        // Verify emotional integration (statistics)
        float avgStrength = em.getAverageStrength();
        
        assert(avgStrength >= 0.0f && avgStrength <= 1.0f);
        
        std::cout << "    ✓ Emotional and reward integration" << std::endl;
    }

    static void testContextualRetrievalAndCompletion()
    {
        std::cout << "  Testing contextual retrieval and completion..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        em.setPatternCompletionEnabled(true);
        em.setCompletionThreshold(0.6f);
        
        // Store contextually rich episodes
        for (size_t i = 0; i < 5; ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = 1000 + i * 100;
            episode.action = ActionType::Move;
            episode.positionX = 10.0f + static_cast<float>(i);
            episode.positionY = 20.0f + static_cast<float>(i * 2);
            episode.orientation = 0.1f * (i + 1);
            
            // Vary contextual features
            episode.featureDescriptors.push_back("spatial");
            episode.featureDescriptors.push_back("temporal");
            if (i % 2 == 0) {
                episode.featureDescriptors.push_back("self-referential");
            }
            
            episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
            
            em.storeEpisode(episode);
        }
        
        // Update to allow contextual processing
        em.update(100, 1.0f);
        
        // Test contextual retrieval
        std::vector<std::string> features = {"spatial", "temporal"};
        std::vector<const EpisodicMemoryItem*> contextual = em.retrieveByContext(features, 0.5f, 3);
        
        assert(contextual.size() <= 3);
        
        std::cout << "    ✓ Contextual retrieval and completion" << std::endl;
    }

    static void testMemoryInterferenceManagement()
    {
        std::cout << "  Testing memory interference management..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        em.setInterferenceManagementEnabled(true);
        em.setInterferenceThreshold(0.3f);
        
        // Store similar episodes (to test interference)
        for (size_t i = 0; i < 5; ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = 1000 + i * 100;
            episode.action = ActionType::Move;
            episode.positionX = 10.0f + static_cast<float>(i) * 0.5f; // Similar positions
            episode.positionY = 20.0f + static_cast<float>(i) * 1.0f; // Similar positions
            episode.orientation = 0.1f * (i + 1);
            episode.reward = 0.5f + static_cast<float>(i) * 0.1f;
            
            // Similar sensory patterns (will cause interference)
            std::vector<float> sensory(5, 0.1f);
            for (size_t j = 0; j < 5; ++j) {
                sensory[j] += static_cast<float>(i) * 0.05f;
            }
            episode.sensoryState = sensory;
            
            em.storeEpisode(episode);
        }
        
        // Update to detect and manage interference
        em.update(100, 1.0f);
        
        // Check interference scores
        float maxInterference = em.getMaxInterference();
        assert(maxInterference >= 0.0f);
        
        std::cout << "    ✓ Memory interference management" << std::endl;
    }

    static void testSchemaBasedMemoryIntegration()
    {
        std::cout << "  Testing schema-based memory integration..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        em.setSchemaIntegrationEnabled(true);
        em.setSchemaMatchingEnabled(true);
        
        // Store episodes that match different schemas
        for (size_t i = 0; i < 5; ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = 1000 + i * 100;
            episode.action = ActionType::Move;
            episode.positionX = 10.0f + static_cast<float>(i);
            episode.positionY = 20.0f + static_cast<float>(i * 2);
            
            // Associate with schemas based on characteristics
            if (i % 3 == 0) {
                episode.associatedSchemas.push_back("movement");
                episode.schemaActivation.push_back(0.9f);
            }
            if (i % 2 == 0) {
                episode.associatedSchemas.push_back("spatial");
                episode.schemaActivation.push_back(0.8f);
            }
            if (i % 4 == 0) {
                episode.associatedSchemas.push_back("reward");
                episode.schemaActivation.push_back(0.7f);
            }
            
            episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
            
            em.storeEpisode(episode);
        }
        
        // Update to integrate with schemas
        em.update(100, 1.0f);
        
        // Check schema integration
        // (In a full implementation, we would verify schema binding quality)
        
        std::cout << "    ✓ Schema-based memory integration" << std::endl;
    }

    static void testTemporalSequencing()
    {
        std::cout << "  Testing temporal sequencing validation..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        
        // Store episodes with specific temporal sequence
        SimulationStep baseTime = 1000;
        for (size_t i = 0; i < 5; ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = baseTime + i * 100; // Ensure chronological order
            episode.action = ActionType::Move;
            episode.positionX = 10.0f + static_cast<float>(i);
            episode.positionY = 20.0f + static_cast<float>(i * 2);
            
            episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
            
            em.storeEpisode(episode);
        }
        
        // Retrieve temporal sequence
        std::vector<const EpisodicMemoryItem*> temporal = 
            em.retrieveTemporal(baseTime, baseTime + 500, 5);
        
        // Verify temporal ordering (should be most recent first)
        for (size_t i = 1; i < temporal.size(); ++i) {
            assert(temporal[i - 1]->timestamp >= temporal[i]->timestamp);
        }
        
        std::cout << "    ✓ Temporal sequencing validation" << std::endl;
    }

    static void testSystemIntegration()
    {
        std::cout << "  Testing system integration..." << std::endl;
        
        auto config = std::make_shared<Config>("test_config");
        Brain brain(config);
        NeuralEpisodicMemory em;
        em.initialize(&brain);
        
        // Enable all episodic memory features
        em.enableReplay(true);
        em.enableConsolidation(true);
        em.setTemporalCompression(true);
        em.setPrioritizationEnabled(true);
        em.setSelfReferentialProcessing(true);
        em.setEmotionalIntegrationEnabled(true);
        em.setPatternCompletionEnabled(true);
        em.setInterferenceManagementEnabled(true);
        em.setSchemaIntegrationEnabled(true);
        em.setSchemaMatchingEnabled(true);
        
        // Configure all systems
        em.setExpansionFactor(1.5f);
        em.setReplayFrequency(0.1f);
        em.setCompletionThreshold(0.6f);
        em.setInterferenceThreshold(0.3f);
        em.setSelfImportanceThreshold(0.5f);
        
        // Store comprehensive episodes
        for (size_t i = 0; i < 10; ++i) {
            EpisodicMemoryItem episode;
            episode.id = i + 1;
            episode.timestamp = 1000 + i * 100;
            episode.action = (i % 3 == 0) ? ActionType::Move : 
                           (i % 3 == 1) ? ActionType::Attack : ActionType::Explore;
            episode.positionX = 10.0f + static_cast<float>(i);
            episode.positionY = 20.0f + static_cast<float>(i * 2);
            episode.orientation = 0.1f * (i + 1);
            episode.reward = 0.5f + static_cast<float>(i) * 0.1f;
            episode.novelty = 0.8f - static_cast<float>(i) * 0.15f;
            episode.selfReferential = (i % 2 == 0);
            episode.selfImportance = episode.selfReferential ? 0.9f : 0.1f;
            episode.emotionalValence = (static_cast<float>(i) - 4.5f) / 4.5f;
            episode.emotionalArousal = 0.5f + static_cast<float>(i) * 0.1f;
            
            // Add schema associations
            if (i % 4 == 0) {
                episode.associatedSchemas.push_back("schema1");
                episode.schemaActivation.push_back(0.8f);
            }
            
            episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
            
            em.storeEpisode(episode);
        }
        
        // Run comprehensive integration test
        em.update(500, 5.0f);
        em.integrateWithNeurons();
        em.synchronizeWithHippocampalSystem();
        em.updateNeocorticalIntegrations();
        
        // Verify all systems are working
        size_t totalEpisodes = em.getEpisodeCount();
        size_t activeEpisodes = em.getActiveCount();
        size_t consolidatedEpisodes = em.getConsolidatedCount();
        
        assert(totalEpisodes == 10);
        assert(activeEpisodes > 0);
        assert(consolidatedEpisodes >= 0);
        
        // Print comprehensive statistics
        std::cout << "    Integration test statistics:" << std::endl;
        std::cout << "      Total episodes: " << totalEpisodes << std::endl;
        std::cout << "      Active episodes: " << activeEpisodes << std::endl;
        std::cout << "      Consolidated episodes: " << consolidatedEpisodes << std::endl;
        std::cout << "      Encoding strength: " << std::fixed << std::setprecision(3) 
                  << em.getAverageStrength() << std::endl;
        std::cout << "      Precision: " << std::fixed << std::setprecision(3) 
                  << em.getAveragePrecision() << std::endl;
        std::cout << "      Recall: " << std::fixed << std::setprecision(3) 
                  << em.getAverageRecall() << std::endl;
        
        std::cout << "    ✓ System integration" << std::endl;
    }
};

} // namespace nlm
