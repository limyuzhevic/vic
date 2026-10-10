// test_common.hpp - Common test utilities and definitions
// 
// This header file provides common definitions, utilities, and helper functions
// for testing the NLM (Neural Learning Machine) project.

#pragma once

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <memory>

// NLM namespace
namespace nlm {
    // Forward declarations of commonly used types
    class Config;
    class Brain;
    class Neuron;
    class Synapse;
    class SimulationClock;
    class RandomGenerator;
    class Neuromodulator;
    class Memory;
    class DevelopmentSystem;
    class PredictionSystem;
    class ConceptFormation;
    class NeuralPlanner;
    class SensoryInput;
    class MotorSystem;
    class Environment;
    class SimpleWorld;
    class VisualizationInterface;
    class MemoryPool;
    class EventQueue;
    class ParallelProcessing;
    class Experiment;
    class ExperimentRunner;
    class Phase3Experiment;
    class Phase4Experiment;
    class Phase5Experiment;
    class Phase6IntegratedExperiment;
    class Metrics;
    class ScalingBenchmark;
    class NLMCLI;
    class AgentBrain;
    class SensoryPercept;
    class AgentBody;
    class PlasticityRule;
    class HebbianRule;
    class AntiHebbianRule;
    class BCMRule;
    class StructuralPlasticity;
}

// Test framework macros
#define ASSERT_EQ(a, b) assert((a) == (b))
#define ASSERT_NE(a, b) assert((a) != (b))
#define ASSERT_GT(a, b) assert((a) > (b))
#define ASSERT_LT(a, b) assert((a) < (b))
#define ASSERT_GE(a, b) assert((a) >= (b))
#define ASSERT_LE(a, b) assert((a) <= (b))
#define ASSERT_TRUE(a) assert(a == true)
#define ASSERT_FALSE(a) assert(a == false)
#define ASSERT_FLOAT_EQ(a, b, epsilon) assert(std::abs((a) - (b)) < epsilon)

// Test utilities
namespace test_utils {
    
    // Random number generator for tests
    class TestRandom {
    public:
        TestRandom() : rng_(std::random_device()()) {}
        
        float uniformFloat(float min = 0.0f, float max = 1.0f) {
            std::uniform_real_distribution<float> dist(min, max);
            return dist(rng_);
        }
        
        int uniformInt(int min, int max) {
            std::uniform_int_distribution<int> dist(min, max);
            return dist(rng_);
        }
        
        float normalFloat(float mean = 0.0f, float stddev = 1.0f) {
            std::normal_distribution<float> dist(mean, stddev);
            return dist(rng_);
        }
        
        bool bernoulli(float p) {
            return uniformFloat() < p;
        }
        
        std::vector<float> randomVector(size_t size, float min = 0.0f, float max = 1.0f) {
            std::vector<float> vec(size);
            for (size_t i = 0; i < size; ++i) {
                vec[i] = uniformFloat(min, max);
            }
            return vec;
        }
        
        std::string randomString(size_t length) {
            const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            std::string result(length);
            for (size_t i = 0; i < length; ++i) {
                result[i] = charset[uniformInt(0, charset.size() - 1)];
            }
            return result;
        }
        
    private:
        std::mt19937 rng_;
    };
    
    // Test configuration helpers
    struct TestConfig {
        static std::shared_ptr<nlm::Config> createDefaultConfig() {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
            config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
            config->set("simulation_step", 0.001f, nlm::ConfigSource::Default);
            return config;
        }
        
        static std::shared_ptr<nlm::Config> createSmallConfig() {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
            config->set("connection_probability", 0.2f, nlm::ConfigSource::Default);
            config->set("simulation_step", 0.001f, nlm::ConfigSource::Default);
            return config;
        }
    };
    
    // Performance measurement
    class Timer {
    public:
        Timer() : start_time_(std::chrono::high_resolution_clock::now()) {}
        
        float elapsed() const {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
            return duration.count() / 1000000.0f; // Convert to seconds
        }
        
    private:
        std::chrono::high_resolution_clock::time_point start_time_;
    };
    
    // Test assertion helpers
    inline void assertWithinBounds(float value, float min, float max, const std::string& message = "") {
        assert(value >= min && value <= max);
        if (!message.empty()) {
            std::cout << "    Assertion passed: " << message << std::endl;
        }
    }
    
    inline void assertApproxEqual(float a, float b, float epsilon, const std::string& message = "") {
        assert(std::abs(a - b) < epsilon);
        if (!message.empty()) {
            std::cout << "    Assertion passed: " << message << std::endl;
        }
    }
}

// Test result tracking
struct TestResult {
    std::string name;
    bool passed;
    float executionTime;
    std::string errorMessage;
};

// Global test results collection
inline std::vector<TestResult> g_testResults;

// Macro for tracking test results
#define TRACK_TEST_RESULT(name, passed, time, error) \
    do { \
        TestResult result; \
        result.name = name; \
        result.passed = passed; \
        result.executionTime = time; \
        result.errorMessage = error; \
        g_testResults.push_back(result); \
    } while(0)

// Test framework initialization
inline void initializeTestFramework() {
    std::cout << "=== NLM Test Framework ===" << std::endl;
    std::cout << "Initializing test environment..." << std::endl;
    std::cout << std::endl;
}

// Test framework cleanup
inline void cleanupTestFramework() {
    std::cout << std::endl;
    std::cout << "=== Test Framework Cleanup ===" << std::endl;
    
    // Print test summary
    int totalTests = g_testResults.size();
    int passedTests = 0;
    int failedTests = 0;
    
    for (const auto& result : g_testResults) {
        if (result.passed) {
            passedTests++;
        } else {
            failedTests++;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Test Summary:" << std::endl;
    std::cout << "  Total Tests: " << totalTests << std::endl;
    std::cout << "  Passed: " << passedTests << std::endl;
    std::cout << "  Failed: " << failedTests << std::endl;
    
    if (failedTests > 0) {
        std::cout << "  Failed Tests:" << std::endl;
        for (const auto& result : g_testResults) {
            if (!result.passed) {
                std::cout << "    - " << result.name << ": " << result.errorMessage << std::endl;
            }
        }
    }
}

// Performance reporting
inline void reportPerformance(const std::string& testName, float executionTime) {
    std::cout << "    Performance: " << testName << " completed in " 
              << executionTime << " seconds" << std::endl;
}

// Memory usage reporting
inline void reportMemoryUsage(const std::string& testName, size_t memoryUsage) {
    std::cout << "    Memory: " << testName << " used " 
              << memoryUsage << " bytes" << std::endl;
}