// Implementation of the new error-based Config class
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, size_t> keyToIndex;
    std::vector<ConfigSchema> schema;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default schema if needed
    optimizeMemory();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// New error-based API implementations
std::vector<ConfigError> Config::loadFromFile(const std::string& filepath) {
    clearErrors();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        addError(ConfigFileNotFoundError(filepath));
        return errors_;
    }
    
    // Determine file type based on extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return parseJsonFile(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return parseYamlFile(filepath);
    } else {
        // Try key=value format as fallback
        addError(ConfigParseError("Unsupported file format. Only JSON and YAML are supported.", "", -1, -1));
        return errors_;
    }
}

ConfigError::Type Config::setValidated(const std::string& key, const std::string& value) {
    clearErrors();
    
    // Check if key exists in schema
    auto schemaIt = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
        [&key](const ConfigSchema& s) { return s.key == key; });
    
    if (schemaIt != pImpl->schema.end()) {
        // Validate against schema
        if (schemaIt->required && has(key)) {
            addError(ConfigMissingRequiredKeyError(key));
            return ConfigError::ErrorType::MissingRequiredKey;
        }
        
        // Parse value based on schema
        ConfigValue parsedValue;
        try {
            // Try to parse based on allowed types
            for (const auto& allowedType : schemaIt->allowedTypes) {
                if (isValidType(parsedValue, {allowedType})) {
                    // Set the value
                    set(key, parsedValue);
                    return ConfigError::ErrorType::ValidationError;
                }
            }
            addError(ConfigInvalidTypeError(key, getTypeName(schemaIt->allowedTypes[0]), "string"));
            return ConfigError::ErrorType::InvalidType;
        } catch (...) {
            addError(ConfigParseError("Failed to parse value: " + value, key));
            return ConfigError::ErrorType::ParseError;
        }
    } else {
        // Not in schema, use legacy behavior
        set(key, value);
        return ConfigError::ErrorType::ValidationError;
    }
}

std::vector<ConfigError> Config::validate() const {
    std::vector<ConfigError> validationErrors;
    
    // Check required keys
    for (const auto& schema : pImpl->schema) {
        if (schema.required && !has(schema.key)) {
            validationErrors.emplace_back(
                ConfigMissingRequiredKeyError(schema.key)
            );
        }
    }
    
    // Check dependencies
    for (const auto& schema : pImpl->schema) {
        if (has(schema.key)) {
            for (const auto& dep : schema.dependencies) {
                if (!has(dep)) {
                    validationErrors.emplace_back(
                        ConfigInvalidDependencyError(schema.key, dep)
                    );
                }
            }
        }
    }
    
    // Check value ranges
    for (const auto& entry : pImpl->entries) {
        auto schemaIt = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
            [&entry](const ConfigSchema& s) { return s.key == entry.key; });
        
        if (schemaIt != pImpl->schema.end()) {
            try {
                double numValue;
                if (std::holds_alternative<int>(entry.value)) {
                    numValue = std::get<int>(entry.value);
                } else if (std::holds_alternative<int64_t>(entry.value)) {
                    numValue = static_cast<double>(std::get<int64_t>(entry.value));
                } else if (std::holds_alternative<double>(entry.value)) {
                    numValue = std::get<double>(entry.value);
                } else {
                    continue; // Not a numeric type
                }
                
                if (!isInRange(numValue, schemaIt->range.first, schemaIt->range.second)) {
                    validationErrors.emplace_back(
                        ConfigOutOfRangeError(entry.key, numValue, 
                                            schemaIt->range.first, schemaIt->range.second)
                    );
                }
            } catch (...) {
                validationErrors.emplace_back(
                    ConfigValidationError(entry.key, "Failed to validate numeric range")
                );
            }
        }
    }
    
    return validationErrors;
}

bool Config::hasRequiredKeys() const {
    for (const auto& schema : pImpl->schema) {
        if (schema.required && !has(schema.key)) {
            return false;
        }
    }
    return true;
}

// Backward compatibility API
std::vector<ConfigError> Config::loadFromFileLegacy(const std::string& filepath) {
    return loadFromFile(filepath);
}

bool Config::loadFromArgs(int argc, char** argv) {
    clearErrors();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    
    // Validate after loading
    auto validationErrors = validate();
    errors_.insert(errors_.end(), validationErrors.begin(), validationErrors.end());
    
    return errors_.empty();
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->keyToIndex.find(key);
    if (it == pImpl->keyToIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(pImpl->entries[index].value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        size_t index = it->second;
        if (index < pImpl->entries.size()) {
            pImpl->entries[index].value = value;
            pImpl->entries[index].source = source;
        }
    } else {
        pImpl->entries.emplace_back(key, value, source);
        pImpl->keyToIndex[key] = pImpl->entries.size() - 1;
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return pImpl->keyToIndex.find(key) != pImpl->keyToIndex.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        size_t index = it->second;
        
        // Remove from entries
        pImpl->entries.erase(pImpl->entries.begin() + index);
        
        // Update keyToIndex for remaining entries
        pImpl->keyToIndex.clear();
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            pImpl->keyToIndex[pImpl->entries[i].key] = i;
        }
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->keyToIndex.clear();
    pImpl->schema.clear();
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

void Config::optimizeMemory() {
    pImpl->entries.shrink_to_fit();
    pImpl->keyToIndex.reserve(pImpl->entries.size());
}

// Helper implementations
std::string Config::getTypeName(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "std::vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "std::vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "std::vector<string>";
    return "unknown";
}

bool Config::isInRange(double value, double min, double max) {
    return value >= min && value <= max;
}

bool Config::isValidType(const ConfigValue& value, const std::vector<ConfigValue>& allowedTypes) {
    for (const auto& allowed : allowedTypes) {
        if (value.index() == allowed.index()) {
            return true;
        }
    }
    return false;
}

void Config::addError(const ConfigError& error) {
    errors_.push_back(error);
}

} // namespace nlm