// Memory System Tests
// Tests for NeuralWorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory

#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_memory {

void testNeuralWorkingMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test storing and retrieving neuron activities
    workingMemory->storeToNeuron(1, 0.5f);
    workingMemory->update(0.001f);
    
    auto memoryNeurons = workingMemory->getMemoryNeurons();
    auto memoryActivities = workingMemory->getMemoryActivities();
    
    assert(memoryNeurons.size() > 0);
    assert(memoryActivities.size() > 0);
    
    // Test capacity setting
    workingMemory->setCapacity(50);
    
    std::cout << "    testNeuralWorkingMemory passed" << std::endl;
}

void testNeuralEpisodicMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test storing an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.reward = 1.0f;
    episode.activeNeurons = {1, 2, 3};
    episode.neuronActivations = {0.5f, 0.3f, 0.7f};
    
    episodicMemory->storeEpisode(episode);
    
    // Test getting episodes for replay
    auto episodes = episodicMemory->getEpisodesForReplay(3);
    assert(episodes.size() > 0);
    
    // Test episode count
    size_t episodeCount = episodicMemory->getEpisodeCount();
    assert(episodeCount > 0);
    
    // Test setting max episodes
    episodicMemory->setMaxEpisodes(1000);
    
    std::cout << "    testNeuralEpisodicMemory passed" << std::endl;
}

void testNeuralAssociativeMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test storing associations
    associativeMemory->storeAssociation(1, 2, 0.8f);
    associativeMemory->storeAssociation(3, 4, 0.6f);
    
    // Test retrieving associations
    float association1 = associativeMemory->getAssociation(1, 2);
    assert(association1 > 0.0f);
    
    float association2 = associativeMemory->getAssociation(3, 4);
    assert(association2 > 0.0f);
    
    // Test finding related neurons
    auto related = associativeMemory->findRelated(1, 0.5f);
    assert(related.size() > 0);
    
    std::cout << "    testNeuralAssociativeMemory passed" << std::endl;
}

void runAll() {
    testNeuralWorkingMemory();
    testNeuralEpisodicMemory();
    testNeuralAssociativeMemory();
}

} // namespace test_memory