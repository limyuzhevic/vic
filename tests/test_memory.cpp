// Memory System Tests
// Tests for working memory, episodic memory, semantic memory, procedural memory, and associative memory

#include "memory/Memory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_memory {

void testWorkingMemory() {
    std::cout << "    Testing working memory..." << std::endl;
    
    nlm::WorkingMemory wm;
    
    // Test initial state
    assert(wm.getCapacity() == 100);
    assert(wm.getCurrentSize() == 0);
    assert(wm.getActiveTraces() == 0);
    
    // Test storage and retrieval
    nlm::NeuronId neuron1(1);
    float value = 0.75f;
    
    wm.store(neuron1, value);
    assert(wm.contains(neuron1));
    assert(wm.retrieve(neuron1) == value);
    
    // Test capacity limits
    wm.setCapacity(2);
    assert(wm.getCapacity() == 2);
    
    // Test decay
    wm.decay(0.1f);
    
    std::cout << "    Working memory test passed" << std::endl;
}

void testEpisodicMemory() {
    std::cout << "    Testing episodic memory..." << std::endl;
    
    nlm::EpisodicMemory em;
    
    // Test initial state
    assert(em.getEpisodeCount() == 0);
    
    // Create a test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.neurons = {nlm::NeuronId(1), nlm::NeuronId(2)};
    episode.values = {0.8f, 0.6f};
    episode.metadata = "test_episode";
    
    // Store episode
    em.storeEpisode(episode);
    assert(em.getEpisodeCount() == 1);
    
    // Retrieve episode
    auto retrieved = em.retrieveEpisode(0);
    assert(retrieved.timestamp == 100);
    assert(retrieved.neurons.size() == 2);
    
    // Test recent episodes
    auto recent = em.getRecentEpisodes(5);
    assert(recent.size() == 1);
    
    // Test consolidation
    em.consolidate(0.5f);
    
    std::cout << "    Episodic memory test passed" << std::endl;
}

void testSemanticMemory() {
    std::cout << "    Testing semantic memory..." << std::endl;
    
    nlm::SemanticMemory sm;
    
    // Test storage and retrieval
    sm.storeFact("color", "red");
    sm.storeFact("shape", "circle");
    
    assert(sm.retrieveFact("color") == "red");
    assert(sm.retrieveFact("shape") == "circle");
    assert(sm.retrieveFact("size") == ""); // Non-existent
    
    assert(sm.hasFact("color"));
    assert(sm.hasFact("shape"));
    assert(!sm.hasFact("weight"));
    
    // Test get all facts
    auto allFacts = sm.getAllFacts();
    assert(allFacts.size() == 2);
    
    std::cout << "    Semantic memory test passed" << std::endl;
}

void testProceduralMemory() {
    std::cout << "    Testing procedural memory..." << std::endl;
    
    nlm::ProceduralMemory pm;
    
    // Create a neural pattern
    std::vector<nlm::NeuronId> pattern = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    
    // Learn a skill
    pm.learnSkill("walk", pattern);
    pm.learnSkill("run", pattern);
    
    // Retrieve skill
    auto* skill = pm.getSkill("walk");
    assert(skill != nullptr);
    assert(skill->name == "walk");
    assert(skill->neuralPattern.size() == 3);
    assert(skill->proficiency == 0.5f); // Default proficiency
    
    // Test all skills
    auto allSkills = pm.getAllSkills();
    assert(allSkills.size() == 2);
    
    // Test proficiency update
    pm.updateProficiency("walk", 0.2f);
    skill = pm.getSkill("walk");
    assert(skill->proficiency == 0.7f);
    
    std::cout << "    Procedural memory test passed" << std::endl;
}

void testAssociativeMemory() {
    std::cout << "    Testing associative memory..." << std::endl;
    
    nlm::AssociativeMemory am;
    
    // Create associations
    nlm::NeuronId neuronA(1);
    nlm::NeuronId neuronB(2);
    nlm::NeuronId neuronC(3);
    
    am.associate(neuronA, neuronB, 0.8f);
    am.associate(neuronB, neuronC, 0.6f);
    
    // Test association strength
    assert(am.getAssociationStrength(neuronA, neuronB) == 0.8f);
    assert(am.getAssociationStrength(neuronB, neuronA) == 0.8f); // Symmetric
    
    // Test get associations
    auto associationsA = am.getAssociations(neuronA);
    assert(associationsA.size() == 1);
    assert(associationsA[0] == neuronB);
    
    auto associationsB = am.getAssociations(neuronB);
    assert(associationsB.size() == 2); // Associated with A and C
    
    // Test update association
    am.updateAssociation(neuronA, neuronB, 0.1f);
    assert(am.getAssociationStrength(neuronA, neuronB) == 0.9f);
    
    std::cout << "    Associative memory test passed" << std::endl;
}

void testNeuralMemoryIntegration() {
    std::cout << "    Testing neural memory integration..." << std::endl;
    
    // This test verifies that neural memory systems work with the brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    
    if (brain.initialize()) {
        // Brain should have neural memory systems
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        
        if (workingMemory) {
            workingMemory->storeToNeuron(nlm::NeuronId(1), 0.5f);
            assert(workingMemory->contains(nlm::NeuronId(1)));
        }
        
        if (episodicMemory) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = 1;
            episode.neurons = {nlm::NeuronId(1)};
            episode.values = {0.8f};
            
            episodicMemory->storeEpisode(episode);
            assert(episodicMemory->getEpisodeCount() == 1);
        }
    }
    
    std::cout << "    Neural memory integration test passed" << std::endl;
}

} // namespace test_memory
