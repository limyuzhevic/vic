# Advanced NLM Configuration System
# Enhanced configuration management with runtime modification and debugging

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace nlm {

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values
    initializeDefaults();
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void Config::initializeDefaults() {
    // Default configuration values for NLM
    pImpl->entries["random_seed"] = ConfigEntry("random_seed", 42, ConfigSource::Default, "Random seed for reproducible experiments");
    pImpl->entries["simulation_timestep"] = ConfigEntry("simulation_timestep", 0.001, ConfigSource::Default, "Simulation timestep in seconds");
    pImpl->entries["neuron_count"] = ConfigEntry("neuron_count", 1000, ConfigSource::Default, "Number of neurons in the brain");
    pImpl->entries["region_count"] = ConfigEntry("region_count", 1, ConfigSource::Default, "Number of brain regions");
    pImpl->entries["connection_probability"] = ConfigEntry("connection_probability", 0.1f, ConfigSource::Default, "Probability of synaptic connections");
    
    // STDP parameters
    pImpl->entries["stdp_ltp_weight"] = ConfigEntry("stdp_ltp_weight", 0.01f, ConfigSource::Default, "STDP long-term potentiation weight");
    pImpl->entries["stdp_ltd_weight"] = ConfigEntry("stdp_ltd_weight", 0.012f, ConfigSource::Default, "STDP long-term depression weight");
    pImpl->entries["stdp_tau"] = ConfigEntry("stdp_tau", 20.0f, ConfigSource::Default, "STDP time constant");
    
    // Structural plasticity
    pImpl->entries["synaptogenesis_rate"] = ConfigEntry("synaptogenesis_rate", 0.0001f, ConfigSource::Default, "Synaptogenesis rate");
    pImpl->entries["pruning_rate"] = ConfigEntry("pruning_rate", 0.00001f, ConfigSource::Default, "Pruning rate");
    
    // Development
    pImpl->entries["development_enabled"] = ConfigEntry("development_enabled", true, ConfigSource::Default, "Enable development system");
    pImpl->entries["development_start_age"] = ConfigEntry("development_start_age", 60.0, ConfigSource::Default, "Development start age");
    pImpl->entries["development_maturity_age"] = ConfigEntry("development_maturity_age", 300.0, ConfigSource::Default, "Development maturity age");
    
    // Neuromodulation
    pImpl->entries["dopamine_sensitivity"] = ConfigEntry("dopamine_sensitivity", 1.0f, ConfigSource::Default, "Dopamine sensitivity");
    pImpl->entries["curiosity_threshold"] = ConfigEntry("curiosity_threshold", 0.3f, ConfigSource::Default, "Curiosity threshold");
    pImpl->entries["novelty_decay_rate"] = ConfigEntry("novelty_decay_rate", 0.99f, ConfigSource::Default, "Novelty decay rate");
    
    // Memory
    pImpl->entries["working_memory_capacity"] = ConfigEntry("working_memory_capacity", 1000, ConfigSource::Default, "Working memory capacity");
    pImpl->entries["episodic_memory_capacity"] = ConfigEntry("episodic_memory_capacity", 500, ConfigSource::Default, "Episodic memory capacity");
    
    // Prediction
    pImpl->entries["prediction_horizon"] = ConfigEntry("prediction_horizon", 10, ConfigSource::Default, "Prediction horizon (steps)");
    pImpl->entries["prediction_error_decay"] = ConfigEntry("prediction_error_decay", 0.9f, ConfigSource::Default, "Prediction error decay rate");
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Could not open configuration file: " + filepath);
        return false;
    }
    
    try {
        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            
            line = trim(line);
            if (line.empty()) continue;
            
            // Parse key=value format
            size_t equalsPos = line.find('=');
            if (equalsPos == std::string::npos) continue;
            
            std::string key = trim(line.substr(0, equalsPos));
            std::string valueStr = trim(line.substr(equalsPos + 1));
            
            // Try to parse different types
            bool parsed = false;
            
            // Try integer
            try {
                int intValue = std::stoi(valueStr);
                set(key, intValue, ConfigSource::File);
                parsed = true;
            } catch (...) {}
            
            // Try double
            if (!parsed) {
                try {
                    double doubleValue = std::stod(valueStr);
                    set(key, doubleValue, ConfigSource::File);
                    parsed = true;
                } catch (...) {}
            }
            
            // Try boolean
            if (!parsed) {
                if (valueStr == "true" || valueStr == "1") {
                    set(key, true, ConfigSource::File);
                    parsed = true;
                } else if (valueStr == "false" || valueStr == "0") {
                    set(key, false, ConfigSource::File);
                    parsed = true;
                }
            }
            
            // Try string
            if (!parsed) {
                set(key, valueStr, ConfigSource::File);
                parsed = true;
            }
            
            if (!parsed) {
                NLM_LOG_WARNING("Could not parse config line: " + line);
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration file: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool loaded = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Check for --key=value format
        if (arg.find("--") == 0) {
            size_t equalsPos = arg.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = trim(arg.substr(2, equalsPos - 2));
                std::string valueStr = trim(arg.substr(equalsPos + 1));
                
                // Try to parse different types
                bool parsed = false;
                
                // Try integer
                try {
                    int intValue = std::stoi(valueStr);
                    set(key, intValue, ConfigSource::CommandLine);
                    parsed = true;
                } catch (...) {}
                
                // Try double
                if (!parsed) {
                    try {
                        double doubleValue = std::stod(valueStr);
                        set(key, doubleValue, ConfigSource::CommandLine);
                        parsed = true;
                    } catch (...) {}
                }
                
                // Try boolean
                if (!parsed) {
                    if (valueStr == "true" || valueStr == "1") {
                        set(key, true, ConfigSource::CommandLine);
                        parsed = true;
                    } else if (valueStr == "false" || valueStr == "0") {
                        set(key, false, ConfigSource::CommandLine);
                        parsed = true;
                    }
                }
                
                // Try string
                if (!parsed) {
                    set(key, valueStr, ConfigSource::CommandLine);
                    parsed = true;
                }
                
                if (parsed) {
                    NLM_LOG_INFO("Loaded from command line: " + key + " = " + valueStr);
                    loaded = true;
                }
            }
        }
    }
    
    return loaded;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open configuration file for writing: " + filepath);
        return false;
    }
    
    try {
        file << "# NLM Configuration File" << std::endl;
        file << "# Generated on " << getCurrentTimestamp() << std::endl;
        file << std::endl;
        
        for (const auto& pair : pImpl->entries) {
            const ConfigEntry& entry = pair.second;
            file << entry.key << " = ";
            
            // Format based on value type
            if (std::holds_alternative<int>(entry.value)) {
                file << std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                file << std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                file << std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                file << (std::get<bool>(entry.value) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(entry.value)) {
                file << "\"" << std::get<std::string>(entry.value) << "\"";
            } else {
                file << "[unknown]";
            }
            
            file << "  # " << entry.description << std::endl;
        }
        
        file << std::endl;
        NLM_LOG_INFO("Configuration saved to file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration file: " + std::string(e.what()));
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        if constexpr (std::is_same_v<T, int>) {
            return std::get<int>(it->second.value);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::get<int64_t>(it->second.value);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::get<double>(it->second.value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return std::get<bool>(it->second.value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return std::get<std::string>(it->second.value);
        }
    } catch (...) {
        return std::nullopt;
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value.has_value()) {
        return *value;
    }
    
    // Try to convert default value to the requested type
    if constexpr (std::is_same_v<T, int>) {
        if (auto intVal = get<int>(key)) {
            return *intVal;
        }
    } else if constexpr (std::is_same_v<T, int64_t>) {
        if (auto int64Val = get<int64_t>(key)) {
            return *int64Val;
        }
    } else if constexpr (std::is_same_v<T, double>) {
        if (auto doubleVal = get<double>(key)) {
            return *doubleVal;
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        if (auto boolVal = get<bool>(key)) {
            return *boolVal;
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (auto stringVal = get<std::string>(key)) {
            return *stringVal;
        }
    }
    
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::string normalizedKey = toLower(key);
    
    if (pImpl->entries.find(normalizedKey) == pImpl->entries.end()) {
        pImpl->entries[normalizedKey] = ConfigEntry(key, value, source, "Dynamic configuration");
        NLM_LOG_INFO("Added new config key: " + key);
    } else {
        pImpl->entries[normalizedKey].value = value;
        pImpl->entries[normalizedKey].source = source;
        NLM_LOG_INFO("Updated config key: " + key);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, value, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
}

bool Config::has(const std::string& key) const {
    return pImpl->entries.find(toLower(key)) != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    std::string normalizedKey = toLower(key);
    if (pImpl->entries.find(normalizedKey) != pImpl->entries.end()) {
        NLM_LOG_INFO("Removed config key: " + key);
        pImpl->entries.erase(normalizedKey);
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& pair : pImpl->entries) {
        keys.push_back(pair.first);
    }
    return keys;
}

void Config::clear() {
    NLM_LOG_INFO("Cleared all configuration entries");
    pImpl->entries.clear();
    initializeDefaults();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "NLM Configuration Summary:" << std::endl;
    
    for (const auto& pair : pImpl->entries) {
        const ConfigEntry& entry = pair.second;
        oss << "  " << entry.key << ": ";
        
        // Format value
        if (std::holds_alternative<int>(entry.value)) {
            oss << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            oss << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            oss << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            oss << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            oss << std::get<std::string>(entry.value);
        }
        
        oss << " (Source: " << static_cast<int>(entry.source) << ")" << std::endl;
    }
    
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return str;
    
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
    return result;
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

} // namespace nlm
