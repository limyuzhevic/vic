#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    NLM_LOG_DEBUG("Attempting to load config from: " + filepath);
    
    // Try JSON/YAML parser first (Phase 2), fallback to simple key=value format
    bool loaded = false;
    
// Try YAML parser first
    if (loadFromYAML(filepath)) {
        loaded = true;
        NLM_LOG_INFO("Successfully loaded config using YAML parser: " + filepath);
    } else if (loadFromJSON(filepath)) {
        loaded = true;
        NLM_LOG_INFO("Successfully loaded config using JSON parser: " + filepath);
    } else {
        // Fallback to simple key=value format
        NLM_LOG_DEBUG("Falling back to simple key=value format parser");
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file: " + filepath);
            return false;
        }
        
        // Clear existing entries before loading new ones
        clear();
        
        std::string line;
        int lineNum = 0;
        while (std::getline(file, line)) {
            ++lineNum;
            line = trim(line);
            
            // Skip empty lines and comments
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
            } else {
                NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": " + line);
            }
        }
        
        return true;
    }
    
    if (!loaded) {
        NLM_LOG_ERROR("Failed to load config file using any parser: " + filepath);
        return false;
    }
}

if (!loaded) {
        NLM_LOG_ERROR("Failed to load config file using any parser: " + filepath);
        return false;
    }
}

// Load configuration from JSON file
bool Config::loadFromJSON(const std::string& filepath) {
    NLM_LOG_DEBUG("Attempting to load config from JSON file: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON config file: " + filepath);
        return false;
    }
    
    try {
        // Parse JSON
        nlohmann::json j;
        file >> j;
        
        // Clear existing entries before loading new ones
        clear();
        
        // Convert JSON to Config entries
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                
                // Handle different JSON value types
                const nlohmann::json& value = it.value();
                if (value.is_string()) {
                    set(key, value.get<std::string>(), ConfigSource::File);
                } else if (value.is_number_integer()) {
                    // Check if it fits in int64_t
                    try {
                        int64_t intVal = value.get<int64_t>();
                        set(key, intVal, ConfigSource::File);
                    } catch (...) {
                        set(key, value.get<double>(), ConfigSource::File);
                    }
                } else if (value.is_number_float()) {
                    set(key, value.get<double>(), ConfigSource::File);
                } else if (value.is_boolean()) {
                    set(key, value.get<bool>(), ConfigSource::File);
                } else if (value.is_array()) {
                    // Handle arrays
                    std::vector<std::string> strArray;
                    for (const auto& item : value) {
                        strArray.push_back(item.get<std::string>());
                    }
                    set(key, strArray, ConfigSource::File);
                } else if (value.is_object()) {
                    // For nested objects, convert to string representation
                    set(key, value.dump(), ConfigSource::File);
                }
            }
            
            NLM_LOG_INFO("Successfully loaded JSON config from: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Invalid JSON format: expected object at root");
            return false;
        }
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in " + filepath + ": " + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON file " + filepath + ": " + e.what());
        return false;
    }
}

// Load configuration from YAML file
bool Config::loadFromYAML(const std::string& filepath) {
    NLM_LOG_DEBUG("Attempting to load config from YAML file: " + filepath);
    
    try {
        // Parse YAML
        YAML::Loader loader;
        YAML::Node doc = loader.LoadFile(filepath);
        
        if (!doc.IsMap()) {
            NLM_LOG_ERROR("Invalid YAML format: expected map at root");
            return false;
        }
        
        // Clear existing entries before loading new ones
        clear();
        
        // Convert YAML to Config entries
        for (auto it = doc.begin(); it != doc.end(); ++it) {
            std::string key = it->first.as<std::string>();
            
            // Handle different YAML value types
            const YAML::Node& value = it->second;
            if (value.IsScalar()) {
                // Check if scalar is boolean
                if (value.IsBoolean()) {
                    set(key, value.as<bool>(), ConfigSource::File);
                } else if (value.IsInteger()) {
                    set(key, value.as<int64_t>(), ConfigSource::File);
                } else if (value.IsFloat()) {
                    set(key, value.as<double>(), ConfigSource::File);
                } else {
                    // String scalar
                    set(key, value.as<std::string>(), ConfigSource::File);
                }
            } else if (value.IsSequence()) {
                // Handle sequences
                std::vector<std::string> strArray;
                for (const auto& item : value) {
                    if (item.IsScalar()) {
                        strArray.push_back(item.as<std::string>());
                    }
                }
                set(key, strArray, ConfigSource::File);
            } else if (value.IsMap()) {
                // For nested objects, convert to string representation
                std::stringstream ss;
                ss << value;
                set(key, ss.str(), ConfigSource::File);
            }
        }
        
        NLM_LOG_INFO("Successfully loaded YAML config from: " + filepath);
        return true;
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("YAML parse error in " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing YAML file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert config value to string
        std::string valueStr;
        std::visit([&valueStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = "\"" + arg + "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStr = arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                valueStr = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) valueStr += ", ";
                    valueStr += std::to_string(arg[i]);
                }
                valueStr += "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                valueStr = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) valueStr += ", ";
                    valueStr += std::to_string(arg[i]);
                }
                valueStr += "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                valueStr = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) valueStr += ", ";
                    valueStr += "\"" + arg[i] + "\"";
                }
                valueStr += "]";
            } else {
                // For int, int64_t, double
                std::ostringstream oss;
                oss << arg;
                valueStr = oss.str();
            }
        }, entry.value);
        
        file << entry.key << " = " << valueStr << "\n\n";
    }
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
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
