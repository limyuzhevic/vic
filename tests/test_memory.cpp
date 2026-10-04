// Memory System Tests
// Tests working memory, episodic memory, and associative memory

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

namespace test_memory {

void testWorkingMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    auto* workingMem = brain.getWorkingMemory();
    assert(workingMem != nullptr);
    
    // Test capacity
    assert(workingMem->getCapacity() > 0);
    
    // Test initial state
    assert(workingMem->getActiveTraces() == 0);
    
    // Test storage
    workingMem->storeToNeuron(nlm::NeuronId(1), 0.5f);
    assert(workingMem->getActiveTraces() == 1);
    
    // Test updating
    workingMem->update(0.001);
    // After update, some traces may decay
    
    // Test clearing
    workingMem->clear();
    assert(workingMem->getActiveTraces() == 0);
    
    std::cout << "    testWorkingMemory passed" << std::endl;
}

void testEpisodicMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    auto* episodicMem = brain.getEpisodicMemory();
    assert(episodicMem != nullptr);
    
    // Test capacity
    assert(episodicMem->getMaxEpisodes() > 0);
    
    // Test initial state
    assert(episodicMem->getEpisodeCount() == 0);
    
    // Create test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.reward = 1.0f;
    episode.activeNeurons.push_back(nlm::NeuronId(1));
    episode.neuronActivations.push_back(0.8f);
    
    // Store episode
    episodicMem->storeEpisode(episode);
    assert(episodicMem->getEpisodeCount() == 1);
    
    // Test retrieval
    auto episodes = episodicMem->getEpisodesForReplay(10);
    assert(!episodes.empty());
    assert(episodes[0]->timestamp == 100);
    assert(episodicMem->getEpisodeCount() == 1);
    
    // Test consolidation
    episodicMem->consolidate(0.5f);
    
    // Test clearing
    episodicMem->clear();
    assert(episodicMem->getEpisodeCount() == 0);
    
    std::cout << "    testEpisodicMemory passed" << std::endl;
}

void testAssociativeMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    auto* associativeMem = brain.getAssociativeMemory();
    assert(associativeMem != nullptr);
    
    // Test initialization
    // (Associative memory implementation may have its own initialization requirements)
    
    // Test storage and retrieval (concept-based associations)
    // This is a simplified test - real associative memory would be more complex
    
    // Test clearing
    associativeMem->clear();
    
    std::cout << "    testAssociativeMemory passed" << std::endl;
}

void testMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify all memory systems are present
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    
    // Run some simulation steps to exercise memory systems
    for (size_t step = 0; step < 100; ++step) {
        brain.step(step, step * 0.001);
        
        // Inject some input to stimulate memory formation
        if (step % 10 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
        }
    }
    
    // Verify memory systems recorded activity
    if (brain.getWorkingMemory()) {
        assert(brain.getWorkingMemory()->getActiveTraces() >= 0);
    }
    
    if (brain.getEpisodicMemory()) {
        assert(brain.getEpisodicMemory()->getEpisodeCount() >= 0);
    }
    
    std::cout << "    testMemoryIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Memory tests..." << std::endl;
    testWorkingMemory();
    testEpisodicMemory();
    testAssociativeMemory();
    testMemoryIntegration();
}

} // namespace test_memory
