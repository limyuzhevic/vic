#include "CommandLineParser.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include "StringUtils.hpp"

namespace nlm {

struct ConfigurationManager::Impl {
    std::unordered_map<std::string, std::string> values;
    std::unordered_map<std::string, std::string> defaults;
};

ConfigurationManager::ConfigurationManager() : pImpl_(std::make_unique<Impl>()) {}

ConfigurationManager::~ConfigurationManager() = default;

bool ConfigurationManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line.find('#') == 0) {
            continue;
        }
        
        // Parse key=value format
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        // Trim whitespace
        key = trim(key);
        value = trim(value);
        
        // Store value
        values[key] = value;
        
        // If not already in defaults, add to defaults
        if (defaults.find(key) == defaults.end()) {
            defaults[key] = value;
        }
    }
    
    return true;
}

bool ConfigurationManager::loadFromCommandLine(int argc, char* argv[]) {
    CommandLineParser parser;
    
    // Add common configuration options
    parser.addOption("config", 'c', "Configuration file path", true, "configs/default.cfg");
    parser.addOption("neuron-count", 'n', "Number of neurons", true, "500");
    parser.addOption("steps", 's', "Number of simulation steps", true, "1000");
    parser.addOption("verbose", 'v', "Enable verbose logging", false, "false", false);
    parser.addOption("quiet", 'q', "Suppress non-error output", false, "false", false);
    parser.addOption("log-level", 0, "Log level (debug, info, warning, error, critical)", 
                    true, "info", false);
    parser.addOption("help", 'h', "Print help message", false, "false", false);
    parser.addOption("version", 0, "Print version information", false, "false", false);
    parser.addOption("benchmark", 0, "Run benchmark mode", false, "false", false);
    
    if (!parser.parse(argc, argv)) {
        return false;
    }
    
    if (parser.hasOption("help")) {
        parser.printHelp();
        return false; // Exit after showing help
    }
    
    if (parser.hasOption("version")) {
        std::cout << "NLM Phase 2 Demo Version 0.1.0" << std::endl;
        return false; // Exit after showing version
    }
    
    // Load configuration file if specified
    std::string configFile = parser.getOption("config");
    if (!configFile.empty() && configFile != "configs/default.cfg") {
        loadFromFile(configFile);
    }
    
    // Override with command line values
    set("neuron_count", static_cast<int64_t>(std::stoll(parser.getOption("neuron-count"))));
    set("simulation_steps", static_cast<int64_t>(std::stoll(parser.getOption("steps"))));
    
    // Handle boolean options
    std::string verbose = parser.getOption("verbose");
    set("verbose_logging", verbose == "true" || verbose == "1" || verbose == "yes");
    
    std::string quiet = parser.getOption("quiet");  
    set("quiet_logging", quiet == "true" || quiet == "1" || quiet == "yes");
    
    // Handle log level
    std::string logLevel = parser.getOption("log-level");
    set("log_level", logLevel);
    
    return true;
}

template<typename T>
T ConfigurationManager::get(const std::string& key, const T& defaultValue) const {
    auto it = values.find(key);
    if (it == values.end()) {
        // Try defaults
        auto defaultIt = defaults.find(key);
        if (defaultIt != defaults.end()) {
            return convertToType<T>(defaultIt->second, defaultValue);
        }
        return defaultValue;
    }
    
    return convertToType<T>(it->second, defaultValue);
}

template<typename T>
void ConfigurationManager::set(const std::string& key, const T& value) {
    std::stringstream ss;
    ss << value;
    values[key] = ss.str();
}

bool ConfigurationManager::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& pair : values) {
        file << pair.first << "=" << pair.second << std::endl;
    }
    
    return true;
}

std::vector<std::string> ConfigurationManager::getAllKeys() const {
    std::vector<std::string> keys;
    keys.reserve(values.size() + defaults.size());
    
    for (const auto& pair : values) {
        keys.push_back(pair.first);
    }
    
    for (const auto& pair : defaults) {
        if (values.find(pair.first) == values.end()) {
            keys.push_back(pair.first);
        }
    }
    
    return keys;
}

void ConfigurationManager::printConfiguration() const {
    std::cout << "=== Current Configuration ===" << std::endl;
    for (const auto& pair : values) {
        std::cout << pair.first << " = " << pair.second << std::endl;
    }
    std::cout << "=========================" << std::endl;
}

void ConfigurationManager::resetToDefaults() {
    values.clear();
}

} // namespace nlm
