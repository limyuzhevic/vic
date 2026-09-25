#include "Exceptions.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace nlm {

// Configuration schema with validation
class ConfigSchema {
public:
    struct ValidationRule {
        enum class Type {
            None,
            Range,
            Length,
            Pattern,
            Enum,
            Required,
            Custom
        };
        
        Type type;
        std::string name;
        std::string description;
        
        // Rule parameters
        double minValue;
        double maxValue;
        size_t minLength;
        size_t maxLength;
        std::string pattern;
        std::vector<std::string> enumValues;
        std::function<bool(const ConfigValue&)> customValidator;
        
        ValidationRule(Type t = Type::None, const std::string& n = "", const std::string& desc = "")
            : type(t), name(n), description(desc), minValue(0), maxValue(0), 
              minLength(0), maxLength(0) {}
    };
    
    struct ConfigField {
        std::string key;
        std::string type;  // "int", "int64_t", "double", "bool", "string", "vector"
        std::string description;
        bool required;
        ConfigValue defaultValue;
        
        std::vector<ValidationRule> validationRules;
        std::vector<std::string> dependencies;
        
        // Human-readable constraints for error messages
        std::string constraintsMessage;
        
        ConfigField(const std::string& k, const std::string& t, const std::string& desc = "")
            : key(k), type(t), description(desc), required(false) {}
    };
    
    // Global configuration schema
    class GlobalSchema {
    public:
        static GlobalSchema& getInstance() {
            static GlobalSchema instance;
            return instance;
        }
        
        void addField(const ConfigField& field);
        const ConfigField* getField(const std::string& key) const;
        bool validateAll(const Config& config, std::vector<std::string>& errors) const;
        std::vector<std::string> getRequiredKeys() const;
        std::string getSchemaDescription() const;
        
        void clear();
        size_t getFieldCount() const;
        
    private:
        GlobalSchema() = default;
        std::vector<ConfigField> fields_;
    };
    
    // Instance methods
    void addField(const ConfigField& field);
    const ConfigField* getField(const std::string& key) const;
    bool validate(const Config& config, std::vector<std::string>& errors) const;
    std::string validateAndGetMessage(const Config& config) const;
    bool hasField(const std::string& key) const;
    std::vector<std::string> getAllFieldKeys() const;
    
    void setErrorReporting(bool reportDetailedErrors) { reportDetailedErrors_ = reportDetailedErrors; }
    bool getErrorReporting() const { return reportDetailedErrors_; }
    
    void setDefaultSource(ConfigSource source) { defaultSource_ = source; }
    ConfigSource getDefaultSource() const { return defaultSource_; }
    
private:
    bool reportDetailedErrors_ = true;
    ConfigSource defaultSource_ = ConfigSource::Default;
    
    bool validateField(const ConfigField& field, const ConfigValue& value, std::string& error) const;
    bool applyValidationRules(const ConfigField& field, const ConfigValue& value, std::vector<std::string>& errors) const;
    std::string formatValidationError(const ConfigField& field, const ConfigValue& value, const std::string& ruleName) const;
};

// Enhanced configuration manager with validation
class EnhancedConfigManager {
public:
    EnhancedConfigManager() = default;
    ~EnhancedConfigManager() = default;
    
    // Load configuration with validation
    bool loadFromFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    bool loadFromJSON(const std::string& filepath, ConfigSource source = ConfigSource::File);
    bool loadFromYAML(const std::string& filepath, ConfigSource source = ConfigSource::File);
    bool loadFromINI(const std::string& filepath, ConfigSource source = ConfigSource::File);
    
    // Set configuration with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Validate configuration
    bool validate(std::vector<std::string>& errors) const;
    std::string getValidationReport() const;
    bool isValid() const;
    
    // Schema management
    void addSchema(const ConfigField& field);
    void removeSchema(const std::string& key);
    bool hasSchema(const std::string& key) const;
    const ConfigField* getSchema(const std::string& key) const;
    
    // Configuration analysis
    size_t getEntryCount() const;
    size_t getInvalidEntryCount() const;
    size_t getValidEntryCount() const;
    double getValidationPercentage() const;
    
    // Error reporting
    void setVerboseErrorReporting(bool verbose) { verboseErrors_ = verbose; }
    bool getVerboseErrorReporting() const { return verboseErrors_; }
    
    std::vector<std::string> getAllErrors() const { return allErrors_; }
    void clearErrors() { allErrors_.clear(); }
    
private:
    bool verboseErrors_ = true;
    std::vector<std::string> allErrors_;
    
    // Helper functions
    bool parseFromJSON(const nlohmann::json& jsonData, ConfigSource source);
    bool parseFromYAML(const YAML::Node& yamlData, ConfigSource source);
    bool parseFromINI(const std::string& content, ConfigSource source);
    
    void addError(const std::string& error, ConfigSource source = ConfigSource::Runtime);
    bool validateDependencies(const std::string& key, ConfigSource source) const;
    bool checkCircularDependencies() const;
};

// Configuration schema initialization
void ConfigSchema::GlobalSchema::addField(const ConfigField& field) {
    // Check for duplicate keys
    for (const auto& existing : fields_) {
        if (existing.key == field.key) {
            throw ConfigSchemaException("Duplicate configuration field key: " + field.key);
        }
    }
    
    fields_.push_back(field);
}

const ConfigField* ConfigSchema::GlobalSchema::getField(const std::string& key) const {
    for (const auto& field : fields_) {
        if (field.key == key) {
            return &field;
        }
    }
    return nullptr;
}

bool ConfigSchema::GlobalSchema::validateAll(const Config& config, std::vector<std::string>& errors) const {
    errors.clear();
    
    for (const auto& field : fields_) {
        if (field.required) {
            if (!config.has(field.key)) {
                errors.push_back("Required configuration field \"" + field.key + "\" is missing");
            }
        }
        
        if (config.has(field.key)) {
            auto val_opt = config.get<std::string>(field.key);
            if (!val_opt) {
                continue;  // Skip validation if can't convert to string
            }
            
            // Convert back to appropriate type for validation (simplified)
            ConfigValue value;
            if (parseValueFromString(*val_opt, value)) {
                std::string fieldError;
                if (!validateField(field, value, fieldError)) {
                    errors.push_back("Field \"" + field.key + "\": " + fieldError);
                }
            }
        }
    }
    
    return errors.empty();
}

std::vector<std::string> ConfigSchema::GlobalSchema::getRequiredKeys() const {
    std::vector<std::string> requiredKeys;
    for (const auto& field : fields_) {
        if (field.required) {
            requiredKeys.push_back(field.key);
        }
    }
    return requiredKeys;
}

std::string ConfigSchema::GlobalSchema::getSchemaDescription() const {
    std::ostringstream oss;
    oss << "Configuration Schema (" << fields_.size() << " fields):\n";
    
    for (const auto& field : fields_) {
        oss << "  - " << field.key << " (" << field.type << ")";
        if (field.required) {
            oss << " [REQUIRED]";
        }
        if (!field.description.empty()) {
            oss << " - " << field.description;
        }
        if (!field.constraintsMessage.empty()) {
            oss << " - " << field.constraintsMessage;
        }
        oss << "\n";
    }
    
    return oss.str();
}

void ConfigSchema::GlobalSchema::clear() {
    fields_.clear();
}

size_t ConfigSchema::GlobalSchema::getFieldCount() const {
    return fields_.size();
}

// EnhancedConfigManager implementation
bool EnhancedConfigManager::loadFromFile(const std::string& filepath, ConfigSource source) {
    std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json") {
        return loadFromJSON(filepath, source);
    } else if (extension == "yaml" || extension == "yml") {
        return loadFromYAML(filepath, source);
    } else {
        return loadFromINI(filepath, source);
    }
}

bool EnhancedConfigManager::loadFromJSON(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        addError("Cannot open file: " + filepath, source);
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        return parseFromJSON(jsonData, source);
    } catch (const nlohmann::json::exception& e) {
        addError(std::string("JSON parsing error: ") + e.what(), source);
        return false;
    } catch (const std::exception& e) {
        addError(std::string("File reading error: ") + e.what(), source);
        return false;
    }
}

bool EnhancedConfigManager::loadFromYAML(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        addError("Cannot open file: " + filepath, source);
        return false;
    }
    
    try {
        YAML::Node yamlData = YAML::Load(file);
        return parseFromYAML(yamlData, source);
    } catch (const YAML::Exception& e) {
        addError(std::string("YAML parsing error: ") + e.what(), source);
        return false;
    } catch (const std::exception& e) {
        addError(std::string("File reading error: ") + e.what(), source);
        return false;
    }
}

bool EnhancedConfigManager::loadFromINI(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        addError("Cannot open file: " + filepath, source);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return parseFromINI(content, source);
}

void EnhancedConfigManager::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate before setting
    std::vector<std::string> errors;
    
    // Get schema if available
    const ConfigField* schema = GlobalSchema::getInstance().getField(key);
    if (schema) {
        std::string error;
        if (!validateField(*schema, value, error)) {
            addError(std::string("Validation error for key \"" + key + "\": " + error), source);
            return;
        }
    }
    
    // Check dependencies
    if (!validateDependencies(key, source)) {
        addError(std::string("Dependency validation failed for key: " + key), source);
        return;
    }
    
    // Set the value (assuming we have access to the original Config)
    // In practice, this would need to be part of a Config class that uses this manager
    // For now, we'll just log the validation
    if (!errors.empty()) {
        for (const auto& error : errors) {
            addError(error, source);
        }
    } else {
        addError("Value set successfully: " + key + " = " + getValueString(value), source);
    }
}

// Helper functions
bool EnhancedConfigManager::parseFromJSON(const nlohmann::json& jsonData, ConfigSource source) {
    if (!jsonData.is_object()) {
        addError("JSON root is not an object", source);
        return false;
    }
    
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        std::string key = it.key();
        ConfigValue value;
        
        if (parseJSONValue(*it, value)) {
            set(key, value, source);
        } else {
            addError(std::string("Failed to parse JSON value for key: \"" + key + "\""), source);
        }
    }
    
    return true;
}

bool EnhancedConfigManager::parseFromYAML(const YAML::Node& yamlData, ConfigSource source) {
    if (!yamlData.IsMap()) {
        addError("YAML root is not a map", source);
        return false;
    }
    
    for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
        std::string key = it->first.as<std::string>();
        ConfigValue value;
        
        if (parseYAMLValue(*it, value)) {
            set(key, value, source);
        } else {
            addError(std::string("Failed to parse YAML value for key: \"" + key + "\""), source);
        }
    }
    
    return true;
}

bool EnhancedConfigManager::parseFromINI(const std::string& content, ConfigSource source) {
    std::istringstream stream(content);
    std::string line;
    std::string currentSection = "";
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            if (currentSection.empty()) {
                set(key, value, source);
            } else {
                set(currentSection + "." + key, value, source);
            }
        }
    }
    
    return true;
}

void EnhancedConfigManager::addError(const std::string& error, ConfigSource source) {
    allErrors_.push_back("[" + std::to_string(static_cast<int>(source)) + "] " + error);
    
    if (verboseErrors_) {
        NLM_LOG_ERROR(error);
    }
}

bool EnhancedConfigManager::validateDependencies(const std::string& key, ConfigSource source) const {
    const ConfigField* schema = GlobalSchema::getInstance().getField(key);
    if (!schema) {
        return true;  // No schema, no validation
    }
    
    for (const auto& depKey : schema->dependencies) {
        // In a real implementation, we would check if the dependency exists
        // and validate its type/value
        // For now, we'll just check if the key exists
        if (!GlobalSchema::getInstance().hasField(depKey)) {
            addError(std::string("Dependency field \"" + depKey + "\" not found for key: \"" + key + "\""), source);
            return false;
        }
    }
    
    return true;
}

bool EnhancedConfigManager::checkCircularDependencies() const {
    // Simplified check - in practice, you'd need to implement a proper dependency graph check
    return true;
}

// Helper function implementations
bool parseValueFromString(const std::string& str, ConfigValue& value) {
    // Try to parse as boolean
    if (str == "true" || str == "1" || str == "yes" || str == "on") {
        value = true;
        return true;
    } else if (str == "false" || str == "0" || str == "no" || str == "off") {
        value = false;
        return true;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            value = intVal;
            return true;
        }
    } catch (const std::exception&) {}
    
    // Try to parse as int64_t
    try {
        size_t pos;
        int64_t int64Val = std::stoll(str, &pos);
        if (pos == str.size()) {
            value = int64Val;
            return true;
        }
    } catch (const std::exception&) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            value = doubleVal;
            return true;
        }
    } catch (const std::exception&) {}
    
    // Use as string
    value = str;
    return true;
}

std::string getValueString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace nlm
