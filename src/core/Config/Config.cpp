#include "Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <unordered_set>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Configuration validation rules
    std::unordered_set<std::string> requiredKeys;
    std::unordered_set<std::string> validKeys;
    
    Impl() {
        // Initialize validation rules
        initializeValidationRules();
    }
    
    void initializeValidationRules() {
        // Define valid configuration keys and their types
        validKeys = {
            // Brain configuration
            "brain.neuron_count",
            "brain.synapse_density", 
            "brain.connection_probability",
            "brain.initial_weight_mean",
            "brain.initial_weight_std",
            "brain.v_thresh",
            "brain.v_rest",
            "brain.v_reset",
            "brain.tau_mem",
            "brain.tau_ref",
            
            // Plasticity configuration
            "plasticity.stdp.enable",
            "plasticity.stdp.learning_rate",
            "plasticity.stdp.tau_plus",
            "plasticity.stdp.tau_minus",
            "plasticity.hebbian.enable",
            "plasticity.structural.enable",
            
            // Neuromodulation configuration
            "neuromod.dopamine.scale",
            "neuromod.curiosity.enable",
            "neuromod.novelty.enable",
            
            // Development configuration
            "development.synaptogenesis_rate",
            "development.pruning_rate",
            "development.maturation_rate",
            
            // Simulation configuration
            "simulation_timestep",
            "random_seed",
            "max_simulation_steps",
            "replay_interval",
            "consolidation_interval",
            
            // Environment configuration
            "environment.name",
            "environment.width",
            "environment.height",
            "environment.vision_width",
            "environment.vision_height",
            
            // Logging configuration
            "log.level",
            "log.to_file",
            "log.filename",
            
            // Visualization configuration
            "visualization.enabled",
            "visualization.update_rate",
            
            // Agent configuration
            "agent.reward_scale",
            "agent.curiosity_weight",
            "agent.novelty_weight",
            
            // Memory configuration
            "memory.episode_capacity",
            "memory.consolidation_threshold"
        };
        
        // Define required keys
        requiredKeys = {
            "brain.neuron_count",
            "brain.synapse_density",
            "simulation_timestep"
        };
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
            return false;
        }
        
        std::string line;
        size_t lineNumber = 0;
        size_t validEntries = 0;
        size_t invalidEntries = 0;
        
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                NLM_LOG_WARNING(std::string("Invalid configuration line ") + 
                              std::to_string(lineNumber) + ": missing '='");
                invalidEntries++;
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_WARNING(std::string("Empty key at line ") + std::to_string(lineNumber));
                invalidEntries++;
                continue;
            }
            
            if (!isValidKey(key)) {
                NLM_LOG_WARNING(std::string("Unknown configuration key: ") + key);
                invalidEntries++;
                continue;
            }
            
            try {
                set(key, value, ConfigSource::File);
                validEntries++;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Failed to set key ") + key + 
                            std::string(" at line ") + std::to_string(lineNumber) + ": " + e.what());
                invalidEntries++;
            }
        }
        
        // Validate configuration
        bool validationPassed = validate();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double loadTime = std::chrono::duration<double>(endTime - startTime).count();
        
        NLM_LOG_INFO(std::string("Loaded configuration from: ") + filepath);
        NLM_LOG_INFO(std::string("  Valid entries: ") + std::to_string(validEntries));
        NLM_LOG_INFO(std::string("  Invalid entries: ") + std::to_string(invalidEntries));
        NLM_LOG_INFO(std::string("  Loading time: ") + std::to_string(loadTime) + "s");
        
        if (!validationPassed) {
            NLM_LOG_WARNING("Configuration validation failed - some required keys may be missing");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading configuration file ") + 
                    filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        size_t validArgs = 0;
        size_t invalidArgs = 0;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    try {
                        set(key, value, ConfigSource::CommandLine);
                        validArgs++;
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR(std::string("Invalid command line argument --") + key + 
                                    std::string("=") + value + ": " + e.what());
                        invalidArgs++;
                    }
                } else {
                    NLM_LOG_WARNING(std::string("Invalid command line argument format: ") + arg);
                    invalidArgs++;
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                try {
                    set(key, value, ConfigSource::CommandLine);
                    validArgs++;
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Invalid command line argument -") + key + 
                                std::string(" ") + value + ": " + e.what());
                    invalidArgs++;
                }
            } else {
                NLM_LOG_WARNING(std::string("Unrecognized command line argument: ") + arg);
                invalidArgs++;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double loadTime = std::chrono::duration<double>(endTime - startTime).count();
        
        NLM_LOG_INFO(std::string("Loaded ") + std::to_string(validArgs) + 
                    std::string(" configuration arguments from command line"));
        NLM_LOG_INFO(std::string("  Invalid arguments: ") + std::to_string(invalidArgs));
        NLM_LOG_INFO(std::string("  Command line loading time: ") + std::to_string(loadTime) + "s");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading configuration from command line: ") + e.what());
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR(std::string("Failed to open configuration file for writing: ") + filepath);
            return false;
        }
        
        file << "# NLM Configuration File\n";
        file << "# Generated on " << getCurrentTimestamp() << "\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << "# Source: ";
            switch (entry.source) {
                case ConfigSource::Default: file << "Default"; break;
                case ConfigSource::File: file << "File"; break;
                case ConfigSource::CommandLine: file << "CommandLine"; break;
                case ConfigSource::Runtime: file << "Runtime"; break;
            }
            file << "\n";
            file << entry.key << " = " << formatConfigValue(entry.value) << "\n\n";
        }
        
        // Write configuration validation
        file << "# Configuration Validation\n";
        file << "# Required keys: ";
        for (const auto& key : pImpl->requiredKeys) {
            if (has(key)) {
                file << key << " ";
            }
        }
        file << "\n\n";
        
        file << "# Validation status: " << (validate() ? "PASSED" : "FAILED") << "\n";
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double saveTime = std::chrono::duration<double>(endTime - startTime).count();
        
        NLM_LOG_INFO(std::string("Saved configuration to: ") + filepath);
        NLM_LOG_INFO(std::string("  Configuration entries: ") + std::to_string(pImpl->entries.size()));
        NLM_LOG_INFO(std::string("  Saving time: ") + std::to_string(saveTime) + "s");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving configuration file ") + 
                    filepath + ": " + e.what());
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });n
        if (it == pImpl->entries.end()) {
            NLM_LOG_DEBUG(std::string("Configuration key not found: ") + key);
            return std::nullopt;
        }
        
        return convertConfigValue<T>(it->value);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error getting configuration key ") + key + ": " + e.what());
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value.has_value()) {
        return *value;
    }
    
    NLM_LOG_DEBUG(std::string("Using default value for key ") + key + ": " + 
                (std::is_same_v<T, std::string> ? std::to_string(defaultValue) : 
                 std::is_same_v<T, double> ? std::to_string(defaultValue) :
                 std::is_same_v<T, int> ? std::to_string(defaultValue) :
                 std::is_same_v<T, int64_t> ? std::to_string(defaultValue) :
                 std::is_same_v<T, bool> ? (defaultValue ? "true" : "false") : 
                 ""));
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (!isValidKey(key)) {
        throw ConfigurationError("Invalid configuration key: " + key);
    }
    
    // Validate value based on key
    validateValue(key, value);
    
    // Remove existing entry
    remove(key);
    
    // Add new entry
    pImpl->entries.emplace_back(key, value, source, getDefaultDescription(key));
    
    NLM_LOG_DEBUG(std::string("Set configuration ") + key + 
                std::string(" = ") + formatConfigValue(value) + 
                std::string(" (source: ") + configSourceToString(source) + ")");
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
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
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        NLM_LOG_DEBUG(std::string("Removed configuration key: ") + key);
        pImpl->entries.erase(it);
    }
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
    NLM_LOG_INFO(std::string("Clearing all configuration entries ( )" + std::to_string(pImpl->entries.size()) + " entries)");
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "NLM Configuration Summary:" << std::endl;
    ss << "  Total entries: " << pImpl->entries.size() << std::endl;
    
    if (pImpl->entries.size() > 0) {
        ss << "  Entries:" << std::endl;
        for (const auto& entry : pImpl->entries) {
            ss << "    " << entry.key << " = " << formatConfigValue(entry.value) << std::endl;
            ss << "      (source: " << configSourceToString(entry.source) << ")" << std::endl;
        }
    }
    
    ss << "  Validation: " << (validate() ? "PASSED" : "FAILED") << std::endl;
    ss << "  Required keys present: ";
    
    size_t presentCount = 0;
    for (const auto& key : pImpl->requiredKeys) {
        if (has(key)) {
            presentCount++;
            ss << key << " ";
        }
    }
    
    ss << std::endl;
    ss << "  Required keys: " << presentCount << "/" << pImpl->requiredKeys.size() << std::endl;
    
    return ss.str();
}

// Private helper methods
bool Config::isValidKey(const std::string& key) const {
    return pImpl->validKeys.find(key) != pImpl->validKeys.end();
}

bool Config::validate() const {
    try {
        // Check required keys
        for (const auto& key : pImpl->requiredKeys) {
            if (!has(key)) {
                NLM_LOG_ERROR(std::string("Missing required configuration key: ") + key);
                return false;
            }
        }
        
        // Validate value ranges for critical parameters
        if (has("brain.neuron_count")) {
            auto neuronCount = get<int64_t>("brain.neuron_count");
            if (!neuronCount || *neuronCount <= 0) {
                NLM_LOG_ERROR("brain.neuron_count must be a positive integer");
                return false;
            }
            if (*neuronCount > 1000000) {
                NLM_LOG_WARNING(std::string("Very high neuron count ( ") + std::to_string(*neuronCount) + ") may impact performance");
            }
        }
        
        if (has("brain.synapse_density")) {
            auto synapseDensity = get<double>("brain.synapse_density");
            if (!synapseDensity || *synapseDensity < 0.0 || *synapseDensity > 1.0) {
                NLM_LOG_ERROR("brain.synapse_density must be between 0.0 and 1.0");
                return false;
            }
        }
        
        if (has("simulation_timestep")) {
            auto timestep = get<double>("simulation_timestep");
            if (!timestep || *timestep <= 0.0) {
                NLM_LOG_ERROR("simulation_timestep must be positive");
                return false;
            }
            if (*timestep > 0.1) {
                NLM_LOG_WARNING(std::string("Large simulation timestep ( ") + std::to_string(*timestep) + ") may affect accuracy");
            }
        }
        
        // Validate plasticity settings
        if (has("plasticity.stdp.learning_rate")) {
            auto learningRate = get<double>("plasticity.stdp.learning_rate");
            if (learningRate && (*learningRate < 0.0 || *learningRate > 1.0)) {
                NLM_LOG_ERROR("plasticity.stdp.learning_rate must be between 0.0 and 1.0");
                return false;
            }
        }
        
        NLM_LOG_INFO("Configuration validation passed");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Configuration validation error: ") + e.what());
        return false;
    }
}

void Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Basic validation based on key patterns
    if (key.find("brain.neuron_count") != std::string::npos) {
        auto val = std::get_if<int64_t>(&value);
        if (!val || *val <= 0) {
            throw ConfigurationError("brain.neuron_count must be a positive integer");
        }
    } else if (key.find("brain.synapse_density") != std::string::npos) {
        auto val = std::get_if<double>(&value);
        if (!val || *val < 0.0 || *val > 1.0) {
            throw ConfigurationError("brain.synapse_density must be between 0.0 and 1.0");
        }
    } else if (key.find("simulation_timestep") != std::string::npos) {
        auto val = std::get_if<double>(&value);
        if (!val || *val <= 0.0) {
            throw ConfigurationError("simulation_timestep must be positive");
        }
    }
}

std::string Config::getDefaultDescription(const std::string& key) const {
    // Return default description for known keys
    static const std::unordered_map<std::string, std::string> descriptions = {
        {"brain.neuron_count", "Total number of neurons in the brain"},
        {"brain.synapse_density", "Connection probability for random connectivity"},
        {"brain.connection_probability", "Probability of connection between neurons"},
        {"brain.initial_weight_mean", "Mean initial synaptic weight"},
        {"brain.initial_weight_std", "Standard deviation of initial weights"},
        {"plasticity.stdp.learning_rate", "STDP learning rate"},
        {"plasticity.stdp.tau_plus", "STDP time constant for potentiation"},
        {"plasticity.stdp.tau_minus", "STDP time constant for depression"},
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"log.level", "Logging level (DEBUG, INFO, WARNING, ERROR)"},
        {"environment.width", "Environment width in meters"},
        {"environment.height", "Environment height in meters"}
    };
    
    auto it = descriptions.find(key);
    if (it != descriptions.end()) {
        return it->second;
    }
    
    return "Configuration parameter: " + key;
}

std::string Config::configSourceToString(ConfigSource source) const {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Runtime: return "Runtime";
        default: return "Unknown";
    }
}

std::string Config::formatConfigValue(const ConfigValue& value) const {
    std::stringstream ss;
    
    if (const auto* v = std::get_if<int>(&value)) {
        ss << *v;
    } else if (const auto* v = std::get_if<int64_t>(&value)) {
        ss << *v;
    } else if (const auto* v = std::get_if<double>(&value)) {
        // Format double to avoid scientific notation
        ss << std::fixed << std::setprecision(6) << *v;
    } else if (const auto* v = std::get_if<bool>(&value)) {
        ss << (*v ? "true" : "false");
    } else if (const auto* v = std::get_if<std::string>(&value)) {
        ss << '"' << *v << '"';
    } else if (const auto* v = std::get_if<std::vector<int>>(&value)) {
        ss << "[";
        for (size_t i = 0; i < v->size(); ++i) {
            if (i > 0) ss << ", ";
            ss << (*v)[i];
        }
        ss << "]";
    } else if (const auto* v = std::get_if<std::vector<double>>(&value)) {
        ss << "[";
        for (size_t i = 0; i < v->size(); ++i) {
            if (i > 0) ss << ", ";
            ss << std::fixed << std::setprecision(6) << (*v)[i];
        }
        ss << "]";
    } else if (const auto* v = std::get_if<std::vector<std::string>>(&value)) {
        ss << "[";
        for (size_t i = 0; i < v->size(); ++i) {
            if (i > 0) ss << ", ";
            ss << '"' << (*v)[i] << '"';
        }
        ss << "]";
    } else {
        ss << "<unknown>";
    }
    
    return ss.str();
}

template<typename T>
T Config::convertConfigValue(const ConfigValue& value) const {
    if constexpr (std::is_same_v<T, int>) {
        if (const auto* v = std::get_if<int>(&value)) {
            return *v;
        } else if (const auto* v = std::get_if<int64_t>(&value)) {
            return static_cast<int>(*v);
        } else if (const auto* v = std::get_if<double>(&value)) {
            return static_cast<int>(*v);
        } else if (const auto* v = std::get_if<std::string>(&value)) {
            // Try to parse string to int
            return std::stoi(*v);
        } else {
            throw ConfigurationError("Cannot convert configuration value to int");
        }
    } else if constexpr (std::is_same_v<T, int64_t>) {
        if (const auto* v = std::get_if<int64_t>(&value)) {
            return *v;
        } else if (const auto* v = std::get_if<int>(&value)) {
            return static_cast<int64_t>(*v);
        } else if (const auto* v = std::get_if<double>(&value)) {
            return static_cast<int64_t>(*v);
        } else if (const auto* v = std::get_if<std::string>(&value)) {
            return std::stoll(*v);
        } else {
            throw ConfigurationError("Cannot convert configuration value to int64_t");
        }
    } else if constexpr (std::is_same_v<T, double>) {
        if (const auto* v = std::get_if<double>(&value)) {
            return *v;
        } else if (const auto* v = std::get_if<int>(&value)) {
            return static_cast<double>(*v);
        } else if (const auto* v = std::get_if<int64_t>(&value)) {
            return static_cast<double>(*v);
        } else if (const auto* v = std::get_if<std::string>(&value)) {
            return std::stod(*v);
        } else {
            throw ConfigurationError("Cannot convert configuration value to double");
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        if (const auto* v = std::get_if<bool>(&value)) {
            return *v;
        } else if (const auto* v = std::get_if<int>(&value)) {
            return *v != 0;
        } else if (const auto* v = std::get_if<double>(&value)) {
            return *v != 0.0;
        } else if (const auto* v = std::get_if<std::string>(&value)) {
            std::string lower = *v;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
        } else {
            throw ConfigurationError("Cannot convert configuration value to bool");
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (const auto* v = std::get_if<std::string>(&value)) {
            return *v;
        } else if (const auto* v = std::get_if<int>(&value)) {
            return std::to_string(*v);
        } else if (const auto* v = std::get_if<int64_t>(&value)) {
            return std::to_string(*v);
        } else if (const auto* v = std::get_if<double>(&value)) {
            return std::to_string(*v);
        } else if (const auto* v = std::get_if<bool>(&value)) {
            return *v ? "true" : "false";
        } else {
            throw ConfigurationError("Cannot convert configuration value to string");
        }
    } else {
        static_assert(!std::is_same_v<T, T>, "Unsupported configuration value type");
        return T();
    }
}

std::string Config::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && std::isspace(str[start])) {
        start++;
    }
    
    while (end > start && std::isspace(str[end - 1])) {
        end--;
    }
    
    return str.substr(start, end - start);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

} // namespace nlm
