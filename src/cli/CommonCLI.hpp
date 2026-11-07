// Common CLI utility for NLM executables
// Provides standardized argument parsing, configuration loading, and help documentation

#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Command line interface utilities
class CommonCLI {
public:
    // Parse command line arguments
    // Supports both --key=value and --key value formats
    // Stores parsed key-value pairs in config parameter
    static bool parseArguments(int argc, char* argv[], std::shared_ptr<Config> config);
    
    // Load configuration from file
    static bool loadConfigFile(const std::string& filename, std::shared_ptr<Config> config);
    
    // Print usage information
    static void printUsage(const std::string& programName);
    
    // Print version information
    static void printVersion();
    
    // Print configuration parameters for help
    static void printConfigHelp();
    
    // Get configuration parameter value as string
    static std::string getStringArg(const std::unordered_map<std::string, std::string>& args, const std::string& key, const std::string& defaultValue = "");
    
    // Get configuration parameter value as integer
    static int getIntArg(const std::unordered_map<std::string, std::string>& args, const std::string& key, int defaultValue = 0);
    
    // Get configuration parameter value as float
    static float getFloatArg(const std::unordered_map<std::string, std::string>& args, const std::string& key, float defaultValue = 0.0f);
    
    // Get configuration parameter value as boolean
    static bool getBoolArg(const std::unordered_map<std::string, std::string>& args, const std::string& key, bool defaultValue = false);
    
    // Check if argument exists
    static bool hasArg(const std::unordered_map<std::string, std::string>& args, const std::string& key);
    
    // Apply parsed arguments to configuration
    static void applyArgumentsToConfig(const std::unordered_map<std::string, std::string>& args, std::shared_ptr<Config> config);
    
    // Merge configuration files
    static void mergeConfigFiles(const std::vector<std::string>& configFiles, std::shared_ptr<Config> config);
};

// Predefined configuration parameter definitions
struct ConfigParameter {
    std::string key;
    std::string type; // "string", "int", "float", "bool"
    std::string defaultValue;
    std::string help;
    std::vector<std::string> validValues;
};

// Brain configuration parameters
const std::vector<ConfigParameter> brainConfigParameters = {
    {"neuron_count", "int", "1000", "Total number of neurons", ""},
    {"region_count", "int", "2", "Number of neural regions", ""},
    {"connection_probability", "float", "0.1", "Probability of connection between neurons (0.0-1.0)", ""},
    {"random_seed", "int", "42", "Random seed for reproducible experiments", ""},
    {"simulation_timestep", "float", "0.001", "Simulation timestep in seconds", ""},
    {"v_thresh", "float", "-50.0", "Neuron threshold potential (mV)", ""},
    {"v_rest", "float", "-70.0", "Resting potential (mV)", ""},
    {"v_reset", "float", "-75.0", "Reset potential after spike (mV)", ""},
    {"plasticity.stdp.enable", "bool", "true", "Enable Spike-Timing-Dependent Plasticity", ""},
    {"plasticity.stdp.learning_rate", "float", "0.01", "STDP learning rate", ""},
    {"plasticity.hebbian.enable", "bool", "true", "Enable Hebbian plasticity", ""},
    {"neuromod.dopamine.scale", "float", "1.0", "Dopamine modulation scale", ""},
    {"neuromod.curiosity.enable", "bool", "true", "Enable curiosity-driven exploration", ""},
    {"neuromod.novelty.enable", "bool", "true", "Enable novelty detection", ""},
};

// Experiment configuration parameters
const std::vector<ConfigParameter> experimentConfigParameters = {
    {"num_episodes", "int", "5", "Number of training episodes", ""},
    {"steps_per_episode", "int", "200", "Number of steps per episode", ""},
    {"max_steps", "int", "1000", "Maximum steps for experiment", ""},
    {"enable_checkpointing", "bool", "false", "Enable checkpoint saving/loading", ""},
    {"enable_replay", "bool", "false", "Enable memory replay", ""},
    {"enable_development", "bool", "false", "Enable developmental processes", ""},
    {"enable_curiosity", "bool", "true", "Enable curiosity-driven exploration", ""},
};

// Debug/verbosity configuration parameters
const std::vector<ConfigParameter> debugConfigParameters = {
    {"verbose", "bool", "false", "Enable verbose logging", ""},
    {"log_level", "string", "info", "Log level (trace, debug, info, warn, error, fatal)", "trace;debug;info;warn;error;fatal"},
    {"quiet", "bool", "false", "Suppress non-essential output", ""},
};
