#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "sensory/SensoryPercept.hpp"
#include "motor/Action.hpp"
#include "environment/ActionResult.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <memory>

using namespace nlm;

class CommandLineInterface {
public:
    CommandLineInterface() {
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }

    void printHelp() {
        std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
        std::cout << "===============================" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: nlm [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h          Show this help message" << std::endl;
        std::cout << "  --version           Show version information" << std::endl;
        std::cout << "  --config <file>     Load configuration from file" << std::endl;
        std::cout << "  --neurons <n>       Number of neurons (default: 500)" << std::endl;
        std::cout << "  --steps <n>         Number birra\", \"brain") << std::endl;
        std::cout << "  --mode <mode>       Run mode: 'phase2', 'phase3', 'phase4', 'phase6' (default: phase2)" << std::endl;
        std::cout << "  --verbose, -v        Enable verbose output" << std::endl;
        std::cout << "  --quiet, -q          Enable quiet output" << std::endl;
        std::cout << "  --list-commands      List available commands" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands (interactively):" << std::endl;
        std::cout << "  help                Show help" << std::endl;
        std::cout << "  status              Show current brain status" << std::endl;
        std::cout << "  step <n>            Step the brain n times" << std::endl;
        std::cout << "  reset               Reset the brain" << std::endl;
        std::cout << "  inject <id> <val>   Inject current into neuron" << std::endl;
        std::cout << "  simulate <n>        Run complete simulation with n steps" << std::endl;
        std::cout << "  config <key> <val>  Set configuration value" << std::endl;
        std::cout << "  exit, quit          Exit the application" << std::endl;
    }

    void printVersion() {
        std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
        std::cout << "Version: 0.1.0 (Phase 6)" << std::endl;
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "Author: NLM Research Team" << std::endl;
    }

    bool parseArguments(int argc, char** argv) {
        std::string mode = "phase2";
        int neurons = 500;
        int steps = 100;
        std::string configFile = "configs/default.cfg";
        bool verbose = false;
        bool quiet = false;

        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return false;
            } else if (arg == "--version") {
                printVersion();
                return false;
            } else if (arg == "--verbose" || arg == "-v") {
                verbose = true;
                if (quiet) quiet = false;
            } else if (arg == "--quiet" || arg == "-q") {
                quiet = true;
                if (verbose) verbose = false;
            } else if (arg == "--list-commands") {
                listCommands();
                return false;
            } else if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            } else if (arg == "--mode" && i + 1 < argc) {
                mode = argv[++i];
            } else if (arg == "--neurons" && i + 1 < argc) {
                neurons = std::stoi(argv[++i]);
            } else if (arg == "--steps" && i + 1 < argc) {
                steps = std::stoi(argv[++i]);
            } else if (arg == "--batch" && i + 1 < argc) {
                // Run batch mode with given steps
                int batchSteps = std::stoi(argv[++i]);
                if (!runBatchMode(mode, neurons, steps, configFile, verbose, quiet)) {
                    return false;
                }
                return false;
            }
        }

        // Interactive mode
        runInteractiveMode(mode, neurons, steps, configFile, verbose, quiet);
        return true;
    }

    void listCommands() {
        std::cout << "Available commands:" << std::endl;
        std::cout << "  help                Show this help" << std::endl;
        std::cout << "  status              Show brain status" << std::endl;
        std::cout << "  reset               Reset the brain" << std::endl;
        std::cout << "  step <n>            Advance brain by n steps" << std::endl;
        std::cout << "  inject <id> <val>   Inject current into neuron" << std::endl;
        std::cout << "  simulate <n>        Run simulation with n steps" << std::endl;
        std::cout << "  config <key> <val>  Set configuration value" << std::endl;
        std::cout << "  exit, quit          Exit" << std::endl;
    }

    bool runBatchMode(const std::string& mode, int neurons, int steps, 
                      const std::string& configFile, bool verbose, bool quiet) {
        NLM_LOG_INFO("Starting NLM in " + mode + " mode");
        NLM_LOG_INFO("Configuration: " + std::to_string(neurons) + " neurons, " + 
                     std::to_string(steps) + " steps");
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::CommandLine);
        cfg->set("simulation_timestep", 0.001, ConfigSource::CommandLine);
        
        // Load config from file if provided
        if (!configFile.empty() && configFile != "configs/default.cfg") {
            if (cfg->loadFromFile(configFile)) {
                NLM_LOG_INFO("Loaded configuration from: " + configFile);
            }
        }
        
        // Create brain
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        brain->logStatus();
        
        // Run simulation
        for (int step = 0; step < steps; ++step) {
            brain->step(step, step * 0argo().);
            
            if (verbose || quiet) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                            " | Total spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        NLM_LOG_INFO("Simulation completed");
        brain->logStatus();
        return true;
    }

    void runInteractiveMode(const std::string& mode, int neurons, int steps,
                           const std::string& configFile, bool verbose, bool quiet) {
        NLM_LOG_INFO("Entering interactive mode");
        NLM_LOG_INFO("Use 'help' for available commands.");
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::CommandLine);
        cfg->set("simulation_timestep", 0.001, ConfigSource::CommandLine);
        
        // Load config from file if provided
        if (!configFile.empty() && configFile != "configs/default.cfg") {
            if (cfg->loadFromFile(configFile)) {
                NLM_LOG_INFO("Loaded configuration from: " + configFile);
            }
        }
        
        // Create brain
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return;
        }
        
        brain->logStatus();
        
        // Interactive command loop
        std::string command;
        while (true) {
            std::cout << "> ";
            std::cin >> command;
            
            if (command == "help") {
                printHelp();
            } else if (command == "status") {
                brain->logStatus();
            } else if (command == "reset") {
                brain->reset();
                brain->initialize();
                brain->logStatus();
            } else if (command == "exit" || command == "quit") {
                break;
            } else if (command == "simulate") {
                int steps;
                std::cin >> steps;
                for (int step = 0; step < steps; ++step) {
                    brain->step(step, step * 0.001);
                }
                brain->logStatus();
            } else {
                NLM_LOG_ERROR("Unknown command: " + command);
            }
        }
    }
};

int main(int argc, char** argv) {
    CommandLineInterface cli;
    if (!cli.parseArguments(argc, argv)) {
        return 0;
    }
    return 0;
}