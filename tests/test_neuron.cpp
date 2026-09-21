// Neuron Tests - Comprehensive LIF Dynamics Testing
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <numeric>

namespace test_neuron {

// Helper function to check if a neuron spikes when potential reaches threshold
void testLIFIntegration() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::RandomGenerator rng(42);
    
    neuron.initializeRandom(rng);
    neuron.setLeakConductance(10.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    
    float initialPotential = neuron.getMembranePotential();
    
    // Test 1: Membrane potential evolves over time with leak
    nlm::Timestamp currentTime = 0.0;
    nlm::TimestepDuration dt = 0.001f;  // 1ms
    
    neuron.setMembranePotential(initialPotential + 20.0f);  // Start above threshold
    neuron.step(currentTime, dt);
    
    float potentialAfterFirstStep = neuron.getMembranePotential();
    assert(potentialAfterFirstStep == neuron.getResetPotential() && 
           "Neuron should have spiked and reset when above threshold");
    
    // Test 2: Sub-threshold input should evolve with leak
    neuron.reset();
    neuron.setMembranePotential(neuron.getRestingPotential());
    
    for (int i = 0; i < 100; ++i) {
        neuron.injectCurrent(5.0f);  // Constant input
        neuron.step(currentTime, dt);
        currentTime += dt;
    }
    
    // After sustained input, potential should be depolarized
    float finalPotential = neuron.getMembranePotential();
    assert(finalPotential > neuron.getRestingPotential() &&
           "Sustained sub-threshold input should depolarize membrane");
    
    std::cout << "    testLIFIntegration passed" << std::endl;
}

// Test spike generation with varying input strengths
void testSpikeGeneration() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::RandomGenerator rng(123);
    
    neuron.initializeRandom(rng);
    neuron.setLeakConductance(10.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    
    nlm::Timestamp currentTime = 0.0;
    nlm::TimestepDuration dt = 0.001f;
    
    // Test with strong sufficient input to reach threshold
    neuron.reset();
    neuron.setMembranePotential(neuron.getRestingPotential());
    
    for (int i = 0; i < 1000; ++i) {
        // Inject current that should reach threshold within reasonable time
        neuron.injectCurrent(15.0f);  // Strong input
        neuron.step(currentTime, dt);
        currentTime += dt;
        
        if (neuron.isFiring()) {
            // Neuron should fire after reaching threshold
            assert(neuron.getLastSpikeTime() > 0.0f && 
                   "Neuron should have recorded spike time");
            
            // Should be in refractory state after firing
            assert(neuron.isRefractory() &&
                   "Neuron should be in refractory state after spike");
            
            break;
        }
        
        if (i == 999) {
            assert(false && "Neuron should have spiked with strong input");
        }
    }
    
    std::cout << "    testSpikeGeneration passed" << std::endl;
}

// Test refractory period dynamics
void testRefractoryPeriod() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::RandomGenerator rng(456);
    
    neuron.initializeRandom(rng);
    neuron.setLeakConductance(10.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    neuron.setRefractoryPeriod(10);  // 10ms refractory period
    
    nlm::Timestamp currentTime = 0.0;
    nlm::TimestepDuration dt = 0.001f;
    
    // Make neuron fire
    neuron.setMembranePotential(neuron.getThreshold());
    neuron.step(currentTime, dt);
    
    assert(neuron.isFiring() && "Neuron should fire when at threshold");
    assert(neuron.isRefractory() && "Neuron should enter refractory period after spike");
    
    // Count remaining steps in refractory period
    int refractorySteps = 0;
    while (neuron.isRefractory() && refractorySteps < 20) {
        neuron.decrementRefractory();
        ++refractorySteps;
        
        // Even with input, neuron should not fire during refractory
        neuron.injectCurrent(50.0f);
        neuron.step(currentTime, dt);
        currentTime += dt;
    }
    
    // Should have exited refractory period
    assert(refractorySteps >= neuron.getRefractoryPeriod() &&
           "Neuron should have completed refractory period");
    assert(!neuron.isRefractory() && "Neuron should be out of refractory period");
    
    std::cout << "    testRefractoryPeriod passed" << std::endl;
}

// Test spike-frequency adaptation
void testSpikeFrequencyAdaptation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::RandomGenerator rng(789);
    
    neuron.initializeRandom(rng);
    neuron.setLeakConductance(10.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    
    nlm::Timestamp currentTime = 0.0;
    nlm::TimestepDuration dt = 0.001f;
    
    // Make neuron fire multiple times rapidly
    neuron.reset();
    neuron.setMembranePotential(neuron.getThreshold() + 10.0f);
    
    std::vector<nlm::Timestamp> spikeTimes;
    for (int i = 0; i < 10; ++i) {
        // Ensure neuron fires
        neuron.step(currentTime, dt);
        currentTime += dt;
        
        if (neuron.isFiring() && neuron.getLastSpikeTime() > 0.0f) {
            spikeTimes.push_back(neuron.getLastSpikeTime());
        }
    }
    
    // Verify adaptation variable increases with spikes
    float adaptationBefore = neuron.getState().adaptationVariable;
    
    // Fire more times
    for (int i = 0; i < 5; ++i) {
        neuron.setMembranePotential(neuron.getThreshold() + 10.0f);
        neuron.step(currentTime, dt);
        currentTime += dt;
    }
    
    float adaptationAfter = neuron.getState().adaptationVariable;
    assert(adaptationAfter > adaptationBefore &&
           "Adaptation variable should increase with more spikes");
    
    std::cout << "    testSpikeFrequencyAdaptation passed" << std::endl;
}

// Test synaptic inputs (excitatory/inhibitory)
void testSynapticInputs() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::RandomGenerator rng(999);
    
    neuron.initializeRandom(rng);
    neuron.setLeakConductance(10.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    
    nlm::Timestamp currentTime = 0.0;
    nlm::TimestepDuration dt = 0.001f;
    
    // Test excitatory input
    neuron.reset();
    neuron.setMembranePotential(neuron.getRestingPotential());
    
    neuron.receiveExcitatoryInput(10.0f);
    neuron.step(currentTime, dt);
    
    // Excitatory input should increase membrane potential
    assert(neuron.getMembranePotential() > neuron.getRestingPotential() &&
           "Excitatory input should depolarize neuron");
    
    // Test inhibitory input
    neuron.reset();
    neuron.setMembranePotential(neuron.getRestingPotential());
    
    neuron.receiveInhibitoryInput(10.0f);
    neuron.step(currentTime, dt);
    
    // Inhibitory input should hyperpolarize neuron
    assert(neuron.getMembranePotential() < neuron.getRestingPotential() &&
           "Inhibitory input should hyperpolarize neuron");
    
    std::cout << "    testSynapticInputs passed" << std::endl;
}

// Test neuromodulatory input effects
void testNeuromodulation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::RandomGenerator rng(111);
    
    neuron.initializeRandom(rng);
    neuron.setLeakConductance(10.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    
    float initialAdaptation = neuron.getState().adaptationVariable;
    
    // Test modulatory input increases adaptation variable
    neuron.receiveModulatoryInput(5.0f);
    neuron.step(0.0, 0.001f);
    
    float adaptationAfterMod = neuron.getState().adaptationVariable;
    assert(adaptationAfterMod > initialAdaptation &&
           "Modulatory input should increase adaptation variable");
    
    std::cout << "    testNeuromodulation passed" << std::endl;
}

// Test spike history and record keeping
void testSpikeHistory() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::RandomGenerator rng(222);
    
    neuron.initializeRandom(rng);
    
    assert(neuron.getSpikeHistory().empty() && "Spike history should be empty initially");
    
    // Add some spikes
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    assert(neuron.getSpikeHistory().size() == 3 && "Should have 3 spikes in history");
    assert(neuron.getSpikeHistory()[0] == 0.1f && "First spike should be at 0.1");
    assert(neuron.getSpikeHistory()[2] == 0.3f && "Third spike should be at 0.3");
    
    // Test clearing spike history
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty() && "Spike history should be empty after clear");
    
    std::cout << "    testSpikeHistory passed" << std::endl;
}

// Test plasticity flags
void testPlasticityFlags() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(!neuron.getPlasticityFlags().hebbian && "Hebbian plasticity should be disabled by default");
    assert(!neuron.getPlasticityFlags().stdp && "STDP plasticity should be disabled by default");
    assert(!neuron.getPlasticityFlags().reward_modulated && "Reward-modulated plasticity should be disabled by default");
    
    neuron.enablePlasticity(true, false, true);
    assert(neuron.getPlasticityFlags().hebbian && "Hebbian plasticity should be enabled");
    assert(!neuron.getPlasticityFlags().stdp && "STDP plasticity should remain disabled");
    assert(neuron.getPlasticityFlags().reward_modulated && "Reward-modulated plasticity should be enabled");
    
    std::cout << "    testPlasticityFlags passed" << std::endl;
}

// Test region and population membership
void testRegionPopulationMembership() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getRegionId().value == 0 && "Default region should be 0");
    assert(neuron.getPopulationId().value == 0 && "Default population should be 0");
    
    neuron.setRegionId(nlm::RegionId(5));
    neuron.setPopulationId(nlm::PopulationId(3));
    
    assert(neuron.getRegionId().value == 5 && "Region ID should be 5");
    assert(neuron.getPopulationId().value == 3 && "Population ID should be 3");
    
    std::cout << "    testRegionPopulationMembership passed" << std::endl;
}

// Test synaptic connectivity management
void testSynapticConnectivity() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getIncomingSynapses().empty() && "Incoming synapses should be empty initially");
    assert(neuron.getOutgoingSynapses().empty() && "Outgoing synapses should be empty initially");
    
    nlm::SynapseHandle incomingHandle = 123;
    nlm::SynapseHandle outgoingHandle = 456;
    
    neuron.addIncomingSynapse(incomingHandle);
    neuron.addOutgoingSynapse(outgoingHandle);
    
    assert(neuron.getIncomingSynapses().size() == 1 && "Should have 1 incoming synapse");
    assert(neuron.getOutgoingSynapses().size() == 1 && "Should have 1 outgoing synapse");
    assert(neuron.getIncomingSynapses()[0] == incomingHandle && "Incoming synapse handle should match");
    assert(neuron.getOutgoingSynapses()[0] == outgoingHandle && "Outgoing synapse handle should match");
    
    std::cout << "    testSynapticConnectivity passed" << std::endl;
}

// Test reset functionality
void testReset() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::RandomGenerator rng(333);
    
    neuron.initializeRandom(rng);
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    neuron.setMembranePotential(-65.0f);
    neuron.setFiringState(nlm::FiringState::Active);
    neuron.enablePlasticity(true, true, false);
    
    neuron.reset();
    
    // Should be back to initial state values
    assert(neuron.getMembranePotential() == -70.0f && "Membrane potential should reset to -70mV");
    assert(neuron.getTotalCurrent() == 0.0f && "Current should be cleared");
    assert(neuron.getSpikeHistory().empty() && "Spike history should be cleared");
    assert(neuron.getState().firingState == nlm::FiringState::Resting && 
           "Firing state should be Resting");
    
    std::cout << "    testReset passed" << std::endl;
}

// Test initialization with random parameters
void testRandomInitialization() {
    nlm::RandomGenerator rng(444);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.initializeRandom(rng);
    
    // Values should be within biologically plausible ranges
    assert(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f);
    assert(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f);
    assert(neuron.getRestingPotential() < -60.0f && neuron.getRestingPotential() > -80.0f);
    assert(neuron.getResetPotential() > neuron.getRestingPotential());  // Reset usually above resting
    assert(neuron.getRefractoryPeriod() >= 2 && neuron.getRefractoryPeriod() <= 10); // 2-10 ms typical
    
    std::cout << "    testRandomInitialization passed" << std::endl;
}

// Test original test functions (backward compatibility)
void testNeuronCreation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getId() == nlm::NeuronId(1));
    assert(neuron.getType() == nlm::NeuronType::Internal);
    
    std::cout << "    testNeuronCreation passed" << std::endl;
}

void testNeuronType() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    assert(neuron.getType() == nlm::NeuronType::Inhibitory);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testMembranePotential() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getMembranePotential() == -70.0f);  // Default value
    
    neuron.setMembranePotential(-65.0f);
    assert(neuron.getMembranePotential() == -65.0f);
    
    neuron.addToMembranePotential(5.0f);
    assert(neuron.getMembranePotential() == -60.0f);
    
    std::cout << "    testMembranePotential passed" << std::endl;
}

void testFiringState() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(!neuron.isFiring());
    assert(!neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Active);
    assert(neuron.isFiring());
    
    neuron.setRefractoryPeriod(10);
    assert(neuron.isRefractory());
    assert(!neuron.isFiring());
    
    std::cout << "    testFiringState passed" << std::endl;
}

void testRefractoryDecrement() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(5);
    assert(neuron.isRefractory());
    
    neuron.decrementRefractory();
    assert(neuron.isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    assert(!neuron.isRefractory());
    
    std::cout << "    testRefractoryDecrement passed" << std::endl;
}

void testCurrentInjection() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getTotalCurrent() == 0.0f);
    
    neuron.injectCurrent(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    std::cout << "    testCurrentInjection passed" << std::endl;
}

void testSynapticInput() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.receiveExcitatoryInput(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    assert(neuron.getTotalCurrent() == -3.0f);
    
    std::cout << "    testSynapticInput passed" << std::endl;
}

void testRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.initializeRandom(rng);
    
    // Just verify it doesn't crash and values are in reasonable range
    assert(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f);
    assert(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f);
    
    std::cout << "    testRandomInitialization passed" << std::endl;
}

void runAll() {
    // Original tests
    testNeuronCreation();
    testNeuronType();
    testMembranePotential();
    testFiringState();
    testRefractoryDecrement();
    testCurrentInjection();
    testSynapticInput();
    testSpikeHistory();
    testRandomInitialization();
    testReset();
    
    // New comprehensive LIF dynamics tests
    testLIFIntegration();
    testSpikeGeneration();
    testRefractoryPeriod();
    testSpikeFrequencyAdaptation();
    testSynapticInputs();
    testNeuromodulation();
    testPlasticityFlags();
    testRegionPopulationMembership();
    testSynapticConnectivity();
}

} // namespace test_neuron
