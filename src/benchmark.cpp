// NLM Performance Optimization and Profiling Integration
// This file integrates the new profiling infrastructure with the existing benchmark

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "performance/TimeProfiler.hpp"
#include "performance/MemoryProfiler.hpp"
#include "performance/SpikeProfiler.hpp"
#include "performance/ConnectivityProfiler.hpp"

#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>

using namespace nlm;

// Global profiling instances for integration
static TimeProfiler timeProfiler;
static MemoryProfiler memoryProfiler;
static SpikeProfiler spikeProfiler;
static ConnectivityProfiler connectivityProfiler;

// Benchmark configuration with profiling support
struct EnhancedBenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;           // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    double avgStepTimeUs;       // microseconds
    size_t peakMemoryKB;
    double totalMemoryAllocatedMB;
    std::string stepDetails;
};

// Thread-safe benchmark execution
class EnhancedBenchmark {
public:
    EnhancedBenchmark() {
        // Initialize global profiling instances
        timeProfiler.setEnabled(true);
        memoryProfiler.setEnabled(true);
        spikeProfiler.setEnabled(true);
        connectivityProfiler.setEnabled(true);
    }
    
    EnhancedBenchmarkResult runBenchmark(size_t neuronCount, size_t steps) {
        std::cout << "  Running enhanced benchmark: " << neuronCount << " neurons, " 
                  << steps << " steps..." << std::flush;
        
        // Start timing
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        
        // Profile brain initialization
        timeProfiler.startProfile("brain_initialization");
        auto brain = std::make_shared<Brain>(config);
        timeProfiler.stopProfile("brain_initialization");
        
        timeProfiler.startProfile("brain_setup");
        brain->initialize();
        timeProfiler.stopProfile("brain_setup");
        
        // Profile connectivity setup
        timeProfiler.startProfile("connectivity_setup");
        // Record connectivity stats
        connectivityProfiler.recordConnectivity("connectivity_setup",
                                                brain->getTotalNeuronCount(),
                                                brain->getTotalSynapseCount(),
                                                0.1f, 0.01f, 10, 100);
        timeProfiler.stopProfile("connectivity_setup");
        
        // Run simulation with profiling
        for (size_t step = 0; step < steps; ++step) {
            timeProfiler.startProfile("step_execution");
            
            // Profile memory operations
            memoryProfiler.recordAllocation("memory_operations", 1024, 1);
            
            // Profile spike processing
            spikeProfiler.recordSpikeActivity("spike_processing", 
                                            brain->getTotalSpikeCount(), 
                                            brain->getTotalSpikeCount(), 
                                            std::chrono::microseconds(100));
            
            // Inject some random input
            for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            timeProfiler.stopProfile("step_execution");
            
            // Periodically profile memory usage
            if (step % 100 == 0) {
                size_t currentMemory = memoryProfiler.getCurrentMemoryUsage();
                size_t peakMemory = memoryProfiler.getPeakMemoryUsage();
            }
        }
        
        // Stop overall timing
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Collect results
        EnhancedBenchmarkResult result;
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        
        // Calculate average step time
        double avgStepTimeUs = (totalTime * 1000000.0) / steps;
        result.avgStepTimeUs = avgStepTimeUs;
        
        // Get memory statistics
        result.peakMemoryKB = memoryProfiler.getPeakMemoryUsage() / 1024;
        result.totalMemoryAllocatedMB = memoryProfiler.getTotalMemoryAllocated() / (1024.0 * 1024.0);
        
        // Collect profiling summary
        std::stringstream details;
        details << "TimeProfiler: Total=" << timeProfiler.getTotalTime() << "μs, Calls=" << timeProfiler.getTotalCallCount();
        details << " | MemoryProfiler: Peak=" << result.peakMemoryKB << "KB, Allocs=" << memoryProfiler.getTotalAllocationCount();
        details << " | SpikeProfiler: Total=" << spikeProfiler.getTotalSpikeCount() << " spikes";
        result.stepDetails = details.str();
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
        std::cout << "    Avg Step Time: " << std::fixed << std::setprecision(2) << avgStepTimeUs << "μs" << std::endl;
        std::cout << "    Memory: " << result.peakMemoryKB << "KB peak" << std::endl;
        
        return result;
    }
    
    // Print comprehensive benchmark results with profiling
    void printEnhancedResults(const std::vector<EnhancedBenchmarkResult>& results) {
        std::cout << std::endl;
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    Enhanced NLM Performance Benchmark Results                          ║" << std::endl;
        std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦═══════════════════════╦════════════╦═══════════════════╣" << std::endl;
        std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec ║ Spikes/step ║ Synapses/Neuron      ║ Avg Step(μs)║ Peak Mem(KB)║" << std::endl;
        std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬═══════════════════════╬════════════╬═══════════════════╣" << std::endl;
        
        for (const auto& r : results) {
            std::cout << "║ " << std::setw(13) << r.neurons 
                      << " ║ " << std::setw(13) << r.synapses
                      << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                      << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                      << " ║ " << std::setw(23) << std::fixed << std::setprecision(2) << r.synapsesPerNeuron
                      << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.avgStepTimeUs
                      << " ║ " << std::setw(11) << r.peakMemoryKB
                      << " ║" << std::endl;
        }
        
        std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩═══════════════════════╩════════════╩═══════════════════╝" << std::endl;
        std::cout << std::endl;
    }
    
    // Print profiling summaries
    void printProfilingSummaries() {
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                        Performance Profiling Summaries                              ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        
        std::cout << "Time Profiler Summary:" << std::endl;
        timeProfiler.printSummary();
        
        std::cout << "\nMemory Profiler Summary:" << std::endl;
        memoryProfiler.printSummary();
        
        std::cout << "\nSpike Profiler Summary:" << std::endl;
        spikeProfiler.printSummary();
        
        std::cout << "\nConnectivity Profiler Summary:" << std::endl;
        connectivityProfiler.printSummary();
        
        std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    }
    
    // Reset all profiling data
    void resetProfiling() {
        timeProfiler.clear();
        memoryProfiler.clear();
        spikeProfiler.clear();
        connectivityProfiler.clear();
    }
};

int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Performance Benchmark with Profiling             ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running Enhanced NLM Performance Benchmarks with Profiling..." << std::endl;
    std::cout << std::endl;
    
    EnhancedBenchmark benchmark;
    std::vector<EnhancedBenchmarkResult> results;
    
    // Benchmark configurations with profiling: (neuron_count, steps)
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 1000},      // 100 neurons, 1000 steps
        {500, 1000},      // 500 neurons, 1000 steps
        {1000, 500},      // 1000 neurons, 500 steps
        {2000, 200},      // 2000 neurons, 200 steps
    };
    
    for (const auto& [neurons, steps] : benchmarks) {
        // Reset profiling between benchmarks for clean measurements
        benchmark.resetProfiling();
        
        auto result = benchmark.runBenchmark(neurons, steps);
        results.push_back(result);
        
        std::cout << std::endl;
    }
    
    // Print enhanced results
    benchmark.printEnhancedResults(results);
    
    // Print profiling summaries
    benchmark.printProfilingSummaries();
    
    std::cout << "Enhanced benchmark complete." << std::endl;
    
    return 0;
}
