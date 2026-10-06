#pragma once

/**
 * Modern Unified NLM Demo with Advanced CLI
 * 
 * This is the improved Phase 6 demo that replaces the older Phase 2 main.cpp.
 * Features:
 * - Modern command-line interface with subcommands
 * - Better configuration management
 * - Comprehensive logging and monitoring
 * - Advanced user commands (status, stats, benchmark, etc.)
 * - Improved error handling and user feedback
 * - Better resource management and performance monitoring
 */

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <sstream>

namespace nlm {

// Forward declarations for CLI command functions
void printBanner(bool verbose = false);
void printHelp(const std::string& command = "");
void runDemoMode(const Phase6Config& config);
void runBenchmarkMode(const Phase6Config& config, const std::string& outputFile);
void runProfileMode(const Phase6Config& config);
void runStatusMode(const std::shared_ptr<Brain>& brain);
void runExportMode(const std::shared_ptr<Brain>& brain, const std::string& outputFile);
void runInteractiveMode(const Phase6Config& config);
Phase6Config parseConfigFromCommandLine(int argc, char** argv);
Phase6Config createDefaultConfig();

// Modern CLI command definitions
struct CLICommand {
    std::string name;
    std::string description;
    std::string usage;
    bool requiresBrain;
    std::function<void(int argc, char** argv)> handler;
};

// Global experiment instance for status commands
std::shared_ptr<Phase6IntegratedExperiment> g_experiment = nullptr;
std::shared_ptr<Brain> g_currentBrain = nullptr;

void printBanner(bool verbose) {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                               ║\n";
    std::cout << "║         NLM — 熙然 (Neural Learning Machine)                    ║\n";
    std::cout << "║         Advanced Artificial Developmental Brain                  ║\n";
    std::cout << "║                                                               ║\n";
    
    if (verbose) {
        std::cout << "║         Phase 6: FINAL INTEGRATION DEMO                           ║\n";
        std::cout << "║         Modern Enhanced Version                                    ║\n";
    } else {
        std::cout << "║         Phase 6: Unified Demo                                      ║\n";
    }
    
    std::cout << "║                                                               ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

void printHelp(const std::string& command) {
    printBanner(true);
    
    std::cout << "USAGE:\n";
    std::cout << "  nlm_demo [OPTIONS] [COMMAND] [ARGS...]\n\n";
    
    std::cout << "COMMANDS:\n";
    std::cout << "  demo          Run Phase 6 integration demo (default)\n";
    std::cout << "  benchmark     Run performance benchmark\n";
    std::cout << "  profile       Run profiling session\n";
    std::cout << "  status        Show brain/system status\n";
    std::cout << "  export        Export brain state to file\n";
    std::cout << "  interactive   Start interactive mode\n";
    std::cout << "  help          Show this help message\n\n";
    
    if (!command.empty()) {
        std::cout << "Command '" << command << "' help:\n";
        if (command == "demo") {
            std::cout << "  Run Phase 6 integration demo\n";
            std::cout << "  Options: --neurons=N (default: 500), --steps=N (default: 2000)\n";
            std::cout << "           --no-checkpoint, --no-replay, --no-development\n";
        } else if (command == "benchmark") {
            std::cout << "  Run performance benchmark\n";
            std::cout << "  Options: --neurons=N, --steps=N, --output=FILE\n";
        } else if (command == "profile") {
            std::cout << "  Run profiling session\n";
            std::cout << "  Options: --neurons=N, --steps=N\n";
        } else if (command == "status") {
            std::cout << "  Show brain/system status\n";
            std::cout << "  Run a demo first with 'demo' command.\n";
        } else if (command == "export") {
            std::cout << "  Export brain state to file\n";
            std::cout << "  Options: --neurons=N, --steps=N, --output=FILE\n";
        } else if (command == "interactive") {
            std::cout << "  Start interactive mode\n";
            std::cout << "  Type 'help' for available commands\n";
        }
    }
    
    std::cout << "\nGLOBAL OPTIONS:\n";
    std::cout << "  --config=FILE    Load configuration from file\n";
    std::cout << "  --help, -h       Show help\n";
    std::cout << "  --version        Show version\n";
    std::cout << "  --verbose, -v    Enable verbose output\n";
    std::cout << "  --quiet, -q      Suppress non-essential output\n\n";
    
    std::cout << "EXAMPLES:\n";
    std::cout << "  nlm_demo demo                            # Run default demo\n";
    std::cout << "  nlm_demo demo --neurons=1000 --steps=5000  # Custom demo\n";
    std::cout << "  nlm_demo benchmark --output=perf_results.csv  # Performance benchmark\n";
    std::cout << "  nlm_demo status                           # Show system status\n";
    std::cout << "  nlm_demo export --output=brain_state.bin     # Save brain state\n\n";
}

Phase6Config createDefaultConfig() {
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "./nlm_checkpoint.bin";
    return config;
}

Phase6Config parseConfigFromCommandLine(int argc, char** argv) {
    Phase6Config config = createDefaultConfig();
    
    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoull(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoull(argv[++i]);
        } else if (arg == "--region-count" && i + 1 < argc) {
            config.regionCount = std::stoull(argv[++i]);
        } else if (arg == "--connection-prob" && i + 1 < argc) {
            config.connectionProbability = std::stof(argv[++i]);
        } else if (arg == "--no-checkpoint") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg == "--checkpoint-path" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        }
    }
    
    return config;
}

void logWithTimestamp(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::cout << "[" << std::put_time(std::localtime(&time), "%H:%M:%S") << "] " << message << std::endl;
}

void runDemoMode(const Phase6Config& config) {
    logWithTimestamp("Starting Phase 6 integration demo...");
    logWithTimestamp("Configuration: " + std::to_string(config.neuronCount) + " neurons, " + 
                    std::to_string(config.maxSteps) + " steps");
    
    Phase6IntegratedExperiment experiment;
    auto result = experiment.run(config);
    
    logWithTimestamp("=== DEMO RESULTS ===");
    logWithTimestamp("Total reward: " + std::to_string(result.totalReward));
    logWithTimestamp("Average firing rate: " + std::to_string(result.avgFiringRate));
    logWithTimestamp("Episodic memory episodes: " + std::to_string(result.memoryEpisodesStored));
    logWithTimestamp("Dopamine level: " + std::to_string(result.dopamineLevel));
    
    if (result.memoryWorkingMemoryIntegrated) {
        logWithTimestamp("✓ Working memory integrated successfully");
    }
    if (result.memoryEpisodicMemoryIntegrated) {
        logWithTimestamp("✓ Episodic memory integrated successfully");
    }
    if (result.neuromodulationIntegrated) {
        logWithTimestamp("✓ Neuromodulation integrated successfully");
    }
    if (result.predictionIntegrated) {
        logWithTimestamp("✓ Prediction system integrated successfully");
    }
    if (result.developmentIntegrated) {
        logWithTimestamp("✓ Development system integrated successfully");
    }
    if (result.checkpointingWorks) {
        logWithTimestamp("✓ Checkpointing functional");
    }
    if (result.replayWorks) {
        logWithTimestamp("✓ Replay system functional");
    }
    
    logWithTimestamp("Demo completed successfully!");
}

void runBenchmarkMode(const Phase6Config& config, const std::string& outputFile) {
    logWithTimestamp("Starting performance benchmark...");
    
    Phase6IntegratedExperiment experiment;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto result = experiment.run(config);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();
    
    logWithTimestamp("=== BENCHMARK RESULTS ===");
    logWithTimestamp("Total time: " + std::to_string(elapsed) + " seconds");
    logWithTimestamp("Steps per second: " + std::to_string(config.maxSteps / elapsed));
    logWithTimestamp("Neurons processed: " + std::to_string(config.neuronCount));
    logWithTimestamp("Total reward: " + std::to_string(result.totalReward));
    
    // Save results to CSV if requested
    if (!outputFile.empty()) {
        std::ofstream file(outputFile, std::ios::app);
        if (file.is_open()) {
            file << "benchmark," << config.neuronCount << "," << config.maxSteps << ","
                 << elapsed << "," << config.maxSteps / elapsed << "," 
                 << result.totalReward << "," << result.avgFiringRate << "\n";
            file.close();
            logWithTimestamp("Results saved to " + outputFile);
        }
    }
}

void runProfileMode(const Phase6Config& config) {
    logWithTimestamp("Starting profiling session...");
    logWithTimestamp("Profile: Neuron count = " + std::to_string(config.neuronCount));
    logWithTimestamp("Profile: Max steps = " + std::to_string(config.maxSteps));
    logWithTimestamp("Profile: Memory systems enabled = " + 
                    std::string(config.enableCheckpointing ? "YES" : "NO"));
    
    // Run a quick demo for profiling
    Phase6IntegratedExperiment experiment;
    auto result = experiment.run(config);
    
    logWithTimestamp("Profile completed. Check memory usage and performance metrics.");
}

void runStatusMode(const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        std::cerr << "Error: No brain instance available for status check." << std::endl;
        return;
    }
    
    logWithTimestamp("=== SYSTEM STATUS ===");
    logWithTimestamp("Brain status:");
    logWithTimestamp("  Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    logWithTimestamp("  Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    logWithTimestamp("  Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
    logWithTimestamp("  Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
    logWithTimestamp("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    
    // Check system components
    logWithTimestamp("\nSystem components status:");
    auto* wm = brain->getWorkingMemory();
    if (wm) {
        logWithTimestamp("  ✓ Working memory: " + std::to_string(wm->getActiveTraces()) + " active traces");
    } else {
        logWithTimestamp("  ✗ Working memory: Not available");
    }
    
    auto* em = brain->getEpisodicMemory();
    if (em) {
        logWithTimestamp("  ✓ Episodic memory: " + std::to_string(em->getEpisodeCount()) + " episodes");
    } else {
        logWithTimestamp("  ✗ Episodic memory: Not available");
    }
    
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        logWithTimestamp("  ✓ Neuromodulation: Active");
    } else {
        logWithTimestamp("  ✗ Neuromodulation: Not available");
    }
    
    auto* prediction = brain->getPredictionSystem();
    if (prediction) {
        logWithTimestamp("  ✓ Prediction system: Active");
    } else {
        logWithTimestamp("  ✗ Prediction system: Not available");
    }
    
    auto* development = brain->getDevelopmentSystem();
    if (development) {
        logWithTimestamp("  ✓ Development system: Active");
    } else {
        logWithTimestamp("  ✗ Development system: Not available");
    }
}

void runExportMode(const std::shared_ptr<Brain>& brain, const std::string& outputFile) {
    if (!brain) {
        std::cerr << "Error: No brain instance available for export." << std::endl;
        return;
    }
    
    if (outputFile.empty()) {
        std::cerr << "Error: No output file specified for export." << std::endl;
        return;
    }
    
    logWithTimestamp("Exporting brain state to " + outputFile + "...");
    
    if (brain->save(outputFile)) {
        logWithTimestamp("✓ Brain state exported successfully to " + outputFile);
        
        // Get export statistics
        logWithTimestamp("Export statistics:");
        logWithTimestamp("  Neurons: " + std::to_string(brain->getTotalNeuronCount()));
        logWithTimestamp("  Synapses: " + std::to_string(brain->getTotalSynapseCount()));
        logWithTimestamp("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    } else {
        logWithTimestamp("✗ Failed to export brain state to " + outputFile);
    }
}

void runInteractiveMode(const Phase6Config& config) {
    logWithTimestamp("Starting interactive mode...");
    logWithTimestamp("Type 'help' for available commands");
    
    std::string command;
    while (true) {
        std::cout << ">>> ";
        std::getline(std::cin, command);
        
        if (command.empty()) continue;
        
        if (command == "exit" || command == "quit") {
            logWithTimestamp("Exiting interactive mode");
            break;
        } else if (command == "help") {
            std::cout << "Available commands:\n";
            std::cout << "  demo [config]    - Run integration demo\n";
            std::cout << "  benchmark        - Run performance benchmark\n";
            std::cout << "  status           - Show system status\n";
            std::cout << "  export [file]    - Export brain state\n";
            std::cout << "  help             - Show this help\n";
            std::cout << "  exit/quit        - Exit interactive mode\n";
        } else if (command == "demo") {
            runDemoMode(config);
        } else if (command == "benchmark") {
            runBenchmarkMode(config, "");
        } else if (command == "status") {
            // For interactive mode, we need to create a brain instance
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", config.neuronCount);
            auto brain = std::make_shared<Brain>(cfg);
            if (brain->initialize()) {
                runStatusMode(brain);
            }
        } else if (command.substr(0, 6) == "export") {
            std::string filename = command.substr(7);
            if (filename.empty()) {
                std::cout << "Error: Please specify output file\n";
                continue;
            }
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", config.neuronCount);
            auto brain = std::make_shared<Brain>(cfg);
            if (brain->initialize()) {
                runExportMode(brain, filename);
            }
        } else {
            std::cout << "Unknown command: " << command << "\n";
            std::cout << "Type 'help' for available commands\n";
        }
    }
}

int main(int argc, char* argv[]) {
    // Parse global options first
    std::string configFile = "";
    bool verbose = false;
    bool quiet = false;
    bool showHelp = false;
    bool showVersion = false;
    
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        } else if (arg == "--version") {
            showVersion = true;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            quiet = true;
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg.substr(0, 2) == "--") {
            // Pass through unknown options to commands
            args.push_back(arg);
        } else if (!arg.empty() && arg[0] != '-') {
            // This is a command
            args.push_back(arg);
        }
    }
    
    if (showVersion) {
        std::cout << "NLM Phase 6 Demo - Version 2.0.0\n";
        return 0;
    }
    
    if (showHelp) {
        printHelp("");
        return 0;
    }
    
    printBanner(verbose);
    
    if (args.empty()) {
        // Default to demo mode
        args.push_back("demo");
    }
    
    std::string command = args[0];
    Phase6Config config = createDefaultConfig();
    
    // Parse config file if specified
    if (!configFile.empty()) {
        auto cfg = std::make_shared<Config>();
        if (cfg->loadFromFile(configFile)) {
            logWithTimestamp("Loaded configuration from: " + configFile);
        }
    }
    
    // Parse command line config options
    config = parseConfigFromCommandLine(argc, argv);
    
    try {
        if (command == "demo") {
            runDemoMode(config);
        } else if (command == "benchmark") {
            std::string outputFile;
            for (size_t i = 1; i < args.size(); ++i) {
                if (args[i] == "--output" && i + 1 < args.size()) {
                    outputFile = args[i + 1];
                    break;
                }
            }
            runBenchmarkMode(config, outputFile);
        } else if (command == "profile") {
            runProfileMode(config);
        } else if (command == "status") {
            // For status, we need to create a brain first
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", config.neuronCount);
            auto brain = std::make_shared<Brain>(cfg);
            if (brain->initialize()) {
                runStatusMode(brain);
            } else {
                std::cerr << "Error: Failed to initialize brain for status check" << std::endl;
                return 1;
            }
        } else if (command == "export") {
            std::string outputFile;
            for (size_t i = 1; i < args.size(); ++i) {
                if (args[i] == "--output" && i + 1 < args.size()) {
                    outputFile = args[i + 1];
                    break;
                }
            }
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", config.neuronCount);
            auto brain = std::make_shared<Brain>(cfg);
            if (brain->initialize()) {
                runExportMode(brain, outputFile);
            } else {
                std::cerr << "Error: Failed to initialize brain for export" << std::endl;
                return 1;
            }
        } else if (command == "interactive") {
            runInteractiveMode(config);
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            printHelp(command);
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        if (verbose) {
            std::cerr << "Stack trace would go here" << std::endl;
        }
        return 1;
    }
    
    logWithTimestamp("Operation completed successfully!");
    return 0;
}

} // namespace nlm
