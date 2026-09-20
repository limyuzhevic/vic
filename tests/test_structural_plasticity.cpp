// Test Structural Plasticity
#include "../../src/plasticity/StructuralPlasticity.hpp"
#include <cassert>
#include <iostream>

namespace test_structural_plasticity {

void testStructuralPlasticity() {
    std::cout << "Testing structural plasticity..." << std::endl;
    
    // Create structural plasticity instance
    StructuralPlasticity plasticity;
    
    // Test initial values
    assert(plasticity.getSynaptogenesisRate() > 0.0f);
    assert(plasticity.getPruningRate() > 0.0f);
    assert(plasticity.getTargetDensity() > 0.0f);
    
    // Test density calculation
    float densityBefore = plasticity.getCurrentDensity();
    assert(densityBefore >= 0.0f);
    
    // Test structural update
    plasticity.update(nullptr, 0.001f); // Null brain for test
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    testStructuralPlasticity();
}

} // namespace test_structural_plasticity