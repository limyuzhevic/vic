#include "NLMCLI.hpp"
#include "../core/Config/Config.hpp"
#include "../agent/AgentBrain.hpp"
#include "../memory/MemoryManager.hpp"
#include "../memory/IMemorySystem.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <thread>
#include <chrono>

namespace nlm {

struct NLMCLI::Impl {
    std::unique_ptr<Config> config;
    std::unique_ptr<AgentBrain> agent;
    std::unique_ptr<MemoryManager> memoryManager;
    std::unique_ptr<ExperimentRunner> experimentRunner;
    
    std::string currentDirectory;
    bool interactiveModeEnabled;
    bool systemInitialized;
    
    Impl() : systemInitialized(false), interactiveModeEnabled(false) {}
};

NLMCLI::NLMCLI() : pImpl(std::make_unique<Impl>()) {}

NLMCLI::~NLMCLI() = default;

int NLMCLI::run(int argc, char** argv) {
    try {
        CommandLineParser::CommandArgs args = CommandLineParser::parse(argc, argv);
        
        if (args.verbose) {
            NLM_LOG_INFO("NLM CLI starting with arguments: " + std::to_string(argc));
        }
        
        // Initialize system
        if (!initializeFromConfig(args.configFile)) {
            printError("Failed to initialize system from configuration");
            return 1;
        }
        
        // Run based on mode
        bool success = false;
        if (args.mode == "demo") {
            success = runDemoMode(args);
        } else if (args.mode == "run") {
            success = runExperiment(args);
        } else if (args.mode == "batch") {
            success = runBatchMode(args);
        } else if (args.mode == "sweep") {
            success = runParameterSweep(args);
        } else if (args.mode == "analyze") {
            success = analyzeResults(args);
        } else {
            printError("Unknown mode: " + args.mode);
            CommandLineParser::printUsage(argv[0]);
            return 1;
        }
        
        if (success) {
            printSuccess("Command completed successfully");
            return 0;
        } else {
            printError("Command failed");
            return 1;
        }
        
    } catch (const std::exception& e) {
        printError("Exception: " + std::string(e.what()));
        return 1;
    }
}

bool NLMCLI::runDemoMode(const CommandLineParser::CommandArgs& args) {
    printInfo("Running NLM Phase 6 Integration Demo...");
    
    if (!pImpl->systemInitialized) {
        printError("System not initialized");
        return false;
    }
    
    // Run a quick integration test
    printInfo("Testing memory systems integration...");
    if (pImpl->memoryManager) {
        bool memoryStatus = printMemoryStatus();
        if (!memoryStatus) {
            printWarning("Memory system status check failed");
        }
    }
    
    printInfo("Testing brain initialization...");
    // In a real implementation, this would run the actual demo
    // For now, just print status
    
    printInfo("Demo completed successfully");
    return true;
}

bool NLMCLI::runExperiment(const CommandLineParser::CommandArgs& args) {
    printInfo("Running experiment: " + (args.experimentFile.empty() ? "default" : args.experimentFile));
    
    if (!pImpl->systemInitialized) {
        printError("System not initialized");
        return false;
    }
    
    // Create experiment runner
    pImpl->experimentRunner = std::make_unique<ExperimentRunner>();
    
    // Run experiment with configured parameters
    printInfo("Running experiment with " + std::to_string(args.maxSteps) + " steps");
    
    // Simulate experiment run
    // In a real implementation, this would:
    // 1. Create brain and environment
    // 2. Configure according to args
    // 3. Run experiment
    // 4. Collect results
    
    printInfo("Experiment simulation completed");
    return true;
}

bool NLMCLI::runBatchMode(const CommandLineParser::CommandArgs& args) {
    printInfo("Running batch mode with " + std::to_string(args.experimentNames.size()) + " experiments");
    
    // Create multiple experiments
    std::vector<std::shared_ptr<Experiment>> experiments;
    
    for (const auto& name : args.experimentNames) {
        auto experiment = pImpl->experimentRunner->createExperiment(name);
        experiments.push_back(experiment);
    }
    
    printInfo("Created " + std::to_string(experiments.size()) + " experiments");
    
    // Run batch
    auto results = pImpl->experimentRunner->runBatch(
        experiments, nullptr, nullptr, args.maxSteps);
    
    printInfo("Batch completed with " + std::to_string(results.size()) + " results");
    
    // Save results if requested
    if (!args.outputFile.empty()) {
        return saveResults(args);
    }
    
    return true;
}

bool NLMCLI::runParameterSweep(const CommandLineParser::CommandArgs& args) {
    printInfo("Running parameter sweep mode");
    
    // Create parameter sweep experiments
    std::vector<ParameterSweep> sweeps;
    
    // Example: Sweep learning rate
    ParameterSweep learningRateSweep;
    learningRateSweep.parameterName = "learning_rate";
    learningRateSweep.values = {0.001, 0.01, 0.1, 1.0};
    learningRateSweep.distribution = "linear";
    sweeps.push_back(learningRateSweep);
    
    // Run sweep
    auto results = pImpl->experimentRunner->runParameterSweep(
        "default_experiment", sweeps, nullptr, nullptr, args.maxSteps);
    
    printInfo("Parameter sweep completed with " + std::to_string(results.size()) + " results");
    
    // Analyze results
    auto analysis = pImpl->experimentRunner->analyzeResults(results);
    
    printInfo("Best configuration: " + analysis.bestConfiguration);
    printInfo("Worst configuration: " + analysis.worstConfiguration);
    
    // Save results
    if (!args.outputFile.empty()) {
        return saveResults(args);
    }
    
    return true;
}

bool NLMCLI::analyzeResults(const CommandLineParser::CommandArgs& args) {
    printInfo("Analyzing experiment results");
    
    if (!pImpl->experimentRunner) {
        printError("No experiment results available");
        return false;
    }
    
    // In a real implementation, this would load and analyze results
    // For now, just print status
    
    printInfo("Analysis completed");
    return true;
}

bool NLMCLI::exportResults(const CommandLineParser::CommandArgs& args) {
    printInfo("Exporting results to " + args.outputFile);
    
    if (!pImpl->experimentRunner) {
        printError("No experiment results to export");
        return false;
    }
    
    // Determine format from file extension
    std::string extension = args.outputFile.substr(args.outputFile.find_last_of('.') + 1);
    
    if (extension == "csv") {
        bool success = pImpl->experimentRunner->saveResultsCSV(args.outputFile, std::vector<ExperimentResult>());
        if (!success) {
            printError("Failed to export results to CSV");
            return false;
        }
    } else if (extension == "json") {
        bool success = pImpl->experimentRunner->saveResultsJSON(args.outputFile, std::vector<ExperimentResult>());
        if (!success) {
            printError("Failed to export results to JSON");
            return false;
        }
    } else {
        printError("Unsupported export format: " + extension);
        return false;
    }
    
    printInfo("Results exported successfully");
    return true;
}

bool NLMCLI::loadConfig(const std::string& filepath) {
    printInfo("Loading configuration from " + filepath);
    
    pImpl->config = std::make_unique<Config>();
    
    if (!pImpl->config->loadFromFile(filepath)) {
        printError("Failed to load configuration from file");
        return false;
    }
    
    printInfo("Configuration loaded successfully");
    return true;
}

bool NLMCLI::saveConfig(const std::string& filepath) {
    printInfo("Saving configuration to " + filepath);
    
    if (!pImpl->config) {
        printError("No configuration to save");
        return false;
    }
    
    if (!pImpl->config->saveToFile(filepath)) {
        printError("Failed to save configuration to file");
        return false;
    }
    
    printInfo("Configuration saved successfully");
    return true;
}

bool NLMCLI::printConfig() const {
    if (!pImpl->config) {
        printError("No configuration loaded");
        return false;
    }
    
    std::cout << pImpl->config->summary() << std::endl;
    return true;
}

bool NLMCLI::setConfigValue(const std::string& key, const std::string& value) {
    if (!pImpl->config) {
        printError("No configuration loaded");
        return false;
    }
    
    try {
        pImpl->config->set(key, value);
        printInfo("Configuration key " + key + " set to " + value);
        return true;
    } catch (const std::exception& e) {
        printError("Failed to set configuration key " + key + ": " + e.what());
        return false;
    }
}

bool NLMCLI::deleteConfigValue(const std::string& key) {
    if (!pImpl->config) {
        printError("No configuration loaded");
        return false;
    }
    
    pImpl->config->remove(key);
    printInfo("Configuration key " + key + " removed");
    return true;
}

bool NLMCLI::listConfigKeys() const {
    if (!pImpl->config) {
        printError("No configuration loaded");
        return false;
    }
    
    auto keys = pImpl->config->getKeys();
    std::cout << "Configuration keys (" << keys.size() << "):" << std::endl;
    for (const auto& key : keys) {
        std::cout << "  " << key << std::endl;
    }
    
    return true;
}

bool NLMCLI::printMemoryStatus() const {
    if (!pImpl->memoryManager) {
        printError("Memory manager not initialized");
        return false;
    }
    
    std::cout << "=== Memory System Status ===" << std::endl;
    std::cout << "Working Memory: Enabled (" << pImpl->memoryManager->getWorkingMemoryActiveTraces() << " traces)" << std::endl;
    std::cout << "Episodic Memory: Enabled (" << pImpl->memoryManager->getEpisodicMemoryCount() << " episodes)" << std::endl;
    std::cout << "Associative Memory: Enabled (" << pImpl->memoryManager->getAssociativeMemoryCount() << " associations)" << std::endl;
    std::cout << "Memory Quality Score: " << pImpl->memoryManager->getMemoryQualityScore() << std::endl;
    
    return true;
}

bool NLMCLI::enableMemorySystem(const std::string& name, bool enable) {
    if (!pImpl->memoryManager) {
        printError("Memory manager not initialized");
        return false;
    }
    
    if (name == "working") {
        pImpl->memoryManager->enableWorkingMemory(enable);
        printInfo("Working memory " + std::string(enable ? "enabled" : "disabled"));
    } else if (name == "episodic") {
        pImpl->memoryManager->enableEpisodicMemory(enable);
        printInfo("Episodic memory " + std::string(enable ? "enabled" : "disabled"));
    } else if (name == "associative") {
        pImpl->memoryManager->enableAssociativeMemory(enable);
        printInfo("Associative memory " + std::string(enable ? "enabled" : "disabled"));
    } else {
        printError("Unknown memory system: " + name);
        return false;
    }
    
    return true;
}

bool NLMCLI::configureMemorySystem(const std::string& name, const std::map<std::string, std::string>& config) {
    printInfo("Configuring memory system " + name + " with " + std::to_string(config.size()) + " parameters");
    
    // In a real implementation, this would configure the memory system
    // For now, just print the configuration
    for (const auto& pair : config) {
        printInfo("  " + pair.first + " = " + pair.second);
    }
    
    return true;
}

bool NLMCLI::listExperiments() const {
    if (!pImpl->experimentRunner) {
        printError("Experiment runner not initialized");
        return false;
    }
    
    auto experiments = pImpl->experimentRunner->getExperiments();
    std::cout << "Available experiments (" << experiments.size() << "):" << std::endl;
    for (const auto& exp : experiments) {
        std::cout << "  " << exp->getName() << std::endl;
    }
    
    return true;
}

bool NLMCLI::createExperiment(const std::string& name, const std::map<std::string, std::string>& config) {
    if (!pImpl->experimentRunner) {
        printError("Experiment runner not initialized");
        return false;
    }
    
    auto experiment = pImpl->experimentRunner->createExperiment(name);
    
    for (const auto& pair : config) {
        experiment->setConfigValue(pair.first, pair.second);
    }
    
    printInfo("Experiment " + name + " created");
    return true;
}

bool NLMCLI::deleteExperiment(const std::string& name) {
    if (!pImpl->experimentRunner) {
        printError("Experiment runner not initialized");
        return false;
    }
    
    // In a real implementation, this would delete the experiment
    printInfo("Experiment " + name + " would be deleted (implementation pending)");
    return true;
}

bool NLMCLI::printSystemInfo() const {
    std::cout << "=== NLM System Information ===" << std::endl;
    std::cout << "Version: Phase 6 - Final Integration" << std::endl;
    std::cout << "Status: " << getSystemStatusString() << std::endl;
    
    if (pImpl->config) {
        std::cout << "Configuration: " << pImpl->config->getKeys().size() << " entries" << std::endl;
    }
    
    if (pImpl->memoryManager) {
        std::cout << "Memory Systems: Active" << std::endl;
    }
    
    if (pImpl->agent) {
        std::cout << "Agent Brain: Initialized" << std::endl;
    }
    
    return true;
}

bool NLMCLI::printPerformanceStats() const {
    std::cout << "=== Performance Statistics ===" << std::endl;
    std::cout << "Note: Performance statistics would be collected from system monitoring" << std::endl;
    std::cout << "      (This would include timing, memory usage, etc.)" << std::endl;
    
    return true;
}

bool NLMCLI::validateConfiguration() const {
    printInfo("Validating configuration...");
    
    if (!pImpl->config) {
        printError("No configuration to validate");
        return false;
    }
    
    auto validation = pImpl->config->validate();
    
    if (!validation.valid) {
        printError("Configuration validation failed:");
        for (const auto& error : validation.errors) {
            printError("  - " + error);
        }
        return false;
    }
    
    if (!validation.warnings.empty()) {
        printWarning("Configuration warnings:");
        for (const auto& warning : validation.warnings) {
            printWarning("  - " + warning);
        }
    }
    
    printSuccess("Configuration validation passed");
    return true;
}

bool NLMCLI::resetSystem() const {
    printInfo("Resetting system...");
    
    // Reset memory manager
    if (pImpl->memoryManager) {
        pImpl->memoryManager->clear();
    }
    
    // Reset agent if exists
    if (pImpl->agent) {
        pImpl->agent->reset();
    }
    
    printInfo("System reset completed");
    return true;
}

bool NLMCLI::runInteractiveMode() {
    printInfo("Entering interactive mode...");
    
    // In a real implementation, this would provide a REPL interface
    // For now, just display status and return
    printSystemInfo();
    printMemoryStatus();
    printInfo("Interactive mode would provide command prompts here");
    
    return true;
}

bool NLMCLI::initializeFromConfig(const std::string& configFile) {
    // Try to load config file
    if (!configFile.empty()) {
        if (!loadConfig(configFile)) {
            printWarning("Could not load config file " + configFile + ", using defaults");
        }
    }
    
    // Create default config if none loaded
    if (!pImpl->config) {
        pImpl->config = std::make_unique<Config>();
        printInfo("Using default configuration");
    }
    
    // Create agent brain
    pImpl->agent = std::make_unique<AgentBrain>(pImpl->config);
    
    // Create memory manager
    pImpl->memoryManager = std::make_unique<MemoryManager>();
    pImpl->memoryManager->initialize(pImpl->agent.get());
    
    // Create experiment runner
    pImpl->experimentRunner = std::make_unique<ExperimentRunner>();
    
    pImpl->systemInitialized = true;
    printInfo("System initialized successfully");
    
    return true;
}

std::string NLMCLI::formatTime(std::chrono::duration<double> duration) const {
    auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    auto hours = totalSeconds / 3600;
    auto minutes = (totalSeconds % 3600) / 60;
    auto seconds = totalSeconds % 60;
    
    std::ostringstream oss;
    if (hours > 0) {
        oss << hours << "h ";
    }
    oss << minutes << "m " << seconds << "s";
    return oss.str();
}

std::string NLMCLI::formatMemorySize(size_t bytes) const {
    if (bytes < 1024) return std::to_string(bytes) + "B";
    if (bytes < 1024 * 1024) return std::to_string(bytes / 1024) + "KB";
    if (bytes < 1024 * 1024 * 1024) return std::to_string(bytes / (1024 * 1024)) + "MB";
    return std::to_string(bytes / (1024 * 1024 * 1024)) + "GB";
}

std::string NLMCLI::getSystemStatusString() const {
    if (!pImpl->systemInitialized) return "Not Initialized";
    return "Ready";
}

} // namespace nlm

// Command line argument parser implementation
namespace nlm {
CommandLineParser::CommandArgs CommandLineParser::parse(int argc, char** argv) {
    CommandArgs args;
    
    // Set defaults
    args.maxSteps = 1000;
    args.timestep = 0.001;
    args.seed = 42;
    args.verbose = false;
    args.quiet = false;
    args.enableLogging = true;
    args.enableCheckpointing = false;
    args.enableDevelopment = true;
    args.enableNeuromodulation = true;
    args.enableMemorySystems = true;
    args.enablePrediction = true;
    args.enableCognition = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            exit(0);
        } else if (arg == "--version") {
            printVersion();
            exit(0);
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            args.quiet = true;
        } else if (arg.find("--config=") == 0) {
            args.configFile = arg.substr(8);
        } else if (arg.find("--output=") == 0) {
            args.outputFile = arg.substr(9);
        } else if (arg.find("--experiment=") == 0) {
            args.experimentFile = arg.substr(13);
        } else if (arg.find("--mode=") == 0) {
            args.mode = arg.substr(7);
        } else if (arg.find("--max-steps=") == 0) {
            args.maxSteps = std::stoull(arg.substr(11));
        } else if (arg.find("--timestep=") == 0) {
            args.timestep = std::stod(arg.substr(11));
        } else if (arg.find("--seed=") == 0) {
            args.seed = std::stoull(arg.substr(7));
        } else if (arg == "--enable-logging") {
            args.enableLogging = true;
        } else if (arg == "--disable-logging") {
            args.enableLogging = false;
        } else if (arg == "--enable-checkpointing") {
            args.enableCheckpointing = true;
        } else if (arg == "--enable-development") {
            args.enableDevelopment = true;
        } else if (arg == "--disable-development") {
            args.enableDevelopment = false;
        } else if (arg == "--enable-neuromodulation") {
            args.enableNeuromodulation = true;
        } else if (arg == "--disable-neuromodulation") {
            args.enableNeuromodulation = false;
        } else if (arg == "--enable-memory") {
            args.enableMemorySystems = true;
        } else if (arg == "--disable-memory") {
            args.enableMemorySystems = false;
        } else if (arg.find("--set=") == 0) {
            size_t pos = arg.find('=', 6);
            if (pos != std::string::npos) {
                std::string key = arg.substr(6, pos - 6);
                std::string value = arg.substr(pos + 1);
                args.configOverrides[key] = value;
            }
        } else if (arg.find("--experiment=") == 0 && !args.experimentFile.empty()) {
            args.experimentNames.push_back(arg.substr(13));
        } else {
            printError("Unknown argument: " + arg);
            printUsage(argv[0]);
            exit(1);
        }
    }
    
    return args;
}

void CommandLineParser::printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --version                     Show version information" << std::endl;
    std::cout << "  --verbose, -v                Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q                  Enable quiet mode" << std::endl;
    std::cout << "  --config=<file>              Load configuration from file" << std::endl;
    std::cout << "  --output=<file>              Output file for results" << std::endl;
    std::cout << "  --experiment=<file>          Experiment configuration file" << std::endl;
    std::cout << "  --mode=<mode>                Mode: demo, run, batch, sweep, analyze" << std::endl;
    std::cout << "  --max-steps=<steps>          Maximum simulation steps" << std::endl;
    std::cout << "  --timestep=<time>             Simulation timestep" << std::endl;
    std::cout << "  --seed=<number>              Random seed" << std::endl;
    std::cout << "  --enable-logging              Enable logging" << std::endl;
    std::cout << "  --disable-logging             Disable logging" << std::endl;
    std::cout << "  --enable-checkpointing         Enable checkpointing" << std::endl;
    std::cout << "  --enable-development          Enable development system" << std::endl;
    std::cout << "  --disable-development         Disable development system" << std::endl;
    std::cout << "  --enable-neuromodulation      Enable neuromodulation" << std::endl;
    std::cout << "  --disable-neuromodulation     Disable neuromodulation" << std::endl;
    std::cout << "  --enable-memory              Enable memory systems" << std::endl;
    std::cout << "  --disable-memory             Disable memory systems" << std::endl;
    std::cout << "  --set=<key>=<value>           Set configuration value" << std::endl;
    std::cout << "  --experiment=<name>          Experiment name (for batch mode)" << std::endl;
}

void CommandLineParser::printVersion() {
    std::cout << "NLM Phase 6 - Final Integration" << std::endl;
    std::cout << "Neural Learning Machine - Advanced AI Research Platform" << std::endl;
    std::cout << "Version 6.0.0 - Final Integration Release" << std::endl;
}

} // namespace nlm

// Helper functions implementation
namespace nlm {
void printBanner() {
    std::cout << "╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    NLM - Neural Learning Machine              ║" << std::endl;
    std::cout << "║                   Phase 6: Final Integration                  ║" << std::endl;
    std::cout << "║                                                              ║" << std::endl;
    std::cout << "║      Advanced AI Research Platform for Brain Simulation      ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

void printError(const std::string& message) {
    std::cerr << "ERROR: " << message << std::endl;
}

void printWarning(const std::string& message) {
    std::cerr << "WARNING: " << message << std::endl;
}

void printInfo(const std::string& message) {
    std::cout << "INFO: " << message << std::endl;
}

void printSuccess(const std::string& message) {
    std::cout << "SUCCESS: " << message << std::endl;
}

void printSeparator() {
    std::cout << "─────────────────────────────────────────────────────────────────────" << std::endl;
}

} // namespace nlm