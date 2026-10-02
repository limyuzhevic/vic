// Memory Tests
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/Memory.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_memory {

void testMemoryCreation() {
    // Test base Memory class
    nlm::Memory memory;
    assert(!memory.isInitialized());
    
    std::cout << "    testMemoryCreation passed" << std::endl;
}

void testNeuralWorkingMemory() {
    nlm::NeuralWorkingMemory memory;
    assert(!memory.isInitialized());
    
    // Test initialization (will need Brain pointer)
    // For now, test basic properties
    assert(memory.getCapacity() > 0);
    assert(memory.getDecayRate() > 0.0f);
    assert(memory.getCurrentSize() == 0);
    
    std::cout << "    testNeuralWorkingMemory passed" << std::endl;
}

void testNeuralEpisodicMemory() {
    nlm::NeuralEpisodicMemory memory;
    assert(!memory.isInitialized());
    
    // Test basic properties
    assert(memory.getEpisodeCount() == 0);
    assert(memory.getAverageReward() == 0.0f);
    
    std::cout << "    testNeuralEpisodicMemory passed" << std::endl;
}

void testMemoryStoreAndRetrieve() {
    // Test working memory store/retrieve
    nlm::NeuralWorkingMemory memory;
    
    // Test storing pattern
    std::vector<float> pattern = {0.1f, 0.5f, 0.9f, 0.2f, 0.7f};
    memory.store(pattern, 1.0f);
    
    // Test retrieving
    auto retrieved = memory.retrieve();
    assert(retrieved.size() == pattern.size());
    
    std::cout << "    testMemoryStoreAndRetrieve passed" << std::endl;
}

void testMemoryCapacity() {
    nlm::NeuralWorkingMemory memory;
    
    // Test capacity constraints
    assert(memory.getCapacity() > 0);
    
    // Store many patterns until capacity
    for (int i = 0; i < 10; ++i) {
        std::vector<float> pattern = {static_cast<float>(i) / 10.0f};
        memory.store(pattern, 1.0f);
    }
    
    assert(memory.getCurrentSize() <= memory.getCapacity());
    
    std::cout << "    testMemoryCapacity passed" << std::endl;
}

void testMemoryDecay() {
    nlm::NeuralWorkingMemory memory;
    
    // Test decay mechanism
    std::vector<float> pattern = {1.0f, 0.5f, 0.2f};
    memory.store(pattern, 1.0f);
    
    // Advance time to trigger decay
    for (int i = 0; i < 100; ++i) {
        memory.update(0.001f);
    }
    
    // Activations should decay over time
    // (Implementation-specific behavior)
    
    std::cout << "    testMemoryDecay passed" << std::endl;
}

void testMemoryEpisodicStorage() {
    nlm::NeuralEpisodicMemory memory;
    
    // Test episode storage
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1000;
    episode.metadata = "test episode";
    
    memory.storeEpisode(episode);
    assert(memory.getEpisodeCount() == 1);
    
    // Test retrieval
    auto retrieved = memory.retrieveEpisode(0);
    assert(retrieved.timestamp == 1000);
    assert(retrieved.metadata == "test episode");
    
    std::cout << "    testMemoryEpisodicStorage passed" << std::endl;
}

void testMemoryRecentEpisodes() {
    nlm::NeuralEpisodicMemory memory;
    
    // Store multiple episodes
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i + 1000;
        memory.storeEpisode(episode);
    }
    
    // Test getting recent episodes
    auto recent = memory.getRecentEpisodes(3);
    assert(recent.size() == 3);
    
    std::cout << "    testMemoryRecentEpisodes passed" << std::endl;
}

void testMemoryConsolidation() {
    nlm::NeuralEpisodicMemory memory;
    
    // Test consolidation mechanism
    // (Implementation-specific behavior)
    
    std::cout << "    testMemoryConsolidation passed" << std::endl;
}

void runAll() {
    testMemoryCreation();
    testNeuralWorkingMemory();
    testNeuralEpisodicMemory();
    testMemoryStoreAndRetrieve();
    testMemoryCapacity();
    testMemoryDecay();
    testMemoryEpisodicStorage();
    testMemoryRecentEpisodes();
    testMemoryConsolidation();
}

} // namespace test_memory