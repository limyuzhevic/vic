#include "tests/Helpers/TestHelpers.hpp"

// Test helper implementations

namespace TestHelpers {
    
    // Implementation of RandomGenerator methods
    // (Declarations are in the header, no need for implementation here)
    
    // Implementation of Timer methods  
    // (Declarations are in the header, no need for implementation here)
    
    // Implementation of MathUtils functions
    // (Declarations are in the header, no need for implementation here)
    
    // Test data provider implementations
    // (Declarations are in the header, no need for implementation here)
}

// Test helper factory functions and utilities

// Performance benchmarking helper
class TestBenchmark {
public:
    struct BenchmarkResult {
        std::string name;
        double time_ms;
        double time_us;
        size_t iterations;
        double throughput;
    };
    
    static BenchmarkResult benchmark(const std::string& name, std::function<void()> func, size_t iterations = 1000) {
        Timer timer;
        timer.start();
        
        for (size_t i = 0; i < iterations; ++i) {
            func();
        }
        
        timer.stop();
        
        double time_ms = timer.elapsedMs();
        double time_us = timer.elapsedMs() * 1000.0;
        double throughput = (iterations > 0) ? (iterations / (time_ms / 1000.0)) : 0.0;
        
        return {name, time_ms, time_us, iterations, throughput};
    }
    
    static void printResults(const std::vector<BenchmarkResult>& results) {
        std::cout << "\n=== Benchmark Results ===" << std::endl;
        std::cout << std::left << std::setw(30) << "Test Name" 
                  << std::right << std::setw(15) << "Time (ms)"
                  << std::right << std::setw(15) << "Time (us)"
                  << std::right << std::setw(12) << "Iterations"
                  << std::right << std::setw(15) << "Throughput" << std::endl;
        std::cout << std::string(87, '-') << std::endl;
        
        for (const auto& result : results) {
            std::cout << std::left << std::setw(30) << result.name
                      << std::right << std::setw(15) << std::fixed << std::setprecision(3) << result.time_ms
                      << std::right << std::setw(15) << std::fixed << std::setprecision(3) << result.time_us
                      << std::right << std::setw(12) << result.iterations
                      << std::right << std::setw(15) << std::fixed << std::setprecision(3) << result.throughput << std::endl;
        }
    }
};

// Memory usage monitoring helper
class MemoryMonitor {
public:
    static size_t getPeakMemoryUsage() {
#ifdef __linux__
        return 0; // Placeholder - would need /proc/self/status parsing
#else
        return 0; // Placeholder - platform-specific implementation
#endif
    }
};

// Test assertion macros for convenience
#define ASSERT_EQUAL(expected, actual, message) TestFramework::getInstance().assertEqual(expected, actual, message)
#define ASSERT_NOT_EQUAL(expected, actual, message) TestFramework::getInstance().assertNotEqual(expected, actual, message)
#define ASSERT_TRUE(condition, message) TestFramework::getInstance().assertTrue(condition, message)
#define ASSERT_FALSE(condition, message) TestFramework::getInstance().assertFalse(condition, message)
#define ASSERT_NULLPTR(ptr, message) TestFramework::getInstance().assertNullptr(ptr, message)
#define ASSERT_NOT_NULLPTR(ptr, message) TestFramework::getInstance().assertNotNullptr(ptr, message)
#define ASSERT_LESS_THAN(a, b, message) TestFramework::getInstance().assertLessThan(a, b, message)
#define ASSERT_GREATER_THAN(a, b, message) TestFramework::getInstance().assertGreaterThan(a, b, message)
#define ASSERT_IN_RANGE(value, min, max, message) TestFramework::getInstance().assertInRange(value, min, max, message)

// Test case registration macro
#define REGISTER_TEST(test_class, test_name) \
    static test_class test_instance_##test_name; \
    TestFramework::getInstance().registerTest(&test_instance_##test_name)
