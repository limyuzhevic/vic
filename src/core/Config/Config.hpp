#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with validation warnings
    std::string summary() const {
        std::string summary = "NLM Configuration:\n";
        
        summary += "Loaded " + std::to_string(pImpl->entries.size()) + " configuration entries:\n";
        
        bool hasValidationWarnings = false;
        for (const auto& entry : pImpl->entries) {
            std::string key = entry.key;
            std::string valueStr = std::visit([](const auto& val) -> std::string {
                if constexpr (std::is_same_v<decltype(val), int>) return std::to_string(val);
                else if constexpr (std::is_same_v<decltype(val), int64_t>) return std::to_string(val);
                else if constexpr (std::is_same_v<decltype(val), double>) return std::to_string(val);
                else if constexpr (std::is_same_v<decltype(val), bool>) return val ? "true" : "false";
                else return val; // string or vector
            }, entry.value);
            
            summary += "  " + key + " = " + valueStr + "\n";
            
            // Add validation warnings for potentially problematic values
            if (validateParameter(key, entry.value)) {
                summary += "    [WARNING] Parameter value may need adjustment for optimal performance\n";
                hasValidationWarnings = true;
            }
        }
        
        if (hasValidationWarnings) {
            summary += "\n[VALIDATION WARNINGS]\n";
            summary += "  - Consider increasing neuron_count for complex tasks\n";
            summary += "  - Adjust connection_probability based on network complexity\n";
            summary += "  - For long simulations, consider enabling checkpointing\n";
            summary += "  - Set appropriate simulation_timestep for desired resolution\n";
        }
        
        return summary;
    }
    
    // Validate parameter value and return true if there are potential issues
    bool validateParameter(const std::string& key, const ConfigValue& value) const {
        // Check for obviously problematic values
        if (key == "neuron_count") {
            if (std::holds_alternative<int>(value)) {
                int neurons = std::get<int>(value);
                if (neurons < 100) {
                    return true; // Warning: Too few neurons for meaningful behavior
                } else if (neurons > 10000) {
                    return true; // Warning: May be computationally expensive
                }
            }
        }
        
        if (key == "region_count") {
            if (std::holds_alternative<int>(value)) {
                int regions = std::get<int>(value);
                if (regions < 1) {
                    return true; // Warning: Need at least one region
                } else if (regions > 10) {
                    return true; // Warning: Many regions may affect performance
                }
            }
        }
        
        if (key == "connection_probability") {
            if (std::holds_alternative<double>(value)) {
                double prob = std::get<double>(value);
                if (prob < 0.01) {
                    return true; // Warning: Very sparse connectivity
                } else if (prob > 0.5) {
                    return true; // Warning: Dense connectivity may cause instability
                }
            }
        }
        
        if (key == "simulation_timestep") {
            if (std::holds_alternative<double>(value)) {
                double timestep = std::get<double>(value);
                if (timestep > 0.1) {
                    return true; // Warning: Large timestep may affect accuracy
                } else if (timestep < 0.0001) {
                    return true; // Warning: Very small timestep may affect performance
                }
            }
        }
        
        if (key == "max_simulation_steps") {
            if (std::holds_alternative<int>(value)) {
                int steps = std::get<int>(value);
                if (steps > 100000) {
                    return true; // Warning: Very long simulations
                } else if (steps < 1000) {
                    return true; // Warning: May not be enough steps for learning
                }
            }
        }
        
        return false; // No validation warnings
    }
    
    // Get configuration validation report
    std::string getValidationReport() const {
        std::string report = "NLM Configuration Validation Report:\n\n";
        
        report += "Validation Status: ";
        if (validateConfiguration()) {
            report += "PASSED\n\n";
            report += "All configuration parameters are within acceptable ranges.\n";
        } else {
            report += "ISSUES DETECTED\n\n";
            report += "The following configuration issues were found:\n";
            
            // Check for specific issues
            for (const auto& entry : pImpl->entries) {
                if (validateParameter(entry.key, entry.value)) {
                    report += "  - ";
                    
                    if (entry.key == "neuron_count") {
                        report += "neuron_count may be too low for meaningful behavior or too high for computational efficiency\n";
                    } else if (entry.key == "region_count") {
                        report += "region_count may affect performance or stability\n";
                    } else if (entry.key == "connection_probability") {
                        report += "connection_probability may affect network stability or efficiency\n";
                    } else if (entry.key == "simulation_timestep") {
                        report += "simulation_timestep may affect accuracy or performance\n";
                    } else if (entry.key == "max_simulation_steps") {
                        report += "max_simulation_steps may not provide enough time for learning\n";
                    } else {
                        report += "Parameter " + entry.key + " may need adjustment\n";
                    }
                }
            }
        }
        
        report += "\nRecommendations:\n";
        report += "  - For complex tasks: neuron_count >= 1000, region_count = 2-4\n";
        report += "  - For realistic simulations: connection_probability = 0.05-0.2\n";
        report += "  - For stable learning: simulation_timestep = 0.001-0.01\n";
        report += "  - For long-term learning: max_simulation_steps = 10000-50000\n";
        report += "  - Enable checkpointing for simulations > 10000 steps\n";
        
        return report;
    }
    
    // Validate entire configuration
    bool validateConfiguration() const {
        bool valid = true;
        
        // Check required parameters
        if (!has("neuron_count")) {
            valid = false;
        }
        
        if (!has("region_count")) {
            valid = false;
        }
        
        if (!has("connection_probability")) {
            valid = false;
        }
        
        if (!has("simulation_timestep")) {
            valid = false;
        }
        
        // Check parameter ranges
        auto neuronCount = get<int>("neuron_count");
        if (neuronCount && (*neuronCount < 100 || *neuronCount > 10000)) {
            valid = false;
        }
        
        auto regionCount = get<int>("region_count");
        if (regionCount && (*regionCount < 1 || *regionCount > 10)) {
            valid = false;
        }
        
        auto connProb = get<double>("connection_probability");
        if (connProb && (*connProb < 0.01 || *connProb > 0.5)) {
            valid = false;
        }
        
        auto timestep = get<double>("simulation_timestep");
        if (timestep && (*timestep < 0.0001 || *timestep > 0.1)) {
            valid = false;
        }
        
        auto maxSteps = get<int>("max_simulation_steps");
        if (maxSteps && *maxSteps < 1000) {
            valid = false;
        }
        
        return valid;
    }
    
    // Get configuration for specific use case
    std::string getUseCaseConfig(const std::string& useCase) const {
        std::string config = "# NLM Configuration for " + useCase + "\n";
        
        if (useCase == "quick_test") {
            config += "neuron_count = 100\n";
            config += "region_count = 1\n";
            config += "connection_probability = 0.1\n";
            config += "simulation_timestep = 0.001\n";
            config += "max_simulation_steps = 1000\n";
        } else if (useCase == "research") {
            config += "neuron_count = 5000\n";
            config += "region_count = 3\n";
            config += "connection_probability = 0.05\n";
            config += "simulation_timestep = 0.001\n";
            config += "max_simulation_steps = 50000\n";
            config += "enable_checkpointing = true\n";
            config += "checkpoint_path = ./checkpoints\n";
        } else if (useCase == "production") {
            config += "neuron_count = 2000\n";
            config += "region_count = 2\n";
            config += "connection_probability = 0.1\n";
            config += "simulation_timestep = 0.001\n";
            config += "max_simulation_steps = 20000\n";
            config += "enable_checkpointing = true\n";
            config += "log_level = WARNING\n";
            config += "log_to_file = true\n";
        } else {
            // Default configuration
            config += "neuron_count = 1000\n";
            config += "region_count = 1\n";
            config += "connection_probability = 0.1\n";
            config += "simulation_timestep = 0.001\n";
            config += "max_simulation_steps = 10000\n";
        }
        
        config += "\n# Copy this configuration to a .cfg file and use loadFromFile()\n";
        config += "# Example: config->loadFromFile(\"my_config.cfg\");\n";
        
        return config;
    }
    
    // Apply use case configuration
    void applyUseCaseConfig(const std::string& useCase) {
        clear(); // Clear existing configuration
        
        std::string config = getUseCaseConfig(useCase);
        
        // Parse the generated configuration
        size_t pos;
        while ((pos = config.find(" = ")) != std::string::npos) {
            std::string key = config.substr(0, pos);
            config = config.substr(pos + 3);
            
            pos = config.find("\n");
            if (pos != std::string::npos) {
                std::string value = config.substr(0, pos);
                config = config.substr(pos + 1);
                
                // Remove whitespace
                key = trim(key);
                value = trim(value);
                
                // Parse value based on type
                if (value.find('.') != std::string::npos) {
                    try {
                        double doubleValue = std::stod(value);
                        set(key, doubleValue);
                    } catch (...) {
                        set(key, value);
                    }
                } else if (value == "true" || value == "false") {
                    bool boolValue = (value == "true");
                    set(key, boolValue);
                } else {
                    try {
                        int intValue = std::stoi(value);
                        set(key, intValue);
                    } catch (...) {
                        set(key, value);
                    }
                }
            }
        }
    }
    
    // Get configuration health score (0-100)
    int getHealthScore() const {
        int score = 100;
        
        // Deduct points for missing parameters
        if (!has("neuron_count")) score -= 20;
        if (!has("region_count")) score -= 20;
        if (!has("connection_probability")) score -= 20;
        if (!has("simulation_timestep")) score -= 10;
        if (!has("max_simulation_steps")) score -= 10;
        
        // Deduct points for parameter ranges
        auto neuronCount = get<int>("neuron_count");
        if (neuronCount) {
            if (*neuronCount < 500) score -= 15;
            else if (*neuronCount > 5000) score -= 10;
        }
        
        auto regionCount = get<int>("region_count");
        if (regionCount) {
            if (*regionCount < 2) score -= 10;
            else if (*regionCount > 4) score -= 5;
        }
        
        auto connProb = get<double>("connection_probability");
        if (connProb) {
            if (*connProb < 0.05) score -= 10;
            else if (*connProb > 0.2) score -= 5;
        }
        
        auto timestep = get<double>("simulation_timestep");
        if (timestep) {
            if (*timestep < 0.001) score -= 10;
            else if (*timestep > 0.01) score -= 5;
        }
        
        // Add points for good practices
        if (has("enable_checkpointing") && get<bool>("enable_checkpointing")) {
            score += 10;
        }
        
        if (has("log_level") && get<std::string>("log_level") == "INFO") {
            score += 5;
        }
        
        return std::max(0, std::min(100, score));
    }
    
    // Validate parameter value and return true if there are potential issues
    bool validateParameter(const std::string& key, const ConfigValue& value) const;
    
    // Get configuration validation report
    std::string getValidationReport() const;
    
    // Validate entire configuration
    bool validateConfiguration() const;
    
    // Get configuration for specific use case
    std::string getUseCaseConfig(const std::string& useCase) const;
    
    // Apply use case configuration
    void applyUseCaseConfig(const std::string& useCase);
    
    // Get configuration health score (0-100)
    int getHealthScore() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
