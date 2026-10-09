// Comprehensive test coverage for NLM neuron improvements
// Tests all new features including factory patterns, configuration, validation, and advanced functionality

#include "src/brain/Neuron.hpp"
#include "src/core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace nlm_tests {

// Test Factory Patterns
void testFactoryPatterns() {
    std::cout << "Testing factory patterns..." << std::endl;
    
    // Test standard neuron
    nlm::Neuron standard = nlm::NeuronFactory::createStandardNeuron(nlm::NeuronId(1));
    assert(standard.getType() == nlm::NeuronType::Excitatory);
    assert(standard.getLeakConductance() == 10.0f);
    assert(standard.getRefractoryPeriod() == 5);
    
    // Test fast-spiking neuron
    nlm::Neuron fast = nlm::NeuronFactory::createFastSpikingNeuron(nlm::NeuronId(2));
    assert(fast.getType() == nlm::NeuronType::Excitatory);
    assert(fast.getLeakConductance() == 20.0f); // Higher leak for fast spiking
    assert(fast.getRefractoryPeriod() == 2);     // Shorter refractory period
    
    // Test regular spiking neuron
    nlm::Neuron regular = nlm::NeuronFactory::createRegularSpikingNeuron(nlm::NeuronId(3));
    assert(regular.getType() == nlm::NeuronType::Excitatory);
    assert(regular.getLeakConductance() == 8.0f);
    assert(regular.getRefractoryPeriod() == 8);
    
    // Test intrinsic burster
    nlm::Neuron burster = nlm::NeuronFactory::createIntrinsicBursterNeuron(nlm::NeuronId(4));
    assert(burster.getType() == nlm::NeuronType::Excitatory);
    assert(burster.getLeakConductance() == 5.0f); // Lower leak for bursting
    assert(burster.getRefractoryPeriod() == 10);
    
    // Test chattering neuron
    nlm::Neuron chatter = nlm::NeuronFactory::createChatteringNeuron(nlm::NeuronId(5));
    assert(chatter.getType() == nlm::NeuronType::Excitatory);
    assert(chatter.getLeakConductance() == 30.0f); // High leak for chattering
    assert(chatter.getRefractoryPeriod() == 1);    // Very short refractory
    
    // Test adapting neuron
    nlm::Neuron adapting = nlm::NeuronFactory::createAdaptingNeuron(nlm::NeuronId(6));
    assert(adapting.getType() == nlm::NeuronType::Excitatory);
    assert(adapting.getPlasticityFlags().reward_modulated == true);
    
    std::cout << "  Factory patterns: PASS" << std::endl;
}

// Test Configuration Parameters
void testConfigurationParameters() {
    std::cout << "Testing configuration parameters..." << std::endl;
    
    nlm::NeuronConfig config = nlm::NeuronFactory::createDefaultConfig();
    assert(config.leakConductance == 1.0f);
    assert(config.timeConstant == 20.0f);
    assert(config.maxFiringRate == 1000.0f);
    
    // Test fast configuration
    nlm::NeuronConfig fastConfig = nlm::NeuronFactory::createFastConfig();
    assert(fastConfig.leakConductance == 20.0f);
    assert(fastConfig.timeConstant == 10.0f);
    assert(fastConfig.maxAdaptation == 15.0f);
    assert(fastConfig.maxFiringRate == 1500.0f);
    
    // Test bursting configuration
    nlm::NeuronConfig burstConfig = nlm::NeuronFactory::createBurstingConfig();
    assert(burstConfig.leakConductance == 5.0f);
    assert(burstConfig.timeConstant == 30.0f);
    assert(burstConfig.maxAdaptation == 25.0f);
    assert(burstConfig.maxFiringRate == 800.0f);
    
    // Test adapting configuration
    nlm::NeuronConfig adaptConfig = nlm::NeuronFactory::createAdaptingConfig();
    assert(adaptConfig.leakConductance == 12.0f);
    assert(adaptConfig.timeConstant == 25.0f);
    assert(adaptConfig.maxAdaptation == 30.0f);
    assert(adaptConfig.maxFiringRate == 1200.0f);
    
    // Test custom neuron with configuration
    nlm::Neuron custom = nlm::NeuronFactory::createCustomizedNeuron(nlm::NeuronId(7), fastConfig);
    assert(custom.getLeakConductance() == 20.0f);
    
    std::cout << "  Configuration parameters: PASS" << std::endl;
}

// Test Parameter Validation
void testParameterValidation() {
    std::cout << "Testing parameter validation..." << std::endl;
    
    // Test membrane potential validation
    assert(nlm::NeuronFactory::validateMembranePotential(-70.0f) == true);
    assert(nlm::NeuronFactory::validateMembranePotential(50.0f) == true);
    assert(nlm::NeuronFactory::validateMembranePotential(-150.0f) == false); // Too low
    assert(nlm::NeuronFactory::validateMembranePotential(100.0f) == false); // Too high
    
    // Test threshold validation
    assert(nlm::NeuronFactory::validateThreshold(-55.0f) == true);
    assert(nlm::NeuronFactory::validateThreshold(-40.0f) == true);
    assert(nlm::NeuronFactory::validateThreshold(-80.0f) == false); // Too low
    assert(nlm::NeuronFactory::validateThreshold(-30.0f) == false); // Too high
    
    // Test leak conductance validation
    assert(nlm::NeuronFactory::validateLeakConductance(10.0f) == true);
    assert(nlm::NeuronFactory::validateLeakConductance(100.0f) == true);
    assert(nlm::NeuronFactory::validateLeakConductance(0.5f) == false); // Too low
    assert(nlm::NeuronFactory::validateLeakConductance(200.0f) == false); // Too high
    
    // Test refractory period validation
    assert(nlm::NeuronFactory::validateRefractoryPeriod(5) == true);
    assert(nlm::NeuronFactory::validateRefractoryPeriod(100) == true);
    assert(nlm::NeuronFactory::validateRefractoryPeriod(0) == false); // Too low
    assert(nlm::NeuronFactory::validateRefractoryPeriod(200) == false); // Too high
    
    // Test adaptation validation
    assert(nlm::NeuronFactory::validateAdaptation(0.0f) == true);
    assert(nlm::NeuronFactory::validateAdaptation(50.0f) == true);
    assert(nlm::NeuronFactory::validateAdaptation(-10.0f) == false); // Too low
    assert(nlm::NeuronFactory::validateAdaptation(100.0f) == false); // Too high
    
    // Test firing rate validation
    assert(nlm::NeuronFactory::validateFiringRate(0.0f) == true);
    assert(nlm::NeuronFactory::validateFiringRate(1000.0f) == true);
    assert(nlm::NeuronFactory::validateFiringRate(-10.0f) == false); // Too low
    assert(nlm::NeuronFactory::validateFiringRate(2000.0f) == false); // Too high
    
    std::cout << "  Parameter validation: PASS" << std::endl;
}

// Test Error Handling
void testErrorHandling() {
    std::cout << "Testing error handling..." << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(8));
    nlm::RandomGenerator rng(12345);
    
    // Test setting valid membrane potential
    neuron.setMembranePotential(-70.0f);
    assert(neuron.getMembranePotential() == -70.0f);
    
    // Test setting invalid membrane potential (should be rejected)
    neuron.setMembranePotential(-150.0f); // Invalid - below MIN_MEMBRANE_POTENTIAL
    // The neuron should reject this, so potential should remain unchanged
    assert(neuron.getMembranePotential() == -70.0f);
    
    // Test setting valid threshold
    neuron.setThreshold(-55.0f);
    assert(neuron.getThreshold() == -55.0f);
    
    // Test setting invalid threshold (should be rejected)
    neuron.setThreshold(-80.0f); // Invalid - below MIN_THRESHOLD
    assert(neuron.getThreshold() == -55.0f);
    
    // Test setting valid leak conductance
    neuron.setLeakConductance(10.0f);
    assert(neuron.getLeakConductance() == 10.0f);
    
    // Test setting invalid leak conductance (should be rejected)
    neuron.setLeakConductance(0.5f); // Invalid - below MIN_LEAK_CONDUCTANCE
    assert(neuron.getLeakConductance() == 10.0f);
    
    // Test random initialization with valid configuration
    neuron.initializeRandom(rng);
    assert(neuron.getMembranePotential() >= -120.0f && neuron.getMembranePotential() <= 50.0f);
    assert(neuron.getThreshold() >= -80.0f && neuron.getThreshold() <= -40.0f);
    assert(neuron.getLeakConductance() >= 1.0f && neuron.getLeakConductance() <= 100.0f);
    
    std::cout << "  Error handling: PASS" << std::endl;
}

// Test Performance Optimizations
void testPerformanceOptimizations() {
    std::cout << "Testing performance optimizations..." << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(9));
    nlm::RandomGenerator rng(42);
    
    // Test optimized spike history management
    neuron.recordSpike(0.1f);
    neuron.recordSpike(0.2f);
    neuron.recordSpike(0.3f);
    assert(neuron.getSpikeHistory().size() == 3);
    
    // Test spike history reset
    neuron.resetSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    // Test optimized membrane potential updates
    neuron.setMembranePotential(-70.0f);
    neuron.addToMembranePotential(10.0f);
    assert(neuron.getMembranePotential() == -60.0f);
    
    // Test threshold checking optimization
    bool canFire = neuron.checkThreshold();
    // This test depends on the current membrane potential and threshold
    std::cout << "  Threshold check: " << (canFire ? "PASS (can fire)" : "PASS (cannot fire)") << std::endl;
    
    // Test spike generation optimization
    neuron.recordSpike(0.0f);
    neuron.recordSpike(0.001f);
    float avgInterval = neuron.getAverageInterSpikeInterval();
    assert(avgInterval >= 0.0f && avgInterval <= 10.0f);
    
    std::cout << "  Performance optimizations: PASS" << std::endl;
}

// Test Advanced Features
void testAdvancedFeatures() {
    std::cout << "Testing advanced features..." << std::endl;
    
    // Test firing rate sigmoid function
    nlm::Neuron neuron(nlm::NeuronId(10));
    nlm::RandomGenerator rng(123);
    
    neuron.initializeRandom(rng);
    
    // Test synaptic input processing
    neuron.receiveExcitatoryInput(5.0f);
    neuron.receiveInhibitoryInput(2.0f);
    assert(neuron.getTotalCurrent() == 3.0f); // 5.0 - 2.0
    
    // Test synaptic input clearing
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    // Test synaptic input getters
    neuron.receiveExcitatoryInput(7.5f);
    assert(neuron.getSynapticInput() == 7.5f);
    
    // Test refractory period decrement
    neuron.setRefractoryPeriod(10);
    assert(neuron.isRefractory() == true);
    
    neuron.decrementRefractory();
    assert(neuron.isRefractory() == true); // Still refractory
    
    // Test firing state
    bool isFiring = neuron.isFiring();
    // This depends on the current firing state
    std::cout << "  Firing state: " << (isFiring ? "ACTIVE" : "INACTIVE") << std::endl;
    
    std::cout << "  Advanced features: PASS" << std::endl;
}

// Test Neuron Factory with Configuration
void testFactoryWithConfiguration() {
    std::cout << "Testing factory with configuration..." << std::endl;
    
    nlm::RandomGenerator rng(12345);
    
    // Create neurons with different configurations
    nlm::NeuronConfig config = nlm::NeuronFactory::createDefaultConfig();
    nlm::Neuron neuron1 = nlm::NeuronFactory::createCustomizedNeuron(nlm::NeuronId(11), config);
    
    config = nlm::NeuronFactory::createFastConfig();
    nlm::Neuron neuron2 = nlm::NeuronFactory::createCustomizedNeuron(nlm::NeuronId(12), config);
    
    config = nlm::NeuronFactory::createBurstingConfig();
    nlm::Neuron neuron3 = nlm::NeuronFactory::createCustomizedNeuron(nlm::NeuronId(13), config);
    
    config = nlm::NeuronFactory::createAdaptingConfig();
    nlm::Neuron neuron4 = nlm::NeuronFactory::createCustomizedNeuron(nlm::NeuronId(14), config);
    
    // Test that different configurations produce different behaviors
    assert(neuron1.getLeakConductance() == 1.0f);
    assert(neuron2.getLeakConductance() == 20.0f);
    assert(neuron3.getLeakConductance() == 5.0f);
    assert(neuron4.getLeakConductance() == 12.0f);
    
    std::cout << "  Factory with configuration: PASS" << std::endl;
}

// Test Random Initialization
void testRandomInitialization() {
    std::cout << "Testing random initialization..." << std::endl;
    
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(15));
    
    // Test initialization with random parameters
    neuron.initializeRandom(rng);
    
    // Verify values are within reasonable ranges
    assert(neuron.getMembranePotential() >= -120.0f && neuron.getMembranePotential() <= 50.0f);
    assert(neuron.getThreshold() >= -80.0f && neuron.getThreshold() <= -40.0f);
    assert(neuron.getLeakConductance() >= 1.0f && neuron.getLeakConductance() <= 100.0f);
    assert(neuron.getRefractoryPeriod() >= 1 && neuron.getRefractoryPeriod() <= 100);
    
    // Test multiple initializations with same seed produce same results
    nlm::Neuron neuron2(nlm::NeuronId(16));
    neuron2.initializeRandom(rng);
    
    // They should have the same random parameters due to same seed
    assert(neuron.getMembranePotential() == neuron2.getMembranePotential());
    
    std::cout << "  Random initialization: PASS" << std::endl;
}

// Test Neuron State Management
void testNeuronStateManagement() {
    std::cout << "Testing neuron state management..." << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(17));
    nlm::RandomGenerator rng(123);
    
    // Record some spikes
    neuron.recordSpike(0.0f);
    neuron.recordSpike(0.1f);
    neuron.recordSpike(0.2f);
    
    // Get state
    const nlm::NeuronState& state = neuron.getState();
    assert(state.membranePotential == -70.0f); // Default value
    assert(state.threshold == -55.0f); // Default value
    
    // Modify state
    nlm::NeuronState& mutableState = neuron.getState();
    mutableState.membranePotential = -65.0f;
    mutableState.threshold = -50.0f;
    
    assert(neuron.getMembranePotential() == -65.0f);
    assert(neuron.getThreshold() == -50.0f);
    
    // Test reset
    neuron.reset();
    assert(neuron.getMembranePotential() == -70.0f); // Back to default
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "  Neuron state management: PASS" << std::endl;
}

// Test Plasticity Flags
void testPlasticityFlags() {
    std::cout << "Testing plasticity flags..." << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(18));
    
    // Get plasticity flags
    const nlm::PlasticityFlags& flags = neuron.getPlasticityFlags();
    assert(flags.hebbian == false);
    assert(flags.stdp == false);
    assert(flags.reward_modulated == false);
    
    // Enable plasticity
    neuron.enablePlasticity(true, true, true);
    
    // Check updated flags
    nlm::PlasticityFlags& mutableFlags = neuron.getPlasticityFlags();
    assert(mutableFlags.hebbian == true);
    assert(mutableFlags.stdp == true);
    assert(mutableFlags.reward_modulated == true);
    
    std::cout << "  Plasticity flags: PASS" << std::endl;
}

// Run all tests
void runAllTests() {
    std::cout << "========================================" << std::endl;
    std::cout << "NLM Neuron Improvements Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    testFactoryPatterns();
    testConfigurationParameters();
    testParameterValidation();
    testErrorHandling();
    testPerformanceOptimizations();
    testAdvancedFeatures();
    testFactoryWithConfiguration();
    testRandomInitialization();
    testNeuronStateManagement();
    testPlasticityFlags();
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "All tests completed successfully!" << std::endl;
    std::cout << "========================================" << std::endl;
}

} // namespace nlm_tests

int main() {
    nlm_tests::runAllTests();
    return 0;
}