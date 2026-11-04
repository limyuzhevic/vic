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

#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <map>
#include <fstream>
#include <sstream>

using namespace nlm;

// Command-line argument parser
class CommandLineArgs {
public:
    bool help = false;
    bool quiet = false;
    bool json = false;
    size_t neuronCount = 1000;
    size_t steps = 1000;
    std::string configFile;
    std::string outputFile;
    std::string benchmarkType = "standard";
    std::map<std::string, std::string> customConfig;
    
    bool parse(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                help = true;
            } else if (arg == "--quiet" || arg == "-q") {
                quiet = true;
            } else if (arg == "--json" || arg == "-j") {
                json = true;
            } else if (arg == "--neurons") {
                if (i + 1 < argc) neuronCount = std::stoul(argv[++i]);
            } else if (arg == "--steps") {
                if (i + 1 < argc) steps = std::stoul(argv[++i]);
            } else if (arg == "--config") {
                if (i + 1 < argc) configFile = argv[++i];
            } else if (arg == "--output") {
                if (i + 1 < argc) outputFile = argv[++i];
            } else if (arg == "--type") {
                if (i + 1 < argc) benchmarkType = argv[++i];
            } else if (arg.substr(0, 6) == "--set=") {
                std::string value = arg.substr(6);
                size_t pos = value.find('=');
                if (pos != std::string::npos) {
                    customConfig[value.substr(0, pos)] = value.substr(pos + 1);
                }
            } else if (arg.starts_with("--")) {
                std::string flag = arg.substr(1);
                size_t eqPos = flag.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = flag.substr(0, eqPos);
                    std::string value = flag.substr(eqPos + 1);
                    customConfig[key] = value;
                }
            }
        }
        return true;
    }
    
    void printHelp() {
        std::cout << "NLM Benchmark - Performance Measurement Tool" << std::endl;
        std::cout << "=============================================" << std::endl;
        std::cout << "Usage: nlm_benchmark [OPTIONS]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -h, --help              Show this help message" << std::endl;
        std::cout << "  -q, --quiet             Suppress progress output" << std::endl;
        std::cout << "  -j, --json              Output results in JSON format" << std::endl;
        std::cout << "      --neurons N         Number of neurons (default: 1000)" << std::endl;
        std::cout << "      --steps S            Number of simulation steps (default: 1000)" << std::endl;
        std::cout << "      --config FILE        Load configuration from JSON/YAML file" << std::endl;
        std::cout << "      --output FILE        Save benchmark results to file" << std::endl;
        std::cout << "      --type TYPE          Benchmark type: standard, scalability, memory" << std::endl;
        std::cout << "      --set KEY=VALUE      Set custom configuration parameter" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm_benchmark --neurons 500 --steps 2000" << std::endl;
        std::cout << "  nlm_benchmark --config benchmark_config.json --output results.json" << std::endl;
        std::cout << "  nlm_benchmark --type scalability --set neuron_count=2000" << std::endl;
    }
};

BenchmarkResult runBenchmark(size_t neuronCount, size_t steps, const CommandLineArgs& args) {
    if (!args.quiet) {
        std::cout << "  Running benchmark: " << neuronCount << " neurons, " 
                  << steps << " steps..." << std::flush;
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    
    // Apply custom config
    for (const auto& [key, value] : args.customConfig) {
        // Simple type conversion for common types
        if (value == "true" || value == "false") {
            config->set(key, value == "true", ConfigSource::CommandLine);
        } else {
            try {
                size_t pos;
                double doubleVal = std::stod(value, &pos);
                if (pos == value.size()) {
                    config->set(key, doubleVal, ConfigSource::CommandLine);
                }
            } catch (...) {
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    
    // Load from config file if specified
    if (!args.configFile.empty()) {
        config->loadFromFile(args.configFile);
    }
    
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
    
    if (!args.quiet) {
        std::cout << " Done!" << std::endl;
        std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
        std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
    }
    
    return result;
}

void printBenchmarkResults(const std::vector<BenchmarkResult>& results, bool jsonFormat = false) {
    if (jsonFormat) {
        std::cout << "{" << std::endl;
        std::cout << "  \"benchmarks\": [" << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& r = results[i];
            std::cout << "    {" << std::endl;
            std::cout << "      \"neurons\": " << r.neurons << "," << std::endl;
            std::cout << "      \"synapses\": " << r.synapses << "," << std::endl;
            std::cout << "      \"steps\": " << r.steps << "," << std::endl;
            std::cout << "      \"totalTime\": " << r.totalTime << "," << std::endl;
            std::cout << "      \"stepsPerSecond\": " << r.stepsPerSecond << "," << std::endl;
            std::cout << "      \"spikesPerStep\": " << r.spikesPerStep << "," << std::endl;
            std::cout << "      \"synapsesPerNeuron\": " << r.synapsesPerNeuron";
            if (i < results.size() - 1) {
                std::cout << "," << std::endl;
            } else {
                std::cout << std::endl;
            }
            std::cout << "    }";
        }
        std::cout << std::endl << "  ]" << std::endl;
        std::cout << "}" << std::endl;
        return;
    }
    
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

int main(int argc, char** argv) {
    CommandLineArgs args;
    if (!args.parse(argc, argv)) {
        return 1;
    }
    
    if (args.help) {
        args.printHelp();
        return 0;
    }
    
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
    auto consoleLogger = std::make_shared<ConsoleLogger>(args.quiet ? LogLevel::Error : LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    if (!args.quiet) {
        std::cout << "Running NLM Performance Benchmarks..." << std::endl;
        std::cout << std::endl;
    }
    
    std::vector<BenchmarkResult> results;
    
    // Determine benchmark configuration based on type
    std::vector<std::pair<size_t, size_t>> benchmarks;
    
    if (args.benchmarkType == "scalability") {
        // Scalability benchmark: vary neuron count, keep steps constant
        benchmarks = {
            {100, 1000},
            {500, 1000},
            {1000, 1000},
            {2000, 1000},
            {5000, 500},
        };
    } else if (args.benchmarkType == "memory") {
        // Memory benchmark: focus on larger networks
        benchmarks = {
            {1000, 500},
            {2000, 500},
            {5000, 200},
        };
    } else {
        // Standard benchmark
        benchmarks = {
            {100, 1000},
            {500, 1000},
            {1000, 500},
            {2000, 200},
        };
    }
    
    // Override with command-line arguments if provided
    if (args.neuronCount > 0 || args.steps > 0) {
        benchmarks.clear();
        size_t neurons = args.neuronCount > 0 ? args.neuronCount : 1000;
        size_t steps = args.steps > 0 ? args.steps : 1000;
        benchmarks.push_back({neurons, steps});
    }
    
    for (const auto& [neurons, steps] : benchmarks) {
        auto result = runBenchmark(neurons, steps, args);
        results.push_back(result);
        std::cout << std::endl;
    }
    
    // Print summary
    printBenchmarkResults(results, args.json);
    
    // Save results to file if specified
    if (!args.outputFile.empty()) {
        std::ofstream outFile(args.outputFile);
        if (outFile.is_open()) {
            printBenchmarkResults(results, true);
            outFile.close();
            if (!args.quiet) {
                std::cout << "Results saved to: " << args.outputFile << std::endl;
            }
        }
    }
    
    std::cout << "Benchmark complete." << std::endl;
    
    return 0;
}
