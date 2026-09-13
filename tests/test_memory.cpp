// Memory System Tests
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_memory {

void testNeuralWorkingMemoryCreation() {
    nlm::NeuralWorkingMemory wmem;
    
    assert(wmem.getCapacity() == 100);
    assert(wmem.getDecayRate() == 0.01f);
    assert(wmem.getActiveTraces() == 0);
    
    std::cout << "    testNeuralWorkingMemoryCreation passed" << std::endl;
}

void testNeuralWorkingMemoryInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::NeuralWorkingMemory wmem;
    
    wmem.initialize(&brain);
    
    assert(wmem.getBrain() != nullptr);
    
    std::cout << "    testNeuralWorkingMemoryInitialization passed" << std::endl;
}

void testNeuralWorkingMemoryStoreAndRetrieve() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::NeuralWorkingMemory wmem;
    
    wmem.initialize(&brain);
    
    // Store a pattern
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    wmem.store(pattern, 1.0f);
    
    // Retrieve pattern
    std::vector<float> retrieved = wmem.retrieve();
    assert(retrieved.size() >= 5);
    
    std::cout << "    testNeuralWorkingMemoryStoreAndRetrieve passed" << std::endl;
}

void testNeuralWorkingMemoryNeuronActivation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::NeuralWorkingMemory wmem;
    
    wmem.initialize(&brain);
    
    // Create a neuron ID (simplified for testing)
    nlm::NeuronId neuronId(12345);
    
    wmem.storeToNeuron(neuronId, 0.8f);
    
    assert(wmem.contains(neuronId));
    assert(wmem.getNeuronActivation(neuronId) == 0.8f);
    
    std::cout << "    testNeuralWorkingMemoryNeuronActivation passed" << std::endl;
}

void testNeuralWorkingMemoryCapacity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::NeuralWorkingMemory wmem;
    
    wmem.initialize(&brain);
    wmem.setCapacity(10);
    
    assert(wmem.getCapacity() == 10);
    
    std::cout << "    testNeuralWorkingMemoryCapacity passed" << std::endl;
}

void testNeuralWorkingMemoryDecay() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::NeuralWorkingMemory wmem;
    
    wmem.initialize(&brain);
    wmem.setDecayRate(0.1f);  // Fast decay for testing
    
    // Store pattern
    std::vector<float> pattern = {0.9f, 0.8f, 0.7f};
    wmem.store(pattern, 1.0f);
    
    // Update should decay weak traces
    wmem.update(0.001);
    
    // Memory should still exist but be partially decayed
    std::vector<float> retrieved = wmem.retrieve();
    assert(!retrieved.empty());
    
    std::cout << "    testNeuralWorkingMemoryDecay passed" << std::endl;
}

void testNeuralWorkingMemoryCompetition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::NeuralWorkingMemory wmem;
    
    wmem.initialize(&brain);
    
    // Store multiple patterns
    std::vector<float> pattern1 = {1.0f, 0.0f, 0.0f};
    std::vector<float> pattern2 = {0.0f, 1.0f, 0.0f};
    
    wmem.store(pattern1, 1.0f);
    wmem.store(pattern2, 1.0f);
    
    // Update should run competition
    wmem.update(0.001);
    
    // Some neurons should be winners
    size_t winners = 0;
    for (const auto& neuron : wmem.getMemoryNeurons()) {
        if (wmem.isWinning(neuron)) {
            winners++;
        }
    }
    
    std::cout << "    testNeuralWorkingMemoryCompetition passed" << std::endl;
}

void testEpisodicMemoryCreation() {
    nlm::NeuralEpisodicMemory mem;
    
    assert(mem.getEpisodeCount() == 0);
    assert(mem.isReplayEnabled());
    
    std::cout << "    testEpisodicMemoryCreation passed" << std::endl;
}

void testEpisodicMemoryInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::NeuralEpisodicMemory mem;
    
    mem.initialize(&brain);
    
    assert(mem.getBrain() != nullptr);
    
    std::cout << "    testEpisodicMemoryInitialization passed" << std::endl;
}

void testEpisodicMemoryStoreAndRetrieve() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::NeuralEpisodicMemory mem;
    
    mem.initialize(&brain);
    
    // Create an episodic memory item
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.sensoryState = {0.1f, 0.2f, 0.3f};
    episode.positionX = 50.0f;
    episode.positionY = 50.0f;
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 10.0f;
    
    // Store episode
    mem.storeEpisode(episode);
    
    assert(mem.getEpisodeCount() == 1);
    
    const nlm::EpisodicMemoryItem* retrieved = mem.getEpisode(0);
    assert(retrieved != nullptr);
    assert(retrieved->reward == 10.0f);
    
    std::cout << "    testEpisodicMemoryStoreAndRetrieve passed" << std::endl;
}

void testEpisodicMemoryRetrieval() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::NeuralEpisodicMemory mem;
    
    mem.initialize(&brain);
    
    // Store multiple episodes with different properties
    nlm::EpisodicMemoryItem episode1;
    episode1.timestamp = 100;
    episode1.sensoryState = {0.1f, 0.2f, 0.3f};
    episode1.positionX = 10.0f;
    episode1.positionY = 10.0f;
    episode1.action = nlm::ActionType::MoveForward;
    episode1.reward = 5.0f;
    
    nlm::EpisodicMemoryItem episode2;
    episode2.timestamp = 200;
    episode2.sensoryState = {0.4f, 0.5f, 0.6f};
    episode2.positionX = 90.0f;
    episode2.positionY = 90.0f;
    episode2.action = nlm::ActionType::Interact;
    episode2.reward = 15.0f;
    
    mem.storeEpisode(episode1);
    mem.storeEpisode(episode2);
    
    // Retrieve by temporal window
    auto temporal = mem.retrieveTemporal(150, 250);
    assert(temporal.size() == 1);
    assert(temporal[0]->timestamp == 200);
    
    // Retrieve by location
    auto location = mem.retrieveByLocation(8.0f, 8.0f, 5.0f);
    assert(location.size() == 1);
    assert(location[0]->positionX == 10.0f);
    
    // Retrieve by action
    auto action = mem.retrieveAfterAction(nlm::ActionType::Interact);
    assert(action.size() == 1);
    assert(action[0]->reward == 15.0f);
    
    std::cout << "    testEpisodicMemoryRetrieval passed" << std::endl;
}

void testEpisodicMemoryReplay() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::NeuralEpisodicMemory mem;
    
    mem.initialize(&brain);
    
    // Store an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.sensoryState = {0.1f, 0.2f, 0.3f};
    mem.storeEpisode(episode);
    
    // Replay episode should not crash
    mem.replayEpisode(&episode);
    
    // Disable replay
    mem.enableReplay(false);
    assert(!mem.isReplayEnabled());
    
    std::cout << "    testEpisodicMemoryReplay passed" << std::endl;
}

void testEpisodicMemoryConsolidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::NeuralEpisodicMemory mem;
    
    mem.initialize(&brain);
    
    // Store many episodes to trigger consolidation
    for (size_t i = 0; i < 100; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = {static_cast<float>(i) * 0.01f};
        mem.storeEpisode(episode);
    }
    
    // Consolidation should not crash
    mem.consolidate(0.5f);  // Remove episodes with relevance < 0.5
    
    std::cout << "    testEpisodicMemoryConsolidation passed" << std::endl;
}

void testAttentionalSelectionCreation() {
    nlm::AttentionalSelection attention;
    
    assert(attention.getInhibitionStrength() == 0.5f);
    assert(attention.getExcitationStrength() == 1.5f);
    assert(attention.getCompetitionThreshold() == 0.3f);
    assert(attention.getWinners().empty());
    
    std::cout << "    testAttentionalSelectionCreation passed" << std::endl;
}

void testAttentionalSelectionInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::AttentionalSelection attention;
    
    attention.initialize(&brain);
    
    assert(attention.getBrain() != nullptr);
    
    std::cout << "    testAttentionalSelectionInitialization passed" << std::endl;
}

void testAttentionalSelectionCompetition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::AttentionalSelection attention;
    
    attention.initialize(&brain);
    
    // Create some competitor neurons
    std::vector<nlm::NeuronId> competitors;
    competitors.push_back(nlm::NeuronId(1001));
    competitors.push_back(nlm::NeuronId(1002));
    competitors.push_back(nlm::NeuronId(1003));
    
    // Apply some biases to make one stronger
    attention.applyTopDownBias(competitors[0], 0.8f);
    attention.applyBottomUpSalience(competitors[1], 0.6f);
    
    // Process competition
    std::vector<nlm::NeuronId> winners = attention.processCompetition(competitors, 0.5f);
    
    // At least one winner should exist
    assert(!winners.empty());
    
    // Check winners
    for (const auto& winner : winners) {
        assert(attention.isAttended(winner));
    }
    
    std::cout << "    testAttentionalSelectionCompetition passed" << std::endl;
}

void testAttentionalSelectionUpdate() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::AttentionalSelection attention;
    
    attention.initialize(&brain);
    
    // Apply some biases
    nlm::NeuronId neuronId(2001);
    attention.applyTopDownBias(neuronId, 1.0f);
    attention.applyBottomUpSalience(neuronId, 0.8f);
    
    // Update should decay biases
    attention.update(0.001);
    
    // Update again - biases should be weaker
    attention.update(0.001);
    
    std::cout << "    testAttentionalSelectionUpdate passed" << std::endl;
}

void runAll() {
    std::cout << "Running Memory system tests..." << std::endl;
    
    testNeuralWorkingMemoryCreation();
    testNeuralWorkingMemoryInitialization();
    testNeuralWorkingMemoryStoreAndRetrieve();
    testNeuralWorkingMemoryNeuronActivation();
    testNeuralWorkingMemoryCapacity();
    testNeuralWorkingMemoryDecay();
    testNeuralWorkingMemoryCompetition();
    
    testEpisodicMemoryCreation();
    testEpisodicMemoryInitialization();
    testEpisodicMemoryStoreAndRetrieve();
    testEpisodicMemoryRetrieval();
    testEpisodicMemoryReplay();
    testEpisodicMemoryConsolidation();
    
    testAttentionalSelectionCreation();
    testAttentionalSelectionInitialization();
    testAttentionalSelectionCompetition();
    testAttentionalSelectionUpdate();
}

} // namespace test_memory
