#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <set>
#include <stdexcept>
#include <functional>

// JSON support (optional - if nlohmann/json is available)
#ifdef HAVE_NLOHMANN_JSON
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#endif

// YAML support (optional - if yaml-cpp is available)
#ifdef HAVE_YAML_CPP
#include "yaml-cpp/yaml.h"
using YAML::Node;
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // For future JSON/YAML support: std::unordered_map<std::string, json> parsedConfig;
    // Configuration validation and schema
    std::set<std::string> validKeys;
    bool strictMode;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default validation
    pImpl->strictMode = false;
    initializeDefaultValidators();
    initializeDefaultKeys();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Initialize default configuration validators
void Config::initializeDefaultValidators() {
    // Basic type validation for known keys
    pImpl->validators["random_seed"] = [](const ConfigValue& val) {
        return std::holds_alternative<int64_t>(val);
    };
    
    pImpl->validators["simulation_timestep"] = [](const ConfigValue& val) {
        return std::holds_alternative<double>(val);
    };
    
    pImpl->validators["neuron_count"] = [](const ConfigValue& val) {
        return std::holds_alternative<int64_t>(val);
    };
    
    pImpl->validators["region_count"] = [](const ConfigValue& val) {
        return std::holds_alternative<int64_t>(val);
    };
    
    pImpl->validators["connection_probability"] = [](const ConfigValue& val) {
        return std::holds_alternative<double>(val);
    };
    
    // Array validation for specific keys
    pImpl->validators["plasticity_learning_rate"] = [](const ConfigValue& val) {
        return std::holds_alternative<double>(val);
    };
    
    pImpl->validators["stdp_ltp_weight"] = [](const ConfigValue& val) {
        return std::holds_alternative<double>(val);
    };
    
    pImpl->validators["stdp_ltd_weight"] = [](const ConfigValue& val) {
        return std::holds_alternative<double>(val);
    };
    
    pImpl->validators["stdp_time_constant"] = [](const ConfigValue& val) {
        return std::holds_alternative<double>(val);
    };
}

// Initialize default valid configuration keys
void Config::initializeDefaultKeys() {
    pImpl->validKeys = {
        "random_seed",
        "simulation_timestep", 
        "neuron_count",
        "region_count",
        "connection_probability",
        "plasticity_learning_rate",
        "stdp_ltp_weight",
        "stdp_ltd_weight",
        "stdp_time_constant",
        "development_synaptogenesis_rate",
        "development_pruning_rate",
        "dopamine_baseline",
        "reward_discount_factor",
        "environment_name",
        "environment_width",
        "environment_height",
        "log_level",
        "log_to_file",
        "log_filename",
        "max_simulation_steps",
        "simulation_time_limit",
        "visualization_enabled",
        "visualization_update_rate"
    };
}

// Validate configuration value for a key
bool Config::validateKey(const std::string& key, const ConfigValue& value) const {
    if (pImpl->strictMode) {
        // Check if key is in valid keys set
        if (pImpl->validKeys.find(key) == pImpl->validKeys.end()) {
            return false;
        }
    }
    
    // Check if there's a specific validator for this key
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end()) {
        return validatorIt->second(value);
    }
    
    // No validator, accept any value
    return true;
}

// Set strict mode for configuration validation
void Config::setStrictMode(bool strict) {
    pImpl->strictMode = strict;
}

// Add custom validator for a configuration key
void Config::addValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator) {
    pImpl->validators[key] = validator;
}

// Add custom valid key
void Config::addValidKey(const std::string& key) {
    pImpl->validKeys.insert(key);
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check first few lines for JSON/YAML indicators
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    // Detect file format
    bool isJSON = false;
    bool isYAML = false;
    
    if (firstLine.find('{') != std::string::npos || 
        firstLine.find('[') != std::string::npos) {
        isJSON = true;
    } else if (firstLine.find(':') != std::string::npos ||
               firstLine.find('-') != std::string::npos) {
        isYAML = true;
    }
    
    // Try JSON first
#ifdef HAVE_NLOHMANN_JSON
    if (isJSON) {
        return loadFromJSONFile(filepath);
    }
#endif
    
    // Try YAML
#ifdef HAVE_YAML_CPP
    if (isYAML) {
        return loadFromYAMLFile(filepath);
    }
#endif
    
    // Fall back to simple key=value format
    return loadFromSimpleFile(filepath);
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
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

#ifdef HAVE_NLOHMANN_JSON
bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        // Process JSON object
        if (j.is_object()) {
            for (auto& [key, value] : j.items()) {
                ConfigValue configValue;
                
                if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number_unsigned()) {
                    configValue = value.get<uint64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_array()) {
                    // Convert JSON array to ConfigValue
                    std::vector<std::string> strArray;
                    for (const auto& item : value) {
                        strArray.push_back(item.get<std::string>());
                    }
                    configValue = strArray;
                }
                
                set(key, configValue, ConfigSource::File);
            }
        }
    } catch (const std::exception& e) {
        return false;
    }
    
    return true;
}
#endif

#ifdef HAVE_YAML_CPP
bool Config::loadFromYAMLFile(const std::string& filepath) {
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        // Process YAML mapping
        if (doc.IsMap()) {
            for (const auto& pair : doc) {
                std::string key = pair.first.as<std::string>();
                ConfigValue configValue;
                
                if (pair.second.IsScalar()) {
                    std::string valueStr = pair.second.as<std::string>();
                    
                    // Try to parse as different types
                    try {
                        bool boolVal = std::stoi(valueStr);
                        configValue = boolVal;
                    } catch (...) {
                        try {
                            double doubleVal = std::stod(valueStr);
                            configValue = doubleVal;
                        } catch (...) {
                            configValue = valueStr;
                        }
                    }
                } else if (pair.second.IsSequence()) {
                    std::vector<std::string> strArray;
                    for (const auto& item : pair.second) {
                        strArray.push_back(item.as<std::string>());
                    }
                    configValue = strArray;
                }
                
                set(key, configValue, ConfigSource::File);
            }
        }
    } catch (const std::exception& e) {
        return false;
    }
    
    return true;
}
#endif

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
    
    // Save in simple key=value format (backward compatible)
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert ConfigValue to string
        std::visit([&](const auto& val) {
            file << entry.key << " = ";
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else {
                file << val;
            }
            file << "\n";
        }, entry.value);
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
    // Validate the configuration key before setting
    if (!validateKey(key, value)) {
        throw std::invalid_argument("Invalid configuration key or value: " + key);
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

} // namespace nlm