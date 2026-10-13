// NLM (熙然) - Neural Learning Machine
// Main entry point for core neural simulation functionality
//
// This file contains the core initialization and main execution loop
// for the NLM neural simulation framework. It focuses on:
// - Loading configuration
// - Initializing the brain
// - Running the main simulation loop
// - Basic error handling and logging

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>
#include <string>

using namespace nlm;

/**
 * Print the main application banner
 */
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Main Neural Simulation                                    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

/**
 * Initialize and run a simple test simulation
 */
int runCoreSimulation(const std::shared_ptr<Config>& config) {
    try {
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);

        NLM_LOG_INFO("=== NLM Core Neural Simulation ===");

        // Create and initialize brain
        auto brain = std::make_shared<Brain>(config);

        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }

        brain->logStatus();

        // Run core simulation
        NLM_LOG_INFO("Running core simulation...");
        const size_t totalSteps = 1000;

        for (SimulationStep step = 0; step < totalSteps; ++step) {
            brain->step(step, step * 0.001);

            // Periodic logging for progress tracking
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) +
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
            }
        }

        // Log final status
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Core Simulation Complete ===");
        NLM_LOG_INFO("Total simulation steps: " + std::to_string(totalSteps));
        NLM_LOG_INFO("Total spikes generated: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));

        return 0;

    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error: " + std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("Unknown error occurred");
        return 1;
    }
}

/**
 * Main application entry point
 */
int main(int argc, char** argv) {
    printBanner();

    std::cout << "Initializing NLM Core Neural Simulation..." << std::endl;

    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    // Load configuration
    auto config = std::make_shared<Config>();

    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
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

    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }

    // Override with command line args
    config->loadFromArgs(argc, argv);

    // Set default values for core simulation
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);

    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));

    // Run core simulation
    return runCoreSimulation(config);
}