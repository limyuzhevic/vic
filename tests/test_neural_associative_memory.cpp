// NeuralAssociativeMemory Tests
// Tests comprehensive associative memory functionality including:
// - Pattern association formation
// - Similarity-based retrieval
// - Association strength tracking
// - Spreading activation
// - Integration with episodic memory
// - Hebbian learning mechanisms

#include "memory/NeuralAssociativeMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_neural_associative_memory {

void testAssociativeMemoryCreation() {
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    brain.initialize();
    
    nlm::NeuralAssociativeMemory memory;
    memory.initialize(&brain);
    
    // Initial state should be empty
    assert(memory.getAssociationCount() == 0);
    
    std::cout << "    testAssociativeMemoryCreation passed" << std::endl;
}

void testAssociativeMemoryBasicAssociation() {
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    brain.initialize();
    
    nlm::NeuralAssociativeMemory memory;
    memory.initialize(&brain);
    
    // Create two patterns
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> patternB = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
    
    // Create association
    memory.associate(patternA, patternB, 1.0f);
    
    // Verify association was created
    assert(memory.getAssociationCount() == 1);
    
    // Retrieve associated pattern
    std::vector<std::vector<float>> retrieved = memory.retrieve(patternA, 5);
    assert(!retrieved.empty());
    assert(retrieved[0] == patternB);
    
    std::cout << "    testAssociativeMemoryBasicAssociation passed" << std::endl;
}

void testAssociativeMemoryMultipleAssociations() {
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    brain.initialize();
    
    nlm::NeuralAssociativeMemory memory;
    memory.initialize(&brain);
    
    // Create multiple patterns
    std::vector<std::vector<float>> patterns = {
        {0.1f, 0.2f, 0.3f},
        {0.4f, 0.5f, 0.6f},
        {0.7f, 0.8f, 0.9f},
        {0.2f, 0.4f, 0.6f},
        {0.3f, 0.5f, 0.7f}
    };
    
    // Create associations between related patterns
    memory.associate(patterns[0], patterns[1], 0.8f);
    memory.associate(patterns[0], patterns[2], 0.6f);
    memory.associate(patterns[1], patterns[3], 0.9f);
    memory.associate(patterns[2], patterns[4], 0.7f);
    
    // Verify associations
    assert(memory.getAssociationCount() == 4);
    
    // Test retrieval
    std::vector<std::vector<float>> retrieved1 = memory.retrieve(patterns[0], 3);
    assert(retrieved1.size() >= 2);  // Should retrieve at least patterns[1] and patterns[2]
    
    std::vector<std::vector<float>> retrieved2 = memory.retrieve(patterns[1], 2);
    assert(retrieved2.size() >= 2);  // Should retrieve patterns[0] and patterns[3]
    
    std::cout << "    testAssociativeMemoryMultipleAssociations passed" << std::endl;
}

void testAssociativeMemoryAssociationStrength() {
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    brain.initialize();
    
    nlm::NeuralAssociativeMemory memory;
    memory.initialize(&brain);
    
    // Create patterns
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f};
    std::vector<float> patternB = {0.4f, 0.5f, 0.6f};
    
    // Create association with different strengths
    memory.associate(patternA, patternB, 0.3f);
    assert(memory.getAssociationCount() == 1);
    
    // Update association strength
    memory.updateAssociation(patternA, patternB, 0.5f);
    
    // Verify strength changed
    float strength = memory.getAssociationStrength(patternA, patternB);
    assert(std::abs(strength - 0.5f) < 0.01f);
    
    std::cout << "    testAssociativeMemoryAssociationStrength passed" << std::endl;
}

void testAssociativeMemorySimilarity() {
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    brain.initialize();
    
    nlm::NeuralAssociativeMemory memory;
    memory.initialize(&brain);
    
    // Create patterns
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> patternB = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};  // Similar to A
    std::vector<float> patternC = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};  // Different from A
    
    // Create associations
    memory.associate(patternA, patternB, 1.0f);
    memory.associate(patternA, patternC, 1.0f);
    
    // Retrieve similar patterns to A
    std::vector<std::vector<float>> retrieved = memory.retrieve(patternA, 5);
    
    // Should retrieve both B and C
    bool foundB = false, foundC = false;
    for (const auto& pat : retrieved) {
        if (pat == patternB) foundB = true;
        if (pat == patternC) foundC = true;
    }
    
    assert(foundB && foundC);
    
    std::cout << "    testAssociativeMemorySimilarity passed" << std::endl;
}

void testAssociativeMemorySpreadingActivation() {
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    brain.initialize();
    
    nlm::NeuralAssociativeMemory memory;
    memory.initialize(&brain);
    
    // Create pattern chains
    std::vector<float> cue = {0.1f, 0.2f, 0.3f};
    std::vector<float> target1 = {0.4f, 0.5f, 0.6f};
    std::vector<float> target2 = {0.7f, 0.8f, 0.9f};
    std::vector<float> target3 = {0.8f, 0.7f, 0.6f};  // Related to target2
    
    // Create associations
    memory.associate(cue, target1, 1.0f);
    memory.associate(target1, target2, 1.0f);
    memory.associate(target2, target3, 0.8f);
    
    // Spread activation from cue
    std::vector<std::vector<float>> spread = memory.spreadActivation(cue, 3);
    
    // Should get all related patterns
    bool foundTarget1 = false, foundTarget2 = false, foundTarget3 = false;
    for (const auto& pat : spread) {
        if (pat == target1) foundTarget1 = true;
        if (pat == target2) foundTarget2 = true;
        if (pat == target3) foundTarget3 = true;
    }
    
    assert(foundTarget1 && foundTarget2 && foundTarget3);
    
    std::cout << "    testAssociativeMemorySpreadingActivation passed" << std::endl;
}

void testAssociativeMemoryFromEpisodic() {
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    brain.initialize();
    
    nlm::NeuralAssociativeMemory memory;
    memory.initialize(&brain);
    
    // Create episodic memory items
    nlm::EpisodicMemoryItem episode1;
    episode1.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    episode1.activeNeurons = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    episode1.neuronActivations = {0.8f, 0.6f, 0.4f};
    
    nlm::EpisodicMemoryItem episode2;
    episode2.sensoryState = {0.4f, 0.5f, 0.6f, 0.7f, 0.8f};
    episode2.activeNeurons = {nlm::NeuronId(4), nlm::NeuronId(5), nlm::NeuronId(6)};
    episode2.neuronActivations = {0.5f, 0.7f, 0.9f};
    
    // Create associations from episodic memory
    memory.associateFromExperience(episode1);
    memory.associateFromExperience(episode2);
    
    // Verify associations were created
    assert(memory.getAssociationCount() > 0);
    
    // Retrieve associations
    std::vector<std::vector<float>> retrieved1 = memory.retrieve(episode1.sensoryState, 5);
    std::vector<std::vector<float>> retrieved2 = memory.retrieve(episode2.sensoryState, 5);
    
    assert(!retrieved1.empty());
    assert(!retrieved2.empty());
    
    std::cout << "    testAssociativeMemoryFromEpisodic passed" << std::endl;
}

void testAssociativeMemoryClear() {
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    brain.initialize();
    
    nlm::NeuralAssociativeMemory memory;
    memory.initialize(&brain);
    
    // Create some associations
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f};
    std::vector<float> patternB = {0.4f, 0.5f, 0.6f};
    
    memory.associate(patternA, patternB, 1.0f);
    assert(memory.getAssociationCount() == 1);
    
    // Clear associations
    memory.clear();
    
    assert(memory.getAssociationCount() == 0);
    
    std::cout << "    testAssociativeMemoryClear passed" << std::endl;
}

void runAll() {
    testAssociativeMemoryCreation();
    testAssociativeMemoryBasicAssociation();
    testAssociativeMemoryMultipleAssociations();
    testAssociativeMemoryAssociationStrength();
    testAssociativeMemorySimilarity();
    testAssociativeMemorySpreadingActivation();
    testAssociativeMemoryFromEpisodic();
    testAssociativeMemoryClear();
}

} // namespace test_neural_associative_memory
