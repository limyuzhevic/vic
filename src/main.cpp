// NLM — 熙然
// Neural Learning Machine - Enhanced Benchmark Demonstration
// 
// This demonstration showcases the enhanced benchmarking and monitoring system,
// integrating with the existing NLM infrastructure while providing comprehensive
// performance analysis, memory tracking, and visualization capabilities.

#include "benchmark/BenchmarkSuite.hpp"
#include "benchmark/config/BenchmarkConfig.hpp"
#include "benchmark/stats/StatisticalUtils.hpp"
#include "benchmark/monitor/PerformanceMonitor.hpp"
#include "benchmark/memory/MemoryTracker.hpp"
#include "benchmark/export/BenchmarkExporter.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <map>
#include <memory>
#include <filesystem>

using namespace nlm;

// Helper functions for demonstration
void printEnhancedBanner() {
    std::cout << R"(
    ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Performance Benchmark Suite                      ║
    ║                                                               ║
    ║     Demonstrating Advanced Benchmarking, Monitoring, and      ║
    ║     Visualization Capabilities                                ║
    ║                                                               ║
    ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printBenchmarkTypes() {
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                             BENCHMARK TYPES                           ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ 0. Performance    - Measures simulation speed and scalability  ║" << std::endl;
    std::cout << "║ 1. Memory         - Tracks memory usage and allocation patterns ║" << std::endl;
    std::cout << "║ 2. Accuracy       - Tests neural network accuracy and stability  ║" << std::endl;
    std::cout << "║ 3. Scalability    - Evaluates scaling with neuron count            ║" << std::endl;
    std::cout << "║ 4. Integration     - Comprehensive system integration testing   ║" << std::endl;
    std::cout << "║ 5. Regression     - Automated performance regression detection ║" << std::endl;
    std::cout << "║ 6. Comparison     - Configuration comparison benchmarks       ║" << std::endl;
    std::cout << "║ 7. Custom        - User-defined benchmark scenarios             ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
}

void printDemonstrationModes() {
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                         DEMONSTRATION MODES                          ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Quick              - Fast benchmarking with basic monitoring      ║" << std::endl;
    std::cout << "║ Comprehensive      - Full benchmarking with advanced analysis      ║" << std::endl;
    std::cout << "║ Configuration      - Custom benchmark scenarios via config files    ║" << std::endl;
    std::cout << "║ Comparison         - Multiple configurations comparison             ║" << std::endl;
    std::cout << "║ Integration        - System integration demonstration              ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
}

void showBenchmarkConfigurationHelp() {
    std::cout << "Benchmark Configuration Options:" << std::endl;
    std::cout << "  benchmark_type = 0                    # Benchmark type" << std::endl;
    std::cout << "  target_steps_per_second = 10000.0    # Target performance" << std::endl;
    std::cout << "  target_memory_mb = 512.0             # Target memory usage" << std::endl;
    std::cout << "  warmup_steps = 100                   # Warmup iterations" << std::endl;
    std::cout << "  measurement_steps = 1000             # Measurement iterations" << std::endl;
    std::cout << "  num_threads = 0                       # Number of threads (0 = auto)" << std::endl;
    std::cout << "  monitor_memory = true                 # Enable memory monitoring" << std::endl;
    std::cout << "  monitor_cpu = true                    # Enable CPU monitoring" << std::endl;
    std::cout << "  monitor_events = true                 # Monitor spike events" << std::endl;
    std::cout << "  monitor_fragmentation = true          # Monitor memory fragmentation" << std::endl;
    std::cout << "  export_results = false                # Enable results export" << std::endl;
    std::cout << "  export_path = results                 # Export directory" << std::endl;
    std::cout << "  export_format = csv                   # Export format (csv, json, both)" << std::endl;
    std::cout << "  compute_statistics = true             # Enable statistical analysis" << std::endl;
    std::cout << "  compute_regression = true             # Enable regression detection" << std::endl;
}

void demonstrateQuickBenchmark() {
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        QUICK BENCHMARK DEMONSTRATION                     ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    // Simple benchmark configuration
    BenchmarkConfig config;
    config.benchmarkType = BenchmarkConfig::BenchmarkType::PERFORMANCE;
    config.warmupSteps = 50;
    config.measurementSteps = 200;
    config.numThreads = 0;
    config.monitorMemory = false;
    config.monitorCpu = false;
    config.exportResults = false;
    config.computeStatistics = false;
    
    // Simple benchmark scenarios
    std::vector<std::pair<size_t, size_t>> scenarios = {
        {100, 500},      // 100 neurons, 500 steps
        {500, 500},      // 500 neurons, 500 steps
        {1000, 200},     // 1000 neurons, 200 steps
    };
    
    std::cout << "Running quick benchmarks..." << std::endl;
    
    for (size_t i = 0; i < scenarios.size(); ++i) {
        auto scenario = scenarios[i];
        std::cout << "  Benchmark " << i + 1 << ": " << scenario.first 
                  << " neurons, " << scenario.second << " steps" << std::endl;
        
        // Create configuration for this benchmark
        auto brainConfig = std::make_shared<Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(scenario.first), ConfigSource::Default);
        brainConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        brainConfig->set("connection_probability", 0.1f, ConfigSource::Default);
        brainConfig->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        brainConfig->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        brainConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(brainConfig);
        brain->initialize();
        
        // Run simulation
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < scenario.second; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        double stepsPerSecond = static_cast<double>(scenario.second) / totalTime;
        size_t totalSpikes = brain->getTotalSpikeCount();
        double spikesPerStep = static_cast<double>(totalSpikes) / scenario.second;
        
        std::cout << "    Time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << stepsPerSecond << std::endl;
        std::cout << "    Spikes/step: " << std::fixed << std::setprecision(3) << spikesPerStep << std::endl;
        std::cout << std::endl;
    }
}

void demonstrateComprehensiveBenchmark() {
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    COMPREHENSIVE BENCHMARK DEMONSTRATION                     ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    // Comprehensive benchmark configuration
    BenchmarkConfig config;
    config.benchmarkType = BenchmarkConfig::BenchmarkType::PERFORMANCE;
    config.warmupSteps = 100;
    config.measurementSteps = 1000;
    config.numThreads = 2;
    config.monitorMemory = true;
    config.monitorCpu = true;
    config.monitorEvents = true;
    config.monitorFragmentation = true;
    config.exportResults = true;
    config.exportPath = "results";
    config.exportFormat = "both";
    config.computeStatistics = true;
    config.computeRegression = true;
    
    // Comprehensive benchmark scenarios
    std::vector<std::pair<size_t, size_t>> scenarios = {
        {1000, 1000},     // 1K neurons, 1K steps
        {5000, 1000},     // 5K neurons, 1K steps
        {10000, 500},    // 10K neurons, 500 steps
    };
    
    std::cout << "Running comprehensive benchmarks with full monitoring..." << std::endl;
    std::cout << "Monitoring: Memory, CPU, Events, Fragmentation" << std::endl;
    std::cout << "Export: Both CSV and JSON formats" << std::endl;
    std::cout << "Statistics: Enabled with regression detection" << std::endl;
    std::cout << std::endl;
    
    for (size_t i = 0; i < scenarios.size(); ++i) {
        auto scenario = scenarios[i];
        std::cout << "Comprehensive Benchmark " << i + 1 << ": " << scenario.first 
                  << " neurons, " << scenario.second << " steps" << std::endl;
        
        // Create brain configuration
        auto brainConfig = std::make_shared<Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(scenario.first), ConfigSource::Default);
        brainConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        brainConfig->set("connection_probability", 0.15f, ConfigSource::Default);
        brainConfig->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        brainConfig->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        brainConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Create brain
        auto brain = std::make_shared<Brain>(brainConfig);
        brain->initialize();
        
        // Performance monitor
        PerformanceMonitor monitor;
        monitor.setBrain(brain);
        monitor.configure(
            config.monitorMemory,
            config.monitorCpu,
            config.monitorEvents,
            config.monitorFragmentation
        );
        
        // Memory tracker
        MemoryTracker memoryTracker;
        
        // Start monitoring
        monitor.start();
        
        // Record start time
        auto startTime = std::chrono::high_resolution_clock::now();
        double startWallTime = std::chrono::duration<double>(startTime.time_since_epoch()).count();
        
        // Warmup
        for (size_t step = 0; step < config.warmupSteps; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step, step * 0.001);
        }
        
        // Measurement
        for (size_t step = config.warmupSteps; step < config.warmupSteps + config.measurementSteps; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step, step * 0.001);
        }
        
        // Stop monitoring
        monitor.stop();
        
        // Record end time
        auto endTime = std::chrono::high_resolution_clock::now();
        double endWallTime = std::chrono::duration<double>(endTime.time_since_epoch()).count();
        
        // Calculate results
        double totalTime = endWallTime - startWallTime;
        size_t stepsPerSecond = static_cast<size_t>(config.measurementSteps / totalTime);
        size_t totalSpikes = brain->getTotalSpikeCount();
        double spikesPerStep = static_cast<double>(totalSpikes) / config.measurementSteps;
        size_t memoryUsage = memoryTracker.getTotalMemoryUsage();
        
        // Get monitoring metrics
        auto systemMetrics = monitor.getSystemMetrics();
        auto brainMetrics = monitor.getBrainMetrics();
        
        std::cout << "  Time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
        std::cout << "  Steps/sec: " << std::fixed << std::setprecision(0) << stepsPerSecond << std::endl;
        std::cout << "  Spikes/step: " << std::fixed << std::setprecision(3) << spikesPerStep << std::endl;
        std::cout << "  Memory: " << (memoryUsage / 1024) << " KB" << std::endl;
        std::cout << "  CPU Usage: " << std::fixed << std::setprecision(1) << systemMetrics["cpu_usage"] << "%" << std::endl;
        std::cout << "  Event Queue Size: " << brainMetrics["event_queue_size"] << std::endl;
        std::cout << "  Fragmentation: " << std::fixed << std::setprecision(4) << memoryTracker.analyzeFragmentation() << std::endl;
        std::cout << std::endl;
        
        // Check for memory leaks
        if (memoryTracker.detectMemoryLeak()) {
            std::cout << "  ⚠ Memory leak detected! " << memoryTracker.getTotalLeakedMemory() << " bytes leaked" << std::endl;
        }
    }
}

void demonstrateConfigurationBenchmark() {
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                  CONFIGURATION BENCHMARK DEMONSTRATION                  ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    std::cout << "Demonstrating configuration-based benchmarking..." << std::endl;
    std::cout << "This would run multiple benchmark scenarios with different configurations." << std::endl;
    
    // Example configurations
    std::vector<std::shared_ptr<Config>> configs;
    
    // Configuration 1: Conservative
    auto config1 = Config::createMinimalConfig();
    config1->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config1->set("connection_probability", 0.05f, ConfigSource::Default);
    configs.push_back(config1);
    
    // Configuration 2: Balanced
    auto config2 = Config::createDefaultConfig();
    config2->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
    config2->set("connection_probability", 0.15f, ConfigSource::Default);
    configs.push_back(config2);
    
    // Configuration 3: Aggressive
    auto config3 = Config::createExpertConfig();
    config3->set("neuron_count", static_cast<int64_t>(10000), ConfigSource::Default);
    config3->set("connection_probability", 0.25f, ConfigSource::Default);
    configs.push_back(config3);
    
    std::cout << "Configurations to compare:" << std::endl;
    for (size_t i = 0; i < configs.size(); ++i) {
        std::cout << "  " << i + 1 << ". " << "Neurons: " << configs[i]->getAsInt("neuron_count") 
                  << ", Connection probability: " << configs[i]->getAsDouble("connection_probability") << std::endl;
    }
    
    std::cout << "In a real implementation, this would run benchmarks with each configuration" << std::endl;
    std::cout << "and provide comparison results with statistical analysis." << std::endl;
}

void demonstrateSystemIntegration() {
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                 SYSTEM INTEGRATION DEMONSTRATION                      ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    std::cout << "Demonstrating integration with existing NLM systems..." << std::endl;
    std::cout << "This shows how the benchmarking system integrates with:" << std::endl;
    std::cout << "  - MemoryPool performance infrastructure" << std::endl;
    std::cout << "  - EventQueue monitoring capabilities" << std::endl;
    std::cout << "  - CheckpointSystem state tracking" << std::endl;
    std::cout << "  - Performance monitoring and analysis" << std::endl;
    
    // Demonstrate MemoryPool integration
    std::cout << std::endl;
    std::cout << "MemoryPool Integration:" << std::endl;
    std::cout << "  - Track memory pool allocation statistics" << std::endl;
    std::cout << "  - Monitor memory fragmentation" << std::endl;
    std::cout << "  - Detect memory leaks" << std::endl;
    
    // Demonstrate EventQueue integration
    std::cout << std::endl;
    std::cout << "EventQueue Integration:" << std::endl;
    std::cout << "  - Monitor spike event processing" << std::endl;
    std::cout << "  - Track queue size and overflow" << std::endl;
    std::cout << "  - Analyze event timing patterns" << std::endl;
    
    // Demonstrate CheckpointSystem integration
    std::cout << std::endl;
    std::cout << "CheckpointSystem Integration:" << std::endl;
    std::cout << "  - Monitor checkpoint performance" << std::endl;
    std::cout << "  - Track save/load times" << std::endl;
    std::cout << "  - Analyze checkpoint sizes" << std::endl;
    
    // Demonstrate Performance Monitoring integration
    std::cout << std::endl;
    std::cout << "Performance Monitoring Integration:" << std::endl;
    std::cout << "  - Real-time system resource monitoring" << std::endl;
    std::cout << "  - CPU and memory usage tracking" << std::endl;
    std::cout << "  - Brain state monitoring (neurons, spikes, plasticity)" << std::endl;
}

int main(int argc, char** argv) {
    printEnhancedBanner();
    
    // Parse command line arguments
    std::string mode = "quick"; // quick, comprehensive, config, integration
    bool help = false;
    bool examples = false;
    std::string configFile = "configs/benchmark.cfg";
    std::string outputFile = "benchmark_results.csv";
    
    std::cout << "Enhanced NLM Benchmark Suite" << std::endl;
    std::cout << "=============================" << std::endl;
    printBenchmarkTypes();
    printDemonstrationModes();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            help = true;
        } else if (arg == "--examples" || arg == "-e") {
            examples = true;
        } else if (arg == "--quick" || arg == "-q") {
            mode = "quick";
        } else if (arg == "--comprehensive" || arg == "-c") {
            mode = "comprehensive";
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--mode" && i + 1 < argc) {
            mode = argv[++i];
        }
    }
    
    if (help) {
        std::cout << "Enhanced NLM Benchmark Suite" << std::endl;
        std::cout << "=============================" << std::endl;
        std::cout << "Demonstrates the enhanced benchmarking and monitoring system." << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h              Show this help message" << std::endl;
        std::cout << "  --examples, -e          Show demonstration examples" << std::endl;
        std::cout << "  --quick, -q             Quick benchmark demonstration" << std::endl;
        std::cout << "  --comprehensive, -c     Comprehensive benchmark demonstration" << std::endl;
        std::cout << "  --config <file>         Load configuration file" << std::endl;
        std::cout << "  --output <file>         Output file for results" << std::endl;
        std::cout << "  --mode <mode>           Demonstration mode (quick, comprehensive, config, integration)" << std::endl;
        std::cout << std::endl;
        std::cout << "Benchmark Types:" << std::endl;
        printBenchmarkTypes();
        std::cout << std::endl;
        std::cout << "Demonstration Modes:" << std::endl;
        printDemonstrationModes();
        std::cout << std::endl;
        showBenchmarkConfigurationHelp();
        return 0;
    }
    
    if (examples) {
        std::cout << "Benchmark Demonstration Examples:" << std::endl;
        std::cout << "1. Quick benchmark demonstration:" << std::endl;
        std::cout << "   ./nlm_benchmark_demonstration --quick" << std::endl;
        std::cout << std::endl;
        std::cout << "2. Comprehensive benchmark demonstration:" << std::endl;
        std::cout << "   ./nlm_benchmark_demonstration --comprehensive" << std::endl;
        std::cout << std::endl;
        std::cout << "3. Configuration-based benchmarking:" << std::endl;
        std::cout << "   ./nlm_benchmark_demonstration --mode config" << std::endl;
        std::cout << std::endl;
        std::cout << "4. System integration demonstration:" << std::endl;
        std::cout << "   ./nlm_benchmark_demonstration --mode integration" << std::endl;
        std::cout << std::endl;
        showBenchmarkConfigurationHelp();
        return 0;
    }
    
    // Run demonstration based on mode
    if (mode == "quick") {
        demonstrateQuickBenchmark();
    } else if (mode == "comprehensive") {
        demonstrateComprehensiveBenchmark();
    } else if (mode == "config") {
        demonstrateConfigurationBenchmark();
    } else if (mode == "integration") {
        demonstrateSystemIntegration();
    } else {
        std::cout << "Unknown mode: " << mode << std::endl;
        std::cout << "Using default (quick) mode." << std::endl;
        demonstrateQuickBenchmark();
    }
    
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                             DEMONSTRATION COMPLETE                                       ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    return 0;
}