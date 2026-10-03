// NLM Command Line Interface
// Main entry point for the enhanced NLM CLI system

#include "NLMCommandLine.hpp"
#include "CLICommands.hpp"
#include "CLIUtils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <filesystem>
#include <signal.h>
#include <unistd.h>

namespace nlm {
namespace cli {

// Signal handler for graceful shutdown
volatile sig_atomic_t interruptReceived = 0;

void signalHandler(int signum) {
    interruptReceived = 1;
}

void setupSignalHandlers() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
}

// Configuration parser for CLI
class CLIConfigParser {
public:
    static bool parseConfigFile(const std::string& filepath, std::unordered_map<std::string, std::string>& config) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            if (line.empty()) {
                continue;
            }
            
            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos) {
                std::string key = line.substr(0, equalPos);
                std::string value = line.substr(equalPos + 1);
                
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                
                config[key] = value;
            }
        }
        
        return true;
    }
};

// Main command line implementation
NLMCommandLine::NLMCommandLine() : initialized(false), running(false), currentLogLevel(LogLevel::Info) {
    setupSignalHandlers();
    initializeDefaultConfig();
}

NLMCommandLine::~NLMCommandLine() {
    shutdown();
}

bool NLMCommandLine::initialize() {
    if (initialized) {
        return true;
    }
    
    utils::printInfo("Initializing NLM Command Line Interface...");
    
    // Parse command line arguments
    parseCommandLineArgs();
    
    // Load configuration
    if (!loadConfig()) {
        utils::printWarning("Failed to load configuration, using defaults");
    }
    
    // Setup logging
    setupLogging();
    
    // Initialize command registry
    initializeCommands();
    
    // Setup help system
    setupHelpSystem();
    
    initialized = true;
    
    utils::printSuccess("NLM CLI initialized successfully!");
    return true;
}

void NLMCommandLine::initializeDefaultConfig() {
    // Set default configuration values
    config["cli_help_enabled"] = "true";
    config["cli_version_output"] = "true";
    config["cli_progress_indicators"] = "true";
    config["cli_checkpoint_format"] = "json";
    
    config["cli_log_level"] = "INFO";
    config["cli_log_to_file"] = "false";
    config["cli_log_filename"] = "nlm_cli.log";
    config["cli_log_rotation"] = "10485760";
    config["cli_log_colors"] = "true";
    
    config["cli_default_mode"] = "demo";
    
    config["demo_mode_brain_size"] = "1000";
    config["demo_mode_steps"] = "1000";
    config["demo_mode_seed"] = "42";
    
    config["integration_mode_brain_size"] = "5000";
    config["integration_mode_steps"] = "5000";
    config["integration_mode_connection_probability"] = "0.15";
    config["integration_mode_plasticity_enabled"] = "true";
    
    config["profiling_mode_duration"] = "30.0";
    config["profiling_mode_samples"] = "1000";
    config["profiling_mode_detailed"] = "true";
    
    config["experiment_mode_default_duration"] = "10000";
    config["experiment_mode_default_repetitions"] = "5";
    config["experiment_mode_auto_save"] = "true";
    
    config["config_mode_save_on_exit"] = "true";
    config["config_mode_export_format"] = "json";
    
    config["brain_neuron_count"] = "1000";
    config["brain_region_count"] = "2";
    config["brain_simulation_timestep"] = "0.001";
    config["brain_random_seed"] = "42";
    
    config["brain_stdp_ltp_weight"] = "0.02";
    config["brain_stdp_ltd_weight"] = "0.015";
    config["brain_stdp_tau"] = "20.0";
    config["brain_hebbian_learning_rate"] = "0.01";
    config["brain_structural_plasticity_rate"] = "0.0001";
    
    config["cli_text_viz_enabled"] = "true";
    config["cli_text_viz_update_rate"] = "10.0";
    config["cli_text_viz_detailed"] = "false";
    
    config["checkpoint_enabled"] = "true";
    config["checkpoint_interval"] = "100";
    config["checkpoint_dir"] = "./checkpoints";
    config["checkpoint_compress"] = "true";
    
    config["performance_profiling_enabled"] = "true";
    config["performance_memory_tracking"] = "true";
    config["performance_thread_count"] = "1";
}

bool NLMCommandLine::loadConfig() {
    // Try to load from command line argument
    for (size_t i = 0; i < commandLineArgs.size(); ++i) {
        if (commandLineArgs[i] == "--config" && i + 1 < commandLineArgs.size()) {
            std::string configFile = commandLineArgs[i + 1];
            if (utils::fileExists(configFile)) {
                return CLIConfigParser::parseConfigFile(configFile, config);
            }
        }
    }
    
    // Try default config files
    std::vector<std::string> defaultConfigs = {
        "configs/default_cli.cfg",
        "configs/default.cfg",
        "./default_cli.cfg",
        "./default.cfg"
    };
    
    for (const auto& configFile : defaultConfigs) {
        if (utils::fileExists(configFile)) {
            return CLIConfigParser::parseConfigFile(configFile, config);
        }
    }
    
    return false;
}

void NLMCommandLine::setupLogging() {
    // Parse log level from config
    std::string logLevelStr = getConfigValue("cli_log_level", "INFO");
    
    if (logLevelStr == "QUIET") {
        currentLogLevel = LogLevel::Quiet;
    } else if (logLevelStr == "ERROR") {
        currentLogLevel = LogLevel::Error;
    } else if (logLevelStr == "WARNING") {
        currentLogLevel = LogLevel::Warning;
    } else if (logLevelStr == "DEBUG") {
        currentLogLevel = LogLevel::Debug;
    } else if (logLevelStr == "VERBOSE") {
        currentLogLevel = LogLevel::Verbose;
    } else {
        currentLogLevel = LogLevel::Info; // Default
    }
    
    // Setup file logging if enabled
    bool logToFile = getConfigValue("cli_log_to_file", "false") == "true";
    if (logToFile) {
        std::string logFilename = getConfigValue("cli_log_filename", "nlm_cli.log");
        logFile.open(logFilename, std::ios::app);
        if (!logFile.is_open()) {
            utils::printWarning("Failed to open log file: " + logFilename);
        }
    }
}

void NLMCommandLine::initializeCommands() {
    // Register all commands
    auto& registry = CommandRegistry::getInstance();
    
    registry.registerCommand(std::make_unique<DemoCommand>());
    registry.registerCommand(std::make_unique<IntegrationCommand>());
    registry.registerCommand(std::make_unique<ProfilingCommand>());
    registry.registerCommand(std::make_unique<ExperimentCommand>());
    registry.registerCommand(std::make_unique<ConfigCommand>());
    registry.registerCommand(std::make_unique<CheckpointCommand>());
    registry.registerCommand(std::make_unique<VisualizationCommand>());
    registry.registerCommand(std::make_unique<BrainAnalysisCommand>());
    registry.registerCommand(std::make_unique<VersionCommand>());
    registry.registerCommand(std::make_unique<HelpCommand>());
    registry.registerCommand(std::make_unique<ExitCommand>());
}

void NLMCommandLine::setupHelpSystem() {
    // Setup detailed help information
    helpSystemInfo = {
        {"demo", "Run demonstration mode with basic neural computation"},
        {"integration", "Run integration tests to verify all brain systems work together"},
        {"profile", "Run performance profiling of the brain system"},
        {"experiment", "Run custom experiments with configurable parameters"},
        {"config", "Manage and display configuration settings"},
        {"checkpoint", "Save and load brain checkpoints for experiments"},
        {"viz", "Run visualization modes (text-based)"},
        {"analysis", "Analyze brain state and neural dynamics"},
        {"version", "Display version information"},
        {"help", "Display help information for commands"},
        {"exit", "Exit the application"}
    };
}

void NLMCommandLine::parseCommandLineArgs() {
    commandLineArgs.clear();
    
    // Parse from global argc/argv (in real implementation, would pass these)
    // For now, demonstrate with example args
    commandLineArgs = {"nlm-cli", "--help"};
}

std::string NLMCommandLine::getConfigValue(const std::string& key, const std::string& defaultValue) const {
    auto it = config.find(key);
    return (it != config.end()) ? it->second : defaultValue;
}

void NLMCommandLine::printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Command Line Interface                           ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void NLMCommandLine::printVersion() {
    std::cout << "NLM CLI Version 2.0.0" << std::endl;
    std::cout << "Enhanced command-line interface for NLM brain systems" << std::endl;
    std::cout << "Built with C++20 and modern design patterns" << std::endl;
}

void NLMCommandLine::printHelp() {
    printBanner();
    std::cout << "" << std::endl;
    std::cout << "Usage: nlm-cli <command> [options]" << std::endl;
    std::cout << "" << std::endl;
    
    // Get registered commands
    auto& registry = CommandRegistry::getInstance();
    auto commands = registry.getCommands();
    
    // Group commands by category
    std::unordered_map<std::string, std::vector<Command*>> categorizedCommands;
    for (auto cmd : commands) {
        categorizedCommands[cmd->getCategory()].push_back(cmd);
    }
    
    for (const auto& categoryPair : categorizedCommands) {
        std::cout << categoryPair.first << " Commands:" << std::endl;
        for (const auto& cmd : categoryPair.second) {
            if (!cmd->isHidden()) {
                std::cout << "  " << cmd->getName() << " - " << cmd->getDescription() << std::endl;
            }
        }
        std::cout << "" << std::endl;
    }
    
    std::cout << "Use 'nlm-cli help <command>' for detailed help on a specific command." << std::endl;
    std::cout << "" << std::endl;
}

void NLMCommandLine::printCommandHelp(const std::string& commandName) {
    auto& registry = CommandRegistry::getInstance();
    Command* cmd = registry.getCommand(commandName);
    
    if (!cmd) {
        utils::printError("Unknown command: " + commandName);
        return;
    }
    
    printBanner();
    std::cout << "" << std::endl;
    std::cout << "Command: " << cmd->getName() << std::endl;
    std::cout << "Description: " << cmd->getDescription() << std::endl;
    std::cout << "Usage: " << cmd->getUsage() << std::endl;
    std::cout << "" << std::endl;
    
    // Show command-specific help based on command type
    if (commandName == "demo") {
        std::cout << "Options:" << std::endl;
        std::cout << "  --steps <num>     Number of simulation steps (default: 1000)" << std::endl;
        std::cout << "  --neurons <num>   Number of neurons (default: 1000)" << std::endl;
        std::cout << "  --seed <num>      Random seed for reproducibility" << std::endl;
    } else if (commandName == "integration") {
        std::cout << "Options:" << std::endl;
        std::cout << "  --neurons <num>   Number of neurons for integration test" << std::endl;
        std::cout << "  --steps <num>     Number of simulation steps" << std::endl;
    } else if (commandName == "profile") {
        std::cout << "Options:" << std::endl;
        std::cout << "  --duration <sec>  Profiling duration in seconds (default: 30)" << std::endl;
        std::cout << "  --samples <num>   Number of samples to collect" << std::endl;
        std::cout << "  --simple          Simple profiling without detailed output" << std::endl;
    } else if (commandName == "experiment") {
        std::cout << "Options:" << std::endl;
        std::cout << "  --name <str>      Experiment name" << std::endl;
        std::cout << "  --duration <num>  Duration in steps (default: 10000)" << std::endl;
        std::cout << "  --repetitions <num> Number of repetitions (default: 5)" << std::endl;
        std::cout << "  --no-save         Disable automatic checkpoint saving" << std::endl;
    } else if (commandName == "config") {
        std::cout << "Options:" << std::endl;
        std::cout << "  --show, -s        Show current configuration" << std::endl;
        std::cout << "  --set, -t <key> <value> Set configuration value" << std::endl;
        std::cout << "  --get, -g <key>   Get configuration value" << std::endl;
        std::cout << "  --export, -e      Export configuration to file" << std::endl;
    } else if (commandName == "checkpoint") {
        std::cout << "Options:" << std::endl;
        std::cout << "  save, -s          Save current brain state" << std::endl;
        std::cout << "  load, -l          Load brain state from file" << std::endl;
        std::cout << "  --file <path>     Specify file path (default: checkpoints/default_checkpoint.nlm)" << std::endl;
    } else if (commandName == "viz") {
        std::cout << "Options:" << std::endl;
        std::cout << "  --detailed, -d    Enable detailed visualization" << std::endl;
        std::cout << "  --continuous, -c  Continuous visualization mode" << std::endl;
        std::cout << "  --update-rate <sec> Visualization update rate in seconds" << std::endl;
    }
}

void NLMCommandLine::start() {
    if (!initialized) {
        if (!initialize()) {
            utils::printError("Failed to initialize NLM CLI");
            return;
        }
    }
    
    running = true;
    
    // Check for help/version flags before starting main loop
    for (const auto& arg : commandLineArgs) {
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return;
        }
    }
    
    // Main command processing loop
    while (running && !interruptReceived) {
        // Process next command
        processNextCommand();
    }
    
    shutdown();
}

void NLMCommandLine::processNextCommand() {
    if (commandLineArgs.empty()) {
        return;
    }
    
    // Get first argument as command
    std::string commandName = commandLineArgs[0];
    
    // Remove command from arguments
    commandLineArgs.erase(commandLineArgs.begin());
    
    // Find and execute command
    auto& registry = CommandRegistry::getInstance();
    Command* cmd = registry.getCommand(commandName);
    
    if (!cmd) {
        utils::printError("Unknown command: " + commandName);
        printHelp();
        return;
    }
    
    utils::printInfo("Executing command: " + commandName);
    
    // Execute command
    CLIResult result = cmd->execute(commandLineArgs);
    
    if (result.success) {
        utils::printSuccess("Command completed: " + commandName);
    } else {
        utils::printError("Command failed: " + commandName + " - " + result.message);
    }
    
    // Check for exit command
    if (commandName == "exit") {
        running = false;
    }
}

void NLMCommandLine::shutdown() {
    if (!initialized) {
        return;
    }
    
    utils::printInfo("Shutting down NLM CLI...");
    
    // Save configuration if requested
    std::string saveOnExit = getConfigValue("config_mode_save_on_exit", "true");
    if (saveOnExit == "true") {
        std::string configFile = "configs/default_cli.cfg";
        // In real implementation, would save current config
        utils::printInfo("Configuration would be saved to: " + configFile);
    }
    
    // Close log file
    if (logFile.is_open()) {
        logFile.close();
    }
    
    initialized = false;
    running = false;
    
    utils::printSuccess("NLM CLI shutdown complete.");
}

void NLMCommandLine::logMessage(const std::string& message, LogLevel level) {
    if (level < currentLogLevel && currentLogLevel != LogLevel::Quiet) {
        return;
    }
    
    // Print to console with appropriate formatting
    switch (level) {
        case LogLevel::Error:
            utils::printError(message);
            break;
        case LogLevel::Warning:
            utils::printWarning(message);
            break;
        case LogLevel::Info:
            utils::printInfo(message);
            break;
        case LogLevel::Debug:
            utils::printDebug(message);
            break;
        case LogLevel::Verbose:
            utils::printInfo("[VERBOSE] " + message);
            break;
        case LogLevel::Quiet:
            break;
    }
    
    // Write to log file if enabled
    if (logFile.is_open()) {
        logFile << utils::currentTimestamp() << " [" 
                << (level == LogLevel::Error ? "ERROR" :
                    level == LogLevel::Warning ? "WARNING" :
                    level == LogLevel::Info ? "INFO" :
                    level == LogLevel::Debug ? "DEBUG" : "VERBOSE")
                << "] " << message << std::endl;
    }
}

} // namespace cli
} // namespace nlm