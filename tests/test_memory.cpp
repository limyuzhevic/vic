// Memory System Tests
#include "memory/Memory.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_memory {

void testWorkingMemory() {
    std::cout << "    Testing Working Memory..." << std::endl;
    
    nlm::WorkingMemory wm;
    
    // Create some test neurons
    nlm::NeuronId neuron1(1), neuron2(2), neuron3(3);
    
    // Test initial state
    assert(!wm.contains(neuron1));
    assert(wm.retrieve(neuron1) == 0.0f);
    assert(wm.getCurrentSize() == 0);
    assert(wm.getCapacity() > 0);
    
    // Test storage and retrieval
    wm.store(neuron1, 0.5f);
    assert(wm.contains(neuron1));
    assert(std::abs(wm.retrieve(neuron1) - 0.5f) < 0.001f);
    assert(wm.getCurrentSize() == 1);
    
    wm.store(neuron2, -0.3f);
    assert(wm.contains(neuron2));
    assert(wm.getCurrentSize() == 2);
    
    // Test overwriting existing value
    wm.store(neuron1, 0.8f);
    assert(std::abs(wm.retrieve(neuron1) - 0.8f) < 0.001f);
    
    // Test capacity limits
    size_t originalCapacity = wm.getCapacity();
    // Add more items to test capacity (implementation may vary)
    
    // Test decay
    wm.decay(0.1f);
    assert(std::abs(wm.retrieve(neuron1) - 0.72f) < 0.001f); // 0.8 * (1-0.1) = 0.72
    
    // Test clearing
    wm.clear();
    assert(wm.getCurrentSize() == 0);
    assert(!wm.contains(neuron1));
    
    std::cout << "    testWorkingMemory passed" << std::endl;
}

void testEpisodicMemory() {
    std::cout << "    Testing Episodic Memory..." << std::endl;
    
    nlm::EpisodicMemory em;
    
    // Create test neurons
    nlm::NeuronId neuron1(1), neuron2(2), neuron3(3);
    std::vector<float> values{0.5f, -0.3f, 0.9f};
    
    // Create test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.neurons = {neuron1, neuron2, neuron3};
    episode.values = values;
    episode.metadata = "Test episode";
    
    // Test storing episode
    assert(em.getEpisodeCount() == 0);
    em.storeEpisode(episode);
    assert(em.getEpisodeCount() == 1);
    
    // Test retrieving episode
    nlm::EpisodicMemoryItem retrieved = em.retrieveEpisode(0);
    assert(retrieved.timestamp == 100);
    assert(retrieved.neurons.size() == 3);
    assert(retrieved.values.size() == 3);
    assert(retrieved.metadata == "Test episode");
    
    // Test recent episodes
    std::vector<nlm::EpisodicMemoryItem> recent = em.getRecentEpisodes(5);
    assert(recent.size() == 1);
    
    // Store more episodes
    nlm::EpisodicMemoryItem episode2;
    episode2.timestamp = 200;
    episode2.neurons = {neuron2};
    episode2.values = {0.7f};
    em.storeEpisode(episode2);
    
    nlm::EpisodicMemoryItem episode3;
    episode3.timestamp = 300;
    episode3.neurons = {neuron3};
    episode3.values = {-0.5f};
    em.storeEpisode(episode3);
    
    // Test recent episodes limit
    std::vector<nlm::EpisodicMemoryItem> recent3 = em.getRecentEpisodes(2);
    assert(recent3.size() == 2);
    assert(recent3[0].timestamp == 200);
    assert(recent3[1].timestamp == 300);
    
    // Test clearing
    em.clear();
    assert(em.getEpisodeCount() == 0);
    
    std::cout << "    testEpisodicMemory passed" << std::endl;
}

void testSemanticMemory() {
    std::cout << "    Testing Semantic Memory..." << std::endl;
    
    nlm::SemanticMemory sm;
    
    // Test initial state
    assert(!sm.hasFact("color"));
    assert(sm.retrieveFact("color") == "");
    assert(sm.getAllFacts().size() == 0);
    
    // Test storing facts
    sm.storeFact("color", "red");
    assert(sm.hasFact("color"));
    assert(sm.retrieveFact("color") == "red");
    
    sm.storeFact("shape", "circle");
    assert(sm.hasFact("shape"));
    assert(sm.retrieveFact("shape") == "circle");
    
    // Test overwriting fact
    sm.storeFact("color", "blue");
    assert(sm.retrieveFact("color") == "blue");
    
    // Test retrieving all facts
    auto allFacts = sm.getAllFacts();
    assert(allFacts.size() == 2);
    
    // Test clearing
    sm.clear();
    assert(sm.getAllFacts().size() == 0);
    assert(!sm.hasFact("color"));
    
    std::cout << "    testSemanticMemory passed" << std::endl;
}

void testProceduralMemory() {
    std::cout << "    Testing Procedural Memory..." << std::endl;
    
    nlm::ProceduralMemory pm;
    
    // Create test neuron pattern
    std::vector<nlm::NeuronId> pattern1{1, 2, 3, 4, 5};
    std::vector<nlm::NeuronId> pattern2{6, 7, 8, 9, 10};
    
    // Test learning skills
    pm.learnSkill("walk", pattern1);
    pm.learnSkill("run", pattern2);
    
    // Test retrieving skills
    nlm::Skill* walkSkill = pm.getSkill("walk");
    assert(walkSkill != nullptr);
    assert(walkSkill->name == "walk");
    assert(walkSkill->neuralPattern.size() == 5);
    assert(walkSkill->proficiency == 0.0f);
    
    nlm::Skill* runSkill = pm.getSkill("run");
    assert(runSkill != nullptr);
    assert(runSkill->name == "run");
    
    // Test updating proficiency
    pm.updateProficiency("walk", 0.5f);
    assert(walkSkill->proficiency == 0.5f);
    
    pm.updateProficiency("walk", 0.6f);
    assert(walkSkill->proficiency == 1.0f);  // Clamped at 1.0
    
    pm.updateProficiency("walk", -0.3f);
    assert(walkSkill->proficiency == 0.7f);
    
    // Test getting all skills
    auto allSkills = pm.getAllSkills();
    assert(allSkills.size() == 2);
    
    // Test clearing
    pm.clear();
    assert(pm.getAllSkills().size() == 0);
    assert(pm.getSkill("walk") == nullptr);
    
    std::cout << "    testProceduralMemory passed" << std::endl;
}

void testAssociativeMemory() {
    std::cout << "    Testing Associative Memory..." << std::endl;
    
    nlm::AssociativeMemory am;
    
    // Create test neurons
    nlm::NeuronId neuronA(1), neuronB(2), neuronC(3);
    
    // Test initial state
    assert(am.getAssociationStrength(neuronA, neuronB) == 0.0f);
    assert(am.getAssociations(neuronA).size() == 0);
    
    // Test creating associations
    am.associate(neuronA, neuronB, 0.8f);
    assert(am.getAssociationStrength(neuronA, neuronB) == 0.8f);
    assert(am.getAssociationStrength(neuronB, neuronA) == 0.8f);  // Should be symmetric
    
    std::vector<nlm::NeuronId> associated = am.getAssociations(neuronA);
    assert(associated.size() == 1);
    assert(associated[0] == neuronB);
    
    // Test overwriting association
    am.associate(neuronA, neuronB, 0.3f);
    assert(am.getAssociationStrength(neuronA, neuronB) == 0.3f);
    
    // Test creating new associations
    am.associate(neuronA, neuronC, 0.6f);
    assert(am.getAssociationStrength(neuronA, neuronC) == 0.6f);
    
    // Test updating association
    am.updateAssociation(neuronA, neuronB, 0.2f);
    assert(am.getAssociationStrength(neuronA, neuronB) == 0.5f);  // 0.3 + 0.2
    
    am.updateAssociation(neuronA, neuronB, 0.8f);
    assert(am.getAssociationStrength(neuronA, neuronB) == 1.0f);  // Clamped at 1.0
    
    // Test clearing
    am.clear();
    assert(am.getAssociationStrength(neuronA, neuronB) == 0.0f);
    assert(am.getAssociationStrength(neuronA, neuronC) == 0.0f);
    
    std::cout << "    testAssociativeMemory passed" << std::endl;
}

void testMemorySystemIntegration() {
    std::cout << "    Testing Memory System Integration..." << std::endl;
    
    // Test that all memory systems can be created and used together
    nlm::WorkingMemory wm;
    nlm::EpisodicMemory em;
    nlm::SemanticMemory sm;
    nlm::ProceduralMemory pm;
    nlm::AssociativeMemory am;
    
    // Test neurons
    nlm::NeuronId neuron1(1), neuron2(2), neuron3(3);
    
    // Store in working memory
    wm.store(neuron1, 0.5f);
    assert(wm.retrieve(neuron1) == 0.5f);
    
    // Store episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.neurons = {neuron1, neuron2};
    episode.values = {0.5f, 0.8f};
    em.storeEpisode(episode);
    
    // Store fact
    sm.storeFact("test", "value");
    
    // Learn skill
    pm.learnSkill("testSkill", {neuron1, neuron2, neuron3});
    
    // Create association
    am.associate(neuron1, neuron2, 0.9f);
    
    // Verify all systems work together
    assert(em.getEpisodeCount() == 1);
    assert(sm.hasFact("test"));
    assert(pm.getSkill("testSkill") != nullptr);
    assert(am.getAssociationStrength(neuron1, neuron2) == 0.9f);
    
    // Clear all systems
    wm.clear();
    em.clear();
    sm.clear();
    pm.clear();
    am.clear();
    
    assert(wm.getCurrentSize() == 0);
    assert(em.getEpisodeCount() == 0);
    assert(sm.getAllFacts().size() == 0);
    assert(pm.getAllSkills().size() == 0);
    assert(am.getAssociationStrength(neuron1, neuron2) == 0.0f);
    
    std::cout << "    testMemorySystemIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Memory System Tests..." << std::endl;
    
    testWorkingMemory();
    testEpisodicMemory();
    testSemanticMemory();
    testProceduralMemory();
    testAssociativeMemory();
    testMemorySystemIntegration();
    
    std::cout << "All memory system tests passed!" << std::endl;
}

} // namespace test_memory
