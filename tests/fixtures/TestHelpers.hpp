// Test configuration for GoogleTest with NLM framework

// This file provides common test fixtures and helper classes for NLM testing
// to ensure consistent test behavior across all test suites

#pragma once

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include "core/Types/Types.hpp"
#include <memory>
#include <gtest/gtest.h>

namespace test {

// Common test constants
constexpr double EPSILON = 1e-6;
constexpr nlm::MembranePotential DEFAULT_POTENTIAL = -70.0f;
constexpr nlm::MembranePotential THRESHOLD = -55.0f;
constexpr nlm::SimulationStep DEFAULT_STEPS = 100;
constexpr nlm::FiringRate DEFAULT_RATE = 0.0f;

// Basic test fixture for NLM components
class NLMTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup for all NLM tests
        testing::internal::GetFlag(&testing::GTEST_FLAG(also_run_disabled_tests)) = true;
    }
    
    void TearDown() override {
        // Common cleanup for all NLM tests
    }
    
    // Helper methods for common operations
    template<typename T>
    bool approximateEqual(T a, T b, double epsilon = EPSILON) {
        return std::abs(a - b) < epsilon;
    }
    
    // Create a default configured brain for testing
    std::shared_ptr<nlm::Brain> createTestBrain(size_t neuronCount = 50, size_t regionCount = 2) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regionCount), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        return brain;
    }
    
    // Create a test neuron with default properties
    std::unique_ptr<nlm::Neuron> createTestNeuron(nlm::NeuronId id = nlm::NeuronId(1)) {
        auto neuron = std::make_unique<nlm::Neuron>(id);
        neuron->setType(nlm::NeuronType::Excitatory);
        neuron->setThreshold(THRESHOLD);
        return neuron;
    }
};

// Performance test fixture with timing utilities
class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void TearDown() override {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << "  Performance: " << duration.count() << " ms" << std::endl;
    }
    
    double getElapsedTime() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        return duration.count() / 1000.0; // Convert to seconds
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
};

// Error handling test fixture
class ErrorHandlingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize error handling for tests
        nlm::initializeErrorHandling(nlm::ErrorReportLevel::Error, nlm::ExceptionStrategy::Throw);
        errorCountBefore = nlm::getErrorHandler().getErrorCount();
    }
    
    void TearDown() override {
        // Verify error count
        int errorCountAfter = nlm::getErrorHandler().getErrorCount();
        int newErrors = errorCountAfter - errorCountBefore;
        if (newErrors > 0) {
            std::cout << "  Errors during test: " << newErrors << std::endl;
        }
    }
    
    bool errorsOccurred() {
        int errorCountAfter = nlm::getErrorHandler().getErrorCount();
        return errorCountAfter > errorCountBefore;
    }
    
private:
    int errorCountBefore;
};

// Integration test fixture for system-level testing
class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup integration test environment
        integrationTestMode = true;
    }
    
    void TearDown() override {
        // Cleanup integration test environment
        integrationTestMode = false;
    }
    
protected:
    bool integrationTestMode = false;
};

// Test macros for common assertions
#define ASSERT_DOUBLE_EQ_APPROX(val1, val2) ASSERT_NEAR(val1, val2, EPSILON)
#define ASSERT_FLOAT_EQ_APPROX(val1, val2) ASSERT_NEAR(val1, val2, EPSILON)
#define ASSERT_CONFIG_HAS_VALUE(config, key, expected) \
    ASSERT_TRUE(config.has(key)); \
    auto val = config.get<std::string>(key); \
    if (val.has_value()) ASSERT_EQ(*val, expected)

// Test naming helper for parameterized tests
#define TEST_NAME_PREFIX "NLM::"

} // namespace test