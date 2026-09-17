// NLM Performance Benchmark
// Measures simulation performance for different network sizes
// Author: NLM Development Team
// Date: 2026

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
#include <stdexcept>

using namespace nlm;

// Simple performance measurement structure
// Contains key metrics for benchmarking neural network performance
struct BenchmarkResult {
    size_t neurons;              // Number of neurons in the network
    size_t synapses;             // Number of synapses in the network
    size_t steps;                // Number of simulation steps executed
    double totalTime;            // Total execution time in seconds
    double stepsPerSecond;       // Computational throughput (steps/second)
    double spikesPerStep;        // Average spike generation rate
    double synapsesPerNeuron;    // Connectivity density
};

BenchmarkResult runBenchmark(size_t neuronCount, size_t steps) {
    // Input validation to prevent division by zero and invalid parameters
    if (steps == 0) {
        throw std::invalid_argument("Number of steps must be greater than 0");
    }
    
    if (neuronCount == 0) {
        throw std::invalid_argument("Neuron count must be greater than 0");
    }
    
    std::cout << "  Running benchmark: " << neuronCount << " neurons, " 
              << steps << " steps..." << std::flush;
    
    // Create configuration with optimized settings for benchmarking
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation with timing
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Calculate input injection count based on network size
    // Scale input with network size to maintain consistent input density
    size_t inputNeurons = std::min<size_t>(10, neuronCount / 10);
    
    for (size_t step = 0; step < steps; ++step) {
        // Inject current into sensory neurons to maintain network activity
        for (size_t i = 0; i < inputNeurons; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        // Use consistent timestep for simulation
        brain->step(step, 0.001);  // Fixed timestep instead of step-dependent
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect results into BenchmarkResult structure
    BenchmarkResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = steps;
    result.totalTime = totalTime;
    
    // Calculate performance metrics
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    
    // Output benchmark results
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
    std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
    
    return result;
}

void printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    // Output formatted benchmark summary with improved readability
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
    
    // Print summary statistics
    if (!results.empty()) {
        std::cout << "Summary Statistics:" << std::endl;
        double totalTime = 0.0, totalSteps = 0.0, totalSpikes = 0.0;
        
        for (const auto& r : results) {
            totalTime += r.totalTime;
            totalSteps += r.steps;
            totalSpikes += r.spikesPerStep * r.steps;
        }
        
        std::cout << "  Average time per benchmark: " << std::fixed << std::setprecision(2) 
                  << (totalTime / results.size()) << "s" << std::endl;
        std::cout << "  Average steps per second: " << std::fixed << std::setprecision(0) 
                  << (totalSteps / totalTime) << std::endl;
        std::cout << "  Average spikes per step: " << std::fixed << std::setprecision(2) 
                  << (totalSpikes / totalSteps) << std::endl;
    }
}

// Main function for NLM performance benchmarking
int main() {
    // Display welcome banner
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Performance Benchmark                                     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger with warning level for cleaner benchmark output
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running NLM Performance Benchmarks..." << std::endl;
    std::cout << std::endl;
    
    // Vector to store all benchmark results for later analysis
    std::vector<BenchmarkResult> results;
    
    // Benchmark configuration: defines different network sizes and simulation steps
    // Each benchmark tests performance at different scales to measure scalability
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 1000},      // Small network: 100 neurons, 1000 steps
        {500, 1000},      // Medium network: 500 neurons, 1000 steps  
        {1000, 500},      // Large network: 1000 neurons, 500 steps
        {2000, 200},      // Very large network: 2000 neurons, 200 steps
    };
    
    // Execute each benchmark sequentially
    for (const auto& [neurons, steps] : benchmarks) {
        try {
            // Run benchmark with error handling
            auto result = runBenchmark(neurons, steps);
            results.push_back(result);
            std::cout << std::endl;
        } catch (const std::exception& e) {
            std::cout << std::endl;
            std::cout << "ERROR: Benchmark failed for " << neurons << " neurons, " 
                      << steps << " steps: " << e.what() << std::endl;
            std::cout << "Skipping this benchmark..." << std::endl;
            std::cout << std::endl;
        }
    }
    
    // Print detailed results with summary statistics
    printBenchmarkResults(results);
    
    // Final completion message
    std::cout << "Benchmark complete." << std::endl;
    
    return 0;
}
