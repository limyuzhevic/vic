// NLM Enhanced Test Infrastructure
// Phase 7: Comprehensive Test Suite
// Base test framework with common utilities and patterns

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>

// Forward declarations for core types
namespace nlm {
    class Neuron;
    class Synapse;
    class Brain;
    class MemorySystem;
    class Config;
}

// Test configuration structure
struct TestConfig {
    std::string name;
    bool enabled = true;
    std::string description;
    double timeout = 30.0; // seconds
    bool should_leak = false; // for memory leak detection
    bool stress_test = false; // for stress testing
    int thread_count = 1;
    std::vector<std::string> tags;
    
    TestConfig(const std::string& test_name, const std::string& test_desc = "")
        : name(test_name), description(test_desc) {}
};

// Test result structure
struct TestResult {
    std::string test_name;
    bool passed = false;
    std::string message;
    double execution_time = 0.0;
    size_t memory_delta = 0;
    std::string error_trace;
    std::vector<std::string> warnings;
    
    TestResult(const std::string& name) : test_name(name) {}
};

// Base test fixture class
class TestFixture {
protected:
    struct AllocationInfo {
        void* pointer;
        size_t size;
        std::string allocation_site;
        std::string test_name;
    };
    
    std::vector<AllocationInfo> tracked_allocations;
    bool isolation_enabled = true;
    
public:
    virtual ~TestFixture() {}
    
    virtual void setup() {}
    virtual void teardown() {}
    virtual void setup_test() {}
    virtual void teardown_test() {}
    
    // Memory tracking utilities
    void track_allocation(void* ptr, size_t size, const std::string& site = "", const std::string& test = "") {
        if (isolation_enabled) {
            tracked_allocations.push_back({ptr, size, site, test});
        }
    }
    
    void track_deallocation(void* ptr) {
        if (isolation_enabled) {
            auto it = std::find_if(tracked_allocations.begin(), tracked_allocations.end(),
                [ptr](const AllocationInfo& info) { return info.pointer == ptr; });
            if (it != tracked_allocations.end()) {
                tracked_allocations.erase(it);
            }
        }
    }
    
    size_t get_leaked_allocations() const {
        return tracked_allocations.size();
    }
    
    void clear_tracked_allocations() {
        tracked_allocations.clear();
    }
    
    // Test isolation
    void enable_isolation(bool enable) {
        isolation_enabled = enable;
    }
};

// Enhanced test framework with timing and reporting
class TestFramework {
private:
    std::vector<TestResult> results;
    std::vector<std::unique_ptr<TestFixture>> fixtures;
    std::string current_test_name;
    std::chrono::high_resolution_clock::time_point test_start;
    
public:
    void start_test(const std::string& test_name) {
        current_test_name = test_name;
        test_start = std::chrono::high_resolution_clock::now();
    }
    
    void end_test(TestResult& result) {
        auto test_end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(test_end - test_start).count();
        result.execution_time = duration;
        results.push_back(result);
        current_test_name = "";
    }
    
    void add_fixture(std::unique_ptr<TestFixture> fixture) {
        fixtures.push_back(std::move(fixture));
    }
    
    void setup_all() {
        for (auto& fixture : fixtures) {
            fixture->setup();
        }
    }
    
    void teardown_all() {
        for (auto& fixture : fixtures) {
            fixture->teardown();
        }
        fixtures.clear();
    }
    
    void setup_test() {
        for (auto& fixture : fixtures) {
            fixture->setup_test();
        }
    }
    
    void teardown_test() {
        for (auto& fixture : fixtures) {
            fixture->teardown_test();
        }
    }
    
    const std::vector<TestResult>& get_results() const {
        return results;
    }
    
    void clear_results() {
        results.clear();
    }
    
    double get_total_execution_time() const {
        double total = 0.0;
        for (const auto& result : results) {
            total += result.execution_time;
        }
        return total;
    }
    
    size_t get_total_tests() const {
        return results.size();
    }
    
    size_t get_passed_tests() const {
        size_t passed = 0;
        for (const auto& result : results) {
            if (result.passed) passed++;
        }
        return passed;
    }
    
    size_t get_failed_tests() const {
        return get_total_tests() - get_passed_tests();
    }
    
    double get_failure_rate() const {
        if (get_total_tests() == 0) return 0.0;
        return static_cast<double>(get_failed_tests()) / get_total_tests() * 100.0;
    }
    
    // Reporting utilities
    std::string generate_report() const {
        std::stringstream ss;
        ss << "=== NLM Enhanced Test Suite Report ===" << std::endl;
        ss << std::endl;
        ss << "Total Tests: " << get_total_tests() << std::endl;
        ss << "Passed: " << get_passed_tests() << std::endl;
        ss << "Failed: " << get_failed_tests() << std::endl;
        ss << "Failure Rate: " << std::fixed << std::setprecision(2) << get_failure_rate() << "%" << std::endl;
        ss << "Total Execution Time: " << std::fixed << std::setprecision(3) << get_total_execution_time() << " seconds" << std::endl;
        ss << std::endl;
        
        ss << "Detailed Results:" << std::endl;
        for (const auto& result : results) {
            ss << "  " << result.test_name << ": " 
               << (result.passed ? "PASSED" : "FAILED")
               << " (" << std::fixed << std::setprecision(3) << result.execution_time << "s)";
            if (!result.message.empty()) {
                ss << " - " << result.message;
            }
            ss << std::endl;
        }
        
        return ss.str();
    }
    
    void print_report() const {
        std::cout << generate_report();
    }
};

// Global test framework instance
extern TestFramework global_test_framework;

// Macro definitions for common testing patterns
#define TEST_CASE(name) \
    void test_##name(); \
    namespace { \
        class Test_##name##_Registration { \
        public: \
            Test_##name##_Registration() { \
                global_test_framework.start_test(#name); \
                test_##name(); \
                global_test_framework.end_test(global_test_framework.get_results().back()); \
            } \
        }; \
        Test_##name##_Registration Test_##name##_instance; \
    } \
    void test_##name()

#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            TestResult& result = global_test_framework.get_results().back(); \
            result.passed = false; \
            std::stringstream ss; \
            ss << "Assertion failed: " << #expected << " (" << (expected) << ") != " << #actual << " (" << (actual) << ")"; \
            result.message = ss.str(); \
            throw std::runtime_error(result.message); \
        } \
    } while(0)

#define ASSERT_NE(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            TestResult& result = global_test_framework.get_results().back(); \
            result.passed = false; \
            std::stringstream ss; \
            ss << "Assertion failed: " << #expected << " (" << (expected) << ") should not equal " << #actual << " (" << (actual) << ")"; \
            result.message = ss.str(); \
            throw std::runtime_error(result.message); \
        } \
    } while(0)

#define ASSERT_GT(expected, actual) \
    do { \
        if ((expected) >= (actual)) { \
            TestResult& result = global_test_framework.get_results().back(); \
            result.passed = false; \
            std::stringstream ss; \
            ss << "Assertion failed: " << #expected << " (" << (expected) << ") should be greater than " << #actual << " (" << (actual) << ")"; \
            result.message = ss.str(); \
            throw std::runtime_error(result.message); \
        } \
    } while(0)

#define ASSERT_LT(expected, actual) \
    do { \
        if ((expected) <= (actual)) { \
            TestResult& result = global_test_framework.get_results().back(); \
            result.passed = false; \
            std::stringstream ss; \
            ss << "Assertion failed: " << #expected << " (" << (expected) << ") should be less than " << #actual << " (" << (actual) << ")"; \
            result.message = ss.str(); \
            throw std::runtime_error(result.message); \
        } \
    } while(0)

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            TestResult& result = global_test_framework.get_results().back(); \
            result.passed = false; \
            std::stringstream ss; \
            ss << "Assertion failed: " << #condition << " is not true"; \
            result.message = ss.str(); \
            throw std::runtime_error(result.message); \
        } \
    } while(0)

#define ASSERT_FALSE(condition) \
    do { \
        if (condition) { \
            TestResult& result = global_test_framework.get_results().back(); \
            result.passed = false; \
            std::stringstream ss; \
            ss << "Assertion failed: " << #condition << " is not false"; \
            result.message = ss.str(); \
            throw std::runtime_error(result.message); \
        } \
    } while(0)

#define ASSERT_DOUBLE_EQ(expected, actual, epsilon) \
    do { \
        if (std::abs((expected) - (actual)) > (epsilon)) { \
            TestResult& result = global_test_framework.get_results().back(); \
            result.passed = false; \
            std::stringstream ss; \
            ss << "Assertion failed: " << #expected << " (" << (expected) << ") != " << #actual << " (" << (actual) << ") within epsilon " << epsilon; \
            result.message = ss.str(); \
            throw std::runtime_error(result.message); \
        } \
    } while(0)

#define TEST_FIXTURE(fixture_class) \
    class fixture_class##_fixture : public TestFixture { \
    public: \
        fixture_class##_fixture() {} \
    }; \
    global_test_framework.add_fixture(std::make_unique<fixture_class##_fixture>())

// Performance timing utilities
class PerformanceTimer {
private:
    std::chrono::high_resolution_clock::time_point start;
    std::string operation_name;
    
public:
    PerformanceTimer(const std::string& name) : operation_name(name) {
        start = std::chrono::high_resolution_clock::now();
    }
    
    ~PerformanceTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start).count();
        std::cout << "[PERF] " << operation_name << ": " << duration << " ms" << std::endl;
    }
    
    double elapsed() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count();
    }
};

// Memory leak detection utilities
class MemoryLeakDetector {
private:
    std::vector<void*> allocated_pointers;
    
public:
    void* allocate(size_t size) {
        void* ptr = std::malloc(size);
        if (ptr) {
            allocated_pointers.push_back(ptr);
        }
        return ptr;
    }
    
    void deallocate(void* ptr) {
        auto it = std::find(allocated_pointers.begin(), allocated_pointers.end(), ptr);
        if (it != allocated_pointers.end()) {
            allocated_pointers.erase(it);
        }
        std::free(ptr);
    }
    
    size_t get_leaked_count() const {
        return allocated_pointers.size();
    }
    
    void clear() {
        for (void* ptr : allocated_pointers) {
            std::free(ptr);
        }
        allocated_pointers.clear();
    }
    
    ~MemoryLeakDetector() {
        if (!allocated_pointers.empty()) {
            std::cerr << "[MEMORY LEAK] Detected " << allocated_pointers.size() << " leaked allocations" << std::endl;
        }
    }
};