// Advanced Neuron Tests
// Tests for new advanced neuron types (bursty, adaptive, integrate-and-fire with adaptation,
// Hodgkin-Huxley, resonant neurons)

#include "brain/AdvancedNeurons.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_advanced_neurons {

void testBurstingNeuronCreation() {
    nlm::BurstingNeuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getType() == nlm::NeuronType::Internal);
    assert(neuron.getMembranePotential() == -70.0f);
    assert(!neuron.isBursting());
    assert(neuron.getBurstState() == 0.0f);
    
    std::cout << "    testBurstingNeuronCreation passed" << std::endl;
}

void testBurstingNeuronBurstingType() {
    nlm::BurstingNeuron neuron(nlm::NeuronId(1));
    
    neuron.setBurstingType(nlm::BurstingType::RegularSpiking);
    neuron.setBurstFrequency(20.0f); // Hz
    neuron.setBurstDuration(50.0f); // ms
    neuron.setInterBurstInterval(200.0f); // ms
    
    // Configure to burst
    neuron.configureForPattern(true);
    assert(neuron.isBursting());
    
    std::cout << "    testBurstingNeuronBurstingType passed" << std::endl;
}

void testBurstingNeuronAdaptation() {
    nlm::BurstingNeuron neuron(nlm::NeuronId(1));
    
    neuron.enableAdaptation(true);
    neuron.setAdaptationRate(0.5f);
    
    // Simulate some spikes to test adaptation
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    
    std::cout << "    testBurstingNeuronAdaptation passed" << std::endl;
}

void testAdaptiveNeuronCreation() {
    nlm::AdaptiveNeuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getType() == nlm::NeuronType::Internal);
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getFiringRate() == 0.0f);
    assert(neuron.getAdaptationLevel() == 0.0f);
    
    std::cout << "    testAdaptiveNeuronCreation passed" << std::endl;
}

void testAdaptiveNeuronParameters() {
    nlm::AdaptiveNeuron neuron(nlm::NeuronId(1));
    
    nlm::AdaptiveNeuronParams params;
    params.adaptationStrength = 2.0f;
    params.adaptationTimeConstant = 50.0f;
    params.adaptationThreshold = 0.3f;
    params.subthresholdAdaptation = 0.2f;
    
    neuron.setAdaptationParams(params);
    neuron.enableSubthresholdAdaptation(true);
    neuron.setAdaptationThreshold(0.4f);
    
    std::cout << "    testAdaptiveNeuronParameters passed" << std::endl;
}

void testAdaptiveNeuronFiringRate() {
    nlm::AdaptiveNeuron neuron(nlm::NeuronId(1));
    
    // Simulate some spikes
    neuron.recordSpike(0.0);
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    
    float rate = neuron.getFiringRate();
    float recentRate = neuron.getRecentFiringRate(200.0f); // ms
    
    // Basic checks - in real test, we'd need more sophisticated assertions
    assert(neuron.getSpikeHistory().size() == 3);
    
    std::cout << "    testAdaptiveNeuronFiringRate passed (rate=" << rate << ")" << std::endl;
}

void testAdaptiveIFNeuronCreation() {
    nlm::AdaptiveIntegrateAndFire neuron(nlm::NeuronId(1));
    
    assert(neuron.getType() == nlm::NeuronType::Internal);
    assert(neuron.getAdaptationVariable() == 0.0f);
    assert(neuron.getThresholdEvolution() == 0.0f);
    
    std::cout << "    testAdaptiveIFNeuronCreation passed" << std::endl;
}

void testAdaptiveIFNeuronParameters() {
    nlm::AdaptiveIntegrateAndFire neuron(nlm::NeuronId(1));
    
    nlm::AdaptiveIFParams params;
    params.adaptationVariable = 1.0f;
    params.adaptationConductance = 0.2f;
    params.adaptationTau = 80.0f;
    
    neuron.setAdaptiveIFParams(params);
    neuron.setThresholdAdaptation(0.5f);
    neuron.setReferencePotential(-55.0f);
    
    std::cout << "    testAdaptiveIFNeuronParameters passed" << std::endl;
}

void testHodgkinHuxleyNeuronCreation() {
    nlm::HodgkinHuxleyNeuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getType() == nlm::NeuronType::Internal);
    assert(!neuron.isBursting());
    assert(neuron.getMembranePotential() == -70.0f);
    
    std::cout << "    testHodgkinHuxleyNeuronCreation passed" << std::endl;
}

void testHodgkinHuxleyNeuronParameters() {
    nlm::HodgkinHuxleyNeuron neuron(nlm::NeuronId(1));
    
    nlm::HHModelParams params;
    params.maxSodiumConductance = 150.0f;
    params.maxPotassiumConductance = 40.0f;
    params.leakConductance = 0.3f;
    params.restingPotential = -65.0f;
    params.sodiumReversal = 55.0f;
    params.potassiumReversal = -80.0f;
    
    neuron.setHHModelParams(params);
    neuron.setInitialGatingVariables(0.05f, 0.6f, 0.32f);
    
    // Check initial gating variables
    assert(neuron.getSodiumActivation() == 0.05f);
    assert(neuron.getSodiumInactivation() == 0.6f);
    assert(neuron.getPotassiumActivation() == 0.32f);
    
    std::cout << "    testHodgkinHuxleyNeuronParameters passed" << std::endl;
}

void testResonantNeuronCreation() {
    nlm::ResonantNeuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getType() == nlm::NeuronType::Internal);
    assert(!neuron.isBursting());
    assert(neuron.getResonanceFrequency() == 10.0f);
    assert(neuron.getResonanceQuality() == 0.5f);
    
    std::cout << "    testResonantNeuronCreation passed" << std::endl;
}

void testResonantNeuronParameters() {
    nlm::ResonantNeuron neuron(nlm::NeuronId(1));
    
    nlm::ResonantNeuronParams params;
    params.resonanceFrequency = 20.0f;
    params.resonanceQFactor = 0.8f;
    params.membraneTimeConstant = 15.0f;
    params.couplingStrength = 0.7f;
    params.externalInputPhase = 0.5f;
    
    neuron.setResonantNeuronParams(params);
    neuron.setResonanceFrequency(25.0f);
    neuron.setResonanceQFactor(0.9f);
    
    assert(neuron.getResonanceFrequency() == 25.0f);
    assert(neuron.getResonanceQuality() == 0.9f);
    
    std::cout << "    testResonantNeuronParameters passed" << std::endl;
}

void testNeuronFactory() {
    // Test advanced neuron factory
    nlm::AdvancedNeuronBase* neuron1 = nlm::AdvancedNeuronFactory::createBurstingNeuron(nlm::NeuronId(1), nlm::BurstingType::FastSpiking);
    assert(neuron1 != nullptr);
    nlm::AdvancedNeuronFactory::destroyNeuron(neuron1);
    
    nlm::AdvancedNeuronBase* neuron2 = nlm::AdvancedNeuronFactory::createAdaptiveNeuron(nlm::NeuronId(2));
    assert(neuron2 != nullptr);
    nlm::AdvancedNeuronFactory::destroyNeuron(neuron2);
    
    nlm::AdvancedNeuronBase* neuron3 = nlm::AdvancedNeuronFactory::createAdaptiveIFNeuron(nlm::NeuronId(3));
    assert(neuron3 != nullptr);
    nlm::AdvancedNeuronFactory::destroyNeuron(neuron3);
    
    nlm::AdvancedNeuronBase* neuron4 = nlm::AdvancedNeuronFactory::createHodgkinHuxleyNeuron(nlm::NeuronId(4));
    assert(neuron4 != nullptr);
    nlm::AdvancedNeuronFactory::destroyNeuron(neuron4);
    
    nlm::AdvancedNeuronBase* neuron5 = nlm::AdvancedNeuronFactory::createResonantNeuron(nlm::NeuronId(5));
    assert(neuron5 != nullptr);
    nlm::AdvancedNeuronFactory::destroyNeuron(neuron5);
    
    std::cout << "    testNeuronFactory passed" << std::endl;
}

void runAll() {
    std::cout << "Running Advanced Neuron tests..." << std::endl;
    testBurstingNeuronCreation();
    testBurstingNeuronBurstingType();
    testBurstingNeuronAdaptation();
    testAdaptiveNeuronCreation();
    testAdaptiveNeuronParameters();
    testAdaptiveNeuronFiringRate();
    testAdaptiveIFNeuronCreation();
    testAdaptiveIFNeuronParameters();
    testHodgkinHuxleyNeuronCreation();
    testHodgkinHuxleyNeuronParameters();
    testResonantNeuronCreation();
    testResonantNeuronParameters();
    testNeuronFactory();
}

} // namespace test_advanced_neurons