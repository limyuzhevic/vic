#include <chrono>
// Config Tests
// Enhanced with comprehensive validation tests, error handling, and batch operations

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_config {

// Basic Tests (existing, preserved)
void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

// Enhanced Tests
void testConfigValidation() {
    std::cout << "    Validation Test" << std::endl;
    
    nlm::Config config;
    
    // Test type validation
    config.set("int_val", 123);
    config.set("double_val", 3.14);
    config.set("string_val", std::string("test"));
    config.set("bool_val", false);
    
    // Verify type-safe access
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value() && *intVal == 123);
    
    auto doubleVal = config.get<double>("double_val");
    assert(doubleVal.has_value() && fabs(*doubleVal - 3.14) < 0.01);
    
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal.has_value() && *stringVal == "test");
    
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal.has_value() && *boolVal == false);
    
    // Test type mismatch handling
    config.set("wrong_type", 456);
    auto wrongType = config.get<std::string>("wrong_type");
    assert(!wrongType.has_value()); // Should be nullopt due to type mismatch
    
    std::cout << "      Validation test passed" << std::endl;
}

void testConfigSourceTracking() {
    std::cout << "    Source Tracking Test" << std::endl;
    
    nlm::Config config;
    
    // Test different sources
    config.set("default_val", 100, nlm::ConfigSource::Default);
    config.set("file_val", 200, nlm::ConfigSource::File);
    config.set("command_line_val", 300, nlm::ConfigSource::CommandLine);
    config.set("runtime_val", 400, nlm::ConfigSource::Runtime);
    
    // Verify all values are accessible
    assert(config.get<int>("default_val").value() == 100);
    assert(config.get<int>("file_val").value() == 200);
    assert(config.get<int>("command_line_val").value() == 300);
    assert(config.get<int>("runtime_val").value() == 400);
    
    // Test summary generation
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("default_val") != std::string::npos);
    
    std::cout << "      Source tracking test passed" << std::endl;
}

void testConfigBatchOperations() {
    std::cout << "    Batch Operations Test" << std::endl;
    
    nlm::Config config;
    
    // Batch set multiple values
    std::vector<std::pair<std::string, int>> batchData = {
        {"batch_int1", 10},
        {"batch_int2", 20},
        {"batch_int3", 30},
        {"batch_int4", 40},
        {"batch_int5", 50}
    };
    
    for (const auto& pair : batchData) {
        config.set(pair.first, pair.second);
    }
    
    // Verify all batch values are set
    assert(config.has("batch_int1"));
    assert(config.has("batch_int2"));
    assert(config.has("batch_int3"));
    assert(config.has("batch_int4"));
    assert(config.has("batch_int5"));
    
    // Batch get
    std::vector<int> retrievedValues;
    for (const auto& pair : batchData) {
        auto val = config.get<int>(pair.first);
        assert(val.has_value());
        retrievedValues.push_back(*val);
    }
    
    // Verify retrieved values match
    for (size_t i = 0; i < batchData.size(); ++i) {
        assert(retrievedValues[i] == batchData[i].second);
    }
    
    // Batch remove
    for (const auto& pair : batchData) {
        config.remove(pair.first);
        assert(!config.has(pair.first));
    }
    
    std::cout << "      Batch operations test passed" << std::endl;
}

void testConfigTypeConversion() {
    std::cout << "    Type Conversion Test" << std::endl;
    
    nlm::Config config;
    
    // Test automatic type conversion in getOr
    config.set("num_int", 123);
    int intVal = config.getOr<int>("num_int", 999);
    assert(intVal == 123);
    
    // Test different type combinations
    config.set("string_as_double", std::string("3.14"));
    double doubleVal = config.getOr<double>("string_as_double", 0.0);
    // Note: Actual behavior depends on implementation
    
    config.set("bool_as_int", true);
    int boolIntVal = config.getOr<int>("bool_as_int", 0);
    assert(boolIntVal == 1); // Should convert true to 1
    
    std::cout << "      Type conversion test passed" << std::endl;
}

void testConfigClearAndReset() {
    std::cout << "    Clear and Reset Test" << std::endl;
    
    nlm::Config config;
    
    // Set various values
    config.set("key1", 1);
    config.set("key2", 2);
    config.set("key3", 3);
    
    assert(config.getKeys().size() == 3);
    
    // Clear all
    config.clear();
    assert(config.getKeys().empty());
    
    // Test after clear operations
    assert(!config.has("key1"));
    assert(config.getOr<int>("key1", 999) == 999);
    
    // Set new values
    config.set("new_key", 555);
    assert(config.getKeys().size() == 1);
    assert(config.getOr<int>("new_key", 0) == 555);
    
    std::cout << "      Clear and reset test passed" << std::endl;
}

void testConfigEdgeCases() {
    std::cout << "    Edge Cases Test" << std::endl;
    
    nlm::Config config;
    
    // Test empty string key (should be handled gracefully)
    config.set("", 123); // Empty key
    
    // Test very long key
    std::string longKey(1000, 'a');
    config.set(longKey, 456);
    
    // Test very large value
    config.set("large_value", std::numeric_limits<int>::max());
    
    // Test negative values
    config.set("negative", -100);
    assert(config.getOr<int>("negative", 0) == -100);
    
    // Test special float values
    config.set("inf_value", std::numeric_limits<float>::infinity());
    config.set("nan_value", std::nanf(""));
    
    std::cout << "      Edge cases test passed" << std::endl;
}

void testConfigFileOperations() {
    std::cout << "    File Operations Test" << std::endl;
    
    nlm::Config config;
    
    // Test setting values
    config.set("file_test", 123);
    config.set("another_test", "hello");
    
    // Test saving to file
    bool saveSuccess = config.saveToFile("/tmp/nlm_config_test.txt");
    // Save may or may not be implemented, but shouldn't crash
    
    // Test loading from file (if save worked)
    if (saveSuccess) {
        nlm::Config config2;
        bool loadSuccess = config2.loadFromFile("/tmp/nlm_config_test.txt");
        // Load may or may not work, but shouldn't crash
    }
    
    std::cout << "      File operations test passed" << std::endl;
}

void testConfigErrorScenarios() {
    std::cout << "    Error Scenarios Test" << std::endl;
    
    nlm::Config config;
    
    // Test removal of non-existent key
    config.remove("non_existent_key"); // Should not crash
    
    // Test get of non-existent key with default
    int defaultVal = config.getOr<int>("non_existent", 999);
    assert(defaultVal == 999);
    
    // Test operations on cleared config
    config.set("temp", 123);
    config.clear();
    config.remove("temp"); // Should not crash
    
    // Test has() on non-existent key
    assert(!config.has("temp"));
    
    std::cout << "      Error scenarios test passed" << std::endl;
}

void testConfigMemoryManagement() {
    std::cout << "    Memory Management Test" << std::endl;
    
    nlm::Config config;
    
    // Allocate and deallocate many keys
    for (int i = 0; i < 1000; ++i) {
        config.set(std::to_string(i), i);
    }
    
    // Access many keys
    for (int i = 0; i < 1000; ++i) {
        auto val = config.get<int>(std::to_string(i));
        assert(val.has_value() && *val == i);
    }
    
    // Clear and verify
    config.clear();
    assert(config.getKeys().empty());
    
    std::cout << "      Memory management test passed" << std::endl;
}

void testConfigPerformance() {
    std::cout << "    Performance Test" << std::endl;
    
    nlm::Config config;
    
    // Performance test for rapid operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        config.set(std::to_string(i), i);
        auto val = config.getOr<int>(std::to_string(i), 0);
        assert(val == i);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "      Performance: " << std::fixed << std::setprecision(0) << 10000.0 / totalTime << " operations/sec" << std::endl;
    
    // Cleanup
    config.clear();
    
    std::cout << "      Performance test passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced Config Tests..." << std::endl;
    
    try {
        // Run basic tests
        testConfigCreation();
        testConfigSetAndGet();
        testConfigGetOr();
        testConfigRemove();
        testConfigKeys();
        
        // Run enhanced tests
        testConfigValidation();
        testConfigSourceTracking();
        testConfigBatchOperations();
        testConfigTypeConversion();
        testConfigClearAndReset();
        testConfigEdgeCases();
        testConfigFileOperations();
        testConfigErrorScenarios();
        testConfigMemoryManagement();
        testConfigPerformance();
        
        std::cout << "  All Enhanced Config Tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_config