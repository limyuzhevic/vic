#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <vector>
#include <typeindex>
#include <limits>
#include <iostream>

using json = nlohmann::json;

namespace nlm {

// Global validation registry - static map for known config keys
static std::map<std::string, ConfigMetadata> validationRegistry;

// Global templates collection
static std::map<std::string, ConfigTemplate> configTemplates;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Validation and template storage will be handled in implementation
};

// Initialize global validation registry with common configuration keys
void initializeValidationRegistry() {
    // Initialize validation registry with common NLM configuration parameters
    validationRegistry["neuron_count"] = ConfigMetadata(
        "Number of neurons in the network", "int", true
    );
    validationRegistry["neuron_count"].range = {1, std::numeric_limits<int>::max()};
    
    validationRegistry["region_count"] = ConfigMetadata(
        "Number of brain regions", "int", true
    );
    validationRegistry["region_count"].range = {1, std::numeric_limits<int>::max()};
    
    validationRegistry["connection_probability"] = ConfigMetadata(
        "Probability of connections between neurons", "float", true
    );
    validationRegistry["connection_probability"].range = {0.0, 1.0};
    
    validationRegistry["simulation_timestep"] = ConfigMetadata(
        "Time step for simulation", "float", false
    );
    validationRegistry["simulation_timestep"].range = {1e-9, 1.0};
    
    validationRegistry["random_seed"] = ConfigMetadata(
        "Random seed for reproducible results", "int", false
    );
    
    validationRegistry["stdp_ltp_weight"] = ConfigMetadata(
        "STDP LTP weight factor", "float", false
    );
    validationRegistry["stdp_ltp_weight"].range = {0.0, 10.0};
    
    validationRegistry["stdp_ltd_weight"] = ConfigMetadata(
        "STDP LTD weight factor", "float", false
    );
    validationRegistry["stdp_ltd_weight"].range = {0.0, 10.0};
    
    validationRegistry["synaptogenesis_rate"] = ConfigMetadata(
        "Rate of synapse creation", "float", false
    );
    validationRegistry["synaptogenesis_rate"].range = {0.0, 0.1};
    
    validationRegistry["pruning_rate"] = ConfigMetadata(
        "Rate of synapse pruning", "float", false
    );
    validationRegistry["pruning_rate"].range = {0.0, 0.1};
    
    validationRegistry["working_memory_capacity"] = ConfigMetadata(
        "Working memory capacity", "int", false
    );
    validationRegistry["working_memory_capacity"].range = {0, std::numeric_limits<int>::max()};
    
    validationRegistry["episodic_memory_max_episodes"] = ConfigMetadata(
        "Maximum number of episodic memory episodes", "int", false
    );
    validationRegistry["episodic_memory_max_episodes"].range = {0, std::numeric_limits<int>::max()};
}

// Initialize global templates
void initializeConfigTemplates() {
    ConfigTemplate smallConfig;
    smallConfig.name = "small";
    smallConfig.description = "Small configuration for quick testing";
    smallConfig.values = {
        {"neuron_count", 100},
        {"region_count", 1},
        {"connection_probability", 0.2},
        {"simulation_timestep", 0.001},
        {"random_seed", 42}
    };
    configTemplates[smallConfig.name] = smallConfig;
    
    ConfigTemplate mediumConfig;
    mediumConfig.name = "medium";
    mediumConfig.description = "Medium configuration for standard experiments";
    mediumConfig.values = {
        {"neuron_count", 1000},
        {"region_count", 2},
        {"connection_probability", 0.15},
        {"simulation_timestep", 0.001},
        {"random_seed", 42},
        {"stdp_ltp_weight", 0.02},
        {"stdp_ltd_weight", 0.018}
    };
    configTemplates[mediumConfig.name] = mediumConfig;
    
    ConfigTemplate largeConfig;
    largeConfig.name = "large";
    largeConfig.description = "Large configuration for intensive research";
    largeConfig.values = {
        {"neuron_count", 10000},
        {"region_count", 5},
        {"connection_probability", 0.1},
        {"simulation_timestep", 0.0005},
        {"random_seed", 42},
        {"stdp_ltp_weight", 0.015},
        {"stdp_ltd_weight", 0.014},
        {"synaptogenesis_rate", 0.00005},
        {"pruning_rate", 0.000005}
    };
    configTemplates[largeConfig.name] = largeConfig;
    
    ConfigTemplate defaultConfig;
    defaultConfig.name = "default";
    defaultConfig.description = "Default configuration with balanced settings";
    defaultConfig.values = {
        {"neuron_count", 1000},
        {"region_count", 2},
        {"connection_probability", 0.1},
        {"simulation_timestep", 0.001},
        {"random_seed", 42},
        {"stdp_ltp_weight", 0.01},
        {"stdp_ltd_weight", 0.012},
        {"synaptogenesis_rate", 0.0001},
        {"pruning_rate", 0.00001},
        {"working_memory_capacity", 100},
        {"episodic_memory_max_episodes", 1000}
    };
    configTemplates[defaultConfig.name] = defaultConfig;
}

// Static initialization
namespace {
    struct GlobalInitializer {
        GlobalInitializer() {
            initializeValidationRegistry();
            initializeConfigTemplates();
        }
    } globalInitializer;
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced loadFromFile with JSON support
bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigError(ConfigError::Type::FileError, 
                "Failed to open configuration file: " + filepath);
        }
        
        json configJson;
        try {
            file >> configJson;
        } catch (const json::parse_error& e) {
            throw ConfigError(ConfigError::Type::ParseError,
                "Failed to parse JSON configuration file: " + std::string(e.what()));
        }
        
        clear();
        
        for (auto it = configJson.begin(); it != configJson.end(); ++it) {
            std::string key = it.key();
            
            try {
                std::string valueStr;
                
                if (it->is_string()) {
                    valueStr = *it;
                    set(key, valueStr, ConfigSource::File);
                }
                else if (it->is_number_integer()) {
                    int value = *it;
                    set(key, value, ConfigSource::File);
                }
                else if (it->is_number_float()) {
                    double value = *it;
                    set(key, value, ConfigSource::File);
                }
                else if (it->is_boolean()) {
                    bool value = *it;
                    set(key, value, ConfigSource::File);
                }
                else if (it->is_array()) {
                    // Handle array types - determine element type
                    bool hasInt = false, hasDouble = false, hasString = false;
                    std::vector<ConfigValue> arrayValues;
                    
                    for (const auto& elem : *it) {
                        if (elem.is_string()) {
                            hasString = true;
                            arrayValues.push_back(elem.get<std::string>());
                        }
                        else if (elem.is_number_integer()) {
                            hasInt = true;
                            arrayValues.push_back(elem.get<int64_t>());
                        }
                        else if (elem.is_number_float()) {
                            hasDouble = true;
                            arrayValues.push_back(elem.get<double>());
                        }
                    }
                    
                    if (hasString) {
                        std::vector<std::string> stringArray;
                        for (const auto& val : arrayValues) {
                            stringArray.push_back(std::get<std::string>(val));
                        }
                        set(key, stringArray, ConfigSource::File);
                    }
                    else if (hasInt) {
                        std::vector<int64_t> intArray;
                        for (const auto& val : arrayValues) {
                            intArray.push_back(std::get<int64_t>(val));
                        }
                        set(key, intArray, ConfigSource::File);
                    }
                    else if (hasDouble) {
                        std::vector<double> doubleArray;
                        for (const auto& val : arrayValues) {
                            doubleArray.push_back(std::get<double>(val));
                        }
                        set(key, doubleArray, ConfigSource::File);
                    }
                }
                else {
                    // Convert to string representation
                    valueStr = it->dump();
                    set(key, valueStr, ConfigSource::File);
                }
                
            } catch (const std::exception& e) {
                throw ConfigError(ConfigError::Type::ParseError,
                    "Error processing key '" + key + "' in config file: " + std::string(e.what()));
            }
        }
        
        // Validate the loaded configuration
        std::vector<std::string> validationErrors;
        validateConfiguration(validationErrors);
        
        if (!validationErrors.empty()) {
            std::string errorMsg = "Configuration file validation failed:\n";
            for (const auto& error : validationErrors) {
                errorMsg += "  - " + error + "\n";
            }
            throw ConfigError(ConfigError::Type::ValidationFailed, errorMsg);
        }
        
        return true;
        
    } catch (const ConfigError&) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigError(ConfigError::Type::FileError,
            "Failed to load configuration file: " + std::string(e.what()));
    }
}

// loadFromArgs - unchanged from original
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

// Enhanced saveToFile with JSON support
bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigError(ConfigError::Type::FileError,
                "Failed to open configuration file for writing: " + filepath);
        }
        
        json configJson;
        
        for (const auto& entry : pImpl->entries) {
            try {
                if (std::holds_alternative<int>(entry.value)) {
                    configJson[entry.key] = std::get<int>(entry.value);
                }
                else if (std::holds_alternative<int64_t>(entry.value)) {
                    configJson[entry.key] = std::get<int64_t>(entry.value);
                }
                else if (std::holds_alternative<double>(entry.value)) {
                    configJson[entry.key] = std::get<double>(entry.value);
                }
                else if (std::holds_alternative<bool>(entry.value)) {
                    configJson[entry.key] = std::get<bool>(entry.value);
                }
                else if (std::holds_alternative<std::string>(entry.value)) {
                    configJson[entry.key] = std::get<std::string>(entry.value);
                }
                else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                    std::vector<int64_t> intArray;
                    for (const auto& val : std::get<std::vector<int>>(entry.value)) {
                        intArray.push_back(val);
                    }
                    configJson[entry.key] = intArray;
                }
                else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                    configJson[entry.key] = std::get<std::vector<double>>(entry.value);
                }
                else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                    configJson[entry.key] = std::get<std::vector<std::string>>(entry.value);
                }
            }
            catch (const std::exception& e) {
                throw ConfigError(ConfigError::Type::SerializationError,
                    "Error serializing key '" + entry.key + "': " + std::string(e.what()));
            }
        }
        
        // Validate the configuration before saving
        std::vector<std::string> validationErrors;
        validateConfiguration(validationErrors);
        
        if (!validationErrors.empty()) {
            std::string errorMsg = "Configuration validation failed before save:\n";
            for (const auto& error : validationErrors) {
                errorMsg += "  - " + error + "\n";
            }
            throw ConfigError(ConfigError::Type::ValidationFailed, errorMsg);
        }
        
        file << configJson.dump(2);
        
        return true;
        
    } catch (const ConfigError&) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigError(ConfigError::Type::FileError,
            "Failed to save configuration file: " + std::string(e.what()));
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
