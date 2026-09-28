// tests/test_episodic_memory.cpp
#include "EpisodicMemory.hpp"
#include <gtest/gtest.h>

class EpisodicMemoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        memory = std::make_unique<EpisodicMemory>();
    }
    
    std::unique_ptr<EpisodicMemory> memory;
};

TEST_F(EpisodicMemoryTest, InitialState) {
    EXPECT_EQ(memory->getEpisodeCount(), 0);
}

TEST_F(EpisodicMemoryTest, StoreAndRetrieve) {
    EpisodicMemoryItem episode;
    episode.timestamp = 1;
    episode.neurons = {1, 2, 3};
    episode.values = {0.5f, 0.7f, 0.3f};
    
    memory->storeEpisode(episode);
    EXPECT_EQ(memory->getEpisodeCount(), 1);
    
    EpisodicMemoryItem retrieved = memory->retrieveEpisode(0);
    EXPECT_EQ(retrieved.timestamp, 1);
    EXPECT_EQ(retrieved.neurons.size(), 3);
    EXPECT_FLOAT_EQ(retrieved.values[0], 0.5f);
}

TEST_F(EpisodicMemoryTest, MaxCapacity) {
    // Fill memory to capacity (default 1000)
    for (size_t i = 0; i < 1200; ++i) {
        EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.neurons = {i % 10, (i + 5) % 10}; // Add some neurons
        memory->storeEpisode(episode);
    }
    
    // Should have at most 1000 episodes (LRU eviction)
    EXPECT_LE(memory->getEpisodeCount(), 1000);
}

TEST_F(EpisodicMemoryTest, GetRecentEpisodes) {
    // Add 10 episodes
    for (size_t i = 0; i < 10; ++i) {
        EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.neurons = {i};
        memory->storeEpisode(episode);
    }
    
    auto recent = memory->getRecentEpisodes(5);
    EXPECT_EQ(recent.size(), 5);
    EXPECT_EQ(recent[0].timestamp, 5); // Most recent
    EXPECT_EQ(recent[4].timestamp, 9); // Oldest recent
}

TEST_F(EpisodicMemoryTest, Consolidation) {
    // Add episodes with different relevance levels
    EpisodicMemoryItem highRelevance;
    highRelevance.timestamp = 1;
    highRelevance.neurons = {1, 2, 3, 4, 5, 6}; // > 5 neurons = high relevance
    highRelevance.values = {0.8f, 0.9f, 0.7f, 0.6f, 0.5f, 0.4f};
    memory->storeEpisode(highRelevance);
    
    EpisodicMemoryItem mediumRelevance;
    mediumRelevance.timestamp = 2;
    mediumRelevance.neurons = {1, 2, 3}; // Medium relevance
    mediumRelevance.values = {0.3f, 0.4f, 0.5f};
    memory->storeEpisode(mediumRelevance);
    
    EpisodicMemoryItem lowRelevance;
    lowRelevance.timestamp = 3;
    lowRelevance.neurons = {1}; // Low relevance
    lowRelevance.values = {0.1f};
    memory->storeEpisode(lowRelevance);
    
    // Consolidate with threshold that should keep high and medium
    memory->consolidate(0.5f);
    
    // Should have 2 episodes (high and medium relevance)
    EXPECT_EQ(memory->getEpisodeCount(), 2);
    
    // Low relevance should be gone
    auto retrieved = memory->retrieveEpisode(2);
    EXPECT_TRUE(retrieved.neurons.empty()); // Invalid index
}