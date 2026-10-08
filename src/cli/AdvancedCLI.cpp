// Advanced CLI implementation
// Provides enhanced command-line interface with improved options and features

#include "cli/AdvancedCLI.hpp"
#include "core/Config/Config.hpp"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace nlm {
namespace cli {

AdvancedCLI::AdvancedCLI() 
    : argc_(0), argv_(nullptr), config_(std::make_shared<Config>()),
      help_(false), version_(false), quiet_(false), verbose_(false), 
      debug_(false), outputFormat_("text"), logLevel_("info"), 
      configFile_(""), workspaceDir_(".") {
}

AdvancedCLI::~AdvancedCLI() {
}

bool AdvancedCLI::parseArgs(int argc, char** argv) {
    argc_ = argc;
    argv_ = argv;
    
    // Define command line options
    std::vector<CommandOption> options = {
        {"--help", "Print help information", "", false, false},
        {"--version", "Print version information", "", false, false},
        {"--quiet", "Quiet mode - suppress non-essential output", "", false, false},
        {"--verbose", "Verbose mode - enable detailed output", "", false, false},
        {"--debug", "Debug mode - enable debugging output", "", false, false},
        {"--output", "Output format (text, json, yaml, csv)", formats::TEXT, true, false},
        {"--log-level", "Log level (debug, info, warning, error, critical)", logLevels::INFO, true, false},
        {"--config", "Configuration file", "", true, false},
        {"--workspace", "Workspace directory", ".", true, false},
    };
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Check for long options
        if (arg.substr(0, 2) == "--") {
            std::string flag = arg.substr(0, arg.find('='));
            std::string value = "";
            
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                flag = arg.substr(0, pos);
                value = arg.substr(pos + 1);
            } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                value = argv[++i];
            }
            
            parseArgs(flag, value);
        }
        // Check for short options
        else if (arg[0] == '-' && arg.size() > 1) {
            for (size_t j = 1; j < arg.size(); ++j) {
                std::string flag("-" + std::string(1, arg[j]));
                parseArgs(flag, "");
            }
        }
        // Positional argument (first non-option after parsing)
        else if (command_.empty()) {
            command_ = arg;
            i--; // Adjust index for next iteration
        } else {
            commandArgs_.push_back(arg);
        }
    }
    
    return validateArguments();
}

int AdvancedCLI::execute() {
    if (help_) {
        printHelp();
        return 0;
    }
    
    if (version_) {
        printVersion();
        return 0;
    }
    
    // Load configuration
    if (!configFile_.empty() && !loadConfiguration()) {
        printError("Failed to load configuration file: " + configFile_);
        return 1;
    }
    
    // Apply command line arguments
    applyConfigurationFromArgs();
    
    // Execute command
    if (command_ == "run") {
        if (!handleRunCommand()) {
            printError("Run command failed");
            return 1;
        }
    } else if (command_ == "config") {
        if (!handleConfigCommand()) {
            printError("Config command failed");
            return 1;
        }
    } else if (command_ == "export") {
        if (!handleExportCommand()) {
            printError("Export command failed");
            return 1;
        }
    } else if (command_ == "import") {
        if (!handleImportCommand()) {
            printError("Import command failed");
            return 1;
        }
    } else if (command_ == "validate") {
        if (!handleValidateCommand()) {
            printError("Validate command failed");
            return 1;
        }
    } else if (!command_.empty()) {
        printError("Unknown command: " + command_);
        return 1;
    } else {
        printHelp();
        return 1;
    }
    
    return 0;
}

void AdvancedCLI::printHelp() const {
    std::cout << "NLM - Neural Learning Machine (Advanced CLI)" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Usage:" << std::endl;
    std::cout << "  nlm [options] [command] [args]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Commands:" << std::endl;
    std::cout << "  run              Run simulation" << std::endl;
    std::cout << "  config           Manage configuration" << std::endl;
    std::cout << "  export <file>    Export configuration to file" << std::endl;
    std::cout << "  import <file>    Import configuration from file" << std::endl;
    std::cout << "  validate         Validate configuration" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  --help              Print help information" << std::endl;
    std::cout << "  --version           Print version information" << std::endl;
    std::cout << "  --quiet             Quiet mode - suppress non-essential output" << std::endl;
    std::cout << "  --verbose           Verbose mode - enable detailed output" << std::endl;
    std::cout << "  --debug             Debug mode - enable debugging output" << std::endl;
    std::cout << "  --output FORMAT     Output format (text, json, yaml, csv)" << std::endl;
    std::cout << "  --log-level LEVEL   Log level (debug, info, warning, error, critical)" << std::endl;
    std::cout << "  --config FILE       Configuration file" << std::endl;
    std::cout << "  --workspace DIR      Workspace directory" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --config myconfig.cfg run" << std::endl;
    std::cout << "  nlm --output json config show" << std::endl;
    std::cout << "  nlm export config.json" << std::endl;
    std::cout << "  nlm import config.json validate" << std::endl;
}

void AdvancedCLI::printVersion() const {
    std::cout << "NLM (Neural Learning Machine)" << std::endl;
    std::cout << "Version 0.1.0" << std::endl;
    std::cout << "Phase 6 - Final Integration" << std::endl;
    std::cout << std::endl;
    std::cout << "Built with C++20" << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  - Real spiking neural computation" << std::endl;
    std::cout << "  - Leaky Integrate-and-Fire (LIF) neurons" << std::endl;
    std << "  - Event-driven spike propagation" << std::endl;
    std << "  - STDP and Hebbian plasticity" << std::endl;
    std << "  - Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std << "  - Working memory systems" << std::endl;
    std << "  - Episodic memory" << std::endl;
    std << "  - Neuromodulation (dopamine, curiosity, novelty)" << std::endl;
    std << "  - Prediction systems" << std::endl;
    std << "  - Cognition (planning, concept formation)" << std::endl;
    std << "  - Development stages" << std::endl;
    std << "  - Checkpoint save/load" << std::endl;
    std << "  - Replay and consolidation" << std::endl;
}

void AdvancedCLI::printConfig() const {
    if (!quiet_) {
        std::cout << config_->summary() << std::endl;
    }
}

bool AdvancedCLI::exportConfig(const std::string& filename) const {
    return writeFile(filename, config_->exportToString());
}

bool AdvancedCLI::importConfig(const std::string& filename) {
    std::string content;
    if (!readFile(filename, content)) {
        return false;
    }
    
    return config_->importFromString(content);
}

bool AdvancedCLI::validateConfig() const {
    return validateConfiguration();
}

bool AdvancedCLI::handleHelpCommand() const {
    printHelp();
    return true;
}

bool AdvancedCLI::handleVersionCommand() const {
    printVersion();
    return true;
}

bool AdvancedCLI::handleRunCommand() const {
    // For now, just print that we're running
    if (!quiet_) {
        printOutput("Running NLM simulation with configuration:");
        printOutput(config_->summary());
    }
    return true;
}

bool AdvancedCLI::handleConfigCommand() const {
    if (commandArgs_.empty()) {
        printOutput("Available config commands:");
        printOutput("  show - Show current configuration");
        printOutput("  set <key> <value> - Set a configuration value");
        printOutput("  get <key> - Get a configuration value");
        printOutput("  list - List all configuration keys");
        return true;
    }
    
    std::string subcommand = commandArgs_[0];
    
    if (subcommand == "show") {
        printConfig();
        return true;
    } else if (subcommand == "list") {
        auto keys = config_->getKeys();
        printOutput("Configuration keys:");
        for (const auto& key : keys) {
            printOutput("  " + key);
        }
        return true;
    } else if (subcommand == "get" && commandArgs_.size() >= 2) {
        std::string key = commandArgs_[1];
        std::string valueType = config_->getValueType(key);
        printOutput("Key: " + key);
        printOutput("Type: " + valueType);
        
        if (valueType != "NOT_FOUND") {
            // For simplicity, just show that value exists
            printOutput("Value: [type not displayed for simplicity]");
        } else {
            printOutput("Key not found");
        }
        return true;
    } else if (subcommand == "set" && commandArgs_.size() >= 3) {
        std::string key = commandArgs_[1];
        std::string valueStr = commandArgs_[2];
        
        // Try to parse the value to the appropriate type
        try {
            // Try as boolean
            if (valueStr == "true" || valueStr == "false") {
                bool boolValue = (valueStr == "true");
                config_->set(key, boolValue);
            }
            // Try as integer
            else if (std::all_of(valueStr.begin(), valueStr.end(), ::isdigit) ||
                    (valueStr.front() == '-' && std::all_of(valueStr.begin() + 1, valueStr.end(), ::isdigit))) {
                int intValue = std::stoi(valueStr);
                config_->set(key, intValue);
            }
            // Try as double
            else {
                size_t pos;
                double doubleValue = std::stod(valueStr, &pos);
                if (pos == valueStr.size()) {
                    config_->set(key, doubleValue);
                } else {
                    config_->set(key, valueStr);
                }
            }
            
            printOutput("Set " + key + " = " + valueStr);
            return true;
        } catch (...) {
            printError("Failed to parse value: " + valueStr);
            return false;
        }
    }
    
    printError("Unknown config subcommand: " + subcommand);
    return false;
}

bool AdvancedCLI::handleExportCommand() const {
    if (commandArgs_.empty()) {
        printError("Export command requires a filename");
        return false;
    }
    
    return exportConfig(commandArgs_[0]);
}

bool AdvancedCLI::handleImportCommand() {
    if (commandArgs_.empty()) {
        printError("Import command requires a filename");
        return false;
    }
    
    return importConfig(commandArgs_[0]);
}

bool AdvancedCLI::handleValidateCommand() const {
    return validateConfig();
}

bool AdvancedCLI::loadConfiguration() {
    return config_->loadFromFile(configFile_);
}

bool AdvancedCLI::saveConfiguration() const {
    // For now, just save to the specified config file
    std::string configFile = configFile_.empty() ? "nlm_config.cfg" : configFile_;
    return config_->saveToFile(configFile);
}

bool AdvancedCLI::applyConfigurationFromArgs() {
    // Apply command line configuration overrides
    // This is a simplified implementation
    return true;
}

void AdvancedCLI::parseCommandLine() {
    // Parse command line arguments
    // This is a simplified implementation
}

void AdvancedCLI::parseCommand(const std::string& cmd) {
    command_ = cmd;
}

void AdvancedCLI::parseArgs(const std::string& key, const std::string& value) {
    // Parse command line arguments
    // This is a simplified implementation
}

bool AdvancedCLI::validateArguments() const {
    return true;
}

bool AdvancedCLI::validateConfiguration() const {
    // Validate configuration
    // This is a simplified implementation
    return true;
}

void AdvancedCLI::printOutput(const std::string& message, bool useFormat) const {
    if (quiet_ && message.find("ERROR") == std::string::npos && 
        message.find("WARNING") == std::string::npos) {
        return;
    }
    
    if (useFormat && outputFormat_ == "json") {
        // Output in JSON format
        std::cout << "{\"message\": \"" << message << "\"}" << std::endl;
    } else if (useFormat && outputFormat_ == "yaml") {
        // Output in YAML format
        std::cout << "message: " << message << std::endl;
    } else if (useFormat && outputFormat_ == "csv") {
        // Output in CSV format
        std::cout << "\"" << message << "\""" << std::endl;
    } else {
        // Output in text format
        std::cout << message << std::endl;
    }
}

void AdvancedCLI::printError(const std::string& error) const {
    printOutput("[ERROR] " + error, true);
}

void AdvancedCLI::printWarning(const std::string& warning) const {
    printOutput("[WARNING] " + warning, true);
}

bool AdvancedCLI::readFile(const std::string& filename, std::string& content) const {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return true;
}

bool AdvancedCLI::writeFile(const std::string& filename, const std::string& content) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

template<typename T>
T AdvancedCLI::convertStringToType(const std::string& value) const {
    // This is a placeholder - implement actual type conversion
    return T();
}

template<typename T>
std::string AdvancedCLI::convertTypeToString(const T& value) const {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void AdvancedCLI::completeCommand(std::string& cmd) const {
    // Complete command
    // This is a simplified implementation
}

} // namespace cli
} // namespace nlm