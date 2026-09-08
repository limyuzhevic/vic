#pragma once

#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <sstream>

namespace nlm {

// Forward declarations
class Logger;
class Config;

// Simple configuration entry for basic key=value format
struct SimpleConfigEntry {
    std::string key;
    std::string value;
    std::string description;
    ConfigSource source;
    
    SimpleConfigEntry() : key(), value(), description(), source(ConfigSource::Default) {}
    SimpleConfigEntry(const std::string& k, const std::string& v, const std::string& d = "", ConfigSource s = ConfigSource::Default)
        : key(k), value(v), description(d), source(s) {}
};

class SimpleConfig {
public:
    SimpleConfig() = default;
    
    // Load from simple key=value file
    bool loadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        int lineNum = 0;
        while (std::getline(file, line)) {
            lineNum++;
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                         (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            } else {
                NLM_LOG_WARN("Invalid config line " + std::to_string(lineNum) + ": " + line);
            }
        }
        
        return true;
    }
    
    // Get value as string
    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        auto it = entries.find(key);
        if (it == entries.end()) {
            return defaultValue;
        }
        return it->second.value;
    }
    
    // Get numeric values with conversion
    int getInt(const std::string& key, int defaultValue = 0) const {
        std::string value = getString(key, "");
        if (value.empty()) return defaultValue;
        try {
            return std::stoi(value);
        } catch (...) {
            return defaultValue;
        }
    }
    
    double getDouble(const std::string& key, double defaultValue = 0.0) const {
        std::string value = getString(key, "");
        if (value.empty()) return defaultValue;
        try {
            return std::stod(value);
        } catch (...) {
            return defaultValue;
        }
    }
    
    bool getBool(const std::string& key, bool defaultValue = false) const {
        std::string value = getString(key, "");
        if (value.empty()) return defaultValue;
        
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        return value == "true" || value == "1" || value == "yes";
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return entries.find(key) != entries.end();
    }
    
private:
    std::unordered_map<std::string, SimpleConfigEntry> entries;
    
    void set(const std::string& key, const std::string& value, ConfigSource source) {
        entries[key] = SimpleConfigEntry(key, value, "Config parameter: " + key, source);
    }
    
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
};

class SimulationClock {
public:
    SimulationClock(double timestep) : timestep_(timestep), currentStep_(0), currentTime_(0.0) {}
    
    void advance() {
        currentStep_++;
        currentTime_ += timestep_;
    }
    
    SimulationStep getCurrentStep() const { return currentStep_; }
    Timestamp getCurrentTime() const { return currentTime_; }
    double getTimestep() const { return timestep_; }
    
private:
    double timestep_;
    SimulationStep currentStep_;
    Timestamp currentTime_;
};

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void runBasicTest(SimpleConfig& config) {
    NLM_LOG_INFO("=== Basic Test ===");
    
    int neuronCount = config.getInt("neuron_count", 100);
    int regionCount = config.getInt("region_count", 1);
    float connectionProb = config.getDouble("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Config loaded: neuron_count=" + std::to_string(neuronCount) +
                 ", region_count=" + std::to_string(regionCount) +
                 ", connection_probability=" + std::to_string(connectionProb));
    
    if (neuronCount > 0 && regionCount > 0) {
        NLM_LOG_INFO("✓ Configuration is valid and usable");
    } else {
        NLM_LOG_WARN("Configuration may be invalid");
    }
}

void runConfigTests(SimpleConfig& config) {
    NLM_LOG_INFO("=== Configuration Test Suite ===");
    
    // Test string values
    NLM_LOG_INFO("Testing string values:");
    NLM_LOG_INFO("  environment_name = " + config.getString("environment_name", "NOT_FOUND"));
    
    // Test numeric values
    NLM_LOG_INFO("Testing numeric values:");
    NLM_LOG_INFO("  neuron_count = " + std::to_string(config.getInt("neuron_count")));
    NLM_LOG_INFO("  simulation_timestep = " + std::to_string(config.getDouble("simulation_timestep")));
    NLM_LOG_INFO("  connection_probability = " + std::to_string(config.getDouble("connection_probability")));
    
    // Test boolean values
    NLM_LOG_INFO("Testing boolean values:");
    NLM_LOG_INFO("  log_to_file = " + std::to_string(config.getBool("log_to_file")));
    
    // Test duplicates handling
    if (config.has("dopamine_baseline")) {
        NLM_LOG_INFO("  Note: 'dopamine_baseline' key found (may have duplicate in file)");
    }
}

void demonstrateConfigIssues(SimpleConfig& config) {
    NLM_LOG_INFO("=== Demonstrating Configuration Issues ===");
    
    // This shows what happens when you try to load a config with issues
    NLM_LOG_INFO("Issues in current config/default.cfg:");
    NLM_LOG_INFO("  1. Duplicate 'dopamine_baseline' keys (lines 30-31)");
    NLM_LOG_INFO("  2. Inconsistent formatting (mixed spacing, no type consistency)");
    NLM_LOG_INFO("  3. Placeholder values in Config::saveToFile (actual values lost)");
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Testing NLM Phase 2 Configuration System..." << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Load configuration
    SimpleConfig config;
    std::string configFile = "configs/default.cfg";
    
    if (config.loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_WARN("Failed to load configuration from: " + configFile);
        NLM_LOG_INFO("Using empty configuration");
    }
    
    // Run tests
    runBasicTest(config);
    runConfigTests(config);
    demonstrateConfigIssues(config);
    
    // Show configuration summary
    NLM_LOG_INFO("=== Configuration Summary ===");
    for (const auto& pair : config.getAllEntries()) {
        NLM_LOG_INFO(pair.first + " = " + pair.second.value);
    }
    
    NLM_LOG_INFO("=== All tests completed ===");
    
    return 0;
}

} // namespace nlm