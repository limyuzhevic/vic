// Episodic Memory Integration Tests
// Tests for NeuralEpisodicMemory system integration with other systems

#pragma once

#include "memory/NeuralEpisodicMemory.hpp"
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
namespace test_episodicmemory {

class EpisodicMemoryIntegrationTests : public IntegrationTestFramework {
public:
    EpisodicMemoryIntegrationTests() {
        auto config = getConfig();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    }
    
    ~EpisodicMemoryIntegrationTests() override = default;
    
    void runAllTests() {
        std::cout << "=== Episodic Memory Integration Tests ===" << std::endl;
        
        testEpisodicMemoryStorageFromExperiences();
        testEpisodicMemoryIntegrationWithWorkingMemory();
        testEpisodicMemoryConsolidationAndReplay();
        testEpisodicMemorySleepRestCycleIntegration();
        testEpisodicMemoryReplayMechanisms();
        
        std::cout << "=== All Episodic Memory Integration Tests PASSED ===" << std::endl;
    }
    
private:
    void testEpisodicMemoryStorageFromExperiences() {
        logTestStart("Episodic Memory Storage from Experiences");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        // Create episodic memory items from experiences
        std::vector<EpisodicMemoryItem> episodes;
        
        // Episode 1: Finding food
        {
            EpisodicMemoryItem episode;
            episode.timestamp = 0;
            episode.positionX = 1.0f;
            episode.positionY = 2.0f;
            episode.orientation = 0.0f;
            episode.action = ActionType::MoveForward;
            episode.reward = 10.0f;
            episode.energy = 0.8f;
            episode.novelty = 0.7f;
            episode.stabilityScore = 0.9f;
            episode.relevanceScore = 0.8f;
            
            // Simulate active neurons
            for (size_t i = 0; i < 50; ++i) {
                episode.activeNeurons.push_back(static_cast<NeuronId>(i));
                episode.neuronActivations.push_back(static_cast<float>(i) / 50.0f);
            }
            
            episodes.push_back(episode);
        }
        
        // Episode 2: Avoiding predator
        {
            EpisodicMemoryItem episode;
            episode.timestamp = 100;
            episode.positionX = 3.0f;
            episode.positionY = 1.0f;
            episode.orientation = 180.0f;
            episode.action = ActionType::RunAway;
            episode.reward = -5.0f;
            episode.energy = 0.6f;
            episode.novelty = 0.9f;
            episode.stabilityScore = 0.7f;
            episode.relevanceScore = 0.9f;
            
            // Simulate active neurons (different pattern)
            for (size_t i = 50; i < 100; ++i) {
                episode.activeNeurons.push_back(static_cast<NeuronId>(i));
                episode.neuronActivations.push_back(1.0f - (static_cast<float>(i - 50) / 50.0f));
            }
            
            episodes.push_back(episode);
        }
        
        // Store episodes
        for (const auto& episode : episodes) {
            episodicMemory->storeEpisode(episode);
        }
        
        // Verify episodes were stored
        assert(episodicMemory->getEpisodeCount() == 2);
        
        logTestResult("Episodic Memory Storage from Experiences", true,
                     "Stored " + std::to_string(episodicMemory->getEpisodeCount()) + " episodes");
    }
    
    void testEpisodicMemoryIntegrationWithWorkingMemory() {
        logTestStart("Episodic Memory Integration with Working Memory");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* episodicMemory = brain.getEpisodicMemory();
        auto* workingMemory = brain.getWorkingMemory();
        assert(episodicMemory != nullptr && workingMemory != nullptr);
        
        // Store an episode
        EpisodicMemoryItem episode;
        episode.timestamp = 0;
        episode.positionX = 1.0f;
        episode.positionY = 2.0f;
        episode.action = ActionType::MoveForward;
        episode.reward = 10.0f;
        episode.stabilityScore = 0.8f;
        
        // Add working memory content
        for (size_t i = 0; i < 20; ++i) {
            episode.workingMemoryContent.push_back(static_cast<float>(i) / 20.0f);
        }
        
        episodicMemory->storeEpisode(episode);
        
        // Retrieve similar episodes
        std::vector<float> sensoryPattern(100, 0.0f);
        sensoryPattern[0] = 0.5f;
        
        auto similarEpisodes = episodicMemory->retrieveSimilar(sensoryPattern, 3);
        assert(similarEpisodes.size() > 0);
        
        // Replay an episode and integrate with working memory
        episodicMemory->replayEpisode(similarEpisodes[0]);
        
        // Verify integration
        assert(episodicMemory->getEpisodeCount() > 0);
        
        logTestResult("Episodic Memory Integration with Working Memory", true,
                     "Integrated episodic memory with " + std::to_string(similarEpisodes.size()) + " similar episodes");
    }
    
    void testEpisodicMemoryConsolidationAndReplay() {
        logTestStart("Episodic Memory Consolidation and Replay");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        // Store multiple episodes with different stability scores
        for (size_t i = 0; i < 10; ++i) {
            EpisodicMemoryItem episode;
            episode.timestamp = i * 10;
            episode.positionX = static_cast<float>(i);
            episode.positionY = static_cast<float>(i * 2);
            episode.action = ActionType::Wait;
            episode.reward = (i % 3 == 0) ? 5.0f : 1.0f;
            episode.stabilityScore = 0.5f + (static_cast<float>(i) / 10.0f); // Increasing stability
            episode.relevanceScore = 0.3f + (static_cast<float>(i) / 15.0f);
            
            episodicMemory->storeEpisode(episode);
        }
        
        // Consolidate with threshold
        episodicMemory->consolidate(0.6f); // Keep episodes with stability >= 0.6
        
        // Get episodes for replay
        auto episodesForReplay = episodicMemory->getEpisodesForReplay(5);
        assert(episodesForReplay.size() > 0);
        
        // Replay episodes
        episodicMemory->replaySequence(std::vector<size_t>(episodesForReplay.begin(), 
                                                          episodesForReplay.end()));
        
        logTestResult("Episodic Memory Consolidation and Replay", true,
                     "Consolidated to " + std::to_string(episodesForReplay.size()) + " episodes for replay");
    }
    
    void testEpisodicMemorySleepRestCycleIntegration() {
        logTestStart("Episodic Memory Sleep/Rest Cycle Integration");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        // Enable replay
        episodicMemory->enableReplay(true);
        
        // Store episodes
        for (size_t i = 0; i < 20; ++i) {
            EpisodicMemoryItem episode;
            episode.timestamp = i;
            episode.positionX = static_cast<float>(i);
            episode.positionY = static_cast<float>(i);
            episode.reward = static_cast<float>(i % 5);
            episode.stabilityScore = 0.1f + (static_cast<float>(i) / 50.0f);
            
            episodicMemory->storeEpisode(episode);
        }
        
        // Simulate sleep/rest cycle integration
        // During rest, episodic memory consolidation is enhanced
        episodicMemory->consolidate(0.3f); // Lower threshold during rest
        
        // Get episodes for replay during sleep
        auto sleepReplayEpisodes = episodicMemory->getEpisodesForReplay(10);
        assert(sleepReplayEpisodes.size() > 0);
        
        logTestResult("Episodic Memory Sleep/Rest Cycle Integration", true,
                     "Integrated sleep/rest consolidation with " + std::to_string(sleepReplayEpisodes.size()) + " replay episodes");
    }
    
    void testEpisodicMemoryReplayMechanisms() {
        logTestStart("Episodic Memory Replay Mechanisms");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        // Store temporal sequence of episodes
        std::vector<EpisodicMemoryItem> episodes;
        for (size_t i = 0; i < 5; ++i) {
            EpisodicMemoryItem episode;
            episode.timestamp = i * 10;
            episode.positionX = static_cast<float>(i * 2);
            episode.positionY = static_cast<float>(i * 3);
            episode.action = ActionType::MoveForward;
            episode.reward = 2.0f + static_cast<float>(i);
            episode.stabilityScore = 0.7f + (static_cast<float>(i) * 0.1f);
            
            episodes.push_back(episode);
        }
        
        for (const auto& episode : episodes) {
            episodicMemory->storeEpisode(episode);
        }
        
        // Test retrieval by time window
        auto temporalEpisodes = episodicMemory->retrieveTemporal(0, 40, 10);
        assert(temporalEpisodes.size() >= 4); // Episodes at 0, 10, 20, 30
        
        // Test retrieval by location
        auto locationEpisodes = episodicMemory->retrieveByLocation(3.0f, 6.0f, 2.0f, 5);
        assert(locationEpisodes.size() > 0);
        
        // Test retrieval by action
        auto actionEpisodes = episodicMemory->retrieveAfterAction(ActionType::MoveForward, 5);
        assert(actionEpisodes.size() >= 5);
        
        logTestResult("Episodic Memory Replay Mechanisms", true,
                     "Replay mechanisms retrieved " + std::to_string(temporalEpisodes.size()) + 
                     " temporal, " + std::to_string(locationEpisodes.size()) + 
                     " location, and " + std::to_string(actionEpisodes.size()) + " action episodes");
    }
};

void runAll() {
    EpisodicMemoryIntegrationTests tests;
    tests.runAllTests();
}

} // namespace test_episodicmemory
} // namespace nlm
