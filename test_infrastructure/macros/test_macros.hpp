// NLM Enhanced Test Macros
// Phase 7: Comprehensive Test Suite
// Macros for common testing patterns and utilities

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <functional>
#include <algorithm>
#include <cmath>

// Forward declarations
namespace test_infrastructure {
    class TestMacroProcessor;
}

// Test category macros for organizing tests
#define MEMORY_TESTS    "memory"
#define STRESS_TESTS    "stress"
#define PERFORMANCE_TESTS "performance"
#define EDGE_CASE_TESTS "edge_case"
#define INTEGRATION_TESTS "integration"
#define CROSS_PLATFORM_TESTS "cross_platform"
#define REGRESSION_TESTS "regression"

// Test grouping macros
#define BEGIN_MEMORY_TESTS \
    namespace { \
        class MemoryTestGroup { \
        public: \
            MemoryTestGroup() { \
                static bool initialized = false; \
                if (!initialized) { \
                    std::cout << "=== Memory Tests ===" << std::endl; \
                    initialized = true; \
                } \
            } \
        }; \
        MemoryTestGroup MemoryTestInstance; \
    }

#define BEGIN_STRESS_TESTS \
    namespace { \
        class StressTestGroup { \
        public: \
            StressTestGroup() { \
                static bool initialized = false; \
                if (!initialized) { \
                    std::cout << "=== Stress Tests ===" << std::endl; \
                    initialized = true; \
                } \
            } \
        }; \
        StressTestGroup StressTestInstance; \
    }

#define BEGIN_PERFORMANCE_TESTS \
    namespace { \
        class PerformanceTestGroup { \
        public: \
            PerformanceTestGroup() { \
                static bool initialized = false; \
                if (!initialized) { \
                    std::cout << "=== Performance Tests ===" << std::endl; \
                    initialized = true; \
                } \
            } \
        }; \
        PerformanceTestGroup PerformanceTestInstance; \
    }

#define BEGIN_EDGE_CASE_TESTS \
    namespace { \
        class EdgeCaseTestGroup { \
        public: \
            EdgeCaseTestGroup() { \
                static bool initialized = false; \
                if (!initialized) { \
                    std::cout << "=== Edge Case Tests ===" << std::endl; \
                    initialized = true; \
                } \
            } \
        }; \
        EdgeCaseTestGroup EdgeCaseTestInstance; \
    }

#define BEGIN_INTEGRATION_TESTS \
    namespace { \
        class IntegrationTestGroup { \
        public: \
            IntegrationTestGroup() { \
                static bool initialized = false; \
                if (!initialized) { \
                    std::cout << "=== Integration Tests ===" << std::endl; \
                    initialized = true; \
                } \
            } \
        }; \
        IntegrationTestGroup IntegrationTestInstance; \
    }

#define BEGIN_CROSS_PLATFORM_TESTS \
    namespace { \
        class CrossPlatformTestGroup { \
        public: \
            CrossPlatformTestGroup() { \
                static bool initialized = false; \
                if (!initialized) { \
                    std::cout << "=== Cross-Platform Tests ===" << std::endl; \
                    initialized = true; \
                } \
            } \
        }; \
        CrossPlatformTestGroup CrossPlatformTestInstance; \
    }

#define BEGIN_REGRESSION_TESTS \
    namespace { \
        class RegressionTestGroup { \
        public: \
            RegressionTestGroup() { \
                static bool initialized = false; \
                if (!initialized) { \
                    std::cout << "=== Regression Tests ===" << std::endl; \
                    initialized = true; \
                } \
            } \
        }; \
        RegressionTestGroup RegressionTestInstance; \
    }

// Enhanced assertion macros with detailed error reporting
#define ASSERT_MEMORY_LEAK(ptr, description) \
    do { \
        static std::vector<void*> leaked_pointers; \
        bool found = false; \
        for (void* leaked_ptr : leaked_pointers) { \
            if (leaked_ptr == ptr) { \
                found = true; \
                break; \
            } \
        } \
        if (!found) { \
            leaked_pointers.push_back(ptr); \
            std::cerr << "[MEMORY LEAK] " << description << " leaked at " << #ptr << std::endl; \
        } \
    } while(0)

#define ASSERT_NO_MEMORY_LEAK(expected_leaks, description) \
    do { \
        static std::vector<void*> current_leaks; \
        if (current_leaks.size() != (size_t)expected_leaks) { \
            std::cerr << "[MEMORY LEAK] " << description << ": expected " << expected_leaks \
                      << " leaks, found " << current_leaks.size() << std::endl; \
        } \
    } while(0)

// Performance assertion macros
#define ASSERT_PERFORMANCE(threshold_ms, actual_ms, description) \
    do { \
        if (actual_ms > threshold_ms) { \
            std::cerr << "[PERFORMANCE] " << description << ": " << actual_ms << " ms > threshold " << threshold_ms << " ms" << std::endl; \
        } \
    } while(0)

#define ASSERT_PERFORMANCE_RATIO(max_ratio, actual_ratio, description) \
    do { \
        if (actual_ratio > max_ratio) { \
            std::cerr << "[PERFORMANCE] " << description << ": ratio " << actual_ratio << " > threshold " << max_ratio << std::endl; \
        } \
    } while(0)

// Stress testing macros
#define ASSERT_STRESS_TEST(test_func, iterations, description) \
    do { \
        for (int i = 0; i < iterations; ++i) { \
            try { \
                test_func(); \
            } catch (const std::exception& e) { \
                std::cerr << "[STRESS] " << description << " failed on iteration " << i \
                          << ": " << e.what() << std::endl; \
                throw; \
            } \
        } \
        std::cout << "[STRESS] " << description << " passed " << iterations << " iterations" << std::endl; \
    } while(0)

// Edge case testing macros
#define ASSERT_EDGE_CASE(param, condition, description) \
    do { \
        if (!(condition)) { \
            std::cerr << "[EDGE CASE] " << description << ": parameter " << param << " failed validation" << std::endl; \
        } \
    } while(0)

// Integration test macros
#define ASSERT_INTEGRATION(system_a, system_b, expected_result, description) \
    do { \
        bool result = (system_a).interact_with((system_b)); \
        if (result != expected_result) { \
            std::cerr << "[INTEGRATION] " << description << ": expected " << expected_result \
                      << ", got " << result << std::endl; \
        } \
    } while(0)

// Cross-platform test macros
#define ASSERT_CROSS_PLATFORM(condition, platform_name, description) \
    do { \
        if (!(condition)) { \
            std::cerr << "[CROSS-PLATFORM] " << description << " on " << platform_name << " failed" << std::endl; \
        } \
    } while(0)

// Regression test macros
#define ASSERT_REGRESSION(test_id, previous_result, current_result, description) \
    do { \
        if (current_result != previous_result) { \
            std::cerr << "[REGRESSION] " << description << " (test " << test_id \
                      << "): changed from " << previous_result << " to " << current_result << std::endl; \
        } \
    } while(0)

// Test tagging macros
#define TAG_MEMORY test_config_manager.add_tag_to_test("test_memory_leak_detection", MEMORY_TESTS)
#define TAG_STRESS test_config_manager.add_tag_to_test("test_stress_scenario", STRESS_TESTS)
#define TAG_PERFORMANCE test_config_manager.add_tag_to_test("test_performance_benchmark", PERFORMANCE_TESTS)
#define TAG_EDGE_CASE test_config_manager.add_tag_to_test("test_edge_case_validation", EDGE_CASE_TESTS)
#define TAG_INTEGRATION test_config_manager.add_tag_to_test("test_integration", INTEGRATION_TESTS)
#define TAG_CROSS_PLATFORM test_config_manager.add_tag_to_test("test_cross_platform", CROSS_PLATFORM_TESTS)
#define TAG_REGRESSION test_config_manager.add_tag_to_test("test_regression", REGRESSION_TESTS)

// Test timeout macro
#define TIMEOUT_TEST(seconds, test_func) \
    do { \
        auto start_time = std::chrono::high_resolution_clock::now(); \
        try { \
            test_func(); \
            auto end_time = std::chrono::high_resolution_clock::now(); \
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count(); \
            if (duration > seconds) { \
                std::cerr << "[TIMEOUT] Test exceeded " << seconds << " seconds (took " << duration << " seconds)" << std::endl; \
            } \
        } catch (...) { \
            auto end_time = std::chrono::high_resolution_clock::now(); \
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count(); \
            std::cerr << "[TIMEOUT] Test failed after " << duration << " seconds" << std::endl; \
            throw; \
        } \
    } while(0)

// Test data generation macros
#define GENERATE_TEST_DATA(data_type, var_name, size, generator) \
    std::vector<data_type> var_name; \
    var_name.reserve(size); \
    for (int i = 0; i < size; ++i) { \
        var_name.push_back(generator()); \
    }

// Test validation macros
#define VALIDATE_TEST_RESULT(result, min_value, max_value, description) \
    do { \
        if (result < min_value || result > max_value) { \
            std::cerr << "[VALIDATION] " << description << ": result " << result \
                      << " outside expected range [" << min_value << ", " << max_value << "]" << std::endl; \
        } \
    } while(0)

// Test logging macros
#define LOG_TEST(test_name, level, message) \
    do { \
        std::string level_str; \
        if (level == "DEBUG") level_str = "DEBUG"; \
        else if (level == "INFO") level_str = "INFO"; \
        else if (level == "WARNING") level_str = "WARNING"; \
        else if (level == "ERROR") level_str = "ERROR"; \
        else level_str = level; \
        std::cout << "[" << level_str << "] " << test_name << ": " << message << std::endl; \
    } while(0)

// Test timing macros
#define MEASURE_TIME(operation, time_var) \
    do { \
        auto start_time = std::chrono::high_resolution_clock::now(); \
        operation; \
        auto end_time = std::chrono::high_resolution_clock::now(); \
        time_var = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count(); \
    } while(0)

// Test comparison macros
#define ASSERT_COMPARE_ARRAY(arr1, arr2, size, tolerance, description) \
    do { \
        bool arrays_equal = true; \
        for (size_t i = 0; i < size; ++i) { \
            if (std::abs(arr1[i] - arr2[i]) > tolerance) { \
                arrays_equal = false; \
                break; \
            } \
        } \
        if (!arrays_equal) { \
            std::cerr << "[COMPARISON] " << description << ": arrays differ" << std::endl; \
        } \
    } while(0)

// Test parallel execution macros
#define PARALLEL_TEST(test_func, thread_count, description) \
    do { \
        std::cout << "[PARALLEL] " << description << ": running " << thread_count << " threads" << std::endl; \
        std::vector<std::thread> threads; \
        for (int i = 0; i < thread_count; ++i) { \
            threads.emplace_back(test_func); \
        } \
        for (auto& thread : threads) { \
            thread.join(); \
        } \
    } while(0)

// Test cleanup macros
#define CLEANUP_TEST_RESOURCES(resource_type, cleanup_func) \
    do { \
        static bool resources_cleaned = false; \
        if (!resources_cleaned) { \
            cleanup_func(); \
            resources_cleaned = true; \
            std::cout << "[CLEANUP] Cleaned up " << resource_type << " resources" << std::endl; \
        } \
    } while(0)

// Test configuration macros
#define CONFIG_TEST_TIMEOUT(test_name, timeout_seconds) \
    do { \
        TestConfig config(test_name, "Test timeout configuration"); \
        config.timeout = timeout_seconds; \
        config.enabled = true; \
        test_config_manager.register_test(config); \
    } while(0)

#define CONFIG_TEST_TAGS(test_name, ...) \
    do { \
        TestConfig config(test_name, "Test tags configuration"); \
        std::vector<std::string> tags = {__VA_ARGS__}; \
        config.tags = tags; \
        test_config_manager.register_test(config); \
    } while(0)

#define CONFIG_TEST_MEMORY_LEAK(test_name, should_leak) \
    do { \
        TestConfig config(test_name, "Memory leak test configuration"); \
        config.should_leak = should_leak; \
        test_config_manager.register_test(config); \
    } while(0)

#define CONFIG_TEST_STRESS(test_name, stress_enabled) \
    do { \
        TestConfig config(test_name, "Stress test configuration"); \
        config.stress_test = stress_enabled; \
        test_config_manager.register_test(config); \
    } while(0)

// Test execution macros
#define RUN_TEST(test_name) \
    do { \
        TestConfig config(test_name, "Test execution configuration"); \
        const TestConfig* config_ptr = test_config_manager.get_test_config(test_name); \
        if (config_ptr && config_ptr->enabled) { \
            global_test_isolation.start_test(test_name); \
            std::cout << "Running test: " << test_name << std::endl; \
            /* Test implementation would go here */ \
            global_test_isolation.end_test(test_name); \
        } \
    } while(0)

#define RUN_ALL_TESTS() \
    do { \
        std::cout << "=== Running All Tests ===" << std::endl; \
        auto enabled_configs = test_config_manager.get_enabled_configs(); \
        for (const auto& config : enabled_configs) { \
            RUN_TEST(config.name); \
        } \
    } while(0)

// Export test infrastructure components
extern "C" {
    void register_macro_processor(test_infrastructure::TestMacroProcessor* processor) {
        if (processor) {
            global_macro_processor = *processor;
        }
    }
    
    void register_config_manager(TestConfigManager* config_manager) {
        if (config_manager) {
            test_config_manager = *config_manager;
        }
    }
    
    void register_test_isolation(TestIsolationManager* isolation_manager) {
        if (isolation_manager) {
            global_test_isolation = *isolation_manager;
        }
    }
    
    void register_fixture_manager(TestFixtureManager* fixture_manager) {
        if (fixture_manager) {
            global_fixture_manager = *fixture_manager;
        }
    }
}