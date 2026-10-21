#pragma once

/**
 * Configuration Management System
 * 
 * This header provides the main Config class for managing neural simulation parameters.
 * Supports multiple configuration sources, file I/O, and command-line overrides.
 * 
 * Key features:
 * - Multiple configuration sources (file, command-line, runtime, default)
 * - Type-safe configuration access with validation
 * - File serialization/deserialization
 * - Command-line argument parsing
 * - Configuration validation
 * 
 * Usage:
 *   Config config;
 *   config.set("brain.neuron_count", 1000, ConfigSource::Runtime);
 *   config->getOr<int>("brain.neuron_count", 1000);
 */

#include <string>
#include <unordered_map>
#include <variant>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdint>

namespace nlm {

/**
 * Enumeration of configuration sources
 */
enum class ConfigSource {
    File,           ///< Loaded from configuration file
    CommandLine,    ///< Set from command line arguments
    Runtime,        ///< Set during runtime
    Default         ///< Default value
};

/**
 * Exception thrown when configuration validation fails
 */
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message)
        : std::runtime_error("Configuration Error: " + message) {}
};

/**
 * Configuration entry with type and source tracking
 */
template<typename T>
struct ConfigEntry {
    T value;
    ConfigSource source;
    uint64_t timestamp;  // For tracking when configuration was set
    
    ConfigEntry(const T& val, ConfigSource src)
        : value(val), source(src), timestamp(0) {}
};

/**
 * Configuration entry for string values (special handling for strings)
 */
struct StringConfigEntry {
    std::string value;
    ConfigSource source;
    uint64_t timestamp;
    
    StringConfigEntry(const std::string& val, ConfigSource src)
        : value(val), source(src), timestamp(0) {}
};

/**
 * Configuration entry for boolean values
 */
struct BoolConfigEntry {
    bool value;
    ConfigSource source;
    uint64_t timestamp;
    
    BoolConfigEntry(bool val, ConfigSource src)
        : value(val), source(src), timestamp(0) {}
};

/**
 * Configuration entry for integer values
 */
struct IntConfigEntry {
    int64_t value;
    ConfigSource source;
    uint64_t timestamp;
    
    IntConfigEntry(int64_t val, ConfigSource src)
        : value(val), source(src), timestamp(0) {}
};

/**
 * Configuration entry for float values
 */
struct FloatConfigEntry {
    float value;
    ConfigSource source;
    uint64_t timestamp;
    
    FloatConfigEntry(float val, ConfigSource src)
        : value(val), source(src), timestamp(0) {}
};

/**
 * Main configuration class for neural simulation parameters
 */
class Config {
private:
    // Type-safe storage using variant
    std::unordered_map<std::string, std::variant<
        StringConfigEntry,
        BoolConfigEntry,
        IntConfigEntry,
        FloatConfigEntry
    >> configMap;
    
    std::vector<std::string> configKeys;
    uint64_t nextTimestamp;
    
    // Helper functions for type operations
    template<typename T>
    static std::string getTypeName() {
        if (std::is_same_v<T, std::string>) return "string";
        if (std::is_same_v<T, bool>) return "bool";
        if (std::is_same_v<T, int64_t>) return "int64";
        if (std::is_same_v<T, float>) return "float";
        return "unknown";
    }
    
public:
    Config() : nextTimestamp(0) {}
    
    /**
     * Set a configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of the configuration
     */
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source) {
        configMap[key] = ConfigEntry<T>(value, source);
        configKeys.push_back(key);
        nextTimestamp++;
    }
    
    /**
     * Check if a configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if key exists, false otherwise
     */
    bool has(const std::string& key) const {
        return configMap.find(key) != configMap.end();
    }
    
    /**
     * Get a configuration value of specified type
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @return Optional containing the value if successful, empty optional if key doesn't exist or type mismatch
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        auto it = configMap.find(key);
        if (it == configMap.end()) {
            return std::nullopt;
        }
        
        const auto& entry = it->second;
        
        // Check type compatibility
        if (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<StringConfigEntry>(entry)) {
                return std::get<StringConfigEntry>(entry).value;
            }
        } else if (std::is_same_v<T, bool>) {
            if (std::holds_alternative<BoolConfigEntry>(entry)) {
                return std::get<BoolConfigEntry>(entry).value;
            }
        } else if (std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<IntConfigEntry>(entry)) {
                return std::get<IntConfigEntry>(entry).value;
            } else if (std::holds_alternative<FloatConfigEntry>(entry)) {
                return static_cast<int64_t>(std::get<FloatConfigEntry>(entry).value);
            } else if (std::holds_alternative<StringConfigEntry>(entry)) {
                try {
                    size_t pos;
                    int64_t val = std::stoll(std::get<StringConfigEntry>(entry).value, &pos);
                    if (pos == std::get<StringConfigEntry>(entry).value.length()) {
                        return val;
                    }
                } catch (...) {
                    // String to int conversion failed
                }
            }
        } else if (std::is_same_v<T, float>) {
            if (std::holds_alternative<FloatConfigEntry>(entry)) {
                return std::get<FloatConfigEntry>(entry).value;
            } else if (std::holds_alternative<IntConfigEntry>(entry)) {
                return static_cast<float>(std::get<IntConfigEntry>(entry).value);
            } else if (std::holds_alternative<StringConfigEntry>(entry)) {
                try {
                    size_t pos;
                    float val = std::stof(std::get<StringConfigEntry>(entry).value, &pos);
                    if (pos == std::get<StringConfigEntry>(entry).value.length()) {
                        return val;
                    }
                } catch (...) {
                    // String to float conversion failed
                }
            }
        }
        
        return std::nullopt;
    }
    
    /**
     * Get configuration value with default if key doesn't exist
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @param defaultValue Default value to return if key doesn't exist
     * @return Value from config or default value
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto value = get<T>(key);
        if (value.has_value()) {
            return *value;
        }
        return defaultValue;
    }
    
    /**
     * Get the source of a configuration value
     * 
     * @param key Configuration key
     * @return ConfigSource of the key, or ConfigSource::Default if key doesn't exist
     */
    ConfigSource getSource(const std::string& key) const {
        auto it = configMap.find(key);
        if (it == configMap.end()) {
            return ConfigSource::Default;
        }
        
        const auto& entry = it->second;
        if (std::holds_alternative<StringConfigEntry>(entry)) {
            return std::get<StringConfigEntry>(entry).source;
        } else if (std::holds_alternative<BoolConfigEntry>(entry)) {
            return std::get<BoolConfigEntry>(entry).source;
        } else if (std::holds_alternative<IntConfigEntry>(entry)) {
            return std::get<IntConfigEntry>(entry).source;
        } else if (std::holds_alternative<FloatConfigEntry>(entry)) {
            return std::get<FloatConfigEntry>(entry).source;
        }
        
        return ConfigSource::Default;
    }
    
    /**
     * Remove a configuration key
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key) {
        configMap.erase(key);
        // Note: Not removing from configKeys for simplicity
    }
    
    /**
     * Get all configuration keys
     * 
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const {
        return configKeys;
    }
    
    /**
     * Load configuration from a file
     * 
     * @param filename Path to configuration file
     * @return true if configuration was loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Parse key = value format
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                continue;
            }
            
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t\n\r"));
            key.erase(key.find_last_not_of(" \t\n\r") + 1);
            value.erase(0, value.find_first_not_of(" \t\n\r"));
            value.erase(value.find_last_not_of(" \t\n\r") + 1);
            
            // Parse value based on content
            if (value == "true" || value == "false") {
                bool boolVal = (value == "true");
                set<bool>(key, boolVal, ConfigSource::File);
            } else if (value.find('.') != std::string::npos) {
                try {
                    float floatVal = std::stof(value);
                    set<float>(key, floatVal, ConfigSource::File);
                } catch (...) {
                    // Try as int
                    try {
                        int64_t intVal = std::stoll(value);
                        set<int64_t>(key, intVal, ConfigSource::File);
                    } catch (...) {
                        set<std::string>(key, value, ConfigSource::File);
                    }
                }
            } else {
                try {
                    int64_t intVal = std::stoll(value);
                    set<int64_t>(key, intVal, ConfigSource::File);
                } catch (...) {
                    set<std::string>(key, value, ConfigSource::File);
                }
            }
        }
        
        file.close();
        return true;
    }
    
    /**
     * Save configuration to a file
     * 
     * @param filename Path to save configuration to
     * @return true if configuration was saved successfully, false otherwise
     */
    bool saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# NLM Configuration File" << std::endl;
        file << "# Generated by NLM configuration system" << std::endl;
        file << std::endl;
        
        for (const auto& key : configKeys) {
            auto it = configMap.find(key);
            if (it == configMap.end()) continue;
            
            const auto& entry = it->second;
            std::string value;
            
            if (std::holds_alternative<StringConfigEntry>(entry)) {
                value = std::get<StringConfigEntry>(entry).value;
            } else if (std::holds_alternative<BoolConfigEntry>(entry)) {
                value = std::get<BoolConfigEntry>(entry).value ? "true" : "false";
            } else if (std::holds_alternative<IntConfigEntry>(entry)) {
                value = std::to_string(std::get<IntConfigEntry>(entry).value);
            } else if (std::holds_alternative<FloatConfigEntry>(entry)) {
                value = std::to_string(std::get<FloatConfigEntry>(entry).value);
            }
            
            file << key << " = " << value << std::endl;
        }
        
        file.close();
        return true;
    }
    
    /**
     * Load configuration from command line arguments
     * 
     * @param argc Number of command line arguments
     * @param argv Command line arguments
     * @return true if configuration was loaded successfully, false otherwise
     */
    bool loadFromArgs(int argc, char** argv) {
        if (argc < 1 || argv == nullptr) {
            return false;
        }
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Skip help and other non-config arguments
            if (arg == "--help" || arg == "-h" || arg == "--demo" || 
                arg == "--integration" || arg == "--test" || arg == "--info" ||
                arg == "--version") {
                continue;
            }
            
            // Parse --key=value or --key value
            if (arg.find("=") != std::string::npos) {
                size_t pos = arg.find("=");
                std::string key = arg.substr(2, pos - 2); // Skip --
                std::string valueStr = arg.substr(pos + 1);
                
                // Convert value based on content
                if (valueStr == "true" || valueStr == "false") {
                    bool value = (valueStr == "true");
                    set<bool>(key, value, ConfigSource::CommandLine);
                } else {
                    try {
                        size_t pos;
                        int64_t intVal = std::stoll(valueStr, &pos);
                        if (pos == valueStr.length()) {
                            set<int64_t>(key, intVal, ConfigSource::CommandLine);
                            continue;
                        }
                    } catch (...) {
                        // Try as float
                        try {
                            size_t pos;
                            float floatVal = std::stof(valueStr, &pos);
                            if (pos == valueStr.length()) {
                                set<float>(key, floatVal, ConfigSource::CommandLine);
                                continue;
                            }
                        } catch (...) {
                            // Try as string
                            set<std::string>(key, valueStr, ConfigSource::CommandLine);
                        }
                    }
                }
            } else if (arg.substr(0, 2) == "--") {
                // Look ahead for value
                std::string key = arg.substr(2);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    std::string valueStr = argv[++i];
                    
                    if (valueStr == "true" || valueStr == "false") {
                        bool value = (valueStr == "true");
                        set<bool>(key, value, ConfigSource::CommandLine);
                    } else {
                        try {
                            size_t pos;
                            int64_t intVal = std::stoll(valueStr, &pos);
                            if (pos == valueStr.length()) {
                                set<int64_t>(key, intVal, ConfigSource::CommandLine);
                                continue;
                            }
                        } catch (...) {
                            // Try as float
                            try {
                                size_t pos;
                                float floatVal = std::stof(valueStr, &pos);
                                if (pos == valueStr.length()) {
                                    set<float>(key, floatVal, ConfigSource::CommandLine);
                                    continue;
                                }
                            } catch (...) {
                                // Try as string
                                set<std::string>(key, valueStr, ConfigSource::CommandLine);
                            }
                        }
                    }
                }
            }
        }
        
        return true;
    }
    
    /**
     * Generate a sample configuration file
     * 
     * @param filename Path to save sample configuration
     * @return true if successful, false otherwise
     */
    bool generateSampleConfig(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# NLM Sample Configuration File" << std::endl;
        file << "# This file contains example configuration values for NLM" << std::endl;
        file << "# See HOW_TO_USE.md for documentation on each parameter" << std::endl;
        file << std::endl;
        file << "# Brain Configuration" << std::endl;
        file << "brain.neuron_count = 1000" << std::endl;
        file << "brain.region_count = 1" << std::endl;
        file << "brain.connection_probability = 0.1" << std::endl;
        file << "brain.initial_weight_mean = 0.5" << std::endl;
        file << "brain.initial_weight_std = 0.1" << std::endl;
        file << "brain.v_thresh = -50.0" << std::endl;
        file << "brain.v_rest = -70.0" << std::endl;
        file << "brain.v_reset = -75.0" << std::endl;
        file << "brain.tau_mem = 20.0" << std::endl;
        file << "brain.tau_ref = 2.0" << std::endl;
        file << std::endl;
        file << "# Plasticity Configuration" << std::endl;
        file << "plasticity.stdp.enable = true" << std::endl;
        file << "plasticity.stdp.learning_rate = 0.001" << std::endl;
        file << "plasticity.stdp.tau_plus = 20.0" << std::endl;
        file << "plasticity.stdp.tau_minus = 20.0" << std::endl;
        file << "plasticity.hebbian.enable = true" << std::endl;
        file << "plasticity.structural.enable = true" << std::endl;
        file << std::endl;
        file << "# Neuromodulation Configuration" << std::endl;
        file << "neuromod.dopamine.scale = 1.0" << std::endl;
        file << "neuromod.curiosity.enable = true" << std::endl;
        file << "neuromod.novelty.enable = true" << std::endl;
        file << std::endl;
        file << "# Simulation Configuration" << std::endl;
        file << "simulation.timestep = 0.001" << std::endl;
        file << "simulation.max_steps = 10000" << std::endl;
        file << std::endl;
        
        file.close();
        return true;
    }
    
    /**
     * Print configuration summary
     */
    void printSummary() const {
        std::cout << "Configuration Summary:" << std::endl;
        std::cout << "====================" << std::endl;
        
        if (configKeys.empty()) {
            std::cout << "No configuration values set." << std::endl;
            return;
        }
        
        for (const auto& key : configKeys) {
            auto it = configMap.find(key);
            if (it == configMap.end()) continue;
            
            const auto& entry = it->second;
            std::string valueStr;
            
            if (std::holds_alternative<StringConfigEntry>(entry)) {
                valueStr = std::get<StringConfigEntry>(entry).value;
            } else if (std::holds_alternative<BoolConfigEntry>(entry)) {
                valueStr = std::get<BoolConfigEntry>(entry).value ? "true" : "false";
            } else if (std::holds_alternative<IntConfigEntry>(entry)) {
                valueStr = std::to_string(std::get<IntConfigEntry>(entry).value);
            } else if (std::holds_alternative<FloatConfigEntry>(entry)) {
                valueStr = std::to_string(std::get<FloatConfigEntry>(entry).value);
            }
            
            std::string sourceStr;
            switch (getSource(key)) {
                case ConfigSource::File: sourceStr = "File"; break;
                case ConfigSource::CommandLine: sourceStr = "Command Line"; break;
                case ConfigSource::Runtime: sourceStr = "Runtime"; break;
                case ConfigSource::Default: sourceStr = "Default"; break;
            }
            
            std::cout << "  " << key << " = " << valueStr 
                     << " (source: " << sourceStr << ")" << std::endl;
        }
    }
};

/**
 * Load a configuration from file and return it
 * 
 * @param filename Path to configuration file
 * @return Shared pointer to configuration object
 */
inline std::shared_ptr<Config> createConfigFromFile(const std::string& filename) {
    auto config = std::make_shared<Config>();
    config->loadFromFile(filename);
    return config;
}

/**
 * Create a default configuration with standard NLM parameters
 * 
 * @return Shared pointer to default configuration object
 */
inline std::shared_ptr<Config> createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Brain configuration
    config->set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("brain.region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("brain.connection_probability", 0.1f, ConfigSource::Default);
    config->set("brain.initial_weight_mean", 0.5f, ConfigSource::Default);
    config->set("brain.initial_weight_std", 0.1f, ConfigSource::Default);
    config->set("brain.v_thresh", -50.0f, ConfigSource::Default);
    config->set("brain.v_rest", -70.0f, ConfigSource::Default);
    config->set("brain.v_reset", -75.0f, ConfigSource::Default);
    config->set("brain.tau_mem", 20.0f, ConfigSource::Default);
    config->set("brain.tau_ref", 2.0f, ConfigSource::Default);
    
    // Plasticity configuration
    config->set("plasticity.stdp.enable", true, ConfigSource::Default);
    config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
    config->set("plasticity.stdp.tau_plus", 20.0f, ConfigSource::Default);
    config->set("plasticity.stdp.tau_minus", 20.0f, ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
    config->set("plasticity.structural.enable", true, ConfigSource::Default);
    
    // Neuromodulation configuration
    config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
    config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
    config->set("neuromod.novelty.enable", true, ConfigSource::Default);
    
    // Simulation configuration
    config->set("simulation.timestep", 0.001f, ConfigSource::Default);
    config->set("simulation.max_steps", 10000LL, ConfigSource::Default);
    
    return config;
}

/**
 * Load configuration from a file with error handling
 * 
 * @param filename Path to configuration file
 * @return Shared pointer to configuration object
 * @throws ConfigException if file cannot be loaded
 */
inline std::shared_ptr<Config> loadConfigFile(const std::string& filename) {
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(filename)) {
        throw ConfigException("Could not load configuration file: " + filename);
    }
    return config;
}

/**
 * Generate a sample configuration file
 * 
 * @param filename Path to save sample configuration
 * @return true if successful, false otherwise
 */
inline bool generateSampleConfigFile(const std::string& filename) {
    auto config = std::make_shared<Config>();
    return config->generateSampleConfig(filename);
}

} // namespace nlm

// Convenience typedefs for backward compatibility
typedef nlm::Config NLMConfig;
typedef std::shared_ptr<nlm::Config> ConfigPtr;

// Global configuration helper functions
inline ConfigPtr createDefaultConfig() {
    return nlm::createDefaultConfig();
}

inline ConfigPtr createConfigFromFile(const std::string& filename) {
    return nlm::createConfigFromFile(filename);
}

inline bool generateSampleConfigFile(const std::string& filename) {
    return nlm::generateSampleConfigFile(filename);
}
