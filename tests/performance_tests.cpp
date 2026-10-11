// Performance Tests
// Comprehensive performance testing for NLM components

#include "performance/Performance.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include <memory>

namespace nlm::tests {

class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        
        brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Initialize performance monitoring
        brain->resetPerformanceStats();
    }
    
    void TearDown() override {
        brain.reset();
        config.reset();
    }
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
};

// Performance Benchmark Tests
TEST_F(PerformanceTest, MinimumStepsPerSecond) {
    constexpr double min_steps_per_second = 1000.0; // 1k steps/sec minimum
    
    auto start_time = std::chrono::high_resolution_clock::now();
    const int steps = 1000;
    
    for (int i = 0; i < steps; ++i) {
        brain->step(i);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time);
    
    double steps_per_second = steps / duration.count();
    EXPECT_GE(steps_per_second, min_steps_per_second);
}

TEST_F(PerformanceTest, MemoryUsage) {
    auto initial_stats = brain->getPerformanceStats();
    double initial_memory = initial_stats.memoryUsageMB;
    
    // Run simulation
    for (int i = 0; i < 100; ++i) {
        brain->step(i);
    }
    
    auto final_stats = brain->getPerformanceStats();
    double final_memory = final_stats.memoryUsageMB;
    
    EXPECT_LT(final_memory - initial_memory, 100.0); // Less than 100MB growth
}

TEST_F(PerformanceTest, SpikeProcessingRate) {
    const int steps = 500;
    int spike_count = 0;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < steps; ++i) {
        brain->step(i);
        spike_count = brain->getTotalSpikeCount();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time);
    
    double spikes_per_second = spike_count / duration.count();
    EXPECT_GT(spikes_per_second, 100.0); // Should process at least 100 spikes/sec
}

TEST_F(PerformanceTest, ActionProductionRate) {
    const int steps = 1000;
    int action_count = 0;
    
    for (int i = 0; i < steps; ++i) {
        auto action = brain->produceAction();
        if (action) {
            action_count++;
        }
    }
    
    double action_rate = static_cast<double>(action_count) / steps;
    EXPECT_GT(action_rate, 0.1); // Should produce actions at least 10% of the time
}

TEST_F(PerformanceTest, MemoryPoolEfficiency) {
    // Test memory pool efficiency
    const int iterations = 1000;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        // Create and destroy brain instances to test memory pool
        auto test_config = std::make_shared<Config>();
        test_config->set("neuron_count", 100);
        auto test_brain = std::make_shared<Brain>(test_config);
        test_brain->initialize();
        
        for (int j = 0; j < 10; ++j) {
            test_brain->step(j);
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time);
    
    double iterations_per_second = iterations / duration.count();
    EXPECT_GT(iterations_per_second, 100.0); // Should handle >100 instances/sec
}

TEST_F(PerformanceTest, SIMDOptimization) {
    // Test SIMD-optimized operations
    const int neurons = 500;
    
    auto simd_config = std::make_shared<Config>();
    simd_config->set("neuron_count", neurons);
    simd_config->set("region_count", 1);
    
    auto simd_brain = std::make_shared<Brain>(simd_config);
    simd_brain->initialize();
    
    // Measure SIMD performance
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        simd_brain->step(i);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time);
    
    double operations_per_second = 100.0 * neurons / duration.count();
    EXPECT_GT(operations_per_second, 1000.0); // Should perform >1k ops/sec
}

TEST_F(PerformanceTest, MultithreadingEfficiency) {
    // Test multithreading performance
    const int steps = 200;
    const int threads = 4;
    
    auto thread_config = std::make_shared<Config>();
    thread_config->set("neuron_count", 1000);
    thread_config->set("region_count", 2);
    
    auto thread_brain = std::make_shared<Brain>(thread_config);
    thread_brain->initialize();
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Simulate threaded execution
    std::vector<std::future<void>> futures;
    for (int t = 0; t < threads; ++t) {
        futures.push_back(std::async(std::launch::async, [this, t]() {
            for (int i = 0; i < 50; ++i) {
                brain->step(t * 50 + i);
            }
        }));
    }
    
    for (auto& future : futures) {
        future.get();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time);
    
    double threads_per_second = threads / duration.count();
    EXPECT_GT(threads_per_second, 0.5); // Should achieve good thread throughput
}

// Memory Leak Detection Tests
TEST_F(PerformanceTest, NoMemoryLeaks) {
    // Run with memory leak detection
    const int iterations = 100;
    
    for (int i = 0; i < iterations; ++i) {
        auto test_config = std::make_shared<Config>();
        auto test_brain = std::make_shared<Brain>(test_config);
        test_brain->initialize();
        
        for (int j = 0; j < 10; ++j) {
            test_brain->step(j);
        }
        
        // Force cleanup
        test_brain.reset();
        test_config.reset();
    }
}

TEST_F(PerformanceTest, MemoryPoolCleanup) {
    // Test memory pool cleanup
    auto pool_stats = nlm::MemoryPool::getStats();
    
    // Create and destroy many objects
    for (int i = 0; i < 1000; ++i) {
        auto test_config = std::make_shared<Config>();
        auto test_brain = std::make_shared<Brain>(test_config);
        test_brain->initialize();
    }
    
    // Force garbage collection
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto cleanup_stats = nlm::MemoryPool::getStats();
    
    EXPECT_LE(cleanup_stats.fragmentation_ratio, 0.2); // Low fragmentation
}

// Stress Tests
TEST_F(PerformanceTest, LargeNetworkPerformance) {
    const int neurons = 5000;
    const int regions = 5;
    
    auto stress_config = std::make_shared<Config>();
    stress_config->set("neuron_count", neurons);
    stress_config->set("region_count", regions);
    stress_config->set("connection_probability", 0.05f);
    
    auto stress_brain = std::make_shared<Brain>(stress_config);
    stress_brain->initialize();
    
    auto start_time = std::chrono::high_resolution_clock::now();
    const int steps = 100;
    
    for (int i = 0; i < steps; ++i) {
        stress_brain->step(i);
        
        if (i % 10 == 0) {
            auto stats = stress_brain->getPerformanceStats();
            EXPECT_GT(stats.neuronsPerSecond, 1000.0);
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time);
    
    double steps_per_second = steps / duration.count();
    EXPECT_GT(steps_per_second, 50.0); // Should handle >50 steps/sec for 5k neurons
}

TEST_F(PerformanceTest, PeakMemoryUsage) {
    const int neurons = 2000;
    
    auto memory_config = std::make_shared<Config>();
    memory_config->set("neuron_count", neurons);
    memory_config->set("region_count", 2);
    
    auto memory_brain = std::make_shared<Brain>(memory_config);
    memory_brain->initialize();
    
    // Measure peak memory usage
    auto initial_stats = memory_brain->getPerformanceStats();
    double peak_memory = initial_stats.memoryUsageMB;
    
    for (int i = 0; i < 100; ++i) {
        memory_brain->step(i);
        
        auto current_stats = memory_brain->getPerformanceStats();
        peak_memory = std::max(peak_memory, current_stats.memoryUsageMB);
        
        EXPECT_LT(current_stats.memoryUsageMB, 500.0); // Less than 500MB for 2k neurons
    }
}

// Integration Performance Tests
TEST_F(PerformanceTest, CompleteSystemPerformance) {
    // Test complete system performance with all components
    auto integration_config = std::make_shared<Config>();
    integration_config->set("neuron_count", 2000);
    integration_config->set("region_count", 3);
    
    auto integration_brain = std::make_shared<Brain>(integration_config);
    integration_brain->initialize();
    
    // Simulate complete system
    const int episodes = 50;
    int total_actions = 0;
    int total_spikes = 0;
    
    for (int episode = 0; episode < episodes; ++episode) {
        integration_brain->reset();
        
        for (int step = 0; step < 100; ++step) {
            integration_brain->step(step);
            total_spikes = integration_brain->getTotalSpikeCount();
            
            auto action = integration_brain->produceAction();
            if (action) {
                total_actions++;
            }
        }
    }
    
    double actions_per_episode = static_cast<double>(total_actions) / episodes;
    double spikes_per_episode = static_cast<double>(total_spikes) / episodes;
    
    EXPECT_GT(actions_per_episode, 50.0); // Should produce >50 actions/episode
    EXPECT_GT(spikes_per_episode, 1000.0); // Should produce >1k spikes/episode
}

// Benchmark Functions for Google Benchmark
static void BM_BrainStep(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", state.range(0));
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    for (auto _ : state) {
        for (int i = 0; i < 10; ++i) {
            brain->step(i);
        }
    }
}

static void BM_BrainAction(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", state.range(0));
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    for (auto _ : state) {
        brain->step(state.range(1));
        brain->produceAction();
    }
}

BENCHMARK(BM_BrainStep)->Arg(100)->Arg(1000)->Arg(5000);
BENCHMARK(BM_BrainAction)->Arg(100)->Arg(1000)->Arg(5000);

} // namespace nlm::tests
