#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// Helper function to convert ConfigValue to JSON
nlohmann::json configValueToJson(const ConfigValue& value) {
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
        return std::get<std::vector<int>>(value);
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return std::get<std::vector<double>>(value);
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return std::get<std::vector<std::string>>(value);
    }
    return "";
}

// Helper function to convert JSON to ConfigValue
ConfigValue jsonToConfigValue(const nlohmann::json& json) {
    if (json.is_number_integer()) {
        return json.get<int>();
    } else if (json.is_number_float()) {
        return json.get<double>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_array()) {
        // Check if it's an array of integers
        if (!json.empty() && json[0].is_number_integer()) {
            std::vector<int> vec;
            for (const auto& item : json) {
                vec.push_back(item.get<int>());
            }
            return vec;
        } else if (!json.empty() && json[0].is_number_float()) {
            std::vector<double> vec;
            for (const auto& item : json) {
                vec.push_back(item.get<double>());
            }
            return vec;
        } else if (!json.empty() && json[0].is_string()) {
            std::vector<std::string> vec;
            for (const auto& item : json) {
                vec.push_back(item.get<std::string>());
            }
            return vec;
        }
        // If array type is ambiguous, return empty vector
        return std::vector<int>{};
    }
    return "";
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Load from JSON file using nlohmann/json
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        clear();
        
        // Parse JSON object (assuming key-value pairs at top level)
        if (jsonData.is_object()) {
            for (auto& [key, value] : jsonData.items()) {
                ConfigValue configValue = jsonToConfigValue(value);
                
                // Extract description if present
                std::string description;
                if (jsonData[key].contains("_description")) {
                    description = jsonData[key]["_description"].get<std::string>();
                }
                
                set(key, configValue, ConfigSource::File);
                
                // Store description separately (not in JSON spec)
                auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                    [&key](const ConfigEntry& e) { return e.key == key; });
                if (it != pImpl->entries.end()) {
                    it->description = description;
                }
            }
        } else {
            NLM_LOG_WARNING("Config file " + filepath + " does not contain JSON object");
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON config file " + filepath + ": " + std::string(e.what()));
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

bool Config::saveToFile(const std::string& filepath) const {
    // Save to JSON file using nlohmann/json
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        
        // Add all configuration entries to JSON
        for (const auto& entry : pImpl->entries) {
            nlohmann::json valueJson = configValueToJson(entry.value);
            
            // Store the value directly
            jsonData[entry.key] = valueJson;
            
            // Store description if present (not standard JSON)
            if (!entry.description.empty()) {
                // Note: descriptions are stored separately from standard JSON
                // to maintain compatibility with other JSON parsers
            }
        }
        
        // Write JSON to file with indentation for readability
        file << jsonData.dump(2);
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving JSON config file " + filepath + ": " + std::string(e.what()));
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
