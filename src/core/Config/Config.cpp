#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

// ConfigUtils implementation
namespace ConfigUtils {
    bool isNumberType(const ConfigValue& value) {
        return std::holds_alternative<int>(value) || 
               std::holds_alternative<int64_t>(value) || 
               std::holds_alternative<double>(value);
    }
    
    bool isStringType(const ConfigValue& value) {
        return std::holds_alternative<std::string>(value);
    }
    
    bool isBoolType(const ConfigValue& value) {
        return std::holds_alternative<bool>(value);
    }
    
    bool isArrayType(const ConfigValue& value) {
        return std::holds_alternative<std::vector<int>>(value) || 
               std::holds_alternative<std::vector<double>>(value) || 
               std::holds_alternative<std::vector<std::string>>(value);
    }
    
    std::string getTypeString(const ConfigValue& value) {
        if (isNumberType(value)) return "number";
        if (isStringType(value)) return "string";
        if (isBoolType(value)) return "boolean";
        if (isArrayType(value)) return "array";
        return "unknown";
    }
    
    // Convert ConfigValue to nlohmann::json
    nlohmann::json toJson(const ConfigValue& value) {
        nlohmann::json j;
        std::visit([&j](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                j = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j = arg;
            } else if constexpr (std::is_same_v<T, int> || 
                               std::is_same_v<T, int64_t> ||
                               std::is_same_v<T, double>) {
                j = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                j = nlohmann::json::array();
                for (const auto& item : arg) {
                    j.push_back(item);
                }
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j = nlohmann::json::array();
                for (const auto& item : arg) {
                    j.push_back(item);
                }
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j = nlohmann::json::array();
                for (const auto& item : arg) {
                    j.push_back(item);
                }
            }
        }, value);
        return j;
    }
    
    // Convert nlohmann::json to ConfigValue
    ConfigValue fromJson(const nlohmann::json& j) {
        ConfigValue value;
        
        if (j.is_string()) {
            value = j.get<std::string>();
        } else if (j.is_boolean()) {
            value = j.get<bool>();
        } else if (j.is_number_integer()) {
            // Try to fit into appropriate integer type
            if (j.get<int64_t>() <= std::numeric_limits<int>::max() && 
                j.get<int64_t>() >= std::numeric_limits<int>::min()) {
                value = j.get<int>();
            } else {
                value = j.get<int64_t>();
            }
        } else if (j.is_number_unsigned()) {
            if (j.get<uint64_t>() <= std::numeric_limits<int>::max()) {
                value = j.get<int>();
            } else {
                value = static_cast<int64_t>(j.get<uint64_t>());
            }
        } else if (j.is_number_float()) {
            value = j.get<double>();
        } else if (j.is_array()) {
            // Determine array element type
            if (!j.empty()) {
                if (j[0].is_number_integer()) {
                    value = j.get<std::vector<int>>();
                } else if (j[0].is_number_float()) {
                    value = j.get<std::vector<double>>();
                } else if (j[0].is_string()) {
                    value = j.get<std::vector<std::string>>();
                } else {
                    // Default to vector of doubles for unknown types
                    std::vector<double> vec;
                    for (const auto& item : j) {
                        vec.push_back(item.get<double>());
                    }
                    value = vec;
                }
            } else {
                // Empty array - default to vector of doubles
                value = std::vector<double>();
            }
        } else {
            // Default to string for unknown types
            value = j.dump();
        }
        
        return value;
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchemaEntry> defaultSchema;
    bool schemaApplied;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->schemaApplied = false;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON parser first
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse JSON
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            ConfigValue value = ConfigUtils::fromJson(*it);
            
            ConfigEntry entry;
            entry.key = key;
            entry.value = value;
            entry.source = ConfigSource::File;
            entry.description = "";  // JSON doesn't have descriptions
            entry.type = ConfigUtils::getTypeString(value);
            
            pImpl->entries.push_back(entry);
        }
        
        return true;
    } catch (const std::exception&) {
        // Fall back to simple key=value parser
        return Config::loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromJsonString(const std::string& jsonString) {
    try {
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse JSON string
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            ConfigValue value = ConfigUtils::fromJson(*it);
            
            ConfigEntry entry;
            entry.key = key;
            entry.value = value;
            entry.source = ConfigSource::File;
            entry.description = "";
            entry.type = ConfigUtils::getTypeString(value);
            
            pImpl->entries.push_back(entry);
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Use the original implementation
    return Config::loadFromSimpleArgs(argc, argv);
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
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
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromSimpleArgs(int argc, char** argv) {
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
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Save as JSON format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = ConfigUtils::toJson(entry.value);
        }
        
        file << std::setw(4) << jsonData;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string Config::toJsonString() const {
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = ConfigUtils::toJson(entry.value);
    }
    
    return jsonData.dump(4);
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
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
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        ConfigEntry entry;
        entry.key = key;
        entry.value = value;
        entry.source = source;
        entry.description = "";
        entry.type = ConfigUtils::getTypeString(value);
        pImpl->entries.push_back(entry);
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
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
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

std::vector<std::string> Config::validate(const std::vector<ConfigSchemaEntry>& schema) const {
    std::vector<std::string> errors;
    
    // Convert schema to map for easy lookup
    std::map<std::string, ConfigSchemaEntry> schemaMap;
    for (const auto& entry : schema) {
        schemaMap[entry.key] = entry;
    }
    
    // Validate each entry against schema
    for (const auto& configEntry : pImpl->entries) {
        auto schemaIt = schemaMap.find(configEntry.key);
        if (schemaIt == schemaMap.end()) {
            // Key not in schema - warn but don't error
            continue;
        }
        
        const ConfigSchemaEntry& schemaEntry = schemaIt->second;
        
        // Check type
        std::string actualType = ConfigUtils::getTypeString(configEntry.value);
        std::string expectedType = schemaEntry.type;
        
        // Type matching
        bool typeMatches = false;
        if (expectedType == "number" && (actualType == "number" || actualType == "integer")) {
            typeMatches = true;
        } else if (expectedType == "string" && actualType == "string") {
            typeMatches = true;
        } else if (expectedType == "boolean" && actualType == "boolean") {
            typeMatches = true;
        } else if (expectedType == "array" && actualType == "array") {
            typeMatches = true;
        }
        
        if (!typeMatches) {
            errors.push_back("Key '" + configEntry.key + "': expected type '" + expectedType + "', got '" + actualType + "'");
        }
        
        // Check value range/min/max for numbers
        if (typeMatches && (actualType == "number" || actualType == "integer")) {
            try {
                double value = 0.0;
                if (std::holds_alternative<int>(configEntry.value)) {
                    value = static_cast<double>(std::get<int>(configEntry.value));
                } else if (std::holds_alternative<int64_t>(configEntry.value)) {
                    value = static_cast<double>(std::get<int64_t>(configEntry.value));
                } else if (std::holds_alternative<double>(configEntry.value)) {
                    value = std::get<double>(configEntry.value);
                }
                
                if (value < schemaEntry.min || value > schemaEntry.max) {
                    errors.push_back("Key '" + configEntry.key + "': value " + std::to_string(value) + 
                                   " outside allowed range [" + std::to_string(schemaEntry.min) + ", " + 
                                   std::to_string(schemaEntry.max) + "]");
                }
                
                // Check against allowed values
                if (!schemaEntry.allowedValues.empty()) {
                    std::string valueStr = std::to_string(value);
                    bool found = false;
                    for (const auto& allowed : schemaEntry.allowedValues) {
                        if (valueStr == allowed) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        errors.push_back("Key '" + configEntry.key + "': value " + valueStr + 
                                       " not in allowed values list");
                    }
                }
            } catch (const std::bad_variant_access&) {
                errors.push_back("Key '" + configEntry.key + "': type mismatch in validation");
            }
        }
        
        // Check required keys
        if (schemaEntry.required && !has(configEntry.key)) {
            errors.push_back("Key '" + configEntry.key + "': is required but missing");
        }
    }
    
    return errors;
}

void Config::applySchema(const std::vector<ConfigSchemaEntry>& schema) {
    // Convert schema to map for easy lookup
    std::map<std::string, ConfigSchemaEntry> schemaMap;
    for (const auto& entry : schema) {
        schemaMap[entry.key] = entry;
    }
    
    // Apply schema defaults
    for (const auto& schemaEntry : schema) {
        if (!has(schemaEntry.key)) {
            // Set default value if key doesn't exist
            // Parse default value based on type
            ConfigValue defaultValue;
            
            if (schemaEntry.type == "int") {
                try {
                    int val = std::stoi(schemaEntry.defaultValue);
                    defaultValue = val;
                } catch (...) {
                    continue;  // Skip invalid defaults
                }
            } else if (schemaEntry.type == "double") {
                try {
                    double val = std::stod(schemaEntry.defaultValue);
                    defaultValue = val;
                } catch (...) {
                    continue;  // Skip invalid defaults
                }
            } else if (schemaEntry.type == "bool") {
                std::string lowerDefault = toLower(schemaEntry.defaultValue);
                bool val = (lowerDefault == "true" || lowerDefault == "1" || lowerDefault == "yes");
                defaultValue = val;
            } else if (schemaEntry.type == "string") {
                defaultValue = schemaEntry.defaultValue;
            } else if (schemaEntry.type == "array") {
                // Parse array from string (simplified - assumes JSON-like format)
                defaultValue = std::vector<double>();  // Default to empty array
            }
            
            if (!std::holds_alternative<std::monostate>(defaultValue)) {
                set(schemaEntry.key, defaultValue, ConfigSource::Default);
            }
        }
    }
    
    pImpl->defaultSchema = schema;
    pImpl->schemaApplied = true;
}

void Config::setDefaultSchema(const std::vector<ConfigSchemaEntry>& schema) {
    pImpl->defaultSchema = schema;
    pImpl->schemaApplied = false;  // Not yet applied
}

std::vector<ConfigSchemaEntry> Config::getDefaultSchema() const {
    return pImpl->defaultSchema;
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

// Explicit template instantiations
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

} // namespace nlm
