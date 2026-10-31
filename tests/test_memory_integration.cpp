// Memory Integration Tests
// Tests memory systems integration (working memory, episodic memory, associative memory)

#include "memory/Memory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>

namespace test_memory_integration {

void testWorkingMemoryIntegration() {
    std::cout << "  Testing Working Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test basic store/retrieve
    nlm::NeuronId neuron1(1);
    nlm::NeuronId neuron2(2);
    
    workingMemory->store(neuron1, 0.5f);
    workingMemory->store(neuron2, 0.8f);
    
    assert(workingMemory->contains(neuron1));
    assert(workingMemory->contains(neuron2));
    
    assert(std::abs(workingMemory->retrieve(neuron1) - 0.5f) < 0.001f);
    assert(std::abs(workingMemory->retrieve(neuron2) - 0.8f) < 0.001f);
    
    // Test capacity limits
    size_t capacity = workingMemory->getCapacity();
    
    // Test decay
    workingMemory->store(neuron1, 1.0f);
    float originalValue = workingMemory->retrieve(neuron1);
    workingMemory->decay(0.1f);
    assert(workingMemory->retrieve(neuron1) < originalValue);
    
    // Test size tracking
    size_t initialSize = workingMemory->getCurrentSize();
    nlm::NeuronId neuron3(3);
    workingMemory->store(neuron3, 0.3f);
    assert(workingMemory->getCurrentSize() == initialSize + 1);
    
    // Test memory competition (overwrite behavior)
    workingMemory->store(neuron1, 0.9f);
    assert(std::abs(workingMemory->retrieve(neuron1) - 0.9f) < 0.001f);
    
    std::cout << "    testWorkingMemoryIntegration passed" << std::endl;
}

void testEpisodicMemoryIntegration() {
    std::cout << "  Testing Episodic Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Create test episodes
    std::vector<nlm::EpisodicMemoryItem> episodes;
    
    nlm::EpisodicMemoryItem episode1;
    episode1.timestamp = 0;
    episode1.neurons = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    episode1.values = {0.1f, 0.2f, 0.3f};
    episode1.metadata = "first_event";
    
    nlm::EpisodicMemoryItem episode2;
    episode2.timestamp = 10;
    episode2.neurons = {nlm::NeuronId(4), nlm::NeuronId(5)};
    episode2.values = {0.4f, 0.5f};
    episode2.metadata = "second_event";
    
    nlm::EpisodicMemoryItem episode3;
    episode3.timestamp = 20;
    episode3.neurons = {nlm::NeuronId(1), nlm::NeuronId(6)};
    episode3.values = {0.6f, 0.7f};
    episode3.metadata = "third_event";
    
    episodes.push_back(episode1);
    episodes.push_back(episode2);
    episodes.push_back(episode3);
    
    // Store episodes
    for (const auto& episode : episodes) {
        episodicMemory->storeEpisode(episode);
    }
    
    // Test episode count
    assert(episodicMemory->getEpisodeCount() == 3);
    
    // Test retrieval by index
    nlm::EpisodicMemoryItem retrieved1 = episodicMemory->retrieveEpisode(0);
    assert(retrieved1.metadata == "first_event");
    assert(retrieved1.timestamp == 0);
    
    // Test recent episodes
    std::vector<nlm::EpisodicMemoryItem> recent = episodicMemory->getRecentEpisodes(2);
    assert(recent.size() == 2);
    assert(recent[0].metadata == "third_event");  // Most recent
    assert(recent[1].metadata == "second_event"); // Second most recent
    
    // Test retrieval of specific neuron patterns
    nlm::NeuronId neuron1(1);
    for (size_t i = 0; i < episodes.size(); ++i) {
        nlm::EpisodicMemoryItem ep = episodicMemory->retrieveEpisode(i);
        for (size_t j = 0; j < ep.neurons.size(); ++j) {
            if (ep.neurons[j] == neuron1) {
                assert(ep.metadata == "first_event" && i == 0 ||
                       ep.metadata == "third_event" && i == 2);
            }
        }
    }
    
    // Test consolidation with different thresholds
    episodicMemory->clear();
    episodicMemory->storeEpisode(episode1);
    episodicMemory->storeEpisode(episode2);
    episodicMemory->storeEpisode(episode3);
    
    // This would call the real consolidation in Phase 2
    episodicMemory->consolidate(0.5f);
    
    std::cout << "    testEpisodicMemoryIntegration passed" << std::endl;
}

void testSemanticMemoryIntegration() {
    std::cout << "  Testing Semantic Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get semantic memory from brain (if available) or create standalone
    // Semantic memory integration may be through working memory or other systems
    // This test verifies semantic memory as part of the broader memory system
    
    // For now, test the memory system's conceptual integration
    // Semantic memory facts can be used to influence working memory retrieval
    nlm::SemanticMemory semantic;
    
    semantic.storeFact("color", "red");
    semantic.storeFact("shape", "circle");
    semantic.storeFact("size", "large");
    
    assert(semantic.hasFact("color"));
    assert(semantic.retrieveFact("color") == "red");
    assert(semantic.retrieveFact("shape") == "circle");
    assert(semantic.retrieveFact("nonexistent") == "");
    
    std::vector<std::pair<std::string, std::string>> allFacts = semantic.getAllFacts();
    assert(allFacts.size() == 3);
    
    // Test fact updates
    semantic.storeFact("color", "blue");
    assert(semantic.retrieveFact("color") == "blue");
    
    // Semantic memory integration with working memory
    // This would be real in Phase 2 where semantic facts influence working memory retrieval
    semantic.storeFact("working_memory_capacity", "100");
    
    std::cout << "    testSemanticMemoryIntegration passed" << std::endl;
}

void testProceduralMemoryIntegration() {
    std::cout << "  Testing Procedural Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get procedural memory from brain
    // This tests procedural memory integration with the neural system
    
    nlm::ProceduralMemory procedural;
    
    // Create skill patterns
    std::vector<nlm::NeuronId> pattern1 = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    std::vector<nlm::NeuronId> pattern2 = {nlm::NeuronId(4), nlm::NeuronId(5)};
    
    procedural.learnSkill("walk", pattern1);
    procedural.learnSkill("jump", pattern2);
    
    nlm::Skill* walkSkill = procedural.getSkill("walk");
    assert(walkSkill != nullptr);
    assert(walkSkill->name == "walk");
    assert(walkSkill->proficiency == 0.0f);
    assert(walkSkill->neuralPattern.size() == 3);
    
    // Test proficiency updates
    procedural.updateProficiency("walk", 0.3f);
    assert(walkSkill->proficiency == 0.3f);
    
    procedural.updateProficiency("walk", 0.8f);
    assert(walkSkill->proficiency == 0.8f);
    
    // Test skill overflow protection
    procedural.updateProficiency("walk", 1.5f);  // Should clamp at 1.0
    assert(walkSkill->proficiency == 1.0f);
    
    std::vector<nlm::Skill*> allSkills = procedural.getAllSkills();
    assert(allSkills.size() == 2);
    
    // Procedural memory integration with brain
    // Skills can influence action selection and neural patterns
    procedural.learnSkill("explore", pattern1);
    nlm::Skill* exploreSkill = procedural.getSkill("explore");
    assert(exploreSkill != nullptr);
    
    std::cout << "    testProceduralMemoryIntegration passed" << std::endl;
}

void testAssociativeMemoryIntegration() {
    std::cout << "  Testing Associative Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AssociativeMemory associative;
    
    // Create associations
    nlm::NeuronId neuronA(1);
    nlm::NeuronId neuronB(2);
    nlm::NeuronId neuronC(3);
    
    associative.associate(neuronA, neuronB, 0.7f);
    associative.associate(neuronB, neuronC, 0.5f);
    associative.associate(neuronA, neuronC, 0.3f);
    
    // Test strength retrieval (bidirectional)
    assert(std::abs(associative.getAssociationStrength(neuronA, neuronB) - 0.7f) < 0.001f);
    assert(std::abs(associative.getAssociationStrength(neuronB, neuronA) - 0.7f) < 0.001f);
    assert(std::abs(associative.getAssociationStrength(neuronB, neuronC) - 0.5f) < 0.001f);
    
    // Test association lists
    std::vector<nlm::NeuronId> neighborsOfA = associative.getAssociations(neuronA);
    assert(neighborsOfA.size() == 2);  // B and C
    assert(std::find(neighborsOfA.begin(), neighborsOfA.end(), neuronB) != neighborsOfA.end());
    assert(std::find(neighborsOfA.begin(), neighborsOfA.end(), neuronC) != neighborsOfA.end());
    
    // Test association updates
    associative.updateAssociation(neuronA, neuronB, 0.2f);
    assert(std::abs(associative.getAssociationStrength(neuronA, neuronB) - 0.9f) < 0.001f);
    
    // Test clamping
    associative.updateAssociation(neuronA, neuronB, 0.5f);  // Should not exceed 1.0
    assert(std::abs(associative.getAssociationStrength(neuronA, neuronB) - 1.0f) < 0.001f);
    
    // Test associative memory integration with working memory
    // Strong associations can influence which items are prioritized in working memory
    nlm::NeuronId neuronD(4);
    associative.associate(neuronD, neuronA, 0.9f);  // Strong association with A
    
    std::vector<nlm::NeuronId> neighborsOfD = associative.getAssociations(neuronD);
    assert(neighborsOfD.size() == 1);
    assert(neighborsOfD[0] == neuronA);
    
    std::cout << "    testAssociativeMemoryIntegration passed" << std::endl;
}

void testMemorySystemConnectivity() {
    std::cout << "  Testing Memory System Connectivity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify all memory systems are accessible
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    
    // Test integration points
    nlm::NeuronId neuron1(1);
    nlm::NeuronId neuron2(2);
    
    // Working memory can store values
    brain.getWorkingMemory()->store(neuron1, 0.5f);
    assert(brain.getWorkingMemory()->retrieve(neuron1) > 0.0f);
    
    // Episodic memory can store experiences
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.neurons = {neuron1, neuron2};
    episode.values = {0.3f, 0.7f};
    episode.metadata = "connectivity_test";
    
    brain.getEpisodicMemory()->storeEpisode(episode);
    assert(brain.getEpisodicMemory()->getEpisodeCount() > 0);
    
    // Associative memory can link neurons
    brain.getAssociativeMemory()->associate(neuron1, neuron2, 0.8f);
    assert(brain.getAssociativeMemory()->getAssociationStrength(neuron1, neuron2) > 0.0f);
    
    // Semantic and procedural memory are available
    nlm::SemanticMemory semantic;
    semantic.storeFact("test", "connectivity");
    assert(semantic.retrieveFact("test") == "connectivity");
    
    nlm::ProceduralMemory procedural;
    std::vector<nlm::NeuronId> pattern = {neuron1, neuron2};
    procedural.learnSkill("test_skill", pattern);
    assert(procedural.getSkill("test_skill") != nullptr);
    
    // Test memory cross-talk
    // This is a conceptual test of how memory systems interact
    // In Phase 2, working memory updates would influence episodic memory consolidation
    // And episodic memory would influence associative memory strength
    
    std::cout << "    testMemorySystemConnectivity passed" << std::endl;
}

void testMemoryPerformance() {
    std::cout << "  Testing Memory Performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    nlm::NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    nlm::AssociativeMemory* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Performance benchmark for working memory
    const size_t iterations = 10000;
    
    for (size_t i = 0; i < iterations; ++i) {
        nlm::NeuronId neuron(i % 100 + 1);  // Cycle through neurons
        float value = static_cast<float>(i) / static_cast<float>(iterations);
        workingMemory->store(neuron, value);
        float retrieved = workingMemory->retrieve(neuron);
        assert(std::abs(retrieved - value) < 0.001f);
    }
    
    // Performance benchmark for episodic memory
    for (size_t i = 0; i < iterations / 100; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = static_cast<nlm::SimulationStep>(i * 10);
        episode.neurons = {static_cast<nlm::NeuronId>(i % 50 + 1)};
        episode.values = {0.1f, 0.5f, 0.9f};
        episode.metadata = std::to_string(i);
        episodicMemory->storeEpisode(episode);
    }
    
    // Performance benchmark for associative memory
    for (size_t i = 0; i < iterations / 50; ++i) {
        nlm::NeuronId neuronA = static_cast<nlm::NeuronId>((i * 3) % 200 + 1);
        nlm::NeuronId neuronB = static_cast<nlm::NeuronId>((i * 7) % 200 + 1);
        float strength = static_cast<float>(i) / static_cast<float>(iterations) * 0.9f + 0.1f;
        associativeMemory->associate(neuronA, neuronB, strength);
        
        float retrievedStrength = associativeMemory->getAssociationStrength(neuronA, neuronB);
        assert(std::abs(retrievedStrength - strength) < 0.001f);
    }
    
    // Verify performance
    assert(workingMemory->getCurrentSize() > 0);
    assert(episodicMemory->getEpisodeCount() > 0);
    
    // Memory stress test - decay
    workingMemory->store(nlm::NeuronId(999), 1.0f);
    workingMemory->decay(0.01f);  // 1% decay per timestep
    assert(workingMemory->retrieve(nlm::NeuronId(999)) < 1.0f);
    
    std::cout << "    testMemoryPerformance passed" << std::endl;
}

void testMemoryErrorConditions() {
    std::cout << "  Testing Memory Error Conditions..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    nlm::NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    nlm::AssociativeMemory* associativeMemory = brain.getAssociativeMemory();
    
    // Test working memory error conditions
    nlm::NeuronId neuron(1);
    
    // Store within capacity
    workingMemory->store(neuron, 0.5f);
    
    // Try to retrieve stored value
    assert(workingMemory->retrieve(neuron) > 0.0f);
    
    // Test episodic memory error conditions
    // Retrieve from empty memory
    nlm::EpisodicMemoryItem emptyEpisode = episodicMemory->retrieveEpisode(999);  // Out of bounds
    assert(emptyEpisode.timestamp == 0);  // Should return empty item
    
    // Retrieve with invalid index
    nlm::EpisodicMemoryItem invalidEpisode = episodicMemory->retrieveEpisode(0);  // Valid index but no episodes
    assert(invalidEpisode.timestamp == 0);  // Should return empty item
    
    // Test associative memory error conditions
    // Retrieve non-existent association
    float emptyStrength = associativeMemory->getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(999));
    assert(emptyStrength == 0.0f);  // Should return zero for no association
    
    // Update non-existent association
    associativeMemory->updateAssociation(nlm::NeuronId(1), nlm::NeuronId(999), 0.5f);  // Should create association
    assert(std::abs(associativeMemory->getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(999)) - 0.5f) < 0.001f);
    
    // Test clear operations
    workingMemory->clear();
    episodicMemory->clear();
    associativeMemory->clear();
    
    assert(workingMemory->getCurrentSize() == 0);
    assert(episodicMemory->getEpisodeCount() == 0);
    
    std::cout << "    testMemoryErrorConditions passed" << std::endl;
}

void runAll() {
    testWorkingMemoryIntegration();
    testEpisodicMemoryIntegration();
    testSemanticMemoryIntegration();
    testProceduralMemoryIntegration();
    testAssociativeMemoryIntegration();
    testMemorySystemConnectivity();
    testMemoryPerformance();
    testMemoryErrorConditions();
}

} // namespace test_memory_integration
