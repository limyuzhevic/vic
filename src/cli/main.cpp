// Command-line interface (CLI) for NLM (Neural Learning Machine)
// Main entry point with argument parsing and command execution

#include "cli_parser.hpp"
#include "simulation_controller.hpp"
#include "config_manager.hpp"
#include "output_handler.hpp"
#include "experiment_runner.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <thread>
#include <csignal>
#include <string>
#include <map>

using namespace nlm;

// Global simulation controller and config manager
std::shared_ptr<Config> globalConfig;
std::unique_ptr<SimulationController> simulationController;
std::unique_ptr<ConfigManager> configManager;
std::unique_ptr<OutputHandler> outputHandler;
std::unique_ptr<ExperimentRunner> experimentRunner;

// Signal handler for graceful shutdown
volatile bool interruptRequested = false;

void signalHandler(int signum) {
    interruptRequested = true;
    if (outputHandler) {
        outputHandler->printInfo("Interrupt received, shutting down gracefully...");
    }
}

// Setup signal handlers
void setupSignalHandlers() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
}

// Initialize global components
void initializeComponents() {
    globalConfig = std::make_shared<Config>();
    simulationController = std::make_unique<SimulationController>(globalConfig);
    configManager = std::make_unique<ConfigManager>(globalConfig);
    outputHandler = std::make_unique<OutputHandler>();
    experimentRunner = std::make_unique<ExperimentRunner>(globalConfig);
    
    // Set up signal handlers
    setupSignalHandlers();
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    if (outputHandler) {
        outputHandler->printInfo("NLM CLI initialized successfully");
    }
}

// Parse simple command line arguments
void parseCommandLine(int argc, char** argv, std::string& command, std::map<std::string, std::string>& flags, std::vector<std::string>& args) {
    if (argc <= 1) {
        return;
    }
    
    command = argv[1];
    
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.find('=') != std::string::npos) {
            size_t equalsPos = arg.find('=');
            std::string key = arg.substr(0, equalsPos);
            std::string value = arg.substr(equalsPos + 1);
            flags[key] = value;
        } else if (arg.substr(0, 2) == "--") {
            flags[arg] = "true";
        } else {
            args.push_back(arg);
        }
    }
}

int main(int argc, char** argv) {
    try {
        // Print banner
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═════════════════════════════════════════════════════════════╝
    )" << std::endl;
        
        // Initialize components
        initializeComponents();
        
        std::string command;
        std::map<std::string, std::string> flags;
        std::vector<std::string> args;
        
        parseCommandLine(argc, argv, command, flags, args);
        
        if (command.empty()) {
            std::cout << "No command provided. Use 'nlm --help' for help." << std::endl;
            return 0;
        }
        
        // Process commands
        if (command == "--help" || command == "-h") {
            std::cout << "NLM (Neural Learning Machine) CLI" << std::endl;
            std::cout << "===============================================" << std::endl;
            std::cout << std::endl;
            std::cout << "Basic Commands:" << std::endl;
            std::cout << "  nlm --help                Show help information" << std::endl;
            std::cout << "  nlm version               Display version information" << std::endl;
            std::cout << "  nlm --verbose             Enable verbose logging" << std::endl;
            std::cout << std::endl;
            std::cout << "Available Commands:" << std::endl;
            
            // Simple command help
            std::map<std::string, std::string> commandHelp = {
                {"--help", "Show help information"},
                {"version", "Display version information"},
                {"--verbose", "Enable verbose logging"},
                {"--config", "Load configuration from file"},
                {"--set", "Set configuration parameter"},
                {"--show-config", "Display current configuration"},
                {"run", "Run simulation for specified steps"},
                {"step", "Execute specified number of steps"},
                {"save", "Save brain state to file"},
                {"load", "Load brain state from file"},
                {"export-stats", "Export simulation statistics"},
                {"print-status", "Display current brain status"},
                {"experiment", "Run predefined experiment"},
                {"benchmark", "Run performance benchmarks"},
                {"validate", "Validate configuration and system integrity"},
                {"simulate", "Create and simulate world"},
                {"episode", "Run a complete agent episode"},
                {"train", "Train agent for specified episodes"}
            };
            
            for (const auto& cmd : commandHelp) {
                std::cout << "  " << cmd.first << " - " << cmd.second << std::endl;
            }
            std::cout << std::endl;
            std::cout << "Examples:" << std::endl;
            std::cout << "  nlm run --steps 100 --with-agent" << std::endl;
            std::cout << "  nlm experiment phase3 500" << std::endl;
            std::cout << "  nlm benchmark --name basic_connectivity" << std::endl;
            return 0;
        }
        
        if (command == "version") {
            std::cout << "NLM CLI Version 0.1.0" << std::endl;
            std::cout << "Built: " << __DATE__ << " " << __TIME__ << std::endl;
            std::cout << "C++ Standard: C++20" << std::endl;
            return 0;
        }
        
        if (command == "--verbose") {
            if (simulationController) {
                simulationController->setVerbose(true);
            }
            if (configManager) {
                configManager->getConfig()->set("verbose", true, ConfigSource::CommandLine);
            }
            std::cout << "Verbose mode enabled" << std::endl;
            return 0;
        }
        
        if (command == "--config") {
            std::string configFile = flags["--config"];
            if (!configFile.empty() && configManager) {
                if (configManager->loadFromFile(configFile)) {
                    std::cout << "Configuration loaded from: " << configFile << std::endl;
                } else {
                    std::cerr << "Failed to load configuration from: " << configFile << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Missing configuration file for --config command" << std::endl;
                return 1;
            }
            return 0;
        }
        
        if (command == "--set") {
            std::string key = flags["--set"];
            std::string value = flags["value"];
            if (!key.empty() && !value.empty() && configManager) {
                if (configManager->setParameter(key, value)) {
                    std::cout << "Configuration parameter set: " << key << " = " << value << std::endl;
                } else {
                    std::cerr << "Failed to set parameter: " << key << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Missing arguments for --set command" << std::endl;
                return 1;
            }
            return 0;
        }
        
        if (command == "--show-config") {
            if (configManager) {
                configManager->showConfig();
                return 0;
            }
            std::cerr << "Configuration manager not initialized" << std::endl;
            return 1;
        }
        
        if (command == "run") {
            size_t steps = 100;
            bool withAgent = false;
            std::string brainConfig;
            std::string worldConfig;
            
            for (const auto& flag : flags) {
                if (flag.first == "--with-agent") {
                    withAgent = true;
                } else if (flag.first == "--brain") {
                    brainConfig = flag.second;
                } else if (flag.first == "--world-size" && flag.second.find(' ') != std::string::npos) {
                    worldConfig = flag.second;
                } else if (flag.first == "--vision" && flag.second.find(' ') != std::string::npos) {
                    worldConfig += " vision:" + flag.second;
                }
            }
            
            for (const auto& arg : args) {
                try {
                    steps = std::stoul(arg);
                } catch (...) {
                    // Ignore non-numeric arguments
                }
            }
            
            if (simulationController) {
                CommandResult result = simulationController->runSteps(steps, withAgent, brainConfig, worldConfig);
                std::cout << result.output;
                if (!result.success) {
                    std::cerr << "Error: " << result.errorMessage << std::endl;
                }
                return result.exitCode;
            }
            return 1;
        }
        
        if (command == "step") {
            if (!args.empty()) {
                size_t steps = 1;
                try {
                    steps = std::stoul(args[0]);
                } catch (...) {
                    std::cerr << "Invalid number of steps: " << args[0] << std::endl;
                    return 1;
                }
                
                if (simulationController) {
                    CommandResult result = simulationController->step(steps);
                    std::cout << result.output;
                    if (!result.success) {
                        std::cerr << "Error: " << result.errorMessage << std::endl;
                    }
                    return result.exitCode;
                }
            } else {
                std::cerr << "Missing steps argument for step command" << std::endl;
                return 1;
            }
            return 0;
        }
        
        if (command == "save") {
            if (!args.empty()) {
                if (simulationController) {
                    CommandResult result = simulationController->saveBrain(args[0]);
                    std::cout << result.output;
                    if (!result.success) {
                        std::cerr << "Error: " << result.errorMessage << std::endl;
                    }
                    return result.exitCode;
                }
            } else {
                std::cerr << "Missing filepath argument for save command" << std::endl;
                return 1;
            }
            return 0;
        }
        
        if (command == "load") {
            if (!args.empty()) {
                if (simulationController) {
                    CommandResult result = simulationController->loadBrain(args[0]);
                    std::cout << result.output;
                    if (!result.success) {
                        std::cerr << "Error: " << result.errorMessage << std::endl;
                    }
                    return result.exitCode;
                }
            } else {
                std::cerr << "Missing filepath argument for load command" << std::endl;
                return 1;
            }
            return 0;
        }
        
        if (command == "export-stats") {
            if (!args.empty()) {
                if (simulationController) {
                    CommandResult result = simulationController->exportStats(args[0]);
                    std::cout << result.output;
                    if (!result.success) {
                        std::cerr << "Error: " << result.errorMessage << std::endl;
                    }
                    return result.exitCode;
                }
            } else {
                std::cerr << "Missing filepath argument for export-stats command" << std::endl;
                return 1;
            }
            return 0;
        }
        
        if (command == "print-status") {
            if (simulationController) {
                simulationController->printStatus();
                return 0;
            }
            std::cerr << "Simulation controller not initialized" << std::endl;
            return 1;
        }
        
        if (command == "experiment") {
            if (args.size() >= 2) {
                if (experimentRunner) {
                    CommandResult result = experimentRunner->runExperiment(args[0], std::stoul(args[1]));
                    std::cout << result.output;
                    if (!result.success) {
                        std::cerr << "Error: " << result.errorMessage << std::endl;
                    }
                    return result.exitCode;
                }
            } else {
                std::cerr << "Missing arguments for experiment command" << std::endl;
                return 1;
            }
            return 0;
        }
        
        if (command == "benchmark") {
            std::string benchmarkName = "basic_connectivity";
            size_t steps = 1000;
            
            if (flags.find("--name") != flags.end()) {
                benchmarkName = flags["--name"];
            }
            if (flags.find("--steps") != flags.end()) {
                try {
                    steps = std::stoul(flags["--steps"]);
                } catch (...) {
                    std::cerr << "Invalid number of steps: " << flags["--steps"] << std::endl;
                    return 1;
                }
            }
            
            if (experimentRunner) {
                CommandResult result = experimentRunner->runBenchmark(benchmarkName, steps);
                std::cout << result.output;
                if (!result.success) {
                    std::cerr << "Error: " << result.errorMessage << std::endl;
                }
                return result.exitCode;
            }
            return 1;
        }
        
        if (command == "validate") {
            if (experimentRunner) {
                CommandResult result = experimentRunner->validate();
                std::cout << result.output;
                if (!result.success) {
                    std::cerr << "Error: " << result.errorMessage << std::endl;
                }
                return result.exitCode;
            }
            std::cerr << "Experiment runner not initialized" << std::endl;
            return 1;
        }
        
        if (command == "simulate") {
            std::string worldConfig;
            for (const auto& flag : flags) {
                if (flag.first == "--world-size") {
                    worldConfig = flag.second;
                } else if (flag.first == "--vision") {
                    worldConfig += " vision:" + flag.second;
                }
            }
            
            std::cout << "World configuration: " << worldConfig << std::endl;
            std::cout << "Note: World simulation not yet fully implemented. Use 'nlm run --with-agent' instead." << std::endl;
            return 0;
        }
        
        if (command == "episode") {
            if (!args.empty()) {
                size_t steps = 100;
                try {
                    steps = std::stoul(args[0]);
                } catch (...) {
                    std::cerr << "Invalid number of steps: " << args[0] << std::endl;
                    return 1;
                }
                
                if (simulationController) {
                    CommandResult result = simulationController->runEpisode(steps);
                    std::cout << result.output;
                    if (!result.success) {
                        std::cerr << "Error: " << result.errorMessage << std::endl;
                    }
                    return result.exitCode;
                }
            } else {
                std::cerr << "Missing steps argument for episode command" << std::endl;
                return 1;
            }
            return 0;
        }
        
        if (command == "train") {
            if (!args.empty()) {
                size_t episodes = 10;
                try {
                    episodes = std::stoul(args[0]);
                } catch (...) {
                    std::cerr << "Invalid number of episodes: " << args[0] << std::endl;
                    return 1;
                }
                
                if (simulationController) {
                    CommandResult result = simulationController->train(episodes);
                    std::cout << result.output;
                    if (!result.success) {
                        std::cerr << "Error: " << result.errorMessage << std::endl;
                    }
                    return result.exitCode;
                }
            } else {
                std::cerr << "Missing episodes argument for train command" << std::endl;
                return 1;
            }
            return 0;
        }
        
        // Unknown command
        std::cerr << "Unknown command: " << command << std::endl;
        std::cerr << "Use 'nlm --help' for available commands" << std::endl;
        return 1;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}