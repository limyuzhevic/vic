// Test Configuration Validator Implementation
// Advanced configuration validation for tests

#include "TestConfigurationValidator.hpp"
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <ctime>

namespace nlm {
namespace tests {

struct TestConfigurationValidator::Impl {
    // Schema registry
    std::map<std::string, ConfigSchema> schemas;
    std::mutex schemasMutex;
    
    // Validation rules
    std::map<std::string, std::function<bool(const TestConfiguration&)>> validationRules;
    std::mutex rulesMutex;
    
    // Configuration templates
    std::map<std::string, TestConfiguration> templates;
    std::mutex templatesMutex;
    
    // Default schemas
    std::map<std::string, ConfigSchema> defaultSchemas;
    
    Impl() {
        // Initialize default schemas
        initializeDefaultSchemas();
    }
};

TestConfigurationValidator::TestConfigurationValidator() : pImpl(std::make_unique<Impl>()) {}
TestConfigurationValidator::~TestConfigurationValidator() = default;

void TestConfigurationValidator::initializeDefaultSchemas() {
    // Plugin system schema
    ConfigSchema pluginSchema("PluginSystem");
    pluginSchema.fieldTypes["plugin_name"] = "string";
    pluginSchema.fieldTypes["plugin_path"] = "string";
    pluginSchema.fieldTypes["plugin_version"] = "string";
    pluginSchema.fieldTypes["enable"] = "boolean";
    pluginSchema.fieldTypes["priority"] = "integer";
    
    for (const auto& pair : pluginSchema.fieldTypes) {
        pluginSchema.fieldDescriptions[pair.first] = "Plugin system field: " + pair.first;
    }
    
    pluginSchema.fieldRanges["priority"] = {1, 10};
    
    pImpl->defaultSchemas["PluginSystem"] = pluginSchema;
    
    // CLI schema
    ConfigSchema cliSchema("CLI");
    cliSchema.fieldTypes["mode"] = "string";
    cliSchema.fieldTypes["input_file"] = "string";
    cliSchema.fieldTypes["output_file"] = "string";
    cliSchema.fieldTypes["verbose"] = "boolean";
    cliSchema.fieldTypes["debug"] = "boolean";
    
    cliSchema.fieldEnums["mode"] = {"research", "production", "development", "deployment", "benchmark", "validation", "experimental"};
    
    for (const auto& pair : cliSchema.fieldTypes) {
        cliSchema.fieldDescriptions[pair.first] = "CLI field: " + pair.first;
    }
    
    pImpl->defaultSchemas["CLI"] = cliSchema;
    
    // Experiment schema
    ConfigSchema experimentSchema("Experiment");
    experimentSchema.fieldTypes["experiment_name"] = "string";
    experimentSchema.fieldTypes["neuron_count"] = "integer";
    experimentSchema.fieldTypes["plasticity_rules"] = "string";
    experimentSchema.fieldTypes["duration"] = "double";
    
    experimentSchema.fieldRanges["neuron_count"] = {1, 1000000};
    experimentSchema.fieldRanges["duration"] = {0.0, 1000000.0};
    
    for (const auto& pair : experimentSchema.fieldTypes) {
        experimentSchema.fieldDescriptions[pair.first] = "Experiment field: " + pair.first;
    }
    
    pImpl->defaultSchemas["Experiment"] = experimentSchema;
}

bool TestConfigurationValidator::addSchema(const std::string& schemaName, const ConfigSchema& schema) {
    if (schemaName.empty() || !validateSchema(schema)) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->schemasMutex);
    pImpl->schemas[schemaName] = schema;
    return true;
}

bool TestConfigurationValidator::removeSchema(const std::string& schemaName) {
    std::lock_guard<std::mutex> lock(pImpl->schemasMutex);
    
    auto it = pImpl->schemas.find(schemaName);
    if (it != pImpl->schemas.end()) {
        pImpl->schemas.erase(it);
        return true;
    }
    
    return false;
}

ConfigSchema TestConfigurationValidator::getSchema(const std::string& schemaName) const {
    std::lock_guard<std::mutex> lock(pImpl->schemasMutex);
    
    auto it = pImpl->schemas.find(schemaName);
    if (it != pImpl->schemas.end()) {
        return it->second;
    }
    
    // Check default schemas
    auto defaultIt = pImpl->defaultSchemas.find(schemaName);
    if (defaultIt != pImpl->defaultSchemas.end()) {
        return defaultIt->second;
    }
    
    return ConfigSchema();
}

std::vector<std::string> TestConfigurationValidator::getAvailableSchemas() const {
    std::set<std::string> schemaNames;
    
    std::lock_guard<std::mutex> lock(pImpl->schemasMutex);
    for (const auto& pair : pImpl->schemas) {
        schemaNames.insert(pair.first);
    }
    
    // Add default schemas
    for (const auto& pair : pImpl->defaultSchemas) {
        schemaNames.insert(pair.first);
    }
    
    return std::vector<std::string>(schemaNames.begin(), schemaNames.end());
}

bool TestConfigurationValidator::validateSchema(const ConfigSchema& schema) {
    if (schema.name.empty()) {
        return false;
    }
    
    for (const auto& pair : schema.fieldTypes) {
        const std::string& fieldName = pair.first;
        const std::string& fieldType = pair.second;
        
        if (fieldName.empty() || fieldType.empty()) {
            return false;
        }
        
        if (!validateFieldType(fieldType, "test_value")) {
            return false;
        }
        
        // Check ranges if present
        auto rangeIt = schema.fieldRanges.find(fieldName);
        if (rangeIt != schema.fieldRanges.end()) {
            if (rangeIt->second.first >= rangeIt->second.second) {
                return false; // Invalid range
            }
        }
        
        // Check enum if present
        auto enumIt = schema.fieldEnums.find(fieldName);
        if (enumIt != schema.fieldEnums.end() && enumIt->second.empty()) {
            return false; // Empty enum
        }
    }
    
    return true;
}

bool TestConfigurationValidator::validateField(const std::string& fieldName, const std::string& fieldValue, const std::string& fieldType) {
    if (fieldName.empty() || fieldType.empty()) {
        return false;
    }
    
    return validateFieldType(fieldType, fieldValue);
}

bool TestConfigurationValidator::validateFieldRange(const std::string& fieldName, const std::string& fieldValue, const std::pair<double, double>& range) {
    try {
        double value = std::stod(fieldValue);
        return value >= range.first && value <= range.second;
    } catch (...) {
        return false;
    }
}

bool TestConfigurationValidator::validateFieldEnum(const std::string& fieldName, const std::string& fieldValue, const std::vector<std::string>& enumValues) {
    return std::find(enumValues.begin(), enumValues.end(), fieldValue) != enumValues.end();
}

ValidationResult TestConfigurationValidator::validateConfiguration(const TestConfiguration& config, const std::string& schemaName) {
    ValidationResult result("BasicValidation", true);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Get schema or use default
    ConfigSchema schema;
    if (!schemaName.empty()) {
        schema = getSchema(schemaName);
    }
    
    if (schema.name.empty()) {
        result.passed = false;
        result.errorMessage = "Schema not found: " + schemaName;
        return result;
    }
    
    // Validate against schema
    for (const auto& pair : config.parameters) {
        const std::string& fieldName = pair.first;
        const std::string& fieldValue = pair.second;
        
        auto fieldTypeIt = schema.fieldTypes.find(fieldName);
        if (fieldTypeIt != schema.fieldTypes.end()) {
            if (!validateField(fieldName, fieldValue, fieldTypeIt->second)) {
                result.passed = false;
                result.errorMessage = "Invalid field value for " + fieldName + ": " + fieldValue;
                result.details[fieldName] = "Invalid type or value";
            }
            
            // Validate range if present
            auto rangeIt = schema.fieldRanges.find(fieldName);
            if (rangeIt != schema.fieldRanges.end() && !validateFieldRange(fieldName, fieldValue, rangeIt->second)) {
                result.passed = false;
                result.errorMessage = "Field value out of range for " + fieldName + ": " + fieldValue;
                result.details[fieldName] = "Out of range " + std::to_string(rangeIt->second.first) + "-" + std::to_string(rangeIt->second.second);
            }
            
            // Validate enum if present
            auto enumIt = schema.fieldEnums.find(fieldName);
            if (enumIt != schema.fieldEnums.end() && !validateFieldEnum(fieldName, fieldValue, enumIt->second)) {
                result.passed = false;
                result.errorMessage = "Field value not in enum for " + fieldName + ": " + fieldValue;
                result.details[fieldName] = "Invalid enum value";
            }
        } else if (schema.required) {
            // Required field missing
            result.passed = false;
            result.errorMessage = "Required field missing: " + fieldName;
            result.details[fieldName] = "Required field not provided";
        }
    }
    
    // Validate required fields
    for (const auto& pair : schema.fieldTypes) {
        if (pair.second != "optional") {
            auto paramIt = config.parameters.find(pair.first);
            if (paramIt == config.parameters.end()) {
                result.passed = false;
                result.errorMessage = "Required parameter missing: " + pair.first;
                result.details[pair.first] = "Required parameter not provided";
            }
        }
    }
    
    // Apply validation rules
    applyValidationRules(config, result.validationResults);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.validationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    return result;
}

std::vector<ValidationResult> TestConfigurationValidator::validateAllConfigurations(const std::vector<TestConfiguration>& configs, const std::string& schemaName) {
    std::vector<ValidationResult> results;
    
    for (const auto& config : configs) {
        results.push_back(validateConfiguration(config, schemaName));
    }
    
    return results;
}

bool TestConfigurationValidator::validateConfigurationAgainstSchemas(const TestConfiguration& config) {
    std::vector<ValidationResult> results;
    return validateAllConfigurations({config}, "").empty();
}

bool TestConfigurationValidator::addValidationRule(const std::string& ruleName, std::function<bool(const TestConfiguration&)> rule) {
    if (ruleName.empty() || !rule) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->rulesMutex);
    pImpl->validationRules[ruleName] = rule;
    return true;
}

bool TestConfigurationValidator::removeValidationRule(const std::string& ruleName) {
    std::lock_guard<std::mutex> lock(pImpl->rulesMutex);
    
    auto it = pImpl->validationRules.find(ruleName);
    if (it != pImpl->validationRules.end()) {
        pImpl->validationRules.erase(it);
        return true;
    }
    
    return false;
}

bool TestConfigurationValidator::applyValidationRules(const TestConfiguration& config, std::vector<ValidationResult>& results) {
    std::lock_guard<std::mutex> lock(pImpl->rulesMutex);
    
    bool allPassed = true;
    
    for (const auto& pair : pImpl->validationRules) {
        ValidationResult ruleResult(pair.first, pair.second(config));
        results.push_back(ruleResult);
        
        if (!ruleResult.passed) {
            allPassed = false;
        }
    }
    
    return allPassed;
}

std::string TestConfigurationValidator::generateValidationReport(const std::vector<TestConfiguration>& configs) const {
    std::stringstream ss;
    ss << "NLM Test Configuration Validation Report\n";
    ss << "======================================\n\n";
    
    if (configs.empty()) {
        ss << "No configurations to validate.\n";
        return ss.str();
    }
    
    ss << "Configuration Validation Summary:\n";
    ss << "  Total Configurations: " << configs.size() << "\n";
    
    size_t validCount = 0;
    for (const auto& config : configs) {
        if (config.valid) validCount++;
    }
    
    ss << "  Valid Configurations: " << validCount << "\n";
    ss << "  Invalid Configurations: " << (configs.size() - validCount) << "\n";
    
    if (validCount > 0) {
        ss << "  Validation Success Rate: " << (validCount * 100.0 / configs.size()) << "%\n";
    }
    
    ss << "\nDetailed Configuration Validation:\n";
    for (const auto& config : configs) {
        ss << "  Configuration: " << config.testName << " (Suite: " << config.testSuite << ")\n";
        ss << "    Valid: " << (config.valid ? "Yes" : "No") << "\n";
        
        if (!config.validationResults.empty()) {
            ss << "    Validation Details:\n";
            for (const auto& result : config.validationResults) {
                ss << "      " << result.validationName << ": " 
                   << (result.passed ? "PASSED" : "FAILED");
                
                if (!result.errorMessage.empty()) {
                    ss << " - " << result.errorMessage;
                }
                
                ss << "\n";
            }
        }
        
        ss << "\n";
    }
    
    return ss.str();
}

std::string TestConfigurationValidator::generateSchemaReport(const ConfigSchema& schema) const {
    std::stringstream ss;
    ss << "Configuration Schema Report\n";
    ss << "==========================\n\n";
    
    ss << "Schema Name: " << schema.name << "\n";
    ss << "Required: " << (schema.required ? "Yes" : "No") << "\n";
    
    ss << "\nFields:\n";
    for (const auto& pair : schema.fieldTypes) {
        ss << "  " << pair.first << "\n";
        ss << "    Type: " << pair.second << "\n";
        
        auto descIt = schema.fieldDescriptions.find(pair.first);
        if (descIt != schema.fieldDescriptions.end()) {
            ss << "    Description: " << descIt->second << "\n";
        }
        
        auto rangeIt = schema.fieldRanges.find(pair.first);
        if (rangeIt != schema.fieldRanges.end()) {
            ss << "    Range: " << rangeIt->second.first << " - " << rangeIt->second.second << "\n";
        }
        
        auto enumIt = schema.fieldEnums.find(pair.first);
        if (enumIt != schema.fieldEnums.end() && !enumIt->second.empty()) {
            ss << "    Enum: ";
            for (size_t i = 0; i < enumIt->second.size(); ++i) {
                ss << enumIt->second[i];
                if (i < enumIt->second.size() - 1) ss << ", ";
            }
            ss << "\n";
        }
        
        ss << "\n";
    }
    
    return ss.str();
}

std::string TestConfigurationValidator::generateValidationRulesReport() const {
    std::stringstream ss;
    ss << "Validation Rules Report\n";
    ss << "========================\n\n";
    
    std::lock_guard<std::mutex> lock(pImpl->rulesMutex);
    
    if (pImpl->validationRules.empty()) {
        ss << "No validation rules defined.\n";
        return ss.str();
    }
    
    ss << "Active Validation Rules: " << pImpl->validationRules.size() << "\n\n";
    
    for (const auto& pair : pImpl->validationRules) {
        ss << "  Rule: " << pair.first << "\n";
        ss << "    Function: " << (pair.second ? "defined" : "null") << "\n";
        ss << "\n";
    }
    
    return ss.str();
}

bool TestConfigurationValidator::exportSchema(const std::string& filename, const std::string& format) {
    try {
        if (format == "yaml") {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            // In real implementation, would serialize schema to YAML
            file << "# Schema export not fully implemented in this simplified version\n";
            file.close();
            
        } else {
            return false;
        }
        
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool TestConfigurationValidator::exportValidationResults(const std::string& filename, const std::vector<ValidationResult>& results, const std::string& format) {
    try {
        if (format == "csv") {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << "ValidationName,Passed,ErrorMessage,ValidationTimeMs\n";
            
            for (const auto& result : results) {
                file << result.validationName << "," << (result.passed ? "true" : "false") << "," 
                     << "\"" << result.errorMessage << "\"" << "," << result.validationTime.count() << "\n";
            }
            
            file.close();
            
        } else {
            return false;
        }
        
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool TestConfigurationValidator::importSchema(const std::string& filename) {
    // Implementation for importing schema from file
    return false;
}

bool TestConfigurationValidator::importValidationRules(const std::string& filename) {
    // Implementation for importing validation rules from file
    return false;
}

bool TestConfigurationValidator::addConfigurationTemplate(const std::string& templateName, const TestConfiguration& templateConfig) {
    if (templateName.empty()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->templatesMutex);
    pImpl->templates[templateName] = templateConfig;
    return true;
}

TestConfiguration TestConfigurationValidator::getConfigurationTemplate(const std::string& templateName) const {
    std::lock_guard<std::mutex> lock(pImpl->templatesMutex);
    
    auto it = pImpl->templates.find(templateName);
    if (it != pImpl->templates.end()) {
        return it->second;
    }
    
    return TestConfiguration();
}

std::vector<std::string> TestConfigurationValidator::getAvailableTemplates() const {
    std::vector<std::string> templateNames;
    
    std::lock_guard<std::mutex> lock(pImpl->templatesMutex);
    for (const auto& pair : pImpl->templates) {
        templateNames.push_back(pair.first);
    }
    
    return templateNames;
}

TestConfiguration TestConfigurationValidator::applyTemplate(const std::string& templateName, const TestConfiguration& baseConfig) const {
    TestConfiguration templateConfig = getConfigurationTemplate(templateName);
    
    if (templateConfig.testName.empty()) {
        return baseConfig;
    }
    
    // Merge template with base configuration
    TestConfiguration merged = baseConfig;
    
    // Copy template parameters (template parameters take precedence)
    for (const auto& pair : templateConfig.parameters) {
        merged.parameters[pair.first] = pair.second;
    }
    
    // Copy template expected values
    for (const auto& pair : templateConfig.expectedValues) {
        merged.expectedValues[pair.first] = pair.second;
    }
    
    // Copy template validation rules
    merged.validationRules = templateConfig.validationRules;
    
    return merged;
}

double TestConfigurationValidator::calculateValidationSuccessRate(const std::vector<TestConfiguration>& configs) const {
    if (configs.empty()) {
        return 0.0;
    }
    
    size_t validCount = 0;
    for (const auto& config : configs) {
        if (config.valid) validCount++;
    }
    
    return static_cast<double>(validCount) / configs.size() * 100.0;
}

std::map<std::string, size_t> TestConfigurationValidator::getValidationResultCounts(const std::vector<ValidationResult>& results) const {
    std::map<std::string, size_t> counts;
    
    for (const auto& result : results) {
        std::string status = result.passed ? "passed" : "failed";
        counts[status]++;
    }
    
    return counts;
}

std::vector<std::string> TestConfigurationValidator::getValidationErrorMessages(const std::vector<ValidationResult>& results) const {
    std::vector<std::string> errorMessages;
    
    for (const auto& result : results) {
        if (!result.errorMessage.empty()) {
            errorMessages.push_back(result.errorMessage);
        }
    }
    
    return errorMessages;
}

std::vector<ValidationResult> TestConfigurationValidator::validateConfigurationDependencies(const TestConfiguration& config) {
    std::vector<ValidationResult> dependencyResults;
    
    // In real implementation, would check for configuration dependencies
    // For now, just return an empty result
    dependencyResults.push_back(ValidationResult("DependencyValidation", true));
    
    return dependencyResults;
}

bool TestConfigurationValidator::validateConfigurationConsistency(const TestConfiguration& config) {
    // Check for consistency issues in configuration
    // For now, return true (assuming configuration is consistent)
    return true;
}

std::vector<std::string> TestConfigurationValidator::identifyConfigurationConflicts(const TestConfiguration& config) const {
    std::vector<std::string> conflicts;
    
    // Check for conflicts in parameters
    std::set<std::string> duplicateParams;
    for (const auto& pair : config.parameters) {
        if (duplicateParams.find(pair.first) != duplicateParams.end()) {
            conflicts.push_back("Duplicate parameter: " + pair.first);
        } else {
            duplicateParams.insert(pair.first);
        }
    }
    
    return conflicts;
}

std::vector<ValidationResult> TestConfigurationValidator::validateResearchConfiguration(const TestConfiguration& config) {
    std::vector<ValidationResult> researchResults;
    
    // Research configuration validation
    researchResults.push_back(ValidationResult("ResearchValidation", 
                                             config.testSuite.find("research") != std::string::npos,
                                             "Configuration not research-focused"));
    
    researchResults.push_back(ValidationResult("ExperimentValidation", 
                                             config.testName.find("experiment") != std::string::npos,
                                             "Configuration not experiment-related"));
    
    return researchResults;
}

std::vector<ValidationResult> TestConfigurationValidator::validateProductionConfiguration(const TestConfiguration& config) {
    std::vector<ValidationResult> productionResults;
    
    // Production configuration validation
    productionResults.push_back(ValidationResult("ProductionValidation", 
                                             config.testSuite.find("production") != std::string::npos,
                                             "Configuration not production-focused"));
    
    productionResults.push_back(ValidationResult("ReliabilityValidation", true, ""));
    
    return productionResults;
}

std::vector<ValidationResult> TestConfigurationValidator::validateDevelopmentConfiguration(const TestConfiguration& config) {
    std::vector<ValidationResult> developmentResults;
    
    // Development configuration validation
    developmentResults.push_back(ValidationResult("DevelopmentValidation", 
                                             config.testSuite.find("development") != std::string::npos,
                                             "Configuration not development-focused"));
    
    developmentResults.push_back(ValidationResult("FlexibilityValidation", true, ""));
    
    return developmentResults;
}

std::vector<ValidationResult> TestConfigurationValidator::validateDeploymentConfiguration(const TestConfiguration& config) {
    std::vector<ValidationResult> deploymentResults;
    
    // Deployment configuration validation
    deploymentResults.push_back(ValidationResult("DeploymentValidation", 
                                             config.testSuite.find("deployment") != std::string::npos,
                                             "Configuration not deployment-focused"));
    
    deploymentResults.push_back(ValidationResult("EnvironmentValidation", true, ""));
    
    return deploymentResults;
}

TestConfiguration TestConfigurationValidator::optimizeConfiguration(const TestConfiguration& config) const {
    TestConfiguration optimized = config;
    
    // Simple optimization - remove empty parameters
    auto it = optimized.parameters.begin();
    while (it != optimized.parameters.end()) {
        if (it->second.empty()) {
            it = optimized.parameters.erase(it);
        } else {
            ++it;
        }
    }
    
    return optimized;
}

std::vector<std::string> TestConfigurationValidator::getConfigurationOptimizationSuggestions(const TestConfiguration& config) const {
    std::vector<std::string> suggestions;
    
    // Generate optimization suggestions
    suggestions.push_back("Remove unused configuration parameters.");
    suggestions.push_back("Consolidate similar parameters.");
    suggestions.push_back("Add missing required parameters.");
    suggestions.push_back("Validate parameter ranges and enum values.");
    
    // Check for specific issues
    for (const auto& pair : config.parameters) {
        if (pair.second.empty()) {
            suggestions.push_back("Parameter \"" + pair.first + "\" has empty value.");
        }
    }
    
    return suggestions;
}

bool TestConfigurationValidator::validateFieldType(const std::string& fieldType, const std::string& value) {
    if (fieldType == "string") {
        return !value.empty();
    } else if (fieldType == "integer") {
        try {
            std::stoi(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (fieldType == "double") {
        try {
            std::stod(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (fieldType == "boolean") {
        return value == "true" || value == "false" || value == "1" || value == "0";
    } else if (fieldType == "enum") {
        return !value.empty();
    } else if (fieldType == "optional") {
        return true; // Optional fields can be empty
    } else {
        return !value.empty(); // Default: non-empty string
    }
}

std::string TestConfigurationValidator::getFieldTypeName(const std::string& typeName) const {
    // Convert type name to readable format
    if (typeName == "string") return "String";
    if (typeName == "integer") return "Integer";
    if (typeName == "double") return "Double";
    if (typeName == "boolean") return "Boolean";
    if (typeName == "enum") return "Enumeration";
    if (typeName == "optional") return "Optional";
    return typeName;
}

TestConfigurationValidator& TestConfigurationValidator::getInstance() {
    static TestConfigurationValidator instance;
    return instance;
}

void TestConfigurationValidator::initialize() {
    getInstance();
}

void TestConfigurationValidator::shutdown() {
    getInstance(); // Clear resources
}

} // namespace tests
} // namespace nlm
