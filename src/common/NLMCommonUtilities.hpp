// Common utilities for NLM demos
// Provides standardized initialization, configuration, and argument parsing

#ifndef NLM_COMMON_UTILITIES_H
#define NLM_COMMON_UTILITIES_H

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "common/NLMConfigFile.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <memory>

namespace nlm {
namespace {

// Command line argument structure for better parsing
struct DemoArgs {
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    int trials = 50;
    std::string configFile = "configs/default.cfg";
    std::string logLevel = "info";
    bool verbose = false;
    bool help = false;
    bool version = false;
    std::vector<std::string> extraArgs;
};

// Parse command line arguments with proper error handling and help
DemoArgs parseCommandLine(int argc, char** argv, const std::string& defaultHelpText = "") {
    DemoArgs args;
    
    // Check for help flag first
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            args.help = true;
        } else if (arg == "--version" || arg == "-v") {
            args.version = true;
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg == "--log-level") {
            if (i + 1 < argc) {
                args.logLevel = argv[++i];
            }
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--episodes" || arg == "-e") {
            if (i + 1 < argc) {
                args.numEpisodes = std::stoi(argv[++i]);
            }
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) {
                args.stepsPerEpisode = std::stoi(argv[++i]);
            }
        } else if (arg == "--trials" || arg == "-t") {
            if (i + 1 < argc) {
                args.trials = std::stoi(argv[++i]);
            }
        } else if (arg.substr(0, 2) == "--") {
            // Unknown flag, store for potential future use
            args.extraArgs.push_back(arg);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                args.extraArgs.push_back(argv[++i]);
            }
        } else {
            // Positional argument
            args.extraArgs.push_back(arg);
        }
    }
    
    return args;
}

// Display help information for the demo
void printHelp(const std::string& programName, const std::string& description) {
    std::cout << "Usage: " << programName << " [options]\n\n";
    std::cout << description << "\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -v, --version           Show version information\n";
    std::cout << "  -c, --config <file>     Load configuration from file (default: configs/default.cfg)\n";
    std::cout << "  -l, --log-level <level> Set logging level (debug, info, warning, error)\n";
    std::cout << "  -v, --verbose           Enable verbose output\n";
    std::cout << "  -e, --episodes <n>      Number of episodes (default: 5)\n";
    std::cout << "  -s, --steps <n>         Steps per episode (default: 200)\n";
    std::cout << "  -t, --trials <n>        Number of trials (default: 50)\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << programName << " --episodes 10 --steps 500\n";
    std::cout << "  " << programName << " --config my_config.cfg --verbose\n";
    std::cout << "  " << programName << " --help\n";
}

// Display version information
void printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine v0.1.0\n";
    std::cout << "Phase 6 Final Integration\n";
    std::cout << "An experimental artificial developmental brain.\n";
}

// Initialize the standard NLM environment (logging, config, etc.)
void initializeEnvironment(const DemoArgs& args) {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(stringToLogLevel(args.logLevel));
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Set log level based on verbosity
    if (args.verbose) {
        Logger::getGlobal().setLevel(LogLevel::Debug);
    } else {
        Logger::getGlobal().setLevel(stringToLogLevel(args.logLevel));
    }
    
    // Log startup information
    NLM_LOG_INFO("=== NLM Demo Started ===");
    NLM_LOG_INFO("Program: " + std::string("unknown"));
    NLM_LOG_INFO("Episodes: " + std::to_string(args.numEpisodes));
    NLM_LOG_INFO("Steps per episode: " + std::to_string(args.stepsPerEpisode));
    if (!args.configFile.empty()) {
        NLM_LOG_INFO("Config file: " + args.configFile);
    }
    NLM_LOG_INFO("============================");
}

// Load configuration from file
bool loadConfigFromFile(std::shared_ptr<Config> config, const std::string& filename) {
    if (!config) return false;
    
    ConfigFile configParser;
    if (!configParser.loadFromFile(filename)) {
        NLM_LOG_WARNING("Failed to load configuration file: " + filename);
        return false;
    }
    
    return configParser.loadToConfig(config);
}

// Common configuration setup for Phase 3 demos
void setupPhase3Config(std::shared_ptr<Config> config) {
    if (!config) return;
    
    // Set default Phase 3 parameters
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
}

// Common configuration setup for Phase 4 demos
void setupPhase4Config(std::shared_ptr<Config> config) {
    if (!config) return;
    
    // Set default Phase 4 parameters
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
}

// Common configuration setup for Phase 5 demos
void setupPhase5Config(std::shared_ptr<Config> config) {
    if (!config) return;
    
    // Set default Phase 5 parameters
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
}

// Common configuration setup for Phase 6 demos
void setupPhase6Config(std::shared_ptr<Config> config) {
    if (!config) return;
    
    // Set default Phase 6 parameters
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
}

// Performance measurement utilities
struct PerformanceMetrics {
    double startTime;
    double endTime;
    size_t steps;
    float avgFiringRate;
    size_t totalSpikes;
    double wallClockTime;
};

PerformanceMetrics startPerformanceMeasurement() {
    PerformanceMetrics metrics;
    metrics.startTime = std::chrono::high_resolution_clock::now();
    return metrics;
}

PerformanceMetrics endPerformanceMeasurement(size_t steps) {
    PerformanceMetrics metrics;
    metrics.endTime = std::chrono::high_resolution_clock::now();
    metrics.steps = steps;
    metrics.wallClockTime = std::chrono::duration<double>(metrics.endTime - metrics.startTime).count();
    return metrics;
}

// Print formatted performance results
void printPerformanceResults(const PerformanceMetrics& metrics) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Performance Summary:\n";
    std::cout << "  Steps: " << metrics.steps << "\n";
    std::cout << "  Steps/sec: " << metrics.steps / metrics.wallClockTime << "\n";
    std::cout << "  Avg Firing Rate: " << metrics.avgFiringRate << " Hz\n";
    std::cout << "  Total Spikes: " << metrics.totalSpikes << "\n";
    std::cout << "  Real-time: " << metrics.wallClockTime << " s\n";
}

} // namespace

} // namespace nlm

#endif // NLM_COMMON_UTILITIES_H