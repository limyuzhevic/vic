// NLM (熙然) - Neural Learning Machine
// Phase 6: Complete Command Line Interface
//
// This phase implements a comprehensive command-line interface with:
// - Multiple execution modes (interactive, run, train, benchmark)
// - Configuration management
// - Command history and help system
// - Output management and checkpoint support

#include "commandline/CommandLineInterface.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/ConsoleLogger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Complete Command Line Interface                  ║
    ║                                                               ║
    ║     Full AI brain with advanced CLI and integration.          ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║     - Comprehensive command-line interface                    ║
    ║     - Advanced configuration management                       ║
    ║     - Multiple execution modes                                 ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printVersion() {
    std::cout << "NLM Phase 6 - Integrated Artificial Brain" << std::endl;
    std::cout << "Version 2.0.0 - Complete CLI Edition" << std::endl;
    std::cout << "Real spiking neural computation with full integration" << std::endl;
    std::cout << "Comprehensive command-line interface with advanced features" << std::endl;
}

void printHelp() {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  (no mode)              Interactive mode (default)" << std::endl;
    std::cout << "  --run                  Run basic neural network" << std::endl;
    std::cout << "  --train                Training mode" << std::endl;
    std::cout << "  --benchmark            Benchmark mode" << std::endl;
    std::cout << "  --visualize            Visualization mode" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --config=FILE          Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --steps=N              Number of simulation steps" << std::endl;
    std::cout << "  --timestep=N           Simulation timestep" << std::endl;
    std::cout << "  --output=FILE          Output file for results" << std::endl;
    std::cout << "  --save-state           Save brain state at the end" << std::endl;
    std::cout << "  --checkpoint=FILE      Save/load checkpoint" << std::endl;
    std::cout << "  --load-checkpoint=N    Load checkpoint N" << std::endl;
    std::cout << "  --set=KEY=VALUE        Set custom configuration value" << std::endl;
    std::cout << "  --verbose              Enable verbose output" << std::endl;
    std::cout << "  -h, --help             Show this help message" << std::endl;
    std::cout << "  --version              Show version information" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --run --steps=1000 --config=my_config.cfg" << std::endl;
    std::cout << "  nlm --benchmark --timestep=0.001 --output=benchmark.csv" << std::endl;
    std::cout << "  nlm --train --steps=5000 --checkpoint=brain_state" << std::endl;
}

int main(int argc, char** argv) {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    printBanner();

    // Parse command line
    CommandLineParser::Options options = CommandLineParser::parse(argc, argv);

    if (options.help) {
        printHelp();
        return 0;
    }

    if (options.version) {
        printVersion();
        return 0;
    }

    // Load configuration
    auto config = std::make_shared<Config>();

    // Try to load from file
    std::string configFile = options.configFile;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }

    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }

    // Set default values for Phase 6
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", options.timestep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);

    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);

    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6 - Integrated)...");
    auto brain = std::make_shared<Brain>(config);

    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }

    brain->logStatus();

    // Determine mode if not explicitly set
    std::string mode = options.mode;
    if (mode == "interactive") {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg == "--run") mode = "run";
            else if (arg == "--train") mode = "train";
            else if (arg == "--benchmark") mode = "benchmark";
            else if (arg == "--visualize") mode = "visualize";
        }
    }

    // Create command interface based on mode
    auto command = CommandFactory::createCommand(mode);
    if (!command) {
        NLM_LOG_ERROR("Unknown mode: " + mode);
        printHelp();
        return 1;
    }

    NLM_LOG_INFO("Starting " + command->getName() + " mode...");

    // Execute the command
    command->execute(brain, options);

    NLM_LOG_INFO("Command completed successfully!");

    return 0;
}
