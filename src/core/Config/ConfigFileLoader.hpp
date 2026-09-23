// ConfigFileLoader - JSON configuration file loader
// Provides robust JSON configuration file support for NLM experiments

#pragma once

#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>

namespace nlm {

class ConfigFileLoader {
public:
    // Load configuration from JSON file
    static bool loadFromJSON(const std::string& filepath, Config& config);
    
    // Save configuration to JSON file
    static bool saveToJSON(const std::string& filepath, const Config& config);
    
    // Load configuration from simple key=value file format
    static bool loadFromSimpleFile(const std::string& filepath, Config& config);
    
    // Validate configuration file format
    static bool validateFile(const std::string& filepath);
    
    // Get list of supported configuration formats
    static std::vector<std::string> getSupportedFormats() {
        return {".json", ".cfg", ".ini", ".txt"};
    }
    
    // Parse JSON-like configuration string
    static bool parseJSONConfig(const std::string& configStr, Config& config);
    
    // Get default configuration template
    static std::string getDefaultConfigTemplate() {
        return R"(
# NLM Phase 6 Configuration File
# ================================
# This file contains configuration settings for NLM Phase 6 integration

# Neuron and network settings
neuron_count = 1000           # Total number of neurons in the network
region_count = 1              # Number of neural regions
connection_probability = 0.1  # Probability of synaptic connections

# Simulation settings
simulation_timestep = 0.001   # Simulation time step in seconds
max_steps = 10000             # Maximum number of simulation steps

# Plasticity settings
stdp_ltp_weight = 0.01        # STDP long-term potentiation weight
stdp_ltd_weight = 0.012       # STDP long-term depression weight
stdp_tau = 20.0               # STDP time constant (ms)
synaptogenesis_rate = 0.0001  # Structural plasticity synaptogenesis rate
pruning_rate = 0.00001        # Structural plasticity pruning rate

# Development settings
development_enabled = true    # Enable developmental processes
plasticity_modifier = 1.0     # Modifier for plasticity rates

# Experiment settings
enable_checkpointing = true   # Enable checkpoint save/load
enable_replay = true          # Enable episodic memory replay
enable_development = true     # Enable developmental processes

# World settings
world_width = 16              # World environment width
world_height = 16             # World environment height

# Agent settings
agent_max_steps = 2000        # Agent maximum steps
agent_reward_decay = 0.99      # Reward decay factor

# Logging settings
log_level = "info"            # Logging level: debug, info, warning, error
log_file = "nlm.log"          # Log file path
""";
    }
    
    // Create sample configuration file
    static bool createSampleConfig(const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << getDefaultConfigTemplate();
        file.close();
        return true;
    }
    
private:
    // Helper functions
    static std::string trim(const std::string& str);
    static bool isComment(const std::string& line);
    static bool hasValidKeyValuePair(const std::string& line);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

