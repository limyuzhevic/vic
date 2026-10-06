#pragma once

// Header file for synapse tests
// Tests for the nlm::Synapse class functionality including
// weight management, type handling, delay settings, spike history,
// eligibility traces, and plasticity mechanisms

namespace test_synapse {
    // Test basic synapse creation and ID assignment
    void testSynapseCreation();
    
    // Test synapse weight modification and validation
    void testSynapseWeight();
    
    // Test synapse weight bounds (clamping)
    void testSynapseWeightBounds();
    
    // Test synapse type management (excitatory/inhibitory)
    void testSynapseType();
    
    // Test synaptic delay settings
    void testSynapseDelay();
    
    // Test pre-synaptic and post-synaptic spike history
    void testSynapseSpikeHistory();
    
    // Test eligibility trace calculation and decay
    void testSynapseEligibilityTrace();
    
    // Test synapse efficacy management
    void testSynapseEfficacy();
    
    // Test random initialization
    void testSynapseRandomInitialization();
    
    // Test synapse reset functionality
    void testSynapseReset();
    
    // Test plasticity flag configuration
    void testSynapsePlasticityFlags();
    
    // Run all synapse tests
    void runAll();
}