// Test Development System
#include "../../src/development/DevelopmentSystem.hpp"
#include <cassert>
#include <iostream>

namespace test_development {

void testDevelopmentSystem() {
    std::cout << "Testing development system..." << std::endl;
    
    // Create development system instance
    DevelopmentSystem development;
    
    // Test initial values
    assert(development.getProgress() >= 0.0f && development.getProgress() <= 1.0f);
    assert(development.getCurrentStage() < DevelopmentalStage::Aging);
    assert(development.getPlasticityBoostFactor() > 0.0f);
    
    // Test maturation
    development.updateMaturation(0.1f);
    
    // Test pruning
    development.updatePruning(0.001f);
    
    // Test synaptogenesis
    development.updateSynaptogenesis(0.001f);
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    testDevelopmentSystem();
}

} // namespace test_development