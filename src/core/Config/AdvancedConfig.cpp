#include "AdvancedConfig.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <ctime>
#include <cmath>
#include <cctype>

using json = nlohmann::json;
using YAML::Node;

namespace nlm {

struct ConfigSchema::Impl {
    std::vector<ConfigField> fields;
    std::unordered_map<std::string, size_t> fieldIndex;
};

ConfigSchema::ConfigSchema() : pImpl(std::make_unique<Impl>()) {}

ConfigSchema::~ConfigSchema() = default;

void ConfigSchema::addField(const ConfigField& field) {
    pImpl->fields.push_back(field);
    pImpl->fieldIndex[field.name] = pImpl->fields.size() - 1;
}

void ConfigSchema::addField(const std::string& name, ConfigDataType dataType,
                           const EnhancedConfigValue& defaultValue,
                           bool required, const std::string& description) {
    ConfigField field;
    field.name = name;
    field.dataType = dataType;
    field.defaultValue = defaultValue;
    field.required = required;
    field.description = description;
    
    pImpl->fields.push_back(field);
    pImpl->fieldIndex[name] = pImpl->fields.size() - 1;
}

const ConfigField* ConfigSchema::getField(const std::string& name) const {
    auto it = pImpl->fieldIndex.find(name);
    if (it == pImpl->fieldIndex.end()) return nullptr;
    return &pImpl->fields[it->second];
}

bool ConfigSchema::hasField(const std::string& name) const {
    return pImpl->fieldIndex.find(name) != pImpl->fieldIndex.end();
}

std::vector<std::string> ConfigSchema::getFieldNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->fields.size());
    for (const auto& field : pImpl->fields) {
        names.push_back(field.name);
    }
    return names;
}

std::vector<std::string> ConfigSchema::validate(const std::unordered_map<std::string, EnhancedConfigValue>& config) const {
    std::vector<std::string> errors;
    
    for (const auto& field : pImpl->fields) {
        auto it = config.find(field.name);
        
        if (it == config.end()) {
            if (field.required) {
                errors.push_back("Missing required field: " + field.name);
            }
            continue;
        }
        
        std::string error = validateField(field.name, it->second);
        if (!error.empty()) {
            errors.push_back(error);
        }
    }
    
    return errors;
}

std::string ConfigSchema::validateField(const std::string& fieldName,
                                       const EnhancedConfigValue& value) const {
    auto it = pImpl->fieldIndex.find(fieldName);
    if (it == pImpl->fieldIndex.end()) {
        return "Unknown field: " + fieldName;
    }
    
    const auto& field = pImpl->fields[it->second];
    
    // Validate type
    if (!ConfigValidator::isType(value, field.dataType)) {
        return "Field '" + fieldName + "' has wrong type. Expected " +
               ConfigValidator::getTypeName(field.dataType) +
               ", got " + ConfigValidator::getTypeName(ConfigValidator::getType(value));
    }
    
    // Validate constraints
    auto constraintErrors = ConfigValidator::validateConstraints(value, field);
    if (!constraintErrors.empty()) {
        return field.name + ": " + constraintErrors[0];
    }
    
    return "";
}

const std::vector<ConfigField>& ConfigSchema::getFields() const {
    return pImpl->fields;
}

void ConfigSchema::merge(const ConfigSchema& other) {
    for (const auto& field : other.pImpl->fields) {
        if (!hasField(field.name)) {
            pImpl->fields.push_back(field);
            pImpl->fieldIndex[field.name] = pImpl->fields.size() - 1;
        }
    }
}

void ConfigSchema::clear() {
    pImpl->fields.clear();
    pImpl->fieldIndex.clear();
}

std::string ConfigSchema::getSummary() const {
    std::ostringstream oss;
    oss << "Configuration Schema Summary:" << std::endl;
    oss << "Total fields: " << pImpl->fields.size() << std::endl;
    oss << std::endl;
    
    for (const auto& field : pImpl->fields) {
        oss << "Field: " << field.name << std::endl;
        oss << "  Type: " << ConfigValidator::getTypeName(field.dataType) << std::endl;
        oss << "  Required: " << (field.required ? "Yes" : "No") << std::endl;
        oss << "  Default: " << ConfigValidator::valueToString(field.defaultValue) << std::endl;
        oss << "  Description: " << field.description << std::endl;
        if (!field.category.empty()) {
            oss << "  Category: " << field.category << std::endl;
        }
        oss << std::endl;
    }
    
    return oss.str();
}

bool ConfigSchema::saveToFile(const std::string& filepath) const {
    json schemaJson;
    
    for (const auto& field : pImpl->fields) {
        json fieldJson;
        fieldJson["name"] = field.name;
        fieldJson["type"] = static_cast<int>(field.dataType);
        fieldJson["required"] = field.required;
        fieldJson["readOnly"] = field.readOnly;
        fieldJson["description"] = field.description;
        fieldJson["category"] = field.category;
        
        // Convert default value to JSON
        if (!ConfigValidator::isType(field.defaultValue, ConfigDataType::None)) {
            fieldJson["default"] = ConfigValidator::valueToString(field.defaultValue);
        }
        
        schemaJson["fields"].push_back(fieldJson);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << schemaJson.dump(2);
    file.close();
    return true;
}

bool ConfigSchema::loadFromFile(const std::string& filepath) {
    clear();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json schemaJson = json::parse(file);
        
        if (!schemaJson.contains("fields")) {
            return false;
        }
        
        for (const auto& fieldJson : schemaJson["fields"]) {
            ConfigField field;
            field.name = fieldJson["name"].get<std::string>();
            field.dataType = static_cast<ConfigDataType>(fieldJson["type"].get<int>());
            field.required = fieldJson["required"].get<bool>();
            field.readOnly = fieldJson.value("readOnly", false);
            field.description = fieldJson.value("description", "");
            field.category = fieldJson.value("category", "");
            
            if (fieldJson.contains("default")) {
                // Parse default value from string (simplified implementation)
                std::string defaultStr = fieldJson["default"].get<std::string>();
                // This is a simplified implementation - real implementation would need
                // to handle different types based on the field type
                if (field.dataType == ConfigDataType::Integer) {
                    field.defaultValue = std::stoll(defaultStr);
                } else if (field.dataType == ConfigDataType::Float) {
                    field.defaultValue = std::stod(defaultStr);
                } else if (field.dataType == ConfigDataType::Boolean) {
                    field.defaultValue = (defaultStr == "true" || defaultStr == "1");
                } else {
                    field.defaultValue = defaultStr;
                }
            }
            
            addField(field);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load schema: ") + e.what());
        clear();
        return false;
    }
}

struct ConfigValidator::ConfigValidatorImpl {
    // Additional implementation details
};

ConfigValidator::ConfigValidator() = default;

ConfigValidator::~ConfigValidator() = default;

bool ConfigValidator::validateType(const std::string& fieldName, const ConfigDataType& fieldType,
                                  const EnhancedConfigValue& value) const {
    return isType(value, fieldType);
}

std::vector<std::string> ConfigValidator::validateConstraints(const EnhancedConfigValue& value,
                                                              const ConfigField& field) const {
    std::vector<std::string> errors;
    
    // Check validation regex
    if (!field.validationRegex.empty() && isType(value, ConfigDataType::String)) {
        std::string strValue;
        if (std::holds_alternative<std::string>(value)) {
            strValue = std::get<std::string>(value);
        }
        
        std::regex regex(field.validationRegex);
        if (!std::regex_match(strValue, regex)) {
            errors.push_back("Value does not match validation pattern");
        }
    }
    
    // Check range constraints
    if (field.validationMin != field.validationMax) {
        double val = 0.0;
        if (isType(value, ConfigDataType::Integer)) {
            val = std::get<int64_t>(value);
        } else if (isType(value, ConfigDataType::Float)) {
            val = std::get<double>(value);
        } else {
            return errors;
        }
        
        if (val < field.validationMin || val > field.validationMax) {
            errors.push_back("Value must be between " + std::to_string(field.validationMin) +
                           " and " + std::to_string(field.validationMax));
        }
    }
    
    // Check length constraints
    size_t length = 0;
    if (isType(value, ConfigDataType::String) || isType(value, ConfigDataType::StringArray)) {
        if (isType(value, ConfigDataType::String)) {
            length = std::get<std::string>(value).size();
        } else {
            length = std::get<std::vector<std::string>>(value).size();
        }
        
        if (field.validationMinLength > 0 && length < field.validationMinLength) {
            errors.push_back("Value must have at least " + std::to_string(field.validationMinLength) +
                           " characters/elements");
        }
        
        if (field.validationMaxLength > 0 && length > field.validationMaxLength) {
            errors.push_back("Value must have at most " + std::to_string(field.validationMaxLength) +
                           " characters/elements");
        }
    }
    
    return errors;
}

std::vector<std::string> ConfigValidator::validate(const std::unordered_map<std::string, EnhancedConfigValue>& config,
                                                   const ConfigSchema& schema) const {
    return schema.validate(config);
}

std::optional<EnhancedConfigValue> ConfigValidator::convertValue(const EnhancedConfigValue& value,
                                                                ConfigDataType targetType) const {
    if (!isType(value, targetType)) {
        return std::nullopt;
    }
    return value;
}

ConfigDataType ConfigValidator::getType(const EnhancedConfigValue& value) {
    if (std::holds_alternative<int64_t>(value)) return ConfigDataType::Integer;
    if (std::holds_alternative<double>(value)) return ConfigDataType::Float;
    if (std::holds_alternative<bool>(value)) return ConfigDataType::Boolean;
    if (std::holds_alternative<std::string>(value)) return ConfigDataType::String;
    if (std::holds_alternative<std::vector<int64_t>>(value)) return ConfigDataType::IntegerArray;
    if (std::holds_alternative<std::vector<double>>(value)) return ConfigDataType::FloatArray;
    if (std::holds_alternative<std::vector<bool>>(value)) return ConfigDataType::BooleanArray;
    if (std::holds_alternative<std::vector<std::string>>(value)) return ConfigDataType::StringArray;
    if (std::holds_alternative<std::map<std::string, std::any>>(value)) return ConfigDataType::Object;
    
    return ConfigDataType::None;
}

std::string ConfigValidator::valueToString(const EnhancedConfigValue& value) {
    if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(6) << std::get<double>(value);
        return oss.str();
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int64_t>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<int64_t>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<bool>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<bool>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << (vec[i] ? "true" : "false");
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << vec[i] << "\"";
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::map<std::string, std::any>>(value)) {
        std::ostringstream oss;
        oss << "{";
        const auto& obj = std::get<std::map<std::string, std::any>>(value);
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            if (it != obj.begin()) oss << ", ";
            oss << "\"" << it->first << "\": " << it->second;
        }
        oss << "}";
        return oss.str();
    }
    
    return "";
}

const char* ConfigValidator::getTypeName(ConfigDataType type) {
    switch (type) {
        case ConfigDataType::None: return "None";
        case ConfigDataType::Integer: return "Integer";
        case ConfigDataType::Float: return "Float";
        case ConfigDataType::Boolean: return "Boolean";
        case ConfigDataType::String: return "String";
        case ConfigDataType::IntegerArray: return "IntegerArray";
        case ConfigDataType::FloatArray: return "FloatArray";
        case ConfigDataType::BooleanArray: return "BooleanArray";
        case ConfigDataType::StringArray: return "StringArray";
        case ConfigDataType::Object: return "Object";
        case ConfigDataType::Array: return "Array";
        default: return "Unknown";
    }
}

bool ConfigValidator::isType(const EnhancedConfigValue& value, ConfigDataType type) {
    ConfigDataType actualType = getType(value);
    return actualType == type;
}

struct ConfigManager::Impl {
    std::unordered_map<std::string, EnhancedConfigValue> config;
    std::shared_ptr<ConfigSchema> schema;
    bool validationEnabled;
    bool environmentExpansionEnabled;
    bool typeCoercionEnabled;
    std::unordered_map<std::string, std::string> snapshots;
    std::vector<std::string> snapshotNames;
    
    Impl() : validationEnabled(true), environmentExpansionEnabled(true), typeCoercionEnabled(true) {}
};

ConfigManager::ConfigManager() : pImpl(std::make_unique<Impl>()) {}

ConfigManager::~ConfigManager() = default;

bool ConfigManager::loadFromFile(const std::string& filepath, bool validate) {
    // Determine file type by extension
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    if (extension == ".json" || extension == ".JSON") {
        return loadFromJSON(filepath, validate);
    } else if (extension == ".yaml" || extension == ".yml" || extension == ".YAML" || extension == ".YML") {
        return loadFromYAML(filepath, validate);
    } else {
        // Fallback to key=value format
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, validate);
            }
        }
        
        return true;
    }
}

bool ConfigManager::loadFromJSON(const std::string& filepath, bool validate) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json configJson = json::parse(file);
        
        for (auto it = configJson.begin(); it != configJson.end(); ++it) {
            std::string key = it->type_name();
            EnhancedConfigValue value;
            
            // Convert JSON value to EnhancedConfigValue
            if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_array()) {
                // Determine array type
                bool allInt = true;
                bool allFloat = true;
                bool allBool = true;
                bool allString = true;
                
                for (const auto& item : *it) {
                    if (!item.is_number_integer()) allInt = false;
                    if (!item.is_number_float()) allFloat = false;
                    if (!item.is_boolean()) allBool = false;
                    if (!item.is_string()) allString = false;
                }
                
                if (allInt) {
                    std::vector<int64_t> arr;
                    for (const auto& item : *it) {
                        arr.push_back(item.get<int64_t>());
                    }
                    value = arr;
                } else if (allFloat) {
                    std::vector<double> arr;
                    for (const auto& item : *it) {
                        arr.push_back(item.get<double>());
                    }
                    value = arr;
                } else if (allBool) {
                    std::vector<bool> arr;
                    for (const auto& item : *it) {
                        arr.push_back(item.get<bool>());
                    }
                    value = arr;
                } else if (allString) {
                    std::vector<std::string> arr;
                    for (const auto& item : *it) {
                        arr.push_back(item.get<std::string>());
                    }
                    value = arr;
                }
            }
            
            set(it.key(), value, validate);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load JSON config: ") + e.what());
        return false;
    }
}

bool ConfigManager::loadFromYAML(const std::string& filepath, bool validate) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        Node configYaml = YAML::Load(file);
        
        for (const auto& node : configYaml) {
            std::string key = node.first.as<std::string>();
            EnhancedConfigValue value;
            
            // Convert YAML node to EnhancedConfigValue
            if (node.second.IsScalar()) {
                if (node.second.IsInt()) {
                    value = node.second.as<int64_t>();
                } else if (node.second.IsFloat()) {
                    value = node.second.as<double>();
                } else if (node.second.IsBool()) {
                    value = node.second.as<bool>();
                } else {
                    value = node.second.as<std::string>();
                }
            } else if (node.second.IsSequence()) {
                // Determine sequence type
                bool allInt = true;
                bool allFloat = true;
                bool allBool = true;
                bool allString = true;
                
                for (const auto& item : node.second) {
                    if (!item.IsInt()) allInt = false;
                    if (!item.IsFloat()) allFloat = false;
                    if (!item.IsBool()) allBool = false;
                    if (!item.IsScalar()) allString = false;
                }
                
                if (allInt) {
                    std::vector<int64_t> arr;
                    for (const auto& item : node.second) {
                        arr.push_back(item.as<int64_t>());
                    }
                    value = arr;
                } else if (allFloat) {
                    std::vector<double> arr;
                    for (const auto& item : node.second) {
                        arr.push_back(item.as<double>());
                    }
                    value = arr;
                } else if (allBool) {
                    std::vector<bool> arr;
                    for (const auto& item : node.second) {
                        arr.push_back(item.as<bool>());
                    }
                    value = arr;
                } else if (allString) {
                    std::vector<std::string> arr;
                    for (const auto& item : node.second) {
                        arr.push_back(item.as<std::string>());
                    }
                    value = arr;
                }
            }
            
            set(key, value, validate);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load YAML config: ") + e.what());
        return false;
    }
}

bool ConfigManager::saveToFile(const std::string& filepath, bool pretty) const {
    json configJson;
    
    for (const auto& pair : pImpl->config) {
        if (std::holds_alternative<int64_t>(pair.second)) {
            configJson[pair.first] = std::get<int64_t>(pair.second);
        } else if (std::holds_alternative<double>(pair.second)) {
            configJson[pair.first] = std::get<double>(pair.second);
        } else if (std::holds_alternative<bool>(pair.second)) {
            configJson[pair.first] = std::get<bool>(pair.second);
        } else if (std::holds_alternative<std::string>(pair.second)) {
            configJson[pair.first] = std::get<std::string>(pair.second);
        } else if (std::holds_alternative<std::vector<int64_t>>(pair.second)) {
            configJson[pair.first] = std::get<std::vector<int64_t>>(pair.second);
        } else if (std::holds_alternative<std::vector<double>>(pair.second)) {
            configJson[pair.first] = std::get<std::vector<double>>(pair.second);
        } else if (std::holds_alternative<std::vector<bool>>(pair.second)) {
            configJson[pair.first] = std::get<std::vector<bool>>(pair.second);
        } else if (std::holds_alternative<std::vector<std::string>>(pair.second)) {
            configJson[pair.first] = std::get<std::vector<std::string>>(pair.second);
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (pretty) {
        file << configJson.dump(2);
    } else {
        file << configJson.dump();
    }
    
    file.close();
    return true;
}

std::optional<EnhancedConfigValue> ConfigManager::get(const std::string& key) const {
    auto it = pImpl->config.find(key);
    if (it == pImpl->config.end()) {
        return std::nullopt;
    }
    return it->second;
}

template<typename T>
std::optional<T> ConfigManager::get(const std::string& key) const {
    auto value = get(key);
    if (!value.has_value()) {
        return std::nullopt;
    }
    
    if (std::is_same<T, int64_t>::value && std::holds_alternative<int64_t>(value.value())) {
        return std::get<int64_t>(value.value());
    } else if (std::is_same<T, double>::value && std::holds_alternative<double>(value.value())) {
        return std::get<double>(value.value());
    } else if (std::is_same<T, bool>::value && std::holds_alternative<bool>(value.value())) {
        return std::get<bool>(value.value());
    } else if (std::is_same<T, std::string>::value && std::holds_alternative<std::string>(value.value())) {
        return std::get<std::string>(value.value());
    }
    
    return std::nullopt;
}

template<typename T>
T ConfigManager::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value.has_value()) {
        return value.value();
    }
    return defaultValue;
}

bool ConfigManager::set(const std::string& key, const EnhancedConfigValue& value, bool validate) {
    if (validate && pImpl->validationEnabled && pImpl->schema) {
        std::string error = ConfigValidator::validateField(key, value, *pImpl->schema);
        if (!error.empty()) {
            NLM_LOG_ERROR("Configuration validation failed for key '" + key + "': " + error);
            return false;
        }
    }
    
    if (pImpl->environmentExpansionEnabled) {
        std::string* strValue = std::get_if<std::string>(&value);
        if (strValue) {
            if (!expandEnvironmentVariables(*strValue)) {
                NLM_LOG_WARNING("Failed to expand environment variables in config key: " + key);
            }
        }
    }
    
    pImpl->config[key] = value;
    return true;
}

bool ConfigManager::set(const std::string& key, ConfigDataType type, const EnhancedConfigValue& value, bool validate) {
    if (!ConfigValidator::isType(value, type)) {
        if (pImpl->typeCoercionEnabled) {
            auto coercedValue = ConfigValidator::convertValue(value, type);
            if (coercedValue) {
                return set(key, coercedValue.value(), validate);
            }
        }
        return false;
    }
    
    return set(key, value, validate);
}

template<typename T>
bool ConfigManager::set(const std::string& key, const T& value, bool validate) {
    EnhancedConfigValue enhancedValue;
    
    if (std::is_same<T, int64_t>::value) {
        enhancedValue = value;
    } else if (std::is_same<T, double>::value) {
        enhancedValue = value;
    } else if (std::is_same<T, bool>::value) {
        enhancedValue = value;
    } else if (std::is_same<T, std::string>::value) {
        enhancedValue = value;
    } else {
        return false;
    }
    
    return set(key, enhancedValue, validate);
}

bool ConfigManager::has(const std::string& key) const {
    return pImpl->config.find(key) != pImpl->config.end();
}

bool ConfigManager::remove(const std::string& key) {
    auto it = pImpl->config.find(key);
    if (it == pImpl->config.end()) {
        return false;
    }
    pImpl->config.erase(it);
    return true;
}

std::vector<std::string> ConfigManager::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->config.size());
    for (const auto& pair : pImpl->config) {
        keys.push_back(pair.first);
    }
    return keys;
}

void ConfigManager::clear() {
    pImpl->config.clear();
}

bool ConfigManager::merge(const ConfigManager& other) {
    for (const auto& pair : other.pImpl->config) {
        pImpl->config[pair.first] = pair.second;
    }
    return true;
}

std::string ConfigManager::getSummary() const {
    std::ostringstream oss;
    oss << "=== Advanced Configuration Manager ===" << std::endl;
    oss << "Validation enabled: " << (pImpl->validationEnabled ? "Yes" : "No") << std::endl;
    oss << "Environment expansion: " << (pImpl->environmentExpansionEnabled ? "Yes" : "No") << std::endl;
    oss << "Type coercion: " << (pImpl->typeCoercionEnabled ? "Yes" : "No") << std::endl;
    oss << "Total entries: " << pImpl->config.size() << std::endl;
    oss << std::endl;
    
    for (const auto& pair : pImpl->config) {
        oss << pair.first << " = " << ConfigValidator::valueToString(pair.second) << std::endl;
    }
    
    return oss.str();
}

std::vector<std::string> ConfigManager::validate(const ConfigSchema& schema) const {
    return schema.validate(pImpl->config);
}

void ConfigManager::setSchema(const ConfigSchema& schema) {
    pImpl->schema = std::make_shared<ConfigSchema>(schema);
}

const ConfigSchema& ConfigManager::getSchema() const {
    static ConfigSchema emptySchema;
    return pImpl->schema ? *pImpl->schema : emptySchema;
}

void ConfigManager::setValidation(bool enabled) {
    pImpl->validationEnabled = enabled;
}

bool ConfigManager::isValidationEnabled() const {
    return pImpl->validationEnabled;
}

void ConfigManager::setEnvironmentExpansion(bool enabled) {
    pImpl->environmentExpansionEnabled = enabled;
}

bool ConfigManager::isEnvironmentExpansionEnabled() const {
    return pImpl->environmentExpansionEnabled;
}

void ConfigManager::setTypeCoercion(bool enabled) {
    pImpl->typeCoercionEnabled = enabled;
}

bool ConfigManager::isTypeCoercionEnabled() const {
    return pImpl->typeCoercionEnabled;
}

std::string ConfigManager::exportToJSON(bool pretty) const {
    json configJson;
    
    for (const auto& pair : pImpl->config) {
        if (std::holds_alternative<int64_t>(pair.second)) {
            configJson[pair.first] = std::get<int64_t>(pair.second);
        } else if (std::holds_alternative<double>(pair.second)) {
            configJson[pair.first] = std::get<double>(pair.second);
        } else if (std::holds_alternative<bool>(pair.second)) {
            configJson[pair.first] = std::get<bool>(pair.second);
        } else if (std::holds_alternative<std::string>(pair.second)) {
            configJson[pair.first] = std::get<std::string>(pair.second);
        } else if (std::holds_alternative<std::vector<int64_t>>(pair.second)) {
            configJson[pair.first] = std::get<std::vector<int64_t>>(pair.second);
        } else if (std::holds_alternative<std::vector<double>>(pair.second)) {
            configJson[pair.first] = std::get<std::vector<double>>(pair.second);
        } else if (std::holds_alternative<std::vector<bool>>(pair.second)) {
            configJson[pair.first] = std::get<std::vector<bool>>(pair.second);
        } else if (std::holds_alternative<std::vector<std::string>>(pair.second)) {
            configJson[pair.first] = std::get<std::vector<std::string>>(pair.second);
        }
    }
    
    std::ostringstream oss;
    if (pretty) {
        oss << configJson.dump(2);
    } else {
        oss << configJson.dump();
    }
    
    return oss.str();
}

std::string ConfigManager::exportToYAML(bool pretty) const {
    // YAML implementation would go here
    return "YAML export not implemented yet";
}

bool ConfigManager::importFromJSON(const std::string& json) {
    try {
        json configJson = json::parse(json);
        
        for (auto it = configJson.begin(); it != configJson.end(); ++it) {
            std::string key = it.key();
            EnhancedConfigValue value;
            
            if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_array()) {
                bool allInt = true;
                bool allFloat = true;
                bool allBool = true;
                bool allString = true;
                
                for (const auto& item : *it) {
                    if (!item.is_number_integer()) allInt = false;
                    if (!item.is_number_float()) allFloat = false;
                    if (!item.is_boolean()) allBool = false;
                    if (!item.is_string()) allString = false;
                }
                
                if (allInt) {
                    std::vector<int64_t> arr;
                    for (const auto& item : *it) {
                        arr.push_back(item.get<int64_t>());
                    }
                    value = arr;
                } else if (allFloat) {
                    std::vector<double> arr;
                    for (const auto& item : *it) {
                        arr.push_back(item.get<double>());
                    }
                    value = arr;
                } else if (allBool) {
                    std::vector<bool> arr;
                    for (const auto& item : *it) {
                        arr.push_back(item.get<bool>());
                    }
                    value = arr;
                } else if (allString) {
                    std::vector<std::string> arr;
                    for (const auto& item : *it) {
                        arr.push_back(item.get<std::string>());
                    }
                    value = arr;
                }
            }
            
            set(key, value, true);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to import JSON config: ") + e.what());
        return false;
    }
}

bool ConfigManager::importFromYAML(const std::string& yaml) {
    try {
        Node configYaml = YAML::Load(yaml);
        
        for (const auto& node : configYaml) {
            std::string key = node.first.as<std::string>();
            EnhancedConfigValue value;
            
            if (node.second.IsScalar()) {
                if (node.second.IsInt()) {
                    value = node.second.as<int64_t>();
                } else if (node.second.IsFloat()) {
                    value = node.second.as<double>();
                } else if (node.second.IsBool()) {
                    value = node.second.as<bool>();
                } else {
                    value = node.second.as<std::string>();
                }
            } else if (node.second.IsSequence()) {
                bool allInt = true;
                bool allFloat = true;
                bool allBool = true;
                bool allString = true;
                
                for (const auto& item : node.second) {
                    if (!item.IsInt()) allInt = false;
                    if (!item.IsFloat()) allFloat = false;
                    if (!item.IsBool()) allBool = false;
                    if (!item.IsScalar()) allString = false;
                }
                
                if (allInt) {
                    std::vector<int64_t> arr;
                    for (const auto& item : node.second) {
                        arr.push_back(item.as<int64_t>());
                    }
                    value = arr;
                } else if (allFloat) {
                    std::vector<double> arr;
                    for (const auto& item : node.second) {
                        arr.push_back(item.as<double>());
                    }
                    value = arr;
                } else if (allBool) {
                    std::vector<bool> arr;
                    for (const auto& item : node.second) {
                        arr.push_back(item.as<bool>());
                    }
                    value = arr;
                } else if (allString) {
                    std::vector<std::string> arr;
                    for (const auto& item : node.second) {
                        arr.push_back(item.as<std::string>());
                    }
                    value = arr;
                }
            }
            
            set(key, value, true);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to import YAML config: ") + e.what());
        return false;
    }
}

std::vector<std::pair<std::string, EnhancedConfigValue>> ConfigManager::diff(const ConfigManager& other) const {
    std::vector<std::pair<std::string, EnhancedConfigValue>> changes;
    
    // Find added and changed entries
    for (const auto& pair : pImpl->config) {
        auto it = other.pImpl->config.find(pair.first);
        if (it == other.pImpl->config.end() || it->second != pair.second) {
            changes.emplace_back(pair.first, pair.second);
        }
    }
    
    // Find removed entries
    for (const auto& pair : other.pImpl->config) {
        auto it = pImpl->config.find(pair.first);
        if (it == pImpl->config.end()) {
            changes.emplace_back(pair.first, pair.second);
        }
    }
    
    return changes;
}

bool ConfigManager::saveSnapshot(const std::string& name) {
    std::string key = "snapshot_" + name;
    std::string value;
    
    std::ostringstream oss;
    oss << exportToJSON();
    value = oss.str();
    
    pImpl->snapshots[key] = value;
    pImpl->snapshotNames.push_back(name);
    return true;
}

bool ConfigManager::loadSnapshot(const std::string& name) {
    std::string key = "snapshot_" + name;
    auto it = pImpl->snapshots.find(key);
    if (it == pImpl->snapshots.end()) {
        return false;
    }
    
    std::istringstream iss(it->second);
    json configJson;
    
    try {
        iss >> configJson;
        
        // Clear current config
        clear();
        
        // Import from JSON
        return importFromJSON(iss.str());
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load snapshot: ") + e.what());
        return false;
    }
}

std::vector<std::string> ConfigManager::getSnapshots() const {
    return pImpl->snapshotNames;
}

bool ConfigManager::deleteSnapshot(const std::string& name) {
    std::string key = "snapshot_" + name;
    auto it = pImpl->snapshots.find(key);
    if (it == pImpl->snapshots.end()) {
        return false;
    }
    
    pImpl->snapshots.erase(it);
    
    // Remove from snapshot names
    auto nameIt = std::find(pImpl->snapshotNames.begin(), pImpl->snapshotNames.end(), name);
    if (nameIt != pImpl->snapshotNames.end()) {
        pImpl->snapshotNames.erase(nameIt);
    }
    
    return true;
}

ConfigManager& ConfigManager::getGlobal() {
    static std::shared_ptr<ConfigManager> globalInstance;
    if (!globalInstance) {
        globalInstance = std::make_shared<ConfigManager>();
    }
    return *globalInstance;
}

void ConfigManager::setGlobal(std::shared_ptr<ConfigManager> manager) {
    static std::shared_ptr<ConfigManager> globalInstance = manager;
}

bool ConfigManager::expandEnvironmentVariables(std::string& value) const {
    std::string result = value;
    size_t pos = 0;
    
    while ((pos = result.find('$', pos)) != std::string::npos) {
        size_t endPos = result.find('}', pos + 1);
        if (endPos == std::string::npos) {
            break;
        }
        
        std::string varName = result.substr(pos + 1, endPos - pos - 1);
        
        // Get environment variable
        const char* envValue = std::getenv(varName.c_str());
        if (envValue) {
            result.replace(pos, endPos - pos + 1, envValue);
        } else {
            result.replace(pos, endPos - pos + 1, "");
        }
    }
    
    if (result != value) {
        value = result;
        return true;
    }
    
    return false;
}

std::optional<EnhancedConfigValue> ConfigManager::coerceType(const EnhancedConfigValue& value, ConfigDataType targetType) const {
    if (ConfigValidator::isType(value, targetType)) {
        return value;
    }
    
    std::optional<EnhancedConfigValue> result;
    
    if (targetType == ConfigDataType::Integer) {
        if (std::holds_alternative<double>(value)) {
            result = static_cast<int64_t>(std::get<double>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            try {
                result = std::stoll(std::get<std::string>(value));
            } catch (...) {}
        }
    } else if (targetType == ConfigDataType::Float) {
        if (std::holds_alternative<int64_t>(value)) {
            result = static_cast<double>(std::get<int64_t>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            try {
                result = std::stod(std::get<std::string>(value));
            } catch (...) {}
        }
    } else if (targetType == ConfigDataType::Boolean) {
        if (std::holds_alternative<std::string>(value)) {
            std::string str = std::get<std::string>(value);
            if (str == "true" || str == "1" || str == "yes" || str == "on") {
                result = true;
            } else if (str == "false" || str == "0" || str == "no" || str == "off") {
                result = false;
            }
        }
    }
    
    return result;
}

bool ConfigManager::resolveReference(const std::string& key, EnhancedConfigValue& value) const {
    // Resolve references (e.g., ${VAR} or $VAR)
    std::string* strValue = std::get_if<std::string>(&value);
    if (!strValue) return false;
    
    std::string resolved = *strValue;
    size_t pos = 0;
    bool changed = false;
    
    while ((pos = resolved.find('$', pos)) != std::string::npos) {
        if (pos + 1 < resolved.size() && resolved[pos + 1] == '{') {
            size_t endPos = resolved.find('}', pos + 2);
            if (endPos == std::string::npos) {
                break;
            }
            
            std::string varName = resolved.substr(pos + 2, endPos - pos - 2);
            const char* envValue = std::getenv(varName.c_str());
            if (envValue) {
                resolved.replace(pos, endPos - pos + 1, envValue);
                changed = true;
            }
        } else {
            size_t endPos = resolved.find_first_of(" \t\n\r", pos + 1);
            if (endPos == std::string::npos) {
                endPos = resolved.size();
            }
            
            std::string varName = resolved.substr(pos + 1, endPos - pos - 1);
            const char* envValue = std::getenv(varName.c_str());
            if (envValue) {
                resolved.replace(pos, endPos - pos, envValue);
                changed = true;
            }
        }
    }
    
    if (changed) {
        value = resolved;
    }
    
    return changed;
}

} // namespace nlm
