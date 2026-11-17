#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

bool Config::validateKey(const std::string& key) const {
    if (key.empty()) {
        NLM_LOG_ERROR("Configuration key cannot be empty");
        return false;
    }
    
    // Check for invalid characters in key
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
            NLM_LOG_ERROR("Invalid character in configuration key: " + key);
            return false;
        }
    }
    
    return true;
}

// Helper function to validate configuration value type
bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // No validation needed for ConfigValue variants as they're already typed
    return true;
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON parsing first
    std::ifstream jsonFile(filepath);
    if (jsonFile.is_open()) {
        try {
            nlohmann::json jsonData;
            jsonFile >> jsonData;
            
            // Clear existing entries
            pImpl->entries.clear();
            
            // Parse JSON and populate configuration
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                std::string description = "Configuration key: " + key;
                
                // Convert JSON value to ConfigValue
                ConfigValue configValue;
                const auto& value = *it;
                
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
                    // Handle arrays
                    nlohmann::json::array_t jsonArray = value.get<nlohmann::json::array_t>();
                    if (!jsonArray.empty()) {
                        // Check array element type
                        if (jsonArray[0].is_string()) {
                            std::vector<std::string> strArray;
                            for (const auto& item : jsonArray) {
                                strArray.push_back(item.get<std::string>());
                            }
                            configValue = strArray;
                        } else if (jsonArray[0].is_number_integer()) {
                            std::vector<int64_t> intArray;
                            for (const auto& item : jsonArray) {
                                intArray.push_back(item.get<int64_t>());
                            }
                            configValue = intArray;
                        } else if (jsonArray[0].is_number_float()) {
                            std::vector<double> doubleArray;
                            for (const auto& item : jsonArray) {
                                doubleArray.push_back(item.get<double>());
                            }
                            configValue = doubleArray;
                        } else {
                            NLM_LOG_ERROR("Unsupported array type in JSON configuration for key: " + key);
                            continue;
                        }
                    } else {
                        // Empty array
                        configValue = std::vector<std::string>();
                    }
                } else {
                    NLM_LOG_ERROR("Unsupported JSON value type for key: " + key);
                    continue;
                }
                
                // Add validation before setting key-value pairs
                if (!validateKey(key)) {
                    NLM_LOG_ERROR("Invalid key in JSON configuration: " + key);
                    continue;
                }
                
                if (!validateValue(key, configValue)) {
                    NLM_LOG_ERROR("Invalid value for configuration key: " + key);
                    continue;
                }
                
                set(key, configValue, ConfigSource::File);
            }
            
            NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to parse JSON configuration file " + filepath + ": " + std::string(e.what()));
            return false;
        }
    }
    
    // Fall back to simple key=value format for backward compatibility
    NLM_LOG_INFO("JSON parsing failed or file not JSON, falling back to simple key=value format for: " + filepath);
    
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
            
            // Add validation before setting key-value pairs
            if (!validateKey(key)) {
                NLM_LOG_ERROR("Invalid key in configuration file: " + key);
                continue;
            }
            
            // Convert string to appropriate ConfigValue type
            ConfigValue configValue;
            // Try to infer type from string value
            if (key == "random_seed") {
                // Try to parse as integer
                try {
                    configValue = std::stoll(value);
                } catch (...) {
                    configValue = value;
                }
            } else if (key == "simulation_timestep" || key == "stdp_ltp_weight" || 
                       key == "stdp_ltd_weight" || key == "stdp_time_constant" ||
                       key == "development_synaptogenesis_rate" ||
                       key == "development_pruning_rate" ||
                       key == "dopamine_baseline" ||
                       key == "reward_discount_factor" ||
                       key == "visualization_update_rate") {
                // Try to parse as float
                try {
                    configValue = std::stod(value);
                } catch (...) {
                    configValue = value;
                }
            } else if (key == "neuron_count" || key == "region_count" ||
                       key == "connection_probability" || key == "max_simulation_steps") {
                // Try to parse as integer
                try {
                    configValue = std::stoll(value);
                } catch (...) {
                    configValue = value;
                }
            } else if (key == "plasticity_learning_rate" || key == "dopamine_baseline") {
                // Try to parse as float
                try {
                    configValue = std::stod(value);
                } catch (...) {
                    configValue = value;
                }
            } else if (key == "log_level" || key == "log_to_file") {
                // Try to parse as boolean (case-insensitive)
                std::string lowerValue = value;
                std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
                if (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes" || lowerValue == "on") {
                    configValue = true;
                } else if (lowerValue == "false" || lowerValue == "0" || lowerValue == "no" || lowerValue == "off") {
                    configValue = false;
                } else {
                    configValue = value;
                }
            } else {
                // Default to string
                configValue = value;
            }
            
            if (!validateValue(key, configValue)) {
                NLM_LOG_ERROR("Invalid value for configuration key: " + key);
                continue;
            }
            
            set(key, configValue, ConfigSource::File);
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

// Helper function to convert ConfigValue to string representation
std::string configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << configValueToString(entry.value) << "\n";
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = pImpl->keyToIndex.find(key);
    
    if (it != pImpl->keyToIndex.end()) {
        // Key exists - update the entry in the vector
        size_t index = it->second;
        pImpl->entries[index].value = value;
        pImpl->entries[index].source = source;
    } else {
        // Key doesn't exist - add new entry
        size_t index = pImpl->entries.size();
        pImpl->keyToIndex[key] = index;
        pImpl->entries.emplace_back(key, value, source);
    }
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

bool Config::has(const std::string& key) const {
    return pImpl->keyToIndex.find(key) != pImpl->keyToIndex.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        size_t index = it->second;
        // Remove from vector
        pImpl->entries.erase(pImpl->entries.begin() + index);
        // Remove from hash map
        pImpl->keyToIndex.erase(it);
        
        // Update indices for all entries that come after the removed one
        for (auto& [k, idx] : pImpl->keyToIndex) {
            if (idx > index) {
                idx--;
            }
        }
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->keyToIndex.find(key);
    if (it == pImpl->keyToIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    const ConfigEntry& entry = pImpl->entries[index];
    
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, entry.value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
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
