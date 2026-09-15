// Brain CLI Configuration System Implementation
// Provides configuration management for the NLM Brain CLI

#include "CLIConfig.hpp"
#include "../src/core/Logger/Logger.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <sstream>

namespace nlm {
namespace cli {

CLIConfig::CLIConfig() {
    initDefaultOptions();
    initApplicationOptions();
    initBrainOptions();
    initVisualizationOptions();
    initAnalysisOptions();
    initPluginOptions();
    initPerformanceOptions();
    initLoggingOptions();
    
    // Set default directories
    defaultConfigDir_ = "./configs";
    defaultLogDir_ = "./logs";
    defaultDataDir_ = "./data";
    
    // Create directories if they don't exist
    std::filesystem::create_directories(defaultConfigDir_);
    std::filesystem::create_directories(defaultLogDir_);
    std::filesystem::create_directories(defaultDataDir_);
    
    NLM_LOG_INFO("CLI Configuration initialized");
}

CLIConfig::~CLIConfig() = default;

void CLIConfig::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        errors_.push_back("Cannot open configuration file: " + filename);
        return;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        
        // Skip comments and empty lines
        if (line.empty() || line.find('#') == 0) {
            continue;
        }
        
        // Parse key = value format
        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) {
            errors_.push_back("Invalid format in line " + std::to_string(lineNum) + ": " + line);
            continue;
        }
        
        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);
        
        // Trim whitespace
        key = trim(key);
        value = trim(value);
        
        if (key.empty() || value.empty()) {
            continue;
        }
        
        configValues_[key] = value;
    }
    
    file.close();
    
    if (errors_.empty()) {
        NLM_LOG_INFO("Configuration loaded from: " + filename);
    }
}

void CLIConfig::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        errors_.push_back("Cannot write configuration to file: " + filename);
        return;
    }
    
    // Sort keys for consistent output
    std::vector<std::string> keys;
    for (const auto& pair : configValues_) {
        keys.push_back(pair.first);
    }
    std::sort(keys.begin(), keys.end());
    
    for (const auto& key : keys) {
        file << key << " = " << configValues_.at(key) << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Configuration saved to: " + filename);
}

void CLIConfig::loadFromArgs(int argc, char** argv) {
    commandLine_.clear();
    for (int i = 0; i < argc; ++i) {
        if (i > 0) commandLine_ += " ";
        commandLine_ += argv[i];
    }
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            setString("general.help", "true");
        } else if (arg.find("--") == 0) {
            // Long option: --key=value or --key value
            size_t eqPos = arg.find('=');
            std::string key, value;
            
            if (eqPos != std::string::npos) {
                key = arg.substr(2, eqPos - 2);
                value = arg.substr(eqPos + 1);
            } else {
                key = arg.substr(2);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    value = argv[++i];
                }
            }
            
            // Convert key to lowercase
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            
            // Set the configuration value
            if (!value.empty()) {
                setString(key, value);
            }
        } else if (arg[0] == '-') {
            // Short option: -x value
            std::string key = arg.substr(1);
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                setString(key, argv[++i]);
            }
        }
    }
}

std::string CLIConfig::getString(const std::string& key) const {
    auto it = configValues_.find(key);
    if (it != configValues_.end()) {
        return it->second;
    }
    
    // Find matching option
    for (const auto& option : configOptions_) {
        if (option.key == key) {
            return option.defaultValue;
        }
    }
    
    return "";
}

int64_t CLIConfig::getInt(const std::string& key) const {
    std::string value = getString(key);
    try {
        return std::stoll(value);
    } catch (...) {
        return 0;
    }
}

double CLIConfig::getDouble(const std::string& key) const {
    std::string value = getString(key);
    try {
        return std::stod(value);
    } catch (...) {
        return 0.0;
    }
}

bool CLIConfig::getBool(const std::string& key) const {
    std::string value = getString(key);
    return parseBool(value);
}

float CLIConfig::getFloat(const std::string& key) const {
    std::string value = getString(key);
    try {
        return std::stof(value);
    } catch (...) {
        return 0.0f;
    }
}

void CLIConfig::setString(const std::string& key, const std::string& value) {
    configValues_[key] = value;
}

void CLIConfig::setInt(const std::string& key, int64_t value) {
    configValues_[key] = std::to_string(value);
}

void CLIConfig::setDouble(const std::string& key, double value) {
    configValues_[key] = std::to_string(value);
}

void CLIConfig::setBool(const std::string& key, bool value) {
    configValues_[key] = value ? "true" : "false";
}

void CLIConfig::setFloat(const std::string& key, float value) {
    configValues_[key] = std::to_string(value);
}

bool CLIConfig::validate() const {
    errors_.clear();
    
    // Check required options
    for (const auto& option : configOptions_) {
        if (option.required && configValues_.find(option.key) == configValues_.end()) {
            errors_.push_back("Required option missing: " + option.key);
        }
    }
    
    // Validate boolean values
    for (const auto& pair : configValues_) {
        if (pair.first.find("bool") != std::string::npos || 
            pair.first.find("enable") != std::string::npos ||
            pair.first.find("flag") != std::string::npos) {
            if (!parseBool(pair.second)) {
                errors_.push_back("Invalid boolean value for option: " + pair.first);
            }
        }
    }
    
    return errors_.empty();
}

std::vector<std::string> CLIConfig::getErrors() const {
    return errors_;
}

void CLIConfig::loadTemplate(const std::string& templateName) {
    std::string filename = defaultConfigDir_ + "/" + templateName + ".cfg";
    loadFromFile(filename);
}

void CLIConfig::saveTemplate(const std::string& templateName) const {
    std::string filename = defaultConfigDir_ + "/" + templateName + ".cfg";
    saveToFile(filename);
}

std::vector<std::string> CLIConfig::search(const std::string& pattern) const {
    std::vector<std::string> results;
    std::string lowerPattern = toLower(pattern);
    
    for (const auto& pair : configValues_) {
        if (toLower(pair.first).find(lowerPattern) != std::string::npos) {
            results.push_back(pair.first + " = " + pair.second);
        }
    }
    
    return results;
}

void CLIConfig::showHelp() const {
    std::cout << "NLM Brain CLI Configuration Options:" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Group by category
    std::map<std::string, std::vector<const ConfigOption*>> categorized;
    
    for (const auto& option : configOptions_) {
        categorized[option.category].push_back(&option);
    }
    
    for (const auto& pair : categorized) {
        std::cout << "\n" << pair.first << ":" << std::endl;
        for (const auto* option : pair.second) {
            std::cout << "  --" << option->key << "=" << option->defaultValue 
                     << "  " << option->description << std::endl;
        }
    }
}

void CLIConfig::showCategories() const {
    std::cout << "Configuration categories:" << std::endl;
    std::set<std::string> categories;
    for (const auto& option : configOptions_) {
        categories.insert(option.category);
    }
    
    for (const auto& category : categories) {
        std::cout << "  - " << category << std::endl;
    }
}

void CLIConfig::merge(const CLIConfig& other) {
    for (const auto& pair : other.configValues_) {
        configValues_[pair.first] = pair.second;
    }
}

void CLIConfig::reset() {
    configValues_.clear();
    errors_.clear();
    initDefaultOptions();
}

void CLIConfig::initDefaultOptions() {
    configOptions_ = {
        // Application options
        {"app.name", "NLM Brain CLI", ConfigValueType::STRING, "NLM Brain CLI", "Application", false},
        {"app.version", "2.0.0", ConfigValueType::STRING, "2.0.0", "Application", false},
        {"app.description", "Neural Learning Machine Brain Simulation CLI", ConfigValueType::STRING, "Neural Learning Machine Brain Simulation CLI", "Application", false},
        
        // Brain options
        {"brain.default_file", "brain.bin", ConfigValueType::STRING, "brain.bin", "Brain", false},
        {"brain.max_neurons", "1000000", ConfigValueType::STRING, "1000000", "Brain", false},
        {"brain.max_synapses", "100000000", ConfigValueType::STRING, "100000000", "Brain", false},
        
        // Simulation options
        {"simulation.default_timestep", "0.001", ConfigValueType::STRING, "0.001", "Simulation", false},
        {"simulation.max_steps", "1000000", ConfigValueType::STRING, "1000000", "Simulation", false},
        
        // Visualization options
        {"visualization.default_format", "png", ConfigValueType::STRING, "png", "Visualization", false},
        {"visualization.width", "1920", ConfigValueType::STRING, "1920", "Visualization", false},
        {"visualization.height", "1080", ConfigValueType::STRING, "1080", "Visualization", false},
        
        // Analysis options
        {"analysis.threshold", "0.01", ConfigValueType::STRING, "0.01", "Analysis", false},
        {"analysis.max_clusters", "100", ConfigValueType::STRING, "100", "Analysis", false},
        
        // Plugin options
        {"plugins.auto_load", "false", ConfigValueType::STRING, "false", "Plugins", false},
        {"plugins.path", "./plugins", ConfigValueType::STRING, "./plugins", "Plugins", false},
        
        // Performance options
        {"performance.max_memory", "0", ConfigValueType::STRING, "0", "Performance", false},
        {"performance.cpu_cores", "0", ConfigValueType::STRING, "0", "Performance", false},
        
        // Logging options
        {"logging.level", "INFO", ConfigValueType::STRING, "INFO", "Logging", false},
        {"logging.file", "nlm.log", ConfigValueType::STRING, "nlm.log", "Logging", false},
        {"logging.max_size", "10485760", ConfigValueType::STRING, "10485760", "Logging", false},
        {"logging.backup_count", "5", ConfigValueType::STRING, "5", "Logging", false},
    };
}

void CLIConfig::initApplicationOptions() {
    // Add application-specific options
}

void CLIConfig::initBrainOptions() {
    // Add brain-specific options
}

void CLIConfig::initVisualizationOptions() {
    // Add visualization-specific options
}

void CLIConfig::initAnalysisOptions() {
    // Add analysis-specific options
}

void CLIConfig::initPluginOptions() {
    // Add plugin-specific options
}

void CLIConfig::initPerformanceOptions() {
    // Add performance-specific options
}

void CLIConfig::initLoggingOptions() {
    // Add logging-specific options
}

std::string CLIConfig::trim(const std::string& str) const {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

bool CLIConfig::parseBool(const std::string& str) const {
    std::string lower = toLower(str);
    return (lower == "true" || lower == "1" || lower == "yes" || lower == "on");
}

int64_t CLIConfig::parseInt(const std::string& str) const {
    try {
        return std::stoll(str);
    } catch (...) {
        return 0;
    }
}

double CLIConfig::parseDouble(const std::string& str) const {
    try {
        return std::stod(str);
    } catch (...) {
        return 0.0;
    }
}

float CLIConfig::parseFloat(const std::string& str) const {
    try {
        return std::stof(str);
    } catch (...) {
        return 0.0f;
    }
}

std::string CLIConfig::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Global configuration instance
namespace {
    CLIConfig globalConfig;
}

CLIConfig& getGlobalConfig() {
    return globalConfig;
}

} // namespace cli
} // namespace nlm
