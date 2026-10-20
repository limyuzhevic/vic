// Individual Test Case Implementations
// Each test case from the user's requirements specification

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <vector>

namespace test_cases {

// Test 1: Enhanced Configuration System Tests - JSON/YAML parsing
void testJsonParsing() {
    std::cout << "=== Test 1: Enhanced Configuration System Tests ===\n";
    std::cout << "Test JSON/YAML parsing functionality\n";
    
    nlm::Config config;
    ConfigResult result = config.loadFromJson("{\"key\": \"value\", \"count\": 42}");
    assert(result.isSuccess());
    std::string value;
    assert(config.get<std::string>("key", value).isSuccess());
    assert(value == "value");
    
    std::cout << "✓ JSON parsing test passed\n";
}

// Test 2: Enhanced Configuration System Tests - Enhanced CLI parsing
void testEnhancedCliParsing() {
    std::cout << "Test enhanced CLI parsing\n";
    nlm::Config config;
    
    const char* argv[] = {"program", "--enable-feature", "-f", "--count", "5"};
    ConfigResult result = config.loadFromArgs(5, const_cast<char**>(argv));
    assert(result.isSuccess());
    assert(config.has("enable-feature"));
    assert(config.has("f"));
    assert(config.has("count"));
    
    std::cout << "✓ Enhanced CLI parsing test passed\n";
}

// Test 3: Enhanced Configuration System Tests - Thread safety
void testThreadSafety() {
    std::cout << "Test thread safety\n";
    nlm::Config config;
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < 100; ++j) {
                config.set("key" + std::to_string(j), j, nlm::ConfigSource::Runtime);
                int value;
                assert(config.get<int>("key" + std::to_string(j), value).isSuccess());
                assert(value == j);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify all values are still correct
    for (int j = 0; j < 100; ++j) {
        int value;
        assert(config.get<int>("key" + std::to_string(j), value).isSuccess());
        assert(value == j);
    }
    
    std::cout << "✓ Thread safety test passed\n";
}

// Test 4: Enhanced Configuration System Tests - Error handling
void testErrorHandling() {
    std::cout << "Test error handling and recovery\n";
    nlm::Config config;
    
    // Test file not found
    ConfigResult result = config.loadFromFile("nonexistent.json");
    assert(result.isFailure());
    assert(result.getErrorCode() == nlm::ConfigErrorCode::FileNotFound);
    
    // Test invalid JSON
    ConfigResult result2 = config.loadFromJson("{invalid json");
    assert(result2.isFailure());
    assert(result2.getErrorCode() == nlm::ConfigErrorCode::ParseError);
    
    std::cout << "✓ Error handling test passed\n";
}

// Test 5: Enhanced Configuration System Tests - Input validation
void testInputValidation() {
    std::cout << "Test input validation\n";
    nlm::Config config;
    
    // Test key validation
    ConfigResult result = config.set("", "value", nlm::ConfigSource::Runtime);
    assert(result.isFailure());
    
    // Test value validation
    ConfigResult result2 = config.set("key", "not_a_number", nlm::ConfigSource::Runtime);
    assert(result2.isSuccess()); // Should convert to string
    
    // Test duplicate key handling
    config.set("key", "value1", nlm::ConfigSource::Runtime);
    config.set("key", "value2", nlm::ConfigSource::Runtime); // Should update
    std::string value;
    assert(config.get<std::string>("key", value).isSuccess());
    assert(value == "value2");
    
    std::cout << "✓ Input validation test passed\n";
}

// Test 6: Enhanced Configuration System Tests - Memory management
void testMemoryManagement() {
    std::cout << "Test memory management\n";
    nlm::Config config;
    
    // Test that destructors clean up properly
    {
        nlm::Config tempConfig;
        tempConfig.set("key1", "value1", nlm::ConfigSource::Runtime);
        tempConfig.set("key2", "value2", nlm::ConfigSource::Runtime);
        tempConfig.set("key3", 42, nlm::ConfigSource::Runtime);
    } // tempConfig should be destroyed here
    
    Config config2;
    // Verify config2 is still functional
    config2.set("new_key", "new_value", nlm::ConfigSource::Runtime);
    std::string value;
    assert(config2.get<std::string>("new_key", value).isSuccess());
    assert(value == "new_value");
    
    std::cout << "✓ Memory management test passed\n";
}

// Test 7: Enhanced Configuration System Tests - Backward compatibility
void testBackwardCompatibility() {
    std::cout << "Test backward compatibility\n";
    nlm::Config config;
    
    // Test legacy API
    assert(config.legacySet("key", "value", nlm::ConfigSource::Runtime));
    assert(config.legacyHas("key"));
    
    std::string value = config.legacyGetOr<std::string>("key", "default");
    assert(value == "value");
    
    std::string value2 = config.legacyGetOr<std::string>("nonexistent", "default");
    assert(value2 == "default");
    
    std::cout << "✓ Backward compatibility test passed\n";
}

// Test 8: Enhanced Configuration System Tests - Performance
void testPerformance() {
    std::cout << "Test performance characteristics\n";
    nlm::Config config;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        config.set("key" + std::to_string(i), i, nlm::ConfigSource::Runtime);
        int value;
        assert(config.get<int>("key" + std::to_string(i), value).isSuccess());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Performance test: 10,000 operations in " << duration.count() << " ms\n";
    
    // Performance should be reasonable (e.g., < 1000ms for 10000 operations)
    assert(duration.count() < 1000);
    
    std::cout << "✓ Performance test passed\n";
}

// Test 9: Enhanced Configuration System Tests - Edge cases
void testEdgeCases() {
    std::cout << "Test edge cases and boundary conditions\n";
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
    config.set("key_with_special!@#$%", "value", nlm::ConfigSource::Runtime);
    config.set("key_with_unicode_αβγ", "value", nlm::ConfigSource::Runtime);
    
    std::cout << "✓ Edge cases test passed\n";
}

// Test 10: Advanced CLI Tests - Command-line argument parsing
void testCliArgumentParsing() {
    std::cout << "=== Test 10: Advanced CLI Tests ===\n";
    std::cout << "Test command-line argument parsing\n";
    
    nlm::Config config;
    
    const char* argv[] = {"program", "--debug", "--port", "8080", "--host", "localhost"};
    ConfigResult result = config.loadFromArgs(6, const_cast<char**>(argv));
    assert(result.isSuccess());
    
    assert(config.has("debug"));
    assert(config.has("port"));
    assert(config.has("host"));
    
    std::cout << "✓ CLI argument parsing test passed\n";
}

// Test 11: Advanced CLI Tests - Subcommand functionality
void testSubcommandFunctionality() {
    std::cout << "Test subcommand functionality\n";
    nlm::Config config;
    
    const char* argv[] = {"program", "server", "start", "--port", "3000"};
    ConfigResult result = config.loadFromArgs(5, const_cast<char**>(argv));
    assert(result.isSuccess());
    
    // Subcommand handling is integrated into the main config parser
    assert(config.has("port"));
    
    std::cout << "✓ Subcommand functionality test passed\n";
}

// Test 12: Advanced CLI Tests - Option parsing
void testOptionParsing() {
    std::cout << "Test option parsing\n";
    nlm::Config config;
    
    const char* argv[] = {"program", "--verbose", "-v", "--output", "file.txt", "--input", "data.csv"};
    ConfigResult result = config.loadFromArgs(7, const_cast<char**>(argv));
    assert(result.isSuccess());
    
    assert(config.has("verbose"));
    assert(config.has("v"));
    assert(config.has("output"));
    assert(config.has("input"));
    
    std::cout << "✓ Option parsing test passed\n";
}

// Test 13: Advanced CLI Tests - Help system
void testHelpSystem() {
    std::cout << "Test help system\n";
    nlm::Config config;
    
    const char* argv[] = {"program", "--help"};
    ConfigResult result = config.loadFromArgs(2, const_cast<char**>(argv));
    assert(result.isSuccess());
    
    // Help flag should be handled without errors
    std::cout << "✓ Help system test passed\n";
}

// Test 14: Advanced CLI Tests - Error conditions
void testCliErrorConditions() {
    std::cout << "Test CLI error conditions\n";
    nlm::Config config;
    
    // Test duplicate key in CLI
    const char* argv[] = {"program", "--key", "value1", "--key", "value2"};
    ConfigResult result = config.loadFromArgs(5, const_cast<char**>(argv));
    assert(result.isFailure());
    assert(result.getErrorCode() == nlm::ConfigErrorCode::DuplicateKey);
    
    std::cout << "✓ CLI error conditions test passed\n";
}

// Test 15: Integration Tests - Integration with existing system
void testIntegrationWithSystem() {
    std::cout << "=== Test 15: Integration Tests ===\n";
    std::cout << "Test integration with existing system\n";
    
    nlm::Config config;
    
    // Set up system configuration
    assert(config.set("system.enabled", true, nlm::ConfigSource::Runtime).isSuccess());
    assert(config.set("system.port", 8080, nlm::ConfigSource::Runtime).isSuccess());
    assert(config.set("system.host", "localhost", nlm::ConfigSource::Runtime).isSuccess());
    
    // Verify configuration
    bool enabled;
    assert(config.get<bool>("system.enabled", enabled).isSuccess());
    assert(enabled == true);
    
    int port;
    assert(config.get<int>("system.port", port).isSuccess());
    assert(port == 8080);
    
    std::string host;
    assert(config.get<std::string>("system.host", host).isSuccess());
    assert(host == "localhost");
    
    std::cout << "✓ Integration with system test passed\n";
}

// Test 16: Integration Tests - Cross-platform compatibility
void testCrossPlatformCompatibility() {
    std::cout << "Test cross-platform compatibility\n";
    nlm::Config config;
    
    // Test different path formats
    assert(config.set("unix_path", "/usr/bin/local", nlm::ConfigSource::Runtime).isSuccess());
    assert(config.set("windows_path", "C:\\Windows\\System32", nlm::ConfigSource::Runtime).isSuccess());
    assert(config.set("mixed_path", "C:/Program Files/App", nlm::ConfigSource::Runtime).isSuccess());
    
    // Test string encodings
    assert(config.set("utf8_string", "Hello, 世界", nlm::ConfigSource::Runtime).isSuccess());
    assert(config.set("emoji_string", "😀🎉🚀", nlm::ConfigSource::Runtime).isSuccess());
    
    std::cout << "✓ Cross-platform compatibility test passed\n";
}

// Test 17: Integration Tests - Performance characteristics
void testIntegrationPerformance() {
    std::cout << "Test integration performance characteristics\n";
    
    nlm::Config config;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Simulate heavy integration operations
    for (int i = 0; i < 5000; ++i) {
        config.set("integration.key_" + std::to_string(i), i, nlm::ConfigSource::Runtime);
        int value;
        assert(config.get<int>("integration.key_" + std::to_string(i), value).isSuccess());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Integration performance: 5,000 operations in " << duration.count() << " ms\n";
    assert(duration.count() < 500);
    
    std::cout << "✓ Integration performance test passed\n";
}

// Test 18: Edge Case Tests - Malformed configuration files
void testMalformedConfigurationFiles() {
    std::cout << "=== Test 18: Edge Case Tests ===\n";
    std::cout << "Test malformed configuration files\n";
    
    nlm::Config config;
    
    // Create malformed JSON file
    std::ofstream badJsonFile("malformed.json");
    badJsonFile << "{\"incomplete": true,\n\"missing_end\"};";
    badJsonFile.close();
    
    ConfigResult result = config.loadFromFile("malformed.json");
    assert(result.isFailure());
    assert(result.getErrorCode() == nlm::ConfigErrorCode::ParseError);
    
    // Create empty file
    std::ofstream emptyFile("empty.txt");
    emptyFile.close();
    
    ConfigResult result2 = config.loadFromFile("empty.txt");
    assert(result2.isSuccess()); // Empty file should be valid
    
    // Create file with invalid characters
    std::ofstream invalidFile("invalid.txt");
    invalidFile << "key = value\ninvalid line without equals\nanother = bad\n\x00null byte";
    invalidFile.close();
    
    ConfigResult result3 = config.loadFromFile("invalid.txt");
    // May succeed or fail depending on implementation
    
    // Cleanup
    std::remove("malformed.json");
    std::remove("empty.txt");
    std::remove("invalid.txt");
    
    std::cout << "✓ Malformed configuration files test passed\n";
}

// Test 19: Edge Case Tests - Memory corruption scenarios
void testMemoryCorruptionScenarios() {
    std::cout << "Test memory corruption scenarios\n";
    
    nlm::Config config;
    
    // Test rapid allocation/deallocation
    for (int i = 0; i < 100; ++i) {
        nlm::Config tempConfig;
        tempConfig.set("temp_key_" + std::to_string(i), "temp_value", nlm::ConfigSource::Runtime);
        std::string value;
        assert(tempConfig.get<std::string>("temp_key_" + std::to_string(i), value).isSuccess());
    }
    
    // Test large data structures
    std::vector<std::string> largeData(1000);
    for (int i = 0; i < 1000; ++i) {
        largeData[i] = "large_data_" + std::to_string(i);
    }
    
    for (int i = 0; i < 100; ++i) {
        config.set("large_key_" + std::to_string(i), largeData[i], nlm::ConfigSource::Runtime);
    }
    
    std::cout << "✓ Memory corruption scenarios test passed\n";
}

// Test 20: Edge Case Tests - Race conditions
void testRaceConditions() {
    std::cout << "Test race conditions\n";
    
    nlm::Config config;
    
    std::vector<std::thread> raceThreads;
    const int numThreads = 5;
    const int operationsPerThread = 200;
    
    for (int t = 0; t < numThreads; ++t) {
        raceThreads.emplace_back([&, t]() {
            for (int i = 0; i < operationsPerThread; ++i) {
                std::string key = "race_key_" + std::to_string(t) + "_" + std::to_string(i);
                config.set(key, t * 1000 + i, nlm::ConfigSource::Runtime);
                int value;
                assert(config.get<int>(key, value).isSuccess());
                assert(value == t * 1000 + i);
            }
        });
    }
    
    for (auto& thread : raceThreads) {
        thread.join();
    }
    
    // Verify all race condition test values
    for (int t = 0; t < numThreads; ++t) {
        for (int i = 0; i < operationsPerThread; ++i) {
            std::string key = "race_key_" + std::to_string(t) + "_" + std::to_string(i);
            int value;
            assert(config.get<int>(key, value).isSuccess());
            assert(value == t * 1000 + i);
        }
    }
    
    std::cout << "✓ Race conditions test passed\n";
}

// Test 21: Edge Case Tests - Boundary conditions
void testBoundaryConditions() {
    std::cout << "Test boundary conditions\n";
    
    nlm::Config config;
    
    // Test zero values
    assert(config.set("zero_int", 0, nlm::ConfigSource::Runtime).isSuccess());
    assert(config.set("zero_double", 0.0, nlm::ConfigSource::Runtime).isSuccess());
    assert(config.set("zero_bool", false, nlm::ConfigSource::Runtime).isSuccess());
    assert(config.set("zero_string", "", nlm::ConfigSource::Runtime).isSuccess());
    
    // Test negative values
    assert(config.set("negative_int", -2147483648, nlm::ConfigSource::Runtime).isSuccess());
    assert(config.set("negative_double", -3.14159, nlm::ConfigSource::Runtime).isSuccess());
    
    // Test very large values
    assert(config.set("large_int", 2147483647, nlm::ConfigSource::Runtime).isSuccess());
    assert(config.set("large_double", 1.7976931348623157e+308, nlm::ConfigSource::Runtime).isSuccess());
    
    // Test maximum key length
    std::string maxKey = "a";
    for (int i = 0; i < 4095; ++i) maxKey += "b"; // Close to MAX_STRING_LENGTH
    assert(config.set(maxKey, "max_value", nlm::ConfigSource::Runtime).isSuccess());
    
    std::cout << "✓ Boundary conditions test passed\n";
}

// Test 22: Performance Tests - Parsing performance
void testParsingPerformance() {
    std::cout << "=== Test 22: Performance Tests ===\n";
    std::cout << "Test parsing performance\n";
    
    nlm::Config config;
    
    // Test JSON parsing performance
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        config.loadFromJson("{\"test\": \"value\", \"number\": " + std::to_string(i) + "}");
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    auto jsonDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "JSON parsing: 1000 operations in " << jsonDuration.count() << " ms\n";
    assert(jsonDuration.count() < 100);
    
    // Clear config for next test
    config.clear();
    
    std::cout << "✓ Parsing performance test passed\n";
}

// Test 23: Performance Tests - Memory usage
void testMemoryUsage() {
    std::cout << "Test memory usage\n";
    
    nlm::Config config;
    
    // Test memory efficiency with many keys
    for (int i = 0; i < 5000; ++i) {
        config.set("mem_key_" + std::to_string(i), "mem_value_" + std::to_string(i), nlm::ConfigSource::Runtime);
    }
    
    // Verify all keys are accessible
    int count = 0;
    for (const auto& key : config.getKeys()) {
        if (key.find("mem_key_") == 0) {
            count++;
        }
    }
    
    assert(count == 5000);
    
    std::cout << "✓ Memory usage test passed\n";
}

// Test 24: Performance Tests - Concurrent access performance
void testConcurrentAccessPerformance() {
    std::cout << "Test concurrent access performance\n";
    
    nlm::Config config;
    
    // Pre-populate with data
    for (int i = 0; i < 1000; ++i) {
        config.set("concurrent_key_" + std::to_string(i), i, nlm::ConfigSource::Runtime);
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> concurrentThreads;
    const int numThreads = 10;
    
    for (int t = 0; t < numThreads; ++t) {
        concurrentThreads.emplace_back([&, t]() {
            for (int i = 0; i < 100; ++i) {
                int index = (t * 100 + i) % 1000;
                int value;
                assert(config.get<int>("concurrent_key_" + std::to_string(index), value).isSuccess());
                assert(value == index);
            }
        });
    }
    
    for (auto& thread : concurrentThreads) {
        thread.join();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Concurrent access: 1000 operations in " << duration.count() << " ms\n";
    assert(duration.count() < 100);
    
    std::cout << "✓ Concurrent access performance test passed\n";
}

// Test 25: Stress Tests - Extreme conditions
void testStressExtremeConditions() {
    std::cout << "=== Test 25: Stress Tests ===\n";
    std::cout << "Test extreme conditions\n";
    
    nlm::Config config;
    
    // Test extreme key/value combinations
    for (int i = 0; i < 100; ++i) {
        std::string key = std::string(100, 'x') + "_" + std::to_string(i);
        std::string value = std::string(100, 'y') + "_" + std::to_string(i);
        assert(config.set(key, value, nlm::ConfigSource::Runtime).isSuccess());
    }
    
    // Test rapid create/destroy cycles
    for (int i = 0; i < 50; ++i) {
        nlm::Config tempConfig;
        for (int j = 0; j < 20; ++j) {
            tempConfig.set("temp_" + std::to_string(j), "value", nlm::ConfigSource::Runtime);
        }
        // tempConfig is destroyed here
    }
    
    std::cout << "✓ Extreme conditions stress test passed\n";
}

// Test 26: Stress Tests - Memory exhaustion simulation
void testStressMemoryExhaustion() {
    std::cout << "Test memory exhaustion simulation\n";
    
    nlm::Config config;
    
    // Allocate as much as possible (simulated)
    std::vector<nlm::Config> configs;
    try {
        for (int i = 0; i < 100; ++i) {
            nlm::Config tempConfig;
            tempConfig.set("stress_key_" + std::to_string(i), "stress_value", nlm::ConfigSource::Runtime);
            configs.push_back(std::move(tempConfig));
        }
    } catch (const std::exception&) {
        // Expected in extreme memory conditions
    }
    
    std::cout << "✓ Memory exhaustion simulation test passed\n";
}

// Test 27: Stress Tests - Thread exhaustion simulation
void testStressThreadExhaustion() {
    std::cout << "Test thread exhaustion simulation\n";
    
    nlm::Config config;
    
    std::vector<std::thread> threads;
    const int numThreads = 50;
    
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&, t]() {
            nlm::Config threadConfig;
            for (int i = 0; i < 20; ++i) {
                threadConfig.set("thread_key_" + std::to_string(t) + "_" + std::to_string(i), 
                                t * 100 + i, nlm::ConfigSource::Runtime);
                int value;
                assert(threadConfig.get<int>("thread_key_" + std::to_string(t) + "_" + std::to_string(i), value).isSuccess());
                assert(value == t * 100 + i);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    std::cout << "✓ Thread exhaustion simulation test passed\n";
}

// Main test runner for individual test cases
void runAllTestCases() {
    std::cout << "=== Individual Test Case Execution ===\n\n";
    
    // Enhanced Configuration System Tests
    testJsonParsing();
    testEnhancedCliParsing();
    testThreadSafety();
    testErrorHandling();
    testInputValidation();
    testMemoryManagement();
    testBackwardCompatibility();
    testPerformance();
    testEdgeCases();
    
    // Advanced CLI Tests
    testCliArgumentParsing();
    testSubcommandFunctionality();
    testOptionParsing();
    testHelpSystem();
    testCliErrorConditions();
    
    // Integration Tests
    testIntegrationWithSystem();
    testCrossPlatformCompatibility();
    testIntegrationPerformance();
    
    // Edge Case Tests
    testMalformedConfigurationFiles();
    testMemoryCorruptionScenarios();
    testRaceConditions();
    testBoundaryConditions();
    
    // Performance Tests
    testParsingPerformance();
    testMemoryUsage();
    testConcurrentAccessPerformance();
    
    // Stress Tests
    testStressExtremeConditions();
    testStressMemoryExhaustion();
    testStressThreadExhaustion();
    
    std::cout << "\n=== All individual test cases completed successfully! ===\n";
}

} // namespace test_cases

int main() {
    test_cases::runAllTestCases();
    return 0;
}
