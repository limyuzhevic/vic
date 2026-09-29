// Neural Dynamics Tests
// Test real implementatons of neural dynamics models

#include "dynamics/NeuralDynamics.hpp"
#include "dynamics/HodgkinHuxleyDynamics.hpp"
#include "dynamics/FitzHughNagumoDynamics.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_neural_dynamics {

void testIntegrateAndFireDynamics() {
    std::cout << "Testing IntegrateAndFireDynamics..." << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::IntegrateAndFireDynamics iaf;
    
    // Set up neuron parameters for LIF simulation
    neuron.setMembranePotential(-65.0f);
    neuron.setThreshold(-50.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setResetPotential(-70.0f);
    neuron.setMembraneTimeConstant(20.0f);
    neuron.setRefractoryPeriod(2);
    
    // Test 1: Neuron below threshold should not fire
    neuron.setMembranePotential(-55.0f);
    bool fired = iaf.shouldFire(&neuron);
    assert(!fired && "Neuron below threshold should not fire");
    
    // Test 2: Neuron above threshold should fire
    neuron.setMembranePotential(-45.0f);
    fired = iaf.shouldFire(&neuron);
    assert(fired && "Neuron above threshold should fire");
    
    // Test 3: Update neuron dynamics
    iaf.updateNeuron(&neuron, 0.001);
    
    // Test 4: Reset dynamics
    iaf.reset();
    
    std::cout << "    testIntegrateAndFireDynamics passed" << std::endl;
}

void testHodgkinHuxleyDynamics() {
    std::cout << "Testing HodgkinHuxleyDynamics..." << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(2));
    nlm::HodgkinHuxleyDynamics hhd;
    
    // Configure HH parameters
    hhd.setSodiumReversal(50.0f);
    hhd.setPotassiumReversal(-77.0f);
    hhd.setLeakReversal(-54.387f);
    hhd.setSodiumConductance(120.0f);
    hhd.setPotassiumConductance(36.0f);
    hhd.setLeakConductance(0.3f);
    hhd.setCapacitance(1.0f);
    
    // Test 1: Set up neuron for HH simulation
    neuron.setMembranePotential(-65.0f);
    neuron.setThreshold(-55.0f);
    neuron.setRestingPotential(-65.0f);
    neuron.setResetPotential(-65.0f);
    neuron.setRefractoryPeriod(2);
    
    // Test 2: Update neuron dynamics
    hhd.updateNeuron(&neuron, 0.001);
    
    // Test 3: Apply synaptic input
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setWeight(0.5f);
    synapse.setType(nlm::SynapseType::Excitatory);
    hhd.applySpikeInput(&neuron, &synapse);
    
    // Test 4: Check parameters
    assert(std::abs(hhd.getSodiumReversal() - 50.0f) < 0.1f);
    assert(std::abs(hhd.getPotassiumReversal() + 77.0f) < 0.1f);
    assert(std::abs(hhd.getLeakReversal() + 54.387f) < 0.1f);
    
    // Test 5: Reset
    hhd.reset();
    
    std::cout << "    testHodgkinHuxleyDynamics passed" << std::endl;
}

void testFitzHughNagumoDynamics() {
    std::cout << "Testing FitzHughNagumoDynamics..." << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(3));
    nlm::FitzHughNagumoDynamics fhn;
    
    // Configure FHN parameters
    fhn.setA(0.08f);
    fhn.setB(0.065f);
    fhn.setTau(12.0f);
    
    // Test 1: Set up neuron for FHN simulation
    neuron.setMembranePotential(-1.0f);
    neuron.setThreshold(-0.5f);
    neuron.setRestingPotential(-1.2f);
    neuron.setResetPotential(-1.2f);
    neuron.setRefractoryPeriod(2);
    
    // Test 2: Update neuron dynamics
    fhn.updateNeuron(&neuron, 0.001);
    
    // Test 3: Apply synaptic input
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(1), nlm::NeuronId(3));
    synapse.setWeight(0.3f);
    synapse.setType(nlm::SynapseType::Excitatory);
    fhn.applySpikeInput(&neuron, &synapse);
    
    // Test 4: Check parameters
    assert(std::abs(fhn.getA() - 0.08f) < 0.01f);
    assert(std::abs(fhn.getB() - 0.065f) < 0.01f);
    assert(std::abs(fhn.getTau() - 12.0f) < 0.1f);
    
    // Test 5: Reset
    fhn.reset();
    
    std::cout << "    testFitzHughNagumoDynamics passed" << std::endl;
}

void testNeuralDynamicsIntegration() {
    std::cout << "Testing NeuralDynamics integration..." << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(4));
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(1), nlm::NeuronId(4));
    nlm::IntegrateAndFireDynamics iaf;
    
    // Test integration between dynamics and neuron
    neuron.setMembranePotential(-60.0f);
    neuron.setThreshold(-55.0f);
    neuron.injectCurrent(2.0f);
    
    iaf.updateNeuron(&neuron, 0.001);
    iaf.applySpikeInput(&neuron, &synapse);
    iaf.updateSynapse(&synapse, 0.001);
    
    // Test reset functionality
    iaf.reset();
    
    std::cout << "    testNeuralDynamicsIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running neural dynamics tests..." << std::endl;
    testIntegrateAndFireDynamics();
    testHodgkinHuxleyDynamics();
    testFitzHughNagumoDynamics();
    testNeuralDynamicsIntegration();
    std::cout << "All neural dynamics tests passed!" << std::endl;
}

} // namespace test_neural_dynamics
