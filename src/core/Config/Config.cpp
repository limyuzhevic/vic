#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <unordered_map>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
    std::unordered_map<std::string, ConfigValue> defaults;
    std::unordered_map<std::string, std::string> descriptions;
};

// Configuration parameter definitions with validation and descriptions
const std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> Config::createDefaultValidators() {
    return {
        // Basic parameters
        {"neuron_count", [](const ConfigValue& v) {
            return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0;
        }},
        {"region_count", [](const ConfigValue& v) {
            return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0;
        }},
        {"connection_probability", [](const ConfigValue& v) {
            return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0;
        }},
        {"simulation_timestep", [](const ConfigValue& v) {
            return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0 && std::get<double>(v) < 1.0;
        }},
        {"random_seed", [](const ConfigValue& v) {
            return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) >= 0;
        }},
        
        // Plasticity parameters
        {"stdp_ltp_weight", [](const ConfigValue& v) {
            return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0;
        }},
        {"stdp_ltd_weight", [](const ConfigValue& v) {
            return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0;
        }},
        {"stdp_tau", [](const ConfigValue& v) {
            return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0;
        }},
        {"synaptogenesis_rate", [](const ConfigValue& v) {
            return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) < 1.0;
        }},
        {"pruning_rate", [](const ConfigValue& v) {
            return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) < 1.0;
        }},
        
        // Memory parameters
        {"working_memory_capacity", [](const ConfigValue& v) {
            return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0;
        }},
        {"episodic_memory_max_episodes", [](const ConfigValue& v) {
            return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0;
        }},
        
        // Neuromodulation parameters
        {"neuromod.dopamine.scale", [](const ConfigValue& v) {
            return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0;
        }},
        
        // Development parameters
        {"development.initial_plasticity", [](const ConfigValue& v) {
            return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0;
        }},
        {"development.critical_period_duration", [](const ConfigValue& v) {
            return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0;
        }},
        
        // Experiment parameters
        {"experiment.max_steps", [](const ConfigValue& v) {
            return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0;
        }},
    };
}

const std::unordered_map<std::string, ConfigValue> Config::createDefaultValues() {
    return {
        // Basic parameters with sensible defaults
        {"neuron_count", static_cast<int64_t>(1000)},
        {"region_count", static_cast<int64_t>(1)},
        {"connection_probability", 0.1},
        {"simulation_timestep", 0.001},
        {"random_seed", static_cast<int64_t>(42)},
        
        // Plasticity parameters
        {"stdp_ltp_weight", 0.01},
        {"stdp_ltd_weight", 0.012},
        {"stdp_tau", 20.0},
        {"synaptogenesis_rate", 0.0001},
        {"pruning_rate", 0.00001},
        
        // Memory parameters
        {"working_memory_capacity", static_cast<int64_t>(100)},
        {"episodic_memory_max_episodes", static_cast<int64_t>(1000)},
        
        // Neuromodulation parameters
        {"neuromod.dopamine.scale", 1.0},
        
        // Development parameters
        {"development.initial_plasticity", 1.0},
        {"development.critical_period_duration", 1000.0},
        
        // Experiment parameters
        {"experiment.max_steps", static_cast<int64_t>(10000)},
    };
}

const std::unordered_map<std::string, std::string> Config::createDescriptions() {
    return {
        // Basic parameters
        {"neuron_count", "Total number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Probability of synaptic connection between neurons"},
        {"simulation_timestep", "Simulation time step in milliseconds"},
        {"random_seed", "Random seed for reproducible simulations"},
        
        // Plasticity parameters
        {"stdp_ltp_weight", "STDP Long-Term Potentiation weight factor"},
        {"stdp_ltd_weight", "STDP Long-Term Depression weight factor"},
        {"stdp_tau", "STDP time constant in milliseconds"},
        {"synaptogenesis_rate", "Rate of new synapse formation"},
        {"pruning_rate", "Rate of synapse elimination"},
        
        // Memory parameters
        {"working_memory_capacity", "Capacity of working memory in number of traces"},
        {"episodic_memory_max_episodes", "Maximum number of episodic memory episodes"},
        
        // Neuromodulation parameters
        {"neuromod.dopamine.scale", "Dopamine modulation scale factor"},
        
        // Development parameters
        {"development.initial_plasticity", "Initial plasticity factor (0.0 = stable, 1.0 = highly plastic)"},
        {"development.critical_period_duration", "Duration of critical period in simulation steps"},
        
        // Experiment parameters
        {"experiment.max_steps", "Maximum simulation steps for experiments"},
    };
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeConfiguration();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeConfiguration() {
    // Initialize validators, defaults, and descriptions
    pImpl->validators = createDefaultValidators();
    pImpl->defaults = createDefaultValues();
    pImpl->descriptions = createDescriptions();
    
    // Load configuration from file if it exists
    std::string defaultConfigFile = "configs/default.cfg";
    if (std::filesystem::exists(defaultConfigFile)) {
        loadFromFile(defaultConfigFile);
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Configuration file not found: " + filepath + ". Using defaults.");
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
            
            // Parse key=value pairs
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
                
                try {
                    if (!setValueWithValidation(key, value, ConfigSource::File)) {
                        NLM_LOG_WARNING("Invalid configuration value in " + filepath + ":\n  " + line + 
                                       " (line " + std::to_string(lineNumber) + ")");
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error parsing configuration in " + filepath + ":\n  " + line + 
                                 " (line " + std::to_string(lineNumber) + ")\n  " + std::string(e.what()));
                }
            } else {
                NLM_LOG_WARNING("Invalid configuration format in " + filepath + " (line " + 
                               std::to_string(lineNumber) + "): missing '='");
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    if (!setValueWithValidation(key, value, ConfigSource::CommandLine)) {
                        NLM_LOG_WARNING("Invalid command-line option: --" + key + "=" + value);
                    }
                } else {
                    NLM_LOG_WARNING("Invalid command-line option: " + arg);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                if (!setValueWithValidation(key, value, ConfigSource::CommandLine)) {
                    NLM_LOG_WARNING("Invalid command-line option: -" + key + " " + value);
                }
            }
        }
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing command-line arguments: " + std::string(e.what()));
        return false;
    }
}

bool Config::setValueWithValidation(const std::string& key, const std::string& value, ConfigSource source) {
    // Check if key is valid
    if (pImpl->validators.find(key) == pImpl->validators.end()) {
        NLM_LOG_WARNING("Unknown configuration key: " + key);
        // Still allow it, but don't validate
    }
    
    try {
        // Parse value based on key type
        ConfigValue parsedValue;
        bool parsed = parseValue(key, value, parsedValue);
        
        if (parsed) {
            // Validate if validator exists
            auto validatorIt = pImpl->validators.find(key);
            if (validatorIt != pImpl->validators.end()) {
                if (!validatorIt->second(parsedValue)) {
                    NLM_LOG_ERROR("Configuration validation failed for " + key + ": " + value +
                                 "\n  Key: " + key + "\n  Expected type/format per key definition");
                    return false;
                }
            }
            
            set(key, parsedValue, source);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to parse configuration value for " + key + ": " + value);
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error setting configuration " + key + "=" + value + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::parseValue(const std::string& key, const std::string& valueStr, ConfigValue& outValue) {
    // Try to parse as different types based on key name patterns
    std::string lowerKey = toLower(key);
    
    // Try to parse as double (most numeric parameters use double)
    try {
        size_t pos;
        double doubleVal = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            outValue = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Try to parse as integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(valueStr, &pos);
        if (pos == valueStr.size()) {
            // Check if it should be int based on key name
            if (lowerKey.find("count") != std::string::npos || 
                lowerKey.find("steps") != std::string::npos ||
                lowerKey.find("size") != std::string::npos) {
                outValue = intVal;
                return true;
            }
        }
    } catch (...) {}
    
    // Try to parse as boolean
    std::string lowerValue = toLower(valueStr);
    if (lowerValue == "true" || lowerValue == "yes" || lowerValue == "1") {
        outValue = true;
        return true;
    } else if (lowerValue == "false" || lowerValue == "no" || lowerValue == "0") {
        outValue = false;
        return true;
    }
    
    // Default to string
    outValue = valueStr;
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# NLM Configuration File\n";
        file << "# Generated on " << getTimestamp() << "\n\n";
        
        for (const auto& entry : pImpl->entries) {
            if (pImpl->descriptions.find(entry.key) != pImpl->descriptions.end()) {
                file << "# " << pImpl->descriptions.at(entry.key) << "\n";
            }
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << " # " << static_cast<int>(entry.source) << "\n\n";
        }
        
        NLM_LOG_INFO("Configuration saved to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        // Check if we have a default value
        auto defaultIt = pImpl->defaults.find(key);
        if (defaultIt != pImpl->defaults.end()) {
            return std::get<T>(defaultIt->second);
        }
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARNING("Type mismatch for configuration key: " + key);
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
    oss << "=== NLM Configuration ===\n";
    oss << "Loaded parameters: " << pImpl->entries.size() << "\n\n";
    
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
        oss << "] (source: " << static_cast<int>(entry.source) << ")\n";
        
        if (pImpl->descriptions.find(entry.key) != pImpl->descriptions.end()) {
            oss << "    " << pImpl->descriptions.at(entry.key) << "\n";
        }
        oss << "\n";
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

std::string Config::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Configuration validation function
std::string validateConfig(const Config& config) {
    std::ostringstream oss;
    oss << "=== Configuration Validation Report ===\n";
    
    bool hasErrors = false;
    bool hasWarnings = false;
    
    for (const auto& key : config.getKeys()) {
        oss << "\nChecking key: \"" << key << "\"";
        
        // Check if value is within expected range (basic validation)
        if (key == "neuron_count") {
            auto val = config.get<int64_t>(key);
            if (!val.has_value()) {
                oss << " - MISSING VALUE\n";
                hasWarnings = true;
            } else if (*val <= 0) {
                oss << " - ERROR: neuron_count must be positive\n";
                hasErrors = true;
            } else if (*val > 100000) {
                oss << " - WARNING: neuron_count is very large (performance impact)\n";
                hasWarnings = true;
            }
        } else if (key == "connection_probability") {
            auto val = config.get<double>(key);
            if (!val.has_value()) {
                oss << " - MISSING VALUE\n";
                hasWarnings = true;
            } else if (*val < 0.0 || *val > 1.0) {
                oss << " - ERROR: connection_probability must be between 0.0 and 1.0\n";
                hasErrors = true;
            }
        }
    }
    
    if (!hasErrors && !hasWarnings) {
        oss << "\n✓ All configuration parameters are valid\n";
    }
    
    return oss.str();
}

// Explicit template instantiations
extern template std::optional<int> Config::get<int>(const std::string&) const;
extern template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
extern template std::optional<double> Config::get<double>(const std::string&) const;
extern template std::optional<bool> Config::get<bool>(const std::string&) const;
extern template std::optional<std::string> Config::get<std::string>(const std::string&) const;

extern template int Config::getOr<int>(const std::string&, const int&) const;
extern template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
extern template double Config::getOr<double>(const std::string&, const double&) const;
extern template bool Config::getOr<bool>(const std::string&, const bool&) const;
extern template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
