// Config.cpp - Configuration System Implementation
// Implements persistent configuration with JSON file support
// Supports command-line configuration, runtime settings, and file I/O

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <cassert>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool sorted;  // For efficient key lookup
    
    Impl() : sorted(true) {}
    
    // Find entry by key (linear search for now)
    ConfigEntry* find(const std::string& key) {
        sorted = false;
        for (auto& entry : entries) {
            if (entry.key == key) {
                return &entry;
            }
        }
        return nullptr;
    }
    
    const ConfigEntry* find(const std::string& key) const {
        for (const auto& entry : entries) {
            if (entry.key == key) {
                return &entry;
            }
        }
        return nullptr;
    }
    
    // Re-sort entries for binary search (if needed)
    void ensureSorted() {
        if (!sorted && entries.size() > 1) {
            std::sort(entries.begin(), entries.end(),
                     [](const ConfigEntry& a, const ConfigEntry& b) {
                         return a.key < b.key;
                     });
            sorted = true;
        }
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values
    pImpl->entries.push_back(ConfigEntry("random_seed", ConfigValue(42), ConfigSource::Default, "Random seed for reproducible experiments"));
    pImpl->entries.push_back(ConfigEntry("simulation_timestep", ConfigValue(0.001), ConfigSource::Default, "Simulation timestep in seconds"));
    pImpl->entries.push_back(ConfigEntry("neuron_count", ConfigValue(1000), ConfigSource::Default, "Number of neurons in the brain"));
    pImpl->entries.push_back(ConfigEntry("region_count", ConfigValue(2), ConfigSource::Default, "Number of brain regions"));
    pImpl->entries.push_back(ConfigEntry("connection_probability", ConfigValue(0.1), ConfigSource::Default, "Connection probability for random connectivity"));
    pImpl->entries.push_back(ConfigEntry("stdp_ltp_weight", ConfigValue(0.01), ConfigSource::Default, "STDP long-term potentiation weight"));
    pImpl->entries.push_back(ConfigEntry("stdp_ltd_weight", ConfigValue(0.012), ConfigSource::Default, "STDP long-term depression weight"));
    pImpl->entries.push_back(ConfigEntry("stdp_tau", ConfigValue(20.0), ConfigSource::Default, "STDP time constant (ms)"));
    pImpl->entries.push_back(ConfigEntry("synaptogenesis_rate", ConfigValue(0.001), ConfigSource::Default, "Rate of new synapse formation"));
    pImpl->entries.push_back(ConfigEntry("pruning_rate", ConfigValue(0.0001), ConfigSource::Default, "Rate of synapse elimination"));
    pImpl->entries.push_back(ConfigEntry("dopamine_scale", ConfigValue(1.0), ConfigSource::Default, "Dopamine modulation scale"));
    pImpl->entries.push_back(ConfigEntry("curiosity_enable", ConfigValue(true), ConfigSource::Default, "Enable curiosity-driven exploration"));
    pImpl->entries.push_back(ConfigEntry("novelty_enable", ConfigValue(true), ConfigSource::Default, "Enable novelty detection"));
    pImpl->entries.push_back(ConfigEntry("prediction_error_sensitivity", ConfigValue(1.0), ConfigSource::Default, "Sensitivity to prediction errors"));
    pImpl->entries.push_back(ConfigEntry("reward_discount_factor", ConfigValue(0.99), ConfigSource::Default, "Reward discount factor"));
    pImpl->entries.push_back(ConfigEntry("environment_name", ConfigValue(std::string("GridWorld")), ConfigSource::Default, "Environment name"));
    pImpl->entries.push_back(ConfigEntry("environment_width", ConfigValue(10), ConfigSource::Default, "Environment width"));
    pImpl->entries.push_back(ConfigEntry("environment_height", ConfigValue(10), ConfigSource::Default, "Environment height"));
    pImpl->entries.push_back(ConfigEntry("working_memory_capacity", ConfigValue(100), ConfigSource::Default, "Capacity of working memory"));
    pImpl->entries.push_back(ConfigEntry("episodic_memory_max_episodes", ConfigValue(1000), ConfigSource::Default, "Maximum episodic memory episodes"));
    pImpl->entries.push_back(ConfigEntry("associative_memory_pattern_limit", ConfigValue(10000), ConfigSource::Default, "Pattern storage limit"));
    pImpl->entries.push_back(ConfigEntry("planning_depth", ConfigValue(5), ConfigSource::Default, "Depth of action planning"));
    pImpl->entries.push_back(ConfigEntry("attention_inhibition_strength", ConfigValue(0.5), ConfigSource::Default, "Strength of attentional inhibition"));
    pImpl->entries.push_back(ConfigEntry("attention_excitation_strength", ConfigValue(1.5), ConfigSource::Default, "Strength of attentional excitation"));
    pImpl->entries.push_back(ConfigEntry("log_level", ConfigValue(std::string("INFO")), ConfigSource::Default, "Log level (DEBUG, INFO, WARNING, ERROR)"));
    pImpl->entries.push_back(ConfigEntry("log_to_file", ConfigValue(false), ConfigSource::Default, "Log to file"));
    pImpl->entries.push_back(ConfigEntry("log_filename", ConfigValue(std::string("nlm.log")), ConfigSource::Default, "Log filename"));
    pImpl->entries.push_back(ConfigEntry("max_simulation_steps", ConfigValue(10000), ConfigSource::Default, "Maximum simulation steps"));
    pImpl->entries.push_back(ConfigEntry("simulation_time_limit", ConfigValue(0.0), ConfigSource::Default, "Simulation time limit"));
    pImpl->entries.push_back(ConfigEntry("visualization_enabled", ConfigValue(false), ConfigSource::Default, "Enable visualization"));
    pImpl->entries.push_back(ConfigEntry("visualization_update_rate", ConfigValue(30.0), ConfigSource::Default, "Visualization update rate (Hz)"));
    pImpl->entries.push_back(ConfigEntry("checkpoint_dir", ConfigValue(std::string("./checkpoints")), ConfigSource::Default, "Checkpoint directory"));
    pImpl->entries.push_back(ConfigEntry("checkpoint_interval", ConfigValue(1000), ConfigSource::Default, "Checkpoint save interval"));
    pImpl->entries.push_back(ConfigEntry("checkpoint_compress", ConfigValue(true), ConfigSource::Default, "Compress checkpoints"));
    pImpl->entries.push_back(ConfigEntry("replay_interval", ConfigValue(100), ConfigSource::Default, "Memory replay interval"));
    pImpl->entries.push_back(ConfigEntry("consolidation_interval", ConfigValue(1000), ConfigSource::Default, "Memory consolidation interval"));
    pImpl->entries.push_back(ConfigEntry("development_update_interval", ConfigValue(1000), ConfigSource::Default, "Development update interval"));
    pImpl->entries.push_back(ConfigEntry("integrate_memory_systems", ConfigValue(true), ConfigSource::Default, "Integrate memory systems"));
    pImpl->entries.push_back(ConfigEntry("integrate_prediction_system", ConfigValue(true), ConfigSource::Default, "Integrate prediction system"));
    pImpl->entries.push_back(ConfigEntry("integrate_cognition_systems", ConfigValue(true), ConfigSource::Default, "Integrate cognition systems"));
    pImpl->entries.push_back(ConfigEntry("integrate_neuromodulation", ConfigValue(true), ConfigSource::Default, "Integrate neuromodulation"));
    pImpl->entries.push_back(ConfigEntry("integrate_development", ConfigValue(true), ConfigSource::Default, "Integrate development system"));
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    NLM_LOG_INFO("Loading configuration from file: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    try {
        // Clear existing configuration
        pImpl->entries.clear();
        pImpl->sorted = true;
        
        // Simple JSON parsing (for demonstration - production would use a proper JSON parser)
        std::string line;
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line.find("//") == 0) {
                continue;
            }
            
            // Parse key = value format (simple)
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                continue;
            }
            
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            // Remove quotes from string values
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to parse value type
            ConfigValue configValue;
            bool parsed = false;
            
            // Try integer
            try {
                size_t pos;
                long long intVal = std::stoll(value, &pos);
                if (pos == value.size()) {
                    configValue = intVal;
                    parsed = true;
                }
            } catch (...) {}
            
            // Try double
            if (!parsed) {
                try {
                    size_t pos;
                    double doubleVal = std::stod(value, &pos);
                    if (pos == value.size()) {
                        configValue = doubleVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try boolean
            if (!parsed) {
                if (value == "true") {
                    configValue = true;
                    parsed = true;
                } else if (value == "false") {
                    configValue = false;
                    parsed = true;
                }
            }
            
            // String (assume it's a string if not parsed)
            if (!parsed) {
                configValue = value;
            }
            
            // Add to configuration
            pImpl->entries.push_back(ConfigEntry(key, configValue, ConfigSource::File, "Loaded from file"));
        }
        
        NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + " configuration entries");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration file: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Loading configuration from command line arguments");
    
    bool changed = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Check for --key=value or --key value patterns
        if (arg.find("--") == 0) {
            // Handle --key=value
            if (arg.find("=") != std::string::npos) {
                std::string key = arg.substr(2, arg.find("=") - 2);
                std::string valueStr = arg.substr(arg.find("=") + 1);
                
                // Try to parse value type
                ConfigValue configValue;
                bool parsed = false;
                
                // Try integer
                try {
                    size_t pos;
                    long long intVal = std::stoll(valueStr, &pos);
                    if (pos == valueStr.size()) {
                        configValue = intVal;
                        parsed = true;
                    }
                } catch (...) {}
                
                // Try double
                if (!parsed) {
                    try {
                        size_t pos;
                        double doubleVal = std::stod(valueStr, &pos);
                        if (pos == valueStr.size()) {
                            configValue = doubleVal;
                            parsed = true;
                        }
                    } catch (...) {}
                }
                
                // Try boolean
                if (!parsed) {
                    if (valueStr == "true") {
                        configValue = true;
                        parsed = true;
                    } else if (valueStr == "false") {
                        configValue = false;
                        parsed = true;
                    }
                }
                
                // String (assume it's a string if not parsed)
                if (!parsed) {
                    configValue = valueStr;
                }
                
                set(key, configValue, ConfigSource::CommandLine);
                changed = true;
                
            } else if (i + 1 < argc && std::string(argv[i + 1])[0] != '-') {
                // Handle --key value pattern
                std::string key = arg.substr(2);
                std::string valueStr = argv[++i];
                
                ConfigValue configValue = valueStr; // Default to string
                set(key, configValue, ConfigSource::CommandLine);
                changed = true;
            }
        }
    }
    
    if (changed) {
        NLM_LOG_INFO("Updated configuration from command line");
    }
    
    return changed;
}

bool Config::saveToFile(const std::string& filepath) const {
    NLM_LOG_INFO("Saving configuration to file: " + filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    try {
        file << "{\n";
        
        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) {
                file << ",\n";
            }
            first = false;
            
            file << "  \"" << entry.key << "\": ";
            
            // Handle different value types
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << std::fixed << std::setprecision(6) << val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (val ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << val << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << val[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << std::fixed << std::setprecision(6) << val[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << val[i] << "\"";
                    }
                    file << "]";
                }
            }, entry.value);
        }
        
        file << "\n}\n";
        
        NLM_LOG_INFO("Configuration saved successfully to " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration file: " + std::string(e.what()));
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    pImpl->ensureSorted();
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            try {
                std::optional<T> result = std::get<T>(entry.value);
                return result;
            } catch (...) {
                // Type mismatch - return empty optional
                return std::nullopt;
            }
        }
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value.has_value()) {
        return *value;
    }
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    ConfigEntry* existing = pImpl->find(key);
    if (existing) {
        // Update existing entry
        existing->value = value;
        existing->source = source;
    } else {
        // Add new entry
        pImpl->entries.push_back(ConfigEntry(key, value, source, "Set at runtime"));
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
    return pImpl->find(key) != nullptr;
}

void Config::remove(const std::string& key) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                           [&key](const ConfigEntry& entry) {
                               return entry.key == key;
                           });
    pImpl->entries.erase(it, pImpl->entries.end());
    pImpl->sorted = false;
}

std::vector<std::string> Config::getKeys() const {
    pImpl->ensureSorted();
    
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->sorted = true;
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "Configuration Summary:" << std::endl;
    
    pImpl->ensureSorted();
    
    for (const auto& entry : pImpl->entries) {
        ss << "  " << entry.key << " = ";
        
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                ss << val;
            } else if constexpr (std::is_same_v<T, double>) {
                ss << std::fixed << std::setprecision(6) << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                ss << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                ss << "\"" << val << "\"";
            } else {
                ss << "[complex value]";
            }
        }, entry.value);
        
        ss << " (source: " << static_cast<int>(entry.source) << ")" << std::endl;
    }
    
    return ss.str();
}

// String helper functions
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return str;
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
    return result;
}

} // namespace nlm