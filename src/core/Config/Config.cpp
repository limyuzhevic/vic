#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <iostream>
#include <map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string lastLoadedFile;
    nlohmann::json cachedJson;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open config file: " + filepath);
            return false;
        }
        
        // Try to parse as JSON first
        try {
            auto json = nlohmann::json::parse(file);
            pImpl->cachedJson = json;
            pImpl->lastLoadedFile = filepath;
            
            // Clear existing entries
            pImpl->entries.clear();
            
            // Convert JSON to Config entries
            convertJsonToConfig(json, ConfigSource::File);
            
            NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
            return true;
        } catch (const nlohmann::json::parse_error&) {
            // If JSON parsing fails, fall back to simple key=value format
            file.clear();
            file.seekg(0);
        }
        
        // Parse as simple key=value format (backwards compatibility)
        std::string line;
        size_t lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
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
                NLM_LOG_DEBUG("Config loaded: " + key + " = " + value + " (from " + filepath + ":" + std::to_string(lineNumber) + ")");
            }
        }
        
        pImpl->lastLoadedFile = filepath;
        NLM_LOG_INFO("Loaded configuration from simple file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

void Config::convertJsonToConfig(const nlohmann::json& json, ConfigSource source) {
    if (!json.is_object()) {
        return;
    }
    
    for (const auto& [key, value] : json.items()) {
        if (value.is_string()) {
            set(key, value.get<std::string>(), source);
        } else if (value.is_number_integer()) {
            set(key, value.get<int64_t>(), source);
        } else if (value.is_number_float()) {
            set(key, value.get<double>(), source);
        } else if (value.is_boolean()) {
            set(key, value.get<bool>(), source);
        } else if (value.is_array()) {
            // Convert arrays to vector format (store as string for now, can be improved)
            std::string arrayStr = "[";
            for (size_t i = 0; i < value.size(); ++i) {
                if (i > 0) arrayStr += ",";
                if (value[i].is_string()) {
                    arrayStr += "\"" + value[i].get<std::string>() + "\"";
                } else {
                    arrayStr += std::to_string(value[i].get<double>());
                }
            }
            arrayStr += "]";
            set(key, arrayStr, source);
        }
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                    NLM_LOG_DEBUG("Config from args: " + key + " = " + value);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_DEBUG("Config from args: " + key + " = " + value);
            }
        }
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing command line args: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
            return false;
        }
        
        // Try to save as JSON first if we have cached JSON
        if (!pImpl->cachedJson.empty()) {
            file << pImpl->cachedJson.dump(2);
            NLM_LOG_INFO("Saved configuration to JSON file: " + filepath);
            return true;
        }
        
        // Fall back to simple key=value format for backwards compatibility
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            // Convert config value to string
            std::string valueStr;
            std::visit([&valueStr](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    valueStr = "\"" + arg + "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    valueStr = "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) valueStr += ",";
                        valueStr += std::to_string(arg[i]);
                    }
                    valueStr += "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    valueStr = "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) valueStr += ",";
                        valueStr += std::to_string(arg[i]);
                    }
                    valueStr += "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    valueStr = "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) valueStr += ",";
                        valueStr += "\"" + arg[i] + "\"";
                    }
                    valueStr += "]";
                } else {
                    valueStr = std::to_string(arg);
                }
            }, entry.value);
            
            file << entry.key << " = " << valueStr << "\n";
        }
        
        NLM_LOG_INFO("Saved configuration to simple file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving config file " + filepath + ": " + std::string(e.what()));
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
        NLM_LOG_WARNING("Type mismatch for config key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    }
    NLM_LOG_DEBUG("Using default value for config key: " + key);
    return defaultValue;
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
    
    // Invalidate cached JSON
    pImpl->cachedJson = nlohmann::json();
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
    
    // Invalidate cached JSON
    pImpl->cachedJson = nlohmann::json();
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
    pImpl->cachedJson = nlohmann::json();
    pImpl->lastLoadedFile.clear();
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
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ",";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ",";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ",";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
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
