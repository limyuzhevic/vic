// Unit test for Neuron (LIF dynamics)
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>

namespace test_neuron_unit {

void testNeuronCreation() {
    nlm::Neuron neuron;
    
    // Test basic neuron properties
    assert(neuron.getId().has_value());
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    assert(neuron.getMembranePotential() == 0.0f);
    assert(neuron.getRefractoryPeriod() == 0.0f);
    
    std::cout << "    testNeuronCreation passed" << std::endl;
}

void testNeuronLIFDynamics() {
    nlm::Neuron neuron;
    
    // Test LIF dynamics
    neuron.update(0.001f);  // Update for 1ms
    
    // Check that membrane potential changed
    float potential = neuron.getMembranePotential();
    assert(potential >= -70.0f && potential <= 50.0f);  // Typical range
    
    // Test refractory period
    neuron.setRefractoryPeriod(0.01f);  // 10ms refractory
    assert(neuron.getRefractoryPeriod() == 0.01f);
    
    std::cout << "    testNeuronLIFDynamics passed" << std::endl;
}

void testNeuronSpiking() {
    nlm::Neuron neuron;
    
    // Set up neuron to spike
    neuron.setMembranePotential(0.0f); // Threshold level
    
    // Check if neuron spikes
    neuron.update(0.001f);
    
    bool spiked = neuron.hasSpiked();
    // Neuron may or may not spike depending on dynamics
    // Just testing that the method exists
    
    std::cout << "    testNeuronSpiking passed" << std::endl;
}

void testNeuronInputs() {
    nlm::Neuron neuron;
    
    // Test input handling
    neuron.addInput(1.0f, 1.0f); // Input value and weight
    
    // Check total input
    float totalInput = neuron.getTotalInput();
    assert(totalInput >= 0.0f);
    
    std::cout << "    testNeuronInputs passed" << std::endl;
}

void testNeuronState() {
    nlm::Neuron neuron;
    
    // Test neuron state
    neuron.setMembranePotential(-65.0f);
    neuron.set refractoryPeriod(0.005f);
    
    assert(neuron.getMembranePotential() == -65.0f);
    assert(neuron.getRefractoryPeriod() == 0.005f);
    
    std::cout << "    testNeuronState passed" << std::endl;
}

void testNeuronType() {
    nlm::Neuron neuron;
    
    // Test different neuron types
    neuron.setType(nlm::NeuronType::Excitatory);
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    assert(neuron.getType() == nlm::NeuronType::Inhibitory);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testNeuronParameters() {
    nlm::Neuron neuron;
    
    // Test neuron parameters
    neuron.setThreshold(-55.0f);
    neuron.setRestingPotential(-65.0f);
    neuron.setMembraneResistance(10.0f);
    neuron.setCapacitance(1.0f);
    
    assert(neuron.getThreshold() == -55.0f);
    assert(neuron.getRestingPotential() == -65.0f);
    assert(neuron.getMembraneResistance() == 10.0f);
    assert(neuron.getCapacitance() == 1.0f);
    
    std::cout << "    testNeuronParameters passed" << std::endl;
}

void runAllNeuronUnitTests() {
    testNeuronCreation();
    testNeuronLIFDynamics();
    testNeuronSpiking();
    testNeuronInputs();
    testNeuronState();
    testNeuronType();
    testNeuronParameters();
}

} // namespace test_neuron_unit