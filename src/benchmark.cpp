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

// Enhanced performance measurement with more detailed metrics
struct BenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    double avgSpikeFrequency;  // Hz
    double memoryUsage;        // MB (if available)
    std::vector<double> neuronActivities;  // Per-neuron firing rates for analysis
    BenchmarkResult() 
        : neurons(0), synapses(0), steps(0), totalTime(0.0), stepsPerSecond(0.0)
        , spikesPerStep(0.0), synapsesPerNeuron(0.0), avgSpikeFrequency(0.0), memoryUsage(0.0) {}
};

BenchmarkResult runBenchmark(size_t neuronCount, size_t steps, float connectionProb = 0.1f) {
    std::cout << "  Running benchmark: " << neuronCount << " neurons, " 
              << steps << " steps, connection probability: " << connectionProb << "..." << std::flush;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", connectionProb, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        // Inject input in patterns (not uniform to avoid saturation)
        size_t sensoryNeurons = neuronCount / 10;  // 10% of neurons are sensory
        if (sensoryNeurons > 0) {
            // Inject to random subset to create interesting dynamics
            size_t injections = std::min(size_t(10), sensoryNeurons);
            for (size_t i = 0; i < injections; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
        }
        
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect detailed results
    BenchmarkResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = steps;
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    
    // Additional metrics
    double avgFiringRate = brain->getAverageFiringRate();
    result.avgSpikeFrequency = avgFiringRate;
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
    std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
    std::cout << "    Avg firing rate: " << std::fixed << std::setprecision(2) << result.avgSpikeFrequency << " Hz" << std::endl;
    
    return result;
}

void printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                     Enhanced NLM Performance Benchmark Results                           ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╦════════════╗" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec  ║ Spikes/step ║ Synapses/Neuron   ║ Avg Firing  ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╬════════════╗" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(20) << std::fixed << std::setprecision(2) << r.synapsesPerNeuron
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.avgSpikeFrequency
                  << " Hz ║" << std::endl;
    }
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╩════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Additional detailed analysis
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                             Performance Analysis                                      ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦════════════╦════════════╗" << std::endl;
    std::cout << "║   Scale       ║   Time (s)    ║   Spikes   ║  Efficiency ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬════════════╬════════════╗" << std::endl;
    
    for (const auto& r : results) {
        double totalSpikes = r.spikesPerStep * r.steps;
        double efficiency = r.synapsesPerNeuron;  // Synapses per neuron as efficiency metric
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.totalTime
                  << " ║ " << std::setw(10) << std::fixed << std::setprecision(0) << totalSpikes
                  << " ║ " << std::setw(13) << std::fixed << std::setprecision(2) << efficiency
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════╩═══════════════╩════════════╩════════════╝" << std::endl;
    std::cout << std::endl;
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
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running NLM Performance Benchmarks..." << std::endl;
    std::cout << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Enhanced benchmark configurations: (neuron_count, steps, connection_probability)
    std::vector<std::tuple<size_t, size_t, float>> benchmarks = {
        {100, 1000, 0.05f},      // 100 neurons, 1000 steps, low connection probability
        {500, 1000, 0.08f},      // 500 neurons, 1000 steps, medium connection probability  
        {1000, 500, 0.12f},      // 1000 neurons, 500 steps, higher connection probability
        {2000, 200, 0.15f},      // 2000 neurons, 200 steps, high connection probability
    };
    
    for (const auto& [neurons, steps, connProb] : benchmarks) {
        auto result = runBenchmark(neurons, steps, connProb);
        results.push_back(result);
        std::cout << std::endl;
    }
    
    // Print summary
    printBenchmarkResults(results);
    
    std::cout << "Benchmark complete." << std::endl;
    
    return 0;
}
