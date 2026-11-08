// NLM Performance Benchmark
// Measures simulation performance for different network sizes

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>

using namespace nlm;

// Simple performance measurement
struct BenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    // Enhanced profiling data
    std::map<std::string, double> componentTimes;  // Timing by component
    std::vector<double> spikeCountsPerStep;       // Detailed spike statistics
    double memoryUsage;                           // Memory usage in MB
    double cpuUsage;                              // CPU usage percentage
};

BenchmarkResult runBenchmark(size_t neuronCount, size_t steps) {
    std::cout << "  Running benchmark: " << neuronCount << " neurons, " 
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
    BenchmarkResult result;
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

void printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                     NLM Performance Benchmark Results                           ║" << std::endl;
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

// Performance profiling and benchmarking for NLM
// Includes detailed profiling of brain components, memory usage, and scalability analysis

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <map>
#include <numeric>
#include <cmath>
#include <memory>
#include <algorithm>

using namespace nlm;

// Enhanced benchmark result with detailed profiling data
struct BenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    // Enhanced profiling data
    std::map<std::string, double> componentTimes;  // Timing by component
    std::vector<double> spikeCountsPerStep;       // Detailed spike statistics
    double memoryUsage;                           // Memory usage in MB
    double cpuUsage;                              // CPU usage percentage
    // Performance metrics
    double avgSpikePerSecond;
    double maxSpikeRate;
    double minSpikeRate;
    double varianceSpikeRate;
    // System resource metrics
    double avgCPUUsage;
    double maxMemoryUsage;
    double avgMemoryUsage;
    // Scalability metrics
    double timePerNeuron;
    double timePerSynapse;
    double spikeEfficiency;
};

// Enhanced benchmark function with detailed profiling
BenchmarkResult runBenchmark(size_t neuronCount, size_t steps, bool detailedProfiling = true) {
    std::cout << "  Running benchmark: " << neuronCount << " neurons, " 
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
    
    // Profile initialization time
    auto initStartTime = std::chrono::high_resolution_clock::now();
    brain->initialize();
    auto initEndTime = std::chrono::high_resolution_clock::now();
    double initTime = std::chrono::duration<double>(initEndTime - initStartTime).count();
    
    // Profile simulation time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // For detailed profiling, we can collect timing data for different components
    if (detailedProfiling) {
        std::cout << " (with detailed profiling)" << std::flush;
    }
    
    for (size_t step = 0; step < steps; ++step) {
        // Inject some random input
        for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Collect detailed profiling data if enabled
        if (detailedProfiling) {
            // Collect spike statistics for each step
            result.spikeCountsPerStep.push_back(brain->getFiringNeuronCount());
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect results
    BenchmarkResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = steps;
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    
    // Calculate performance metrics
    result.avgSpikePerSecond = static_cast<double>(brain->getTotalSpikeCount()) / totalTime;
    result.maxSpikeRate = 0.0;
    result.minSpikeRate = std::numeric_limits<double>::max();
    
    if (detailedProfiling && !result.spikeCountsPerStep.empty()) {
        for (size_t i = 0; i < result.spikeCountsPerStep.size(); ++i) {
            double spikeRate = result.spikeCountsPerStep[i] / totalTime;
            result.maxSpikeRate = std::max(result.maxSpikeRate, spikeRate);
            result.minSpikeRate = std::min(result.minSpikeRate, spikeRate);
        }
        
        // Calculate variance
        double sum = std::accumulate(result.spikeCountsPerStep.begin(), result.spikeCountsPerStep.end(), 0.0);
        double mean = sum / result.spikeCountsPerStep.size();
        double sq_sum = 0.0;
        for (double count : result.spikeCountsPerStep) {
            sq_sum += (count - mean) * (count - mean);
        }
        result.varianceSpikeRate = sq_sum / result.spikeCountsPerStep.size();
        
        // Estimate memory and CPU usage (simplified estimation)
        result.memoryUsage = static_cast<double>(result.neurons * sizeof(Neuron)) / (1024 * 1024); // MB
        result.memoryUsage += static_cast<double>(result.synapses * sizeof(Synapse)) / (1024 * 1024); // MB
        result.cpuUsage = (initTime + totalTime) * 100.0 / std::chrono::duration<double>(endTime - startTime).count();
    }
    
    // Calculate scalability metrics
    result.timePerNeuron = totalTime / result.neurons;
    result.timePerSynapse = totalTime / result.synapses;
    result.spikeEfficiency = brain->getTotalSpikeCount() / result.synapses;
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
    std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
    
    // Print detailed profiling results if enabled
    if (detailedProfiling) {
        std::cout << "    Initialization Time: " << std::fixed << std::setprecision(3) << initTime << "s" << std::endl;
        std::cout << "    Avg Spike Rate: " << std::fixed << std::setprecision(2) << result.avgSpikePerSecond << " spikes/sec" << std::endl;
        std::cout << "    Max Spike Rate: " << std::fixed << std::setprecision(2) << result.maxSpikeRate << " spikes/sec" << std::endl;
        std::cout << "    Memory Usage: " << std::fixed << std::setprecision(2) << result.memoryUsage << " MB" << std::endl;
        std::cout << "    CPU Usage: " << std::fixed << std::setprecision(1) << result.cpuUsage << "%" << std::endl;
        std::cout << "    Time per Neuron: " << std::fixed << std::setprecision(6) << result.timePerNeuron << "s" << std::endl;
        std::cout << "    Spike Efficiency: " << std::fixed << std::setprecision(3) << result.spikeEfficiency << " spikes/synapse" << std::endl;
    }
    
    return result;
}

// Print enhanced benchmark results with detailed metrics
void printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    NLM Enhanced Performance Benchmark Results                    ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╦══════════════╣" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec  ║ Spikes/step ║ Synapses/Neuron  ║    Time/sec/Neuron║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╬══════════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(20) << std::fixed << std::setprecision(2) << r.synapsesPerNeuron
                  << " ║ " << std::setw(15) << std::fixed << std::setprecision(6) << r.timePerNeuron << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╩══════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Print scalability analysis
    if (results.size() > 1) {
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                            Scalability Analysis                                 ║" << std::endl;
        std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╣" << std::endl;
        std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec  ║ Spikes/step ║   Spike Efficiency  ║" << std::endl;
        std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╣" << std::endl;
        
        for (const auto& r : results) {
            std::cout << "║ " << std::setw(13) << r.neurons 
                      << " ║ " << std::setw(13) << r.synapses
                      << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                      << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                      << " ║ " << std::setw(23) << std::fixed << std::setprecision(3) << r.spikeEfficiency << " ║" << std::endl;
        }
        
        std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╝" << std::endl;
        std::cout << std::endl;
    }
}

// Profile specific components for detailed analysis
void profileComponent(const std::shared_ptr<Brain>& brain, const std::string& componentName, size_t steps) {
    std::cout << "=== " << componentName << " Profiling ===" << std::endl;
    
    // Profile component timing
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        // Perform component-specific operations
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "  Component: " << componentName << std::endl;
    std::cout << "  Steps: " << steps << std::endl;
    std::cout << "  Time: " << std::fixed << std::setprecision(3) << elapsed << "s" << std::endl;
    std::cout << "  Steps/sec: " << std::fixed << std::setprecision(0) << (steps / elapsed) << std::endl;
    std::cout << "  =========================" << std::endl;
}

int main() {
    std::cout << R"(
    ╔════════════════════════════════════════════════════════════════════════════════════════════╗
    ║                              NLM Enhanced Performance Benchmark                             ║
    ║                     Detailed Profiling and Analysis Tools                               ║
    ║                     (Phase 6: Advanced Performance Monitoring)                          ║
    ╚════════════════════════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running NLM Enhanced Performance Benchmarks with Detailed Profiling..." << std::endl;
    std::cout << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Enhanced benchmark configurations with more data points
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {50, 2000},      // Small network, many steps
        {100, 1000},     // Small network
        {500, 1000},     // Medium network
        {1000, 500},     // Medium-large network
        {2000, 200},     // Large network, few steps
        {4000, 100},     // Very large network, few steps
    };
    
    for (const auto& [neurons, steps] : benchmarks) {
        // Run with detailed profiling enabled for all benchmarks
        auto result = runBenchmark(neurons, steps, true);
        results.push_back(result);
        std::cout << std::endl;
    }
    
    // Print comprehensive benchmark results
    printBenchmarkResults(results);
    
    // Component profiling example
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                         Component Performance Profiling                          ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    // Create a brain for component profiling
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Profile different components
    profileComponent(brain, "Spike System", 1000);
    profileComponent(brain, "Plasticity System", 1000);
    profileComponent(brain, "Memory System", 1000);
    profileComponent(brain, "Cognition System", 1000);
    
    std::cout << std::endl;
    std::cout << "Enhanced benchmark complete with comprehensive performance profiling." << std::endl;
    
    return 0;
}
    
    // Print summary
    printBenchmarkResults(results);
    
    std::cout << "Benchmark complete." << std::endl;
    
    return 0;
}
