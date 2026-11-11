#include "Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cmath>
#include <pybind11/pybind11.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schema;
    bool schemaEnabled;
    
    Impl() : schemaEnabled(false) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        std::cerr << "Error parsing config file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
    
    return loadFromJson(j);
}

bool Config::loadFromJson(const json& j) {
    clear();
    
    if (!j.is_object()) {
        return false;
    }
    
    for (const auto& item : j.items()) {
        const std::string& key = item.key();
        ConfigValue value;
        
        try {
            value = item.value().get<ConfigValue>();
        } catch (const json::type_error& e) {
            std::cerr << "Error parsing config value for key " << key << ": " << e.what() << std::endl;
            // Try to convert to string if it's a string in JSON
            if (item.value().is_string()) {
                value = item.value().get<std::string>();
            } else {
                continue;
            }
        }
        
        set(key, value, ConfigSource::File);
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    clear();
    
    // Default values from the original Config class
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        size_t eqPos = arg.find('=');
        
        if (eqPos != std::string::npos) {
            std::string key = arg.substr(0, eqPos);
            std::string valueStr = arg.substr(eqPos + 1);
            ConfigValue configValue;
            
            // Try to determine type
            if (isNumeric(valueStr)) {
                // Try to parse as int
                try {
                    int intVal = std::stoi(valueStr);
                    configValue = intVal;
                } catch (...) {
                    // Try to parse as double
                    try {
                        double doubleVal = std::stod(valueStr);
                        configValue = doubleVal;
                    } catch (...) {
                        configValue = valueStr;
                    }
                }
            } else if (valueStr == "true" || valueStr == "false") {
                configValue = (valueStr == "true");
            } else {
                configValue = valueStr;
            }
            
            set(key, configValue, ConfigSource::CommandLine);
        } else {
            // Handle boolean flags
            if (arg == "--enable_debug") {
                set("debug_mode", true, ConfigSource::CommandLine);
            } else if (arg == "--disable_debug") {
                set("debug_mode", false, ConfigSource::CommandLine);
            }
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    json j;
    if (!saveToJson(j)) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        file << j.dump(2); // Pretty print with 2-space indentation
    } catch (const json::exception& e) {
        std::cerr << "Error saving config to file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
    
    return true;
}

bool Config::saveToJson(json& j) const {
    j = json::object();
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = entry.value;
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            const ConfigValue& value = entry.value;
            try {
                return std::get<T>(value);
            } catch (const std::bad_variant_access&) {
                return std::nullopt;
            }
        }
    }
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    return value.has_value() ? *value : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate if schema is enabled
    if (pImpl->schemaEnabled) {
        for (const auto& schema : pImpl->schema) {
            if (schema.key == key) {
                ValidationError error;
                error.key = key;
                error.expectedType = schema.type;
                error.actualType = configTypeToString(value);
                error.message = "Type validation failed for key: " + key;
                
                if (!schema.validate(value)) {
                    error.message = schema.validationErrors.empty() ? "Validation failed" : schema.validationErrors[0].message;
                    // In a real implementation, we might want to log this error
                    // For now, we'll just continue but mark as invalid
                }
                break;
            }
        }
    }
    
    // Remove existing entry
    for (auto it = pImpl->entries.begin(); it != pImpl->entries.end(); ++it) {
        if (it->key == key) {
            pImpl->entries.erase(it);
            break;
        }
    }
    
    // Add new entry
    pImpl->entries.emplace_back(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

int Config::getInt(const std::string& key) const {
    return getOr<int>(key, 0);
}

int64_t Config::getInt64(const std::string& key) const {
    return getOr<int64_t>(key, 0);
}

double Config::getDouble(const std::string& key) const {
    return getOr<double>(key, 0.0);
}

bool Config::getBool(const std::string& key) const {
    return getOr<bool>(key, false);
}

std::string Config::getString(const std::string& key) const {
    return getOr<std::string>(key, "");
}

std::vector<int> Config::getIntVector(const std::string& key) const {
    return getOr<std::vector<int>>(key, std::vector<int>());
}

std::vector<double> Config::getDoubleVector(const std::string& key) const {
    return getOr<std::vector<double>>(key, std::vector<double>());
}

std::vector<std::string> Config::getStringVector(const std::string& key) const {
    return getOr<std::vector<std::string>>(key, std::vector<std::string>());
}

void Config::setInt(const std::string& key, int value, ConfigSource source) {
    set(key, value, source);
}

void Config::setInt64(const std::string& key, int64_t value, ConfigSource source) {
    set(key, value, source);
}

void Config::setDouble(const std::string& key, double value, ConfigSource source) {
    set(key, value, source);
}

void Config::setBool(const std::string& key, bool value, ConfigSource source) {
    set(key, value, source);
}

void Config::setString(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, value, source);
}

void Config::setIntVector(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, value, source);
}

void Config::setDoubleVector(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, value, source);
}

void Config::setStringVector(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, value, source);
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
    for (auto it = pImpl->entries.begin(); it != pImpl->entries.end(); ++it) {
        if (it->key == key) {
            pImpl->entries.erase(it);
            return;
        }
    }
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
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:";
    oss << "\nTotal entries: " << pImpl->entries.size();
    oss << "\nSchema enabled: " << (pImpl->schemaEnabled ? "Yes" : "No");
    oss << "\nSchema entries: " << pImpl->schema.size();
    
    oss << "\n\nConfiguration entries:";
    for (const auto& entry : pImpl->entries) {
        oss << "\n  " << entry.key << " = " << configValueToString(entry.value)
            << " (source: " << configSourceToString(entry.source) << ")";
    }
    
    // Run validation if schema is enabled
    if (pImpl->schemaEnabled) {
        ValidationResult validation = validate();
        oss << "\n\nValidation: " << validation.summary();
    }
    
    return oss.str();
}

ValidationResult Config::validate(const std::vector<ConfigSchema>& schema) const {
    ValidationResult result;
    
    if (!schema.empty()) {
        result = validateSchema(schema);
    } else if (pImpl->schemaEnabled) {
        result = validateSchema(pImpl->schema);
    }
    
    return result;
}

ValidationResult Config::validateSchema(const std::vector<ConfigSchema>& schema) const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        for (const auto& schemaEntry : schema) {
            if (schemaEntry.key == entry.key) {
                if (!schemaEntry.validate(entry.value)) {
                    for (const auto& error : schemaEntry.validationErrors) {
                        result.addError(error);
                    }
                }
                break;
            }
        }
    }
    
    return result;
}

void Config::setSchema(const std::vector<ConfigSchema>& schema) {
    pImpl->schema = schema;
    pImpl->schemaEnabled = !schema.empty();
}

void Config::addSchema(const ConfigSchema& schema) {
    pImpl->schema.push_back(schema);
    pImpl->schemaEnabled = true;
}

const std::vector<ConfigSchema>& Config::getSchema() const {
    return pImpl->schema;
}

std::vector<ConfigSchema> Config::getValidEntries() const {
    std::vector<ConfigSchema> validEntries;
    for (const auto& schema : pImpl->schema) {
        if (schema.validationErrors.empty()) {
            validEntries.push_back(schema);
        }
    }
    return validEntries;
}

void Config::setSource(const std::string& key, ConfigSource source) {
    for (auto& entry : pImpl->entries) {
        if (entry.key == key) {
            entry.source = source;
            break;
        }
    }
}

ConfigSource Config::getSource(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            return entry.source;
        }
    }
    return ConfigSource::Default;
}

time_t Config::getTimestamp(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            return entry.timestamp;
        }
    }
    return 0;
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& otherEntry : other.pImpl->entries) {
            if (entry.key == otherEntry.key && entry.value == otherEntry.value) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    
    return true;
}

json Config::toJson() const {
    json j;
    
    // Add entries
    j["entries"] = json::array();
    for (const auto& entry : pImpl->entries) {
        j["entries"].push_back(entry.to_json());
    }
    
    // Add schema if enabled
    if (pImpl->schemaEnabled) {
        j["schema"] = json::array();
        for (const auto& schema : pImpl->schema) {
            j["schema"].push_back(schema.to_json());
        }
    }
    
    return j;
}

Config Config::fromJson(const json& j) {
    Config config;
    
    // Restore entries
    if (j.contains("entries")) {
        for (const auto& item : j["entries"]) {
            ConfigEntry entry = ConfigEntry::from_json(item);
            // Reconstruct the entry (simplified - in real implementation would need proper reconstruction)
            config.pImpl->entries.push_back(entry);
        }
    }
    
    // Restore schema if present
    if (j.contains("schema")) {
        for (const auto& item : j["schema"]) {
            ConfigSchema schema = ConfigSchema::from_json(item);
            config.pImpl->schema.push_back(schema);
        }
        config.pImpl->schemaEnabled = true;
    }
    
    return config;
}

std::string Config::configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    
    try {
        if (std::holds_alternative<int>(value)) {
            oss << std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            oss << std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            oss << std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            oss << (std::get<bool>(value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(value)) {
            oss << "\"" << std::get<std::string>(value) << "\"";
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            oss << "[";
            const auto& vec = std::get<std::vector<int>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            oss << "[";
            const auto& vec = std::get<std::vector<double>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            oss << "[";
            const auto& vec = std::get<std::vector<std::string>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << vec[i] << "\"";
            }
            oss << "]";
        }
    } catch (const std::bad_variant_access&) {
        oss << "<invalid>";
    }
    
    return oss.str();
}

std::string Config::configSourceToString(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Runtime: return "Runtime";
        case ConfigSource::Python: return "Python";
        default: return "Unknown";
    }
}

std::string Config::configTypeToString(const ConfigValue& value) {
    try {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64";
        if (std::holds_alternative<double>(value)) return "double";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
        if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
        if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    } catch (const std::bad_variant_access&) {}
    return "unknown";
}

// Utility functions implementation
std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(c);
    }
    return result;
}

bool Config::isNumeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit) ||
           (str[0] == '-' && str.size() > 1 && std::all_of(str.begin() + 1, str.end(), ::isdigit)) ||
           (str.find('.') != std::string::npos && !str.empty());
}

ConfigValue Config::stringToConfigValue(const std::string& str) {
    std::string trimmed = trim(str);
    
    if (trimmed == "true" || trimmed == "false") {
        return trimmed == "true";
    }
    
    if (isNumeric(trimmed)) {
        try {
            size_t pos;
            int64_t intVal = std::stoll(trimmed, &pos);
            if (pos == trimmed.length()) {
                return intVal;
            }
            
            double doubleVal = std::stod(trimmed, &pos);
            if (pos == trimmed.length()) {
                return doubleVal;
            }
        } catch (...) {
            // Fall through to string
        }
    }
    
    return trimmed;
}

std::vector<std::string> Config::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        result.push_back(trim(token));
    }
    
    return result;
}

void Config::configValueToJson(const ConfigValue& value, json& j) {
    try {
        if (std::holds_alternative<int>(value)) {
            j = std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            j = std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            j = std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            j = std::get<bool>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            j = std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            j = std::get<std::vector<int>>(value);
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            j = std::get<std::vector<double>>(value);
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            j = std::get<std::vector<std::string>>(value);
        }
    } catch (const std::bad_variant_access&) {
        j = "<invalid>";
    }
}

ConfigValue Config::jsonToConfigValue(const json& j) {
    if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        // Determine array type
        if (!j.empty()) {
            if (j[0].is_number_integer()) {
                std::vector<int> vec;
                for (const auto& item : j) {
                    vec.push_back(item.get<int>());
                }
                return vec;
            } else if (j[0].is_number_float()) {
                std::vector<double> vec;
                for (const auto& item : j) {
                    vec.push_back(item.get<double>());
                }
                return vec;
            } else if (j[0].is_string()) {
                std::vector<std::string> vec;
                for (const auto& item : j) {
                    vec.push_back(item.get<std::string>());
                }
                return vec;
            }
        }
    }
    
    return std::string("<invalid>");
}

bool Config::validateType(const ConfigValue& value, const std::string& type) const {
    std::string actualType = configTypeToString(value);
    
    if (type == "int" && actualType == "int") return true;
    if (type == "int64" && actualType == "int64") return true;
    if ((type == "double" || type == "float") && actualType == "double") return true;
    if (type == "bool" && actualType == "bool") return true;
    if (type == "string" && actualType == "string") return true;
    if (type == "vector<int>" && actualType == "vector<int>") return true;
    if ((type == "vector<double>" || type == "vector<float>") && actualType == "vector<double>") return true;
    if (type == "vector<string>" && actualType == "vector<string>") return true;
    
    return false;
}

ValidationError Config::createValidationError(const std::string& key, const std::string& expected, const std::string& actual, const std::string& message) {
    ValidationError error;
    error.key = key;
    error.expectedType = expected;
    error.actualType = actual;
    error.message = message;
    return error;
}

} // namespace nlm