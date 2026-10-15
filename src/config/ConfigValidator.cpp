#include "ConfigValidator.hpp"
#include "../core/Logger/Logger.hpp"
#include <yaml-cpp/yaml.h>
#include <json/json.h>
#include <algorithm>
#include <unordered_map>

namespace nlm {

struct ConfigValidator::Impl {
    // Schema storage
    struct Schema {
        std::string name;
        std::string definition;
        std::string format; // "yaml" or "json"
        std::vector<ValidationRule> rules;
        std::map<std::string, std::string> constraints;
        bool isActive;
        
        Schema() : isActive(true) {}
    };
    
    std::unordered_map<std::string, Schema> schemas;
    
    // Custom validators
    std::vector<std::pair<std::string, std::function<ValidationResult(const Config&)>>> customValidators;
    
    // Validation statistics
    struct ValidationStats {
        size_t totalValidations;
        size_t successfulValidations;
        std::chrono::system_clock::time_point lastValidation;
        std::map<std::string, size_t> schemaValidationCounts;
    };
    
    ValidationStats stats;
    
    Logger* logger;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {
    pImpl->logger = Logger::getInstance();
    pImpl->stats.totalValidations = 0;
    pImpl->stats.successfulValidations = 0;
    pImpl->stats.lastValidation = std::chrono::system_clock::now();
}

ConfigValidator::~ConfigValidator() {
    shutdown();
}

ConfigValidator::ConfigValidator(ConfigValidator&& other) noexcept : pImpl(std::move(other.pImpl)) {}

ConfigValidator& ConfigValidator::operator=(ConfigValidator&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool ConfigValidator::initialize(const std::string& schemaFile) {
    bool success = true;
    
    if (!schemaFile.empty()) {
        success = loadSchemaFromYAML(schemaFile);
    }
    
    pImpl->logger->info("Configuration validator initialized");
    return success;
}

void ConfigValidator::update(double timestep) {
    pImpl->stats.lastValidation = std::chrono::system_clock::now();
}

void ConfigValidator::shutdown() {
    pImpl->logger->info("Configuration validator shutdown");
    pImpl->schemas.clear();
    pImpl->customValidators.clear();
}

ValidationResult ConfigValidator::validate(const Config& config) const {
    ValidationResult result;
    pImpl->stats.totalValidations++;
    
    // Validate against all active schemas
    for (const auto& schemaPair : pImpl->schemas) {
        if (!schemaPair.second.isActive) continue;
        
        ValidationResult schemaResult = validateConfigAgainstSchema(config, schemaPair.first);
        if (!schemaResult.isValid) {
            result.isValid = false;
            result.errors.insert(result.errors.end(), schemaResult.errors.begin(), schemaResult.errors.end());
            result.warnings.insert(result.warnings.end(), schemaResult.warnings.begin(), schemaResult.warnings.end());
        } else {
            pImpl->stats.schemaValidationCounts[schemaPair.first]++;
            pImpl->stats.successfulValidations++;
        }
    }
    
    // Run custom validators
    for (const auto& validator : pImpl->customValidators) {
        ValidationResult customResult = validator.second(config);
        if (!customResult.isValid) {
            result.isValid = false;
            result.errors.insert(result.errors.end(), customResult.errors.begin(), customResult.errors.end());
            result.warnings.insert(result.warnings.end(), customResult.warnings.begin(), customResult.warnings.end());
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validate(const std::string& configKey, const ConfigValue& value) const {
    ValidationResult result;
    
    // Validate specific key-value pair
    for (const auto& schemaPair : pImpl->schemas) {
        if (!schemaPair.second.isActive) continue;
        
        ValidationRule rule = findRuleForKey(schemaPair.second, configKey);
        if (rule.ruleName.empty()) continue;
        
        if (!validateValueAgainstRule(value, rule)) {
            result.isValid = false;
            result.errors.push_back("Validation failed for key '" + configKey + "': " + rule.errorMessage);
        }
    }
    
    return result;
}

bool ConfigValidator::addSchema(const std::string& schemaName, const std::string& schemaDefinition) {
    if (schemaName.empty() || schemaDefinition.empty()) {
        pImpl->logger->error("Invalid schema name or definition");
        return false;
    }
    
    // Detect schema format
    std::string format;
    if (schemaDefinition.find("required:") != std::string::npos || 
        schemaDefinition.find("properties:") != std::string::npos) {
        format = "yaml";
    } else if (schemaDefinition.find("required") != std::string::npos || 
               schemaDefinition.find("properties") != std::string::npos) {
        format = "json";
    }
    
    if (format.empty()) {
        pImpl->logger->error("Unknown schema format for: " + schemaName);
        return false;
    }
    
    Schema schema;
    schema.name = schemaName;
    schema.definition = schemaDefinition;
    schema.format = format;
    
    pImpl->schemas[schemaName] = schema;
    pImpl->logger->info("Schema added: " + schemaName);
    return true;
}

bool ConfigValidator::removeSchema(const std::string& schemaName) {
    auto it = pImpl->schemas.find(schemaName);
    if (it != pImpl->schemas.end()) {
        pImpl->schemas.erase(it);
        pImpl->logger->info("Schema removed: " + schemaName);
        return true;
    }
    
    pImpl->logger->warning("Schema not found: " + schemaName);
    return false;
}

bool ConfigValidator::updateSchema(const std::string& schemaName, const std::string& schemaDefinition) {
    auto it = pImpl->schemas.find(schemaName);
    if (it == pImpl->schemas.end()) {
        pImpl->logger->error("Schema not found: " + schemaName);
        return false;
    }
    
    // Update definition
    std::string format;
    if (schemaDefinition.find("required:") != std::string::npos || 
        schemaDefinition.find("properties:") != std::string::npos) {
        format = "yaml";
    } else if (schemaDefinition.find("required") != std::string::npos || 
               schemaDefinition.find("properties") != std::string::npos) {
        format = "json";
    }
    
    it->second.definition = schemaDefinition;
    it->second.format = format;
    
    pImpl->logger->info("Schema updated: " + schemaName);
    return true;
}

std::vector<std::string> ConfigValidator::getAvailableSchemas() const {
    std::vector<std::string> names;
    names.reserve(pImpl->schemas.size());
    for (const auto& schemaPair : pImpl->schemas) {
        names.push_back(schemaPair.first);
    }
    return names;
}

bool ConfigValidator::addCustomValidator(const std::string& name, 
                                        std::function<ValidationResult(const Config&)> validator) {
    if (name.empty() || !validator) {
        pImpl->logger->error("Invalid custom validator name or function");
        return false;
    }
    
    // Check for duplicate
    for (const auto& existing : pImpl->customValidators) {
        if (existing.first == name) {
            pImpl->logger->warning("Custom validator already exists: " + name);
            return false;
        }
    }
    
    pImpl->customValidators.push_back({name, validator});
    pImpl->logger->info("Custom validator added: " + name);
    return true;
}

bool ConfigValidator::removeCustomValidator(const std::string& name) {
    auto it = std::find_if(pImpl->customValidators.begin(), pImpl->customValidators.end(),
                          [&name](const auto& pair) { return pair.first == name; });
    if (it != pImpl->customValidators.end()) {
        pImpl->customValidators.erase(it);
        pImpl->logger->info("Custom validator removed: " + name);
        return true;
    }
    
    pImpl->logger->warning("Custom validator not found: " + name);
    return false;
}

std::vector<std::string> ConfigValidator::getCustomValidators() const {
    std::vector<std::string> names;
    names.reserve(pImpl->customValidators.size());
    for (const auto& validator : pImpl->customValidators) {
        names.push_back(validator.first);
    }
    return names;
}

std::vector<std::string> ConfigValidator::getRecommendations(const Config& config) const {
    std::vector<std::string> recommendations;
    
    // Check for missing required configurations
    for (const auto& schemaPair : pImpl->schemas) {
        if (!schemaPair.second.isActive) continue;
        
        std::vector<std::string> missing = findMissingRequiredKeys(config, schemaPair.second);
        for (const auto& missingKey : missing) {
            recommendations.push_back("Consider setting required configuration: " + missingKey);
        }
    }
    
    return recommendations;
}

std::map<std::string, std::string> ConfigValidator::getDependencyGraph() const {
    std::map<std::string, std::string> dependencies;
    
    // Build dependency graph from schemas
    for (const auto& schemaPair : pImpl->schemas) {
        // Parse schema to extract dependencies
        // This is a simplified implementation
        dependencies[schemaPair.first] = "dependencies_placeholder";
    }
    
    return dependencies;
}

size_t ConfigValidator::getTotalValidations() const {
    return pImpl->stats.totalValidations;
}

size_t ConfigValidator::getSuccessfulValidations() const {
    return pImpl->stats.successfulValidations;
}

double ConfigValidator::getValidationSuccessRate() const {
    if (pImpl->stats.totalValidations == 0) return 0.0;
    return static_cast<double>(pImpl->stats.successfulValidations) / pImpl->stats.totalValidations;
}

std::string ConfigValidator::getValidationStats() const {
    std::stringstream ss;
    ss << "Validation Statistics:\n";
    ss << "===================\n";
    ss << "Total validations: " << pImpl->stats.totalValidations << "\n";
    ss << "Successful validations: " << pImpl->stats.successfulValidations << "\n";
    ss << "Success rate: " << getValidationSuccessRate() * 100 << "%\n";
    ss << "Active schemas: " << pImpl->schemas.size() << "\n";
    ss << "Custom validators: " << pImpl->customValidators.size() << "\n";
    
    ss << "\nSchema validation counts:\n";
    for (const auto& countPair : pImpl->stats.schemaValidationCounts) {
        ss << "  " << countPair.first << ": " << countPair.second << "\n";
    }
    
    return ss.str();
}

bool ConfigValidator::loadSchemaFromYAML(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file) {
            pImpl->logger->error("Failed to open schema file: " + filepath);
            return false;
        }
        
        YAML::Node root = YAML::Load(file);
        
        for (auto it = root.begin(); it != root.end(); ++it) {
            std::string schemaName = it->first.as<std::string>();
            std::string schemaDefinition = YAML::Dump(*it);
            
            addSchema(schemaName, schemaDefinition);
        }
        
        pImpl->logger->info("Schemas loaded from YAML: " + filepath);
        return true;
    } catch (const YAML::Exception& e) {
        pImpl->logger->error("YAML parsing error in schema file: " + std::string(e.what()));
        return false;
    }
}

bool ConfigValidator::loadSchemaFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file) {
            pImpl->logger->error("Failed to open JSON schema file: " + filepath);
            return false;
        }
        
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::string errors;
        
        if (!builder(&file, &file, &root, &errors)) {
            pImpl->logger->error("JSON parsing error: " + errors);
            return false;
        }
        
        for (auto it = root.begin(); it != root.end(); ++it) {
            std::string schemaName = it->asString();
            
            // Get schema definition (assuming it's nested)
            std::string schemaDefinition;
            if ((*it).isObject() && (*it).isMember("definition")) {
                schemaDefinition = (*it)["definition"].asString();
            }
            
            addSchema(schemaName, schemaDefinition);
        }
        
        pImpl->logger->info("Schemas loaded from JSON: " + filepath);
        return true;
    } catch (const std::exception& e) {
        pImpl->logger->error("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

std::string ConfigValidator::exportSchemaToYAML() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& schemaPair : pImpl->schemas) {
        emitter << YAML::Key << schemaPair.first;
        
        // Try to parse as YAML first, otherwise use JSON
        try {
            YAML::Node node = YAML::Load(schemaPair.second.definition);
            emitter << node;
        } catch (const YAML::Exception& e) {
            // If not valid YAML, use as raw string
            emitter << schemaPair.second.definition;
        }
    }
    
    emitter << YAML::EndMap;
    
    return emitter.c_str();
}

std::string ConfigValidator::exportSchemaToJSON() const {
    Json::Value root;
    
    for (const auto& schemaPair : pImpl->schemas) {
        // Try to parse as JSON first, otherwise use YAML
        try {
            Json::Value node;
            Json::CharReaderBuilder builder;
            std::string errors;
            std::istringstream stream(schemaPair.second.definition);
            
            if (builder(&stream, &stream, &node, &errors)) {
                root[schemaPair.first] = node;
            } else {
                // If not valid JSON, store as string
                root[schemaPair.first]["definition"] = schemaPair.second.definition;
            }
        } catch (const std::exception& e) {
            // Store as string if parsing fails
            root[schemaPair.first]["definition"] = schemaPair.second.definition;
        }
    }
    
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "  ";
    
    std::stringstream ss;
    std::unique_ptr<Json::StreamWriter> writerInstance(writer.newStreamWriter());
    writerInstance->write(root, &ss);
    
    return ss.str();
}

bool ConfigValidator::addConstraint(const std::string& constraintName, 
                                    std::function<bool(const Config&)> constraint) {
    if (constraintName.empty() || !constraint) {
        pImpl->logger->error("Invalid constraint name or function");
        return false;
    }
    
    // Note: This would typically store constraints in a separate system
    // For now, we just log it as custom validation
    pImpl->logger->info("Constraint registered: " + constraintName);
    return addCustomValidator(constraintName, constraint);
}

bool ConfigValidator::removeConstraint(const std::string& constraintName) {
    return removeCustomValidator(constraintName);
}

std::vector<std::string> ConfigValidator::getConstraints() const {
    return getCustomValidators();
}

bool ConfigValidator::addValidationRule(const std::string& schemaName, const ValidationRule& rule) {
    auto it = pImpl->schemas.find(schemaName);
    if (it == pImpl->schemas.end()) {
        pImpl->logger->error("Schema not found: " + schemaName);
        return false;
    }
    
    // Check for duplicate rule
    for (const auto& existingRule : it->second.rules) {
        if (existingRule.ruleName == rule.ruleName) {
            pImpl->logger->warning("Rule already exists in schema: " + rule.ruleName);
            return false;
        }
    }
    
    it->second.rules.push_back(rule);
    pImpl->logger->info("Validation rule added: " + rule.ruleName + " in schema " + schemaName);
    return true;
}

bool ConfigValidator::removeValidationRule(const std::string& schemaName, const std::string& ruleName) {
    auto it = pImpl->schemas.find(schemaName);
    if (it == pImpl->schemas.end()) {
        pImpl->logger->error("Schema not found: " + schemaName);
        return false;
    }
    
    auto ruleIt = std::find_if(it->second.rules.begin(), it->second.rules.end(),
                              [&ruleName](const ValidationRule& r) { return r.ruleName == ruleName; });
    if (ruleIt != it->second.rules.end()) {
        it->second.rules.erase(ruleIt);
        pImpl->logger->info("Validation rule removed: " + ruleName + " from schema " + schemaName);
        return true;
    }
    
    pImpl->logger->warning("Validation rule not found: " + ruleName + " in schema " + schemaName);
    return false;
}

std::vector<ConfigValidator::ValidationRule> ConfigValidator::getValidationRules(const std::string& schemaName) const {
    auto it = pImpl->schemas.find(schemaName);
    if (it == pImpl->schemas.end()) {
        return std::vector<ValidationRule>();
    }
    
    return it->second.rules;
}

ValidationResult ConfigValidator::validateConfigAgainstSchema(const Config& config, const std::string& schemaName) const {
    ValidationResult result;
    
    auto schemaIt = pImpl->schemas.find(schemaName);
    if (schemaIt == pImpl->schemas.end() || !schemaIt->second.isActive) {
        result.isValid = false;
        result.errors.push_back("Schema not found or inactive: " + schemaName);
        return result;
    }
    
    // Validate against each rule in the schema
    for (const auto& rule : schemaIt->second.rules) {
        ValidationResult ruleResult = validateRule(config, rule);
        if (!ruleResult.isValid) {
            result.isValid = false;
            result.errors.insert(result.errors.end(), ruleResult.errors.begin(), ruleResult.errors.end());
            result.warnings.insert(result.warnings.end(), ruleResult.warnings.begin(), ruleResult.warnings.end());
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateRule(const Config& config, const ValidationRule& rule) const {
    ValidationResult result;
    
    if (rule.ruleType == "required") {
        if (!config.has(rule.ruleDefinition)) {
            result.isValid = false;
            result.errors.push_back("Required configuration missing: " + rule.ruleDefinition);
        }
    } else if (rule.ruleType == "range") {
        // Parse range definition (e.g., "min=0,max=100")
        double minVal = -std::numeric_limits<double>::max();
        double maxVal = std::numeric_limits<double>::max();
        
        // Simple parsing - in real implementation would use proper parser
        size_t minPos = rule.ruleDefinition.find("min=");
        if (minPos != std::string::npos) {
            size_t commaPos = rule.ruleDefinition.find(",", minPos);
            std::string minStr = rule.ruleDefinition.substr(minPos + 4, commaPos - minPos - 4);
            minVal = std::stod(minStr);
        }
        
        size_t maxPos = rule.ruleDefinition.find("max=");
        if (maxPos != std::string::npos) {
            std::string maxStr = rule.ruleDefinition.substr(maxPos + 4);
            maxVal = std::stod(maxStr);
        }
        
        if (config.has(rule.ruleDefinition)) {
            auto value = config.get<ConfigValue>(rule.ruleDefinition);
            if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                if (val < minVal || val > maxVal) {
                    result.isValid = false;
                    result.errors.push_back("Value out of range for '" + rule.ruleDefinition + "': " + 
                                         rule.errorMessage.empty() ? rule.errorMessage : 
                                         ("Value must be between " + std::to_string(minVal) + " and " + std::to_string(maxVal)));
                }
            }
        }
    } else if (rule.ruleType == "enum") {
        // Parse enum values
        std::vector<std::string> allowedValues = splitString(rule.ruleDefinition, ',');
        if (config.has(rule.ruleDefinition)) {
            auto value = config.get<ConfigValue>(rule.ruleDefinition);
            std::string strValue;
            if (std::holds_alternative<std::string>(value)) {
                strValue = std::get<std::string>(value);
            } else if (std::holds_alternative<int>(value)) {
                strValue = std::to_string(std::get<int>(value));
            }
            
            bool found = false;
            for (const auto& allowed : allowedValues) {
                if (strValue == allowed) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                result.isValid = false;
                result.errors.push_back("Value not in allowed list for '" + rule.ruleDefinition + "': " + rule.errorMessage);
            }
        }
    } else if (rule.ruleType == "pattern") {
        // Simple pattern matching (regex-like)
        if (config.has(rule.ruleDefinition)) {
            auto value = config.get<ConfigValue>(rule.ruleDefinition);
            if (std::holds_alternative<std::string>(value)) {
                std::string strValue = std::get<std::string>(value);
                if (!matchPattern(strValue, rule.ruleDefinition)) {
                    result.isValid = false;
                    result.errors.push_back("Value does not match pattern for '" + rule.ruleDefinition + "': " + rule.errorMessage);
                }
            }
        }
    }
    
    return result;
}

ValidationRule ConfigValidator::findRuleForKey(const Schema& schema, const std::string& key) const {
    ValidationRule rule;
    
    for (const auto& schemaRule : schema.rules) {
        // In real implementation, this would map keys to rules
        // For simplicity, we return a placeholder
        if (schemaRule.ruleDefinition == key) {
            return schemaRule;
        }
    }
    
    return rule;
}

std::vector<std::string> ConfigValidator::findMissingRequiredKeys(const Config& config, const Schema& schema) const {
    std::vector<std::string> missing;
    
    for (const auto& rule : schema.rules) {
        if (rule.ruleType == "required" && !config.has(rule.ruleDefinition)) {
            missing.push_back(rule.ruleDefinition);
        }
    }
    
    return missing;
}

bool ConfigValidator::validateValueAgainstRule(const ConfigValue& value, const ValidationRule& rule) const {
    if (rule.ruleType == "required") {
        return !value.valueless_by_exception();
    }
    
    // Additional validation logic would go here
    return true;
}

bool ConfigValidator::matchPattern(const std::string& value, const std::string& pattern) const {
    // Simple pattern matching - in real implementation would use proper regex
    if (pattern == "email") {
        return value.find("@") != std::string::npos && value.find(".") != std::string::npos;
    } else if (pattern == "positive_number") {
        return !value.empty() && std::all_of(value.begin(), value.end(), ::isdigit);
    }
    
    return value == pattern; // Exact match for other patterns
}

std::vector<std::string> ConfigValidator::splitString(const std::string& str, char delimiter) const {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    
    tokens.push_back(str.substr(start));
    return tokens;
}

} // namespace nlm
