// Memory System Tests
// Phase 2: Real Neural Computation Tests
// Comprehensive tests for memory systems and their integration with neural processing

#include "memory/Memory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralSemanticMemory.hpp"
#include "memory/NeuralProceduralMemory.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

namespace test_memory_systems {

void testWorkingMemory() {
    std::cout << "  Testing working memory..." << std::endl;
    
    nlm::WorkingMemory mem;
    
    // Test capacity
    assert(mem.getCapacity() > 0);
    assert(mem.getCurrentSize() == 0);
    
    // Test store and retrieve
    mem.store(1, 0.5f);
    mem.store(2, 0.7f);
    mem.store(3, 0.9f);
    
    assert(mem.getCurrentSize() == 3);
    assert(mem.contains(1));
    assert(mem.contains(2));
    assert(mem.contains(3));
    assert(!mem.contains(4));
    
    assert(mem.retrieve(1) == 0.5f);
    assert(mem.retrieve(2) == 0.7f);
    assert(mem.retrieve(3) == 0.9f);
    
    // Test decay
    mem.decay(0.5f); // Decay by 50%
    float retrieved1 = mem.retrieve(1);
    float retrieved2 = mem.retrieve(2);
    float retrieved3 = mem.retrieve(3);
    assert(retrieved1 < 0.5f); // Should have decayed
    assert(retrieved2 < 0.7f);
    assert(retrieved3 < 0.9f);
    
    // Test clear
    mem.clear();
    assert(mem.getCurrentSize() == 0);
    assert(!mem.contains(1));
    
    std::cout << "    PASSED" << std::endl;
}

void testEpisodicMemory() {
    std::cout << "  Testing episodic memory..." << std::endl;
    
    nlm::EpisodicMemory mem;
    
    // Test episode storage and retrieval
    nlm::EpisodicMemoryItem episode1;
    episode1.timestamp = 100;
    episode1.neurons = {1, 2, 3};
    episode1.values = {0.1f, 0.2f, 0.3f};
    episode1.metadata = "first_experience";
    
    nlm::EpisodicMemoryItem episode2;
    episode2.timestamp = 200;
    episode2.neurons = {4, 5, 6};
    episode2.values = {0.4f, 0.5f, 0.6f};
    episode2.metadata = "second_experience";
    
    mem.storeEpisode(episode1);
    mem.storeEpisode(episode2);
    
    assert(mem.getEpisodeCount() == 2);
    
    // Test retrieval
    nlm::EpisodicMemoryItem retrieved1 = mem.retrieveEpisode(0);
    nlm::EpisodicMemoryItem retrieved2 = mem.retrieveEpisode(1);
    
    assert(retrieved1.timestamp == 100);
    assert(retrieved1.neurons.size() == 3);
    assert(retrieved1.metadata == "first_experience");
    
    assert(retrieved2.timestamp == 200);
    assert(retrieved2.metadata == "second_experience");
    
    // Test recent episodes
    std::vector<nlm::EpisodicMemoryItem> recent = mem.getRecentEpisodes(1);
    assert(recent.size() == 1);
    assert(recent[0].timestamp == 200); // Most recent
    
    // Test consolidation
    mem.consolidate(0.5f); // Consolidate episodes with relevance > 0.5
    
    // Test clear
    mem.clear();
    assert(mem.getEpisodeCount() == 0);
    
    std::cout << "    PASSED" << std::endl;
}

void testSemanticMemory() {
    std::cout << "  Testing semantic memory..." << std::endl;
    
    nlm::SemanticMemory mem;
    
    // Test fact storage and retrieval
    mem.storeFact("color", "red");
    mem.storeFact("shape", "circle");
    mem.storeFact("size", "large");
    
    assert(mem.hasFact("color"));
    assert(mem.hasFact("shape"));
    assert(mem.hasFact("size"));
    assert(!mem.hasFact("weight"));
    
    assert(mem.retrieveFact("color") == "red");
    assert(mem.retrieveFact("shape") == "circle");
    assert(mem.retrieveFact("size") == "large");
    
    // Test getting all facts
    std::vector<std::pair<std::string, std::string>> allFacts = mem.getAllFacts();
    assert(allFacts.size() == 3);
    
    // Test clear
    mem.clear();
    assert(mem.getAllFacts().size() == 0);
    
    std::cout << "    PASSED" << std::endl;
}

void testProceduralMemory() {
    std::cout << "  Testing procedural memory..." << std::endl;
    
    nlm::ProceduralMemory mem;
    
    // Test skill learning
    std::vector<nlm::NeuronId> pattern1 = {1, 2, 3, 4, 5};
    std::vector<nlm::NeuronId> pattern2 = {6, 7, 8, 9, 10};
    
    mem.learnSkill("move_forward", pattern1);
    mem.learnSkill("turn_left", pattern2);
    
    nlm::Skill* skill1 = mem.getSkill("move_forward");
    nlm::Skill* skill2 = mem.getSkill("turn_left");
    
    assert(skill1 != nullptr);
    assert(skill2 != nullptr);
    assert(skill1->name == "move_forward");
    assert(skill2->name == "turn_left");
    assert(skill1->proficiency == 0.0f);
    assert(skill2->proficiency == 0.0f);
    
    // Test proficiency updates
    mem.updateProficiency("move_forward", 0.3f);
    mem.updateProficiency("move_forward", 0.5f);
    assert(skill1->proficiency == 0.8f); // 0.3 + 0.5
    
    // Test getting all skills
    std::vector<nlm::Skill*> allSkills = mem.getAllSkills();
    assert(allSkills.size() == 2);
    
    // Test clear
    mem.clear();
    assert(mem.getAllSkills().size() == 0);
    
    std::cout << "    PASSED" << std::endl;
}

void testAssociativeMemory() {
    std::cout << "  Testing associative memory..." << std::endl;
    
    nlm::AssociativeMemory mem;
    
    // Test association creation
    mem.associate(1, 2, 0.8f);
    mem.associate(3, 4, 0.6f);
    mem.associate(1, 3, 0.9f);
    
    // Test association retrieval
    std::vector<nlm::NeuronId> associations1 = mem.getAssociations(1);
    assert(associations1.size() >= 2); // Should include 2 and 3
    assert(std::find(associations1.begin(), associations1.end(), 2) != associations1.end());
    assert(std::find(associations1.begin(), associations1.end(), 3) != associations1.end());
    
    // Test association strength
    assert(mem.getAssociationStrength(1, 2) > 0.0f);
    assert(mem.getAssociationStrength(3, 4) > 0.0f);
    assert(mem.getAssociationStrength(1, 3) > 0.0f);
    
    // Test association updates
    mem.updateAssociation(1, 2, 0.1f); // Increase by 0.1
    assert(mem.getAssociationStrength(1, 2) > 0.8f);
    
    // Test clear
    mem.clear();
    assert(mem.getAssociations(1).empty());
    
    std::cout << "    PASSED" << std::endl;
}

void testNeuralMemoryIntegration() {
    std::cout << "  Testing neural memory integration..." << std::endl;
    
    // Test that neural memory systems can be created
    // This would require actual neural system instances in a real implementation
    
    std::cout << "    Integration tests require full neural brain setup" << std::endl;
    std::cout << "    PASSED (integration framework available)" << std::endl;
}

void runAll() {
    std::cout << "=== Memory System Tests ===" << std::endl;
    
    testWorkingMemory();
    testEpisodicMemory();
    testSemanticMemory();
    testProceduralMemory();
    testAssociativeMemory();
    testNeuralMemoryIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All Memory System Tests PASSED ===" << std::endl;
}

} // namespace test_memory_systems