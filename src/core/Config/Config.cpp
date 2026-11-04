#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <stdexcept>
#include <cmath>

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
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

namespace nlm {

struct ConfigValidationRules {
    struct ValidationRule {
        std::string key;
        std::string validationType;
        std::string description;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
    };
    
    static std::vector<ValidationRule> getDefaultRules() {
        return {
            // Numeric range validation
            {"neuron_count", "range", "Number of neurons must be positive", 
             [](const ConfigValue& v) {
                 if (const auto* val = std::get_if<int64_t>(&v)) return (*val > 0);
                 if (const auto* val = std::get_if<int>(&v)) return (*val > 0);
                 return false;
             }, "neuron_count must be a positive integer"},
            
            {"region_count", "range", "Number of regions must be positive", 
             [](const ConfigValue& v) {
                 if (const auto* val = std::get_if<int64_t>(&v)) return (*val > 0);
                 if (const auto* val = std::get_if<int>(&v)) return (*val > 0);
                 return false;
             }, "region_count must be a positive integer"},
            
            {"connection_probability", "range", "Connection probability must be between 0 and 1", 
             [](const ConfigValue& v) {
                 if (const auto* val = std::get_if<double>(&v)) return (*val >= 0.0 && *val <= 1.0);
                 if (const auto* val = std::get_if<float>(&v)) return (*val >= 0.0f && *val <= 1.0f);
                 return false;
             }, "connection_probability must be between 0 and 1"},
            
            {"random_seed", "range", "Random seed must be non-negative", 
             [](const ConfigValue& v) {
                 if (const auto* val = std::get_if<int64_t>(&v)) return (*val >= 0);
                 if (const auto* val = std::get_if<int>(&v)) return (*val >= 0);
                 return false;
             }, "random_seed must be a non-negative integer"},
            
            // Positive values for rates
            {"plasticity_learning_rate", "positive", "Learning rate must be positive", 
             [](const ConfigValue& v) {
                 if (const auto* val = std::get_if<double>(&v)) return (*val > 0.0);
                 if (const auto* val = std::get_if<float>(&v)) return (*val > 0.0f);
                 return false;
             }, "plasticity_learning_rate must be positive"},
            
            {"synaptogenesis_rate", "positive", "Synaptogenesis rate must be positive", 
             [](const ConfigValue& v) {
                 if (const auto* val = std::get_if<double>(&v)) return (*val >= 0.0);
                 if (const auto* val = std::get_if<float>(&v)) return (*val >= 0.0f);
                 return false;
             }, "synaptogenesis_rate must be non-negative"},
            
            {"pruning_rate", "positive", "Pruning rate must be positive", 
             [](const ConfigValue& v) {
                 if (const auto* val = std::get_if<double>(&v)) return (*val >= 0.0);
                 if (const auto* val = std::get_if<float>(&v)) return (*val >= 0.0f);
                 return false;
             }, "pruning_rate must be non-negative"},
            
            // STDP parameters
            {"stdp_ltp_weight", "range", "LTP weight must be between 0 and 1", 
             [](const ConfigValue& v) {
                 if (const auto* val = std::get_if<double>(&v)) return (*val >= 0.0 && *val <= 1.0);
                 if (const auto* val = std::get_if<float>(&v)) return (*val >= 0.0f && *val <= 1.0f);
                 return false;
             }, "stdp_ltp_weight must be between 0 and 1"},
            
            {"stdp_ltd_weight", "range", "LTD weight must be between 0 and 1", 
             [](const ConfigValue& v) {
                 if (const auto* val = std::get_if<double>(&v)) return (*val >= 0.0 && *val <= 1.0);
                 if (const auto* val = std::get_if<float>(&v)) return (*val >= 0.0f && *val <= 1.0f);
                 return false;
             }, "stdp_ltd_weight must be between 0 and 1"},
            
            {"stdp_time_constant", "range", "STDP time constant must be positive", 
             [](const ConfigValue& v) {
                 if (const auto* val = std::get_if<double>(&v)) return (*val > 0.0);
                 if (const auto* val = std::get_if<float>(&v)) return (*val > 0.0f);
                 return false;
             }, "stdp_time_constant must be positive"},
            
            // Environment dimensions
            {"environment_width", "range", "Environment width must be positive", 
             [](const ConfigValue& v) {
                 if (const auto* val = std::get_if<int64_t>(&v)) return (*val > 0);
                 if (const auto* val = std::get_if<int>(&v)) return (*val > 0);
                 return false;
             }, "environment_width must be positive"},
            
            {"environment_height", "range", "Environment height must be positive", 
             [](const ConfigValue& v) {
                 if (const auto* val = std::get_if<int64_t>(&v)) return (*val > 0);
                 if (const auto* val = std::get_if<int>(&v)) return (*val > 0);
                 return false;
             }, "environment_height must be positive"},
            
            // Simulation parameters
            {"simulation_timestep", "range", "Simulation timestep must be positive", 
             [](const ConfigValue& v) {
                 if (const auto* val = std::get_if<double>(&v)) return (*val > 0.0);
                 if (const auto* val = std::get_if<float>(&v)) return (*val > 0.0f);
                 return false;
             }, "simulation_timestep must be positive"},
            
            {"max_simulation_steps", "range", "Max steps must be positive", 
             [](const ConfigValue& v) {
                 if (const auto* val = std::get_if<int64_t>(&v)) return (*val > 0);
                 if (const auto* val = std::get_if<int>(&v)) return (*val > 0);
                 return false;
             }, "max_simulation_steps must be positive"},
            
            // Log level validation
            {"log_level", "enum", "Log level must be valid", 
             [](const ConfigValue& v) {
                 if (const auto* val = std::get_if<std::string>(&v)) {
                     static const std::vector<std::string> validLevels = {"off", "error", "warning", "info", "debug"};
                     return std::find(validLevels.begin(), validLevels.end(), *val) != validLevels.end();
                 }
                 return false;
             }, "log_level must be one of: off, error, warning, info, debug"},
        };
    }
}
;

std::string Config::getDefaultDescription(const std::string& key) {
    static const std::unordered_map<std::string, std::string> descriptions = {
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Probability of connection between neurons"},
        {"random_seed", "Random seed for reproducible experiments"},
        {"plasticity_learning_rate", "Learning rate for plasticity rules"},
        {"stdp_ltp_weight", "STDP long-term potentiation weight"},
        {"stdp_ltd_weight", "STDP long-term depression weight"},
        {"stdp_time_constant", "STDP time constant (ms)"},
        {"synaptogenesis_rate", "Rate of new synapse formation"},
        {"pruning_rate", "Rate of synapse elimination"},
        {"environment_width", "Width of the simulated environment"},
        {"environment_height", "Height of the simulated environment"},
        {"simulation_timestep", "Time step for simulation (seconds)"},
        {"max_simulation_steps", "Maximum number of simulation steps"},
        {"log_level", "Logging level (off, error, warning, info, debug)"},
        {"log_to_file", "Whether to log to file"},
        {"log_filename", "Name of log file"},
        {"visualization_enabled", "Whether visualization is enabled"},
        {"visualization_update_rate", "Visualization update rate (Hz)"},
        {"reward_discount_factor", "Discount factor for rewards"},
        {"environment_name", "Name of the environment"},
    };
    
    auto it = descriptions.find(key);
    return it != descriptions.end() ? it->second : "Configuration parameter";
}

void Config::validate(const std::string& key, const ConfigValue& value, ConfigSource source) const {
    const auto& rules = ConfigValidationRules::getDefaultRules();
    
    for (const auto& rule : rules) {
        if (rule.key == key) {
            if (!rule.validator(value)) {
                throw std::invalid_argument("Configuration validation failed for key '" + key + "': " + rule.errorMessage);
            }
        }
    }
}

bool Config::validateAll() const {
    const auto& rules = ConfigValidationRules::getDefaultRules();
    
    for (const auto& entry : pImpl->entries) {
        for (const auto& rule : rules) {
            if (rule.key == entry.key) {
                if (!rule.validator(entry.value)) {
                    NLM_LOG_ERROR("Configuration validation failed for key '" + entry.key + "': " + rule.errorMessage);
                    return false;
                }
            }
        }
    }
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    validate(key, value, source);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, getDefaultDescription(key));
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
