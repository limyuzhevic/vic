// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include "core/Exception.hpp"
#include <iostream>

namespace test_neuron {

void testNeuronCreation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    if (neuron.getId() != nlm::NeuronId(1)) {
        throw nlm::NLMTestException("Neuron creation test failed: ID should be 1, got " + 
                                   std::to_string(neuron.getId().getId()));
    }
    if (neuron.getType() != nlm::NeuronType::Internal) {
        throw nlm::NLMTestException("Neuron creation test failed: type should be Internal, got " + 
                                   std::to_string(static_cast<int>(neuron.getType())));
    }
    
    std::cout << "    testNeuronCreation passed" << std::endl;
}

void testNeuronType() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    if (neuron.getType() != nlm::NeuronType::Excitatory) {
        throw nlm::NLMTestException("Neuron type test failed: type should be Excitatory after set, got " + 
                                   std::to_string(static_cast<int>(neuron.getType())));
    }
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    if (neuron.getType() != nlm::NeuronType::Inhibitory) {
        throw nlm::NLMTestException("Neuron type test failed: type should be Inhibitory after set, got " + 
                                   std::to_string(static_cast<int>(neuron.getType())));
    }
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testMembranePotential() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    if (neuron.getMembranePotential() != -70.0f) {  // Default value
        throw nlm::NLMTestException("Neuron membrane potential test failed: default should be -70.0, got " + 
                                   std::to_string(neuron.getMembranePotential()));
    }
    
    neuron.setMembranePotential(-65.0f);
    if (neuron.getMembranePotential() != -65.0f) {
        throw nlm::NLMTestException("Neuron membrane potential test failed: after set should be -65.0, got " + 
                                   std::to_string(neuron.getMembranePotential()));
    }
    
    neuron.addToMembranePotential(5.0f);
    if (neuron.getMembranePotential() != -60.0f) {
        throw nlm::NLMTestException("Neuron membrane potential test failed: after add should be -60.0, got " + 
                                   std::to_string(neuron.getMembranePotential()));
    }
    
    std::cout << "    testMembranePotential passed" << std::endl;
}

void testFiringState() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    if (neuron.isFiring()) {
        throw nlm::NLMTestException("Neuron firing state test failed: should not be firing initially");
    }
    if (neuron.isRefractory()) {
        throw nlm::NLMTestException("Neuron firing state test failed: should not be refractory initially");
    }
    
    neuron.setFiringState(nlm::FiringState::Active);
    if (!neuron.isFiring()) {
        throw nlm::NLMTestException("Neuron firing state test failed: should be firing after setActive");
    }
    
    neuron.setRefractoryPeriod(10);
    if (!neuron.isRefractory()) {
        throw nlm::NLMTestException("Neuron firing state test failed: should be refractory after setRefractoryPeriod");
    }
    if (neuron.isFiring()) {
        throw nlm::NLMTestException("Neuron firing state test failed: should not be firing when refractory");
    }
    
    std::cout << "    testFiringState passed" << std::endl;
}

void testRefractoryDecrement() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(5);
    if (!neuron.isRefractory()) {
        throw nlm::NLMTestException("Neuron refractory decrement test failed: should be refractory after set");
    }
    
    neuron.decrementRefractory();
    if (!neuron.isRefractory()) {
        throw nlm::NLMTestException("Neuron refractory decrement test failed: should still be refractory after first decrement");
    }
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    if (neuron.isRefractory()) {
        throw nlm::NLMTestException("Neuron refractory decrement test failed: should not be refractory after decrements");
    }
    
    std::cout << "    testRefractoryDecrement passed" << std::endl;
}

void testCurrentInjection() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    if (neuron.getTotalCurrent() != 0.0f) {
        throw nlm::NLMTestException("Neuron current injection test failed: initial total current should be 0.0, got " + 
                                   std::to_string(neuron.getTotalCurrent()));
    }
    
    neuron.injectCurrent(5.0f);
    if (neuron.getTotalCurrent() != 5.0f) {
        throw nlm::NLMTestException("Neuron current injection test failed: after inject should be 5.0, got " + 
                                   std::to_string(neuron.getTotalCurrent()));
    }
    
    neuron.clearTotalCurrent();
    if (neuron.getTotalCurrent() != 0.0f) {
        throw nlm::NLMTestException("Neuron current injection test failed: after clear should be 0.0, got " + 
                                   std::to_string(neuron.getTotalCurrent()));
    }
    
    std::cout << "    testCurrentInjection passed" << std::endl;
}

void testSynapticInput() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.receiveExcitatoryInput(5.0f);
    if (neuron.getTotalCurrent() != 5.0f) {
        throw nlm::NLMTestException("Neuron synaptic input test failed: after excitatory input should be 5.0, got " + 
                                   std::to_string(neuron.getTotalCurrent()));
    }
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    if (neuron.getTotalCurrent() != -3.0f) {
        throw nlm::NLMTestException("Neuron synaptic input test failed: after inhibitory input should be -3.0, got " + 
                                   std::to_string(neuron.getTotalCurrent()));
    }
    
    std::cout << "    testSynapticInput passed" << std::endl;
}

void testSpikeHistory() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    if (!neuron.getSpikeHistory().empty()) {
        throw nlm::NLMTestException("Neuron spike history test failed: spike history should be empty initially");
    }
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    if (neuron.getSpikeHistory().size() != 3) {
        throw nlm::NLMTestException("Neuron spike history test failed: spike history size should be 3, got " + 
                                   std::to_string(neuron.getSpikeHistory().size()));
    }
    
    neuron.clearSpikeHistory();
    if (!neuron.getSpikeHistory().empty()) {
        throw nlm::NLMTestException("Neuron spike history test failed: spike history should be empty after clear");
    }
    
    std::cout << "    testSpikeHistory passed" << std::endl;
}

void testRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.initializeRandom(rng);
    
    // Just verify it doesn't crash and values are in reasonable range
    if (neuron.getMembranePotential() < -80.0f || neuron.getMembranePotential() > -60.0f) {
        throw nlm::NLMTestException("Neuron random initialization test failed: membrane potential " + 
                                   std::to_string(neuron.getMembranePotential()) + " not in reasonable range [-80, -60]");
    }
    if (neuron.getThreshold() < -60.0f || neuron.getThreshold() > -50.0f) {
        throw nlm::NLMTestException("Neuron random initialization test failed: threshold " + 
                                   std::to_string(neuron.getThreshold()) + " not in reasonable range [-60, -50]");
    }
    
    std::cout << "    testRandomInitialization passed" << std::endl;
}

void testReset() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    
    if (neuron.getMembranePotential() != -70.0f) {
        throw nlm::NLMTestException("Neuron reset test failed: membrane potential after reset should be -70.0, got " + 
                                   std::to_string(neuron.getMembranePotential()));
    }
    if (neuron.getTotalCurrent() != 0.0f) {
        throw nlm::NLMTestException("Neuron reset test failed: total current after reset should be 0.0, got " + 
                                   std::to_string(neuron.getTotalCurrent()));
    }
    if (!neuron.getSpikeHistory().empty()) {
        throw nlm::NLMTestException("Neuron reset test failed: spike history should be empty after reset");
    }
    
    std::cout << "    testReset passed" << std::endl;
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
}

} // namespace test_neuron
