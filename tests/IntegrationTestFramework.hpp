// Integration Test Framework for NLM Neural Network Simulation
// 
// This file provides base classes and utilities for system-wide integration testing.
// Tests the interactions between different subsystems rather than individual components.

#ifndef NLM_INTEGRATION_TEST_FRAMEWORK_HPP
#define NLM_INTEGRATION_TEST_FRAMEWORK_HPP

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/MotorSystem.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "memory/Memory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "development/DevelopmentSystem.hpp"
#include <memory>
#include <vector>
#include <chrono>
#include <cassert>
#include <iostream>
#include <map>
#include <string>

namespace nlm {
namespace integration_tests {

// Test metrics collection for integration tests
struct IntegrationTestMetrics {
    double startTime{0.0};
    double endTime{0.0};
    double duration{0.0};
    size_t neuronsCreated{0};
    size_t synapsesCreated{0};
    size_t neuronsFire{0};
    double averageFiringRate{0.0};
    double peakActivity{0.0};
    std::map<std::string, double> customMetrics;
    bool success{false};
    std::string failureReason;
    
    void reset() {
        neuronsCreated = 0;
        synapsesCreated = 0;
        neuronsFire = 0;
        averageFiringRate = 0.0;
        peakActivity = 0.0;
        customMetrics.clear();
        success = false;
        failureReason.clear();
    }
};

// Base class for all integration tests
class IntegrationTestBase {
protected:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    IntegrationTestMetrics metrics_;
    
    // Helper methods
    void recordStartTime() {
        metrics_.startTime = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    void recordEndTime() {
        metrics_.endTime = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        metrics_.duration = metrics_.endTime - metrics_.startTime;
    }
    
    void recordMetric(const std::string& name, double value) {
        metrics_.customMetrics[name] = value;
    }
    
    template<typename T>
    void assertNotNull(const std::string& name, const std::shared_ptr<T>& ptr) {
        if (!ptr) {
            metrics_.success = false;
            metrics_.failureReason = name + " is null";
            throw std::runtime_error(metrics_.failureReason);
        }
    }
    
    template<typename T>
    void assertValid(const std::string& name, T value, T min, T max) {
        if (value < min || value > max) {
            metrics_.success = false;
            metrics_.failureReason = name + " out of range: " + std::to_string(value);
            throw std::runtime_error(metrics_.failureReason);
        }
    }
    
    void assertCondition(bool condition, const std::string& message) {
        if (!condition) {
            metrics_.success = false;
            metrics_.failureReason = message;
            throw std::runtime_error(message);
        }
    }
    
public:
    IntegrationTestBase() {
        config_ = std::make_shared<Config>();
        setupConfiguration();
    }
    
    virtual ~IntegrationTestBase() {}
    
    virtual void setupConfiguration() = 0;
    virtual void setupSystem() = 0;
    virtual void teardownSystem() {}
    virtual void runTest() = 0;
    
    void run() {
        try {
            recordStartTime();
            metrics_.reset();
            metrics_.success = true;
            
            setupConfiguration();
            setupSystem();
            runTest();
            
            recordEndTime();
            
            // Collect final metrics
            if (brain_) {
                metrics_.neuronsCreated = brain_->getTotalNeuronCount();
                metrics_.synapsesCreated = brain_->getTotalSynapseCount();
                metrics_.neuronsFire = brain_->getFiringNeuronCount();
                metrics_.averageFiringRate = brain_->getAverageFiringRate();
            }
            
        } catch (const std::exception& e) {
            if (!metrics_.failureReason.empty()) {
                std::cerr << "Integration test failed: " << metrics_.failureReason << std::endl;
            } else {
                std::cerr << "Integration test failed: " << e.what() << std::endl;
            }
        }
    }
    
    const IntegrationTestMetrics& getMetrics() const { return metrics_; }
};

// Performance benchmarking utilities
class PerformanceBenchmarker {
public:
    struct BenchmarkResult {
        std::string testName;
        double duration{0.0};
        size_t operationsPerSecond{0};
        double memoryUsage{0.0};
        bool passed{false};
    };
    
    static BenchmarkResult runBenchmark(const std::string& testName, 
                                        std::function<void()> testFunction,
                                        size_t iterations = 10) {
        BenchmarkResult result;
        result.testName = testName;
        
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < iterations; ++i) {
            testFunction();
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        double totalDuration = std::chrono::duration<double>(end - start).count();
        result.duration = totalDuration;
        result.operationsPerSecond = (iterations * 1000) / totalDuration;  // per ms
        
        return result;
    }
};

// System state snapshot for comparison
class SystemStateSnapshot {
public:
    size_t totalNeuronCount{0};
    size_t totalSynapseCount{0};
    size_t firingNeuronCount{0};
    double averageFiringRate{0.0};
    double totalActivity{0.0};
    
    void capture(const Brain& brain) {
        totalNeuronCount = brain.getTotalNeuronCount();
        totalSynapseCount = brain.getTotalSynapseCount();
        firingNeuronCount = brain.getFiringNeuronCount();
        averageFiringRate = brain.getAverageFiringRate();
        totalActivity = static_cast<double>(firingNeuronCount) * averageFiringRate;
    }
    
    bool equals(const SystemStateSnapshot& other) const {
        return totalNeuronCount == other.totalNeuronCount &&
               totalSynapseCount == other.totalSynapseCount &&
               firingNeuronCount == other.firingNeuronCount &&
               std::abs(averageFiringRate - other.averageFiringRate) < 0.001;
    }
};

// Test environment manager
class TestEnvironmentManager {
public:
    static void setupTestWorld() {
        // Setup test configuration
        std::cout << "Setting up integration test environment..." << std::endl;
    }
    
    static void cleanupTestWorld() {
        std::cout << "Cleaning up integration test environment..." << std::endl;
    }
    
    static bool isSystemReady() {
        // Check if all core subsystems are available
        return true;  // Placeholder
    }
};

} // namespace integration_tests
} // namespace nlm

#endif // NLM_INTEGRATION_TEST_FRAMEWORK_HPP
