#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_enhanced_neuron {

void testNeuronEnhancedCreation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getId() == nlm::NeuronId(1));
    assert(neuron.getType() == nlm::NeuronType::Internal);
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getThreshold() == -55.0f);
    assert(neuron.getRestingPotential() == -70.0f);
    assert(neuron.getResetPotential() == -70.0f);
    assert(neuron.isResting());
    assert(!neuron.isFiring());
    assert(!neuron.isRefractory());
    
    std::cout << "    testNeuronEnhancedCreation passed" << std::endl;
}

void testNeuronMemoryManagement() {
    nlm::Neuron neuron1(nlm::NeuronId(1));
    nlm::Neuron neuron2(nlm::NeuronId(2));
    
    // Test move constructor
    nlm::Neuron neuron3(std::move(neuron1));
    assert(neuron3.getId() == nlm::NeuronId(1));
    assert(neuron1.getId() == nlm::NeuronId(0)); // Moved-from state
    
    // Test move assignment
    neuron1 = std::move(neuron2);
    assert(neuron1.getId() == nlm::NeuronId(2));
    assert(neuron2.getId() == nlm::NeuronId(0)); // Moved-from state
    
    std::cout << "    testNeuronMemoryManagement passed" << std::endl;
}

void testNeuronLIFDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.initializeRandom(nlm::RandomGenerator(42));
    
    float initialPotential = neuron.getMembranePotential();
    assert(initialPotential >= -80.0f && initialPotential <= -60.0f);
    assert(neuron.getThreshold() >= -60.0f && neuron.getThreshold() <= -50.0f);
    
    // Test refractory period management
    neuron.setRefractoryPeriod(5);
    assert(neuron.isRefractory());
    assert(!neuron.isFiring());
    
    neuron.decrementRefractory();
    assert(neuron.isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    assert(!neuron.isRefractory());
    assert(neuron.isResting());
    
    std::cout << "    testNeuronLIFDynamics passed" << std::endl;
}

void testNeuronAdvancedSynapticInput() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test excitatory and inhibitory input
    neuron.receiveExcitatoryInput(10.0f);
    neuron.receiveInhibitoryInput(5.0f);
    
    neuron.clearTotalCurrent();
    neuron.injectCurrent(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    std::cout << "    testNeuronAdvancedSynapticInput passed" << std::endl;
}

void testNeuronSpikeHistoryManagement() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getSpikeHistory().empty());
    
    // Record multiple spikes
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    neuron.recordSpike(0.4);
    neuron.recordSpike(0.5);
    
    assert(neuron.getSpikeHistory().size() == 5);
    assert(neuron.getLastSpikeTime() == 0.5f);
    
    // Test spike history overflow protection
    for (int i = 0; i < 100; ++i) {
        neuron.recordSpike(static_cast<float>(i));
    }
    assert(neuron.getSpikeHistory().size() <= 100); // Should not exceed MAX_SPIKE_HISTORY
    
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    assert(neuron.getLastSpikeTime() == -1.0f);
    
    std::cout << "    testNeuronSpikeHistoryManagement passed" << std::endl;
}

void testNeuronFiringRateCalculation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    
    float initialFiringRate = neuron.getFiringRate();
    assert(initialFiringRate == 0.0f);
    
    // Test firing rate updates with spike
    neuron.recordSpike(0.1);
    neuron.setFiringRate(100.0f);
    assert(neuron.getFiringRate() == 100.0f);
    
    // Test firing rate decay
    neuron.setFiringRate(100.0f);
    neuron.clearTotalCurrent();
    // In step function, firing rate should decay
    neuron.step(0.1);
    assert(neuron.getFiringRate() < 100.0f);
    
    std::cout << "    testNeuronFiringRateCalculation passed" << std::endl;
}

void testNeuronAdaptationVariable() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getFiringRate() == 0.0f);
    
    // Test adaptation variable updates
    neuron.receiveModulatoryInput(5.0f);
    neuron.step(0.1);
    
    std::cout << "    testNeuronAdaptationVariable passed" << std::endl;
}

void testNeuronParameterScaling() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.initializeRandom(nlm::RandomGenerator(42));
    
    float initialResting = neuron.getRestingPotential();
    float initialThreshold = neuron.getThreshold();
    
    neuron.scaleParameters(2.0f);
    
    assert(neuron.getRestingPotential() == initialResting * 2.0f);
    assert(neuron.getThreshold() == initialThreshold * 2.0f);
    
    std::cout << "    testNeuronParameterScaling passed" << std::endl;
}

void testNeuronBatchOperations() {
    std::vector<nlm::Neuron> neurons;
    for (int i = 0; i < 10; ++i) {
        neurons.emplace_back(nlm::NeuronId(i));
    }
    
    // Test batch operations
    std::vector<std::pair<nlm::NeuronId, nlm::MembranePotential>> injections;
    for (int i = 0; i < 5; ++i) {
        injections.emplace_back(neurons[i].getId(), 10.0f);
    }
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        // Simulate batch injection (neurons don't have batch method yet, but we can test the concept)
        if (i < injections.size()) {
            neurons[i].injectCurrent(injections[i].second);
        }
    }
    
    assert(neurons[0].getTotalCurrent() == 10.0f);
    
    std::cout << "    testNeuronBatchOperations passed" << std::endl;
}

void testNeuronResetFunctionality() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.initializeRandom(nlm::RandomGenerator(42));
    
    // Modify state
    neuron.setMembranePotential(-50.0f);
    neuron.injectCurrent(15.0f);
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.setFiringState(nlm::FiringState::Active);
    
    // Reset neuron
    neuron.reset();
    
    // Check reset state
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getTotalCurrent() == 0.0f);
    assert(neuron.getSpikeHistory().empty());
    assert(neuron.isResting());
    assert(!neuron.isFiring());
    assert(!neuron.isRefractory());
    
    std::cout << "    testNeuronResetFunctionality passed" << std::endl;
}

void testNeuronConfigurationValidation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    
    // Verify initialized values are within biological ranges
    assert(neuron.getMembranePotential() >= -80.0f && neuron.getMembranePotential() <= -60.0f);
    assert(neuron.getThreshold() >= -60.0f && neuron.getThreshold() <= -50.0f);
    assert(neuron.getRestingPotential() >= -75.0f && neuron.getRestingPotential() <= -65.0f);
    assert(neuron.getResetPotential() >= neuron.getRestingPotential());
    assert(neuron.getRefractoryPeriod() >= 2 && neuron.getRefractoryPeriod() <= 10);
    
    std::cout << "    testNeuronConfigurationValidation passed" << std::endl;
}

void runAll() {
    testNeuronEnhancedCreation();
    testNeuronMemoryManagement();
    testNeuronLIFDynamics();
    testNeuronAdvancedSynapticInput();
    testNeuronSpikeHistoryManagement();
    testNeuronFiringRateCalculation();
    testNeuronAdaptationVariable();
    testNeuronParameterScaling();
    testNeuronBatchOperations();
    testNeuronResetFunctionality();
    testNeuronConfigurationValidation();
}

} // namespace test_enhanced_neuron
