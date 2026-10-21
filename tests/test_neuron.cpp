// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include "TestFramework.hpp"
#include <cassert>
#include <iostream>

// Register tests with the test framework
RUN_TEST_CATEGORY(testNeuronCreation, "Neuron")
{
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    nlm::test::assert::equal(neuron.getId(), nlm::NeuronId(1));
    nlm::test::assert::equal(neuron.getType(), nlm::NeuronType::Internal);
}

RUN_TEST_CATEGORY(testNeuronType, "Neuron")
{
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    nlm::test::assert::equal(neuron.getType(), nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    nlm::test::assert::equal(neuron.getType(), nlm::NeuronType::Inhibitory);
}

RUN_TEST_CATEGORY(testMembranePotential, "Neuron")
{
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    nlm::test::assert::equal(neuron.getMembranePotential(), -70.0f);  // Default value
    
    neuron.setMembranePotential(-65.0f);
    nlm::test::assert::equal(neuron.getMembranePotential(), -65.0f);
    
    neuron.addToMembranePotential(5.0f);
    nlm::test::assert::equal(neuron.getMembranePotential(), -60.0f);
}

RUN_TEST_CATEGORY(testFiringState, "Neuron")
{
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    nlm::test::assert::isFalse(neuron.isFiring());
    nlm::test::assert::isFalse(neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Active);
    nlm::test::assert::isTrue(neuron.isFiring());
    nlm::test::assert::isTrue(neuron.isRefractory());
    
    neuron.setRefractoryPeriod(10);
    nlm::test::assert::isTrue(neuron.isRefractory());
    nlm::test::assert::isFalse(neuron.isFiring());
}

RUN_TEST_CATEGORY(testRefractoryDecrement, "Neuron")
{
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(5);
    nlm::test::assert::isTrue(neuron.isRefractory());
    
    neuron.decrementRefractory();
    nlm::test::assert::isTrue(neuron.isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    nlm::test::assert::isFalse(neuron.isRefractory());
}

RUN_TEST_CATEGORY(testCurrentInjection, "Neuron")
{
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    nlm::test::assert::equal(neuron.getTotalCurrent(), 0.0f);
    
    neuron.injectCurrent(5.0f);
    nlm::test::assert::equal(neuron.getTotalCurrent(), 5.0f);
    
    neuron.clearTotalCurrent();
    nlm::test::assert::equal(neuron.getTotalCurrent(), 0.0f);
}

RUN_TEST_CATEGORY(testSynapticInput, "Neuron")
{
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.receiveExcitatoryInput(5.0f);
    nlm::test::assert::equal(neuron.getTotalCurrent(), 5.0f);
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    nlm::test::assert::equal(neuron.getTotalCurrent(), -3.0f);
}

RUN_TEST_CATEGORY(testSpikeHistory, "Neuron")
{
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    nlm::test::assert::isTrue(neuron.getSpikeHistory().empty());
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    nlm::test::assert::equal(neuron.getSpikeHistory().size(), 3);
    
    neuron.clearSpikeHistory();
    nlm::test::assert::isTrue(neuron.getSpikeHistory().empty());
}

RUN_TEST_CATEGORY(testRandomInitialization, "Neuron")
{
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.initializeRandom(rng);
    
    // Just verify it doesn't crash and values are in reasonable range
    nlm::test::assert::isTrue(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f);
    nlm::test::assert::isTrue(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f);
}

RUN_TEST_CATEGORY(testReset, "Neuron")
{
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    
    nlm::test::assert::equal(neuron.getMembranePotential(), -70.0f);
    nlm::test::assert::equal(neuron.getTotalCurrent(), 0.0f);
    nlm::test::assert::isTrue(neuron.getSpikeHistory().empty());
}

// Legacy test runner for backward compatibility
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
