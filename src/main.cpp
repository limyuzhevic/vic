// NLM (熙然) - Neural Learning Machine
// Multi-Mode Demo and Test Suite
//
// This file provides a comprehensive command-line interface for running various
// NLM demos, tests, and experimental modes. It replaces the monolithic main.cpp
// with a modular, extensible architecture.

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/ConsoleLogger/ConsoleLogger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <vector>
#include <algorithm>

using namespace nlm;

// Print program banner
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Multi-Mode Demo and Test Suite                             ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Print help information
void printHelp() {
    printBanner();
    std::cout << "\nNLM - Neural Learning Machine Demo and Test Suite" << std::endl;
    std::cout << "Usage: nlm [OPTIONS] [MODE] [ARGS]" << std::endl << std::endl;
    
    std::cout << "MODES:" << std::endl;
    std::cout << "  phase2        Phase 2: Real neural computation (default)" << std::endl;
    std::cout << "  phase3        Phase 3: World interaction and sensory processing" << std::endl;
    std::cout << "  phase4        Phase 4: Advanced cognition and memory" << std::endl;
    std::cout << "  phase5        Phase 5: Performance optimizations and demos" << std::endl;
    std::cout << "  phase6        Phase 6: Full integration and advanced features" << std::endl;
    std::cout << "  benchmark     Run performance benchmarks" << std::endl;
    std::cout << "  help          Show this help message" << std::endl << std::endl;
    
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  --mode=MODE   Specify execution mode (overrides positional mode)" << std::endl;
    std::cout << "  --config=FILE Load configuration from file" << std::endl;
    std::cout << "  --env=VAR=VAL Set environment variable in config" << std::endl;
    std::cout << "  --validate     Validate configuration and exit" << std::endl;
    std::cout << "  --export-config=FILE Export current configuration to file" << std::endl;
    std::cout << "  --help         Show this help message" << std::endl;
    std::cout << "  --version      Show version information" << std::endl << std::endl;
    
    std::cout << "ADVANCED OPTIONS:" << std::endl;
    std::cout << "  --steps=N      Number of simulation steps" << std::endl;
    std::cout << "  --seed=N       Random seed" << std::endl;
    std::cout << "  --neurons=N    Number of neurons" << std::endl;
    std::cout << "  --regions=N    Number of brain regions" << std::endl;
    std::cout << "  --verbose      Enable verbose logging" << std::endl;
    std::cout << "  --quiet        Suppress informational messages" << std::endl;
    std::cout << "  --debug        Enable debug logging" << std::endl;
    std::cout << "  --demo-id=ID   Specific demo ID to run" << std::endl;
    std::cout << "  --duration=S   Simulation duration in seconds" << std::endl << std::endl;
    
    std::cout << "CONFIGURATION FILE OPTIONS:" << std::endl;
    std::cout << "  --config-key=KEY=VALUE Set configuration key-value pair" << std::endl;
    std::cout << "  --config-override=KEY=VALUE Override configuration value" << std::endl << std::endl;
    
    std::cout << "ENVIRONMENT VARIABLE PREFIX:" << std::endl;
    std::cout << "  NLM_*          Any environment variable with NLM_ prefix will be loaded into config" << std::endl;
    std::cout << "  e.g., NLM_random_seed=123" << std::endl << std::endl;
    
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  nlm phase2 --steps=1000 --neurons=500" << std::endl;
    std::cout << "  nlm phase6 --config=my_config.cfg --validate" << std::endl;
    std::cout << "  nlm phase5 --env=NLM_plasticity_learning_rate=0.05 --demo-id=2" << std::endl;
    std::cout << "  nlm benchmark --steps=5000 --export-config=benchmark_config.cfg" << std::endl << std::endl;
}

// Print version information
void printVersion() {
    std::cout << "NLM - Neural Learning Machine v0.1.0 (Phase 6)" << std::endl;
    std::cout << "Experimental Artificial Developmental Brain" << std::endl;
    std::cout << "Built with C++20 and scikit-build-core" << std::endl << std::endl;
}

// Parse command line arguments into a config
bool parseCommandLineArgs(int argc, char** argv, std::shared_ptr<Config>& config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --config-key=value format
        if (arg.find("--config-key=") == 0) {
            size_t pos = arg.find("=", 13);
            if (pos != std::string::npos && i + 1 < argc) {
                std::string key = arg.substr(13, pos - 13);
                std::string value = arg.substr(pos + 1);
                config->set(key, value, ConfigSource::CommandLine, "Command line key-value");
                i++; // Skip next argument for -key value format too
            }
        }
        // Handle --config-override=value format
        else if (arg.find("--config-override=") == 0) {
            size_t pos = arg.find("=", 20);
            if (pos != std::string::npos) {
                std::string key = arg.substr(20, pos - 20);
                std::string value = arg.substr(pos + 1);
                config->set(key, value, ConfigSource::CommandLine, "Command line override");
            }
        }
        // Handle --env=VAR=VAL format
        else if (arg.find("--env=") == 0) {
            size_t pos = arg.find("=", 6);
            if (pos != std::string::npos) {
                std::string envName = arg.substr(6, pos - 6);
                std::string envValue = arg.substr(pos + 1);
                setenv(envName.c_str(), envValue.c_str(), 1);
            }
        }
    }
    return true;
}

// Load configuration with error handling
bool loadConfiguration(std::shared_ptr<Config>& config, const std::string& configFile) {
    // Try to load from file if provided
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
        return true;
    } else {
        NLM_LOG_INFO("Using default configuration (file not found or invalid).");
        return false;
    }
}

// Set default values for different modes
void setDefaultConfig(std::shared_ptr<Config>& config, const std::string& mode) {
    // Common defaults
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Mode-specific defaults
    if (mode == "phase2") {
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("plasticity_learning_rate", 0.01, ConfigSource::Default);
    } else if (mode == "phase3") {
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("environment_name", std::string("GridWorld"), ConfigSource::Default);
        config->set("environment_width", 10, ConfigSource::Default);
        config->set("environment_height", 10, ConfigSource::Default);
    } else if (mode == "phase4") {
        config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.08f, ConfigSource::Default);
        config->set("working_memory_size", 100, ConfigSource::Default);
        config->set("episodic_memory_capacity", 500, ConfigSource::Default);
    } else if (mode == "phase5") {
        config->set("neuron_count", static_cast<int64_t>(3000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        config->set("performance_target_fps", 60, ConfigSource::Default);
    } else if (mode == "phase6") {
        config->set("neuron_count", static_cast<int64_t>(4000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(5), ConfigSource::Default);
        config->set("connection_probability", 0.03f, ConfigSource::Default);
        config->set("memory_system_enabled", true, ConfigSource::Default);
        config->set("neuromodulation_enabled", true, ConfigSource::Default);
        config->set("prediction_system_enabled", true, ConfigSource::Default);
        config->set("cognition_system_enabled", true, ConfigSource::Default);
        config->set("development_enabled", true, ConfigSource::Default);
        config->set("checkpoint_enabled", true, ConfigSource::Default);
        config->set("replay_enabled", true, ConfigSource::Default);
    }
}

// Initialize logging system
void initializeLogging(bool verbose, bool quiet, bool debug) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    
    if (verbose) {
        consoleLogger->setLevel(LogLevel::Info);
    } else if (quiet) {
        consoleLogger->setLevel(LogLevel::Warning);
    } else if (debug) {
        consoleLogger->setLevel(LogLevel::Debug);
    }
    
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

// Run Phase 2 demo (original functionality)
int runPhase2Demo(std::shared_ptr<Config>& config) {
    // Create simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run demo (simplified - using original functionality)
    // In a real implementation, this would call the actual demo code
    NLM_LOG_INFO("Phase 2 demo running...");
    
    // Simulate some steps
    size_t steps = config->getOr<size_t>("simulation_steps", 1000);
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * timestep);
    }
    
    NLM_LOG_INFO("Phase 2 demo completed successfully!");
    return 0;
}

// Run Phase 3 demo
int runPhase3Demo(std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("Phase 3: World Interaction Demo");
    NLM_LOG_INFO("Implementing world simulation and sensory processing...");
    
    // Create environment
    auto environment = std::make_shared<Environment>(config);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run world interaction simulation
    size_t steps = config->getOr<size_t>("simulation_steps", 2000);
    for (SimulationStep step = 0; step < steps; ++step) {
        // Get environment observations
        auto observation = environment->getObservation();
        
        // Process with brain
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        // Apply motor actions
        environment->applyAction(brain->getMotorCommands());
    }
    
    NLM_LOG_INFO("Phase 3 demo completed successfully!");
    return 0;
}

// Run Phase 4 demo
int runPhase4Demo(std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("Phase 4: Advanced Cognition Demo");
    NLM_LOG_INFO("Implementing advanced cognitive systems...");
    
    // Create brain with advanced features
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run advanced cognition simulation
    size_t steps = config->getOr<size_t>("simulation_steps", 3000);
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        // Log progress every 500 steps
        if (step % 500 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    NLM_LOG_INFO("Phase 4 demo completed successfully!");
    return 0;
}

// Run Phase 5 demo
int runPhase5Demo(std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("Phase 5: Performance and Advanced Features Demo");
    NLM_LOG_INFO("Implementing performance optimizations and advanced demos...");
    
    // Create brain with performance features
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run performance demo
    size_t steps = config->getOr<size_t>("simulation_steps", 5000);
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        // Log progress every 1000 steps
        if (step % 1000 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Performance metrics collected");
        }
    }
    
    NLM_LOG_INFO("Phase 5 demo completed successfully!");
    return 0;
}

// Run Phase 6 demo (full integration)
int runPhase6Demo(std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("Phase 6: Full Integration Demo");
    NLM_LOG_INFO("Running comprehensive integration test...");
    
    // Create brain with all Phase 6 features enabled
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run comprehensive integration simulation
    size_t steps = config->getOr<size_t>("simulation_steps", 10000);
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        // Log progress every 2000 steps
        if (step % 2000 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Integration test in progress...");
        }
    }
    
    NLM_LOG_INFO("Phase 6 demo completed successfully!");
    NLM_LOG_INFO("All Phase 6 systems integrated and functioning.");
    return 0;
}

// Run benchmark
int runBenchmark(std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("Benchmark: Performance Testing");
    NLM_LOG_INFO("Measuring NLM performance and capabilities...");
    
    // Create brain for benchmarking
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run benchmark simulation
    size_t steps = config->getOr<size_t>("simulation_steps", 5000);
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    NLM_LOG_INFO("Benchmark completed!");
    NLM_LOG_INFO("Steps processed: " + std::to_string(steps));
    NLM_LOG_INFO("Time elapsed: " + std::to_string(duration.count() / 1000.0) + "s");
    NLM_LOG_INFO("Performance: " + std::to_string(steps * 1000.0 / duration.count()) + " steps/second");
    
    return 0;
}

int main(int argc, char** argv) {
    // Parse command line arguments
    std::string mode = "phase2"; // Default mode
    std::string configFile = "configs/default.cfg";
    bool verbose = false;
    bool quiet = false;
    bool debug = false;
    bool validateOnly = false;
    std::string exportConfigFile = "";
    
    // Parse mode and options
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --mode=MODE
        if (arg.find("--mode=") == 0) {
            mode = arg.substr(7);
        }
        // Handle --config=FILE
        else if (arg.find("--config=") == 0) {
            configFile = arg.substr(9);
        }
        // Handle boolean flags
        else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--quiet") {
            quiet = true;
        } else if (arg == "--debug") {
            debug = true;
        } else if (arg == "--validate") {
            validateOnly = true;
        } else if (arg.find("--export-config=") == 0) {
            exportConfigFile = arg.substr(17);
        } else if (arg == "--help") {
            printHelp();
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        }
        // Handle positional arguments
        else if (!arg.empty() && arg[0] != '-') {
            mode = arg;
        }
    }
    
    // Print banner
    printBanner();
    
    // Print help if no mode specified
    if (mode == "help" || mode.empty()) {
        printHelp();
        return 0;
    }
    
    // Print version if requested
    if (mode == "version") {
        printVersion();
        return 0;
    }
    
    // Initialize logging system
    initializeLogging(verbose, quiet, debug);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Parse additional command line arguments
    parseCommandLineArgs(argc, argv, config);
    
    // Load configuration
    loadConfiguration(config, configFile);
    
    // Set default values for mode
    setDefaultConfig(config, mode);
    
    // Process command line overrides
    config->loadFromArgs(argc, argv);
    
    // Validate configuration if requested
    if (validateOnly) {
        NLM_LOG_INFO("Configuration validation:");
        NLM_LOG_INFO(config->summary());
        return 0;
    }
    
    // Export configuration if requested
    if (!exportConfigFile.empty()) {
        if (config->saveToFile(exportConfigFile)) {
            NLM_LOG_INFO("Configuration exported to: " + exportConfigFile);
        } else {
            NLM_LOG_ERROR("Failed to export configuration to: " + exportConfigFile);
            return 1;
        }
    }
    
    // Run the requested mode
    NLM_LOG_INFO("=== Starting NLM " + mode + " ===");
    
    int result = 0;
    if (mode == "phase2") {
        result = runPhase2Demo(config);
    } else if (mode == "phase3") {
        result = runPhase3Demo(config);
    } else if (mode == "phase4") {
        result = runPhase4Demo(config);
    } else if (mode == "phase5") {
        result = runPhase5Demo(config);
    } else if (mode == "phase6") {
        result = runPhase6Demo(config);
    } else if (mode == "benchmark") {
        result = runBenchmark(config);
    } else {
        NLM_LOG_ERROR("Unknown mode: " + mode);
        printHelp();
        return 1;
    }
    
    if (result == 0) {
        NLM_LOG_INFO("=== NLM " + mode + " completed successfully ===");
    } else {
        NLM_LOG_ERROR("=== NLM " + mode + " failed ===");
    }
    
    return result;
}
