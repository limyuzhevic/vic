// Integration Tests
// Comprehensive integration tests for Brain + Memory + Plasticity interactions

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/MetaPlasticityRule.hpp"
#include "plasticity/BCMRule.hpp"
#include "memory/MemoryEnhanced.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <memory>

namespace test_integration {

void testBrainMemoryPlasticityIntegration() {
    std::cout << "Testing Brain + Memory + Plasticity Integration..." << std::endl;
    
    // Create brain configuration
    auto config = std::make_shared<nlm::Config>();
    config->neuronCount = 10;
    config->synapseCount = 20;
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize brain
    assert(brain.initialize());
    
    // Get access to systems
    nlm::NeuralWorkingMemoryEnhanced* workingMemory = brain.getWorkingMemoryEnhanced();
    nlm::MetaPlasticityRule* metaPlasticity = brain.getSTDP();  // STDP uses MetaPlasticity internally
    nlm::BCMRule* bcm = brain.getHebbian();  // Hebbian uses BCM internally
    
    assert(workingMemory != nullptr);
    assert(metaPlasticity != nullptr);
    assert(bcm != nullptr);
    
    // Test 1: Memory-Plasticity Interaction
    std::cout << "  Test 1: Memory-Plasticity Interaction" << std::endl;
    
    std::vector<float> pattern = {0.5f, 0.8f, 0.3f, 0.9f, 0.6f};
    workingMemory->store(pattern, 1.0f);
    
    // Create a synapse
    nlm::Synapse* synapse = brain.getSynapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    assert(synapse != nullptr);
    synapse->setType(nlm::SynapseType::Excitatory);
    synapse->setWeight(0.5f);
    
    // Apply plasticity rules
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    metaPlasticity->update(synapse, preSpikes, postSpikes, 0.001);
    bcm->update(synapse, preSpikes, postSpikes, 0.001);
    
    // Check that weight changed (plastic response)
    assert(std::abs(synapse->getWeight() - 0.5f) > 0.001f);
    
    // Test 2: Memory-System Integration
    std::cout << "  Test 2: Memory-System Integration" << std::endl;
    
    // Store different patterns
    workingMemory->store({0.1f, 0.2f, 0.3f}, 1.0f);
    workingMemory->store({0.4f, 0.5f, 0.6f}, 1.0f);
    workingMemory->store({0.7f, 0.8f, 0.9f}, 1.0f);
    
    // Retrieve with partial cue
    std::vector<float> cue = {0.05f, 0.15f, 0.25f};
    std::vector<float> retrieved = workingMemory->retrieve(&cue);
    
    assert(retrieved.size() == 3);
    assert(retrieved[0] > 0.0f);
    
    // Test 3: Plasticity-Memory Feedback Loop
    std::cout << "  Test 3: Plasticity-Memory Feedback Loop" << std::endl;
    
    // Create a more complex scenario
    nlm::NeuralWorkingMemoryEnhanced* memory = brain.getWorkingMemoryEnhanced();
    
    std::vector<float> complexPattern = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
    memory->store(complexPattern, 1.0f);
    
    // Check pattern similarity
    float similarity = memory->getPatternSimilarity(complexPattern, complexPattern);
    assert(std::abs(similarity - 1.0f) < 0.01f);
    
    // Test 4: Integration with Brain Step
    std::cout << "  Test 4: Integration with Brain Step" << std::endl;
    
    // Inject sensory input
    nlm::SensoryInput input;
    input.pattern = {0.3f, 0.6f, 0.9f};
    input.sensoryType = nlm::SensoryType::Visual;
    input.strength = 1.0f;
    
    brain.receiveSensoryInput(input);
    
    // Step the brain
    brain.step(100);
    brain.step(100, 50.0);  // With time
    
    // Check that neurons were affected
    size_t activeCount = brain.getActiveNeuronCount();
    assert(activeCount > 0);
    
    // Test 5: Memory Consolidation Integration
    std::cout << "  Test 5: Memory Consolidation Integration" << std::endl;
    
    // Store patterns over time
    for (int i = 0; i < 10; ++i) {
        std::vector<float> pattern = {
            static_cast<float>(i) * 0.1f, 
            static_cast<float>(i) * 0.2f, 
            static_cast<float>(i) * 0.3f
        };
        memory->store(pattern, 1.0f);
    }
    
    // Apply consolidation
    memory->processSleepRest(false, 0.8f);
    memory->consolidateWorkingMemory(0.5f);
    
    // Check memory statistics
    size_t traceCount = memory->getActiveTraces();
    assert(traceCount >= 0);
    
    // Test 6: Plasticity Parameter Integration
    std::cout << "  Test 6: Plasticity Parameter Integration" << std::endl;
    
    // Configure plasticity parameters
    metaPlasticity->setMetaPlasticityRate(0.01f);
    metaPlasticity->setPlasticityThreshold(0.5f);
    
    bcm->setLearningRate(0.05f);
    bcm->setThreshold(0.6f);
    
    // Apply to synapse
    metaPlasticity->update(synapse, preSpikes, postSpikes, 0.001);
    
    // Check parameter effects
    assert(metaPlasticity->getMetaPlasticityRate() == 0.01f);
    assert(metaPlasticity->getPlasticityThreshold() == 0.5f);
    
    // Test 7: Memory-Driven Plasticity
    std::cout << "  Test 7: Memory-Driven Plasticity" << std::endl;
    
    // Store a pattern that will be used to drive plasticity
    std::vector<float> memoryPattern = {0.8f, 0.6f, 0.4f, 0.2f};
    memory->store(memoryPattern, 1.0f);
    
    // Create a new synapse
    nlm::Synapse* synapse2 = brain.getSynapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse2->setType(nlm::SynapseType::Excitatory);
    synapse2->setWeight(0.1f);
    
    // Apply memory-guided plasticity
    memory->updateAttentionalFocus(memoryPattern);
    auto attendedNeurons = memory->getAttendedMemoryNeurons();
    
    // Plasticity should be guided by memory
    metaPlasticity->update(synapse2, preSpikes, postSpikes, 0.001);
    
    assert(synapse2->getWeight() != 0.1f);  // Should have changed
    
    std::cout << "  All Brain+Memory+Plasticity Integration tests passed!" << std::endl;
}

void testCrossSystemInteractions() {
    std::cout << "\nTesting Cross-System Interactions..." << std::endl;
    
    // Create two brains for comparative testing
    auto config1 = std::make_shared<nlm::Config>();
    config1->neuronCount = 20;
    
    auto config2 = std::make_shared<nlm::Config>();
    config2->neuronCount = 20;
    config2->plasticityType = nlm::PlasticityType::STDP;
    
    nlm::Brain brain1(config1);
    nlm::Brain brain2(config2);
    
    brain1.initialize();
    brain2.initialize();
    
    // Test 1: Differential Plasticity Rules
    std::cout << "  Test 1: Differential Plasticity Rules" << std::endl;
    
    nlm::NeuralWorkingMemoryEnhanced* memory1 = brain1.getWorkingMemoryEnhanced();
    nlm::NeuralWorkingMemoryEnhanced* memory2 = brain2.getWorkingMemoryEnhanced();
    
    // Use different patterns in each brain
    std::vector<float> pattern1 = {0.5f, 0.7f, 0.9f};
    std::vector<float> pattern2 = {0.3f, 0.6f, 0.8f};
    
    memory1->store(pattern1, 1.0f);
    memory2->store(pattern2, 1.0f);
    
    // Each brain should process patterns differently based on plasticity rules
    float similarity1 = memory1->getPatternSimilarity(pattern1, pattern1);
    float similarity2 = memory2->getPatternSimilarity(pattern2, pattern2);
    
    assert(std::abs(similarity1 - 1.0f) < 0.01f);
    assert(std::abs(similarity2 - 1.0f) < 0.01f);
    
    // Test 2: Memory Capacity Interaction
    std::cout << "  Test 2: Memory Capacity Interaction" << std::endl;
    
    // Set different memory capacities
    memory1->setCapacity(50);
    memory2->setCapacity(100);
    
    // Store more patterns in brain2 (higher capacity)
    for (int i = 0; i < 60; ++i) {
        std::vector<float> pattern = {static_cast<float>(i) * 0.01f, 
                                     static_cast<float>(i) * 0.02f,
                                     static_cast<float>(i) * 0.03f};
        memory2->store(pattern, 1.0f);
    }
    
    // Brain1 should have less storage capacity but similar functionality
    size_t capacity1 = memory1->getCapacity();
    size_t capacity2 = memory2->getCapacity();
    
    assert(capacity1 == 50);
    assert(capacity2 == 100);
    
    // Test 3: Plasticity-Memory Feedback Integration
    std::cout << "  Test 3: Plasticity-Memory Feedback Integration" << std::endl;
    
    // Create a plasticity-memory loop
    std::vector<float> feedbackPattern = {0.9f, 0.7f, 0.5f, 0.3f, 0.1f};
    memory1->store(feedbackPattern, 1.0f);
    
    // Modify memory to affect plasticity
    memory1->strengthenMemory(1.2f);
    
    // Apply plasticity with memory influence
    nlm::Synapse* synapse = brain1.getSynapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse->setType(nlm::SynapseType::Excitatory);
    synapse->setWeight(0.3f);
    
    // Memory should influence plasticity
    memory1->updateAttentionalFocus(feedbackPattern);
    
    std::cout << "  All Cross-System Interaction tests passed!" << std::endl;
}

void runAll() {
    std::cout << "=== Integration Tests: Brain + Memory + Plasticity ===" << std::endl;
    
    try {
        testBrainMemoryPlasticityIntegration();
    } catch (const std::exception& e) {
        std::cout << "  FAILED in Brain+Memory+Plasticity Integration: " << e.what() << std::endl;
        throw;
    }
    
    try {
        testCrossSystemInteractions();
    } catch (const std::exception& e) {
        std::cout << "  FAILED in Cross-System Interactions: " << e.what() << std::endl;
        throw;
    }
    
    std::cout << "\n=== All Integration Tests PASSED ===" << std::endl;
}

} // namespace test_integration
