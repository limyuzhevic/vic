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
#include <algorithm>
#include <map>
#include <ctime>

using namespace nlm;

// Enhanced benchmark result structure with more metrics
struct EnhancedBenchmarkResult {
    std::string name;
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    std::vector<double> stepTimes;
    size_t peakMemoryUsage;
    double avgFiringRate;
    double learningRate;
    double stabilityScore;
    double efficiencyScore;
};

// Simple performance measurement (backward compatible)
struct BenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
};

// Command-line options for benchmark
struct BenchmarkOptions {
    std::vector<std::pair<size_t, size_t>> customConfigs;
    bool help = false;
    bool version = false;
    bool verbose = false;
    bool quiet = false;
    std::string outputFormat = "table";
    std::string outputFile;
    size_t warmupSteps = 100;
    bool enableStabilityAnalysis = false;
    bool enableEfficiencyAnalysis = false;
    double targetPerformance = 0.0;
    std::string analysisMode = "full";
    
    void printHelp() const {
        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                            NLM Benchmark Command Line Options                      ║" << std::endl;
        std::cout << "║                                                                              ║" << std::endl;
        std::cout << "║  Usage: nlm_benchmark [options]                                             ║" << std::endl;
        std::cout << "║                                                                              ║" << std::endl;
        std::cout << "║  Custom Benchmarks:                                                         ║" << std::endl;
        std::cout << "║    --benchmark <neurons>,<steps>       Add custom benchmark configuration   ║" << std::endl;
        std::cout << "║    --benchmarks <config1>,<config2>    Comma-separated list of benchmarks    ║" << std::endl;
        std::cout << "║                                                                              ║" << std::endl;
        std::cout << "║  Options:                                                                    ║" << std::endl;
        std::cout << "║    --help, -h                     Show this help message                       ║" << std::endl;
        std::cout << "║    --version, -v                  Show version information                  ║" << std::endl;
        std::cout << "║    --verbose, -V                  Enable verbose output                      ║" << std::endl;
        std::cout << "║    --quiet, -q                    Suppress non-essential output               ║" << std::endl;
        std::cout << "║    --output-format <format>        Output format: table, json, csv          ║" << std::endl;
        std::cout << "║    --output <file>                Save results to file                      ║" << std::endl;
        std::cout << "║    --warmup <steps>               Warmup steps before benchmarking         ║" << std::endl;
        std::cout << "║                                                                              ║" << std::endl;
        std::cout << "║  Analysis Options:                                                         ║" << std::endl;
        std::cout << "║    --stability-analysis           Enable stability analysis                 ║" << std::endl;
        std::cout << "║    --efficiency-analysis          Enable efficiency analysis                ║" << std::endl;
        std::cout << "║    --analysis-mode <mode>         Analysis mode: full, quick, deep         ║" << std::endl;
        std::cout << "║    --target-performance <perf>     Target performance threshold (steps/sec)  ║" << std::endl;
        std::cout << "║                                                                              ║" << std::endl;
        std::cout << "║  Built with C++20                                                            ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << "\n";
    }
    
    void printVersion() const {
        std::cout << "NLM Benchmark Tool version 0.1.0 (C++20)" << std::endl;
        std::cout << "Advanced neural network performance benchmarking" << std::endl;
    }
    
    bool parseArgs(int argc, char** argv) {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        for (size_t i = 0; i < args.size(); ++i) {
            std::string arg = args[i];
            
            if (arg == "--help" || arg == "-h") {
                help = true;
                return true;
            }
            else if (arg == "--version" || arg == "-v") {
                version = true;
                return true;
            }
            else if (arg.find("--benchmark") == 0) {
                if (arg.find("=") != std::string::npos) {
                    std::string config = arg.substr(arg.find("=") + 1);
                    parseBenchmarkConfig(config);
                } else if (i + 1 < args.size()) {
                    parseBenchmarkConfig(args[++i]);
                }
            }
            else if (arg.find("--benchmarks") == 0) {
                if (arg.find("=") != std::string::npos) {
                    std::string configs = arg.substr(arg.find("=") + 1);
                    parseBenchmarkList(configs);
                } else if (i + 1 < args.size()) {
                    parseBenchmarkList(args[++i]);
                }
            }
            else if (arg == "--verbose" || arg == "-V") {
                verbose = true;
            }
            else if (arg == "--quiet" || arg == "-q") {
                quiet = true;
            }
            else if (arg.find("--output-format") == 0) {
                if (arg.find("=") != std::string::npos) {
                    outputFormat = arg.substr(arg.find("=") + 1);
                } else if (i + 1 < args.size()) {
                    outputFormat = args[++i];
                }
                std::transform(outputFormat.begin(), outputFormat.end(), outputFormat.begin(), ::tolower);
            }
            else if (arg.find("--output") == 0) {
                if (arg.find("=") != std::string::npos) {
                    outputFile = arg.substr(arg.find("=") + 1);
                } else if (i + 1 < args.size()) {
                    outputFile = args[++i];
                }
            }
            else if (arg.find("--warmup") == 0) {
                if (arg.find("=") != std::string::npos) {
                    warmupSteps = std::stoul(arg.substr(arg.find("=") + 1));
                } else if (i + 1 < args.size()) {
                    warmupSteps = std::stoul(args[++i]);
                }
            }
            else if (arg == "--stability-analysis") {
                enableStabilityAnalysis = true;
            }
            else if (arg == "--efficiency-analysis") {
                enableEfficiencyAnalysis = true;
            }
            else if (arg.find("--analysis-mode") == 0) {
                if (arg.find("=") != std::string::npos) {
                    analysisMode = arg.substr(arg.find("=") + 1);
                } else if (i + 1 < args.size()) {
                    analysisMode = args[++i];
                }
                std::transform(analysisMode.begin(), analysisMode.end(), analysisMode.begin(), ::tolower);
            }
            else if (arg.find("--target-performance") == 0) {
                if (arg.find("=") != std::string::npos) {
                    targetPerformance = std::stod(arg.substr(arg.find("=") + 1));
                } else if (i + 1 < args.size()) {
                    targetPerformance = std::stod(args[++i]);
                }
            }
        }
        
        return true;
    }
    
    void parseBenchmarkConfig(const std::string& config) {
        size_t commaPos = config.find(',');
        if (commaPos != std::string::npos) {
            try {
                size_t neurons = std::stoul(config.substr(0, commaPos));
                size_t steps = std::stoul(config.substr(commaPos + 1));
                customConfigs.emplace_back(neurons, steps);
            } catch (...) {
                std::cerr << "ERROR: Invalid benchmark format: " << config << std::endl;
            }
        }
    }
    
    void parseBenchmarkList(const std::string& configs) {
        size_t start = 0;
        size_t end = configs.find(',');
        
        while (end != std::string::npos) {
            parseBenchmarkConfig(configs.substr(start, end - start));
            start = end + 1;
            end = configs.find(',', start);
        }
        
        if (start < configs.size()) {
            parseBenchmarkConfig(configs.substr(start));
        }
    }
    
    bool validate() const {
        if (outputFormat != "table" && outputFormat != "json" && outputFormat != "csv") {
            std::cerr << "ERROR: Invalid output format: " << outputFormat << std::endl;
            return false;
        }
        
        if (warmupSteps == 0) {
            std::cerr << "ERROR: Warmup steps must be greater than 0" << std::endl;
            return false;
        }
        
        if (analysisMode != "full" && analysisMode != "quick" && analysisMode != "deep") {
            std::cerr << "ERROR: Invalid analysis mode: " << analysisMode << std::endl;
            return false;
        }
        
        return true;
    }
    
    std::vector<std::pair<size_t, size_t>> getBenchmarkConfigs() const {
        if (customConfigs.empty()) {
            // Default benchmarks
            return {
                {100, 1000},      // 100 neurons, 1000 steps
                {500, 1000},      // 500 neurons, 1000 steps
                {1000, 500},      // 1000 neurons, 500 steps
                {2000, 200}       // 2000 neurons, 200 steps
            };
        }
        return customConfigs;
    }
};

// Helper function to calculate standard deviation
double calculateStdDev(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    double mean = sum / values.size();
    
    double sqSum = 0.0;
    for (double val : values) {
        sqSum += (val - mean) * (val - mean);
    }
    
    return std::sqrt(sqSum / values.size());
}

// Helper function to calculate stability score
 double calculateStabilityScore(const std::vector<double>& stepTimes, size_t warmupSteps) {
    if (stepTimes.size() <= warmupSteps) return 0.0;
    
    std::vector<double> steadyStateTimes;
    steadyStateTimes.reserve(stepTimes.size() - warmupSteps);
    
    for (size_t i = warmupSteps; i < stepTimes.size(); ++i) {
        steadyStateTimes.push_back(stepTimes[i]);
    }
    
    if (steadyStateTimes.empty()) return 0.0;
    
    double mean = std::accumulate(steadyStateTimes.begin(), steadyStateTimes.end(), 0.0) / steadyStateTimes.size();
    double variance = 0.0;
    for (double val : steadyStateTimes) {
        variance += (val - mean) * (val - mean);
    }
    variance /= steadyStateTimes.size();
    
    return 1.0 / (1.0 + std::sqrt(variance)); // Higher is more stable
}

// Helper function to calculate efficiency score
 double calculateEfficiencyScore(const EnhancedBenchmarkResult& result) {
    double baseEfficiency = result.stepsPerSecond / 1000.0; // Normalize to 1000 steps/sec
    double spikeEfficiency = 1.0 / (1.0 + result.spikesPerStep); // Lower spike rate is more efficient
    double learningEfficiency = result.learningRate / 0.1; // Normalize to 0.1 learning rate
    
    return (baseEfficiency + spikeEfficiency + learningEfficiency) / 3.0;
}

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
