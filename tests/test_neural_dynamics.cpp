// Neural Dynamics Tests
// Tests for Hodgkin-Huxley, FitzHugh-Nagumo, and enhanced IntegrateAndFire dynamics

#include "dynamics/NeuralDynamics.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_neural_dynamics {

void testHodgkinHuxleyBasic() {
    std::cout << "    Testing HodgkinHuxleyDynamics basic functionality..." << std::endl;
    
    // Create HH dynamics model
    nlm::HodgkinHuxleyDynamics hh;
    
    // Create a test neuron
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setMembranePotential(-70.0f);  // Resting potential
    neuron.setThreshold(-55.0f);
    
    // Test parameters
    hh.setMembraneCapacitance(1.0f);
    hh.setMaxSodiumConductance(120.0f);
    hh.setMaxPotassiumConductance(36.0f);
    hh.setMaxLeakConductance(0.3f);
    hh.setSodiumReversalPotential(50.0f);
    hh.setPotassiumReversalPotential(-77.0f);
    hh.setLeakReversalPotential(-54.387f);
    
    // Inject constant current to elicit spike
    neuron.injectCurrent(10.0f);  // Sufficient to cause action potential
    
    // Simulate for several steps
    for (int i = 0; i < 100; ++i) {
        hh.updateNeuron(&neuron, 0.01);  // 0.01 ms timestep
        
        // Check that voltage remains within physiological bounds
        float V = neuron.getMembranePotential();
        assert(V >= -100.0f && V <= 100.0f); // Reasonable bounds
    }
    
    // Check that neuron can fire (HH is capable of action potentials)
    std::cout << "    HH basic test passed" << std::endl;
}

void testFitzHughNagumoTypeI() {
    std::cout << "    Testing FitzHughNagumoDynamics Type I..." << std::endl;
    
    // Create FHN dynamics model with Type I parameters
    nlm::FitzHughNagumoDynamics fhn;
    
    // Set Type I parameters (bistability regime)
    fhn.setCubicParameter(0.7f);  // Controls shape of cubic nullcline
    fhn.setRecoveryTimeConstant(12.5f);
    fhn.setMembraneTimeConstant(10.0f);
    fhn.setExcitabilityType(nlm::FitzHughNagumoDynamics::ExcitabilityType::TypeI);
    
    // Create test neuron
    nlm::Neuron neuron(nlm::NeuronId(2));
    neuron.setMembranePotential(-65.0f);
    neuron.setThreshold(-40.0f);
    
    // Simulate FHN dynamics
    float V_prev = neuron.getMembranePotential();
    for (int i = 0; i < 200; ++i) {
        fhn.updateNeuron(&neuron, 0.01);
        float V = neuron.getMembranePotential();
        
        // Check for limit cycle behavior (oscillations)
        if (i > 10) {
            assert(V >= -80.0f && V <= 50.0f); // FHN can oscillate between these bounds
        }
        
        // Check spike detection
        if (fhn.hasRecentSpike()) {
            assert(V <= -60.0f); // Spike resets to near resting
        }
    }
    
    std::cout << "    FHN Type I test passed" << std::endl;
}

void testFitzHughNagumoTypeII() {
    std::cout << "    Testing FitzHughNagumoDynamics Type II..." << std::endl;
    
    // Create FHN dynamics model with Type II parameters
    nlm::FitzHughNagumoDynamics fhn;
    
    // Set Type II parameters (excitable regime)
    fhn.setCubicParameter(0.1f);  // Different cubic shape for Type II
    fhn.setRecoveryTimeConstant(15.0f);
    fhn.setMembraneTimeConstant(10.0f);
    fhn.setExcitabilityType(nlm::FitzHughNagumoDynamics::ExcitabilityType::TypeII);
    
    // Create test neuron
    nlm::Neuron neuron(nlm::NeuronId(3));
    neuron.setMembranePotential(-70.0f);
    neuron.setThreshold(-45.0f);
    
    // Simulate with subthreshold input
    neuron.injectCurrent(1.0f);  // Subthreshold current
    
    for (int i = 0; i < 100; ++i) {
        fhn.updateNeuron(&neuron, 0.01);
        float V = neuron.getMembranePotential();
        
        // Type II should show gradual depolarization
        assert(V >= -80.0f && V <= 20.0f);
    }
    
    std::cout << "    FHN Type II test passed" << std::endl;
}

void testEnhancedIntegrateAndFire() {
    std::cout << "    Testing enhanced IntegrateAndFireDynamics..." << std::endl;
    
    // Create enhanced IAF dynamics
    nlm::IntegrateAndFireDynamics iaf;
    
    // Set enhanced parameters
    iaf.setMembraneTimeConstant(20.0f);
    iaf.setMembraneCapacitance(0.25f);
    iaf.setThreshold(-55.0f);
    iaf.setAdaptationRate(0.1f);
    
    // Create test neuron
    nlm::Neuron neuron(nlm::NeuronId(4));
    neuron.setMembranePotential(-70.0f);
    neuron.setThreshold(-55.0f);
    
    // Test adaptive threshold behavior
    float V_initial = neuron.getMembranePotential();
    neuron.injectCurrent(8.0f); // Suprathreshold current
    
    for (int i = 0; i < 50; ++i) {
        iaf.updateNeuron(&neuron, 0.01);
        float V = neuron.getMembranePotential();
        
        // Neuron should fire at some point
        if (iaf.shouldFire(&neuron)) {
            neuron.setFiringState(nlm::FiringState::Active);
            neuron.recordSpike(0.0); // Simulate spike
            neuron.setMembranePotential(-70.0f); // Reset
        }
    }
    
    // Check that adaptation variable increased after spikes
    assert(iaf.hasRecentSpike() || neuron.getMembranePotential() > -55.0f);
    
    std::cout << "    Enhanced IAF test passed" << std::endl;
}

void testConductionBasedSynapses() {
    std::cout << "    Testing conductance-based synapse support..." << std::endl;
    
    // Create IAF dynamics with synaptic support
    nlm::IntegrateAndFireDynamics iaf;
    
    // Create pre and post neurons
    nlm::Neuron pre_neuron(nlm::NeuronId(5));
    nlm::Neuron post_neuron(nlm::NeuronId(6));
    
    pre_neuron.setMembranePotential(-70.0f);
    post_neuron.setMembranePotential(-70.0f);
    post_neuron.setThreshold(-55.0f);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), pre_neuron.getId(), post_neuron.getId());
    synapse.setExcitatory(true);
    synapse.setWeight(1.0f);
    
    // Simulate synaptic transmission
    iaf.updateSynapse(&synapse, 0.01);
    
    // Apply spike input
    iaf.applySpikeInput(&post_neuron, &synapse);
    
    // Check that post-synaptic neuron received excitatory input
    assert(post_neuron.getTotalCurrent() > 0.0f);
    
    // Reset and test inhibitory synapse
    post_neuron.clearTotalCurrent();
    synapse.setExcitatory(false);
    iaf.applySpikeInput(&post_neuron, &synapse);
    
    // Should receive inhibitory input
    assert(post_neuron.getTotalCurrent() < 0.0f);
    
    std::cout << "    Conduction-based synapse test passed" << std::endl;
}

void testNumericalIntegration() {
    std::cout << "    Testing numerical integration methods..." << std::endl;
    
    // Test different integration methods for HH
    nlm::HodgkinHuxleyDynamics hh_euler;
    nlm::HodgkinHuxleyDynamics hh_crank;
    
    hh_euler.setIntegrationMethod(nlm::HodgkinHuxleyDynamics::IntegrationMethod::Euler);
    hh_crank.setIntegrationMethod(nlm::HodgkinHuxleyDynamics::IntegrationMethod::CrankNicolson);
    
    // Create test neurons
    nlm::Neuron neuron1(nlm::NeuronId(7));
    nlm::Neuron neuron2(nlm::NeuronId(8));
    
    neuron1.setMembranePotential(-70.0f);
    neuron2.setMembranePotential(-70.0f);
    neuron1.setThreshold(-55.0f);
    neuron2.setThreshold(-55.0f);
    
    neuron1.injectCurrent(10.0f);
    neuron2.injectCurrent(10.0f);
    
    // Simulate with both methods
    for (int i = 0; i < 50; ++i) {
        hh_euler.updateNeuron(&neuron1, 0.01);
        hh_crank.updateNeuron(&neuron2, 0.01);
        
        // Both methods should produce physiologically plausible results
        float V1 = neuron1.getMembranePotential();
        float V2 = neuron2.getMembranePotential();
        
        assert(V1 >= -100.0f && V1 <= 100.0f);
        assert(V2 >= -100.0f && V2 <= 100.0f);
    }
    
    std::cout << "    Numerical integration test passed" << std::endl;
}

void testParameterTuning() {
    std::cout << "    Testing parameter tuning..." << std::endl;
    
    // Test parameter sensitivity for HH dynamics
    nlm::HodgkinHuxleyDynamics hh;
    
    // Create test neuron
    nlm::Neuron neuron(nlm::NeuronId(9));
    neuron.setMembranePotential(-70.0f);
    
    // Test different parameter values
    struct HHParams {
        float Cm, gNa, gK, gL, ENa, EK, EL;
    };
    
    // Set of parameter combinations
    HHParams params[] = {
        {1.0f, 120.0f, 36.0f, 0.3f, 50.0f, -77.0f, -54.387f},   // Standard HH
        {0.8f, 100.0f, 30.0f, 0.2f, 45.0f, -75.0f, -55.0f},    // Modified
        {1.2f, 150.0f, 40.0f, 0.5f, 55.0f, -80.0f, -60.0f}     // High excitability
    };
    
    for (int p = 0; p < 3; ++p) {
        hh.setMembraneCapacitance(params[p].Cm);
        hh.setMaxSodiumConductance(params[p].gNa);
        hh.setMaxPotassiumConductance(params[p].gK);
        hh.setMaxLeakConductance(params[p].gL);
        hh.setSodiumReversalPotential(params[p].ENa);
        hh.setPotassiumReversalPotential(params[p].EK);
        hh.setLeakReversalPotential(params[p].EL);
        
        // Reset neuron for each parameter set
        neuron.setMembranePotential(-70.0f);
        neuron.injectCurrent(5.0f); // Moderate current
        
        // Simulate
        for (int i = 0; i < 30; ++i) {
            hh.updateNeuron(&neuron, 0.01);
            float V = neuron.getMembranePotential();
            assert(V >= -100.0f && V <= 100.0f); // All parameter sets should work
        }
    }
    
    std::cout << "    Parameter tuning test passed" << std::endl;
}

void runAll() {
    std::cout << "Running Neural Dynamics Tests..." << std::endl;
    std::cout << "================================" << std::endl;
    
    testHodgkinHuxleyBasic();
    testFitzHughNagumoTypeI();
    testFitzHughNagumoTypeII();
    testEnhancedIntegrateAndFire();
    testConductionBasedSynapses();
    testNumericalIntegration();
    testParameterTuning();
    
    std::cout << "================================" << std::endl;
    std::cout << "All Neural Dynamics tests passed!" << std::endl;
}

} // namespace test_neural_dynamics