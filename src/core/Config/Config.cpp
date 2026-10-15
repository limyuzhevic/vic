#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cassert>

namespace nlm {

bool Config::loadFromJson(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << filepath << std::endl;
        return false;
    }
    
    try {
        JsonValue json;
        file >> json;
        return parseJsonToConfig(json, entries_);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON config file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromJsonString(const std::string& jsonString) {
    try {
        JsonValue json = JsonValue::parse(jsonString);
        return parseJsonToConfig(json, entries_);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON string: " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromYaml(const std::string& filepath) {
    // TODO: Implement YAML support with yaml-cpp
    std::cerr << "YAML parsing not yet implemented" << std::endl;
    return false;
}

bool Config::loadFromArgs(int argc, char** argv) {
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

bool Config::saveToJson(const std::string& filepath, bool pretty) const {
    JsonValue json;
    
    for (const auto& entry : entries_) {
        json[entry.key] = convertToJson(entry.value);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file for writing: " << filepath << std::endl;
        return false;
    }
    
    try {
        if (pretty) {
            file << json.dump(2);
        } else {
            file << json.dump();
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error writing JSON config file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = entries_.find(key);
    if (it == entries_.end()) {
        return std::nullopt;
    }
    
    try {
        const ConfigValue& variantValue = it->second.value;
        const T* valuePtr = std::get_if<T>(&variantValue);
        if (valuePtr) {
            return *valuePtr;
        } else {
            std::cerr << "Config::get: Type mismatch for key '" << key << "'" << std::endl;
            return std::nullopt;
        }
    } catch (const std::bad_variant_access&) {
        std::cerr << "Config::get: Bad variant access for key '" << key << "'" << std::endl;
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    entries_[key] = ConfigEntry(key, value, source);
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setString(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, value, source);
}

void Config::setNumber(const std::string& key, double value, ConfigSource source) {
    set(key, value, source);
}

void Config::setBool(const std::string& key, bool value, ConfigSource source) {
    set(key, value, source);
}

bool Config::has(const std::string& key) const {
    return entries_.find(key) != entries_.end();
}

void Config::remove(const std::string& key) {
    entries_.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(entries_.size());
    for (const auto& entry : entries_) {
        keys.push_back(entry.first);
    }
    return keys;
}

void Config::clear() {
    entries_.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << entries_.size() << " entries):\n";
    for (const auto& entry : entries_) {
        oss << "  " << entry.first << " = [";
        const ConfigValue& value = entry.second.value;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, value);
        oss << "] (" << static_cast<int>(entry.second.source) << ")\n";
    }
    return oss.str();
}

bool Config::validateWithSchema(const ConfigSchema& schema) const {
    return schema.validate(entries_);
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    bool success = true;
    
    for (auto& [key, entry] : entries_) {
        if (key.find(prefix) == 0) {
            std::string envKey = prefix + key.substr(prefix.length());
            const char* envValue = std::getenv(envKey.c_str());
            if (envValue) {
                set(key, std::string(envValue), ConfigSource::Environment);
            }
        }
    }
    
    return success;
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

bool Config::parseJsonToConfig(const JsonValue& json, std::unordered_map<std::string, ConfigEntry>& entries) {
    if (!json.is_object()) {
        std::cerr << "JSON config must be an object" << std::endl;
        return false;
    }
    
    entries.clear();
    
    for (auto it = json.begin(); it != json.end(); ++it) {
        const std::string& key = it.key();
        const JsonValue& value = it.value();
        
        ConfigValue configValue;
        
        if (value.is_number_integer()) {
            configValue = value.get<int64_t>();
        } else if (value.is_number_unsigned()) {
            configValue = value.get<uint64_t>();
        } else if (value.is_number_float()) {
            configValue = value.get<double>();
        } else if (value.is_boolean()) {
            configValue = value.get<bool>();
        } else if (value.is_string()) {
            configValue = value.get<std::string>();
        } else if (value.is_array()) {
            // Determine array type based on elements
            bool allInts = true;
            bool allDoubles = true;
            bool allStrings = true;
            
            for (const auto& element : value) {
                if (!element.is_number_integer()) allInts = false;
                if (!element.is_number_float() && !element.is_number_integer()) allDoubles = false;
                if (!element.is_string()) allStrings = false;
            }
            
            if (allInts) {
                std::vector<int64_t> intArray;
                for (const auto& element : value) {
                    intArray.push_back(element.get<int64_t>());
                }
                configValue = intArray;
            } else if (allDoubles) {
                std::vector<double> doubleArray;
                for (const auto& element : value) {
                    doubleArray.push_back(element.get<double>());
                }
                configValue = doubleArray;
            } else if (allStrings) {
                std::vector<std::string> stringArray;
                for (const auto& element : value) {
                    stringArray.push_back(element.get<std::string>());
                }
                configValue = stringArray;
            } else {
                std::cerr << "Unsupported JSON array type for key '" << key << "'" << std::endl;
                return false;
            }
        } else {
            std::cerr << "Unsupported JSON value type for key '" << key << "'" << std::endl;
            return false;
        }
        
        entries[key] = ConfigEntry(key, configValue, ConfigSource::File);
    }
    
    return true;
}

JsonValue Config::convertToJson(const ConfigValue& value) {
    JsonValue json;
    
    std::visit([&json](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json = arg;
        }
    }, value);
    
    return json;
}

// Template instantiations
namespace nlm {

// Config getter template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;

template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;

template std::optional<double> Config::get<double>(const std::string&) const;

template std::optional<bool> Config::get<bool>(const std::string&) const;

template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// Config getOr template instantiations

template int Config::getOr<int>(const std::string&, const int&) const;

template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;

template double Config::getOr<double>(const std::string&, const double&) const;

template bool Config::getOr<bool>(const std::string&, const bool&) const;

template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

// Explicit template instantiations for common types
template std::optional<int> Config::get<int>(const std::string& key) const {
    return Config::get<int>(key);
}

template std::optional<int64_t> Config::get<int64_t>(const std::string& key) const {
    return Config::get<int64_t>(key);
}

template std::optional<double> Config::get<double>(const std::string& key) const {
    return Config::get<double>(key);
}

template std::optional<bool> Config::get<bool>(const std::string& key) const {
    return Config::get<bool>(key);
}

template std::optional<std::string> Config::get<std::string>(const std::string& key) const {
    return Config::get<std::string>(key);
}

// getOr instantiations
template int Config::getOr<int>(const std::string& key, const int& defaultValue) const {
    return Config::getOr<int>(key, defaultValue);
}

template int64_t Config::getOr<int64_t>(const std::string& key, const int64_t& defaultValue) const {
    return Config::getOr<int64_t>(key, defaultValue);
}

template double Config::getOr<double>(const std::string& key, const double& defaultValue) const {
    return Config::getOr<double>(key, defaultValue);
}

template bool Config::getOr<bool>(const std::string& key, const bool& defaultValue) const {
    return Config::getOr<bool>(key, defaultValue);
}

template std::string Config::getOr<std::string>(const std::string& key, const std::string& defaultValue) const {
    return Config::getOr<std::string>(key, defaultValue);
}
