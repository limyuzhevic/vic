#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Improved file loading with better error handling and validation
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Could not open config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + 
                          ": missing '=' in: " + line);
            hasErrors = true;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": empty key");
            hasErrors = true;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '\"' && value.back() == '\"') ||
             (value.front() == \'\' && value.back() == \'\'))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Validate value (basic check for empty)
        if (value.empty()) {
            NLM_LOG_WARNING("Config key '" + key + "' has empty value at line " + std::to_string(lineNumber));
            // Allow empty values for some keys (e.g., string properties)
        }
        
        try {
            set(key, value, ConfigSource::File);
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Error setting config key '" + key + "' at line " + std::to_string(lineNumber) + ": " + e.what());
            hasErrors = true;
        }
    }
    
    if (hasErrors) {
        NLM_LOG_WARNING("Config file loaded with errors from: " + filepath);
    } else {
        NLM_LOG_INFO("Loaded configuration from: " + filepath);
    }
    
    return !hasErrors;  // Return false if there were parsing errors
}

bool Config::loadFromArgs(int argc, char** argv) {
    int processedArgs = 0;
    bool hasErrors = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        processedArgs++;
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (!validateConfigKey(key)) {
                    hasErrors = true;
                    continue;
                }
                if (!validateConfigValue(key, value)) {
                    hasErrors = true;  // Warning logged by validate function
                }
                
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error setting config from command line " + key + "=" + value + ": " + e.what());
                    hasErrors = true;
                }
            } else {
                NLM_LOG_ERROR("Invalid command line argument (missing '='): " + arg);
                hasErrors = true;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                processedArgs++;
                
                if (!validateConfigKey(key)) {
                    hasErrors = true;
                    continue;
                }
                if (!validateConfigValue(key, value)) {
                    hasErrors = true;
                }
                
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error setting config from command line " + key + "=" + value + ": " + e.what());
                    hasErrors = true;
                }
            } else {
                NLM_LOG_ERROR("Missing value for command line argument: " + arg);
                hasErrors = true;
            }
        } else {
            NLM_LOG_ERROR("Unknown command line argument: " + arg);
            hasErrors = true;
        }
    }
    
    if (hasErrors) {
        NLM_LOG_WARNING("Command line configuration loaded with errors");
    } else {
        NLM_LOG_INFO("Command line configuration loaded successfully (" + std::to_string(processedArgs) + " arguments processed)");
    }
    
    return !hasErrors;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Could not save config file: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File (Phase 2 Real Neural Computation)\n";
    file << "# Generated by NLM Phase 2\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    file << "# Note: This file contains the current configuration state.\n";
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
    // Validate configuration keys for Phase 2
    static const std::vector<std::string> validKeys = {
        "random_seed", "simulation_timestep", "neuron_count", "region_count",
        "connection_probability", "stdp_ltp_weight", "stdp_ltd_weight", "stdp_tau",
        "synaptogenesis_rate", "pruning_rate"
    };
    
    // Check if key is valid (optional validation)
    bool isValidKey = std::any_of(validKeys.begin(), validKeys.end(),
        [&key](const std::string& validKey) { return validKey == key; });
    
    if (!isValidKey) {
        NLM_LOG_WARNING("Setting unknown config key: " + key + ". This may indicate missing configuration validation.");
    }
    
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

// Additional convenience methods for common use cases

// Get configuration as string with default
std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    auto val = get<std::string>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Get configuration as integer with default
int Config::getInt(const std::string& key, int defaultValue) const {
    auto val = get<int>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Get configuration as float with default
float Config::getFloat(const std::string& key, float defaultValue) const {
    auto val = get<float>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Get configuration as boolean with default
bool Config::getBool(const std::string& key, bool defaultValue) const {
    auto val = get<bool>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Helper functions for enhanced configuration management

namespace {
    // Helper function to validate and parse command line arguments
    bool validateConfigKey(const std::string& key) {
        // Basic validation - can be extended with more strict validation
        if (key.empty()) {
            NLM_LOG_WARNING("Empty config key provided");
            return false;
        }
        if (key.find('=') != std::string::npos) {
            NLM_LOG_WARNING("Config key contains '=' character: " + key);
            return false;
        }
        if (key.find(' ') != std::string::npos) {
            NLM_LOG_WARNING("Config key contains spaces: " + key);
            return false;
        }
        return true;
    }
    
    // Helper function to validate configuration values
    bool validateConfigValue(const std::string& key, const std::string& value) {
        // Basic validation - can be extended with type-specific validation
        if (value.empty() && key != "log_filename") { // Allow empty filename
            NLM_LOG_DEBUG("Empty value provided for key: " + key);
        }
        return true;
    }
}

// Helper to get config as specific type with validation
template<typename T>
T Config::getValidated(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        NLM_LOG_DEBUG("Using default value for missing config key: " + key);
        return defaultValue;
    }
    return *val;
}

// Helper to check if config value matches expected type
bool Config::isType(const std::string& key, ConfigSource minSource) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return static_cast<int>(it->source) >= static_cast<int>(minSource);
}

// Helper to get all config values as strings for easier processing
std::map<std::string, std::string> Config::getAllAsStrings() const {
    std::map<std::string, std::string> stringConfig;
    
    for (const auto& entry : pImpl->entries) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << "[complex value]";
            }
        }, entry.value);
        
        stringConfig[entry.key] = oss.str();
    }
    
    return stringConfig;
}

// Helper to check if config was modified from defaults
bool Config::isModified(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return it->source != ConfigSource::Default;
}

// Helper to get config entries with specific source
std::vector<std::pair<std::string, ConfigValue>> Config::getBySource(ConfigSource source) const {
    std::vector<std::pair<std::string, ConfigValue>> result;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.emplace_back(entry.key, entry.value);
        }
    }
    
    return result;
}

// Helper to validate critical config values
bool Config::validateCriticalValues() const {
    bool allValid = true;
    
    // Check critical parameters with validation ranges
    auto validateRange = [this](const std::string& key, double min, double max, const std::string& name) {
        auto val = get<double>(key);
        if (val.has_value()) {
            if (val.value() < min || val.value() > max) {
                NLM_LOG_WARNING("Invalid value for " + name + " (" + key + "): " + 
                              std::to_string(val.value()) + " outside range [" + 
                              std::to_string(min) + ", " + std::to_string(max) + "]");
                allValid = false;
            }
        }
    };
    
    validateRange("simulation_timestep", 0.0001, 1.0, "simulation timestep");
    validateRange("connection_probability", 0.0, 1.0, "connection probability");
    validateRange("stdp_ltp_weight", 0.0, 1.0, "STDP LTP weight");
    validateRange("stdp_ltd_weight", 0.0, 1.0, "STDP LTD weight");
    validateRange("stdp_tau", 1.0, 100.0, "STDP time constant");
    validateRange("synaptogenesis_rate", 0.0, 1.0, "synaptogenesis rate");
    validateRange("pruning_rate", 0.0, 1.0, "pruning rate");
    
    return allValid;
}

} // namespace nlm
