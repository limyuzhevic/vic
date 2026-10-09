# Configuration management and validation
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace NLMConfig {

    // Configuration schema validation
    struct ConfigSchema {
        std::string section;
        std::string key;
        std::string type; // "int", "float", "string", "bool"
        std::string description;
        std::string defaultValue;
        std::string validationRegex;
        bool required;
    };

    // Configuration documentation
    const std::vector<ConfigSchema> CONFIG_SCHEMA = {
        // Simulation section
        {"simulation", "neuron_count", "int64", "Number of neurons", "1000", "^[0-9]+", true},
        {"simulation", "region_count", "int64", "Number of brain regions", "1", "^[0-9]+", true},
        {"simulation", "connection_probability", "float", "Synapse formation probability", "0.1", "^(0\.\d+|1\.0)$", true},
        
        // Plasticity section
        {"plasticity", "stdp_ltp_weight", "float", "STDP LTP weight", "0.01", "^0\.\d+$", true},
        {"plasticity", "stdp_ltd_weight", "float", "STDP LTD weight", "0.012", "^0\.\d+$", true},
        {"plasticity", "stdp_tau", "float", "STDP time constant", "20.0", "^[0-9]+\.?\d*$", true},
        {"plasticity", "synaptogenesis_rate", "float", "New synapse formation rate", "0.0001", "^0\.\d+$", true},
        {"plasticity", "pruning_rate", "float", "Synapse removal rate", "0.00001", "^0\.\d+$", true},
        
        // Development section
        {"development", "enable_development", "bool", "Enable developmental processes", "true", "^(true|false|0|1)$", true},
        
        // Memory section
        {"memory", "working_memory_capacity", "int64", "Working memory capacity", "100", "^[0-9]+", true},
        {"memory", "max_episodes", "int64", "Maximum episodic memories", "1000", "^[0-9]+", true},
        
        // Neuromodulation section
        {"neuromodulation", "enable_curiosity", "bool", "Enable curiosity-driven exploration", "true", "^(true|false|0|1)$", true},
        {"neuromodulation", "enable_novelty", "bool", "Enable novelty detection", "true", "^(true|false|0|1)$", true},
        {"neuromodulation", "reward_scaling", "float", "Reward scaling factor", "1.0", "^\d+\.?\d*$", true},
        
        // Visualization section
        {"visualization", "enable_viz", "bool", "Enable visualization", "false", "^(true|false|0|1)$", true},
        {"visualization", "viz_update_rate", "float", "Visualization update rate (Hz)", "0.1", "^\d+\.?\d*$", true},
        
        // Checkpoint section
        {"checkpoint", "enable_checkpointing", "bool", "Enable checkpointing", "true", "^(true|false|0|1)$", true},
        {"checkpoint", "checkpoint_interval", "int64", "Checkpoint interval (steps)", "1000", "^[0-9]+", true},
        {"checkpoint", "checkpoint_dir", "string", "Checkpoint directory", "./checkpoints", "^.*$", true},
        {"checkpoint", "max_checkpoints", "int64", "Maximum checkpoints to keep", "10", "^[0-9]+", true},
        
        // Logging section
        {"logging", "log_level", "string", "Log level", "info", "^(error|warning|info|debug)$", true},
        {"logging", "log_file", "string", "Log file path", "nlm.log", "^.*$", true},
        {"logging", "console_output", "bool", "Enable console output", "true", "^(true|false|0|1)$", true},
    };

    // Configuration preset definitions
    struct Preset {
        std::string name;
        std::map<std::string, std::string> parameters;
        std::string description;
    };

    const std::vector<Preset> PRESETS = {
        {
            "cognitive",
            {
                {"simulation.neuron_count", "1000"},
                {"simulation.connection_probability", "0.15"},
                {"plasticity.stdp_ltp_weight", "0.02"},
                {"plasticity.stdp_ltd_weight", "0.018"},
                {"development.enable_development", "true"},
                {"memory.working_memory_capacity", "150"},
                {"memory.max_episodes", "500"}
            },
            "Cognitive learning experiment with high plasticity"
        },
        {
            "social",
            {
                {"simulation.neuron_count", "800"},
                {"simulation.connection_probability", "0.12"},
                {"plasticity.stdp_ltp_weight", "0.015"},
                {"plasticity.stdp_ltd_weight", "0.014"},
                {"development.enable_development", "true"},
                {"neuromodulation.enable_curiosity", "true"},
                {"neuromodulation.enable_novelty", "true"},
                {"memory.max_episodes", "500"}
            },
            "Social learning experiment with interaction capabilities"
        },
        {
            "developmental",
            {
                {"simulation.neuron_count", "600"},
                {"simulation.connection_probability", "0.1"},
                {"plasticity.stdp_ltp_weight", "0.01"},
                {"plasticity.stdp_ltd_weight", "0.012"},
                {"development.enable_development", "true"},
                {"checkpoint.enable_checkpointing", "true"},
                {"checkpoint.checkpoint_interval", "500"},
                {"visualization.enable_viz", "false"}
            },
            "Developmental maturation experiment with long-term processes"
        },
        {
            "performance",
            {
                {"simulation.neuron_count", "2000"},
                {"simulation.connection_probability", "0.05"},
                {"development.enable_development", "false"},
                {"checkpoint.enable_checkpointing", "false"},
                {"logging.log_level", "warning"},
                {"logging.console_output", "false"}
            },
            "High-performance simulation with reduced overhead"
        }
    };

    // Configuration validation
    bool validateConfig(const Config& config) {
        std::vector<std::string> errors;
        
        for (const auto& schema : CONFIG_SCHEMA) {
            auto value = config.get(schema.section + "." + schema.key);
            if (!value.has_value()) {
                if (schema.required) {
                    errors.push_back(std::string("Missing required parameter: ") + schema.section + "." + schema.key);
                }
                continue;
            }
            
            std::string strValue;
            if (schema.type == "int64") {
                strValue = std::to_string(value.value<int64_t>());
            } else if (schema.type == "float") {
                strValue = std::to_string(value.value<float>());
            } else if (schema.type == "bool") {
                strValue = value.value<bool>() ? "true" : "false";
            } else {
                strValue = value.value<std::string>();
            }
            
            // Validate against regex if provided
            if (!schema.validationRegex.empty()) {
                // Simple regex validation (could be enhanced)
                if (schema.validationRegex != "^.*$") {
                    // Basic check - could use std::regex for full validation
                    if (schema.type == "int64") {
                        try {
                            std::stoll(strValue);
                        } catch (...) {
                            errors.push_back(std::string("Invalid value for ") + schema.section + "." + schema.key + ": " + strValue);
                        }
                    } else if (schema.type == "float") {
                        try {
                            std::stof(strValue);
                        } catch (...) {
                            errors.push_back(std::string("Invalid value for ") + schema.section + "." + schema.key + ": " + strValue);
                        }
                    } else if (schema.type == "bool") {
                        if (strValue != "true" && strValue != "false" && strValue != "0" && strValue != "1") {
                            errors.push_back(std::string("Invalid boolean value for ") + schema.section + "." + schema.key + ": " + strValue);
                        }
                    }
                }
            }
            
            // Validate range
            if (schema.type == "int64") {
                int64_t intValue = value.value<int64_t>();
                if (schema.key == "neuron_count" && (intValue < 100 || intValue > 10000)) {
                    errors.push_back(std::string("neuron_count out of range [100, 10000]: ") + strValue);
                } else if (schema.key == "region_count" && (intValue < 1 || intValue > 10)) {
                    errors.push_back(std::string("region_count out of range [1, 10]: ") + strValue);
                }
            }
            
            if (schema.type == "float") {
                float floatValue = value.value<float>();
                if (schema.key == "connection_probability" && (floatValue < 0.01f || floatValue > 0.5f)) {
                    errors.push_back(std::string("connection_probability out of range [0.01, 0.5]: ") + strValue);
                } else if (schema.key == "stdp_tau" && (floatValue < 5.0f || floatValue > 100.0f)) {
                    errors.push_back(std::string("stdp_tau out of range [5.0, 100.0]: ") + strValue);
                }
            }
        }
        
        if (!errors.empty()) {
            NLM_LOG_ERROR("Configuration validation failed:");
            for (const auto& error : errors) {
                NLM_LOG_ERROR("  " + error);
            }
            return false;
        }
        
        return true;
    }

    // Configuration preset application
    void applyPreset(Config& config, const std::string& presetName) {
        for (const auto& preset : PRESETS) {
            if (preset.name == presetName) {
                NLM_LOG_INFO(std::string("Applying preset: ") + presetName);
                NLM_LOG_INFO("  " + preset.description);
                
                for (const auto& param : preset.parameters) {
                    size_t dotPos = param.first.find('.');
                    if (dotPos != std::string::npos) {
                        std::string section = param.first.substr(0, dotPos);
                        std::string key = param.first.substr(dotPos + 1);
                        
                        // Parse parameter based on type
                        if (param.second.find('.') != std::string::npos) {
                            // Float
                            float floatValue = std::stof(param.second);
                            config.set(section + "." + key, floatValue, ConfigSource::Preset);
                        } else {
                            // Try int first, then string
                            try {
                                int64_t intValue = std::stoll(param.second);
                                config.set(section + "." + key, intValue, ConfigSource::Preset);
                            } catch (...) {
                                config.set(section + "." + key, param.second, ConfigSource::Preset);
                            }
                        }
                        
                        NLM_LOG_INFO(std::string("    ") + section + "." + key + " = " + param.second);
                    }
                }
                return;
            }
        }
        
        throw std::runtime_error(std::string("Unknown preset: ") + presetName);
    }

    // Configuration template generation
    void generateConfigTemplate(const std::string& templateType, const std::string& outputFile) {
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            throw std::runtime_error(std::string("Cannot open output file: ") + outputFile);
        }
        
        if (templateType == "minimal") {
            file << "# Minimal NLM Configuration\n";
            file << "# This configuration is suitable for quick testing\n\n";
            file << "[simulation]\n";
            file << "neuron_count = 100\n";
            file << "region_count = 1\n";
            file << "connection_probability = 0.2\n\n";
            file << "[plasticity]\n";
            file << "stdp_ltp_weight = 0.02\n";
            file << "stdp_ltd_weight = 0.018\n\n";
            file << "[development]\n";
            file << "enable_development = false\n\n";
            file << "[logging]\n";
            file << "log_level = warning\n";
            file << "console_output = true\n";
        
        } else if (templateType == "research") {
            file << "# Research Configuration\n";
            file << "# This configuration is suitable for research experiments\n\n";
            file << "[simulation]\n";
            file << "neuron_count = 2000\n";
            file << "region_count = 2\n";
            file << "connection_probability = 0.15\n\n";
            file << "[plasticity]\n";
            file << "stdp_ltp_weight = 0.025\n";
            file << "stdp_ltd_weight = 0.024\n";
            file << "stdp_tau = 30.0\n";
            file << "synaptogenesis_rate = 0.0005\n";
            file << "pruning_rate = 0.00005\n\n";
            file << "[development]\n";
            file << "enable_development = true\n\n";
            file << "[memory]\n";
            file << "working_memory_capacity = 300\n";
            file << "max_episodes = 3000\n\n";
            file << "[neuromodulation]\n";
            file << "enable_curiosity = true\n";
            file << "enable_novelty = true\n";
            file << "reward_scaling = 1.0\n\n";
            file << "[visualization]\n";
            file << "enable_viz = true\n";
            file << "viz_update_rate = 1.0\n\n";
            file << "[checkpoint]\n";
            file << "enable_checkpointing = true\n";
            file << "checkpoint_interval = 2000\n";
            file << "checkpoint_dir = ./research_checkpoints\n";
            file << "max_checkpoints = 50\n\n";
            file << "[logging]\n";
            file << "log_level = info\n";
            file << "log_file = nlm_research.log\n";
            file << "console_output = true\n";
        
        } else if (templateType == "production") {
            file << "# Production Configuration\n";
            file << "# This configuration is optimized for stable long-running experiments\n\n";
            file << "[simulation]\n";
            file << "neuron_count = 5000\n";
            file << "region_count = 3\n";
            file << "connection_probability = 0.08\n\n";
            file << "[plasticity]\n";
            file << "stdp_ltp_weight = 0.008\n";
            file << "stdp_ltd_weight = 0.009\n\n";
            file << "[development]\n";
            file << "enable_development = false\n\n";
            file << "[checkpoint]\n";
            file << "enable_checkpointing = true\n";
            file << "checkpoint_interval = 5000\n";
            file << "checkpoint_dir = ./production_checkpoints\n";
            file << "max_checkpoints = 20\n\n";
            file << "[logging]\n";
            file << "log_level = warning\n";
            file << "log_file = nlm_production.log\n";
            file << "console_output = false\n";
        
        } else {
            throw std::runtime_error(std::string("Unknown config template: ") + templateType);
        }
        
        file << "\n# Configuration file generated by NLM Configuration Template Generator\n";
        file << "# For more information, visit: https://github.com/yourrepo/nlm\n";
        
        NLM_LOG_INFO(std::string("Configuration template generated: ") + outputFile);
    }

    // Configuration parameter help
    void printConfigHelp() {
        std::cout << "NLM Configuration System Help" << std::endl;
        std::cout << std::string(50, '=') << std::endl << std::endl;
        
        std::cout << "Configuration Files:" << std::endl;
        std::cout << "  configs/default.cfg      - Default configuration" << std::endl;
        std::cout << "  configs/CONFIG_PARAMETERS.md - Parameter documentation" << std::endl;
        std::cout << "  configs/experiments.batch - Example batch file" << std::endl << std::endl;
        
        std::cout << "Preset Configurations:" << std::endl;
        for (const auto& preset : PRESETS) {
            std::cout << "  " << preset.name << ": " << preset.description << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "Config Template Types:" << std::endl;
        std::cout << "  minimal     - Quick testing configuration" << std::endl;
        std::cout << "  research    - Full research configuration" << std::endl;
        std::cout << "  production  - Stable long-running configuration" << std::endl << std::endl;
        
        std::cout << "Parameter Override:" << std::endl;
        std::cout << "  Use --param section.key=value from command line" << std::endl;
        std::cout << "  Example: nlm --experiment cognitive --param simulation.neuron_count=2000" << std::endl << std::endl;
        
        std::cout << "Configuration Validation:" << std::endl;
        std::cout << "  Configuration files are validated for syntax and parameter ranges" << std::endl;
        std::cout << "  Invalid configurations will cause errors before experiment starts" << std::endl << std::endl;
        
        std::cout << "Preset Usage:" << std::endl;
        std::cout << "  Use --param preset=preset_name" << std::endl;
        std::cout << "  Example: nlm --experiment social --param preset=research" << std::endl;
    }

} // namespace NLMConfig
