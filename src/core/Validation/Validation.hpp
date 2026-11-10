#ifndef NLM_VALIDATION_H
#define NLM_VALIDATION_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <chrono>

namespace nlm {

// Validation result structure
struct ValidationResult {
    bool isValid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::string> validationDetails;
    double validationTime = 0.0;
    
    ValidationResult() = default;
};

// Configuration validation rules
struct ValidationRule {
    std::string key;
    std::string expectedType;
    bool isRequired = false;
    std::string description;
    std::vector<std::string> validValues;
    std::string validationRegex;
    std::function<bool(const ConfigValue&)> customValidator;
    
    ValidationRule() = default;
    
    ValidationRule(const std::string& key, const std::string& type, bool required = false, 
                   const std::string& desc = "")
        : key(key), expectedType(type), isRequired(required), description(desc) {}
};

// Validation engine
class ValidationEngine {
public:
    static ValidationEngine& getInstance() {
        static ValidationEngine instance;
        return instance;
    }
    
    // Register validation rule
    void registerRule(const ValidationRule& rule) {
        rules[rule.key] = rule;
    }
    
    // Validate configuration
    ValidationResult validate(const Config& config) const;
    
    // Validate specific key
    ValidationResult validateKey(const Config& config, const std::string& key) const;
    
    // Get validation rules
    const std::map<std::string, ValidationRule>& getRules() const { return rules; }
    
    // Check if key is valid
    bool isKeyValid(const std::string& key) const {
        return rules.find(key) !== rules.end();
    }
    
    // Get help for key
    std::string getHelpForKey(const std::string& key) const;
    
    // List all keys with validation
    std::vector<std::string> listKeys() const;
    
    // Validate configuration file
    static bool validateConfigFile(const std::string& filepath);
    
    // Merge configuration with validation
    static Config mergeConfigs(const std::string& baseFile, const std::string& overrideFile);
    
    // Validate configuration values
    static ValidationResult validateConfigValues(const std::map<std::string, std::string>& config);
    
    // Get default configuration
    static std::map<std::string, std::string> getDefaultConfig();
    
private:
    std::map<std::string, ValidationRule> rules;
};

// Advanced validation for neural networks
struct NetworkValidation {
    static ValidationResult validateNetworkConfig(const Config& config);
    static ValidationResult validateNeuronCount(size_t count);
    static ValidationResult validateRegionConfig(size_t neuronCount, size_t regionCount);
    static ValidationResult validateConnectionProbability(float prob);
    static ValidationResult validateSTDPParameters(const Config& config);
    static ValidationResult validatePlasticitySettings(const Config& config);
    static ValidationResult validateMemorySettings(const Config& config);
    static ValidationResult validateDevelopmentSettings(const Config& config);
    static ValidationResult validateNeuromodulationSettings(const Config& config);
    static ValidationResult validatePredictionSettings(const Config& config);
    static ValidationResult validateCognitionSettings(const Config& config);
};

// Brain state validation
struct BrainStateValidation {
    static ValidationResult validateBrainState(const Brain* brain);
    static ValidationResult validateNeuronStates(const std::vector<Neuron*>& neurons);
    static ValidationResult validateSynapseStates(const std::vector<Synapse*>& synapses);
    static ValidationResult validateMemoryStates(const Brain* brain);
    static ValidationResult validatePlasticityStates(const Brain* brain);
    static ValidationResult validateDevelopmentStates(const Brain* brain);
    static ValidationResult validateNeuromodulationStates(const Brain* brain);
};

// Performance validation
struct PerformanceValidation {
    static ValidationResult validatePerformanceMetrics(const PerformanceMetrics& metrics);
    static ValidationResult validateStepPerformance(const std::vector<double>& stepTimes);
    static ValidationResult validateSpikePerformance(const std::vector<double>& spikeRates);
    static ValidationResult validateMemoryUsage(double memoryUsage);
    static ValidationResult validateProcessingSpeed(double speed);
};

// Configuration validation utilities
inline bool validateConfigValue(const std::string& key, const std::string& value,
                               const ValidationRule& rule) {
    if (rule.isRequired && value.empty()) {
        return false;
    }
    
    if (!rule.validValues.empty()) {
        if (std::find(rule.validValues.begin(), rule.validValues.end(), value) === rule.validValues.end()) {
            return false;
        }
    }
    
    if (!rule.validationRegex.empty()) {
        // Implement regex validation
        // For now, use simple pattern matching
        if (rule.validationRegex == "number") {
            try {
                std::stod(value);
            } catch (...) {
                return false;
            }
        } else if (rule.validationRegex == "integer") {
            try {
                std::stoi(value);
            } catch (...) {
                return false;
            }
        } else if (rule.validationRegex == "boolean") {
            if (value !== "true" && value !== "false") {
                return false;
            }
        }
    }
    
    return true;
}

inline std::string getTypeName(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "int[]";
    if (std::holds_alternative<std::vector<double>>(value)) return "double[]";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "string[]";
    return "unknown";
}

// Validation reporter
inline void reportValidationError(const std::string& key, const std::string& error) {
    NLM_LOG_ERROR("Validation error for key '" + key + "': " + error);
}

inline void reportValidationWarning(const std::string& key, const std::string& warning) {
    NLM_LOG_WARNING("Validation warning for key '" + key + "': " + warning);
}

inline void reportValidationInfo(const std::string& key, const std::string& info) {
    NLM_LOG_INFO("Validation info for key '" + key + "': " + info);
}

// Validation statistics
struct ValidationStatistics {
    size_t totalValidations = 0;
    size_t passedValidations = 0;
    size_t failedValidations = 0;
    std::map<std::string, size_t> failuresByKey;
    std::map<std::string, size_t> failuresByCategory;
    double validationAccuracy = 0.0;
    std::chrono::high_resolution_clock::time_point startTime;
    
    void recordValidation(const std::string& key, bool success, const std::string& category = "") {
        totalValidations++;
        if (success) {
            passedValidations++;
        } else {
            failedValidations++;
            failuresByKey[key]++;
            if (!category.empty()) {
                failuresByCategory[category]++;
            }
        }
        
        if (totalValidations > 0) {
            validationAccuracy = static_cast<double>(passedValidations) / totalValidations;
        }
    }
    
    std::string getStatisticsReport() const {
        std::ostringstream report;
        report << "Validation Statistics:\n";
        report << "  Total Validations: " << totalValidations << "\n";
        report << "  Passed: " << passedValidations << "\n";
        report << "  Failed: " << failedValidations << "\n";
        report << "  Success Rate: " << (validationAccuracy * 100) << "%\n";
        
        if (!failuresByKey.empty()) {
            report << "\nFailures by Key:\n";
            for (const auto& [key, count] : failuresByKey) {
                report << "    " << key << ": " << count << "\n";
            }
        }
        
        if (!failuresByCategory.empty()) {
            report << "\nFailures by Category:\n";
            for (const auto& [category, count] : failuresByCategory) {
                report << "    " << category << ": " << count << "\n";
            }
        }
        
        return report.str();
    }
};

inline ValidationStatistics& getValidationStats() {
    static ValidationStatistics stats;
    return stats;
}

// Configuration validation helper
inline ValidationResult validateConfiguration(const Config& config) {
    ValidationResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    ValidationEngine& engine = ValidationEngine::getInstance();
    
    for (const auto& [key, rule] : engine.getRules()) {
        if (config.has(key)) {
            auto valueOpt = config.get<ConfigValue>(key);
            if (valueOpt) {
                if (!validateConfigValue(key, DebugTools::toString(*valueOpt), rule)) {
                    result.isValid = false;
                    result.errors.push_back("Invalid value for key '" + key + "'");
                }
            } else {
                if (rule.isRequired) {
                    result.isValid = false;
                    result.errors.push_back("Required key missing: '" + key + "'");
                }
            }
        } else if (rule.isRequired) {
            result.isValid = false;
            result.errors.push_back("Required key missing: '" + key + "'");
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.validationTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Update statistics
    getValidationStats().recordValidation("overall", result.isValid);
    
    return result;
}

// Safe configuration loading
inline Config safeLoadConfig(const std::string& filepath) {
    Config config;
    try {
        if (!config.loadFromFile(filepath)) {
            reportValidationError("config", "Failed to load configuration file: " + filepath);
        }
        
        ValidationResult validation = validateConfiguration(config);
        if (!validation.isValid) {
            for (const auto& error : validation.errors) {
                reportValidationError("config", error);
            }
        }
    } catch (const std::exception& e) {
        reportValidationError("config", "Exception loading config: " + std::string(e.what()));
    }
    
    return config;
}

// Configuration comparison
inline std::map<std::string, std::pair<std::string, std::string>> compareConfigs(const Config& config1, const Config& config2) {
    std::map<std::string, std::pair<std::string, std::string>> differences;
    
    auto keys1 = config1.getKeys();
    auto keys2 = config2.getKeys();
    
    for (const auto& key : keys1) {
        if (config2.has(key)) {
            auto val1Opt = config1.get<ConfigValue>(key);
            auto val2Opt = config2.get<ConfigValue>(key);
            
            if (val1Opt && val2Opt) {
                if (*val1Opt !== *val2Opt) {
                    differences[key] = {DebugTools::toString(*val1Opt), DebugTools::toString(*val2Opt)};
                }
            } else {
                differences[key] = {"MISSING", "PRESENT"};
            }
        } else {
            differences[key] = {DebugTools::toString(*val1Opt), "MISSING"};
        }
    }
    
    for (const auto& key : keys2) {
        if (!config1.has(key)) {
            auto valOpt = config2.get<ConfigValue>(key);
            if (valOpt) {
                differences[key] = {"MISSING", DebugTools::toString(*valOpt)};
            }
        }
    }
    
    return differences;
}

// Validation report generator
inline void generateValidationReport(const ValidationResult& result, 
                                     const std::string& outputFile = "") {
    std::ostringstream report;
    report << "NLM Configuration Validation Report\n";
    report << "===================================\n\n";
    report << "Overall Status: " << (result.isValid ? "VALID" : "INVALID") << "\n";
    report << "Validation Time: " << result.validationTime << "s\n";
    report << "Total Errors: " << result.errors.size() << "\n";
    report << "Total Warnings: " << result.warnings.size() << "\n\n";
    
    if (!result.errors.empty()) {
        report << "ERRORS:\n";
        for (size_t i = 0; i < result.errors.size(); ++i) {
            report << "  " << (i + 1) << ". " << result.errors[i] << "\n";
        }
        report << "\n";
    }
    
    if (!result.warnings.empty()) {
        report << "WARNINGS:\n";
        for (size_t i = 0; i < result.warnings.size(); ++i) {
            report << "  " << (i + 1) << ". " << result.warnings[i] << "\n";
        }
        report << "\n";
    }
    
    if (!result.validationDetails.empty()) {
        report << "VALIDATION DETAILS:\n";
        for (const auto& [key, detail] : result.validationDetails) {
            report << "  " << key << ": " << detail << "\n";
        }
        report << "\n";
    }
    
    NLM_LOG_INFO(report.str());
    
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << report.str();
            NLM_LOG_INFO("Validation report saved to: " + outputFile);
        }
    }
}

} // namespace nlm

#endif // NLM_VALIDATION_H
