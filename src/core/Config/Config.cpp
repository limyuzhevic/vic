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
                
                // Enhanced argument parsing with type detection
                if (key == ConfigParams::NEURON_COUNT || key == ConfigParams::REGION_COUNT) {
                    try {
                        size_t neuronCount = std::stoul(value);
                        set(key, neuronCount, ConfigSource::CommandLine);
                    } catch (...) {
                        NLM_LOG_WARNING("Invalid " + key + " value: " + value);
                    }
                } else if (key == ConfigParams::CONNECTION_PROBABILITY || 
                          key == ConfigParams::SIMULATION_TIMESTEP ||
                          key == ConfigParams::RANDOM_SEED ||
                          key == ConfigParams::STDP_LTP_WEIGHT ||
                          key == ConfigParams::STDP_LTD_WEIGHT ||
                          key == ConfigParams::STDP_TAU ||
                          key == ConfigParams::STRUCTURAL_PLASTICITY_RATE ||
                          key == ConfigParams::PRUNING_RATE ||
                          key == ConfigParams::DOPAMINE_BASELINE ||
                          key == ConfigParams::CURIOUSITY_THRESHOLD ||
                          key == ConfigParams::NOVELTY_THRESHOLD ||
                          key == ConfigParams::CRITICAL_PERIOD_DURATION ||
                          key == ConfigParams::PLASTICITY_DEVELOPMENT_FACTOR) {
                    try {
                        float floatValue = std::stof(value);
                        set(key, floatValue, ConfigSource::CommandLine);
                    } catch (...) {
                        NLM_LOG_WARNING("Invalid " + key + " value: " + value);
                    }
                } else {
                    // Generic string handling for other parameters
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Type-aware parsing for known parameters
            if (key == ConfigParams::NEURON_COUNT || key == ConfigParams::REGION_COUNT) {
                try {
                    size_t intValue = std::stoul(value);
                    set(key, intValue, ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_WARNING("Invalid " + key + " value: " + value);
                }
            } else if (key == ConfigParams::CONNECTION_PROBABILITY || 
                      key == ConfigParams::SIMULATION_TIMESTEP) {
                try {
                    double doubleValue = std::stod(value);
                    set(key, doubleValue, ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_WARNING("Invalid " + key + " value: " + value);
                }
            } else {
                set(key, value, ConfigSource::CommandLine);
            }
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

// Enhanced error handling and validation functions
void Config::validateParameter(const std::string& key, const ConfigValue& value) {
    // Enhanced parameter validation
    ConfigError error = ConfigError::SYSTEM_ERROR;
    std::string errorMessage;
    
    // Validate neuron parameters
    if (key == ConfigParams::NEURON_COUNT) {
        try {
            size_t neuronCount = std::get<size_t>(value);
            if (neuronCount == 0) {
                error = ConfigError::OUT_OF_RANGE;
                errorMessage = "Neuron count cannot be zero: " + std::to_string(neuronCount);
                NLM_LOG_ERROR(errorMessage);
            } else if (neuronCount > 10000) {
                NLM_LOG_WARNING("High neuron count may impact performance: " + std::to_string(neuronCount));
            }
        } catch (const std::bad_variant_access&) {
            error = ConfigError::INVALID_VALUE_TYPE;
            errorMessage = "Invalid value type for neuron_count (expected integer): " + key;
            NLM_LOG_ERROR(errorMessage);
        }
    }
    else if (key == ConfigParams::REGION_COUNT) {
        try {
            size_t regionCount = std::get<size_t>(value);
            if (regionCount == 0) {
                error = ConfigError::OUT_OF_RANGE;
                errorMessage = "Region count cannot be zero: " + std::to_string(regionCount);
                NLM_LOG_ERROR(errorMessage);
            }
        } catch (const std::bad_variant_access&) {
            error = ConfigError::INVALID_VALUE_TYPE;
            errorMessage = "Invalid value type for region_count (expected integer): " + key;
            NLM_LOG_ERROR(errorMessage);
        }
    }
    else if (key == ConfigParams::CONNECTION_PROBABILITY) {
        try {
            float prob = std::get<float>(value);
            if (prob < 0.0f || prob > 1.0f) {
                error = ConfigError::OUT_OF_RANGE;
                errorMessage = "Connection probability must be between 0.0 and 1.0: " + std::to_string(prob);
                NLM_LOG_ERROR(errorMessage);
            }
        } catch (const std::bad_variant_access&) {
            error = ConfigError::INVALID_VALUE_TYPE;
            errorMessage = "Invalid value type for connection_probability (expected float): " + key;
            NLM_LOG_ERROR(errorMessage);
        }
    }
    else if (key == ConfigParams::SIMULATION_TIMESTEP) {
        try {
            double timestep = std::get<double>(value);
            if (timestep <= 0.0) {
                error = ConfigError::OUT_OF_RANGE;
                errorMessage = "Simulation timestep must be positive: " + std::to_string(timestep);
                NLM_LOG_ERROR(errorMessage);
            }
        } catch (const std::bad_variant_access&) {
            error = ConfigError::INVALID_VALUE_TYPE;
            errorMessage = "Invalid value type for simulation_timestep (expected double): " + key;
            NLM_LOG_ERROR(errorMessage);
        }
    }
    else if (key == ConfigParams::RANDOM_SEED) {
        try {
            uint64_t seed = std::get<uint64_t>(value);
            // Warning about default seed usage
            if (seed == 42) {
                NLM_LOG_WARNING("Using default random seed (42). For reproducibility, consider setting a specific seed.");
            }
        } catch (const std::bad_variant_access&) {
            error = ConfigError::INVALID_VALUE_TYPE;
            errorMessage = "Invalid value type for random_seed (expected uint64_t): " + key;
            NLM_LOG_ERROR(errorMessage);
        }
    }
    else if (key == ConfigParams::STDP_LTP_WEIGHT || key == ConfigParams::STDP_LTD_WEIGHT) {
        try {
            float weight = std::get<float>(value);
            if (weight < 0.0f) {
                error = ConfigError::OUT_OF_RANGE;
                errorMessage = "STDP weight must be non-negative: " + std::to_string(weight);
                NLM_LOG_ERROR(errorMessage);
            }
        } catch (const std::bad_variant_access&) {
            error = ConfigError::INVALID_VALUE_TYPE;
            errorMessage = "Invalid value type for STDP weight (expected float): " + key;
            NLM_LOG_ERROR(errorMessage);
        }
    }
    else if (key == ConfigParams::STRUCTURAL_PLASTICITY_RATE || key == ConfigParams::PRUNING_RATE) {
        try {
            float rate = std::get<float>(value);
            if (rate < 0.0f || rate > 1.0f) {
                error = ConfigError::OUT_OF_RANGE;
                errorMessage = "Plasticity rate must be between 0.0 and 1.0: " + std::to_string(rate);
                NLM_LOG_ERROR(errorMessage);
            }
        } catch (const std::bad_variant_access&) {
            error = ConfigError::INVALID_VALUE_TYPE;
            errorMessage = "Invalid value type for plasticity rate (expected float): " + key;
            NLM_LOG_ERROR(errorMessage);
        }
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
                
                // Enhanced argument parsing with type detection and validation
                try {
                    // Determine parameter type based on known keys
                    if (key == ConfigParams::NEURON_COUNT || key == ConfigParams::REGION_COUNT) {
                        size_t intValue = std::stoul(value);
                        validateParameter(key, ConfigValue(intValue));
                        set(key, intValue, ConfigSource::CommandLine);
                    } else if (key == ConfigParams::CONNECTION_PROBABILITY || 
                              key == ConfigParams::SIMULATION_TIMESTEP ||
                              key == ConfigParams::RANDOM_SEED ||
                              key == ConfigParams::STDP_LTP_WEIGHT ||
                              key == ConfigParams::STDP_LTD_WEIGHT ||
                              key == ConfigParams::STDP_TAU ||
                              key == ConfigParams::STRUCTURAL_PLASTICITY_RATE ||
                              key == ConfigParams::PRUNING_RATE ||
                              key == ConfigParams::DOPAMINE_BASELINE ||
                              key == ConfigParams::CURIOUSITY_THRESHOLD ||
                              key == ConfigParams::NOVELTY_THRESHOLD ||
                              key == ConfigParams::CRITICAL_PERIOD_DURATION ||
                              key == ConfigParams::PLASTICITY_DEVELOPMENT_FACTOR) {
                        float floatValue = std::stof(value);
                        validateParameter(key, ConfigValue(floatValue));
                        set(key, floatValue, ConfigSource::CommandLine);
                    } else if (key == ConfigParams::CHECKPOINT_INTERVAL || 
                              key == ConfigParams::MAX_CHECKPOINTS ||
                              key == ConfigParams::REPLAY_INTERVAL ||
                              key == ConfigParams::CONSOLIDATION_INTERVAL) {
                        size_t sizeValue = std::stoul(value);
                        validateParameter(key, ConfigValue(sizeValue));
                        set(key, sizeValue, ConfigSource::CommandLine);
                    } else {
                        // Generic string handling for other parameters
                        validateParameter(key, ConfigValue(value));
                        set(key, value, ConfigSource::CommandLine);
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Invalid command line argument --" + key + "=" + value + ": " + e.what());
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Type-aware parsing and validation for known parameters
            try {
                if (key == ConfigParams::NEURON_COUNT || key == ConfigParams::REGION_COUNT) {
                    size_t intValue = std::stoul(value);
                    validateParameter(key, ConfigValue(intValue));
                    set(key, intValue, ConfigSource::CommandLine);
                } else if (key == ConfigParams::CONNECTION_PROBABILITY || 
                          key == ConfigParams::SIMULATION_TIMESTEP) {
                    double doubleValue = std::stod(value);
                    validateParameter(key, ConfigValue(doubleValue));
                    set(key, doubleValue, ConfigSource::CommandLine);
                } else if (key == ConfigParams::RANDOM_SEED ||
                          key == ConfigParams::STDP_LTP_WEIGHT ||
                          key == ConfigParams::STDP_LTD_WEIGHT ||
                          key == ConfigParams::STDP_TAU ||
                          key == ConfigParams::STRUCTURAL_PLASTICITY_RATE ||
                          key == ConfigParams::PRUNING_RATE ||
                          key == ConfigParams::DOPAMINE_BASELINE ||
                          key == ConfigParams::CURIOUSITY_THRESHOLD ||
                          key == ConfigParams::NOVELTY_THRESHOLD ||
                          key == ConfigParams::CRITICAL_PERIOD_DURATION ||
                          key == ConfigParams::PLASTICITY_DEVELOPMENT_FACTOR) {
                    float floatValue = std::stof(value);
                    validateParameter(key, ConfigValue(floatValue));
                    set(key, floatValue, ConfigSource::CommandLine);
                } else if (key == ConfigParams::CHECKPOINT_INTERVAL || 
                          key == ConfigParams::MAX_CHECKPOINTS ||
                          key == ConfigParams::REPLAY_INTERVAL ||
                          key == ConfigParams::CONSOLIDATION_INTERVAL) {
                    size_t sizeValue = std::stoul(value);
                    validateParameter(key, ConfigValue(sizeValue));
                    set(key, sizeValue, ConfigSource::CommandLine);
                } else {
                    validateParameter(key, ConfigValue(value));
                    set(key, value, ConfigSource::CommandLine);
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Invalid command line argument -" + key + " " + value + ": " + e.what());
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    // Write header with metadata
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM configuration system\n";
    file << "# Format: # comments, key = value\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        // Write value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]\n\n";
            } else {
                file << entry.key << " = " << arg << "\n\n";
            }
        }, entry.value);
    }
    
    NLM_LOG_INFO("Configuration saved successfully to: " + filepath);
    return true;
}

// Explicit template instantiations
// Rest of the original file remains unchanged

} // namespace nlm
