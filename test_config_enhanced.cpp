// Simple Config Test Program
// This program tests the enhanced configuration system

#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <string>

int main() {
    std::cout << "=== Testing Enhanced Configuration System ===" << std::endl;
    std::cout << std::endl;
    
    nlm::Config config;
    
    // Test 1: Basic set and get
    std::cout << "Test 1: Basic set and get..." << std::endl;
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    assert(config.get<int>("int_value").value() == 42);
    std::cout << "  PASSED" << std::endl;
    
    // Test 2: Type safety and error handling
    std::cout << "Test 2: Type safety and error handling..." << std::endl;
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    assert(config.get<std::string>("string_value").value() == "hello");
    
    try {
        int wrongType = config.get<int>("string_value");
        assert(false); // Should not reach here
    } catch (const nlm::ConfigTypeMismatchException& e) {
        std::cout << "  Type mismatch correctly caught: " << e.what() << std::endl;
    }
    
    // Test 3: Required getter
    std::cout << "Test 3: Required getter..." << std::endl;
    config.set("required_value", 100, nlm::ConfigSource::Runtime);
    assert(config.getRequired<int>("required_value") == 100);
    
    try {
        int missing = config.getRequired<int>("missing_value");
        assert(false); // Should not reach here
    } catch (const nlm::ConfigValidationException& e) {
        std::cout << "  Missing value correctly caught: " << e.what() << std::endl;
    }
    
    // Test 4: Schema validation
    std::cout << "Test 4: Schema validation..." << std::endl;
    nlm::SchemaConstraint intConstraint;
    intConstraint.key = "test_int";
    intConstraint.type = "int";
    intConstraint.range = std::make_pair(0.0, 100.0);
    config.addSchemaConstraint(intConstraint);
    
    config.set("test_int", 50, nlm::ConfigSource::Runtime);
    assert(config.validate()); // Should be valid
    
    config.set("test_int", 150, nlm::ConfigSource::Runtime);
    auto errors = config.validateAll();
    assert(!errors.empty());
    std::cout << "  Validation correctly failed: " << errors[0] << std::endl;
    
    // Test 5: Dependency validation
    std::cout << "Test 5: Dependency validation..." << std::endl;
    nlm::Dependency dep;
    dep.dependentKey = "result";
    dep.requiredKeys.push_back("input1");
    dep.requiredKeys.push_back("input2");
    dep.condition = "all_present";
    config.addDependency(dep);
    
    config.set("input1", 10, nlm::ConfigSource::Runtime);
    assert(!config.validate()); // Missing input2
    
    config.set("input2", 20, nlm::ConfigSource::Runtime);
    assert(config.validate()); // Now valid
    std::cout << "  Dependency validation passed" << std::endl;
    
    // Test 6: JSON export/import
    std::cout << "Test 6: JSON export/import..." << std::endl;
    config.set("json_test", std::string("json_value"), nlm::ConfigSource::Runtime);
    config.set("json_int", 123, nlm::ConfigSource::Runtime);
    
    std::string jsonString = config.exportToJSON();
    assert(!jsonString.empty());
    
    nlm::Config config2;
    assert(config2.importFromJSON(jsonString));
    
    assert(config2.get<std::string>("json_test").value() == "json_value");
    assert(config2.get<int>("json_int").value() == 123);
    std::cout << "  JSON export/import passed" << std::endl;
    
    // Test 7: Configuration comparison and merging
    std::cout << "Test 7: Configuration comparison and merging..." << std::endl;
    nlm::Config config3;
    config3.set("key1", 10, nlm::ConfigSource::Runtime);
    config3.set("key2", "value", nlm::ConfigSource::Runtime);
    
    nlm::Config config4;
    config4.set("key1", 10, nlm::ConfigSource::Runtime);
    config4.set("key2", "value", nlm::ConfigSource::Runtime);
    
    assert(config3.equals(config4)); // Should be equal
    
    config4.set("key3", 30, nlm::ConfigSource::Runtime);
    assert(!config3.equals(config4)); // Should not be equal
    
    nlm::Config config5;
    config5.set("key1", 5, nlm::ConfigSource::Runtime);
    config5.merge(config3);
    
    assert(config5.has("key1"));
    assert(config5.has("key2"));
    std::cout << "  Comparison and merging passed" << std::endl;
    
    // Test 8: Source tracking
    std::cout << "Test 8: Source tracking..." << std::endl;
    assert(config.getSource("int_value") == nlm::ConfigSource::Runtime);
    assert(config.getSource("string_value") == nlm::ConfigSource::Runtime);
    
    std::string sourceInfo = config.getSourceInfo("int_value");
    assert(sourceInfo.find("int_value") != std::string::npos);
    assert(sourceInfo.find("Runtime") != std::string::npos);
    std::cout << "  Source tracking passed" << std::endl;
    
    // Test 9: Memory usage
    std::cout << "Test 9: Memory usage..." << std::endl;
    double memoryUsage = config.getMemoryUsage();
    assert(memoryUsage > 0.0);
    std::cout << "  Memory usage: " << memoryUsage << " bytes" << std::endl;
    
    // Test 10: Configuration summary
    std::cout << "Test 10: Configuration summary..." << std::endl;
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("Configuration Summary") != std::string::npos);
    std::cout << "  Summary generated successfully" << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== All Tests PASSED ===" << std::endl;
    
    return 0;
}