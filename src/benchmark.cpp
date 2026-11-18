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

// Enhanced benchmark runner with error handling
BenchmarkResult runBenchmark(size_t neuronCount, size_t steps, bool quiet = false, bool detailed = false) {
    if (steps == 0) {
        std::cout << "Error: Steps must be > 0" << std::endl;
        return {0, 0, 0, 0.0, 0.0, 0.0, 0.0};
    }
    
    if (neuronCount == 0) {
        std::cout << "Error: Neuron count must be > 0" << std::endl;
        return {0, 0, 0, 0.0, 0.0, 0.0, 0.0};
    }
    
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
    
    if (!brain->initialize()) {
        std::cout << " FAILED! Brain initialization failed." << std::endl;
        return {neuronCount, 0, steps, 0.0, 0.0, 0.0, 0.0};
    }
    
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
    
    if (!quiet) {
        std::cout << " Done!" << std::endl;
        std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
        std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
    }
    
    if (detailed) {
        std::cout << "    Neurons: " << result.neurons << std::endl;
        std::cout << "    Synapses: " << result.synapses << std::endl;
        std::cout << "    Network density: " << std::fixed << std::setprecision(4) 
                  << (result.synapses / (result.neurons * result.neurons)) << " synapses/neuron²" << std::endl;
        std::cout << "    Activity: " << std::fixed << std::setprecision(2) << result.spikesPerStep << " spikes/step" << std::endl;
        std::cout << "    Throughput: " << std::fixed << std::setprecision(2) 
                  << (result.synapses * result.steps / totalTime / 1000000) << " M synapses/sec" << std::endl;
    }
    
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

void printDetailedBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                 NLM Detailed Performance Benchmark Results                     ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╦════════════════════════╗" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec ║ Spikes/step ║ Synapses/Neuron      ║ Total Time (s)  ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╬════════════════════════╗" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(20) << std::fixed << std::setprecision(2) << r.synapsesPerNeuron
                  << " ║ " << std::setw(15) << std::fixed << std::setprecision(3) << r.totalTime
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╩════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Calculate and display statistics
    if (!results.empty()) {
        double totalNeurons = 0, totalSynapses = 0, totalStepsPerSec = 0;
        size_t count = 0;
        
        for (const auto& r : results) {
            totalNeurons += r.neurons;
            totalSynapses += r.synapses;
            totalStepsPerSec += r.stepsPerSecond;
            count++;
        }
        
        if (count > 0) {
            std::cout << "╔════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                              Benchmark Statistics                              ║" << std::endl;
            std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
            std::cout << "║ Average Neurons: " << std::setw(50) << std::fixed << std::setprecision(1) 
                      << (totalNeurons / count) << "                          ║" << std::endl;
            std::cout << "║ Average Synapses: " << std::setw(48) << std::fixed << std::setprecision(1) 
                      << (totalSynapses / count) << "                         ║" << std::endl;
            std::cout << "║ Average Steps/sec: " << std::setw(44) << std::fixed << std::setprecision(0) 
                      << (totalStepsPerSec / count) << "                         ║" << std::endl;
            std::cout << "╚════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        }
    }
}

void printUsage() {
    std::cout << "Usage: nlm_benchmark [options] [neurons] [neurons,steps]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help          Show this help message" << std::endl;
    std::cout << "  --config file        Load configuration from file" << std::endl;
    std::cout << "  --benchmarks file    Load benchmarks from file" << std::endl;
    std::cout << "  --quiet              Suppress detailed output" << std::endl;
    std::cout << "  --detailed           Enable detailed statistics" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Arguments:" << std::endl;
    std::cout << "  neurons              Run single benchmark with given neuron count (default steps: 1000)" << std::endl;
    std::cout << "  neurons,steps        Run single benchmark with specified neuron count and steps" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_benchmark                    Run default benchmarks" << std::endl;
    std::cout << "  nlm_benchmark --detailed         Run with detailed output" << std::endl;
    std::cout << "  nlm_benchmark 500               Run single benchmark: 500 neurons" << std::endl;
    std::cout << "  nlm_benchmark 1000,500          Run single benchmark: 1000 neurons, 500 steps" << std::endl;
    std::cout << "  nlm_benchmark --quiet 2000      Run quiet benchmark: 2000 neurons" << std::endl;
}

int main(int argc, char** argv) {
    // Helper variables for benchmark customization
    bool quietMode = false;
    bool detailedMode = false;
    
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
    
    // Benchmark configurations: (neuron_count, steps)
    // Default benchmarks - can be overridden via command line
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 1000},      // 100 neurons, 1000 steps
        {500, 1000},      // 500 neurons, 1000 steps
        {1000, 500},      // 1000 neurons, 500 steps
        {2000, 200},      // 2000 neurons, 200 steps
    };
    
    // Parse command-line arguments for benchmark configuration
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg == "--help" || arg == "-h") {
                printUsage();
                return 0;
            } else if (arg == "--config") {
                // Load configuration from file (placeholder - implement if needed)
                std::cout << "Warning: Config file loading not yet implemented" << std::endl;
            } else if (arg == "--benchmarks") {
                // Custom benchmarks: parse as neuron_count,steps pairs
                std::cout << "Error: --benchmarks option not yet implemented" << std::endl;
                return 1;
            } else if (arg == "--quiet") {
                quietMode = true;
            } else if (arg == "--detailed") {
                detailedMode = true;
            } else {
                // Try to parse as neuron_count or neuron_count,steps
                try {
                    size_t neurons = std::stoull(arg);
                    benchmarks.clear();
                    benchmarks.push_back({neurons, 1000}); // Default 1000 steps
                    
                    // Check if next arg is steps
                    if (i + 1 < argc) {
                        size_t steps = std::stoull(argv[++i]);
                        benchmarks.clear();
                        benchmarks.push_back({neurons, steps});
                    }
                } catch (...) {
                    // Try to parse as neuron_count,steps
                    try {
                        size_t comma = arg.find(',');
                        if (comma != std::string::npos) {
                            size_t neurons = std::stoull(arg.substr(0, comma));
                            size_t steps = std::stoull(arg.substr(comma + 1));
                            benchmarks.clear();
                            benchmarks.push_back({neurons, steps});
                        }
                    } catch (...) {
                        std::cout << "Warning: Ignoring unrecognized argument: " << arg << std::endl;
                    }
                }
            }
        }
    }
    
    for (const auto& [neurons, steps] : benchmarks) {
        auto result = runBenchmark(neurons, steps, quietMode, detailedMode);
        results.push_back(result);
        std::cout << std::endl;
    }
    
    // Print summary based on mode
    if (detailedMode) {
        printDetailedBenchmarkResults(results);
    } else {
        printBenchmarkResults(results);
    }
    
    std::cout << "Benchmark complete." << std::endl;
    
    return 0;
}
