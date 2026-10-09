// NLM Integration Tests Main
// 
// This file provides comprehensive integration testing for the NLM neural network simulation system.
// It runs all categories of integration tests:
// 1. Integration Test Framework - Base classes and utilities
// 2. End-to-End Simulation Tests - Complete neural network simulations
// 3. Cross-Component Integration Tests - Interactions between subsystems
// 4. Performance Integration Tests - Benchmark system performance under load
// 5. Stress Integration Tests - System behavior under extreme conditions

#include <iostream>
#include <cassert>
#include "IntegrationTestFramework.hpp"
#include "EndToEndTests.hpp"
#include "CrossComponentTests.hpp"
#include "PerformanceTests.hpp"
#include "StressTests.hpp"

// Forward declarations of integration test runners
namespace integration_tests {
    void runAllIntegrationTests();
}

namespace end_to_end_tests {
    void EndToEndTestRunner::runAllTests();
}

namespace cross_component_tests {
    void CrossComponentTestRunner::runAllTests();
}

namespace performance_tests {
    void PerformanceTestRunner::runAllTests();
}

namespace stress_tests {
    void StressTestRunner::runAllTests();
}

int main() {
    std::cout << "=== NLM Integration Test Suite ===" << std::endl;
    std::cout << "Testing system-wide functionality and subsystem interactions" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Run Integration Test Framework Tests
    std::cout << "=== Integration Test Framework Tests ===" << std::endl;
    try {
        integration_tests::runAllIntegrationTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    // Run End-to-End Simulation Tests
    std::cout << "=== End-to-End Simulation Tests ===" << std::endl;
    try {
        end_to_end_tests::EndToEndTestRunner::runAllTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    // Run Cross-Component Integration Tests
    std::cout << "=== Cross-Component Integration Tests ===" << std::endl;
    try {
        cross_component_tests::CrossComponentTestRunner::runAllTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    // Run Performance Integration Tests
    std::cout << "=== Performance Integration Tests ===" << std::endl;
    try {
        performance_tests::PerformanceTestRunner::runAllTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    // Run Stress Integration Tests
    std::cout << "=== Stress Integration Tests ===" << std::endl;
    try {
        stress_tests::StressTestRunner::runAllTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    std::cout << "===============================================" << std::endl;
    if (allPassed) {
        std::cout << "=== ALL INTEGRATION TESTS PASSED ===" << std::endl;
        std::cout << "The NLM neural network simulation system is ready for production use." << std::endl;
        return 0;
    } else {
        std::cout << "=== SOME INTEGRATION TESTS FAILED ===" << std::endl;
        std::cout << "Please review the failed tests above for issues." << std::endl;
        return 1;
    }
}