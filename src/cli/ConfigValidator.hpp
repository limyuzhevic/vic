// Configuration validation for CLI
// Validation rules and error handling for command-line arguments

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <algorithm>
#include <cctype>
#include "core/Config/Config.hpp"

namespace nlm::cli {

// Validation rule for configuration values
class ConfigValidator {
public:
    // Validate configuration value against rules
    static bool validate(const std::string& key, const nlm::ConfigValue& value, 
                        const std::shared_ptr<Config>& config = nullptr) {
        
        // Type-specific validation
        if (key == "random_seed") {
            if (!std::holds_alternative<int64_t>(value)) {
                std::cerr << "Error: random_seed must be an integer" << std::endl;
                return false;
            }
            int64_t seed = std::get<int64_t>(value);
            if (seed <= 0) {
                std::cerr << "Error: random_seed must be positive" << std::endl;
                return false;
            }
        }
        
        else if (key == "neuron_count") {
            if (!std::holds_alternative<int64_t>(value)) {
                std::cerr << "Error: neuron_count must be an integer" << std::endl;
                return false;
            }
            int64_t neurons = std::get<int64_t>(value);
            if (neurons <= 0) {
                std::cerr << "Error: neuron_count must be positive" << std::endl;
                return false;
            }
            if (neurons > 100000) {
                std::cerr << "Error: neuron_count too large (max: 100000)" << std::endl;
                return false;
            }
        }
        
        else if (key == "simulation_timestep") {
            if (!std::holds_alternative<double>(value)) {
                std::cerr << "Error: simulation_timestep must be a float" << std::endl;
                return false;
            }
            double timestep = std::get<double>(value);
            if (timestep <= 0) {
                std::cerr << "Error: simulation_timestep must be positive" << std::endl;
                return false;
            }
            if (timestep > 1.0) {
                std::cerr << "Error: simulation_timestep too large (max: 1.0)" << std::endl;
                return false;
            }
        }
        
        else if (key == "region_count") {
            if (!std::holds_alternative<int64_t>(value)) {
                std::cerr << "Error: region_count must be an integer" << std::endl;
                return false;
            }
            int64_t regions = std::get<int64_t>(value);
            if (regions <= 0) {
                std::cerr << "Error: region_count must be positive" << std::endl;
                return false;
            }
            if (regions > 100) {
                std::cerr << "Error: region_count too large (max: 100)" << std::endl;
                return false;
            }
        }
        
        else if (key == "connection_probability") {
            if (!std::holds_alternative<float>(value)) {
                std::cerr << "Error: connection_probability must be a float" << std::endl;
                return false;
            }
            float prob = std::get<float>(value);
            if (prob < 0 || prob > 1) {
                std::cerr << "Error: connection_probability must be between 0 and 1" << std::endl;
                return false;
            }
        }
        
        else if (key == "max_simulation_steps") {
            if (!std::holds_alternative<int64_t>(value)) {
                std::cerr << "Error: max_simulation_steps must be an integer" << std::endl;
                return false;
            }
            int64_t steps = std::get<int64_t>(value);
            if (steps <= 0) {
                std::cerr << "Error: max_simulation_steps must be positive" << std::endl;
                return false;
            }
            if (steps > 10000000) {
                std::cerr << "Error: max_simulation_steps too large (max: 10000000)" << std::endl;
                return false;
            }
        }
        
        else if (key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || 
                 key == "stdp_tau" || key == "plasticity_learning_rate") {
            if (!std::holds_alternative<float>(value)) {
                std::cerr << "Error: " << key << " must be a float" << std::endl;
                return false;
            }
            float param = std::get<float>(value);
            if (param < 0) {
                std::cerr << "Error: " << key << " must be positive" << std::endl;
                return false;
            }
        }
        
        else if (key == "synaptogenesis_rate" || key == "pruning_rate") {
            if (!std::holds_alternative<float>(value)) {
                std::cerr << "Error: " << key << " must be a float" << std::endl;
                return false;
            }
            float rate = std::get<float>(value);
            if (rate < 0 || rate > 1) {
                std::cerr << "Error: " << key << " must be between 0 and 1" << std::endl;
                return false;
            }
        }
        
        else if (key == "dopamine_scale") {
            if (!std::holds_alternative<float>(value)) {
                std::cerr << "Error: dopamine_scale must be a float" << std::endl;
                return false;
            }
            float scale = std::get<float>(value);
            if (scale < 0) {
                std::cerr << "Error: dopamine_scale must be positive" << std::endl;
                return false;
            }
        }
        
        // Validate against dependencies
        if (config) {
            if (!validateDependencies(key, value, config)) {
                return false;
            }
        }
        
        return true;
    }
    
private:
    // Validate parameter dependencies
    static bool validateDependencies(const std::string& key, const nlm::ConfigValue& value, 
                                     const std::shared_ptr<Config>& config) {
        
        // Check if there are any cross-parameter validations
        if (key == "neuron_count" && config->has("region_count")) {
            int64_t neurons = std::get<int64_t>(value);
            int64_t regions = config->getOr<int64_t>("region_count", 1);
            if (neurons / regions < 10) {
                std::cerr << "Warning: Very few neurons per region (" << neurons / regions << ")" << std::endl;
            }
        }
        
        if (key == "connection_probability" && config->has("neuron_count")) {
            float prob = std::get<float>(value);
            int64_t neurons = config->getOr<int64_t>("neuron_count", 1000);
            int64_t expected_connections = static_cast<int64_t>(neurons * (neurons - 1) * prob / 2);
            if (expected_connections > 1000000) {
                std::cerr << "Warning: Large number of connections (" << expected_connections << ") may impact performance" << std::endl;
            }
        }
        
        if (key == "simulation_timestep" && config->has("max_simulation_steps")) {
            double timestep = std::get<double>(value);
            int64_t steps = config->getOr<int64_t>("max_simulation_steps", 1000);
            double total_time = timestep * steps;
            if (total_time > 3600) {
                std::cerr << "Warning: Long simulation time (" << total_time << "s)" << std::endl;
            }
        }
        
        return true;
    }
};

// Configuration file discovery
class ConfigDiscovery {
public:
    static std::vector<std::string> findConfigFiles() {
        std::vector<std::string> config_files;
        
        // Common configuration file locations
        std::vector<std::string> search_paths = {
            "./configs",
            "configs",
            ".",
            ".."
        };
        
        for (const auto& path : search_paths) {
            findConfigFilesInPath(path, config_files);
        }
        
        // Add specific file patterns
        std::vector<std::string> patterns = {
            "*.json",
            "*.yaml",
            "*.yml",
            "*.cfg",
            "*.txt"
        };
        
        for (const auto& pattern : patterns) {
            // This would use file system operations to find matching files
            // For now, return a few common configuration files
        }
        
        return config_files;
    }
    
private:
    static void findConfigFilesInPath(const std::string& path, std::vector<std::string>& config_files) {
        // This would use file system operations
        // For demonstration, return some common config files
        static const std::vector<std::string> common_configs = {
            "configs/default.cfg",
            "configs/performance.cfg",
            "configs/debug.cfg",
            "configs/development.cfg"
        };
        
        for (const auto& config : common_configs) {
            config_files.push_back(config);
        }
    }
};

// Environment variable handler
class EnvironmentVariableHandler {
public:
    static std::map<std::string, std::string> getEnvironmentVariables() {
        std::map<std::string, std::string> env_vars;
        
        // NLM-specific environment variables
        const char* nlm_config = std::getenv("NLM_CONFIG");
        if (nlm_config != nullptr) {
            env_vars["NLM_CONFIG"] = std::string(nlm_config);
        }
        
        const char* nlm_log_level = std::getenv("NLM_LOG_LEVEL");
        if (nlm_log_level != nullptr) {
            env_vars["NLM_LOG_LEVEL"] = std::string(nlm_log_level);
        }
        
        const char* nlm_seed = std::getenv("NLM_SEED");
        if (nlm_seed != nullptr) {
            env_vars["NLM_SEED"] = std::string(nlm_seed);
        }
        
        const char* nlm_verbose = std::getenv("NLM_VERBOSE");
        if (nlm_verbose != nullptr) {
            env_vars["NLM_VERBOSE"] = std::string(nlm_verbose);
        }
        
        const char* nlm_quiet = std::getenv("NLM_QUIET");
        if (nlm_quiet != nullptr) {
            env_vars["NLM_QUIET"] = std::string(nlm_quiet);
        }
        
        const char* nlm_neurons = std::getenv("NLM_NEURONS");
        if (nlm_neurons != nullptr) {
            env_vars["NLM_NEURONS"] = std::string(nlm_neurons);
        }
        
        const char* nlm_regions = std::getenv("NLM_REGIONS");
        if (nlm_regions != nullptr) {
            env_vars["NLM_REGIONS"] = std::string(nlm_regions);
        }
        
        return env_vars;
    }
    
    static std::string getEnvironmentVariable(const std::string& name) {
        const char* value = std::getenv(name.c_str());
        return (value != nullptr) ? std::string(value) : "";
    }
};