#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <optional>

// Include nlohmann JSON library for proper JSON parsing
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if nlohmann/json is available and try to parse as JSON first
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Try to parse as JSON
        nlohmann::json jsonData;
        try {
            file >> jsonData;
        } catch (...) {
            file.close();
            // Fall back to legacy key=value format
            loadLegacyFile(filepath);
            return true;
        }
        
        file.close();
        
        // Convert JSON to Config entries
        clear(); // Clear existing entries
        
        // Recursively process JSON to handle nested structures
        std::function<void(const nlohmann::json&, const std::string&, bool)> processJsonNode;
        processJsonNode = [&](const nlohmann::json& node, const std::string& prefix, bool isNested) {
            if (node.is_object()) {
                for (auto it = node.begin(); it != node.end(); ++it) {
                    std::string key = prefix + "." + it.key();
                    if (isNested) {
                        // Remove leading dot for nested keys
                        key = key.substr(1);
                    }
                    processJsonNode(it.value(), key, true);
                }
            } else if (node.is_array()) {
                for (size_t i = 0; i < node.size(); ++i) {
                    std::string key = prefix + "[" + std::to_string(i) + "]";
                    processJsonNode(node[i], key, true);
                }
            } else {
                // Leaf node - convert to appropriate ConfigValue type
                setConfigEntryFromJson(key, node);
            }
        };
        
        // Start processing from root
        processJsonNode(jsonData, "", false);
        
        // Add descriptions based on key prefixes
        for (auto& entry : pImpl->entries) {
            if (entry.description.empty()) {
                if (entry.key.find("brain.") != std::string::npos) {
                    if (entry.key == "brain.neuron_count") entry.description = "Number of neurons in the brain";
                    else if (entry.key == "brain.synapse_density") entry.description = "Synapse connectivity density (0-1)";
                    else if (entry.key == "brain.connection_probability") entry.description = "Probability of connection between neurons (0-1)";
                    else if (entry.key == "brain.v_thresh") entry.description = "Neuron threshold potential (mV)";
                    else if (entry.key == "brain.v_rest") entry.description = "Resting potential (mV)";
                    else if (entry.key == "brain.v_reset") entry.description = "Reset potential after spike (mV)";
                    else if (entry.key == "brain.tau_mem") entry.description = "Membrane time constant (ms)";
                    else if (entry.key == "brain.tau_ref") entry.description = "Refractory period (ms)";
                    else entry.description = "Brain configuration parameter";
                } else if (entry.key.find("plasticity.") != std::string::npos) {
                    entry.description = "Plasticity rule parameter";
                } else if (entry.key.find("neuromod.") != std::string::npos) {
                    entry.description = "Neuromodulation parameter";
                } else if (entry.key.find("reward.") != std::string::npos) {
                    entry.description = "Reward-related parameter";
                } else if (entry.key.find("development.") != std::string::npos) {
                    entry.description = "Developmental process parameter";
                } else if (entry.key.find("environment.") != std::string::npos) {
                    entry.description = "Environment setting";
                } else if (entry.key == "random_seed") {
                    entry.description = "Random seed for reproducible experiments";
                } else if (entry.key == "simulation_timestep") {
                    entry.description = "Simulation timestep in seconds";
                } else if (entry.key == "log_level") {
                    entry.description = "Logging verbosity level";
                } else {
                    entry.description = "Configuration parameter";
                }
            }
        }
        
        return true;
        
    } catch (...) {
        // If JSON parsing fails, fall back to legacy format
        loadLegacyFile(filepath);
        return true;
    }
}

// Legacy file loading method for backward compatibility
void Config::loadLegacyFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return;
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
}

void Config::setConfigEntryFromJson(const std::string& key, const nlohmann::json& jsonValue) {
    // Determine the appropriate type for the value
    if (jsonValue.is_boolean()) {
        set(key, jsonValue.get<bool>(), ConfigSource::File);
    } else if (jsonValue.is_number_integer()) {
        set(key, jsonValue.get<int64_t>(), ConfigSource::File);
    } else if (jsonValue.is_number_float()) {
        set(key, jsonValue.get<double>(), ConfigSource::File);
    } else if (jsonValue.is_string()) {
        set(key, jsonValue.get<std::string>(), ConfigSource::File);
    } else if (jsonValue.is_array()) {
        // Try to determine if it's an int array or string array
        bool allNumbers = true;
        bool allIntegers = true;
        
        for (const auto& item : jsonValue) {
            if (!item.is_number()) {
                allNumbers = false;
                break;
            }
            if (item.is_number_integer()) {
                // Integer is fine
            } else {
                allIntegers = false;
            }
        }
        
        if (allNumbers && allIntegers) {
            std::vector<int64_t> intArray;
            for (const auto& item : jsonValue) {
                intArray.push_back(item.get<int64_t>());
            }
            set(key, intArray, ConfigSource::File);
        } else if (allNumbers) {
            std::vector<double> doubleArray;
            for (const auto& item : jsonValue) {
                doubleArray.push_back(item.get<double>());
            }
            set(key, doubleArray, ConfigSource::File);
        } else {
            // Could be string array or other - store as JSON string for now
            set(key, jsonValue.dump(), ConfigSource::File);
        }
    } else if (jsonValue.is_null()) {
        set(key, "null", ConfigSource::File);
    } else {
        // Fallback to JSON string representation
        set(key, jsonValue.dump(), ConfigSource::File);
    }
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
        
        // Helper lambda to convert ConfigValue to string representation
        auto valueToString = [](const ConfigValue& v) -> std::string {
            std::ostringstream oss;
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else if constexpr (std::is_arithmetic_v<T>) {
                    oss << arg;
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
                    oss << "\"" << "unknown_type" << "\"";
                }
            }, v);
            return oss.str();
        };
        
        file << entry.key << " = " << valueToString(entry.value) << "\n";
    }
    
    return true;
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
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
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
    }, it->value);
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
