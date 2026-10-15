// NLM Performance Benchmark Suite
// Enhanced comprehensive benchmarking system with multiple suites,
// statistical analysis, performance monitoring, and memory tracking.
// Maintains backward compatibility with original benchmark.cpp.

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

using namespace nlm;

// Helper function to create benchmark configuration from file
BenchmarkConfig loadBenchmarkConfig(const std::string& configFile = "configs/benchmark.cfg") {
    BenchmarkConfig config;
    
    // Default benchmark scenarios
    config.benchmarkType = BenchmarkConfig::BenchmarkType::PERFORMANCE;
    config.targetStepsPerSecond = 10000.0;
    config.targetMemoryMB = 512.0;
    config.targetEfficiency = 0.8;
    
    // Load from file if it exists
    std::ifstream file(configFile);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            // Simple parsing of key=value format
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                
                // Parse based on key
                if (key == "benchmark_type") {
                    if (value == "performance") config.benchmarkType = BenchmarkConfig::BenchmarkType::PERFORMANCE;
                    else if (value == "memory") config.benchmarkType = BenchmarkConfig::BenchmarkType::MEMORY;
                    else if (value == "accuracy") config.benchmarkType = BenchmarkConfig::BenchmarkType::ACCURACY;
                    else if (value == "scalability") config.benchmarkType = BenchmarkConfig::BenchmarkType::SCALABILITY;
                } else if (key == "target_steps_per_second") {
                    config.targetStepsPerSecond = std::stod(value);
                } else if (key == "target_memory_mb") {
                    config.targetMemoryMB = std::stod(value);
                } else if (key == "warmup_steps") {
                    config.warmupSteps = static_cast<size_t>(std::stol(value));
                } else if (key == "measurement_steps") {
                    config.measurementSteps = static_cast<size_t>(std::stol(value));
                } else if (key == "num_threads") {
                    config.numThreads = static_cast<int>(std::stol(value));
                }
            }
        }
        file.close();
    }
    
    return config;
}

// Original benchmark result structure (for backward compatibility)
struct OriginalBenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
};

// Convert enhanced benchmark result to original format
OriginalBenchmarkResult convertToOriginal(const BenchmarkResult& enhanced) {
    OriginalBenchmarkResult original;
    original.neurons = enhanced.neurons;
    original.synapses = enhanced.synapses;
    original.steps = enhanced.steps;
    original.totalTime = enhanced.totalTime;
    original.stepsPerSecond = enhanced.stepsPerSecond;
    original.spikesPerStep = enhanced.spikesPerStep;
    original.synapsesPerNeuron = enhanced.synapsesPerNeuron;
    return original;
}

// Enhanced benchmark run function
BenchmarkResult runEnhancedBenchmark(size_t neuronCount, size_t steps, BenchmarkConfig config) {
    // Create configuration
    auto brainConfig = std::make_shared<Config>();
    brainConfig->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    brainConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    brainConfig->set("connection_probability", 0.1f, ConfigSource::Default);
    brainConfig->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    brainConfig->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    brainConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create brain
    auto brain = std::make_shared<Brain>(brainConfig);
    brain->initialize();
    
    // Initialize performance monitor
    PerformanceMonitor monitor;
    monitor.setBrain(brain);
    monitor.configure(
        config.monitorMemory,
        config.monitorCpu, 
        config.monitorEvents,
        config.monitorFragmentation
    );
    
    // Initialize memory tracker
    MemoryTracker memoryTracker;
    
    // Start monitoring
    monitor.start();
    
    // Record start time
    auto startTime = std::chrono::high_resolution_clock::now();
    double startWallTime = std::chrono::duration<double>(startTime.time_since_epoch()).count();
    
    // Warmup phase
    for (size_t step = 0; step < config.warmupSteps; ++step) {
        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        brain->step(step, step * 0.001);
    }
    
    // Measurement phase
    for (size_t step = config.warmupSteps; step < config.warmupSteps + config.measurementSteps; ++step) {
        // Inject some random input
        for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
    }
    
    // Stop monitoring
    monitor.stop();
    
    // Record end time
    auto endTime = std::chrono::high_resolution_clock::now();
    double endWallTime = std::chrono::duration<double>(endTime.time_since_epoch()).count();
    
    // Collect metrics
    double totalTime = endWallTime - startWallTime;
    
    // Get brain statistics
    size_t totalSpikes = brain->getTotalSpikeCount();
    size_t activeNeurons = brain->getActiveNeuronCount();
    size_t peakActiveNeurons = brain->getFiringNeuronCount();
    double averageFiringRate = brain->getAverageFiringRate();
    
    // Get memory stats
    size_t memoryPoolAllocated = 0, memoryPoolAvailable = 0;
    // TODO: Get actual memory pool stats from brain
    
    // Get event queue stats
    size_t eventQueueSize = 0;
    
    // Create enhanced result
    BenchmarkResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = config.measurementSteps;
    result.totalTime = totalTime;
    result.stepsPerSecond = static_cast<double>(config.measurementSteps) / totalTime;
    result.spikesPerStep = static_cast<double>(totalSpikes) / config.measurementSteps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    
    // Enhanced metrics
    result.cpuTime = totalTime; // Simplified - would need actual CPU time
    result.memoryUsage = memoryTracker.getTotalMemoryUsage() / (1024 * 1024); // Convert to MB
    result.cpuUsage = monitor.getMetric("cpu_usage"); // Would need actual CPU usage
    result.efficiency = StatisticalUtils::efficiencyScore(
        result.stepsPerSecond, config.targetStepsPerSecond);
    
    result.totalSpikes = totalSpikes;
    result.activeNeurons = activeNeurons;
    result.peakActiveNeurons = peakActiveNeurons;
    result.averageFiringRate = averageFiringRate;
    result.stdDeviationFiringRate = StatisticalUtils::standardDeviation(std::vector<double>()); // TODO
    
    result.memoryPoolAllocated = memoryPoolAllocated;
    result.memoryPoolAvailable = memoryPoolAvailable;
    result.eventQueueSize = eventQueueSize;
    result.memoryFragmentation = memoryTracker.analyzeFragmentation();
    
    result.minStepTime = 0.0; // TODO
    result.maxStepTime = 0.0; // TODO
    result.medianStepTime = 0.0; // TODO
    result.cvStepTime = 0.0; // TODO
    
    result.bandwidthUsage = 0.0; // TODO
    result.cacheMissRate = 0.0; // TODO
    
    result.startTime = startWallTime;
    result.endTime = endWallTime;
    
    result.numThreadsUsed = config.numThreads;
    result.parallelEfficiency = StatisticalUtils::parallelEfficiency(
        config.numThreads, totalTime, 0.0); // TODO sequential time
    
    return result;
}

// Run benchmark suite
std::vector<BenchmarkResult> runBenchmarkSuite(const std::vector<std::pair<size_t, size_t>>& scenarios, 
                                               BenchmarkConfig config) {
    std::vector<BenchmarkResult> results;
    
    std::cout << "Running NLM Enhanced Benchmark Suite..." << std::endl;
    std::cout << "Benchmark type: " << static_cast<int>(config.benchmarkType) << std::endl;
    std::cout << "Warmup steps: " << config.warmupSteps << std::endl;
    std::cout << "Measurement steps: " << config.measurementSteps << std::endl;
    std::cout << "Number of threads: " << config.numThreads << std::endl;
    std::cout << std::endl;
    
    for (const auto& scenario : scenarios) {
        size_t neurons = scenario.first;
        size_t steps = scenario.second;
        
        std::cout << "Running benchmark: " << neurons << " neurons, " 
                  << steps << " steps..." << std::flush;
        
        BenchmarkResult result = runEnhancedBenchmark(neurons, steps, config);
        
        std::cout << " Done!" << std::endl;
        std::cout << "  Time: " << std::fixed << std::setprecision(2) << result.totalTime << "s" << std::endl;
        std::cout << "  Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
        std::cout << "  Memory: " << std::fixed << std::setprecision(2) << result.memoryUsage << " MB" << std::endl;
        std::cout << "  Efficiency: " << std::fixed << std::setprecision(2) << (result.efficiency * 100) << "%" << std::endl;
        std::cout << std::endl;
        
        results.push_back(result);
    }
    
    return results;
}

// Print enhanced benchmark results
void printEnhancedBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        NLM Enhanced Performance Benchmark Results                           ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╦═══════════════╣" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec  ║ Memory (MB) ║ Efficiency (%)  ║  Throughput   ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╬═══════════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.memoryUsage
                  << " ║ " << std::setw(14) << std::fixed << std::setprecision(2) << (r.efficiency * 100)
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(0) << r.stepsPerSecond * r.memoryUsage
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╩═══════════════╝" << std::endl;
    std::cout << std::endl;
}

// Print statistical analysis
void printStatisticalAnalysis(const std::vector<BenchmarkResult>& results) {
    if (results.empty()) return;
    
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                       Statistical Analysis of Benchmark Results                             ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╣" << std::endl;
    std::cout << "║   Metric      ║   Mean        ║   Median    ║   Std Dev    ║   Coefficient of Var ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╣" << std::endl;
    
    // Collect metrics for analysis
    std::vector<double> stepsPerSec, memoryUsage, efficiency, throughput;
    
    for (const auto& r : results) {
        stepsPerSec.push_back(r.stepsPerSecond);
        memoryUsage.push_back(r.memoryUsage);
        efficiency.push_back(r.efficiency);
        throughput.push_back(r.stepsPerSecond * r.memoryUsage); // steps/sec per MB
    }
    
    // Steps per second analysis
    double stepsMean = StatisticalUtils::mean(stepsPerSec);
    double stepsMedian = StatisticalUtils::median(stepsPerSec);
    double stepsStdDev = StatisticalUtils::standardDeviation(stepsPerSec);
    double stepsCV = StatisticalUtils::coefficientOfVariation(stepsPerSec);
    
    std::cout << "║ Steps/sec     ║ " << std::setw(13) << std::fixed << std::setprecision(2) << stepsMean
              << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << stepsMedian
              << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << stepsStdDev
              << " ║ " << std::setw(14) << std::fixed << std::setprecision(4) << (stepsCV * 100)
              << " % ║" << std::endl;
    
    // Memory usage analysis
    double memMean = StatisticalUtils::mean(memoryUsage);
    double memMedian = StatisticalUtils::median(memoryUsage);
    double memStdDev = StatisticalUtils::standardDeviation(memoryUsage);
    double memCV = StatisticalUtils::coefficientOfVariation(memoryUsage);
    
    std::cout << "║ Memory (MB)   ║ " << std::setw(13) << std::fixed << std::setprecision(2) << memMean
              << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << memMedian
              << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << memStdDev
              << " ║ " << std::fixed << std::setprecision(4) << (memCV * 100)
              << " % ║" << std::endl;
    
    // Efficiency analysis
    double effMean = StatisticalUtils::mean(efficiency);
    double effMedian = StatisticalUtils::median(efficiency);
    double effStdDev = StatisticalUtils::standardDeviation(efficiency);
    double effCV = StatisticalUtils::coefficientOfVariation(efficiency);
    
    std::cout << "║ Efficiency     ║ " << std::setw(13) << std::fixed << std::setprecision(2) << (effMean * 100)
              << " % ║ " << std::setw(11) << std::fixed << std::setprecision(2) << (effMedian * 100)
              << " % ║ " << std::setw(11) << std::fixed << std::setprecision(2) << (effStdDev * 100)
              << " % ║ " << std::fixed << std::setprecision(4) << (effCV * 100)
              << " % ║" << std::endl;
    
    // Throughput analysis
    double thrMean = StatisticalUtils::mean(throughput);
    double thrMedian = StatisticalUtils::median(throughput);
    double thrStdDev = StatisticalUtils::standardDeviation(throughput);
    double thrCV = StatisticalUtils::coefficientOfVariation(throughput);
    
    std::cout << "║ Throughput    ║ " << std::setw(13) << std::fixed << std::setprecision(0) << thrMean
              << " ║ " << std::setw(11) << std::fixed << std::setprecision(0) << thrMedian
              << " ║ " << std::setw(11) << std::fixed << std::setprecision(0) << thrStdDev
              << " ║ " << std::fixed << std::setprecision(4) << (thrCV * 100)
              << " % ║" << std::endl;
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╝" << std::endl;
    std::cout << std::endl;
}

// Backward compatibility function - original benchmark.cpp behavior
OriginalBenchmarkResult runOriginalBenchmark(size_t neuronCount, size_t steps) {
    std::cout << "  Running original benchmark: " << neuronCount << " neurons, " 
              << steps << " steps..." << std::flush;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        // Inject some random input
        for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect results
    OriginalBenchmarkResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = steps;
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
    std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
    
    return result;
}

// Print original benchmark results
void printOriginalBenchmarkResults(const std::vector<OriginalBenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                     NLM Original Performance Benchmark Results                           ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╣" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec ║ Spikes/step ║ Synapses/Neuron      ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(20) << std::fixed << std::setprecision(2) << r.synapsesPerNeuron
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╝" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Performance Benchmark                           ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Parse command line arguments
    bool runEnhanced = true;
    bool runOriginal = false;
    bool help = false;
    bool examples = false;
    std::string configFile = "configs/benchmark.cfg";
    std::string outputFile = "benchmark_results.csv";
    
    std::cout << "Enhanced NLM Benchmark Suite" << std::endl;
    std::cout << "=============================" << std::endl;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            help = true;
        } else if (arg == "--original" || arg == "-o") {
            runEnhanced = false;
            runOriginal = true;
        } else if (arg == "--both" || arg == "-b") {
            runEnhanced = true;
            runOriginal = true;
        } else if (arg == "--help-examples" || arg == "-e") {
            examples = true;
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--quick" || arg == "-q") {
            runEnhanced = true;
            runOriginal = false;
        }
    }
    
    if (help) {
        std::cout << "Enhanced NLM Benchmark Suite" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h              Show this help message" << std::endl;
        std::cout << "  --examples, -e          Show benchmark examples" << std::endl;
        std::cout << "  --config <file>         Load benchmark configuration" << std::endl;
        std::cout << "  --output <file>         Export results to file" << std::endl;
        std::cout << "  --original, -o          Run only original benchmark" << std::endl;
        std::cout << "  --both, -b              Run both enhanced and original benchmarks" << std::endl;
        std::cout << "  --quick, -q             Quick benchmark (enhanced only)" << std::endl;
        std::cout << "  (default)               Run enhanced benchmark suite" << std::endl;
        return 0;
    }
    
    if (examples) {
        std::cout << "Benchmark Examples:" << std::endl;
        std::cout << "1. Standard benchmarks:" << std::endl;
        std::cout << "   ./nlm_benchmark --quick" << std::endl;
        std::cout << "2. Enhanced with monitoring:" << std::endl;
        std::cout << "   ./nlm_benchmark --config my_benchmark.cfg" << std::endl;
        std::cout << "3. Compare original vs enhanced:" << std::endl;
        std::cout << "   ./nlm_benchmark --both" << std::endl;
        return 0;
    }
    
    // Load benchmark configuration
    BenchmarkConfig config = loadBenchmarkConfig(configFile);
    
    // Display configuration
    std::cout << "Configuration: " << configFile << std::endl;
    std::cout << "Benchmark type: " << static_cast<int>(config.benchmarkType) << std::endl;
    std::cout << std::endl;
    
    // Define benchmark scenarios
    std::vector<std::pair<size_t, size_t>> scenarios = {
        {100, 1000},      // 100 neurons, 1000 steps
        {500, 1000},      // 500 neurons, 1000 steps
        {1000, 500},      // 1000 neurons, 500 steps
        {2000, 200},      // 2000 neurons, 200 steps
    };
    
    std::vector<BenchmarkResult> enhancedResults;
    std::vector<OriginalBenchmarkResult> originalResults;
    
    // Run benchmarks
    if (runEnhanced) {
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                           ENHANCED BENCHMARK RESULTS                                 ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        
        enhancedResults = runBenchmarkSuite(scenarios, config);
        
        printEnhancedBenchmarkResults(enhancedResults);
        
        if (config.computeStatistics) {
            printStatisticalAnalysis(enhancedResults);
        }
        
        if (config.exportResults) {
            std::cout << "Exporting enhanced results to: " << outputFile << std::endl;
            // TODO: Implement export functionality
        }
    }
    
    if (runOriginal) {
        std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                           ORIGINAL BENCHMARK RESULTS                                ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        
        for (const auto& scenario : scenarios) {
            OriginalBenchmarkResult result = runOriginalBenchmark(scenario.first, scenario.second);
            originalResults.push_back(result);
        }
        
        printOriginalBenchmarkResults(originalResults);
        
        if (config.exportResults) {
            std::cout << "Exporting original results to: " << outputFile << std::endl;
            // TODO: Implement export functionality
        }
    }
    
    if (!runEnhanced && !runOriginal) {
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                           ENHANCED BENCHMARK RESULTS                                 ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        
        enhancedResults = runBenchmarkSuite(scenarios, config);
        
        printEnhancedBenchmarkResults(enhancedResults);
        
        if (config.computeStatistics) {
            printStatisticalAnalysis(enhancedResults);
        }
        
        if (config.exportResults) {
            std::cout << "Exporting results to: " << outputFile << std::endl;
            // TODO: Implement export functionality
        }
    }
    
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                            BENCHMARK COMPLETE                                        ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    return 0;
}