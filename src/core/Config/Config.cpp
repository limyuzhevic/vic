#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <string_view>
#include <memory>
#include <variant>
#include <limits>
#include <chrono>
#include <cstring>

namespace nlm {

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <string_view>
#include <memory>
#include <variant>
#include <limits>
#include <chrono>
#include <cstring>
#include <map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigEntry> defaultValues;  // Store default values for easy lookup
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with essential default values
    initializeDefaultValues();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultValues() {
    // Essential default values for NLM brain operation
    pImpl->defaultValues = {
        {"random_seed", ConfigEntry("random_seed", ConfigValue(42), ConfigSource::Default, "Random seed for reproducible experiments")},
        {"simulation_timestep", ConfigEntry("simulation_timestep", ConfigValue(0.001), ConfigSource::Default, "Simulation timestep in seconds")},
        {"neuron_count", ConfigEntry("neuron_count", ConfigValue(static_cast<int64_t>(1000)), ConfigSource::Default, "Number of neurons in the brain")},
        {"region_count", ConfigEntry("region_count", ConfigValue(static_cast<int64_t>(2)), ConfigSource::Default, "Number of brain regions")},
        {"connection_probability", ConfigEntry("connection_probability", ConfigValue(0.1f), ConfigSource::Default, "Connection probability for random connectivity")},
        {"plasticity_learning_rate", ConfigEntry("plasticity_learning_rate", ConfigValue(0.01f), ConfigSource::Default, "Plasticity learning rate")},
        {"stdp_ltp_weight", ConfigEntry("stdp_ltp_weight", ConfigValue(0.01f), ConfigSource::Default, "STDP LTP weight")},
        {"stdp_ltd_weight", ConfigEntry("stdp_ltd_weight", ConfigValue(0.012f), ConfigSource::Default, "STDP LTD weight")},
        {"stdp_time_constant", ConfigEntry("stdp_time_constant", ConfigValue(20.0f), ConfigSource::Default, "STDP time constant")},
        {"development_synaptogenesis_rate", ConfigEntry("development_synaptogenesis_rate", ConfigValue(0.001f), ConfigSource::Default, "Development synaptogenesis rate")},
        {"development_pruning_rate", ConfigEntry("development_pruning_rate", ConfigValue(0.0001f), ConfigSource::Default, "Development pruning rate")},
        {"dopamine_baseline", ConfigEntry("dopamine_baseline", ConfigValue(0.1f), ConfigSource::Default, "Dopamine baseline level")},
        {"reward_discount_factor", ConfigEntry("reward_discount_factor", ConfigValue(0.99), ConfigSource::Default, "Reward discount factor")},
        {"environment_name", ConfigEntry("environment_name", ConfigValue("GridWorld"), ConfigSource::Default, "Environment name")},
        {"environment_width", ConfigEntry("environment_width", ConfigValue(static_cast<int64_t>(10)), ConfigSource::Default, "Environment width")},
        {"environment_height", ConfigEntry("environment_height", ConfigValue(static_cast<int64_t>(10)), ConfigSource::Default, "Environment height")},
        {"log_level", ConfigEntry("log_level", ConfigValue("INFO"), ConfigSource::Default, "Log level")},
        {"max_simulation_steps", ConfigEntry("max_simulation_steps", ConfigValue(static_cast<int64_t>(10000)), ConfigSource::Default, "Maximum simulation steps")},
        {"visualization_enabled", ConfigEntry("visualization_enabled", ConfigValue(false), ConfigSource::Default, "Enable visualization")},
        {"visualization_update_rate", ConfigEntry("visualization_update_rate", ConfigValue(30.0), ConfigSource::Default, "Visualization update rate (Hz)")},
        {"replay_interval", ConfigEntry("replay_interval", ConfigValue(static_cast<int64_t>(100)), ConfigSource::Default, "Memory replay interval (steps)")},
        {"consolidation_interval", ConfigEntry("consolidation_interval", ConfigValue(static_cast<int64_t>(1000)), ConfigSource::Default, "Memory consolidation interval (steps)")},
    };
    
    // Copy defaults to entries
    for (const auto& pair : pImpl->defaultValues) {
        pImpl->entries.push_back(pair.second);
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first
    try {
        // Use jsoncpp or similar if available, otherwise fall back to simple parsing
        // For now, continue with simple parsing but improve error handling
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        size_t lineNumber = 0;
        while (std::getline(file, line)) {
            ++lineNumber;
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
            } else {
                // Log warning about malformed line
                NLM_LOG_WARNING("Config: Malformed line " + std::to_string(lineNumber) + ": " + line);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Config: Failed to load from file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!argv) {
        NLM_LOG_ERROR("Config: Invalid arguments (nullptr)");
        return false;
    }
    
    bool success = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (key.empty()) {
                    NLM_LOG_ERROR("Config: Empty key in command line argument: " + arg);
                    success = false;
                    continue;
                }
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_ERROR("Config: Malformed command line argument (missing =): " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (key.empty()) {
                NLM_LOG_ERROR("Config: Empty key in command line argument: " + arg);
                success = false;
                continue;
            }
            set(key, value, ConfigSource::CommandLine);
        } else {
            NLM_LOG_ERROR("Config: Unrecognized command line argument format: " + arg);
            success = false;
        }
    }
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Config: Failed to open file for saving: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File (Saved at " << std::to_string(std::time(nullptr)) << ")\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        // Format the value based on its type
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_arithmetic_v<T>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        file << "\"" << arg[i] << "\"";
                    } else {
                        file << arg[i];
                    }
                }
                file << "]";
            } else {
                file << "\"unknown_type\"";
            }
        }, entry.value);
        file << "\n\n";
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_ERROR("Config: Cannot set empty key");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Config: Updated existing key '" + key + "'")
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Config: Added new key '" + key + "'")
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
    if (key.empty()) {
        NLM_LOG_ERROR("Config: Cannot check empty key");
        return false;
    }
    return config_helpers::keyExists(key, pImpl->entries);
}

void Config::remove(const std::string& key) {
    if (key.empty()) {
        NLM_LOG_ERROR("Config: Cannot remove empty key");
        return;
    }
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    NLM_LOG_DEBUG("Config: Removed key '" + key + "'");
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
    NLM_LOG_DEBUG("Config: Cleared all configuration entries");
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (Phase 6 - Enhanced):
";
    oss << "Total entries: " << pImpl->entries.size() << 
        " (" << pImpl->defaultValues.size() << " defaults available)
";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
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
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << "\"unknown_type\"";
            }
        }, entry.value);
        oss << "] (" << config_helpers::sourceToString(entry.source) << ") - " << entry.description << "\n";
    }
    
    oss << "\nConfiguration is " << (validateConfig(pImpl->entries) ? "VALID" : "INVALID");
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

namespace nlm {

// Validation function implementation
bool validateConfig(const std::vector<ConfigEntry>& entries) {
    if (entries.empty()) {
        NLM_LOG_WARNING("Config: Empty configuration - no settings found");
        return false;
    }
    
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // Check for duplicate keys (shouldn't happen with current set logic)
    std::vector<std::string> keys;
    for (const auto& entry : entries) {
        if (std::find(keys.begin(), keys.end(), entry.key) != keys.end()) {
            errors.push_back("Duplicate key: '" + entry.key + "'");
        }
        keys.push_back(entry.key);
    }
    
    // Check for required essential keys
    const std::vector<std::string> essentialKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    for (const auto& essentialKey : essentialKeys) {
        if (!config_helpers::keyExists(essentialKey, entries)) {
            warnings.push_back("Missing recommended configuration key: '" + essentialKey + "'");
        }
    }
    
    // Log all warnings
    for (const auto& warning : warnings) {
        NLM_LOG_WARNING("Config: " + warning);
    }
    
    // Log all errors
    for (const auto& error : errors) {
        NLM_LOG_ERROR("Config: " + error);
    }
    
    // Return true if no errors (warnings are okay)
    return errors.empty();
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
