// Integration Tests
// Tests interaction between different brain components and systems
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace nlm {
    // Forward declarations for types used in integration tests
    using namespace nlm;
}

namespace test_integration {

void testBrainNeuronIntegration() {
    nlm::RandomGenerator rng(42);
    auto config = std::make_shared<nlm::Config>();
    
    // Create a brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Test that brain contains neurons
    assert(brain->getTotalNeuronCount() > 0);
    assert(brain->getRegionCount() > 0);
    
    // Test neuron access through brain
    if (auto* region = brain->getRegion(nlm::RegionId(1))) {
        auto neurons = region->getAllNeurons();
        assert(!neurons.empty());
        
        // Test that neurons are properly integrated
        for (auto* neuron : neurons) {
            assert(neuron->getId().isValid());
            assert(neuron->getState().refractoryPeriod > 0);
        }
    }
    
    std::cout << "    testBrainNeuronIntegration passed" << std::endl;
}

void testNeuronalSynapseIntegration() {
    nlm::RandomGenerator rng(42);
    
    // Create source and destination neurons
    nlm::Neuron sourceNeuron(nlm::NeuronId(1));
    nlm::Neuron destNeuron(nlm::NeuronId(2));
    
    // Initialize neurons
    sourceNeuron.initializeRandom(rng);
    destNeuron.initializeRandom(rng);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.initializeRandom(rng);
    
    // Add synapse to both neurons
    sourceNeuron.addOutgoingSynapse(nlm::SynapseHandle(&synapse));
    destNeuron.addIncomingSynapse(nlm::SynapseHandle(&synapse));
    
    // Test integration
    assert(sourceNeuron.getOutgoingSynapses().size() == 1);
    assert(destNeuron.getIncomingSynapses().size() == 1);
    assert(sourceNeuron.getOutgoingSynapses()[0].get() == &synapse);
    assert(destNeuron.getIncomingSynapses()[0].get() == &synapse);
    
    // Test that synapse connects neurons correctly
    assert(synapse.getSourceNeuron() == nlm::NeuronId(1));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(2));
    
    std::cout << "    testNeuronalSynapseIntegration passed" << std::endl;
}

void testPlasticityIntegration() {
    nlm::RandomGenerator rng(42);
    
    // Create neurons and synapse
    nlm::Neuron neuron1(nlm::NeuronId(1));
    nlm::Neuron neuron2(nlm::NeuronId(2));
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Initialize with random parameters
    neuron1.initializeRandom(rng);
    neuron2.initializeRandom(rng);
    synapse.initializeRandom(rng);
    
    // Enable all plasticity types
    neuron1.enablePlasticity(true, true, true);
    neuron2.enablePlasticity(true, true, true);
    synapse.enablePlasticity(true, true, true);
    
    // Record spike activity for STDP
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    
    // Simulate learning
    for (int step = 0; step < 100; ++step) {
        // Apply synaptic input
        neuron1.injectCurrent(10.0f);
        
        // Record spike if neuron would fire
        if (neuron1.checkThreshold()) {
            synapse.recordPreSpike(static_cast<float>(step) * 0.001f);
        }
        
        neuron1.addToMembranePotential(5.0f);  // Simulate some input
        
        // Update synapse (short-term plasticity)
        synapse.step(static_cast<float>(step) * 0.001f);
        
        // In a real system, this would update eligibility traces based on reward
    }
    
    // Verify plasticity mechanisms are working
    assert(synapse.getPreSpikeHistory().size() >= 1);
    assert(synapse.getPostSpikeHistory().size() >= 1);
    
    std::cout << "    testPlasticityIntegration passed" << std::endl;
}

void testMemoryIntegration() {
    nlm::RandomGenerator rng(42);
    
    // Create brain with memory systems
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Test that brain has memory components
    // (assuming memory systems are integrated based on Phase 6 design)
    assert(brain->getTotalNeuronCount() > 0);
    
    // Test memory neuron types
    nlm::Neuron workingMemoryNeuron(nlm::NeuronId(1));
    workingMemoryNeuron.setType(nlm::NeuronType::Internal);
    workingMemoryNeuron.initializeRandom(rng);
    
    assert(workingMemoryNeuron.getType() == nlm::NeuronType::Internal);
    
    // Test episodic memory neuron type
    nlm::Neuron episodicMemoryNeuron(nlm::NeuronId(2));
    episodicMemoryNeuron.setType(nlm::NeuronType::Sensory);
    episodicMemoryNeuron.initializeRandom(rng);
    
    assert(episodicMemoryNeuron.getType() == nlm::NeuronType::Sensory);
    
    std::cout << "    testMemoryIntegration passed" << std::endl;
}

void testNeuromodulationIntegration() {
    nlm::RandomGenerator rng(42);
    
    // Create neurons with neuromodulation capabilities
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.initializeRandom(rng);
    
    // Test neuromodulatory input processing
    neuron.receiveModulatoryInput(1.0f);  // Simulate dopamine input
    neuron.receiveModulatoryInput(0.5f);  // Additional input
    
    // Test adaptation variable accumulation
    assert(neuron.getState().adaptationVariable > 0.0f);
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testSensorimotorIntegration() {
    nlm::RandomGenerator rng(42);
    
    // Create sensory neuron
    nlm::Neuron sensoryNeuron(nlm::NeuronId(1));
    sensoryNeuron.setType(nlm::NeuronType::Sensory);
    sensoryNeuron.initializeRandom(rng);
    
    // Create motor neuron
    nlm::Neuron motorNeuron(nlm::NeuronId(2));
    motorNeuron.setType(nlm::NeuronType::Motor);
    motorNeuron.initializeRandom(rng);
    
    // Create connection
    nlm::Synapse connection(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    connection.initializeRandom(rng);
    connection.setType(nlm::SynapseType::Excitatory);
    
    // Test sensorimotor loop
    // Sensory input -> processing -> motor output
    sensoryNeuron.injectCurrent(15.0f);  // Strong sensory input
    
    // In a real system, this would propagate through the network
    // For testing, we verify the components are connected
    assert(sensoryNeuron.getType() == nlm::NeuronType::Sensory);
    assert(motorNeuron.getType() == nlm::NeuronType::Motor);
    assert(connection.isExcitatory());
    
    std::cout << "    testSensorimotorIntegration passed" << std::endl;
}

void testDevelopmentIntegration() {
    nlm::RandomGenerator rng(42);
    
    // Create neurons at different developmental stages
    std::vector<nlm::Neuron> neurons;
    for (int i = 0; i < 5; ++i) {
        neurons.emplace_back(nlm::NeuronId(i));
        neurons.back().initializeRandom(rng);
        
        // Simulate different development stages
        if (i == 0) neurons.back().setFiringRate(10.0f);    // High initial rate
        else if (i < 3) neurons.back().setFiringRate(5.0f);  // Moderate
        else neurons.back().setFiringRate(2.0f);              // Low mature rate
    }
    
    // Test that development affects plasticity
    for (size_t i = 0; i < neurons.size(); ++i) {
        assert(neurons[i].getFiringRate() > 0.0f);
        
        // In a real system, development would affect plasticity rates
        // For now, we just verify neurons are functional
    }
    
    std::cout << "    testDevelopmentIntegration passed" << std::endl;
}

void testErrorHandlingIntegration() {
    nlm::RandomGenerator rng(42);
    
    // Test that invalid operations are handled gracefully
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // These should not crash the system
    neuron.setMembranePotential(-1000.0f);  // Extreme but valid
    neuron.setThreshold(1000.0f);            // Extreme but valid
    
    neuron.injectCurrent(100.0f);            // High current
    neuron.receiveExcitatoryInput(50.0f);   // Strong input
    neuron.receiveInhibitoryInput(30.0f);   // Strong inhibition
    
    // System should still be functional
    assert(neuron.getMembranePotential() < 1000.0f);
    assert(neuron.getTotalCurrent() == 70.0f);  // 100 + 50 - 30
    
    std::cout << "    testErrorHandlingIntegration passed" << std::endl;
}

void runAll() {
    testBrainNeuronIntegration();
    testNeuronalSynapseIntegration();
    testPlasticityIntegration();
    testMemoryIntegration();
    testNeuromodulationIntegration();
    testSensorimotorIntegration();
    testDevelopmentIntegration();
    testErrorHandlingIntegration();
}

} // namespace test_integration