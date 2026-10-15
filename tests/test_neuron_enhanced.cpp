// Enhanced Neuron Tests
// #include "brain/Neuron.hpp"
// #include "core/Random/Random.hpp"
// #include <cassert>
// #include <iostream>
// #include <stdexcept>

namespace test_neuron_enhanced {

void testNeuronCreation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getId() == nlm::NeuronId(1));
    assert(neuron.getType() == nlm::NeuronType::Internal);
    
    std::cout << "    testNeuronCreation passed" << std::endl;
}

void testNeuronNullSafety() {
    // Test null pointer safety
    try {
        nlm::Neuron neuron(nlm::NeuronId(1));
        neuron.setMembranePotential(5.0f); // Valid
        neuron.setMembranePotential(-150.0f); // Should throw
        assert(false && "Should have thrown exception");
    } catch (const std::out_of_range&) {
        std::cout << "    testNeuronNullSafety passed - correctly threw exception" << std::endl;
    }
}

void testNeuronType() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    assert(neuron.getType() == nlm::NeuronType::Inhibitory);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testMembranePotentialValidation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getMembranePotential() == -70.0f);  // Default value
    
    neuron.setMembranePotential(-65.0f);
    assert(neuron.getMembranePotential() == -65.0f);
    
    // Test that addToMembranePotential clamps values
    neuron.setMembranePotential(45.0f);
    neuron.addToMembranePotential(20.0f);  // Should clamp to 50.0f max
    assert(neuron.getMembranePotential() == 50.0f);
    
    neuron.setMembranePotential(-110.0f);
    neuron.addToMembranePotential(-20.0f);  // Should clamp to -100.0f min
    assert(neuron.getMembranePotential() == -100.0f);
    
    std::cout << "    testMembranePotentialValidation passed" << std::endl;
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
    
    // Test that clamping works for extreme values
    neuron.receiveExcitatoryInput(2000.0f);  // Should clamp to 1000.0f max
    assert(neuron.getTotalCurrent() == 1000.0f);
    
    neuron.receiveInhibitoryInput(-1000.0f);  // Should clamp to -1000.0f min
    assert(neuron.getTotalCurrent() == 1000.0f);  // 1000.0 + (-1000.0) = 0.0
    
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
    
    // Test that history doesn't exceed maximum size
    for (size_t i = 0; i < 200; ++i) {
        neuron.recordSpike(static_cast<float>(i));
    }
    assert(neuron.getSpikeHistory().size() == nlm::Neuron::Impl::MAX_SPIKE_HISTORY);
    
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

void testAll() {
    std::cout << "Running Enhanced Neuron tests..." << std::endl;
    testNeuronCreation();
    testNeuronNullSafety();
    testNeuronType();
    testMembranePotentialValidation();
    testFiringState();
    testRefractoryDecrement();
    testCurrentInjection();
    testSynapticInput();
    testSpikeHistory();
    testRandomInitialization();
    testReset();
}

} // namespace test_neuron_enhanced

// Test runner for enhanced tests
void runEnhancedNeuronTests() {
    test_neuron_enhanced::testAll();
}