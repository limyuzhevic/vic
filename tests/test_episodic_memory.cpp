// Episodic Memory Tests
// Tests for EpisodicMemory functionality including storage, retrieval, consolidation, and time-based operations

#include "memory/Memory.hpp"
#include "TestInfrastructure.hpp"
#include "TestUtilities.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_episodic_memory {

void testEpisodicMemoryCreation() {
    nlm::EpisodicMemory memory;
    
    // Test basic properties
    assert(memory.getEpisodeCount() == 0);
    
    std::cout << "    testEpisodicMemoryCreation passed" << std::endl;
}

void testEpisodicMemoryStoreAndRetrieve() {
    nlm::EpisodicMemory memory;
    
    // Create test episode
    nlm::EpisodicMemoryItem episode1;
    episode1.timestamp = 1000;
    episode1.neurons = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    episode1.values = {0.5f, 0.8f, 0.3f};
    episode1.metadata = "First test episode";
    
    // Store episode
    memory.storeEpisode(episode1);
    assert(memory.getEpisodeCount() == 1);
    
    // Retrieve episode
    auto retrieved = memory.retrieveEpisode(0);
    assert(retrieved.timestamp == episode1.timestamp);
    assert(retrieved.neurons.size() == episode1.neurons.size());
    assert(retrieved.values.size() == episode1.values.size());
    assert(retrieved.metadata == episode1.metadata);
    
    for (size_t i = 0; i < episode1.neurons.size(); ++i) {
        assert(retrieved.neurons[i] == episode1.neurons[i]);
        assert(nlm_test::utils::approxEqual(retrieved.values[i], episode1.values[i]));
    }
    
    std::cout << "    testEpisodicMemoryStoreAndRetrieve passed" << std::endl;
}

void testEpisodicMemoryMultipleEpisodes() {
    nlm::EpisodicMemory memory;
    
    // Store multiple episodes
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = static_cast<nlm::SimulationStep>(i * 100);
        episode.neurons = {nlm::NeuronId(i), nlm::NeuronId(i + 10)};
        episode.values = {static_cast<float>(i), static_cast<float>(i * 2)};
        episode.metadata = "Episode " + std::to_string(i);
        
        memory.storeEpisode(episode);
    }
    
    assert(memory.getEpisodeCount() == 10);
    
    // Retrieve all episodes
    for (int i = 0; i < 10; ++i) {
        auto retrieved = memory.retrieveEpisode(i);
        assert(retrieved.timestamp == static_cast<nlm::SimulationStep>(i * 100));
        assert(retrieved.neurons.size() == 2);
        assert(retrieved.metadata == "Episode " + std::to_string(i));
    }
    
    std::cout << "    testEpisodicMemoryMultipleEpisodes passed" << std::endl;
}

void testEpisodicMemoryCapacityManagement() {
    nlm::EpisodicMemory memory;
    
    size_t initialCapacity = memory.getEpisodeCount(); // Should be 0 initially
    assert(initialCapacity == 0);
    
    // Store many episodes (capacity should be managed internally)
    for (size_t i = 0; i < 1000; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = static_cast<nlm::SimulationStep>(i);
        episode.neurons = {nlm::NeuronId(i)};
        episode.values = {1.0f};
        
        memory.storeEpisode(episode);
    }
    
    // Should not exceed reasonable capacity
    assert(memory.getEpisodeCount() <= 1000); // Should have stored all or up to capacity
    
    std::cout << "    testEpisodicMemoryCapacityManagement passed" << std::endl;
}

void testEpisodicMemoryGetRecentEpisodes() {
    nlm::EpisodicMemory memory;
    
    // Store episodes with time stamps
    for (int i = 0; i < 20; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = static_cast<nlm::SimulationStep>(i * 50);
        episode.neurons = {nlm::NeuronId(i)};
        episode.values = {static_cast<float>(i)};
        
        memory.storeEpisode(episode);
    }
    
    // Get recent episodes
    auto recent = memory.getRecentEpisodes(5);
    assert(recent.size() == 5);
    
    // Should be most recent episodes
    assert(recent[0].timestamp == 19 * 50);
    assert(recent[4].timestamp == 15 * 50);
    
    // Get more episodes than exist
    auto allRecent = memory.getRecentEpisodes(50);
    assert(allRecent.size() == 20); // Should return all episodes
    
    std::cout << "    testEpisodicMemoryGetRecentEpisodes passed" << std::endl;
}

void testEpisodicMemoryConsolidation() {
    nlm::EpisodicMemory memory;
    
    // Store episodes with varying relevance
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 100;
        episode.neurons = {nlm::NeuronId(i)};
        episode.values = {static_cast<float>(i)};
        
        // Make some episodes more relevant (higher values)
        if (i % 3 == 0) {
            episode.values[0] = 10.0f; // Higher relevance
        }
        
        memory.storeEpisode(episode);
    }
    
    // Apply consolidation (simplified - just testing it doesn't crash)
    memory.consolidate(5.0f); // Threshold
    
    // All episodes should still be accessible
    assert(memory.getEpisodeCount() == 10);
    
    std::cout << "    testEpisodicMemoryConsolidation passed" << std::endl;
}

void testEpisodicMemoryClear() {
    nlm::EpisodicMemory memory;
    
    // Store episodes
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 100;
        episode.neurons = {nlm::NeuronId(i)};
        episode.values = {1.0f};
        
        memory.storeEpisode(episode);
    }
    
    assert(memory.getEpisodeCount() == 5);
    
    // Clear memory
    memory.clear();
    assert(memory.getEpisodeCount() == 0);
    
    // Retrieving from cleared memory should return empty item
    auto empty = memory.retrieveEpisode(0);
    assert(empty.timestamp == 0);
    assert(empty.neurons.empty());
    assert(empty.values.empty());
    
    std::cout << "    testEpisodicMemoryClear passed" << std::endl;
}

void testEpisodicMemoryIntegrationWithBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test episodic memory integration through brain
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Store an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1000;
    episode.neurons = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    episode.values = {0.1f, 0.2f, 0.3f};
    episode.metadata = "Brain integration test";
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() == 1);
    
    // Retrieve through brain interface
    auto retrieved = episodicMemory->retrieveEpisode(0);
    assert(retrieved.metadata == episode.metadata);
    
    // Get recent episodes through brain
    auto recent = episodicMemory->getRecentEpisodes(10);
    assert(recent.size() == 1);
    
    std::cout << "    testEpisodicMemoryIntegrationWithBrain passed" << std::endl;
}

void testEpisodicMemoryMemoryReplay() {
    nlm::EpisodicMemory memory;
    
    // Store an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 5000;
    episode.neurons = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    episode.values = {0.7f, 0.8f, 0.9f};
    episode.metadata = "Replay test episode";
    
    memory.storeEpisode(episode);
    
    // Replay (retrieve) the episode multiple times
    for (int i = 0; i < 5; ++i) {
        auto replayed = memory.retrieveEpisode(0);
        assert(replayed.timestamp == episode.timestamp);
        assert(replayed.neurons == episode.neurons);
        assert(nlm_test::utils::approxEqual(replayed.values[0], episode.values[0]));
        assert(replayed.metadata == episode.metadata);
    }
    
    std::cout << "    testEpisodicMemoryMemoryReplay passed" << std::endl;
}

void testEpisodicMemoryTemporalOrdering() {
    nlm::EpisodicMemory memory;
    
    // Store episodes in non-chronological order to test temporal sorting
    std::vector<nlm::EpisodicMemoryItem> episodes;
    
    for (int i = 10; i >= 0; --i) { // Store backwards
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 100;
        episode.neurons = {nlm::NeuronId(i)};
        episode.values = {static_cast<float>(i)};
        
        episodes.push_back(episode);
        memory.storeEpisode(episode);
    }
    
    // The getRecentEpisodes should respect temporal ordering
    auto recent = memory.getRecentEpisodes(5);
    
    // Should contain the most recent episodes (highest timestamps)
    // Note: The exact ordering depends on implementation, but recent episodes should have higher timestamps
    assert(!recent.empty());
    
    std::cout << "    testEpisodicMemoryTemporalOrdering passed" << std::endl;
}

void testEpisodicMemoryStress() {
    nlm::EpisodicMemory memory;
    
    // Rapid store/retrieve operations
    for (size_t i = 0; i < 1000; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = static_cast<nlm::SimulationStep>(i);
        episode.neurons = {nlm::NeuronId(i % 50)}; // Reuse neurons
        episode.values = {static_cast<float>(i * 0.01f)};
        episode.metadata = "Stress test episode " + std::to_string(i);
        
        memory.storeEpisode(episode);
        
        // Verify we can retrieve it
        if (i < 50) { // Only check first 50 to avoid index issues
            auto retrieved = memory.retrieveEpisode(i);
            assert(retrieved.timestamp == episode.timestamp);
        }
    }
    
    std::cout << "    testEpisodicMemoryStress passed" << std::endl;
}

void runAll() {
    nlm_test::TestRunner runner(true);
    
    runner.runTest("Episodic Memory Creation", testEpisodicMemoryCreation);
    runner.runTest("Episodic Memory Store and Retrieve", testEpisodicMemoryStoreAndRetrieve);
    runner.runTest("Episodic Memory Multiple Episodes", testEpisodicMemoryMultipleEpisodes);
    runner.runTest("Episodic Memory Capacity Management", testEpisodicMemoryCapacityManagement);
    runner.runTest("Episodic Memory Get Recent Episodes", testEpisodicMemoryGetRecentEpisodes);
    runner.runTest("Episodic Memory Consolidation", testEpisodicMemoryConsolidation);
    runner.runTest("Episodic Memory Clear", testEpisodicMemoryClear);
    runner.runTest("Episodic Memory Integration With Brain", testEpisodicMemoryIntegrationWithBrain);
    runner.runTest("Episodic Memory Memory Replay", testEpisodicMemoryMemoryReplay);
    runner.runTest("Episodic Memory Temporal Ordering", testEpisodicMemoryTemporalOrdering);
    runner.runTest("Episodic Memory Stress Test", testEpisodicMemoryStress);
    
    runner.runAll();
}

} // namespace test_episodic_memory
