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

// Validate a single configuration value
bool Config::validateEntry(const ConfigEntry& entry) const {
    std::string key = entry.key;
    std::string valueStr = "";
    
    // Convert value to string for validation
    std::visit([&valueStr](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            valueStr = val;
        } else if constexpr (std::is_same_v<T, int>) {
            valueStr = std::to_string(val);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            valueStr = std::to_string(val);
        } else if constexpr (std::is_same_v<T, double>) {
            valueStr = std::to_string(val);
        } else if constexpr (std::is_same_v<T, bool>) {
            valueStr = val ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            valueStr = "[int]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            valueStr = "[double]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            valueStr = "[string]";
        }
    }, entry.value);
    
    // Key validation
    if (key.empty()) {
        NLM_LOG_ERROR("Configuration key cannot be empty");
        return false;
    }
    
    // Key character validation (alphanumeric and underscores)
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            NLM_LOG_ERROR("Invalid character in configuration key: '" + std::string(1, c) + "'. Keys must contain only alphanumeric characters, underscores, or hyphens");
            return false;
        }
    }
    
    // Value validation based on key
    if (key == "neuron_count" || key == "region_count") {
        try {
            int64_t val = std::get<int64_t>(entry.value);
            if (val <= 0) {
                NLM_LOG_ERROR("Invalid " + key + ": " + valueStr + ". Must be greater than 0");
                return false;
            }
        } catch (const std::bad_variant_access&) {
            NLM_LOG_ERROR("Invalid value type for " + key + ". Expected integer");
            return false;
        }
    } else if (key == "connection_probability" || key == "dopamine_baseline" || key == "plasticity_learning_rate" || 
               key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || key == "stdp_time_constant" ||
               key == "development_synaptogenesis_rate" || key == "development_pruning_rate" ||
               key == "dopamine_excitability" || key == "dopamine_plasticity" ||
               key == "reward_discount_factor" || key == "simulation_timestep" ||
               key == "visualization_update_rate") {
        try {
            double val = std::get<double>(entry.value);
            if (val < 0.0) {
                NLM_LOG_ERROR("Invalid " + key + ": " + valueStr + ". Must be non-negative");
                return false;
            }
            
            // Additional range validation for specific parameters
            if (key == "connection_probability" || key == "dopamine_baseline" || key == "plasticity_learning_rate" ||
                key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || key == "dopamine_excitability" ||
                key == "dopamine_plasticity" || key == "reward_discount_factor") {
                if (val > 1.0) {
                    NLM_LOG_ERROR("Invalid " + key + ": " + valueStr + ". Must be between 0.0 and 1.0");
                    return false;
                }
            }
            
            if (key == "simulation_timestep" || key == "stdp_time_constant" || key == "visualization_update_rate") {
                if (val <= 0.0 || val > 1000.0) {
                    NLM_LOG_ERROR("Invalid " + key + ": " + valueStr + ". Must be between 0.001 and 1000.0");
                    return false;
                }
            }
        } catch (const std::bad_variant_access&) {
            NLM_LOG_ERROR("Invalid value type for " + key + ". Expected floating point number");
            return false;
        }
    } else if (key == "log_level" || key == "log_to_file") {
        try {
            int val = std::get<int>(entry.value);
            if (key == "log_level" && (val < 0 || val > 3)) {
                NLM_LOG_ERROR("Invalid log_level: " + valueStr + ". Must be between 0 and 3");
                return false;
            }
        } catch (const std::bad_variant_access&) {
            NLM_LOG_ERROR("Invalid value type for " + key + ". Expected integer");
            return false;
        }
    } else if (key == "max_simulation_steps") {
        try {
            int64_t val = std::get<int64_t>(entry.value);
            if (val < 0 || val > 1000000) {
                NLM_LOG_ERROR("Invalid max_simulation_steps: " + valueStr + ". Must be between 0 and 1,000,000");
                return false;
            }
        } catch (const std::bad_variant_access&) {
            NLM_LOG_ERROR("Invalid value type for " + key + ". Expected integer");
            return false;
        }
    }
    
    return true;
}

// Validate all configuration entries
bool Config::validateAll() const {
    NLM_LOG_INFO("Validating all configuration entries...");
    
    bool allValid = true;
    for (const auto& entry : pImpl->entries) {
        if (!validateEntry(entry)) {
            NLM_LOG_ERROR("Validation failed for key: " + entry.key);
            allValid = false;
        }
    }
    
    // Check for critical missing values with sensible defaults
    std::vector<std::string> criticalKeys = {
        "neuron_count", "region_count", "connection_probability", 
        "simulation_timestep", "random_seed"
    };
    
    for (const auto& key : criticalKeys) {
        if (!has(key)) {
            NLM_LOG_WARNING("Missing critical configuration key: '" + key + "'. Using sensible default.");
            // Set sensible defaults
            if (key == "neuron_count") {
                set(key, static_cast<int64_t>(500), ConfigSource::Default);
            } else if (key == "region_count") {
                set(key, static_cast<int64_t>(1), ConfigSource::Default);
            } else if (key == "connection_probability") {
                set(key, 0.15f, ConfigSource::Default);
            } else if (key == "simulation_timestep") {
                set(key, 0.001, ConfigSource::Default);
            } else if (key == "random_seed") {
                set(key, static_cast<int64_t>(42), ConfigSource::Default);
            }
        }
    }
    
    if (allValid) {
        NLM_LOG_INFO("All configuration entries are valid.");
    } else {
        NLM_LOG_ERROR("Configuration validation failed.");
    }
    
    return allValid;
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
