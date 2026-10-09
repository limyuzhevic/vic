#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstdlib>
#include <stdexcept>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Validation rules for known parameters
    static const std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validationRules;
};

// Initialize validation rules for known parameters
const std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> Config::Impl::validationRules = {
    // Parameter ranges and constraints
    {"random_seed", [](const ConfigValue& v) {
        try {
            auto val = std::get<int64_t>(v);
            return val >= 0 && val < std::numeric_limits<int64_t>::max() / 2;
        } catch (...) { return false; }
    }},
    {"simulation_timestep", [](const ConfigValue& v) {
        try {
            auto val = std::get<double>(v);
            return val > 0.0 && val <= 1.0;  // Reasonable timestep range
        } catch (...) { return false; }
    }},
    {"neuron_count", [](const ConfigValue& v) {
        try {
            auto val = std::get<int64_t>(v);
            return val > 0 && val <= 100000;  // Reasonable neuron count
        } catch (...) { return false; }
    }},
    {"region_count", [](const ConfigValue& v) {
        try {
            auto val = std::get<int64_t>(v);
            return val > 0 && val <= 1000;  // Reasonable region count
        } catch (...) { return false; }
    }},
    {"connection_probability", [](const ConfigValue& v) {
        try {
            auto val = std::get<double>(v);
            return val >= 0.0 && val <= 1.0;  // Valid probability range
        } catch (...) { return false; }
    }},
    {"plasticity_learning_rate", [](const ConfigValue& v) {
        try {
            auto val = std::get<double>(v);
            return val >= 0.0 && val <= 1.0;  // Valid learning rate
        } catch (...) { return false; }
    }},
    {"stdp_ltp_weight", [](const ConfigValue& v) {
        try {
            auto val = std::get<double>(v);
            return val >= 0.0 && val <= 10.0;  // Valid STDP LTP weight
        } catch (...) { return false; }
    }},
    {"stdp_ltd_weight", [](const ConfigValue& v) {
        try {
            auto val = std::get<double>(v);
            return val >= 0.0 && val <= 10.0;  // Valid STDP LTD weight
        } catch (...) { return false; }
    }},
    {"stdp_time_constant", [](const ConfigValue& v) {
        try {
            auto val = std::get<double>(v);
            return val > 0.0 && val <= 1000.0;  // Valid time constant
        } catch (...) { return false; }
    }},
    {"synaptogenesis_rate", [](const ConfigValue& v) {
        try {
            auto val = std::get<double>(v);
            return val >= 0.0 && val <= 1.0;  // Valid rate
        } catch (...) { return false; }
    }},
    {"pruning_rate", [](const ConfigValue& v) {
        try {
            auto val = std::get<double>(v);
            return val >= 0.0 && val <= 1.0;  // Valid rate
        } catch (...) { return false; }
    }},
    {"dopamine_baseline", [](const ConfigValue& v) {
        try {
            auto val = std::get<double>(v);
            return val >= 0.0 && val <= 10.0;  // Valid baseline
        } catch (...) { return false; }
    }},
    {"reward_discount_factor", [](const ConfigValue& v) {
        try {
            auto val = std::get<double>(v);
            return val >= 0.0 && val <= 1.0;  // Valid discount factor
        } catch (...) { return false; }
    }},
    {"log_level", [](const ConfigValue& v) {
        try {
            auto val = std::get<std::string>(v);
            return val == "DEBUG" || val == "INFO" || val == "WARN" || 
                   val == "ERROR" || val == "OFF";  // Valid log levels
        } catch (...) { return false; }
    }},
    {"log_to_file", [](const ConfigValue& v) {
        try {
            auto val = std::get<bool>(v);
            return true;  // Just need to be a bool
        } catch (...) { return false; }
    }},
    {"visualization_enabled", [](const ConfigValue& v) {
        try {
            auto val = std::get<bool>(v);
            return true;  // Just need to be a bool
        } catch (...) { return false; }
    }},
    {"max_simulation_steps", [](const ConfigValue& v) {
        try {
            auto val = std::get<int64_t>(v);
            return val > 0 && val <= 1000000;  // Valid step count
        } catch (...) { return false; }
    }},
    {"simulation_time_limit", [](const ConfigValue& v) {
        try {
            auto val = std::get<double>(v);
            return val >= 0.0 && val <= 1000000.0;  // Valid time limit
        } catch (...) { return false; }
    }},
    {"environment_width", [](const ConfigValue& v) {
        try {
            auto val = std::get<int64_t>(v);
            return val > 0 && val <= 10000;  // Valid dimension
        } catch (...) { return false; }
    }},
    {"environment_height", [](const ConfigValue& v) {
        try {
            auto val = std::get<int64_t>(v);
            return val > 0 && val <= 10000;  // Valid dimension
        } catch (...) { return false; }
    }},
    {"visualization_update_rate", [](const ConfigValue& v) {
        try {
            auto val = std::get<double>(v);
            return val > 0.0 && val <= 1000.0;  // Valid update rate
        } catch (...) { return false; }
    }},
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to validate and convert parameter value
static bool validateParameterValue(const std::string& key, const std::string& valueStr) {
    auto& rules = Config::Impl::validationRules;
    auto it = rules.find(key);
    
    if (it == rules.end()) {
        return true;  // No validation rules for this parameter
    }
    
    try {
        // Try to convert to appropriate type based on value content
        if (valueStr.find('.') != std::string::npos || valueStr.find('e') != std::string::npos || 
            valueStr.find('E') != std::string::npos) {
            // Likely a float/double
            double val = std::stod(valueStr);
            return it->second(ConfigValue(val));
        } else if (valueStr == "true" || valueStr == "false") {
            // Boolean
            bool val = (valueStr == "true");
            return it->second(ConfigValue(val));
        } else {
            // Likely integer
            // Try int64_t first
            try {
                int64_t val = std::stoll(valueStr);
                return it->second(ConfigValue(val));
            } catch (...) {
                // Try regular int
                int val = std::stoi(valueStr);
                return it->second(ConfigValue(val));
            }
        }
    } catch (const std::exception& e) {
        return false;
    }
    
    return false;
}

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
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
            
            // Validate key
            if (key.empty()) {
                // TODO PHASE 2: Log warning for empty key
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate parameter value
            if (!validateParameterValue(key, value)) {
                // TODO PHASE 2: Log warning for invalid parameter
                continue;
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate parameter
                if (!validateParameterValue(key, value)) {
                    // TODO PHASE 2: Log warning for invalid parameter
                    success = false;
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate parameter
            if (!validateParameterValue(key, value)) {
                // TODO PHASE 2: Log warning for invalid parameter
                success = false;
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
        }
    }
    
    return success;
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
    // Validate parameter if rules exist
    auto& rules = Config::Impl::validationRules;
    auto itRule = rules.find(key);
    
    if (itRule != rules.end() && !itRule->second(value)) {
        // TODO PHASE 2: Log warning for invalid parameter value
        return;
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

// Helper functions for main.cpp
namespace {
    std::string getConfigSourceName(ConfigSource source) {
        switch (source) {
            case ConfigSource::Default: return "Default";
            case ConfigSource::File: return "File";
            case ConfigSource::CommandLine: return "CommandLine";
            case ConfigSource::Runtime: return "Runtime";
            default: return "Unknown";
        }
    }

    template<typename T>
    T stringToNumber(const std::string& str) {
        std::istringstream iss(str);
        T result;
        if (!(iss >> result)) {
            throw std::runtime_error("Cannot convert string to number: " + str);
        }
        return result;
    }

    std::unordered_map<std::string, std::string> parseParamOverrides(int argc, char** argv) {
        std::unordered_map<std::string, std::string> params;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --param <key>=<value> syntax
            if (arg.find("--param=") == 0 && arg.find('=') != std::string::npos) {
                std::string param = arg.substr(7); // Remove --param=
                size_t equals_pos = param.find('=');
                if (equals_pos != std::string::npos) {
                    std::string key = param.substr(0, equals_pos);
                    std::string value = param.substr(equals_pos + 1);
                    params[key] = value;
                }
            }
            // Handle --param <key> <value> syntax
            else if (arg == "--param" && i + 2 < argc) {
                std::string key = argv[++i];
                std::string value = argv[++i];
                params[key] = value;
            }
            // Handle direct parameter overrides (--neuron-count, etc.)
            else if (arg.find("--neuron-count=") == 0) {
                params["neuron_count"] = arg.substr(14);
            } else if (arg.find("--region-count=") == 0) {
                params["region_count"] = arg.substr(14);
            } else if (arg.find("--connection-prob=") == 0) {
                params["connection_probability"] = arg.substr(18);
            } else if (arg.find("--timestep=") == 0) {
                params["simulation_timestep"] = arg.substr(11);
            } else if (arg.find("--seed=") == 0) {
                params["random_seed"] = arg.substr(7);
            }
        }
        
        return params;
    }

    void applyParamOverrides(std::shared_ptr<Config> config, 
                            const std::unordered_map<std::string, std::string>& params) {
        for (const auto& pair : params) {
            const std::string& key = pair.first;
            const std::string& value = pair.second;
            
            try {
                if (key == "neuron_count" || key == "neuron-count") {
                    int64_t val = stringToNumber<int64_t>(value);
                    config->set(key, val, ConfigSource::CommandLine);
                    NLM_LOG_INFO("Override: " + key + " = " + value);
                } else if (key == "region_count" || key == "region-count") {
                    int64_t val = stringToNumber<int64_t>(value);
                    config->set(key, val, ConfigSource::CommandLine);
                    NLM_LOG_INFO("Override: " + key + " = " + value);
                } else if (key == "connection_probability" || key == "connection-prob") {
                    float val = stringToNumber<float>(value);
                    config->set(key, val, ConfigSource::CommandLine);
                    NLM_LOG_INFO("Override: " + key + " = " + value);
                } else if (key == "simulation_timestep" || key == "timestep") {
                    double val = stringToNumber<double>(value);
                    config->set(key, val, ConfigSource::CommandLine);
                    NLM_LOG_INFO("Override: " + key + " = " + value);
                } else if (key == "random_seed" || key == "seed") {
                    int64_t val = stringToNumber<int64_t>(value);
                    config->set(key, val, ConfigSource::CommandLine);
                    NLM_LOG_INFO("Override: " + key + " = " + value);
                } else {
                    // Try to parse as integer first, then float, then keep as string
                    try {
                        int64_t int_val = stringToNumber<int64_t>(value);
                        config->set(key, int_val, ConfigSource::CommandLine);
                        NLM_LOG_INFO("Override: " + key + " = " + value);
                    } catch (...) {
                        try {
                            float float_val = stringToNumber<float>(value);
                            config->set(key, float_val, ConfigSource::CommandLine);
                            NLM_LOG_INFO("Override: " + key + " = " + value);
                        } catch (...) {
                            NLM_LOG_INFO("Warning: Could not parse parameter " + key + " = " + value);
                        }
                    }
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to apply parameter override " + key + " = " + value + ": " + e.what());
            }
        }
    }
}

} // namespace nlm
