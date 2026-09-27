// Config.cpp - Enhanced Configuration Management System
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cctype>
#include <cstring>
#include <unordered_set>
#include <iostream>
#include <regex>

// Include our mock headers
#include "nlohmann/json.hpp"
#include "yaml-cpp/yaml.h"

// Validation Rule Implementations
bool TypeValidationRule::validate(const std::string& key, const ConfigValue& value) const {
    if (!matchesPattern(key)) return true; // Not matching pattern, skip validation
    
    switch (type_) {
        case ValueType::Int:
            return std::holds_alternative<int>(value);
        case ValueType::Int64:
            return std::holds_alternative<int64_t>(value) || std::holds_alternative<int>(value);
        case ValueType::Double:
            return std::holds_alternative<double>(value) || std::holds_alternative<int>(value) || 
                   std::holds_alternative<int64_t>(value);
        case ValueType::Bool:
            return std::holds_alternative<bool>(value);
        case ValueType::String:
            return std::holds_alternative<std::string>(value);
        case ValueType::VectorInt:
            return std::holds_alternative<std::vector<int>>(value);
        case ValueType::VectorDouble:
            return std::holds_alternative<std::vector<double>>(value);
        case ValueType::VectorString:
            return std::holds_alternative<std::vector<std::string>>(value);
        case ValueType::Any:
            return true;
    }
    return false;
}

std::string TypeValidationRule::getErrorMessage() const {
    std::string typeName;
    switch (type_) {
        case ValueType::Int: typeName = "integer"; break;
        case ValueType::Int64: typeName = "64-bit integer"; break;
        case ValueType::Double: typeName = "floating-point number"; break;
        case ValueType::Bool: typeName = "boolean"; break;
        case ValueType::String: typeName = "string"; break;
        case ValueType::VectorInt: typeName = "array of integers"; break;
        case ValueType::VectorDouble: typeName = "array of numbers"; break;
        case ValueType::VectorString: typeName = "array of strings"; break;
        case ValueType::Any: typeName = "any type"; break;
    }
    
    return "Key '" + keyPattern_ + "' must be of type " + typeName + ".";
}

std::string TypeValidationRule::getExpectedValueType() const {
    switch (type_) {
        case ValueType::Int: return "int";
        case ValueType::Int64: return "int64_t";
        case ValueType::Double: return "double";
        case ValueType::Bool: return "bool";
        case ValueType::String: return "std::string";
        case ValueType::VectorInt: return "std::vector<int>";
        case ValueType::VectorDouble: return "std::vector<double>";
        case ValueType::VectorString: return "std::vector<std::string>";
        case ValueType::Any: return "any";
    }
    return "unknown";
}

bool RangeValidationRule::validate(const std::string& key, const ConfigValue& value) const {
    if (!matchesPattern(key)) return true;
    
    try {
        double actualValue = 0.0;
        
        if (std::holds_alternative<int>(value)) {
            actualValue = static_cast<double>(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            actualValue = static_cast<double>(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            actualValue = std::get<double>(value);
        } else {
            return false; // Wrong type
        }
        
        return actualValue >= min_ && actualValue <= max_;
    } catch (...) {
        return false;
    }
}

std::string RangeValidationRule::getErrorMessage() const {
    return "Key '" + keyPattern_ + "' must be between " + std::to_string(min_) + 
           " and " + std::to_string(max_) + " (type: " + type_ + ").";
}

bool EnumValidationRule::validate(const std::string& key, const ConfigValue& value) const {
    if (!matchesPattern(key)) return true;
    
    if (std::holds_alternative<std::string>(value)) {
        const std::string& strValue = std::get<std::string>(value);
        return std::find(allowedValues_.begin(), allowedValues_.end(), strValue) != allowedValues_.end();
    }
    return false;
}

std::string EnumValidationRule::getErrorMessage() const {
    std::string valuesStr = "";
    for (size_t i = 0; i < allowedValues_.size(); ++i) {
        if (i > 0) valuesStr += ", ";
        valuesStr += "'" + allowedValues_[i] + "'";
    }
    
    return "Key '" + keyPattern_ + "' must be one of: " + valuesStr + ".";
}

// ConfigValidator Implementation
void ConfigValidator::addRule(std::unique_ptr<ValidationRule> rule) {
    rules_.push_back(std::move(rule));
}

std::vector<ValidationError> ConfigValidator::validate(const std::unordered_map<std::string, ConfigValue>& config) const {
    std::vector<ValidationError> errors;
    
    for (const auto& rule : rules_) {
        // For each rule, check if it matches any keys in the config
        for (const auto& [key, value] : config) {
            if (rule->matchesPattern(key)) {
                if (!rule->validate(key, value)) {
                    ValidationError error(rule->getErrorMessage(), key, 
                                         rule->getExpectedValueType(), getValueString(value));
                    errors.push_back(error);
                }
            }
        }
    }
    
    return errors;
}

bool ConfigValidator::hasRuleForKey(const std::string& key) const {
    for (const auto& rule : rules_) {
        if (rule->matchesPattern(key)) {
            return true;
        }
    }
    return false;
}

std::string ConfigValidator::getRuleDescription(const ValidationRule& rule) const {
    return rule.getErrorMessage();
}

// ConfigTemplateManager Implementation
void ConfigTemplateManager::addTemplate(std::unique_ptr<ConfigTemplate> template) {
    templates_[template->getTemplateName()] = std::move(template);
}

std::optional<ConfigTemplate> ConfigTemplateManager::getTemplate(const std::string& name) const {
    auto it = templates_.find(name);
    if (it != templates_.end()) {
        return std::make_optional(*it->second);
    }
    return std::nullopt;
}

bool ConfigTemplateManager::hasTemplate(const std::string& name) const {
    return templates_.find(name) != templates_.end();
}

std::vector<std::string> ConfigTemplateManager::getTemplateNames() const {
    std::vector<std::string> names;
    for (const auto& pair : templates_) {
        names.push_back(pair.first);
    }
    return names;
}

// ConfigMigrator Implementation
void ConfigMigrator::addMigration(std::unique_ptr<ConfigMigration> migration) {
    migrations_.push_back(std::move(migration));
}

bool ConfigMigrator::migrate(std::unordered_map<std::string, ConfigValue>& config, 
                             int fromVersion, int toVersion) const {
    for (const auto& migration : migrations_) {
        if (migration->getFromVersion() == fromVersion && 
            migration->getToVersion() == toVersion) {
            return migration->migrate(config);
        }
    }
    return false; // No migration found for this version range
}

// ConfigInheritance Implementation
void ConfigInheritance::addParent(const ParentConfig& parent) {
    parents_.push_back(parent);
}

bool ConfigInheritance::applyInheritance(Config& config) const {
    bool allSuccess = true;
    for (const auto& parent : parents_) {
        if (!parent.loadParentConfig(config)) {
            allSuccess = false;
        }
    }
    return allSuccess;
}

// ConfigMigration Implementations
class IntToDoubleMigration : public ConfigMigration {
public:
    IntToDoubleMigration(int fromVersion, int toVersion) 
        : ConfigMigration(fromVersion, toVersion) {}
    
    bool migrate(std::unordered_map<std::string, ConfigValue>& config) override {
        bool changed = false;
        
        for (auto& [key, value] : config) {
            if (std::holds_alternative<int>(value)) {
                double newValue = static_cast<double>(std::get<int>(value));
                value = newValue;
                changed = true;
            }
        }
        
        return changed;
    }
};

class RenameKeyMigration : public ConfigMigration {
public:
    RenameKeyMigration(int fromVersion, int toVersion, 
                      const std::string& oldKey, const std::string& newKey)
        : ConfigMigration(fromVersion, toVersion), oldKey_(oldKey), newKey_(newKey) {}
    
    bool migrate(std::unordered_map<std::string, ConfigValue>& config) override {
        bool changed = false;
        
        auto it = config.find(oldKey_);
        if (it != config.end()) {
            config[newKey_] = it->second;
            config.erase(it);
            changed = true;
        }
        
        return changed;
    }
    
private:
    std::string oldKey_;
    std::string newKey_;
};

class AddDefaultValueMigration : public ConfigMigration {
public:
    AddDefaultValueMigration(int fromVersion, int toVersion,
                           const std::string& key, const ConfigValue& value,
                           const std::string& description = "")
        : ConfigMigration(fromVersion, toVersion), key_(key), value_(value), 
          description_(description) {}
    
    bool migrate(std::unordered_map<std::string, ConfigValue>& config) override {
        if (config.find(key_) == config.end()) {
            config[key_] = value_;
            return true;
        }
        return false;
    }
    
private:
    std::string key_;
    ConfigValue value_;
    std::string description_;
};

// ParentConfig Implementations
bool ParentConfig::loadParentConfig(Config& config) const {
    if (parentPath_.empty()) {
        return false;
    }
    
    // Try to load parent configuration
    return config.loadFromFile(parentPath_);
}

// ConfigTemplate Implementations
def ConfigTemplate::addDefaultValue(const std::string& key, const ConfigValue& value,
                               const std::string& description) {
    defaults_[key] = std::make_pair(value, description);
}

def std::optional<ConfigValue> ConfigTemplate::getDefaultValue(const std::string& key) const {
    auto it = defaults_.find(key);
    if (it != defaults_.end()) {
        return std::make_optional(it->second.first);
    }
    return std::nullopt;
}

// Config Implementations
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Additional internal storage for enhanced features
    std::unordered_map<std::string, ConfigValue> templateDefaults;
    std::vector<std::string> validationErrors;
    std::unordered_map<std::string, ConfigValue> environmentOverrides;
};

Config::Config() : pImpl(std::make_unique<Impl>()), version_(1) {
    // Initialize validators with default rules
    validator_ = std::make_unique<ConfigValidator>();
    inheritance_ = std::make_unique<ConfigInheritance>();
    schema_ = std::make_unique<ConfigSchema>();
    templateManager_ = std::make_unique<ConfigTemplateManager>();
    migrator_ = std::make_unique<ConfigMigrator>();
    
    // Add default migrations
    migrator_->addMigration(std::make_unique<IntToDoubleMigration>(1, 2));
    migrator_->addMigration(std::make_unique<RenameKeyMigration>(2, 3, "old_param", "new_param"));
    migrator_->addMigration(std::make_unique<AddDefaultValueMigration>(3, 4, "timeout", 30.0, "Default timeout in seconds"));
    
    // Add default validation rules
    validator_->addRule(std::make_unique<TypeValidationRule>(TypeValidationRule::ValueType::String, "*"));
    validator_->addRule(std::make_unique<TypeValidationRule>(TypeValidationRule::ValueType::Int, "port*"));
    validator_->addRule(std::make_unique<RangeValidationRule>("port*", 1, 65535, "port"));
    validator_->addRule(std::make_unique<EnumValidationRule>("log_level*", 
                                                             {"debug", "info", "warn", "error"}, "log level"));
    
    // Set default environment variable prefix
    envVarPrefix_ = "NLM_";
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFileWithValidation(filepath);
}

bool Config::loadFromFileWithValidation(const std::string& filepath) {
    // Validate file format before loading
    if (!validateFileFormat(filepath)) {
        return false;
    }
    
    // Detect version from file if possible
    int versionHint = 1; // Default version
    
    // Try to load with version hint
    return loadFromFile(filepath, versionHint);
}

bool Config::loadFromFile(const std::string& filepath, int versionHint) {
    clear(); // Clear existing configuration
    
    // Detect format based on file extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    
    // Try JSON format first if extension is .json
    if (ext == ".json") {
        success = loadJsonFile(filepath);
    }
    // Try YAML format if extension is .yaml or .yml
    else if (ext == ".yaml" || ext == ".yml") {
        success = loadYamlFile(filepath);
    }
    // For .cfg files and others, try key=value format first for backward compatibility
    else if (ext == ".cfg") {
        success = loadKeyValueFile(filepath);
    }
    // Auto-detect format based on content
    else {
        // Try JSON first
        success = loadJsonFile(filepath);
        if (!success) {
            // Try YAML if JSON fails
            success = loadYamlFile(filepath);
            if (!success) {
                // Fall back to key=value format
                success = loadKeyValueFile(filepath);
            }
        }
    }
    
    if (success) {
        // Apply migration if needed
        if (versionHint > 1 && versionHint < getVersion()) {
            migrate(versionHint);
        }
        
        // Apply inheritance if parents are set
        if (hasInheritance()) {
            applyInheritance();
        }
        
        // Apply environment overrides
        applyEnvironmentOverrides();
        
        // Apply template defaults
        for (const auto& templateName : templateManager_->getTemplateNames()) {
            applyTemplate(templateName);
        }
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        size_t pos = arg.find('=');
        if (pos != std::string::npos && arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle --key value format (next argument)
        else if (arg.substr(0, 2) == "--" && i + 1 < argc) {
            std::string key = arg.substr(2);
            std::string value = argv[++i];
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::CommandLine);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    return saveToFileWithValidation(filepath);
}

bool Config::saveToFileWithValidation(const std::string& filepath) const {
    // Validate file format before saving
    if (!validateFileFormat(filepath)) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Detect format based on file extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return saveJsonFile(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return saveYamlFile(filepath);
    } else {
        // Default to key=value format for backward compatibility
        return saveKeyValueFile(filepath);
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Check if key already exists
    for (auto& entry : pImpl->entries) {
        if (entry.key == key) {
            entry.value = value;
            entry.source = source;
            return;
        }
    }
    
    // Add new entry
    pImpl->entries.push_back(ConfigEntry(key, value, source));
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

std::optional<ConfigValue> Config::get(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            return entry.value;
        }
    }
    return std::nullopt;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto value = get(key);
    if (!value.has_value()) {
        return std::nullopt;
    }
    
    try {
        if constexpr (std::is_same_v<T, int>) {
            if (std::holds_alternative<int>(*value)) {
                return std::get<int>(*value);
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<int64_t>(*value)) {
                return std::get<int64_t>(*value);
            } else if (std::holds_alternative<int>(*value)) {
                return static_cast<int64_t>(std::get<int>(*value));
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(*value)) {
                return std::get<double>(*value);
            } else if (std::holds_alternative<int>(*value)) {
                return static_cast<double>(std::get<int>(*value));
            } else if (std::holds_alternative<int64_t>(*value)) {
                return static_cast<double>(std::get<int64_t>(*value));
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(*value)) {
                return std::get<bool>(*value);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(*value)) {
                return std::get<std::string>(*value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (std::holds_alternative<std::vector<int>>(*value)) {
                return std::get<std::vector<int>>(*value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (std::holds_alternative<std::vector<double>>(*value)) {
                return std::get<std::vector<double>>(*value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (std::holds_alternative<std::vector<std::string>>(*value)) {
                return std::get<std::vector<std::string>>(*value);
            }
        }
    } catch (...) {
        // Conversion failed
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value.has_value()) {
        return *value;
    }
    return defaultValue;
}

bool Config::has(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            return true;
        }
    }
    return false;
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& entry) { return entry.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->validationErrors.clear();
    pImpl->environmentOverrides.clear();
    pImpl->templateDefaults.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::getMetadata() const {
    std::ostringstream oss;
    oss << "Configuration Metadata:\n";
    oss << "  Version: " << version_ << "\n";
    oss << "  Environment Prefix: " << envVarPrefix_ << "\n";
    oss << "  Inheritance Count: " << inheritance_->getParentCount() << "\n";
    oss << "  Template Count: " << templateManager_->getTemplateNames().size() << "\n";
    oss << "  Schema Rules: " << schema_->getRuleCount() << "\n";
    oss << "  Validation Errors: " << pImpl->validationErrors.size() << "\n";
    
    return oss.str();
}

void Config::addValidationRule(std::unique_ptr<ValidationRule> rule) {
    validator_->addRule(std::move(rule));
}

std::vector<ValidationError> Config::validate() const {
    return validateInternal();
}

bool Config::isValid() const {
    return validateInternal().empty();
}

void Config::addParentConfig(const ParentConfig& parent) {
    inheritance_->addParent(parent);
}

bool Config::hasInheritance() const {
    return inheritance_->hasParent();
}

bool Config::applyInheritance() {
    // Create a temporary config to apply parent values
    Config tempConfig;
    tempConfig = *this; // This would be a move operation in a real implementation
    
    bool success = inheritance_->applyInheritance(tempConfig);
    
    // Merge inherited values (child overrides parent)
    // In a real implementation, this would be handled properly
    
    return success;
}

void Config::setSchema(const ConfigSchema& schema) {
    // In a real implementation, this would copy the schema
    schema_ = std::make_unique<ConfigSchema>(schema);
}

bool Config::validateSchema() const {
    if (!schema_) {
        return true; // No schema defined means no validation
    }
    
    // Convert internal entries to map for schema validation
    std::unordered_map<std::string, ConfigValue> configMap;
    for (const auto& entry : pImpl->entries) {
        configMap[entry.key] = entry.value;
    }
    
    auto errors = schema_->validate(configMap);
    pImpl->validationErrors.clear();
    for (const auto& error : errors) {
        pImpl->validationErrors.push_back(error.message);
    }
    
    return errors.empty();
}

void Config::addTemplate(std::unique_ptr<ConfigTemplate> template) {
    templateManager_->addTemplate(std::move(template));
}

bool Config::applyTemplate(const std::string& templateName) {
    auto templateOpt = templateManager_->getTemplate(templateName);
    if (!templateOpt.has_value()) {
        return false;
    }
    
    const auto& template = templateOpt.value();
    bool applied = false;
    
    for (const auto& [key, valueDesc] : template.defaults_) {
        const auto& value = valueDesc.first;
        const auto& description = valueDesc.second;
        
        if (!has(key)) {
            set(key, value, ConfigSource::Default);
            pImpl->templateDefaults[key] = value;
            applied = true;
        }
    }
    
    return applied;
}

void Config::setMigrator(const ConfigMigrator& migrator) {
    migrator_ = std::make_unique<ConfigMigrator>(migrator);
}

bool Config::migrate(int targetVersion) {
    if (!migrator_) {
        return targetVersion == version_;
    }
    
    // Convert internal entries to map for migration
    std::unordered_map<std::string, ConfigValue> configMap;
    for (const auto& entry : pImpl->entries) {
        configMap[entry.key] = entry.value;
    }
    
    bool success = migrator_->migrate(configMap, version_, targetVersion);
    
    if (success) {
        // Update internal entries
        pImpl->entries.clear();
        for (const auto& [key, value] : configMap) {
            pImpl->entries.push_back(ConfigEntry(key, value, ConfigSource::File));
        }
        version_ = targetVersion;
    }
    
    return success;
}

void Config::setEnvironmentVariablePrefix(const std::string& prefix) {
    envVarPrefix_ = prefix;
}

bool Config::applyEnvironmentOverrides() {
    return applyEnvironmentOverridesInternal();
}

bool Config::loadFromEnvironment() {
    return applyEnvironmentOverridesInternal();
}

bool Config::applyEnvironmentOverridesInternal() {
    bool applied = false;
    
    for (const auto& [key, value] : pImpl->entries) {
        std::string envKey = envVarPrefix_ + toUpper(key);
        
        // Try to get environment variable
        const char* envValue = std::getenv(envKey.c_str());
        if (envValue != nullptr) {
            std::string envStr(envValue);
            
            // Convert to appropriate type
            if (std::holds_alternative<int>(value)) {
                try {
                    int intValue = std::stoi(envStr);
                    set(key, intValue, ConfigSource::Environment);
                    applied = true;
                } catch (...) {}
            } else if (std::holds_alternative<double>(value)) {
                try {
                    double doubleValue = std::stod(envStr);
                    set(key, doubleValue, ConfigSource::Environment);
                    applied = true;
                } catch (...) {}
            } else if (std::holds_alternative<bool>(value)) {
                // Convert common boolean strings
                std::string lowerEnv = toLower(envStr);
                if (lowerEnv == "true" || lowerEnv == "yes" || lowerEnv == "1") {
                    set(key, true, ConfigSource::Environment);
                    applied = true;
                } else if (lowerEnv == "false" || lowerEnv == "no" || lowerEnv == "0") {
                    set(key, false, ConfigSource::Environment);
                    applied = true;
                }
            } else if (std::holds_alternative<std::string>(value)) {
                set(key, envStr, ConfigSource::Environment);
                applied = true;
            }
        }
    }
    
    return applied;
}

std::vector<ValidationError> Config::validateInternal() const {
    // Convert internal entries to map for validation
    std::unordered_map<std::string, ConfigValue> configMap;
    for (const auto& entry : pImpl->entries) {
        configMap[entry.key] = entry.value;
    }
    
    return validator_->validate(configMap);
}

bool Config::validateFileFormat(const std::string& filepath) const {
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return validateJsonFormat(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return validateYamlFormat(filepath);
    } else if (ext == ".cfg") {
        return validateKeyValueFormat(filepath);
    } else {
        // For unknown extensions, try to detect
        return validateJsonFormat(filepath) || 
               validateYamlFormat(filepath) || 
               validateKeyValueFormat(filepath);
    }
}

bool Config::validateJsonFormat(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json::parse(file);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::validateYamlFormat(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Load(file);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::validateKeyValueFormat(const std::string& filepath) const {
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
        if (pos == std::string::npos) {
            return false; // Invalid format
        }
    }
    
    return true;
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string Config::getValueString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        oss << arg;
    }, value);
    return oss.str();
}

// Template method instantiations
namespace {

template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // anonymous namespace

} // namespace nlm

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Phase 2: Implement proper JSON/YAML parser with backward compatibility
    
    // Detect format based on file extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    
    // Try JSON format first if extension is .json
    if (ext == ".json") {
        success = loadJsonFile(filepath);
    }
    // Try YAML format if extension is .yaml or .yml
    else if (ext == ".yaml" || ext == ".yml") {
        success = loadYamlFile(filepath);
    }
    // For .cfg files and others, try key=value format first for backward compatibility
    else if (ext == ".cfg") {
        success = loadKeyValueFile(filepath);
    }
    // Auto-detect format based on content
    else {
        // Try JSON first
        success = loadJsonFile(filepath);
        if (!success) {
            // Try YAML if JSON fails
            success = loadYamlFile(filepath);
            if (!success) {
                // Fall back to key=value format
                success = loadKeyValueFile(filepath);
            }
        }
    }
    
    return success;
}

bool Config::loadKeyValueFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json json_data = nlohmann::json::parse(file);
        convertJsonToConfig(json_data);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadYamlFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node yaml_data = YAML::Load(file);
        convertYamlToConfig(yaml_data);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void Config::convertJsonToConfig(const nlohmann::json& json_data) {
    for (auto it = json_data.begin(); it != json_data.end(); ++it) {
        std::string key = it.key();
        std::optional<ConfigValue> value = convertJsonValue(*it);
        
        if (value.has_value()) {
            set(key, value.value(), ConfigSource::File);
        }
    }
}

void Config::convertYamlToConfig(const YAML::Node& yaml_data) {
    for (auto it = yaml_data.begin(); it != yaml_data.end(); ++it) {
        std::string key = it->first.as<std::string>();
        std::optional<ConfigValue> value = convertYamlValue(*it);
        
        if (value.has_value()) {
            set(key, value.value(), ConfigSource::File);
        }
    }
}

std::optional<ConfigValue> Config::convertJsonValue(const nlohmann::json& json_value) {
    if (json_value.is_string()) {
        return json_value.get<std::string>();
    } else if (json_value.is_number_integer()) {
        return json_value.get<int>();
    } else if (json_value.is_number_unsigned()) {
        return json_value.get<uint64_t>();
    } else if (json_value.is_number()) {
        return json_value.get<double>();
    } else if (json_value.is_boolean()) {
        return json_value.get<bool>();
    } else if (json_value.is_array()) {
        if (json_value.empty()) {
            return std::vector<std::string>{};
        }
        
        // Try to detect array type
        bool all_strings = std::all_of(json_value.begin(), json_value.end(),
            [](const nlohmann::json& item) { return item.is_string(); });
        bool all_numbers = std::all_of(json_value.begin(), json_value.end(),
            [](const nlohmann::json& item) { return item.is_number(); });
        
        if (all_strings) {
            std::vector<std::string> result;
            for (const auto& item : json_value) {
                result.push_back(item.get<std::string>());
            }
            return result;
        } else if (all_numbers) {
            // Check if all are integers
            bool all_integers = std::all_of(json_value.begin(), json_value.end(),
                [](const nlohmann::json& item) { return item.is_number_integer(); });
            
            if (all_integers) {
                std::vector<int> result;
                for (const auto& item : json_value) {
                    result.push_back(item.get<int>());
                }
                return result;
            } else {
                std::vector<double> result;
                for (const auto& item : json_value) {
                    result.push_back(item.get<double>());
                }
                return result;
            }
        }
    }
    
    return std::nullopt;
}

std::optional<ConfigValue> Config::convertYamlValue(const YAML::Node& yaml_value) {
    if (yaml_value.IsScalar()) {
        if (yaml_value.IsBoolean()) {
            return yaml_value.as<bool>();
        } else if (yaml_value.IsInteger()) {
            return yaml_value.as<int>();
        } else if (yaml_value.IsDouble()) {
            return yaml_value.as<double>();
        } else {
            return yaml_value.as<std::string>();
        }
    } else if (yaml_value.IsSequence()) {
        if (yaml_value.empty()) {
            return std::vector<std::string>{};
        }
        
        // Try to detect sequence type
        bool all_strings = std::all_of(yaml_value.begin(), yaml_value.end(),
            [](const YAML::Node& item) { return item.IsScalar() && item.IsString(); });
        bool all_integers = std::all_of(yaml_value.begin(), yaml_value.end(),
            [](const YAML::Node& item) { return item.IsScalar() && item.IsInteger(); });
        bool all_doubles = std::all_of(yaml_value.begin(), yaml_value.end(),
            [](const YAML::Node& item) { return item.IsScalar() && item.IsDouble(); });
        
        if (all_strings) {
            std::vector<std::string> result;
            for (const auto& item : yaml_value) {
                result.push_back(item.as<std::string>());
            }
            return result;
        } else if (all_integers) {
            std::vector<int> result;
            for (const auto& item : yaml_value) {
                result.push_back(item.as<int>());
            }
            return result;
        } else if (all_doubles) {
            std::vector<double> result;
            for (const auto& item : yaml_value) {
                result.push_back(item.as<double>());
            }
            return result;
        }
    }
    
    return std::nullopt;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Detect format based on file extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return saveJsonFile(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return saveYamlFile(filepath);
    } else {
        // Default to key=value format for backward compatibility
        return saveKeyValueFile(filepath);
    }
}

bool Config::saveKeyValueFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by Config class\n\n";
    
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [\"";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << "\", \"";
                    file << arg[i];
                }
                file << "\"]\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            }
        }, entry.value);
        file << "\n";
    }
    
    return true;
}

bool Config::saveJsonFile(const std::string& filepath) const {
    nlohmann::json json_data;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&json_data, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                json_data[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                json_data[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                json_data[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                json_data[entry.key] = arg;
            } else {
                json_data[entry.key] = arg;
            }
        }, entry.value);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << json_data.dump(2);
    return true;
}

bool Config::saveYamlFile(const std::string& filepath) const {
    YAML::Emitter yaml_emitter;
    yaml_emitter.SetIndent(2);
    yaml_emitter.SetMapFormat(YAML::E_MultiDoc);
    
    yaml_emitter << YAML::BeginDoc;
    yaml_emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        yaml_emitter << YAML::Key << entry.key;
        std::visit([&yaml_emitter](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                yaml_emitter << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                yaml_emitter << arg;
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t>) {
                yaml_emitter << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                yaml_emitter << arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                yaml_emitter << YAML::BeginSeq;
                for (const auto& item : arg) {
                    yaml_emitter << item;
                }
                yaml_emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                yaml_emitter << YAML::BeginSeq;
                for (const auto& item : arg) {
                    yaml_emitter << item;
                }
                yaml_emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                yaml_emitter << YAML::BeginSeq;
                for (const auto& item : arg) {
                    yaml_emitter << item;
                }
                yaml_emitter << YAML::EndSeq;
            }
        }, entry.value);
    }
    
    yaml_emitter << YAML::EndMap;
    yaml_emitter << YAML::EndDoc;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << yaml_emitter.c_str();
    return true;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

}

} // namespace nlm