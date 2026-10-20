// Config implementation with enhanced error handling

#include "Config.hpp"
#include "Errors.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::unordered_map<std::string, ConfigEntry> entries;
    std::vector<std::string> keys;
    
    // Schema for validation
    struct SchemaEntry {
        std::string name;
        std::string type;  // "int", "double", "string", "bool", "vector"
        bool required = false;
        std::variant<int, double, std::string> default_value;
        std::vector<std::string> allowed_values;
        std::pair<double, double> range;  // min, max for numeric types
    };
    
    std::vector<SchemaEntry> schema;
    
    Impl() = default;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default schema
    initializeDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigLoadException(filepath, "File not found or cannot be opened");
        }
        
        json j;
        file >> j;
        
        loadFromJson(j, ConfigSource::File);
    } catch (const ConfigLoadException& e) {
        throw; // Re-throw config-specific exceptions
    } catch (const std::exception& e) {
        throw ConfigLoadException(filepath, std::string("Invalid JSON format: ") + e.what());
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        size_t equals_pos = arg.find('=');
        if (equals_pos != std::string::npos && equals_pos > 0) {
            std::string key = arg.substr(0, equals_pos);
            std::string value = arg.substr(equals_pos + 1);
            
            // Try to determine type based on value content
            try {
                // Try bool
                if (value == "true" || value == "false") {
                    set(key, value == "true", ConfigSource::CommandLine);
                }
                // Try int
                else if (std::all_of(value.begin(), value.end(), ::isdigit) || 
                        (value[0] == '-' && std::all_of(value.begin() + 1, value.end(), ::isdigit))) {
                    set(key, std::stoi(value), ConfigSource::CommandLine);
                }
                // Try double
                else {
                    size_t dec_pos = value.find('.');
                    if (dec_pos != std::string::npos && dec_pos < value.size() - 1) {
                        set(key, std::stod(value), ConfigSource::CommandLine);
                    }
                    else {
                        set(key, value, ConfigSource::CommandLine);
                    }
                }
            } catch (...) {
                // Fall back to string
                set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        json j;
        
        for (const auto& pair : pImpl->entries) {
            const auto& entry = pair.second;
            if (entry.source != ConfigSource::Default) {
                j[pair.first] = valueToJson(entry.value);
            }
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(2); // Pretty print with 2-space indentation
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::string lowerKey = toLower(key);
    
    auto it = pImpl->entries.find(lowerKey);
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return valueToTyped<T>(it->second.value);
    } catch (const std::exception& e) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value) {
        return *value;
    }
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::string lowerKey = toLower(key);
    
    // Validate against schema if key exists in schema
    if (!validateKey(lowerKey)) {
        throw ConfigValidationException(lowerKey, "unknown key", "Key not in schema");
    }
    
    // Validate type based on schema
    validateType(lowerKey, value);
    
    pImpl->entries[lowerKey] = ConfigEntry(lowerKey, value, source);
    
    // Update keys list
    bool found = false;
    for (auto& k : pImpl->keys) {
        if (k == lowerKey) {
            found = true;
            break;
        }
    }
    if (!found) {
        pImpl->keys.push_back(lowerKey);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue v = value;
    set(key, v, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    ConfigValue v = value;
    set(key, v, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    ConfigValue v = value;
    set(key, v, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    ConfigValue v = value;
    set(key, v, source);
}

bool Config::has(const std::string& key) const {
    std::string lowerKey = toLower(key);
    return pImpl->entries.find(lowerKey) != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    std::string lowerKey = toLower(key);
    auto it = pImpl->entries.find(lowerKey);
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it);
        
        // Update keys list
        pImpl->keys.erase(std::remove(pImpl->keys.begin(), pImpl->keys.end(), lowerKey), pImpl->keys.end());
    }
}

std::vector<std::string> Config::getKeys() const {
    return pImpl->keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->keys.clear();
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "Configuration Summary:" << std::endl;
    ss << "Total keys: " << pImpl->entries.size() << std::endl;
    
    for (const auto& pair : pImpl->entries) {
        const auto& entry = pair.second;
        ss << "  " << pair.first << " (source: ";
        
        switch (entry.source) {
            case ConfigSource::Default: ss << "Default"; break;
            case ConfigSource::File: ss << "File"; break;
            case ConfigSource::CommandLine: ss << "CommandLine"; break;
            case ConfigSource::Runtime: ss << "Runtime"; break;
        }
        ss << ") = " << valueToString(entry.value) << std::endl;
    }
    
    return ss.str();
}

// Enhanced error handling methods
Result Config::loadFromFileSafe(const std::string& filepath) {
    Result result;
    try {
        loadFromFile(filepath);
        result.setValue(true);
    } catch (const ConfigLoadException& e) {
        result = Result(ErrorCode::ConfigError, std::string("Failed to load config from ") + filepath + ": " + e.what());
    } catch (const std::exception& e) {
        result = Result(ErrorCode::ConfigError, std::string("Unexpected error loading config from ") + filepath + ": " + e.what());
    }
    return result;
}

Result Config::setSafe(const std::string& key, const ConfigValue& value, ConfigSource source) {
    Result result;
    try {
        set(key, value, source);
        result.setValue(true);
    } catch (const ConfigValidationException& e) {
        result = Result(ErrorCode::InvalidParameter, std::string("Validation error for key '" + key + "': " + e.what()));
    } catch (const std::exception& e) {
        result = Result(ErrorCode::UnknownError, std::string("Error setting key '" + key + "': " + e.what()));
    }
    return result;
}

// Private helper methods
void Config::initializeDefaultSchema() {
    // Initialize with basic schema entries
    pImpl->schema.push_back({"simulation_step", "int", true, 1, {}, {1, 1000}});
    pImpl->schema.push_back({"timestep_duration", "double", true, 0.001, {}, {0.0, 1.0}});
    pImpl->schema.push_back({"max_neurons", "int", false, 10000, {}, {0, 100000}});
    pImpl->schema.push_back({"enable_stdp", "bool", false, true, {}, {}});
    pImpl->schema.push_back({"enable_hebbian", "bool", false, true, {}, {}});
    pImpl->schema.push_back({"enable_structural_plasticity", "bool", false, false, {}, {}});
    pImpl->schema.push_back({"spike_history_size", "int", false, 10000, {}, {100, 1000000}});
}

bool Config::validateKey(const std::string& key) const {
    // Allow runtime-added keys for flexibility
    if (key.find('_') != std::string::npos || key.find('-') != std::string::npos) {
        return true; // Allow custom keys with special characters
    }
    
    for (const auto& schema_entry : pImpl->schema) {
        if (schema_entry.name == key) {
            return true;
        }
    }
    
    return false;
}

void Config::validateType(const std::string& key, const ConfigValue& value) const {
    // Find schema entry for this key
    for (const auto& schema_entry : pImpl->schema) {
        if (schema_entry.name == key) {
            validateTypeInternal(schema_entry, value);
            return;
        }
    }
    
    // If no schema entry found, it's a custom key - less strict validation
    if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
        return; // int types are fine
    }
    if (std::holds_alternative<double>(value)) {
        return; // double types are fine
    }
    if (std::holds_alternative<bool>(value)) {
        return; // bool types are fine
    }
    if (std::holds_alternative<std::string>(value)) {
        return; // string types are fine
    }
    if (std::holds_alternative<std::vector<int>>(value)) {
        return; // int vector types are fine
    }
    if (std::holds_alternative<std::vector<double>>(value)) {
        return; // double vector types are fine
    }
    if (std::holds_alternative<std::vector<std::string>>(value)) {
        return; // string vector types are fine
    }
}

void Config::validateTypeInternal(const Impl::SchemaEntry& schema, const ConfigValue& value) const {
    // Check if value type matches schema type
    std::string actual_type = getTypeString(value);
    
    if (actual_type != schema.type) {
        throw ConfigValidationException(schema.name, schema.type, actual_type);
    }
    
    // Additional validation based on schema constraints
    if (schema.type == "int" || schema.type == "double") {
        double val = getNumericValue(value);
        if (val < schema.range.first || val > schema.range.second) {
            throw ConfigValidationException(schema.name, 
                "in range [" + std::to_string(schema.range.first) + ", " + std::to_string(schema.range.second) + "]",
                std::to_string(val));
        }
    }
    
    if (!schema.allowed_values.empty()) {
        std::string val_str = getStringValue(value);
        if (std::find(schema.allowed_values.begin(), schema.allowed_values.end(), val_str) == schema.allowed_values.end()) {
            throw ConfigValidationException(schema.name,
                "one of: " + vectorToString(schema.allowed_values),
                val_str);
        }
    }
}

void Config::loadFromJson(const json& j, ConfigSource source) {
    try {
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            json value = *it;
            
            ConfigValue config_value = jsonToValue(value);
            set(key, config_value, source);
        }
    } catch (const std::exception& e) {
        throw ConfigLoadException("JSON parsing", std::string("Error parsing JSON: ") + e.what());
    }
}

ConfigValue Config::jsonToValue(const json& j) {
    if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        // Determine array type by examining first element
        if (j.empty()) {
            return std::vector<int>(); // Default to int vector
        }
        
        const json& first = j[0];
        if (first.is_boolean()) {
            std::vector<std::string> vec;
            for (const auto& item : j) {
                vec.push_back(item.get<bool>() ? "true" : "false");
            }
            return vec;
        } else if (first.is_number_integer()) {
            std::vector<int> vec;
            for (const auto& item : j) {
                vec.push_back(item.get<int>());
            }
            return vec;
        } else if (first.is_number_float()) {
            std::vector<double> vec;
            for (const auto& item : j) {
                vec.push_back(item.get<double>());
            }
            return vec;
        } else {
            std::vector<std::string> vec;
            for (const auto& item : j) {
                vec.push_back(item.get<std::string>());
            }
            return vec;
        }
    }
    
    throw ConfigLoadException("unknown", "Unsupported JSON type");
}

json Config::valueToJson(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        json j = std::get<std::vector<int>>(value);
        return j;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        json j = std::get<std::vector<double>>(value);
        return j;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        json j = std::get<std::vector<std::string>>(value);
        return j;
    }
    
    throw ConfigLoadException("unknown", "Unsupported value type");
}

template<typename T>
T Config::valueToTyped(const ConfigValue& value) const {
    if constexpr (std::is_same_v<T, int>) {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<int>(std::get<int64_t>(value));
        }
    } else if constexpr (std::is_same_v<T, double>) {
        if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<double>(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            std::string str = std::get<std::string>(value);
            return str == "true" || str == "1" || str == "yes";
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        }
    }
    
    throw std::runtime_error("Type conversion failed");
}

std::string Config::valueToString(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        const auto& vec = std::get<std::vector<int>>(value);
        return "[" + vectorToString(vec) + "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& vec = std::get<std::vector<double>>(value);
        return "[" + vectorToString(vec) + "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        return "[" + vectorToString(vec) + "]";
    }
    
    return "<unknown>";
}

std::string Config::getTypeString(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
        return "int";
    } else if (std::holds_alternative<double>(value)) {
        return "double";
    } else if (std::holds_alternative<bool>(value)) {
        return "bool";
    } else if (std::holds_alternative<std::string>(value)) {
        return "string";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        return "vector<int>";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return "vector<double>";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return "vector<string>";
    }
    return "unknown";
}

double Config::getNumericValue(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return static_cast<double>(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    }
    throw std::runtime_error("Value is not numeric");
}

std::string Config::getStringValue(const ConfigValue& value) const {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    }
    throw std::runtime_error("Value is not a string");
}

std::string Config::vectorToString(const std::vector<int>& vec) const {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << vec[i];
    }
    ss << "]";
    return ss.str();
}

std::string Config::vectorToString(const std::vector<double>& vec) const {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << vec[i];
    }
    ss << "]";
    return ss.str();
}

std::string Config::vectorToString(const std::vector<std::string>& vec) const {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << "\"" << vec[i] << "\"";
    }
    ss << "]";
    return ss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

} // namespace nlm