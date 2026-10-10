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
};

// Enhanced performance benchmark with comprehensive metrics
struct EnhancedBenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;        // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    double memoryUsage;      // MB
    double electricityCost;  // Estimated power consumption
    std::string scalabilityScore;  // A/B/C based on performance
    std::vector<double> detailedMetrics;  // Additional performance metrics
};

// Advanced benchmark with more comprehensive measurements
EnhancedBenchmarkResult runEnhancedBenchmark(size_t neuronCount, size_t steps) {
    std::cout << "  Running enhanced benchmark: " << neuronCount << " neurons, " 
              << steps << " steps..." << std::flush;
    
    // Create configuration with enhanced settings
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation with detailed timing
    auto startTime = std::chrono::high_resolution_clock::now();
    auto startCpuTime = std::chrono::steady_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        // Variable input pattern for realistic simulation
        if (step % 10 == 0) {
            // Burst input (10% of time)
            for (size_t i = 0; i < std::min(size_t(15), neuronCount / 8); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
            }
        } else if (step % 3 == 0) {
            // Moderate input (30% of time)
            for (size_t i = 0; i < std::min(size_t(8), neuronCount / 12); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            }
        } else {
            // Low input (60% of time)
            for (size_t i = 0; i < std::min(size_t(5), neuronCount / 20); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            }
        }
        
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect comprehensive results
    EnhancedBenchmarkResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = steps;
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    
    // Memory usage (approximate - based on neuron count)
    // Each neuron ~ 1KB, each synapse ~ 100 bytes
    result.memoryUsage = (neuronCount * 1.0 + result.synapses * 0.1) / 1024.0;  // MB
    
    // Estimate power consumption (rough approximation based on operations)
    // Each neuron step ~ 100 operations, each synapse operation ~ 50 operations
    double operations = (neuronCount * steps * 100) + (result.synapses * steps * 50);
    result.electricityCost = operations * 1e-9;  // kWh approximation
    
    // Calculate scalability score
    if (result.stepsPerSecond > 10000) {
        result.scalabilityScore = "A+ (Excellent)";
    } else if (result.stepsPerSecond > 5000) {
        result.scalabilityScore = "A (Good)";
    } else if (result.stepsPerSecond > 1000) {
        result.scalabilityScore = "B (Acceptable)";
    } else if (result.stepsPerSecond > 100) {
        result.scalabilityScore = "C (Poor)";
    } else {
        result.scalabilityScore = "F (Very Poor)";
    }
    
    // Additional detailed metrics
    result.detailedMetrics = {
        brain->getFiringNeuronCount(),  // Current firing rate
        brain->getActiveNeuronCount(),   // Active neurons
        brain->getPendingSpikeEventCount(), // Pending spike events
        brain->getExcitationInhibitionRatio() // E/I ratio
    };
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
    std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
    std::cout << "    Memory usage: " << std::fixed << std::setprecision(2) << result.memoryUsage << " MB" << std::endl;
    std::cout << "    Power: " << std::fixed << std::setprecision(4) << result.electricityCost << " kWh" << std::endl;
    std::cout << "    Scalability: " << result.scalabilityScore << std::endl;
    
    return result;
}

void printEnhancedBenchmarkResults(const std::vector<EnhancedBenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                      NLM Enhanced Performance Benchmark Results                          ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦════════════╦════════════╦════════════════════╗" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec  ║ Spikes/step ║ Memory(MB) ║ Power(kWh) ║ Scalability     ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬════════════╬════════════╬════════════════════╗" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.memoryUsage
                  << " ║ " << std::setw(10) << std::fixed << std::setprecision(4) << r.electricityCost
                  << " ║ " << std::setw(16) << r.scalabilityScore
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩════════════╩════════════╩════════════════════╝" << std::endl;
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
    bool useEnhanced = true;
    std::vector<std::pair<size_t, size_t>> benchmarks;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--basic") {
            useEnhanced = false;
        } else if (arg == "--small") {
            benchmarks = {{100, 1000}, {200, 2000}};
        } else if (arg == "--medium") {
            benchmarks = {{500, 1000}, {1000, 500}, {2000, 200}};
        } else if (arg == "--large") {
            benchmarks = {{2000, 200}, {5000, 100}, {10000, 50}};
        } else if (arg.substr(0, 12) == "--neurons=") {
            // Custom neuron count
            size_t neurons = std::stoul(arg.substr(12));
            benchmarks.push_back({neurons, 500});
        } else if (arg.substr(0, 9) == "--steps=") {
            // Custom steps
            size_t steps = std::stoul(arg.substr(9));
            if (!benchmarks.empty()) {
                benchmarks.back().second = steps;
            }
        } else if (arg == "--help") {
            std::cout << "Enhanced NLM Benchmark Options:" << std::endl;
            std::cout << "  --basic              Use basic benchmark mode (legacy)" << std::endl;
            std::cout << "  --small              Small scale benchmarks (200-2000 neurons)" << std::endl;
            std::cout << "  --medium             Medium scale benchmarks (500-2000 neurons)" << std::endl;
            std::cout << "  --large              Large scale benchmarks (2000-10000 neurons)" << std::endl;
            std::cout << "  --neurons=<num>      Specify neuron count" << std::endl;
            std::cout << "  --steps=<num>        Specify simulation steps" << std::endl;
            std::cout << "  --help               Show this help message" << std::endl;
            return 0;
        }
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running NLM Enhanced Performance Benchmarks..." << std::endl;
    std::cout << "Benchmark mode: " << (useEnhanced ? "Enhanced" : "Basic") << std::endl;
    std::cout << std::endl;
    
    std::vector<EnhancedBenchmarkResult> results;
    
    // Use default benchmarks if none specified
    if (benchmarks.empty()) {
        benchmarks = {
            {100, 1000},      // 100 neurons, 1000 steps
            {500, 1000},      // 500 neurons, 1000 steps
            {1000, 500},      // 1000 neurons, 500 steps
            {2000, 200},      // 2000 neurons, 200 steps
        };
    }
    
    for (const auto& [neurons, steps] : benchmarks) {
        if (useEnhanced) {
            auto result = runEnhancedBenchmark(neurons, steps);
            results.push_back(result);
        } else {
            auto result = runBenchmark(neurons, steps);
            results.push_back(reinterpret_cast<EnhancedBenchmarkResult&>(result));
        }
        std::cout << std::endl;
    }
    
    // Print summary
    if (useEnhanced) {
        printEnhancedBenchmarkResults(results);
    } else {
        printBenchmarkResults(results);
    }
    
    std::cout << "Benchmark complete." << std::endl;
    
    return 0;
}
