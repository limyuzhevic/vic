#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    nlm::Config config;
    
    std::cout << "Testing Config refactoring..." << std::endl;
    
    // Test setting values with validation
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello", nlm::ConfigSource::Runtime, "Test parameter");
    config.set("test_width", 100, nlm::ConfigSource::Runtime);
    config.set("test_opacity", 0.5, nlm::ConfigSource::Runtime);
    config.set("test_format", "PNG", nlm::ConfigSource::Runtime);
    
    // Test getting values
    auto val = config.get<int>("test_int");
    if (val && *val == 42) {
        std::cout << "✓ Integer parameter set/get test passed" << std::endl;
    } else {
        std::cout << "✗ Integer parameter test failed" << std::endl;
        return 1;
    }
    
    // Test validation report
    std::string report = config.generateValidationReport();
    if (report.find("Summary:") != std::string::npos) {
        std::cout << "✓ Validation report generation test passed" << std::endl;
    } else {
        std::cout << "✗ Validation report test failed" << std::endl;
        return 1;
    }
    
    std::cout << "\n✓ All tests passed! Config refactoring successful." << std::endl;
    return 0;
}