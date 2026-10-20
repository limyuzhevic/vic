#include "Validation.hpp"
#include <algorithm>
#include <sstream>
#include <cctype>

namespace nlm {

void ConfigSchemaManager::initializeDefaultSchemas() {
    // Neuromodulation settings
    ConfigSchema neuromodulationSchema("Neuromodulation", ConfigCategory::Neuromodulation);
    neuromodulationSchema.addRule(ValidationRule("dopamine_baseline", ConfigDataType::Float,
        "Baseline dopamine level (0.0-1.0)", true, 0.0, 1.0, ValidationMode::Strict));
    neuromodulationSchema.addRule(ValidationRule("reward_discount_factor", ConfigDataType::Float,
        "Reward discount factor (0.0-1.0)", true, 0.0, 1.0, ValidationMode::Strict));
    neuromodulationSchema.addRule(ValidationRule("curiosity", ConfigDataType::Float,
        "Curiosity coefficient (0.0-1.0)", true, 0.0, 1.0, ValidationMode::Strict));
    neuromodulationSchema.addRule(ValidationRule("novelty_sensitivity", ConfigDataType::Float,
        "Novelty sensitivity (0.0-1.0)", true, 0.0, 1.0, ValidationMode::Strict));
    neuromodulationSchema.addRule(ValidationRule("prediction_error_baseline", ConfigDataType::Float,
        "Prediction error baseline (-10.0 to 10.0)", true, -10.0, 10.0, ValidationMode::Lenient));
    registerSchema(neuromodulationSchema);

    // Simulation settings
    ConfigSchema simulationSchema("Simulation", ConfigCategory::Simulation);
    simulationSchema.addRule(ValidationRule("simulation_timestep", ConfigDataType::Float,
        "Simulation timestep in seconds", true, 1e-6, 0.1, ValidationMode::Strict));
    simulationSchema.addRule(ValidationRule("max_simulation_steps", ConfigDataType::Integer,
        "Maximum simulation steps", true, 1.0, 1e9, ValidationMode::Strict));
    simulationSchema.addRule(ValidationRule("simulation_time_limit", ConfigDataType::Float,
        "Simulation time limit in seconds (0 = unlimited)", false, 0.0, 1e6, ValidationMode::Lenient));
    simulationSchema.addRule(ValidationRule("random_seed", ConfigDataType::Integer,
        "Random seed for reproducibility", true, 0.0, 1e9, ValidationMode::Strict));
    registerSchema(simulationSchema);

    // Development settings
    ConfigSchema developmentSchema("Development", ConfigCategory::Development);
    developmentSchema.addRule(ValidationRule("synaptogenesis_rate", ConfigDataType::Float,
        "Synaptogenesis rate per step (0.0-0.1)", true, 0.0, 0.1, ValidationMode::Strict));
    developmentSchema.addRule(ValidationRule("pruning_rate", ConfigDataType::Float,
        "Pruning rate per step (0.0-0.01)", true, 0.0, 0.01, ValidationMode::Strict));
    developmentSchema.addRule(ValidationRule("development_threshold", ConfigDataType::Float,
        "Development threshold (0.0-1.0)", true, 0.0, 1.0, ValidationMode::Lenient));
    developmentSchema.addRule(ValidationRule("critical_period_end", ConfigDataType::Float,
        "End of critical period (0.0-1.0)", false, 0.0, 1.0, ValidationMode::Lenient));
    registerSchema(developmentSchema);

    // Memory settings
    ConfigSchema memorySchema("Memory", ConfigCategory::Memory);
    memorySchema.addRule(ValidationRule("working_memory_capacity", ConfigDataType::Integer,
        "Working memory capacity in items", true, 1.0, 1e6, ValidationMode::Strict));
    memorySchema.addRule(ValidationRule("working_memory_decay_rate", ConfigDataType::Float,
        "Working memory decay rate (0.0-1.0)", true, 0.0, 1.0, ValidationMode::Strict));
    memorySchema.addRule(ValidationRule("episodic_memory_capacity", ConfigDataType::Integer,
        "Episodic memory capacity in episodes", true, 1.0, 1e6, ValidationMode::Strict));
    memorySchema.addRule(ValidationRule("associative_memory_capacity", ConfigDataType::Integer,
        "Associative memory capacity in patterns", true, 1.0, 1e6, ValidationMode::Strict));
    registerSchema(memorySchema);

    // Environment settings
    ConfigSchema environmentSchema("Environment", ConfigCategory::Environment);
    environmentSchema.addRule(ValidationRule("width", ConfigDataType::Integer,
        "Environment width in cells", true, 1.0, 1e4, ValidationMode::Strict));
    environmentSchema.addRule(ValidationRule("height", ConfigDataType::Integer,
        "Environment height in cells", true, 1.0, 1e4, ValidationMode::Strict));
    environmentSchema.addRule(ValidationRule("environment_name", ConfigDataType::String,
        "Name of the environment", true, std::nullopt, std::nullopt, ValidationMode::Strict));
    environmentSchema.addRule(ValidationRule("wall_penalty", ConfigDataType::Float,
        "Penalty for hitting walls", false, -100.0, 0.0, ValidationMode::Lenient));
    registerSchema(environmentSchema);

    // Logging settings
    ConfigSchema loggingSchema("Logging", ConfigCategory::Logging);
    loggingSchema.addRule(ValidationRule("log_level", ConfigDataType::String,
        "Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)", true, std::nullopt, std::nullopt, ValidationMode::Strict));
    loggingSchema.addRule(ValidationRule("log_to_file", ConfigDataType::Boolean,
        "Whether to log to file", true, std::nullopt, std::nullopt, ValidationMode::Strict));
    loggingSchema.addRule(ValidationRule("log_filename", ConfigDataType::String,
        "Log filename", false, std::nullopt, std::nullopt, ValidationMode::Lenient));
    registerSchema(loggingSchema);

    // Visualization settings
    ConfigSchema visualizationSchema("Visualization", ConfigCategory::Visualization);
    visualizationSchema.addRule(ValidationRule("visualization_enabled", ConfigDataType::Boolean,
        "Whether visualization is enabled", true, std::nullopt, std::nullopt, ValidationMode::Strict));
    visualizationSchema.addRule(ValidationRule("visualization_update_rate", ConfigDataType::Float,
        "Visualization update rate in Hz", false, 1.0, 1000.0, ValidationMode::Lenient));
    visualizationSchema.addRule(ValidationRule("visualization_size", ConfigDataType::Integer,
        "Visualization window size", false, 100.0, 2000.0, ValidationMode::Lenient));
    registerSchema(visualizationSchema);

    // Reward settings
    ConfigSchema rewardSchema("Reward", ConfigCategory::Reward);
    rewardSchema.addRule(ValidationRule("reward_discount_factor", ConfigDataType::Float,
        "Reward discount factor (0.0-1.0)", true, 0.0, 1.0, ValidationMode::Strict));
    rewardSchema.addRule(ValidationRule("reward_scaling", ConfigDataType::Float,
        "Reward scaling factor", false, 0.0, 1000.0, ValidationMode::Lenient));
    registerSchema(rewardSchema);
}

std::string ConfigValidator::dataTypeToString(ConfigDataType type) {
    switch (type) {
        case ConfigDataType::Integer: return "integer";
        case ConfigDataType::Float: return "float";
        case ConfigDataType::Boolean: return "boolean";
        case ConfigDataType::String: return "string";
        case ConfigDataType::IntegerVector: return "integer vector";
        case ConfigDataType::FloatVector: return "float vector";
        case ConfigDataType::StringVector: return "string vector";
        case ConfigDataType::Any: return "any";
        default: return "unknown";
    }
}

std::string ConfigValidator::formatValue(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool ConfigValidator::validateType(const ConfigValue& value, ConfigDataType expectedType) const {
    switch (expectedType) {
        case ConfigDataType::Integer:
            return std::holds_alternative<int>(value);
        case ConfigDataType::Float:
            return std::holds_alternative<double>(value);
        case ConfigDataType::Boolean:
            return std::holds_alternative<bool>(value);
        case ConfigDataType::String:
            return std::holds_alternative<std::string>(value);
        case ConfigDataType::IntegerVector:
            return std::holds_alternative<std::vector<int>>(value);
        case ConfigDataType::FloatVector:
            return std::holds_alternative<std::vector<double>>(value);
        case ConfigDataType::StringVector:
            return std::holds_alternative<std::vector<std::string>>(value);
        case ConfigDataType::Any:
            return true;
        default:
            return false;
    }
}

bool ConfigValidator::validateRange(const ConfigValue& value, const ValidationRule& rule) const {
    if (!rule.minValue && !rule.maxValue) {
        return true; // No range validation
    }
    
    double val = 0.0;
    bool converted = false;
    
    if (std::holds_alternative<int>(value)) {
        val = static_cast<double>(std::get<int>(value));
        converted = true;
    } else if (std::holds_alternative<double>(value)) {
        val = std::get<double>(value);
        converted = true;
    } else if (std::holds_alternative<float>(value)) {
        val = static_cast<double>(std::get<float>(value));
        converted = true;
    }
    
    if (!converted) {
        return false;
    }
    
    if (rule.minValue && val < rule.minValue) {
        return false;
    }
    if (rule.maxValue && val > rule.maxValue) {
        return false;
    }
    
    return true;
}

bool ConfigValidator::validateStringPattern(const ConfigValue& value, const ValidationRule& rule) const {
    if (!rule.pattern) {
        return true; // No pattern validation
    }
    
    if (!std::holds_alternative<std::string>(value)) {
        return false;
    }
    
    const std::string& strValue = std::get<std::string>(value);
    return strValue.find(*rule.pattern) != std::string::npos;
}

bool ConfigValidator::validateCustom(const ConfigValue& value, const ValidationRule& rule) const {
    if (!rule.customValidator) {
        return true;
    }
    return rule.customValidator(value);
}

bool ConfigValidator::validateConsistency(const std::string& fieldName, const ConfigValue& value,
                                         const std::unordered_map<std::string, ConfigValue>& allConfig,
                                         const std::vector<std::string>& relatedFields) const {
    if (relatedFields.empty()) {
        return true;
    }
    
    // Simple consistency checks for common patterns
    if (fieldName == "simulation_timestep" && relatedFields.size() >= 2) {
        // Check that simulation_timestep is reasonable relative to max_simulation_steps
        bool hasTimestep = false, hasSteps = false;
        double timestep = 0.0;
        int64_t maxSteps = 0;
        
        for (const auto& [key, val] : allConfig) {
            if (key == "simulation_timestep") {
                hasTimestep = true;
                if (std::holds_alternative<double>(val)) {
                    timestep = std::get<double>(val);
                }
            } else if (key == "max_simulation_steps") {
                hasSteps = true;
                if (std::holds_alternative<int64_t>(val)) {
                    maxSteps = std::get<int64_t>(val);
                }
            }
        }
        
        if (hasTimestep && hasSteps) {
            double timeLimit = timestep * maxSteps;
            // Check if time limit is reasonable (between 1 second and 1 hour)
            return timeLimit >= 1.0 && timeLimit <= 3600.0;
        }
    }
    
    if (fieldName == "width" && relatedFields.size() >= 1) {
        // Check that width and height are positive
        bool hasWidth = false;
        int64_t width = 0;
        
        for (const auto& [key, val] : allConfig) {
            if (key == "width") {
                hasWidth = true;
                if (std::holds_alternative<int64_t>(val)) {
                    width = std::get<int64_t>(val);
                }
            }
        }
        
        if (hasWidth) {
            return width > 0;
        }
    }
    
    return true;
}

ValidationResult ConfigValidator::getSeverityForMode(bool isValid, ValidationMode mode) const {
    if (isValid) {
        return ValidationResult::Valid;
    }
    
    switch (mode) {
        case ValidationMode::Strict:
            return ValidationResult::Invalid;
        case ValidationMode::Lenient:
            return ValidationResult::PartiallyValid;
        case ValidationMode::Permissive:
            return ValidationResult::PartiallyValid;
        default:
            return ValidationResult::Invalid;
    }
}

std::vector<ValidationError> ConfigValidator::validate(const ConfigValue& value, const ValidationRule& rule,
                                                     ConfigCategory category, const std::string& field) const {
    std::vector<ValidationError> errors;
    
    // Check type validation
    if (!validateType(value, rule.dataType)) {
        std::string expectedType = dataTypeToString(rule.dataType);
        std::string actualType = "unknown";
        
        if (std::holds_alternative<int>(value)) actualType = "integer";
        else if (std::holds_alternative<double>(value)) actualType = "float";
        else if (std::holds_alternative<bool>(value)) actualType = "boolean";
        else if (std::holds_alternative<std::string>(value)) actualType = "string";
        else if (std::holds_alternative<std::vector<int>>(value)) actualType = "integer vector";
        else if (std::holds_alternative<std::vector<double>>(value)) actualType = "float vector";
        else if (std::holds_alternative<std::vector<std::string>>(value)) actualType = "string vector";
        
        errors.emplace_back(field, categoryToString(category),
                           "Expected type '" + expectedType + "' but got '" + actualType + "'",
                           ValidationResult::Invalid);
    }
    
    // Check range validation
    if (validateType(value, rule.dataType) && !validateRange(value, rule)) {
        std::string rangeStr;
        if (rule.minValue && rule.maxValue) {
            rangeStr = "[" + std::to_string(*rule.minValue) + ", " + std::to_string(*rule.maxValue) + "]";
        } else if (rule.minValue) {
            rangeStr = "[".append(std::to_string(*rule.minValue)).append(", +inf)";
        } else if (rule.maxValue) {
            rangeStr = "(-inf, ".append(std::to_string(*rule.maxValue)).append(")]";
        }
        
        std::string valueStr = formatValue(value);
        errors.emplace_back(field, categoryToString(category),
                           "Value '" + valueStr + "' is outside allowed range " + rangeStr,
                           ValidationResult::Invalid);
    }
    
    // Check string pattern validation
    if (validateType(value, ConfigDataType::String) && !validateStringPattern(value, rule)) {
        errors.emplace_back(field, categoryToString(category),
                           "String value does not match pattern '" + *rule.pattern + "'",
                           ValidationResult::Invalid);
    }
    
    // Check custom validation
    if (validateType(value, rule.dataType) && !validateCustom(value, rule)) {
        errors.emplace_back(field, categoryToString(category),
                           "Custom validation failed for field '" + field + "'",
                           ValidationResult::Invalid);
    }
    
    return errors;
}

ValidationResult ConfigValidator::validateValue(const std::string& fieldName, const ConfigValue& value,
                                               ConfigCategory category,
                                               std::vector<ValidationError>& errors) const {
    const auto& schemaManager = ConfigSchemaManager::getInstance();
    const auto& schema = schemaManager.getSchema(category);
    
    auto it = schema.rules.find(fieldName);
    if (it == schema.rules.end()) {
        // Field not found in schema
        ValidationError error(fieldName, categoryToString(category),
                             "Unknown configuration field",
                             ValidationResult::Invalid);
        errors.push_back(error);
        return ValidationResult::Invalid;
    }
    
    const auto& rule = it->second;
    
    // Validate consistency with related fields
    bool consistencyValid = validateConsistency(fieldName, value, std::unordered_map<std::string, ConfigValue>(), rule.relatedFields);
    
    // Validate value
    auto fieldErrors = validate(value, rule, category, fieldName);
    
    // Merge errors
    errors.insert(errors.end(), fieldErrors.begin(), fieldErrors.end());
    
    // Determine overall result based on mode and error count
    bool isValid = consistencyValid && fieldErrors.empty();
    ValidationResult result = getSeverityForMode(isValid, currentMode);
    
    return result;
}

std::vector<ValidationError> ConfigValidator::validateAll(const std::unordered_map<std::string, ConfigValue>& configData,
                                                       ConfigCategory category) const {
    std::vector<ValidationError> allErrors;
    
    for (const auto& [fieldName, value] : configData) {
        validateValue(fieldName, value, category, allErrors);
    }
    
    return allErrors;
}

} // namespace nlm