#pragma once

// Header file for neuron tests
// Tests for the nlm::Neuron class functionality including
// membrane potential dynamics, firing states, synaptic inputs,
// and spike recording mechanisms

namespace test_neuron {
    // Test basic neuron creation and ID assignment
    void testNeuronCreation();
    
    // Test neuron type switching capabilities
    void testNeuronType();
    
    // Test membrane potential dynamics
    void testMembranePotential();
    
    // Test firing state management
    void testFiringState();
    
    // Test refractory period decrement logic
    void testRefractoryDecrement();
    
    // Test current injection functionality
    void testCurrentInjection();
    
    // Test synaptic input handling (excitatory and inhibitory)
    void testSynapticInput();
    
    // Test spike history recording and management
    void testSpikeHistory();
    
    // Test random initialization
    void testRandomInitialization();
    
    // Test neuron reset functionality
    void testReset();
    
    // Run all neuron tests
    void runAll();
}