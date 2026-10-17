#include "core/CLI/NLMCLI.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <map>

namespace nlm {

NLMCLI::NLMCLI() {
    // Initialize default configuration
    config_.configFile = "configs/default.cfg";
    config_.outputFile = "";
    config_.checkpointFile = "";
    config_.logLevel = "INFO";
    config_.verbose = false;
    config_.quiet = false;
    config_.maxSteps = 1000;
    config_.numSteps = 100;
    config_.timestep = 0.001;
    config_.mode = "demo";
    config_.enableCheckpointing = false;
    config_.enableReplay = false;
    config_.enableDevelopment = false;
    config_.enableCuriosity = false;
    config_.enableRewardModulation = false;
}

bool NLMCLI::parseArguments(int argc, char* argv[]) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == "--help" || arg == "-h") {
                printHelp();
                return false;
            }
            else if (arg == "--version") {
                printVersion();
                return false;
            }
            else if (arg == "--quiet" || arg == "-q") {
                config_.quiet = true;
            }
            else if (arg == "--verbose" || arg == "-v") {
                config_.verbose = true;
                config_.logLevel = "DEBUG";
            }
            else if (arg == "--config" && i + 1 < argc) {
                config_.configFile = argv[++i];
            }
            else if (arg == "--output" && i + 1 < argc) {
                config_.outputFile = argv[++i];
            }
            else if (arg == "--checkpoint" && i + 1 < argc) {
                config_.checkpointFile = argv[++i];
            }
            else if (arg == "--mode" && i + 1 < argc) {
                config_.mode = argv[++i];
                std::transform(config_.mode.begin(), config_.mode.end(), config_.mode.begin(),
                              [](unsigned char c) { return std::tolower(c); });
            }
            else if (arg == "--steps" && i + 1 < argc) {
                config_.maxSteps = std::stoi(argv[++i]);
            }
            else if (arg == "--num-steps" && i + 1 < argc) {
                config_.numSteps = std::stoi(argv[++i]);
            }
            else if (arg == "--timestep" && i + 1 < argc) {
                config_.timestep = std::stof(argv[++i]);
            }
            else if (arg == "--enable-checkpointing") {
                config_.enableCheckpointing = true;
            }
            else if (arg == "--enable-replay") {
                config_.enableReplay = true;
            }
            else if (arg == "--enable-development") {
                config_.enableDevelopment = true;
            }
            else if (arg == "--enable-curiosity") {
                config_.enableCuriosity = true;
            }
            else if (arg == "--enable-reward-modulation") {
                config_.enableRewardModulation = true;
            }
            else if (arg == "--log-level" && i + 1 < argc) {
                config_.logLevel = argv[++i];
            }
            else {
                // Store unknown arguments as custom args
                config_.customArgs.push_back(arg);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    config_.customArgs.push_back(argv[++i]);
                }
            }
        }
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing arguments: " << e.what() << std::endl;
        return false;
    }
}

bool NLMCLI::parseConfigFile(const std::string& filename) {
    config_.configFile = filename;
    return loadConfigFromFile(filename);
}

bool NLMCLI::loadFromArgs(int argc, char* argv[]) {
    if (!parseArguments(argc, argv)) {
        return false;
    }

    // Load main configuration file if specified
    if (!config_.configFile.empty()) {
        if (!parseConfigFile(config_.configFile)) {
            std::cerr << "Warning: Could not load configuration file: " << config_.configFile << std::endl;
        }
    }

    // Load checkpoint if specified
    if (!config_.checkpointFile.empty()) {
        std::cerr << "Checkpoint loading not yet implemented in CLI" << std::endl;
    }

    return true;
}

int NLMCLI::execute() {
    if (!validateConfig()) {
        std::cerr << "Configuration validation failed!" << std::endl;
        return 1;
    }

    initializeBrain();

    logMessage("NLM CLI Starting...");

    int result = 1;
    if (config_.mode == "demo") {
        result = runDemoMode();
    }
    else if (config_.mode == "simulate") {
        result = runSimulationMode();
    }
    else if (config_.mode == "train") {
        result = runTrainingMode();
    }
    else if (config_.mode == "monitor") {
        result = runMonitoringMode();
    }
    else if (config_.mode == "benchmark") {
        result = runBenchmarkMode();
    }
    else {
        std::cerr << "Unknown mode: " << config_.mode << std::endl;
        return 1;
    }

    if (config_.outputFile.empty() && !config_.checkpointFile.empty()) {
        // Save checkpoint if requested
        // Note: Save functionality would need to be added to Brain class
        std::cerr << "Checkpoint saving not yet implemented in CLI" << std::endl;
    }

    logMessage("NLM CLI Finished with result: " + std::to_string(result));
    return result;
}

int NLMCLI::runDemoMode() const {
    logMessage("Running demo mode...");

    // This would integrate with the existing Phase 6 demo
    // For now, create a simple demonstration
    auto config = std::make_shared<Config>();

    // Set basic configuration
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("simulation_timestep", config_.timestep);
    config->set("random_seed", 42);

    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        logMessage("Failed to initialize brain!", "ERROR");
        return 1;
    }

    logMessage("Brain initialized with " + std::to_string(brain->getTotalNeuronCount()) + " neurons");

    // Run demonstration steps
    for (int step = 0; step < config_.numSteps; ++step) {
        brain->step(step, static_cast<Timestamp>(step) * config_.timestep);

        if (step % 100 == 0 && !config_.quiet) {
            logMessage("Step " + std::to_string(step) + ": " +
                      std::to_string(brain->getFiringNeuronCount()) + " firing neurons, " +
                      std::to_string(brain->getTotalSpikeCount()) + " total spikes");
        }
    }

    logMessage("Demo completed successfully!");
    return 0;
}

int NLMCLI::runSimulationMode() const {
    logMessage("Running simulation mode...");
    // TODO: Implement full simulation mode with world interaction
    return runDemoMode(); // Temporary implementation
}

int NLMCLI::runTrainingMode() const {
    logMessage("Running training mode...");
    // TODO: Implement training mode with data files
    return runDemoMode(); // Temporary implementation
}

int NLMCLI::runMonitoringMode() const {
    logMessage("Running monitoring mode...");
    // TODO: Implement monitoring mode with real-time visualization
    return runDemoMode(); // Temporary implementation
}

int NLMCLI::runBenchmarkMode() const {
    logMessage("Running benchmark mode...");

    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("simulation_timestep", config_.timestep);
    config->set("random_seed", 42);

    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        logMessage("Failed to initialize brain for benchmark!", "ERROR");
        return 1;
    }

    // Warm-up
    for (int step = 0; step < 100; ++step) {
        brain->step(step, static_cast<Timestamp>(step) * config_.timestep);
    }

    // Benchmark timing
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int step = 0; step < config_.maxSteps; ++step) {
        brain->step(step, static_cast<Timestamp>(step) * config_.timestep);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    double stepsPerSecond = (config_.maxSteps / (duration.count() / 1000.0));
    double neuronsPerSecond = (brain->getTotalNeuronCount() * stepsPerSecond);

    logMessage("Benchmark Results:");
    logMessage("  Steps: " + std::to_string(config_.maxSteps));
    logMessage("  Time: " + std::to_string(duration.count() / 1000.0) + " seconds");
    logMessage("  Steps/second: " + std::to_string(stepsPerSecond));
    logMessage("  Neurons/second: " + std::to_string(neuronsPerSecond));
    logMessage("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));

    return 0;
}

int NLMCLI::exportBrainState() const {
    logMessage("Exporting brain state...");
    // TODO: Implement brain state export functionality
    return 0;
}

void NLMCLI::printHelp() const {
    std::cout << "NLM (Neural Learning Machine) - Command Line Interface" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << "Usage: nlm [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  --mode DEMO        Run demonstration (default)" << std::endl;
    std::cout << "  --mode SIMULATE    Run world simulation" << std::endl;
    std::cout << "  --mode TRAIN       Train with data files" << std::endl;
    std::cout << "  --mode MONITOR     Real-time monitoring" << std::endl;
    std::cout << "  --mode BENCHMARK   Performance benchmarking" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h           Show this help message" << std::endl;
    std::cout << "  --version            Show version information" << std::endl;
    std::cout << "  --quiet, -q          Suppress non-essential output" << std::endl;
    std::cout << "  --verbose, -v        Enable verbose output" << std::endl;
    std::cout << "  --config FILE        Load configuration from file" << std::endl;
    std::cout << "  --output FILE        Save output to file" << std::endl;
    std::cout << "  --checkpoint FILE    Load/save brain checkpoint" << std::endl;
    std::cout << "  --steps N            Maximum number of steps (default: 1000)" << std::endl;
    std::cout << "  --num-steps N        Number of steps to run (default: 100)" << std::endl;
    std::cout << "  --timestep T         Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --log-level LEVEL    Set log level (DEBUG, INFO, WARNING, ERROR)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  --enable-checkpointing     Enable checkpoint saving" << std::endl;
    std::cout << "  --enable-replay            Enable memory replay" << std::endl;
    std::cout << "  --enable-development       Enable developmental processes" << std::endl;
    std::cout << "  --enable-curiosity        Enable curiosity-driven exploration" << std::endl;
    std::cout << "  --enable-reward-modulation Enable reward-based learning" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --mode demo --steps 500" << std::endl;
    std::cout << "  nlm --mode benchmark --steps 10000" << std::endl;
    std::cout << "  nlm --config my_config.cfg --output results.txt" << std::endl;
}

void NLMCLI::printVersion() const {
    std::cout << "NLM Phase 6 Integration (Version 0.1.0)" << std::endl;
    std::cout << "Developed as part of the Neural Learning Machine research project." << std::endl;
}

void NLMCLI::printUsage() const {
    std::cout << "Usage: nlm [options]" << std::endl;
    std::cout << "Use --help for detailed information." << std::endl;
}

bool NLMCLI::validateConfig() const {
    if (config_.mode.empty()) {
        logMessage("Error: Mode not specified", "ERROR");
        return false;
    }

    if (config_.maxSteps < 1) {
        logMessage("Error: Maximum steps must be positive", "ERROR");
        return false;
    }

    if (config_.numSteps < 1) {
        logMessage("Error: Number of steps must be positive", "ERROR");
        return false;
    }

    if (config_.timestep <= 0.0f) {
        logMessage("Error: Timestep must be positive", "ERROR");
        return false;
    }

    if (config_.numSteps > config_.maxSteps) {
        logMessage("Error: Number of steps cannot exceed maximum steps", "ERROR");
        return false;
    }

    return true;
}

bool NLMCLI::initializeBrain() const {
    brainConfig_ = std::make_shared<Config>();

    // Set basic configuration from CLI args
    brainConfig_->set("neuron_count", 500);
    brainConfig_->set("region_count", 1);
    brainConfig_->set("connection_probability", 0.1f);
    brainConfig_->set("simulation_timestep", config_.timestep);
    brainConfig_->set("random_seed", 42);
    brainConfig_->set("stdp_ltp_weight", 0.01f);
    brainConfig_->set("stdp_ltd_weight", 0.012f);
    brainConfig_->set("stdp_tau", 20.0f);

    // Enable optional features
    if (config_.enableDevelopment) {
        brainConfig_->set("development_enabled", true);
    }
    if (config_.enableCuriosity) {
        brainConfig_->set("curiosity_enabled", true);
    }
    if (config_.enableRewardModulation) {
        brainConfig_->set("reward_modulation_enabled", true);
    }
    if (config_.enableCheckpointing) {
        brainConfig_->set("checkpoint_dir", "./checkpoints");
    }
    if (config_.enableReplay) {
        brainConfig_->set("replay_enabled", true);
    }

    brain_ = std::make_shared<Brain>(brainConfig_);
    if (!brain_->initialize()) {
        logMessage("Failed to initialize brain!", "ERROR");
        return false;
    }

    // Set log level
    Logger::getInstance().setLevel(getLoggerLevelFromString(config_.logLevel));

    return true;
}

void NLMCLI::logMessage(const std::string& message, const std::string& level) const {
    if (config_.quiet && level != "ERROR") {
        return;
    }

    if (!config_.quiet) {
        std::cout << "[" << level << "] " << message << std::endl;
    }
}

bool NLMCLI::saveConfigToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // TODO: Implement configuration serialization
    file << "NLM Configuration (CLI)" << std::endl;
    file << "Mode: " << config_.mode << std::endl;
    file << "Steps: " << config_.maxSteps << std::endl;
    file << "Timestep: " << config_.timestep << std::endl;

    file.close();
    return true;
}

bool NLMCLI::loadConfigFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // TODO: Parse configuration from file
        // Simple parsing for now
        if (line.find("Mode:") != std::string::npos) {
            config_.mode = "demo"; // Parse from line
        }
    }

    file.close();
    return true;
}

Logger::Level NLMCLI::getLoggerLevelFromString(const std::string& levelStr) const {
    static const std::map<std::string, Logger::Level> levelMap = {
        {"DEBUG", Logger::Level::Debug},
        {"INFO", Logger::Level::Info},
        {"WARNING", Logger::Level::Warning},
        {"ERROR", Logger::Level::Error}
    };

    auto it = levelMap.find(levelStr);
    if (it != levelMap.end()) {
        return it->second;
    }

    return Logger::Level::Info;
}

} // namespace nlm
