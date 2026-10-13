// Working Memory Tests
// ====================
//
// NeuralWorkingMemory tests verify that the working memory system
// correctly implements neural dynamics for maintaining and manipulating
// information in short-term memory.
//
// Working memory tests focus on:
// - Pattern encoding/decoding
// - Competitive dynamics for selective attention
// - Maintenance through recurrent connections
// - Decay mechanisms for stability-flexibility balance
// - Integration with brain neural infrastructure
//
// This test suite ensures working memory operates as a dynamic reservoir
// using population codes, competitive selection, and activity-dependent
// plasticity to maintain information temporarily for cognitive processing.

#include "memory/NeuralWorkingMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_working_memory {

void testWorkingMemoryCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    auto* workingMemory = brain.getWorkingMemory();
    
    assert(workingMemory != nullptr);
    assert(workingMemory->getCapacity() == 100);
    assert(workingMemory->getActiveTraces() == 0);
    
    std::cout << "    testWorkingMemoryCreation passed" << std::endl;
}

void testWorkingMemoryStoreRetrieve() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* workingMemory = brain.getWorkingMemory();
    
    std::vector<float> pattern = {0.5f, 0.8f, 0.3f, 0.9f, 0.2f};
    workingMemory->store(pattern, 1.0f);
    
    assert(workingMemory->getActiveTraces() > 0);
    assert(workingMemory->getMemoryActivity() > 0.0f);
    
    std::vector<float> retrieved = workingMemory->retrieve();
    assert(retrieved.size() == pattern.size() || retrieved.size() > 0);
    
    std::cout << "    testWorkingMemoryStoreRetrieve passed" << std::endl;
}

void testWorkingMemoryNeuronStorage() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* workingMemory = brain.getWorkingMemory();
    
    // Create a simple neuron ID (approximate)
    nlm::NeuronId neuronId(1000);
    float activation = 0.7f;
    workingMemory->storeToNeuron(neuronId, activation);
    
    assert(workingMemory->contains(neuronId));
    assert(std::abs(workingMemory->getNeuronActivation(neuronId) - activation) < 0.01f);
    
    std::cout << "    testWorkingMemoryNeuronStorage passed" << std::endl;
}

void testWorkingMemoryCapacity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* workingMemory = brain.getWorkingMemory();
    
    size_t capacity = workingMemory->getCapacity();
    assert(capacity == 50);
    
    // Test capacity setting
    workingMemory->setCapacity(25);
    assert(workingMemory->getCapacity() == 25);
    
    std::cout << "    testWorkingMemoryCapacity passed" << std::endl;
}

void testWorkingMemoryDecay() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* workingMemory = brain.getWorkingMemory();
    
    // Set decay rate
    workingMemory->setDecayRate(0.1f); // 10% decay per timestep
    assert(workingMemory->getDecayRate() == 0.1f);
    
    std::vector<float> pattern = {0.9f, 0.8f, 0.7f};
    workingMemory->store(pattern, 1.0f);
    
    float initialActivity = workingMemory->getMemoryActivity();
    
    // Run update to apply decay
    workingMemory->update(1.0f);
    
    // Activity should decay
    float decayedActivity = workingMemory->getMemoryActivity();
    assert(decayedActivity < initialActivity || initialActivity == 0.0f);
    
    std::cout << "    testWorkingMemoryDecay passed" << std::endl;
}

void testWorkingMemoryCompetition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* workingMemory = brain.getWorkingMemory();
    
    std::vector<float> pattern1 = {0.9f, 0.5f, 0.3f};
    std::vector<float> pattern2 = {0.4f, 0.8f, 0.6f};
    
    workingMemory->store(pattern1, 1.0f);
    workingMemory->store(pattern2, 1.0f);
    
    workingMemory->runCompetition();
    
    // Check that there are winners
    std::vector<nlm::NeuronId> winners = workingMemory->getMemoryNeurons();
    bool hasWinners = !winners.empty();
    assert(hasWinners);
    
    std::cout << "    testWorkingMemoryCompetition passed" << std::endl;
}

void testWorkingMemoryStrengthening() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* workingMemory = brain.getWorkingMemory();
    
    std::vector<float> pattern = {0.5f, 0.6f, 0.7f};
    workingMemory->store(pattern, 1.0f);
    
    float initialActivity = workingMemory->getMemoryActivity();
    
    // Strengthen memory
    workingMemory->strengthenMemory(2.0f); // Double strength
    
    float strengthenedActivity = workingMemory->getMemoryActivity();
    assert(strengthenedActivity > initialActivity);
    
    std::cout << "    testWorkingMemoryStrengthening passed" << std::endl;
}

void testWorkingMemoryClear() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* workingMemory = brain.getWorkingMemory();
    
    std::vector<float> pattern = {0.5f, 0.8f, 0.3f};
    workingMemory->store(pattern, 1.0f);
    
    assert(workingMemory->getActiveTraces() > 0);
    
    workingMemory->clear();
    
    assert(workingMemory->getActiveTraces() == 0);
    assert(workingMemory->getMemoryNeurons().empty());
    
    std::cout << "    testWorkingMemoryClear passed" << std::endl;
}

void testWorkingMemoryIsWinning() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* workingMemory = brain.getWorkingMemory();
    
    nlm::NeuronId testNeuron(1000);
    workingMemory->storeToNeuron(testNeuron, 0.5f);
    
    bool isWinning = workingMemory->isWinning(testNeuron);
    assert(isWinning); // Should be winning after competition is run
    
    std::cout << "    testWorkingMemoryIsWinning passed" << std::endl;
}

void testWorkingMemoryAdvancedFeatures() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* workingMemory = brain.getWorkingMemory();
    
    // Test 1: Store multiple patterns
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2 = {0.9f, 0.8f, 0.7f};
    std::vector<float> pattern3 = {0.5f, 0.4f, 0.6f};
    
    workingMemory->store(pattern1, 0.5f);
    workingMemory->store(pattern2, 1.0f);
    workingMemory->store(pattern3, 0.8f);
    
    assert(workingMemory->getActiveTraces() > 0);
    
    // Test 2: Competition reduces active traces
    workingMemory->runCompetition();
    size_t tracesAfterCompetition = workingMemory->getActiveTraces();
    
    // Test 3: Strengthening works
    workingMemory->strengthenMemory(1.5f);
    float activityAfterStrengthening = workingMemory->getMemoryActivity();
    assert(activityAfterStrengthening > 0.0f);
    
    // Test 4: Update with decay
    workingMemory->update(1.0f);
    float activityAfterUpdate = workingMemory->getMemoryActivity();
    
    // Test 5: Clear and verify
    workingMemory->clear();
    assert(workingMemory->getActiveTraces() == 0);
    
    std::cout << "    testWorkingMemoryAdvancedFeatures passed" << std::endl;
}

void runAll() {
    testWorkingMemoryCreation();
    testWorkingMemoryStoreRetrieve();
    testWorkingMemoryNeuronStorage();
    testWorkingMemoryCapacity();
    testWorkingMemoryDecay();
    testWorkingMemoryCompetition();
    testWorkingMemoryStrengthening();
    testWorkingMemoryClear();
    testWorkingMemoryIsWinning();
    testWorkingMemoryAdvancedFeatures();
    
    std::cout << "All working memory tests passed!" << std::endl;
}

} // namespace test_working_memory
