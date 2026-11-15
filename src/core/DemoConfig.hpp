#pragma once

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <memory>

namespace nlm {

class DemoConfig {
public:
    struct Parameter {
        std::string name;
        std::string type;
        std::string value;
        std::string description;
        bool hasDefault = false;
    };
    
    DemoConfig() = default;
    
    // Load from configuration file
    bool loadFromFile(const std::string& filename);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char* argv[]);
    
    // Set parameter value
    template<typename T>
    void set(const std::string& name, const T& value);
    
    // Get parameter value
    template<typename T>
    T get(const std::string& name, const T& defaultValue = T()) const;
    
    // Get all parameter names
    std::vector<std::string> getParameterNames() const;
    
    // Get parameter info
    const Parameter* getParameterInfo(const std::string& name) const;
    
    // Print configuration
    void printConfig(std::ostream& out = std::cout) const;
    
    // Validate configuration
    bool validate() const;

private:
    std::map<std::string, Parameter> parameters;
};

// Demo configuration presets
struct DemoPresets {
    static DemoConfig getConnectivityDemo() {
        DemoConfig config;
        config.set("steps", 100);
        config.set("neurons", 500);
        config.set("regions", 1);
        config.set("connection_prob", 0.15f);
        config.set("plasticity_enabled", false);
        config.set("verbose", true);
        return config;
    }
    
    static DemoConfig getPlasticityDemo() {
        DemoConfig config;
        config.set("steps", 1000);
        config.set("neurons", 500);
        config.set("regions", 1);
        config.set("connection_prob", 0.15f);
        config.set("plasticity_enabled", true);
        config.set("learning_rate", 0.01f);
        config.set("verbose", true);
        return config;
    }
    
    static DemoConfig getStdpDemo() {
        DemoConfig config;
        config.set("steps", 500);
        config.set("neurons", 500);
        config.set("regions", 1);
        config.set("connection_prob", 0.15f);
        config.set("plasticity_enabled", true);
        config.set("stdp_enabled", true);
        config.set("hebbian_enabled", false);
        config.set("verbose", true);
        return config;
    }
    
    static DemoConfig getLearningDemo() {
        DemoConfig config;
        config.set("steps", 2000);
        config.set("neurons", 500);
        config.set("regions", 1);
        config.set("connection_prob", 0.15f);
        config.set("plasticity_enabled", true);
        config.set("learning_rate", 0.01f);
        config.set("verbose", true);
        return config;
    }
};

// Demo mode enumeration
enum class DemoMode {
    CONNECTIVITY,
    PLASTICITY,
    STDP,
    LEARNING,
    CUSTOM
};

DemoMode demoModeFromString(const std::string& modeStr);
std::string demoModeToString(DemoMode mode);

// Helper function to get demo configuration based on mode
DemoConfig getDemoConfig(DemoMode mode);

// Legacy compatibility functions
void setupBrainFromConfig(std::shared_ptr<Config> config, const DemoConfig& demoConfig);
void runDemoWithConfig(const DemoConfig& config);

} // namespace nlm

// Implementation for DemoConfig
#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

bool DemoConfig::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse "key = value" format
        size_t equalsPos = line.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = line.substr(0, equalsPos);
            std::string value = line.substr(equalsPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            set(key, value);
        }
    }
    
    return true;
}

template<typename T>
void DemoConfig::set(const std::string& name, const T& value) {
    Parameter param;
    param.name = name;
    param.value = std::to_string(value);
    parameters[name] = param;
}

template<typename T>
T DemoConfig::get(const std::string& name, const T& defaultValue) const {
    auto it = parameters.find(name);
    if (it != parameters.end()) {
        try {
            if constexpr (std::is_same_v<T, std::string>) {
                return it->second.value;
            } else if constexpr (std::is_same_v<T, int>) {
                return std::stoi(it->second.value);
            } else if constexpr (std::is_same_v<T, float>) {
                return std::stof(it->second.value);
            } else if constexpr (std::is_same_v<T, bool>) {
                std::string val = it->second.value;
                std::transform(val.begin(), val.end(), val.begin(), ::tolower);
                return val == "true" || val == "1" || val == "yes" || val == "on";
            }
        } catch (...) {
            // If parsing fails, return default
        }
    }
    return defaultValue;
}

std::vector<std::string> DemoConfig::getParameterNames() const {
    std::vector<std::string> names;
    for (const auto& pair : parameters) {
        names.push_back(pair.first);
    }
    return names;
}

const DemoConfig::Parameter* DemoConfig::getParameterInfo(const std::string& name) const {
    auto it = parameters.find(name);
    return (it != parameters.end()) ? &it->second : nullptr;
}

void DemoConfig::printConfig(std::ostream& out) const {
    out << "=== NLM Demo Configuration ===" << std::endl;
    for (const auto& pair : parameters) {
        out << pair.first << " = " << pair.second.value << std::endl;
    }
    out << "=============================" << std::endl;
}

bool DemoConfig::validate() const {
    // Basic validation - check for required parameters
    if (parameters.empty()) {
        std::cerr << "Warning: Configuration has no parameters" << std::endl;
    }
    return true;
}

DemoMode demoModeFromString(const std::string& modeStr) {
    std::string mode = modeStr;
    std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
    
    if (mode == "connectivity" || mode == "connect") {
        return DemoMode::CONNECTIVITY;
    } else if (mode == "plasticity") {
        return DemoMode::PLASTICITY;
    } else if (mode == "stdp") {
        return DemoMode::STDP;
    } else if (mode == "learning") {
        return DemoMode::LEARNING;
    } else {
        return DemoMode::CUSTOM;
    }
}

std::string demoModeToString(DemoMode mode) {
    switch (mode) {
        case DemoMode::CONNECTIVITY:
            return "connectivity";
        case DemoMode::PLASTICITY:
            return "plasticity";
        case DemoMode::STDP:
            return "stdp";
        case DemoMode::LEARNING:
            return "learning";
        case DemoMode::CUSTOM:
            return "custom";
        default:
            return "unknown";
    }
}

DemoConfig getDemoConfig(DemoMode mode) {
    switch (mode) {
        case DemoMode::CONNECTIVITY:
            return DemoPresets::getConnectivityDemo();
        case DemoMode::PLASTICITY:
            return DemoPresets::getPlasticityDemo();
        case DemoMode::STDP:
            return DemoPresets::getStdpDemo();
        case DemoMode::LEARNING:
            return DemoPresets::getLearningDemo();
        case DemoMode::CUSTOM:
            return DemoConfig();
        default:
            return DemoPresets::getConnectivityDemo();
    }
}

} // namespace nlm
