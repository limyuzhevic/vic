// NLM Comprehensive Test Suite
// Phase 2: Real Neural Computation Tests
// This is a Google Test-based test suite for the NLM codebase
// Includes unit tests, integration tests, and performance benchmarks

#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <cassert>

// Include test components
#include "tests/common/test_base.hpp"
#include "tests/unit/test_neuron.hpp"
#include "tests/unit/test_synapse.hpp"
#include "tests/unit/test_plasticity.hpp"
#include "tests/unit/test_memory.hpp"
#include "tests/unit/test_brain.hpp"
#include "tests/unit/test_config.hpp"
#include "tests/unit/test_clock.hpp"
#include "tests/unit/test_random.hpp"
#include "tests/unit/test_types.hpp"
#include "tests/integration/test_integration.hpp"
#include "tests/performance/test_performance.hpp"

int main(int argc, char **argv) {
    std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
    std::cout << "Phase 2: Real Neural Computation Testing" << std::endl;
    std::cout << std::endl;
    
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Run all tests
    int result = RUN_ALL_TESTS();
    
    if (result == 0) {
        std::cout << std::endl;
        std::cout << "=== All Tests Passed ===" << std::endl;
    } else {
        std::cout << std::endl;
        std::cout << "=== Test Suite Failed ===" << std::endl;
        std::cout << "Failed tests: " << result << std::endl;
    }
    
    return result;
}
