#include <gtest/gtest.h>
#include "../memory/Memory.hpp"
#include "../brain/Neuron.hpp"

class WorkingMemoryTest : public ::testing::Test {
protected:
    nlm::WorkingMemory workingMemory;
    nlm::Neuron* neuron;
};

TEST_F(WorkingMemoryTest, InitialState) {
    EXPECT_EQ(workingMemory.getCurrentSize(), 0U);
    EXPECT_GT(workingMemory.getCapacity(), 0U);
}

TEST_F(WorkingMemoryTest, StoreAndRetrieve) {
    NeuronId testNeuron = 1;
    float testValue = 0.75f;
    
    workingMemory.store(testNeuron, testValue);
    EXPECT_TRUE(workingMemory.contains(testNeuron));
    
    float retrieved = workingMemory.retrieve(testNeuron);
    EXPECT_FLOAT_EQ(retrieved, testValue);
}

TEST_F(WorkingMemoryTest, Capacity) {
    size_t capacity = workingMemory.getCapacity();
    EXPECT_GT(capacity, 0U);
    
    // Test that capacity is enforced (in real implementation)
    workingMemory.clear();
}

TEST_F(WorkingMemoryTest, Decay) {
    NeuronId testNeuron = 1;
    float initialValue = 1.0f;
    
    workingMemory.store(testNeuron, initialValue);
    workingMemory.decay(0.1f); // 10% decay
    
    float decayedValue = workingMemory.retrieve(testNeuron);
    EXPECT_LT(decayedValue, initialValue);
}

class EpisodicMemoryTest : public ::testing::Test {
protected:
    nlm::EpisodicMemory episodicMemory;
};

TEST_F(EpisodicMemoryTest, InitialState) {
    EXPECT_EQ(episodicMemory.getEpisodeCount(), 0U);
}

TEST_F(EpisodicMemoryTest, StoreAndRetrieveEpisode) {
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1000;
    episode.neurons.push_back(1);
    episode.values.push_back(0.5f);
    
    episodicMemory.storeEpisode(episode);
    EXPECT_EQ(episodicMemory.getEpisodeCount(), 1U);
    
    auto retrieved = episodicMemory.retrieveEpisode(0);
    EXPECT_EQ(retrieved.timestamp, 1000);
    EXPECT_EQ(retrieved.neurons.size(), 1U);
}

TEST_F(EpisodicMemoryTest, GetRecentEpisodes) {
    nlm::EpisodicMemoryItem episode1;
    episode1.timestamp = 1000;
    
    nlm::EpisodicMemoryItem episode2;
    episode2.timestamp = 2000;
    
    episodicMemory.storeEpisode(episode1);
    episodicMemory.storeEpisode(episode2);
    
    auto recent = episodicMemory.getRecentEpisodes(1);
    EXPECT_EQ(recent.size(), 1U);
    EXPECT_EQ(recent[0].timestamp, 2000); // Most recent
}

class SemanticMemoryTest : public ::testing::Test {
protected:
    nlm::SemanticMemory semanticMemory;
};

TEST_F(SemanticMemoryTest, StoreAndRetrieveFact) {
    semanticMemory.storeFact("color", "red");
    EXPECT_TRUE(semanticMemory.hasFact("color"));
    
    std::string retrieved = semanticMemory.retrieveFact("color");
    EXPECT_EQ(retrieved, "red");
}

TEST_F(SemanticMemoryTest, RetrieveNonExistentFact) {
    std::string retrieved = semanticMemory.retrieveFact("nonexistent");
    EXPECT_EQ(retrieved, "");
}

TEST_F(SemanticMemoryTest, GetAllFacts) {
    semanticMemory.storeFact("color", "red");
    semanticMemory.storeFact("size", "large");
    
    auto allFacts = semanticMemory.getAllFacts();
    EXPECT_EQ(allFacts.size(), 2U);
}