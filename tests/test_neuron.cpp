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

void testReset() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getTotalCurrent() == 0.0f);
    assert(neuron.getSpikeHistory().size() == 0);
    
    std::cout << "    testReset passed" << std::endl;
}

void testLIFDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setMembranePotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setLeakConductance(10.0f);
    
    // Single timestep - should remain at resting potential without input
    neuron.step(0.0);
    assert(neuron.getMembranePotential() < -69.0f);  // Leak should bring it closer to resting
    
    // Inject current to drive neuron toward threshold
    neuron.setMembranePotential(-70.0f);
    neuron.injectCurrent(10.0f);
    neuron.step(0.0);
    
    std::cout << "    testLIFDynamics passed" << std::endl;
}

void testSpikingDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setMembranePotential(-70.0f);
    neuron.setThreshold(-50.0f);
    neuron.setRefractoryPeriod(5);
    neuron.setResetPotential(-70.0f);
    
    // Inject current until threshold is reached
    neuron.injectCurrent(30.0f);
    bool fired = neuron.step(1.0);
    assert(fired == true);  // Should have spiked
    
    assert(neuron.isFiring() == true);
    assert(neuron.isRefractory() == true);
    assert(neuron.getMembranePotential() == -70.0f);  // Should be reset
    
    // During refractory period, shouldn't spike
    neuron.injectCurrent(30.0f);
    bool firedDuringRefractory = neuron.step(2.0);
    assert(firedDuringRefractory == false);
    
    // Test refractory decay
    for (int i = 0; i < 6; ++i) {
        neuron.step(static_cast<double>(3.0 + i));
    }
    assert(neuron.isRefractory() == false);
    assert(neuron.isFiring() == false);
    
    std::cout << "    testSpikingDynamics passed" << std::endl;
}

void testSynapticIntegration() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setMembranePotential(-70.0f);
    
    // Test excitatory input
    neuron.receiveExcitatoryInput(5.0f);
    neuron.setMembranePotential(-70.0f);
    neuron.step(0.0);
    
    // Test inhibitory input
    neuron.setMembranePotential(-70.0f);
    neuron.receiveInhibitoryInput(3.0f);
    neuron.step(0.0);
    
    std::cout << "    testSynapticIntegration passed" << std::endl;
}

void testPlasticityFlags() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(!neuron.getPlasticityFlags().hebbian);
    assert(!neuron.getPlasticityFlags().stdp);
    assert(!neuron.getPlasticityFlags().reward_modulated);
    
    neuron.enablePlasticity(true, true, false);
    
    assert(neuron.getPlasticityFlags().hebbian);
    assert(neuron.getPlasticityFlags().stdp);
    assert(!neuron.getPlasticityFlags().reward_modulated);
    
    neuron.enablePlasticity(false, false, true);
    assert(!neuron.getPlasticityFlags().hebbian);
    assert(!neuron.getPlasticityFlags().stdp);
    assert(neuron.getPlasticityFlags().reward_modulated);
    
    std::cout << "    testPlasticityFlags passed" << std::endl;
}

void testFiringRate() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setFiringRate(0.0f);
    assert(neuron.getFiringRate() == 0.0f);
    
    neuron.setFiringRate(10.5f);
    assert(neuron.getFiringRate() == 10.5f);
    
    neuron.setFiringRate(0.0f);
    neuron.recordSpike(1.0);
    neuron.recordSpike(2.0);
    neuron.recordSpike(3.0);
    
    // Check spike history size
    auto history = neuron.getSpikeHistory();
    assert(history.size() == 3);
    assert(history[0] == 1.0f);
    assert(history[1] == 2.0f);
    assert(history[2] == 3.0f);
    
    std::cout << "    testFiringRate passed" << std::endl;
}

void testLIFParameters() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test default values
    assert(neuron.getLeakConductance() == 10.0f);
    assert(neuron.getRefractoryPeriod() == 5);
    assert(neuron.getRestingPotential() == -70.0f);
    assert(neuron.getResetPotential() == -70.0f);
    assert(neuron.getThreshold() == -55.0f);
    
    // Test parameter modifications
    neuron.setLeakConductance(15.0f);
    assert(neuron.getLeakConductance() == 15.0f);
    
    neuron.setRefractoryPeriod(10);
    assert(neuron.getRefractoryPeriod() == 10);
    
    neuron.setRestingPotential(-75.0f);
    assert(neuron.getRestingPotential() == -75.0f);
    
    neuron.setResetPotential(-80.0f);
    assert(neuron.getResetPotential() == -80.0f);
    
    neuron.setThreshold(-50.0f);
    assert(neuron.getThreshold() == -50.0f);
    
    std::cout << "    testLIFParameters passed" << std::endl;
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
    testLIFDynamics();
    testSpikingDynamics();
    testSynapticIntegration();
    testPlasticityFlags();
    testFiringRate();
    testLIFParameters();
}

} // namespace test_neuron
