// NLM Command Line Interface Implementation
// Provides comprehensive control over NLM simulations and experiments

#include "NLMCLI.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <filesystem>
#include <iomanip>
#include <algorithm>

namespace nlm {

// Static members
bool NLMCLI::shouldExit_ = false;

NLMCLI::NLMCLI()
    : config_(std::make_shared<Config>())
    , currentExperiment_(nullptr)
    , currentBrain_(nullptr)
    , currentAgent_(nullptr)
    , currentWorld_(nullptr)
    , isRunning_(false)
    , isPaused_(false)
    , simulationSteps_(0)
    , historyIndex_(0)
    , totalStepsRun_(0)
    , totalReward_(0.0f)
    , totalWallTime_(0.0)
{
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create default config if not set
    if (!config_->has("neuron_count")) {
        config_->set("neuron_count", Defaults::DEFAULT_NEURON_COUNT);
    }
    if (!config_->has("region_count")) {
        config_->set("region_count", Defaults::DEFAULT_REGION_COUNT);
    }
    if (!config_->has("connection_probability")) {
        config_->set("connection_probability", Defaults::DEFAULT_CONNECTION_PROBABILITY);
    }
}

NLMCLI::~NLMCLI() {
    if (logFile_.is_open()) {
        logFile_.close();
    }
    if (debugFile_.is_open()) {
        debugFile_.close();
    }
}

int NLMCLI::run(int argc, char* argv[]) {
    // Parse command line arguments
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    if (args.empty()) {
        // Start interactive mode
        displayWelcome();
        return runInteractiveMode();
    } else {
        // Process command line arguments
        return runCommandLineMode(args);
    }
}

int NLMCLI::runInteractiveMode() {
    std::string input;
    
    while (!shouldExit_) {
        displayPrompt();
        
        if (!std::getline(std::cin, input)) {
            break; // EOF or error
        }
        
        // Trim whitespace
        input.erase(0, input.find_first_not_of(" \t\n\r"));
        input.erase(input.find_last_not_of(" \t\n\r") + 1);
        
        if (input.empty()) {
            continue;
        }
        
        // Add to history
        commandHistory_.push_back(input);
        historyIndex_ = commandHistory_.size();
        
        // Process command
        if (!processCommand(input)) {
            std::cerr << "Error: Command failed to execute" << std::endl;
        }
    }
    
    return 0;
}

int NLMCLI::runCommandLineMode(const std::vector<std::string>& args) {
    // Parse command line options
    std::string experimentType;
    uint64_t steps = Defaults::DEFAULT_MAX_STEPS;
    std::string configFile;
    std::string outputFile;
    std::string logLevel = "info";
    bool debugMode = false;
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--help" || args[i] == "-h") {
            displayHelp();
            return 0;
        } else if (args[i] == "--version" || args[i] == "-v") {
            displayVersion();
            return 0;
        } else if (args[i] == "--run") {
            if (i + 1 < args.size()) {
                experimentType = args[++i];
            }
        } else if (args[i] == "--steps") {
            if (i + 1 < args.size()) {
                steps = std::stoull(args[++i]);
            }
        } else if (args[i] == "--config") {
            if (i + 1 < args.size()) {
                configFile = args[++i];
            }
        } else if (args[i] == "--output") {
            if (i + 1 < args.size()) {
                outputFile = args[++i];
            }
        } else if (args[i] == "--log-level") {
            if (i + 1 < args.size()) {
                logLevel = args[++i];
            }
        } else if (args[i] == "--debug") {
            debugMode = true;
        } else {
            std::cerr << "Unknown option: " << args[i] << std::endl;
            displayHelp();
            return 1;
        }
    }
    
    // Set up logging
    if (logLevel == "debug") {
        Logger::getInstance().setLevel(Logger::Level::Debug);
    } else if (logLevel == "warning") {
        Logger::getInstance().setLevel(Logger::Level::Warning);
    } else if (logLevel == "error") {
        Logger::getInstance().setLevel(Logger::Level::Error);
    }
    
    // Start simulation if experiment type specified
    if (!experimentType.empty()) {
        logInfo("Starting NLM experiment: " + experimentType + ", steps: " + std::to_string(steps));
        
        if (configFile.empty()) {
            configFile = "default_config.json";
        }
        
        bool success = startSimulation(experimentType, steps, configFile);
        
        if (success) {
            logInfo("Experiment completed successfully");
            
            if (!outputFile.empty()) {
                logInfo("Exporting results to: " + outputFile);
                exportData(outputFile);
            }
            
            return 0;
        } else {
            logError("Experiment failed to start or complete");
            return 1;
        }
    }
    
    // If no experiment specified, show help
    displayHelp();
    return 0;
}

bool NLMCLI::processCommand(const std::string& input) {
    std::istringstream iss(input);
    std::string command;
    iss >> command;
    
    // Convert command to lowercase for case-insensitive matching
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    // Parse arguments
    std::vector<std::string> args;
    std::string arg;
    while (iss >> arg) {
        args.push_back(arg);
    }
    
    // Dispatch to command handler
    if (command == "help" || command == "?") {
        return handleHelp(args);
    } else if (command == "version") {
        return handleVersion(args);
    } else if (command == "run") {
        return handleRun(args);
    } else if (command == "config") {
        return handleConfig(args);
    } else if (command == "export") {
        return handleExport(args);
    } else if (command == "import") {
        return handleImport(args);
    } else if (command == "debug") {
        return handleDebug(args);
    } else if (command == "status") {
        return handleStatus(args);
    } else if (command == "reset") {
        return handleReset(args);
    } else if (command == "quit" || command == "exit") {
        return handleQuit(args);
    } else if (command == "clear") {
        // Clear screen (simple implementation)
        std::cout << "\033[2J\033[1;1H";
        return true;
    } else if (command == "history") {
        // Show command history
        std::cout << "Command history:" << std::endl;
        for (size_t i = 0; i < commandHistory_.size(); ++i) {
            std::cout << i + 1 << ". " << commandHistory_[i] << std::endl;
        }
        return true;
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        std::cerr << "Type 'help' for available commands" << std::endl;
        return false;
    }
}

bool NLMCLI::handleHelp(const std::vector<std::string>& args) {
    if (args.empty()) {
        displayHelp();
    } else {
        // Show help for specific command
        std::string command = args[0];
        
        if (command == "run") {
            std::cout << "Run an experiment" << std::endl;
            std::cout << "  Usage: run <experiment_type> [steps] [config_file]" << std::endl;
            std::cout << "  Examples:" << std::endl;
            std::cout << "    run phase6 10000 config.json" << std::endl;
            std::cout << "    run quick 1000" << std::endl;
        } else if (command == "config") {
            std::cout << "Configuration management" << std::endl;
            std::cout << "  Usage: config <get|set|show|load|save> [key] [value]" << std::endl;
            std::cout << "  Examples:" << std::endl;
            std::cout << "    config show" << std::endl;
            std::cout << "    config set neuron_count 500" << std::endl;
            std::cout << "    config load my_config.json" << std::endl;
        } else if (command == "export") {
            std::cout << "Export data" << std::endl;
            std::cout << "  Usage: export <filename> [format]" << std::endl;
            std::cout << "  Formats: json, csv, binary, txt" << std::endl;
        } else {
            std::cerr << "No help available for command: " << command << std::endl;
            return false;
        }
    }
    return true;
}

bool NLMCLI::handleVersion(const std::vector<std::string>& args) {
    std::cout << "NLM v0.1.0 - Neural Learning Machine" << std::endl;
    std::cout << "An experimental artificial developmental brain" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
    return true;
}

bool NLMCLI::handleRun(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Error: No experiment type specified" << std::endl;
        return false;
    }
    
    std::string experimentType = args[0];
    uint64_t steps = Defaults::DEFAULT_MAX_STEPS;
    std::string configFile;
    
    if (args.size() > 1) {
        // Try to parse steps
        try {
            steps = std::stoull(args[1]);
            if (args.size() > 2) {
                configFile = args[2];
            }
        } catch (...) {
            // Not a number, assume it's config file
            configFile = args[1];
        }
    }
    
    return startSimulation(experimentType, steps, configFile);
}

bool NLMCLI::handleConfig(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Error: No config command specified" << std::endl;
        return false;
    }
    
    std::string subcommand = args[0];
    
    if (subcommand == "get" || subcommand == "show") {
        return showConfig();
    } else if (subcommand == "set") {
        if (args.size() < 2) {
            std::cerr << "Error: No key specified for config set" << std::endl;
            return false;
        }
        std::string key = args[1];
        std::string value;
        if (args.size() > 2) {
            value = args[2];
        }
        return setConfig(key, value);
    } else if (subcommand == "load") {
        if (args.size() < 2) {
            std::cerr << "Error: No filename specified for config load" << std::endl;
            return false;
        }
        return loadConfig(args[1]);
    } else if (subcommand == "save") {
        if (args.size() < 2) {
            std::cerr << "Error: No filename specified for config save" << std::endl;
            return false;
        }
        return saveConfig(args[1]);
    } else {
        std::cerr << "Error: Unknown config subcommand: " << subcommand << std::endl;
        return false;
    }
}

bool NLMCLI::handleExport(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Error: No filename specified for export" << std::endl;
        return false;
    }
    
    std::string filename = args[0];
    std::string format = (args.size() > 1) ? args[1] : "json";
    
    return exportData(filename, format);
}

bool NLMCLI::handleImport(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Error: No filename specified for import" << std::endl;
        return false;
    }
    
    return importData(args[0]);
}

bool NLMCLI::handleDebug(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Error: No debug command specified" << std::endl;
        return false;
    }
    
    std::string command = args[0];
    
    if (command == "status") {
        return debugSystem("status");
    } else if (command == "memory") {
        return debugSystem("memory");
    } else if (command == "neurons") {
        return debugSystem("neurons");
    } else if (command == "synapses") {
        return debugSystem("synapses");
    } else {
        std::cerr << "Error: Unknown debug command: " << command << std::endl;
        return false;
    }
}

bool NLMCLI::handleStatus(const std::vector<std::string>& args) {
    return showStatus();
}

bool NLMCLI::handleReset(const std::vector<std::string>& args) {
    return stopSimulation();
}

bool NLMCLI::handleQuit(const std::vector<std::string>& args) {
    logInfo("Shutting down NLM CLI...")
    shouldExit_ = true;
    return true;
}

bool NLMCLI::startSimulation(const std::string& experimentType, uint64_t steps, const std::string& configFile) {
    logInfo("Starting simulation: " + experimentType + ", steps: " + std::to_string(steps));
    
    try {
        // Load config if specified
        if (!configFile.empty()) {
            loadConfig(configFile);
        }
        
        // Create experiment based on type
        if (experimentType == "phase6" || experimentType == "integration") {
            currentExperiment_ = std::make_shared<Phase6IntegratedExperiment>();
            
            // Run the experiment
            Phase6Config phase6Config;
            phase6Config.maxSteps = steps;
            phase6Config.neuronCount = config_->getOr<size_t>("neuron_count", Defaults::DEFAULT_NEURON_COUNT);
            phase6Config.regionCount = config_->getOr<size_t>("region_count", Defaults::DEFAULT_REGION_COUNT);
            phase6Config.connectionProbability = config_->getOr<float>("connection_probability", Defaults::DEFAULT_CONNECTION_PROBABILITY);
            phase6Config.enableCheckpointing = true;
            phase6Config.enableReplay = true;
            phase6Config.enableDevelopment = true;
            phase6Config.checkpointPath = config_->getOr<std::string>("checkpoint_path", Defaults::DEFAULT_CHECKPOINT_PATH);
            
            auto result = currentExperiment_->run(phase6Config);
            
            // Update statistics
            totalStepsRun_ += steps;
            totalReward_ += result.totalReward;
            totalWallTime_ += result.totalWallClockTime;
            
            logInfo("Experiment completed: " + std::to_string(result.totalReward) + " total reward, " +
                   std::to_string(result.avgFiringRate) + " avg firing rate");
            
        } else {
            logError("Unknown experiment type: " + experimentType);
            return false;
        }
        
        currentExperiment_ = nullptr;
        isRunning_ = false;
        return true;
        
    } catch (const std::exception& e) {
        logError(std::string("Exception during simulation: ") + e.what());
        currentExperiment_ = nullptr;
        isRunning_ = false;
        return false;
    }
}

bool NLMCLI::pauseSimulation() {
    if (!isRunning_) {
        logWarning("No simulation is running");
        return false;
    }
    
    isPaused_ = true;
    logInfo("Simulation paused");
    return true;
}

bool NLMCLI::resumeSimulation() {
    if (!isPaused_) {
        logWarning("No simulation is paused");
        return false;
    }
    
    isPaused_ = false;
    logInfo("Simulation resumed");
    return true;
}

bool NLMCLI::stopSimulation() {
    if (isRunning_) {
        logInfo("Stopping simulation...");
        isRunning_ = false;
        isPaused_ = false;
        currentExperiment_ = nullptr;
        
        // Reset statistics
        totalStepsRun_ = 0;
        totalReward_ = 0.0f;
        totalWallTime_ = 0.0;
        
        logInfo("Simulation stopped");
        return true;
    } else {
        logWarning("No simulation is running");
        return false;
    }
}

bool NLMCLI::loadConfig(const std::string& filename) {
    logInfo("Loading configuration from: " + filename);
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        logError("Failed to open config file: " + filename);
        return false;
    }
    
    std::string line;
    bool success = true;
    
    while (std::getline(file, line)) {
        std::string key, value;
        if (parseKeyValue(line, key, value)) {
            if (!setConfig(key, value)) {
                logWarning("Failed to set config key: " + key);
                success = false;
            }
        }
    }
    
    if (success) {
        logInfo("Configuration loaded successfully");
    } else {
        logError("Some configuration values failed to load");
    }
    
    return success;
}

bool NLMCLI::saveConfig(const std::string& filename) const {
    logInfo("Saving configuration to: " + filename);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        logError("Failed to open config file for writing: " + filename);
        return false;
    }
    
    // Get all config keys
    std::vector<std::string> keys = config_->getKeys();
    
    // Write each key-value pair
    for (const std::string& key : keys) {
        auto value = config_->get<std::string>(key);
        if (value.has_value()) {
            file << key << "=" << *value << std::endl;
        }
    }
    
    logInfo("Configuration saved successfully");
    return true;
}

bool NLMCLI::showConfig() const {
    std::cout << "Current Configuration:" << std::endl;
    
    std::vector<std::string> keys = config_->getKeys();
    for (const std::string& key : keys) {
        auto value = config_->get<std::string>(key);
        if (value.has_value()) {
            std::cout << "  " << key << " = " << *value << std::endl;
        }
    }
    
    return true;
}

bool NLMCLI::setConfig(const std::string& key, const std::string& value) {
    try {
        // Try different value types
        bool success = false;
        
        // Try integer
        try {
            int intValue = std::stoi(value);
            config_->set(key, intValue);
            success = true;
        } catch (...) {}
        
        // Try double
        if (!success) {
            try {
                double doubleValue = std::stod(value);
                config_->set(key, doubleValue);
                success = true;
            } catch (...) {}
        }
        
        // Try boolean
        if (!success) {
            if (value == "true" || value == "1") {
                config_->set(key, true);
                success = true;
            } else if (value == "false" || value == "0") {
                config_->set(key, false);
                success = true;
            }
        }
        
        // Try string
        if (!success) {
            config_->set(key, value);
            success = true;
        }
        
        if (success) {
            logInfo("Set config: " + key + " = " + value);
        } else {
            logError("Failed to parse config value: " + value);
        }
        
        return success;
        
    } catch (const std::exception& e) {
        logError(std::string("Error setting config: ") + e.what());
        return false;
    }
}

bool NLMCLI::exportData(const std::string& filename, const std::string& format) {
    logInfo("Exporting data to: " + filename + " (format: " + format + ")");
    
    try {
        if (format == "json") {
            // TODO: Implement JSON export
            logWarning("JSON export not yet implemented");
            return false;
        } else if (format == "csv") {
            // TODO: Implement CSV export
            logWarning("CSV export not yet implemented");
            return false;
        } else if (format == "binary") {
            // TODO: Implement binary export
            logWarning("Binary export not yet implemented");
            return false;
        } else if (format == "txt") {
            // TODO: Implement text export
            logWarning("Text export not yet implemented");
            return false;
        } else {
            logError("Unknown export format: " + format);
            return false;
        }
        
    } catch (const std::exception& e) {
        logError(std::string("Error during export: ") + e.what());
        return false;
    }
    
    return true;
}

bool NLMCLI::importData(const std::string& filename) {
    logInfo("Importing data from: " + filename);
    
    // TODO: Implement import functionality
    logWarning("Import functionality not yet implemented");
    return false;
}

bool NLMCLI::showStatus() const {
    std::cout << "=== NLM System Status ===" << std::endl;
    std::cout << "Experiment: " << currentExperiment_ << std::endl;
    std::cout << "Running: " << (isRunning_ ? "Yes" : "No") << std::endl;
    std::cout << "Paused: " << (isPaused_ ? "Yes" : "No") << std::endl;
    std::cout << "Total Steps: " << totalStepsRun_ << std::endl;
    std::cout << "Total Reward: " << totalReward_ << std::endl;
    std::cout << "Wall Time: " << totalWallTime_ << " seconds" << std::endl;
    std::cout << "Current Experiment: " << currentExperiment_ << std::endl;
    
    return true;
}

bool NLMCLI::debugSystem(const std::string& component) {
    logInfo("Debugging component: " + component);
    
    // TODO: Implement debug functionality
    logWarning("Debug functionality for " + component + " not yet implemented");
    return false;
}

void NLMCLI::displayPrompt() const {
    std::cout << ">>> ";
    std::cout.flush();
}

void NLMCLI::displayWelcome() const {
    std::cout << "=========================================" << std::endl;
    std::cout << "  NLM - Neural Learning Machine CLI" << std::endl;
    std::cout << "  Phase 6: Final Integration" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "Type 'help' for available commands" << std::endl;
    std::cout << std::endl;
}

void NLMCLI::displayHelp() const {
    std::cout << "=========================================" << std::endl;
    std::cout << "  NLM Command Line Interface - Help" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "General Commands:" << std::endl;
    std::cout << "  help           Show this help message" << std::endl;
    std::cout << "  version        Show version information" << std::endl;
    std::cout << "  clear          Clear screen" << std::endl;
    std::cout << "  history        Show command history" << std::endl;
    std::cout << "  status         Show system status" << std::endl;
    std::cout << "  quit/exit      Exit NLM CLI" << std::endl;
    std::cout << std::endl;
    std::cout << "Simulation Commands:" << std::endl;
    std::cout << "  run <experiment> [steps] [config]  Run an experiment" << std::endl;
    std::cout << "  pause          Pause current simulation" << std::endl;
    std::cout << "  resume         Resume paused simulation" << std::endl;
    std::cout << "  reset          Stop and reset simulation" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration Commands:" << std::endl;
    std::cout << "  config <subcommand> [args]  Manage configuration" << std::endl;
    std::cout << "    get <key>                    Get config value" << std::endl;
    std::cout << "    set <key> <value>             Set config value" << std::endl;
    std::cout << "    show                          Show all config values" << std::endl;
    std::cout << "    load <filename>               Load config from file" << std::endl;
    std::cout << "    save <filename>               Save config to file" << std::endl;
    std::cout << std::endl;
    std::cout << "Data Management Commands:" << std::endl;
    std::cout << "  export <filename> [format]      Export simulation data" << std::endl;
    std::cout << "  import <filename>              Import simulation data" << std::endl;
    std::cout << "  backup <filename>              Create backup of current state" << std::endl;
    std::cout << std::endl;
    std::cout << "Debugging Commands:" << std::endl;
    std::cout << "  debug <component>              Debug system component" << std::endl;
    std::cout << "    status                       Show system status" << std::endl;
    std::cout << "    memory                       Show memory usage" << std::endl;
    std::cout << "    neurons                      Show neuron information" << std::endl;
    std::cout << "    synapses                     Show synapse information" << std::endl;
    std::cout << std::endl;
}

void NLMCLI::displayVersion() const {
    std::cout << "NLM v0.1.0 - Neural Learning Machine" << std::endl;
    std::cout << "An experimental artificial developmental brain" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
    std::cout << "Built with C++20" << std::endl;
}

void NLMCLI::logError(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
    if (logFile_.is_open()) {
        logFile_ << "[ERROR] " << message << std::endl;
    }
}

void NLMCLI::logWarning(const std::string& message) {
    std::cerr << "[WARNING] " << message << std::endl;
    if (logFile_.is_open()) {
        logFile_ << "[WARNING] " << message << std::endl;
    }
}

void NLMCLI::logInfo(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
    if (logFile_.is_open()) {
        logFile_ << "[INFO] " << message << std::endl;
    }
}

void NLMCLI::logDebug(const std::string& message) {
    std::cout << "[DEBUG] " << message << std::endl;
    if (logFile_.is_open()) {
        logFile_ << "[DEBUG] " << message << std::endl;
    }
}

std::string NLMCLI::parseKeyValue(const std::string& input, std::string& key, std::string& value) const {
    size_t equalsPos = input.find('=');
    if (equalsPos == std::string::npos || equalsPos == 0) {
        return "Invalid config format";
    }
    
    key = input.substr(0, equalsPos);
    value = input.substr(equalsPos + 1);
    
    // Trim whitespace
    key.erase(0, key.find_first_not_of(" \t\n\r"));
    key.erase(key.find_last_not_of(" \t\n\r") + 1);
    value.erase(0, value.find_first_not_of(" \t\n\r"));
    value.erase(value.find_last_not_of(" \t\n\r") + 1);
    
    return "";
}

bool NLMCLI::parseConfigLine(const std::string& line) {
    std::string key, value;
    std::string error = parseKeyValue(line, key, value);
    if (!error.empty()) {
        logWarning("Skipping invalid config line: " + line + " (" + error + ")");
        return false;
    }
    
    return setConfig(key, value);
}

void NLMCLI::signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        logInfo("Received shutdown signal, exiting...");
        shouldExit_ = true;
    }
}

} // namespace nlm
