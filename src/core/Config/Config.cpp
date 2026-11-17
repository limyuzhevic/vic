#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

/**
 * @brief Pimpl implementation structure for Config class
 * 
 * This structure contains all the implementation details of the Config class,
 * following the Pimpl idiom to separate interface from implementation and
 * reduce compilation dependencies. The implementation handles:
 * - File parsing for multiple formats (JSON, YAML, TOML, key=value)
 * - Configuration validation and type checking
 * - Source tracking for configuration values
 * - File I/O operations
 * - Template instantiation for type-safe accessors
 */
struct Config::Impl {
    std::vector<ConfigEntry> entries;  ///< Configuration entries
    
    /**
     * @brief Default constructor
     */
    Impl() {}
    
    /**
     * @brief Add or update a configuration entry
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of the configuration
     * @param description Description of the setting
     */
    void set(const std::string& key, const ConfigValue& value, 
             const ConfigSource& source, const std::string& description = "") {
        auto it = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != entries.end()) {
            it->value = value;
            it->source = source;
            it->description = description;
        } else {
            entries.emplace_back(key, value, source, description);
        }
    }
    
    /**
     * @brief Check if configuration has a key
     */
    bool has(const std::string& key) const {
        return std::any_of(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    }
    
    /**
     * @brief Get configuration entry by key
     */
    const ConfigEntry* get(const std::string& key) const {
        auto it = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        return (it != entries.end()) ? &(*it) : nullptr;
    }
    
    /**
     * @brief Remove configuration entry
     */
    void remove(const std::string& key) {
        entries.erase(std::remove_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
            entries.end());
    }
    
    /**
     * @brief Get all configuration keys
     */
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(entries.size());
        for (const auto& entry : entries) {
            keys.push_back(entry.key);
        }
        return keys;
    }
    
    /**
     * @brief Clear all configuration
     */
    void clear() {
        entries.clear();
    }
    
    /**
     * @brief Get configuration value as string representation
     */
    std::string getStringValue(const ConfigValue& value) const {
        std::ostringstream oss;
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
        }, value);
        return oss.str();
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with built-in defaults
    initializeDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaults() {
    // Core brain parameters
    pImpl->set("brain.neuron_count", 1000, ConfigSource(ConfigSource::Type::Default), 
               "Total number of neurons in the brain");
    pImpl->set("brain.synapse_density", 0.1f, ConfigSource(ConfigSource::Type::Default),
               "Synapse connectivity density (0.0-1.0)");
    pImpl->set("brain.connection_probability", 0.05f, ConfigSource(ConfigSource::Type::Default),
               "Probability of connection between neurons");
    
    // Neuron parameters
    pImpl->set("brain.v_thresh", -50.0f, ConfigSource(ConfigSource::Type::Default),
               "Neuron firing threshold (mV)");
    pImpl->set("brain.v_rest", -70.0f, ConfigSource(ConfigSource::Type::Default),
               "Resting membrane potential (mV)");
    pImpl->set("brain.v_reset", -75.0f, ConfigSource(ConfigSource::Type::Default),
               "Reset membrane potential after spike (mV)");
    pImpl->set("brain.tau_mem", 20.0f, ConfigSource(ConfigSource::Type::Default),
               "Membrane time constant (ms)");
    pImpl->set("brain.tau_ref", 2.0f, ConfigSource(ConfigSource::Type::Default),
               "Refractory period (ms)");
    
    // Plasticity parameters
    pImpl->set("plasticity.stdp.enable", true, ConfigSource(ConfigSource::Type::Default),
               "Enable Spike-Timing-Dependent Plasticity");
    pImpl->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource(ConfigSource::Type::Default),
               "STDP learning rate");
    pImpl->set("plasticity.stdp.tau_plus", 20.0f, ConfigSource(ConfigSource::Type::Default),
               "STDP time constant for long-term potentiation");
    pImpl->set("plasticity.stdp.tau_minus", 20.0f, ConfigSource(ConfigSource::Type::Default),
               "STDP time constant for long-term depression");
    
    pImpl->set("plasticity.hebbian.enable", true, ConfigSource(ConfigSource::Type::Default),
               "Enable Hebbian learning");
    
    pImpl->set("plasticity.structural.enable", true, ConfigSource(ConfigSource::Type::Default),
               "Enable structural plasticity (neuron/synapse creation)");
    pImpl->set("plasticity.structural.synaptogenesis_rate", 0.0001f, ConfigSource(ConfigSource::Type::Default),
               "Rate of new synapse formation");
    pImpl->set("plasticity.structural.pruning_rate", 0.00001f, ConfigSource(ConfigSource::Type::Default),
               "Rate of synapse elimination");
    
    // Neuromodulation parameters
    pImpl->set("neuromod.dopamine.scale", 1.0f, ConfigSource(ConfigSource::Type::Default),
               "Dopamine modulation scale");
    
    pImpl->set("neuromod.curiosity.enable", true, ConfigSource(ConfigSource::Type::Default),
               "Enable curiosity-driven exploration");
    
    pImpl->set("neuromod.novelty.enable", true, ConfigSource(ConfigSource::Type::Default),
               "Enable novelty detection");
    
    // Simulation parameters
    pImpl->set("simulation.timestep", 0.001, ConfigSource(ConfigSource::Type::Default),
               "Simulation timestep (seconds)");
    
    // Memory parameters
    pImpl->set("memory.working.capacity", 100, ConfigSource(ConfigSource::Type::Default),
               "Working memory capacity (number of items)");
    
    pImpl->set("memory.episodic.max_episodes", 1000, ConfigSource(ConfigSource::Type::Default),
               "Maximum number of episodic memory episodes");
    
    // Development parameters
    pImpl->set("development.initial_plasticity", 1.0f, ConfigSource(ConfigSource::Type::Default),
               "Plasticity rate during initial stage");
    
    pImpl->set("development.critical_period_plasticity", 0.8f, ConfigSource(ConfigSource::Type::Default),
               "Plasticity rate during critical period");
    
    pImpl->set("development.maturation_plasticity", 0.5f, ConfigSource(ConfigSource::Type::Default),
               "Plasticity rate during maturation stage");
    
    pImpl->set("development.adult_plasticity", 0.2f, ConfigSource(ConfigSource::Type::Default),
               "Plasticity rate during adult stage");
    
    // Checkpoint parameters
    pImpl->set("checkpoint.directory", "./checkpoints", ConfigSource(ConfigSource::Type::Default),
               "Directory for checkpoint files");
    
    pImpl->set("checkpoint.max_checkpoints", 10000, ConfigSource(ConfigSource::Type::Default),
               "Maximum number of checkpoints to store");
    
    pImpl->set("checkpoint.auto_save_interval", 5000, ConfigSource(ConfigSource::Type::Default),
               "Auto-save interval (simulation steps)");
    
    pImpl->set("checkpoint.compress", true, ConfigSource(ConfigSource::Type::Default),
               "Enable checkpoint compression");
    
    // Random seed
    pImpl->set("random_seed", 42, ConfigSource(ConfigSource::Type::Default),
               "Random seed for reproducible simulations");
}

bool Config::loadFromFile(const std::string& filepath) {
    // Detect format from file extension
    std::string format = detectFormat(filepath);
    if (format.empty()) {
        std::cerr << "Unsupported file format for " << filepath << ". Try .json, .yaml, .yml, .toml, or .cfg" << std::endl;
        return false;
    }
    
    bool success = parseFile(filepath, format);
    if (success) {
        std::cout << "Loaded configuration from " << filepath << " (" << format << ")" << std::endl;
    }
    return success;
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
                set(key, value, ConfigSource(ConfigSource::Type::CommandLine), 
                    "Command line argument");
            } else {
                std::cerr << "Warning: Invalid argument format " << arg << ". Use --key=value" << std::endl;
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            if (i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource(ConfigSource::Type::CommandLine),
                    "Command line argument");
            } else {
                std::cerr << "Error: Missing value for argument " << arg << std::endl;
                success = false;
            }
        } else {
            std::cerr << "Warning: Ignoring argument " << arg << ". Use --key=value or -key value format" << std::endl;
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath, const std::string& format) {
    std::string detectedFormat = format.empty() ? detectFormat(filepath) : format;
    
    if (detectedFormat.empty()) {
        std::cerr << "Unsupported file format for " << filepath << ". Try .json, .yaml, .yml, .toml, or .cfg" << std::endl;
        return false;
    }
    
    bool success = writeToFile(filepath, detectedFormat);
    if (success) {
        std::cout << "Saved configuration to " << filepath << " (" << detectedFormat << ")" << std::endl;
    }
    return success;
}

void Config::set(const std::string& key, const std::string& value, ConfigSource::Type source) {
    pImpl->set(key, value, ConfigSource(source), "");
}

void Config::set(const std::string& key, int value, ConfigSource::Type source) {
    pImpl->set(key, value, ConfigSource(source), "");
}

void Config::set(const std::string& key, int64_t value, ConfigSource::Type source) {
    pImpl->set(key, value, ConfigSource(source), "");
}

void Config::set(const std::string& key, double value, ConfigSource::Type source) {
    pImpl->set(key, value, ConfigSource(source), "");
}

void Config::set(const std::string& key, bool value, ConfigSource::Type source) {
    pImpl->set(key, value, ConfigSource(source), "");
}

template<typename T>
void Config::set(const std::string& key, const T& value, ConfigSource::Type source) {
    // Convert vector types to string for simplicity
    if constexpr (std::is_same_v<T, std::vector<int>> ||
                  std::is_same_v<T, std::vector<double>> ||
                  std::is_same_v<T, std::vector<std::string>>) {
        // For vectors, convert to string representation
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < value.size(); ++i) {
            if (i) oss << ", ";
            oss << value[i];
        }
        oss << "]";
        pImpl->set(key, oss.str(), ConfigSource(source), "");
    } else {
        pImpl->set(key, value, ConfigSource(source), "");
    }
}

bool Config::has(const std::string& key) const {
    return pImpl->has(key);
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    const ConfigEntry* entry = pImpl->get(key);
    if (!entry) {
        return std::nullopt;
    }
    
    try {
        if constexpr (std::is_same_v<T, int>) {
            if (std::holds_alternative<int>(entry->value)) {
                return std::get<int>(entry->value);
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<int>(entry->value)) {
                return static_cast<int64_t>(std::get<int>(entry->value));
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(entry->value)) {
                return std::get<double>(entry->value);
            } else if (std::holds_alternative<int>(entry->value)) {
                return static_cast<double>(std::get<int>(entry->value));
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(entry->value)) {
                return std::get<bool>(entry->value);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(entry->value)) {
                return std::get<std::string>(entry->value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (std::holds_alternative<std::vector<int>>(entry->value)) {
                return std::get<std::vector<int>>(entry->value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (std::holds_alternative<std::vector<double>>(entry->value)) {
                return std::get<std::vector<double>>(entry->value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (std::holds_alternative<std::vector<std::string>>(entry->value)) {
                return std::get<std::vector<std::string>>(entry->value);
            }
        }
        
        return std::nullopt;
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    return value.has_value() ? value.value() : defaultValue;
}

void Config::remove(const std::string& key) {
    pImpl->remove(key);
}

void Config::clear() {
    pImpl->clear();
}

std::vector<std::string> Config::getKeys() const {
    return pImpl->getKeys();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = ";
        oss << pImpl->getStringValue(entry.value);
        oss << " (" << static_cast<int>(entry.source.getType()) << ")\n";
        if (!entry.description.empty()) {
            oss << "    // " << entry.description << "\n";
        }
    }
    return oss.str();
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    const ConfigEntry* entry = pImpl->get(key);
    if (!entry) {
        return defaultValue;
    }
    
    if (std::holds_alternative<std::string>(entry->value)) {
        return std::get<std::string>(entry->value);
    }
    return pImpl->getStringValue(entry->value);
}

int Config::getInt(const std::string& key, int defaultValue) const {
    auto value = get<int>(key);
    return value.has_value() ? value.value() : defaultValue;
}

double Config::getDouble(const std::string& key, double defaultValue) const {
    auto value = get<double>(key);
    return value.has_value() ? value.value() : defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue) const {
    auto value = get<bool>(key);
    return value.has_value() ? value.value() : defaultValue;
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

std::string Config::detectFormat(const std::string& filepath) {
    if (filepath.empty()) return "";
    
    std::string ext = std::filesystem::path(filepath).extension().string();
    if (!ext.empty() && ext[0] == '.') {
        ext = ext.substr(1);
    }
    
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return "json";
    } else if (ext == "yaml" || ext == "yml") {
        return "yaml";
    } else if (ext == "toml") {
        return "toml";
    } else if (ext == "cfg" || ext == "ini" || ext == "properties") {
        return "key=value";
    }
    
    return "";
}

bool Config::parseFile(const std::string& filepath, const std::string& format) {
    if (format == "json") {
        return parseJSON(filepath);
    } else if (format == "yaml") {
        return parseYAML(filepath);
    } else if (format == "toml") {
        return parseTOML(filepath);
    } else if (format == "key=value") {
        return parseKeyValue(filepath);
    }
    
    std::cerr << "Unsupported format: " << format << std::endl;
    return false;
}

bool Config::writeToFile(const std::string& filepath, const std::string& format) {
    if (format == "json") {
        return writeJSON(filepath);
    } else if (format == "yaml") {
        return writeYAML(filepath);
    } else if (format == "toml") {
        return writeTOML(filepath);
    } else if (format == "key=value") {
        return writeKeyValue(filepath);
    }
    
    std::cerr << "Unsupported format: " << format << std::endl;
    return false;
}

bool Config::parseJSON(const std::string& filepath) {
    // Simple JSON parser implementation
    // In a real implementation, you would use nlohmann/json
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, fallback to key=value parsing
    return parseKeyValue(filepath);
}

bool Config::parseYAML(const std::string& filepath) {
    // Simple YAML parser implementation
    // In a real implementation, you would use YAML-cpp
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, fallback to key=value parsing
    return parseKeyValue(filepath);
}

bool Config::parseTOML(const std::string& filepath) {
    // Simple TOML parser implementation
    // In a real implementation, you would use toml11 or similar
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, fallback to key=value parsing
    return parseKeyValue(filepath);
}

bool Config::parseKeyValue(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Cannot open configuration file: " << filepath << std::endl;
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Skip TOML comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
            line = trim(line);
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            std::cerr << "Warning: Invalid line " << lineNumber << " in " << filepath << ": missing '='" << std::endl;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Determine type
        bool isNumeric = false;
        bool isBoolean = false;
        double numericValue = 0.0;
        
        // Check if it's a number
        if (!value.empty() && (value[0] == '-' || value[0] == '+')) {
            if (value.size() > 1 && std::isdigit(value[1])) {
                isNumeric = true;
            }
        } else if (!value.empty() && std::isdigit(value[0])) {
            isNumeric = true;
        }
        
        // Check if it's a boolean
        if (value == "true" || value == "false" || value == "yes" || value == "no") {
            isBoolean = true;
        }
        
        if (isNumeric) {
            try {
                // Check if it's an integer
                size_t posEnd;
                int intValue = std::stoi(value, &posEnd);
                if (posEnd == value.size()) {
                    set(key, intValue, ConfigSource(ConfigSource::Type::File));
                    continue;
                }
                
                // Check if it's a float
                double floatValue = std::stod(value, &posEnd);
                if (posEnd == value.size()) {
                    set(key, floatValue, ConfigSource(ConfigSource::Type::File));
                    continue;
                }
            } catch (const std::exception&) {
                // Fall through to string handling
            }
        }
        
        if (isBoolean) {
            bool boolValue = (value == "true" || value == "yes");
            set(key, boolValue, ConfigSource(ConfigSource::Type::File));
            continue;
        }
        
        // Default to string
        set(key, value, ConfigSource(ConfigSource::Type::File));
    }
    
    return true;
}

bool Config::writeJSON(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        file << "  \"" << entry.key << "\": ";
        
        if (std::holds_alternative<int>(entry.value)) {
            file << std::get<int>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            file << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            file << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            file << "\"" << std::get<std::string>(entry.value) << "\"";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            const auto& vec = std::get<std::vector<int>>(entry.value);
            file << "[";
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j) file << ", ";
                file << vec[j];
            }
            file << "]";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            const auto& vec = std::get<std::vector<double>>(entry.value);
            file << "[";
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j) file << ", ";
                file << vec[j];
            }
            file << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            const auto& vec = std::get<std::vector<std::string>>(entry.value);
            file << "[";
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j) file << ", ";
                file << "\"" << vec[j] << "\"";
            }
            file << "]";
        }
        
        if (i < pImpl->entries.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "}\n";
    return true;
}

bool Config::writeYAML(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << entry.key << ": ";
        
        if (std::holds_alternative<int>(entry.value)) {
            file << std::get<int>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            file << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            file << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            file << "\"" << std::get<std::string>(entry.value) << "\"";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            const auto& vec = std::get<std::vector<int>>(entry.value);
            file << "[";
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j) file << ", ";
                file << vec[j];
            }
            file << "]";
        }
        
        file << "\n";
    }
    
    return true;
}

bool Config::writeTOML(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << entry.key << " = ";
        
        if (std::holds_alternative<int>(entry.value)) {
            file << std::get<int>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            file << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            file << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            file << "\"" << std::get<std::string>(entry.value) << "\"";
        }
        
        file << "\n";
    }
    
    return true;
}

bool Config::writeKeyValue(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << entry.key << " = ";
        
        if (std::holds_alternative<int>(entry.value)) {
            file << std::get<int>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            file << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            file << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            file << "\"" << std::get<std::string>(entry.value) << "\"";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            const auto& vec = std::get<std::vector<int>>(entry.value);
            file << "[";
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j) file << ", ";
                file << vec[j];
            }
            file << "]";
        }
        
        file << "\n";
    }
    
    return true;
}

} // namespace nlm