#include "ConfigValidation.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace nlm {
namespace config {

// RequiredFieldsRule implementation
ValidationResult RequiredFieldsRule::validate(const ConfigManager& config) const {
    ValidationResult result;
    
    for (const auto& fieldName : requiredFields_) {
        if (!config.has(fieldName)) {
            result.addError("Missing required configuration field: " + fieldName);
        }
    }
    
    return result;
}

// FieldTypeRule implementation
ValidationResult FieldTypeRule::validate(const ConfigManager& config) const {
    ValidationResult result;
    
    for (const auto& constraint : constraints_) {
        const ConfigEntry* entry = config.findEntry(constraint.fieldName);
        if (!entry) {
            if (constraint.fieldName.find("optional") == std::string::npos) {
                result.addError("Missing configuration field: " + constraint.fieldName);
            }
            continue;
        }
        
        // Validate field type
        bool typeValid = false;
        std::string actualType;
        
        switch (entry->value.index()) {
            case 0: // int
            case 1: // int64_t
                actualType = "int";
                if (constraint.expectedType == "int" || constraint.expectedType == "number") {
                    typeValid = true;
                }
                break;
            case 2: // double
                actualType = "double";
                if (constraint.expectedType == "double" || constraint.expectedType == "float" || constraint.expectedType == "number") {
                    typeValid = true;
                }
                break;
            case 3: // bool
                actualType = "bool";
                if (constraint.expectedType == "bool") {
                    typeValid = true;
                }
                break;
            case 4: // string
                actualType = "string";
                if (constraint.expectedType == "string") {
                    typeValid = true;
                }
                break;
            case 5: // vector<int>
                actualType = "vector<int>";
                if (constraint.expectedType == "vector<int>" || constraint.expectedType == "array") {
                    typeValid = true;
                }
                break;
            case 6: // vector<double>
                actualType = "vector<double>";
                if (constraint.expectedType == "vector<double>" || constraint.expectedType == "array") {
                    typeValid = true;
                }
                break;
            case 7: // vector<string>
                actualType = "vector<string>";
                if (constraint.expectedType == "vector<string>" || constraint.expectedType == "array") {
                    typeValid = true;
                }
                break;
        }
        
        if (!typeValid) {
            result.addError("Field \"" + constraint.fieldName + "\" has incorrect type. "
                          "Expected: " + constraint.expectedType + ", "
                          "Actual: " + actualType);
        }
        
        // Validate allowed values
        if (!constraint.allowedValues.empty()) {
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    if (std::find(constraint.allowedValues.begin(), constraint.allowedValues.end(), val) == constraint.allowedValues.end()) {
                        result.addError("Field \"" + constraint.fieldName + "\" has invalid value. "
                                      "Allowed values: " + utils::joinStrings(constraint.allowedValues, ", "));
                    }
                } else {
                    // Non-string types - check against string representation
                    std::stringstream ss;
                    ss << val;
                    std::string strVal = ss.str();
                    if (std::find(constraint.allowedValues.begin(), constraint.allowedValues.end(), strVal) == constraint.allowedValues.end()) {
                        result.addError("Field \"" + constraint.fieldName + "\" has invalid value. "
                                      "Allowed values: " + utils::joinStrings(constraint.allowedValues, ", "));
                    }
                }
            }, entry->value);
        }
    }
    
    return result;
}

// RangeRule implementation
ValidationResult RangeRule::validate(const ConfigManager& config) const {
    ValidationResult result;
    
    for (const auto& constraint : constraints_) {
        const ConfigEntry* entry = config.findEntry(constraint.fieldName);
        if (!entry) {
            continue; // Field not present, skip range validation
        }
        
        // Extract numeric value
        double value = 0.0;
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                value = static_cast<double>(val);
            } else if constexpr (std::is_same_v<T, double>) {
                value = val;
            } else if constexpr (std::is_same_v<T, float>) {
                value = static_cast<double>(val);
            }
        }, entry->value);
        
        // Validate range
        bool withinRange = true;
        std::string rangeDescription;
        
        if (constraint.inclusiveMin && constraint.inclusiveMax) {
            if (value < constraint.minValue || value > constraint.maxValue) {
                withinRange = false;
                rangeDescription = "[" + std::to_string(constraint.minValue) + ", " + std::to_string(constraint.maxValue) + "]";
            }
        } else if (constraint.inclusiveMin) {
            if (value < constraint.minValue || value > constraint.maxValue) {
                withinRange = false;
                rangeDescription = "[" + std::to_string(constraint.minValue) + ", " + std::to_string(constraint.maxValue) + ")";
            }
        } else if (constraint.inclusiveMax) {
            if (value < constraint.minValue || value > constraint.maxValue) {
                withinRange = false;
                rangeDescription = "(" + std::to_string(constraint.minValue) + ", " + std::to_string(constraint.maxValue) + "]";
            }
        } else {
            if (value <= constraint.minValue || value >= constraint.maxValue) {
                withinRange = false;
                rangeDescription = "(" + std::to_string(constraint.minValue) + ", " + std::to_string(constraint.maxValue) + ")";
            }
        }
        
        if (!withinRange) {
            result.addError("Field \"" + constraint.fieldName + "\" has value " + 
                          std::to_string(value) + " outside valid range " + rangeDescription);
        }
    }
    
    return result;
}

// DependencyRule implementation
ValidationResult DependencyRule::validate(const ConfigManager& config) const {
    ValidationResult result;
    
    for (const auto& constraint : constraints_) {
        const ConfigEntry* entry = config.findEntry(constraint.requiredIfField);
        if (!entry) {
            continue; // Dependency field not present, skip validation
        }
        
        // Check if dependency field has one of the required values
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            bool dependencyMet = false;
            
            if constexpr (std::is_same_v<T, std::string>) {
                if (std::find(constraint.requiredIfValues.begin(), constraint.requiredIfValues.end(), val) != constraint.requiredIfValues.end()) {
                    dependencyMet = true;
                }
            } else {
                std::stringstream ss;
                ss << val;
                std::string strVal = ss.str();
                if (std::find(constraint.requiredIfValues.begin(), constraint.requiredIfValues.end(), strVal) != constraint.requiredIfValues.end()) {
                    dependencyMet = true;
                }
            }
            
            if (dependencyMet) {
                // Dependency condition is met, check if required field exists
                if (!config.has(constraint.fieldName)) {
                    result.addError(constraint.errorMessage.empty() ? 
                                  ("Field \"" + constraint.fieldName + "\" is required when " +
                                   constraint.requiredIfField + " is one of: " +
                                   utils::joinStrings(constraint.requiredIfValues, ", "))
                                  : constraint.errorMessage);
                }
            }
        }, entry->value);
    }
    
    return result;
}

// SchemaRule implementation
ValidationResult SchemaRule::validate(const ConfigManager& config) const {
    ValidationResult result;
    
    // For now, just return a successful validation
    // In a real implementation, this would validate against the schema
    return result;
}

// ConfigValidator implementation
void ConfigValidator::addRule(std::unique_ptr<ValidationRule> rule) {
    if (rule) {
        rules_[rule->getName()] = std::move(rule);
    }
}

void ConfigValidator::removeRule(const std::string& ruleName) {
    rules_.erase(ruleName);
}

bool ConfigValidator::hasRule(const std::string& ruleName) const {
    return rules_.find(ruleName) != rules_.end();
}

ValidationResult ConfigValidator::validate(const ConfigManager& config) const {
    ValidationResult result;
    
    for (const auto& pair : rules_) {
        const std::string& ruleName = pair.first;
        const ValidationRule* rule = pair.second.get();
        
        ValidationResult ruleResult = rule->validate(config);
        
        // Merge results
        if (!ruleResult.valid) {
            result.valid = false;
        }
        
        result.errors.insert(result.errors.end(), ruleResult.errors.begin(), ruleResult.errors.end());
        result.warnings.insert(result.warnings.end(), ruleResult.warnings.begin(), ruleResult.warnings.end());
        
        // Include rule name in errors for better debugging
        for (const auto& error : ruleResult.errors) {
            if (rule->isCritical()) {
                result.addError("[" + ruleName + "] " + error);
            } else {
                result.addError("[" + ruleName + "] " + error);
            }
        }
        
        for (const auto& warning : ruleResult.warnings) {
            result.addWarning("[" + ruleName + "] " + warning);
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validate(const std::string& configFile) const {
    ConfigManager config;
    if (!config.loadFromFile(configFile)) {
        ValidationResult result;
        result.addError("Failed to load configuration file: " + configFile);
        return result;
    }
    
    return validate(config);
}

ValidationResult ConfigValidator::validate(const std::string& configString, const std::string& format) const {
    ConfigManager config;
    if (!config.loadFromString(configString, format)) {
        ValidationResult result;
        result.addError("Failed to parse configuration string");
        return result;
    }
    
    return validate(config);
}

ValidationResult ConfigValidator::validateWithRule(const std::string& ruleName, const ConfigManager& config) const {
    auto it = rules_.find(ruleName);
    if (it == rules_.end()) {
        ValidationResult result;
        result.addError("Validation rule not found: " + ruleName);
        return result;
    }
    
    return it->second->validate(config);
}

std::vector<std::string> ConfigValidator::getRuleNames() const {
    std::vector<std::string> names;
    names.reserve(rules_.size());
    
    for (const auto& pair : rules_) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::string ConfigValidator::generateValidationReport(const ValidationResult& result) const {
    std::ostringstream oss;
    oss << "=== Configuration Validation Report ===" << std::endl;
    oss << "Generated at: " << utils::generateTimestamp() << std::endl;
    oss << std::endl;
    oss << "Validation Status: " << (result.valid ? "PASS" : "FAIL") << std::endl;
    oss << "Total Errors: " << result.errors.size() << std::endl;
    oss << "Total Warnings: " << result.warnings.size() << std::endl;
    oss << std::endl;
    
    if (!result.errors.empty()) {
        oss << "=== Errors ===" << std::endl;
        for (size_t i = 0; i < result.errors.size(); ++i) {
            oss << i + 1 << ". " << result.errors[i] << std::endl;
        }
        oss << std::endl;
    }
    
    if (!result.warnings.empty()) {
        oss << "=== Warnings ===" << std::endl;
        for (size_t i = 0; i < result.warnings.size(); ++i) {
            oss << i + 1 << ". " << result.warnings[i] << std::endl;
        }
        oss << std::endl;
    }
    
    return oss.str();
}

std::string ConfigValidator::generateValidationSummary(const ValidationResult& result) const {
    std::ostringstream oss;
    oss << "Configuration Validation Summary:" << std::endl;
    oss << "Status: " << (result.valid ? "VALID" : "INVALID") << std::endl;
    oss << "Errors: " << result.errors.size() << std::endl;
    oss << "Warnings: " << result.warnings.size() << std::endl;
    
    if (!result.errors.empty()) {
        oss << "\nFirst error: " << result.errors[0];
    }
    
    return oss.str();
}

std::string ConfigValidator::generateValidationDetailedReport(const ValidationResult& result) const {
    std::ostringstream oss;
    oss << "=== Detailed Validation Report ===" << std::endl;
    oss << "Timestamp: " << utils::generateTimestamp() << std::endl;
    oss << "Validation Status: " << (result.valid ? "PASS" : "FAIL") << std::endl;
    oss << std::endl;
    
    if (result.errors.empty() && result.warnings.empty()) {
        oss << "Configuration validation passed with no errors or warnings." << std::endl;
        return oss.str();
    }
    
    if (!result.errors.empty()) {
        oss << "--- ERRORS ---" << std::endl;
        for (size_t i = 0; i < result.errors.size(); ++i) {
            oss << "Error " << (i + 1) << ": " << result.errors[i] << std::endl;
        }
        oss << std::endl;
    }
    
    if (!result.warnings.empty()) {
        oss << "--- WARNINGS ---" << std::endl;
        for (size_t i = 0; i < result.warnings.size(); ++i) {
            oss << "Warning " << (i + 1) << ": " << result.warnings[i] << std::endl;
        }
        oss << std::endl;
    }
    
    return oss.str();
}

// SchemaValidator implementation
void SchemaValidator::addSchema(const ConfigSchema& schema) {
    // Generate schema name from schema content
    std::string schemaName = "schema_" + std::to_string(std::hash<std::string>{}(schema.getSchemaDescription()));
    schemas_[schemaName] = schema;
}

bool SchemaValidator::removeSchema(const std::string& schemaName) {
    auto it = schemas_.find(schemaName);
    if (it == schemas_.end()) {
        return false;
    }
    
    schemas_.erase(it);
    return true;
}

ConfigSchema* SchemaValidator::getSchema(const std::string& schemaName) {
    auto it = schemas_.find(schemaName);
    return it != schemas_.end() ? &it->second : nullptr;
}

const ConfigSchema* SchemaValidator::getSchema(const std::string& schemaName) const {
    auto it = schemas_.find(schemaName);
    return it != schemas_.end() ? &it->second : nullptr;
}

ValidationResult SchemaValidator::validate(const ConfigManager& config, const std::string& schemaName) const {
    const ConfigSchema* schema = getSchema(schemaName);
    if (!schema) {
        ValidationResult result;
        result.addError("Schema not found: " + schemaName);
        return result;
    }
    
    return schema->validate(config);
}

ValidationResult SchemaValidator::validate(const std::string& configFile, const std::string& schemaName) {
    ConfigManager config;
    if (!config.loadFromFile(configFile)) {
        ValidationResult result;
        result.addError("Failed to load configuration file: " + configFile);
        return result;
    }
    
    return validate(config, schemaName);
}

void SchemaValidator::registerDefaultSchemas() {
    ConfigSchema defaultSchema;
    // Add default schema fields
    ConfigSchema::Field neuronCountField;
    neuronCountField.name = "neuron_count";
    neuronCountField.type = "int";
    neuronCountField.required = true;
    neuronCountField.defaultValue = size_t(1000);
    neuronCountField.description = "Number of neurons";
    defaultSchema.addField(neuronCountField);
    
    addSchema(defaultSchema);
}

bool SchemaValidator::hasSchema(const std::string& schemaName) const {
    return schemas_.find(schemaName) != schemas_.end();
}

std::vector<std::string> SchemaValidator::getSchemaNames() const {
    std::vector<std::string> names;
    names.reserve(schemas_.size());
    
    for (const auto& pair : schemas_) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::string SchemaValidator::analyzeSchemaCompatibility(const ConfigManager& config) const {
    std::ostringstream oss;
    oss << "Schema Compatibility Analysis:" << std::endl;
    oss << "Configuration entries: " << config.getEntryCount() << std::endl;
    oss << "Schemas available: " << schemas_.size() << std::endl;
    
    if (schemas_.empty()) {
        oss << "No schemas defined for validation." << std::endl;
        return oss.str();
    }
    
    oss << std::endl << "Schema compatibility scores:" << std::endl;
    
    for (const auto& pair : schemas_) {
        const std::string& schemaName = pair.first;
        const ConfigSchema& schema = pair.second;
        
        ValidationResult validationResult = schema.validate(config);
        
        double score = 100.0;
        if (!validationResult.valid) {
            score -= 50.0; // Penalize invalid configurations
        }
        
        if (!validationResult.warnings.empty()) {
            score -= validationResult.warnings.size() * 10.0;
        }
        
        oss << "  - " << schemaName << ": " << score << "%" << std::endl;
    }
    
    return oss.str();
}

std::map<std::string, double> SchemaValidator::getSchemaComplianceScore(const ConfigManager& config, const std::string& schemaName) const {
    std::map<std::string, double> scores;
    
    const ConfigSchema* schema = getSchema(schemaName);
    if (!schema) {
        scores["schema_not_found"] = 0.0;
        return scores;
    }
    
    ValidationResult validationResult = schema->validate(config);
    
    double baseScore = 100.0;
    if (!validationResult.valid) {
        baseScore -= 50.0;
    }
    
    if (!validationResult.warnings.empty()) {
        baseScore -= validationResult.warnings.size() * 10.0;
    }
    
    scores["overall"] = baseScore;
    scores["valid"] = validationResult.valid ? 1.0 : 0.0;
    scores["error_count"] = static_cast<double>(validationResult.errors.size());
    scores["warning_count"] = static_cast<double>(validationResult.warnings.size());
    
    return scores;
}

// ConfigConsistencyValidator implementation
ValidationResult ConfigConsistencyValidator::validate(const ConfigManager& config) const {
    ValidationResult result;
    
    // Add consistency checks
    result = validateCrossFieldDependencies(config);
    if (!result.valid) {
        return result;
    }
    
    result = validateValueConsistency(config);
    if (!result.valid) {
        return result;
    }
    
    result = validateConfigurationIntegrity(config);
    
    return result;
}

ValidationResult ConfigConsistencyValidator::validateCrossFieldDependencies(const ConfigManager& config) const {
    ValidationResult result;
    
    // Check for conflicting settings
    result = validateNoConflictingSettings(config);
    
    // Check resource constraints
    result = validateResourceConstraints(config);
    
    return result;
}

ValidationResult ConfigConsistencyValidator::validateValueConsistency(const ConfigManager& config) const {
    ValidationResult result;
    
    // Check simulation parameters consistency
    result = validateSimulationParameters(config);
    
    return result;
}

ValidationResult ConfigConsistencyValidator::validateConfigurationIntegrity(const ConfigManager& config) const {
    ValidationResult result;
    
    // Check overall configuration integrity
    std::vector<ValidationResult> subResults;
    
    // Check for required fields
    std::vector<std::string> requiredFields = {"neuron_count", "simulation_timestep"};
    for (const auto& fieldName : requiredFields) {
        if (!config.has(fieldName)) {
            ValidationResult subResult;
            subResult.addError("Missing required configuration field: " + fieldName);
            subResults.push_back(subResult);
        }
    }
    
    // Check for reasonable values
    auto neuronCountOpt = config.get<size_t>("neuron_count");
    if (neuronCountOpt.has_value()) {
        size_t neuronCount = neuronCountOpt.value();
        if (neuronCount == 0) {
            ValidationResult subResult;
            subResult.addError("Invalid neuron_count: must be greater than 0");
            subResults.push_back(subResult);
        } else if (neuronCount > 1000000) {
            ValidationResult subResult;
            subResult.addWarning("Very large neuron_count may impact performance");
            subResults.push_back(subResult);
        }
    }
    
    // Check timestep
    auto timestepOpt = config.get<double>("simulation_timestep");
    if (timestepOpt.has_value()) {
        double timestep = timestepOpt.value();
        if (timestep <= 0.0) {
            ValidationResult subResult;
            subResult.addError("Invalid simulation_timestep: must be greater than 0");
            subResults.push_back(subResult);
        } else if (timestep > 1.0) {
            ValidationResult subResult;
            subResult.addWarning("Large timestep may cause numerical instability");
            subResults.push_back(subResult);
        }
    }
    
    // Combine all sub-results
    for (const auto& subResult : subResults) {
        result.errors.insert(result.errors.end(), subResult.errors.begin(), subResult.errors.end());
        result.warnings.insert(result.warnings.end(), subResult.warnings.begin(), subResult.warnings.end());
        if (!subResult.valid) {
            result.valid = false;
        }
    }
    
    return result;
}

ValidationResult ConfigConsistencyValidator::validateNoConflictingSettings(const ConfigManager& config) const {
    ValidationResult result;
    
    // Check for conflicting plasticity settings
    auto stdpLtpOpt = config.get<float>("stdp_ltp_weight");
    auto stdpLtdOpt = config.get<float>("stdp_ltd_weight");
    
    if (stdpLtpOpt.has_value() && stdpLtdOpt.has_value()) {
        float ltpWeight = stdpLtpOpt.value();
        float ltdWeight = stdpLtdOpt.value();
        
        if (ltpWeight <= 0.0 || ltdWeight <= 0.0) {
            result.addError("STDP weights must be positive (LTP: " + std::to_string(ltpWeight) + 
                          ", LTD: " + std::to_string(ltdWeight) + ")");
        }
        
        if (std::abs(ltpWeight - ltdWeight) < 0.001) {
            result.addWarning("STDP LTP and LTD weights are nearly identical (LTP: " + 
                            std::to_string(ltpWeight) + ", LTD: " + std::to_string(ltdWeight) +
                            ") - consider using different values for stability");
        }
    }
    
    return result;
}

ValidationResult ConfigConsistencyValidator::validateResourceConstraints(const ConfigManager& config) const {
    ValidationResult result;
    
    // Check for resource constraints
    auto neuronCountOpt = config.get<size_t>("neuron_count");
    auto timestepOpt = config.get<double>("simulation_timestep");
    
    if (neuronCountOpt.has_value() && timestepOpt.has_value()) {
        size_t neuronCount = neuronCountOpt.value();
        double timestep = timestepOpt.value();
        
        // Estimate memory requirements (simplified calculation)
        size_t estimatedMemory = neuronCount * 100 + neuronCount * neuronCount * 4 / 1000000; // 100 bytes per neuron + 4MB per synapse connection
        
        if (estimatedMemory > 2048) { // 2GB limit
            result.addWarning("Estimated memory usage: " + std::to_string(estimatedMemory) + 
                            "MB. Consider reducing neuron_count or connection_probability");
        }
        
        // Check timestep for stability
        if (timestep > 0.01) { // 10ms timestep
            result.addWarning("Large timestep (" + std::to_string(timestep) + 
                            "s) may cause numerical instability. "
                            "Consider using smaller timestep for better stability");
        }
    }
    
    return result;
}

ValidationResult ConfigConsistencyValidator::validateSimulationParameters(const ConfigManager& config) const {
    ValidationResult result;
    
    // Check simulation parameter consistency
    auto neuronCountOpt = config.get<size_t>("neuron_count");
    auto timestepOpt = config.get<double>("simulation_timestep");
    auto connectionProbOpt = config.get<float>("connection_probability");
    
    if (neuronCountOpt.has_value() && timestepOpt.has_value()) {
        size_t neuronCount = neuronCountOpt.value();
        double timestep = timestepOpt.value();
        
        // Check neuron count vs timestep ratio
        double ratio = static_cast<double>(neuronCount) / timestep;
        if (ratio > 100000) {
            result.addWarning("High neuron count (" + std::to_string(neuronCount) + ") with large timestep (" +
                            std::to_string(timestep) + ") may cause performance issues. "
                            "Consider reducing timestep or neuron count");
        }
    }
    
    if (connectionProbOpt.has_value()) {
        float connectionProb = connectionProbOpt.value();
        if (connectionProb < 0.0 || connectionProb > 1.0) {
            result.addError("Invalid connection_probability: must be between 0.0 and 1.0 (got " +
                          std::to_string(connectionProb) + ")");
        } else if (connectionProb > 0.2) {
            result.addWarning("High connection_probability (" + std::to_string(connectionProb) +
                            ") may result in overly dense connectivity and impact performance");
        }
    }
    
    return result;
}

// ConfigAuditLogger implementation
void ConfigAuditLogger::logValidationStart(const std::string& configSource, const std::string& validatorName) {
    std::lock_guard<std::mutex> lock(logMutex_);
    AuditEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.eventType = "VALIDATION_START";
    entry.configSource = configSource;
    entry.validatorName = validatorName;
    entry.message = "Validation started for configuration source: " + configSource;
    entry.level = "INFO";
    
    logEntries_.push_back(entry);
}

void ConfigAuditLogger::logValidationResult(const ValidationResult& result) {
    std::lock_guard<std::mutex> lock(logMutex_);
    AuditEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.eventType = "VALIDATION_RESULT";
    entry.validatorName = "ConfigValidator";
    entry.message = "Validation completed - Status: " + (result.valid ? "PASS" : "FAIL") +
                   ", Errors: " + std::to_string(result.errors.size()) +
                   ", Warnings: " + std::to_string(result.warnings.size());
    entry.level = result.valid ? "INFO" : "ERROR";
    
    logEntries_.push_back(entry);
}

void ConfigAuditLogger::logValidationError(const std::string& error) {
    std::lock_guard<std::mutex> lock(logMutex_);
    AuditEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.eventType = "VALIDATION_ERROR";
    entry.validatorName = "ConfigValidator";
    entry.message = "Validation error: " + error;
    entry.level = "ERROR";
    
    logEntries_.push_back(entry);
}

void ConfigAuditLogger::logValidationWarning(const std::string& warning) {
    std::lock_guard<std::mutex> lock(logMutex_);
    AuditEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.eventType = "VALIDATION_WARNING";
    entry.validatorName = "ConfigValidator";
    entry.message = "Validation warning: " + warning;
    entry.level = "WARNING";
    
    logEntries_.push_back(entry);
}

void ConfigAuditLogger::clearLog() {
    std::lock_guard<std::mutex> lock(logMutex_);
    logEntries_.clear();
}

std::string ConfigAuditLogger::generateAuditReport() const {
    std::ostringstream oss;
    oss << "=== Configuration Audit Report ===" << std::endl;
    oss << "Generated at: " << utils::generateTimestamp() << std::endl;
    oss << "Total log entries: " << logEntries_.size() << std::endl;
    oss << std::endl;
    
    // Group entries by event type
    std::map<std::string, std::vector<AuditEntry>> entriesByEvent;
    for (const auto& entry : logEntries_) {
        entriesByEvent[entry.eventType].push_back(entry);
    }
    
    for (const auto& pair : entriesByEvent) {
        const std::string& eventType = pair.first;
        const std::vector<AuditEntry>& entries = pair.second;
        
        oss << "=== " << eventType << " ===" << std::endl;
        oss << "Count: " << entries.size() << std::endl;
        
        for (size_t i = 0; i < std::min<size_t>(10, entries.size()); ++i) {
            const AuditEntry& entry = entries[i];
            oss << "  " << utils::formatTimestamp(entry.timestamp) << " - " << entry.message << std::endl;
        }
        
        if (entries.size() > 10) {
            oss << "  ... and " << (entries.size() - 10) << " more entries" << std::endl;
        }
        
        oss << std::endl;
    }
    
    return oss.str();
}

std::string ConfigAuditLogger::generateAuditLog(const std::string& configSource) const {
    std::ostringstream oss;
    oss << "=== Audit Log for Configuration Source: " << configSource << " ===" << std::endl;
    oss << "Generated at: " << utils::generateTimestamp() << std::endl;
    oss << std::endl;
    
    std::vector<AuditEntry> relevantEntries;
    for (const auto& entry : logEntries_) {
        if (entry.configSource == configSource || configSource.empty()) {
            relevantEntries.push_back(entry);
        }
    }
    
    for (const auto& entry : relevantEntries) {
        oss << "[" << utils::formatTimestamp(entry.timestamp) << "] "
            << "[" << entry.level << "] "
            << entry.message << std::endl;
    }
    
    return oss.str();
}

// Validation utilities implementation
ValidationResult utils::validateConfigFile(const std::string& filepath) {
    ValidationResult result;
    
    std::ifstream file(filepath);
    if (!file.good()) {
        result.addError("Cannot open configuration file: " + filepath);
        return result;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), 
                        std::istreambuf_iterator<char>());
    
    return validateConfigString(content, "auto");
}

ValidationResult utils::validateConfigString(const std::string& content, const std::string& format) {
    ValidationResult result;
    
    if (content.empty()) {
        result.addError("Configuration content is empty");
        return result;
    }
    
    // Basic format detection
    std::string detectedFormat = format;
    if (format == "auto") {
        if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
            detectedFormat = "JSON";
        } else if (content.find(':') != std::string::npos && content.find('-') != std::string::npos) {
            detectedFormat = "YAML";
        } else {
            result.addError("Cannot detect configuration format. Please specify format explicitly.");
            return result;
        }
    }
    
    // For now, just return successful validation
    // In a real implementation, this would parse and validate the configuration
    result.addWarning("Configuration validation not fully implemented - returning success");
    
    return result;
}

ConfigSchema utils::loadSchemaFromFile(const std::string& filepath) {
    ConfigSchema schema;
    // Implementation would load schema from file
    return schema;
}

bool utils::saveSchemaToFile(const std::string& filepath, const ConfigSchema& schema) {
    // Implementation would save schema to file
    return false;
}

std::string utils::schemaToString(const ConfigSchema& schema) {
    std::ostringstream oss;
    oss << "ConfigSchema with " << schema.getFieldCount() << " fields";
    return oss.str();
}

ConfigSchema utils::stringToSchema(const std::string& schemaString) {
    // Implementation would parse schema from string
    return ConfigSchema();
}

std::string utils::formatValidationResult(const ValidationResult& result) {
    return "Status: " + (result.valid ? "VALID" : "INVALID") +
           ", Errors: " + std::to_string(result.errors.size()) +
           ", Warnings: " + std::to_string(result.warnings.size());
}

std::string utils::formatValidationErrors(const ValidationResult& result) {
    if (result.errors.empty()) return "";
    
    std::ostringstream oss;
    oss << "Errors:" << std::endl;
    for (size_t i = 0; i < result.errors.size(); ++i) {
        oss << "  " << (i + 1) << ". " << result.errors[i] << std::endl;
    }
    
    return oss.str();
}

std::string utils::formatValidationWarnings(const ValidationResult& result) {
    if (result.warnings.empty()) return "";
    
    std::ostringstream oss;
    oss << "Warnings:" << std::endl;
    for (size_t i = 0; i < result.warnings.size(); ++i) {
        oss << "  " << (i + 1) << ". " << result.warnings[i] << std::endl;
    }
    
    return oss.str();
}

bool utils::isValidConfigName(const std::string& name) {
    return !name.empty() && 
           std::all_of(name.begin(), name.end(),
                      [](char c) { return std::isalnum(c) || c == '_' || c == '-'; });
}

bool utils::isValidConfigValue(const std::string& type, const std::any& value) {
    // Implementation would validate based on type
    return true;
}

std::string utils::getConfigTypeName(const ConfigValue& value) {
    return "unknown"; // Would implement type detection
}

std::string utils::generateErrorMessage(const std::string& fieldName, const std::string& expected, const std::string& actual) {
    return "Field \"" + fieldName + "\" expected \"" + expected + "\" but got \"" + actual + "\"";
}

std::string utils::generateDependencyError(const std::string& fieldName, const std::string& dependency, const std::string& condition) {
    return "Field \"" + fieldName + "\" depends on \"" + dependency + "\" with condition: " + condition;
}

std::string utils::generateRangeError(const std::string& fieldName, double value, double min, double max) {
    return "Field \"" + fieldName + "\" value " + std::to_string(value) + " is outside valid range [" +
           std::to_string(min) + ", " + std::to_string(max) + "]";
}

ValidationResult utils::compareConfigs(const ConfigManager& config1, const ConfigManager& config2) {
    ValidationResult result;
    
    // Compare keys
    std::vector<std::string> keys1 = config1.getKeys();
    std::vector<std::string> keys2 = config2.getKeys();
    
    std::set<std::string> set1(keys1.begin(), keys1.end());
    std::set<std::string> set2(keys2.begin(), keys2.end());
    
    std::set<std::string> onlyIn1, onlyIn2;
    std::set_symmetric_difference(set1.begin(), set1.end(), set2.begin(), set2.end(),
                                  std::inserter(onlyIn1, onlyIn1.begin()),
                                  std::inserter(onlyIn2, onlyIn2.begin()));
    
    if (!onlyIn1.empty()) {
        result.addWarning("Configuration 1 has fields not present in configuration 2: " +
                         utils::joinStrings(std::vector<std::string>(onlyIn1.begin(), onlyIn1.end()), ", "));
    }
    
    if (!onlyIn2.empty()) {
        result.addWarning("Configuration 2 has fields not present in configuration 1: " +
                         utils::joinStrings(std::vector<std::string>(onlyIn2.begin(), onlyIn2.end()), ", "));
    }
    
    // Compare values for common keys
    for (const auto& key : set1) {
        if (set2.find(key) != set2.end()) {
            const ConfigEntry* entry1 = config1.findEntry(key);
            const ConfigEntry* entry2 = config2.findEntry(key);
            
            if (entry1 && entry2) {
                if (entry1->value != entry2->value) {
                    result.addWarning("Different values for field \"" + key + "\":");
                    result.addWarning("  Config 1: \"" + valueToString(entry1->value) + "\"");
                    result.addWarning("  Config 2: \"" + valueToString(entry2->value) + "\"");
                }
            }
        }
    }
    
    return result;
}

std::map<std::string, std::pair<std::any, std::any>> utils::findConfigDifferences(
    const ConfigManager& config1, const ConfigManager& config2) {
    std::map<std::string, std::pair<std::any, std::any>> differences;
    
    std::vector<std::string> keys1 = config1.getKeys();
    std::vector<std::string> keys2 = config2.getKeys();
    
    std::set<std::string> set1(keys1.begin(), keys1.end());
    std::set<std::string> set2(keys2.begin(), keys2.end());
    
    for (const auto& key : set1) {
        if (set2.find(key) != set2.end()) {
            const ConfigEntry* entry1 = config1.findEntry(key);
            const ConfigEntry* entry2 = config2.findEntry(key);
            
            if (entry1 && entry2 && entry1->value != entry2->value) {
                differences[key] = std::make_pair(entry1->value, entry2->value);
            }
        }
    }
    
    return differences;
}

std::map<std::string, size_t> utils::countValidationErrors(const ValidationResult& result) {
    std::map<std::string, size_t> counts;
    
    for (const auto& error : result.errors) {
        // Extract error type from error message
        if (error.find("Missing required") != std::string::npos) {
            counts["missing_field"]++;
        } else if (error.find("incorrect type") != std::string::npos) {
            counts["type_error"]++;
        } else if (error.find("outside valid range") != std::string::npos) {
            counts["range_error"]++;
        } else if (error.find("invalid value") != std::string::npos) {
            counts["value_error"]++;
        } else {
            counts["other_error"]++;
        }
    }
    
    return counts;
}

std::map<std::string, size_t> utils::countValidationWarnings(const ValidationResult& result) {
    std::map<std::string, size_t> counts;
    
    for (const auto& warning : result.warnings) {
        // Extract warning type from warning message
        if (warning.find("impact performance") != std::string::npos) {
            counts["performance_warning"]++;
        } else if (warning.find("numerical instability") != std::string::npos) {
            counts["stability_warning"]++;
        } else if (warning.find("may result in") != std::string::npos) {
            counts["behavior_warning"]++;
        } else {
            counts["other_warning"]++;
        }
    }
    
    return counts;
}

std::string utils::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&](const auto& val) {
        oss << val;
    }, value);
    return oss.str();
}

} // namespace utils
} // namespace config
} // namespace nlm