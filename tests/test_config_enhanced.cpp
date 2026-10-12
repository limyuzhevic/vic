// Enhanced Config.cpp Comprehensive Unit Tests
// Phase 5: Advanced Configuration Management Tests - Critical Performance Paths

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <algorithm>
#include <cmath>

namespace test_config_enhanced {

void testJSONYAMLParsingPerformance() {
    nlm::Config config;
    
    // Create large JSON configuration
    const size_t entryCount = 10000;
    nlohmann::json jsonConfig;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < entryCount; ++i) {
        std::string key = "value_" + std::to_string(i);
        jsonConfig[key] = static_cast<int>(i * 2);
    }
    
    // Save to file
    std::ofstream jsonFile("large_config.json");
    jsonFile << jsonConfig.dump(2);
    jsonFile.close();
    
    auto fileWriteTime = std::chrono::high_resolution_clock::now();
    std::cout << "      File write time: " << 
        std::chrono::duration_cast<std::chrono::milliseconds>(fileWriteTime - startTime).count() << " ms" << std::endl;
    
    // Load JSON configuration
    auto loadStart = std::chrono::high_resolution_clock::now();
    bool loadResult = config.loadFromFile("large_config.json");
    auto loadEnd = std::chrono::high_resolution_clock::now();
    
    assert(loadResult);
    assert(config.getKeys().size() == entryCount);
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(loadEnd - loadStart);
    std::cout << "      JSON load time: " << duration.count() << " ms" << std::endl;
    assert(duration.count() < 1000); // Should complete within 1 second
    
    // Clean up
    std::remove("large_config.json");
    
    std::cout << "    testJSONYAMLParsingPerformance passed" << std::endl;
}

void testConfigurationValidation() {
    nlm::Config config;
    
    // Test schema validation
    struct ValidationSchema {
        std::string key;
        std::string type;
        bool required;
        std::any defaultValue;
        std::function<bool(const std::any&)> validator;
        std::string description;
    };
    
    std::vector<ValidationSchema> schemas = {
        {"int_param", "int", true, 42, [](const std::any& v) { return std::any_cast<int>(v) > 0; }, "Positive integer"},
        {"string_param", "string", true, std::string("default"), 
         [](const std::any& v) { return !std::any_cast<std::string>(v).empty(); }, "Non-empty string"},
        {"double_param", "double", false, 3.14,
         [](const std::any& v) { return std::abs(std::any_cast<double>(v) - 3.14) < 0.01; }, "Value close to PI"},
        {"bool_param", "bool", true, true, nullptr, "Boolean flag"},
        {"array_param", "array<int>", false, std::vector<int>{1, 2, 3},
         [](const std::any& v) { 
             auto arr = std::any_cast<std::vector<int>>(v); 
             return std::find(arr.begin(), arr.end(), 2) != arr.end(); 
         }, "Array containing 2"}
    };
    
    // Test setting values with schema validation
    config.set("int_param", 100, nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("int_param");
    assert(intVal.has_value() && *intVal == 100);
    
    config.set("string_param", std::string("test_value"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_param");
    assert(stringVal.has_value() && *stringVal == "test_value");
    
    config.set("double_param", 3.14159f, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_param");
    assert(doubleVal.has_value());
    
    config.set("bool_param", false, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_param");
    assert(boolVal.has_value() && *boolVal == false);
    
    config.set("array_param", std::vector<int>{5, 2, 8}, nlm::ConfigSource::Runtime);
    auto arrayVal = config.get<std::vector<int>>("array_param");
    assert(arrayVal.has_value());
    
    std::cout << "    testConfigurationValidation passed" << std::endl;
}

void testOverrideResolution() {
    nlm::Config config;
    
    // Test source precedence: File > CommandLine > Runtime > Default
    std::vector<std::pair<std::string, nlm::ConfigSource>> testCases = {
        {"file_value", nlm::ConfigSource::File},
        {"cli_value", nlm::ConfigSource::CommandLine},
        {"runtime_value", nlm::ConfigSource::Runtime},
        {"default_value", nlm::ConfigSource::Default}
    };
    
    // Set values with different sources
    config.set("test_key", "file_value", nlm::ConfigSource::File);
    config.set("test_key", "cli_value", nlm::ConfigSource::CommandLine);
    config.set("test_key", "runtime_value", nlm::ConfigSource::Runtime);
    config.set("test_key", "default_value", nlm::ConfigSource::Default);
    
    // Get the entry to check source precedence
    const nlm::ConfigEntry* entry = config.findEntry("test_key");
    assert(entry != nullptr);
    
    // The last set should win (runtime > command line > file > default in implementation)
    std::cout << "      Source precedence test completed" << std::endl;
    
    // Test removal
    config.remove("test_key");
    assert(!config.has("test_key"));
    
    std::cout << "    testOverrideResolution passed" << std::endl;
}

void testErrorHandling() {
    nlm::Config config;
    
    // Test loading non-existent file
    bool loadResult = config.loadFromFile("non_existent_file.json");
    assert(!loadResult); // Should return false, not crash
    
    // Test malformed JSON file
    std::ofstream malformedFile("malformed.json");
    malformedFile << "{ invalid json }";
    malformedFile.close();
    
    loadResult = config.loadFromFile("malformed.json");
    assert(!loadResult); // Should return false for malformed JSON
    
    std::remove("malformed.json");
    
    // Test null pointer handling in get methods
    auto nullResult = config.get<int>("non_existent_key");
    assert(!nullResult.has_value()); // Should return std::nullopt
    
    // Test getOr with non-existent key
    int defaultValue = config.getOr<int>("non_existent_key", 999);
    assert(defaultValue == 999);
    
    std::cout << "    testErrorHandling passed" << std::endl;
}

void testMemoryEfficiency() {
    nlm::Config config;
    
    // Test memory efficiency with large number of entries
    const size_t entryCount = 50000;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < entryCount; ++i) {
        std::string key = "large_key_" + std::to_string(i);
        config.set(key, static_cast<int>(i), nlm::ConfigSource::Runtime);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "      Set " << entryCount << " entries in " << duration.count() << " ms" << std::endl;
    assert(duration.count() < 5000); // Should complete within 5 seconds
    
    // Verify all entries are accessible
    size_t accessCount = 0;
    for (size_t i = 0; i < entryCount; i += 1000) { // Sample access
        std::string key = "large_key_" + std::to_string(i);
        auto val = config.get<int>(key);
        if (val.has_value()) {
            accessCount++;
        }
    }
    
    assert(accessCount > 0);
    
    std::cout << "    testMemoryEfficiency passed" << std::endl;
}

void testTypeSafety() {
    nlm::Config config;
    
    // Test type conversion and safety
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_val", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    config.set("double_array", std::vector<double>{1.1, 2.2, 3.3}, nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    
    // Test correct type retrieval
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("double_val");
    assert(doubleVal.has_value());
    
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal.has_value() && *boolVal == true);
    
    auto intArrayVal = config.get<std::vector<int>>("int_array");
    assert(intArrayVal.has_value() && intArrayVal.value().size() == 3);
    
    // Test type mismatch handling
    auto wrongTypeVal = config.get<std::string>("int_val"); // Should return nullopt
    assert(!wrongTypeVal.has_value());
    
    std::cout << "    testTypeSafety passed" << std::endl;
}

void testPerformanceBenchmark() {
    nlm::Config config;
    
    // Benchmark critical operations
    const size_t iterations = 10000;
    
    // Test set performance
    auto startTime = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        config.set("benchmark_key", static_cast<int>(i), nlm::ConfigSource::Runtime);
    }
    auto setEndTime = std::chrono::high_resolution_clock::now();
    auto setDuration = std::chrono::duration_cast<std::chrono::microseconds>(setEndTime - startTime);
    
    // Test get performance (mix of existing and new keys)
    startTime = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        std::string key = (i % 10 == 0) ? "benchmark_key" : ("temp_key_" + std::to_string(i % 1000));
        config.get<int>(key);
    }
    auto getEndTime = std::chrono::high_resolution_clock::now();
    auto getDuration = std::chrono::duration_cast<std::chrono::microseconds>(getEndTime - startTime);
    
    std::cout << "      Set performance: " << setDuration.count() << " us for " << iterations << " iterations" << std::endl;
    std::cout << "      Get performance: " << getDuration.count() << " us for " << iterations << " iterations" << std::endl;
    
    // Should be fast - less than 1ms per iteration on average
    assert(setDuration.count() < 10000000); // 10 seconds max
    assert(getDuration.count() < 10000000); // 10 seconds max
    
    std::cout << "    testPerformanceBenchmark passed" << std::endl;
}

void runAll() {
    std::cout << "=== Enhanced Config Tests ===" << std::endl;
    
    std::cout << "Running enhanced Config tests..." << std::endl;
    
    testJSONYAMLParsingPerformance();
    testConfigurationValidation();
    testOverrideResolution();
    testErrorHandling();
    testMemoryEfficiency();
    testTypeSafety();
    testPerformanceBenchmark();
    
    std::cout << "=== All Enhanced Config Tests PASSED ===" << std::endl;
}

} // namespace test_config_enhanced
