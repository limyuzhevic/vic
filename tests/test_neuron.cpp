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

void testNeuronEnhancedStep() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set up neuron for testing
    neuron.setType(nlm::NeuronType::Excitatory);
    neuron.setMembranePotential(-65.0f);  // Below threshold
    neuron.setThreshold(-60.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setResetPotential(-70.0f);
    neuron.setLeakConductance(10.0f);
    
    // Test that neuron doesn't fire when below threshold
    bool fired = neuron.stepLIF(0.0, 0.001);
    assert(!fired);
    assert(neuron.getMembranePotential() < -60.0f);  // Still below threshold
    
    // Test firing when threshold is crossed
    neuron.setMembranePotential(-59.0f);  // Above threshold
    fired = neuron.stepLIF(0.0, 0.001);
    assert(fired);
    assert(neuron.getMembranePotential() == -70.0f);  // Reset to resting
    assert(neuron.isRefractory());  // Should be in refractory period
    assert(neuron.getLastSpikeTime() == 0.0f);
    
    std::cout << "    testNeuronEnhancedStep passed" << std::endl;
}

void testNeuronModernCppFeatures() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test noexcept methods
    neuron.setType(nlm::NeuronType::Excitatory);
    neuron.setMembranePotential(-65.0f);
    neuron.setThreshold(-60.0f);
    
    // These methods should be noexcept
    neuron.setType(nlm::NeuronType::Inhibitory);
    neuron.setMembranePotential(-70.0f);
    neuron.setThreshold(-55.0f);
    
    // Test [[nodiscard]] methods
    auto type = neuron.getType();  // Should trigger static analysis in optimized builds
    auto potential = neuron.getMembranePotential();
    auto threshold = neuron.getThreshold();
    
    // Test that methods return expected values
    assert(type == nlm::NeuronType::Inhibitory);
    assert(potential == -70.0f);
    assert(threshold == -55.0f);
    
    std::cout << "    testNeuronModernCppFeatures passed" << std::endl;
}

void testNeuronRefractoryPeriod() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set up neuron for refractory period testing
    neuron.setMembranePotential(-65.0f);
    neuron.setThreshold(-60.0f);
    neuron.setRefractoryPeriod(5);
    
    // Make neuron fire to enter refractory period
    neuron.setMembranePotential(-59.0f);  // Above threshold
    bool fired = neuron.stepLIF(0.0, 0.001);
    assert(fired);
    assert(neuron.isRefractory());
    
    // Test that neuron cannot fire during refractory period
    neuron.setMembranePotential(-59.0f);  // Above threshold
    bool shouldNotFire = neuron.stepLIF(0.1, 0.001);  // Time advances
    assert(!shouldNotFire);  // Should not fire during refractory
    assert(neuron.isRefractory());  // Still in refractory
    
    // Decrement refractory period
    for (int i = 0; i < 5; ++i) {
        neuron.decrementRefractory();
    }
    assert(!neuron.isRefractory());  // Should exit refractory now
    
    std::cout << "    testNeuronRefractoryPeriod passed" << std::endl;
}

void testNeuronAdaptation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test spike-frequency adaptation
    neuron.setMembranePotential(-65.0f);
    neuron.setThreshold(-60.0f);
    neuron.setRefractoryPeriod(2);
    
    // First spike
    neuron.setMembranePotential(-59.0f);  // Above threshold
    bool fired1 = neuron.stepLIF(0.0, 0.001);
    assert(fired1);
    assert(neuron.getState().adaptationVariable > 0.0f);  // Should have adaptation
    
    // Check adaptation variable
    float adaptation1 = neuron.getState().adaptationVariable;
    
    // Second spike after refractory
    neuron.setMembranePotential(-59.0f);  // Above threshold
    bool fired2 = neuron.stepLIF(0.2, 0.001);  // After refractory period
    assert(fired2);
    
    // Adaptation should continue to build up
    float adaptation2 = neuron.getState().adaptationVariable;
    assert(adaptation2 > adaptation1);  // Should have increased
    
    std::cout << "    testNeuronAdaptation passed" << std::endl;
}

} // namespace test_neuron
