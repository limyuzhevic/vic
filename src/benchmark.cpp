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
#include <map>

using namespace nlm;

// Print banner
void printBanner() {
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
}

// Print help message
void printHelp() {
    std::cout << "\nUsage: nlm_benchmark [OPTIONS]\n\n";
    std::cout << "NLM Neural Learning Machine - Performance Benchmark\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help, -h              Show this help message\n";
    std::cout << "  --version, --ver        Show version information\n";
    std::cout << "  --verbose, -v           Enable verbose logging\n";
    std::cout << "  --quiet                 Suppress output\n";
    std::cout << "  --config=<file>         Load configuration from file\n";
    std::cout << "  --neurons=<N>           Override neuron count (default: 500)\n";
    std::cout << "  --steps=<N>             Override simulation steps (default: 1000)\n";
    std::cout << "  --regions=<N>           Override region count (default: 1)\n";
    std::cout << "  --seed=<S>              Override random seed (default: 42)\n";
    std::cout << "  --timestep=<t>          Override simulation timestep (default: 0.001)\n";
    std::cout << "  --benchmark=<N>         Benchmark to run (1-4, or 'all' for all, default: 1-4)\n";
    std::cout << "  --timeout=<s>           Timeout per benchmark in seconds\n";
    std::cout << "\nExamples:\n";
    std::cout << "  nlm_benchmark --help                  Show help\n";
    std::cout << "  nlm_benchmark --verbose --neurons=1000 Run benchmark with 1000 neurons\n";
    std::cout << "  nlm_benchmark --steps=500 --regions=2   Run with custom steps and regions\n";
    std::cout << "  nlm_benchmark --benchmark=2            Run only the second benchmark\n";
    std::cout << "  nlm_benchmark --benchmark=all          Run all benchmarks\n";
    std::cout << std::endl;
}

// Print version
void printVersion() {
    std::cout << "NLM Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Performance Benchmark Suite" << std::endl;
    std::cout << "Build with C++20, scikit-build-core, pybind11" << std::endl;
}

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
    
    for (size=_t step = 0; step < steps; ++step) {
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
    std::cout << "    Time: " << std::fixed << std::setprecision( gynauto endTime = std::chrono::high_resolution_clock::now();
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

int main() {
    printBanner();
    
    // Check for command line arguments
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --help/-h
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return 0;
            }
            
            // Handle --version/--ver
            if (arg == "--version" || arg == "--ver") {
                printVersion();
                return 0;
            }
            
            // Handle --verbose/-v
            if (arg == "--verbose" || arg == "-v") {
                std::cout << "Verbose mode enabled" << std::endl;
                continue;
            }
            
            // Handle --quiet
            if (arg == "--quiet") {
                std::cout << std::fixed << std::setprecision(0);
                continue;
            }
            
            // Handle --config=file
            if (arg.substr(0, 9) == "--config=") {
                // TODO: Load configuration from file
                std::cout << "Loading config: " << arg.substr(9) << std::endl;
                continue;
            }
            
            // Handle --benchmark=number
            if (arg.substr(0, 13) == "--benchmark=") {
                // TODO: Set benchmark selection
                std::cout << "Benchmark selection: " << arg.substr(13) << std::endl;
                continue;
            }
        }
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running NLM Performance Benchmarks..." << std::endl;
    std::cout << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Benchmark configurations: (neuron_count, steps)
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 1000},      // 100 neurons, 1000 steps
        {500, 1000},      // 500 neurons, 1000 steps
        {1000, 500},      // 1000 neurons, 500 steps
        {2000, 200},      // 2000 neurons, 200 steps
    };
    
    for (const auto& [neurons, steps] : benchmarks) {
        auto result = runBenchmark(neurons, steps);
        results.push_back(result);
        std::cout << std::endl;
    }
    
    // Print summary
    printBenchmarkResults(results);
    
    std::cout << "Benchmark complete." << std::endl;
    
    return 0;
}