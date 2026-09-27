#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

// Parameter constraints for validation
struct ParameterConstraint {
    std::string name;
    std::string description;
    std::string unit;
    std::string category;  // neural, memory, plasticity, neuromodulation, development
    bool required;
    std::string defaultValue;
    std::string minValue;
    std::string maxValue;
    std::string validationRegex;
    bool (*validator)(const std::string&);  // Custom validator function
    
    ParameterConstraint() 
        : name(), description(), unit(), category(), required(false), 
          defaultValue(), minValue(), maxValue(), validationRegex(), validator(nullptr) {}
};

// Parameter change listener
class ParameterChangeListener {
public:
    virtual ~ParameterChangeListener() = default;
    virtual void onParameterChanged(const std::string& key, const std::string& oldValue, const std::string& newValue) = 0;
};

// Enhanced Config implementation
class Config::Impl {
public:
    std::unordered_map<std::string, ConfigEntry> parameters;
    std::vector<ParameterChangeListener*> changeListeners;
    
    // Parameter definitions for Phase 6
    std::unordered_map<std::string, ParameterConstraint> parameterDefinitions;
    
    Impl() {
        // Initialize parameter definitions for Phase 6
        initializeParameterDefinitions();
    }
    
    void initializeParameterDefinitions() {
        // Neural system parameters
        parameterDefinitions["neuron_count"] = {
            "neuron_count",
            "Number of neurons in the brain",
            "neurons",
            "neural",
            true,
            "1000",
            "1",
            "100000",
            "^[0-9]+$",
            [](const std::string& val) { return std::stoi(val) > 0; }
        };
        
        parameterDefinitions["region_count"] = {
            "region_count",
            "Number of brain regions",
            "regions",
            "neural",
            true,
            "1",
            "1",
            "100",
            "^[0-9]+$",
            [](const std::string& val) { return std::stoi(val) > 0; }
        };
        
        parameterDefinitions["connection_probability"] = {
            "connection_probability",
            "Probability of connection between neurons",
            "probability",
            "neural",
            true,
            "0.1",
            "0.0",
            "1.0",
            "^[0-9]*\.?[0-9]+$",
            [](const std::string& val) { return std::stod(val) >= 0.0 && std::stod(val) <= 1.0; }
        };
        
        // Plasticity parameters
        parameterDefinitions["stdp_ltp_weight"] = {
            "stdp_ltp_weight",
            "STDP long-term potentiation weight",
            "weight",
            "plasticity",
            true,
            "0.01",
            "0.0",
            "1.0",
            "^[0-9]*\.?[0-9]+$",
            [](const std::string& val) { return std::stod(val) >= 0.0 && std::stod(val) <= 1.0; }
        };
        
        parameterDefinitions["stdp_ltd_weight"] = {
            "stdp_ltd_weight",
            "STDP long-term depression weight",
            "weight",
            "plasticity",
            true,
            "0.012",
            "0.0",
            "1.0",
            "^[0-9]*\.?[0-9]+$",
            [](const std::string& val) { return std::stod(val) >= 0.0 && std::stod(val) <= 1.0; }
        };
        
        parameterDefinitions["stdp_tau"] = {
            "stdp_tau",
            "STDP time constant (ms)",
            "ms",
            "plasticity",
            true,
            "20.0",
            "1.0",
            "100.0",
            "^[0-9]*\.?[0-9]+$",
            [](const std::string& val) { return std::stod(val) > 0.0; }
        };
        
        // Memory system parameters
        parameterDefinitions["working_memory_capacity"] = {
            "working_memory_capacity",
            "Capacity of working memory in neurons",
            "neurons",
            "memory",
            true,
            "100",
            "1",
            "10000",
            "^[0-9]+$",
            [](const std::string& val) { return std::stoi(val) > 0; }
        };
        
        parameterDefinitions["episodic_memory_max_episodes"] = {
            "episodic_memory_max_episodes",
            "Maximum number of episodic memory episodes",
            "episodes",
            "memory",
            true,
            "1000",
            "1",
            "100000",
            "^[0-9]+$",
            [](const std::string& val) { return std::stoi(val) > 0; }
        };
        
        // Neuromodulation parameters
        parameterDefinitions["dopamine_baseline"] = {
            "dopamine_baseline",
            "Baseline dopamine level",
            "level",
            "neuromodulation",
            true,
            "0.0",
            "0.0",
            "10.0",
            "^[0-9]*\.?[0-9]*$",
            [](const std::string& val) { return std::stod(val) >= 0.0; }
        };
        
        parameterDefinitions["curiosity_threshold"] = {
            "curiosity_threshold",
            "Threshold for curiosity-driven exploration",
            "threshold",
            "neuromodulation",
            true,
            "0.5",
            "0.0",
            "1.0",
            "^[0-9]*\.?[0-9]+$",
            [](const std::string& val) { return std::stod(val) >= 0.0 && std::stod(val) <= 1.0; }
        };
        
        // Development parameters
        parameterDefinitions["synaptogenesis_rate"] = {
            "synaptogenesis_rate",
            "Rate of new synapse formation",
            "rate per step",
            "development",
            true,
            "0.0001",
            "0.0",
            "0.01",
            "^[0-9]*\.?[0-9]+$",
            [](const std::string& val) { return std::stod(val) >= 0.0; }
        };
        
        parameterDefinitions["pruning_rate"] = {
            "pruning_rate",
            "Rate of synapse pruning",
            "rate per step",
            "development",
            true,
            "0.00001",
            "0.0",
            "0.001",
            "^[0-9]*\.?[0-9]+$",
            [](const std::string& val) { return std::stod(val) >= 0.0; }
        };
        
        // Simulation parameters
        parameterDefinitions["simulation_timestep"] = {
            "simulation_timestep",
            "Time step for simulation (seconds)",
            "s",
            "simulation",
            true,
            "0.001",
            "0.0",
            "1.0",
            "^[0-9]*\.?[0-9]+$",
            [](const std::string& val) { return std::stod(val) > 0.0 && std::stod(val) <= 0.1; }
        };
        
        parameterDefinitions["max_simulation_steps"] = {
            "max_simulation_steps",
            "Maximum number of simulation steps",
            "steps",
            "simulation",
            true,
            "10000",
            "1",
            "1000000",
            "^[0-9]+$",
            [](const std::string& val) { return std::stoi(val) > 0; }
        };
        
        // Checkpoint parameters
        parameterDefinitions["checkpoint_dir"] = {
            "checkpoint_dir",
            "Directory for checkpoint files",
            "path",
            "checkpoint",
            false,
            "./checkpoints",
            "",
            "",
            "^[a-zA-Z0-9_/.]+$",
            [](const std::string& val) { return !val.empty(); }
        };
        
        parameterDefinitions["checkpoint_interval"] = {
            "checkpoint_interval",
            "Interval between checkpoints (steps)",
            "steps",
            "checkpoint",
            true,
            "10000",
            "0",
            "1000000",
            "^[0-9]+$",
            [](const std::string& val) { return std::stoi(val) >= 0; }
        };
        
        parameterDefinitions["max_checkpoints"] = {
            "max_checkpoints",
            "Maximum number of checkpoints to keep",
            "checkpoints",
            "checkpoint",
            true,
            "10",
            "1",
            "1000",
            "^[0-9]+$",
            [](const std::string& val) { return std::stoi(val) > 0; }
        };
        
        // Visualization parameters
        parameterDefinitions["visualization_enabled"] = {
            "visualization_enabled",
            "Enable visualization output",
            "enabled",
            "visualization",
            true,
            "false",
            "",
            "",
            "true|false",
            [](const std::string& val) { return val == "true" || val == "false"; }
        };
        
        parameterDefinitions["visualization_update_rate"] = {
            "visualization_update_rate",
            "Visualization update rate (Hz)",
            "Hz",
            "visualization",
            true,
            "30.0",
            "0.0",
            "1000.0",
            "^[0-9]*\.?[0-9]+$",
            [](const std::string& val) { return std::stod(val) > 0.0; }
        };
    }
    
    bool loadFromFile(const std::string& filepath) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            // Parse JSON (simplified implementation)
            std::string line;
            std::vector<ConfigEntry> entries;
            
            while (std::getline(file, line)) {
                line = trim(line);
                if (line.empty() || line.find('#') == 0) {
                    continue;
                }
                
                // Parse key = value format
                size_t pos = line.find('=');
                if (pos == std::string::npos) {
                    continue;
                }
                
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Validate parameter exists
                if (parameterDefinitions.find(key) == parameterDefinitions.end()) {
                    // Unknown parameter - store anyway for backward compatibility
                    ConfigEntry entry(key, value, ConfigSource::File);
                    parameters[key] = entry;
                    continue;
                }
                
                // Validate parameter value
                if (!validateParameter(key, value)) {
                    return false;
                }
                
                ConfigEntry entry(key, value, ConfigSource::File);
                entries.push_back(entry);
            }
            
            // Add to parameters
            for (const auto& entry : entries) {
                parameters[entry.key] = entry;
            }
            
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    bool loadFromArgs(int argc, char** argv) {
        try {
            for (int i = 1; i < argc; ++i) {
                std::string arg(argv[i]);
                
                // Check for --key=value format
                if (arg.find('=') != std::string::npos) {
                    size_t pos = arg.find('=');
                    std::string key = arg.substr(0, pos);
                    std::string value = arg.substr(pos + 1);
                    
                    // Validate parameter exists
                    if (parameterDefinitions.find(key) == parameterDefinitions.end()) {
                        // Unknown parameter - store anyway
                        ConfigEntry entry(key, value, ConfigSource::CommandLine);
                        parameters[key] = entry;
                        continue;
                    }
                    
                    // Validate parameter value
                    if (!validateParameter(key, value)) {
                        return false;
                    }
                    
                    ConfigEntry entry(key, value, ConfigSource::CommandLine);
                    parameters[key] = entry;
                }
                // Check for --key value format
                else if (arg.find('=') == std::string::npos && i + 1 < argc) {
                    std::string nextArg(argv[i + 1]);
                    if (nextArg.find('=') == std::string::npos) {
                        std::string key = arg;
                        std::string value = argv[++i];
                        
                        if (parameterDefinitions.find(key) == parameterDefinitions.end()) {
                            ConfigEntry entry(key, value, ConfigSource::CommandLine);
                            parameters[key] = entry;
                            continue;
                        }
                        
                        if (!validateParameter(key, value)) {
                            return false;
                        }
                        
                        ConfigEntry entry(key, value, ConfigSource::CommandLine);
                        parameters[key] = entry;
                    }
                }
            }
            
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    bool saveToFile(const std::string& filepath) const {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            // Write header
            file << "# NLM Configuration File\n";
            file << "# Generated by NLM\n";
            file << "\n";
            
            // Write parameters
            for (const auto& paramDef : parameterDefinitions) {
                const std::string& key = paramDef.first;
                
                if (parameters.find(key) != parameters.end()) {
                    const ConfigEntry& entry = parameters.at(key);
                    file << key << " = " << entry.value << "  # " << entry.description << "\n";
                }
            }
            
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        if (parameters.find(key) == parameters.end()) {
            return std::nullopt;
        }
        
        const ConfigEntry& entry = parameters.at(key);
        
        try {
            if constexpr (std::is_same_v<T, int>) {
                return std::stoi(entry.value);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return std::stoll(entry.value);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::stod(entry.value);
            } else if constexpr (std::is_same_v<T, bool>) {
                if (entry.value == "true" || entry.value == "1") return true;
                if (entry.value == "false" || entry.value == "0") return false;
                return std::nullopt;
            } else if constexpr (std::is_same_v<T, std::string>) {
                return entry.value;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                // Parse vector
                return std::nullopt; // Simplified
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                // Parse vector
                return std::nullopt; // Simplified
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                // Parse vector
                return std::nullopt; // Simplified
            }
        } catch (...) {
            return std::nullopt;
        }
        
        return std::nullopt;
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto value = get<T>(key);
        return value.has_value() ? value.value() : defaultValue;
    }
    
    void set(const std::string& key, const ConfigValue& value, ConfigSource source) {
        // Validate parameter exists
        if (parameterDefinitions.find(key) != parameterDefinitions.end()) {
            const ParameterConstraint& constraint = parameterDefinitions.at(key);
            std::string strValue;
            
            // Convert value to string for validation
            if constexpr (std::is_same_v<ConfigValue, std::string>) {
                strValue = std::get<std::string>(value);
            } else if constexpr (std::is_same_v<ConfigValue, int>) {
                strValue = std::to_string(std::get<int>(value));
            } else if constexpr (std::is_same_v<ConfigValue, int64_t>) {
                strValue = std::to_string(std::get<int64_t>(value));
            } else if constexpr (std::is_same_v<ConfigValue, double>) {
                strValue = std::to_string(std::get<double>(value));
            } else if constexpr (std::is_same_v<ConfigValue, bool>) {
                strValue = std::get<bool>(value) ? "true" : "false";
            }
            
            // Validate
            if (!validateParameter(key, strValue)) {
                throw std::invalid_argument("Invalid value for parameter: " + key);
            }
            
            // Convert to string for storage
            if constexpr (std::is_same_v<ConfigValue, std::string>) {
                parameters[key] = ConfigEntry(key, value, source);
            } else {
                std::string strVal;
                if constexpr (std::is_same_v<ConfigValue, int>) {
                    strVal = std::to_string(std::get<int>(value));
                } else if constexpr (std::is_same_v<ConfigValue, int64_t>) {
                    strVal = std::to_string(std::get<int64_t>(value));
                } else if constexpr (std::is_same_v<ConfigValue, double>) {
                    strVal = std::to_string(std::get<double>(value));
                } else if constexpr (std::is_same_v<ConfigValue, bool>) {
                    strVal = std::get<bool>(value) ? "true" : "false";
                }
                parameters[key] = ConfigEntry(key, strVal, source);
            }
        } else {
            // Unknown parameter - store anyway
            std::string strValue;
            if constexpr (std::is_same_v<ConfigValue, std::string>) {
                strValue = std::get<std::string>(value);
            } else if constexpr (std::is_same_v<ConfigValue, int>) {
                strValue = std::to_string(std::get<int>(value));
            } else if constexpr (std::is_same_v<ConfigValue, int64_t>) {
                strVal = std::to_string(std::get<int64_t>(value));
            } else if constexpr (std::is_same_v<ConfigValue, double>) {
                strVal = std::to_string(std::get<double>(value));
            } else if constexpr (std::is_same_v<ConfigValue, bool>) {
                strVal = std::get<bool>(value) ? "true" : "false";
            }
            parameters[key] = ConfigEntry(key, strValue, source);
        }
        
        // Notify listeners
        notifyChangeListeners(key, "", strValue);
    }
    
    bool has(const std::string& key) const {
        return parameters.find(key) != parameters.end();
    }
    
    void remove(const std::string& key) {
        auto it = parameters.find(key);
        if (it != parameters.end()) {
            parameters.erase(it);
        }
    }
    
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(parameters.size());
        for (const auto& pair : parameters) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    void clear() {
        parameters.clear();
    }
    
    std::string summary() const {
        std::stringstream ss;
        ss << "NLM Configuration Summary\n";
        ss << "======================\n\n";
        
        for (const auto& paramDef : parameterDefinitions) {
            const std::string& key = paramDef.first;
            const ParameterConstraint& constraint = paramDef.second;
            
            if (parameters.find(key) != parameters.end()) {
                const ConfigEntry& entry = parameters.at(key);
                ss << key << " = " << entry.value;
                
                if (entry.source != ConfigSource::Default) {
                    ss << " [" << static_cast<int>(entry.source) << "]";
                }
                
                ss << "  # " << constraint.description << "\n";
            } else {
                ss << key << " = " << constraint.defaultValue << " (default)\n";
            }
        }
        
        return ss.str();
    }
    
    void addChangeListener(ParameterChangeListener* listener) {
        changeListeners.push_back(listener);
    }
    
    void removeChangeListener(ParameterChangeListener* listener) {
        changeListeners.erase(std::remove(changeListeners.begin(), changeListeners.end(), listener),
                             changeListeners.end());
    }
    
    bool validateParameter(const std::string& key, const std::string& value) const {
        if (parameterDefinitions.find(key) == parameterDefinitions.end()) {
            return true;  // Unknown parameter - allow for backward compatibility
        }
        
        const ParameterConstraint& constraint = parameterDefinitions.at(key);
        
        // Check type-specific validation
        if constexpr (std::is_same_v<decltype(parameterDefinitions), std::unordered_map<std::string, ParameterConstraint>>) {
            if (constraint.validator) {
                if (!constraint.validator(value)) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
    void notifyChangeListeners(const std::string& key, const std::string& oldValue, const std::string& newValue) {
        for (ParameterChangeListener* listener : changeListeners) {
            listener->onParameterChanged(key, oldValue, newValue);
        }
    }
    
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    return pImpl->loadFromFile(filepath);
}

bool Config::loadFromArgs(int argc, char** argv) {
    return pImpl->loadFromArgs(argc, argv);
}

bool Config::saveToFile(const std::string& filepath) const {
    return pImpl->saveToFile(filepath);
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    return pImpl->get<T>(key);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    return pImpl->getOr<T>(key, defaultValue);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    pImpl->set(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    pImpl->set(key, value, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    pImpl->set(key, value, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    pImpl->set(key, value, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    pImpl->set(key, value, source);
}

bool Config::has(const std::string& key) const {
    return pImpl->has(key);
}

void Config::remove(const std::string& key) {
    pImpl->remove(key);
}

std::vector<std::string> Config::getKeys() const {
    return pImpl->getKeys();
}

void Config::clear() {
    pImpl->clear();
}

std::string Config::summary() const {
    return pImpl->summary();
}

} // namespace nlm
