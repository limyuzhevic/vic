#include "CLISubcommands.hpp"
#include "benchmark/Benchmark.hpp"
#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase4Demo.hpp"
#include "experiments/Phase5Demo.hpp"
#include "experiments/Phase6Demo.hpp"
#include "experiments/Phase6TextDemo.hpp"
#include "experiments/Phase3Experiment.hpp"
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <cmath>

namespace nlm {

// Forward declarations for demo classes
class Phase3Demo;
class Phase4Demo;
class Phase5Demo;
class Phase6Demo;

// Phase 3 CLI commands
int phase3Command(int argc, char** argv) {
    printPhaseInfo(3);
    std::cout << "Running Phase 3 experiments..." << std::endl;
    
    // Default configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(1000));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    
    // Parse additional arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                std::string configFile = arg.substr(arg.find('=') + 1);
                if (!config->loadFromFile(configFile)) {
                    printError("Failed to load config from: " + configFile);
                }
            }
        } else if (arg.substr(0, 7) == "--trials") {
            if (arg.find('=') != std::string::npos) {
                int trials = std::stoi(arg.substr(arg.find('=') + 1));
                config->set("trials", trials);
            }
        }
    }
    
    // Create and run Phase 3 experiment
    TemporalPredictionExperiment tempExp;
    Phase4Results tempResults = tempExp.run(nullptr, config->getOr<int>("trials", 50));
    
    std::cout << "Temporal Prediction - Initial Error: " << tempResults.predictionErrorInitial << std::endl;
    std::cout << "Temporal Prediction - Final Error: " << tempResults.predictionErrorFinal << std::endl;
    
    // Additional experiments would be added here
    printPhaseInfo(3);
    return 0;
}

int phase3DemoCommand(int argc, char** argv) {
    printPhaseInfo(3);
    std::cout << "Running Phase 3 demo..." << std::endl;
    
    // Parse arguments
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--episodes") {
            if (arg.find('=') != std::string::npos) {
                numEpisodes = std::stoi(arg.substr(arg.find('=') + 1));
            }
        } else if (arg.substr(0, 9) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                stepsPerEpisode = std::stoi(arg.substr(arg.find('=') + 1));
            }
        }
    }
    
    // Create demo and run
    Phase3Demo demo;
    demo.run(numEpisodes, stepsPerEpisode);
    
    return 0;
}

// Phase 4 CLI commands
int phase4Command(int argc, char** argv) {
    printPhaseInfo(4);
    std::cout << "Running Phase 4 experiments..." << std::endl;
    
    // Similar implementation as Phase 3 but for Phase 4 experiments
    // For brevity, I'll implement the main functionality but could be expanded
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(1000));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", static_cast<int64_t>(42));
    
    // Run all Phase 4 experiments
    std::cout << "Running Temporal Prediction..." << std::endl;
    TemporalPredictionExperiment tempExp;
    Phase4Results tempResults = tempExp.run(nullptr, 50);
    
    std::cout << "Phase 4 - Temporal Prediction: " << tempResults.predictionErrorInitial << " -> " << tempResults.predictionErrorFinal << std::endl;
    
    // Additional experiments would be added here
    printPhaseInfo(4);
    return 0;
}

int phase4DemoCommand(int argc, char** argv) {
    printPhaseInfo(4);
    std::cout << "Running Phase 4 demo..." << std::endl;
    
    // Parse arguments
    size_t numTrials = 50;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--trials") {
            if (arg.find('=') != std::string::npos) {
                numTrials = static_cast<size_t>(std::stoi(arg.substr(arg.find('=') + 1)));
            }
        }
    }
    
    // Create and run Phase 4 demo
    Phase4Demo demo;
    return demo.main(static_cast<int>(numTrials), const_cast<char**>(&"dummy"[0]));
}

// Phase 5 CLI commands
int phase5Command(int argc, char** argv) {
    printPhaseInfo(5);
    std::cout << "Running Phase 5 experiments..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", static_cast<int64_t>(42));
    
    // Run Phase 5 experiments
    std::cout << "Pattern probe experiments..." << std::endl;
    // Would be implemented based on Phase5Experiment.cpp
    
    printPhaseInfo(5);
    return 0;
}

int phase5DemoCommand(int argc, char** argv) {
    printPhaseInfo(5);
    std::cout << "Running Phase 5 demo..." << std::endl;
    
    // Create and run Phase 5 demo
    Phase5Demo demo;
    return demo.main();
}

// Phase 6 CLI commands
int phase6Command(int argc, char** argv) {
    printPhaseInfo(6);
    std::cout << "Running Phase 6 integration experiments..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500));
    config->set("max_steps", static_cast<int64_t>(2000));
    config->set("enable_checkpointing", true);
    config->set("enable_replay", true);
    config->set("enable_development", true);
    
    Phase6IntegratedExperiment experiment;
    Phase6Config phase6Config;
    phase6Config.neuronCount = 500;
    phase6Config.maxSteps = 2000;
    phase6Config.enableCheckpointing = true;
    phase6Config.enableReplay = true;
    phase6Config.enableDevelopment = true;
    
    auto result = experiment.run(phase6Config);
    
    std::cout << "Phase 6 Integration Results:" << std::endl;
    std::cout << "  Total Reward: " << result.totalReward << std::endl;
    std::cout << "  Avg Firing Rate: " << result.avgFiringRate << std::endl;
    std::cout << "  Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    
    printPhaseInfo(6);
    return 0;
}

int phase6DemoCommand(int argc, char** argv) {
    printPhaseInfo(6);
    std::cout << "Running Phase 6 integration demo..." << std::endl;
    
    // Create and run Phase 6 demo
    Phase6Demo demo;
    return demo.main(argc, argv);
}

int phase6TextDemoCommand(int argc, char** argv) {
    printPhaseInfo(6);
    std::cout << "Running Phase 6 text/thought demo..." << std::endl;
    
    // Create and run Phase 6 text demo
    Phase6TextDemo demo;
    return demo.main();
}

// Configuration management commands
int configCommand(int argc, char** argv) {
    std::cout << "NLM Configuration Management" << std::endl;
    std::cout << "Available subcommands: load, save, validate, show" << std::endl;
    std::cout << "Use 'nlm config <subcommand> --help' for more details." << std::endl;
    return 0;
}

int configLoadCommand(int argc, char** argv) {
    std::string filename = "configs/default.cfg";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--file") {
            if (arg.find('=') != std::string::npos) {
                filename = arg.substr(arg.find('=') + 1);
            }
        }
    }
    
    std::cout << "Loading configuration from: " << filename << std::endl;
    
    auto config = std::make_shared<Config>();
    if (config->loadFromFile(filename)) {
        std::cout << "Configuration loaded successfully." << std::endl;
        printConfigSummary(*config);
    } else {
        printError("Failed to load configuration from: " + filename);
        std::cout << "Using default configuration." << std::endl;
    }
    
    return 0;
}

int configSaveCommand(int argc, char** argv) {
    std::string filename = "configs/default.cfg";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--file") {
            if (arg.find('=') != std::string::npos) {
                filename = arg.substr(arg.find('=') + 1);
            }
        }
    }
    
    std::cout << "Saving configuration to: " << filename << std::endl;
    
    auto config = std::make_shared<Config>();
    // In a real implementation, this would use the actual config object
    // For now, we just show the concept
    
    if (config->saveToFile(filename)) {
        std::cout << "Configuration saved successfully." << std::endl;
    } else {
        printError("Failed to save configuration to: " + filename);
    }
    
    return 0;
}

int configValidateCommand(int argc, char** argv) {
    std::cout << "Validating configuration..." << std::endl;
    // Configuration validation would be implemented here
    std::cout << "Configuration validation complete (placeholder)." << std::endl;
    return 0;
}

int configShowCommand(int argc, char** argv) {
    std::cout << "Showing configuration..." << std::endl;
    // Configuration display would be implemented here
    std::cout << "Configuration show complete (placeholder)." << std::endl;
    return 0;
}

// Benchmark commands
int benchmarkCommand(int argc, char** argv) {
    printPhaseInfo(5); // Phase 5 is benchmark
    std::cout << "Running NLM benchmarks..." << std::endl;
    
    // Parse benchmark arguments
    std::vector<std::pair<size_t, size_t>> benchmarks;
    bool custom = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--quick") {
            benchmarks = {{100, 100}, {500, 100}, {1000, 50}};
            custom = true;
        } else if (arg == "--comprehensive") {
            benchmarks = {{100, 1000}, {500, 1000}, {1000, 500}, {2000, 200}, {5000, 100}};
            custom = true;
        } else if (arg.substr(0, 7) == "--neurons") {
            if (arg.find('=') != std::string::npos) {
                size_t neurons = static_cast<size_t>(std::stoull(arg.substr(arg.find('=') + 1)));
                benchmarks.push_back({neurons, 100});
                custom = true;
            }
        }
    }
    
    if (!custom) {
        benchmarks = {{100, 1000}, {500, 1000}, {1000, 500}};
    }
    
    // Run benchmarks
    std::vector<BenchmarkResult> results;
    
    for (const auto& bench : benchmarks) {
        std::cout << "Benchmarking " << bench.first << " neurons, " << bench.second << " steps..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(bench.first));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < bench.second; ++step) {
            for (size_t i = 0; i < std::min(size_t(10), bench.first / 10); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkResult result;
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = bench.second;
        result.totalTime = totalTime;
        result.stepsPerSecond = bench.second / totalTime;
        result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / bench.second;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        
        results.push_back(result);
    }
    
    printBenchmarkResults(results);
    return 0;
}

// Utility commands
int helpCommand(int argc, char** argv) {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Usage: nlm <command> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help                     Show this help message" << std::endl;
    std::cout << "  version                  Show version information" << std::endl;
    std::cout << "  phase3 [options]         Run Phase 3 experiments" << std::endl;
    std::cout << "  phase4 [options]         Run Phase 4 experiments" << std::endl;
    std::cout << "  phase5 [options]         Run Phase 5 experiments" << std::endl;
    std::cout << "  phase6 [options]         Run Phase 6 integration experiments" << std::endl;
    std::cout << "  benchmark [options]      Run performance benchmarks" << std::endl;
    std::cout << "  config [options]         Configuration management" << std::endl;
    std::cout << std::endl;
    std::cout << "For detailed help on a specific command, use:" << std::endl;
    std::cout << "  nlm <command> --help" << std::endl;
    return 0;
}

int versionCommand(int argc, char** argv) {
    std::cout << "NLM - Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Phase 2-6 implemented" << std::endl;
    return 0;
}

// Common utilities
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Unified CLI Framework                                     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printPhaseInfo(int phase) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  NLM Phase " << phase << "" << std::endl;
    std::cout << std::string(60, '=') << "\n";
}

void setupLogging(const std::string& logLevel) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

std::shared_ptr<Config> loadConfigFromFile(const std::string& filename) {
    auto config = std::make_shared<Config>();
    if (!filename.empty() && config->loadFromFile(filename)) {
        return config;
    }
    return config;
}

void printConfigSummary(const Config& config) {
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << "  Total entries: " << config.getKeys().size() << std::endl;
    for (const auto& key : config.getKeys()) {
        std::cout << "  " << key << ": " << "[value]" << std::endl; // TODO: Get actual value
    }
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

// Argument parsing helpers
template<typename T>
T getArgValue(int argc, char** argv, int index, const T& defaultValue) {
    if (index >= argc) {
        return defaultValue;
    }
    std::string arg(argv[index]);
    try {
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, size_t>) {
            return std::stoi(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg == "true" || arg == "1" || arg == "yes";
        }
    } catch (const std::exception&) {
        return defaultValue;
    }
    return defaultValue;
}

} // namespace nlm
