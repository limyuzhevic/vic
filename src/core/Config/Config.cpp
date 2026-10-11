// Minimal implementation of Config.cpp for testing
#include "Config.hpp"
#include "Error.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <any>
#include <mutex>
#include <map>
#include <unordered_set>
#include <ctime>
#include <stdexcept>
#include <system_error>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigChangeCallback> changeCallbacks;
    mutable std::mutex mutex;
};

struct Config::DefaultConfigurationBuilder::Impl {
    std::vector<ConfigEntry> configEntries;
    std::vector<std::function<ValidationResult(const ConfigValue&)>> validators;
    std::vector<std::pair<std::string, std::string>> dependencies;
    std::string currentSection;
    std::shared_ptr<Configuration> config;
};

struct Config::DefaultConfigurationValidator::Impl {
    std::vector<std::function<ValidationResult(const ConfigValue&)>> rules;
    std::vector<std::tuple<std::string, std::string, std::function<ValidationResult(const ConfigValue&, const ConfigValue&)>>> crossValidations;
    std::shared_ptr<ConfigurationValidator> validator;
};

// Implementation of Configuration interface
Config::DefaultConfiguration::DefaultConfiguration() : pImpl(std::make_unique<Impl>()) {}

Config::DefaultConfiguration::~DefaultConfiguration() = default;

// Core file loading with error handling
bool Config::DefaultConfiguration::loadFromFile(const std::string& filepath) {
    try {
        InputValidator::isNonEmpty("filepath", filepath);
        
        std::filesystem::path path(filepath);
        if (!std::filesystem::exists(path)) {
            throw ConfigError("Configuration file not found", "loadFromFile: " + filepath, 
                           ConfigError::Code::FileNotFound);
        }
        
        if (!std::filesystem::is_regular_file(path)) {
            throw ConfigError("Path is not a file", "loadFromFile: " + filepath,
                           ConfigError::Code::ParseError);
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigError("Failed to open file", "loadFromFile: " + filepath,
                           ConfigError::Code::FileNotFound);
        }
        
        // Determine file format
        std::string extension = path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        if (extension == ".json") {
            return loadFromJSON(std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()));
        } else if (extension == ".yaml" || extension == ".yml") {
            return loadFromYAML(std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()));
        } else {
            // Try simple format for backward compatibility
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();
            auto entries = ConfigUtils::parseSimpleFormat(content);
            
            clear();
            for (const auto& pair : entries) {
                auto valueOpt = ConfigUtils::stringToConfigValue(pair.second);
                if (valueOpt) {
                    set(pair.first, *valueOpt, ConfigSource::File);
                }
            }
            return true;
        }
    } catch (const ConfigError& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigError(std::string("File loading failed: ") + e.what(), "loadFromFile: " + filepath,
                       ConfigError::Code::FileNotFound);
    }
}

bool Config::DefaultConfiguration::loadFromJSON(const std::string& jsonStr) {
    try {
        InputValidator::isNonEmpty("jsonStr", jsonStr);
        
        // Validate JSON format
        if (jsonStr.empty() || jsonStr.find('{') == std::string::npos) {
            throw ConfigError("Invalid JSON format", "loadFromJSON",
                           ConfigError::Code::ParseError);
        }
        
        // Parse JSON (placeholder - would need a JSON library)
        // For now, use simple key=value format
        auto entries = ConfigUtils::parseSimpleFormat(jsonStr);
        
        clear();
        for (const auto& pair : entries) {
            auto valueOpt = ConfigUtils::stringToConfigValue(pair.second);
            if (valueOpt) {
                set(pair.first, *valueOpt, ConfigSource::File);
            }
        }
        return true;
    } catch (const ConfigError& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigError(std::string("JSON parsing failed: ") + e.what(), "loadFromJSON",
                       ConfigError::Code::ParseError);
    }
}

bool Config::DefaultConfiguration::loadFromYAML(const std::string& yamlStr) {
    try {
        InputValidator::isNonEmpty("yamlStr", yamlStr);
        
        // Basic YAML validation (placeholder)
        if (yamlStr.empty()) {
            throw ConfigError("Empty YAML string", "loadFromYAML",
                           ConfigError::Code::ParseError);
        }
        
        // Parse YAML (placeholder - would need a YAML library)
        auto entries = ConfigUtils::parseSimpleFormat(yamlStr);
        
        clear();
        for (const auto& pair : entries) {
            auto valueOpt = ConfigUtils::stringToConfigValue(pair.second);
            if (valueOpt) {
                set(pair.first, *valueOpt, ConfigSource::File);
            }
        }
        return true;
    } catch (const ConfigError& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigError(std::string("YAML parsing failed: ") + e.what(), "loadFromYAML",
                       ConfigError::Code::ParseError);
    }
}

bool Config::DefaultConfiguration::saveToJSON(std::string& output) const {
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Generate JSON representation
        output = "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            output += "  \"" + entry.key + "\": " + ConfigUtils::configValueToString(entry.value);
            if (i < pImpl->entries.size() - 1) {
                output += ",\n";
            } else {
                output += "\n";
            }
        }
        output += "}";
        
        return true;
    } catch (const std::exception& e) {
        throw ConfigError(std::string("JSON serialization failed: ") + e.what(), "saveToJSON",
                       ConfigError::Code::ParseError);
    }
}

bool Config::DefaultConfiguration::saveToYAML(std::string& output) const {
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Generate YAML representation
        output = "# Configuration file\n";
        for (const auto& entry : pImpl->entries) {
            output += entry.key + ": " + ConfigUtils::configValueToString(entry.value) + "\n";
        }
        
        return true;
    } catch (const std::exception& e) {
        throw ConfigError(std::string("YAML serialization failed: ") + e.what(), "saveToYAML",
                       ConfigError::Code::ParseError);
    }
}

ValidationResult Config::DefaultConfiguration::validate() const {
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        ValidationResult result = validateInternal();
        
        // Check for cross-validation rules
        for (const auto& kv : pImpl->entries) {
            const auto& key = kv.key;
            const auto& value = kv.value;
            
            // Check validators
            for (const auto& validator : pImpl->validators) {
                if (validator(key, value) == ValidationResult{false}) {
                    result.valid = false;
                    result.errorMessage = "Validation failed for key: " + key;
                    break;
                }
            }
        }
        
        return result;
    } catch (const std::exception& e) {
        return ValidationResult(false, std::string("Validation error: ") + e.what(), "validate");
    }
}

// Config error handling methods
void Config::DefaultConfiguration::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        InputValidator::isNonEmpty("key", key);
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        ConfigValue oldValue;
        bool existed = it != pImpl->entries.end();
        
        if (existed) {
            oldValue = it->value;
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
            oldValue = value;
        }
        
        // Notify change
        notifyChange(ConfigChange(key, oldValue, value, source));
    } catch (const ConfigError& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigError(std::string("Failed to set configuration: ") + e.what(),
                       "set: " + key, ConfigError::Code::ValidationFailed);
    }
}

std::optional<ConfigValue> Config::DefaultConfiguration::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        InputValidator::isNonEmpty("key", key);
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            return std::nullopt;
        }
        
        return it->value;
    } catch (const ConfigError& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigError(std::string("Failed to get configuration: ") + e.what(),
                       "get: " + key, ConfigError::Code::ValidationFailed);
    }
}

bool Config::DefaultConfiguration::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        InputValidator::isNonEmpty("key", key);
        
        return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    } catch (const ConfigError& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigError(std::string("Failed to check configuration: ") + e.what(),
                       "has: " + key, ConfigError::Code::ValidationFailed);
    }
}

void Config::DefaultConfiguration::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        InputValidator::isNonEmpty("key", key);
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            ConfigValue oldValue = it->value;
            pImpl->entries.erase(it);
            notifyChange(ConfigChange(key, oldValue, ConfigValue(), ConfigSource::Runtime));
        }
    } catch (const ConfigError& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigError(std::string("Failed to remove configuration: ") + e.what(),
                       "remove: " + key, ConfigError::Code::ValidationFailed);
    }
}

std::vector<std::string> Config::DefaultConfiguration::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    
    try {
        for (const auto& entry : pImpl->entries) {
            keys.push_back(entry.key);
        }
        return keys;
    } catch (const std::exception& e) {
        throw ConfigError(std::string("Failed to get configuration keys: ") + e.what(),
                       "getKeys", ConfigError::Code::ValidationFailed);
    }
}

void Config::DefaultConfiguration::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        for (const auto& entry : pImpl->entries) {
            notifyChange(ConfigChange(entry.key, entry.value, ConfigValue(), ConfigSource::Runtime));
        }
        
        pImpl->entries.clear();
    } catch (const ConfigError& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigError(std::string("Failed to clear configuration: ") + e.what(),
                       "clear", ConfigError::Code::ValidationFailed);
    }
}

bool Config::DefaultConfiguration::loadFromJSON(const std::string& jsonStr) {
    return false; // TODO: Implement JSON parsing
}

bool Config::DefaultConfiguration::loadFromYAML(const std::string& yamlStr) {
    return false; // TODO: Implement YAML parsing
}

bool Config::DefaultConfiguration::loadFromFile(const std::string& filepath) {
    return false; // TODO: Implement file loading
}

bool Config::DefaultConfiguration::saveToJSON(std::string& output) const {
    return false; // TODO: Implement JSON serialization
}

bool Config::DefaultConfiguration::saveToYAML(std::string& output) const {
    return false; // TODO: Implement YAML serialization
}

ValidationResult Config::DefaultConfiguration::validate() const {
    return validateInternal();
}

std::string Config::DefaultConfiguration::getSchema() const {
    return "{}";
}

void Config::DefaultConfiguration::addChangeCallback(const ConfigChangeCallback& callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->changeCallbacks.push_back(callback);
}

void Config::DefaultConfiguration::removeChangeCallback(const ConfigChangeCallback& callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find(pImpl->changeCallbacks.begin(), pImpl->changeCallbacks.end(), callback);
    if (it != pImpl->changeCallbacks.end()) {
        pImpl->changeCallbacks.erase(it);
    }
}

size_t Config::DefaultConfiguration::getEntryCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->entries.size();
}

std::string Config::DefaultConfiguration::summary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, float>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<float>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::shared_ptr<Configuration> Config::DefaultConfiguration::getBuilder() {
    return std::make_shared<DefaultConfigurationBuilder>();
}

std::shared_ptr<ConfigurationValidator> Config::DefaultConfiguration::getValidator() {
    return std::make_shared<DefaultConfigurationValidator>();
}

void Config::DefaultConfiguration::initializeDefaults(const std::map<std::string, ConfigValue>& defaults) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : defaults) {
        bool exists = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
            [&pair](const ConfigEntry& e) { return e.key == pair.first; });
        
        if (!exists) {
            pImpl->entries.emplace_back(pair.first, pair.second, ConfigSource::Default);
        }
    }
}

bool Config::DefaultConfiguration::loadFromArgs(int argc, char** argv) {
    return false;
}

void Config::DefaultConfiguration::notifyChange(const ConfigChange& change) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& callback : pImpl->changeCallbacks) {
        callback(change);
    }
}

ValidationResult Config::DefaultConfiguration::validateInternal() const {
    return ValidationResult(true);
}

ConfigValue Config::DefaultConfiguration::convertToConfigValue(const std::any& value) const {
    return ConfigValue();
}

std::any Config::DefaultConfiguration::convertFromConfigValue(const ConfigValue& value) const {
    return std::any();
}

// DefaultConfigurationBuilder implementation
Config::DefaultConfigurationBuilder::DefaultConfigurationBuilder() : pImpl(std::make_unique<Impl>()) {}

Config::DefaultConfigurationBuilder::~DefaultConfigurationBuilder() = default;

ConfigurationBuilder& Config::DefaultConfigurationBuilder::addInt(const std::string& key, int64_t defaultValue, const std::string& description) {
    pImpl->configEntries.emplace_back(key, defaultValue, ConfigSource::Builder, description);
    return *this;
}

ConfigurationBuilder& Config::DefaultConfigurationBuilder::addDouble(const std::string& key, double defaultValue, const std::string& description) {
    pImpl->configEntries.emplace_back(key, defaultValue, ConfigSource::Builder, description);
    return *this;
}

ConfigurationBuilder& Config::DefaultConfigurationBuilder::addFloat(const std::string& key, float defaultValue, const std::string& description) {
    pImpl->configEntries.emplace_back(key, defaultValue, ConfigSource::Builder, description);
    return *this;
}

ConfigurationBuilder& Config::DefaultConfigurationBuilder::addBool(const std::string& key, bool defaultValue, const std::string& description) {
    pImpl->configEntries.emplace_back(key, defaultValue, ConfigSource::Builder, description);
    return *this;
}

ConfigurationBuilder& Config::DefaultConfigurationBuilder::addString(const std::string& key, const std::string& defaultValue, const std::string& description) {
    pImpl->configEntries.emplace_back(key, defaultValue, ConfigSource::Builder, description);
    return *this;
}

ConfigurationBuilder& Config::DefaultConfigurationBuilder::addIntList(const std::string& key, const std::vector<int64_t>& defaultValue, const std::string& description) {
    pImpl->configEntries.emplace_back(key, defaultValue, ConfigSource::Builder, description);
    return *this;
}

ConfigurationBuilder& Config::DefaultConfigurationBuilder::addDoubleList(const std::string& key, const std::vector<double>& defaultValue, const std::string& description) {
    pImpl->configEntries.emplace_back(key, defaultValue, ConfigSource::Builder, description);
    return *this;
}

ConfigurationBuilder& Config::DefaultConfigurationBuilder::addStringList(const std::string& key, const std::vector<std::string>& defaultValue, const std::string& description) {
    pImpl->configEntries.emplace_back(key, defaultValue, ConfigSource::Builder, description);
    return *this;
}

ConfigurationBuilder& Config::DefaultConfigurationBuilder::addValidator(const std::string& key, std::function<ValidationResult(const ConfigValue&)> validator) {
    pImpl->validators.push_back(validator);
    return *this;
}

ConfigurationBuilder& Config::DefaultConfigurationBuilder::addDependency(const std::string& key, const std::string& dependsOn) {
    pImpl->dependencies.emplace_back(key, dependsOn);
    return *this;
}

std::shared_ptr<Configuration> Config::DefaultConfigurationBuilder::build() {
    pImpl->config = std::make_shared<DefaultConfiguration>();
    
    for (const auto& entry : pImpl->configEntries) {
        pImpl->config->set(entry.key, entry.value, entry.source);
    }
    
    return pImpl->config;
}

// DefaultConfigurationValidator implementation
Config::DefaultConfigurationValidator::DefaultConfigurationValidator() : pImpl(std::make_unique<Impl>()) {}

Config::DefaultConfigurationValidator::~DefaultConfigurationValidator() = default;

ValidationResult Config::DefaultConfigurationValidator::validate(const Configuration& config) const {
    return ValidationResult(true);
}

std::string Config::DefaultConfigurationValidator::getValidationSchema() const {
    return "{}";
}

ConfigurationValidator& Config::DefaultConfigurationValidator::addRule(const std::string& key, std::function<ValidationResult(const ConfigValue&)> rule) {
    pImpl->rules.push_back(rule);
    return *this;
}

ConfigurationValidator& Config::DefaultConfigurationValidator::addCrossValidation(const std::string& key1, const std::string& key2, std::function<ValidationResult(const ConfigValue&, const ConfigValue&)> rule) {
    pImpl->crossValidations.push_back(std::make_tuple(key1, key2, rule));
    return *this;
}

// Factory functions
std::shared_ptr<Configuration> createConfiguration() {
    return std::make_shared<Config::DefaultConfiguration>();
}

std::shared_ptr<ConfigurationBuilder> createConfigurationBuilder() {
    return std::make_shared<Config::DefaultConfigurationBuilder>();
}

std::shared_ptr<ConfigurationValidator> createConfigurationValidator() {
    return std::make_shared<Config::DefaultConfigurationValidator>();
}

} // namespace nlm

// Template implementations for type-safe accessors
template<typename T>
std::optional<T> Config::DefaultConfiguration::getTyped(const std::string& key) const {
    auto value = get(key);
    if (!value) {
        return std::nullopt;
    }
    
    try {
        if constexpr (std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<int64_t>(*value)) {
                return std::get<int64_t>(*value);
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(*value)) {
                return std::get<double>(*value);
            }
        } else if constexpr (std::is_same_v<T, float>) {
            if (std::holds_alternative<float>(*value)) {
                return std::get<float>(*value);
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(*value)) {
                return std::get<bool>(*value);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(*value)) {
                return std::get<std::string>(*value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            if (std::holds_alternative<std::vector<int64_t>>(*value)) {
                return std::get<std::vector<int64_t>>(*value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (std::holds_alternative<std::vector<double>>(*value)) {
                return std::get<std::vector<double>>(*value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<float>>) {
            if (std::holds_alternative<std::vector<float>>(*value)) {
                return std::get<std::vector<float>>(*value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (std::holds_alternative<std::vector<std::string>>(*value)) {
                return std::get<std::vector<std::string>>(*value);
            }
        }
    } catch (const std::bad_variant_access&) {}
    
    return std::nullopt;
}

template<typename T>
T Config::DefaultConfiguration::getOr(const std::string& key, const T& defaultValue) const {
    auto value = getTyped<T>(key);
    return value.has_value() ? value.value() : defaultValue;
}

// Explicit template instantiations
namespace nlm {
    template std::optional<int64_t> Config::DefaultConfiguration::getTyped<int64_t>(const std::string&) const;
    template std::optional<double> Config::DefaultConfiguration::getTyped<double>(const std::string&) const;
    template std::optional<float> Config::DefaultConfiguration::getTyped<float>(const std::string&) const;
    template std::optional<bool> Config::DefaultConfiguration::getTyped<bool>(const std::string&) const;
    template std::optional<std::string> Config::DefaultConfiguration::getTyped<std::string>(const std::string&) const;
    template std::optional<std::vector<int64_t>> Config::DefaultConfiguration::getTyped<std::vector<int64_t>>(const std::string&) const;
    template std::optional<std::vector<double>> Config::DefaultConfiguration::getTyped<std::vector<double>>(const std::string&) const;
    template std::optional<std::vector<float>> Config::DefaultConfiguration::getTyped<std::vector<float>>(const std::string&) const;
    template std::optional<std::vector<std::string>> Config::DefaultConfiguration::getTyped<std::vector<std::string>>(const std::string&) const;
    
    template int64_t Config::DefaultConfiguration::getOr<int64_t>(const std::string&, const int64_t&) const;
    template double Config::DefaultConfiguration::getOr<double>(const std::string&, const double&) const;
    template float Config::DefaultConfiguration::getOr<float>(const std::string&, const float&) const;
    template bool Config::DefaultConfiguration::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::DefaultConfiguration::getOr<std::string>(const std::string&, const std::string&) const;
    template std::vector<int64_t> Config::DefaultConfiguration::getOr<std::vector<int64_t>>(const std::string&, const std::vector<int64_t>&) const;
    template std::vector<double> Config::DefaultConfiguration::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
    template std::vector<float> Config::DefaultConfiguration::getOr<std::vector<float>>(const std::string&, const std::vector<float>&) const;
    template std::vector<std::string> Config::DefaultConfiguration::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

    // Original Config template implementations for backward compatibility
    template std::optional<int64_t> Config::get(const std::string&) const;
    template std::optional<double> Config::get(const std::string&) const;
    template std::optional<float> Config::get(const std::string&) const;
    template std::optional<bool> Config::get(const std::string&) const;
    template std::optional<std::string> Config::get(const std::string&) const;
    
    template int64_t Config::getOr(const std::string&, const int64_t&) const;
    template double Config::getOr(const std::string&, const double&) const;
    template float Config::getOr(const std::string&, const float&) const;
    template bool Config::getOr(const std::string&, const bool&) const;
    template std::string Config::getOr(const std::string&, const std::string&) const;
}
