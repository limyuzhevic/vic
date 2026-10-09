#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <algorithm>
#include <chrono>
#include <format>
#include <set>
#include <unordered_map>
#include <variant>
#include "ErrorHandling.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Validation result structure
struct ValidationResult {
    bool success;
    std::vector<std::shared_ptr<NLMError>> errors;
    std::map<std::string, std::string> warnings;
    std::map<std::string, std::any> validatedValues;
    
    ValidationResult() : success(true) {}
    
    void addError(std::shared_ptr<NLMError> error) {
        errors.push_back(error);
        success = false;
    }
    
    void addWarning(const std::string& key, const std::string& message) {
        warnings[key] = message;
    }
    
    void addValue(const std::string& key, const std::any& value) {
        validatedValues[key] = value;
    }
    
    template<typename T>
    bool getValue(const std::string& key, T& value) const {
        auto it = validatedValues.find(key);
        if (it != validatedValues.end()) {
            try {
                value = std::any_cast<T>(it->second);
                return true;
            } catch (const std::bad_any_cast&) {}
        }
        return false;
    }
    
    std::string summary() const {
        std::ostringstream oss;
        oss << "Validation Result: " << (success ? "PASS" : "FAIL") << std::endl;
        oss << "Errors: " << errors.size() << std::endl;
        oss << "Warnings: " << warnings.size() << std::endl;
        for (const auto& error : errors) {
            oss << "  - " << error->what() << std::endl;
        }
        for (const auto& warning : warnings) {
            oss << "  - Warning: " << warning.second << std::endl;
        }
        return oss.str();
    }
};

// Validation rules
class ValidationRule {
public:
    virtual ~ValidationRule() = default;
    virtual bool validate(const std::any& value, std::string& errorMessage) const = 0;
    virtual std::string getRuleDescription() const = 0;
};

// Range validation rule
class RangeValidationRule : public ValidationRule {
private:
    double min_;
    double max_;
    bool inclusive_ = true;
    
public:
    RangeValidationRule(double min, double max, bool inclusive = true)
        : min_(min), max_(max), inclusive_(inclusive) {}
    
    bool validate(const std::any& value, std::string& errorMessage) const override {
        try {
            double val = std::any_cast<double>(value);
            if (inclusive_) {
                if (val < min_ || val > max_) {
                    errorMessage = std::format("Value {} must be between {} and {}", val, min_, max_);
                    return false;
                }
            } else {
                if (val <= min_ || val >= max_) {
                    errorMessage = std::format("Value {} must be strictly between {} and {}", val, min_, max_);
                    return false;
                }
            }
            return true;
        } catch (const std::bad_any_cast&) {
            errorMessage = "Value is not a number";
            return false;
        }
    }
    
    std::string getRuleDescription() const override {
        return std::format("Range validation: [{}...{}]", min_, max_);
    }
};

// String format validation rule
class StringFormatValidationRule : public ValidationRule {
private:
    std::function<bool(const std::string&)> validator_;
    std::string formatDescription_;
    
public:
    StringFormatValidationRule(std::function<bool(const std::string&)> validator,
        const std::string& formatDescription)
        : validator_(validator), formatDescription_(formatDescription) {}
    
    bool validate(const std::any& value, std::string& errorMessage) const override {
        try {
            std::string val = std::any_cast<std::string>(value);
            if (!validator_(val)) {
                errorMessage = std::format("String format validation failed: {}", formatDescription_);
                return false;
            }
            return true;
        } catch (const std::bad_any_cast&) {
            errorMessage = "Value is not a string";
            return false;
        }
    }
    
    std::string getRuleDescription() const override {
        return std::format("String format validation: {}", formatDescription_);
    }
};

// Enum validation rule
class EnumValidationRule : public ValidationRule {
private:
    std::set<std::string> validValues_;
    
public:
    EnumValidationRule(const std::set<std::string>& validValues)
        : validValues_(validValues) {}
    
    bool validate(const std::any& value, std::string& errorMessage) const override {
        try {
            std::string val = std::any_cast<std::string>(value);
            if (validValues_.find(val) == validValues_.end()) {
                errorMessage = std::format("Value '{}' is not one of the valid options: {}",
                    val, std::accumulate(validValues_.begin(), validValues_.end(), std::string(),
                        [](const std::string& acc, const std::string& v) {
                            return acc.empty() ? v : acc + ", " + v;
                        }));
                return false;
            }
            return true;
        } catch (const std::bad_any_cast&) {
            errorMessage = "Value is not a string";
            return false;
        }
    }
    
    std::string getRuleDescription() const override {
        return std::format("Enum validation: valid values are {}",
            std::accumulate(validValues_.begin(), validValues_.end(), std::string(),
                [](const std::string& acc, const std::string& v) {
                    return acc.empty() ? v : acc + ", " + v;
                }));
    }
};

// Custom validation rule
class CustomValidationRule : public ValidationRule {
private:
    std::function<bool(const std::any&)> validator_;
    std::string description_;
    
public:
    CustomValidationRule(std::function<bool(const std::any&)> validator,
        const std::string& description)
        : validator_(validator), description_(description) {}
    
    bool validate(const std::any& value, std::string& errorMessage) const override {
        if (!validator_(value)) {
            errorMessage = std::format("Custom validation failed: {}", description_);
            return false;
        }
        return true;
    }
    
    std::string getRuleDescription() const override {
        return description_;
    }
};

// Validation schema
class ValidationSchema {
private:
    std::map<std::string, std::vector<std::shared_ptr<ValidationRule>>> rules_;
    std::map<std::string, std::any> defaultValues_;
    std::map<std::string, std::string> descriptions_;
    bool required_ = false;
    
public:
    void addRule(const std::string& field, std::shared_ptr<ValidationRule> rule) {
        rules_[field].push_back(rule);
    }
    
    void addDefault(const std::string& field, const std::any& value) {
        defaultValues_[field] = value;
    }
    
    void setDescription(const std::string& field, const std::string& description) {
        descriptions_[field] = description;
    }
    
    void setRequired(bool required) {
        required_ = required;
    }
    
    bool validate(const std::map<std::string, std::any>& data, ValidationResult& result) const {
        bool overallSuccess = true;
        
        for (const auto& pair : rules_) {
            const std::string& field = pair.first;
            auto it = data.find(field);
            
            if (it == data.end()) {
                // Check if field has default value
                auto defaultIt = defaultValues_.find(field);
                if (defaultIt != defaultValues_.end()) {
                    // Use default value
                    result.addValue(field, defaultIt->second);
                    continue;
                } else if (required_) {
                    auto error = ErrorHandling::createValidationError(
                        ErrorCode::ParameterValidationFailed,
                        std::format("Required field '{}' is missing", field));
                    result.addError(error);
                    overallSuccess = false;
                }
                continue;
            }
            
            // Validate field
            bool fieldValid = true;
            for (const auto& rule : pair.second) {
                std::string errorMsg;
                if (!rule->validate(it->second, errorMsg)) {
                    auto error = ErrorHandling::createValidationError(
                        ErrorCode::ValidationError,
                        std::format("Field '{}': {}", field, errorMsg));
                    result.addError(error);
                    fieldValid = false;
                    break;
                }
            }
            
            if (fieldValid) {
                result.addValue(field, it->second);
            }
        }
        
        return overallSuccess;
    }
    
    std::string getSchemaDescription() const {
        std::ostringstream oss;
        oss << "Validation Schema:" << std::endl;
        for (const auto& pair : descriptions_) {
            oss << "  " << pair.first << ": " << pair.second << std::endl;
        }
        return oss.str();
    }
    
    // Get required fields
    std::set<std::string> getRequiredFields() const {
        std::set<std::string> required;
        for (const auto& pair : rules_) {
            auto it = defaultValues_.find(pair.first);
            if (it == defaultValues_.end()) {
                required.insert(pair.first);
            }
        }
        return required;
    }
};

// Configuration validator
class ConfigValidator {
public:
    // Validate configuration parameters
    static ValidationResult validateConfig(const Config& config) {
        ValidationResult result;
        
        // Define validation schemas for different configuration sections
        ValidationSchema brainSchema;
        brainSchema.setDescription("Brain Configuration", "Configuration for brain simulation");
        brainSchema.addRule("neuron_count", std::make_shared<RangeValidationRule>(1, 1000000, true));
        brainSchema.addRule("region_count", std::make_shared<RangeValidationRule>(1, 1000, true));
        brainSchema.addRule("connection_probability", std::make_shared<RangeValidationRule>(0.0, 1.0, true));
        brainSchema.addRule("simulation_timestep", std::make_shared<RangeValidationRule>(1e-6, 1.0, true));
        brainSchema.addRule("stdp_ltp_weight", std::make_shared<RangeValidationRule>(0.0, 1.0, true));
        brainSchema.addRule("stdp_ltd_weight", std::make_shared<RangeValidationRule>(0.0, 1.0, true));
        brainSchema.addRule("stdp_tau", std::make_shared<RangeValidationRule>(1.0, 100.0, true));
        brainSchema.addRule("synaptogenesis_rate", std::make_shared<RangeValidationRule>(0.0, 0.1, true));
        brainSchema.addRule("pruning_rate", std::make_shared<RangeValidationRule>(0.0, 0.01, true));
        
        // Validate brain-specific configuration
        std::map<std::string, std::any> brainConfig;
        auto neuronCount = config.get<size_t>("neuron_count");
        if (neuronCount) {
            brainConfig["neuron_count"] = *neuronCount;
        }
        
        auto regionCount = config.get<size_t>("region_count");
        if (regionCount) {
            brainConfig["region_count"] = *regionCount;
        }
        
        auto connectionProb = config.get<double>("connection_probability");
        if (connectionProb) {
            brainConfig["connection_probability"] = *connectionProb;
        }
        
        auto timestep = config.get<double>("simulation_timestep");
        if (timestep) {
            brainConfig["simulation_timestep"] = *timestep;
        }
        
        auto ltpWeight = config.get<double>("stdp_ltp_weight");
        if (ltpWeight) {
            brainConfig["stdp_ltp_weight"] = *ltpWeight;
        }
        
        auto ltdWeight = config.get<double>("stdp_ltd_weight");
        auto tau = config.get<double>("stdp_tau");
        if (ltdWeight && tau) {
            brainConfig["stdp_ltd_weight"] = *ltdWeight;
            brainConfig["stdp_tau"] = *tau;
        }
        
        auto synaptogenesisRate = config.get<double>("synaptogenesis_rate");
        auto pruningRate = config.get<double>("pruning_rate");
        if (synaptogenesisRate && pruningRate) {
            brainConfig["synaptogenesis_rate"] = *synaptogenesisRate;
            brainConfig["pruning_rate"] = *pruningRate;
        }
        
        if (!brainSchema.validate(brainConfig, result)) {
            result.addWarning("config", "Some configuration parameters may not be optimal for simulation stability");
        }
        
        return result;
    }
    
    // Validate input parameters
    static ValidationResult validateInput(const std::map<std::string, std::any>& input) {
        ValidationResult result;
        
        // Define input validation schema
        ValidationSchema inputSchema;
        inputSchema.setDescription("Input Validation", "Validation for system input parameters");
        
        // Required fields
        std::set<std::string> requiredFields = {"data"};
        for (const auto& field : requiredFields) {
            if (input.find(field) == input.end()) {
                auto error = ErrorHandling::createValidationError(
                    ErrorCode::ParameterValidationFailed,
                    std::format("Required input field '{}' is missing", field));
                result.addError(error);
            }
        }
        
        // Validate data field
        auto dataIt = input.find("data");
        if (dataIt != input.end()) {
            auto& data = dataIt->second;
            if (std::any_cast<std::vector<double>>(data).empty()) {
                auto error = ErrorHandling::createValidationError(
                    ErrorCode::ParameterValidationFailed,
                    "Input data vector is empty");
                result.addError(error);
            }
        }
        
        return result;
    }
    
    // Validate simulation parameters
    static ValidationResult validateSimulation(const Config& config) {
        ValidationResult result;
        
        // Check if simulation can be initialized
        auto neuronCount = config.get<size_t>("neuron_count");
        if (!neuronCount) {
            auto error = ErrorHandling::createConfigurationError(
                ErrorCode::ConfigMissingRequired,
                "neuron_count configuration parameter is required");
            result.addError(error);
        } else if (*neuronCount == 0) {
            auto error = ErrorHandling::createConfigurationError(
                ErrorCode::ConfigValueOutOfRange,
                "neuron_count must be greater than 0");
            result.addError(error);
        }
        
        auto regionCount = config.get<size_t>("region_count");
        if (!regionCount) {
            auto error = ErrorHandling::createConfigurationError(
                ErrorCode::ConfigMissingRequired,
                "region_count configuration parameter is required");
            result.addError(error);
        } else if (*regionCount == 0) {
            auto error = ErrorHandling::createConfigurationError(
                ErrorCode::ConfigValueOutOfRange,
                "region_count must be greater than 0");
            result.addError(error);
        }
        
        auto timestep = config.get<double>("simulation_timestep");
        if (!timestep) {
            auto error = ErrorHandling::createConfigurationError(
                ErrorCode::ConfigMissingRequired,
                "simulation_timestep configuration parameter is required");
            result.addError(error);
        } else if (*timestep <= 0.0) {
            auto error = ErrorHandling::createConfigurationError(
                ErrorCode::ConfigValueOutOfRange,
                "simulation_timestep must be positive");
            result.addError(error);
        } else if (*timestep > 1.0) {
            auto error = ErrorHandling::createWarning(
                ErrorCode::ConfigValueOutOfRange,
                "simulation_timestep is very large, simulation may be unstable");
            result.addWarning("simulation_timestep", "Value is very large, simulation may be unstable");
        }
        
        auto connectionProb = config.get<double>("connection_probability");
        if (!connectionProb) {
            auto error = ErrorHandling::createConfigurationError(
                ErrorCode::ConfigMissingRequired,
                "connection_probability configuration parameter is required");
            result.addError(error);
        } else if (*connectionProb < 0.0 || *connectionProb > 1.0) {
            auto error = ErrorHandling::createConfigurationError(
                ErrorCode::ConfigValueOutOfRange,
                "connection_probability must be between 0.0 and 1.0");
            result.addError(error);
        }
        
        return result;
    }
    
    // Validate memory allocation requirements
    static ValidationResult validateMemoryRequirements(const Config& config) {
        ValidationResult result;
        
        // Calculate estimated memory requirements
        auto neuronCount = config.get<size_t>("neuron_count");
        if (!neuronCount) {
            auto error = ErrorHandling::createConfigurationError(
                ErrorCode::ConfigMissingRequired,
                "neuron_count configuration parameter is required for memory validation");
            result.addError(error);
            return result;
        }
        
        // Estimate memory usage (simplified calculation)
        size_t estimatedMemoryMB = (*neuronCount * 100) / (1024 * 1024); // Rough estimate
        
        if (estimatedMemoryMB > 2048) { // 2GB limit
            auto error = ErrorHandling::createMemoryError(
                ErrorCode::MemoryAllocationFailed,
                std::format("Estimated memory usage ({}) exceeds 2GB limit", estimatedMemoryMB));
            result.addError(error);
        } else if (estimatedMemoryMB > 1024) { // 1GB warning threshold
            result.addWarning("memory", std::format("Estimated memory usage ({}) approaches 1GB limit", estimatedMemoryMB));
        }
        
        return result;
    }
    
    // Validate neuromodulation parameters
    static ValidationResult validateNeuromodulation(const Config& config) {
        ValidationResult result;
        
        // Check neuromodulation parameters
        auto dopamineAlpha = config.get<double>("dopamine_alpha");
        if (dopamineAlpha && (*dopamineAlpha < 0.0 || *dopamineAlpha > 1.0)) {
            auto error = ErrorHandling::createConfigurationError(
                ErrorCode::ConfigValueOutOfRange,
                "dopamine_alpha must be between 0.0 and 1.0");
            result.addError(error);
        }
        
        auto curiosityAlpha = config.get<double>("curiosity_alpha");
        if (curiosityAlpha && (*curiosityAlpha < 0.0 || *curiosityAlpha > 1.0)) {
            auto error = ErrorHandling::createConfigurationError(
                ErrorCode::ConfigValueOutOfRange,
                "curiosity_alpha must be between 0.0 and 1.0");
            result.addError(error);
        }
        
        auto noveltyThreshold = config.get<double>("novelty_threshold");
        if (noveltyThreshold && *noveltyThreshold < 0.0) {
            auto error = ErrorHandling::createConfigurationError(
                ErrorCode::ConfigValueOutOfRange,
                "novelty_threshold must be non-negative");
            result.addError(error);
        }
        
        return result;
    }
    
    // Comprehensive configuration validation
    static ValidationResult validateConfigComprehensive(const Config& config) {
        ValidationResult result;
        
        // Combine all validation results
        auto configResult = validateConfig(config);
        auto simulationResult = validateSimulation(config);
        auto memoryResult = validateMemoryRequirements(config);
        auto neuromodulationResult = validateNeuromodulation(config);
        
        // Merge results
        if (!configResult.success) {
            result.errors.insert(result.errors.end(), configResult.errors.begin(), configResult.errors.end());
            result.warnings.insert(result.warnings.end(), configResult.warnings.begin(), configResult.warnings.end());
        }
        
        if (!simulationResult.success) {
            result.errors.insert(result.errors.end(), simulationResult.errors.begin(), simulationResult.errors.end());
            result.warnings.insert(result.warnings.end(), simulationResult.warnings.begin(), simulationResult.warnings.end());
        }
        
        if (!memoryResult.success) {
            result.errors.insert(result.errors.end(), memoryResult.errors.begin(), memoryResult.errors.end());
            result.warnings.insert(result.warnings.end(), memoryResult.warnings.begin(), memoryResult.warnings.end());
        }
        
        if (!neuromodulationResult.success) {
            result.errors.insert(result.errors.end(), neuromodulationResult.errors.begin(), neuromodulationResult.errors.end());
            result.warnings.insert(result.warnings.end(), neuromodulationResult.warnings.begin(), neuromodulationResult.warnings.end());
        }
        
        // Set overall success
        result.success = result.errors.empty();
        
        return result;
    }
    
    // Validate checkpoint data
    static ValidationResult validateCheckpointData(const std::string& filepath) {
        ValidationResult result;
        
        if (!ErrorHandling::validateFileExists(filepath, "checkpoint validation")) {
            return result;
        }
        
        // Try to open checkpoint file and validate basic structure
        std::ifstream file(filepath);
        if (!file.is_open()) {
            auto error = ErrorHandling::createFileSystemError(
                ErrorCode::FileReadError,
                std::format("Failed to open checkpoint file: {}", filepath));
            result.addError(error);
            return result;
        }
        
        // Check file size
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        if (fileSize == 0) {
            auto error = ErrorHandling::createFileSystemError(
                ErrorCode::FileCorrupted,
                std::format("Checkpoint file is empty: {}", filepath));
            result.addError(error);
            return result;
        }
        
        if (fileSize > 100 * 1024 * 1024) { // 100MB limit
            auto error = ErrorHandling::createFileSystemError(
                ErrorCode::FileTooLarge,
                std::format("Checkpoint file size ({}) exceeds 100MB limit", fileSize));
            result.addError(error);
        }
        
        return result;
    }
    
    // Validate neural network connectivity
    static ValidationResult validateNeuralConnectivity(size_t neuronCount, size_t synapseCount,
        size_t regionCount, double connectionProbability) {
        ValidationResult result;
        
        // Check neuron-region ratio
        double neuronsPerRegion = static_cast<double>(neuronCount) / regionCount;
        if (neuronsPerRegion < 1.0) {
            auto error = ErrorHandling::createSimulationError(
                ErrorCode::InvalidConnection,
                std::format("Not enough neurons per region: {} neurons / {} regions", neuronCount, regionCount));
            result.addError(error);
        }
        
        // Check synapse density
        double synapsesPerNeuron = static_cast<double>(synapseCount) / neuronCount;
        if (synapsesPerNeuron < 0.1) {
            result.addWarning("connectivity", std::format("Low synapse density: {} synapses per neuron", synapsesPerNeuron));
        } else if (synapsesPerNeuron > 10.0) {
            result.addWarning("connectivity", std::format("High synapse density: {} synapses per neuron", synapsesPerNeuron));
        }
        
        // Check expected vs actual connection probability
        double expectedConnections = neuronCount * (neuronCount - 1) * connectionProbability;
        double actualConnectionRatio = static_cast<double>(synapseCount) / expectedConnections;
        if (actualConnectionRatio < 0.5) {
            result.addWarning("connectivity", std::format("Connection probability mismatch: expected {}% of possible connections", connectionProbability * 100));
        }
        
        return result;
    }
};