// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include "core/test/TestFramework.hpp"
#include <iostream>

namespace test_neuron {

void testNeuronCreation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    nlm::test::TestAssert::equal(1ULL, neuron.getId().value, "testNeuronCreation");
    nlm::test::TestAssert::equal(nlm::NeuronType::Internal, neuron.getType(), "testNeuronCreation");
    
    std::cout << "    testNeuronCreation passed" << std::endl;
}

void testNeuronType() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    nlm::test::TestAssert::equal(nlm::NeuronType::Excitatory, neuron.getType(), "testNeuronType");
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    nlm::test::TestAssert::equal(nlm::NeuronType::Inhibitory, neuron.getType(), "testNeuronType");
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testMembranePotential() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    nlm::test::TestAssert::equal(-70.0f, neuron.getMembranePotential(), "testMembranePotential");  // Default value
    
    neuron.setMembranePotential(-65.0f);
    nlm::test::TestAssert::equal(-65.0f, neuron.getMembranePotential(), "testMembranePotential");
    
    neuron.addToMembranePotential(5.0f);
    nlm::test::TestAssert::equal(-60.0f, neuron.getMembranePotential(), "testMembranePotential");
    
    std::cout << "    testMembranePotential passed" << std::endl;
}

void testFiringState() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    nlm::test::TestAssert::equal(false, neuron.isFiring(), "testFiringState");
    nlm::test::TestAssert::equal(false, neuron.isRefractory(), "testFiringState");
    
    neuron.setFiringState(nlm::FiringState::Active);
    nlm::test::TestAssert::equal(true, neuron.isFiring(), "testFiringState");
    
    neuron.setRefractoryPeriod(10);
    nlm::test::TestAssert::equal(true, neuron.isRefractory(), "testFiringState");
    nlm::test::TestAssert::equal(false, neuron.isFiring(), "testFiringState");
    
    std::cout << "    testFiringState passed" << std::endl;
}

void testRefractoryDecrement() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(5);
    nlm::test::TestAssert::equal(true, neuron.isRefractory(), "testRefractoryDecrement");
    
    neuron.decrementRefractory();
    nlm::test::TestAssert::equal(true, neuron.isRefractory(), "testRefractoryDecrement");
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    nlm::test::TestAssert::equal(false, neuron.isRefractory(), "testRefractoryDecrement");
    
    std::cout << "    testRefractoryDecrement passed" << std::endl;
}

void testCurrentInjection() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    nlm::test::TestAssert::equal(0.0f, neuron.getTotalCurrent(), "testCurrentInjection");
    
    neuron.injectCurrent(5.0f);
    nlm::test::TestAssert::equal(5.0f, neuron.getTotalCurrent(), "testCurrentInjection");
    
    neuron.clearTotalCurrent();
    nlm::test::TestAssert::equal(0.0f, neuron.getTotalCurrent(), "testCurrentInjection");
    
    std::cout << "    testCurrentInjection passed" << std::endl;
}

void testSynapticInput() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.receiveExcitatoryInput(5.0f);
    nlm::test::TestAssert::equal(5.0f, neuron.getTotalCurrent(), "testSynapticInput");
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    nlm::test::TestAssert::equal(-3.0f, neuron.getTotalCurrent(), "testSynapticInput");
    
    std::cout << "    testSynapticInput passed" << std::endl;
}

void testSpikeHistory() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    nlm::test::TestAssert::equal(true, neuron.getSpikeHistory().empty(), "testSpikeHistory");
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    nlm::test::TestAssert::equal(3, neuron.getSpikeHistory().size(), "testSpikeHistory");
    
    neuron.clearSpikeHistory();
    nlm::test::TestAssert::equal(true, neuron.getSpikeHistory().empty(), "testSpikeHistory");
    
    std::cout << "    testSpikeHistory passed" << std::endl;
}

void testRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.initializeRandom(rng);
    
    // Just verify it doesn't crash and values are in reasonable range
    nlm::test::TestAssert::greaterEqual(-80.0f, neuron.getMembranePotential(), "testRandomInitialization");
    nlm::test::TestAssert::lessEqual(-60.0f, neuron.getMembranePotential(), "testRandomInitialization");
    nlm::test::TestAssert::greaterEqual(-60.0f, neuron.getThreshold(), "testRandomInitialization");
    nlm::test::TestAssert::lessEqual(-50.0f, neuron.getThreshold(), "testRandomInitialization");
    
    std::cout << "    testRandomInitialization passed" << std::endl;
}

void testReset() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    
    nlm::test::TestAssert::equal(-70.0f, neuron.getMembranePotential(), "testReset");
    nlm::test::TestAssert::equal(0.0f, neuron.getTotalCurrent(), "testReset");
    nlm::test::TestAssert::equal(true, neuron.getSpikeHistory().empty(), "testReset");
    
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
