// NLM (熙然) - Neural Learning Machine
// Unified Command Line Interface with comprehensive documentation
// Provides access to all NLM phases with advanced configuration options

#include "core/CommandLineInterface.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Unified Command Line Interface                           ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

/**
 * @brief Create and configure experiment runner with all available experiments
 * 
 * This function registers all NLM experiments from different phases:
 * - Phase 2: Basic neural computation
 * - Phase 3: World interaction  
 * - Phase 4: Full cognitive system
 * - Phase 6: Integration and final testing
 * 
 * @return Shared pointer to initialized ExperimentRunner
 */
std::shared_ptr<ExperimentRunner> createExperimentRunner() {
    auto runner = std::make_shared<ExperimentRunner>();
    
    // Register available experiments
    // Phase 2: Basic neural computation with real spiking neurons
    auto phase2Experiment = std::make_shared<Experiment>();
    phase2Experiment->setName("phase2_basic");
    phase2Experiment->setDescription("Real neural computation with LIF neurons, STDP, and Hebbian plasticity");
    runner->registerExperiment(phase2Experiment);
    
    // Phase 3: World interaction with sensory-motor loop
    auto phase3Experiment = std::make_shared<Experiment>();
    phase3Experiment->setName("phase3_world");
    phase3Experiment->setDescription("Sensory-motor loop with reward prediction and developmental stages");
    runner->registerExperiment(phase3Experiment);
    
    // Phase 4: Full cognitive architecture with memory and planning
    auto phase4Experiment = std::make_shared<Experiment>();
    phase4Experiment->setName("phase4_cognitive");
    phase4Experiment->setDescription("Full cognitive architecture with memory, prediction, and planning");
    runner->registerExperiment(phase4Experiment);
    
    // Phase 6: Integration test for all systems
    auto phase6Experiment = std::make_shared<Phase6IntegratedExperiment>();
    phase6Experiment->setName("phase6_integration");
    phase6Experiment->setDescription("Complete integration of all brain systems");
    runner->registerExperiment(phase6Experiment);
    
    return runner;
}

/**
 * @brief Print comprehensive system information
 * 
 * Displays version information, phase status, and usage instructions
 */
void printSystemInfo() {
    std::cout << "\n=== NLM System Information ===" << std::endl;
    std::cout << "Version: NLM 0.1.0 (Phase 6 - Final Integration)" << std::endl;
    std::cout << "Description: Experimental artificial developmental brain" << std::endl;
    std::cout << "Architecture: 23 specialized modules with 280+ files" << std::endl;
    std::cout << "Core capabilities: Real neural computation, learning, memory, cognition" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: nlm [options] [command] [arguments]" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  --batch                    Run commands from command line" << std::endl;
    std::cout << "  --command=<command>        Execute single command (quoted)" << std::endl;
    std::cout << "  --config=<file>            Load configuration from file" << std::endl;
    std::cout << "  --loglevel=<level>         Set log level (debug, info, warning, error)" << std::endl;
    std::cout << "  --help                     Show this help message" << std::endl;
    std::cout << "  --version                  Show version information" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Commands: Type 'help' for available commands" << std::endl;
    std::cout << "" << std::endl;
}

/**
 * @brief Parse command line arguments and configure NLM
 * 
 * Handles all command line options for unified CLI interface
 * @param argc Argument count
 * @param argv Argument values
 * @param batchMode Output parameter for batch mode flag
 * @param singleCommand Output parameter for single command
 * @param configFile Output parameter for config file path
 * @param logLevelStr Output parameter for log level string
 */
void parseCommandLineArguments(int argc, char** argv, bool& batchMode, 
                               std::string& singleCommand, std::string& configFile,
                               std::string& logLevelStr) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            std::cout << "=== NLM Help ===" << std::endl;
            printSystemInfo();
            exit(0);
        } else if (arg == "--version" || arg == "-v") {
            std::cout << "NLM - Neural Learning Machine v0.1.0" << std::endl;
            std::cout << "Phase 6: Final Integration of all brain systems" << std::endl;
            std::cout << "Built with C++20 and modern neuroscience algorithms" << std::endl;
            exit(0);
        } else if (arg == "--batch") {
            batchMode = true;
        } else if (arg.find("--command=") == 0) {
            singleCommand = arg.substr(10);
        } else if (arg.find("--config=") == 0) {
            configFile = arg.substr(9);
        } else if (arg.find("--loglevel=") == 0) {
            logLevelStr = arg.substr(11);
        } else if (arg.find("--") == 0) {
            // Unknown option - handled by CLI
        }
    }
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Neural Learning Machine...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Unified Command Line Interface ===");
    NLM_LOG_INFO("Starting NLM with advanced command-line interface");
    NLM_LOG_INFO("Phase 6: Final Integration of all brain systems");
    NLM_LOG_INFO("Ready for unified neural simulation with comprehensive command interface");
    NLM_LOG_INFO("");
    
    // Parse command line arguments
    bool batchMode = false;
    std::string singleCommand;
    std::string configFile = "configs/default.cfg";
    std::string logLevelStr = "info";
    
    parseCommandLineArguments(argc, argv, batchMode, singleCommand, 
                              configFile, logLevelStr);
    
    // Configure log level
    if (logLevelStr == "debug") {
        consoleLogger->setLevel(LogLevel::Debug);
    } else if (logLevelStr == "warning") {
        consoleLogger->setLevel(LogLevel::Warning);
    } else if (logLevelStr == "error") {
        consoleLogger->setLevel(LogLevel::Error);
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set comprehensive default values for unified interface
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("enable_checkpointing", true, ConfigSource::Default);
    config->set("enable_development", true, ConfigSource::Default);
    config->set("enable_replay", true, ConfigSource::Default);
    config->set("enable_prediction", true, ConfigSource::Default);
    config->set("enable_neuromodulation", true, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Development parameters
    config->set("maturation_rate", 0.001f, ConfigSource::Default);
    config->set("initial_development_stage", 0, ConfigSource::Default);
    
    // Neuromodulation parameters
    config->set("dopamine_sensitivity", 1.0f, ConfigSource::Default);
    config->set("curiosity_strength", 0.5f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("  enable_checkpointing: " + std::to_string(config->getOr<bool>("enable_checkpointing", true)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain with all systems...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Create experiment runner
    auto experimentRunner = createExperimentRunner();
    
    // Initialize Command Line Interface
    CommandLineInterface cli;
    cli.setConfig(config);
    cli.setBrain(brain);
    cli.setExperimentRunner(experimentRunner);
    
    // Execute single command if specified
    if (!singleCommand.empty()) {
        NLM_LOG_INFO("Executing single command: " + singleCommand);
        if (!cli.execute(singleCommand)) {
            std::cerr << "Error: Command failed" << std::endl;
            return 1;
        }
        NLM_LOG_INFO("Command completed successfully.");
    } else if (batchMode) {
        // In batch mode, show system info and wait for input
        printSystemInfo();
        NLM_LOG_INFO("Batch mode enabled - commands can be executed via --command= argument");
        NLM_LOG_INFO("Example: ./nlm --batch --command=\"brain status\"");
    } else {
        // Interactive mode
        std::cout << "\n=== NLM Interactive Mode ===\n" << std::endl;
        std::cout << "Type 'help' for available commands or press Ctrl+C to exit." << std::endl;
        std::cout << "\nCurrent working directory: /workspace/app" << std::endl;
        std::cout << "Brain is ready for neural simulation and command processing." << std::endl;
        
        std::string line;
        while (true) {
            std::cout << ">>> ";
            std::getline(std::cin, line);
            
            if (line.empty()) {
                continue;
            }
            
            if (line == "exit" || line == "quit") {
                std::cout << "Exiting NLM... Goodbye!" << std::endl;
                break;
            }
            
            cli.execute(line);
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Session Complete ===");
    NLM_LOG_INFO("Thank you for using NLM Neural Learning Machine");
    
    return 0;
}