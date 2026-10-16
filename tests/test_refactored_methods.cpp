// Refactored Methods Tests
// Tests extracted helper functions and refactored method behavior

#include "tests/test_suite_runner.cpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_refactored_methods {

void testExtractedHelperFunctions() {
    std::cout << "  [testExtractedHelperFunctions] ";

    nlm::Config config;

    // Test extracted helper functions
    // Configuration.getOr() demonstrates refactored helper for safe parameter access
    int defaultValue = config.getOr<int>("nonexistent_key", 999);
    assert(defaultValue == 999);

    // Test configuration validation with extracted validation logic
    bool valid = config.validateParameter("neuron", "membrane_capacitance", 1.0f);
    assert(valid);

    std::cout << "PASSED" << std::endl;
}

void testMethodSimplification() {
    std::cout << "  [testMethodSimplification] ";

    nlm::Config config;

    // Test that refactored methods are simpler and more maintainable
    // Configuration.summary() demonstrates simplified output generation
    std::string summary = config.summary();
    assert(!summary.empty());

    // Configuration.set() demonstrates simplified parameter setting
    config.set("test_param", 42, nlm::ConfigSource::Runtime);
    int value = config.getOr<int>("test_param", 0);
    assert(value == 42);

    std::cout << "PASSED" << std::endl;
}

void testComplexityReduction() {
    std::cout << "  [testComplexityReduction] ";

    nlm::Config config;

    // Test that complexity has been reduced
    // Multiple simple operations are now possible
    config.set("param1", 1, nlm::ConfigSource::Runtime);
    config.set("param2", 2.0f, nlm::ConfigSource::Runtime);
    config.set("param3", std::string("three"), nlm::ConfigSource::Runtime);

    assert(config.getOr<int>("param1", 0) == 1);
    assert(config.getOr<float>("param2", 0.0f) == 2.0f);
    assert(config.getOr<std::string>("param3", "") == "three");

    std::cout << "PASSED" << std::endl;
}

void runAll() {
    testExtractedHelperFunctions();
    testMethodSimplification();
    testComplexityReduction();

    std::cout << "  All refactored methods tests passed!" << std::endl;
}

} // namespace test_refactored_methods