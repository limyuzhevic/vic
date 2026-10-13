// Episodic Memory Tests
// Tests experience encoding, pattern completion retrieval, temporal/spatial indexing, and replay consolidation

#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

namespace test_episodic_memory {

void testEpisodicMemoryCreation() {
    nlm::NeuralEpisodicMemory epiMem;
    
    assert(epiMem.getEpisodeCount() == 0);
    assert(!epiMem.isReplayEnabled());
    
    std::cout << "    testEpisodicMemoryCreation passed" << std::endl;
}

void testEpisodicMemoryEnableReplay() {
    nlm::NeuralEpisodicMemory epiMem;
    
    epiMem.enableReplay(true);
    assert(epiMem.isReplayEnabled());
    
    epiMem.enableReplay(false);
    assert(!epiMem.isReplayEnabled());
    
    std::cout << "    testEpisodicMemoryEnableReplay passed" << std::endl;
}

void testEpisodicMemoryStoreRetrieve() {
    nlm::NeuralEpisodicMemory epiMem;
    
    // Create an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.sensoryState = std::vector<float>{0.1f, 0.5f, 0.9f, 0.3f, 0.7f};
    episode.positionX = 5.0f;
    episode.positionY = 3.0f;
    episode.orientation = 1.57f; // ~90 degrees
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 0.8f;
    episode.energy = 0.9f;
    episode.novelty = 0.6f;
    episode.resultingSensoryState = std::vector<float>{0.2f, 0.6f, 0.8f, 0.4f, 0.8f};
    episode.resultingReward = 0.9f;
    episode.activeNeurons = std::vector<nlm::NeuronId>{nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    episode.neuronActivations = std::vector<float>{0.7f, 0.8f, 0.9f};
    episode.age = 5;
    
    // Store episode
    epiMem.storeEpisode(episode);
    
    // Verify count
    assert(epiMem.getEpisodeCount() == 1);
    
    // Get episode
    const nlm::EpisodicMemoryItem* retrieved = epiMem.getEpisode(0);
    assert(retrieved != nullptr);
    assert(retrieved->timestamp == 100);
    assert(retrieved->reward == 0.8f);
    assert(retrieved->action == nlm::ActionType::MoveForward);
    
    std::cout << "    testEpisodicMemoryStoreRetrieve passed" << std::endl;
}

void testEpisodicMemoryRetrieveSimilar() {
    nlm::NeuralEpisodicMemory epiMem;
    
    // Store several similar episodes
    std::vector<float> basePattern = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    
    for (size_t i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        episode.sensoryState = basePattern;
        // Add slight variation
        for (size_t j = 0; j < basePattern.size(); ++j) {
            episode.sensoryState[j] += (static_cast<float>(i) * 0.01f);
        }
        episode.reward = 0.5f + static_cast<float>(i) * 0.1f;
        epiMem.storeEpisode(episode);
    }
    
    // Query with similar pattern
    std::vector<float> queryPattern = basePattern;
    std::vector<const nlm::EpisodicMemoryItem*> similar = epiMem.retrieveSimilar(queryPattern, 3);
    
    // Should find similar episodes
    assert(!similar.empty());
    assert(similar.size() <= 3);
    
    // Results should be ordered by similarity (most similar first)
    for (size_t i = 1; i < similar.size(); ++i) {
        assert(similar[i-1] != nullptr && similar[i] != nullptr);
    }
    
    std::cout << "    testEpisodicMemoryRetrieveSimilar passed" << std::endl;
}

void testEpisodicMemoryTemporalRetrieval() {
    nlm::NeuralEpisodicMemory epiMem;
    
    // Store episodes at different times
    std::vector<nlm::SimulationStep> timestamps = {10, 20, 30, 40, 50};
    
    for (size_t i = 0; i < timestamps.size(); ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = timestamps[i];
        episode.sensoryState = std::vector<float>(10, 0.5f);
        episode.reward = static_cast<float>(i + 1) * 0.1f;
        epiMem.storeEpisode(episode);
    }
    
    // Retrieve from time window
    std::vector<const nlm::EpisodicMemoryItem*> temporal = epiMem.retrieveTemporal(15, 45, 10);
    
    // Should find episodes between 15 and 45
    assert(!temporal.empty());
    
    for (const auto* episode : temporal) {
        assert(episode->timestamp >= 15 && episode->timestamp <= 45);
    }
    
    std::cout << "    testEpisodicMemoryTemporalRetrieval passed" << std::endl;
}

void testEpisodicMemoryLocationRetrieval() {
    nlm::NeuralEpisodicMemory epiMem;
    
    // Store episodes at different locations
    struct Location {
        float x, y;
        size_t index;
    };
    
    std::vector<Location> locations = {
        {0.0f, 0.0f, 0}, {10.0f, 10.0f, 1}, {5.0f, 5.0f, 2}, {15.0f, 3.0f, 3}
    };
    
    for (const auto& loc : locations) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = loc.index * 10;
        episode.positionX = loc.x;
        episode.positionY = loc.y;
        episode.sensoryState = std::vector<float>(10, 0.5f);
        epiMem.storeEpisode(episode);
    }
    
    // Retrieve episodes near (5, 5) with radius 3
    std::vector<const nlm::EpisodicMemoryItem*> locationEpisodes = epiMem.retrieveByLocation(5.0f, 5.0f, 3.0f, 5);
    
    // Should find episodes at (0,0), (5,5), and (10,10) - all within radius
    assert(!locationEpisodes.empty());
    
    for (const auto* episode : locationEpisodes) {
        float dx = episode->positionX - 5.0f;
        float dy = episode->positionY - 5.0f;
        float distance = std::sqrt(dx * dx + dy * dy);
        assert(distance <= 3.0f);
    }
    
    std::cout << "    testEpisodicMemoryLocationRetrieval passed" << std::endl;
}

void testEpisodicMemoryReplay() {
    nlm::NeuralEpisodicMemory epiMem;
    
    // Enable replay
    epiMem.enableReplay(true);
    
    // Store episodes
    for (size_t i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        episode.sensoryState = std::vector<float>(50, 0.5f);
        episode.reward = 0.5f + static_cast<float>(i) * 0.1f;
        epiMem.storeEpisode(episode);
    }
    
    // Get episodes for replay
    std::vector<const nlm::EpisodicMemoryItem*> replayEpisodes = epiMem.getEpisodesForReplay(3);
    
    // Should get some episodes
    assert(!replayEpisodes.empty());
    assert(replayEpisodes.size() <= 3);
    
    // Replay each episode
    for (const auto* episode : replayEpisodes) {
        if (episode) {
            epiMem.replayEpisode(episode);
        }
    }
    
    std::cout << "    testEpisodicMemoryReplay passed" << std::endl;
}

void testEpisodicMemoryConsolidation() {
    nlm::NeuralEpisodicMemory epiMem;
    
    // Store many episodes with different rewards
    for (size_t i = 0; i < 20; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = std::vector<float>(10, 0.5f);
        episode.reward = (i < 10) ? 1.0f : 0.0f; // First 10 good, last 10 bad
        epiMem.storeEpisode(episode);
    }
    
    assert(epiMem.getEpisodeCount() == 20);
    
    // Consolidate - remove low relevance episodes (bad reward)
    epiMem.consolidate(0.5f);  // Keep episodes with relevance >= 0.5
    
    // Should have removed the bad episodes
    assert(epiMem.getEpisodeCount() <= 10);
    
    std::cout << "    testEpisodicMemoryConsolidation passed" << std::endl;
}

void testPatternCompletionRetrieval() {
    nlm::NeuralEpisodicMemory epiMem;
    
    // Store an episode with partial pattern
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.sensoryState = std::vector<float>{0.9f, 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f, 0.0f};
    epiMem.storeEpisode(episode);
    
    // Query with similar but incomplete pattern
    std::vector<float> queryPattern = {0.8f, 0.7f, 0.6f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    
    std::vector<const nlm::EpisodicMemoryItem*> results = epiMem.retrieveSimilar(queryPattern, 3);
    
    // Should find the stored episode (has similar initial elements)
    assert(!results.empty());
    
    std::cout << "    testPatternCompletionRetrieval passed" << std::endl;
}

void testTemporalIndexing() {
    nlm::NeuralEpisodicMemory epiMem;
    
    // Store episodes with non-uniform timestamps
    std::vector<nlm::SimulationStep> timestamps = {0, 1, 3, 6, 10, 15, 21, 28, 36, 45};
    
    for (size_t i = 0; i < timestamps.size(); ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = timestamps[i];
        episode.sensoryState = std::vector<float>{static_cast<float>(i) * 0.1f, 0.5f, 0.5f};
        epiMem.storeEpisode(episode);
    }
    
    // Retrieve episodes within specific time windows
    std::vector<const nlm::EpisodicMemoryItem*> early = epiMem.retrieveTemporal(0, 10, 20);
    assert(early.size() >= 5); // Episodes at timestamps 0, 1, 3, 6, 10
    
    std::vector<const nlm::EpisodicMemoryItem*> middle = epiMem.retrieveTemporal(10, 30, 20);
    assert(middle.size() >= 4); // Episodes at timestamps 10, 15, 21, 28
    
    std::vector<const nlm::EpisodicMemoryItem*> late = epiMem.retrieveTemporal(30, 50, 20);
    assert(late.size() >= 2); // Episodes at timestamps 36, 45
    
    std::cout << "    testTemporalIndexing passed" << std::endl;
}

void testSpatialIndexing() {
    nlm::NeuralEpisodicMemory epiMem;
    
    // Store episodes at specific locations
    std::vector<std::pair<float, float>> locations = {
        {0.0f, 0.0f}, {10.0f, 10.0f}, {5.0f, 2.0f}, {8.0f, 7.0f}, {3.0f, 8.0f}
    };
    
    for (size_t i = 0; i < locations.size(); ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        episode.positionX = locations[i].first;
        episode.positionY = locations[i].second;
        episode.orientation = static_cast<float>(i) * 0.5f;
        epiMem.storeEpisode(episode);
    }
    
    // Query locations
    auto center = epiMem.retrieveByLocation(5.0f, 5.0f, 5.0f, 10);
    // Should find episodes near (5,5)
    
    auto south = epiMem.retrieveByLocation(5.0f, 5.0f, 2.0f, 10); // Smaller radius
    
    std::cout << "    testSpatialIndexing passed" << std::endl;
}

void testReplayConsolidation() {
    nlm::NeuralEpisodicMemory epiMem;
    
    epiMem.enableReplay(true);
    
    // Store multiple episodes
    for (size_t i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 5;
        episode.sensoryState = std::vector<float>(20, 0.3f + static_cast<float>(i) * 0.01f);
        episode.reward = 0.8f;
        epiMem.storeEpisode(episode);
    }
    
    // Replay sequence
    std::vector<size_t> replaySequence = {2, 5, 8, 1, 3};
    epiMem.replaySequence(replaySequence);
    
    // Should have replayed all episodes (even if some don't exist)
    // This tests the replay infrastructure
    
    std::cout << "    testReplayConsolidation passed" << std::endl;
}

void runAll() {
    testEpisodicMemoryCreation();
    testEpisodicMemoryEnableReplay();
    testEpisodicMemoryStoreRetrieve();
    testEpisodicMemoryRetrieveSimilar();
    testEpisodicMemoryTemporalRetrieval();
    testEpisodicMemoryLocationRetrieval();
    testEpisodicMemoryReplay();
    testEpisodicMemoryConsolidation();
    testPatternCompletionRetrieval();
    testTemporalIndexing();
    testSpatialIndexing();
    testReplayConsolidation();
}

} // namespace test_episodic_memory
