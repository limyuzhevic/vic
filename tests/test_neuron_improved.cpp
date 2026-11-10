// Neuron Tests - Updated with modern testing framework
#include "tests/nlm_test_framework.hpp"
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"

namespace test_neuron {

void testNeuronCreation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    ASSERT(neuron.getId() == nlm::NeuronId(1), "Neuron should have correct ID");
    ASSERT(neuron.getType() == nlm::NeuronType::Internal, "Neuron should have default internal type");
}

void testNeuronType() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    ASSERT(neuron.getType() == nlm::NeuronType::Excitatory, "Neuron type should be excitatory");
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    ASSERT(neuron.getType() == nlm::NeuronType::Inhibitory, "Neuron type should be inhibitory");
}

void testMembranePotential() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    ASSERT(neuron.getMembranePotential() == -70.0f, "Neuron should have default membrane potential");
    
    neuron.setMembranePotential(-65.0f);
    ASSERT(neuron.getMembranePotential() == -65.0f, "Neuron membrane potential should be -65.0f");
    
    neuron.addToMembranePotential(5.0f);
    ASSERT(neuron.getMembranePotential() == -60.0f, "Neuron membrane potential should be -60.0f after adding 5.0f");
}

void testFiringState() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    ASSERT(!neuron.isFiring(), "Neuron should not be firing initially");
    ASSERT(!neuron.isRefractory(), "Neuron should not be refractory initially");
    
    neuron.setFiringState(nlm::FiringState::Active);
    ASSERT(neuron.isFiring(), "Neuron should be firing when set to active");
    
    neuron.setRefractoryPeriod(10);
    ASSERT(neuron.isRefractory(), "Neuron should be refractory when period > 0");
    ASSERT(!neuron.isFiring(), "Neuron should not be firing when refractory");
}

void testRefractoryDecrement() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(5);
    ASSERT(neuron.isRefractory(), "Neuron should be refractory with period 5");
    
    neuron.decrementRefractory();
    ASSERT(neuron.isRefractory(), "Neuron should still be refractory after first decrement");
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    ASSERT(!neuron.isRefractory(), "Neuron should not be refractory after 5 decrements");
}

void testCurrentInjection() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    ASSERT(neuron.getTotalCurrent() == 0.0f, "Neuron should have no total current initially");
    
    neuron.injectCurrent(5.0f);
    ASSERT(neuron.getTotalCurrent() == 5.0f, "Neuron should have total current after injection");
    
    neuron.clearTotalCurrent();
    ASSERT(neuron.getTotalCurrent() == 0.0f, "Neuron should have no total current after clear");
}

void testSynapticInput() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.receiveExcitatoryInput(5.0f);
    ASSERT(neuron.getTotalCurrent() == 5.0f, "Excitatory input should increase current");
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    ASSERT(neuron.getTotalCurrent() == -3.0f, "Inhibitory input should decrease current");
}

void testSpikeHistory() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    ASSERT(neuron.getSpikeHistory().empty(), "Neuron should have empty spike history initially");
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    ASSERT(neuron.getSpikeHistory().size() == 3, "Neuron should have 3 spikes in history");
    
    neuron.clearSpikeHistory();
    ASSERT(neuron.getSpikeHistory().empty(), "Neuron should have empty spike history after clear");
}

void testRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.initializeRandom(rng);
    
    // Verify values are in reasonable range (based on neuron implementation)
    ASSERT(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f, 
           "Membrane potential should be in valid range");
    ASSERT(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f, 
           "Threshold should be in valid range");
}

void testReset() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    
    ASSERT(neuron.getMembranePotential() == -70.0f, "Neuron reset should reset membrane potential to default");
    ASSERT(neuron.getTotalCurrent() == 0.0f, "Neuron reset should clear current");
    ASSERT(neuron.getSpikeHistory().empty(), "Neuron reset should clear spike history");
}

void runAllTests() {
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

// Auto-register test suite
NLM_TEST_MAIN()