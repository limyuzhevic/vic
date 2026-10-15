// Memory System Tests
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_memory_systems {

void testWorkingMemory() {
    std::cout << "  Testing NeuralWorkingMemory...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test storing neuron activity
    workingMemory->storeToNeuron(1, 0.8f);
    workingMemory->storeToNeuron(2, 0.6f);
    workingMemory->storeToNeuron(3, 0.9f);
    
    // Test capacity
    workingMemory->setCapacity(10);
    assert(workingMemory->getCapacity() == 10);
    
    // Test activation levels
    workingMemory->storeToNeuron(4, 0.3f);  // Should be rejected due to low activation
    
    // Test update mechanism
    workingMemory->update(0.001);
    
    std::cout << " passed" << std::endl;
}

void testEpisodicMemory() {
    std::cout << "  Testing NeuralEpisodicMemory...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test storing an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.reward = 0.5f;
    episode.activeNeurons = {1, 2, 3, 4, 5};
    episode.neuronActivations = {0.8f, 0.7f, 0.9f, 0.6f, 0.5f};
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() > 0);
    
    // Test retrieval
    auto* retrievedEpisode = episodicMemory->getLatestEpisode();
    assert(retrievedEpisode != nullptr);
    assert(retrievedEpisode->reward == 0.5f);
    assert(retrievedEpisode->activeNeurons.size() > 0);
    
    // Test replay
    std::cout << " passed" << std::endl;
}

void testAssociativeMemory() {
    std::cout << "  Testing NeuralAssociativeMemory...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test storing an association
    associativeMemory->storeAssociation(1, 2, 0.8f);
    associativeMemory->storeAssociation(3, 4, 0.6f);
    
    // Test retrieval
    float strength = associativeMemory->getAssociationStrength(1, 2);
    assert(strength > 0.7f);
    
    // Test bidirectional retrieval
    strength = associativeMemory->getAssociationStrength(2, 1);
    assert(strength > 0.7f);
    
    // Test association spreading
    associativeMemory->propagateAssociation(1, 5, 0.3f);
    
    std::cout << " passed" << std::endl;
}

void testNeuromodulation() {
    std::cout << "  Testing Neuromodulation...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    
    // Test dopamine update and reward
    dopamine->update(0.001);
    float dopamineLevel = dopamine->getLevel();
    assert(dopamineLevel >= 0.0f);
    
    // Test curiosity update
    curiosity->update(0.001);
    float curiosityLevel = curiosity->getCuriosityLevel();
    assert(curiosityLevel >= 0.0f);
    
    // Test reward signaling
    dopamine->signalReward(0.5f);
    dopamine->signalRewardPredictionError(0.2f);
    
    // Test plasticit modulation
    float plasticityFactor = dopamine->getPlasticityFactor();
    assert(plasticityFactor >= 0.0f);
    
    std::cout << " passed" << std::endl;
}

void testMemoryIntegration() {
    std::cout << "  Testing Memory Integration...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("max_simulation_steps", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory storage from brain step
    brain.step(0);
    
    // Test episodic memory storage during brain step
    brain.step(10);
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test associative memory integration
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    std::cout << " passed" << std::endl;
}

void runAll() {
    std::cout << "Running Memory System Tests..." << std::endl;
    
    testWorkingMemory();
    testEpisodicMemory();
    testAssociativeMemory();
    testNeuromodulation();
    testMemoryIntegration();
    
    std::cout << "All memory system tests passed!" << std::endl;
}

} // namespace test_memory_systems