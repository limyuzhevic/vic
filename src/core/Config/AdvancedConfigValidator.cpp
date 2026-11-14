// NLM Advanced Configuration Validation System
// Comprehensive configuration validation, schema definition, and error handling

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <json/json.h>  // Assuming jsoncpp is available

using namespace std;
using Json::Value;
using Json::Reader;
using Json::Writer;
using Json::FastWriter;

namespace nlm {

// Validation result enum
enum class ValidationResult {
    Success,
    Error,
    Warning,
    Info
};

// Validation error type
struct ValidationError {
    string message;
    ValidationResult severity;
    string path; // Configuration path where error occurred
    string expected;
    string actual;
    
    ValidationError(const string& msg, ValidationResult sev, const string& path = "",
                   const string& expected = "", const string& actual = "")
        : message(msg), severity(sev), path(path), expected(expected), actual(actual) {}
};

// Configuration schema definition
struct SchemaField {
    string name;
    string type; // "int", "double", "string", "bool", "vector"
    bool required;
    string defaultValue;
    string description;
    map<string, string> constraints; // min, max, pattern, etc.
    vector<SchemaField> nestedFields;
    
    SchemaField(const string& n, const string& t, bool req = false, 
               const string& def = "", const string& desc = "")
        : name(n), type(t), required(req), defaultValue(def), description(desc) {}
};

// Configuration schema class
class ConfigurationSchema {
private:
    map<string, SchemaField> fields;
    vector<SchemaField> topLevelFields;
    map<string, ConfigurationSchema> nestedSchemas;
    
    static map<string, function<bool(const string&)>> typeValidators;
    static map<string, function<bool(const string&, const map<string, string>&)>> constraintValidators;

public:
    ConfigurationSchema() {
        initializeDefaultSchema();
    }

    void initializeDefaultSchema() {
        // Core configuration
        topLevelFields = {
            SchemaField("random_seed", "int64", false, "42", "Random seed for reproducible experiments"),
            SchemaField("simulation_timestep", "double", false, "0.001", "Simulation timestep in seconds"),
            SchemaField("neuron_count", "int64", false, "1000", "Total number of neurons"),
            SchemaField("region_count", "int64", false, "1", "Number of brain regions"),
            SchemaField("connection_probability", "double", false, "0.1", "Connection probability"),
            SchemaField("enable_checkpointing", "bool", false, "true", "Enable checkpoint save/load"),
            SchemaField("enable_replay", "bool", false, "true", "Enable memory replay"),
            SchemaField("enable_development", "bool", false, "true", "Enable developmental processes"),
            SchemaField("max_simulation_steps", "int64", false, "10000", "Maximum simulation steps"),
            SchemaField("simulation_time_limit", "double", false, "0.0", "Simulation time limit (0 = no limit)"),
        };
        
        // Plasticity configuration
        topLevelFields.insert(topLevelFields.end(), {
            SchemaField("plasticity", "object", false, "{}", "Plasticity configuration"),
        });
        
        nestedSchemas["plasticity"] = createPlasticitySchema();
    }

    ConfigurationSchema createPlasticitySchema() {
        ConfigurationSchema plasticitySchema;
        plasticitySchema.topLevelFields = {
            SchemaField("stdp_ltp_weight", "double", false, "0.02", "STDP LTP weight"),
            SchemaField("stdp_ltd_weight", "double", false, "0.015", "STDP LTD weight"),
            SchemaField("stdp_time_constant", "double", false, "20.0", "STDP time constant"),
            SchemaField("hebbian_learning_rate", "double", false, "0.01", "Hebbian learning rate"),
            SchemaField("structural_enabled", "bool", false, "true", "Enable structural plasticity"),
            SchemaField("structural_synaptogenesis_rate", "double", false, "0.0001", "Synaptogenesis rate"),
            SchemaField("structural_pruning_rate", "double", false, "0.00001", "Pruning rate"),
        };
        return plasticitySchema;
    }

    // Get all field names
    vector<string> getFieldNames() const {
        vector<string> names;
        for (const auto& field : topLevelFields) {
            names.push_back(field.name);
            if (field.type == "object" && nestedSchemas.count(field.name)) {
                // For nested objects, add their field names with prefix
                ConfigurationSchema nested = nestedSchemas.at(field.name);
                vector<string> nestedNames = nested.getFieldNames();
                for (const string& nestedName : nestedNames) {
                    names.push_back(field.name + "." + nestedName);
                }
            }
        }
        return names;
    }

    // Validate configuration
    vector<ValidationError> validate(const Value& config) const {
        vector<ValidationError> errors;
        
        for (const auto& field : topLevelFields) {
            string fieldName = field.name;
            bool found = false;
            
            if (config.isMember(fieldName)) {
                found = true;
                validateField(fieldName, config[fieldName], field, errors, "");
            } else if (field.required) {
                errors.emplace_back("Missing required field: " + fieldName, 
                                 ValidationResult::Error, fieldName);
            }
        }
        
        return errors;
    }

    // Validate a single field
    void validateField(const string& fieldName, const Value& value, const SchemaField& field,
                      vector<ValidationError>& errors, const string& parentPath) const {
        string fullPath = parentPath.empty() ? fieldName : parentPath + "." + fieldName;
        
        if (field.type == "object" && value.isObject()) {
            // Handle nested object
            if (nestedSchemas.count(fieldName)) {
                ConfigurationSchema nested = nestedSchemas.at(fieldName);
                vector<ValidationError> nestedErrors = nested.validate(value);
                for (const auto& error : nestedErrors) {
                    errors.push_back(error);
                }
            }
            return;
        }
        
        // Validate simple field type
        if (!value.isNull()) {
            if (!validateType(value, field.type)) {
                errors.emplace_back(formatErrorMessage(fullPath, field, "Invalid type. Expected: " + field.type + ", Actual: " + getValueTypeName(value)),
                                 ValidationResult::Error, fullPath);
            }
            
            if (!validateConstraints(value, field)) {
                errors.emplace_back(formatErrorMessage(fullPath, field, "Value does not meet constraints"),
                                 ValidationResult::Error, fullPath);
            }
        }
    }

    bool validateType(const Value& value, const string& type) const {
        if (type == "int64") return value.isInt64();
        if (type == "double") return value.isDouble() || value.isInt();
        if (type == "string") return value.isString();
        if (type == "bool") return value.isBool();
        if (type == "vector") return value.isArray();
        if (type == "object") return value.isObject();
        return false;
    }

    bool validateConstraints(const Value& value, const SchemaField& field) const {
        // Check constraints based on field type
        if (field.type == "int64" || field.type == "double") {
            double val = getNumericValue(value);
            
            if (field.constraints.count("min")) {
                double min = stod(field.constraints.at("min"));
                if (val < min) return false;
            }
            
            if (field.constraints.count("max")) {
                double max = stod(field.constraints.at("max"));
                if (val > max) return false;
            }
        }
        
        if (field.type == "string") {
            if (field.constraints.count("pattern")) {
                regex pattern(field.constraints.at("pattern"));
                if (!regex_match(value.asString(), pattern)) return false;
            }
        }
        
        if (field.type == "int64" || field.type == "double") {
            if (field.constraints.count("multiple_of")) {
                double val = getNumericValue(value);
                double divisor = stod(field.constraints.at("multiple_of"));
                if (fabs(val) > 1e-9 && fabs(fmod(val, divisor)) > 1e-9) return false;
            }
        }
        
        return true;
    }

    double getNumericValue(const Value& value) const {
        if (value.isInt64()) return value.asInt64();
        if (value.isDouble()) return value.asDouble();
        if (value.isInt()) return value.asInt();
        return 0.0;
    }

    string getValueTypeName(const Value& value) const {
        if (value.isInt64()) return "int64";
        if (value.isDouble()) return "double";
        if (value.isString()) return "string";
        if (value.isBool()) return "bool";
        if (value.isArray()) return "vector";
        if (value.isObject()) return "object";
        if (value.isNull()) return "null";
        return "unknown";
    }

    string formatErrorMessage(const string& path, const SchemaField& field, const string& message) const {
        return "[" + path + "] " + message + ". Description: " + field.description;
    }

    // Get default configuration
    Value getDefaultConfig() const {
        Value config;
        
        for (const auto& field : topLevelFields) {
            if (!field.defaultValue.empty()) {
                config[field.name] = field.defaultValue;
            }
        }
        
        // Add nested objects with defaults
        for (const auto& schemaPair : nestedSchemas) {
            Value nestedConfig = schemaPair.second.getDefaultConfig();
            config[schemaPair.first] = nestedConfig;
        }
        
        return config;
    }

    // Load configuration from JSON string
    Value loadFromJsonString(const string& jsonString) const {
        Reader reader;
        Value config;
        
        if (!reader.parse(jsonString, config)) {
            throw runtime_error("Failed to parse JSON configuration");
        }
        
        return config;
    }

    // Load configuration from file
    Value loadFromFile(const string& filepath) const {
        ifstream file(filepath);
        if (!file.is_open()) {
            throw runtime_error("Cannot open configuration file: " + filepath);
        }
        
        stringstream buffer;
        buffer << file.rdbuf();
        
        return loadFromJsonString(buffer.str());
    }

    // Merge configuration with overrides
    Value mergeConfig(const Value& baseConfig, const map<string, string>& overrides) const {
        Value result = baseConfig;
        
        for (const auto& override : overrides) {
            string path = override.first;
            string value = override.second;
            
            Value* current = &result;
            vector<string> pathParts = splitPath(path);
            
            // Navigate to the target path
            for (size_t i = 0; i < pathParts.size() - 1; ++i) {
                if (!current->isObject() || !current->isMember(pathParts[i])) {
                    // Create missing path components
                    (*current)[pathParts[i]] = Value();
                }
                current = &(*current)[pathParts[i]];
            }
            
            // Set the value
            if (pathParts.size() > 0) {
                setValueByType(*current, pathParts.back(), value);
            }
        }
        
        return result;
    }

    vector<string> splitPath(const string& path) const {
        vector<string> parts;
        stringstream stream(path);
        string part;
        
        while (getline(stream, part, '.')) {
            if (!part.empty()) {
                parts.push_back(part);
            }
        }
        
        return parts;
    }

    void setValueByType(Value& target, const string& fieldName, const string& valueString) const {
        // Try to determine the type based on schema
        for (const auto& field : topLevelFields) {
            if (field.name == fieldName) {
                if (field.type == "int64") {
                    try {
                        target[fieldName] = stoll(valueString);
                    } catch (...) {
                        target[fieldName] = 0;
                    }
                } else if (field.type == "double") {
                    try {
                        target[fieldName] = stod(valueString);
                    } catch (...) {
                        target[fieldName] = 0.0;
                    }
                } else if (field.type == "bool") {
                    transform(valueString.begin(), valueString.end(), valueString.begin(), ::tolower);
                    target[fieldName] = (valueString == "true" || valueString == "1");
                } else {
                    target[fieldName] = valueString;
                }
                return;
            }
        }
        
        // Check nested schemas
        for (const auto& schemaPair : nestedSchemas) {
            ConfigurationSchema nested = schemaPair.second;
            for (const auto& field : nested.topLevelFields) {
                if (fieldName == schemaPair.first + "." + field.name) {
                    // For nested fields, we need to create the nested object first
                    if (!target.isMember(schemaPair.first)) {
                        target[schemaPair.first] = Value();
                    }
                    setValueByType(target[schemaPair.first], field.name, valueString);
                    return;
                }
            }
        }
        
        // Fallback to string
        target[fieldName] = valueString;
    }

    // Export configuration to JSON string
    string exportToJson(const Value& config) const {
        FastWriter writer;
        return writer.write(config);
    }

    // Validate configuration against schema and return human-readable report
    string generateValidationReport(const Value& config) const {
        vector<ValidationError> errors = validate(config);
        
        ostringstream report;
        report << "=== NLM Configuration Validation Report ===" << endl;
        report << "Validation: " << (errors.empty() ? "PASSED" : "FAILED") << endl;
        report << "" << endl;
        
        if (errors.empty()) {
            report << "Configuration is valid and meets all schema requirements." << endl;
        } else {
            report << "Configuration errors and warnings:" << endl;
            report << "" << endl;
            
            for (const auto& error : errors) {
                string severityStr;
                switch (error.severity) {
                    case ValidationResult::Error: severityStr = "ERROR"; break;
                    case ValidationResult::Warning: severityStr = "WARNING"; break;
                    case ValidationResult::Info: severityStr = "INFO"; break;
                    default: severityStr = "UNKNOWN"; break;
                }
                
                report << "[" << severityStr << "] " << error.message << endl;
                if (!error.expected.empty()) {
                    report << "   Expected: " << error.expected << endl;
                }
                if (!error.actual.empty()) {
                    report << "   Actual: " << error.actual << endl;
                }
                report << "" << endl;
            }
        }
        
        return report.str();
    }
};

// Global configuration validator instance
inline ConfigurationSchema& getConfigurationSchema() {
    static ConfigurationSchema schema;
    return schema;
}

// Convenience functions for configuration validation
inline vector<ValidationError> validateConfiguration(const string& jsonString) {
    return getConfigurationSchema().validate(getConfigurationSchema().loadFromJsonString(jsonString));
}

inline vector<ValidationError> validateConfigurationFromFile(const string& filepath) {
    return getConfigurationSchema().validate(getConfigurationSchema().loadFromFile(filepath));
}

inline string validateAndGenerateReport(const string& jsonString) {
    Value config = getConfigurationSchema().loadFromJsonString(jsonString);
    return getConfigurationSchema().generateValidationReport(config);
}

inline Value loadConfiguration(const string& jsonString) {
    return getConfigurationSchema().loadFromJsonString(jsonString);
}

inline Value loadConfigurationFromFile(const string& filepath) {
    return getConfigurationSchema().loadFromFile(filepath);
}

inline Value mergeConfiguration(const Value& baseConfig, const map<string, string>& overrides) {
    return getConfigurationSchema().mergeConfig(baseConfig, overrides);
}

} // namespace nlm

// Example usage
#ifdef NLM_CONFIG_EXAMPLES
void demonstration() {
    // Example 1: Load and validate configuration
    string jsonConfig = R"({
        "random_seed": 42,
        "neuron_count": 1000,
        "connection_probability": 0.1,
        "plasticity": {
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015
        }
    })";
    
    auto errors = nlm::validateConfiguration(jsonConfig);
    if (errors.empty()) {
        cout << "Configuration is valid!" << endl;
    } else {
        cout << "Configuration validation failed:" << endl;
        for (const auto& error : errors) {
            cout << "  " << static_cast<int>(error.severity) << ": " << error.message << endl;
        }
    }
    
    // Example 2: Generate validation report
    cout << nlm::validateAndGenerateReport(jsonConfig) << endl;
    
    // Example 3: Merge configuration with overrides
    map<string, string> overrides;
    overrides["neuron_count"] = "2000";
    overrides["plasticity.stdp_ltp_weight"] = "0.03";
    
    Value config = nlm::loadConfiguration(jsonConfig);
    Value mergedConfig = nlm::mergeConfiguration(config, overrides);
    
    cout << "Merged configuration:" << endl;
    cout << nlm::getConfigurationSchema().exportToJson(mergedConfig) << endl;
    
    // Example 4: Load from file and validate
    // Value configFile = nlm::loadConfigurationFromFile("configs/my_config.json");
    // auto fileErrors = nlm::validateConfigurationFromFile("configs/my_config.json");
}
#endif // NLM_CONFIG_EXAMPLES

```