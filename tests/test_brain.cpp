// Brain Tests - Enhanced with parameterization and framework integration
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "TestFramework.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>

namespace test_brain {

// Parameterized brain configuration test
class BrainConfigParameterizedTest : public test_framework::TestBase {
private:
    size_t neuronCount_;
    size_t regionCount_;
    float connectionProb_;
public:
    BrainConfigParameterizedTest(const std::string& name, size_t neurons, 
                                size_t regions, float prob)
        : TestBase(name, test_framework::TestCategory::Integration),
          neuronCount_(neurons), regionCount_(regions), connectionProb_(prob) {}
    
    void run() override {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount_), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regionCount_), nlm::ConfigSource::Default);
        config->set("connection_probability", connectionProb_, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        bool success = brain.initialize();
        assert(success);
        
        // Verify configuration
        assert(brain.getRegionCount() == regionCount_);
        assert(brain.getTotalNeuronCount() == neuronCount_);
        assert(brain.getTotalSynapseCount() > 0);
        
        std::cout << "    Parameterized brain config test for " << getName() << " passed" << std::endl;
    }
};

// Performance benchmark for brain operations
class BrainPerformanceBenchmark : public test_framework::TestBase {
private:
    size_t neuronCount_;
    size_t steps_;
public:
    BrainPerformanceBenchmark(const std::string& name, size_t neurons, size_t steps)
        : TestBase(name, test_framework::TestCategory::Performance),
          neuronCount_(neurons), steps_(steps) {}
    
    void run() override {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount_), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Measure step performance
        auto start = std::chrono::high_resolution_clock::now();
        
        for (nlm::SimulationStep step = 0; step < steps_; ++step) {
            brain.step(step);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double, std::milli>(end - start).count();
        double avgTimePerStep = totalTime / steps_;
        
        std::cout << "    Performance benchmark " << getName() << ": " << steps_ << " steps in " 
                  << totalTime << "ms (avg: " << avgTimePerStep << "ms/step)" << std::endl;
        
        // Verify brain state
        assert(brain.getTotalNeuronCount() == neuronCount_);
    }
};

// System integration test for complete brain lifecycle
class BrainLifecycleIntegrationTest : public test_framework::TestBase {
public:
    BrainLifecycleIntegrationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Integration) {}
    
    void run() override {
        // Create brain with default config
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        
        // Phase 1: Initialization
        assert(brain.initialize());
        assert(brain.getRegionCount() == 2);
        assert(brain.getTotalNeuronCount() == 100);
        
        // Phase 2: Simulate some activity
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain.step(step);
        }
        
        // Phase 3: Action production
        auto action = brain.produceAction();
        assert(action != nullptr);
        
        // Phase 4: Reset
        brain.reset();
        assert(brain.getTotalNeuronCount() == 100);
        
        // Phase 5: Re-initialize
        assert(brain.initialize());
        
        std::cout << "    Brain lifecycle integration test for " << getName() << " passed" << std::endl;
    }
};

// Edge case test for invalid configurations
class BrainEdgeCaseTest : public test_framework::TestBase {
public:
    BrainEdgeCaseTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Unit) {}
    
    void run() override {
        // Test with minimal valid configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        assert(brain.getTotalNeuronCount() == 1);
        
        // Test with larger configuration
        config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
        
        nlm::Brain largeBrain(config);
        assert(largeBrain.initialize());
        assert(largeBrain.getTotalNeuronCount() == 10000);
        assert(largeBrain.getRegionCount() == 10);
        
        std::cout << "    Brain edge case test for " << getName() << " passed" << std::endl;
    }
};

// Unit Tests
void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

void runAll() {
    std::cout << "=== Running Enhanced Brain Tests ===" << std::endl;
    
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    
    std::cout << "\n=== Running Parameterized Tests ===" << std::endl;
    
    // Create test suite for parameterized tests
    std::vector<size_t> neuronCounts = {10, 100, 1000};
    std::vector<size_t> regionCounts = {1, 2, 5};
    std::vector<float> connectionProbs = {0.01f, 0.1f, 0.5f};
    
    for (size_t neurons : neuronCounts) {
        for (size_t regions : regionCounts) {
            for (float prob : connectionProbs) {
                if (neurons * regions < 10000) {  // Limit test complexity
                    BrainConfigParameterizedTest test(
                        "config_" + std::to_string(neurons) + "_neurons_" + 
                        std::to_string(regions) + "_regions_" + std::to_string(prob) + "_prob",
                        neurons, regions, prob
                    );
                    test.run();
                }
            }
        }
    }
    
    std::cout << "\n=== Running Integration Tests ===" << std::endl;
    
    // Run integration tests
    BrainLifecycleIntegrationTest lifecycleTest("brain_lifecycle_integration");
    lifecycleTest.run();
    
    std::cout << "\n=== Running Edge Case Tests ===" << std::endl;
    
    // Run edge case tests
    BrainEdgeCaseTest edgeCaseTest("brain_edge_cases");
    edgeCaseTest.run();
    
    std::cout << "\n=== Running Performance Benchmarks ===" << std::endl;
    
    // Run performance benchmarks
    std::vector<size_t> benchmarkSizes = {100, 1000, 5000};
    for (size_t neurons : benchmarkSizes) {
        BrainPerformanceBenchmark benchmark(
            "perf_benchmark_" + std::to_string(neurons) + "neurons",
            neurons, 100
        );
        benchmark.run();
    }
    
    std::cout << "\n=== All Brain Tests Completed ===" << std::endl;
}

} // namespace test_brain
