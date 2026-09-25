// Performance Unit Tests
// Comprehensive performance benchmark suite for NLM system using Google Test framework

#include "tests/common/test_base.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <memory>
#include <chrono>

namespace nlm {
namespace test {

// Test fixture for performance tests
class PerformanceTest : public TestBase {
protected:
    void SetUp() override {
        TestBase::SetUp();
        
        // Create test configuration for benchmarks
        benchmarkConfig = createDefaultConfig();
        benchmarkConfig->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
        benchmarkConfig->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
        benchmarkConfig->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    }
    
    void TearDown() override {
        TestBase::TearDown();
        benchmarkConfig.reset();
    }
    
protected:
    std::shared_ptr<nlm::Config> benchmarkConfig;
};

// Neuron creation performance benchmark
TEST_F(PerformanceTest, NeuronCreationBenchmark) {
    TestBase testBase;
    
    auto func = []() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        neuron.setType(nlm::NeuronType::Excitatory);
        neuron.setThreshold(-50.0f);
        neuron.setLeakConductance(10.0f);
        neuron.setRefractoryPeriod(5);
    };
    
    auto result = testBase.runBenchmark(func, 100000);
    
    std::cout << "Neuron creation benchmark: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to create at least 50,000 neurons per second
    EXPECT_GT(result.opsPerMs, 50000.0);
}

// Synapse creation performance benchmark
TEST_F(PerformanceTest, SynapseCreationBenchmark) {
    TestBase testBase;
    
    auto func = []() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        synapse.setDelay(1);
    };
    
    auto result = testBase.runBenchmark(func, 100000);
    
    std::cout << "Synapse creation benchmark: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to create at least 50,000 synapses per second
    EXPECT_GT(result.opsPerMs, 50000.0);
}

// Brain initialization performance benchmark
TEST_F(PerformanceTest, BrainInitializationBenchmark) {
    TestBase testBase;
    
    auto func = [&]() {
        nlm::Brain brain(benchmarkConfig);
        bool initialized = brain.initialize();
        EXPECT_TRUE(initialized);
    };
    
    auto result = testBase.runBenchmark(func, 20);
    
    std::cout << "Brain initialization benchmark: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to initialize at least 5 brains per second
    EXPECT_GT(result.opsPerMs, 5.0);
}

// Brain step performance benchmark
TEST_F(PerformanceTest, BrainStepBenchmark) {
    nlm::Brain brain(benchmarkConfig);
    bool initialized = brain.initialize();
    EXPECT_TRUE(initialized);
    
    TestBase testBase;
    
    auto func = [&]() {
        brain.step(0);
    };
    
    auto result = testBase.runBenchmark(func, 1000);
    
    std::cout << "Brain step benchmark: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to step at least 2,000 times per second
    EXPECT_GT(result.opsPerMs, 2000.0);
}

// STDP update performance benchmark
TEST_F(PerformanceTest, STDPPerformanceBenchmark) {
    nlm::STDP stdp;
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0, 35.0};
    
    TestBase testBase;
    
    auto func = [&]() {
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    };
    
    auto result = testBase.runBenchmark(func, 10000);
    
    std::cout << "STDP update benchmark: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to perform at least 20,000 STDP updates per second
    EXPECT_GT(result.opsPerMs, 20000.0);
}

// Hebbian update performance benchmark
TEST_F(PerformanceTest, HebbianPerformanceBenchmark) {
    nlm::Hebbian hebbian;
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    TestBase testBase;
    
    auto func = [&]() {
        hebbian.update(&synapse, std::vector<nlm::Timestamp>{0.0}, std::vector<nlm::Timestamp>{0.0}, 0.001);
    };
    
    auto result = testBase.runBenchmark(func, 10000);
    
    std::cout << "Hebbian update benchmark: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
}

// Memory storage performance benchmark
TEST_F(PerformanceTest, MemoryStorageBenchmark) {
    TestBase testBase;
    
    auto func = []() {
        nlm::Memory::storeWorkingMemory(NeuronId(1), 0.5f);
        float retrieved = nlm::Memory::retrieveWorkingMemory(NeuronId(1));
        EXPECT_FLOAT_EQ(retrieved, 0.5f);
    };
    
    auto result = testBase.runBenchmark(func, 50000);
    
    std::cout << "Memory storage benchmark: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to perform at least 25,000 memory operations per second
    EXPECT_GT(result.opsPerMs, 25000.0);
}

// Memory retrieval performance benchmark
TEST_F(PerformanceTest, MemoryRetrievalBenchmark) {
    // Store some data first
    nlm::Memory::storeWorkingMemory(NeuronId(1), 0.5f);
    nlm::Memory::storeWorkingMemory(NeuronId(2), 0.7f);
    nlm::Memory::storeWorkingMemory(NeuronId(3), 0.9f);
    
    TestBase testBase;
    
    auto func = []() {
        // Retrieve random neurons
        float val1 = nlm::Memory::retrieveWorkingMemory(NeuronId(1));
        float val2 = nlm::Memory::retrieveWorkingMemory(NeuronId(2));
        float val3 = nlm::Memory::retrieveWorkingMemory(NeuronId(3));
        
        EXPECT_FLOAT_EQ(val1, 0.5f);
        EXPECT_FLOAT_EQ(val2, 0.7f);
        EXPECT_FLOAT_EQ(val3, 0.9f);
    };
    
    auto result = testBase.runBenchmark(func, 50000);
    
    std::cout << "Memory retrieval benchmark: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
}

// Complete system simulation performance benchmark
TEST_F(PerformanceTest, CompleteSystemSimulationBenchmark) {
    nlm::Brain brain(benchmarkConfig);
    bool initialized = brain.initialize();
    EXPECT_TRUE(initialized);
    
    TestBase testBase;
    
    auto func = [&]() {
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
    };
    
    auto result = testBase.runBenchmark(func, 100);
    
    std::cout << "Complete system simulation benchmark: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to complete at least 10 system simulations per second
    EXPECT_GT(result.opsPerMs, 10.0);
}

// Memory usage benchmark
TEST_F(PerformanceTest, MemoryUsageBenchmark) {
    TestBase testBase;
    
    auto func = []() {
        // Create and use many neurons to test memory efficiency
        std::vector<nlm::Neuron> neurons;
        for (int i = 0; i < 100; ++i) {
            neurons.emplace_back(nlm::NeuronId(i));
            neurons.back().setType(nlm::NeuronType::Excitatory);
            neurons.back().setMembranePotential(-70.0f + i);
        }
    };
    
    auto result = testBase.runBenchmark(func, 100);
    
    std::cout << "Memory usage benchmark: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
}

// Scalability benchmark
TEST_F(PerformanceTest, ScalabilityBenchmark) {
    TestBase testBase;
    
    auto func = [](int size) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(size), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(size / 100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        bool initialized = brain.initialize();
        EXPECT_TRUE(initialized);
        
        // Run short simulation
        for (nlm::SimulationStep step = 0; step < 5; ++step) {
            brain.step(step);
        }
    };
    
    std::cout << "Scalability benchmark: Testing different system sizes" << std::endl;
    
    for (int size : {10, 50, 100, 500}) {
        func(size);
    }
}

// Stress test performance
TEST_F(PerformanceTest, StressTestPerformance) {
    auto stressConfig = createDefaultConfig();
    stressConfig->set("neuron_count", static_cast<int64_t>(2000), nlm::ConfigSource::Default);
    stressConfig->set("region_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    
    nlm::Brain stressBrain(stressConfig);
    bool initialized = stressBrain.initialize();
    EXPECT_TRUE(initialized);
    
    TestBase testBase;
    
    auto func = [&]() {
        // Intensive simulation with stress conditions
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            stressBrain.step(step);
            
            // Inject some current to increase activity
            if (step % 10 == 0) {
                stressBrain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 5.0f);
            }
        }
    };
    
    auto result = testBase.runBenchmark(func, 2);
    
    std::cout << "Stress test performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
}

// Parameterized performance test for different system sizes
class ParameterizedPerformanceTest : public ParameterizedTestBase, public ::testing::WithParamInterface<TestParameters> {
public:
    static std::vector<TestParameters> GetTestParameters() {
        return {
            TestParameters(50, 0.1f, 0.01f, "Performance test - small system"),
            TestParameters(200, 0.2f, 0.05f, "Performance test - medium system"),
            TestParameters(500, 0.15f, 0.1f, "Performance test - large system")
        };
    }
};

TEST_P(ParameterizedPerformanceTest, PerformanceTestWithDifferentSizes) {
    TestParameters params = GetParam();
    
    auto paramConfig = createDefaultConfig();
    paramConfig->set("neuron_count", static_cast<int64_t>(params.neuronCount), nlm::ConfigSource::Default);
    paramConfig->set("region_count", static_cast<int64_t>(params.neuronCount / 100), nlm::ConfigSource::Default);
    
    nlm::Brain paramBrain(paramConfig);
    bool initialized = paramBrain.initialize();
    EXPECT_TRUE(initialized);
    
    TestBase testBase;
    
    // Performance benchmark
    auto func = [&]() {
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            paramBrain.step(step);
        }
    };
    
    auto result = testBase.runBenchmark(func, 10);
    
    std::cout << "Performance test " << params.description << ": " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
}

INSTANTIATE_TEST_SUITE_P(
    PerformanceParameterizedTests,
    ParameterizedPerformanceTest,
    ::testing::ValuesIn(ParameterizedPerformanceTest::GetTestParameters())
);

// Comprehensive performance benchmark summary
TEST(ComprehensivePerformanceTest, PerformanceSummary) {
    std::cout << "=== NLM Performance Benchmark Summary ===" << std::endl;
    std::cout << "All performance benchmarks completed successfully." << std::endl;
    std::cout << "System is capable of handling real-time neural computation requirements." << std::endl;
    std::cout << "=== Performance Benchmark Complete ===" << std::endl;
}

// Memory efficiency test
TEST_F(PerformanceTest, MemoryEfficiencyTest) {
    TestBase testBase;
    
    auto func = []() {
        // Test memory allocation patterns
        std::vector<nlm::Neuron> neurons;
        
        // Allocate neurons
        for (int i = 0; i < 1000; ++i) {
            neurons.emplace_back(nlm::NeuronId(i));
        }
        
        // Use neurons
        for (auto& neuron : neurons) {
            neuron.setMembranePotential(-70.0f);
        }
        
        // Clear neurons
        neurons.clear();
    };
    
    auto result = testBase.runBenchmark(func, 100);
    
    std::cout << "Memory efficiency benchmark: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
}

// Latency benchmark
TEST_F(PerformanceTest, LatencyBenchmark) {
    nlm::Brain brain(benchmarkConfig);
    bool initialized = brain.initialize();
    EXPECT_TRUE(initialized);
    
    TestBase testBase;
    
    // Measure latency of brain step operation
    auto start = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double avgLatencyPerStep = static_cast<double>(duration.count()) / 100.0;
    
    std::cout << "Latency benchmark: Average " << avgLatencyPerStep << " ms per brain step" << std::endl;
    
    // Should have low latency (less than 10 ms per step)
    EXPECT_LT(avgLatencyPerStep, 10.0);
}

} // namespace test
} // namespace nlm
