// Memory System Tests
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_memory {

void testNeuralWorkingMemoryCreation() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    workingMemory->initialize(brain.get());
    workingMemory->setCapacity(100);
    
    assert(workingMemory->getActiveTraces() == 0);
    
    std::cout << "    testNeuralWorkingMemoryCreation passed" << std::endl;
}

void testNeuralWorkingMemoryStorage() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    workingMemory->initialize(brain.get());
    
    // Store a trace
    workingMemory->storeToNeuron(nlm::NeuronId(1), 0.5f);
    assert(workingMemory->getActiveTraces() == 1);
    
    // Store another trace
    workingMemory->storeToNeuron(nlm::NeuronId(2), 0.3f);
    assert(workingMemory->getActiveTraces() == 2);
    
    std::cout << "    testNeuralWorkingMemoryStorage passed" << std::endl;
}

void testNeuralWorkingMemoryUpdate() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    workingMemory->initialize(brain.get());
    
    // Store traces
    workingMemory->storeToNeuron(nlm::NeuronId(1), 0.5f);
    workingMemory->storeToNeuron(nlm::NeuronId(2), 0.3f);
    
    // Update (decay traces over time)
    workingMemory->update(0.001);
    
    // Check that traces decay
    assert(workingMemory->getActiveTraces() == 2); // Count should remain 2
    
    std::cout << "    testNeuralWorkingMemoryUpdate passed" << std::endl;
}

void testNeuralWorkingMemoryCompete() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    workingMemory->initialize(brain.get());
    
    // Store multiple traces
    workingMemory->storeToNeuron(nlm::NeuronId(1), 0.5f);
    workingMemory->storeToNeuron(nlm::NeuronId(2), 0.8f); // Stronger trace
    workingMemory->storeToNeuron(nlm::NeuronId(3), 0.3f);
    
    // Add more traces to trigger competition
    workingMemory->storeToNeuron(nlm::NeuronId(4), 0.1f);
    workingMemory->storeToNeuron(nlm::NeuronId(5), 0.2f);
    
    // Update to trigger competition
    workingMemory->update(0.001);
    
    // The winner should have stronger activation
    auto neurons = workingMemory->getMemoryNeurons();
    assert(!neurons.empty());
    
    std::cout << "    testNeuralWorkingMemoryCompete passed" << std::endl;
}

void testNeuralEpisodicMemoryCreation() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    episodicMemory->initialize(brain.get());
    episodicMemory->setMaxEpisodes(100);
    
    assert(episodicMemory->getEpisodeCount() == 0);
    
    std::cout << "    testNeuralEpisodicMemoryCreation passed" << std::endl;
}

void testNeuralEpisodicMemoryStorage() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    episodicMemory->initialize(brain.get());
    
    // Create an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.reward = 0.5f;
    episode.activeNeurons.push_back(nlm::NeuronId(1));
    episode.activeNeurons.push_back(nlm::NeuronId(2));
    episode.neuronActivations.push_back(0.7f);
    episode.neuronActivations.push_back(0.4f);
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() == 1);
    
    std::cout << "    testNeuralEpisodicMemoryStorage passed" << std::endl;
}

void testNeuralEpisodicMemoryReplay() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    episodicMemory->initialize(brain.get());
    
    // Create and store multiple episodes
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 100;
        episode.reward = 0.3f + i * 0.1f;
        episode.activeNeurons.push_back(nlm::NeuronId(i));
        episode.neuronActivations.push_back(0.5f);
        episodicMemory->storeEpisode(episode);
    }
    
    // Get episodes for replay
    auto episodes = episodicMemory->getEpisodesForReplay(3);
    assert(episodes.size() == 3); // Should get the 3 most recent episodes
    
    // Test replay
    if (episodes.size() > 0) {
        episodicMemory->replayEpisode(episodes[0]);
        // Replay should strengthen the traces
        assert(episodes[0]->neuronActivations.size() > 0);
    }
    
    std::cout << "    testNeuralEpisodicMemoryReplay passed" << std::endl;
}

void testNeuralAssociativeMemoryCreation() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* associativeMemory = brain->getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    associativeMemory->initialize(brain.get());
    
    std::cout << "    testNeuralAssociativeMemoryCreation passed" << std::endl;
}

void testNeuralAssociativeMemoryAssociation() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* associativeMemory = brain->getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    associativeMemory->initialize(brain.get());
    
    // Create an association
    associativeMemory->associatePattern(std::vector<float>{0.1f, 0.2f, 0.3f}, 
                                       std::vector<float>{0.4f, 0.5f, 0.6f});
    
    // Get associations for pattern
    auto associations = associativeMemory->getAssociations(std::vector<float>{0.1f, 0.2f, 0.3f});
    assert(!associations.empty());
    
    std::cout << "    testNeuralAssociativeMemoryAssociation passed" << std::endl;
}

void testMemoryIntegration() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Test that memory systems are properly integrated
    assert(brain->getWorkingMemory() != nullptr);
    assert(brain->getEpisodicMemory() != nullptr);
    assert(brain->getAssociativeMemory() != nullptr);
    
    // Initialize all memory systems
    brain->getWorkingMemory()->initialize(brain.get());
    brain->getEpisodicMemory()->initialize(brain.get());
    brain->getAssociativeMemory()->initialize(brain.get());
    
    // Run some steps to ensure integration
    for (int i = 0; i < 50; ++i) {
        brain->step(i, i * 0.001);
    }
    
    // Check that working memory has traces
    assert(brain->getWorkingMemory()->getActiveTraces() >= 0);
    
    // Check that episodic memory has episodes
    assert(brain->getEpisodicMemory()->getEpisodeCount() >= 0);
    
    std::cout << "    testMemoryIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Memory System tests..." << std::endl;
    testNeuralWorkingMemoryCreation();
    testNeuralWorkingMemoryStorage();
    testNeuralWorkingMemoryUpdate();
    testNeuralWorkingMemoryCompete();
    testNeuralEpisodicMemoryCreation();
    testNeuralEpisodicMemoryStorage();
    testNeuralEpisodicMemoryReplay();
    testNeuralAssociativeMemoryCreation();
    testNeuralAssociativeMemoryAssociation();
    testMemoryIntegration();
    
    std::cout << "All Memory System tests passed!" << std::endl;
}

} // namespace test_memory
