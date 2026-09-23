// EpisodicMemory Tests
#include "brain/NeuralEpisodicMemory.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>

namespace test_episodicmemory {

void testEpisodicMemoryCreation() {
    nlm::NeuralEpisodicMemory episodicMem;
    
    // Should be able to access via Brain interface
    std::cout << "    testEpisodicMemoryCreation passed" << std::endl;
}

void testEpisodicMemoryStorage() {
    nlm::NeuralEpisodicMemory episodicMem;
    
    // Create a test episodic memory item
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = {0.1f, 0.2f, 0.3f};
    episode.positionX = 0.0f;
    episode.positionY = 0.0f;
    episode.orientation = 0.0f;
    episode.action = nlm::ActionType::Wait;
    episode.reward = 5.0f;
    episode.energy = 1.0f;
    episode.novelty = 0.3f;
    episode.resultingSensoryState = {0.2f, 0.3f, 0.4f};
    episode.resultingReward = 3.0f;
    episode.age = 0;
    
    // Store episode
    episodicMem.storeEpisode(episode);
    
    // Verify retrieval
    std::vector<const nlm::EpisodicMemoryItem*> retrieved = 
        episodicMem.retrieveSimilar(episode.sensoryState, 5);
    assert(!retrieved.empty());
    
    std::cout << "    testEpisodicMemoryStorage passed" << std::endl;
}

void testEpisodicMemoryRetrieval() {
    nlm::NeuralEpisodicMemory episodicMem;
    
    // Store an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = {0.5f, 0.6f, 0.7f};
    episode.positionX = 10.0f;
    episode.positionY = 20.0f;
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 8.0f;
    
    episodicMem.storeEpisode(episode);
    
    // Retrieve by similar pattern
    std::vector<const nlm::EpisodicMemoryItem*> similar = 
        episodicMem.retrieveSimilar(episode.sensoryState, 5);
    assert(!similar.empty());
    
    // Retrieve by temporal window
    std::vector<const nlm::EpisodicMemoryItem*> temporal = 
        episodicMem.retrieveTemporal(0, 10, 5);
    assert(!temporal.empty());
    
    // Retrieve by location
    std::vector<const nlm::EpisodicMemoryItem*> location = 
        episodicMem.retrieveByLocation(10.0f, 20.0f, 5.0f, 5);
    assert(!location.empty());
    
    // Retrieve by action
    std::vector<const nlm::EpisodicMemoryItem*> action = 
        episodicMem.retrieveAfterAction(nlm::ActionType::MoveForward, 5);
    assert(!action.empty());
    
    std::cout << "    testEpisodicMemoryRetrieval passed" << std::endl;
}

void testEpisodicMemoryReplay() {
    nlm::NeuralEpisodicMemory episodicMem;
    
    // Store episodes
    std::vector<nlm::EpisodicMemoryItem> episodes;
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        episode.sensoryState = {static_cast<float>(i), static_cast<float>(i), static_cast<float>(i)};
        episode.positionX = i * 2.0f;
        episode.positionY = i * 3.0f;
        episode.action = nlm::ActionType::Wait;
        episode.reward = i * 2.0f;
        episode.age = i;
        
        episodicMem.storeEpisode(episode);
        episodes.push_back(episode);
    }
    
    // Replay first episode
    if (!episodes.empty()) {
        episodicMem.replayEpisode(&episodes[0]);
    }
    
    std::cout << "    testEpisodicMemoryReplay passed" << std::endl;
}

void testEpisodicMemoryRelevance() {
    nlm::NeuralEpisodicMemory episodicMem;
    
    // Store an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = {0.1f, 0.2f, 0.3f};
    
    episodicMem.storeEpisode(episode);
    
    // Update relevance
    episodicMem.updateRelevance(0, 0.5f);
    
    std::cout << "    testEpisodicMemoryRelevance passed" << std::endl;
}

void runAll() {
    std::cout << "Running EpisodicMemory tests..." << std::endl;
    testEpisodicMemoryStorage();
    testEpisodicMemoryRetrieval();
    testEpisodicMemoryReplay();
    testEpisodicMemoryRelevance();
}

} // namespace test_episodicmemory

void runAll() {
    std::cout << "Running EpisodicMemory tests..." << std::endl;
    testEpisodicMemoryCreation();
    testEpisodicMemoryCapacity();
    testEpisodicMemoryStorage();
    testEpisodicMemoryConsolidation();
    testEpisodicMemoryPatternLearning();
    testEpisodicMemoryReplay();
    testEpisodicMemoryPruning();
}

} // namespace test_episodicmemory