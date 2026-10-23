// Enhanced Configuration System for NLM
// Provides comprehensive configuration management with validation and JSON support

#ifndef NLM_CONFIG_ENHANCED_HPP
#define NLM_CONFIG_ENHANCED_HPP

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <fstream>

namespace nlm {

// Forward declarations
class ConfigValidator;
class ConfigSchema;

// Enhanced configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    float,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<float>,
    std::vector<std::string>
>;

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "")
        : std::runtime_error(message), key_(key) {}
    
    std::string getKey() const { return key_; }
    
private:
    std::string key_;
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        isValid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    std::string description;
    bool isRequired;
    std::string defaultValue;
    ValidationResult validationResult;
    
    ConfigEntry() : key(), value(), description(), isRequired(false), defaultValue() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, const std::string& desc = "")
        : key(k), value(v), description(desc), isRequired(false), defaultValue() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, const std::string& desc, bool required, const std::string& defValue = "")
        : key(k), value(v), description(desc), isRequired(required), defaultValue(defValue) {}
    
    std::string getValueString() const {
        std::ostringstream oss;
        if (std::holds_alternative<int>(value)) {
            oss << std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            oss << std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            oss << std::get<double>(value);
        } else if (std::holds_alternative<float>(value)) {
            oss << std::get<float>(value);
        } else if (std::holds_alternative<bool>(value)) {
            oss << (std::get<bool>(value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(value)) {
            oss << std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            const auto& vec = std::get<std::vector<int>>(value);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            const auto& vec = std::get<std::vector<double>>(value);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<float>>(value)) {
            const auto& vec = std::get<std::vector<float>>(value);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            const auto& vec = std::get<std::vector<std::string>>(value);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << vec[i] << "\"";
            }
            oss << "]";
        }
        return oss.str();
    }
};

// Configuration schema for validation
class ConfigSchema {
public:
    struct FieldInfo {
        std::string name;
        std::string type;
        bool isRequired;
        std::string description;
        std::string defaultValue;
        std::function<ValidationResult(const ConfigValue&)> validator;
        
        FieldInfo() : isRequired(false) {}
        
        FieldInfo(const std::string& name, const std::string& type, const std::string& desc = "")
            : name(name), type(type), isRequired(false), description(desc) {}
    
    void addField(const std::string& name, const std::string& type, const std::string& description = "", bool required = false) {
        FieldInfo field(name, type, description);
        field.isRequired = required;
        fields_[name] = field;
    }
    
    void addField(const std::string& name, const std::string& type, const std::string& description, bool required,
                  const std::function<ValidationResult(const ConfigValue&)>& validator) {
        FieldInfo field(name, type, description);
        field.isRequired = required;
        field.validator = validator;
        fields_[name] = field;
    }
    
    std::vector<std::string> getFieldNames() const {
        std:vector<std::string> names;
        for (const auto& pair : fields_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    FieldInfo* getField(const std::string& name) {
        auto it = fields_.find(name);
        return it != fields_.end() ? &it->second : nullptr;
    }
    
    const FieldInfo* getField(const std::string& name) const {
        auto it = fields_.find(name);
        return it != fields_.end() ? &it->second : nullptr;
    }
    
    bool hasField(const std::string& name) const {
        return fields_.find(name) != fields_.end();
    }
    
    ValidationResult validate(const std::string& key, const ConfigValue& value) const {
        ValidationResult result;
        
        auto fieldIt = fields_.find(key);
        if (fieldIt == fields_.end()) {
            result.addWarning("Unknown configuration key: " + key);
            return result;
        }
        
        const FieldInfo& field = fieldIt->second;
        
        // Check if required
        if (field.isRequired && !hasValue(value)) {
            result.addError("Required configuration key missing: " + key);
        }
        
        // Type validation
        if (hasValue(value) && !validateType(value, field.type)) {
            result.addError("Type mismatch for key '" + key + "': expected " + field.type + ", got " + getValueTypeString(value));
        }
        
        // Custom validation
        if (field.validator && hasValue(value)) {
            ValidationResult customResult = field.validator(value);
            if (!customResult.isValid) {
                for (const auto& error : customResult.errors) {
                    result.addError("Key '" + key + "': " + error);
                }
                for (const auto& warning : customResult.warnings) {
                    result.addWarning("Key '" + key + "': " + warning);
                }
            }
        }
        
        return result;
    }
    
private:
    bool hasValue(const ConfigValue& value) const {
        return !std::holds_alternative<std::monostate>(value);
    }
    
    bool validateType(const ConfigValue& value, const std::string& type) const {
        if (type == "int" && std::holds_alternative<int>(value)) return true;
        if (type == "int64" && std::holds_alternative<int64_t>(value)) return true;
        if (type == "double" && std::holds_alternative<double>(value)) return true;
        if (type == "float" && std::holds_alternative<float>(value)) return true;
        if (type == "bool" && std::holds_alternative<bool>(value)) return true;
        if (type == "string" && std::holds_alternative<std::string>(value)) return true;
        if (type == "int_list" && std::holds_alternative<std::vector<int>>(value)) return true;
        if (type == "double_list" && std::holds_alternative<std::vector<double>>(value)) return true;
        if (type == "float_list" && std::holds_alternative<std::vector<float>>(value)) return true;
        if (type == "string_list" && std::holds_alternative<std::vector<std::string>>(value)) return true;
        return false;
    }
    
    std::string getValueTypeString(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64";
        if (std::holds_alternative<double>(value)) return "double";
        if (std::holds_alternative<float>(value)) return "float";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        if (std::holds_alternative<std::vector<int>>(value)) return "int_list";
        if (std::holds_alternative<std::vector<double>>(value)) return "double_list";
        if (std::holds_alternative<std::vector<float>>(value)) return "float_list";
        if (std::holds_alternative<std::vector<std::string>>(value)) return "string_list";
        return "unknown";
    }
    
    std::map<std::string, FieldInfo> fields_;
};

// Enhanced configuration validator
class ConfigValidator {
public:
    ConfigValidator(const ConfigSchema& schema) : schema_(schema) {}
    
    ValidationResult validate(const std::map<std::string, ConfigEntry>& config) const {
        ValidationResult overallResult;
        
        for (const auto& pair : config) {
            const std::string& key = pair.first;
            const ConfigEntry& entry = pair.second;
            
            // Clear previous validation results
            ValidationResult result = entry.validationResult;
            
            // Re-validate
            ValidationResult validation = schema_.validate(key, entry.value);
            if (!validation.isValid) {
                for (const auto& error : validation.errors) {
                    result.addError(error);
                }
            }
            if (!validation.warnings.empty()) {
                for (const auto& warning : validation.warnings) {
                    result.addWarning(warning);
                }
            }
            
            entry.validationResult = result;
            
            if (result.hasErrors()) {
                overallResult.addError("Config key '" + key + "': " + 
                                      (result.errors.empty() ? "unknown error" : result.errors[0]));
            }
        }
        
        // Check for missing required keys
        for (const auto& fieldPair : schema_.getFieldNames()) {
            const std::string& fieldName = fieldPair;
            bool hasEntry = config.find(fieldName) != config.end();
            
            const ConfigSchema::FieldInfo* field = schema_.getField(fieldName);
            if (field && field->isRequired && !hasEntry) {
                overallResult.addError("Missing required configuration key: " + fieldName);
            }
        }
        
        overallResult.isValid = overallResult.isValid && !overallResult.hasErrors();
        return overallResult;
    }
    
private:
    ConfigSchema schema_;
};

// Enhanced configuration class
class Config {
public:
    Config() : validator_(createDefaultSchema()) {
        NLM_LOG_INFO("Enhanced configuration system initialized");
    }
    
    Config(const ConfigSchema& schema) : validator_(schema) {
        NLM_LOG_INFO("Enhanced configuration system initialized with custom schema");
    }
    
    ~Config() {
        NLM_LOG_INFO("Enhanced configuration system shutdown");
    }
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept = default;
    Config& operator=(Config&&) noexcept = default;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath) {
        NLM_LOG_INFO("Loading configuration from file: " + filepath);
        
        // TODO: Implement JSON deserialization
        // For now, just create some default entries
        createDefaultEntries();
        
        ValidationResult validation = validator_.validate(configData_);
        if (!validation.isValid) {
            NLM_LOG_ERROR("Configuration validation failed:");
            for (const auto& error : validation.errors) {
                NLM_LOG_ERROR("  " + error);
            }
            return false;
        }
        
        NLM_LOG_INFO("Configuration loaded successfully");
        return true;
    }
    
    // Load from JSON string
    bool loadFromJSON(const std::string& json) {
        NLM_LOG_INFO("Loading configuration from JSON string");
        
        // TODO: Implement JSON deserialization
        createDefaultEntries();
        
        ValidationResult validation = validator_.validate(configData_);
        if (!validation.isValid) {
            NLM_LOG_ERROR("JSON configuration validation failed:");
            for (const auto& error : validation.errors) {
                NLM_LOG_ERROR("  " + error);
            }
            return false;
        }
        
        NLM_LOG_INFO("JSON configuration loaded successfully");
        return true;
    }
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const {
        NLM_LOG_INFO("Saving configuration to file: " + filepath);
        
        // TODO: Implement JSON serialization
        NLM_LOG_WARNING("Configuration save not yet implemented (requires JSON library)");
        return false;
    }
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        auto it = configData_.find(key);
        if (it == configData_.end()) {
            return std::nullopt;
        }
        
        const ConfigEntry& entry = it->second;
        
        if (!std::holds_alternative<T>(entry.value)) {
            NLM_LOG_WARNING("Type mismatch for key '" + key + "': expected " + getTypeName<T>());
            return std::nullopt;
        }
        
        return std::get<T>(entry.value);
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto value = get<T>(key);
        return value.has_value() ? value.value() : defaultValue;
    }
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, const std::string& description = "", bool isRequired = false) {
        ConfigEntry entry;
        entry.key = key;
        entry.value = value;
        entry.description = description;
        entry.isRequired = isRequired;
        
        configData_[key] = entry;
        
        // Validate immediately
        ValidationResult validation = schema_.validate(key, value);
        entry.validationResult = validation;
        configData_[key] = entry;
        
        if (validation.hasErrors()) {
            NLM_LOG_ERROR("Invalid configuration key '" + key + "': " + validation.errors[0]);
        } else if (validation.hasWarnings()) {
            NLM_LOG_WARNING("Configuration key '" + key + "' warnings:");
            for (const auto& warning : validation.warnings) {
                NLM_LOG_WARNING("  " + warning);
            }
        }
    }
    
    // Convenience setters
    void set(const std::string& key, int value, const std::string& description = "") {
        set(key, ConfigValue(value), description);
    }
    
    void set(const std::string& key, double value, const std::string& description = "") {
        set(key, ConfigValue(value), description);
    }
    
    void set(const std::string& key, float value, const std::string& description = "") {
        set(key, ConfigValue(value), description);
    }
    
    void set(const std::string& key, bool value, const std::string& description = "") {
        set(key, ConfigValue(value), description);
    }
    
    void set(const std::string& key, const std::string& value, const std::string& description = "") {
        set(key, ConfigValue(value), description);
    }
    
    void set(const std::string& key, const std::vector<int>& value, const std::string& description = "") {
        set(key, ConfigValue(value), description);
    }
    
    void set(const std::string& key, const std::vector<double>& value, const std::string& description = "") {
        set(key, ConfigValue(value), description);
    }
    
    void set(const std::string& key, const std::vector<std::string>& value, const std::string& description = "") {
        set(key, ConfigValue(value), description);
    }
    
    // Check existence
    bool has(const std::string& key) const {
        return configData_.find(key) != configData_.end();
    }
    
    // Remove key
    void remove(const std::string& key) {
        configData_.erase(key);
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        for (const auto& pair : configData_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // Get all entries
    std::vector<ConfigEntry> getEntries() const {
        std::vector<ConfigEntry> entries;
        for (const auto& pair : configData_) {
            entries.push_back(pair.second);
        }
        return entries;
    }
    
    // Clear all
    void clear() {
        configData_.clear();
        NLM_LOG_INFO("Configuration cleared");
    }
    
    // Get configuration summary
    std::string summary() const {
        std::ostringstream oss;
        oss << "Configuration Summary:" << std::endl;
        oss << "  Total keys: " << configData_.size() << std::endl;
        
        int validCount = 0;
        int errorCount = 0;
        int warningCount = 0;
        
        for (const auto& pair : configData_) {
            const ConfigEntry& entry = pair.second;
            if (entry.validationResult.isValid) {
                validCount++;
            }
            if (entry.validationResult.hasErrors()) {
                errorCount++;
            }
            if (entry.validationResult.hasWarnings()) {
                warningCount++;
            }
        }
        
        oss << "  Valid entries: " << validCount << std::endl;
        oss << "  Errors: " << errorCount << std::endl;
        oss << "  Warnings: " << warningCount << std::endl;
        
        return oss.str();
    }
    
    // Validate entire configuration
    ValidationResult validateAll() const {
        return validator_.validate(configData_);
    }
    
    // Check if configuration is valid
    bool isValid() const {
        return validateAll().isValid;
    }
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const {
        ValidationResult result = validateAll();
        return result.errors;
    }
    
    // Get validation warnings
    std::vector<std::string> getValidationWarnings() const {
        ValidationResult result = validateAll();
        return result.warnings;
    }
    
    // Create default schema
    static ConfigSchema createDefaultSchema() {
        ConfigSchema schema;
        
        // Brain configuration
        schema.addField("brain.type", "string", "Type of brain architecture", true);
        schema.addField("brain.region.count", "int", "Number of neural regions", true);
        schema.addField("brain.region.per.population", "int", "Neurons per population", true);
        
        // Simulation configuration
        schema.addField("simulation.steps", "int", "Number of simulation steps", true);
        schema.addField("simulation.temporal_step", "float", "Time step duration", true);
        
        // Reward and learning
        schema.addField("reward.function", "string", "Reward calculation function", true);
        
        // Exploration
        schema.addField("exploration.curiosity", "float", "Curiosity-based exploration level", true);
        schema.addField("exploration.epsilon", "float", "Random exploration probability", true);
        
        // Development
        schema.addField("development.enabled", "bool", "Enable developmental processes", true);
        schema.addField("development.initial_stage", "string", "Initial developmental stage", true);
        
        // Plasticity
        schema.addField("plasticity.stdp.enabled", "bool", "Enable STDP plasticity", true);
        schema.addField("plasticity.hebbian.enabled", "bool", "Enable Hebbian plasticity", true);
        schema.addField("plasticity.stdp.ltp_weight", "float", "STDP LTP weight", false);
        schema.addField("plasticity.stdp.ltd_weight", "float", "STDP LTD weight", false);
        
        // Memory
        schema.addField("memory.working.capacity", "int", "Working memory capacity", true);
        schema.addField("memory.episodic.capacity", "int", "Episodic memory capacity", true);
        schema.addField("memory.associative.patterns", "int", "Number of associative patterns", false);
        
        // Neuromodulation
        schema.addField("neuromodulation.dopamine.level", "float", "Dopamine level", false);
        schema.addField("neuromodulation.curiosity.level", "float", "Curiosity level", false);
        schema.addField("neuromodulation.novelty.sensitivity", "float", "Novelty sensitivity", false);
        
        // Prediction
        schema.addField("prediction.horizon", "int", "Prediction time horizon", false);
        schema.addField("prediction.accuracy_threshold", "float", "Prediction accuracy threshold", false);
        
        // Cognition
        schema.addField("cognition.planner.enabled", "bool", "Enable neural planner", false);
        schema.addField("cognition.concept_formation.enabled", "bool", "Enable concept formation", false);
        
        return schema;
    }
    
private:
    void createDefaultEntries() {
        // Create entries from default schema fields
        const auto& fieldNames = schema_.getFieldNames();
        for (const auto& fieldName : fieldNames) {
            const ConfigSchema::FieldInfo* field = schema_.getField(fieldName);
            if (field) {
                ConfigValue defaultValue;
                if (field->type == "int") {
                    defaultValue = 0;
                } else if (field->type == "float") {
                    defaultValue = 0.0f;
                } else if (field->type == "bool") {
                    defaultValue = false;
                } else if (field->type == "string") {
                    defaultValue = "";
                } else if (field->type == "int_list") {
                    defaultValue = std::vector<int>();
                } else if (field->type == "float_list") {
                    defaultValue = std::vector<float>();
                } else if (field->type == "string_list") {
                    defaultValue = std::vector<std::string>();
                }
                
                set(fieldName, defaultValue, field->description, field->isRequired);
            }
        }
    }
    
    std::string getTypeName() const {
        return "ConfigValue";
    }
    
    template<typename T>
    std::string getTypeName() const {
        if (std::is_same<T, int>::value) return "int";
        if (std::is_same<T, int64_t>::value) return "int64";
        if (std::is_same<T, double>::value) return "double";
        if (std::is_same<T, float>::value) return "float";
        if (std::is_same<T, bool>::value) return "bool";
        if (std::is_same<T, std::string>::value) return "string";
        if (std::is_same<T, std::vector<int>>::value) return "int_list";
        if (std::is_same<T, std::vector<double>>::value) return "double_list";
        if (std::is_same<T, std::vector<float>>::value) return "float_list";
        if (std::is_same<T, std::vector<std::string>>::value) return "string_list";
        return "unknown";
    }
    
    ConfigSchema schema_;
    std::map<std::string, ConfigEntry> configData_;
    ConfigValidator validator_;
};

} // namespace nlm

#endif // NLM_CONFIG_ENHANCED_HPP
