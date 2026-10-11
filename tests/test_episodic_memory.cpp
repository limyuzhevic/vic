// EpisodicMemory Tests
#include "memory/Memory.hpp"
#include <cassert>
#include <iostream>

namespace test_episodic_memory {

void testEpisodicMemoryCreation() {
    nlm::EpisodicMemory memory;
    
    assert(memory.getEpisodeCount() == 0);
    assert(memory.getRecentEpisodes(10).empty());
    
    std::cout << "    testEpisodicMemoryCreation passed" << std::endl;
}

void testEpisodicMemoryStorage() {
    nlm::EpisodicMemory memory;
    
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.positionX = 1.0f;
    episode.positionY = 2.0f;
    episode.orientation = 0.5f;
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 10.0f;
    episode.energy = 50.0f;
    episode.novelty = 0.8f;
    episode.resultingReward = 15.0f;
    
    memory.storeEpisode(episode);
    
    assert(memory.getEpisodeCount() == 1);
    
    auto retrieved = memory.retrieveEpisode(0);
    assert(retrieved.timestamp == 100);
    assert(retrieved.positionX == 1.0f);
    assert(retrieved.action == nlm::ActionType::MoveForward);
    assert(retrieved.reward == 10.0f);
    
    std::cout << "    testEpisodicMemoryStorage passed" << std::endl;
}

void testEpisodicMemoryRetrieval() {
    nlm::EpisodicMemory memory;
    
    // Store multiple episodes
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        episode.positionX = i * 0.5f;
        episode.positionY = i * 0.5f;
        episode.action = nlm::ActionType::Wait;
        episode.reward = static_cast<float>(i * 2);
        
        memory.storeEpisode(episode);
    }
    
    // Test temporal retrieval
    auto recent = memory.getRecentEpisodes(3);
    assert(recent.size() == 3);
    assert(recent[0].timestamp == 40);
    assert(recent[1].timestamp == 30);
    assert(recent[2].timestamp == 20);
    
    // Test retrieval by location
    auto location = memory.retrieveByLocation(1.0f, 1.0f, 0.5f);
    assert(location.size() == 1);
    assert(location[0]->positionX == 1.0f);
    assert(location[0]->positionY == 1.0f);
    
    // Test retrieval by action
    auto actions = memory.retrieveAfterAction(nlm::ActionType::Wait, 5);
    assert(actions.size() == 5);
    
    std::cout << "    testEpisodicMemoryRetrieval passed" << std::endl;
}

void testEpisodicMemoryConsolidation() {
    nlm::EpisodicMemory memory;
    
    // Store many episodes
    for (int i = 0; i < 150; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.positionX = static_cast<float>(i);
        episode.positionY = 0.0f;
        episode.action = nlm::ActionType::Wait;
        episode.reward = 1.0f;
        
        memory.storeEpisode(episode);
    }
    
    assert(memory.getEpisodeCount() == 150);
    
    // Consolidate with threshold
    memory.consolidate(0.5f);
    
    // Should reduce to at most 80% of max (150 * 0.8 = 120)
    assert(memory.getEpisodeCount() <= 120);
    
    std::cout << "    testEpisodicMemoryConsolidation passed" << std::endl;
}

void testEpisodicMemoryReplay() {
    nlm::EpisodicMemory memory;
    
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 5.0f;
    
    memory.storeEpisode(episode);
    
    assert(memory.getEpisodeCount() == 1);
    
    // Enable replay
    memory.enableReplay(true);
    assert(memory.isReplayEnabled());
    
    // Test episode selection for replay
    auto episodes = memory.getEpisodesForReplay(1);
    assert(episodes.size() == 1);
    
    std::cout << "    testEpisodicMemoryReplay passed" << std::endl;
}

void testEpisodicMemoryClear() {
    nlm::EpisodicMemory memory;
    
    // Store some episodes
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        memory.storeEpisode(episode);
    }
    
    assert(memory.getEpisodeCount() == 10);
    
    // Clear all
    memory.clear();
    
    assert(memory.getEpisodeCount() == 0);
    
    std::cout << "    testEpisodicMemoryClear passed" << std::endl;
}

void runAll() {
    std::cout << "Running EpisodicMemory tests..." << std::endl;
    testEpisodicMemoryCreation();
    testEpisodicMemoryStorage();
    testEpisodicMemoryRetrieval();
    testEpisodicMemoryConsolidation();
    testEpisodicMemoryReplay();
    testEpisodicMemoryClear();
    
    std::cout << "All EpisodicMemory tests passed!" << std::endl;
}

} // namespace test_episodic_memory
