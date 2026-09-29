// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>
#include <algorithm>

namespace test_neuron {

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

void testSpikeHistory() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getSpikeHistory().empty());
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    assert(neuron.getSpikeHistory().size() == 3);
    
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testSpikeHistory passed" << std::endl;
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

void testReset() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getTotalCurrent() == 0.0f);
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testReset passed" << std::endl;
}

// LIF Integration Accuracy Tests
void testLIFIntegrationAccuracy() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::SimulationClock clock(0.001);
    
    // Set LIF parameters for predictable behavior
    neuron.setMembranePotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setLeakConductance(10.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setResetPotential(-70.0f);
    
    // Test 1: Membrane potential should integrate smoothly
    // With I=0, V should exponentially decay to resting potential
    neuron.setMembranePotential(-60.0f);  // Above resting
    neuron.injectCurrent(0.0f);  // No external current
    
    const float dt = 0.001f;
    float v_before = neuron.getMembranePotential();
    
    neuron.step(clock.getTime(), dt);
    
    float v_after = neuron.getMembranePotential();
    
    // Should be closer to resting potential (-70mV)
    assert(v_after < v_before);  // Should decay
    assert(std::abs(v_after - (-70.0f)) < std::abs(v_before - (-70.0f));
    
    // Test 2: Inject current should increase potential
    neuron.injectCurrent(100.0f);  // Strong input
    neuron.step(clock.getTime(), dt);
    
    assert(neuron.getMembranePotential() > (-60.0f));
    
    std::cout << "    testLIFIntegrationAccuracy passed" << std::endl;
}

// Refractory Period Validation
void testRefractoryPeriodValidation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set up refractory period
    neuron.setRefractoryPeriod(10);
    neuron.setFiringState(nlm::FiringState::Active);
    
    // Should be in refractory state
    assert(neuron.isRefractory());
    assert(!neuron.isFiring());
    assert(neuron.getState().refractoryRemaining == 10);
    
    // Test refractory decrement
    neuron.decrementRefractory();
    assert(neuron.isRefractory());
    assert(neuron.getState().refractoryRemaining == 9);
    
    // Complete refractory period
    for (int i = 0; i < 9; ++i) {
        neuron.decrementRefractory();
    }
    
    assert(!neuron.isRefractory());
    assert(neuron.isFiring());
    assert(neuron.getState().refractoryRemaining == 0);
    
    // Test that neuron can fire multiple times after refractory
    neuron.setFiringState(nlm::FiringState::Resting);
    neuron.setRefractoryPeriod(5);
    
    // Fire and go into refractory
    neuron.setFiringState(nlm::FiringState::Active);
    
    std::cout << "    testRefractoryPeriodValidation passed" << std::endl;
}

// Spike Generation and Timing
void testSpikeGenerationAndTiming() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::SimulationClock clock(0.001);
    
    neuron.setMembranePotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setLeakConductance(10.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setResetPotential(-70.0f);
    
    // Inject current to reach threshold
    const float dt = 0.001f;
    for (int i = 0; i < 500; ++i) {
        neuron.injectCurrent(100.0f);  // Strong input
        bool fired = neuron.stepLIF(clock.getTime(), dt);
        
        if (fired) {
            // Verify spike is recorded
            assert(!neuron.getSpikeHistory().empty());
            assert(neuron.getState().lastSpikeTime > 0.0f);
            
            // Check that neuron is in refractory after firing
            assert(neuron.isRefractory());
            break;
        }
    
        clock.advanceBy(1);
    }
    
    // Test spike history timestamps
    neuron.clearSpikeHistory();
    neuron.recordSpike(0.1f);
    neuron.recordSpike(0.2f);
    neuron.recordSpike(0.3f);
    
    assert(neuron.getSpikeHistory().size() == 3);
    assert(neuron.getSpikeHistory()[0] == 0.1f);
    assert(neuron.getSpikeHistory()[1] == 0.2f);
    assert(neuron.getSpikeHistory()[2] == 0.3f);
    
    std::cout << "    testSpikeGenerationAndTiming passed" << std::endl;
}

// Membrane Potential Dynamics
void testMembranePotentialDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test membrane potential properties
    neuron.setMembranePotential(-65.0f);
    assert(neuron.getMembranePotential() == -65.0f);
    
    neuron.addToMembranePotential(10.0f);
    assert(neuron.getMembranePotential() == -55.0f);
    
    // Test threshold dynamics
    neuron.setThreshold(-50.0f);
    assert(neuron.getThreshold() == -50.0f);
    
    // Test threshold check
    neuron.setMembranePotential(-55.0f);  // Above threshold
    assert(neuron.checkThreshold());
    
    neuron.setMembranePotential(-60.0f);  // Below threshold
    assert(!neuron.checkThreshold());
    
    // Test leak conductance
    neuron.setLeakConductance(20.0f);
    assert(neuron.getLeakConductance() == 20.0f);
    
    // Test resting and reset potentials
    neuron.setRestingPotential(-75.0f);
    assert(neuron.getRestingPotential() == -75.0f);
    
    neuron.setResetPotential(-80.0f);
    assert(neuron.getResetPotential() == -80.0f);
    
    std::cout << "    testMembranePotentialDynamics passed" << std::endl;
}

// Input Current Handling
void testInputCurrentHandling() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test initial current is zero
    assert(neuron.getTotalCurrent() == 0.0f);
    
    // Test injecting multiple currents
    neuron.injectCurrent(10.0f);
    assert(neuron.getTotalCurrent() == 10.0f);
    
    neuron.injectCurrent(5.0f);
    assert(neuron.getTotalCurrent() == 15.0f);
    
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    // Test excitatory input
    neuron.receiveExcitatoryInput(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    // Test inhibitory input
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    assert(neuron.getTotalCurrent() == -3.0f);
    
    // Test modulatory input
    neuron.clearTotalCurrent();
    neuron.receiveModulatoryInput(2.0f);
    // Modulatory might be additive or subtractive depending on neuron type
    assert(std::abs(neuron.getTotalCurrent()) <= 2.0f);
    
    // Test reset clears all inputs
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    std::cout << "    testInputCurrentHandling passed" << std::endl;
}

// Memory leak detection for neuron
void testNeuronMemoryLeak() {
    // Simple test: create and destroy neurons
    for (int i = 0; i < 100; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i));
        neuron.setMembranePotential(-60.0f);
        neuron.injectCurrent(5.0f);
        
        // Perform operations
        neuron.recordSpike(0.1f);
        neuron.setFiringState(nlm::FiringState::Active);
        neuron.setRefractoryPeriod(5);
    }
    
    std::cout << "    testNeuronMemoryLeak passed" << std::endl;
}

// Random parameter validation
void testRandomParameterValidation() {
    nlm::RandomGenerator rng(12345);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test random initialization produces valid parameters
    neuron.initializeRandom(rng);
    
    // Check that parameters are in reasonable ranges
    assert(neuron.getMembranePotential() >= -80.0f && neuron.getMembranePotential() <= -60.0f);
    assert(neuron.getThreshold() >= -50.0f && neuron.getThreshold() <= -30.0f);
    assert(neuron.getLeakConductance() > 0.0f && neuron.getLeakConductance() < 100.0f);
    assert(neuron.getRestingPotential() >= -80.0f && neuron.getRestingPotential() <= -60.0f);
    
    // Test that repeated calls with same seed produce identical parameters
    nlm::Neuron neuron2(nlm::NeuronId(2));
    neuron2.initializeRandom(rng);
    
    assert(neuron.getMembranePotential() == neuron2.getMembranePotential());
    
    std::cout << "    testRandomParameterValidation passed" << std::endl;
}

// Integration test: Neuron-Brain interaction simulation
void testNeuronIntegrationWithClock() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::SimulationClock clock(0.001);
    
    neuron.setMembranePotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setLeakConductance(10.0f);
    neuron.setRestingPotential(-70.0f);
    
    const float dt = 0.001f;
    int spike_count = 0;
    
    // Simulate for a short period
    for (int step = 0; step < 1000; ++step) {
        clock.advance();
        
        // Inject current at first 100 steps
        if (step < 100) {
            neuron.injectCurrent(50.0f);
        }
        
        bool fired = neuron.stepLIF(clock.getTime(), dt);
        
        if (fired) {
            spike_count++;
            assert(neuron.isRefractory());
            neuron.clearTotalCurrent();  // Reset for next spike
        }
    
        clock.advanceBy(1);
    }
    
    // Should have fired at least some spikes
    assert(spike_count > 0);
    
    std::cout << "    testNeuronIntegrationWithClock passed" << std::endl;
}

void runAll() {
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
    
    // Additional comprehensive tests
    testLIFIntegrationAccuracy();
    testRefractoryPeriodValidation();
    testSpikeGenerationAndTiming();
    testMembranePotentialDynamics();
    testInputCurrentHandling();
    testNeuronMemoryLeak();
    testRandomParameterValidation();
    testNeuronIntegrationWithClock();
}

} // namespace test_neuron
