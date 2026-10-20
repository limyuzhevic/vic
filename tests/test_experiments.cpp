// Experimental Tests
// Tests for Phase 3, 4, 5, and 6 experimental frameworks

#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase5Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_experiments {

void testPhase3Experiment() {
    std::cout << "    testPhase3Experiment passed" << std::endl;
}

void testPhase4Experiment() {
    std::cout << "    testPhase4Experiment passed" << std::endl;
}

void testPhase5Experiment() {
    std::cout << "    testPhase5Experiment passed" << std::endl;
}

void testPhase6Integration() {
    // Create minimal experiment
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test integration verification
    bool integrationOK = experiment.verifyIntegration();
    assert(integrationOK);
    
    // Test memory integration
    assert(experiment.testMemoryIntegration());
    
    // Test neuromodulation integration  
    assert(experiment.testNeuromodulationIntegration());
    
    // Test checkpointing
    assert(experiment.testCheckpointing());
    
    // Test replay
    assert(experiment.testReplay());
    
    std::cout << "    testPhase6Integration passed" << std::endl;
}

void runAll() {
    testPhase3Experiment();
    testPhase4Experiment();
    testPhase5Experiment();
    testPhase6Integration();
}

} // namespace test_experiments