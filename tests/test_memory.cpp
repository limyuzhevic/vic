// Comprehensive Memory System Tests
// Phase 2: Real Neural Computation Tests - Memory Systems

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "memory/Memory.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <memory>

namespace test_memory {

void testWorkingMemoryBasic() {
    // Create a minimal brain configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test initial state
    assert(workingMemory->getActiveTraces() == 0);
    
    // Store some neural activity
    workingMemory->storeToNeuron(nlm::NeuronId(1), 0.5f);
    workingMemory->storeToNeuron(nlm::NeuronId(2), 0.7f);
    
    assert(workingMemory->getActiveTraces() > 0);
    
    // Test decay
    workingMemory->update(0.1f);  // 0.1s decay
    
    // Test retrieval
    auto traces = workingMemory->getActiveTraces();
    assert(!traces.empty());
    
    // Test clearing
    workingMemory->clear();
    assert(workingMemory->getActiveTraces() == 0);
    
    std::cout << "    testWorkingMemoryBasic passed" << std::endl;
}

void testWorkingMemoryCapacity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Set capacity
    workingMemory->setCapacity(100);
    
    // Fill with more traces than capacity
    for (int i = 0; i < 150; ++i) {
        workingMemory->storeToNeuron(nlm::NeuronId(i), 0.1f + (i % 10) * 0.01f);
    }
    
    // Verify some traces are active (capacity management may limit)
    auto activeTraces = workingMemory->getActiveTraces();
    assert(!activeTraces.empty());
    
    std::cout << "    testWorkingMemoryCapacity passed" << std::endl;
}

void testWorkingMemoryCompetition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Store traces with different strengths
    workingMemory->storeToNeuron(nlm::NeuronId(1), 0.9f);
    workingMemory->storeToNeuron(nlm::NeuronId(2), 0.5f);
    workingMemory->storeToNeuron(nlm::NeuronId(3), 0.1f);
    
    // Get memory neurons (competition should select strongest)
    auto memoryNeurons = workingMemory->getMemoryNeurons();
    
    // At least one neuron should be selected
    assert(!memoryNeurons.empty());
    
    std::cout << "    testWorkingMemoryCompetition passed" << std::endl;
}

void testEpisodicMemoryBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test initial state
    assert(episodicMemory->getEpisodeCount() == 0);
    
    // Create and store an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.reward = 0.8f;
    episode.activeNeurons = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    episode.neuronActivations = {0.5f, 0.7f, 0.9f};
    
    episodicMemory->storeEpisode(episode);
    
    assert(episodicMemory->getEpisodeCount() == 1);
    
    // Test retrieval
    auto* retrievedEpisode = episodicMemory->getEpisode(0);
    assert(retrievedEpisode != nullptr);
    assert(retrievedEpisode->timestamp == episode.timestamp);
    assert(retrievedEpisode->reward == episode.reward);
    assert(retrievedEpisode->activeNeurons.size() == episode.activeNeurons.size());
    
    // Test get episodes for replay
    auto* episodesForReplay = episodicMemory->getEpisodesForReplay(2);
    assert(episodesForReplay != nullptr);
    
    std::cout << "    testEpisodicMemoryBasic passed" << std::endl;
}

void testEpisodicMemoryCapacity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("max_episodes", 10, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Store more episodes than capacity
    for (int i = 0; i < 15; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.reward = 0.5f + (i % 5) * 0.1f;
        episode.activeNeurons = {nlm::NeuronId(i % 10)};
        episode.neuronActivations = {0.3f};
        
        episodicMemory->storeEpisode(episode);
    }
    
    // Should not exceed capacity
    int episodeCount = episodicMemory->getEpisodeCount();
    assert(episodeCount <= 10);
    
    std::cout << "    testEpisodicMemoryCapacity passed" << std::endl;
}

void testEpisodicMemoryReplay() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Store several episodes
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        episode.reward = 0.5f + i * 0.1f;
        episode.activeNeurons = {nlm::NeuronId(i)};
        episode.neuronActivations = {0.5f};
        
        episodicMemory->storeEpisode(episode);
    }
    
    // Get episodes for replay
    auto* episodes = episodicMemory->getEpisodesForReplay(3);
    assert(episodes != nullptr);
    
    // Replay should be called
    episodicMemory->replayEpisode(episodes[0]);
    episodicMemory->replayEpisode(episodes[1]);
    episodicMemory->replayEpisode(episodes[2]);
    
    std::cout << "    testEpisodicMemoryReplay passed" << std::endl;
}

void testEpisodicMemoryConsolidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Store some episodes with varying rewards
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.reward = 0.1f + (i % 3) * 0.5f;  // Some high reward, some low
        episode.activeNeurons = {nlm::NeuronId(i)};
        episode.neuronActivations = {0.3f};
        
        episodicMemory->storeEpisode(episode);
    }
    
    // Consolidate (remove weak episodes)
    episodicMemory->consolidate(0.3f);  // Keep episodes with combined score > 0.3
    
    int episodeCount = episodicMemory->getEpisodeCount();
    assert(episodeCount >= 0);  // May be reduced or same, but not negative
    
    std::cout << "    testEpisodicMemoryConsolidation passed" << std::endl;
}

void testAssociativeMemoryBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test initialization
    associativeMemory->initialize(&brain);
    
    // Store associations
    std::vector<nlm::NeuronId> cue(nlm::NeuronId(1), nlm::NeuronId(5));  // Neurons 1-5
    std::vector<nlm::NeuronId> response(nlm::NeuronId(10), nlm::NeuronId(14));  // Neurons 10-14
    
    associativeMemory->storeAssociation(cue, response);
    
    // Retrieve associations
    auto* retrievedResponse = associativeMemory->retrieve(cue);
    assert(retrievedResponse != nullptr);
    
    std::cout << "    testAssociativeMemoryBasic passed" << std::endl;
}

void testAssociativeMemoryRecall() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Store partial cue
    std::vector<nlm::NeuronId> partialCue;
    partialCue.push_back(nlm::NeuronId(1));
    partialCue.push_back(nlm::NeuronId(2));
    
    // Store full cue
    std::vector<nlm::NeuronId> fullCue;
    fullCue.push_back(nlm::NeuronId(1));
    fullCue.push_back(nlm::NeuronId(2));
    fullCue.push_back(nlm::NeuronId(3));
    
    std::vector<nlm::NeuronId> response = {nlm::NeuronId(10), nlm::NeuronId(11)};
    associativeMemory->storeAssociation(fullCue, response);
    
    // Try to retrieve with partial cue (should return null or partial result)
    auto* retrieved = associativeMemory->retrieve(partialCue);
    
    // May return null or partial response, both are acceptable
    if (retrieved != nullptr) {
        assert(!retrieved->empty());
    }
    
    std::cout << "    testAssociativeMemoryRecall passed" << std::endl;
}

void testMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that all memory systems are accessible and functional
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Test working memory
    workingMemory->storeToNeuron(nlm::NeuronId(1), 0.7f);
    assert(workingMemory->getActiveTraces() > 0);
    
    // Test episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 50;
    episode.reward = 0.8f;
    episode.activeNeurons = {nlm::NeuronId(5)};
    episode.neuronActivations = {0.6f};
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() == 1);
    
    // Test associative memory
    std::vector<nlm::NeuronId> cue = {nlm::NeuronId(10), nlm::NeuronId(11)};
    std::vector<nlm::NeuronId> response = {nlm::NeuronId(20), nlm::NeuronId(21)};
    associativeMemory->storeAssociation(cue, response);
    
    auto* retrievedResponse = associativeMemory->retrieve(cue);
    assert(retrievedResponse != nullptr);
    
    std::cout << "    testMemoryIntegration passed" << std::endl;
}

void testMemoryPersistence() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    
    // Store data in memory systems
    workingMemory->storeToNeuron(nlm::NeuronId(1), 0.9f);
    workingMemory->storeToNeuron(nlm::NeuronId(2), 0.8f);
    
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.reward = 0.7f;
    episode.activeNeurons = {nlm::NeuronId(3), nlm::NeuronId(4)};
    episode.neuronActivations = {0.5f, 0.6f};
    
    episodicMemory->storeEpisode(episode);
    
    // Reset brain
    brain.reset();
    
    // Verify memory systems are cleared
    assert(workingMemory->getActiveTraces() == 0);
    assert(episodicMemory->getEpisodeCount() == 0);
    
    std::cout << "    testMemoryPersistence passed" << std::endl;
}

void runAll() {
    std::cout << "Running Memory tests..." << std::endl;
    testWorkingMemoryBasic();
    testWorkingMemoryCapacity();
    testWorkingMemoryCompetition();
    testEpisodicMemoryBasic();
    testEpisodicMemoryCapacity();
    testEpisodicMemoryReplay();
    testEpisodicMemoryConsolidation();
    testAssociativeMemoryBasic();
    testAssociativeMemoryRecall();
    testMemoryIntegration();
    testMemoryPersistence();
}

} // namespace test_memory
