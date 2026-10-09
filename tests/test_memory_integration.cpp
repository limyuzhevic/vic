// Test for Memory Integration (WorkingMemory, EpisodicMemory, AssociativeMemory)
// Covers requirement: 1. Memory Integration Tests

#include "memory/Memory.hpp"
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_memory_integration {

void testWorkingMemoryBasic() {
    nlm::WorkingMemory wm;
    
    // Test initial state
    assert(wm.getCurrentSize() == 0);
    assert(wm.getCapacity() > 0);
    assert(!wm.contains(nlm::NeuronId(1)));
    
    // Store and retrieve item
    wm.store(nlm::NeuronId(1), 5.0f);
    assert(wm.contains(nlm::NeuronId(1)));
    assert(wm.retrieve(nlm::NeuronId(1)) == 5.0f);
    
    // Update item
    wm.store(nlm::NeuronId(1), 10.0f);
    assert(wm.retrieve(nlm::NeuronId(1)) == 10.0f);
    
    // Store multiple items
    wm.store(nlm::NeuronId(2), 3.0f);
    wm.store(nlm::NeuronId(3), 7.0f);
    assert(wm.getCurrentSize() == 3);
    
    // Decay
    wm.decay(0.1f);
    
    std::cout << "    testWorkingMemoryBasic passed" << std::endl;
}

void testEpisodicMemoryBasic() {
    nlm::EpisodicMemory em;
    
    // Test initial state
    assert(em.getEpisodeCount() == 0);
    
    // Create test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.neurons.push_back(nlm::NeuronId(1));
    episode.neurons.push_back(nlm::NeuronId(2));
    episode.values.push_back(5.0f);
    episode.values.push_back(10.0f);
    episode.metadata = "test_episode";
    
    // Store episode
    em.storeEpisode(episode);
    assert(em.getEpisodeCount() == 1);
    
    // Retrieve episode
    nlm::EpisodicMemoryItem retrieved = em.retrieveEpisode(0);
    assert(retrieved.timestamp == 100);
    assert(retrieved.neurons.size() == 2);
    assert(retrieved.neurons[0] == nlm::NeuronId(1));
    assert(retrieved.values[0] == 5.0f);
    assert(retrieved.metadata == "test_episode");
    
    // Get recent episodes
    auto recent = em.getRecentEpisodes(1);
    assert(recent.size() == 1);
    assert(recent[0].timestamp == 100);
    
    std::cout << "    testEpisodicMemoryBasic passed" << std::endl;
}

void testAssociativeMemoryBasic() {
    nlm::AssociativeMemory am;
    
    // Test initial state
    assert(am.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 0.0f);
    assert(am.getAssociations(nlm::NeuronId(1)).empty());
    
    // Create association
    am.associate(nlm::NeuronId(1), nlm::NeuronId(2), 0.8f);
    assert(am.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 0.8f);
    
    // Get associations
    auto associations = am.getAssociations(nlm::NeuronId(1));
    assert(associations.size() == 1);
    assert(associations[0] == nlm::NeuronId(2));
    
    // Test reverse association
    assert(am.getAssociationStrength(nlm::NeuronId(2), nlm::NeuronId(1)) == 0.0f);
    
    // Create reverse association
    am.associate(nlm::NeuronId(2), nlm::NeuronId(1), 0.6f);
    assert(am.getAssociationStrength(nlm::NeuronId(2), nlm::NeuronId(1)) == 0.6f);
    
    // Update association
    am.updateAssociation(nlm::NeuronId(1), nlm::NeuronId(2), 0.1f);
    assert(am.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 0.9f);
    
    std::cout << "    testAssociativeMemoryBasic passed" << std::endl;
}

void testMemoryIntegrationWithNeurons() {
    nlm::WorkingMemory wm;
    nlm::EpisodicMemory em;
    
    // Create neurons
    nlm::Neuron neuron1(nlm::NeuronId(1));
    nlm::Neuron neuron2(nlm::NeuronId(2));
    
    // Store neuron state in working memory
    wm.store(neuron1.getId(), neuron1.getMembranePotential());
    assert(wm.retrieve(neuron1.getId()) == neuron1.getMembranePotential());
    
    // Create episode with neuron activity
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1000;
    episode.neurons.push_back(neuron1.getId());
    episode.neurons.push_back(neuron2.getId());
    episode.values.push_back(0.5f);
    episode.values.push_back(0.8f);
    episode.metadata = "neural_activity";
    
    em.storeEpisode(episode);
    assert(em.getEpisodeCount() == 1);
    
    // Test association between neurons based on co-occurrence
    am.associate(neuron1.getId(), neuron2.getId(), 0.7f);
    assert(am.getAssociationStrength(neuron1.getId(), neuron2.getId()) == 0.7f);
    
    std::cout << "    testMemoryIntegrationWithNeurons passed" << std::endl;
}

void runAll() {
    testWorkingMemoryBasic();
    testEpisodicMemoryBasic();
    testAssociativeMemoryBasic();
    testMemoryIntegrationWithNeurons();
}

} // namespace test_memory_integration
