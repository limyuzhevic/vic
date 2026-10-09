// Advanced Neuron Test Suite
// Tests for the refactored Neuron class with advanced biophysical dynamics
// Follows the existing NLM testing framework

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_advanced_neuron {

void testNeuronInitialization() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    assert(neuron.getState().neuronType == nlm::NeuronType::Excitatory);
    assert(neuron.getState().membraneCapacitance > 0.0f);
    assert(neuron.getState().timeConstant > 0.0f);
    
    nlm::RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    
    // Verify initialization values are in reasonable ranges
    assert(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f);
    assert(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f);
    
    std::cout << "    testNeuronInitialization passed" << std::endl;
}

void testAdvancedRefractoryDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    
    // Set advanced refractory properties
    neuron.setRefractoryProperties(3, 2);
    
    // Simulate spike to enter refractory period
    Timestamp spikeTime = 100.0;
    bool fired = neuron.stepLIF(spikeTime, 0.001);
    assert(fired);
    
    // Check that neuron is in refractory state
    assert(neuron.getState().refractoryRemaining > 0);
    assert(neuron.getState().firingState == nlm::FiringState::Refractory);
    
    // Check refractory recovery
    float recovery = neuron.getRefractoryRecovery();
    assert(recovery < 1.0f);
    
    // Simulate recovery
    neuron.updateRefractoryRecovery(0.001);
    assert(neuron.getRefractoryRecovery() > recovery);
    
    std::cout << "    testAdvancedRefractoryDynamics passed" << std::endl;
}

void testAdaptationCurrentsIntegration() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    
    // Set adaptation parameters
    neuron.setAdaptationParameters(100.0f, 10.0f, -60.0f, -50.0f);
    
    // Simulate spike train
    Timestamp baseTime = 0.0;
    for (int i = 0; i < 10; ++i) {
        neuron.stepLIF(baseTime + i * 0.001, 0.001);
    }
    
    // Check adaptation variable
    assert(neuron.getState().adaptationVariable > 0.0f);
    
    // Update adaptation currents
    neuron.updateAdaptationCurrents(neuron.getMembranePotential());
    
    // Check adaptation currents are calculated
    float adaptationCurrents = neuron.getAdaptationCurrents();
    assert(adaptationCurrents >= 0.0f);
    assert(std::isfinite(adaptationCurrents));
    
    std::cout << "    testAdaptationCurrentsIntegration passed" << std::endl;
}

void testSynapticConductanceDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    
    // Set synaptic properties
    neuron.setSynapticProperties(5.0f, 100.0f, 10.0f, 100.0f);
    
    float dt = 0.001f;
    
    // Initial conductances should be zero
    assert(neuron.getState().ampaConductance == 0.0f);
    assert(neuron.getState().nmdaConductance == 0.0f);
    assert(neuron.getState().GABAaConductance == 0.0f);
    assert(neuron.getState().gabaBConductance == 0.0f);
    
    // Update synaptic conductances
    neuron.updateSynapticConductances(dt);
    
    // Check conductances remain non-negative
    assert(neuron.getState().ampaConductance >= 0.0f);
    assert(neuron.getState().nmdaConductance >= 0.0f);
    assert(neuron.getState().GABAaConductance >= 0.0f);
    assert(neuron.getState().gabaBConductance >= 0.0f);
    
    // Simulate multiple updates
    for (int i = 0; i < 20; ++i) {
        neuron.updateSynapticConductances(dt);
    }
    
    // Verify stability
    assert(std::isfinite(neuron.getState().ampaConductance));
    assert(std::isfinite(neuron.getState().nmdaConductance));
    assert(std::isfinite(neuron.getState().GABAaConductance));
    assert(std::isfinite(neuron.getState().gabaBConductance));
    
    std::cout << "    testSynapticConductanceDynamics passed" << std::endl;
}

void testStochasticDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    
    // Set stochastic properties
    neuron.setStochasticProperties(1.0f, 0.0f, 5.0f);
    
    float dt = 0.001f;
    
    // Run simulation with different random seeds
    for (int seed = 0; seed < 3; ++seed) {
        nlm::RandomGenerator seedRng(seed);
        
        // Create a temporary neuron for this seed
        nlm::Neuron testNeuron(nlm::NeuronId(seed + 2));
        testNeuron.setType(neuron.getType());
        testNeuron.initializeRandom(seedRng);
        testNeuron.setStochasticProperties(1.0f, 0.0f, 5.0f);
        
        // Update stochastic dynamics
        testNeuron.updateStochasticDynamics(dt, seedRng);
        
        // Check that membrane potential is within reasonable bounds
        nlm::MembranePotential V = testNeuron.getMembranePotential();
        assert(V >= neuron.getState().restingPotential - 20.0f);
        assert(V <= neuron.getState().threshold + 20.0f);
        assert(std::isfinite(V));
    }
    
    std::cout << "    testStochasticDynamics passed" << std::endl;
}

void testShortTermPlasticity() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    
    float dt = 0.001f;
    
    // Initial STP values
    assert(neuron.getState().facilitation == 0.0f);
    assert(neuron.getState().depression == 0.0f);
    
    // Update STP
    neuron.updateShortTermPlasticity(dt);
    
    // Check values remain in valid range
    float facilitation = neuron.getSTPValue();
    assert(facilitation >= 0.0f);
    assert(facilitation <= 1.0f);
    assert(std::isfinite(facilitation));
    
    // Run multiple updates
    for (int i = 0; i < 50; ++i) {
        neuron.updateShortTermPlasticity(dt);
        
        float depression = neuron.getSTPValue();
        assert(depression >= 0.0f);
        assert(depression <= 1.0f);
        assert(std::isfinite(depression));
    }
    
    std::cout << "    testShortTermPlasticity passed" << std::endl;
}

void testResetAndPostInhibitoryCurrents() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    
    // Record spike to simulate neural activity
    neuron.recordSpike(0.0);
    
    // Update reset and post-inhibitory currents
    neuron.updateResetCurrents();
    neuron.updatePostInhibitoryCurrents();
    
    // Check that currents are in valid ranges
    assert(neuron.getState().resetCurrent >= 0.0f);
    assert(neuron.getState().postInhibitoryCurrent >= 0.0f);
    assert(std::isfinite(neuron.getState().resetCurrent));
    assert(std::isfinite(neuron.getState().postInhibitoryCurrent));
    
    std::cout << "    testResetAndPostInhibitoryCurrents passed" << std::endl;
}

void testNeuronTypeSpecificConfigurations() {
    // Test different neuron type configurations
    nlm::NeuronConfig excitatoryConfig = nlm::NeuronConfig::createExcitatoryNeuronConfig();
    nlm::NeuronConfig inhibitoryConfig = nlm::NeuronConfig::createInhibitoryNeuronConfig();
    nlm::NeuronConfig sensoryConfig = nlm::NeuronConfig::createSensoryNeuronConfig();
    nlm::NeuronConfig motorConfig = nlm::NeuronConfig::createMotorNeuronConfig();
    nlm::NeuronConfig internalConfig = nlm::NeuronConfig::createInternalNeuronConfig();
    
    // Verify type-specific properties
    assert(excitatoryConfig.membraneCapacitance > 0.0f);
    assert(inhibitoryConfig.membraneCapacitance < excitatoryConfig.membraneCapacitance);
    assert(sensoryConfig.membraneCapacitance > motorConfig.membraneCapacitance);
    
    // Test type-specific refractory periods
    assert(excitatoryConfig.excitatoryRefractoryPeriod > 0);
    assert(inhibitoryConfig.inhibitoryRefractoryPeriod > 0);
    assert(sensoryConfig.sensoryRefractoryPeriod > excitatoryConfig.excitatoryRefractoryPeriod);
    
    std::cout << "    testNeuronTypeSpecificConfigurations passed" << std::endl;
}

void testConfigurationValidation() {
    // Test valid configurations
    nlm::NeuronConfig validConfig = nlm::NeuronConfig::createDefaultConfig(nlm::NeuronType::Excitatory);
    assert(nlm::NeuronConfig::validateConfig(validConfig));
    
    // Test that configurations are in reasonable ranges
    assert(validConfig.membraneCapacitance > 0.0f);
    assert(validConfig.timeConstant > 0.0f);
    assert(validConfig.absoluteRefractorySteps > 0);
    assert(validConfig.relativeRefractorySteps > 0);
    
    // Test type-specific validation
    nlm::NeuronConfig excitatory = nlm::NeuronConfig::createExcitatoryNeuronConfig();
    nlm::NeuronConfig inhibitory = nlm::NeuronConfig::createInhibitoryNeuronConfig();
    
    assert(nlm::NeuronConfig::validateConfig(excitatory));
    assert(nlm::NeuronConfig::validateConfig(inhibitory));
    
    std::cout << "    testConfigurationValidation passed" << std::endl;
}

void runAll() {
    std::cout << "Running Advanced Neuron Tests..." << std::endl;
    
    testNeuronInitialization();
    testAdvancedRefractoryDynamics();
    testAdaptationCurrentsIntegration();
    testSynapticConductanceDynamics();
    testStochasticDynamics();
    testShortTermPlasticity();
    testResetAndPostInhibitoryCurrents();
    testNeuronTypeSpecificConfigurations();
    testConfigurationValidation();
    
    std::cout << "Advanced Neuron tests completed!" << std::endl;
}

} // namespace test_advanced_neuron
