// NeuralEpisodicMemory Tests
#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_neural_episodic_memory {

void testMemoryCreation() {
    nlm::NeuralEpisodicMemory memory;
    
    assert(memory.getEpisodeCount() == 0);
    assert(memory.getAverageReward() == 0.0f);
    assert(memory.isReplayEnabled() == true);  // Default should be enabled
    
    std::cout << "    testMemoryCreation passed" << std::endl;
}

void testMemoryInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    assert(memory.getEpisodeCount() == 0);
    
    std::cout << "    testMemoryInitialization passed" << std::endl;
}

void testEpisodeStorage() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    // Create an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    episode.positionX = 1.0f;
    episode.positionY = 2.0f;
    episode.orientation = 0.0f;
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 10.0f;
    episode.energy = 5.0f;
    episode.novelty = 0.8f;
    episode.resultingSensoryState = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    episode.resultingReward = 12.0f;
    
    // Add some active neurons
    for (int i = 0; i < 5; ++i) {
        episode.activeNeurons.push_back(nlm::NeuronId(9000 + i));
        episode.neuronActivations.push_back(0.5f + (i * 0.1f));
    }
    
    episode.age = 0;
    
    // Store the episode
    memory.storeEpisode(episode);
    
    assert(memory.getEpisodeCount() == 1);
    
    std::cout << "    testEpisodeStorage passed" << std::endl;
}

void testEpisodeRetrievalSimilar() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    // Store multiple episodes with similar and different patterns
    std::vector<nlm::EpisodicMemoryItem> episodes;
    
    // Episode 1: similar pattern
    nlm::EpisodicMemoryItem episode1;
    episode1.timestamp = 100;
    episode1.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    episode1.action = nlm::ActionType::MoveForward;
    episode1.reward = 10.0f;
    episodes.push_back(episode1);
    
    // Episode 2: slightly different pattern
    nlm::EpisodicMemoryItem episode2;
    episode2.timestamp = 101;
    episode2.sensoryState = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};  // Similar but different
    episode2.action = nlm::ActionType::MoveLeft;
    episode2.reward = 8.0f;
    episodes.push_back(episode2);
    
    // Episode 3: very different pattern
    nlm::EpisodicMemoryItem episode3;
    episode3.timestamp = 102;
    episode3.sensoryState = {0.8f, 0.9f, 1.0f, 1.1f, 1.2f};  // Very different
    episode3.action = nlm::ActionType::Turn;
    episode3.reward = 5.0f;
    episodes.push_back(episode3);
    
    // Store all episodes
    for (auto& episode : episodes) {
        memory.storeEpisode(episode);
    }
    
    // Retrieve similar episodes for pattern close to episode1
    std::vector<float> queryPattern = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    auto similarResults = memory.retrieveSimilar(queryPattern, 3);
    
    // Should find at least episode1 (very similar) and episode2 (somewhat similar)
    assert(!similarResults.empty());
    
    std::cout << "    testEpisodeRetrievalSimilar passed" << std::endl;
}

void testEpisodeRetrievalTemporal() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    // Store episodes with different timestamps
    std::vector<nlm::EpisodicMemoryItem> episodes;
    
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;  // 0, 10, 20, ..., 90
        episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        episode.action = nlm::ActionType::Wait;
        episode.reward = 1.0f;
        episodes.push_back(episode);
    }
    
    for (auto& episode : episodes) {
        memory.storeEpisode(episode);
    }
    
    // Retrieve episodes from time window [20, 60]
    auto temporalResults = memory.retrieveTemporal(20, 60, 5);
    
    // Should find episodes with timestamps 20, 30, 40, 50, 60
    assert(!temporalResults.empty());
    assert(temporalResults.size() <= 5);
    
    std::cout << "    testEpisodeRetrievalTemporal passed" << std::endl;
}

void testEpisodeRetrievalByLocation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    // Store episodes at different locations
    std::vector<nlm::EpisodicMemoryItem> episodes;
    
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.positionX = 10.0f + i * 2.0f;
        episode.positionY = 10.0f + i;
        episode.orientation = 0.0f;
        episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        episode.action = nlm::ActionType::Wait;
        episode.reward = 1.0f;
        episodes.push_back(episode);
    }
    
    for (auto& episode : episodes) {
        memory.storeEpisode(episode);
    }
    
    // Retrieve episodes near location (10, 10) with radius 3
    auto locationResults = memory.retrieveByLocation(10.0f, 10.0f, 3.0f, 5);
    
    // Should find episodes at (10,10), (12,11), (14,12) - but (14,12) is outside radius 3
    assert(!locationResults.empty());
    
    std::cout << "    testEpisodeRetrievalByLocation passed" << std::endl;
}

void testEpisodeRetrievalAfterAction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    // Store episodes with different actions
    std::vector<nlm::EpisodicMemoryItem> episodes;
    
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        episode.action = (i % 3 == 0) ? nlm::ActionType::MoveForward : 
                        (i % 3 == 1) ? nlm::ActionType::MoveLeft : nlm::ActionType::Wait;
        episode.reward = 1.0f;
        episodes.push_back(episode);
    }
    
    for (auto& episode : episodes) {
        memory.storeEpisode(episode);
    }
    
    // Retrieve episodes after MoveForward action
    auto actionResults = memory.retrieveAfterAction(nlm::ActionType::MoveForward, 5);
    
    // Should find episodes with MoveForward action
    assert(!actionResults.empty());
    
    std::cout << "    testEpisodeRetrievalAfterAction passed" << std::endl;
}

void testEpisodeReplay() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    // Store an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 10.0f;
    
    memory.storeEpisode(episode);
    
    // Get the stored episode
    assert(memory.getEpisodeCount() == 1);
    const nlm::EpisodicMemoryItem* storedEpisode = memory.getEpisode(0);
    assert(storedEpisode != nullptr);
    
    // Replay the episode
    memory.replayEpisode(storedEpisode);
    
    // Replay should complete without errors
    
    std::cout << "    testEpisodeReplay passed" << std::endl;
}

void testEpisodeRelevance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    // Store some episodes
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        episode.action = nlm::ActionType::Wait;
        episode.reward = 1.0f;
        
        memory.storeEpisode(episode);
    }
    
    // Update relevance for first episode
    memory.updateRelevance(0, 2.0f);  // Increase relevance
    
    // Episodes for replay should include the relevant one first
    auto replayEpisodes = memory.getEpisodesForReplay(3);
    assert(!replayEpisodes.empty());
    
    std::cout << "    testEpisodeRelevance passed" << std::endl;
}

void testEpisodeConsolidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    // Store many episodes with varying rewards
    for (int i = 0; i < 20; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        episode.action = nlm::ActionType::Wait;
        episode.reward = (i % 5 == 0) ? 10.0f : 1.0f;  // Some high reward
        
        memory.storeEpisode(episode);
    }
    
    // Consolidate with relevance threshold of 5.0
    // Episodes with low reward (< 5.0) might be removed
    memory.consolidate(5.0f);
    
    // Average reward should be reasonable after consolidation
    float avgReward = memory.getAverageReward();
    assert(avgReward >= 1.0f && avgReward <= 10.0f);
    
    std::cout << "    testEpisodeConsolidation passed" << std::endl;
}

void testMemoryReplayControl() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    assert(memory.isReplayEnabled() == true);  // Default
    
    // Disable replay
    memory.enableReplay(false);
    assert(memory.isReplayEnabled() == false);
    
    // Enable replay again
    memory.enableReplay(true);
    assert(memory.isReplayEnabled() == true);
    
    std::cout << "    testMemoryReplayControl passed" << std::endl;
}

void testMemoryClear() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralEpisodicMemory memory;
    memory.initialize(&brain);
    
    // Store some episodes
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        episode.action = nlm::ActionType::Wait;
        episode.reward = 1.0f;
        
        memory.storeEpisode(episode);
    }
    
    assert(memory.getEpisodeCount() == 5);
    
    // Clear memory
    memory.clear();
    
    assert(memory.getEpisodeCount() == 0);
    
    std::cout << "    testMemoryClear passed" << std::endl;
}

void runAll() {
    testMemoryCreation();
    testMemoryInitialization();
    testEpisodeStorage();
    testEpisodeRetrievalSimilar();
    testEpisodeRetrievalTemporal();
    testEpisodeRetrievalByLocation();
    testEpisodeRetrievalAfterAction();
    testEpisodeReplay();
    testEpisodeRelevance();
    testEpisodeConsolidation();
    testMemoryReplayControl();
    testMemoryClear();
}

} // namespace test_neural_episodic_memory
