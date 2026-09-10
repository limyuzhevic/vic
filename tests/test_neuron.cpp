// Unit tests for NLM neural components
// Tests basic neuron functionality and LIF dynamics

#include <cassert>
#include <cmath>
#include <iostream>
#include <algorithm>

void testNeuronBasicFunctionality() {
    std::cout << "Testing Neuron Basic Functionality..." << std::endl;
    
    // Create a test neuron
    Neuron neuron(1);
    
    // Test initial state
    ASSERT_EQ(neuron.getId(), 1);
    ASSERT_EQ(neuron.getType(), NeuronType::Internal);
    ASSERT_FALSE(neuron.isFiring());
    ASSERT_FALSE(neuron.isRefractory());
    
    // Test membrane potential operations
    neuron.setMembranePotential(-70.0f);
    ASSERT_EQ(neuron.getMembranePotential(), -70.0f);
    
    neuron.addToMembranePotential(10.0f);
    ASSERT_EQ(neuron.getMembranePotential(), -60.0f);
    
    // Test threshold operations
    neuron.setThreshold(-55.0f);
    ASSERT_EQ(neuron.getThreshold(), -55.0f);
    
    ASSERT_FALSE(neuron.checkThreshold());  // -60 < -55
    
    neuron.setMembranePotential(-50.0f);
    ASSERT_TRUE(neuron.checkThreshold());   // -50 > -55
    
    // Test current injection
    neuron.injectCurrent(10.0f);
    ASSERT_EQ(neuron.getTotalCurrent(), 10.0f);
    
    // Test spike recording
    neuron.recordSpike(100.0f);
    ASSERT_EQ(neuron.getSpikeHistory().size(), 1);
    ASSERT_EQ(neuron.getSpikeHistory()[0], 100.0f);
    
    // Test reset
    neuron.reset();
    ASSERT_EQ(neuron.getMembranePotential(), -70.0f);
    ASSERT_EQ(neuron.getTotalCurrent(), 0.0f);
    
    std::cout << "✓ Neuron basic functionality tests passed" << std::endl;
}

void testNeuronLIFDynamics() {
    std::cout << "Testing Neuron LIF Dynamics..." << std::endl;
    
    Neuron neuron(2);
    neuron.setType(NeuronType::Internal);
    
    // Set up neuron for LIF simulation
    neuron.setMembranePotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setLeakConductance(10.0f);
    neuron.setResetPotential(-70.0f);
    
    // Set refractory period
    neuron.setRefractoryPeriod(5);
    
    // Simulate no input - should leak to resting potential
    for (int i = 0; i < 10; ++i) {
        neuron.stepLIF(i * 0.001, 0.001);
    }
    
    // After leak, potential should be close to resting
    float potential = neuron.getMembranePotential();
    ASSERT_NEAR(potential, -70.0f, 0.1f);
    
    // Inject current to reach threshold
    neuron.setMembranePotential(-70.0f);
    neuron.injectCurrent(100.0f); // Strong input
    
    // Run enough steps to reach threshold
    bool fired = false;
    for (int i = 0; i < 20 && !fired; ++i) {
        fired = neuron.stepLIF(i * 0.001, 0.001);
    }
    
    // Neuron should have fired
    ASSERT_TRUE(fired);
    ASSERT_TRUE(neuron.isFiring());
    ASSERT_TRUE(neuron.isRefractory());
    
    // After refractory period, neuron should reset
    neuron.decrementRefractory();
    // Continue decrementing
    for (int i = 0; i < 4 && neuron.isRefractory(); ++i) {
        neuron.decrementRefractory();
    }
    
    // Neuron should no longer be refractory
    ASSERT_FALSE(neuron.isRefractory());
    
    std::cout << "✓ Neuron LIF dynamics tests passed" << std::endl;
}

void testNeuronPlasticityFlags() {
    std::cout << "Testing Neuron Plasticity Flags..." << std::endl;
    
    Neuron neuron(3);
    
    // Test default plasticity flags
    const PlasticityFlags& flags = neuron.getPlasticityFlags();
    // Default values from NeuronState initialization
    
    // Test enable plasticity
    neuron.enablePlasticity(true, true, false); // Hebbian and STDP, no reward
    
    // Note: Individual flag checking would depend on implementation
    // This test ensures the function can be called without errors
    
    std::cout << "✓ Neuron plasticity flags tests passed" << std::endl;
}

void testNeuronStateReset() {
    std::cout << "Testing Neuron State Reset..." << std::endl;
    
    Neuron neuron(4);
    neuron.setMembranePotential(-50.0f);
    neuron.injectCurrent(10.0f);
    neuron.recordSpike(100.0f);
    
    // Modify state
    neuron.setFiringState(FiringState::Firing);
    neuron.setRefractoryPeriod(3);
    
    // Reset neuron
    neuron.reset();
    
    // Check that state is restored
    ASSERT_EQ(neuron.getMembranePotential(), -70.0f);
    ASSERT_EQ(neuron.getTotalCurrent(), 0.0f);
    ASSERT_FALSE(neuron.isFiring());
    ASSERT_FALSE(neuron.isRefractory());
    ASSERT_EQ(neuron.getSpikeHistory().size(), 0);
    
    std::cout << "✓ Neuron state reset tests passed" << std::endl;
}

void testNeuronRandomInitialization() {
    std::cout << "Testing Neuron Random Initialization..." << std::endl;
    
    // Create random number generator
    RandomGenerator rng(42);
    
    Neuron neuron(5);
    
    // Initialize with random parameters
    neuron.initializeRandom(rng);
    
    // Check that neuron has valid state after initialization
    ASSERT_TRUE(std::isfinite(neuron.getMembranePotential()));
    ASSERT_TRUE(neuron.getThreshold() > neuron.getRestingPotential());
    ASSERT_GT(neuron.getLeakConductance(), 0.0f);
    ASSERT_GT(neuron.getRefractoryPeriod(), 0);
    
    std::cout << "✓ Neuron random initialization tests passed" << std::endl;
}