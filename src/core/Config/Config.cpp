#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <memory>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::vector<ConfigSchema>> defaultSchemas;
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default schemas
    initializeDefaultSchemas();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultSchemas() {
    // Add all known default configuration values
    addDefault("random_seed", ConfigSource::Default, "42", "Random seed for reproducible experiments");
    addDefault("simulation_timestep", ConfigSource::Default, "0.001", "Simulation timestep in seconds");
    addDefault("neuron_count", ConfigSource::Default, "1000", "Number of neurons in the brain");
    addDefault("region_count", ConfigSource::Default, "2", "Number of brain regions");
    addDefault("connection_probability", ConfigSource::Default, "0.1", "Connection probability for random connectivity");
    addDefault("plasticity_learning_rate", ConfigSource::Default, "0.01", "Plasticity learning rate");
    addDefault("stdp_ltp_weight", ConfigSource::Default, "0.01", "STDP LTP weight");
    addDefault("stdp_ltd_weight", ConfigSource::Default, "0.012", "STDP LTD weight");
    addDefault("stdp_time_constant", ConfigSource::Default, "20.0", "STDP time constant");
    addDefault("development_synaptogenesis_rate", ConfigSource::Default, "0.001", "Development synaptogenesis rate");
    addDefault("development_pruning_rate", ConfigSource::Default, "0.0001", "Development pruning rate");
    addDefault("dopamine_baseline", ConfigSource::Default, "0.1", "Dopamine baseline level");
    addDefault("reward_discount_factor", ConfigSource::Default, "0.99", "Reward discount factor");
    addDefault("environment_name", ConfigSource::Default, "GridWorld", "Environment name");
    addDefault("environment_width", ConfigSource::Default, "10", "Environment width");
    addDefault("environment_height", ConfigSource::Default, "10", "Environment height");
    addDefault("log_level", ConfigSource::Default, "INFO", "Logging level");
    addDefault("log_to_file", ConfigSource::Default, "false", "Log to file");
    addDefault("log_filename", ConfigSource::Default, "nlm.log", "Log filename");
    addDefault("max_simulation_steps", ConfigSource::Default, "10000", "Maximum simulation steps");
    addDefault("simulation_time_limit", ConfigSource::Default, "0.0", "Simulation time limit");
    addDefault("visualization_enabled", ConfigSource::Default, "false", "Enable visualization");
    addDefault("visualization_update_rate", ConfigSource::Default, "30.0", "Visualization update rate");
    
    // Phase 6 specific defaults
    addDefault("replay_interval", ConfigSource::Default, "100", "Memory replay interval");
    addDefault("consolidation_interval", ConfigSource::Default, "1000", "Memory consolidation interval");
    addDefault("working_memory_capacity", ConfigSource::Default, "100", "Working memory capacity");
    addDefault("episodic_memory_max_episodes", ConfigSource::Default, "1000", "Maximum episodic memory episodes");
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        // Fall back to simple key=value format for backward compatibility
        return loadFromSimpleFile(filepath);
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        clear();
        
        // Parse JSON entries
        if (jsonData.is_object()) {
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                ConfigValue value;
                
                if (it->is_string()) {
                    value = it->get<std::string>();
                } else if (it->is_number_integer()) {
                    value = it->get<int64_t>();
                } else if (it->is_number_unsigned()) {
                    value = it->get<uint64_t>();
                } else if (it->is_number_float()) {
                    value = it->get<double>();
                } else if (it->is_boolean()) {
                    value = it->get<bool>();
                } else if (it->is_array()) {
                    // Parse array
                    std::vector<double> arrayData;
                    for (const auto& item : *it) {
                        arrayData.push_back(item.get<double>());
                    }
                    value = arrayData;
                } else {
                    // Skip unsupported types
                    continue;
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        // Fall back to simple format if JSON parsing fails
        file.close();
        file.open(filepath);
        if (file.is_open()) {
            return loadFromSimpleFile(filepath);
        }
        return false;
    }
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
            } else {
                // Handle --key value format (without =)
                set(arg.substr(2), "true", ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle boolean flags
        else if (arg[0] == '-' && arg.size() > 1 && arg[1] != '-') {
            std::string key = arg.substr(1);
            set(key, "true", ConfigSource::CommandLine);
        }
    }
    return true;
}

void Config::loadFromEnv(const std::string& prefix) {
    for (const auto& pair : std::filesystem::environ) {
        std::string key = pair.first;
        if (key.rfind(prefix, 0) == 0) {
            std::string configKey = key.substr(prefix.size());
            set(configKey, pair.second, ConfigSource::Environment);
        }
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    // Create JSON representation
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&jsonData, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                std::is_same_v<T, int> ||
                                std::is_same_v<T, uint64_t> ||
                                std::is_same_v<T, double>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                std::vector<int64_t> intArray(arg.begin(), arg.end());
                jsonData[entry.key] = intArray;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonData[entry.key] = arg;
            }
        }, entry.value);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << jsonData.dump(2);
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        // Check defaults
        if (pImpl->defaultSchemas.count(key)) {
            const auto& schemas = pImpl->defaultSchemas.at(key);
            for (const auto& schema : schemas) {
                if (schema.defaultSource != ConfigSource::Default) continue;
                
                // Try to convert default value to type T
                try {
                    if constexpr (std::is_same_v<T, std::string>) {
                        return schema.defaultValue;
                    } else if constexpr (std::is_same_v<T, int>) {
                        return std::stoi(schema.defaultValue);
                    } else if constexpr (std::is_same_v<T, int64_t>) {
                        return std::stoll(schema.defaultValue);
                    } else if constexpr (std::is_same_v<T, double>) {
                        return std::stod(schema.defaultValue);
                    } else if constexpr (std::is_same_v<T, bool>) {
                        return schema.defaultValue == "true" || schema.defaultValue == "1" || schema.defaultValue == "yes";
                    }
                } catch (const std::exception&) {
                    continue;
                }
            }
        }
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
    pImpl->validationErrors.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    
    // Count by source
    size_t runtimeCount = 0, fileCount = 0, cliCount = 0, envCount = 0, defaultCount = 0;
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Runtime: runtimeCount++; break;
            case ConfigSource::File: fileCount++; break;
            case ConfigSource::CommandLine: cliCount++; break;
            case ConfigSource::Environment: envCount++; break;
            case ConfigSource::Default: defaultCount++; break;
        }
    }
    
    oss << "  Runtime: " << runtimeCount << "\n";
    oss << "  File: " << fileCount << "\n";
    oss << "  Command Line: " << cliCount << "\n";
    oss << "  Environment: " << envCount << "\n";
    oss << "  Default: " << defaultCount << "\n";
    
    // Show validation errors if any
    if (!pImpl->validationErrors.empty()) {
        oss << "\nValidation Errors:\n";
        for (const auto& error : pImpl->validationErrors) {
            oss << "  - " << error << "\n";
        }
    }
    
    return oss.str();
}

bool Config::validate() const {
    pImpl->validationErrors.clear();
    
    for (const auto& entry : pImpl->entries) {
        // Check if key has a schema definition
        if (pImpl->defaultSchemas.count(entry.key)) {
            const auto& schemas = pImpl->defaultSchemas.at(entry.key);
            
            // Find matching schema (exact match or closest match)
            bool schemaFound = false;
            for (const auto& schema : schemas) {
                if (schema.key == entry.key) {
                    schemaFound = true;
                    break;
                }
            }
            
            if (!schemaFound) {
                pImpl->validationErrors.push_back("Unknown configuration key: " + entry.key);
            }
        } else {
            pImpl->validationErrors.push_back("Unknown configuration key: " + entry.key);
        }
    }
    
    return pImpl->validationErrors.empty();
}

std::string Config::getValidationErrors() const {
    std::ostringstream oss;
    for (const auto& error : pImpl->validationErrors) {
        oss << error << "\n";
    }
    return oss.str();
}

void Config::addDefault(const std::string& key, ConfigSource source, const std::string& value, const std::string& description) {
    pImpl->defaultSchemas[key].emplace_back(key, source, value, description, true);
}

std::vector<std::string> Config::getDefaultKeys() {
    static std::vector<std::string> defaultKeys;
    if (defaultKeys.empty()) {
        Config tempConfig;
        defaultKeys = tempConfig.pImpl->defaultSchemas.size() < Config::getInstance().pImpl->defaultSchemas.size() ?
            tempConfig.pImpl->defaultSchemas.size() : Config::getInstance().pImpl->defaultSchemas.size();
    }
    // This is a simplification - proper implementation would need static access
    return std::vector<std::string>();
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
namespace {
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
}

} // namespace nlm

// Global default config instance
Config& Config::getInstance() {
    static Config instance;
    return instance;
}

