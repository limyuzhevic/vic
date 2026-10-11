// Memory Tests
// Phase 2 implementation tests for NLM memory systems

#include "memory/Memory.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

namespace test_memory {

void testWorkingMemory() {
    std::cout << "    Testing WorkingMemory..." << std::endl;
    
    nlm::WorkingMemory wm;
    
    // Test initial state
    assert(wm.getCapacity() > 0);
    assert(wm.getCurrentSize() == 0);
    
    // Test storing and retrieving
    nlm::NeuronId neuron1(1);
    nlm::NeuronId neuron2(42);
    
    wm.store(neuron1, 0.8f);
    assert(wm.contains(neuron1));
    assert(wm.retrieve(neuron1) == 0.8f);
    assert(wm.getCurrentSize() == 1);
    
    wm.store(neuron2, -0.3f);
    assert(wm.contains(neuron2));
    assert(wm.retrieve(neuron2) == -0.3f);
    assert(wm.getCurrentSize() == 2);
    
    // Test decay
    wm.decay(0.5f);
    // After decay, values should be reduced (implementation dependent)
    assert(wm.getCurrentSize() == 2); // Size shouldn't change
    
    // Test clearing
    wm.clear();
    assert(wm.getCurrentSize() == 0);
    assert(!wm.contains(neuron1));
    assert(!wm.contains(neuron2));
    
    std::cout << "      WorkingMemory tests passed" << std::endl;
}

void testEpisodicMemory() {
    std::cout << "    Testing EpisodicMemory..." << std::endl;
    
    nlm::EpisodicMemory em;
    
    // Test initial state
    assert(em.getEpisodeCount() == 0);
    
    // Create test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.neurons = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    episode.values = {0.1f, 0.5f, 0.9f};
    episode.metadata = "test_episode_1";
    
    // Store episode
    em.storeEpisode(episode);
    assert(em.getEpisodeCount() == 1);
    
    // Retrieve episode
    nlm::EpisodicMemoryItem retrieved = em.retrieveEpisode(0);
    assert(retrieved.timestamp == 100);
    assert(retrieved.neurons.size() == 3);
    assert(retrieved.neurons[0].index() == 1);
    assert(retrieved.values[2] == 0.9f);
    assert(retrieved.metadata == "test_episode_1");
    
    // Store more episodes
    for (size_t i = 1; i <= 5; ++i) {
        nlm::EpisodicMemoryItem ep;
        ep.timestamp = 100 + i * 10;
        ep.neurons = {nlm::NeuronId(i)};
        ep.values = {static_cast<float>(i) * 0.1f};
        ep.metadata = "episode_" + std::to_string(i);
        em.storeEpisode(ep);
    }
    
    assert(em.getEpisodeCount() == 6);
    
    // Test recent episodes
    std::vector<nlm::EpisodicMemoryItem> recent = em.getRecentEpisodes(3);
    assert(recent.size() == 3);
    assert(recent[0].timestamp == 150); // Most recent
    assert(recent[1].timestamp == 140);
    assert(recent[2].timestamp == 130);
    
    // Test consolidation
    em.consolidate(0.5f); // Should consolidate episodes with high relevance
    assert(em.getEpisodeCount() >= 1); // May consolidate some away
    
    // Clear all
    em.clear();
    assert(em.getEpisodeCount() == 0);
    
    std::cout << "      EpisodicMemory tests passed" << std::endl;
}

void testSemanticMemory() {
    std::cout << "    Testing SemanticMemory..." << std::endl;
    
    nlm::SemanticMemory sm;
    
    // Test initial state
    assert(!sm.hasFact("color"));
    assert(sm.getAllFacts().empty());
    
    // Store facts
    sm.storeFact("color", "red");
    sm.storeFact("size", "large");
    sm.storeFact("shape", "circular");
    sm.storeFact("temperature", "warm");
    
    assert(sm.hasFact("color"));
    assert(sm.hasFact("size"));
    assert(sm.hasFact("shape"));
    assert(sm.hasFact("temperature"));
    assert(!sm.hasFact("weight"));
    
    // Retrieve facts
    assert(sm.retrieveFact("color") == "red");
    assert(sm.retrieveFact("size") == "large");
    assert(sm.retrieveFact("shape") == "circular");
    assert(sm.retrieveFact("temperature") == "warm");
    
    // Get all facts
    auto allFacts = sm.getAllFacts();
    assert(allFacts.size() == 4);
    
    // Check that we got all expected facts
    bool hasColor = false, hasSize = false, hasShape = false, hasTemp = false;
    for (const auto& fact : allFacts) {
        if (fact.first == "color") hasColor = true;
        if (fact.first == "size") hasSize = true;
        if (fact.first == "shape") hasShape = true;
        if (fact.first == "temperature") hasTemp = true;
    }
    
    assert(hasColor && hasSize && hasShape && hasTemp);
    
    // Clear all
    sm.clear();
    assert(sm.getAllFacts().empty());
    assert(!sm.hasFact("color"));
    
    std::cout << "      SemanticMemory tests passed" << std::endl;
}

void testProceduralMemory() {
    std::cout << "    Testing ProceduralMemory..." << std::endl;
    
    nlm::ProceduralMemory pm;
    
    // Test initial state
    assert(pm.getAllSkills().empty());
    
    // Learn skills
    std::vector<nlm::NeuronId> pattern1 = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    std::vector<nlm::NeuronId> pattern2 = {nlm::NeuronId(10), nlm::NeuronId(20)};
    
    pm.learnSkill("jump", pattern1);
    pm.learnSkill("walk", pattern2);
    
    // Retrieve skills
    nlm::Skill* jumpSkill = pm.getSkill("jump");
    nlm::Skill* walkSkill = pm.getSkill("walk");
    
    assert(jumpSkill != nullptr);
    assert(walkSkill != nullptr);
    assert(jumpSkill->name == "jump");
    assert(walkSkill->name == "walk");
    assert(jumpSkill->neuralPattern.size() == 3);
    assert(walkSkill->neuralPattern.size() == 2);
    
    // Update proficiency
    pm.updateProficiency("jump", 0.3f);
    pm.updateProficiency("walk", -0.1f);
    
    assert(jumpSkill->proficiency == 0.3f);
    assert(walkSkill->proficiency == -0.1f);
    
    // Get all skills
    auto allSkills = pm.getAllSkills();
    assert(allSkills.size() == 2);
    
    // Clear all
    pm.clear();
    assert(pm.getAllSkills().empty());
    assert(pm.getSkill("jump") == nullptr);
    assert(pm.getSkill("walk") == nullptr);
    
    std::cout << "      ProceduralMemory tests passed" << std::endl;
}

void testAssociativeMemory() {
    std::cout << "    Testing AssociativeMemory..." << std::endl;
    
    nlm::AssociativeMemory am;
    
    // Test initial state
    assert(am.getAssociations(nlm::NeuronId(1)).empty());
    
    // Create associations
    nlm::NeuronId neuronA(100);
    nlm::NeuronId neuronB(200);
    nlm::NeuronId neuronC(300);
    
    am.associate(neuronA, neuronB, 0.8f);
    am.associate(neuronA, neuronC, 0.3f);
    am.associate(neuronB, neuronC, 0.6f);
    
    // Retrieve associations
    std::vector<nlm::NeuronId> assocB = am.getAssociations(neuronA);
    std::vector<nlm::NeuronId> assocC = am.getAssociations(neuronB);
    
    assert(assocB.size() == 2); // B and C
    assert(assocC.size() == 2); // A and B
    
    // Check specific associations exist
    bool hasBFromA = false, hasCFromA = false;
    for (const auto& assoc : assocB) {
        if (assoc.index() == 200) hasBFromA = true;
        if (assoc.index() == 300) hasCFromA = true;
    }
    assert(hasBFromA && hasCFromA);
    
    // Check association strength
    float strengthAB = am.getAssociationStrength(neuronA, neuronB);
    float strengthAC = am.getAssociationStrength(neuronA, neuronC);
    float strengthBC = am.getAssociationStrength(neuronB, neuronC);
    
    assert(strengthAB == 0.8f);
    assert(strengthAC == 0.3f);
    assert(strengthBC == 0.6f);
    
    // Update association strength
    am.updateAssociation(neuronA, neuronB, 0.2f); // Should become 1.0f
    assert(am.getAssociationStrength(neuronA, neuronB) == 1.0f);
    
    // Clear all
    am.clear();
    assert(am.getAssociations(neuronA).empty());
    assert(am.getAssociationStrength(neuronA, neuronB) == 0.0f);
    
    std::cout << "      AssociativeMemory tests passed" << std::endl;
}

void runAll() {
    testWorkingMemory();
    testEpisodicMemory();
    testSemanticMemory();
    testProceduralMemory();
    testAssociativeMemory();
}

} // namespace test_memory
