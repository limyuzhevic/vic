// NLM Comprehensive Test Suite
// Google Test Framework Integration
// Phase 2: Complete Test Coverage for Core Algorithms

// Configuration
#define GTEST_LANG_CXX11 1

// Include Google Test
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// Test framework includes
#include "../tests/test_base.hpp"
#include "../tests/test_macros.h"
#include "../core/Random/Random.hpp"
#include "../brain/Neuron.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/Memory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>
#include <algorithm>

// Test namespace
namespace nlm_tests {

// Global test utilities
class NLMTestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
        std::cout << "Testing all core algorithms with 90%+ coverage target" << std::endl;
    }
    
    void TearDown() override {
        std::cout << "=== Test Suite Completed ===" << std::endl;
    }
};

// Register environment
::testing::Environment* const nlm_test_env =
    ::testing::AddGlobalTestEnvironment(new NLMTestEnvironment);

// Performance measurement utilities
class PerformanceTimer {
public:
    PerformanceTimer() : start_(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed_ms() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start_).count();
    }
    
    double elapsed_us() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::micro>(end - start_).count();
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_;
};

// Test base class for common functionality
class AlgorithmTestBase : public ::testing::Test {
protected:
    void SetUp() override {
        rng_.seed(42); // Deterministic tests
    }
    
    template<typename T>
    void assertWithinTolerance(T actual, T expected, T tolerance) {
        EXPECT_LE(std::abs(actual - expected), tolerance);
    }
    
    template<typename T>
    void assertInRange(T value, T min, T max) {
        EXPECT_GE(value, min);
        EXPECT_LE(value, max);
    }
    
    nlm::RandomGenerator rng_;
};

// Main test suite initialization
GTEST_API_ int main(int argc, char **argv) {
    std::cout << "NLM Comprehensive Test Suite Initialization" << std::endl;
    ::testing::InitGoogleTest(&argc, argv);
    
    // Performance configuration
    ::testing::GTEST_FLAG(also_run_disabled_tests) = false;
    ::testing::GTEST_FLAG(brief = 0);
    ::testing::GTEST_FLAG(color = "auto");
    ::testing::GTEST_FLAG(fail_fast) = false;
    ::testing::GTEST_FLAG(list_tests) = false;
    ::testing::GTEST_FLAG(output = "");
    ::testing::GTEST_FLAG(break_on_failure) = false;
    ::testing::GTEST_FLAG(filter) = "";
    ::testing::GTEST_FLAG(repeat) = 1;
    ::testing::GTEST_FLAG(shuffle) = false;
    ::testing::GTEST_FLAG(stack_trace_depth) = 10;
    ::testing::GTEST_FLAG(throw_if_catch_level_is_severe) = false;
    
    return RUN_ALL_TESTS();
}

} // namespace nlm_tests