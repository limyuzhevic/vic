// CLI Demo - Demonstrates the advanced NLM command-line interface features
// Shows profiling, benchmarking, and experiment management capabilities

#include "core/CLI/CLIFramework.hpp"
#include "core/Profiling/ProfilingSystem.hpp"
#include "core/Performance/PerformanceBenchmark.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

using namespace nlm;

// Demo experiment functions
void demoNeuralComputation() {
    std::cout << "Running neural computation demonstration...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "Neural computation completed successfully!\n";
}

void demoMemoryProfile() {
    std::cout << "Collecting memory profile...\n";
    // Simulate memory usage collection
    Profiler& profiler = ProfilingSystem::getInstance().getProfiler();
    profiler.start("memory_test");
    
    // Simulate memory-intensive operation
    std::vector<std::vector<int>> matrix(1000, std::vector<int>(1000, 1));
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    profiler.stop("memory_test");
    
    std::cout << "Memory profile collected.\n";
}

void demoBenchmarks() {
    std::cout << "Running performance benchmarks...\n";
    
    BenchmarkSuite suite;
    
    // Add a simple benchmark
    suite.addBenchmark("CPU Test", "CPU processing benchmark",
                      []() {
                          std::this_thread::sleep_for(std::chrono::milliseconds(100));
                      }, 10);
    
    // Run benchmarks
    auto results = suite.runAll();
    
    for (const auto& result : results) {
        std::cout << "Benchmark: " << result.name << "\n";
        std::cout << "  Time: " << result.timeSeconds << "s\n";
        std::cout << "  OPS: " << result.operationsPerSecond << "\n";
        std::cout << "  Threads: " << ThreadPool().getThreadCount() << "\n";
    }
}

void demoConfiguration() {
    std::cout << "Demonstrating advanced configuration...\n";
    
    // Create and save configuration
    auto config = std::make_shared<Config>();
    config->set("neural_neurons", 1000);
    config->set("simulation_timestep", 0.001);
    config->set("plasticity_enabled", true);
    config->set("debug_mode", false);
    
    // Save to file
    bool saved = config->saveToFile("config_demo.cfg");
    if (saved) {
        std::cout << "Configuration saved to config_demo.cfg\n";
        
        // Load and verify
        auto config2 = std::make_shared<Config>();
        bool loaded = config2->loadFromFile("config_demo.cfg");
        if (loaded) {
            std::cout << "Configuration loaded successfully.\n";
            std::cout << "  Neural neurons: " << config2->getOr<int>("neural_neurons", 0) << "\n";
            std::cout << "  Simulation timestep: " << config2->getOr<double>("simulation_timestep", 0.0) << "\n";
        }
    }
}

void demoProfiling() {
    std::cout << "Demonstrating profiling capabilities...\n";
    
    Profiler& profiler = ProfilingSystem::getInstance().getProfiler();
    MetricsCollector& metrics = ProfilingSystem::getInstance().getMetricsCollector();
    MemoryProfiler& memory = ProfilingSystem::getInstance().getMemoryProfiler();n    
    // Start profiling
    ProfilingSystem::getInstance().startAllProfiling();
    
    // Perform some operations
    profiler.start("operation_1");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    profiler.stop("operation_1");
    
    metrics.record("cpu_usage", 0.75);
    metrics.record("memory_usage", 512);
    
    profiler.start("operation_2");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    profiler.stop("operation_2");
    
    memory.trackAllocation(nullptr, 1024, "benchmark");
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Generate reports
    std::cout << "\nProfiling Report:\n";
    std::cout << profiler.generateReport() << "\n";
    
    std::cout << "Memory Report:\n";
    std::cout << memory.generateMemoryReport() << "\n";
    
    std::cout << "Metrics:\n";
    std::cout << "  Total metrics recorded: " << metrics.getTotalCount() << "\n";
    
    // Stop profiling
    ProfilingSystem::getInstance().stopAllProfiling();
}

void demoCommandLine() {
    std::cout << "Demonstrating command-line interface...\n";
    
    // Create CLI context
    CLIContext& cli = globalCLIContext;
    
    auto config = std::make_shared<Config>();
    config->set("system", "NLM CLI Demo");
    config->set("version", "1.0.0");
    cli.initialize(config);
    
    // Execute sample commands
    std::cout << "\nExecuting: config show\n";
    CLI::execute("config show");
    
    std::cout << "\nExecuting: help\n";
    CLI::help();
    
    std::cout << "\nExecuting: version\n";
    CLI::version();
}

void demoExperimentRunner() {
    std::cout << "Demonstrating experiment runner...\n";
    
    // Create a simple experiment suite
    BenchmarkSuite experimentSuite;
    
    experimentSuite.addBenchmark("Phase 1", "Phase 1 neural setup",
                                []() { demoNeuralComputation(); }, 1);
    
    experimentSuite.addBenchmark("Phase 2", "Phase 2 dynamics",
                                []() { std::this_thread::sleep_for(std::chrono::milliseconds(150)); }, 1);
    
    experimentSuite.addBenchmark("Phase 3", "Phase 3 plasticity",
                                []() { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }, 1);
    
    // Run experiments
    std::cout << "Running experiment suite...\n";
    auto results = experimentSuite.runAll();
    
    std::cout << "\nExperiment Results:\n";
    for (const auto& result : results) {
        std::cout << "  " << result.name << ": " << result.timeSeconds << "s\n";
    }
    
    // Save results
    bool saved = experimentSuite.saveResults(results, "experiment_results.csv");
    if (saved) {
        std::cout << "Results saved to experiment_results.csv\n";
    }
}

int main(int argc, char* argv[]) {
    std::cout << "===========================================\n";
    std::cout << "NLM CLI Demo - Advanced Interface Showcase\n";
    std::cout << "===========================================\n\n";
    
    // Initialize CLI system
    CLI::initialize();
    
    // Run demonstrations based on command line arguments
    if (argc > 1) {
        std::string arg = argv[1];
        
        if (arg == "demo" || arg == "d") {
            demoNeuralComputation();
        } else if (arg == "profile" || arg == "p") {
            demoProfiling();
        } else if (arg == "benchmark" || arg == "b") {
            demoBenchmarks();
        } else if (arg == "config" || arg == "c") {
            demoConfiguration();
        } else if (arg == "experiment" || arg == "e") {
            demoExperimentRunner();
        } else if (arg == "full" || arg == "f") {
            demoNeuralComputation();
            demoProfiling();
            demoBenchmarks();
            demoConfiguration();
            demoExperimentRunner();
        } else {
            std::cout << "Unknown demo mode: " << arg << "\n";
            std::cout << "Available modes: demo, profile, benchmark, config, experiment, full\n";
        }
    } else {
        // Run all demonstrations
        demoNeuralComputation();
        demoProfiling();
        demoBenchmarks();
        demoConfiguration();
        demoExperimentRunner();
    }
    
    // Generate final report
    std::cout << "\n===========================================\n";
    std::cout << "Demo completed successfully!\n";
    std::cout << "===========================================\n\n";
    
    // Show system performance profile
    std::cout << "System Performance Profile:\n";
    std::cout << CLI::getPerformanceStats() << "\n";
    
    // Cleanup
    CLI::cleanup();
    
    return 0;
}