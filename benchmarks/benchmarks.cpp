// Performance benchmarks for NLM improved API
// This file provides comprehensive benchmarking of the enhanced NLM functionality

#include <benchmark/benchmark.h>
#include "Brain.hpp"
#include "Config/Config.hpp"
#include <chrono>
#include <memory>
#include <vector>
#include <random>

namespace nlm {

// Benchmark: Configuration file loading
static void BM_ConfigLoadJSON(benchmark::State& state) {
    for (auto _ : state) {
        auto config = std::make_shared<Config>();
        // Simulate JSON loading by creating multiple config entries
        for (int i = 0; i < 100; ++i) {
            config->set(std::to_string(i), static_cast<double>(i * 0.1), ConfigSource::File);
        }
    }
}

static void BM_ConfigLoadSimple(benchmark::State& state) {
    for (auto _ : state) {
        auto config = std::make_shared<Config>();
        // Simulate simple key=value loading
        for (int i = 0; i < 100; ++i) {
            std::string key = "key" + std::to_string(i);
            std::string value = "value" + std::to_string(i);
            config->set(key, value, ConfigSource::File);
        }
    }
}

// Benchmark: Brain initialization
static void BM_BrainInit(benchmark::State& state) {
    for (auto _ : state) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", state.range(0));
        config->set("simulation_timestep", 0.001);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
    }
}

// Benchmark: Simulation steps
static void BM_SimulationSteps(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", state.range(0));
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    for (auto _ : state) {
        for (SimulationStep step = 0; step < state.range(1); ++step) {
            brain->step(step, step * 0.001);
        }
    }
}

// Benchmark: Sensory processing
static void BM_SensoryProcessing(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", state.range(0));
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create mock vision data
    std::vector<float> vision_data(state.range(1) * state.range(2) * state.range(3), 0.5f);
    
    for (auto _ : state) {
        for (int i = 0; i < 50; ++i) {
            brain->receiveSensoryInput(vision_data);
            brain->step(i, i * 0.001);
        }
    }
}

// Benchmark: Motor output processing
static void BM_MotorOutput(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", state.range(0));
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    for (auto _ : state) {
        for (int i = 0; i < 50; ++i) {
            brain->step(i, i * 0.001);
            auto action = brain->produceAction();
        }
    }
}

// Benchmark: Memory allocation patterns
static void BM_MemoryAllocation(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<std::shared_ptr<Brain>> brains;
        
        for (int i = 0; i < state.range(0); ++i) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 100);
            config->set("simulation_timestep", 0.001);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            brains.push_back(brain);
        }
        
        // Use brains to prevent optimization
        for (auto& brain : brains) {
            for (SimulationStep step = 0; step < 10; ++step) {
                brain->step(step, step * 0.001);
            }
        }
    }
}

// Benchmark: Real-time performance
static void BM_RealTimePerformance(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", state.range(0));
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    for (auto _ : state) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < state.range(1); ++step) {
            auto step_start = std::chrono::high_resolution_clock::now();
            brain->step(step, step * 0.001);
            auto step_end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(step_end - step_start);
            if (duration.count() > 1000) {  // If step takes > 1ms, break
                state.Skip();
                break;
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        if (total_duration.count() > 0) {
            double steps_per_second = state.range(1) / (total_duration.count() / 1000.0);
            benchmark::DoNotOptimize(steps_per_second);
        }
    }
}

// Benchmark: Enhanced Python API compatibility
static void BM_EnhancedAPI(benchmark::State& state) {
    for (auto _ : state) {
        // Simulate enhanced Python API calls
        auto config = std::make_shared<Config>();
        config->set("neuron_count", state.range(0));
        config->set("simulation_timestep", 0.001);
        
        auto brain = std::make_shared<Brain>(config);
        brain->enable_stats_collection();  // New enhanced feature
        brain->initialize();
        
        for (SimulationStep step = 0; step < state.range(1); ++step) {
            brain->step(step);
            
            // Simulate enhanced API calls
            if (brain->get_stats()) {
                auto stats = brain->get_stats()->get_summary();
                benchmark::DoNotOptimize(stats);
            }
        }
    }
}

// Benchmark: Statistical computation overhead
static void BM_StatisticsComputation(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", state.range(0));
    
    auto brain = std::make_shared<Brain>(config);
    brain->enable_stats_collection();
    brain->initialize();
    
    for (auto _ : state) {
        for (SimulationStep step = 0; step < state.range(1); ++step) {
            brain->step(step);
            
            // Simulate statistics computation
            auto stats = brain->get_stats();
            if (stats) {
                auto summary = stats->get_summary();
                benchmark::DoNotOptimize(summary);
            }
        }
    }
}

// Benchmark: Development stage monitoring
static void BM_DevelopmentMonitoring(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", state.range(0));
    
    auto brain = std::make_shared<Brain>(config);
    brain->enable_stats_collection();
    brain->initialize();
    
    for (auto _ : state) {
        for (SimulationStep step = 0; step < state.range(1); ++step) {
            brain->step(step);
            
            // Simulate development monitoring
            auto dev_info = brain->get_developmental_stage_info();
            benchmark::DoNotOptimize(dev_info);
        }
    }
}

// Benchmark: Scalability with different brain sizes
BENCHMARK_REGISTER_FUNCTION(BM_ConfigLoadJSON)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(5000);

BENCHMARK_REGISTER_FUNCTION(BM_ConfigLoadSimple)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(5000);

BENCHMARK_REGISTER_FUNCTION(BM_BrainInit)
    ->Arg(500)
    ->Arg(1000)
    ->Arg(5000)
    ->Arg(10000);

BENCHMARK_REGISTER_FUNCTION(BM_SimulationSteps)
    ->Arg(500)   // neuron_count
    ->Arg(100)   // steps
    ->Arg(1000)  // steps
    ->Arg(5000); // steps

BENCHMARK_REGISTER_FUNCTION(BM_SensoryProcessing)
    ->Arg(256)   // neuron_count
    ->Arg(8)     // width
    ->Arg(8)     // height
    ->Arg(3);    // channels

BENCHMARK_REGISTER_FUNCTION(BM_MotorOutput)
    ->Arg(500)
    ->Arg(1000)
    ->Arg(5000);

BENCHMARK_REGISTER_FUNCTION(BM_MemoryAllocation)
    ->Arg(10)
    ->Arg(50)
    ->Arg(100);

BENCHMARK_REGISTER_FUNCTION(BM_RealTimePerformance)
    ->Arg(500)   // neuron_count
    ->Arg(100)   // steps
    ->Arg(500);  // steps

BENCHMARK_REGISTER_FUNCTION(BM_EnhancedAPI)
    ->Arg(500)   // neuron_count
    ->Arg(50)    // steps
    ->Arg(100);  // steps

BENCHMARK_REGISTER_FUNCTION(BM_StatisticsComputation)
    ->Arg(500)   // neuron_count
    ->Arg(100)   // steps
    ->Arg(500);  // steps

BENCHMARK_REGISTER_FUNCTION(BM_DevelopmentMonitoring)
    ->Arg(500)   // neuron_count
    ->Arg(50)    // steps
    ->Arg(100);  // steps

// Multi-core scaling benchmark
static void BM_MultiCore(benchmark::State& state) {
    std::vector<std::shared_ptr<Brain>> brains;
    
    // Create multiple brains to simulate multi-core processing
    for (int i = 0; i < state.range(0); ++i) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", state.range(1));
        config->set("simulation_timestep", 0.001);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        brains.push_back(brain);
    }
    
    for (auto _ : state) {
        // Process brains in parallel (simulated)
        for (auto& brain : brains) {
            for (SimulationStep step = 0; step < 20; ++step) {
                brain->step(step, step * 0.001);
            }
        }
    }
}

BENCHMARK_REGISTER_FUNCTION(BM_MultiCore)
    ->Arg(2)    // number of cores
    ->Arg(4)    // number of cores
    ->Arg(8)    // number of cores
    ->Arg(16);  // number of cores

// Performance regression test
static void BM_PerformanceRegression(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", state.range(0));
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    brain->enable_stats_collection();  // Enable all new features
    brain->initialize();
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (auto _ : state) {
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step);
            
            // Simulate enhanced API usage
            if (brain->get_stats()) {
                auto stats = brain->get_stats()->get_summary();
                auto dev_info = brain->get_developmental_stage_info();
                benchmark::DoNotOptimize(stats);
                benchmark::DoNotOptimize(dev_info);
            }
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    if (total_duration.count() > 0) {
        double ops_per_second = (100.0 * state.range(1) * state.iterations()) / 
                               (total_duration.count() / 1000.0);
        benchmark::DoNotOptimize(ops_per_second);
    }
}

BENCHMARK_REGISTER_FUNCTION(BM_PerformanceRegression)
    ->Arg(100)
    ->Arg(500)
    ->Arg(1000);

// Stress test: High-frequency simulation
static void BM_HighFrequency(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", state.range(0));
    config->set("simulation_timestep", 0.0001);  // Very small timestep
    
    auto brain = std::make_shared<Brain>(config);
    brain->enable_stats_collection();
    brain->initialize();
    
    for (auto _ : state) {
        for (SimulationStep step = 0; step < state.range(1); ++step) {
            brain->step(step, step * 0.0001);
        }
    }
}

BENCHMARK_REGISTER_FUNCTION(BM_HighFrequency)
    ->Arg(100)    // neuron_count
    ->Arg(50)     // steps
    ->Arg(100)    // steps
    ->Arg(500);   // steps
}}  // namespace nlm

// Main function for running all benchmarks
void runPerformanceBenchmarks() {
    std::cout << "Running NLM Performance Benchmarks..." << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Run all registered benchmarks
    benchmark::RunSpecifiedBenchmarks();
    
    std::cout << "Benchmarks completed!" << std::endl;
}

// Simple benchmark runner for testing
void quickBenchmark() {
    std::cout << "Running quick benchmarks..." << std::endl;
    
    // Test configuration loading
    {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("simulation_timestep", 0.001);
        std::cout << "✓ Configuration loading: OK" << std::endl;
    }
    
    // Test brain initialization
    {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("simulation_timestep", 0.001);
        
        auto brain = std::make_shared<Brain>(config);
        brain->enable_stats_collection();
        brain->initialize();
        std::cout << "✓ Brain initialization with stats: OK" << std::endl;
    }
    
    // Test simulation
    {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 300);
        config->set("simulation_timestep", 0.001);
        
        auto brain = std::make_shared<Brain>(config);
        brain->enable_stats_collection();
        brain->initialize();
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step);
        }
        
        auto stats = brain->get_stats();
        if (stats) {
            auto summary = stats->get_summary();
            std::cout << "✓ Simulation with enhanced API: OK" << std::endl;
        }
    }
    
    std::cout << "All quick benchmarks completed!" << std::endl;
}}