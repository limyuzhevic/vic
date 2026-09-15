// NLM (熙然) - Neural Learning Machine
// Command-line interface for running simulations
// Provides a unified interface for NLM simulations with configuration management

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <chrono>
#include <thread>
#include <csignal>

using namespace nlm;

// Global signal handling
volatile sig_atomic_t running = 1;

void signalHandler(int signum) {
    running = 0;
}

void printUsage() {
    std::cout << "NLM - Neural Learning Machine Command Line Interface" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm_run [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --config <file>     Configuration file path (default: configs/default.cfg)" << std::endl;
    std::cout << "  --help              Show this help message" << std::endl;
    std::cout << "  --version           Show version information" << std::endl;
    std::cout << "  --validate-only     Only validate configuration, don't run simulation" << std::endl;
    std::cout << "  --quiet             Suppress output logs" << std::endl;
    std::cout << "  --benchmark         Run benchmark mode (short simulation, timed)" << std::endl;
    std::cout << "  --seed <value>      Random seed (overrides config)" << std::endl;
    std::cout << "  --steps <value>     Number of simulation steps to run" << std::endl;
    std::cout << "  --help-advanced     Show advanced options" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced Options:" << std::endl;
    std::cout << "  --log-level <level> Log level: TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL" << std::endl;
    std::cout << "  --enable-plasticity Enable synaptic plasticity" << std::endl;
    std::cout << "  --save-path <file>  Save brain state to file after simulation" << std::endl;
    std::cout << "  --load-path <file>  Load brain state from file before simulation" << std::endl;
    std::cout << "  --environment <name> Environment name" << std::endl;
    std::cout << "  --experiment <name> Experiment type to run" << std::endl;
    std::cout << "  --interactive       Enable interactive mode (if supported)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_run --config myconfig.cfg --steps 10000" << std::endl;
    std::cout << "  nlm_run --benchmark --seed 12345" << std::endl;
    std::cout << "  nlm_run --load-path checkpoint.brn --save-path final.brn" << std::endl;
}

void printVersion() {
    std::cout << "NLM 0.1.0 - Neural Learning Machine" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
}

void runSimulation(const std::shared_ptr<Config>& config, bool benchmarkMode, 
                  int steps, const std::string& savePath) {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    NLM_LOG_INFO("=== NLM Simulation Started ===");
    
    if (benchmarkMode) {
        NLM_LOG_INFO("Running in BENCHMARK mode - short performance test");
    } else {
        NLM_LOG_INFO("Running in STANDARD mode");
    }

    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }

    brain->logStatus();

    // Get step count from config or parameter
    size_t totalSteps = steps > 0 ? static_cast<size_t>(steps) : 
                       config->getOr<size_t>("max_simulation_steps", 10000);

    NLM_LOG_INFO("Running " + std::to_string(totalSteps) + " simulation steps");

    // Record start time for benchmark
    auto startTime = std::chrono::high_resolution_clock::now();

    // Run simulation
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    for (SimulationStep step = 0; step < totalSteps && running; ++step) {
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        // Log progress every 1000 steps
        if (step % 1000 == 0 && step > 0) {
            NLM_LOG_INFO("Progress: Step " + std::to_string(step) + "/" + std::to_string(totalSteps) +
                         " | Spikes: " + std::to_string(brain->getTotalSpikeCount() - initialSpikes));
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    // Record final statistics
    size_t finalSpikes = brain->getTotalSpikeCount();
    size_t spikeCount = finalSpikes - initialSpikes;

    NLM_LOG_INFO("=== Simulation Complete ===");
    NLM_LOG_INFO("Duration: " + std::to_string(duration.count()) + " ms");
    NLM_LOG_INFO("Steps completed: " + std::to_string(totalSteps));
    NLM_LOG_INFO("Total spikes generated: " + std::to_string(spikeCount));
    NLM_LOG_INFO("Spikes per second: " + std::to_string(spikeCount * 1000.0 / duration.count()));

    if (benchmarkMode) {
        NLM_LOG_INFO("Benchmark completed successfully");
    }

    // Save brain state if requested
    if (!savePath.empty()) {
        NLM_LOG_INFO("Saving brain state to: " + savePath);
        if (brain->save(savePath)) {
            NLM_LOG_INFO("Brain state saved successfully");
        } else {
            NLM_LOG_ERROR("Failed to save brain state");
        }
    }

    brain->logStatus();

    NLM_LOG_INFO("=== NLM Simulation Finished ===");
}

int main(int argc, char** argv) {
    // Set up signal handling
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Default values
    std::string configFile = "configs/default.cfg";
    bool validateOnly = false;
    bool quiet = false;
    bool benchmarkMode = false;
    int steps = -1;
    int64_t seed = -1;
    std::string logLevel = "INFO";
    bool enablePlasticity = false;
    std::string savePath;
    std::string loadPath;
    std::string environmentName = "GridWorld";
    std::string experimentName = "";
    bool interactive = false;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);

        if (arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "--help-advanced") {
            printUsage(); // For now, same as help
            return 0;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--validate-only") {
            validateOnly = true;
        } else if (arg == "--quiet") {
            quiet = true;
        } else if (arg == "--benchmark") {
            benchmarkMode = true;
        } else if (arg.substr(0, 6) == "--seed") {
            if (arg.find('=') != std::string::npos) {
                seed = std::stoll(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                seed = std::stoll(argv[++i]);
            }
        } else if (arg.substr(0, 7) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                steps = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                steps = std::stoi(argv[++i]);
            }
        } else if (arg.substr(0, 10) == "--log-level") {
            if (arg.find('=') != std::string::npos) {
                logLevel = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                logLevel = argv[++i];
            }
        } else if (arg == "--enable-plasticity") {
            enablePlasticity = true;
        } else if (arg.substr(0, 10) == "--save-path") {
            if (arg.find('=') != std::string::npos) {
                savePath = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                savePath = argv[++i];
            }
        } else if (arg.substr(0, 10) == "--load-path") {
            if (arg.find('=') != std::string::npos) {
                loadPath = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                loadPath = argv[++i];
            }
        } else if (arg.substr(0, 13) == "--environment") {
            if (arg.find('=') != std::string::npos) {
                environmentName = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                environmentName = argv[++i];
            }
        } else if (arg.substr(0, 11) == "--experiment") {
            if (arg.find('=') != std::string::npos) {
                experimentName = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                experimentName = argv[++i];
            }
        } else if (arg == "--interactive") {
            interactive = true;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage();
            return 1;
        }
    }

    // Set log level
    if (quiet) {
        LogLevel level = LogLevel::Error;
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(level);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    } else {
        LogLevel level;
        if (logLevel == "TRACE") level = LogLevel::Trace;
        else if (logLevel == "DEBUG") level = LogLevel::Debug;
        else if (logLevel == "INFO") level = LogLevel::Info;
        else if (logLevel == "WARN") level = LogLevel::Warn;
        else if (logLevel == "ERROR") level = LogLevel::Error;
        else if (logLevel == "CRITICAL") level = LogLevel::Critical;
        else level = LogLevel::Info;
        
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(level);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }

    // Load configuration
    auto config = std::make_shared<Config>();
    
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_WARN("Using default configuration (file not found: " + configFile + ")");
    }

    // Set defaults for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);

    // Override with command line
    if (seed != -1) {
        config->set("random_seed", seed, ConfigSource::CommandLine);
    }

    if (steps > 0) {
        config->set("max_simulation_steps", static_cast<int64_t>(steps), ConfigSource::CommandLine);
    }

    // Load brain if checkpoint provided
    std::shared_ptr<Brain> brain;
    if (!loadPath.empty()) {
        NLM_LOG_INFO("Loading brain state from: " + loadPath);
        // Create a temporary brain for loading
        brain = std::make_shared<Brain>(config);
        if (brain->load(loadPath)) {
            NLM_LOG_INFO("Brain state loaded successfully");
        } else {
            NLM_LOG_ERROR("Failed to load brain state, starting fresh");
            brain.reset();
        }
    }

    if (!brain) {
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
    }

    // Validate configuration
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO(config->summary());

    if (validateOnly) {
        NLM_LOG_INFO("Configuration validation complete - simulation skipped (--validate-only)");
        brain->logStatus();
        return 0;
    }

    // Run simulation
    runSimulation(config, benchmarkMode, steps, savePath);

    return 0;
}
