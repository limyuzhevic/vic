// Modern Test Framework Base Class for NLM
// Enhanced testing infrastructure with enterprise-grade features

#ifndef NLM_TEST_FRAMEWORK_H
#define NLM_TEST_FRAMEWORK_H

#include <gtest/gtest.h>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <random>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <iomanip>

// NLM-specific testing utilities
namespace nlm_test {

// Seed management for reproducible tests
class TestSeedManager {
private:
    static std::mt19937 global_rng;
    static std::atomic<uint64_t> test_counter;
    
public:
    static void initSeed(uint64_t seed = 0) {
        if (seed == 0) {
            std::random_device rd;
            global_rng.seed(rd());
        } else {
            global_rng.seed(seed);
        }
    }
    
    static std::mt19937& getRNG() { return global_rng; }
    static uint64_t getNextTestId() { return test_counter++; }
};

// Memory tracking for leak detection
class MemoryTracker {
private:
    struct AllocationInfo {
        void* ptr;
        size_t size;
        const char* file;
        int line;
        std::string stack_trace;
    };
    
    static std::unordered_set<void*> allocated_ptrs;
    static std::mutex mutex;
    
public:
    static void* trackAllocation(void* ptr, size_t size, const char* file, int line);
    static void trackDeallocation(void* ptr);
    static bool hasLeaks();
    static void reportLeaks();
    static void clear();
};

// Performance measurement utilities
class PerformanceTimer {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::string operation_name;
    
public:
    PerformanceTimer(const std::string& name = "Operation") : operation_name(name) {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    ~PerformanceTimer() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        std::cout << "[PERF] " << operation_name << ": " << duration.count() << " us" << std::endl;
    }
    
    double elapsed_us() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        return static_cast<double>(duration.count());
    }
};

// Thread safety testing utilities
class ThreadSafetyTester {
private:
    static std::atomic<int> concurrent_access_count;
    static std::mutex test_mutex;
    
public:
    static void incrementAccess() { concurrent_access_count++; }
    static int getAccessCount() { return concurrent_access_count.load(); }
    static void resetAccessCount() { concurrent_access_count.store(0); }
    
    template<typename Func>
    static void concurrent_test(Func func, int thread_count = 8, int iterations = 100) {
        std::vector<std::thread> threads;
        std::atomic<bool> test_passed{true};
        
        for (int i = 0; i < thread_count; ++i) {
            threads.emplace_back([&, i]() {
                for (int j = 0; j < iterations; ++j) {
                    try {
                        func(i, j);
                    } catch (...) {
                        test_passed.store(false);
                    }
                }
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        ASSERT_TRUE(test_passed.load()) << "Thread safety test failed";
    }
};

// Configuration validation utilities
class ConfigurationValidator {
public:
    template<typename T>
    static void validateRange(const T& value, const T& min, const T& max, 
                             const std::string& param_name = "Parameter") {
        EXPECT_GE(value, min) << param_name << " (" << value << ") is less than minimum (" << min << ")";
        EXPECT_LE(value, max) << param_name << " (" << value << ") is greater than maximum (" << max << ")";
    }
    
    static void validateNeuronParameters() {
        // Validate neuron-specific parameters
        validateRange(0.0f, -100.0f, 100.0f, "Membrane potential");
        validateRange(-70.0f, -100.0f, -40.0f, "Default membrane potential");
        validateRange(0.0f, 0.0f, 1.0f, "Synaptic weight");
    }
};

} // namespace nlm_test

#endif // NLM_TEST_FRAMEWORK_H
