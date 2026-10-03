// Config.cpp - Implementation of the improved configuration system

#include "Config.hpp"
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <fstream>

namespace nlm {

// Forward declaration for pImpl
struct Config::Impl {
    std::unordered_map<std::string, ConfigEntry> entries;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
    
    Impl() = default;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

// Set default values with type safety
void Config::setDefault(const std::string& key, int value, const std::string& description) {
    pImpl->entries[key] = ConfigEntry(key, ConfigInt(value, description), ConfigSource::Default, description);
}

void Config::setDefault(const std::string& key, double value, const std::string& description) {
    pImpl->entries[key] = ConfigEntry(key, ConfigDouble(value, description), ConfigSource::Default, description);
}

void Config::setDefault(const std::string& key, const std::string& value, const std::string& description) {
    pImpl->entries[key] = ConfigEntry(key, ConfigString(value, description), ConfigSource::Default, description);
}

void Config::setDefault(const std::string& key, bool value, const std::string& description) {
    pImpl->entries[key] = ConfigEntry(key, ConfigBool(value, description), ConfigSource::Default, description);
}

void Config::setDefault(const std::string& key, const std::vector<int>& value, const std::string& description) {
    pImpl->entries[key] = ConfigEntry(key, ConfigVectorInt(value, description), ConfigSource::Default, description);
}

void Config::setDefault(const std::string& key, const std::vector<double>& value, const std::string& description) {
    pImpl->entries[key] = ConfigEntry(key, ConfigVectorDouble(value, description), ConfigSource::Default, description);
}

// Override with validation
template<typename T>
bool Config::setValidated(const std::string& key, const T& value,
                          std::function<bool(const ConfigValue&)> validator,
                          ConfigSource source) {
    ConfigValue configValue = value;
    
    // Store validator
    if (validator) {
        pImpl->validators[key] = validator;
    }
    
    // Check if validator passes
    if (validator && !validator(configValue)) {
        return false;
    }
    
    // Update or add entry
    auto it = pImpl->entries.find(key);
    if (it != pImpl->entries.end()) {
        it->second.value = configValue;
        it->second.source = source;
    } else {
        pImpl->entries[key] = ConfigEntry(key, configValue, source, "", validator);
    }
    return true;
}

// Set values
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    setString(key, value, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    setInt(key, value, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    setDouble(key, value, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    setBool(key, value, source);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    setVectorInt(key, value, source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    setVectorDouble(key, value, source);
}

// Type-safe getters with template implementation
template<typename T>
T Config::get(const std::string& key, const T& defaultValue) const {
    auto it = pImpl->entries.find(key);
    if (it != pImpl->entries.end()) {
        const ConfigEntry& entry = it->second;
        if (entry.isValid()) {
            if (std::holds_alternative<T>(entry.value)) {
                return std::get<T>(entry.value);
            } else if constexpr (std::is_same_v<T, int>) {
                if (std::holds_alternative<ConfigInt>(entry.value)) {
                    return std::get<ConfigInt>(entry.value).value;
                }
            } else if constexpr (std::is_same_v<T, double>) {
                if (std::holds_alternative<ConfigDouble>(entry.value)) {
                    return std::get<ConfigDouble>(entry.value).value;
                }
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (std::holds_alternative<ConfigString>(entry.value)) {
                    return std::get<ConfigString>(entry.value).value;
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                if (std::holds_alternative<ConfigBool>(entry.value)) {
                    return std::get<ConfigBool>(entry.value).value;
                }
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                if (std::holds_alternative<ConfigVectorInt>(entry.value)) {
                    return std::get<ConfigVectorInt>(entry.value).value;
                }
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                if (std::holds_alternative<ConfigVectorDouble>(entry.value)) {
                    return std::get<ConfigVectorDouble>(entry.value).value;
                }
            }
        }
    }
    return defaultValue;
}

template<typename T>
std::optional<T> Config::getOptional(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it != pImpl->entries.end()) {
        const ConfigEntry& entry = it->second;
        if (entry.isValid()) {
            if (std::holds_alternative<T>(entry.value)) {
                return std::get<T>(entry.value);
            } else if constexpr (std::is_same_v<T, int>) {
                if (std::holds_alternative<ConfigInt>(entry.value)) {
                    return std::get<ConfigInt>(entry.value).value;
                }
            } else if constexpr (std::is_same_v<T, double>) {
                if (std::holds_alternative<ConfigDouble>(entry.value)) {
                    return std::get<ConfigDouble>(entry.value).value;
                }
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (std::holds_alternative<ConfigString>(entry.value)) {
                    return std::get<ConfigString>(entry.value).value;
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                if (std::holds_alternative<ConfigBool>(entry.value)) {
                    return std::get<ConfigBool>(entry.value).value;
                }
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                if (std::holds_alternative<ConfigVectorInt>(entry.value)) {
                    return std::get<ConfigVectorInt>(entry.value).value;
                }
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                if (std::holds_alternative<ConfigVectorDouble>(entry.value)) {
                    return std::get<ConfigVectorDouble>(entry.value).value;
                }
            }
        }
    }
    return std::nullopt;
}

// Check existence
bool Config::has(const std::string& key) const {
    return pImpl->entries.find(key) != pImpl->entries.end();
}

// Remove key
void Config::remove(const std::string& key) {
    pImpl->entries.erase(key);
    pImpl->validators.erase(key);
}

// Get all keys
std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& pair : pImpl->entries) {
        keys.push_back(pair.first);
    }
    return keys;
}

// Clear all
void Config::clear() {
    pImpl->entries.clear();
    pImpl->validators.clear();
}

// Get configuration summary
std::string Config::summary() const {
    std::stringstream ss;
    ss << "=== NLM Configuration Summary ===" << std::endl;
    
    for (const auto& pair : pImpl->entries) {
        const ConfigEntry& entry = pair.second;
        ss << pair.first << ": ";
        
        switch (entry.source) {
            case ConfigSource::Default: ss << "[DEFAULT] "; break;
            case ConfigSource::File: ss << "[FILE]    "; break;
            case ConfigSource::CommandLine: ss << "[CLI]     "; break;
            case ConfigSource::Runtime: ss << "[RUNTIME] "; break;
        }
        
        // Print value
        if (std::holds_alternative<ConfigInt>(entry.value)) {
            ss << std::get<ConfigInt>(entry.value).value << " (int)";
        } else if (std::holds_alternative<ConfigDouble>(entry.value)) {
            ss << std::get<ConfigDouble>(entry.value).value << " (double)";
        } else if (std::holds_alternative<ConfigString>(entry.value)) {
            ss << "\"" << std::get<ConfigString>(entry.value).value << "\" (string)";
        } else if (std::holds_alternative<ConfigBool>(entry.value)) {
            ss << (std::get<ConfigBool>(entry.value).value ? "true" : "false") << " (bool)";
        } else if (std::holds_alternative<ConfigVectorInt>(entry.value)) {
            ss << "[vector of int]";
        } else if (std::holds_alternative<ConfigVectorDouble>(entry.value)) {
            ss << "[vector of double]";
        }
        
        ss << " - " << entry.description << std::endl;
    }
    
    return ss.str();
}

// Get all entries
const std::unordered_map<std::string, ConfigEntry>& Config::getAllEntries() const {
    return pImpl->entries;
}

// Validate all entries
std::vector<std::string> Config::validateAll() const {
    std::vector<std::string> errors;
    
    for (const auto& pair : pImpl->entries) {
        const ConfigEntry& entry = pair.second;
        if (!entry.isValid()) {
            errors.push_back("Invalid configuration for key: " + pair.first);
        }
    }
    
    return errors;
}

// Type-specific getters
std::optional<int> Config::getInt(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it != pImpl->entries.end()) {
        const ConfigEntry& entry = it->second;
        if (entry.isValid() && std::holds_alternative<ConfigInt>(entry.value)) {
            return std::get<ConfigInt>(entry.value).value;
        }
    }
    return std::nullopt;
}

std::optional<double> Config::getDouble(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it != pImpl->entries.end()) {
        const ConfigEntry& entry = it->second;
        if (entry.isValid() && std::holds_alternative<ConfigDouble>(entry.value)) {
            return std::get<ConfigDouble>(entry.value).value;
        }
    }
    return std::nullopt;
}

std::optional<std::string> Config::getString(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it != pImpl->entries.end()) {
        const ConfigEntry& entry = it->second;
        if (entry.isValid() && std::holds_alternative<ConfigString>(entry.value)) {
            return std::get<ConfigString>(entry.value).value;
        }
    }
    return std::nullopt;
}

std::optional<bool> Config::getBool(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it != pImpl->entries.end()) {
        const ConfigEntry& entry = it->second;
        if (entry.isValid() && std::holds_alternative<ConfigBool>(entry.value)) {
            return std::get<ConfigBool>(entry.value).value;
        }
    }
    return std::nullopt;
}

std::optional<std::vector<int>> Config::getVectorInt(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it != pImpl->entries.end()) {
        const ConfigEntry& entry = it->second;
        if (entry.isValid() && std::holds_alternative<ConfigVectorInt>(entry.value)) {
            return std::get<ConfigVectorInt>(entry.value).value;
        }
    }
    return std::nullopt;
}

std::optional<std::vector<double>> Config::getVectorDouble(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it != pImpl->entries.end()) {
        const ConfigEntry& entry = it->second;
        if (entry.isValid() && std::holds_alternative<ConfigVectorDouble>(entry.value)) {
            return std::get<ConfigVectorDouble>(entry.value).value;
        }
    }
    return std::nullopt;
}

// Type-specific setters
void Config::setInt(const std::string& key, int value, ConfigSource source) {
    pImpl->entries[key] = ConfigEntry(key, ConfigInt(value), source);
}

void Config::setDouble(const std::string& key, double value, ConfigSource source) {
    pImpl->entries[key] = ConfigEntry(key, ConfigDouble(value), source);
}

void Config::setString(const std::string& key, const std::string& value, ConfigSource source) {
    pImpl->entries[key] = ConfigEntry(key, ConfigString(value), source);
}

void Config::setBool(const std::string& key, bool value, ConfigSource source) {
    pImpl->entries[key] = ConfigEntry(key, ConfigBool(value), source);
}

void Config::setVectorInt(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    pImpl->entries[key] = ConfigEntry(key, ConfigVectorInt(value), source);
}

void Config::setVectorDouble(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    pImpl->entries[key] = ConfigEntry(key, ConfigVectorDouble(value), source);
}

// Type-safe getters with defaults (overloads)
int Config::getOrInt(const std::string& key, int defaultValue) const {
    auto optionalVal = getOptional<int>(key);
    return optionalVal ? *optionalVal : defaultValue;
}

double Config::getOrDouble(const std::string& key, double defaultValue) const {
    auto optionalVal = getOptional<double>(key);
    return optionalVal ? *optionalVal : defaultValue;
}

std::string Config::getOrString(const std::string& key, const std::string& defaultValue) const {
    auto optionalVal = getOptional<std::string>(key);
    return optionalVal ? *optionalVal : defaultValue;
}

bool Config::getOrBool(const std::string& key, bool defaultValue) const {
    auto optionalVal = getOptional<bool>(key);
    return optionalVal ? *optionalVal : defaultValue;
}

std::vector<int> Config::getOrVectorInt(const std::string& key, const std::vector<int>& defaultValue) const {
    auto optionalVal = getOptional<std::vector<int>>(key);
    return optionalVal ? *optionalVal : defaultValue;
}

std::vector<double> Config::getOrVectorDouble(const std::string& key, const std::vector<double>& defaultValue) const {
    auto optionalVal = getOptional<std::vector<double>>(key);
    return optionalVal ? *optionalVal : defaultValue;
}

// Advanced configuration utilities
std::string Config::dumpJSON() const {
    std::stringstream ss;
    ss << "{\n";
    bool first = true;
    for (const auto& pair : pImpl->entries) {
        const ConfigEntry& entry = pair.second;
        if (!first) ss << ",\n";
        first = false;
        
        ss << "  \"" << pair.first << "\": ";
        
        if (std::holds_alternative<ConfigInt>(entry.value)) {
            const auto& val = std::get<ConfigInt>(entry.value);
            ss << val.value;
        } else if (std::holds_alternative<ConfigDouble>(entry.value)) {
            const auto& val = std::get<ConfigDouble>(entry.value);
            ss << std::fixed << std::setprecision(6) << val.value;
        } else if (std::holds_alternative<ConfigString>(entry.value)) {
            const auto& val = std::get<ConfigString>(entry.value);
            ss << "\"" << val.value << "\"";
        } else if (std::holds_alternative<ConfigBool>(entry.value)) {
            const auto& val = std::get<ConfigBool>(entry.value);
            ss << (val.value ? "true" : "false");
        }
    }
    ss << "\n}\n";
    return ss.str();
}

bool Config::validateAll() const {
    for (const auto& pair : pImpl->entries) {
        const ConfigEntry& entry = pair.second;
        if (!entry.isValid()) {
            return false;
        }
    }
    return true;
}

Config Config::loadFromDefaults(const std::string& filepath) {
    Config config;
    // Set sensible defaults
    config.setInt("neuron_count", 1000, ConfigSource::Default);
    config.setDouble("simulation_timestep", 0.001, ConfigSource::Default);
    config.setDouble("replay_interval", 100, ConfigSource::Default);
    config.setDouble("consolidation_interval", 1000, ConfigSource::Default);
    config.setString("checkpoint_dir", "./checkpoints", ConfigSource::Default);
    config.setBool("enable_audio", true, ConfigSource::Default);
    config.setBool("enable_visualization", false, ConfigSource::Default);
    config.setInt("max_neurons", 10000, ConfigSource::Default);
    config.setDouble("max_memory_gb", 4.0, ConfigSource::Default);
    
    // Load from file if provided
    if (!filepath.empty()) {
        config.loadFromFile(filepath);
    }
    
    return config;
}

// Load from file (JSON format)
bool Config::loadFromFile(const std::string& filepath) {
    // Basic JSON-like parsing implementation
    // This is a simplified implementation for demonstration
    if (filepath.empty()) {
        return false;
    }
    
    // TODO: Implement proper JSON parsing
    // For now, just set some test values
    setInt("neuron_count", 1000, ConfigSource::File);
    setDouble("simulation_timestep", 0.001, ConfigSource::File);
    setString("checkpoint_dir", "./checkpoints", ConfigSource::File);
    setBool("enable_audio", true, ConfigSource::File);
    
    return true;
}

// Load from command line arguments
bool Config::loadFromArgs(int argc, char** argv) {
    if (argc < 2) {
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2);
            size_t eqPos = key.find('=');
            if (eqPos != std::string::npos) {
                std::string value = key.substr(eqPos + 1);
                key = key.substr(0, eqPos);
                
                // Parse value based on key
                if (key == "neuron_count") {
                    try {
                        setInt(key, std::stoi(value), ConfigSource::CommandLine);
                    } catch (const std::exception&) {
                        // Invalid value, use default
                    }
                } else if (key == "simulation_timestep") {
                    try {
                        setDouble(key, std::stod(value), ConfigSource::CommandLine);
                    } catch (const std::exception&) {
                        // Invalid value, use default
                    }
                } else if (key == "checkpoint_dir") {
                    setString(key, value, ConfigSource::CommandLine);
                } else if (key == "enable_audio") {
                    setBool(key, value == "true" || value == "1", ConfigSource::CommandLine);
                }
            }
        }
    }
    
    return true;
}

// Save to file
bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        return false;
    }
    
    // Basic JSON-like serialization
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    bool first = true;
    for (const auto& pair : pImpl->entries) {
        const ConfigEntry& entry = pair.second;
        if (!first) file << ",\n";
        first = false;
        
        file << "  \"" << pair.first << "\": ";
        
        if (std::holds_alternative<ConfigInt>(entry.value)) {
            const auto& val = std::get<ConfigInt>(entry.value);
            file << val.value;
        } else if (std::holds_alternative<ConfigDouble>(entry.value)) {
            const auto& val = std::get<ConfigDouble>(entry.value);
            file << std::fixed << std::setprecision(6) << val.value;
        } else if (std::holds_alternative<ConfigString>(entry.value)) {
            const auto& val = std::get<ConfigString>(entry.value);
            file << "\"" << val.value << "\"";
        } else if (std::holds_alternative<ConfigBool>(entry.value)) {
            const auto& val = std::get<ConfigBool>(entry.value);
            file << (val.value ? "true" : "false");
        }
    }
    file << "\n}\n";
    
    file.close();
    return true;
}

} // namespace nlm
