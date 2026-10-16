// Backward Compatibility Tests
// Tests behavior preservation and API compatibility

#include "tests/test_suite_runner.cpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_backward_compatibility {

void testAPIPreservation() {
    std::cout << "  [testAPIPreservation] ";

    nlm::Config config;

    // Test that the API has been preserved
    // Old set/get patterns still work
    config.set("old_style_int", 100, nlm::ConfigSource::Runtime);
    config.set("old_style_string", std::string("compatible"), nlm::ConfigSource::Runtime);

    int oldInt = config.getOr<int>("old_style_int", 0);
    std::string oldString = config.getOr<std::string>("old_style_string", "");

    assert(oldInt == 100);
    assert(oldString == "compatible");

    std::cout << "PASSED" << std::endl;
}

void testBehaviorPreservation() {
    std::cout << "  [testBehaviorPreservation] ";

    nlm::Config config;

    // Test that behavior has been preserved
    // Configuration operations behave as expected
    assert(!config.has("nonexistent"));

    config.set("test_key", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test_key"));

    config.remove("test_key");
    assert(!config.has("test_key"));

    std::cout << "PASSED" << std::endl;
}

void testInterfaceCompatibility() {
    std::cout << "  [testInterfaceCompatibility] ";

    nlm::Config config;

    // Test interface compatibility
    // All existing methods still work as expected
    auto keys = config.getKeys();
    assert(keys.empty());  // Initially no keys

    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);

    auto newKeys = config.getKeys();
    assert(newKeys.size() == 2);

    std::cout << "PASSED" << std::endl;
}

void runAll() {
    testAPIPreservation();
    testBehaviorPreservation();
    testInterfaceCompatibility();

    std::cout << "  All backward compatibility tests passed!" << std::endl;
}

} // namespace test_backward_compatibility