//!/usr/bin/env cpp
/**
 * Example: Using Performance Profiling in NLM Phase 6 Demo
 * 
 * This example demonstrates how to use the performance profiling system
 * in the NLM Phase 6 demo. It shows basic profiling setup and usage.
 */

#include "experiments/PerformanceProfiler.hpp"
#include "experiments/Phase6DemoPerformance.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <memory>

using namespace nlm;

/**
 * Example 1: Basic Performance Profiling Setup
 */
void exampleBasicProfiling() {
    std::cout << "=== Example 1: Basic Performance Profiling ===" << std::endl;
    
    // Create a performance profiler
    PerformanceProfiler profiler(true, "./example_profiles", "text", true);
    
    // Start profiling
    profiler.start();
    
    // Simulate some work
    for (int i = 0; i < 1000; ++i) {
        profiler.recordStepMetrics(i, 1000.0); // 1ms per step
        profiler.recordSystemEvent("WorkItem", 1000.0);
    }
    
    // Stop profiling
    profiler.stop();
    
    // Export results
    profiler.exportToText("./example_profiles/basic_profiling.txt");
    
    // Print summary
    profiler.printSummary();
    
    std::cout << "Basic profiling completed. Results saved to example_profiles/basic_profiling.txt" << std::endl << std::endl;
}

/**
 * Example 2: Brain System Profiling
 */
void exampleBrainProfiling(std::shared_ptr<Brain> brain) {
    std::cout << "=== Example 2: Brain System Profiling ===" << std::endl;
    
    // Create a performance profiler with brain system focus
    PerformanceProfiler profiler(true, "./example_profiles", "json", true);
    profiler.setSamplingInterval(100.0); // 100 microseconds
    
    // Profile brain systems for 10 steps
    for (uint64_t step = 0; step < 10; ++step) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Step the brain (this would be the actual simulation step)
        brain->step(step, std::chrono::duration<double>::count(
            std::chrono::high_resolution_clock::now().time_since_epoch()));
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto stepTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        
        // Record brain system performance
        profiler.recordStepMetrics(step, stepTimeUs);
        profiler.profileBrainSystems(*brain, step);
        
        // Small delay to simulate computation
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    
    profiler.stop();
    
    // Export results
    profiler.exportToJSON("./example_profiles/brain_profiling.json");
    
    std::cout << "Brain system profiling completed. Results saved to example_profiles/brain_profiling.json" << std::endl << std::endl;
}

/**
 * Example 3: Phase 6 Demo Performance Integration
 */
void examplePhase6DemoIntegration() {
    std::cout << "=== Example 3: Phase 6 Demo Performance Integration ===" << std::endl;
    
    // Create configuration with profiling enabled
    Phase6DemoConfig config;
    config.enableProfiling = true;
    config.profileOutputDir = "./example_profiles";
    config.profileFormat = "csv";
    config.profileConsoleOutput = true;
    config.profileIntervalMs = 10.0;
    
    // Create performance profiler
    PerformanceProfiler profiler;
    Phase6DemoProfiling::applyProfileConfig(config, profiler);
    
    // Simulate demo execution with metrics
    Phase6DemoMetrics demoMetrics;
    
    for (uint64_t step = 0; step < 100; ++step) {
        double visualizationTimeUs = 5000.0 + (rand() % 2000); // 5-7ms
        double exportTimeUs = 1000.0 + (rand() % 500);       // 1-1.5ms
        size_t memoryUsage = 100 * 1024 * 1024 + (rand() % 50 * 1024 * 1024); // 100-150MB
        
        demoMetrics.recordStep(step, visualizationTimeUs, exportTimeUs, memoryUsage);
        
        // Simulate system events
        if (step % 10 == 0) demoMetrics.recordSpikeEvent();
        if (step % 15 == 0) demoMetrics.recordPlasticityEvent();
        if (step % 20 == 0) demoMetrics.recordDevelopmentEvent();
        if (step % 25 == 0) demoMetrics.recordPredictionEvent();
        if (step % 30 == 0) demoMetrics.recordMemoryEvent();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Export metrics
    Phase6DemoProfiling::exportPhase6DemoMetrics(demoMetrics, "./example_profiles/demo_metrics");
    
    // Generate performance analysis
    Phase6DemoProfiling::generatePerformanceAnalysis(demoMetrics, "./example_profiles/demo_analysis");
    
    std::cout << "Phase 6 demo performance integration completed." << std::endl;
    std::cout << "Metrics saved to example_profiles/demo_metrics.txt" << std::endl;
    std::cout << "Analysis saved to example_profiles/demo_analysis.txt" << std::endl << std::endl;
}

/**
 * Example 4: Command-Line Usage
 */
void exampleCommandLineUsage() {
    std::cout << "=== Example 4: Command-Line Usage ===" << std::endl;
    std::cout << "Command-line options for performance profiling:" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Visualization options:" << std::endl;
    std::cout << "  --help                    Display help information" << std::endl;
    std::cout << "  --width [pixels]          Set window width" << std::endl;
    std::cout << "  --height [pixels]         Set window height" << std::endl;
    std::cout << "  --profile-console [0|1]   Enable/disable console output" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Performance profiling options:" << std::endl;
    std::cout << "  --phase6-profile [dir]     Enable profiling with output directory" << std::endl;
    std::cout << "  --phase6-profile-format [fmt] Specify output format (text, json, csv)" << std::endl;
    std::cout << "  --phase6-profile-console [0|1] Enable/disable console output" << std::endl;
    std::cout << "  --phase6-profile-interval [ms] Sampling interval in milliseconds" << std::endl;
    std::cout << "  --phase6-profile-memory    Enable memory profiling" << std::endl;
    std::cout << "  --phase6-profile-cpu       Enable CPU profiling" << std::endl;
    std::cout << "  --phase6-profile-system    Enable system-wide profiling" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Example command-line usage:" << std::endl;
    std::cout << "./Phase6Demo --phase6-profile ./profiles --phase6-profile-format json --phase6-profile-console 1" << std::endl;
    std::cout << std::endl;
}

/**
 * Example 5: Performance Analysis
 */
void examplePerformanceAnalysis() {
    std::cout << "=== Example 5: Performance Analysis ===" << std::endl;
    
    // Create some sample metrics
    Phase6DemoMetrics metrics;
    
    // Record sample data
    for (uint64_t step = 0; step < 50; ++step) {
        double visualizationTimeUs = 3000.0 + (rand() % 1000); // 3-4ms
        double exportTimeUs = 500.0 + (rand() % 200);       // 0.5-0.7ms
        size_t memoryUsage = 80 * 1024 * 1024 + (rand() % 30 * 1024 * 1024); // 80-110MB
        
        metrics.recordStep(step, visualizationTimeUs, exportTimeUs, memoryUsage);
        
        // Simulate events
        if (step % 10 == 0) metrics.recordSpikeEvent();
        if (step % 15 == 0) metrics.recordMemoryEvent();
    }
    
    // Analyze performance
    Phase6DemoPerformanceAnalyzer analyzer;
    
    std::string analysis = analyzer.analyzePerformance(metrics);
    std::string report = analyzer.generatePerformanceReport(metrics, "Phase 6 Demo");
    std::string bottlenecks = analyzer.generateBottleneckAnalysis(metrics);
    std::string memoryAnalysis = analyzer.analyzeMemoryPerformance(metrics);
    std::string cpuAnalysis = analyzer.analyzeCPUPerformance(metrics);
    
    // Display analysis
    std::cout << analysis << std::endl;
    std::cout << report << std::endl;
    std::cout << bottlenecks << std::endl;
    std::cout << memoryAnalysis << std::endl;
    std::cout << cpuAnalysis << std::endl;
    
    std::cout << "Performance analysis completed successfully." << std::endl << std::endl;
}

int main(int argc, char* argv[]) {
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    std::cout << "=== Performance Profiling Examples ===" << std::endl;
    std::cout << "This program demonstrates various performance profiling scenarios." << std::endl;
    std::cout << std::endl;
    
    // Run examples based on command-line arguments
    if (argc > 1) {
        std::string arg = argv[1];
        
        if (arg == "basic") {
            exampleBasicProfiling();
        } else if (arg == "brain") {
            // Create a simple brain for demonstration
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 100);
            cfg->set("region_count", 1);
            cfg->set("connection_probability", 0.1f);
            auto brain = std::make_shared<Brain>(cfg);
            brain->initialize();
            exampleBrainProfiling(brain);
        } else if (arg == "demo") {
            examplePhase6DemoIntegration();
        } else if (arg == "cli") {
            exampleCommandLineUsage();
        } else if (arg == "analysis") {
            examplePerformanceAnalysis();
        } else if (arg == "all") {
            exampleBasicProfiling();
            exampleCommandLineUsage();
            examplePerformanceAnalysis();
        } else if (arg == "help") {
            exampleCommandLineUsage();
        } else {
            std::cout << "Unknown example: " << arg << std::endl;
            std::cout << "Available examples: basic, brain, demo, cli, analysis, all, help" << std::endl;
            return 1;
        }
    } else {
        std::cout << "Performance profiling examples are available:" << std::endl;
        std::cout << "  ./performance_examples basic     - Basic performance profiling" << std::endl;
        std::cout << "  ./performance_examples brain      - Brain system profiling" << std::endl;
        std::cout << "  ./performance_examples demo       - Phase 6 demo integration" << std::endl;
        std::cout << "  ./performance_examples cli        - Command-line usage" << std::endl;
        std::cout << "  ./performance_examples analysis  - Performance analysis" << std::endl;
        std::cout << "  ./performance_examples all        - Run all examples" << std::endl;
        std::cout << "  ./performance_examples help       - Show help" << std::endl;
    }
    
    return 0;
}
