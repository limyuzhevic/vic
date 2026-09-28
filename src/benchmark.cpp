// NLM Performance Benchmark
// Measures simulation performance for different network sizes
// Provides quantitative analysis of computational efficiency and scalability

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
// Contains comprehensive metrics for evaluating NLM simulation efficiency
struct BenchmarkResult {
    size_t neurons;                    // Total number of neurons in the network
    size_t synapses;                   // Total number of synaptic connections
    size_t steps;                      // Number of simulation steps executed
    double totalTime;                  // Total execution time in seconds
    double stepsPerSecond;             // Computational throughput
    double spikesPerStep;              // Average spike generation rate
    double synapsesPerNeuron;          // Average connectivity density
}; 

BenchmarkResult runBenchmark(size_t neuronCount, size_t steps) {
    // Validate input parameters to prevent invalid benchmarks
    if (neuronCount == 0) {
        throw std::invalid_argument("Neuron count must be greater than 0");
    }
    if (steps == 0) {
        throw std::invalid_argument("Step count must be greater than 0");
    }
    
    std::cout << "  Running benchmark: " << neuronCount << " neurons, " 
              << steps << " steps..." << std::flush;
    
    // Create configuration for benchmark
    auto config = std::make_shared<Config>();
    
    // Configure benchmark-specific parameters
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    
    // Create and initialize brain for benchmark
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain for benchmark");
    }
    
    // Record start time for performance measurement
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run benchmark simulation with controlled input patterns
    for (size_t step = 0; step < steps; ++step) {
        // Inject controlled input to maintain consistent activity levels
        for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        // Execute single simulation step
        brain->step(step, step * 0.001);
    }
    
    // Record end time and calculate performance metrics
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Calculate comprehensive performance statistics
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
    // Display benchmark results in a formatted, readable table
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

int main() {
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
    
    // Initialize logger for performance benchmarking
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running NLM Performance Benchmarks..." << std::endl;
    std::cout << std::endl;
    
    // Define benchmark configurations for scalability testing
    // Tests performance across different network sizes
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 1000},      // Small network, many steps
        {500, 1000},      // Medium network, moderate steps
        {1000, 500},      // Large network, fewer steps
        {2000, 200},      // Very large network, minimal steps
    };
    
    std::vector<BenchmarkResult> results;
    
    // Execute all benchmark configurations
    for (const auto& [neurons, steps] : benchmarks) {
        try {
            auto result = runBenchmark(neurons, steps);
            results.push_back(result);
            std::cout << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Benchmark failed for " << neurons << " neurons: " << e.what() << std::endl;
            // Continue with next benchmark even if one fails
        }
    }
    
    // Display comprehensive benchmark summary
    printBenchmarkResults(results);
    
    std::cout << "Benchmark complete." << std::endl;
    
    return 0;
}
