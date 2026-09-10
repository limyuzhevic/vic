#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if the file exists
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    try {
        json j;
        file >> j;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Convert JSON to ConfigEntry objects
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Handle different JSON types
            if (it->is_string()) {
                set(key, it->get<std::string>(), ConfigSource::File);
            } else if (it->is_number_integer()) {
                set(key, it->get<int>(), ConfigSource::File);
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), ConfigSource::File);
            } else if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::File);
            } else {
                // For complex types, store as string representation
                set(key, it->dump(), ConfigSource::File);
            }
        }
        
        return true;
    } catch (const json::exception& e) {
        // If JSON parsing fails, fall back to simple key=value format
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        // Reset file stream
        file.clear();
        file.seekg(0, std::ios::beg);
    }
    
    // Fallback to simple key=value format for backward compatibility
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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Create JSON object from entries
        json j;
        
        for (const auto& entry : pImpl->entries) {
            // Convert value to appropriate JSON type
            if (std::holds_alternative<std::string>(entry.value)) {
                j[entry.key] = std::get<std::string>(entry.value);
            } else if (std::holds_alternative<int>(entry.value)) {
                j[entry.key] = std::get<int>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                j[entry.key] = std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                j[entry.key] = std::get<bool>(entry.value);
            } else {
                j[entry.key] = "UNSUPPORTED_TYPE";
            }
        }
        
        // Write JSON to file with indentation
        file << j.dump(4);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving config: " << e.what() << std::endl;
        return false;
    }
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

// Set template instantiations (defined inline)
inline void Config::set<std::string>(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

inline void Config::set<int>(const std::string& key, const int& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

inline void Config::set<double>(const std::string& key, const double& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

inline void Config::set<bool>(const std::string& key, const bool& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
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

} // namespace nlm

