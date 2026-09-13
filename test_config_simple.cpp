// Simple test for Config
#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    nlm::Config config;
    
    // Test setting and getting
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime, "A string parameter");
    config.set("width", 100, nlm::ConfigSource::Runtime);
    config.set("opacity", 0.5, nlm::ConfigSource::Runtime);
    config.set("format", "PNG", nlm::ConfigSource::Runtime);
    
    // Test get
    auto val = config.get<int>("int_value");
    if (val.has_value() && *val == 42) {
        std::cout << "✓ int_value test passed" << std::endl;
    } else {
        std::cout << "✗ int_value test failed" << std::endl;
        return 1;
    }
    
    // Test validation report
    std::string report = config.generateValidationReport();
    if (report.find("Summary:") != std::string::npos) {
        std::cout << "✓ Validation report test passed" << std::endl;
    } else {
        std::cout << "✗ Validation report test failed" << std::endl;
        return 1;
    }
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}