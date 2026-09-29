// Configuration validation utilities
// 
// Provides validation functions for NLM configuration files and command-line arguments

#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <set>

namespace nlm {
namespace config {

// Required configuration keys for Phase 6
struct RequiredKeys {
    std::set<std::string> brain;
    std::set<std::string> plasticity;
    std::set<std::string> neuromodulation;
    std::set<std::string> memory;
    std::set<std::string> development;
    std::set<std::string> prediction;
    
    RequiredKeys() {
        // Brain system
        brain = {"neuron_count", "region_count", "connection_probability"};
        
        // Plasticity
        plasticity = {"stdp_ltp_weight", "stdp_ltd_weight"};
        
        // Neuromodulation
        neuromodulation = {"dopamine.scale", "curiosity.adaptation_rate"};
        
        // Memory
        memory = {"working_memory.capacity", "episodic_memory.max_episodes"};
        
        // Development
        development = {"development.initial_plasticity", "development.maturity_age"};
        
        // Prediction
        prediction = {"prediction.horizon", "prediction.confidence_threshold"};
    }
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getSummary() const {
        std::string result = "Validation: " + std::string(valid ? "PASSED" : "FAILED");
        if (!errors.empty()) {
            result += " (Errors: " + std::to_string(errors.size()) + ")";
        }
        if (!warnings.empty()) {
            result += " (Warnings: " + std::to_string(warnings.size()) + ")";
        }
        return result;
    }
};

// Validate configuration against required schema
ValidationResult validateConfig(const Config& config, const RequiredKeys& required = RequiredKeys()) {
    ValidationResult result;
    
    // Check brain system keys
    for (const auto& key : required.brain) {
        if (!config.has(key)) {
            result.addError("Missing required brain configuration key: " + key);
        }
    }
    
    // Check plasticity keys
    for (const auto& key : required.plasticity) {
        if (!config.has(key)) {
            result.addWarning("Missing plasticity configuration key (may use defaults): " + key);
        }
    }
    
    // Check neuromodulation keys
    for (const auto& key : required.neuromodulation) {
        if (!config.has(key)) {
            result.addWarning("Missing neuromodulation configuration key (may use defaults): " + key);
        }
    }
    
    // Check memory keys
    for (const auto& key : required.memory) {
        if (!config.has(key)) {
            result.addWarning("Missing memory configuration key (may use defaults): " + key);
        }
    }
    
    // Check development keys
    for (const auto& key : required.development) {
        if (!config.has(key)) {
            result.addWarning("Missing development configuration key (may use defaults): " + key);
        }
    }
    
    // Check prediction keys
    for (const auto& key : required.prediction) {
        if (!config.has(key)) {
            result.addWarning("Missing prediction configuration key (may use defaults): " + key);
        }
    }
    
    // Validate value ranges where applicable
    if (config.has("neuron_count")) {
        int64_t neuronCount = config.getOr<int64_t>("neuron_count", 0);
        if (neuronCount <= 0) {
            result.addError("neuron_count must be positive, got: " + std::to_string(neuronCount));
        } else if (neuronCount > 100000) {
            result.addWarning("neuron_count is very large (" + std::to_string(neuronCount) + "), may impact performance");
        }
    }
    
    if (config.has("region_count")) {
        int64_t regionCount = config.getOr<int64_t>("region_count", 0);
        if (regionCount <= 0) {
            result.addError("region_count must be positive, got: " + std::to_string(regionCount));
        }
    }
    
    if (config.has("connection_probability")) {
        float prob = config.getOr<float>("connection_probability", 0.0f);
        if (prob < 0.0f || prob > 1.0f) {
            result.addError("connection_probability must be between 0.0 and 1.0, got: " + std::to_string(prob));
        }
    }
    
    if (config.has("simulation_timestep")) {
        double timestep = config.getOr<double>("simulation_timestep", 0.0);
        if (timestep <= 0.0) {
            result.addError("simulation_timestep must be positive, got: " + std::to_string(timestep));
        } else if (timestep > 0.1) {
            result.addWarning("simulation_timestep is large (" + std::to_string(timestep) + "), may affect simulation accuracy");
        }
    }
    
    return result;
}

// Load and validate configuration file
ValidationResult loadAndValidateConfig(Config& config, const std::string& configFile) {
    ValidationResult result;
    
    // Try to load config from file
    if (!configFile.empty() && config.loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
        
        // Validate the loaded configuration
        ValidationResult validation = validateConfig(config);
        result.errors = std::move(validation.errors);
        result.warnings = std::move(validation.warnings);
        
        if (!validation.valid) {
            NLM_LOG_ERROR("Configuration validation failed:");
            for (const auto& error : validation.errors) {
                NLM_LOG_ERROR("  ERROR: " + error);
            }
        }
        
        if (!validation.warnings.empty()) {
            NLM_LOG_WARNING("Configuration warnings:");
            for (const auto& warning : validation.warnings) {
                NLM_LOG_WARNING("  WARNING: " + warning);
            }
        }
    } else if (!configFile.empty()) {
        NLM_LOG_WARNING("Could not load configuration from: " + configFile);
        NLM_LOG_WARNING("Using default configuration.");
    } else {
        NLM_LOG_WARNING("No configuration file specified, using defaults.");
    }
    
    return result;
}

// Parse and validate command line arguments
ValidationResult parseAndValidateCommandLineArgs(int argc, char** argv, Config& config) {
    ValidationResult result;
    
    // First, let the config parse the args
    config.loadFromArgs(argc, argv);
    
    // Additional validation for command line specific args
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 9) == "--neurons") {
            if (arg.find('=') != std::string::npos) {
                std::string valueStr = arg.substr(arg.find('=') + 1);
                try {
                    int64_t value = std::stoll(valueStr);
                    if (value <= 0) {
                        result.addError("--neurons value must be positive, got: " + valueStr);
                    }
                } catch (const std::exception& e) {
                    result.addError("Invalid --neurons value: " + valueStr);
                }
            } else if (i + 1 < argc) {
                try {
                    int64_t value = std::stoll(argv[++i]);
                    if (value <= 0) {
                        result.addError("--neurons value must be positive, got: " + std::to_string(value));
                    }
                } catch (const std::exception& e) {
                    result.addError("Invalid --neurons value");
                }
            }
        } else if (arg.substr(0, 7) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                std::string valueStr = arg.substr(arg.find('=') + 1);
                try {
                    uint64_t value = std::stoull(valueStr);
                    if (value == 0) {
                        result.addError("--steps value must be positive, got: " + valueStr);
                    }
                } catch (const std::exception& e) {
                    result.addError("Invalid --steps value: " + valueStr);
                }
            } else if (i + 1 < argc) {
                try {
                    uint64_t value = std::stoull(argv[++i]);
                    if (value == 0) {
                        result.addError("--steps value must be positive, got: " + std::to_string(value));
                    }
                } catch (const std::exception& e) {
                    result.addError("Invalid --steps value");
                }
            }
        } else if (arg.substr(0, 9) == "--timestep") {
            if (arg.find('=') != std::string::npos) {
                std::string valueStr = arg.substr(arg.find('=') + 1);
                try {
                    double value = std::stod(valueStr);
                    if (value <= 0.0) {
                        result.addError("--timestep value must be positive, got: " + valueStr);
                    }
                } catch (const std::exception& e) {
                    result.addError("Invalid --timestep value: " + valueStr);
                }
            } else if (i + 1 < argc) {
                try {
                    double value = std::stod(argv[++i]);
                    if (value <= 0.0) {
                        result.addError("--timestep value must be positive, got: " + std::to_string(value));
                    }
                } catch (const std::exception& e) {
                    result.addError("Invalid --timestep value");
                }
            }
        } else if (arg.substr(0, 14) == "--random-seed") {
            if (arg.find('=') != std::string::npos) {
                std::string valueStr = arg.substr(arg.find('=') + 1);
                try {
                    int64_t value = std::stoll(valueStr);
                } catch (const std::exception& e) {
                    result.addError("Invalid --random-seed value: " + valueStr);
                }
            } else if (i + 1 < argc) {
                try {
                    int64_t value = std::stoll(argv[++i]);
                } catch (const std::exception& e) {
                    result.addError("Invalid --random-seed value");
                }
            }
        }
    }
    
    return result;
}

// Validate Phase 6 specific configuration completeness
ValidationResult validatePhase6Configuration(const Config& config) {
    ValidationResult result;
    
    // Check for essential Phase 6 system integration
    bool hasMemorySystems = config.has("working_memory.capacity") && config.has("episodic_memory.max_episodes");
    bool hasNeuromodulation = config.has("dopamine.scale") && config.has("curiosity.adaptation_rate");
    bool hasPrediction = config.has("prediction.horizon") && config.has("prediction.confidence_threshold");
    bool hasDevelopment = config.has("development.initial_plasticity") && config.has("development.maturity_age");
    
    if (!hasMemorySystems) {
        result.addWarning("Memory system configuration incomplete - consider adding working_memory.capacity and episodic_memory.max_episodes");
    }
    
    if (!hasNeuromodulation) {
        result.addWarning("Neuromodulation system configuration incomplete - consider adding dopamine.scale and curiosity.adaptation_rate");
    }
    
    if (!hasPrediction) {
        result.addWarning("Prediction system configuration incomplete - consider adding prediction.horizon and prediction.confidence_threshold");
    }
    
    if (!hasDevelopment) {
        result.addWarning("Development system configuration incomplete - consider adding development.initial_plasticity and development.maturity_age");
    }
    
    // Log Phase 6 integration status
    NLM_LOG_INFO("Phase 6 System Integration Check:");
    NLM_LOG_INFO("  Memory Systems: " + std::string(hasMemorySystems ? "CONFIGURED" : "INCOMPLETE"));
    NLM_LOG_INFO("  Neuromodulation: " + std::string(hasNeuromodulation ? "CONFIGURED" : "INCOMPLETE"));
    NLM_LOG_INFO("  Prediction: " + std::string(hasPrediction ? "CONFIGURED" : "INCOMPLETE"));
    NLM_LOG_INFO("  Development: " + std::string(hasDevelopment ? "CONFIGURED" : "INCOMPLETE"));
    
    if (hasMemorySystems && hasNeuromodulation && hasPrediction && hasDevelopment) {
        NLM_LOG_INFO("  ✓ All Phase 6 systems properly configured");
    } else {
        NLM_LOG_WARNING("  ⚠ Some Phase 6 systems may need additional configuration");
    }
    
    return result;
}

} // namespace config
} // namespace nlm
