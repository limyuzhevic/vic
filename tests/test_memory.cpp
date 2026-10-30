// Memory and Neuromodulation Tests
#include "memory/Memory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_memory {

void testWorkingMemory() {
    std::cout << "    Testing Working Memory..." << std::endl;
    
    nlm::WorkingMemory memory;
    
    // Test initial state
    assert(memory.getCurrentSize() == 0);
    assert(memory.getCapacity() == 100);
    assert(!memory.contains(0));
    assert(memory.retrieve(0) == 0.0f);
    
    // Test storing and retrieving
    memory.store(0, 0.5f);
    assert(memory.contains(0));
    assert(memory.retrieve(0) == 0.5f);
    
    memory.store(1, 0.8f);
    assert(memory.retrieve(1) == 0.8f);
    assert(memory.getCurrentSize() == 2);
    
    // Test updating existing value
    memory.store(0, 0.9f);
    assert(memory.retrieve(0) == 0.9f);
    assert(memory.getCurrentSize() == 2); // Should not increase
    
    // Test capacity management (current implementation has basic capacity)
    for (size_t i = 100; i < 105; ++i) {
        memory.store(static_cast<nlm::NeuronId>(i), 0.1f * (i - 100));
    }
    
    std::cout << "    testWorkingMemory passed" << std::endl;
}

void testEpisodicMemory() {
    std::cout << "    Testing Episodic Memory..." << std::endl;
    
    nlm::EpisodicMemory memory;
    
    // Test initial state
    assert(memory.getEpisodeCount() == 0);
    assert(memory.getRecentEpisodes(10).empty());
    
    // Create test episodes
    nlm::EpisodicMemoryItem episode1;
    episode1.timestamp = 100;
    episode1.neurons = {0, 1, 2};
    episode1.values = {0.5f, 0.7f, 0.3f};
    episode1.metadata = "First experience";
    
    nlm::EpisodicMemoryItem episode2;
    episode2.timestamp = 200;
    episode2.neurons = {3, 4};
    episode2.values = {0.9f, 0.2f};
    episode2.metadata = "Second experience";
    
    // Store episodes
    memory.storeEpisode(episode1);
    memory.storeEpisode(episode2);
    
    assert(memory.getEpisodeCount() == 2);
    
    // Test retrieval
    auto retrieved1 = memory.retrieveEpisode(0);
    assert(retrieved1.timestamp == 100);
    assert(retrieved1.neurons.size() == 3);
    
    auto retrieved2 = memory.retrieveEpisode(1);
    assert(retrieved2.timestamp == 200);
    assert(retrieved2.neurons.size() == 2);
    
    // Test recent episodes
    auto recent = memory.getRecentEpisodes(1);
    assert(recent.size() == 1);
    assert(recent[0].timestamp == 200);
    
    recent = memory.getRecentEpisodes(5);
    assert(recent.size() == 2); // Should return all episodes
    
    // Test consolidation (just check it doesn't crash)
    memory.consolidate(0.5f);
    
    std::cout << "    testEpisodicMemory passed" << std::endl;
}

void testSemanticMemory() {
    std::cout << "    Testing Semantic Memory..." << std::endl;
    
    nlm::SemanticMemory memory;
    
    // Test initial state
    assert(!memory.hasFact("test"));
    assert(memory.retrieveFact("test").empty());
    assert(memory.getAllFacts().empty());
    
    // Store and retrieve facts
    memory.storeFact("color", "red");
    memory.storeFact("shape", "circle");
    
    assert(memory.hasFact("color"));
    assert(memory.retrieveFact("color") == "red");
    
    assert(memory.hasFact("shape"));
    assert(memory.retrieveFact("shape") == "circle");
    
    // Test updating existing fact
    memory.storeFact("color", "blue");
    assert(memory.retrieveFact("color") == "blue");
    
    // Get all facts
    auto allFacts = memory.getAllFacts();
    assert(allFacts.size() == 2);
    
    std::cout << "    testSemanticMemory passed" << std::endl;
}

void testProceduralMemory() {
    std::cout << "    Testing Procedural Memory..." << std::endl;
    
    nlm::ProceduralMemory memory;
    
    // Test initial state
    assert(memory.getAllSkills().empty());
    
    // Create test pattern
    std::vector<nlm::NeuronId> pattern = {0, 1, 2, 3, 4};
    
    // Learn skill
    memory.learnSkill("jump", pattern);
    
    // Retrieve skill
    nlm::Skill* skill = memory.getSkill("jump");
    assert(skill != nullptr);
    assert(skill->name == "jump");
    assert(skill->neuralPattern.size() == 5);
    assert(skill->proficiency == 0.0f);
    
    // Update proficiency
    memory.updateProficiency("jump", 0.5f);
    assert(skill->proficiency == 0.5f);
    
    memory.updateProficiency("jump", 0.6f); // Should clamp at 1.0
    assert(skill->proficiency == 1.0f);
    
    // Test multiple skills
    memory.learnSkill("walk", {5, 6, 7});
    memory.updateProficiency("walk", 0.3f);
    
    auto allSkills = memory.getAllSkills();
    assert(allSkills.size() == 2);
    
    std::cout << "    testProceduralMemory passed" << std::endl;
}

void testAssociativeMemory() {
    std::cout << "    Testing Associative Memory..." << std::endl;
    
    nlm::AssociativeMemory memory;
    
    // Test initial state
    assert(memory.getAssociations(0).empty());
    assert(memory.getAssociationStrength(0, 1) == 0.0f);
    
    // Create association
    memory.associate(0, 1, 0.8f);
    
    // Get associations
    auto associations = memory.getAssociations(0);
    assert(associations.size() == 1);
    assert(associations[0] == 1);
    
    auto associations2 = memory.getAssociations(1);
    assert(associations2.size() == 1);
    assert(associations2[0] == 0);
    
    // Get strength
    assert(memory.getAssociationStrength(0, 1) == 0.8f);
    assert(memory.getAssociationStrength(1, 0) == 0.8f);
    
    // Update strength
    memory.updateAssociation(0, 1, 0.2f);
    assert(memory.getAssociationStrength(0, 1) == 1.0f); // Clamped at 1.0
    
    std::cout << "    testAssociativeMemory passed" << std::endl;
}

void runAll() {
    testWorkingMemory();
    testEpisodicMemory();
    testSemanticMemory();
    testProceduralMemory();
    testAssociativeMemory();
}

} // namespace test_memory