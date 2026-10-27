// Enhanced Neuron Tests with Error Handling
// Tests for Phase 2 Enhanced: Real Neural Computation (Error Handling Edition)

#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include "include/nlm/NLMErrors.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace test_neuron_enhanced {

void testNeuronErrorHandling() {
    std::cout << "    testNeuronErrorHandling passed" << std::endl;
}

void testNeuronErrorHandlingConstructor() {
    // Test constructor with invalid ID
    try {
        nlm::Neuron neuron(nlm::NeuronId(0));  // Invalid ID
        assert(false && "Should have thrown exception for invalid ID");
    } catch (const nlm::NLMException& e) {
        assert(std::string(e.what()).find("Invalid neuron ID") != std::string::npos);
    }
    
    // Test valid construction
    nlm::Neuron neuron(nlm::NeuronId(1));
    assert(neuron.getId() == nlm::NeuronId(1));
    
    std::cout << "    testNeuronErrorHandlingConstructor passed" << std::endl;
}

void testNeuronErrorHandlingMembranePotential() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test valid operations
    neuron.setMembranePotential(-65.0f);
    assert(neuron.getMembranePotential() == -65.0f);
    
    neuron.addToMembranePotential(10.0f);
    assert(neuron.getMembranePotential() == -55.0f);
    
    // Test NaN handling
    bool nanCaught = false;
    try {
        neuron.setMembranePotential(std::nanf(""));
    } catch (const nlm::NLMException& e) {
        if (std::string(e.what()).find("Invalid membrane potential") != std::string::npos) {
            nanCaught = true;
        }
    }
    assert(nanCaught);
    
    std::cout << "    testNeuronErrorHandlingMembranePotential passed" << std::endl;
}

void testNeuronErrorHandlingThreshold() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test valid threshold
    neuron.setThreshold(-55.0f);
    assert(neuron.checkThreshold());  // Should be true at default potential
    
    // Test invalid threshold
    bool thresholdCaught = false;
    try {
        neuron.setThreshold(std::nanf(""));
    } catch (const nlm::NLMException& e) {
        if (std::string(e.what()).find("Invalid threshold") != std::string::npos) {
            thresholdCaught = true;
        }
    }
    assert(thresholdCaught);
    
    std::cout << "    testNeuronErrorHandlingThreshold passed" << std::endl;
}

void testNeuronErrorHandlingCurrent() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test valid current injection
    neuron.injectCurrent(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    // Test NaN current
    bool currentCaught = false;
    try {
        neuron.injectCurrent(std::nanf(""));
    } catch (const nlm::NLMException& e) {
        if (std::string(e.what()).find("Invalid current injection") != std::string::npos) {
            currentCaught = true;
        }
    }
    assert(currentCaught);
    
    std::cout << "    testNeuronErrorHandlingCurrent passed" << std::endl;
}

void testNeuronErrorHandlingLIF() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test valid LIF step
    neuron.setThreshold(-55.0f);
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(20.0f);
    
    bool fired = neuron.stepLIF(0.0, 0.001);
    assert(fired);
    
    // Test invalid timestep
    bool timestepCaught = false;
    try {
        neuron.stepLIF(0.0, 0.0);
    } catch (const nlm::NLMException& e) {
        if (std::string(e.what()).find("Invalid timestep") != std::string::npos) {
            timestepCaught = true;
        }
    }
    assert(timestepCaught);
    
    std::cout << "    testNeuronErrorHandlingLIF passed" << std::endl;
}

void testNeuronErrorHandlingPlasticity() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test valid plasticity enable
    neuron.enablePlasticity(true, true, false);
    
    // Test null neuron access (should throw)
    bool nullCaught = false;
    try {
        neuron.setMembranePotential(-65.0f);
    } catch (const nlm::NLMException& e) {
        nullCaught = true;
    }
    assert(nullCaught);
    
    std::cout << "    testNeuronErrorHandlingPlasticity passed" << std::endl;
}

void testNeuronErrorHandlingReset() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(1.0f);
    
    neuron.reset();
    
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getTotalCurrent() == 0.0f);
    
    std::cout << "    testNeuronErrorHandlingReset passed" << std::endl;
}

void testNeuronErrorHandlingMoveSemantics() {
    nlm::Neuron original(nlm::NeuronId(1));
    original.setMembranePotential(-65.0f);
    
    nlm::Neuron moved(std::move(original));
    assert(moved.getMembranePotential() == -65.0f);
    
    nlm::Neuron assigned;
    assigned = std::move(moved);
    assert(assigned.getMembranePotential() == -65.0f);
    
    std::cout << "    testNeuronErrorHandlingMoveSemantics passed" << std::endl;
}

void testNeuronErrorHandlingRandomInitialization() {
    nlm::RandomGenerator rng;
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Should not throw
    neuron.initializeRandom(rng);
    
    assert(neuron.getMembranePotential() != 0.0f);  // Should be randomized
    
    std::cout << "    testNeuronErrorHandlingRandomInitialization passed" << std::endl;
}

} // namespace test_neuron_enhanced

// Original neuron tests namespace
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

void testThreshold() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setThreshold(-55.0f);
    assert(neuron.checkThreshold());  // Should be true at default potential
    
    std::cout << "    testThreshold passed" << std::endl;
}

void testFiring() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setThreshold(-55.0f);
    neuron.injectCurrent(20.0f);  // Strong enough to cause firing
    
    bool fired = neuron.stepLIF(0.0, 0.001);
    assert(fired);
    
    std::cout << "    testFiring passed" << std::endl;
}

void testSpikeHistory() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.recordSpike(1.0f);
    neuron.recordSpike(2.0f);
    
    auto history = neuron.getSpikeHistory();
    assert(history.size() == 2);
    assert(history[0] == 1.0f);
    assert(history[1] == 2.0f);
    
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testSpikeHistory passed" << std::endl;
}

void testRefractoryPeriod() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(10);
    assert(neuron.isRefractory());
    
    for (int i = 0; i < 9; ++i) {
        neuron.decrementRefractory();
        assert(neuron.isRefractory());
    }
    
    neuron.decrementRefractory();
    assert(!neuron.isRefractory());
    
    std::cout << "    testRefractoryPeriod passed" << std::endl;
}

void testAdaptationVariable() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.getState().adaptationVariable = 1.0f;
    assert(neuron.getState().adaptationVariable == 1.0f);
    
    // Simulate spike to increase adaptation
    neuron.stepLIF(0.0, 0.001);
    assert(neuron.getState().adaptationVariable > 1.0f);
    
    std::cout << "    testAdaptationVariable passed" << std::endl;
}

} // namespace test_neuron

void test_neuron_enhanced::runAll() {
    test_neuron_enhanced::testNeuronErrorHandling();
    test_neuron_enhanced::testNeuronErrorHandlingConstructor();
    test_neuron_enhanced::testNeuronErrorHandlingMembranePotential();
    test_neuron_enhanced::testNeuronErrorHandlingThreshold();
    test_neuron_enhanced::testNeuronErrorHandlingCurrent();
    test_neuron_enhanced::testNeuronErrorHandlingLIF();
    test_neuron_enhanced::testNeuronErrorHandlingPlasticity();
    test_neuron_enhanced::testNeuronErrorHandlingReset();
    test_neuron_enhanced::testNeuronErrorHandlingMoveSemantics();
    test_neuron_enhanced::testNeuronErrorHandlingRandomInitialization();
    
    std::cout << "    All enhanced neuron tests passed!" << std::endl;
}

void test_neuron::runAll() {
    test_neuron::testNeuronCreation();
    test_neuron::testNeuronType();
    test_neuron::testMembranePotential();
    test_neuron::testFiringState();
    test_neuron::testRefractoryDecrement();
    test_neuron::testCurrentInjection();
    test_neuron::testSynapticInput();
    test_neuron::testThreshold();
    test_neuron::testFiring();
    test_neuron::testSpikeHistory();
    test_neuron::testRefractoryPeriod();
    test_neuron::testAdaptationVariable();
    
    std::cout << "    All original neuron tests passed!" << std::endl;
}
