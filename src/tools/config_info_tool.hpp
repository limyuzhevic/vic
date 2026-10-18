// Configuration Tool Implementation
//
// This file implements the config-info CLI tool for displaying configuration details.

#pragma once

#include "tools/config_tool.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {

class ConfigInfoTool : public CLITool {
public:
    ConfigInfoTool() = default;
    ~ConfigInfoTool() = default;
    
    // Tool information
    std::string getName() const override {
        return "config-info";
    }
    
    std::string getDescription() const override {
        return "Show configuration details and summary information";
    }
    
    std::string getUsage() const override {
        return "nlm-cli config-info [options]";
    }
    
    CommandType getType() const override {
        return CommandType::CONFIG;
    }
    
    std::string getCategory() const override {
        return "Configuration Management";
    }
    
    // Tool execution
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override {
        printInfo("=== Configuration Information ===");
        
        // Display configuration summary
        std::string summary = config.summary();
        printInfo(summary);
        
        // Display key configuration values
        printInfo("\n=== Key Configuration Values ===");
        
        // Display all configuration values
        printInfo("Random seed: " + std::to_string(config.getOr<int64_t>("random_seed", 42)));
        printInfo("Simulation timestep: " + std::to_string(config.getOr<double>("simulation_timestep", 0.001)) + "s");
        printInfo("Neuron count: " + std::to_string(config.getOr<int64_t>("neuron_count", 500)));
        printInfo("Region count: " + std::to_string(config.getOr<int64_t>("region_count", 1)));
        printInfo("Connection probability: " + std::to_string(config.getOr<float>("connection_probability", 0.15f)));
        printInfo("STDP LTP weight: " + std::to_string(config.getOr<float>("stdp_ltp_weight", 0.02f)));
        printInfo("STDP LTD weight: " + std::to_string(config.getOr<float>("stdp_ltd_weight", 0.015f)));
        printInfo("Max simulation steps: " + std::to_string(config.getOr<int64_t>("max_simulation_steps", 10000)));
        
        // Display enable/disable flags
        printInfo("\n=== Configuration Flags ===");
        printInfo("Validation enabled: " + std::string(config.getOr<bool>("enable_validation", false) ? "Yes" : "No"));
        printInfo("Log to file: " + std::string(config.getOr<bool>("log_to_file", false) ? "Yes" : "No"));
        printInfo("Record neurons: " + std::string(config.getOr<bool>("record_neurons", false) ? "Yes" : "No"));
        printInfo("Record synapses: " + std::string(config.getOr<bool>("record_synapses", false) ? "Yes" : "No"));
        printInfo("Record memories: " + std::string(config.getOr<bool>("record_memories", false) ? "Yes" : "No"));
        printInfo("Enable SIMD: " + std::string(config.getOr<bool>("enable_simd", false) ? "Yes" : "No"));
        
        // Display log information
        std::string logLevel = config.getOr<std::string>("log_level", "INFO");
        std::string logFilename = config.getOr<std::string>("log_filename", "");
        if (!logFilename.empty()) {
            printInfo("Log level: " + logLevel);
            printInfo("Log filename: " + logFilename);
        }
        
        printSuccess("Configuration information displayed successfully");
        return ExecutionResult::SUCCESS;
    }
    
    // Tool-specific help
    void displayHelp() const override {
        std::cout << "=== config-info Help ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Description: Show configuration details and summary information" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: nlm-cli config-info [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "This tool displays the current configuration state including:" << std::endl;
        std::cout << "  - Configuration summary" << std::endl;
        std::cout << "  - Key configuration values (seed, timestep, neuron count, etc.)" << std::endl;
        std::cout << "  - Configuration flags and settings" << std::endl;
        std::cout << "  - Log configuration details" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm-cli config-info" << std::endl;
        std::cout << "  nlm-cli config-info --verbose" << std::endl;
        std::cout << std::endl;
        std::cout << "Category: Configuration Management" << std::endl;
    }
    
    // Tool-specific validation
    bool validateArgs(const std::vector<std::string>& args) const override {
        // Basic validation - no required arguments
        return true;
    }
};

} // namespace nlm