// NeuralWorkingMemory Tests
#include "memory/NeuralWorkingMemory.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_neural_working_memory {

void testMemoryCreation() {
    nlm::NeuralWorkingMemory memory;
    
    assert(memory.getCapacity() == 100);  // Default capacity
    assert(memory.getDecayRate() == 0.01f);  // Default decay rate
    assert(memory.getActiveTraces() == 0);
    
    std::cout << "    testMemoryCreation passed" << std::endl;
}

void testMemoryInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    
    assert(memory.getMemoryNeurons().empty());
    assert(memory.getActiveTraces() == 0);
    
    std::cout << "    testMemoryInitialization passed" << std::endl;
}

void testStoreRetrieve() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    
    // Store a pattern
    std::vector<float> pattern = {0.5f, 0.8f, 0.3f, 0.9f, 0.2f};
    memory.store(pattern, 1.0f);
    
    // Retrieve should return the pattern (or at least something)
    std::vector<float> retrieved = memory.retrieve();
    assert(!retrieved.empty());
    assert(retrieved.size() == pattern.size() || retrieved.size() == memory.getMemoryNeurons().size());
    
    std::cout << "    testStoreRetrieve passed" << std::endl;
}

void testStoreToNeuron() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    
    // Test storing to a specific neuron
    nlm::NeuronId testNeuron = nlm::NeuronId(1001);
    memory.storeToNeuron(testNeuron, 0.7f);
    
    assert(memory.contains(testNeuron));
    assert(memory.getNeuronActivation(testNeuron) == 0.7f);
    assert(memory.isWinning(testNeuron) == false);  // Not enough activation to win
    
    std::cout << "    testStoreToNeuron passed" << std::endl;
}

void testMemoryContains() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    
    nlm::NeuronId testNeuron = nlm::NeuronId(2001);
    
    assert(!memory.contains(testNeuron));
    
    memory.storeToNeuron(testNeuron, 0.5f);
    assert(memory.contains(testNeuron));
    
    std::cout << "    testMemoryContains passed" << std::endl;
}

void testMemoryCapacity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    
    // Change capacity
    memory.setCapacity(50);
    assert(memory.getCapacity() == 50);
    
    // Store more neurons than capacity
    for (int i = 0; i < 60; ++i) {
        nlm::NeuronId neuron = nlm::NeuronId(3000 + i);
        memory.storeToNeuron(neuron, 0.5f);
    }
    
    // Should still have the right number of neurons (or at least not exceed capacity significantly)
    assert(memory.getMemoryNeurons().size() <= memory.getCapacity());
    
    std::cout << "    testMemoryCapacity passed" << std::endl;
}

void testMemoryDecay() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    
    // Set high decay rate
    memory.setDecayRate(0.1f);
    assert(memory.getDecayRate() == 0.1f);
    
    // Store some neurons
    for (int i = 0; i < 10; ++i) {
        nlm::NeuronId neuron = nlm::NeuronId(4000 + i);
        memory.storeToNeuron(neuron, 1.0f);
    }
    
    // Get current activations
    std::vector<float> activations;
    for (size_t i = 0; i < memory.getMemoryNeurons().size(); ++i) {
        activations.push_back(memory.getNeuronActivation(memory.getMemoryNeurons()[i]));
    }
    
    // Update memory (should decay)
    memory.update(1); // 1 timestep
    
    // Activations should have decreased due to decay
    for (size_t i = 0; i < activations.size(); ++i) {
        assert(memory.getNeuronActivation(memory.getMemoryNeurons()[i]) <= activations[i]);
    }
    
    std::cout << "    testMemoryDecay passed" << std::endl;
}

void testMemoryCompetition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    
    // Store multiple neurons with different activations
    nlm::NeuronId winner1 = nlm::NeuronId(5001);
    nlm::NeuronId winner2 = nlm::NeuronId(5002);
    nlm::NeuronId loser1 = nlm::NeuronId(5003);
    nlm::NeuronId loser2 = nlm::NeuronId(5004);
    
    memory.storeToNeuron(winner1, 0.8f);  // High activation
    memory.storeToNeuron(winner2, 0.7f);  // High activation
    memory.storeToNeuron(loser1, 0.2f);   // Low activation
    memory.storeToNeuron(loser2, 0.1f);   // Low activation
    
    // Run competition
    memory.runCompetition();
    
    // Winners should have higher activation than losers
    assert(memory.isWinning(winner1) || memory.isWinning(winner2));
    assert(!memory.isWinning(loser1));
    assert(!memory.isWinning(loser2));
    
    std::cout << "    testMemoryCompetition passed" << std::endl;
}

void testMemoryStrengthening() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    
    // Store neurons with low activation
    for (int i = 0; i < 5; ++i) {
        nlm::NeuronId neuron = nlm::NeuronId(6000 + i);
        memory.storeToNeuron(neuron, 0.3f);
    }
    
    // Get current activations
    std::vector<float> beforeActivations;
    for (const auto& neuron : memory.getMemoryNeurons()) {
        beforeActivations.push_back(memory.getNeuronActivation(neuron));
    }
    
    // Strengthen memory
    memory.strengthenMemory(2.0f);  // Double activations
    
    // Activations should have increased (but not exceed 1.0)
    for (size_t i = 0; i < memory.getMemoryNeurons().size(); ++i) {
        float current = memory.getNeuronActivation(memory.getMemoryNeurons()[i]);
        assert(current >= beforeActivations[i]);
        assert(current <= 1.0f);  // Should not exceed 1.0 due to min(1.0f, ...)
    }
    
    std::cout << "    testMemoryStrengthening passed" << std::endl;
}

void testMemoryActivity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    
    assert(memory.getMemoryActivity() == 0.0f);  // Empty memory has 0 activity
    
    // Store some neurons
    for (int i = 0; i < 10; ++i) {
        nlm::NeuronId neuron = nlm::NeuronId(7000 + i);
        memory.storeToNeuron(neuron, 0.5f);
    }
    
    float activity = memory.getMemoryActivity();
    assert(activity > 0.0f);  // Should have positive activity
    
    std::cout << "    testMemoryActivity passed" << std::endl;
}

void testMemoryClear() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    
    // Store some neurons
    for (int i = 0; i < 5; ++i) {
        nlm::NeuronId neuron = nlm::NeuronId(8000 + i);
        memory.storeToNeuron(neuron, 0.5f);
    }
    
    assert(memory.getActiveTraces() > 0);
    assert(!memory.getMemoryNeurons().empty());
    
    // Clear memory
    memory.clear();
    
    assert(memory.getMemoryNeurons().empty());
    assert(memory.getActiveTraces() == 0);
    
    std::cout << "    testMemoryClear passed" << std::endl;
}

void runAll() {
    testMemoryCreation();
    testMemoryInitialization();
    testStoreRetrieve();
    testStoreToNeuron();
    testMemoryContains();
    testMemoryCapacity();
    testMemoryDecay();
    testMemoryCompetition();
    testMemoryStrengthening();
    testMemoryActivity();
    testMemoryClear();
}

} // namespace test_neural_working_memory
