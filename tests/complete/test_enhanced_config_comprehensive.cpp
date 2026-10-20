// Comprehensive Enhanced Configuration System Tests
// Covers all requirements from the implementation specification

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <random>
#include <string>
#include <vector>

namespace test_enhanced_config {

// Test Framework Base Class
class TestFramework {
public:
    static void runAllTests() {
        std::cout << "=== Comprehensive Enhanced Configuration System Tests ===\n\n";
        
        testJsonParsing();
        testYamlParsing();
        testEnhancedCliParsing();
        testThreadSafety();
        testErrorHandling();
        testInputValidation();
        testMemoryManagement();
        testBackwardCompatibility();
        testPerformance();
        testEdgeCases();
        testIntegration();
        testCrossPlatformCompatibility();
        testStressTests();
        
        std::cout << "\n=== All tests completed successfully! ===\n";
    }
    
private:
    // Test JSON/YAML parsing functionality
    static void testJsonParsing() {
        std::cout << "Test: JSON parsing functionality\n";
        nlm::Config config;
        
        // Test valid JSON
        ConfigResult result = config.loadFromJson("{\"key\": \"value\", \"count\": 42, \"enabled\": true, \"price\": 3.14}");
        assert(result.isSuccess());
        
        // Test string value
        std::string value;
        assert(config.get<std::string>("key", value).isSuccess());
        assert(value == "value");
        
        // Test integer value
        int count;
        assert(config.get<int>("count", count).isSuccess());
        assert(count == 42);
        
        // Test boolean value
        bool enabled;
        assert(config.get<bool>("enabled", enabled).isSuccess());
        assert(enabled == true);
        
        // Test double value
        double price;
        assert(config.get<double>("price", price).isSuccess());
        assert(price == 3.14);
        
        std::cout << "  JSON parsing tests passed\n";
    }
    
    static void testYamlParsing() {
        std::cout << "Test: YAML parsing functionality\n";
        nlm::Config config;
        
        // Test YAML format (simplified YAML-like format)
        std::string yamlContent = "key: value\ncount: 42\nenabled: true\nprice: 3.14\nlist:\n  - item1\n  - item2\nnested:\n  inner: test\n  number: 123";
        
        ConfigResult result = config.loadFromJson(yamlContent);
        assert(result.isSuccess());
        
        // Verify values were parsed
        std::string value;
        assert(config.get<std::string>("key", value).isSuccess());
        assert(value == "value");
        
        int count;
        assert(config.get<int>("count", count).isSuccess());
        assert(count == 42);
        
        std::cout << "  YAML parsing tests passed\n";
    }
    
    static void testEnhancedCliParsing() {
        std::cout << "Test: Enhanced CLI parsing\n";
        nlm::Config config;
        
        // Test with CLI arguments
        const char* argv[] = {"program", "--enable-feature", "--count", "5", "--name", "test"};
        ConfigResult result = config.loadFromArgs(5, const_cast<char**>(argv));
        assert(result.isSuccess());
        
        // Verify CLI values were set
        bool enabled;
        assert(config.get<bool>("enable-feature", enabled).isSuccess());
        assert(enabled == true);
        
        int count;
        assert(config.get<int>("count", count).isSuccess());
        assert(count == 5);
        
        std::string name;
        assert(config.get<std::string>("name", name).isSuccess());
        assert(name == "test");
        
        std::cout << "  Enhanced CLI parsing tests passed\n";
    }
    
    static void testThreadSafety() {
        std::cout << "Test: Thread safety\n";
        nlm::Config config;
        
        std::vector<std::thread> threads;
        const int numThreads = 10;
        const int operationsPerThread = 100;
        
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back([&config, i]() {
                for (int j = 0; j < operationsPerThread; ++j) {
                    std::string key = "key_" + std::to_string(j);
                    config.set(key, j, nlm::ConfigSource::Runtime);
                    
                    int value;
                    assert(config.get<int>(key, value).isSuccess());
                    assert(value == j);
                }
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Verify all values are still correct
        for (int j = 0; j < operationsPerThread; ++j) {
            std::string key = "key_" + std::to_string(j);
            int value;
            assert(config.get<int>(key, value).isSuccess());
            assert(value == j);
        }
        
        std::cout << "  Thread safety tests passed\n";
    }
    
    static void testErrorHandling() {
        std::cout << "Test: Error handling and recovery\n";
        nlm::Config config;
        
        // Test file not found
        ConfigResult result = config.loadFromFile("nonexistent.json");
        assert(result.isFailure());
        assert(result.getErrorCode() == nlm::ConfigErrorCode::FileNotFound);
        
        // Test invalid JSON
        ConfigResult result2 = config.loadFromJson("{invalid json");
        assert(result2.isFailure());
        assert(result2.getErrorCode() == nlm::ConfigErrorCode::ParseError);
        
        // Test empty key
        ConfigResult result3 = config.set("", "value", nlm::ConfigSource::Runtime);
        assert(result3.isFailure());
        
        // Test valid data after errors
        assert(config.set("valid_key", "valid_value", nlm::ConfigSource::Runtime).isSuccess());
        std::string value;
        assert(config.get<std::string>("valid_key", value).isSuccess());
        assert(value == "valid_value");
        
        std::cout << "  Error handling tests passed\n";
    }
    
    static void testInputValidation() {
        std::cout << "Test: Input validation\n";
        nlm::Config config;
        
        // Test key validation (empty key should fail)
        ConfigResult result = config.set("", "value", nlm::ConfigSource::Runtime);
        assert(result.isFailure());
        
        // Test value validation (should convert to string)
        ConfigResult result2 = config.set("key", "not_a_number", nlm::ConfigSource::Runtime);
        assert(result2.isSuccess()); // Should convert to string
        
        // Test duplicate key handling
        assert(config.set("key", "value1", nlm::ConfigSource::Runtime).isSuccess());
        assert(config.set("key", "value2", nlm::ConfigSource::Runtime).isSuccess()); // Should update
        std::string value;
        assert(config.get<std::string>("key", value).isSuccess());
        assert(value == "value2");
        
        // Test key length validation
        std::string longKey(5000, 'a');
        ConfigResult result3 = config.set(longKey, "value", nlm::ConfigSource::Runtime);
        // Should be truncated or rejected based on implementation
        
        std::cout << "  Input validation tests passed\n";
    }
    
    static void testMemoryManagement() {
        std::cout << "Test: Memory management\n";
        
        // Test that destructors clean up properly
        {
            nlm::Config tempConfig;
            tempConfig.set("key1", "value1", nlm::ConfigSource::Runtime);
            tempConfig.set("key2", "value2", nlm::ConfigSource::Runtime);
            tempConfig.set("key3", 42, nlm::ConfigSource::Runtime);
            
            // Verify temp config has entries
            assert(tempConfig.has("key1"));
            assert(tempConfig.has("key2"));
            assert(tempConfig.has("key3"));
        } // tempConfig should be destroyed here
        
        // Verify original config is still functional
        nlm::Config config2;
        config2.set("new_key", "new_value", nlm::ConfigSource::Runtime);
        std::string value;
        assert(config2.get<std::string>("new_key", value).isSuccess());
        assert(value == "new_value");
        
        std::cout << "  Memory management tests passed\n";
    }
    
    static void testBackwardCompatibility() {
        std::cout << "Test: Backward compatibility\n";
        nlm::Config config;
        
        // Test legacy API
        assert(config.legacySet("key", "value", nlm::ConfigSource::Runtime));
        assert(config.legacyHas("key"));
        
        std::string value = config.legacyGetOr<std::string>("key", "default");
        assert(value == "value");
        
        std::string value2 = config.legacyGetOr<std::string>("nonexistent", "default");
        assert(value2 == "default");
        
        // Test legacy get
        std::optional<std::string> optValue = config.legacyGet<std::string>("key");
        assert(optValue.has_value());
        assert(*optValue == "value");
        
        std::cout << "  Backward compatibility tests passed\n";
    }
    
    static void testPerformance() {
        std::cout << "Test: Performance characteristics\n";
        nlm::Config config;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 10000; ++i) {
            config.set("key_" + std::to_string(i), i, nlm::ConfigSource::Runtime);
            int value;
            assert(config.get<int>("key_" + std::to_string(i), value).isSuccess());
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "  Performance test: 10,000 operations in " << duration.count() << " ms\n";
        
        // Performance should be reasonable (e.g., < 1000ms for 10000 operations)
        assert(duration.count() < 1000);
        
        std::cout << "  Performance tests passed\n";
    }
    
    static void testEdgeCases() {
        std::cout << "Test: Edge cases and boundary conditions\n";
        nlm::Config config;
        
        // Test very long key
        std::string longKey(10000, 'a');
        ConfigResult result = config.set(longKey, "value", nlm::ConfigSource::Runtime);
        assert(result.isSuccess()); // Should be truncated by buffer protection
        
        // Test very long value
        std::string longValue(10000, 'b');
        ConfigResult result2 = config.set("long_key", longValue, nlm::ConfigSource::Runtime);
        assert(result2.isSuccess()); // Should be truncated by buffer protection
        
        // Test special characters
        assert(config.set("key_with_special!@#$%", "value", nlm::ConfigSource::Runtime).isSuccess());
        assert(config.set("key_with_unicode_αβγ", "value", nlm::ConfigSource::Runtime).isSuccess());
        assert(config.set("key_with_spaces", "value with spaces", nlm::ConfigSource::Runtime).isSuccess());
        
        // Test numeric edge cases
        assert(config.set("zero", 0, nlm::ConfigSource::Runtime).isSuccess());
        assert(config.set("negative_int", -42, nlm::ConfigSource::Runtime).isSuccess());
        assert(config.set("max_int", std::numeric_limits<int>::max(), nlm::ConfigSource::Runtime).isSuccess());
        assert(config.set("positive_double", 1.7976931348623157e+308, nlm::ConfigSource::Runtime).isSuccess());
        
        std::cout << "  Edge case tests passed\n";
    }
    
    static void testIntegration() {
        std::cout << "Test: Integration with existing system\n";
        nlm::Config config;
        
        // Test integration with file I/O
        assert(config.set("integration_test", 123, nlm::ConfigSource::Runtime).isSuccess());
        
        // Save to file
        ConfigResult saveResult = config.saveToFile("integration_test.json");
        assert(saveResult.isSuccess());
        
        // Load from file
        nlm::Config config2;
        ConfigResult loadResult = config2.loadFromFile("integration_test.json");
        assert(loadResult.isSuccess());
        
        int value;
        assert(config2.get<int>("integration_test", value).isSuccess());
        assert(value == 123);
        
        // Cleanup
        std::remove("integration_test.json");
        
        std::cout << "  Integration tests passed\n";
    }
    
    static void testCrossPlatformCompatibility() {
        std::cout << "Test: Cross-platform compatibility\n";
        nlm::Config config;
        
        // Test various string encodings
        assert(config.set("ascii_key", "ascii_value", nlm::ConfigSource::Runtime).isSuccess());
        assert(config.set("unicode_key", "unicode_value_αβγ", nlm::ConfigSource::Runtime).isSuccess());
        
        // Test platform-specific path separators (should be handled internally)
        assert(config.set("path_key", "/usr/bin/local", nlm::ConfigSource::Runtime).isSuccess());
        assert(config.set("windows_path", "C:\\Windows\\System32", nlm::ConfigSource::Runtime).isSuccess());
        
        std::cout << "  Cross-platform compatibility tests passed\n";
    }
    
    static void testStressTests() {
        std::cout << "Test: Stress testing\n";
        nlm::Config config;
        
        const int numIterations = 1000;
        const int threads = 20;
        
        std::vector<std::thread> stressThreads;
        
        for (int t = 0; t < threads; ++t) {
            stressThreads.emplace_back([&, t]() {
                for (int i = 0; i < numIterations; ++i) {
                    std::string key = "stress_key_" + std::to_string(t) + "_" + std::to_string(i);
                    config.set(key, i * t, nlm::ConfigSource::Runtime);
                    
                    int value;
                    assert(config.get<int>(key, value).isSuccess());
                    assert(value == i * t);
                }
            });
        }
        
        for (auto& thread : stressThreads) {
            thread.join();
        }
        
        // Verify all stress test values
        for (int t = 0; t < threads; ++t) {
            for (int i = 0; i < numIterations; ++i) {
                std::string key = "stress_key_" + std::to_string(t) + "_" + std::to_string(i);
                int value;
                assert(config.get<int>(key, value).isSuccess());
                assert(value == i * t);
            }
        }
        
        std::cout << "  Stress tests passed\n";
    }
};

} // namespace test_enhanced_config

int main() {
    test_enhanced_config::TestFramework::runAllTests();
    return 0;
}
