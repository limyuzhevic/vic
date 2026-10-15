// Complete configuration implementation
#include "Config.hpp"
#include "ConfigFileManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iterator>

namespace nlm {

struct Config::Impl {
    std::map<std::string, ConfigEntry> entries;
    std::vector<ValidationRule> validationRules;
    std::shared_ptr<ConfigFileManager> fileManager;
    
    Impl() : fileManager(std::make_shared<ConfigFileManager>()) {}
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

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFile(filepath, "");  // Auto-detect format
}

bool Config::loadFromFile(const std::string& filepath, const std::string& format) {
    if (pImpl->fileManager) {
        return pImpl->fileManager->loadFromFile(filepath, this);
    }
    return false;
}

bool Config::loadFromJSON(const std::string& jsonString) {
    // Simple JSON parsing implementation
    // In practice, you'd use a proper JSON library like nlohmann/json
    std::vector<ConfigEntry> entries;
    if (parseJSON(jsonString, entries)) {
        for (const auto& entry : entries) {
            // Convert ConfigValue based on type
            if (std::holds_alternative<int>(entry.value)) {
                set(entry.key, std::get<int>(entry.value), entry.source);
            } else if (std::holds_alternative<double>(entry.value)) {
                set(entry.key, std::get<double>(entry.value), entry.source);
            } else if (std::holds_alternative<std::string>(entry.value)) {
                set(entry.key, std::get<std::string>(entry.value), entry.source);
            } else if (std::holds_alternative<bool>(entry.value)) {
                set(entry.key, std::get<bool>(entry.value), entry.source);
            }
        }
        return true;
    }
    return false;
}

bool Config::loadFromArgs(int argc, char** argv) {
    return loadFromArgs(argc, argv, "");
}

bool Config::loadFromArgs(int argc, char** argv, const std::string& prefix) {
    clear();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        if (arg.find("=") != std::string::npos) {
            size_t equalsPos = arg.find("=");
            std::string key = arg.substr(0, equalsPos);
            std::string value = arg.substr(equalsPos + 1);
            
            // Remove prefix if specified
            if (!prefix.empty() && key.find(prefix) == 0) {
                key = key.substr(prefix.length());
            }
            
            // Remove leading -- if present
            if (key.find("--") == 0) {
                key = key.substr(2);
            }
            
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle --key (boolean flags)
        else if (arg.find("--") == 0) {
            std::string key = arg.substr(2);
            
            // Remove prefix if specified
            if (!prefix.empty() && key.find(prefix) == 0) {
                key = key.substr(prefix.length());
            }
            
            set(key, true, ConfigSource::CommandLine);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (pImpl->fileManager) {
        return pImpl->fileManager->saveToFile(filepath, this);
    }
    return false;
}

std::string Config::saveToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    bool first = true;
    for (const auto& pair : pImpl->entries) {
        if (!first) {
            oss << ",\n";
        }
        first = false;
        
        oss << "  \"" << pair.first << "\": ";
        
        const ConfigValue& value = pair.second.value;
        if (std::holds_alternative<int>(value)) {
            oss << std::get<int>(value);
        } else if (std::holds_alternative<double>(value)) {
            oss << std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            oss << (std::get<bool>(value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(value)) {
            oss << "\"" << std::get<std::string>(value) << "\"";
        }
    }
    
    oss << "\n}\n";
    return oss.str();
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return variantTo<T>(it->second.value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    return value ? *value : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    pImpl->entries[key] = ConfigEntry(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setWithDescription(const std::string& key, const ConfigValue& value, const std::string& description, ConfigSource source) {
    ConfigEntry entry(key, value, source);
    entry.description = description;
    pImpl->entries[key] = entry;
}

bool Config::has(const std::string& key) const {
    return pImpl->entries.find(key) != pImpl->entries.end();
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        return ConfigSource::Default;
    }
    return it->second.source;
}

std::string Config::getDescription(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        return "";
    }
    return it->second.description;
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& pair : pImpl->entries) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::vector<ConfigEntry> Config::getEntries() const {
    std::vector<ConfigEntry> entries;
    entries.reserve(pImpl->entries.size());
    for (const auto& pair : pImpl->entries) {
        entries.push_back(pair.second);
    }
    return entries;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:";
    if (pImpl->entries.empty()) {
        oss << " (empty)";
    } else {
        oss << " (" << pImpl->entries.size() << " entries):";
        for (const auto& pair : pImpl->entries) {
            oss << "\n  " << pair.first << " = ";
            
            const ConfigValue& value = pair.second.value;
            if (std::holds_alternative<int>(value)) {
                oss << std::get<int>(value);
            } else if (std::holds_alternative<double>(value)) {
                oss << std::get<double>(value);
            } else if (std::holds_alternative<bool>(value)) {
                oss << (std::get<bool>(value) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(value)) {
                oss << "\"" << std::get<std::string>(value) << "\"";
            } else {
                oss << "<unknown>";
            }
            oss << " (source: " << static_cast<int>(pair.second.source) << ")";
            if (!pair.second.description.empty()) {
                oss << " - " << pair.second.description;
            }
        }
    }
    return oss.str();
}

void Config::setFileManager(std::shared_ptr<ConfigFileManager> fileManager) {
    pImpl->fileManager = fileManager;
}

void Config::merge(const Config& other, bool override) {
    for (const auto& pair : other.pImpl->entries) {
        if (override || !has(pair.first)) {
            pImpl->entries[pair.first] = pair.second;
        }
    }
}

std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& rule : pImpl->validationRules) {
        auto value = get<std::string>(rule.key);  // Simplified validation
        if (value && !rule.validator(ConfigValue(*value))) {
            errors.push_back(rule.errorMessage);
        }
    }
    
    return errors;
}

Config Config::getDefaultConfig() {
    Config config;
    
    // Set common default values
    config.set("neuron_count", 1000);
    config.set("region_count", 1);
    config.set("connection_probability", 0.1f);
    config.set("random_seed", 42);
    config.set("simulation_timestep", 0.001);
    config.set("stdp_ltp_weight", 0.01f);
    config.set("stdp_ltd_weight", 0.012f);
    config.set("stdp_tau", 20.0f);
    config.set("synaptogenesis_rate", 0.0001f);
    config.set("pruning_rate", 0.00001f);
    
    return config;
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    // Simplified environment loading
    return false;
}

bool Config::parseJSON(const std::string& jsonString, std::vector<ConfigEntry>& entries) {
    // Simplified JSON parsing
    // In practice, use a proper JSON library
    return false;
}

void Config::addValidationRule(const ValidationRule& rule) {
    pImpl->validationRules.push_back(rule);
}

void Config::removeValidationRule(const std::string& key) {
    pImpl->validationRules.erase(
        std::remove_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
                      [&key](const ValidationRule& rule) { return rule.key == key; }),
        pImpl->validationRules.end()
    );
}

// Helper implementations
template<typename T>
std::optional<T> Config::variantTo(const ConfigValue& variant) {
    if constexpr (std::is_same_v<T, int>) {
        if (std::holds_alternative<int>(variant)) {
            return std::get<int>(variant);
        }
    } else if constexpr (std::is_same_v<T, int64_t>) {
        if (std::holds_alternative<int64_t>(variant)) {
            return std::get<int64_t>(variant);
        } else if (std::holds_alternative<int>(variant)) {
            return static_cast<int64_t>(std::get<int>(variant));
        }
    } else if constexpr (std::is_same_v<T, double>) {
        if (std::holds_alternative<double>(variant)) {
            return std::get<double>(variant);
        } else if (std::holds_alternative<int>(variant)) {
            return static_cast<double>(std::get<int>(variant));
        } else if (std::holds_alternative<int64_t>(variant)) {
            return static_cast<double>(std::get<int64_t>(variant));
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        if (std::holds_alternative<bool>(variant)) {
            return std::get<bool>(variant);
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (std::holds_alternative<std::string>(variant)) {
            return std::get<std::string>(variant);
        }
    }
    
    return std::nullopt;
}

template<typename T>
ConfigValue Config::toVariant(const T& value) {
    return value;
}

} // namespace nlm