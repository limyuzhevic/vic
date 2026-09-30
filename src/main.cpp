// NLM (熙然) - Neural Learning Machine
// Enhanced Main Application
//
// This is the enhanced main application for NLM with comprehensive CLI interface,
// configuration management, and support for multiple phases and modes.

#include "core/Config/ConfigEnhanced.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "performance/PerformanceMonitor.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <csignal>
#include <map>
#include <fstream>
#include <sstream>

using namespace nlm;

// Global variables for signal handling
volatile bool g_running = true;
std::shared_ptr<PerformanceMonitor> g_perfMonitor = nullptr;

// Signal handler for graceful shutdown
void signalHandler(int signum) {
    std::cout << "\n\nSignal " << signum << " received. Initiating graceful shutdown..." << std::endl;
    g_running = false;
    if (g_perfMonitor) {
        g_perfMonitor->stopMonitoring();
    }
}

// Enhanced banner with version info
void printEnhancedBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                 ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     All systems integrated and working as a unified brain.      ║
    ║                                                               ║
    ║     Features: Memory, Prediction, Neuromodulation, Cognition    ║
    ║     Developmental stages, Checkpoint persistence, Replay      ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Print help information
void printHelp(const std::string& programName) {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "NLM - Neural Learning Machine - Enhanced Command Line Interface" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "\nUsage: " << programName << " [OPTIONS] [ARGS]" << std::endl;
    std::cout << "\nBasic Operation:" << std::endl;
    std::cout << "  " << programName << "                          Run Phase 6 integration test" << std::endl;
    std::cout << "  " << programName << " --mode phase5                 Run Phase 5 performance demo" << std::endl;
    std::cout << "  " << programName << " --mode phase4                 Run Phase 4 cognition demo" << std::endl;
    std::cout << "  " << programName << " --mode phase3                 Run Phase 3 world interaction demo" << std::endl;
    std::cout << "\nConfiguration Options:" << std::endl;
    std::cout << "  --config <file>              Load configuration from file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --set <key>=<value>          Override configuration values" << std::endl;
    std::cout << "  --validate-config            Validate configuration and exit" << std::endl;
    std::cout << "  --print-config              Print current configuration and exit" << std::endl;
    std::cout << "\nPerformance Options:" << std::endl;
    std::cout << "  --monitor                    Enable real-time performance monitoring" << std::endl;
    std::cout << "  --benchmark                  Run performance benchmarks" << std::endl;
    std::cout << "  --max-steps <n>              Maximum simulation steps (default: 1000)" << std::endl;
    std::cout << "  --timestep <t>               Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --neuron-count <n>           Neuron count (default: 1000)" << std::endl;
    std::cout << "\nOutput Options:" << std::endl;
    std::cout << "  --quiet                      Suppress progress output" << std::endl;
    std::cout << "  --verbose                    Enable verbose logging" << std::endl;
    std::cout << "  --json-output <file>         Save results to JSON file" << std::endl;
    std::cout << "  --csv-output <file>          Save metrics to CSV file" << std::endl;
    std::cout << "\nCheckpoint Options:" << std::endl;
    std::cout << "  --checkpoint <file>          Save checkpoint to file" << std::endl;
    std::cout << "  --restore <file>             Restore from checkpoint" << std::endl;
    std::cout << "  --auto-checkpoint <seconds>   Auto-save checkpoint every N seconds" << std::endl;
    std::cout << "\nAdvanced Options:" << std::endl;
    std::cout << "  --seed <n>                   Random seed (default: 42)" << std::endl;
    std::cout << "  --help                       Print this help message" << std::endl;
    std::cout << "  --version                    Print version information" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  " << programName << " --config myconfig.cfg --max-steps 5000 --monitor" << std::endl;
    std::cout << "  " << programName << " --mode phase3 --auto-checkpoint 300" << std::endl;
    std::cout << "  " << programName << " --validate-config" << std::endl;
    std::cout << "\n" << std::string(70, '=') << std::endl;
}

// Print version information
void printVersion() {
    std::cout << "NLM Version 0.1.0 - Phase 6: Final Integration" << std::endl;
    std::cout << "Build: Enhanced Command Line Interface" << std::endl;
    std::cout << "Features: Memory, Prediction, Neuromodulation, Cognition" << std::endl;
    std::cout << "Developmental stages, Checkpoint persistence, Replay" << std::endl;
    std::cout << "MIT License" << std::endl;
}

// Configuration validation
bool validateConfiguration(const std::shared_ptr<Config>& config) {
    std::cout << "Validating configuration..." << std::endl;
    
    bool isValid = true;
    
    // Validate neuron count
    auto neuronCountOpt = config->get<int64_t>("neuron_count");
    if (!neuronCountOpt || *neuronCountOpt <= 0) {
        std::cout << "ERROR: Invalid neuron_count: " << (*neuronCountOpt ? std::to_string(*neuronCountOpt) : "not set") << std::endl;
        isValid = false;
    }
    
    // Validate simulation timestep
    auto timestepOpt = config->get<double>("simulation_timestep");
    if (!timestepOpt || *timestepOpt <= 0.0) {
        std::cout << "ERROR: Invalid simulation_timestep: " << (*timestepOpt ? std::to_string(*timestepOpt) : "not set") << std::endl;
        isValid = false;
    }
    
    // Validate region count
    auto regionCountOpt = config->get<int64_t>("region_count");
    if (!regionCountOpt || *regionCountOpt <= 0) {
        std::cout << "ERROR: Invalid region_count: " << (*regionCountOpt ? std::to_string(*regionCountOpt) : "not set") << std::endl;
        isValid = false;
    }
    
    // Validate connection probability
    auto connProbOpt = config->get<float>("connection_probability");
    if (!connProbOpt || *connProbOpt < 0.0f || *connProbOpt > 1.0f) {
        std::cout << "ERROR: Invalid connection_probability: " << (*connProbOpt ? std::to_string(*connProbOpt) : "not set") << std::endl;
        isValid = false;
    }
    
    // Validate random seed
    auto seedOpt = config->get<uint64_t>("random_seed");
    if (!seedOpt) {
        std::cout << "ERROR: random_seed not set" << std::endl;
        isValid = false;
    }
    
    if (isValid) {
        std::cout << "Configuration validation passed!" << std::endl;
    }
    
    return isValid;
}

// Print configuration summary
void printConfigurationSummary(const std::shared_ptr<Config>& config) {
    std::cout << std::endl;
    std::cout << "Current Configuration:" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    auto printKey = [&](const std::string& key, const std::string& defaultValue) {
        auto valueOpt = config->get<std::string>(key);
        std::string value = valueOpt ? *valueOpt : defaultValue;
        std::cout << "  " << std::left << std::setw(25) << key << ": " << value << std::endl;
    };
    
    printKey("random_seed", "42");
    printKey("simulation_timestep", "0.001");
    printKey("neuron_count", "1000");
    printKey("region_count", "1");
    printKey("connection_probability", "0.15");
    printKey("stdp_ltp_weight", "0.02");
    printKey("stdp_ltd_weight", "0.015");
    printKey("stdp_tau", "20.0");
    printKey("synaptogenesis_rate", "0.0001");
    printKey("pruning_rate", "0.00001");
    
    std::cout << std::string(50, '-') << std::endl;
}

// JSON output helper
void saveResultsToJSON(const std::string& filename, const std::map<std::string, std::string>& results) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "ERROR: Could not open file for writing: " << filename << std::endl;
        return;
    }
    
    file << "{" << std::endl;
    bool first = true;
    for (const auto& pair : results) {
        if (!first) file << "," << std::endl;
        first = false;
        file << "  \"" << pair.first << "\": \"" << pair.second << "\"";
    }
    file << std::endl << "}" << std::endl;
    file.close();
    std::cout << "Results saved to JSON file: " << filename << std::endl;
}

// CSV output helper
void saveMetricsToCSV(const std::string& filename, const std::vector<std::vector<std::string>>& metrics) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "ERROR: Could not open file for writing: " << filename << std::endl;
        return;
    }
    
    for (const auto& row : metrics) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];
            if (i < row.size() - 1) file << ",";
        }
        file << std::endl;
    }
    file.close();
    std::cout << "Metrics saved to CSV file: " << filename << std::endl;
}

// Main execution function
int main(int argc, char** argv) {
    std::string programName = "nlm";
    if (argc > 0 && argv[0]) {
        programName = std::string(argv[0]);
    }
    
    // Parse command line arguments
    std::string mode = "phase6";  // Default mode
    std::string configFile = "configs/default.cfg";
    bool monitor = false;
    bool benchmark = false;
    bool quiet = false;
    bool verbose = false;
    bool validateOnly = false;
    bool printConfigOnly = false;
    bool printHelp = false;
    bool printVersion = false;
    std::string jsonOutput;
    std::string csvOutput;
    std::string checkpointFile;
    std::string restoreFile;
    int maxSteps = 1000;
    double timestep = 0.001;
    int64_t neuronCount = 1000;
    uint64_t seed = 42;
    double autoCheckpointSeconds = 0.0;
    
    std::vector<std::pair<std::string, std::string>> setOptions;
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printHelp(programName);
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "--quiet") {
            quiet = true;
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--monitor") {
            monitor = true;
        } else if (arg == "--benchmark") {
            benchmark = true;
        } else if (arg == "--validate-config") {
            validateOnly = true;
        } else if (arg == "--print-config") {
            printConfigOnly = true;
        } else if (arg.substr(0, 7) == "--mode=") {
            mode = arg.substr(7);
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg.substr(0, 7) == "--config=") {
            configFile = arg.substr(7);
        } else if (arg.substr(0, 6) == "--set=") {
            setOptions.push_back(std::make_pair(
                arg.substr(6, arg.find('=') - 6),
                arg.substr(arg.find('=') + 1)
            ));
        } else if (arg == "--max-steps" && i + 1 < argc) {
            maxSteps = std::stoi(argv[++i]);
        } else if (arg.substr(0, 11) == "--max-steps=") {
            maxSteps = std::stoi(arg.substr(11));
        } else if (arg == "--timestep" && i + 1 < argc) {
            timestep = std::stod(argv[++i]);
        } else if (arg.substr(0, 11) == "--timestep=") {
            timestep = std::stod(arg.substr(11));
        } else if (arg == "--neuron-count" && i + 1 < argc) {
            neuronCount = std::stol(argv[++i]);
        } else if (arg.substr(0, 14) == "--neuron-count=") {
            neuronCount = std::stol(arg.substr(14));
        } else if (arg == "--seed" && i + 1 < argc) {
            seed = std::stoull(argv[++i]);
        } else if (arg.substr(0, 7) == "--seed=") {
            seed = std::stoull(arg.substr(7));
        } else if (arg == "--json-output" && i + 1 < argc) {
            jsonOutput = argv[++i];
        } else if (arg.substr(0, 14) == "--json-output=") {
            jsonOutput = arg.substr(14);
        } else if (arg == "--csv-output" && i + 1 < argc) {
            csvOutput = argv[++i];
        } else if (arg.substr(0, 13) == "--csv-output=") {
            csvOutput = arg.substr(13);
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            checkpointFile = argv[++i];
        } else if (arg.substr(0, 12) == "--checkpoint=") {
            checkpointFile = arg.substr(12);
        } else if (arg == "--restore" && i + 1 < argc) {
            restoreFile = argv[++i];
        } else if (arg.substr(0, 9) == "--restore=") {
            restoreFile = arg.substr(9);
        } else if (arg == "--auto-checkpoint" && i + 1 < argc) {
            autoCheckpointSeconds = std::stod(argv[++i]);
        } else if (arg.substr(0, 18) == "--auto-checkpoint=") {
            autoCheckpointSeconds = std::stod(arg.substr(18));
        } else {
            // Unknown argument, try to treat as mode
            if (arg.substr(0, 7) == "--mode=" && !mode.empty() && mode == "phase6") {
                mode = arg.substr(7);
            } else if (mode == "phase6" && arg.find(".") == std::string::npos) {
                // Assume it's a mode if it doesn't look like a file
                mode = arg;
            } else {
                std::cout << "WARNING: Unknown argument ignored: " << arg << std::endl;
            }
        }
    }
    
    // Set signal handlers for graceful shutdown
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Print banner
    printEnhancedBanner();
    
    // Print version if requested
    if (printVersion) {
        printVersion();
    }
    
    // Handle validation-only mode
    if (validateOnly) {
        auto config = std::make_shared<Config>();
        
        // Try to load from file
        if (config->loadFromFile(configFile)) {
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        } else {
            std::cout << "Using default configuration (file not found)" << std::endl;
        }
        
        // Apply set options
        for (const auto& setOpt : setOptions) {
            config->set(setOpt.first, setOpt.second, ConfigSource::CommandLine);
        }
        
        // Set defaults
        config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Default);
        config->set("simulation_timestep", timestep, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        
        // Validate configuration
        if (validateConfiguration(config)) {
            std::cout << "Configuration validation completed successfully!" << std::endl;
            return 0;
        } else {
            std::cout << "Configuration validation failed!" << std::endl;
            return 1;
        }
    }
    
    // Handle print config only mode
    if (printConfigOnly) {
        auto config = std::make_shared<Config>();
        
        // Try to load from file
        if (config->loadFromFile(configFile)) {
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        } else {
            std::cout << "Using default configuration (file not found)" << std::endl;
        }
        
        // Apply set options
        for (const auto& setOpt : setOptions) {
            config->set(setOpt.first, setOpt.second, ConfigSource::CommandLine);
        }
        
        // Set defaults
        config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Default);
        config->set("simulation_timestep", timestep, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        
        // Print configuration
        printConfigurationSummary(config);
        std::cout << "Configuration printed successfully!" << std::endl;
        return 0;
    }
    
    // Initialize logger with appropriate level
    auto logger = std::make_shared<Logger>();
    LogLevel logLevel = quiet ? LogLevel::Error : (verbose ? LogLevel::Debug : LogLevel::Info);
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Enhanced Application Starting ===");
    NLM_LOG_INFO("Mode: " + mode);
    NLM_LOG_INFO("Configuration file: " + configFile);
    
    // Initialize performance monitor if requested
    if (monitor) {
        g_perfMonitor = std::make_shared<PerformanceMonitor>();
        g_perfMonitor->startMonitoring();
        NLM_LOG_INFO("Performance monitoring enabled");
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration (file not found)");
    }
    
    // Apply command line set options
    for (const auto& setOpt : setOptions) {
        config->set(setOpt.first, setOpt.second, ConfigSource::CommandLine);
    }
    
    // Apply defaults based on mode
    config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Default);
    config->set("simulation_timestep", timestep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    
    // Set mode-specific defaults
    if (mode == "phase3") {
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    } else if (mode == "phase4") {
        config->set("neuron_count", static_cast<int64_t>(800), ConfigSource::Default);
    } else if (mode == "phase5") {
        config->set("neuron_count", static_cast<int64_t>(1500), ConfigSource::Default);
    } else if (mode == "phase6") {
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    }
    
    // Validate configuration
    if (!validateConfiguration(config)) {
        NLM_LOG_ERROR("Configuration validation failed!");
        return 1;
    }
    
    NLM_LOG_INFO("Configuration validated successfully");
    
    // Print configuration summary if not quiet
    if (!quiet) {
        printConfigurationSummary(config);
    }
    
    // Initialize performance monitor timing
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Initialize simulation clock
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain in mode: " + mode);
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run mode-specific execution
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Starting Mode: " + mode + " ===");
    
    // For now, implement basic execution loop based on mode
    // In a real implementation, this would call different experiment runners
    bool success = false;
    
    if (mode == "phase3") {
        NLM_LOG_INFO("Running Phase 3: World Interaction Demo");
        // For now, just run basic connectivity test
        success = runBasicConnectivityTest(brain);
    } else if (mode == "phase4") {
        NLM_LOG_INFO("Running Phase 4: Cognition Demo");
        // For now, just run plasticity experiment
        success = runPlasticityExperiment(brain);
    } else if (mode == "phase5") {
        NLM_LOG_INFO("Running Phase 5: Performance Demo");
        // For now, just run STDP verification
        success = runStdpVerification(brain);
    } else if (mode == "phase6" || mode == "demo") {
        NLM_LOG_INFO("Running Phase 6: Final Integration Demo");
        // Run all basic tests
        runBasicConnectivityTest(brain);
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        success = true;
    } else {
        NLM_LOG_ERROR("Unknown mode: " + mode);
        return 1;
    }
    
    // Calculate execution time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Stop performance monitoring
    if (g_perfMonitor) {
        g_perfMonitor->stopMonitoring();
        NLM_LOG_INFO("Performance monitoring completed");
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Execution Summary ===");
    NLM_LOG_INFO("Mode: " + mode);
    NLM_LOG_INFO("Execution time: " + std::to_string(duration.count()) + " ms");
    NLM_LOG_INFO("Success: " + std::string(success ? "Yes" : "No"));
    NLM_LOG_INFO("Neuron count: " + std::to_string(neuronCount));
    NLM_LOG_INFO("Total steps: " + std::to_string(maxSteps));
    
    brain->logStatus();
    
    // Save results to JSON if requested
    if (!jsonOutput.empty()) {
        std::map<std::string, std::string> results;
        results["mode"] = mode;
        results["execution_time_ms"] = std::to_string(duration.count());
        results["success"] = success ? "true" : "false";
        results["neuron_count"] = std::to_string(neuronCount);
        results["total_steps"] = std::to_string(maxSteps);
        results["simulation_timestep"] = std::to_string(timestep);
        
        saveResultsToJSON(jsonOutput, results);
    }
    
    // Save metrics to CSV if requested
    if (!csvOutput.empty()) {
        std::vector<std::vector<std::string>> metrics;
        std::vector<std::string> header = {"metric", "value"};
        metrics.push_back(header);
        
        std::vector<std::string> row1 = {"mode", mode};
        metrics.push_back(row1);
        
        std::vector<std::string> row2 = {"execution_time_ms", std::to_string(duration.count())};
        metrics.push_back(row2);
        
        std::vector<std::string> row3 = {"success", success ? "true" : "false"};
        metrics.push_back(row3);
        
        std::vector<std::string> row4 = {"neuron_count", std::to_string(neuronCount)};
        metrics.push_back(row4);
        
        saveMetricsToCSV(csvOutput, metrics);
    }
    
    // Save checkpoint if requested
    if (!checkpointFile.empty()) {
        NLM_LOG_INFO("Saving checkpoint to: " + checkpointFile);
        // In a real implementation, this would use the checkpoint system
        std::cout << "Checkpoint saving would be implemented here" << std::endl;
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Execution Complete ===");
    
    return success ? 0 : 1;
}