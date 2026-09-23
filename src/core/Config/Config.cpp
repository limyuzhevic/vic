#include "Config.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>

namespace nlm {

// TOML parsing utilities
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// TOML parser for simple key=value pairs
static bool parseTOML(const std::string& content, std::vector<ConfigEntry>& entries) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos && pos > 0) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Parse value based on type
            ConfigValue configValue;
            bool parsed = false;
            
            // Try different types in order of preference
            try {
                // Try boolean
                if (value == "true" || value == "false") {
                    configValue = (value == "true");
                    parsed = true;
                }
                // Try integer
                else {
                    size_t posInt;
                    int intValue = std::stoi(value, &posInt);
                    if (posInt == value.size()) {
                        configValue = intValue;
                        parsed = true;
                    }
                }
            } catch (...) {
                // If parsing fails, try string
                configValue = value;
                parsed = true;
            }
            
            if (parsed) {
                ConfigEntry entry(key, configValue, ConfigSource::File);
                // Add description based on key
                if (key == "simulation_timestep") {
                    entry.description = "Time step in milliseconds for simulation";
                } else if (key == "neuron_count") {
                    entry.description = "Total number of neurons in the simulation";
                } else if (key == "enable_event_driven") {
                    entry.description = "Enable event-driven spike processing";
                } else if (key == "enable_multithreading") {
                    entry.description = "Enable parallel processing";
                } else {
                    entry.description = "Configuration parameter";
                }
                entries.push_back(entry);
            }
        }
    }
    
    return true;
}

// Additional template implementations

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return toLower(e.key) == toLower(key); });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        if constexpr (std::is_same_v<T, int>) {
            return std::get<int>(it->value);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::get<int64_t>(it->value);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::get<double>(it->value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return std::get<bool>(it->value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return std::get<std::string>(it->value);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return std::get<std::vector<int>>(it->value);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return std::get<std::vector<double>>(it->value);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return std::get<std::vector<std::string>>(it->value);
        }
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value.has_value()) {
        return *value;
    }
    return defaultValue;
}

// Implementation of other methods
bool Config::has(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return toLower(e.key) == toLower(key); });
    return it != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return toLower(e.key) == toLower(key); });
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it);
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
    std::stringstream ss;
    ss << "Configuration Summary:" << std::endl;
    ss << "Total parameters: " << pImpl->entries.size() << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        ss << "  " << entry.key << " (source: ";
        switch (entry.source) {
            case ConfigSource::Default: ss << "Default" << std::endl; break;
            case ConfigSource::File: ss << "File" << std::endl; break;
            case ConfigSource::CommandLine: ss << "CommandLine" << std::endl; break;
            case ConfigSource::Runtime: ss << "Runtime" << std::endl; break;
        }
        ss << ") = ";
        
        // Try to print the value
        try {
            if (std::holds_alternative<int>(entry.value)) {
                ss << std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                ss << std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                ss << std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                ss << (std::get<bool>(entry.value) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(entry.value)) {
                ss << "\"" << std::get<std::string>(entry.value) << "\"";
            } else {
                ss << "[complex]";
            }
        } catch (const std::bad_variant_access&) {
            ss << "[invalid]";
        }
        ss << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm
