// Common test base and utilities for NLM test suite
// Provides test fixtures, macros, and helper functions

#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <string>
#include <random>
#include <chrono>

// Include NLM headers for testing
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "memory/Memory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "development/DevelopmentSystem.hpp"
#include "development/Pruning.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Maturation.hpp"

namespace nlm {
namespace test {

// Test base class for common setup/teardown
class TestBase : public ::testing::Test {
protected:
    void SetUp() override {
        // Common initialization for all tests
        setupTime = std::chrono::high_resolution_clock::now();
        randomSeed = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
        rng.seed(randomSeed);
        
        // Enable verbose test output
        verbose_ = true;
    }
    
    void TearDown() override {
        // Common cleanup for all tests
        cleanupTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(cleanupTime - setupTime);
        
        if (verbose_) {
            std::cout << "[TEST] Teardown completed in " << duration.count() << " ms" << std::endl;
        }
    }
    
    // Helper methods
    std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        config->set("simulation_timestep", 1.0, ConfigSource::Default);
        config->set("max_simulation_steps", 1000, ConfigSource::Default);
        return config;
    }
    
    nlm::Neuron createTestNeuron(NeuronId id, NeuronType type = NeuronType::Internal) {
        nlm::Neuron neuron(id);
        neuron.setType(type);
        neuron.setThreshold(-50.0f);
        neuron.setLeakConductance(10.0f);
        neuron.setRefractoryPeriod(5);
        return neuron;
    }
    
    nlm::Synapse createTestSynapse(SynapseId id, NeuronId source, NeuronId dest) {
        nlm::Synapse synapse(id, source, dest);
        synapse.setType(SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        synapse.setDelay(1);
        return synapse;
    }
    
    // Mathematical helper methods
    float normalize(float value, float min, float max) {
        return (value - min) / (max - min);
    }
    
    float clamp(float value, float min, float max) {
        return std::max(min, std::min(max, value));
    }
    
    float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }
    
    // Floating-point comparison for testing
    bool approximatelyEqual(float a, float b, float epsilon = 1e-6f) {
        return std::abs(a - b) < epsilon;
    }
    
    // Test assertions with better messages
    void EXPECT_MEMB_POTENTIAL_APPROX(nlm::Neuron& neuron, nlm::MembranePotential expected) {
        EXPECT_TRUE(approximatelyEqual(neuron.getMembranePotential(), expected))
            << "Neuron membrane potential: expected " << expected 
            << ", got " << neuron.getMembranePotential();
    }
    
    void EXPECT_WEIGHT_APPROX(nlm::Synapse& synapse, SynapticWeight expected) {
        EXPECT_TRUE(approximatelyEqual(synapse.getWeight(), expected))
            << "Synapse weight: expected " << expected 
            << ", got " << synapse.getWeight();
    }
    
    // Performance timing
    void startTimer() {
        timerStart = std::chrono::high_resolution_clock::now();
    }
    
    double getElapsedTimeMs() {
        auto timerEnd = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timerEnd - timerStart);
        return static_cast<double>(duration.count());
    }
    
    // Test validation utilities
    class TestValidator {
    public:
        static bool validateNeuron(const nlm::Neuron& neuron) {
            return neuron.getId().isValid() && neuron.getMembranePotential() >= -100.0f && 
                   neuron.getMembranePotential() <= 100.0f &&
                   neuron.getThreshold() >= -100.0f && neuron.getThreshold() <= 100.0f;
        }
        
        static bool validateSynapse(const nlm::Synapse& synapse) {
            return synapse.getId().isValid() && 
                   synapse.getSourceNeuron().isValid() && 
                   synapse.getDestinationNeuron().isValid() &&
                   synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f &&
                   synapse.getDelay() >= 1 && synapse.getDelay() <= 100;
        }
    };
    
    // Benchmark helpers
    struct BenchmarkResult {
        double timeMs;
        size_t iterations;
        double opsPerMs;
        
        BenchmarkResult(double time, size_t iters) 
            : timeMs(time), iterations(iters), opsPerMs(iters / time) {}
    };
    
    BenchmarkResult runBenchmark(std::function<void()> func, size_t iterations = 1000) {
        startTimer();
        for (size_t i = 0; i < iterations; ++i) {
            func();
        }
        double elapsed = getElapsedTimeMs();
        return BenchmarkResult(elapsed, iterations);
    }

protected:
    std::chrono::high_resolution_clock::time_point setupTime;
    std::chrono::high_resolution_clock::time_point cleanupTime;
    std::chrono::high_resolution_clock::time_point timerStart;
    uint64_t randomSeed;
    nlm::RandomGenerator rng;
    bool verbose_;
};

// Test macros for better error messages
#define EXPECT_MEMB_POTENTIAL_APPROX(neuron, expected) \
    do { \
        float neuronMem = neuron.getMembranePotential(); \
        float expectedMem = expected; \
        ASSERT_NEAR(neuronMem, expectedMem, 1e-6f) << \
            "Neuron membrane potential: expected " << expectedMem << \
            ", got " << neuronMem; \
    } while (0)

#define EXPECT_WEIGHT_APPROX(synapse, expected) \
    do { \
        float synapseWeight = synapse.getWeight(); \
        float expectedWeight = expected; \
        ASSERT_NEAR(synapseWeight, expectedWeight, 1e-6f) << \
            "Synapse weight: expected " << expectedWeight << \
            ", got " << synapseWeight; \
    } while (0)

// Parameterized test base for testing multiple scenarios
class ParameterizedTestBase : public TestBase {
protected:
    struct TestParameters {
        int neuronCount;
        float connectionProbability;
        float learningRate;
        std::string description;
        
        TestParameters(int neurons, float prob, float rate, const std::string& desc)
            : neuronCount(neurons), connectionProbability(prob), learningRate(rate), description(desc) {}
    };
    
    std::vector<TestParameters> getTestScenarios() {
        return {
            TestParameters(10, 0.1f, 0.01f, "Small network"),
            TestParameters(100, 0.2f, 0.05f, "Medium network"),
            TestParameters(1000, 0.15f, 0.1f, "Large network")
        };
    }
};

} // namespace test
} // namespace nlm
