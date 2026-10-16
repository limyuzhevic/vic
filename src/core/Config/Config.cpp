#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <cstring> // For strcmp

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    nlohmann::json schema;
    bool autoValidate;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->autoValidate = false;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        return loadFromString(jsonConfig.dump());
    } catch (const std::exception&) {
        // If JSON parsing fails, try simple key=value format for backward compatibility
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
                
                // Try to infer type
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
}

bool Config::loadFromString(const std::string& jsonString) {
    try {
        clear();
        auto jsonConfig = nlohmann::json::parse(jsonString);
        
        if (jsonConfig.is_object()) {
            for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
                set(it.key(), it.value(), ConfigSource::File);
            }
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool changed = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
                changed = true;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            changed = true;
        }
    }
    return changed;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        nlohmann::json jsonConfig;
        
        for (const auto& entry : pImpl->entries) {
            // Convert value to JSON
            nlohmann::json jsonValue;
            
            if (entry.value.is_number_integer()) {
                jsonValue = entry.value.get<int64_t>();
            } else if (entry.value.is_number()) {
                jsonValue = entry.value.get<double>();
            } else if (entry.value.is_boolean()) {
                jsonValue = entry.value.get<bool>();
            } else if (entry.value.is_string()) {
                jsonValue = entry.value.get<std::string>();
            } else if (entry.value.is_array()) {
                jsonValue = entry.value.get<nlohmann::json::array_t>();
            } else if (entry.value.is_object()) {
                jsonValue = entry.value.get<nlohmann::json::object_t>();
            } else {
                jsonValue = entry.value;
            }
            
            jsonConfig[entry.key] = jsonValue;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << jsonConfig.dump(2); // Pretty print with 2-space indentation
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        if constexpr (std::is_same_v<T, int>) {
            if (it->value.is_number_integer()) {
                return it->value.get<int>();
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (it->value.is_number_integer()) {
                return it->value.get<int64_t>();
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (it->value.is_number()) {
                return it->value.get<double>();
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (it->value.is_boolean()) {
                return it->value.get<bool>();
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (it->value.is_string()) {
                return it->value.get<std::string>();
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (it->value.is_array()) {
                auto vec = it->value.get<std::vector<int>>();
                return vec;
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (it->value.is_array()) {
                auto vec = it->value.get<std::vector<double>>();
                return vec;
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (it->value.is_array()) {
                auto vec = it->value.get<std::vector<std::string>>();
                return vec;
            }
        }
    } catch (const std::exception&) {
        // Type conversion failed
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const nlohmann::json& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        ConfigEntry newEntry(key, value, source);
        pImpl->entries.push_back(newEntry);
    }
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, toJson(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, toJson(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, toJson(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, toJson(value), source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, toJson(value), source);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, toJson(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, toJson(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, toJson(value), source);
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
        oss << entry.value.dump();
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::vector<ConfigValidationError> Config::validateSchema(const nlohmann::json& schema) const {
    std::vector<ConfigValidationError> errors;
    
    if (!schema.is_object()) {
        errors.emplace_back("SCHEMA", "Invalid schema format: must be a JSON object", "format_error");
        return errors;
    }
    
    // Validate each entry against schema
    for (const auto& entry : pImpl->entries) {
        auto schemaIt = schema.find(entry.key);
        if (schemaIt == schema.end()) {
            if (entry.required) {
                errors.emplace_back(entry.key, "Required configuration key is missing", "missing_required");
            }
            continue;
        }
        
        const auto& schemaValue = *schemaIt;
        
        // Check type
        if (!schemaValue.contains("type")) {
            errors.emplace_back(entry.key, "Schema entry missing type specification", "invalid_schema");
            continue;
        }
        
        std::string expectedType = schemaValue["type"];
        if (!validateValue(entry.value, expectedType)) {
            errors.emplace_back(entry.key, 
                "Expected type " + expectedType + ", got " + getJsonType(entry.value), 
                "type_mismatch");
        }
        
        // Check constraints
        if (schemaValue.contains("min") && entry.value.is_number()) {
            double min = schemaValue["min"];
            if (entry.value.get<double>() < min) {
                errors.emplace_back(entry.key, 
                    "Value " + std::to_string(entry.value.get<double>()) + " is less than minimum " + std::to_string(min), 
                    "constraint_violation");
            }
        }
        
        if (schemaValue.contains("max") && entry.value.is_number()) {
            double max = schemaValue["max"];
            if (entry.value.get<double>() > max) {
                errors.emplace_back(entry.key, 
                    "Value " + std::to_string(entry.value.get<double>()) + " exceeds maximum " + std::to_string(max), 
                    "constraint_violation");
            }
        }
    }
    
    return errors;
}

nlohmann::json Config::createSchema(const std::vector<ConfigEntry>& entries) {
    nlohmann::json schema = nlohmann::json::object();
    
    for (const auto& entry : entries) {
        nlohmann::json schemaEntry = nlohmann::json::object();
        schemaEntry["type"] = entry.type;
        schemaEntry["description"] = entry.description;
        schemaEntry["required"] = entry.required;
        
        // Add type-specific constraints
        if (entry.type == ConfigType::INTEGER || entry.type == ConfigType::DOUBLE) {
            if (entry.value.is_number()) {
                double value = entry.value.get<double>();
                schemaEntry["min"] = value - 1; // Example constraint
                schemaEntry["max"] = value + 1;
            }
        }
        
        schema[entry.key] = schemaEntry;
    }
    
    return schema;
}

nlohmann::json Config::toJson(int value) {
    return nlohmann::json(value);
}

nlohmann::json Config::toJson(int64_t value) {
    return nlohmann::json(value);
}

nlohmann::json Config::toJson(double value) {
    return nlohmann::json(value);
}

nlohmann::json Config::toJson(bool value) {
    return nlohmann::json(value);
}

nlohmann::json Config::toJson(const std::string& value) {
    return nlohmann::json(value);
}

nlohmann::json Config::toJson(const std::vector<int>& value) {
    return nlohmann::json(value);
}

nlohmann::json Config::toJson(const std::vector<double>& value) {
    return nlohmann::json(value);
}

nlohmann::json Config::toJson(const std::vector<std::string>& value) {
    return nlohmann::json(value);
}

std::optional<int> Config::fromJson(const nlohmann::json& json) {
    if (json.is_number_integer()) {
        return json.get<int>();
    }
    return std::nullopt;
}

std::optional<int64_t> Config::fromJson(const nlohmann::json& json) {
    if (json.is_number_integer()) {
        return json.get<int64_t>();
    }
    return std::nullopt;
}

std::optional<double> Config::fromJson(const nlohmann::json& json) {
    if (json.is_number()) {
        return json.get<double>();
    }
    return std::nullopt;
}

std::optional<bool> Config::fromJson(const nlohmann::json& json) {
    if (json.is_boolean()) {
        return json.get<bool>();
    }
    return std::nullopt;
}

std::optional<std::string> Config::fromJson(const nlohmann::json& json) {
    if (json.is_string()) {
        return json.get<std::string>();
    }
    return std::nullopt;
}

std::optional<std::vector<int>> Config::fromJson(const nlohmann::json& json) {
    if (json.is_array()) {
        auto vec = json.get<std::vector<int>>();
        return vec;
    }
    return std::nullopt;
}

std::optional<std::vector<double>> Config::fromJson(const nlohmann::json& json) {
    if (json.is_array()) {
        auto vec = json.get<std::vector<double>>();
        return vec;
    }
    return std::nullopt;
}

std::optional<std::vector<std::string>> Config::fromJson(const nlohmann::json& json) {
    if (json.is_array()) {
        auto vec = json.get<std::vector<std::string>>();
        return vec;
    }
    return std::nullopt;
}

bool Config::validateValue(const nlohmann::json& value, const std::string& expectedType) const {
    if (expectedType == ConfigType::ANY) {
        return true;
    }
    
    if (expectedType == ConfigType::INTEGER && value.is_number_integer()) {
        return true;
    } else if (expectedType == ConfigType::INTEGER64 && value.is_number_integer()) {
        return true;
    } else if (expectedType == ConfigType::DOUBLE && value.is_number()) {
        return true;
    } else if (expectedType == ConfigType::STRING && value.is_string()) {
        return true;
    } else if (expectedType == ConfigType::BOOLEAN && value.is_boolean()) {
        return true;
    } else if (expectedType == ConfigType::ARRAY && value.is_array()) {
        return true;
    } else if (expectedType == ConfigType::OBJECT && value.is_object()) {
        return true;
    }
    
    return false;
}

std::string Config::getJsonType(const nlohmann::json& value) const {
    if (value.is_null()) return "null";
    if (value.is_boolean()) return "boolean";
    if (value.is_number_integer()) return "integer";
    if (value.is_number()) return "number";
    if (value.is_string()) return "string";
    if (value.is_array()) return "array";
    if (value.is_object()) return "object";
    return "unknown";
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
namespace nlm {
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
}

} // namespace nlm
