// Project: NLM (Neural Learning Machine)
// Description: Enhanced Config class with JSON/YAML support and backward compatibility
// File: Config.cpp
// Author: Kilo
// Date: 2026-09-14

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <unordered_map>
#include <cctype>
#include <numeric>

// Try to use nlohmann/json if available, otherwise provide minimal JSON support
#if __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::parseFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read first few bytes to detect format
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    // Check if file appears to be JSON (starts with { or [ or has ", etc.)
    bool isJson = false;
    if (!firstLine.empty()) {
        // Check for obvious JSON indicators
        if ((firstLine.find('{') != std::string::npos && firstLine.find('}') != std::string::npos) ||
            (firstLine.find('[') != std::string::npos && firstLine.find(']') != std::string::npos) ||
            firstLine.find('"') != std::string::npos) {
            isJson = true;
        }
        
        // Also check for JSON-like content in the file
        if (!isJson) {
            file.clear();
            file.seekg(0);
            std::string line;
            size_t bracketCount = 0;
            while (std::getline(file, line)) {
                if (line.find('"') != std::string::npos) {
                    isJson = true;
                    break;
                }
                bracketCount += std::count(line.begin(), line.end(), '{');
                bracketCount -= std::count(line.begin(), line.end(), '}');
                if (bracketCount > 0) {
                    isJson = true;
                    break;
                }
            }
            file.clear();
            file.seekg(0);
        }
    }
    
    // Parse based on detected format
    if (isJson) {
#if __has_include(<nlohmann/json.hpp>)
        return parseJsonFile(filepath);
#else
        // Fallback to key=value format
        clear();
        return parseKeyValueFile(filepath);
#endif
    } else {
        // Parse as key=value format
        clear();
        return parseKeyValueFile(filepath);
    }
}

bool Config::parseKeyValueFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear();
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
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

#if __has_include(<nlohmann/json.hpp>)
bool Config::parseJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear();
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Parse JSON object
        if (jsonData.is_object()) {
            for (const auto& item : jsonData.items()) {
                ConfigEntry entry;
                entry.key = item.key();
                entry.source = ConfigSource::File;
                
                // Try to convert JSON value to ConfigValue
                if (jsonToConfigValue(item.value(), entry.value)) {
                    // Set description if present
                    if (item.value().is_object() && item.value().contains("__description__")) {
                        entry.description = item.value()["__description__"].get<std::string>();
                    }
                    pImpl->entries.push_back(entry);
                }
            }
        } else {
            // JSON array not supported for config
            return false;
        }
    } catch (const std::exception& e) {
        // JSON parsing failed, try legacy format
        file.clear();
        file.seekg(0);
        return parseKeyValueFile(filepath);
    }
    
    return true;
}
#endif

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
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

// Helper functions for JSON conversion
namespace {

inline bool isNumber(const std::string& str, double& value) {
    try {
        size_t pos;
        value = std::stod(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

inline bool isInt(const std::string& str, int64_t& value) {
    try {
        size_t pos;
        value = std::stoll(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

inline bool isBool(const std::string& str, bool& value) {
    if (str == "true" || str == "1") {
        value = true;
        return true;
    } else if (str == "false" || str == "0") {
        value = false;
        return true;
    }
    return false;
}

} // anonymous namespace

#if __has_include(<nlohmann/json.hpp>)

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    nlohmann::json jsonValue;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            jsonValue = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonValue.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            jsonValue = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonValue.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            jsonValue = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonValue.push_back(item);
            }
        }
    }, value);
    
    return jsonValue;
}

bool Config::jsonToConfigValue(const nlohmann::json& json, ConfigValue& value) {
    if (json.is_number_integer()) {
        int64_t intVal = json.get<int64_t>();
        if (intVal >= std::numeric_limits<int>::min() && intVal <= std::numeric_limits<int>::max()) {
            value = static_cast<int>(intVal);
        } else {
            value = intVal;
        }
        return true;
    } else if (json.is_number_float()) {
        double doubleVal = json.get<double>();
        if (std::floor(doubleVal) == doubleVal) {
            int64_t intVal = static_cast<int64_t>(doubleVal);
            if (intVal >= std::numeric_limits<int>::min() && intVal <= std::numeric_limits<int>::max()) {
                value = static_cast<int>(intVal);
            } else {
                value = intVal;
            }
            return true;
        }
        value = doubleVal;
        return true;
    } else if (json.is_boolean()) {
        value = json.get<bool>();
        return true;
    } else if (json.is_string()) {
        value = json.get<std::string>();
        return true;
    } else if (json.is_array()) {
        if (!json.empty()) {
            const auto& first = json[0];
            if (first.is_number_integer()) {
                std::vector<int> intArray;
                for (const auto& item : json) {
                    intArray.push_back(item.get<int>());
                }
                value = intArray;
                return true;
            } else if (first.is_number_float()) {
                std::vector<double> doubleArray;
                for (const auto& item : json) {
                    doubleArray.push_back(item.get<double>());
                }
                value = doubleArray;
                return true;
            } else if (first.is_string()) {
                std::vector<std::string> stringArray;
                for (const auto& item : json) {
                    stringArray.push_back(item.get<std::string>());
                }
                value = stringArray;
                return true;
            }
        }
    }
    return false;
}

bool Config::jsonToConfigEntry(const nlohmann::json& json, ConfigEntry& entry) {
    if (!json.is_object()) {
        return false;
    }
    
    for (const auto& item : json.items()) {
        if (item.key() == "__key__") {
            entry.key = item.value().get<std::string>();
        } else if (item.key() == "__value__") {
            if (jsonToConfigValue(item.value(), entry.value)) {
                if (json.contains("__source__")) {
                    entry.source = static_cast<ConfigSource>(json["__source__"].get<int>());
                }
                if (json.contains("__description__")) {
                    entry.description = json["__description__"].get<std::string>();
                }
                return true;
            }
        }
    }
    return false;
}

void Config::configValueToJson(const ConfigValue& value, nlohmann::json& json) const {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            json = nlohmann::json::array();
            for (const auto& item : arg) {
                json.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json = nlohmann::json::array();
            for (const auto& item : arg) {
                json.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json = nlohmann::json::array();
            for (const auto& item : arg) {
                json.push_back(item);
            }
        }
    }, value);
}

void Config::configEntryToJson(const ConfigEntry& entry, nlohmann::json& json) const {
    json["__key__"] = entry.key;
    json["__value__"] = configValueToJson(entry.value);
    json["__source__"] = static_cast<int>(entry.source);
    json["__description__"] = entry.description;
}

#endif

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
