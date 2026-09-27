// Memory Integration Tests for Phase 6 NLM Brain
// Tests working memory, episodic memory, and associative memory integration

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_memory_integration {

void testWorkingMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get working memory
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test working memory capacity
    workingMemory->setCapacity(50);
    assert(workingMemory->getCapacity() == 50);
    
    // Simulate storing neurons in working memory
    workingMemory->storeToNeuron(1, 0.5f);
    workingMemory->storeToNeuron(2, 0.8f);
    
    // Test that neurons are stored
    auto memoryNeurons = workingMemory->getMemoryNeurons();
    assert(!memoryNeurons.empty());
    assert(memoryNeurons.size() <= 50); // Should not exceed capacity
    
    // Test working memory update
    workingMemory->update(0.01f);
    
    std::cout << "    testWorkingMemoryIntegration passed" << std::endl;
}

void testEpisodicMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get episodic memory
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Set max episodes
    episodicMemory->setMaxEpisodes(100);
    assert(episodicMemory->getMaxEpisodes() == 100);
    
    // Create a test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.reward = 1.0f;
    
    // Add some active neurons (simulating current brain state)
    for (size_t i = 1; i <= 10; ++i) {
        episode.activeNeurons.push_back(i);
        episode.neuronActivations.push_back(0.5f + (i % 5) * 0.1f);
    }
    
    // Store the episode
    episodicMemory->storeEpisode(episode);
    
    // Verify episode was stored
    assert(episodicMemory->getEpisodeCount() > 0);
    
    // Test replay
    auto* loadedEpisode = episodicMemory->getEpisodesForReplay(1)[0];
    assert(loadedEpisode != nullptr);
    assert(loadedEpisode->timestamp == episode.timestamp);
    assert(loadedEpisode->reward == episode.reward);
    
    std::cout << "    testEpisodicMemoryIntegration passed" << std::endl;
}

void testAssociativeMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get associative memory
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test associative memory
    // Store an association between two neurons
    associativeMemory->storeAssociation(1, 2, 0.8f);
    
    // Retrieve the association
    float association = associativeMemory->getAssociation(1, 2);
    assert(association > 0.0f);
    
    // Test pattern completion
    std::vector<float> pattern = {0.5f, 0.3f, 0.7f, 0.9f, 0.2f};
    std::vector<float> completion = associativeMemory->completePattern(pattern);
    
    // Completion should have same size as pattern
    assert(completion.size() == pattern.size());
    
    std::cout << "    testAssociativeMemoryIntegration passed" << std::endl;
}

void testMemoryConsolidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("consolidation_interval", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get episodic memory
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Create multiple episodes with different activation levels
    for (size_t i = 0; i < 20; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.reward = 0.1f + (i % 3) * 0.5f; // Varying reward
        
        // Vary activation levels
        for (size_t j = 0; j < 5; ++j) {
            episode.activeNeurons.push_back(j + 1);
            episode.neuronActivations.push_back(0.3f + (i % 4) * 0.2f);
        }
        
        episodicMemory->storeEpisode(episode);
    }
    
    // Run multiple steps to trigger consolidation
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    // Memory should still have episodes
    assert(episodicMemory->getEpisodeCount() > 0);
    
    std::cout << "    testMemoryConsolidation passed" << std::endl;
}

} // namespace test_memory_integration
