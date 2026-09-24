#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// Include nlohmann/json for JSON parsing
// Note: This assumes nlohmann/json is available in the system
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if file has JSON extension
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return parseJsonFromFile(filepath);
    }
    
    // Otherwise try simple key=value format for backward compatibility
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

bool Config::parseJsonFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Validate JSON structure
        if (!jsonData.is_object()) {
            return false;
        }
        
        // Clear existing entries
        clear();
        
        // Parse each entry
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            ConfigEntry entry;
            entry.key = key;
            entry.source = ConfigSource::File;
            entry.description = "Loaded from JSON file";
            
            if (fromJson(it.value(), entry)) {
                pImpl->entries.push_back(entry);
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromJson(const std::string& jsonStr) {
    clear();
    
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonStr);
        
        if (!jsonData.is_object()) {
            return false;
        }
        
        // Parse each entry
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            ConfigEntry entry;
            entry.key = key;
            entry.source = ConfigSource::File;
            entry.description = "Loaded from JSON string";
            
            if (fromJson(it.value(), entry)) {
                pImpl->entries.push_back(entry);
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::parseJson(const std::string& jsonStr) {
    return loadFromJson(jsonStr);
}

bool Config::saveToFile(const std::string& filepath) const {
    // Always save as JSON for better compatibility
    return saveToJsonFile(filepath);
}

bool Config::saveToJsonFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData = toJson();
        file << jsonData.dump(4);  // Pretty print with 4-space indentation
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string Config::toJson() const {
    nlohmann::json jsonData = nlohmann::json::object();
    
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = jsonValueToJson(entry.value);
    }
    
    return jsonData.dump();
}

bool Config::fromJson(const std::string& jsonStr, ConfigEntry& entry) {
    try {
        nlohmann::json jsonValue = nlohmann::json::parse(jsonStr);
        return jsonToConfigValue(jsonValue, entry.value);
    } catch (const std::exception&) {
        return false;
    }
}

nlohmann::json Config::configToJson(const ConfigEntry& entry) {
    return jsonValueToJson(entry.value);
}

bool Config::jsonToConfig(const nlohmann::json& json, ConfigEntry& entry) {
    entry.value = jsonToConfigValue(json);
    return true;
}

nlohmann::json Config::jsonValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return arg;
        } else {
            return nlohmann::json();
        }
    }, value);
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& json) {
    ConfigValue result;
    
    try {
        if (json.is_number_integer()) {
            result = json.get<int>();
        } else if (json.is_number_unsigned()) {
            result = json.get<uint64_t>();
        } else if (json.is_number_float()) {
            result = json.get<double>();
        } else if (json.is_boolean()) {
            result = json.get<bool>();
        } else if (json.is_string()) {
            result = json.get<std::string>();
        } else if (json.is_array()) {
            // Determine array type
            bool isIntArray = std::all_of(json.begin(), json.end(), 
                [](const auto& item) { return item.is_number_integer(); });
            bool isDoubleArray = std::all_of(json.begin(), json.end(), 
                [](const auto& item) { return item.is_number_float(); });
            bool isStringArray = std::all_of(json.begin(), json.end(), 
                [](const auto& item) { return item.is_string(); });
            
            if (isIntArray) {
                std::vector<int> vec;
                vec.reserve(json.size());
                for (const auto& item : json) {
                    vec.push_back(item.get<int>());
                }
                result = vec;
            } else if (isDoubleArray) {
                std::vector<double> vec;
                vec.reserve(json.size());
                for (const auto& item : json) {
                    vec.push_back(item.get<double>());
                }
                result = vec;
            } else if (isStringArray) {
                std::vector<std::string> vec;
                vec.reserve(json.size());
                for (const auto& item : json) {
                    vec.push_back(item.get<std::string>());
                }
                result = vec;
            } else {
                // Mixed type array - store as string
                result = json.dump();
            }
        } else {
            // Null or other types - store as empty string
            result = std::string();
        }
    } catch (const std::exception&) {
        result = std::string();
    }
    
    return result;
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
        pImpl->entries.emplace_back(key, value, source);
    }
}

template<typename T>
void Config::setTyped(const std::string& key, const T& value, ConfigSource source) {
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
    validationErrors.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        oss << jsonValueToString(entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

bool Config::isValidJson(const std::string& jsonStr) {
    try {
        nlohmann::json::parse(jsonStr);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string Config::toJson(const ConfigEntry& entry) {
    return jsonValueToString(entry.value);
}

bool Config::stringToJsonValue(const std::string& str, ConfigValue& value) {
    // Try to parse as different types
    try {
        // Try boolean
        if (str == "true" || str == "false") {
            value = (str == "true");
            return true;
        }
        
        // Try integer
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            value = intVal;
            return true;
        }
        
        // Try double
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            value = doubleVal;
            return true;
        }
        
        // Try string
        value = str;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string Config::jsonValueToString(const ConfigValue& value) {
    return std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        } else {
            return std::to_string(arg);
        }
    }, value);
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
