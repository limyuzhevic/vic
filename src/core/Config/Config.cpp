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
            
            // Validate key before setting
            if (!validateKey(key)) {
                NLM_LOG_WARNING("Invalid configuration key: " + key);
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const ConfigValidationError& e) {
                NLM_LOG_WARNING(std::string("Configuration validation error for key '" + key + "': " + e.what()));
            }
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key before setting
    if (!validateKey(key)) {
        throw ConfigValidationError("Invalid configuration key: " + key);
    }
    
    // Validate value if key exists and we have a default value to compare against
    if (has(key)) {
        auto defaultVal = getDefaultValue(key);
        if (defaultVal.has_value()) {
            if (!validateValue(key, value)) {
                throw ConfigValidationError("Invalid configuration value for key '" + key + "': " + 
                    "value type or range not valid for this key");
            }
        }
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

bool Config::validateKey(const std::string& key) {
    static const std::vector<std::string> validKeys = {
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
    
    return std::find(validKeys.begin(), validKeys.end(), key) != validKeys.end();
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) {
    // Basic validation based on key
    if (key == "random_seed") {
        return std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) >= 0;
    } else if (key == "simulation_timestep") {
        return std::holds_alternative<double>(value) && std::get<double>(value) > 0.0;
    } else if (key == "neuron_count") {
        return std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) > 0;
    } else if (key == "region_count") {
        return std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) > 0;
    } else if (key == "connection_probability") {
        return std::holds_alternative<double>(value) && std::get<double>(value) >= 0.0 && std::get<double>(value) <= 1.0;
    } else if (key == "plasticity_learning_rate") {
        return std::holds_alternative<double>(value) && std::get<double>(value) >= 0.0;
    } else if (key == "stdp_ltp_weight" || key == "stdp_ltd_weight") {
        return std::holds_alternative<double>(value) && std::get<double>(value) >= 0.0;
    } else if (key == "stdp_time_constant") {
        return std::holds_alternative<double>(value) && std::get<double>(value) > 0.0;
    } else if (key == "development_synaptogenesis_rate" || key == "development_pruning_rate") {
        return std::holds_alternative<double>(value) && std::get<double>(value) >= 0.0;
    } else if (key == "dopamine_baseline") {
        return std::holds_alternative<double>(value) && std::get<double>(value) >= 0.0;
    } else if (key == "reward_discount_factor") {
        return std::holds_alternative<double>(value) && std::get<double>(value) >= 0.0 && std::get<double>(value) <= 1.0;
    } else if (key == "environment_width" || key == "environment_height") {
        return std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) > 0;
    } else if (key == "log_to_file") {
        return std::holds_alternative<bool>(value);
    } else if (key == "max_simulation_steps") {
        return std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) > 0;
    } else if (key == "simulation_time_limit") {
        return std::holds_alternative<double>(value) && std::get<double>(value) >= 0.0;
    } else if (key == "visualization_enabled") {
        return std::holds_alternative<bool>(value);
    } else if (key == "visualization_update_rate") {
        return std::holds_alternative<double>(value) && std::get<double>(value) > 0.0;
    }
    
    // For other keys, accept any type
    return true;
}

std::vector<std::string> Config::getValidKeys() {
    return {
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

std::optional<ConfigValue> Config::getDefaultValue(const std::string& key) {
    // Return default values for known keys
    if (key == "random_seed") {
        return ConfigValue(int64_t(42));
    } else if (key == "simulation_timestep") {
        return ConfigValue(0.001);
    } else if (key == "neuron_count") {
        return ConfigValue(int64_t(1000));
    } else if (key == "region_count") {
        return ConfigValue(int64_t(2));
    } else if (key == "connection_probability") {
        return ConfigValue(0.1f);
    } else if (key == "plasticity_learning_rate") {
        return ConfigValue(0.01);
    } else if (key == "stdp_ltp_weight") {
        return ConfigValue(0.01f);
    } else if (key == "stdp_ltd_weight") {
        return ConfigValue(0.012f);
    } else if (key == "stdp_time_constant") {
        return ConfigValue(20.0f);
    } else if (key == "development_synaptogenesis_rate") {
        return ConfigValue(0.001f);
    } else if (key == "development_pruning_rate") {
        return ConfigValue(0.0001f);
    } else if (key == "dopamine_baseline") {
        return ConfigValue(0.1f);
    } else if (key == "reward_discount_factor") {
        return ConfigValue(0.99f);
    } else if (key == "environment_name") {
        return ConfigValue(std::string("GridWorld"));
    } else if (key == "environment_width") {
        return ConfigValue(int64_t(10));
    } else if (key == "environment_height") {
        return ConfigValue(int64_t(10));
    } else if (key == "log_level") {
        return ConfigValue(std::string("INFO"));
    } else if (key == "log_to_file") {
        return ConfigValue(false);
    } else if (key == "log_filename") {
        return ConfigValue(std::string("nlm.log"));
    } else if (key == "max_simulation_steps") {
        return ConfigValue(int64_t(10000));
    } else if (key == "simulation_time_limit") {
        return ConfigValue(0.0f);
    } else if (key == "visualization_enabled") {
        return ConfigValue(false);
    } else if (key == "visualization_update_rate") {
        return ConfigValue(30.0f);
    }
    
    return std::nullopt;
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
