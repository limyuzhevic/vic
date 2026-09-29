// Comprehensive NLM Testing Framework
// This file provides a complete testing framework for NLM with multiple test suites,
// performance benchmarks, and integration testing capabilities.

#include <iostream>
#include <vector>
#include <chrono>
#include <memory>
#include <cassert>
#include <algorithm>
#include <iomanip>

// Core includes
#include "core/Config/Config.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Neuron.hpp"
#include "brain/Brain.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "development/DevelopmentSystem.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "agent/AgentBody.hpp"

namespace test {

// Performance benchmarking utilities
struct BenchmarkResult {
    std::string test_name;
    double duration_ms;
    size_t iterations;
    double operations_per_second;
    std::string unit;
};

class Benchmark {
public:
    static void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    static double stop(const std::string& unit = "seconds") {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        double seconds = duration.count() / 1e9;
        
        if (unit == "milliseconds") {
            return duration.count() / 1e6;
        } else if (unit == "microseconds") {
            return duration.count() / 1e3;
        } else {
            return seconds;
        }
    }
    
private:
    static std::chrono::high_resolution_clock::time_point start_time;
};

std::chrono::high_resolution_clock::time_point Benchmark::start_time;

// Enhanced test base class
class TestBase {
public:
    virtual ~TestBase() = default;
    virtual std::string getName() const = 0;
    virtual void run() = 0;
    virtual bool setup() { return true; }
    virtual void teardown() {}
    
    void runTest() {
        if (!setup()) {
            std::cerr << "Setup failed for " << getName() << std::endl;
            return;
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        run();
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "✓ " << getName() << " passed (" << duration.count() << " ms)" << std::endl;
        
        teardown();
    }
};

// Main test runner
class TestRunner {
public:
    static void runAllTests() {
        std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl << std::endl;
        
        std::vector<std::unique_ptr<TestBase>> tests;
        
        // Unit tests
        tests.push_back(std::make_unique<NeuronTests>());
        tests.push_back(std::make_unique<BrainTests>());
        tests.push_back(std::make_unique<MemoryTests>());
        tests.push_back(std::make_unique<AgentWorldTests>());
        
        // Performance benchmarks
        tests.push_back(std::make_unique<BenchmarkTests>());
        
        // Integration tests
        tests.push_back(std::make_unique<IntegrationTests>());
        
        int passed = 0;
        int failed = 0;
        
        for (auto& test : tests) {
            try {
                test->runTest();
                passed++;
            } catch (const std::exception& e) {
                std::cerr << "✗ " << test->getName() << " failed: " << e.what() << std::endl;
                failed++;
            } catch (...) {
                std::cerr << "✗ " << test->getName() << " failed with unknown error" << std::endl;
                failed++;
            }
        }
        
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Tests passed: " << passed << std::endl;
        std::cout << "Tests failed: " << failed << std::endl;
        std::cout << "Total tests: " << (passed + failed) << std::endl;
        
        if (failed > 0) {
            std::cerr << std::endl << "Some tests failed!" << std::endl;
            exit(1);
        }
    }
};

// Include actual test implementations
#include "test_clock.cpp"
#include "test_neuron.cpp"
#include "test_brain.cpp"

// Entry point
int main() {
    TestRunner::runAllTests();
    return 0;
}

} // namespace test