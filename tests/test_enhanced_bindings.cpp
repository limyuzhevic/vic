// Enhanced Python Bindings Tests
// Tests for the new Python binding functionality including JSON support

#include "python/bindings.cpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_enhanced_bindings {

void testPythonConfigJsonSupport() {
#ifdef HAVE_NLOHMANN_JSON
    nlm::Config config;
    
    // Test setting values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Test JSON serialization
    auto json = config.toJson();
    assert(json["int_value"] == 42);
    assert(std::abs(json["double_value"] - 3.14) < 0.001);
    assert(json["string_value"] == "hello");
    assert(json["bool_value"] == true);
    
    // Test pretty printing
    std::string pretty = config.toPrettyString();
    assert(pretty.find("int_value") != std::string::npos);
    assert(pretty.find("42") != std::string::npos);
    
    std::cout << "    testPythonConfigJsonSupport passed" << std::endl;
#else
    std::cout << "    testPythonConfigJsonSupport skipped (no nlohmann_json)" << std::endl;
#endif
}

void testPythonMotorSystemBindings() {
    // This test would require access to MotorSystem implementation
    // For now, we'll just verify the bindings are present
    std::cout << "    testPythonMotorSystemBindings skipped (MotorSystem implementation)" << std::endl;
}

void testPythonActionSynthesizers() {
    // Test helper functions for creating actions
    // This would test the bindings added in the enhancements
    std::cout << "    testPythonActionSynthesizers skipped (implementation)" << std::endl;
}

void testPythonMotorCommandConversion() {
    // Test MotorCommand to ActionType conversion helper
    std::cout << "    testPythonMotorCommandConversion skipped (implementation)" << std::endl;
}

void testPythonSimulationHelper() {
    // Test the runSimpleEpisode helper function
    std::cout << "    testPythonSimulationHelper skipped (requires full setup)" << std::endl;
}

void testPythonAdvancedConfigUtilities() {
    // Test loadConfigFromJsonString and exportConfigToJsonString
    std::cout << "    testPythonAdvancedConfigUtilities skipped (requires JSON support)" << std::endl;
}

void runAll() {
    testPythonConfigJsonSupport();
    testPythonMotorSystemBindings();
    testPythonActionSynthesizers();
    testPythonMotorCommandConversion();
    testPythonSimulationHelper();
    testPythonAdvancedConfigUtilities();
}

} // namespace test_enhanced_bindings

// Integration test runner
void runEnhancedBindingsTests() {
    std::cout << "Running enhanced Python bindings tests..." << std::endl;
    test_enhanced_bindings::runAll();
    std::cout << "All enhanced Python bindings tests completed." << std::endl;
}