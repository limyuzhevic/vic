// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>

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

void testNumericalIntegration() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Configure neuron for integration testing
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    
    // Test exponential Euler integration (current implementation)
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    
    // Step with exponential Euler
    bool fired1 = neuron.stepExponentialEuler(0.0, 0.001);
    assert(!fired1);  // Should not fire at -60mV with threshold -55mV
    
    // Test Runge-Kutta integration
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    
    bool fired2 = neuron.stepRK4(0.0, 0.001);
    assert(!fired2);
    
    // Test basic Euler integration
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    
    bool fired3 = neuron.stepEuler(0.0, 0.001);
    assert(!fired3);
    
    std::cout << "    testNumericalIntegration passed" << std::endl;
}

void testSpikeGeneration() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set up neuron to fire
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    neuron.setRefractoryPeriod(10);
    
    // Start at threshold
    neuron.setMembranePotential(-55.0f);
    neuron.injectCurrent(1.0f);  // Small current to trigger spike
    
    // Step with sufficient current to cross threshold
    bool fired = neuron.stepExponentialEuler(0.0, 0.001);
    assert(fired);  // Should fire
    
    // Verify spike was recorded
    assert(neuron.getSpikeHistory().size() == 1);
    assert(neuron.getLastSpikeTime() == 0.0f);
    
    // Verify neuron is now in refractory state
    assert(neuron.isRefractory());
    assert(!neuron.isFiring());
    
    std::cout << "    testSpikeGeneration passed" << std::endl;
}

void testNumericalIntegrationMethods() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    neuron.setRefractoryPeriod(10);
    
    // Test all three integration methods produce similar results
    neuron.setMembranePotential(-65.0f);
    neuron.injectCurrent(2.0f);
    
    // Record membrane potential before each step
    float v1 = neuron.getMembranePotential();
    bool f1 = neuron.stepExponentialEuler(0.0, 0.001);
    float v2 = neuron.getMembranePotential();
    
    neuron.setMembranePotential(-65.0f);
    neuron.injectCurrent(2.0f);
    bool f2 = neuron.stepRK4(0.0, 0.001);
    float v3 = neuron.getMembranePotential();
    
    neuron.setMembranePotential(-65.0f);
    neuron.injectCurrent(2.0f);
    bool f3 = neuron.stepEuler(0.0, 0.001);
    float v4 = neuron.getMembranePotential();
    
    // All methods should produce similar changes (within numerical tolerance)
    assert(std::abs(v2 - v3) < 0.01f);  // RK4 and exponential Euler should be close
    assert(std::abs(v2 - v4) < 0.05f);  // Basic Euler may be less accurate
    
    std::cout << "    testNumericalIntegrationMethods passed" << std::endl;
}

void testConfigurationUtilities() {
    // Test configuration utilities using Config class
    nlm::Config config;
    
    // Test safe getters with defaults
    assert(config.getSafeString("missing_key", "default_value") == "default_value");
    assert(config.getSafeInt("missing_int", 42) == 42);
    assert(config.getSafeFloat("missing_float", 3.14f) == 3.14f);
    assert(config.getSafeBool("missing_bool", true) == true);
    
    // Test configuration validation
    bool configValid = nlm::Neuron::configureDefaultParameters(std::make_shared<nlm::Config>());
    assert(configValid);
    
    std::cout << "    testConfigurationUtilities passed" << std::endl;
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
    testNumericalIntegration();
    testSpikeGeneration();
    testNumericalIntegrationMethods();
    testConfigurationUtilities();
}

} // namespace test_neuron
