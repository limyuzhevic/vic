#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <optional>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> jsonDescriptions;  // Store descriptions from JSON files
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first, then fallback to simple key=value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check if file appears to be JSON (starts with { or [)
    file.seekg(0, std::ios::beg);
    char firstChar;
    if (file.get(firstChar)) {
        file.seekg(0, std::ios::beg);
        // Simple JSON detection - check if file starts with { or contains key-value pairs in JSON format
        std::string firstLine;
        if (std::getline(file, firstLine)) {
            file.seekg(0, std::ios::beg);
            
            bool isJson = false;
            if (firstLine.find('{') != std::string::npos || firstLine.find('[') != std::string::npos) {
                // Try to parse as JSON
                try {
                    auto json = nlohmann::json::parse(file);
                    
                    // Clear existing entries before loading JSON
                    pImpl->entries.clear();
                    pImpl->jsonDescriptions.clear();
                    
                    // Helper function to convert JSON value to ConfigValue
                    auto jsonToConfigValue = [](const nlohmann::json& j) -> ConfigValue {
                        if (j.is_string()) {
                            return j.get<std::string>();
                        } else if (j.is_number_integer()) {
                            return j.get<int64_t>();
                        } else if (j.is_number_unsigned()) {
                            return j.get<uint64_t>();
                        } else if (j.is_number_float()) {
                            return j.get<double>();
                        } else if (j.is_boolean()) {
                            return j.get<bool>();
                        } else if (j.is_array()) {
                            // Convert array to std::vector
                            if (!j.empty() && j[0].is_number_integer()) {
                                std::vector<int64_t> vec;
                                vec.reserve(j.size());
                                for (const auto& item : j) {
                                    vec.push_back(item.get<int64_t>());
                                }
                                return vec;
                            } else if (!j.empty() && j[0].is_number_float()) {
                                std::vector<double> vec;
                                vec.reserve(j.size());
                                for (const auto& item : j) {
                                    vec.push_back(item.get<double>());
                                }
                                return vec;
                            } else if (!j.empty() && j[0].is_string()) {
                                std::vector<std::string> vec;
                                vec.reserve(j.size());
                                for (const auto& item : j) {
                                    vec.push_back(item.get<std::string>());
                                }
                                return vec;
                            }
                            // Default empty vector for unknown array types
                            return std::vector<double>();
                        }
                        // Default to empty string for unknown types
                        return std::string();
                    };
                    
                    // Recursively load JSON data
                    auto loadJsonData = [&jsonToConfigValue, this](const nlohmann::json& j, const std::string& prefix = "") {
                        if (j.is_object()) {
                            for (const auto& [key, value] : j.items()) {
                                std::string fullKey = prefix.empty() ? key : prefix + "." + key;
                                ConfigSource source = ConfigSource::File;
                                
                                // Extract description from JSON comments (not supported in nlohmann::json directly)
                                // For now, use default description
                                std::string description = "Configuration parameter from JSON file";
                                
                                ConfigValue configValue = jsonToConfigValue(value);
                                pImpl->entries.emplace_back(fullKey, configValue, source, description);
                                
                                // Recursively process nested objects
                                if (value.is_object()) {
                                    loadJsonData(value, fullKey);
                                }
                            }
                        } else if (j.is_array()) {
                            // Handle arrays as a single parameter (JSON arrays as values)
                            // This is a design choice - arrays can be individual config values
                            ConfigValue configValue = jsonToConfigValue(j);
                            std::string key = prefix.empty() ? "array_data" : prefix + ".array";
                            pImpl->entries.emplace_back(key, configValue, ConfigSource::File, "Array parameter from JSON file");
                        }
                    };
                    
                    loadJsonData(json);
                    
                    // Store descriptions for later use
                    for (const auto& entry : pImpl->entries) {
                        pImpl->jsonDescriptions[entry.key] = "JSON parameter: " + entry.key;
                    }
                    
                    return true;
                    
                } catch (const nlohmann::json::parse_error& e) {
                    // JSON parsing failed, fallback to simple format
                    file.clear();
                    file.seekg(0, std::ios::beg);
                } catch (const std::exception&) {
                    // Other error, fallback to simple format
                    file.clear();
                    file.seekg(0, std::ios::beg);
                }
            }
        }
    }
    
    // Fallback to simple key=value format
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
            
            // Determine description based on key pattern
            std::string description = "Configuration parameter";
            if (key.find("random_seed") != std::string::npos) {
                description = "Random seed for reproducible experiments";
            } else if (key.find("neuron_count") != std::string::npos) {
                description = "Number of neurons in the brain";
            } else if (key.find("region_count") != std::string::npos) {
                description = "Number of brain regions";
            } else if (key.find("connection_probability") != std::string::npos) {
                description = "Connection probability for random connectivity";
            } else if (key.find("stdp_") != std::string::npos) {
                description = "Spike-Timing-Dependent Plasticity parameters";
            } else if (key.find("synaptogenesis") != std::string::npos) {
                description = "Synaptogenesis rate (structural plasticity)";
            } else if (key.find("pruning_rate") != std::string::npos) {
                description = "Pruning rate (structural plasticity)";
            } else if (key.find("dopamine") != std::string::npos) {
                description = "Dopamine neuromodulation level";
            } else if (key.find("curiosity") != std::string::npos) {
                description = "Curiosity neuromodulation level";
            } else if (key.find("novelty") != std::string::npos) {
                description = "Novelty detection signal";
            } else if (key.find("simulation_timestep") != std::string::npos) {
                description = "Simulation timestep in seconds";
            } else if (key.find("log_level") != std::string::npos) {
                description = "Logging verbosity level";
            }
            
            set(key, value, ConfigSource::File, description);
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
