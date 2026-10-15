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

// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

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

void testNeuronAdvancedParameters() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test LIF neuron parameters
    assert(neuron.getRestingPotential() == -70.0f); // Default
    assert(neuron.getThreshold() == -55.0f); // Default
    assert(neuron.getResetPotential() == -70.0f); // Default
    assert(neuron.getLeakConductance() == 10.0f); // Default
    assert(neuron.getRefractoryPeriod() == 5); // Default
    
    // Set custom values
    neuron.setRestingPotential(-65.0f);
    neuron.setThreshold(-50.0f);
    neuron.setResetPotential(-60.0f);
    neuron.setLeakConductance(15.0f);
    neuron.setRefractoryPeriod(10);
    
    assert(neuron.getRestingPotential() == -65.0f);
    assert(neuron.getThreshold() == -50.0f);
    assert(neuron.getResetPotential() == -60.0f);
    assert(neuron.getLeakConductance() == 15.0f);
    assert(neuron.getRefractoryPeriod() == 10);
    
    // Test firing rate
    assert(neuron.getFiringRate() == 0.0f); // Default
    neuron.setFiringRate(10.0f);
    assert(neuron.getFiringRate() == 10.0f);
    
    // Test spike detection
    neuron.setMembranePotential(-40.0f); // Above threshold (-50)
    assert(neuron.checkThreshold()); // Should detect spike
    
    std::cout << "    testNeuronAdvancedParameters passed" << std::endl;
}

void testNeuronLIFDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set up neuron for LIF simulation
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    neuron.setLeakConductance(10.0f);
    neuron.setRefractoryPeriod(5);
    
    nlm::Timestamp currentTime = 0.0;
    nlm::TimestepDuration dt = 0.001; // 1ms timestep
    
    // Inject current to bring neuron above threshold
    neuron.injectCurrent(50.0f); // Large current injection
    neuron.setMembranePotential(-50.0f); // Manually set to test spike
    
    // Test LIF step
    bool fired = neuron.stepLIF(currentTime, dt);
    assert(fired); // Should have fired
    assert(neuron.isFiring());
    assert(neuron.isRefractory()); // Should be in refractory period
    
    // Step until refractory ends
    bool stillFiring = neuron.stepLIF(currentTime + 0.01, dt);
    assert(stillFiring == false); // Still firing but not refractory
    
    std::cout << "    testNeuronLIFDynamics passed" << std::endl;
}

void testNeuronPlasticityFlags() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Check default plasticity flags
    const auto& flags = neuron.getPlasticityFlags();
    assert(!flags.hebbian);
    assert(!flags.stdp);
    assert(!flags.reward_modulated);
    
    // Enable plasticity
    neuron.enablePlasticity(true, true, true);
    
    const auto& flags2 = neuron.getPlasticityFlags();
    assert(flags2.hebbian);
    assert(flags2.stdp);
    assert(flags2.reward_modulated);
    
    // Test mutable plasticity flags reference
    auto& mutableFlags = neuron.getPlasticityFlags();
    mutableFlags.hebbian = false;
    assert(!neuron.getPlasticityFlags().hebbian);
    
    std::cout << "    testNeuronPlasticityFlags passed" << std::endl;
}

void testNeuronRegionPopulationMembership() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Default region and population should be invalid
    assert(neuron.getRegionId().index() == 0); // INVALID_REGION_ID
    assert(neuron.getPopulationId().index() == 0); // INVALID_POPULATION_ID
    
    // Set valid region and population (simplified)
    neuron.setRegionId(nlm::RegionId(1));
    neuron.setPopulationId(nlm::PopulationId(1));
    
    assert(neuron.getRegionId() == nlm::RegionId(1));
    assert(neuron.getPopulationId() == nlm::PopulationId(1));
    
    std::cout << "    testNeuronRegionPopulationMembership passed" << std::endl;
}

void testNeuronModulatoryInput() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test modulatory input (doesn't directly affect membrane potential but affects adaptation)
    neuron.receiveModulatoryInput(0.5f);
    
    // The adaptation variable should be affected
    // This depends on implementation - just ensure it doesn't crash
    assert(neuron.getLastSpikeTime() == -1.0f); // No spike yet
    
    std::cout << "    testNeuronModulatoryInput passed" << std::endl;
}

void testNeuronFiringRateControl() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test firing rate setting and getting
    neuron.setFiringRate(5.0f);
    assert(neuron.getFiringRate() == 5.0f);
    
    neuron.setFiringRate(0.0f);
    assert(neuron.getFiringRate() == 0.0f);
    
    // Negative firing rate might not make sense but test for robustness
    neuron.setFiringRate(-1.0f);
    assert(neuron.getFiringRate() == -1.0f);
    
    std::cout << "    testNeuronFiringRateControl passed" << std::endl;
}

void testNeuronMultipleSpikeRecording() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Record multiple spikes
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    neuron.recordSpike(0.4);
    neuron.recordSpike(0.5);
    
    assert(neuron.getSpikeHistory().size() == 5);
    assert(neuron.getSpikeHistory()[0] == 0.1);
    assert(neuron.getSpikeHistory()[4] == 0.5);
    
    // Clear history
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testNeuronMultipleSpikeRecording passed" << std::endl;
}

void testNeuronTypeSpecificBehavior() {
    // Test different neuron types
    nlm::Neuron excitatoryNeuron(nlm::NeuronId(1));
    nlm::Neuron inhibitoryNeuron(nlm::NeuronId(2));
    nlm::Neuron internalNeuron(nlm::NeuronId(3));
    
    excitatoryNeuron.setType(nlm::NeuronType::Excitatory);
    inhibitoryNeuron.setType(nlm::NeuronType::Inhibitory);
    internalNeuron.setType(nlm::NeuronType::Internal);
    
    assert(excitatoryNeuron.getType() == nlm::NeuronType::Excitatory);
    assert(inhibitoryNeuron.getType() == nlm::NeuronType::Inhibitory);
    assert(internalNeuron.getType() == nlm::NeuronType::Internal);
    
    // All should have default state
    assert(excitatoryNeuron.getMembranePotential() == -70.0f);
    assert(inhibitoryNeuron.getMembranePotential() == -70.0f);
    assert(internalNeuron.getMembranePotential() == -70.0f);
    
    std::cout << "    testNeuronTypeSpecificBehavior passed" << std::endl;
}

void runAll() {
    std::cout << "Running Neuron tests..." << std::endl;
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
    testNeuronAdvancedParameters();
    testNeuronLIFDynamics();
    testNeuronPlasticityFlags();
    testNeuronRegionPopulationMembership();
    testNeuronModulatoryInput();
    testNeuronFiringRateControl();
    testNeuronMultipleSpikeRecording();
    testNeuronTypeSpecificBehavior();
}

} // namespace test_neuron
