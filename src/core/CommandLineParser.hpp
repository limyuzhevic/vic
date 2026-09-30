#!/usr/bin/env python3
"""
Enhanced command-line argument parsing and argument handling for NLM.
Provides more sophisticated CLI options for users and developers.
"""

#include "core/Config/Config.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

namespace nlm {

class CommandLineParser {
public:
    struct Argument {
        std::string longName;
        char shortName;
        std::string description;
        std::string defaultValue;
        bool required;
        bool hasValue;
    };
    
    CommandLineParser() = default;
    
    // Add an argument definition
    void addArgument(const std::string& longName, char shortName, 
                     const std::string& description, const std::string& defaultValue = "",
                     bool required = false, bool hasValue = true) {
        Argument arg;
        arg.longName = longName;
        arg.shortName = shortName;
        arg.description = description;
        arg.defaultValue = defaultValue;
        arg.required = required;
        arg.hasValue = hasValue;
        
        arguments[arg.longName] = arg;
    }
    
    // Parse command line arguments
    bool parse(int argc, char** argv, std::map<std::string, std::string>& parsedArgs) {
        parsedArgs.clear();
        
        // Build help message
        std::cout << "Usage: nlm [OPTIONS]" << std::endl;
        std::cout << "Options:" << std::endl;
        
        for (const auto& pair : arguments) {
            const auto& arg = pair.second;
            std::string option = "  ";
            
            if (arg.shortName != '\0') {
                option += "-" + std::string(1, arg.shortName) + ", ";
            }
            option += "--" + arg.longName;
            
            if (arg.hasValue) {
                option += "=" + arg.longName.toUpper();
            }
            
            option += std::string(50 - option.length(), ' ');
            option += arg.description;
            
            if (!arg.defaultValue.empty()) {
                option += " (default: " + arg.defaultValue + ")";
            }
            
            if (arg.required) {
                option += " [REQUIRED]";
            }
            
            std::cout << option << std::endl;
        }
        
        // Parse arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Check for help flag
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return false;  // Exit after help
            }
            
            // Check for version flag
            if (arg == "--version" || arg == "-v") {
                printVersion();
                return false;
            }
            
            // Check for argument with value
            size_t eqPos = arg.find('=');
            if (eqPos != std::string::npos) {
                std::string name = arg.substr(0, eqPos);
                std::string value = arg.substr(eqPos + 1);
                
                if (arguments.find(name) != arguments.end()) {
                    parsedArgs[name] = value;
                } else {
                    std::cerr << "Error: Unknown option '--" << name << "'" << std::endl;
                    return false;
                }
            } else if (arguments.find(arg) != arguments.end()) {
                const auto& argDef = arguments[arg];
                
                if (argDef.hasValue) {
                    // Next argument should be the value
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        parsedArgs[arg] = argv[++i];
                    } else {
                        std::cerr << "Error: Option '--" << arg << "' requires a value" << std::endl;
                        return false;
                    }
                } else {
                    parsedArgs[arg] = "true";
                }
            } else {
                std::cerr << "Error: Unknown option '" << arg << "'" << std::endl;
                return false;
            }
        }
        
        // Check for required arguments
        for (const auto& pair : arguments) {
            const auto& arg = pair.second;
            if (arg.required && parsedArgs.find(arg.longName) == parsedArgs.end()) {
                std::cerr << "Error: Required option '--" << arg.longName << "' is missing" << std::endl;
                return false;
            }
        }
        
        return true;
    }
    
    // Get argument value with default
    std::string getValue(const std::map<std::string, std::string>& parsedArgs, 
                        const std::string& name, const std::string& defaultValue = "") const {
        auto it = parsedArgs.find(name);
        if (it != parsedArgs.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    // Get boolean argument value
    bool getBoolValue(const std::map<std::string, std::string>& parsedArgs, 
                     const std::string& name, bool defaultValue = false) const {
        auto it = parsedArgs.find(name);
        if (it != parsedArgs.end()) {
            std::string value = it->second;
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            return value == "true" || value == "1" || value == "yes" || value == "on";
        }
        return defaultValue;
    }
    
    // Get integer argument value
    int getIntValue(const std::map<std::string, std::string>& parsedArgs, 
                   const std::string& name, int defaultValue = 0) const {
        auto it = parsedArgs.find(name);
        if (it != parsedArgs.end()) {
            try {
                return std::stoi(it->second);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Invalid integer value for option '--" << name 
                         << "': " << e.what() << ". Using default value." << std::endl;
            }
        }
        return defaultValue;
    }
    
    // Get float argument value
    float getFloatValue(const std::map<std::string, std::string>& parsedArgs, 
                       const std::string& name, float defaultValue = 0.0f) const {
        auto it = parsedArgs.find(name);
        if (it != parsedArgs.end()) {
            try {
                return std::stof(it->second);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Invalid float value for option '--" << name 
                         << "': " << e.what() << ". Using default value." << std::endl;
            }
        }
        return defaultValue;
    }
    
    // Get double argument value
    double getDoubleValue(const std::map<std::string, std::string>& parsedArgs, 
                         const std::string& name, double defaultValue = 0.0) const {
        auto it = parsedArgs.find(name);
        if (it != parsedArgs.end()) {
            try {
                return std::stod(it->second);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Invalid double value for option '--" << name 
                         << "': " << e.what() << ". Using default value." << std::endl;
            }
        }
        return defaultValue;
    }
    
    // Print help information
    void printHelp() const {
        std::cout << "NLM Neural Learning Machine - Advanced Command Line Options" << std::endl;
        std::cout << "=================================================================" << std::endl;
        std::cout << std::endl;
        std::cout << "General Options:" << std::endl;
        std::cout << "  -h, --help                    Show this help message" << std::endl;
        std::cout << "  -v, --version                 Show version information" << std::endl;
        std::cout << "  --verbose                      Enable verbose logging" << std::endl;
        std::cout << "  --quiet                        Disable most logging" << std::endl;
        std::cout << std::endl;
        std::cout << "Simulation Control:" << std::endl;
        std::cout << "  --timestep=VALUE              Set simulation timestep (default: 0.001)" << std::endl;
        std::cout << "  --steps=VALUE                  Number of simulation steps to run" << std::endl;
        std::cout << "  --neuron-count=VALUE          Number of neurons in network" << std::endl;
        std::cout << "  --region-count=VALUE          Number of brain regions" << std::endl;
        std::cout << "  --seed=VALUE                  Random seed (default: 42)" << std::endl;
        std::cout << std::endl;
        std::cout << "Learning and Plasticity:" << std::endl;
        std::cout << "  --enable-hebbian              Enable Hebbian plasticity" << std::endl;
        std::cout << "  --enable-stdp                 Enable Spike-Timing-Dependent Plasticity" << std::endl;
        std::cout << "  --enable-reward               Enable reward-modulated plasticity" << std::endl;
        std::cout << "  --stdp-ltp-weight=VALUE       STDP LTP weight (default: 0.02)" << std::endl;
        std::cout << "  --stdp-ltd-weight=VALUE       STDP LTD weight (default: 0.015)" << std::endl;
        std::cout << "  --stdp-tau=VALUE              STDP time constant (default: 20.0)" << std::endl;
        std::cout << "  --synaptogenesis-rate=VALUE   Synaptogenesis rate (default: 0.0001)" << std::endl;
        std::cout << "  --pruning-rate=VALUE          Pruning rate (default: 0.00001)" << std::endl;
        std::cout << std::endl;
        std::cout << "Memory and Development:" << std::endl;
        std::cout << "  --enable-working-memory        Enable working memory system" << std::endl;
        std::cout << "  --enable-episodic-memory      Enable episodic memory system" << std::endl;
        std::cout << "  --max-episodes=VALUE          Maximum number of episodes (default: 1000)" << std::endl;
        std::cout << "  --enable-development          Enable developmental stages" << std::endl;
        std::cout << "  --developmental-stage=VALUE   Developmental stage (0=Initial, 1=Critical, 2=Maturation, 3=Adult)" << std::endl;
        std::cout << std::endl;
        std::cout << "Neuromodulation:" << std::endl;
        std::cout << "  --enable-dopamine             Enable dopamine system" << std::endl;
        std::cout << "  --enable-curiosity           Enable curiosity system" << std::endl;
        std::cout << "  --enable-prediction-error    Enable prediction error system" << std::endl;
        std::cout << "  --dopamine-level=VALUE        Dopamine level (0.0-1.0, default: 0.5)" << std::endl;
        std::cout << "  --curiosity-threshold=VALUE   Curiosity threshold (default: 0.1)" << std::endl;
        std::cout << std::endl;
        std::cout << "Experimental and Advanced:" << std::endl;
        std::cout << "  --save-checkpoint=FILE        Save brain state to checkpoint file" << std::endl;
        std::cout << "  --load-checkpoint=FILE        Load brain state from checkpoint file" << std::endl;
        std::cout << "  --benchmark                   Run benchmark tests" << std::endl;
        std::cout << "  --diagnose                    Run diagnostic tests" << std::endl;
        std::cout << "  --stats=VALUE                 Save statistics to file (json, txt, csv)" << std::endl;
        std::cout << "  --plot=VALUE                  Generate plots (neuro, memory, learning)" << std::endl;
        std::cout << "  --debug-mode                  Enable debug mode with additional logging" << std::endl;
        std::cout << "  --experimental-mode          Enable experimental features" << std::endl;
        std::cout << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  --config=FILE                 Load configuration from file" << std::endl;
        std::cout << "  --config-save=FILE            Save current configuration to file" << std::endl;
        std::cout << std::endl;
    }
    
    // Print version information
    void printVersion() const {
        std::cout << "NLM Neural Learning Machine v0.1.0" << std::endl;
        std::cout << "Phase 6: Final Integration" << std::endl;
        std::cout << "An experimental artificial developmental brain." << std::endl;
        std::cout << std::endl;
        std::cout << "Build Information:" << std::endl;
        std::cout << "  C++ Standard: C++20" << std::endl;
        std::cout << "  Architecture: x86_64" << std::endl;
        std::cout << "  Build Date: " << __DATE__ << " " << __TIME__ << std::endl;
    }
    
private:
    std::map<std::string, Argument> arguments;
};

// Enhanced configuration loading with command line parsing
class AdvancedConfig {
public:
    static std::shared_ptr<Config> loadConfig(int argc, char** argv) {
        auto config = std::make_shared<Config>();
        CommandLineParser parser;
        
        // Add general options
        parser.addArgument("help", 'h', "Show help information", "", false, false);
        parser.addArgument("version", 'v', "Show version information", "", false, false);
        parser.addArgument("verbose", 'v', "Enable verbose logging", "false", false, false);
        parser.addArgument("quiet", 'q', "Disable most logging", "false", false, false);
        
        // Simulation control
        parser.addArgument("timestep", 't', "Simulation timestep", "0.001", false, true);
        parser.addArgument("steps", 's', "Number of simulation steps", "100", false, true);
        parser.addArgument("neuron-count", 'n', "Number of neurons in network", "500", false, true);
        parser.addArgument("region-count", 'r', "Number of brain regions", "1", false, true);
        parser.addArgument("seed", 'S', "Random seed", "42", false, true);
        
        // Learning and plasticity
        parser.addArgument("enable-hebbian", '', "Enable Hebbian plasticity", "false", false, false);
        parser.addArgument("enable-stdp", '', "Enable STDP plasticity", "false", false, false);
        parser.addArgument("enable-reward", '', "Enable reward-modulated plasticity", "false", false, false);
        parser.addArgument("stdp- LTP-weight", '', "STDP LTP weight", "0.02", false, true);
        parser.addArgument("stdp-ltd-weight", '', "STDP LTD weight", "0.015", false, true);
        parser.addArgument("stdp-tau", '', "STDP time constant", "20.0", false, true);
        parser.addArgument("synaptogenesis-rate", '', "Synaptogenesis rate", "0.0001", false, true);
        parser.addArgument("pruning-rate", '', "Pruning rate", "0.00001", false, true);
        
        // Memory and development
        parser.addArgument("enable-working-memory", '', "Enable working memory system", "false", false, false);
        parser.addArgument("enable-episodic-memory", '', "Enable episodic memory system", "false", false, false);
        parser.addArgument("max-episodes", '', "Maximum number of episodes", "1000", false, true);
        parser.addArgument("enable-development", '', "Enable developmental stages", "false", false, false);
        parser.addArgument("developmental-stage", '', "Developmental stage (0-3)", "0", false, true);
        
        // Neuromodulation
        parser.addArgument("enable-dopamine", '', "Enable dopamine system", "false", false, false);
        parser.addArgument("enable-curiosity", '', "Enable curiosity system", "false", false, false);
        parser.addArgument("enable-prediction-error", '', "Enable prediction error system", "false", false, false);
        parser.addArgument("dopamine-level", '', "Dopamine level (0.0-1.0)", "0.5", false, true);
        parser.addArgument("curiosity-threshold", '', "Curiosity threshold", "0.1", false, true);
        
        // Experimental and advanced
        parser.addArgument("save-checkpoint", '', "Save brain state to checkpoint file", "", false, true);
        parser.addArgument("load-checkpoint", '', "Load brain state from checkpoint file", "", false, true);
        parser.addArgument("benchmark", '', "Run benchmark tests", "false", false, false);
        parser.addArgument("diagnose", '', "Run diagnostic tests", "false", false, false);
        parser.addArgument("stats", '', "Save statistics to file (json, txt, csv)", "", false, true);
        parser.addArgument("plot", '', "Generate plots (neuro, memory, learning)", "", false, true);
        parser.addArgument("debug-mode", '', "Enable debug mode with additional logging", "false", false, false);
        parser.addArgument("experimental-mode", '', "Enable experimental features", "false", false, false);
        
        // Configuration file
        parser.addArgument("config", 'c', "Load configuration from file", "", false, true);
        parser.addArgument("config-save", '', "Save current configuration to file", "", false, true);
        
        // Parse command line
        std::map<std::string, std::string> parsedArgs;
        bool parseSuccess = parser.parse(argc, argv, parsedArgs);
        
        if (!parseSuccess) {
            return nullptr;
        }
        
        // Process parsed arguments
        for (const auto& pair : parsedArgs) {
            const std::string& name = pair.first;
            const std::string& value = pair.second;
            
            try {
                if (name == "seed") {
                    config->set("random_seed", static_cast<int64_t>(parser.getIntValue(parsedArgs, name, 42)), ConfigSource::CommandLine);
                } else if (name == "timestep") {
                    config->set("simulation_timestep", parser.getDoubleValue(parsedArgs, name, 0.001), ConfigSource::CommandLine);
                } else if (name == "steps") {
                    config->set("simulation_steps", parser.getIntValue(parsedArgs, name, 100), ConfigSource::CommandLine);
                } else if (name == "neuron-count") {
                    config->set("neuron_count", parser.getIntValue(parsedArgs, name, 500), ConfigSource::CommandLine);
                } else if (name == "region-count") {
                    config->set("region_count", parser.getIntValue(parsedArgs, name, 1), ConfigSource::CommandLine);
                } else if (name == "stdp-ltp-weight") {
                    config->set("stdp_ltp_weight", parser.getFloatValue(parsedArgs, name, 0.02f), ConfigSource::CommandLine);
                } else if (name == "stdp-ltd-weight") {
                    config->set("stdp_ltd_weight", parser.getFloatValue(parsedArgs, name, 0.015f), ConfigSource::CommandLine);
                } else if (name == "stdp-tau") {
                    config->set("stdp_tau", parser.getFloatValue(parsedArgs, name, 20.0f), ConfigSource::CommandLine);
                } else if (name == "synaptogenesis-rate") {
                    config->set("synaptogenesis_rate", parser.getFloatValue(parsedArgs, name, 0.0001f), ConfigSource::CommandLine);
                } else if (name == "pruning-rate") {
                    config->set("pruning_rate", parser.getFloatValue(parsedArgs, name, 0.00001f), ConfigSource::CommandLine);
                } else if (name == "max-episodes") {
                    config->set("max_episodes", parser.getIntValue(parsedArgs, name, 1000), ConfigSource::CommandLine);
                } else if (name == "dopamine-level") {
                    config->set("dopamine_level", parser.getFloatValue(parsedArgs, name, 0.5f), ConfigSource::CommandLine);
                } else if (name == "curiosity-threshold") {
                    config->set("curiosity_threshold", parser.getFloatValue(parsedArgs, name, 0.1f), ConfigSource::CommandLine);
                } else if (name == "developmental-stage") {
                    config->set("developmental_stage", parser.getIntValue(parsedArgs, name, 0), ConfigSource::CommandLine);
                } else if (name == "verbose") {
                    config->set("verbose_logging", parser.getBoolValue(parsedArgs, name, false), ConfigSource::CommandLine);
                } else if (name == "quiet") {
                    config->set("quiet_logging", parser.getBoolValue(parsedArgs, name, false), ConfigSource::CommandLine);
                } else if (name == "benchmark") {
                    config->set("benchmark_mode", parser.getBoolValue(parsedArgs, name, false), ConfigSource::CommandLine);
                } else if (name == "diagnose") {
                    config->set("diagnose_mode", parser.getBoolValue(parsedArgs, name, false), ConfigSource::CommandLine);
                } else if (name == "debug-mode") {
                    config->set("debug_mode", parser.getBoolValue(parsedArgs, name, false), ConfigSource::CommandLine);
                } else if (name == "experimental-mode") {
                    config->set("experimental_mode", parser.getBoolValue(parsedArgs, name, false), ConfigSource::CommandLine);
                } else if (name == "enable-hebbian") {
                    config->set("hebbian_enabled", parser.getBoolValue(parsedArgs, name, false), ConfigSource::CommandLine);
                } else if (name == "enable-stdp") {
                    config->set("stdp_enabled", parser.getBoolValue(parsedArgs, name, false), ConfigSource::CommandLine);
                } else if (name == "enable-reward") {
                    config->set("reward_plasticity_enabled", parser.getBoolValue(parsedArgs, name, false), ConfigSource::CommandLine);
                } else if (name == "enable-working-memory") {
                    config->set("working_memory_enabled", parser.getBoolValue(parsedArgs, name, false), ConfigSource::CommandLine);
                } else if (name == "enable-episodic-memory") {
                    config->set("episodic_memory_enabled", parser.getBoolValue(parsedArgs, name, false), ConfigSource::CommandLine);
                } else if (name == "enable-development") {
                    config->set("development_enabled", parser.getBoolValue(parsedArgs, name, false), ConfigSource::CommandLine);
                } else if (name == "enable-dopamine") {
                    config->set("dopamine_enabled", parser.getBoolValue(parsedArgs, name, false), ConfigSource::CommandLine);
                } else if (name == "enable-curiosity") {
                    config->set("curiosity_enabled", parser.getBoolValue(parsedArgs, name, false), ConfigSource::CommandLine);
                } else if (name == "enable-prediction-error") {
                    config->set("prediction_error_enabled", parser.getBoolValue(parsedArgs, name, false), ConfigSource::CommandLine);
                } else if (name == "config-save") {
                    // Save configuration to file
                    if (config->saveToFile(value)) {
                        std::cout << "Configuration saved to: " << value << std::endl;
                    } else {
                        std::cerr << "Error: Failed to save configuration to: " << value << std::endl;
                    }
                    return config; // Exit after save
                } else if (name == "load-checkpoint") {
                    // Load checkpoint will be handled in main()
                    config->set("load_checkpoint", value, ConfigSource::CommandLine);
                } else if (name == "stats") {
                    // Save stats will be handled in main()
                    config->set("stats_output", value, ConfigSource::CommandLine);
                } else if (name == "plot") {
                    // Plot will be handled in main()
                    config->set("plot_output", value, ConfigSource::CommandLine);
                }
            } catch (const std::exception& e) {
                std::cerr << "Error processing option '--" << name << "': " << e.what() << std::endl;
                return nullptr;
            }
        }
        
        return config;
    }
};

} // namespace nlm
