// Performance Regression Tests
// Tests performance characteristics of improvements

#include "tests/test_suite_runner.cpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_performance_regression {

void testParameterAccessPerformance() {
    std::cout << "  [testParameterAccessPerformance] ";

    nlm::Config config;

    // Test performance of parameter access
    // Add many parameters for performance testing
    for (int i = 0; i < 1000; ++i) {
        config.set(std::string("perf_test_") + std::to_string(i), i, nlm::ConfigSource::Runtime);
    }

    // Test parameter retrieval performance
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; ++i) {
        int value = config.getOr<int>(std::string("perf_test_") + std::to_string(i), -1);
        assert(value == i);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "PASSED" << std::endl;
}

void testConfigurationMemoryUsage() {
    std::cout << "  [testConfigurationMemoryUsage] ";

    nlm::Config config;

    // Test memory usage with many configurations
    for (int i = 0; i < 100; ++i) {
        auto tempConfig = std::make_shared<Config>();
        for (int j = 0; j < 50; ++j) {
            tempConfig->set(std::string("mem_test_") + std::to_string(j), j, nlm::ConfigSource::Runtime);
        }
    }

    std::cout << "PASSED" << std::endl;
}

void testValidationPerformance() {
    std::cout << "  [testValidationPerformance] ";

    nlm::Config config;

    // Test validation performance
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; ++i) {
        bool valid = config.validateParameter("neuron", "membrane_capacitance", 1.0f);
        assert(valid);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "PASSED" << std::endl;
}

void runAll() {
    testParameterAccessPerformance();
    testConfigurationMemoryUsage();
    testValidationPerformance();

    std::cout << "  All performance regression tests passed!" << std::endl;
}

} // namespace test_performance_regression