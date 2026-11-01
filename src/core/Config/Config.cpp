#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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
        
        // Properly serialize the value based on its type
        std::visit([&](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << v << "\"\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        file << "\"" << v[i] << "\"";
                    } else {
                        file << v[i];
                    }
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << v << "\n";
            }
        }, entry.value);
    }
    
    return true;
}

// Validate configuration values
std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        std::string error = validateEntry(entry);
        if (!error.empty()) {
            errors.push_back(entry.key + ": " + error);
        }
    }
    
    return errors;
}

// Validate a single configuration entry
std::string Config::validateEntry(const ConfigEntry& entry) const {
    // TODO PHASE 3: Implement configuration validation based on key and value type
    // For now, basic validation of non-empty keys and sensible ranges for known parameters
    
    if (entry.key.empty()) {
        return "Empty configuration key";
    }
    
    // Basic validation for known parameters
    if (entry.key == "random_seed") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value && (*value < 0 || *value > 1000000)) {
            return "Random seed should be between 0 and 1,000,000";
        }
    } else if (entry.key == "simulation_timestep") {
        auto value = std::get_if<double>(&entry.value);
        if (value && (*value <= 0.0 || *value > 1.0)) {
            return "Simulation timestep must be positive and less than 1.0";
        }
    } else if (entry.key == "neuron_count") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value && (*value <= 0 || *value > 10000)) {
            return "Neuron count must be between 1 and 10,000";
        }
    } else if (entry.key == "connection_probability") {
        auto value = std::get_if<double>(&entry.value);
        if (value && (*value < 0.0 || *value > 1.0)) {
            return "Connection probability must be between 0.0 and 1.0";
        }
    } else if (entry.key == "stdp_ltp_weight") {
        auto value = std::get_if<double>(&entry.value);
        if (value && (*value < 0.0 || *value > 1.0)) {
            return "STDP LTP weight must be between 0.0 and 1.0";
        }
    } else if (entry.key == "stdp_ltd_weight") {
        auto value = std::get_if<double>(&entry.value);
        if (value && (*value < 0.0 || *value > 1.0)) {
            return "STDP LTD weight must be between 0.0 and 1.0";
        }
    } else if (entry.key == "stdp_time_constant") {
        auto value = std::get_if<double>(&entry.value);
        if (value && (*value <= 0.0 || *value > 100.0)) {
            return "STDP time constant must be positive and less than 100.0";
        }
    } else if (entry.key == "reward_discount_factor") {
        auto value = std::get_if<double>(&entry.value);
        if (value && (*value < 0.0 || *value > 1.0)) {
            return "Reward discount factor must be between 0.0 and 1.0";
        }
    } else if (entry.key == "environment_width" || entry.key == "environment_height") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value && (*value <= 0 || *value > 1000)) {
            return "Environment dimension must be between 1 and 1,000";
        }
    } else if (entry.key == "max_simulation_steps") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value && (*value <= 0 || *value > 1000000)) {
            return "Max simulation steps must be between 1 and 1,000,000";
        }
    } else if (entry.key == "visualization_update_rate") {
        auto value = std::get_if<double>(&entry.value);
        if (value && (*value <= 0.0 || *value > 1000.0)) {
            return "Visualization update rate must be positive and reasonable";
        }
    }
    
    return "";
}

// Get validation errors as a single string
std::string Config::getValidationErrors() const {
    auto errors = validate();
    if (errors.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << "Configuration validation errors:";
    for (const auto& error : errors) {
        oss << "\n  - " << error;
    }
    return oss.str();
}

// Enhanced validation with specific error reporting
bool Config::validateAndSet(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // First set the value
    set(key, value, source);
    
    // Then validate and rollback if invalid
    auto errors = validate();
    if (!errors.empty()) {
        // Rollback: remove the key we just set
        remove(key);
        
        // Report validation errors
        std::cerr << "Configuration validation failed for key '" << key << "':";
        for (const auto& error : errors) {
            std::cerr << "\n  - " << error;
        }
        std::cerr << std::endl;
        return false;
    }
    
    return true;
}

// Check if configuration is valid
bool Config::isValid() const {
    return validate().empty();
}

// Get validation warnings
std::vector<std::string> Config::getWarnings() const {
    std::vector<std::string> warnings;
    
    for (const auto& entry : pImpl->entries) {
        std::string warning = getWarningForEntry(entry);
        if (!warning.empty()) {
            warnings.push_back(entry.key + ": " + warning);
        }
    }
    
    return warnings;
}

// Get warning for a configuration entry
std::string Config::getWarningForEntry(const ConfigEntry& entry) const {
    // TODO PHASE 3: Implement configuration warnings for potential issues
    // For now, just a placeholder
    
    if (entry.key == "neuron_count" && entry.source == ConfigSource::Default) {
        return "Using default neuron count may limit brain complexity";
    } else if (entry.key == "simulation_timestep" && entry.source == ConfigSource::Default) {
        return "Default simulation timestep may not be optimal for your use case";
    }
    
    return "";
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
