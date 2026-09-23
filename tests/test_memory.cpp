// Memory System Tests
// Testing working memory, episodic memory, and associative memory

#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_memory {

void testWorkingMemory() {
    std::cout << "    Testing working memory..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    auto* workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test initialization
    workingMemory->initialize(brain.get());
    workingMemory->setCapacity(50);
    
    // Store some neuron traces
    workingMemory->storeToNeuron(nlm::NeuronId(1), 0.8f);
    workingMemory->storeToNeuron(nlm::NeuronId(2), 0.6f);
    workingMemory->storeToNeuron(nlm::NeuronId(3), 0.9f);
    
    // Test update
    workingMemory->update(0.001);
    
    // Test active traces
    size_t activeTraces = workingMemory->getActiveTraces();
    assert(activeTraces > 0);
    
    // Test clear
    workingMemory->clear();
    
    std::cout << "      ✓ Working memory tests passed" << std::endl;
}

void testEpisodicMemory() {
    std::cout << "    Testing episodic memory..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test initialization
    episodicMemory->initialize(brain.get());
    episodicMemory->setMaxEpisodes(100);
    
    // Create and store an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1;
    episode.reward = 0.5f;
    episode.activeNeurons = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    episode.neuronActivations = {0.8f, 0.6f, 0.9f};
    
    episodicMemory->storeEpisode(episode);
    
    // Test retrieval
    auto episodes = episodicMemory->getEpisodes();
    assert(episodes.size() >= 1);
    
    // Test consolidation
    episodicMemory->consolidate(0.5f);
    
    // Test replay
    auto replayEpisodes = episodicMemory->getEpisodesForReplay(1);
    assert(replayEpisodes.size() > 0);
    
    // Test clear
    episodicMemory->clear();
    assert(episodicMemory->getEpisodeCount() == 0);
    
    std::cout << "      ✓ Episodic memory tests passed" << std::endl;
}

void testAssociativeMemory() {
    std::cout << "    Testing associative memory..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    auto* associativeMemory = brain->getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test initialization
    associativeMemory->initialize(brain.get());
    
    // Test storage and retrieval
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> pattern2 = {0.5f, 0.4f, 0.3f, 0.2f, 0.1f};
    
    associativeMemory->storePattern(pattern1);
    associativeMemory->storePattern(pattern2);
    
    // Test pattern association
    std::vector<float> query = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    std::vector<size_t> similarPatterns = associativeMemory->findSimilarPatterns(query, 0.8f);
    assert(!similarPatterns.empty());
    
    // Test clear
    associativeMemory->clear();
    
    std::cout << "      ✓ Associative memory tests passed" << std::endl;
}

void runAll() {
    std::cout << "=== Memory System Tests ===" << std::endl;
    
    testWorkingMemory();
    testEpisodicMemory();
    testAssociativeMemory();
    
    std::cout << "=== All Memory Tests PASSED ===" << std::endl;
}

} // namespace test_memory
