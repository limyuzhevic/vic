#include "Config.hpp"
#include "../Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
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
    // Implement proper JSON parser using nlohmann/json
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        clear();
        
        // Process JSON data
        if (jsonData.is_object()) {
            for (const auto& [key, value] : jsonData.items()) {
                setJSONValue(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to parse JSON config file ") + filepath + ": " + e.what());
        return false;
    }
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

// Helper function to convert JSON value to ConfigValue
static ConfigValue jsonValueToConfigValue(const nlohmann::json& jsonValue) {
    if (jsonValue.is_number_integer()) {
        return jsonValue.get<int>();
    } else if (jsonValue.is_number()) {
        return jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>();
    } else if (jsonValue.is_string()) {
        return jsonValue.get<std::string>();
    } else if (jsonValue.is_array()) {
        // Handle arrays
        if (jsonValue.empty()) return std::vector<std::string>{};
        
        // Check array element type
        if (jsonValue[0].is_number_integer()) {
            std::vector<int> arr;
            arr.reserve(jsonValue.size());
            for (const auto& item : jsonValue) {
                arr.push_back(item.get<int>());
            }
            return arr;
        } else if (jsonValue[0].is_number()) {
            std::vector<double> arr;
            arr.reserve(jsonValue.size());
            for (const auto& item : jsonValue) {
                arr.push_back(item.get<double>());
            }
            return arr;
        } else {
            std::vector<std::string> arr;
            arr.reserve(jsonValue.size());
            for (const auto& item : jsonValue) {
                arr.push_back(item.get<std::string>());
            }
            return arr;
        }
    } else {
        // Unknown type, convert to string
        return jsonValue.dump();
    }
}

// Helper function to set JSON value
void setJSONValue(const std::string& key, const nlohmann::json& jsonValue, ConfigSource source) {
    try {
        ConfigValue configValue = jsonValueToConfigValue(jsonValue);
        set(key, configValue, source);
    } catch (const std::exception& e) {
        NLM_LOG_WARNING(std::string("Failed to set config key ") + key + ": " + e.what());
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    // Implement proper JSON save using nlohmann/json
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        
        // Convert all config entries to JSON
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = configValueToJSON(entry.value);
        }
        
        // Write JSON to file
        file << jsonData.dump(2);  // Pretty print with 2-space indentation
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to save JSON config file ") + filepath + ": " + e.what());
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

// Helper function to convert ConfigValue to JSON value
static nlohmann::json configValueToJSON(const ConfigValue& configValue) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return nlohmann::json(arg);
        } else {
            // Fallback for unknown types
            return nlohmann::json("# unknown type");
        }
    }, configValue);
}

std::string Config::summary() const {
    // Convert to JSON and back to string for pretty printing
    nlohmann::json jsonData;
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = configValueToJSON(entry.value);
        // Add description as a comment (this is tricky in JSON, so we'll handle it separately)
    }
    
    // Return pretty JSON summary
    return "Configuration (" + std::to_string(pImpl->entries.size()) + " entries):\n" + 
           jsonData.dump(2) + "\n";
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
