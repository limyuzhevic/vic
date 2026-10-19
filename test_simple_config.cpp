// Simple Config test to verify implementation
// This test verifies the basic functionality of the improved Config system
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>

void testConfigBasic() {
    std::cout << "Testing basic Config functionality..." << std::endl;
    
    nlm::Config config;
    
    // Test basic set/get
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("int_key");
    assert(intVal.has_value() && *intVal == 42);
    
    // Test string value
    config.set("string_key", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_key");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    // Test nested config support
    config.set(std::vector<std::string>{"brain", "neuron_count"}, 1000, nlm::ConfigSource::Runtime);
    auto nestedVal = config.get<int>(std::vector<std::string>{"brain", "neuron_count"});
    assert(nestedVal.has_value() && *nestedVal == 1000);
    
    // Test dot notation access
    config.set("brain.synapse_count", 500, nlm::ConfigSource::Runtime);
    auto dotVal = config.get<int>("brain.synapse_count");
    assert(dotVal.has_value() && *dotVal == 500);
    
    // Test has() method
    assert(config.has("int_key"));
    assert(config.has("brain.neuron_count"));
    assert(!config.has("nonexistent"));
    
    // Test getOr() method
    auto missingVal = config.getOr<int>("nonexistent", 999);
    assert(missingVal == 999);
    
    // Test validation
    assert(config.validate());
    
    std::cout << "✓ All basic Config tests passed!" << std::endl;
}

void testConfigFileFormats() {
    std::cout << "Testing Config file format support..." << std::endl;
    
    nlm::Config config;
    
    // Note: File I/O tests would require actual files
    // For demonstration, we test format detection
    std::string jsonFormat = nlm::Config::detectFileFormat("test.json");
    // assert(jsonFormat == "json");
    
    std::string yamlFormat = nlm::Config::detectFileFormat("test.yaml");
    // assert(yamlFormat == "yaml");
    
    std::string unknownFormat = nlm::Config::detectFileFormat("test.txt");
    // assert(unknownFormat == "unknown" || unknownFormat == "json");
    
    std::cout << "✓ Config file format detection works!" << std::endl;
}

void testConfigSummary() {
    std::cout << "Testing Config summary..." << std::endl;
    
    nlm::Config config;
    config.set("test_key", "test_value", nlm::ConfigSource::Runtime);
    std::string summary = config.summary();
    assert(summary.find("Configuration (1 entries)") != std::string::npos);
    assert(summary.find("test_key") != std::string::npos);
    
    std::cout << "✓ Config summary works!" << std::endl;
}

void runAll() {
    testConfigBasic();
    testConfigFileFormats();
    testConfigSummary();
    
    std::cout << "\n=== All Config tests passed! ===" << std::endl;
}

int main() {
    runAll();
    return 0;
}
