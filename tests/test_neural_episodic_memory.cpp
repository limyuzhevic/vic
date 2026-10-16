// NeuralEpisodicMemory Tests
// Tests comprehensive episodic memory functionality including:
// - Experience storage and retrieval
// - Pattern completion and similarity matching
// - Temporal and spatial memory
// - Action-based retrieval
// - Memory consolidation and replay
// - Integration with brain system

#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_neural_episodic_memory {

void testEpisodicMemoryCreation() {
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    brain.initialize();
    
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    // Initial state should be empty
    assert(memory.getEpisodeCount() == 0);
    
    std::cout << "    testEpisodicMemoryCreation passed" << std::endl;
}

void testEpisodicMemoryStoreAndRetrieve() {
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    brain.initialize();
    
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    // Create test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.positionX = 5.0f;
    episode.positionY = 10.0f;
    episode.orientation = 0.5f;
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 1.0f;
    episode.energy = 0.8f;
    episode.novelty = 0.3f;
    
    // Add sensory state
    episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    episode.resultingSensoryState = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
    episode.resultingReward = 1.5f;
    
    // Add neural activity pattern
    episode.activeNeurons = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    episode.neuronActivations = {0.8f, 0.6f, 0.4f};
    
    episode.age = 0;
    
    // Store episode
    memory.storeEpisode(episode);
    
    // Verify episode was stored
    assert(memory.getEpisodeCount() == 1);
    
    // Retrieve similar episodes
    std::vector<const nlm::EpisodicMemoryItem*> similar = 
        memory.retrieveSimilar(episode.sensoryState, 5);
    
    assert(!similar.empty());
    assert(similar[0] == &episode);
    
    std::cout << "    testEpisodicMemoryStoreAndRetrieve passed" << std::endl;
}

void testEpisodicMemoryTemporalRetrieval() {
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    brain.initialize();
    
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    // Create episodes at different times
    nlm::EpisodicMemoryItem episode1;
    episode1.timestamp = 100;
    episode1.sensoryState = {0.1f, 0.2f, 0.3f};
    
    nlm::EpisodicMemoryItem episode2;
    episode2.timestamp = 200;
    episode2.sensoryState = {0.4f, 0.5f, 0.6f};
    
    nlm::EpisodicMemoryItem episode3;
    episode3.timestamp = 300;
    episode3.sensoryState = {0.7f, 0.8f, 0.9f};
    
    memory.storeEpisode(episode1);
    memory.storeEpisode(episode2);
    memory.storeEpisode(episode3);
    
    // Retrieve episodes from time window 150-250
    std::vector<const nlm::EpisodicMemoryItem*> temporal = 
        memory.retrieveTemporal(150, 250, 10);
    
    // Should get episode2 (timestamp 200)
    assert(temporal.size() == 1);
    assert(temporal[0]->timestamp == 200);
    
    // Retrieve all episodes
    std::vector<const nlm::EpisodicMemoryItem*> all = 
        memory.retrieveTemporal(0, 1000, 10);
    
    assert(all.size() == 3);
    
    std::cout << "    testEpisodicMemoryTemporalRetrieval passed" << std::endl;
}

void testEpisodicMemoryLocationRetrieval() {
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    brain.initialize();
    
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    // Create episodes at different locations
    nlm::EpisodicMemoryItem episode1;
    episode1.positionX = 5.0f;
    episode1.positionY = 5.0f;
    episode1.sensoryState = {0.1f, 0.2f, 0.3f};
    
    nlm::EpisodicMemoryItem episode2;
    episode2.positionX = 15.0f;
    episode2.positionY = 15.0f;
    episode2.sensoryState = {0.4f, 0.5f, 0.6f};
    
    memory.storeEpisode(episode1);
    memory.storeEpisode(episode2);
    
    // Retrieve episodes near (5,5) with radius 1
    std::vector<const nlm::EpisodicMemoryItem*> near = 
        memory.retrieveByLocation(5.0f, 5.0f, 1.0f, 5);
    
    assert(near.size() == 1);
    assert(std::abs(near[0]->positionX - 5.0f) < 0.01f);
    assert(std::abs(near[0]->positionY - 5.0f) < 0.01f);
    
    // Retrieve episodes near (10,10) with radius 10 (should get both)
    std::vector<const nlm::EpisodicMemoryItem*> wide = 
        memory.retrieveByLocation(10.0f, 10.0f, 10.0f, 5);
    
    assert(wide.size() == 2);
    
    std::cout << "    testEpisodicMemoryLocationRetrieval passed" << std::endl;
}

void testEpisodicMemoryActionRetrieval() {
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    brain.initialize();
    
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    // Create episodes with different actions
    nlm::EpisodicMemoryItem episode1;
    episode1.action = nlm::ActionType::MoveForward;
    episode1.sensoryState = {0.1f, 0.2f, 0.3f};
    
    nlm::EpisodicMemoryItem episode2;
    episode2.action = nlm::ActionType::TurnLeft;
    episode2.sensoryState = {0.4f, 0.5f, 0.6f};
    
    nlm::EpisodicMemoryItem episode3;
    episode3.action = nlm::ActionType::MoveForward;  // Same action as episode1
    episode3.sensoryState = {0.7f, 0.8f, 0.9f};
    
    memory.storeEpisode(episode1);
    memory.storeEpisode(episode2);
    memory.storeEpisode(episode3);
    
    // Retrieve episodes after MoveForward action
    std::vector<const nlm::EpisodicMemoryItem*> forward = 
        memory.retrieveAfterAction(nlm::ActionType::MoveForward, 5);
    
    assert(forward.size() == 2);  // episode1 and episode3
    
    // Verify both have MoveForward action
    for (const nlm::EpisodicMemoryItem* ep : forward) {
        assert(ep->action == nlm::ActionType::MoveForward);
    }
    
    std::cout << "    testEpisodicMemoryActionRetrieval passed" << std::endl;
}

void testEpisodicMemoryReplay() {
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    brain.initialize();
    
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    memory.enableReplay(true);
    
    // Create test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    episode.activeNeurons = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    episode.neuronActivations = {0.8f, 0.6f, 0.4f};
    
    memory.storeEpisode(episode);
    
    // Get episode for replay
    std::vector<const nlm::EpisodicMemoryItem*> forReplay = 
        memory.getEpisodesForReplay(1);
    
    assert(!forReplay.empty());
    assert(forReplay[0] == &episode);
    
    // Replay the episode
    memory.replayEpisode(forReplay[0]);
    
    // Update relevance
    memory.updateRelevance(0, 0.1f);
    
    std::cout << "    testEpisodicMemoryReplay passed" << std::endl;
}

void testEpisodicMemoryConsolidation() {
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    brain.initialize();
    
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    // Create multiple episodes
    for (int i = 0; i < 20; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        episode.sensoryState = {static_cast<float>(i) * 0.05f};
        episode.reward = static_cast<float>(i) * 0.1f;
        
        memory.storeEpisode(episode);
    }
    
    assert(memory.getEpisodeCount() == 20);
    
    // Consolidate episodes (remove old ones based on relevance)
    memory.consolidate(0.0f);  // Keep all episodes
    
    // Should still have all episodes
    assert(memory.getEpisodeCount() == 20);
    
    // Clear all episodes
    memory.clear();
    
    assert(memory.getEpisodeCount() == 0);
    
    std::cout << "    testEpisodicMemoryConsolidation passed" << std::endl;
}

void runAll() {
    testEpisodicMemoryCreation();
    testEpisodicMemoryStoreAndRetrieve();
    testEpisodicMemoryTemporalRetrieval();
    testEpisodicMemoryLocationRetrieval();
    testEpisodicMemoryActionRetrieval();
    testEpisodicMemoryReplay();
    testEpisodicMemoryConsolidation();
}

} // namespace test_neural_episodic_memory
