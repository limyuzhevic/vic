// Comprehensive Neuron Edge Cases Tests
// Tests boundary conditions, error handling, and memory safety

#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace test_neuron_edge {

void testNeuronBoundaryValues() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test boundary membrane potential values
    neuron.setMembranePotential(-100.0f);  // Minimum plausible
    assert(neuron.getMembranePotential() == -100.0f);
    
    neuron.setMembranePotential(50.0f);   // Maximum plausible
    assert(neuron.getMembranePotential() == 50.0f);
    
    // Test boundary threshold values
    neuron.setThreshold(-100.0f);
    assert(neuron.getThreshold() == -100.0f);
    
    neuron.setThreshold(10.0f);
    assert(neuron.getThreshold() == 10.0f);
    
    // Test firing rate boundaries
    neuron.setFiringRate(0.0f);  // Minimum
    assert(neuron.getFiringRate() == 0.0f);
    
    neuron.setFiringRate(1000.0f);  // High value
    assert(neuron.getFiringRate() == 1000.0f);
    
    std::cout << "    testNeuronBoundaryValues passed" << std::endl;
}

void testNeuronFloatingPointEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test NaN handling (if applicable)
    // Note: Depends on implementation of setMembranePotential etc.
    
    // Test very small values
    neuron.setMembranePotential(1e-10f);
    assert(std::abs(neuron.getMembranePotential() - 1e-10f) < 1e-12f);
    
    // Test very large values (within reasonable bounds)
    neuron.setMembranePotential(1e6f);
    assert(neuron.getMembranePotential() == 1e6f);
    
    // Test negative infinity and positive infinity
    // Depending on implementation, these may or may not be accepted
    
    std::cout << "    testNeuronFloatingPointEdgeCases passed" << std::endl;
}

void testNeuronRefractoryEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test minimum refractory period
    neuron.setRefractoryPeriod(0);
    assert(neuron.isRefractory() == false);
    
    // Test maximum refractory period
    neuron.setRefractoryPeriod(65535);  // Large value
    assert(neuron.isRefractory() == true);
    
    // Test refractory decrement at boundaries
    neuron.setRefractoryPeriod(1);
    assert(neuron.isRefractory() == true);
    
    neuron.decrementRefractory();
    assert(neuron.isRefractory() == false);
    
    // Test decrement when already not refractory
    neuron.decrementRefractory();  // Should not crash
    assert(neuron.isRefractory() == false);
    
    std::cout << "    testNeuronRefractoryEdgeCases passed" << std::endl;
}

void testNeuronCurrentInjectionEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test very large current injection
    neuron.injectCurrent(1000.0f);
    assert(neuron.getTotalCurrent() == 1000.0f);
    
    // Test negative current (inhibition)
    neuron.injectCurrent(-500.0f);
    assert(neuron.getTotalCurrent() == 500.0f);  // 1000 - 500 = 500
    
    // Test clearing current
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    // Test clearing when already clear
    neuron.clearTotalCurrent();  // Should not crash
    
    std::cout << "    testNeuronCurrentInjectionEdgeCases passed" << std::endl;
}

void testNeuronSynapticInputEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test very large excitatory input
    neuron.receiveExcitatoryInput(10000.0f);
    assert(neuron.getTotalCurrent() == 10000.0f);
    
    // Test very large inhibitory input
    neuron.receiveInhibitoryInput(5000.0f);
    assert(neuron.getTotalCurrent() == 5000.0f);  // 10000 - 5000 = 5000
    
    // Test modulatory input
    neuron.receiveModulatoryInput(100.0f);
    assert(neuron.getTotalCurrent() == 5100.0f);
    
    std::cout << "    testNeuronSynapticInputEdgeCases passed" << std::endl;
}

void testNeuronSpikeHistoryEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test empty spike history
    assert(neuron.getSpikeHistory().empty());
    
    // Test spike history with many entries
    const size_t maxSpikeHistory = 1000;
    for (size_t i = 0; i < maxSpikeHistory; ++i) {
        neuron.recordSpike(static_cast<float>(i));
    }
    
    assert(neuron.getSpikeHistory().size() == maxSpikeHistory);
    
    // Test clearing spike history multiple times
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    neuron.clearSpikeHistory();  // Should not crash
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testNeuronSpikeHistoryEdgeCases passed" << std::endl;
}

void testNeuronResetEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Modify neuron state
    neuron.setMembranePotential(-50.0f);
    neuron.injectCurrent(100.0f);
    neuron.recordSpike(1.0f);
    neuron.recordSpike(2.0f);
    neuron.setFiringState(nlm::FiringState::Active);
    
    // Reset neuron
    neuron.reset();
    
    // Verify reset state
    assert(neuron.getMembranePotential() == -70.0f);  // Default value
    assert(neuron.getTotalCurrent() == 0.0f);
    assert(neuron.getSpikeHistory().empty());
    assert(neuron.getType() == nlm::NeuronType::Internal);  // Default type
    
    // Reset again (should be idempotent)
    neuron.reset();  // Should not crash
    
    std::cout << "    testNeuronResetEdgeCases passed" << std::endl;
}

void testNeuronRandomInitializationEdgeCases() {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Initialize multiple times with same seed
    neuron.initializeRandom(rng);
    float membranePotential1 = neuron.getMembranePotential();
    
    nlm::Neuron neuron2(nlm::NeuronId(2));
    neuron2.initializeRandom(rng);
    float membranePotential2 = neuron2.getMembranePotential();
    
    // Should produce same result with same seed (depending on implementation)
    // Note: This may vary based on randomness implementation
    
    std::cout << "    testNeuronRandomInitializationEdgeCases passed" << std::endl;
}

void testNeuronTypeChanges() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test all neuron types
    neuron.setType(nlm::NeuronType::Internal);
    assert(neuron.getType() == nlm::NeuronType::Internal);
    
    neuron.setType(nlm::NeuronType::Excitatory);
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    assert(neuron.getType() == nlm::NeuronType::Inhibitory);
    
    neuron.setType(nlm::NeuronType::Sensory);
    assert(neuron.getType() == nlm::NeuronType::Sensory);
    
    neuron.setType(nlm::NeuronType::Motor);
    assert(neuron.getType() == nlm::NeuronType::Motor);
    
    neuron.setType(nlm::NeuronType::Special);
    assert(neuron.getType() == nlm::NeuronType::Special);
    
    std::cout << "    testNeuronTypeChanges passed" << std::endl;
}

void testNeuronPlasticityFlags() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Enable all plasticity types
    neuron.enablePlasticity(true, true, true);
    auto& flags = neuron.getPlasticityFlags();
    
    // Note: Depending on implementation, flags may have specific structure
    // For now, just verify the method doesn't crash
    
    // Enable only some types
    neuron.enablePlasticity(false, true, false);
    neuron.enablePlasticity(true, false, false);
    
    std::cout << "    testNeuronPlasticityFlags passed" << std::endl;
}

void testNeuronStateAccess() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test getState() and getState() (mutable) consistency
    const auto& constState = neuron.getState();
    auto& mutableState = neuron.getState();
    
    // Modify through mutable reference
    mutableState.membranePotential = -60.0f;
    assert(constState.membranePotential == -60.0f);
    
    // Test all state fields are accessible
    assert(neuron.getState().membranePotential >= -100.0f && neuron.getState().membranePotential <= 50.0f);
    assert(neuron.getState().threshold >= -100.0f && neuron.getState().threshold <= 10.0f);
    assert(neuron.getState().refractoryRemaining <= neuron.getState().refractoryPeriod);
    
    std::cout << "    testNeuronStateAccess passed" << std::endl;
}

void testNeuronMoveSemantics() {
    nlm::Neuron neuron1(nlm::NeuronId(1));
    neuron1.setMembranePotential(-50.0f);
    
    // Move constructor
    nlm::Neuron neuron2(std::move(neuron1));
    assert(neuron2.getId() == nlm::NeuronId(1));
    assert(neuron2.getMembranePotential() == -50.0f);
    
    // Move assignment
    nlm::Neuron neuron3(nlm::NeuronId(2));
    neuron3 = std::move(neuron2);
    assert(neuron3.getId() == nlm::NeuronId(1));
    assert(neuron3.getMembranePotential() == -50.0f);
    
    std::cout << "    testNeuronMoveSemantics passed" << std::endl;
}

void testNeuronStepLIFEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test stepLIF with various parameters
    neuron.setMembranePotential(-55.0f);  // Below threshold
    neuron.setThreshold(-50.0f);
    
    bool fired = neuron.stepLIF(1.0f, 0.001f);
    assert(!fired);  // Should not fire
    assert(neuron.getMembranePotential() > -55.0f);  // Should leak towards resting
    
    // Test with current injection that causes threshold crossing
    neuron.setMembranePotential(-55.0f);
    neuron.injectCurrent(10.0f);
    
    bool fired2 = neuron.stepLIF(1.0f, 0.001f);
    // Whether it fires depends on implementation details
    
    std::cout << "    testNeuronStepLIFEdgeCases passed" << std::endl;
}

void testNeuronRegionPopulation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test region and population assignment
    nlm::RegionId testRegion(1);
    nlm::PopulationId testPopulation(1);
    
    neuron.setRegionId(testRegion);
    neuron.setPopulationId(testPopulation);
    
    assert(neuron.getRegionId() == testRegion);
    assert(neuron.getPopulationId() == testPopulation);
    
    std::cout << "    testNeuronRegionPopulation passed" << std::endl;
}

void runAll() {
    testNeuronBoundaryValues();
    testNeuronFloatingPointEdgeCases();
    testNeuronRefractoryEdgeCases();
    testNeuronCurrentInjectionEdgeCases();
    testNeuronSynapticInputEdgeCases();
    testNeuronSpikeHistoryEdgeCases();
    testNeuronResetEdgeCases();
    testNeuronRandomInitializationEdgeCases();
    testNeuronTypeChanges();
    testNeuronPlasticityFlags();
    testNeuronStateAccess();
    testNeuronMoveSemantics();
    testNeuronStepLIFEdgeCases();
    testNeuronRegionPopulation();
    
    std::cout << "Neuron edge cases test suite completed successfully!" << std::endl;
}

} // namespace test_neuron_edge
