#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <fmt/core.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Configuration schema for validation
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    namespace fs = std::filesystem;
    
    // Check if file exists
    if (!fs::exists(filepath)) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    // Determine format based on extension
    std::string ext = fs::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    try {
        if (ext == ".json") {
            return loadFromJSON(filepath);
        } else if (ext == ".yaml" || ext == ".yml") {
            return loadFromYAML(filepath);
        } else {
            // Default to simple key=value format for backward compatibility
            return loadFromSimpleFormat(filepath);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load config from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    // TODO: Integrate with nlohmann/json library
    // For now, keep backward compatibility
    NLM_LOG_WARNING("JSON parsing not implemented yet, falling back to simple format");
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromYAML(const std::string& filepath) {
    // TODO: Integrate with YAML library
    // For now, keep backward compatibility
    NLM_LOG_WARNING("YAML parsing not implemented yet, falling back to simple format");
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
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
        } else {
            NLM_LOG_WARNING(fmt::format("Invalid config line {} in {}: {}", 
                                       lineNumber, filepath, line));
        }
    }
    
    NLM_LOG_INFO(fmt::format("Loaded {} configuration entries from {}", 
                           pImpl->entries.size(), filepath));
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc < 2) {
        return true; // No command line arguments
    }
    
    bool success = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.find("--") == 0) {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!setFromString(key, value, ConfigSource::CommandLine)) {
                    NLM_LOG_ERROR("Invalid command line argument: " + arg);
                    success = false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (!setFromString(key, value, ConfigSource::CommandLine)) {
                NLM_LOG_ERROR("Invalid command line argument: -" + key + " " + value);
                success = false;
            }
        }
        // Handle boolean flags
        else if (arg.find("--") == 0) {
            std::string key = arg.substr(2);
            set(key, true, ConfigSource::CommandLine);
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n";
    file << "# Format: key = value\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Format value based on type
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
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
        NLM_LOG_WARNING("Type mismatch for config key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    } else {
        NLM_LOG_DEBUG("Using default value for config key: " + key);
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate before setting
    if (!validateKey(key) || !validateValue(value)) {
        throw std::invalid_argument("Invalid key or value for configuration");
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated config key: " + key);
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Added new config key: " + key);
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
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it, pImpl->entries.end());
        NLM_LOG_DEBUG("Removed config key: " + key);
    }
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
    NLM_LOG_DEBUG("Cleared all configuration entries");
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

// New methods for enhanced configuration system

bool Config::validateKey(const std::string& key) const {
    // Basic validation - key should not be empty and should contain only valid characters
    if (key.empty()) {
        return false;
    }
    // Allow alphanumeric, underscore, and hyphen
    return std::all_of(key.begin(), key.end(), 
        [](char c) { return std::isalnum(c) || c == '_' || c == '-'; });
}

bool Config::validateValue(const ConfigValue& value) const {
    // Basic validation - check if variant holds a valid type
    return std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            // String validation - reasonable length
            return arg.length() < 1024;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            // Vector validation - reasonable size
            return arg.size() < 1000;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return arg.size() < 1000;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return arg.size() < 100;
        } else {
            return true; // For primitive types
        }
    }, value);
}

bool Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Try to determine type and set appropriately
    // This is a simplified type detection - in production you'd want more sophisticated parsing
    
    if (valueStr == "true" || valueStr == "false") {
        bool boolVal = (valueStr == "true");
        set(key, boolVal, source);
        return true;
    }
    
    try {
        // Try to parse as integer
        size_t pos;
        int intVal = std::stoi(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, intVal, source);
            return true;
        }
        
        // Try to parse as double
        double doubleVal = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, doubleVal, source);
            return true;
        }
        
        // Fall back to string
        set(key, valueStr, source);
        return true;
    } catch (const std::exception&) {
        // If parsing fails, treat as string
        set(key, valueStr, source);
        return true;
    }
}

// Explicit template instantiations for common types
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
