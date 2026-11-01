#!/usr/bin/env g++ -std=c++20 -O2 -Wall -Wextra
//============================================================================
// NLM Command Line Interface (Enhanced)
//============================================================================

#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>

using namespace nlm;

// Configuration option definition
struct CLIOption {
    std::string name;
    std::string shortFlag;
    std::string description;
    std::string defaultValue;
    bool required;
    std::string type;
};

// CLI application class
class NLMCLI {
private:
    std::vector<CLIOption> options;
    Config config;
    bool showHelp;
    bool showVersion;
    bool verbose;
    
public:
    NLMCLI(int argc, char** argv) : showHelp(false), showVersion(false), verbose(false) {
        // Define available options
        options = {
            {"neuron_count", "n", "Number of neurons in the brain", "1000", false, "int64"},
            {"simulation_timestep", "t", "Time step for simulation (seconds)", "0.001", false, "double"},
            {"connection_probability", "c", "Probability of synapse creation (0.0-1.0)", "0.1", false, "float"},
            {"region_count", "r", "Number of brain regions", "1", false, "int64"},
            {"config", "", "Load configuration from file", "", false, "string"},
            {"log_level", "l", "Log level (Debug/Info/Warn/Error/Fatal)", "Info", false, "string"},
            {"random_seed", "", "Random seed for reproducible results", "42", false, "int64"},
            {"verbose", "v", "Enable verbose output", "", false, "flag"},
            {"help", "h", "Show help information", "", false, "flag"},
            {"version", "", "Show version information", "", false, "flag"},
            {"validate", "", "Validate configuration and exit", "", false, "flag"}
        };
        
        // Parse command line arguments
        parseArguments(argc, argv);
    }
    
    void parseArguments(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Check for flags
            if (arg == "--help" || arg == "-h") {
                showHelp = true;
            } else if (arg == "--version") {
                showVersion = true;
            } else if (arg == "--verbose" || arg == "-v") {
                verbose = true;
            } else if (arg == "--validate") {
                // Will be handled after parsing
            } 
            
            // Check for --key=value format
            else if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    processOption(key, value);
                }
            }
            
            // Check for -key value format
            else if (arg[0] == '-') {
                std::string key = arg.substr(1);
                // Find next argument if it doesn't start with -
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    std::string value = argv[++i];
                    processOption(key, value);
                }
            }
        }
    }
    
    void processOption(const std::string& key, const std::string& value) {
        for (auto& option : options) {
            if (option.name == key) {
                setConfigValue(key, value, option.type);
                if (verbose) {
                    std::cout << "[VERBOSE] Set " << key << " = " << value << std::endl;
                }
                return;
            }
        }
        
        // Unknown option
        std::cerr << "Warning: Unknown option --" << key << std::endl;
    }
    
    void setConfigValue(const std::string& key, const std::string& value, const std::string& type) {
        try {
            if (type == "int64") {
                long long int_val = std::stoll(value);
                config.set(key, int_val, ConfigSource::CommandLine);
            } else if (type == "double") {
                double double_val = std::stod(value);
                config.set(key, double_val, ConfigSource::CommandLine);
            } else if (type == "float") {
                float float_val = std::stof(value);
                config.set(key, float_val, ConfigSource::CommandLine);
            } else if (type == "string") {
                config.set(key, value, ConfigSource::CommandLine);
            } else if (type == "flag") {
                config.set(key, true, ConfigSource::CommandLine);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing value for " << key << ": " << e.what() << std::endl;
        }
    }
    
    void loadConfigFile(const std::string& filepath) {
        if (verbose) {
            std::cout << "[VERBOSE] Loading configuration from: " << filepath << std::endl;
        }
        
        bool loaded = config.loadFromFile(filepath);
        if (!loaded) {
            std::cerr << "Error: Failed to load configuration file: " << filepath << std::endl;
        }
    }
    
    void showHelpText() {
        std::cout << "NLM - Neural Learning Machine (熙然)" << std::endl;
        std::cout << "Version: 0.1.0 (Phase 2 - Real Neural Computation)" << std::endl;
        std::cout << std::endl;
        std::cout << "USAGE:" << std::endl;
        std::cout << "  nlm [OPTIONS]" << std::endl;
        std::cout << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        
        // Show long options
        for (const auto& option : options) {
            if (option.shortFlag.empty()) {
                std::cout << "  --" << option.name << "=" << option.type << std::endl;
                std::cout << "      " << option.description << std::endl;
                if (!option.defaultValue.empty()) {
                    std::cout << "      Default: " << option.defaultValue << std::endl;
                }
                std::cout << std::endl;
            }
        }
        
        // Show short options
        std::cout << "  Short options:" << std::endl;
        for (const auto& option : options) {
            if (!option.shortFlag.empty()) {
                std::cout << "  -" << option.shortFlag;
                if (option.type != "flag") {
                    std::cout << " " << option.name << "=" << option.type;
                }
                std::cout << std::endl;
                std::cout << "      " << option.description << std::endl;
                std::cout << std::endl;
            }
        }
        
        std::cout << "COMMON EXAMPLES:" << std::endl;
        std::cout << "  nlm --neuron_count=5000 --simulation_timestep=0.001" << std::endl;
        std::cout << "  nlm -n 2000 -t 0.005 -c 0.2 --config=custom.cfg" << std::endl;
        std::cout << "  nlm --validate --neuron_count=100" << std::endl;
        std::cout << std::endl;
        std::cout << "VALIDATION:" << std::endl;
        std::cout << "  nlm --validate will check configuration validity" << std::endl;
        std::cout << "  Invalid values will show descriptive error messages" << std::endl;
    }
    
    void showVersionInfo() {
        std::cout << "NLM Neural Learning Machine (熙然)" << std::endl;
        std::cout << "Version: 0.1.0 (Phase 2 - Real Neural Computation)" << std::endl;
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "Author: NLM Research Project" << std::endl;
        std::cout << "License: MIT" << std::endl;
        std::cout << std::endl;
        std::cout << "This is an experimental artificial developmental brain system." << std::endl;
        std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
    }
    
    void run() {
        // Handle help and version flags first
        if (showHelp) {
            showHelpText();
            return;
        }
        
        if (showVersion) {
            showVersionInfo();
            return;
        }
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(verbose ? LogLevel::Debug : LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("Starting NLM Neural Learning Machine (Phase 2)");
        
        // Validate configuration
        auto errors = config.validate();
        if (!errors.empty()) {
            std::cout << "Configuration validation failed:" << std::endl;
            for (const auto& error : errors) {
                std::cout << "  Error: " << error.key << " - " << error.message << std::endl;
            }
            
            if (verbose) {
                std::cout << std::endl << "Current configuration:" << std::endl;
                std::cout << config.summary() << std::endl;
            }
            
            std::cout << std::endl << "Use --validate flag to auto-fix common issues" << std::endl;
            return;
        }
        
        NLM_LOG_INFO("Configuration validated successfully");
        
        // Show final configuration
        std::cout << std::endl << "Final Configuration:" << std::endl;
        std::cout << config.summary() << std::endl;
        
        NLM_LOG_INFO("NLM configuration complete");
    }
};

int main(int argc, char** argv) {
    try {
        NLMCLI cli(argc, argv);
        cli.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}