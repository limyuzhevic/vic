#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    // Enhanced with validation for key format and type checking
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Basic validation of key format
            if (!isValidConfigKey(key)) {
                throw std::runtime_error("Invalid configuration key: '" + key + "' at line " + std::to_string(lineNumber));
            }
            
            // Basic type validation based on key name
            try {
                validateConfigKeyType(key, value);
            } catch (const std::exception& e) {
                throw std::runtime_error("Configuration validation error at line " + std::to_string(lineNumber) + ": " + std::string(e.what()));
            }
            
            set(key, value, ConfigSource::File);
            
            if (!std::string(key).empty()) {
                NLM_LOG_DEBUG("Loaded config: " + key + " = " + value);
            }
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
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
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
}

bool Config::isValidConfigKey(const std::string& key) {
    // Validate configuration key format
    if (key.empty()) {
        throw std::runtime_error("Empty configuration key");
    }
    
    // Check for invalid characters
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
            throw std::runtime_error("Invalid character in configuration key: '" + std::string(1, c) + "'");
        }
    }
    
    // Check for key length (reasonable limit)
    if (key.length() > 100) {
        throw std::runtime_error("Configuration key too long (max 100 characters): '" + key + "'");
    }
    
    return true;
}

void Config::validateConfigKeyType(const std::string& key, const std::string& value) {
    // Validate configuration value type based on key name
    // This provides basic type checking for common configuration patterns
    
    std::string lowerKey = toLower(key);
    
    // Integer types
    if (lowerKey.find("count") != std::string::npos || 
        lowerKey.find("size") != std::string::npos ||
        lowerKey.find("steps") != std::string::npos ||
        lowerKey.find("iterations") != std::string::npos ||
        lowerKey.find("delay") != std::string::npos ||
        lowerKey.find("refractory") != std::string::npos) {
        // Try to parse as integer
        try {
            size_t pos;
            long val = std::stol(value, &pos);
            if (pos != value.length()) {
                throw std::runtime_error("Invalid integer format for key: '" + key + "'");
            }
            // Check reasonable bounds
            if (val < 0) {
                throw std::runtime_error("Negative value not allowed for key: '" + key + "'");
            }
            if (val > 1000000000) {
                throw std::runtime_error("Value too large for key: '" + key + "'");
            }
        } catch (const std::invalid_argument&) {
            throw std::runtime_error("Invalid integer value for key: '" + key + "' = '" + value + "'");
        } catch (const std::out_of_range&) {
            throw std::runtime_error("Integer value out of range for key: '" + key + "'");
        }
    }
    
    // Float/double types
    else if (lowerKey.find("rate") != std::string::npos || 
             lowerKey.find("weight") != std::string::npos ||
             lowerKey.find("prob") != std::string::npos ||
             lowerKey.find("potential") != std::string::npos ||
             lowerKey.find("conductance") != std::string::npos ||
             lowerKey.find("amplitude") != std::string::npos ||
             lowerKey.find("threshold") != std::string::npos ||
             lowerKey.find("delay") != std::string::npos ||
             lowerKey.find("time") != std::string::npos) {
        // Try to parse as float
        try {
            size_t pos;
            double val = std::stod(value, &pos);
            if (pos != value.length()) {
                throw std::runtime_error("Invalid float format for key: '" + key + "'");
            }
            // Check reasonable bounds
            if (val < -1000.0) {
                throw std::runtime_error("Value too negative for key: '" + key + "'");
            }
            if (val > 1000.0) {
                throw std::runtime_error("Value too large for key: '" + key + "'");
            }
        } catch (const std::invalid_argument&) {
            throw std::runtime_error("Invalid float value for key: '" + key + "' = '" + value + "'");
        } catch (const std::out_of_range&) {
            throw std::runtime_error("Float value out of range for key: '" + key + "'");
        }
    }
    
    // Boolean types
    else if (lowerKey.find("enable") != std::string::npos || 
             lowerKey.find("enabled") != std::string::npos ||
             lowerKey.find("active") != std::string::npos ||
             lowerKey.find("debug") != std::string::npos ||
             lowerKey.find("verbose") != std::string::npos ||
             lowerKey.find("quiet") != std::string::npos) {
        // Check for boolean values
        std::string lowerValue = toLower(value);
        if (lowerValue != "true" && lowerValue != "false" && 
            lowerValue != "1" && lowerValue != "0" &&
            lowerValue != "on" && lowerValue != "off") {
            throw std::runtime_error("Invalid boolean value for key: '" + key + "' = '" + value + "'");
        }
    }
    
    // String types (passthrough for other keys)
}

std::string Config::getDefaultConfigFile() const {
    return "configs/default.cfg";
}

bool Config::validateRequiredKeys() const {
    // Validate that all required configuration keys are present
    // This is a basic implementation - more sophisticated schema validation
    // can be added in Phase 2
    
    // Check for essential keys that should always be present
    std::vector<std::string> essentialKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count",
        "connection_probability"
    };
    
    std::string missingKeys;
    for (const auto& essentialKey : essentialKeys) {
        if (!has(essentialKey)) {
            if (!missingKeys.empty()) missingKeys += ", ";
            missingKeys += essentialKey;
        }
    }
    
    if (!missingKeys.empty()) {
        NLM_LOG_WARNING("Missing required configuration keys: " + missingKeys);
        return false;
    }
    
    return true;
}

void Config::validateAll() const {
    // Comprehensive configuration validation
    if (!validateRequiredKeys()) {
        throw std::runtime_error("Configuration validation failed: missing required keys");
    }
    
    // Additional validation can be added here
    // Check for value ranges, dependencies, etc.
}
