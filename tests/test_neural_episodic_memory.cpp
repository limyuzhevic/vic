// NeuralEpisodicMemory Comprehensive Unit Tests
// Phase 4: Advanced Neural Memory Tests - Critical Performance Paths

#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>

namespace test_neural_episodic_memory {

void testNeuralEpisodicMemoryCreation() {
    nlm::NeuralEpisodicMemory memory;
    assert(memory.getEpisodeCount() == 0);
    std::cout << "    testNeuralEpisodicMemoryCreation passed" << std::endl;
}

void testSimilarityComputationWithEarlyTermination() {
    nlm::NeuralEpisodicMemory memory;
    
    // Initialize with mock brain
    memory.initialize(nullptr);
    
    // Setup test episodes with different sensory patterns
    const size_t episodeCount = 100;
    std::vector<nlm::NeuralEpisodicMemory::EpisodicMemoryItem> episodes;
    
    // Create episodes with varying similarity
    for (size_t i = 0; i < episodeCount; ++i) {
        nlm::NeuralEpisodicMemory::EpisodicMemoryItem episode;
        episode.timestamp = static_cast<nlm::SimulationStep>(i * 10);
        episode.reward = 0.5f + (static_cast<float>(i) * 0.01f);
        episode.positionX = static_cast<float>(i % 10) * 0.1f;
        episode.positionY = static_cast<float>((i * 3) % 10) * 0.1f;
        
        // Create sensory pattern
        std::vector<float> pattern(50, 0.0f);
        for (size_t j = 0; j < 10 && j < pattern.size(); ++j) {
            pattern[j] = static_cast<float>(i + j) * 0.01f;
        }
        episode.sensoryState = pattern;
        
        episodes.push_back(episode);
        memory.storeEpisode(episode);
    }
    
    // Test similarity computation with early termination
    std::vector<float> queryPattern(50, 0.0f);
    for (size_t j = 0; j < 10 && j < queryPattern.size(); ++j) {
        queryPattern[j] = static_cast<float>(50 + j) * 0.01f;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    std::vector<const nlm::NeuralEpisodicMemory::EpisodicMemoryItem*> similar = 
        memory.retrieveSimilar(queryPattern, 10);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    // Should be fast due to early termination optimization
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    std::cout << "      Retrieved " << similar.size() << " similar episodes in " << duration.count() << " microseconds" << std::endl;
    assert(duration.count() < 100000); // Should complete within 100ms
    
    std::cout << "    testSimilarityComputationWithEarlyTermination passed" << std::endl;
}

void testTemporalAndSpatialIndexing() {
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(nullptr);
    
    // Setup episodes with different timestamps and positions
    const size_t episodeCount = 50;
    std::vector<nlm::NeuralEpisodicMemory::EpisodicMemoryItem> episodes;
    
    for (size_t i = 0; i < episodeCount; ++i) {
        nlm::NeuralEpisodicMemory::EpisodicMemoryItem episode;
        episode.timestamp = static_cast<nlm::SimulationStep>(i * 100);
        episode.positionX = static_cast<float>(i % 20) * 0.05f;
        episode.positionY = static_cast<float>((i * 7) % 20) * 0.05f;
        
        std::vector<float> pattern(10, 0.1f);
        episode.sensoryState = pattern;
        
        episodes.push_back(episode);
        memory.storeEpisode(episode);
    }
    
    // Test temporal indexing (O(1) timestamp lookup)
    auto startTime = std::chrono::high_resolution_clock::now();
    std::vector<const nlm::NeuralEpisodicMemory::EpisodicMemoryItem*> temporalResults = 
        memory.retrieveTemporal(200, 800, 10);
    auto temporalEndTime = std::chrono::high_resolution_clock::now();
    
    // Should be O(1) per lookup
    auto temporalDuration = std::chrono::duration_cast<std::chrono::microseconds>(temporalEndTime - startTime);
    std::cout << "      Temporal lookup retrieved " << temporalResults.size() << " episodes in " << temporalDuration.count() << " microseconds" << std::endl;
    
    // Test spatial indexing (O(1) grid-based lookup)
    startTime = std::chrono::high_resolution_clock::now();
    std::vector<const nlm::NeuralEpisodicMemory::EpisodicMemoryItem*> spatialResults = 
        memory.retrieveByLocation(0.5f, 0.3f, 2.0f, 10);
    auto spatialEndTime = std::chrono::high_resolution_clock::now();
    
    auto spatialDuration = std::chrono::duration_cast<std::chrono::microseconds>(spatialEndTime - startTime);
    std::cout << "      Spatial lookup retrieved " << spatialResults.size() << " episodes in " << spatialDuration.count() << " microseconds" << std::endl;
    
    assert(temporalDuration.count() < 1000);
    assert(spatialDuration.count() < 1000);
    
    std::cout << "    testTemporalAndSpatialIndexing passed" << std::endl;
}

void testPriorityQueueOperations() {
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(nullptr);
    
    // Setup episodes with varying recency and reward
    const size_t episodeCount = 100;
    for (size_t i = 0; i < episodeCount; ++i) {
        nlm::NeuralEpisodicMemory::EpisodicMemoryItem episode;
        episode.timestamp = static_cast<nlm::SimulationStep>(episodeCount - i); // Reverse chronological
        episode.age = static_cast<size_t>(i);
        episode.reward = 0.5f + static_cast<float>(std::rand() % 100) / 100.0f;
        
        std::vector<float> pattern(20, 0.1f);
        episode.sensoryState = pattern;
        
        memory.storeEpisode(episode);
    }
    
    // Test priority queue for replay episodes (O(n log k) instead of O(n log n))
    auto startTime = std::chrono::high_resolution_clock::now();
    std::vector<const nlm::NeuralEpisodicMemory::EpisodicMemoryItem*> replayEpisodes = 
        memory.getEpisodesForReplay(10);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    std::cout << "      Priority queue retrieved " << replayEpisodes.size() << " episodes in " << duration.count() << " microseconds" << std::endl;
    assert(replayEpisodes.size() <= 10);
    assert(duration.count() < 1000);
    
    // Verify episodes are sorted by score (recency + reward)
    for (size_t i = 1; i < replayEpisodes.size(); ++i) {
        const auto* prev = replayEpisodes[i - 1];
        const auto* current = replayEpisodes[i];
        assert(prev->reward >= current->reward || prev->age <= current->age);
    }
    
    std::cout << "    testPriorityQueueOperations passed" << std::endl;
}

void testMemoryPoolManagement() {
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(nullptr);
    
    // Set a smaller max episodes to trigger memory pool usage
    memory.setMaxEpisodes(50);
    
    const size_t initialCount = memory.getEpisodeCount();
    std::cout << "      Initial episode count: " << initialCount << std::endl;
    
    // Store many episodes to trigger pool management
    const size_t episodeCount = 200;
    for (size_t i = 0; i < episodeCount; ++i) {
        nlm::NeuralEpisodicMemory::EpisodicMemoryItem episode;
        episode.timestamp = static_cast<nlm::SimulationStep>(i);
        episode.reward = static_cast<float>(i % 100) / 100.0f;
        
        std::vector<float> pattern(30, 0.1f);
        episode.sensoryState = pattern;
        
        memory.storeEpisode(episode);
        
        // Occasionally trigger consolidation
        if (i % 50 == 0) {
            memory.consolidate(0.1f);
        }
    }
    
    // Verify episode count doesn't exceed maximum
    size_t finalCount = memory.getEpisodeCount();
    std::cout << "      Final episode count: " << finalCount << " (max: 50)" << std::endl;
    assert(finalCount <= 50);
    
    std::cout << "    testMemoryPoolManagement passed" << std::endl;
}

void testIntegrationWithWorkingMemory() {
    // Test integration between episodic and working memory
    // This would require a Brain instance, but we can test the interface
    
    nlm::NeuralEpisodicMemory episodicMemory;
    nlm::NeuralWorkingMemory workingMemory;
    
    // Both should initialize independently
    episodicMemory.initialize(nullptr);
    workingMemory.initialize(nullptr);
    
    // Test that they can store different types of information
    nlm::NeuralEpisodicMemory::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.reward = 0.8f;
    
    std::vector<float> pattern(20, 0.1f);
    episode.sensoryState = pattern;
    
    episodicMemory.storeEpisode(episode);
    
    // Working memory can store activation patterns
    workingMemory.storeToNeuron(1001, 0.9f);
    
    assert(episodicMemory.getEpisodeCount() > 0);
    assert(workingMemory.contains(1001));
    
    std::cout << "    testIntegrationWithWorkingMemory passed" << std::endl;
}

void testErrorHandling() {
    nlm::NeuralEpisodicMemory memory;
    
    // Test null brain handling
    memory.initialize(nullptr);
    
    // Try to replay episode with null pointer (should not crash)
    const nlm::NeuralEpisodicMemory::EpisodicMemoryItem* nullEpisode = nullptr;
    memory.replayEpisode(nullEpisode);
    
    // Try to store null episode (implementation should handle gracefully)
    nlm::NeuralEpisodicMemory::EpisodicMemoryItem nullEpisode;
    nullEpisode.timestamp = 0;
    memory.storeEpisode(nullEpisode);
    
    std::cout << "    testErrorHandling passed" << std::endl;
}

void runAll() {
    std::cout << "=== NeuralEpisodicMemory Tests ===" << std::endl;
    
    std::cout << "Running NeuralEpisodicMemory tests..." << std::endl;
    
    testNeuralEpisodicMemoryCreation();
    testSimilarityComputationWithEarlyTermination();
    testTemporalAndSpatialIndexing();
    testPriorityQueueOperations();
    testMemoryPoolManagement();
    testIntegrationWithWorkingMemory();
    testErrorHandling();
    
    std::cout << "=== All NeuralEpisodicMemory Tests PASSED ===" << std::endl;
}

} // namespace test_neural_episodic_memory
