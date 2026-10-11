#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Config.hpp"
#include "ConfigEntry.hpp"

namespace nlm {

using json = nlohmann::json;

class ConfigValidator {
public:
    using ValidationRule = std::function<bool(const ConfigValue&)>;
    using SchemaRule = std::function<bool(const ConfigValue&, const std::string& key)>;
    
    struct ValidationError {
        std::string key;
        std::string message;
        std::string ruleType;
        
        ValidationError(const std::string& k, const std::string& msg, const std::string& type)
            : key(k), message(msg), ruleType(type) {}
    };
    
    struct Schema {
        std::string name;
        std::vector<ValidationRule> rules;
        std::vector<SchemaRule> schemaRules;
        std::unordered_map<std::string, std::string> typeHints;
        
        Schema(const std::string& n = "Default") : name(n) {}
        
        void addRule(const ValidationRule& rule) {
            rules.push_back(rule);
        }
        
        void addSchemaRule(const SchemaRule& rule) {
            schemaRules.push_back(rule);
        }
        
        void setTypeHint(const std::string& key, const std::string& typeHint) {
            typeHints[key] = typeHint;
        }
        
        bool validate(const Config& config) const {
            std::vector<std::string> errors = validate(config, std::vector<std::string>());
            return errors.empty();
        }
        
        std::vector<std::string> validate(const Config& config, const std::vector<std::string>& existingErrors) const {
            std::vector<std::string> errors = existingErrors;
            
            for (const auto& rule : rules) {
                // This is a simplified validation - in practice, we'd need more sophisticated rule checking
                // For now, we'll just validate that the rule doesn't throw
                try {
                    // We can't actually apply generic validation rules without knowing what they validate
                    // So this is a placeholder for rule implementation
                } catch (...) {
                    errors.push_back("Validation rule failed");
                }
            }
            
            return errors;
        }
    };
    
    // Main validation methods
    bool validate(const Config& config) const {
        std::vector<std::string> errors = validate(config, std::vector<std::string>());
        return errors.empty();
    }
    
    std::vector<std::string> validate(const Config& config, const std::vector<std::string>& existingErrors = {}) const {
        std::vector<std::string> errors = existingErrors;
        
        for (const auto& rule : validationRules) {
            if (!rule(config)) {
                errors.push_back(rule.errorMessage);
            }
        }
        
        for (const auto& schemaRule : schemaRules) {
            // Apply schema rules if any
            // For now, this is a placeholder
        }
        
        return errors;
    }
    
    // Rule management
    void addValidationRule(const std::string& key, const ValidationRule& rule, const std::string& errorMessage) {
        validationRules.emplace_back(key, rule, errorMessage);
    }
    
    void addSchemaRule(const SchemaRule& rule) {
        schemaRules.push_back(rule);
    }
    
    // Schema management
    void addSchema(const Schema& schema) {
        schemas.push_back(schema);
    }
    
    bool useSchema(const std::string& schemaName) {
        for (const auto& schema : schemas) {
            if (schema.name == schemaName) {
                currentSchema = schema;
                return true;
            }
        }
        return false;
    }
    
    // Schema file loading
    bool loadSchema(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            json schemaJson;
            file >> schemaJson;
            
            // Parse schema JSON and populate schemas
            // This is a simplified implementation
            Schema schema;
            
            if (schemaJson.contains("name")) {
                schema.name = schemaJson["name"].get<std::string>();
            }
            
            // Parse validation rules
            if (schemaJson.contains("validation_rules")) {
                auto rules = schemaJson["validation_rules"];
                for (const auto& ruleItem : rules.items()) {
                    // Simplified rule parsing
                    schema.rules.push_back([](const ConfigValue&) { return true; });
                }
            }
            
            schemas.push_back(schema);
            currentSchema = schema;
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Configuration validation
    std::vector<ValidationError> validateConfig(const Config& config) const {
        std::vector<ValidationError> errors;
        
        for (const auto& entry : config.getKeys()) {
            std::vector<std::string> keyErrors = validateKey(config, entry);
            for (const auto& error : keyErrors) {
                errors.emplace_back(entry, error, "key_validation");
            }
        }
        
        return errors;
    }
    
    // Type validation
    std::vector<std::string> validateKey(const Config& config, const std::string& key) const {
        std::vector<std::string> errors;
        
        if (!config.has(key)) {
            errors.push_back("Key does not exist");
            return errors;
        }
        
        auto value = config.get<std::string>(key);
        if (!value) {
            errors.push_back("Key has no value");
            return errors;
        }
        
        // Type validation based on key patterns
        if (validateKeyType(key, *value)) {
            return errors;  // No errors
        }
        
        errors.push_back("Value type does not match key pattern");
        return errors;
    }
    
    // Key type validation
    bool validateKeyType(const std::string& key, const std::string& value) const {
        // Basic type inference based on key patterns
        if (key.find("enable") != std::string::npos || key.find("enabled") != std::string::npos) {
            // Boolean values
            return (value == "true" || value == "false" || 
                   value == "1" || value == "0" ||
                   value == "yes" || value == "no");
        }
        
        if (key.find("count") != std::string::npos || key.find("size") != std::string::npos ||
            key.find("capacity") != std::string::npos || key.find("interval") != std::string::npos) {
            // Integer values
            try {
                size_t pos;
                int64_t num = std::stoll(value, &pos);
                return pos == value.length() && num > 0;
            } catch (...) {
                return false;
            }
        }
        
        if (key.find("rate") != std::string::npos || key.find("scale") != std::string::npos ||
            key.find("weight") != std::string::npos || key.find("probability") != std::string::npos ||
            key.find("timestep") != std::string::npos) {
            // Float values
            try {
                size_t pos;
                double num = std::stod(value, &pos);
                return pos == value.length();
            } catch (...) {
                return false;
            }
        }
        
        // Default to string for unknown key types
        return true;
    }
    
    // Schema validation
    bool validateAgainstSchema(const Config& config, const Schema& schema) const {
        for (const auto& rule : schema.rules) {
            if (!rule(config)) {
                return false;
            }
        }
        return true;
    }
    
    // Get validation statistics
    struct ValidationStats {
        size_t totalRules;
        size_t validEntries;
        size_t invalidEntries;
        double validationScore;
        
        ValidationStats(size_t total, size_t valid, size_t invalid)
            : totalRules(total), validEntries(valid), invalidEntries(invalid),
              validationScore(total > 0 ? static_cast<double>(valid) / total : 0.0) {}
    };
    
    ValidationStats getValidationStats(const Config& config) const {
        size_t validCount = 0;
        size_t totalCount = 0;
        
        for (const auto& key : config.getKeys()) {
            totalCount++;
            if (validateKey(config, key).empty()) {
                validCount++;
            }
        }
        
        return ValidationStats(totalCount, validCount, totalCount - validCount);
    }
    
    // Export validation rules as JSON
    json exportRulesAsJSON() const {
        json j;
        
        j["validation_rules"] = json::array();
        for (const auto& rule : validationRules) {
            json ruleJson;
            ruleJson["key"] = rule.key;
            ruleJson["error_message"] = rule.errorMessage;
            j["validation_rules"].push_back(ruleJson);
        }
        
        j["schema_rules"] = json::array();
        // Add schema rules if any
        
        j["schemas"] = json::array();
        for (const auto& schema : schemas) {
            json schemaJson;
            schemaJson["name"] = schema.name;
            j["schemas"].push_back(schemaJson);
        }
        
        return j;
    }
    
private:
    struct ValidationRuleInfo {
        std::string key;
        ValidationRule rule;
        std::string errorMessage;
        
        ValidationRuleInfo(const std::string& k, const ValidationRule& r, const std::string& msg)
            : key(k), rule(r), errorMessage(msg) {}
        
        bool operator()(const Config& config) const {
            try {
                return rule(config);
            } catch (...) {
                return false;
            }
        }
    };
    
    std::vector<ValidationRuleInfo> validationRules;
    std::vector<SchemaRule> schemaRules;
    std::vector<Schema> schemas;
    Schema currentSchema;
};

} // namespace nlm