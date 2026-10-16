// Phase 6 Integration Tests
// Tests the integration of all Phase 6 systems

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

using namespace nlm;

namespace test_phase6_integration {

void testCoreIntegration() {
    std::cout << "  Testing Core Integration..." << std::endl;
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Verify integration
    bool success = experiment.verifyIntegration();
    assert(success);
    
    std::cout << "    PASSED" << std::endl;
}

void testMemoryIntegration() {
    std::cout << "  Testing Memory Integration..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    bool success = experiment.testMemoryIntegration();
    assert(success);
    
    std::cout << "    PASSED" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "  Testing Neuromodulation Integration..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    bool success = experiment.testNeuromodulationIntegration();
    assert(success);
    
    std::cout << "    PASSED" << std::endl;
}

void testCheckpointing() {
    std::cout << "  Testing Checkpoint System..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    bool success = experiment.testCheckpointing();
    assert(success);
    
    std::cout << "    PASSED" << std::endl;
}

void testReplay() {
    std::cout << "  Testing Replay System..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    bool success = experiment.testReplay();
    assert(success);
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
    
    // Initialize logger
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    testCoreIntegration();
    testMemoryIntegration();
    testNeuromodulationIntegration();
    testCheckpointing();
    testReplay();
    
    std::cout << std::endl;
    std::cout << "=== All Phase 6 Integration Tests PASSED ===" << std::endl;
}

} // namespace test_phase6_integration