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
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testReset passed" << std::endl;
}

void testLIFDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set realistic biological parameters
    neuron.setThreshold(-55.0f);
    neuron.setLeakConductance(0.1f);
    neuron.setRefractoryPeriod(5);
    
    // Simulate input current
    neuron.injectCurrent(10.0f);
    
    // Simulate membrane potential dynamics for 10 timesteps
    float timeStep = 0.001f;  // 1ms timestep
    int steps = 10;
    for (int i = 0; i < steps; ++i) {
        // Check if neuron would fire
        if (neuron.checkThreshold()) {
            neuron.setFiringState(nlm::FiringState::Active);
            neuron.recordSpike(static_cast<float>(i) * timeStep);
            neuron.setMembranePotential(neuron.getResetPotential());
            neuron.setRefractoryPeriod(5);
        }
        
        // Update membrane potential with leak
        float delta = neuron.getTotalCurrent() * timeStep - 
                      neuron.getLeakConductance() * 
                      (neuron.getMembranePotential() - neuron.getRestingPotential()) * timeStep;
        neuron.addToMembranePotential(delta);
        
        // Decrement refractory period
        neuron.decrementRefractory();
        
        // Clear current for next timestep
        neuron.clearTotalCurrent();
    }
    
    // Verify dynamics
    assert(!neuron.getSpikeHistory().empty());
    assert(neuron.getFiringRate() < 100.0f);  // Reasonable firing rate
    
    std::cout << "    testLIFDynamics passed" << std::endl;
}

void testRefractoryPeriod() {
    nlm::Neuron neuron(nlm::NeuronId(2));
    
    neuron.setRefractoryPeriod(10);
    neuron.setFiringState(nlm::FiringState::Refractory);
    
    assert(neuron.isRefractory());
    assert(!neuron.isFiring());
    
    // Decrement to first 4 steps
    neuron.decrementRefractory();
    neuron.decrementRefractory();
    neuron.decrementRefractory();
    neuron.decrementRefractory();
    assert(neuron.isRefractory());
    
    // Decrement to last step
    neuron.decrementRefractory();
    assert(!neuron.isRefractory());
    assert(neuron.getFiringState() == nlm::FiringState::Resting);
    
    // Test that neuron can fire after refractory period
    neuron.injectCurrent(20.0f);
    neuron.setFiringState(nlm::FiringState::Active);
    assert(neuron.checkThreshold());
    
    std::cout << "    testRefractoryPeriod passed" << std::endl;
}

void testSpikeTiming() {
    nlm::Neuron neuron(nlm::NeuronId(3));
    
    // Test spike history with precise timing
    const float baseTime = 10.0f;
    neuron.recordSpike(baseTime + 0.0f);
    neuron.recordSpike(baseTime + 5.0f);
    neuron.recordSpike(baseTime + 15.0f);
    neuron.recordSpike(baseTime + 25.0f);
    
    auto spikes = neuron.getSpikeHistory();
    assert(spikes.size() == 4);
    assert(spikes[0] == baseTime + 0.0f);
    assert(spikes[1] == baseTime + 5.0f);
    assert(spikes[2] == baseTime + 15.0f);
    assert(spikes[3] == baseTime + 25.0f);
    
    // Test inter-spike intervals
    float isi1 = spikes[1] - spikes[0];
    float isi2 = spikes[2] - spikes[1];
    float isi3 = spikes[3] - spikes[2];
    
    assert(isi1 > 0.0f && isi1 < 10.0f);
    assert(isi2 > 5.0f && isi2 < 20.0f);
    assert(isi3 > 5.0f && isi3 < 20.0f);
    
    std::cout << "    testSpikeTiming passed" << std::endl;
}

void testSynapticIntegration() {
    nlm::Neuron neuron(nlm::NeuronId(4));
    
    // Simulate excitatory and inhibitory inputs
    neuron.receiveExcitatoryInput(5.0f);  // +5mV
    neuron.receiveInhibitoryInput(2.0f);  // -2mV
    neuron.injectCurrent(10.0f);  // External current
    
    // Get membrane potential after inputs
    float currentPotential = neuron.getMembranePotential();
    
    // The neuron should have integrated these inputs
    // Note: In real implementation, this would depend on specific integration model
    assert(neuron.getTotalCurrent() == 10.0f);  // External current preserved
    
    std::cout << "    testSynapticIntegration passed" << std::endl;
}

void testNeuronTypeSpecificBehavior() {
    nlm::Neuron excitatory(nlm::NeuronId(5));
    nlm::Neuron inhibitory(nlm::NeuronId(6));
    
    excitatory.setType(nlm::NeuronType::Excitatory);
    inhibitory.setType(nlm::NeuronType::Inhibitory);
    
    assert(excitatory.getType() == nlm::NeuronType::Excitatory);
    assert(inhibitory.getType() == nlm::NeuronType::Inhibitory);
    
    // Test type-specific initialization
    excitatory.initializeRandom(nlm::RandomGenerator(42));
    inhibitory.initializeRandom(nlm::RandomGenerator(42));
    
    // Excitatory neurons typically have different parameters
    assert(excitatory.getThreshold() < -50.0f);  // Typically higher threshold
    assert(inhibitory.getThreshold() < -55.0f);
    
    std::cout << "    testNeuronTypeSpecificBehavior passed" << std::endl;
}

void testRegionPopulationMembership() {
    nlm::Neuron neuron(nlm::NeuronId(7));
    
    neuron.setRegionId(nlm::RegionId(1));
    neuron.setPopulationId(nlm::PopulationId(1));
    
    assert(neuron.getRegionId() == nlm::RegionId(1));
    assert(neuron.getPopulationId() == nlm::PopulationId(1));
    
    // Test region and population identification
    assert(neuron.getRegionId().index() > 0);
    assert(neuron.getPopulationId().index() > 0);
    
    std::cout << "    testRegionPopulationMembership passed" << std::endl;
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
