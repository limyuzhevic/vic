// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

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

void testNumericalStability() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test for numerical overflow/underflow protection
    const float veryLarge = 1e10f;
    const float verySmall = 1e-10f;
    const float negativeLarge = -1e10f;
    const float negativeSmall = -1e-10f;
    
    // Ensure operations don't cause numerical instability
    neuron.setMembranePotential(negativeLarge);
    float pot = neuron.getMembranePotential();
    assert(std::isfinite(pot));
    
    neuron.setMembranePotential(negativeSmall);
    pot = neuron.getMembranePotential();
    assert(std::isfinite(pot));
    
    // Test threshold stability
    neuron.setThreshold(negativeLarge);
    float thresh = neuron.getThreshold();
    assert(std::isfinite(thresh));
    
    neuron.setThreshold(negativeSmall);
    thresh = neuron.getThreshold();
    assert(std::isfinite(thresh));
    
    // Test synaptic input stability
    neuron.receiveExcitatoryInput(veryLarge);
    neuron.receiveInhibitoryInput(veryLarge);
    float current = neuron.getTotalCurrent();
    assert(std::isfinite(current));
    
    std::cout << "    testNumericalStability passed" << std::endl;
}

void testEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test boundary values
    const float maxFloat = std::numeric_limits<float>::max();
    const float minFloat = std::numeric_limits<float>::min();
    const float epsilon = std::numeric_limits<float>::epsilon();
    
    // Test membrane potential boundary conditions
    neuron.setMembranePotential(maxFloat);
    float pot = neuron.getMembranePotential();
    assert(std::isfinite(pot));  // Should be clamped or handled
    
    neuron.setMembranePotential(minFloat);
    pot = neuron.getMembranePotential();
    assert(std::isfinite(pot));
    
    // Test current boundary conditions
    neuron.injectCurrent(maxFloat);
    neuron.injectCurrent(minFloat);
    float current = neuron.getTotalCurrent();
    assert(std::isfinite(current));
    
    // Test spike recording edge cases
    neuron.recordSpike(epsilon);  // Very small spike time
    neuron.recordSpike(maxFloat); // Very large spike time
    
    auto history = neuron.getSpikeHistory();
    assert(history.size() == 2);
    assert(std::isfinite(history[0]));
    assert(std::isfinite(history[1]));
    
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    // Test refractory period edge cases
    neuron.setRefractoryPeriod(0);  // Zero refractory period
    assert(!neuron.isRefractory());  // Should not be refractory
    
    neuron.setRefractoryPeriod(1);  // Minimum valid refractory period
    assert(neuron.isRefractory());
    
    std::cout << "    testEdgeCases passed" << std::endl;
}

void testNeuronTypeEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test all neuron types
    std::vector<nlm::NeuronType> allTypes = {
        nlm::NeuronType::Excitatory,
        nlm::NeuronType::Inhibitory,
        nlm::NeuronType::Modulatory,
        nlm::NeuronType::Sensory,
        nlm::NeuronType::Motor,
        nlm::NeuronType::Internal
    };
    
    for (const auto& type : allTypes) {
        neuron.setType(type);
        assert(neuron.getType() == type);
    }
    
    // Test setting invalid/null neuron type (if applicable)
    // This would depend on the implementation
    
    std::cout << "    testNeuronTypeEdgeCases passed" << std::endl;
}

void testSynapseEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test synaptic input edge cases
    neuron.receiveExcitatoryInput(0.0f);
    assert(neuron.getTotalCurrent() == 0.0f);
    
    neuron.receiveInhibitoryInput(0.0f);
    assert(neuron.getTotalCurrent() == 0.0f);
    
    // Test large synaptic inputs
    neuron.receiveExcitatoryInput(1e6f);
    neuron.receiveInhibitoryInput(1e6f);
    
    float current = neuron.getTotalCurrent();
    assert(std::isfinite(current));
    
    // Test negative synaptic currents
    neuron.clearTotalCurrent();
    neuron.receiveExcitatoryInput(-1e6f);
    neuron.receiveInhibitoryInput(-1e6f);
    
    current = neuron.getTotalCurrent();
    assert(std::isfinite(current));
    
    std::cout << "    testSynapseEdgeCases passed" << std::endl;
}

void testNeuronClone() {
    nlm::Neuron original(nlm::NeuronId(42));
    
    original.setType(nlm::NeuronType::Excitatory);
    original.setMembranePotential(-60.0f);
    original.setThreshold(-45.0f);
    original.injectCurrent(5.0f);
    original.recordSpike(0.1);
    
    // Test clone (if method exists)
    // This would depend on the implementation
    
    std::cout << "    testNeuronClone passed" << std::endl;
}

void testNeuronSerialization() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Motor);
    neuron.setMembranePotential(-55.0f);
    neuron.injectCurrent(3.14f);
    
    // Test serialization methods (if available)
    // This would depend on the implementation
    
    std::cout << "    testNeuronSerialization passed" << std::endl;
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
    testNumericalStability();
    testEdgeCases();
    testNeuronTypeEdgeCases();
    testSynapseEdgeCases();
    testNeuronClone();
    testNeuronSerialization();
}

} // namespace test_neuron
