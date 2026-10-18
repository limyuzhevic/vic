#include "performance_monitor.h"
#include "profiler.h"
#include "benchmark.h"
#include "performance_report.h"
#include "resources_monitor.h"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <memory>
#include <chrono>
#include <fstream>
#include <iostream>

namespace nlm {

class PerformanceIntegration {
public:
    PerformanceIntegration() {
        logger = std::make_shared<Logger>();
        consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        
        // Initialize performance components
        monitor = std::make_shared<PerformanceMonitor>();
        profiler = std::make_shared<Profiler>();
        benchmarkSuite = std::make_shared<BenchmarkSuite>();
        reportGenerator = std::make_shared<PerformanceReportGenerator>();
        resourceMonitor = std::make_shared<ResourceMonitor>();
        
        NLM_LOG_INFO("Performance integration initialized");
    }

    // Core performance monitoring
    void setupCoreMonitoring(const std::string& configFile = "configs/default.cfg") {
        NLM_LOG_INFO("Setting up core performance monitoring");
        
        // Load configuration
        config = std::make_shared<Config>();
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Configuration loaded from: " + configFile);
        }
        
        // Configure profiler
        ProfilerConfig profilerConfig;
        profilerConfig.name = "simulation_profiler";
        profilerConfig.enabled = true;
        profilerConfig.samplingInterval = 0.1; // 100ms
        profilerConfig.trackMemory = true;
        profilerConfig.trackCPU = true;
        profilerConfig.trackIO = true;
        profilerConfig.trackEvents = true;
        profilerConfig.maxSamples = 10000;
        profilerConfig.outputFormat = "json";
        profilerConfig.outputFile = "performance_profile.json";
        profilerConfig.realTimeMonitoring = true;
        
        profiler->configure(profilerConfig);
        
        // Configure resource monitoring
        resourceMonitor->setSamplingInterval(1.0); // 1 second
        resourceMonitor->enableTemperatureMonitoring(true);
        resourceMonitor->setCPUMonitorEnabled(true);
        resourceMonitor->setMemoryMonitorEnabled(true);
        resourceMonitor->setDiskMonitorEnabled(true);
        resourceMonitor->setNetworkMonitorEnabled(true);
        
        // Start resource monitoring
        resourceMonitor->startMonitoring();
        
        NLM_LOG_INFO("Core performance monitoring setup complete");
    }

    // Simulation performance metrics
    void setupSimulationPerformanceMonitoring() {
        NLM_LOG_INFO("Setting up simulation performance monitoring");
        
        // Configure benchmark suite
        benchmarkSuite->setIterations(1000);
        benchmarkSuite->setTimeout(30.0); // 30 seconds per benchmark
        benchmarkSuite->enableWarmup(true);
        benchmarkSuite->setWarmupIterations(100);
        
        // Add default benchmarks
        benchmarkSuite->addDefaultBenchmarks();
        benchmarkSuite->addSimulationBenchmarks();
        benchmarkSuite->addMemoryBenchmarks();
        benchmarkSuite->addCPUBenchmarks();
        
        NLM_LOG_INFO("Simulation performance monitoring setup complete");
    }

    // Integration with NLM brain
    void integrateWithBrain(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Integrating performance monitoring with NLM brain");
        
        // Set up timers for key brain operations
        monitor->startTimer("brain_initialization");
        monitor->startTimer("simulation_step");
        monitor->startTimer("plasticity_computation");
        monitor->startTimer("memory_operations");
        monitor->startTimer("neuromodulation");
        monitor->startTimer("development_system");
        
        // Record brain status
        brain->logStatus();
        
        NLM_LOG_INFO("Brain integration setup complete");
    }

    // Run performance benchmarks
    void runPerformanceBenchmarks() {
        NLM_LOG_INFO("Running performance benchmarks");
        
        // Run all benchmarks
        auto results = benchmarkSuite->runAllBenchmarks();
        
        // Generate benchmark report
        benchmarkSuite->generateBenchmarkReport("benchmark_report.html");
        
        // Generate comparison report if we have multiple results
        if (results.size() > 1) {
            benchmarkSuite->generateComparisonReport(results, "benchmark_comparison.html");
        }
        
        NLM_LOG_INFO("Performance benchmarks completed");
    }

    // Profile simulation
    void profileSimulation(std::shared_ptr<Brain> brain, size_t steps) {
        NLM_LOG_INFO("Starting simulation profiling for " + std::to_string(steps) + " steps");
        
        // Start monitoring
        profiler->startProfiling();
        resourceMonitor->startMonitoring();
        
        // Record simulation start time
        auto simulationStart = std::chrono::high_resolution_clock::now();
        
        // Run simulation with profiling
        for (size_t step = 0; step < steps; ++step) {
            // Start step timer
            monitor->startTimer("simulation_step");
            monitor->startTimer("plasticity_computation");
            monitor->startTimer("memory_operations");
            monitor->startTimer("neuromodulation");
            monitor->startTimer("development_system");
            
            // Record events
            monitor->recordEvent("simulation_step_started");
            
            // Run brain step
            brain->step(step, step * 0.001);
            
            // Stop timers
            monitor->stopTimer("simulation_step");
            monitor->stopTimer("plasticity_computation");
            monitor->stopTimer("memory_operations");
            monitor->stopTimer("neuromodulation");
            monitor->stopTimer("development_system");
            
            // Record events
            monitor->recordEvent("simulation_step_completed");
            
            // Profile I/O
            monitor->trackRead(1024); // Simulate I/O
            monitor->trackWrite(512); // Simulate I/O
            
            // Profile CPU
            monitor->startCPUMeasurement();
            double cpuTime = 0.001; // Simulate CPU time
            monitor->stopCPUMeasurement();
            
            // Profile memory
            monitor->trackMemoryAllocation(1024);
            monitor->trackMemoryDeallocation(512);
            
            // Log progress
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + " completed, " +
                            "step time: " + std::to_string(monitor->getElapsedTime("simulation_step")) + "s");
            }
        }
        
        // Stop monitoring
        auto simulationEnd = std::chrono::high_resolution_clock::now();
        profiler->stopProfiling();
        resourceMonitor->stopMonitoring();
        
        double simulationDuration = std::chrono::duration<double>(simulationEnd - simulationStart).count();
        
        NLM_LOG_INFO("Simulation profiling completed in " + std::to_string(simulationDuration) + "s");
        
        // Generate performance report
        generatePerformanceReport();
    }

    // Generate performance report
    void generatePerformanceReport() {
        NLM_LOG_INFO("Generating comprehensive performance report");
        
        // Get performance data from monitor
        reportGenerator->setTitle("NLM Performance Report");
        reportGenerator->setDescription("Comprehensive performance analysis of NLM simulation");
        
        // Add performance metrics
        reportGenerator->addTimeMeasurement("brain_initialization", monitor->getElapsedTime("brain_initialization"));
        reportGenerator->addTimeMeasurement("simulation_step", monitor->getElapsedTime("simulation_step"));
        reportGenerator->addTimeMeasurement("plasticity_computation", monitor->getElapsedTime("plasticity_computation"));
        reportGenerator->addTimeMeasurement("memory_operations", monitor->getElapsedTime("memory_operations"));
        reportGenerator->addTimeMeasurement("neuromodulation", monitor->getElapsedTime("neuromodulation"));
        reportGenerator->addTimeMeasurement("development_system", monitor->getElapsedTime("development_system"));
        
        reportGenerator->addMemoryMeasurement("current", monitor->getCurrentMemoryUsage());
        reportGenerator->addMemoryMeasurement("peak", monitor->getPeakMemoryUsage());
        
        reportGenerator->addCPUMeasurement("total", monitor->getCPUUtilization());
        
        reportGenerator->addEventCount("simulation_step_started", monitor->getEventCount("simulation_step_started"));
        reportGenerator->addEventCount("simulation_step_completed", monitor->getEventCount("simulation_step_completed"));
        
        // Generate reports in multiple formats
        reportGenerator->generateTextReport("performance_report.txt");
        reportGenerator->generateJSONReport("performance_report.json");
        reportGenerator->generateHTMLReport("performance_report.html");
        reportGenerator->generateCSVReport("performance_report.csv");
        
        NLM_LOG_INFO("Performance report generated in multiple formats");
    }

    // Get performance statistics
    std::string getPerformanceStatistics() const {
        std::ostringstream oss;
        oss << "=== NLM Performance Integration Statistics ===" << std::endl;
        
        oss << "Performance Monitor:" << std::endl;
        oss << monitor->getPerformanceStatistics() << std::endl;
        
        oss << std::endl << "Resource Monitor:" << std::endl;
        oss << resourceMonitor->getResourceSummary() << std::endl;
        
        oss << std::endl << "Profiler:" << std::endl;
        oss << profiler->generatePerformanceAnalysis() << std::endl;
        
        return oss.str();
    }

    // Run optimization
    void runOptimization() {
        NLM_LOG_INFO("Running performance optimization");
        
        // Generate optimization report
        auto recommendations = reportGenerator->generateOptimizationRecommendations();
        
        NLM_LOG_INFO("Performance optimization recommendations:");
        for (const auto& recommendation : recommendations) {
            NLM_LOG_INFO("  - " + recommendation);
        }
        
        // Run monitor optimization
        monitor->optimizePerformance();
        
        NLM_LOG_INFO("Performance optimization complete");
    }

private:
    std::shared_ptr<Logger> logger;
    std::shared_ptr<ConsoleLogger> consoleLogger;
    
    std::shared_ptr<PerformanceMonitor> monitor;
    std::shared_ptr<Profiler> profiler;
    std::shared_ptr<BenchmarkSuite> benchmarkSuite;
    std::shared_ptr<PerformanceReportGenerator> reportGenerator;
    std::shared_ptr<ResourceMonitor> resourceMonitor;
    std::shared_ptr<Config> config;
};

// CLI integration for performance monitoring
void setupPerformanceCLI(CLIArgumentParser& parser) {
    NLM_LOG_INFO("Setting up CLI performance monitoring commands");
    
    // Add performance monitoring CLI options
    parser.addArgument("profile", "Enable performance profiling", false);
    parser.addArgument("profile-config", "Load performance profiling configuration", false);
    parser.addArgument("benchmark", "Run performance benchmarks", false);
    parser.addArgument("benchmark-suite", "Run benchmark suite", false);
    parser.addArgument("performance-report", "Generate performance report", false);
    parser.addArgument("performance-stats", "Show performance statistics", false);
    parser.addArgument("resource-monitor", "Enable resource monitoring", false);
    parser.addArgument("resource-report", "Generate resource usage report", false);
    parser.addArgument("performance-optimize", "Run performance optimization", false);
}

// Integration function for main.cpp
void integratePerformanceMonitoring(CLIArgumentParser& parser, std::shared_ptr<Brain> brain, 
                                  std::shared_ptr<Config> config, SimulationController& simulationController,
                                  DebugMonitor& debugMonitor) {
    // Create performance integration
    auto performanceIntegration = std::make_shared<PerformanceIntegration>();
    
    // Setup core monitoring
    performanceIntegration->setupCoreMonitoring();
    
    // Setup simulation performance monitoring
    performanceIntegration->setupSimulationPerformanceMonitoring();
    
    // Integrate with brain
    performanceIntegration->integrateWithBrain(brain);
    
    // Process performance CLI commands
    if (parser.hasValue("profile")) {
        NLM_LOG_INFO("Performance profiling enabled via CLI");
        debugMonitor.enableProfiling(true);
        
        // Start profiling
        performanceIntegration->profileSimulation(brain, 100); // Profile 100 steps
    }
    
    if (parser.hasValue("benchmark")) {
        NLM_LOG_INFO("Running performance benchmarks via CLI");
        performanceIntegration->runPerformanceBenchmarks();
    }
    
    if (parser.hasValue("performance-report")) {
        NLM_LOG_INFO("Generating performance report via CLI");
        performanceIntegration->generatePerformanceReport();
    }
    
    if (parser.hasValue("performance-stats")) {
        NLM_LOG_INFO("Displaying performance statistics via CLI");
        std::cout << performanceIntegration->getPerformanceStatistics() << std::endl;
    }
    
    if (parser.hasValue("resource-monitor")) {
        NLM_LOG_INFO("Resource monitoring enabled via CLI");
        performanceIntegration->getResourceMonitor()->startMonitoring();
    }
    
    if (parser.hasValue("resource-report")) {
        NLM_LOG_INFO("Generating resource report via CLI");
        performanceIntegration->getResourceMonitor()->generateResourceReport("resource_report.html");
    }
    
    if (parser.hasValue("performance-optimize")) {
        NLM_LOG_INFO("Running performance optimization via CLI");
        performanceIntegration->runOptimization();
    }
}

} // namespace nlm
