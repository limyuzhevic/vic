#include "ConfigManager.hpp"
#include <algorithm>
#include <sstream>
#include <unordered_set>
#include <chrono>
#include <stdexcept>

namespace nlm {

struct ConfigManager::Impl {
    std::shared_ptr<Config> config;
    std::vector<ConfigSchema> schema;
    std::unordered_set<ConfigChangeListener*> listeners;
    std::unordered_map<std::string, ConfigValue> defaultValues;
    std::unordered_map<std::string, std::string> descriptions;
    std::chrono::steady_clock::time_point lastModified;
    std::unordered_map<std::string, std::vector<std::string>> templates;
};

ConfigManager::ConfigManager() : pImpl(std::make_unique<Impl>()) {
    pImpl->config = std::make_shared<Config>();
    pImpl->lastModified = std::chrono::steady_clock::now();
}

ConfigManager::~ConfigManager() = default;

bool ConfigManager::loadFromFile(const std::string& filepath) {
    if (pImpl->config->loadFromFile(filepath)) {
        pImpl->lastModified = std::chrono::steady_clock::now();
        return true;
    }
    return false;
}

bool ConfigManager::loadFromArgs(int argc, char** argv) {
    pImpl->config->loadFromArgs(argc, argv);
    pImpl->lastModified = std::chrono::steady_clock::now();
    return true;
}

bool ConfigManager::saveToFile(const std::string& filepath) const {
    return pImpl->config->saveToFile(filepath);
}

template<typename T>
std::optional<T> ConfigManager::get(const std::string& key) const {
    return pImpl->config->get<T>(key);
}

template<typename T>
T ConfigManager::getOr(const std::string& key, const T& defaultValue) const {
    return pImpl->config->getOr<T>(key, defaultValue);
}

template<typename T>
bool ConfigManager::set(const std::string& key, const T& value, ConfigSource source) {
    std::optional<T> oldValue = get<T>(key);
    ConfigValue oldConfigValue;
    
    if (oldValue.has_value()) {
        oldConfigValue = ConfigValue(oldValue.value());
    }
    
    bool success = pImpl->config->set<T>(key, value, source);
    
    if (success) {
        ConfigValue newValue = ConfigValue(value);
        pImpl->lastModified = std::chrono::steady_clock::now();
        notifyListeners(key, oldConfigValue, newValue);
    }
    
    return success;
}

void ConfigManager::applySchema(const std::vector<ConfigSchema>& schema) {
    pImpl->schema = schema;
    
    for (const auto& entry : schema) {
        if (!pImpl->config->has(entry.key)) {
            if (entry.required) {
                throw std::runtime_error("Required configuration key missing: " + entry.key);
            }
            pImpl->config->set(entry.key, entry.defaultValue, entry.source);
        }
        
        pImpl->descriptions[entry.key] = entry.description;
        pImpl->defaultValues[entry.key] = entry.defaultValue;
    }
}

void ConfigManager::validateCurrentConfig() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->schema) {
        if (entry.required && !pImpl->config->has(entry.key)) {
            errors.push_back("Required configuration key missing: " + entry.key);
        } else if (pImpl->config->has(entry.key)) {
            auto value = pImpl->config->get(entry.key);
            if (!isValueValid(entry.key, value)) {
                errors.push_back("Invalid value for key " + entry.key + ": " + 
                               getValidationErrorForValue(entry.key, value));
            }
        }
    }
    
    if (!errors.empty()) {
        std::ostringstream oss;
        oss << "Configuration validation failed:\n";
        for (const auto& error : errors) {
            oss << "  " << error << "\n";
        }
        throw std::runtime_error(oss.str());
    }
}

void ConfigManager::addChangeListener(ConfigChangeListener* listener) {
    if (listener) {
        pImpl->listeners.insert(listener);
    }
}

void ConfigManager::removeChangeListener(ConfigChangeListener* listener) {
    if (listener) {
        pImpl->listeners.erase(listener);
    }
}

void ConfigManager::notifyListeners(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) {
    for (auto* listener : pImpl->listeners) {
        try {
            listener->onConfigChanged(key, oldValue, newValue);
        } catch (...) {
        }
    }
}

bool ConfigManager::has(const std::string& key) const {
    return pImpl->config->has(key);
}

std::vector<std::string> ConfigManager::getKeys() const {
    return pImpl->config->getKeys();
}

std::string ConfigManager::getDescription(const std::string& key) const {
    auto it = pImpl->descriptions.find(key);
    return it != pImpl->descriptions.end() ? it->second : "";
}

ConfigSource ConfigManager::getSource(const std::string& key) const {
    auto keys = pImpl->config->getKeys();
    auto it = std::find(keys.begin(), keys.end(), key);
    return it != keys.end() ? ConfigSource::Runtime : ConfigSource::Default;
}

void ConfigManager::remove(const std::string& key) {
    if (pImpl->config->has(key)) {
        ConfigValue oldValue;
        ConfigValue newValue;
        pImpl->config->remove(key);
        notifyListeners(key, oldValue, newValue);
    }
}

void ConfigManager::clear() {
    pImpl->config->clear();
}

void ConfigManager::resetToDefaults() {
    for (const auto& schema : pImpl->schema) {
        set(schema.key, schema.defaultValue, schema.source);
    }
    
    auto keys = getKeys();
    for (const auto& key : keys) {
        if (!hasDefaultValue(key)) {
            remove(key);
        }
    }
}

size_t ConfigManager::getEntryCount() const {
    return pImpl->config->getKeys().size();
}

std::string ConfigManager::generateDocumentation() const {
    std::ostringstream oss;
    oss << "=== NLM Configuration Documentation ===\n\n";
    
    for (const auto& schema : pImpl->schema) {
        oss << "Key: " << schema.key << "\n";
        oss << "  Description: " << schema.description << "\n";
        oss << "  Type: " << config::formatValue(schema.defaultValue) << "\n";
        oss << "  Required: " << (schema.required ? "Yes" : "No") << "\n";
        oss << "  Default Source: " << static_cast<int>(schema.source) << "\n";
        oss << "  Current Value: ";
        
        if (pImpl->config->has(schema.key)) {
            auto value = pImpl->config->get(schema.key);
            oss << config::formatValue(value) << "\n";
            oss << "  Current Source: [Runtime]\n";
        } else {
            oss << "[NOT SET]\n";
        }
        
        oss << "\n";
    }
    
    oss << "=== End Documentation ===\n";
    return oss.str();
}

std::string ConfigManager::generateConfigSample() const {
    std::ostringstream oss;
    oss << "# Sample configuration file for NLM\n";
    oss << "# Copy this to config.cfg and modify values as needed\n\n";
    
    for (const auto& schema : pImpl->schema) {
        oss << "# " << schema.description << "\n";
        oss << schema.key << " = ";
        std::visit([&oss](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (value ? "true" : "false");
            } else {
                oss << value;
            }
        }, schema.defaultValue);
        oss << "\n\n";
    }
    
    return oss.str();
}

bool ConfigManager::validateKey(const std::string& key) const {
    return std::any_of(pImpl->schema.begin(), pImpl->schema.end(),
                      [&key](const ConfigSchema& schema) { return schema.key == key; }) ||
           pImpl->config->has(key);
}

bool ConfigManager::validateValue(const std::string& key, const ConfigValue& value) const {
    if (!validateKey(key)) {
        return false;
    }
    
    auto schemaIt = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
                                [&key](const ConfigSchema& schema) { return schema.key == key; });
    
    if (schemaIt == pImpl->schema.end()) {
        return true;
    }
    
    return isValueValid(key, value);
}

std::vector<std::string> ConfigManager::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& schema : pImpl->schema) {
        if (schema.required && !pImpl->config->has(schema.key)) {
            errors.push_back("Required configuration key missing: " + schema.key);
        } else if (pImpl->config->has(schema.key)) {
            auto value = pImpl->config->get(schema.key);
            if (!isValueValid(schema.key, value)) {
                errors.push_back("Invalid value for key " + schema.key + ": " + 
                               getValidationErrorForValue(schema.key, value));
            }
        }
    }
    
    return errors;
}

void ConfigManager::inheritFrom(const ConfigManager& other) {
    for (const auto& key : other.getKeys()) {
        if (!pImpl->config->has(key)) {
            set(key, other.get(key), ConfigSource::Runtime);
        }
    }
}

bool ConfigManager::equals(const ConfigManager& other) const {
    return pImpl->config->getKeys() == other.pImpl->config->getKeys();
}

void ConfigManager::merge(const ConfigManager& other) {
    for (const auto& key : other.getKeys()) {
        auto value = other.get(key);
        set(key, value, ConfigSource::Runtime);
    }
}

void ConfigManager::saveTemplate(const std::string& name, const std::vector<std::string>& keys) {
    pImpl->templates[name] = keys;
}

void ConfigManager::loadTemplate(const std::string& name) {
    auto it = pImpl->templates.find(name);
    if (it != pImpl->templates.end()) {
        for (const auto& key : it->second) {
            if (hasDefaultValue(key)) {
                set(key, getDefaultValue(key), ConfigSource::Runtime);
            }
        }
    }
}

std::vector<std::string> ConfigManager::getAvailableTemplates() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->templates) {
        names.push_back(pair.first);
    }
    return names;
}

bool ConfigManager::isValueValid(const std::string& key, const ConfigValue& value) const {
    auto schemaIt = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
                                [&key](const ConfigSchema& schema) { return schema.key == key; });
    
    if (schemaIt != pImpl->schema.end()) {
        for (const auto& rule : schemaIt->validationRules) {
            if (!rule.validate(value)) {
                return false;
            }
        }
    }
    
    return true;
}

std::string ConfigManager::getValidationErrorForValue(const std::string& key, const ConfigValue& value) const {
    auto schemaIt = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
                                [&key](const ConfigSchema& schema) { return schema.key == key; });
    
    if (schemaIt == pImpl->schema.end()) {
        return "No validation rules defined for key";
    }
    
    for (const auto& rule : schemaIt->validationRules) {
        if (!rule.validate(value)) {
            return rule.getErrorMessage();
        }
    }
    
    return "Unknown validation error";
}

ConfigValue ConfigManager::getDefaultValue(const std::string& key) const {
    auto it = pImpl->defaultValues.find(key);
    return it != pImpl->defaultValues.end() ? it->second : ConfigValue("");
}

bool ConfigManager::hasDefaultValue(const std::string& key) const {
    return pImpl->defaultValues.find(key) != pImpl->defaultValues.end();
}

} // namespace nlm
