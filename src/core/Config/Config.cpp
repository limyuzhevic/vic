#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <random>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Set default values for key configuration parameters
    set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default, "Total number of neurons in the brain");
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default, "Number of neural regions");
    set("connection_probability", 0.1f, ConfigSource::Default, "Probability of connections between neurons");
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default, "Random seed for reproducibility");
    set("simulation_timestep", 0.001, ConfigSource::Default, "Simulation time step in seconds");
    set("v_thresh", -50.0f, ConfigSource::Default, "Neuron threshold potential (mV)");
    set("v_rest", -70.0f, ConfigSource::Default, "Neuron resting potential (mV)");
    set("v_reset", -75.0f, ConfigSource::Default, "Neuron reset potential after spike (mV)");
    set("tau_mem", 20.0f, ConfigSource::Default, "Membrane time constant (ms)");
    set("tau_ref", 2.0f, ConfigSource::Default, "Refractory period (ms)");
    
    // STDP parameters
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default, "STDP long-term potentiation weight");
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default, "STDP long-term depression weight");
    set("stdp_tau", 20.0f, ConfigSource::Default, "STDP time constant (ms)");
    
    // Structural plasticity parameters
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default, "Rate of new synapse formation");
    set("pruning_rate", 0.00001f, ConfigSource::Default, "Rate of synapse elimination");
    
    // Neuromodulation parameters
    set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default, "Dopamine modulation scale");
    set("neuromod.curiosity.enable", true, ConfigSource::Default, "Enable curiosity-driven exploration");
    set("neuromod.novelty.enable", true, ConfigSource::Default, "Enable novelty detection");
    
    // Memory system parameters
    set("memory.working.capacity", 1000, ConfigSource::Default, "Working memory capacity");
    set("memory.episodic.max_episodes", 1000, ConfigSource::Default, "Maximum episodic memory episodes");
    set("memory.associative.max_patterns", 5000, ConfigSource::Default, "Maximum associative memory patterns");
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    if (filepath.empty()) {
        NLM_LOG_ERROR("Cannot load from empty filepath");
        return false;
    }
    
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_WARN("Configuration file does not exist: " + filepath);
        return false;
    }
    
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
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
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to detect type and convert
            setWithTypeDetection(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARN("Invalid config line " + std::to_string(lineNumber) + " in " + filepath + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from: " + filepath);
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        return true; // No arguments to parse
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setWithTypeDetection(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setWithTypeDetection(key, value, ConfigSource::CommandLine);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        NLM_LOG_ERROR("Cannot save to empty filepath");
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM system\n\n";
    
    // Write all configuration entries
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << " (source: " << 
            (entry.source == ConfigSource::Default ? "default" :
             entry.source == ConfigSource::File ? "file" :
             entry.source == ConfigSource::CommandLine ? "command_line" :
             entry.source == ConfigSource::Runtime ? "runtime" : "unknown") << ")\n";
        
        // Format based on type
        std::visit([&file, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, int>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, float>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << "\n";
    }
    
    NLM_LOG_INFO("Saved configuration to: " + filepath);
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        NLM_LOG_WARN("Cannot get value for empty key");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Config key not found: " + key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access& e) {
        NLM_LOG_ERROR("Config type mismatch for key " + key + ": " + std::string(e.what()));
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value.has_value()) {
        return *value;
    }
    
    NLM_LOG_DEBUG("Using default value for key " + key + ": " + std::to_string(defaultValue));
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    setWithValidation(key, value, source, "");
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    setWithValidation(key, ConfigValue(value), source, "");
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    setWithValidation(key, ConfigValue(value), source, "");
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    setWithValidation(key, ConfigValue(value), source, "");
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    setWithValidation(key, ConfigValue(value), source, "");
}

bool Config::has(const std::string& key) const {
    if (key.empty()) {
        return false;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    return it != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    if (key.empty()) {
        NLM_LOG_WARN("Cannot remove empty key");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it);
        NLM_LOG_INFO("Removed config key: " + key);
    } else {
        NLM_LOG_DEBUG("Config key not found for removal: " + key);
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    NLM_LOG_INFO("Cleared all configuration entries");
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "Configuration Summary (" << pImpl->entries.size() << " entries):";
    
    for (const auto& entry : pImpl->entries) {
        ss << "\n  " << entry.key << " = ";
        
        std::visit([&ss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                ss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                ss << (arg ? "true" : "false");
            } else {
                ss << arg;
            }
        }, entry.value);
        
        ss << " (" << static_cast<int>(entry.source) << ")";
    }
    
    return ss.str();
}

void Config::setWithTypeDetection(const std::string& key, const std::string& value, ConfigSource source) {
    // Try different types in order of likelihood
    if (value.empty() || (value.size() > 1 && value.front() == '"' && value.back() == '"')) {
        // String value
        setWithValidation(key, ConfigValue(value), source, "String value: " + value);
    } else if (value == "true" || value == "false") {
        // Boolean value
        bool boolValue = (value == "true");
        setWithValidation(key, ConfigValue(boolValue), source, "Boolean value: " + value);
    } else {
        // Try numeric types
        try {
            if (value.find('.') != std::string::npos || value.find('e') != std::string::npos || value.find('E') != std::string::npos) {
                // Float/double
                float floatValue = std::stof(value);
                setWithValidation(key, ConfigValue(floatValue), source, "Float value: " + value);
            } else {
                // Integer
                int intValue = std::stoi(value);
                setWithValidation(key, ConfigValue(intValue), source, "Integer value: " + value);
            }
        } catch (const std::exception& e) {
            // If numeric parsing fails, treat as string
            setWithValidation(key, ConfigValue(value), source, "String value (numeric parse failed): " + value);
        }
    }
}

void Config::setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description) {
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set configuration with empty key");
        return;
    }
    
    // Remove existing entry with same key
    remove(key);
    
    // Add new entry
    ConfigEntry entry;
    entry.key = key;
    entry.value = value;
    entry.source = source;
    entry.description = description.empty() ? "" : description;
    
    pImpl->entries.push_back(entry);
    
    NLM_LOG_DEBUG("Set config " + key + " = " + getValueString(value));
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string Config::getValueString(const ConfigValue& value) {
    std::stringstream ss;
    std::visit([&ss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            ss << (arg ? "true" : "false");
        } else {
            ss << arg;
        }
    }, value);
    return ss.str();
}

} // namespace nlm
