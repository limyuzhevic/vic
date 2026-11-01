#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <random>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, bool> validationStatus;
};

// Initialize static validation registry
std::unordered_map<std::string, ConfigValidation> Config::validationRegistry;

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Register default validations for common keys
    registerDefaultValidation("neuron_count", ConfigValidation{
        "neuron_count", "int", "Number of neurons in the brain",
        1, 100000, "", true
    });
    registerDefaultValidation("region_count", ConfigValidation{
        "region_count", "int", "Number of brain regions",
        1, 1000, "", true
    });
    registerDefaultValidation("connection_probability", ConfigValidation{
        "connection_probability", "float", "Probability of synapse formation",
        0.0, 1.0, "", false
    });
    registerDefaultValidation("simulation_timestep", ConfigValidation{
        "simulation_timestep", "float", "Time step size for simulation",
        0.0, 1.0, "", false
    });
    registerDefaultValidation("random_seed", ConfigValidation{
        "random_seed", "int", "Random seed for reproducibility",
        0, 2147483647, "", false
    });
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Try to parse as JSON first
        json j;
        file >> j;
        
        clear();
        
        // Extract configuration from JSON
        if (j.contains("nlm_config")) {
            const auto& config = j["nlm_config"];
            for (const auto& [key, value] : config.items()) {
                ConfigValue configValue;
                
                if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number_unsigned()) {
                    configValue = value.get<uint64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_array()) {
                    // Convert array to appropriate type based on elements
                    if (!value.empty()) {
                        if (std::all_of(value.begin(), value.end(), 
                                     [](const json& v) { return v.is_number_integer(); })) {
                            std::vector<int> intList;
                            intList.reserve(value.size());
                            for (const auto& v : value) {
                                intList.push_back(v.get<int>());
                            }
                            configValue = intList;
                        } else if (std::all_of(value.begin(), value.end(), 
                                            [](const json& v) { return v.is_number_float(); })) {
                            std::vector<double> doubleList;
                            doubleList.reserve(value.size());
                            for (const auto& v : value) {
                                doubleList.push_back(v.get<double>());
                            }
                            configValue = doubleList;
                        } else if (std::all_of(value.begin(), value.end(), 
                                            [](const json& v) { return v.is_string(); })) {
                            std::vector<std::string> stringList;
                            stringList.reserve(value.size());
                            for (const auto& v : value) {
                                stringList.push_back(v.get<std::string>());
                            }
                            configValue = stringList;
                        }
                    }
                }
                
                if (!configValue.index()) {
                    continue; // Skip if we couldn't convert
                }
                
                set(key, configValue, ConfigSource::File);
            }
        }
        
        // Also support simple key=value format for backward compatibility
        std::ifstream file2(filepath);
        if (file2.is_open()) {
            std::string line;
            while (std::getline(file2, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    // Only set if key doesn't already exist (file config has lower priority)
                    if (!has(key)) {
                        set(key, value, ConfigSource::File);
                    }
                }
            }
            file2.close();
        }
        
        return true;
    } catch (const std::exception& e) {
        // Fall back to simple format if JSON parsing fails
        return loadFromFileSimple(filepath);
    }
}

bool Config::loadFromFileSimple(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
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
    
    // Save as JSON format
    json j;
    j["nlm_version"] = "2.0";
    j["nlm_description"] = "Neural Learning Machine Configuration";
    
    json configObj;
    for (const auto& entry : pImpl->entries) {
        std::visit([&configObj, &entry](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) {
                configObj[entry.key] = static_cast<int64_t>(val);
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                configObj[entry.key] = static_cast<double>(val);
            } else if constexpr (std::is_same_v<T, bool>) {
                configObj[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                configObj[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                configObj[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                configObj[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                configObj[entry.key] = val;
            }
        }, entry.value);
    }
    
    j["nlm_config"] = configObj;
    
    // Add metadata
    j["created_by"] = "NLM Phase 6";
    j["creation_date"] = std::chrono::system_clock::now();
    
    file << std::setw(2) << j << std::endl;
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, bool validate) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigEntry entry(key, value, source, "Configuration entry for " + key);
    
    if (validate) {
        // Check if key has registered validation
        auto valIt = validationRegistry.find(key);
        if (valIt != validationRegistry.end()) {
            if (!valIt->second.validate(value)) {
                std::cerr << "Validation failed for key '" << key << "' with value '" << getErrorMessage(key, value) << "'" << std::endl;
                return;
            }
            pImpl->validationStatus[key] = true;
        }
    }
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.push_back(entry);
        pImpl->validationStatus[key] = validate && entry.source != ConfigSource::Default;
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, int value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, double value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, bool value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

bool Config::isValid(const std::string& key) const {
    auto valIt = pImpl->validationStatus.find(key);
    if (valIt == pImpl->validationStatus.end()) {
        return true; // No validation registered means valid
    }
    return valIt->second;
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    pImpl->validationStatus.erase(key);
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
    pImpl->validationStatus.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "NLM Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "Validation Status:\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << ": ";
        if (isValid(entry.key)) {
            oss << "✓ VALID";
        } else {
            oss << "✗ INVALID";
        }
        oss << " (Source: " << static_cast<int>(entry.source) << ") " << entry.description << "\n";
    }
    
    return oss.str();
}

void Config::registerDefaultValidation(const std::string& key, ConfigValidation validation) {
    validationRegistry[key] = validation;
}

void Config::unregisterValidation(const std::string& key) {
    validationRegistry.erase(key);
}

bool Config::validateAll() const {
    for (const auto& entry : pImpl->entries) {
        auto valIt = validationRegistry.find(entry.key);
        if (valIt != validationRegistry.end() && !valIt->second.validate(entry.value)) {
            return false;
        }
    }
    return true;
}

std::string Config::toJSON() const {
    json j;
    j["nlm_config"] = json::object();
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&j, &entry](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) {
                j["nlm_config"][entry.key] = static_cast<int64_t>(val);
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                j["nlm_config"][entry.key] = static_cast<double>(val);
            } else if constexpr (std::is_same_v<T, bool>) {
                j["nlm_config"][entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                j["nlm_config"][entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                j["nlm_config"][entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j["nlm_config"][entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j["nlm_config"][entry.key] = val;
            }
        }, entry.value);
    }
    
    return j.dump(2);
}

bool Config::fromJSON(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        clear();
        
        if (j.contains("nlm_config")) {
            const auto& config = j["nlm_config"];
            for (const auto& [key, value] : config.items()) {
                ConfigValue configValue;
                
                if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number_unsigned()) {
                    configValue = value.get<uint64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_array()) {
                    // Convert array to appropriate type
                    if (!value.empty()) {
                        if (std::all_of(value.begin(), value.end(), 
                                     [](const json& v) { return v.is_number_integer(); })) {
                            std::vector<int> intList;
                            intList.reserve(value.size());
                            for (const auto& v : value) {
                                intList.push_back(v.get<int>());
                            }
                            configValue = intList;
                        } else if (std::all_of(value.begin(), value.end(), 
                                            [](const json& v) { return v.is_number_float(); })) {
                            std::vector<double> doubleList;
                            doubleList.reserve(value.size());
                            for (const auto& v : value) {
                                doubleList.push_back(v.get<double>());
                            }
                            configValue = doubleList;
                        } else if (std::all_of(value.begin(), value.end(), 
                                            [](const json& v) { return v.is_string(); })) {
                            std::vector<std::string> stringList;
                            stringList.reserve(value.size());
                            for (const auto& v : value) {
                                stringList.push_back(v.get<std::string>());
                            }
                            configValue = stringList;
                        }
                    }
                }
                
                if (!configValue.index()) {
                    continue;
                }
                
                set(key, configValue, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

Config Config::createDefaultConfig() {
    Config config;
    
    // Basic system configuration
    config.set("neuron_count", 500);
    config.set("region_count", 1);
    config.set("connection_probability", 0.15f);
    
    // Simulation parameters
    config.set("simulation_timestep", 0.001);
    config.set("random_seed", static_cast<int64_t>(42));
    
    // Plasticity parameters
    config.set("stdp_ltp_weight", 0.02f);
    config.set("stdp_ltd_weight", 0.015f);
    config.set("stdp_tau", 20.0f);
    
    // Structural plasticity
    config.set("synaptogenesis_rate", 0.0001f);
    config.set("pruning_rate", 0.00001f);
    
    // Memory and learning
    config.set("working_memory_size", 100);
    config.set("episodic_memory_capacity", 1000);
    
    // Advanced features
    config.set("enable_checkpointing", true);
    config.set("enable_replay", true);
    config.set("enable_development", true);
    
    return config;
}

Config Config::createQuickStartConfig() {
    Config config;
    
    // Quick start settings for rapid prototyping
    config.set("neuron_count", 200);
    config.set("region_count", 1);
    config.set("connection_probability", 0.1f);
    
    config.set("simulation_timestep", 0.001);
    config.set("random_seed", static_cast<int64_t>(123));
    
    config.set("stdp_ltp_weight", 0.01f);
    config.set("stdp_ltd_weight", 0.012f);
    
    config.set("synaptogenesis_rate", 0.00005f);
    config.set("pruning_rate", 0.000005f);
    
    // Enable only essential features
    config.set("enable_checkpointing", false);
    config.set("enable_replay", false);
    config.set("enable_development", true);
    
    return config;
}

Config Config::createResearchConfig() {
    Config config;
    
    // High-performance research settings
    config.set("neuron_count", 5000);
    config.set("region_count", 4);
    config.set("connection_probability", 0.05f);
    
    config.set("simulation_timestep", 0.0005);
    config.set("random_seed", static_cast<int64_t>(456));
    
    config.set("stdp_ltp_weight", 0.005f);
    config.set("stdp_ltd_weight", 0.006f);
    config.set("stdp_tau", 10.0f);
    
    config.set("synaptogenesis_rate", 0.00001f);
    config.set("pruning_rate", 0.000001f);
    
    // Enable all features for comprehensive research
    config.set("enable_checkpointing", true);
    config.set("enable_replay", true);
    config.set("enable_development", true);
    
    // Memory-intensive features
    config.set("working_memory_size", 1000);
    config.set("episodic_memory_capacity", 10000);
    
    return config;
}

bool Config::validateValue(const ConfigValue& value, ConfigSource source, bool validate) {
    return true; // Simplified validation
}

std::string Config::getErrorMessage(const std::string& key, const ConfigValue& value) const {
    return "Invalid value for key: " + key;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

namespace nlm {
    // Implementation of helper functions
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
}} // namespace nlm