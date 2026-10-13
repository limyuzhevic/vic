#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <algorithm>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase integrates all previous systems.                ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Command-line argument parsing
struct CLIArgs {
    std::string configFile = "configs/default.cfg";
    std::string saveFile;
    std::string loadFile;
    std::string configKey;
    std::string configValue;
    size_t simulationSteps = 1000;
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    bool verbose = false;
    bool quiet = false;
    bool interactive = false;
    bool background = false;
    bool continueFromCheckpoint = false;
    std::string checkpointDir;
    size_t checkpointInterval = 1000;
    std::string logLevel = "info";
    std::string outputFile;
    bool showHelp = false;
    bool showVersion = false;
    std::string command;
};

CLIArgs parseArgs(int argc, char** argv) {
    CLIArgs args;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            args.showHelp = true;
        } else if (arg == "--version") {
            args.showVersion = true;
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            args.quiet = true;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                try {
                    args.simulationSteps = std::stoull(argv[++i]);
                } catch (...) {
                    std::cerr << "Invalid steps value" << std::endl;
                }
            }
        } else if (arg == "--neuron-count") {
            if (i + 1 < argc) {
                try {
                    args.neuronCount = std::stoull(argv[++i]);
                } catch (...) {
                    std::cerr << "Invalid neuron count value" << std::endl;
                }
            }
        } else if (arg == "--region-count") {
            if (i + 1 < argc) {
                try {
                    args.regionCount = std::stoull(argv[++i]);
                } catch (...) {
                    std::cerr << "Invalid region count value" << std::endl;
                }
            }
        } else if (arg == "save") {
            args.command = "save";
            if (i + 1 < argc) {
                args.saveFile = argv[++i];
            }
        } else if (arg == "load") {
            args.command = "load";
            if (i + 1 < argc) {
                args.loadFile = argv[++i];
            }
        } else if (arg == "config") {
            args.command = "config";
            if (i + 1 < argc) {
                args.configKey = argv[++i];
                if (i + 1 < argc) {
                    args.configValue = argv[++i];
                }
            }
        } else if (arg == "init") {
            args.command = "init";
        } else if (arg == "run") {
            args.command = "run";
        } else if (arg == "status") {
            args.command = "status";
        } else if (arg == "info") {
            args.command = "info";
        } else if (arg == "stats") {
            args.command = "stats";
        } else if (arg == "simulate") {
            args.command = "simulate";
            if (i + 1 < argc) {
                try {
                    args.simulationSteps = std::stoull(argv[++i]);
                } catch (...) {
                    std::cerr << "Invalid steps value" << std::endl;
                }
            }
        } else if (arg == "visualize") {
            args.command = "visualize";
        } else if (arg == "benchmark") {
            args.command = "benchmark";
        } else if (arg == "--interactive") {
            args.interactive = true;
        } else if (arg == "--background") {
            args.background = true;
        } else if (arg == "--continue") {
            args.continueFromCheckpoint = true;
        } else if (arg == "--checkpoint") {
            if (i + 1 < argc) {
                args.checkpointDir = argv[++i];
            }
        } else if (arg == "--checkpoint-interval") {
            if (i + 1 < argc) {
                try {
                    args.checkpointInterval = std::stoull(argv[++i]);
                } catch (...) {
                    std::cerr << "Invalid checkpoint interval value" << std::endl;
                }
            }
        } else if (arg == "--log-level") {
            if (i + 1 < argc) {
                args.logLevel = argv[++i];
            }
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                args.outputFile = argv[++i];
            }
        } else if (arg == "--help") {
            args.showHelp = true;
        } else {
            // If not recognized and command not set, treat as command
            if (args.command.empty()) {
                args.command = arg;
            }
        }
    }

    return args;
}

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [COMMAND] [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "COMMANDS:" << std::endl;
    std::cout << "  run          Run NLM simulation" << std::endl;
    stdcout << "  init         Initialize NLM brain" << std::endl;
    std::cout << "  save <FILE>  Save brain state to checkpoint" << std::endl;
    std::cout << "  load <FILE>  Load brain state from checkpoint" << std::endl;
    std::cout << "  config <KEY> [VALUE]  Get or set configuration" << std::endl;
    std::cout << "  info         Display system information" << std::endl;
    std::cout << "  status       Show brain status" << std::endl;
    std::cout << "  stats        Display performance statistics" << std::endl;
    std::cout << "  simulate <STEPS>  Run simulation for steps" << std::endl;
    std::cout << "  visualize    Launch visualization interface" << std::endl;
    std::cout << "  benchmark    Run performance benchmarks" << std::endl;
    std::cout << "  help         Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "GLOBAL OPTIONS:" << std::endl;
    std::cout << "  --config FILE         Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --verbose, -v        Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q          Suppress non-essential output" << std::endl;
    std::cout << "  --help, -h           Show help message" << std::endl;
}

void printInfo() {
    printBanner();
    std::cout << std::endl;
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Version 0.1.0 (Phase 6: Final Integration)" << std::endl;
    std::cout << std::endl;
    std::cout << "A comprehensive artificial brain simulation that integrates:" << std::endl;
    std::cout << "  • Real spiking neural computation (LIF neurons)" << std::endl;
    std::cout << "  • Multiple memory systems (working, episodic, associative)" << std::endl;
    std::cout << "  • Neuromodulation (dopamine, curiosity, novelty)" << std::endl;
    std::cout << "  • Prediction and cognition systems" << std::endl;
    std::cout << "  • Developmental stages and plasticity" << std::endl;
    std::cout << "  • Event-driven spike propagation with delays" << std::endl;
    std::cout << "  • Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << std::endl;
    std::cout << "For more information, visit: https://github.com/natural-neural-machine/NLM" << std::endl;
}

int main(int argc, char** argv) {
    CLIArgs args = parseArgs(argc, argv);

    // Handle global options
    if (args.showHelp) {
        printUsage(argv[0]);
        return 0;
    }

    if (args.showVersion) {
        std::cout << "NLM v0.1.0 (Phase 6: Final Integration)" << std::endl;
        return 0;
    }

    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    // Load configuration
    auto config = std::make_shared<Config>();

    // Load config from file (ignore if not found)
    if (config->loadFromFile(args.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + args.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }

    // Set default values for Phase 6
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(args.neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(args.regionCount), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);

    // Phase 6 specific settings
    config->set("replay_interval", 100, ConfigSource::Default);
    config->set("consolidation_interval", 1000, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);

    // Set default values for development
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);

    // Set log level
    if (args.logLevel == "debug") {
        consoleLogger->setLevel(LogLevel::Debug);
    } else if (args.logLevel == "warning") {
        consoleLogger->setLevel(LogLevel::Warning);
    } else if (args.logLevel == "error") {
        consoleLogger->setLevel(LogLevel::Error);
    }

    // Process commands
    if (args.command == "help") {
        printUsage(argv[0]);
        return 0;
    } else if (args.command == "info") {
        printInfo();
        return 0;
    } else if (args.command == "version") {
        std::cout << "NLM v0.1.0 (Phase 6: Final Integration)" << std::endl;
        return 0;
    }

    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);

    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6 - Integrated)...");
    auto brain = std::make_shared<Brain>(config);

    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }

    brain->logStatus();

    if (args.command == "init") {
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        return 0;
    } else if (args.command == "run") {
        NLM_LOG_INFO("Running NLM simulation...");
        for (SimulationStep step = 0; step < static_cast<SimulationStep>(args.simulationSteps); ++step) {
            brain->step(step, step * timestep);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        brain->logStatus();
        NLM_LOG_INFO("NLM simulation complete (Phase 6 - Integrated)");
        return 0;
    } else if (args.command == "save") {
        if (args.saveFile.empty()) {
            NLM_LOG_ERROR("Save command requires a file argument");
            return 1;
        }
        NLM_LOG_INFO("Saving brain state to " + args.saveFile);
        if (brain->save(args.saveFile)) {
            NLM_LOG_INFO("Brain state saved successfully");
            return 0;
        } else {
            NLM_LOG_ERROR("Failed to save brain state");
            return 1;
        }
    } else if (args.command == "load") {
        if (args.loadFile.empty()) {
            NLM_LOG_ERROR("Load command requires a file argument");
            return 1;
        }
        NLM_LOG_INFO("Loading brain state from " + args.loadFile);
        if (brain->load(args.loadFile)) {
            NLM_LOG_INFO("Brain state loaded successfully");
            return 0;
        } else {
            NLM_LOG_ERROR("Failed to load brain state");
            return 1;
        }
    } else if (args.command == "config") {
        if (args.configKey.empty()) {
            NLM_LOG_ERROR("Config command requires a key argument");
            return 1;
        }
        
        if (!args.configValue.empty()) {
            // Set configuration value
            if (args.configKey == "random_seed") {
                try {
                    int64_t value = std::stoll(args.configValue);
                    config->set("random_seed", value, ConfigSource::User);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid integer value for random_seed");
                    return 1;
                }
            } else if (args.configKey == "neuron_count") {
                try {
                    int64_t value = std::stoll(args.configValue);
                    config->set("neuron_count", value, ConfigSource::User);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid integer value for neuron_count");
                    return 1;
                }
            } else if (args.configKey == "region_count") {
                try {
                    int64_t value = std::stoll(args.configValue);
                    config->set("region_count", value, ConfigSource::User);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid integer value for region_count");
                    return 1;
                }
            } else if (args.configKey == "connection_probability") {
                try {
                    float value = std::stof(args.configValue);
                    config->set("connection_probability", value, ConfigSource::User);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid float value for connection_probability");
                    return 1;
                }
            } else if (args.configKey == "simulation_timestep") {
                try {
                    double value = std::stod(args.configValue);
                    config->set("simulation_timestep", value, ConfigSource::User);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid double value for simulation_timestep");
                    return 1;
                }
            } else if (args.configKey == "stdp_ltp_weight") {
                try {
                    float value = std::stof(args.configValue);
                    config->set("stdp_ltp_weight", value, ConfigSource::User);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid float value for stdp_ltp_weight");
                    return 1;
                }
            } else if (args.configKey == "stdp_ltd_weight") {
                try {
                    float value = std::stof(args.configValue);
                    config->set("stdp_ltd_weight", value, ConfigSource::User);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid float value for stdp_ltd_weight");
                    return 1;
                }
            } else if (args.configKey == "stdp_tau") {
                try {
                    float value = std::stof(args.configValue);
                    config->set("stdp_tau", value, ConfigSource::User);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid float value for stdp_tau");
                    return 1;
                }
            } else if (args.configKey == "synaptogenesis_rate") {
                try {
                    float value = std::stof(args.configValue);
                    config->set("synaptogenesis_rate", value, ConfigSource::User);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid float value for synaptogenesis_rate");
                    return 1;
                }
            } else if (args.configKey == "pruning_rate") {
                try {
                    float value = std::stof(args.configValue);
                    config->set("pruning_rate", value, ConfigSource::User);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid float value for pruning_rate");
                    return 1;
                }
            } else {
                NLM_LOG_ERROR("Unsupported configuration key: " + args.configKey);
                return 1;
            }
            
            NLM_LOG_INFO("Configuration key " + args.configKey + " set to " + args.configValue);
        } else {
            // Get configuration value
            if (args.configKey == "random_seed") {
                auto value = config->getOr<int64_t>("random_seed", 42);
                std::cout << "random_seed: " << value << std::endl;
            } else if (args.configKey == "neuron_count") {
                auto value = config->getOr<int64_t>("neuron_count", 1000);
                std::cout << "neuron_count: " << value << std::endl;
            } else if (args.configKey == "region_count") {
                auto value = config->getOr<int64_t>("region_count", 1);
                std::cout << "region_count: " << value << std::endl;
            } else if (args.configKey == "connection_probability") {
                auto value = config->getOr<float>("connection_probability", 0.1f);
                std::cout << "connection_probability: " << value << std::endl;
            } else if (args.configKey == "simulation_timestep") {
                auto value = config->getOr<double>("simulation_timestep", 0.001);
                std::cout << "simulation_timestep: " << value << std::endl;
            } else if (args.configKey == "stdp_ltp_weight") {
                auto value = config->getOr<float>("stdp_ltp_weight", 0.01f);
                std::cout << "stdp_ltp_weight: " << value << std::endl;
            } else if (args.configKey == "stdp_ltd_weight") {
                auto value = config->getOr<float>("stdp_ltd_weight", 0.012f);
                std::cout << "stdp_ltd_weight: " << value << std::endl;
            } else if (args.configKey == "stdp_tau") {
                auto value = config->getOr<float>("stdp_tau", 20.0f);
                std::cout << "stdp_tau: " << value << std::endl;
            } else if (args.configKey == "synaptogenesis_rate") {
                auto value = config->getOr<float>("synaptogenesis_rate", 0.0001f);
                std::cout << "synaptogenesis_rate: " << value << std::endl;
            } else if (args.configKey == "pruning_rate") {
                auto value = config->getOr<float>("pruning_rate", 0.00001f);
                std::cout << "pruning_rate: " << value << std::endl;
            } else {
                NLM_LOG_ERROR("Unsupported configuration key: " + args.configKey);
                return 1;
            }
        }
        return 0;
    } else if (args.command == "status") {
        brain->logStatus();
        return 0;
    } else if (args.command == "stats") {
        NLM_LOG_INFO("=== NLM Performance Statistics ===");
        NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
        NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
        NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(brain->getFiringNeuronCount()));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
        NLM_LOG_INFO("Excitatory/Inhibitory ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
        return 0;
    } else if (args.command == "simulate") {
        NLM_LOG_INFO("Running NLM simulation for " + std::to_string(args.simulationSteps) + " steps...");
        for (SimulationStep step = 0; step < static_cast<SimulationStep>(args.simulationSteps); ++step) {
            brain->step(step, step * timestep);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        brain->logStatus();
        NLM_LOG_INFO("NLM simulation complete");
        return 0;
    } else {
        NLM_LOG_ERROR("Unknown command: " + args.command);
        printUsage(argv[0]);
        return 1;
    }
}