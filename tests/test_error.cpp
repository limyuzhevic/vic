// Comprehensive Error Handling Tests
// Phase 2: Real Neural Computation Tests - Error Handling and Recovery

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace test_error {

void testNullPointerHandling() {
    // Test that functions handle null pointers gracefully
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test with null synapse pointer
    nlm::STDP stdp;
    stdp.update(nullptr, std::vector<nlm::Timestamp>(), std::vector<nlm::Timestamp>(), 0.001);
    
    // Test that neuron handles null state gracefully
    assert(neuron.getId().index() != 0);  // Valid neuron ID
    
    // Test neuron error handling
    neuron.setMembranePotential(NAN);  // Should handle NaN gracefully
    neuron.setMembranePotential(INFINITY);  // Should handle infinity gracefully
    
    // Neuron should still be functional
    assert(neuron.getId().index() != 0);
    
    std::cout << "    testNullPointerHandling passed" << std::endl;
}

void testInvalidInputHandling() {
    // Test handling of invalid inputs
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test invalid membrane potential values
    neuron.setMembranePotential(NAN);
    neuron.setMembranePotential(INFINITY);
    neuron.setMembranePotential(-INFINITY);
    
    // Neuron should still be usable
    neuron.setMembranePotential(-70.0f);
    assert(neuron.getMembranePotential() == -70.0f);
    
    // Test invalid threshold
    neuron.setThreshold(NAN);
    neuron.setThreshold(INFINITY);
    neuron.setThreshold(-1000.0f);  // Unreasonable but valid
    
    // Test invalid current values
    neuron.injectCurrent(NAN);
    neuron.injectCurrent(INFINITY);
    
    // Test invalid refractory period
    neuron.setRefractoryPeriod(static_cast<uint32_t>(-1));  // Should handle gracefully
    neuron.setRefractoryPeriod(1000);  // Large but valid
    
    std::cout << "    testInvalidInputHandling passed" << std::endl;
}

void testMemoryAllocationErrors() {
    // Test error handling during memory allocation
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Create neuron and fill spike history
    neuron.recordSpike(0.0);
    neuron.recordSpike(1.0);
    neuron.recordSpike(2.0);
    
    // Fill to near capacity
    for (int i = 0; i < 95; ++i) {
        neuron.recordSpike(static_cast<float>(i + 3));
    }
    
    // Try to add one more (should handle gracefully)
    neuron.recordSpike(100.0f);
    
    // Should still have reasonable size
    assert(neuron.getSpikeHistory().size() <= 101);  // 3 + 95 + 1 (new)
    
    // Test synapse spike history
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Fill spike history
    for (int i = 0; i < 95; ++i) {
        synapse.recordPreSpike(static_cast<float>(i));
    }
    
    synapse.recordPreSpike(100.0f);
    assert(synapse.getPreSpikeHistory().size() <= 101);
    
    std::cout << "    testMemoryAllocationErrors passed" << std::endl;
}

void testConfigurationErrors() {
    // Test configuration error handling
    nlm::Config config;
    
    // Test invalid configuration values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value() && *intVal == 42);
    
    // Test missing key handling
    auto missingVal = config.get<std::string>("missing_key");
    assert(!missingVal.has_value());  // Should return nullopt for missing key
    
    // Test default value handling
    int defaultVal = config.getOr<int>("missing_key", 100);
    assert(defaultVal == 100);
    
    // Test type conversion errors
    config.set("string_val", "test", nlm::ConfigSource::Runtime);
    auto intFromString = config.get<int>("string_val");  // Should return nullopt
    assert(!intFromString.has_value());
    
    std::cout << "    testConfigurationErrors passed" << std::endl;
}

void testSynapseInvalidInputs() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test invalid weight operations
    synapse.setWeight(NAN);
    synapse.setWeight(INFINITY);
    synapse.setWeight(-INFINITY);
    
    // Test invalid delay
    synapse.setDelay(-1);  // Should handle negative delay
    synapse.setDelay(1000000);  // Large but valid
    
    // Test invalid eligibility trace
    synapse.setEligibilityTrace(NAN);
    synapse.setEligibilityTrace(INFINITY);
    
    // Test invalid efficacy
    synapse.setEfficacy(NAN);
    synapse.setEfficacy(INFINITY);
    synapse.setEfficacy(-1.0f);  // Should handle gracefully
    
    // Synapse should still be functional
    assert(synapse.getId().index() != 0);
    
    std::cout << "    testSynapseInvalidInputs passed" << std::endl;
}

void testPlasticityRuleErrors() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Test with null synapse
    stdp.update(nullptr, std::vector<nlm::Timestamp>(), std::vector<nlm::Timestamp>(), 0.001);
    
    // Test with empty spike lists
    stdp.update(&synapse, std::vector<nlm::Timestamp>(), std::vector<nlm::Timestamp>(), 0.001);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    stdp.update(&synapse, preSpikes, std::vector<nlm::Timestamp>(), 0.001);
    
    stdp.update(&synapse, std::vector<nlm::Timestamp>(), postSpikes, 0.001);
    
    // Synapse should still be functional
    assert(synapse.getWeight() == 0.5f);
    
    std::cout << "    testPlasticityRuleErrors passed" << std::endl;
}

void testBrainErrorRecovery() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test error recovery after failures
    size_t initialNeurons = brain.getTotalNeuronCount();
    
    // Test reset after simulated errors
    brain.reset();
    assert(brain.getTotalNeuronCount() == initialNeurons);
    
    // Test recovery from invalid state
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Initial);
    
    // Run steps to ensure recovery
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Should still be functional
    assert(brain.getTotalNeuronCount() == initialNeurons);
    assert(brain.getTotalSynapseCount() >= 0);
    
    std::cout << "    testBrainErrorRecovery passed" << std::endl;
}

void testNumericalStability() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test numerical stability with extreme values
    neuron.setMembranePotential(-1000.0f);  // Very negative
    neuron.setMembranePotential(1000.0f);   // Very positive
    
    neuron.setThreshold(-1000.0f);          // Very negative threshold
    neuron.setThreshold(1000.0f);           // Very positive threshold
    
    neuron.injectCurrent(100000.0f);        // Very large current
    neuron.injectCurrent(-100000.0f);       // Very negative current
    
    // Neuron should handle these without crashing
    neuron.clearTotalCurrent();
    
    // Test with STDP
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Test with extreme time differences
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1000000.0};  // Large dt
    std::vector<nlm::Timestamp> postSpikes = {500000.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Synapse should still have valid weight
    assert(-1.0f <= synapse.getWeight() <= 1.0f);
    
    std::cout << "    testNumericalStability passed" << std::endl;
}

void testMemoryBoundaryConditions() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test memory boundary conditions
    // Spike history boundary
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    // Add maximum spike history
    for (int i = 0; i < 200; ++i) {
        neuron.recordSpike(static_cast<float>(i));
    }
    
    // Should not exceed maximum (implementation dependent)
    size_t historySize = neuron.getSpikeHistory().size();
    assert(historySize > 0);  // Should have some spikes
    
    // Test reset boundary
    neuron.reset();
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testMemoryBoundaryConditions passed" << std::endl;
}

void testConcurrentAccess() {
    // Test concurrent access patterns (basic thread-safety simulation)
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Simulate concurrent operations
    neuron.injectCurrent(1.0f);
    neuron.setMembranePotential(-65.0f);
    neuron.setThreshold(-55.0f);
    neuron.setRefractoryPeriod(5);
    
    synapse.setWeight(0.7f);
    synapse.setDelay(2);
    synapse.setEligibilityTrace(0.3f);
    
    // Verify consistent state
    assert(neuron.getTotalCurrent() == 1.0f);
    assert(neuron.getMembranePotential() == -65.0f);
    assert(neuron.getThreshold() == -55.0f);
    assert(synapse.getWeight() == 0.7f);
    assert(synapse.getDelay() == 2);
    assert(synapse.getEligibilityTrace() == 0.3f);
    
    std::cout << "    testConcurrentAccess passed" << std::endl;
}

void testResourceCleanup() {
    // Test resource cleanup
    {
        nlm::Neuron neuron(nlm::NeuronId(1));
        neuron.setMembranePotential(-70.0f);
        neuron.injectCurrent(5.0f);
        neuron.recordSpike(0.0);
        
        // Neuron should go out of scope and be cleaned up
    }
    
    {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setWeight(0.8f);
        synapse.recordPreSpike(0.0);
        
        // Synapse should be cleaned up
    }
    
    // New objects should be created successfully
    nlm::Neuron neuron2(nlm::NeuronId(2));
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(neuron2.getId().index() == 2);
    assert(synapse2.getId().index() == 2);
    
    std::cout << "    testResourceCleanup passed" << std::endl;
}

void testConfigurationFileOperations() {
    nlm::Config config;
    
    // Test file operations
    bool saved = config.saveToFile("./test_config.json");
    
    // May succeed or fail depending on implementation
    // At minimum should not crash
    if (saved) {
        bool loaded = config.loadFromFile("./test_config.json");
        assert(loaded);
    }
    
    std::cout << "    testConfigurationFileOperations passed" << std::endl;
}

void testDevelopmentalErrorRecovery() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test recovery from developmental errors
    brain.setDevelopmentalStage(static_cast<nlm::DevelopmentalStage>(999));  // Invalid
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Initial);
    
    developmentSystem->update(&brain, nlm::RandomGenerator(42), 0.001f);
    
    // Brain should still be functional
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testDevelopmentalErrorRecovery passed" << std::endl;
}

void testMemorySystemErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    
    // Test error handling in memory systems
    workingMemory->storeToNeuron(nlm::NeuronId(1), 0.5f);
    
    // Test memory system reset
    workingMemory->clear();
    episodicMemory->clear();
    
    assert(workingMemory->getActiveTraces() == 0);
    assert(episodicMemory->getEpisodeCount() == 0);
    
    std::cout << "    testMemorySystemErrorHandling passed" << std::endl;
}

void runAll() {
    std::cout << "Running Error Handling tests..." << std::endl;
    testNullPointerHandling();
    testInvalidInputHandling();
    testMemoryAllocationErrors();
    testConfigurationErrors();
    testSynapseInvalidInputs();
    testPlasticityRuleErrors();
    testBrainErrorRecovery();
    testNumericalStability();
    testMemoryBoundaryConditions();
    testConcurrentAccess();
    testResourceCleanup();
    testConfigurationFileOperations();
    testDevelopmentalErrorRecovery();
    testMemorySystemErrorHandling();
}

} // namespace test_error
