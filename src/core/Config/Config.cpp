#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <set>
#include <unordered_map>
#include <limits>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

struct Config::SchemaImpl {
    std::vector<ConfigSchema> schemas;
    std::unordered_map<std::string, size_t> keyToIndex;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schemaImpl(std::make_unique<SchemaImpl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Configuration validation helper methods
bool Config::isValid(const std::string& key) const {
    if (!has(key)) {
        return !isKeyRequired(key);
    }
    
    // Check schema validation if schema exists
    auto it = schemaImpl->keyToIndex.find(key);
    if (it != schemaImpl->keyToIndex.end()) {
        const ConfigSchema& schema = schemaImpl->schemas[it->second];
        return schema.validator ? schema.validator(get<ConfigValue>(key).value()) : true;
    }
    
    return true; // If no schema, assume valid
}

std::vector<std::string> Config::getInvalidKeys() const {
    std::vector<std::string> invalidKeys;
    for (const auto& entry : pImpl->entries) {
        if (!isValid(entry.key)) {
            invalidKeys.push_back(entry.key);
        }
    }
    return invalidKeys;
}

bool Config::validate(const std::string& key) const {
    return isValid(key);
}

std::optional<double> Config::toDouble(const std::string& key) const {
    if (!has(key)) return std::nullopt;
    
    const ConfigValue& value = get<ConfigValue>(key).value();
    return std::visit([](const auto& v) -> std::optional<double> {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_arithmetic_v<T>) {
            return static_cast<double>(v);
        } else {
            return std::nullopt;
        }
    }, value);
}

std::optional<int> Config::toInt(const std::string& key) const {
    if (!has(key)) return std::nullopt;
    
    const ConfigValue& value = get<ConfigValue>(key).value();
    return std::visit([](const auto& v) -> std::optional<int> {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int>) {
            return v;
        } else if constexpr (std::is_same_v<T, double>) {
            return static_cast<int>(v);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return static_cast<int>(v);
        } else {
            return std::nullopt;
        }
    }, value);
}

std::optional<bool> Config::toBool(const std::string& key) const {
    if (!has(key)) return std::nullopt;
    
    const ConfigValue& value = get<ConfigValue>(key).value();
    return std::visit([](const auto& v) -> std::optional<bool> {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, bool>) {
            return v;
        } else if constexpr (std::is_same_v<T, std::string>) {
            // Parse string to bool
            if (v == "true" || v == "1" || v == "yes") return true;
            if (v == "false" || v == "0" || v == "no") return false;
            return std::nullopt;
        } else if constexpr (std::is_arithmetic_v<T>) {
            return v != 0;
        } else {
            return std::nullopt;
        }
    }, value);
}

std::optional<std::string> Config::toString(const std::string& key) const {
    if (!has(key)) return std::nullopt;
    
    const ConfigValue& value = get<ConfigValue>(key).value();
    return std::visit([](const auto& v) -> std::optional<std::string> {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<T>) {
            return std::to_string(v);
        } else {
            return std::nullopt;
        }
    }, value);
}

bool Config::isKeyRequired(const std::string& key) const {
    auto it = schemaImpl->keyToIndex.find(key);
    if (it != schemaImpl->keyToIndex.end()) {
        return schemaImpl->schemas[it->second].required;
    }
    return false;
}

ConfigValue Config::normalize(const std::string& key, const ConfigValue& value) const {
    auto it = schemaImpl->keyToIndex.find(key);
    if (it != schemaImpl->keyToIndex.end()) {
        const ConfigSchema& schema = schemaImpl->schemas[it->second];
        if (schema.normalizer) {
            return schema.normalizer(value);
        }
    }
    return value;
}

bool Config::validateRange(const ConfigValue& value, double min, double max) {
    return std::visit([min, max](const auto& v) -> bool {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_arithmetic_v<T>) {
            double d = static_cast<double>(v);
            return d >= min && d <= max;
        } else {
            return true; // Non-numeric types pass range validation
        }
    }, value);
}

bool Config::validateChoices(const ConfigValue& value, const std::vector<std::string>& choices) {
    return std::visit([choices](const auto& v) -> bool {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return std::find(choices.begin(), choices.end(), v) != choices.end();
        } else {
            return true; // Non-string types pass choice validation
        }
    }, value);
}

bool Config::isNumericType(const ConfigValue& value) {
    return std::visit([](const auto& v) -> bool {
        using T = std::decay_t<decltype(v)>;
        return std::is_arithmetic_v<T>;
    }, value);
}

bool Config::isStringType(const ConfigValue& value) {
    return std::visit([](const auto& v) -> bool {
        using T = std::decay_t<decltype(v)>;
        return std::is_same_v<T, std::string>;
    }, value);
}

bool Config::isVectorType(const ConfigValue& value) {
    return std::visit([](const auto& v) -> bool {
        using T = std::decay_t<decltype(v)>;
        return std::is_same_v<T, std::vector<int>> ||
               std::is_same_v<T, std::vector<double>> ||
               std::is_same_v<T, std::vector<std::string>>;
    }, value);
}

bool Config::parseValue(const std::string& str, ConfigValue& outValue) {
    // Trim whitespace
    std::string trimmed = trim(str);
    
    if (trimmed.empty()) {
        outValue = std::string();
        return true;
    }
    
    // Check for boolean values
    if (trimmed == "true" || trimmed == "false") {
        outValue = (trimmed == "true");
        return true;
    }
    
    // Check for numeric values
    try {
        // Try integer
        size_t pos;
        int64_t intVal = std::stoll(trimmed, &pos);
        if (pos == trimmed.length()) {
            outValue = intVal;
            return true;
        }
        
        // Try double
        double doubleVal = std::stod(trimmed, &pos);
        if (pos == trimmed.length()) {
            outValue = doubleVal;
            return true;
        }
    } catch (...) {
        // Fall through to string parsing
    }
    
    // Check for vector values (simplified: [1,2,3])
    if (trimmed.front() == '[' && trimmed.back() == ']') {
        std::string content = trimmed.substr(1, trimmed.length() - 2);
        std::stringstream ss(content);
        std::string item;
        std::vector<int> intVector;
        bool isVector = true;
        
        while (std::getline(ss, item, ',')) {
            item = trim(item);
            try {
                intVector.push_back(std::stoi(item));
            } catch (...) {
                isVector = false;
                break;
            }
        }
        
        if (isVector && !intVector.empty()) {
            outValue = intVector;
            return true;
        }
    }
    
    // Default to string
    outValue = trimmed;
    return true;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try to load configuration file with error handling
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    size_t errors = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
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
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_ERROR("Configuration error at line " + std::to_string(lineNumber) + ": empty key");
                ++errors;
                continue;
            }
            
            // Parse value
            ConfigValue parsedValue;
            if (parseValue(value, parsedValue)) {
                // Validate against schema if exists
                auto it = schemaImpl->keyToIndex.find(key);
                if (it != schemaImpl->keyToIndex.end()) {
                    const ConfigSchema& schema = schemaImpl->schemas[it->second];
                    if (schema.validator && !schema.validator(parsedValue)) {
                        NLM_LOG_ERROR("Configuration validation error at line " + std::to_string(lineNumber) + ": " + key + " value invalid");
                        ++errors;
                        continue;
                    }
                }
                
                // Normalize value if schema exists
                ConfigValue normalizedValue = normalize(key, parsedValue);
                
                // Set the value
                set(key, normalizedValue, ConfigSource::File);
            } else {
                NLM_LOG_ERROR("Configuration error at line " + std::to_string(lineNumber) + ": cannot parse value '" + value + "'");
                ++errors;
            }
        } else {
            NLM_LOG_WARNING("Configuration warning at line " + std::to_string(lineNumber) + ": invalid format (missing '=')");
            ++errors;
        }
    }
    
    if (errors > 0) {
        NLM_LOG_WARNING("Configuration file parsing completed with " + std::to_string(errors) + " warnings/errors");
    }
    
    return true; // Still return true even with warnings, as we loaded what we could
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!argv || argc <= 0) {
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key
                if (key.empty()) {
                    NLM_LOG_ERROR("Invalid command line argument: " + arg);
                    return false;
                }
                
                // Parse value
                ConfigValue parsedValue;
                if (parseValue(value, parsedValue)) {
                    // Validate against schema if exists
                    auto it = schemaImpl->keyToIndex.find(key);
                    if (it != schemaImpl->keyToIndex.end()) {
                        const ConfigSchema& schema = schemaImpl->schemas[it->second];
                        if (schema.validator && !schema.validator(parsedValue)) {
                            NLM_LOG_ERROR("Configuration validation error: " + key + " value invalid");
                            return false;
                        }
                    }
                    
                    // Normalize value if schema exists
                    ConfigValue normalizedValue = normalize(key, parsedValue);
                    set(key, normalizedValue, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_ERROR("Invalid command line argument value: " + value);
                    return false;
                }
            } else {
                NLM_LOG_ERROR("Invalid command line argument: " + arg);
                return false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (key.empty()) {
                NLM_LOG_ERROR("Invalid command line argument: " + arg);
                return false;
            }
            
            ConfigValue parsedValue;
            if (parseValue(value, parsedValue)) {
                auto it = schemaImpl->keyToIndex.find(key);
                if (it != schemaImpl->keyToIndex.end()) {
                    const ConfigSchema& schema = schemaImpl->schemas[it->second];
                    if (schema.validator && !schema.validator(parsedValue)) {
                        NLM_LOG_ERROR("Configuration validation error: " + key + " value invalid");
                        return false;
                    }
                }
                
                ConfigValue normalizedValue = normalize(key, parsedValue);
                set(key, normalizedValue, ConfigSource::CommandLine);
            } else {
                NLM_LOG_ERROR("Invalid command line argument value: " + value);
                return false;
            }
        }
        else {
            NLM_LOG_ERROR("Invalid command line argument: " + arg);
            return false;
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n\n";
    
    // Write configuration entries
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Write the actual value based on its type
        std::visit([&file](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, bool>) {
                file << (v ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << '"' << v << '"';
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << v[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << v[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << '"' << v[i] << '"';
                }
                file << "]";
            } else {
                // For numeric types (int, int64_t, double, float)
                file << v;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    // Validate against schema if exists
    auto it = schemaImpl->keyToIndex.find(key);
    if (it != schemaImpl->keyToIndex.end()) {
        const ConfigSchema& schema = schemaImpl->schemas[it->second];
        if (schema.validator && !schema.validator(value)) {
            throw std::invalid_argument("Configuration validation failed for key: " + key);
        }
        
        // Apply normalizer if exists
        ConfigValue normalizedValue = schema.normalizer ? schema.normalizer(value) : value;
        
        // Check range validation
        if (schema.minValue != -1e9 || schema.maxValue != 1e9) {
            if (!validateRange(normalizedValue, schema.minValue, schema.maxValue)) {
                throw std::invalid_argument("Configuration value for key " + key + " is out of range [" + 
                                           std::to_string(schema.minValue) + ", " + std::to_string(schema.maxValue) + "]");
            }
        }
        
        // Check choice validation
        if (!schema.allowedValues.empty() && !validateChoices(normalizedValue, schema.allowedValues)) {
            throw std::invalid_argument("Configuration value for key " + key + " is not in allowed choices");
        }
        
        // Update with normalized value
        auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (entryIt != pImpl->entries.end()) {
            entryIt->value = normalizedValue;
            entryIt->source = source;
        } else {
            pImpl->entries.emplace_back(key, normalizedValue, source);
        }
    } else {
        // No schema, allow any value
        auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (entryIt != pImpl->entries.end()) {
            entryIt->value = value;
            entryIt->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    // Also remove from schema if exists
    auto it = schemaImpl->keyToIndex.find(key);
    if (it != schemaImpl->keyToIndex.end()) {
        size_t index = it->second;
        schemaImpl->schemas.erase(schemaImpl->schemas.begin() + index);
        
        // Update keyToIndex
        schemaImpl->keyToIndex.clear();
        for (size_t i = 0; i < schemaImpl->schemas.size(); ++i) {
            schemaImpl->keyToIndex[schemaImpl->schemas[i].key] = i;
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
}

bool Config::validateSchema() const {
    for (const auto& schema : schemaImpl->schemas) {
        if (schema.required && !has(schema.key)) {
            NLM_LOG_ERROR("Schema validation failed: required key missing: " + schema.key);
            return false;
        }
        
        if (has(schema.key)) {
            ConfigValue value = get<ConfigValue>(schema.key).value();
            if (schema.validator && !schema.validator(value)) {
                NLM_LOG_ERROR("Schema validation failed for key " + schema.key + ": value invalid");
                return false;
            }
            
            if (!validateRange(value, schema.minValue, schema.maxValue)) {
                NLM_LOG_ERROR("Schema validation failed for key " + schema.key + ": value out of range");
                return false;
            }
            
            if (!schema.allowedValues.empty() && !validateChoices(value, schema.allowedValues)) {
                NLM_LOG_ERROR("Schema validation failed for key " + schema.key + ": value not in allowed choices");
                return false;
            }
        }
    }
    
    return true;
}

bool Config::addSchema(const ConfigSchema& schema) {
    // Validate schema
    if (schema.key.empty()) {
        NLM_LOG_ERROR("Schema validation failed: key cannot be empty");
        return false;
    }
    
    if (schema.allowedValues.empty() && schema.required) {
        NLM_LOG_ERROR("Schema validation failed: required key must have allowed values");
        return false;
    }
    
    // Check if schema key already exists
    if (schemaImpl->keyToIndex.find(schema.key) != schemaImpl->keyToIndex.end()) {
        NLM_LOG_ERROR("Schema validation failed: duplicate schema key: " + schema.key);
        return false;
    }
    
    // Add schema
    schemaImpl->schemas.push_back(schema);
    schemaImpl->keyToIndex[schema.key] = schemaImpl->schemas.size() - 1;
    
    NLM_LOG_INFO("Schema added for key: " + schema.key);
    return true;
}

bool Config::removeSchema(const std::string& key) {
    auto it = schemaImpl->keyToIndex.find(key);
    if (it == schemaImpl->keyToIndex.end()) {
        NLM_LOG_WARNING("Schema removal skipped: key not found in schema: " + key);
        return false;
    }
    
    size_t index = it->second;
    schemaImpl->schemas.erase(schemaImpl->schemas.begin() + index);
    
    // Update keyToIndex
    schemaImpl->keyToIndex.clear();
    for (size_t i = 0; i < schemaImpl->schemas.size(); ++i) {
        schemaImpl->keyToIndex[schemaImpl->schemas[i].key] = i;
    }
    
    NLM_LOG_INFO("Schema removed for key: " + key);
    return true;
}

std::vector<ConfigSchema> Config::getSchema() const {
    return schemaImpl->schemas;
}

void Config::clearSchema() {
    schemaImpl->schemas.clear();
    schemaImpl->keyToIndex.clear();
    NLM_LOG_INFO("All schemas cleared");
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << v << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i) oss << ", ";
                    oss << v[i];
                }
                oss << "]";
            } else {
                oss << v;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    // Show schema validation status
    std::vector<std::string> invalidKeys = getInvalidKeys();
    if (!invalidKeys.empty()) {
        oss << "\nValidation Errors:";
        for (const auto& key : invalidKeys) {
            oss << "\n  - " << key << " (invalid)";
        }
    }
    
    return oss.str();
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm