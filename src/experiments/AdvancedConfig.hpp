#pragma once

/**
 * Advanced Configuration System for NLM Power Users
 * 
 * This module provides enhanced configuration capabilities for advanced users,
 * including validation, schema definition, and advanced parameter handling.
 */

#include "../core/Config/Config.hpp"
#include <string>
#include <map>
#include <functional>
#include <unordered_map>
#include <vector>
#include <variant>

namespace nlm {

/**
 * Validation error types for configuration validation
 */
enum class ValidationErrorType {
    NoError,
    InvalidType,
    MissingRequired,
    OutOfRange,
    InvalidFormat,
    Custom
};

/**
 * Configuration validation error
 */
struct ValidationError {
    ValidationErrorType type;
    std::string key;
    std::string message;
    std::string value;
    
    ValidationError(ValidationErrorType t, const std::string& k, const std::string& msg, const std::string& val = "")
        : type(t), key(k), message(msg), value(val) {}
};

/**
 * Configuration schema definition
 */
class ConfigSchema {
public:
    struct ParameterInfo {
        std::string name;
        std::string type;  // "int", "float", "bool", "string", "vec3", "vec4"
        std::string description;
        std::string unit;  // "ms", "Hz", "nF", "mV", etc.
        bool required;
        ConfigValue defaultValue;
        
        // Validation parameters
        std::optional<double> min;
        std::optional<double> max;
        std::vector<std::string> allowedValues;
        
        // Range validation for numeric types
        double minValue = std::numeric_limits<double>::lowest();
        double maxValue = std::numeric_limits<double>::max();
        
        ParameterInfo() : required(false), min(std::nullopt), max(std::nullopt) {}
    };
    
    std::unordered_map<std::string, ParameterInfo> parameters;
    
    /**
     * Add a parameter definition to the schema
     */
    void addParameter(const std::string& name, const ParameterInfo& info) {
        parameters[name] = info;
    }
    
    /**
     * Validate configuration against the schema
     */
    std::vector<ValidationError> validate(const Config& config) const {
        std::vector<ValidationError> errors;
        
        for (const auto& [name, info] : parameters) {
            if (info.required && !config.has(name)) {
                errors.emplace_back(ValidationErrorType::MissingRequired, name,
                    "Required parameter missing", "");
            }
            
            if (config.has(name)) {
                auto valueOpt = config.get<std::string>(name);
                if (!valueOpt) {
                    errors.emplace_back(ValidationErrorType::InvalidType, name,
                        "Parameter type mismatch", "");
                    continue;
                }
                
                // Additional validation based on type
                if (info.type == "float" || info.type == "double") {
                    try {
                        double val = std::stod(valueOpt.value());
                        if (info.min && val < info.min.value()) {
                            errors.emplace_back(ValidationErrorType::OutOfRange, name,
                                "Value below minimum", valueOpt.value());
                        }
                        if (info.max && val > info.max.value()) {
                            errors.emplace_back(ValidationErrorType::OutOfRange, name,
                                "Value above maximum", valueOpt.value());
                        }
                    } catch (const std::exception&) {
                        errors.emplace_back(ValidationErrorType::InvalidFormat, name,
                            "Invalid numeric format", valueOpt.value());
                    }
                } else if (info.allowedValues.size() > 0) {
                    if (std::find(info.allowedValues.begin(), info.allowedValues.end(),
                                  valueOpt.value()) == info.allowedValues.end()) {
                        std::string allowedList = "";
                        for (size_t i = 0; i < info.allowedValues.size(); ++i) {
                            if (i > 0) allowedList += ", ";
                            allowedList += info.allowedValues[i];
                        }
                        errors.emplace_back(ValidationErrorType::InvalidFormat, name,
                            "Value not in allowed list. Allowed: " + allowedList, valueOpt.value());
                    }
                }
            }
        }
        
        return errors;
    }
    
    /**
     * Get parameter info for a given name
     */
    const ParameterInfo* getParameterInfo(const std::string& name) const {
        auto it = parameters.find(name);
        if (it != parameters.end()) {
            return &it->second;
        }
        return nullptr;
    }
};

/**
 * Advanced configuration manager with validation and schema support
 */
class AdvancedConfig {
public:
    AdvancedConfig() = default;
    
    /**
     * Set up schema for validation
     */
    void setSchema(const ConfigSchema& schema) {
        currentSchema_ = schema;
    }
    
    /**
     * Load configuration from file with validation
     */
    bool loadFromFile(const std::string& filepath, std::vector<ValidationError>& errors) {
        if (!config_.loadFromFile(filepath)) {
            errors.emplace_back(ValidationErrorType::InvalidFormat, "",
                "Failed to load configuration file", filepath);
            return false;
        }
        
        // Validate if schema exists
        if (currentSchema_.parameters.size() > 0) {
            errors = currentSchema_.validate(config_);
            if (!errors.empty()) {
                return false;
            }
        }
        
        return true;
    }
    
    /**
     * Get the underlying config object
     */
    const Config& getConfig() const {
        return config_;
    }
    
    /**
     * Get the underlying config object (mutable)
     */
    Config& getConfig() {
        return config_;
    }
    
    /**
     * Add preset configuration
     */
    void addPreset(const std::string& name, const ConfigSchema::ParameterInfo& params) {
        presets_[name] = params;
    }
    
    /**
     * Apply a preset to the configuration
     */
    bool applyPreset(const std::string& name) {
        auto it = presets_.find(name);
        if (it == presets_.end()) {
            return false;
        }
        
        const auto& params = it->second;
        for (const auto& [key, info] : params) {
            // Apply preset values (simplified - in practice would use proper type conversion)
            if (info.required) {
                // Set default or required values
                if (info.defaultValue.index() != static_cast<size_t>(ConfigValueType::None)) {
                    config_.set(key, info.defaultValue, ConfigSource::Runtime);
                }
            }
        }
        
        return true;
    }
    
    /**
     * Export configuration to JSON format
     */
    std::string exportToJSON() const {
        std::string json = "{\n";
        bool first = true;
        
        for (const auto& entry : config_.getKeys()) {
            if (!first) json += ",\n";
            first = false;
            
            json += "  \"" + entry + "\": \"";
            auto value = config_.get<std::string>(entry);
            if (value) {
                // Escape quotes in value
                std::string escaped = *value;
                size_t pos = 0;
                while ((pos = escaped.find('"', pos)) != std::string::npos) {
                    escaped.replace(pos, 1, "\\\"");
                    pos += 2;
                }
                json += escaped;
            }
            json += "\"";
        }
        
        json += "\n}";
        return json;
    }
    
    /**
     * Generate configuration report with descriptions
     */
    std::string generateReport() const {
        std::string report = "=== NLM Advanced Configuration Report ===\n\n";
        
        for (const auto& entry : config_.getKeys()) {
            report += "Key: " + entry + "\n";
            auto value = config_.get<std::string>(entry);
            if (value) {
                report += "  Value: " + *value + "\n";
            }
            
            // Add schema description if available
            if (currentSchema_.parameters.find(entry) != currentSchema_.parameters.end()) {
                const auto& param = currentSchema_.parameters.at(entry);
                report += "  Type: " + param.type + "\n";
                if (!param.unit.empty()) {
                    report += "  Unit: " + param.unit + "\n";
                }
                if (!param.description.empty()) {
                    report += "  Description: " + param.description + "\n";
                }
                if (param.required) {
                    report += "  Required: Yes\n";
                }
            }
            
            report += "\n";
        }
        
        return report;
    }

private:
    Config config_;
    ConfigSchema currentSchema_;
    std::unordered_map<std::string, std::unordered_map<std::string, ConfigSchema::ParameterInfo>> presets_;
};

/**
 * Enhanced visualization interface for advanced users
 */
class AdvancedVisualization {
public:
    AdvancedVisualization() = default;
    
    /**
     * Generate detailed brain state visualization
     */
    std::string generateBrainStateVisualization(const Brain* brain) const {
        if (!brain) {
            return "Error: Brain pointer is null";
        }
        
        std::string output;
        output += "╔═══════════════════════════════════════════════════════════════╗\n";
        output += "║                    NLM BRAIN STATE VISUALIZATION                     ║\n";
        output += "╚═══════════════════════════════════════════════════════════════╝\n\n";
        
        // Basic brain metrics
        output += "║ BASIC METRICS:";
        output += "\n║───────────────────────────────────────────────────────────────║\n";
        output += "║ Total Neurons: " + std::to_string(brain->getTotalNeuronCount()) + "\n";
        output += "║ Total Synapses: " + std::to_string(brain->getTotalSynapseCount()) + "\n";
        output += "║ Active Neurons: " + std::to_string(brain->getActiveNeuronCount()) + "\n";
        output += "║ Firing Neurons: " + std::to_string(brain->getFiringNeuronCount()) + "\n";
        output += "║ Average Firing Rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz\n\n";
        
        // Memory systems
        output += "║ MEMORY SYSTEMS:";
        output += "\n║───────────────────────────────────────────────────────────────║\n";
        
        auto* wm = brain->getWorkingMemory();
        if (wm) {
            output += "║ Working Memory: ACTIVE (" + std::to_string(wm->getActiveTraces()) + " traces)\n";
        } else {
            output += "║ Working Memory: NOT CONNECTED\n";
        }
        
        auto* em = brain->getEpisodicMemory();
        if (em) {
            output += "║ Episodic Memory: ACTIVE (" + std::to_string(em->getEpisodeCount()) + " episodes)\n";
        } else {
            output += "║ Episodic Memory: NOT CONNECTED\n";
        }
        
        auto* am = brain->getAssociativeMemory();
        if (am) {
            output += "║ Associative Memory: ACTIVE\n";
        } else {
            output += "║ Associative Memory: NOT CONNECTED\n";
        }
        
        // Neuromodulation
        output += "║ NEUROMODULATION:";
        output += "\n║───────────────────────────────────────────────────────────────║\n";
        
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            output += "║ Dopamine: ACTIVE (level: " + std::to_string(dopamine->getLevel()) + ")\n";
        } else {
            output += "║ Dopamine: NOT CONNECTED\n";
        }
        
        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            output += "║ Curiosity: ACTIVE (level: " + std::to_string(curiosity->getLevel()) + ")\n";
        } else {
            output += "║ Curiosity: NOT CONNECTED\n";
        }
        
        auto* novelty = brain->getNovelty();
        if (novelty) {
            output += "║ Novelty: ACTIVE (level: " + std::to_string(novelty->getLevel()) + ")\n";
        } else {
            output += "║ Novelty: NOT CONNECTED\n";
        }
        
        // Development
        output += "║ DEVELOPMENT:";
        output += "\n║───────────────────────────────────────────────────────────────║\n";
        
        auto* dev = brain->getDevelopmentSystem();
        if (dev) {
            output += "║ Development System: ACTIVE (stage: " + std::to_string(static_cast<int>(dev->getDevelopmentalStage())) + ")\n";
            output += "║ Plasticity Modifier: " + std::to_string(dev->getPlasticityModifier()) + "\n";
        } else {
            output += "║ Development System: NOT CONNECTED\n";
        }
        
        // Prediction
        output += "║ PREDICTION:";
        output += "\n║───────────────────────────────────────────────────────────────║\n";
        
        auto* prediction = brain->getPredictionSystem();
        if (prediction) {
            output += "║ Prediction System: ACTIVE\n";
        } else {
            output += "║ Prediction System: NOT CONNECTED\n";
        }
        
        // Cognition
        output += "║ COGNITION:";
        output += "\n║───────────────────────────────────────────────────────────────║\n";
        
        auto* planner = brain->getPlanner();
        if (planner) {
            output += "║ Neural Planner: ACTIVE\n";
        } else {
            output += "║ Neural Planner: NOT CONNECTED\n";
        }
        
        auto* concept = brain->getConceptFormation();
        if (concept) {
            output += "║ Concept Formation: ACTIVE\n";
        } else {
            output += "║ Concept Formation: NOT CONNECTED\n";
        }
        
        auto* attention = brain->getAttention();
        if (attention) {
            output += "║ Attentional Selection: ACTIVE\n";
        } else {
            output += "║ Attentional Selection: NOT CONNECTED\n";
        }
        
        // Integration status
        output += "║ INTEGRATION STATUS:";
        output += "\n║───────────────────────────────────────────────────────────────║\n";
        
        bool memoryIntegrated = (wm != nullptr) && (em != nullptr);
        bool neuromodIntegrated = (dopamine != nullptr) && (curiosity != nullptr) && (novelty != nullptr);
        bool cognitionIntegrated = (planner != nullptr) && (concept != nullptr) && (attention != nullptr);
        bool predictionIntegrated = (prediction != nullptr);
        bool developmentIntegrated = (dev != nullptr);
        
        output += "║ Memory Systems: " + std::string(memoryIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") + "\n";
        output += "║ Neuromodulation: " + std::string(neuromodIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") + "\n";
        output += "║ Cognition: " + std::string(cognitionIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") + "\n";
        output += "║ Prediction: " + std::string(predictionIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") + "\n";
        output += "║ Development: " + std::string(developmentIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") + "\n\n";
        
        output += "╚═══════════════════════════════════════════════════════════════╝\n";
        
        return output;
    }
    
    /**
     * Generate performance metrics visualization
     */
    std::string generatePerformanceVisualization(const Brain* brain, double timestep) const {
        if (!brain) {
            return "Error: Brain pointer is null";
        }
        
        std::string output;
        output += "╔═══════════════════════════════════════════════════════════════╗\n";
        output += "║                 NLM PERFORMANCE METRICS VISUALIZATION                ║\n";
        output += "╚═══════════════════════════════════════════════════════════════╝\n\n";
        
        // Time metrics
        output += "║ TIME METRICS:";
        output += "\n║───────────────────────────────────────────────────────────────║\n";
        output += "║ Current Step: " + std::to_string(brain->getCurrentStep()) + "\n";
        output += "║ Current Time: " + std::to_string(brain->getCurrentTime()) + " s\n";
        output += "║ Timestep: " + std::to_string(timestep) + " s\n";
        output += "║ Simulation Speed: " + std::to_string(1.0 / timestep) + " Hz\n\n";
        
        // Neural activity metrics
        output += "║ NEURAL ACTIVITY:";
        output += "\n║───────────────────────────────────────────────────────────────║\n";
        output += "║ Total Spikes: " + std::to_string(brain->getTotalSpikeCount()) + "\n";
        output += "║ Average Firing Rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz\n";
        output += "║ Active Neuron Ratio: " + std::to_string(
            static_cast<double>(brain->getActiveNeuronCount()) / std::max(brain->getTotalNeuronCount(), 1ULL) * 100.0) + " %\n";
        output += "║ Firing Neuron Ratio: " + std::to_string(
            static_cast<double>(brain->getFiringNeuronCount()) / std::max(brain->getTotalNeuronCount(), 1ULL) * 100.0) + " %\n\n";
        
        // Synapse metrics
        output += "║ SYNAPTIC METRICS:";
        output += "\n║───────────────────────────────────────────────────────────────║\n";
        output += "║ Total Synapses: " + std::to_string(brain->getTotalSynapseCount()) + "\n";
        output += "║ Average Synaptic Weight: " + std::to_string(brain->getAverageSynapticWeight()) + "\n";
        output += "║ Synaptic Density: " + std::to_string(brain->getSynapticDensity() * 100.0) + " %\n\n";
        
        // Memory system metrics
        output += "║ MEMORY SYSTEM METRICS:";
        output += "\n║───────────────────────────────────────────────────────────────║\n";
        
        auto* wm = brain->getWorkingMemory();
        if (wm) {
            output += "║ Working Memory: " + std::to_string(wm->getActiveTraces()) + " active traces\n";
        }
        
        auto* em = brain->getEpisodicMemory();
        if (em) {
            output += "║ Episodic Memory: " + std::to_string(em->getEpisodeCount()) + " episodes stored\n";
        }
        
        auto* am = brain->getAssociativeMemory();
        if (am) {
            output += "║ Associative Memory: active\n";
        }
        
        // Neuromodulation metrics
        output += "║ NEUROMODULATION METRICS:";
        output += "\n║───────────────────────────────────────────────────────────────║\n";
        
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            output += "║ Dopamine Level: " + std::to_string(dopamine->getLevel()) + "\n";
        }
        
        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            output += "║ Curiosity Level: " + std::to_string(curiosity->getLevel()) + "\n";
        }
        
        auto* novelty = brain->getNovelty();
        if (novelty) {
            output += "║ Novelty Level: " + std::to_string(novelty->getLevel()) + "\n";
        }
        
        // Development metrics
        output += "║ DEVELOPMENT METRICS:";
        output += "\n║───────────────────────────────────────────────────────────────║\n";
        
        auto* dev = brain->getDevelopmentSystem();
        if (dev) {
            output += "║ Developmental Stage: " + std::to_string(static_cast<int>(dev->getDevelopmentalStage())) + "\n";
            output += "║ Plasticity Modifier: " + std::to_string(dev->getPlasticityModifier()) + "\n";
        }
        
        output += "╚═══════════════════════════════════════════════════════════════╝\n";
        
        return output;
    }

private:
    // Helper function for numeric limits
    double numeric_limits_min() const { return -1e100; }
    double numeric_limits_max() const { return 1e100; }
};

} // namespace nlm