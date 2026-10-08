#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

/**
 * Implementation of the Config class for NLM (Neural Learning Machine).
 * 
 * The Config class provides a flexible key-value configuration system used throughout
 * the NLM brain simulation. It supports multiple value types (integers, strings, doubles,
 * booleans, and arrays) and can load configuration from files, command line arguments,
 * or runtime sets. Each configuration value has a source (file, command line, or runtime)
 * to track its origin for dependency management and override resolution.
 * 
 * Configuration is a central aspect of NLM that controls:
 * - Neural network parameters (neuron count, connectivity, etc.)
 * - Simulation settings (time steps, accuracy, etc.)
 * - Plasticity parameters (learning rates, thresholds, etc.)
 * - Neuromodulation settings (dopamine, curiosity, etc.)
 * - Memory system configuration
 * - Development and maturation parameters
 * 
 * The configuration system is designed to be:
 * - Hierarchical: values can be overridden by command line or runtime
 * - Type-safe: uses std::variant to store different value types
 * - Flexible: supports both simple key=value and complex array values
 * - Portable: uses simple text format for easy sharing and documentation
 * - Extensible: can be easily extended to support new value types
 * 
 * Common usage patterns:
 * 1. Load default configuration from file: config->loadFromFile("default.cfg")
 * 2. Override with command line arguments: config->loadFromArgs(argc, argv)
 * 3. Set runtime values: config->set("key", value, ConfigSource::Runtime)
 * 4. Get values: config->get<T>("key") or config->getOr<T>("key", default)
 * 
 * Example configuration file format:
 * # Neural network configuration
 * neuron_count = 1000
 * region_count = 2
 * connection_probability = 0.1
 * # Plasticity parameters
 * stdp_ltp_weight = 0.01
 * stdp_ltd_weight = 0.012
 * stdp_tau = 20.0
 * # Neuromodulation
 * dopamine_baseline = 0.1
 * curiosity_rate = 0.05
 */

/**
 * Implementation details using the Pimpl idiom to hide implementation
 * and reduce compilation dependencies.
 */
struct Config::Impl {
    std::vector<ConfigEntry> entries;  ///< List of configuration key-value pairs
};

/**
 * Default constructor creates an empty configuration.
 */
Config::Config() : pImpl(std::make_unique<Impl>()) {}

/**
 * Destructor cleans up implementation details.
 */
Config::~Config() = default;

/**
 * Move constructor - transfers ownership of implementation.
 */
Config::Config(Config&&) noexcept = default;

/**
 * Move assignment operator - transfers ownership and cleans up.
 */
Config& Config::operator=(Config&&) noexcept = default;

/**
 * Load configuration from a file.
 * 
 * Currently implements Phase 1 format: simple key=value pairs with comments.
 * Future phases will implement proper JSON/YAML support.
 * 
 * @param filepath Path to configuration file
 * @return true if file was successfully loaded, false otherwise
 * @note File format: each line is "key = value" or a comment starting with # or /
 * @note Values can be quoted: "value" or 'value'
 * @see loadFromArgs for command line loading
 */
bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
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
            NLM_LOG_DEBUG("Loaded from file: " + key + " = " + value);
        } else {
            NLM_LOG_WARNING("Invalid configuration line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from file: " + filepath);
    return true;
}

/**
 * Load configuration from command line arguments.
 * 
 * Supports two formats:
 * --key=value (long option)
 * -key value  (short option)
 * 
 * Command line values override file configuration and have the highest priority.
 * 
 * @param argc Argument count
 * @param argv Argument vector
 * @return true if arguments were successfully processed
 * @note Command line values have ConfigSource::CommandLine
 * @see loadFromFile for file loading
 */
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
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_INFO("Set from command line (--): " + key + " = " + value);
            } else {
                NLM_LOG_WARNING("Invalid command line argument: " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            NLM_LOG_INFO("Set from command line (-): " + key + " = " + value);
        }
        else {
            NLM_LOG_WARNING("Unrecognized command line argument: " + arg);
            success = false;
        }
    }
    
    return success;
}

/**
 * Save configuration to a file.
 * 
 * Currently saves in Phase 1 format with placeholder values.
 * Future phases will implement proper JSON/YAML output.
 * 
 * @param filepath Path where configuration should be saved
 * @return true if file was successfully saved, false otherwise
 * @note Only saves configuration entries with their descriptions
 * @note Format: "# description\nkey = value\n"
 */
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM configuration system\n";
    file << "# This is the default configuration for Phase 1 skeleton\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Format value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Saved configuration to file: " + filepath);
    return true;
}

/**
 * Get configuration value of specified type.
 * 
 * Retrieves the value associated with the given key, converting it to the
 * requested type T. If the key doesn't exist or has the wrong type, returns
 * std::nullopt.
 * 
 * @tparam T Type to convert value to (int, int64_t, double, bool, std::string, etc.)
 * @param key Configuration key to retrieve
 * @return Optional containing value if found and type matches, nullopt otherwise
 * @note Common types: get<int>("neuron_count"), get<double>("learning_rate")
 * @example auto count = config->get<int>("neuron_count");
 * @see getOr for providing default values
 */
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Configuration key not found: " + key);
        return std::nullopt;
    }
    
    try {
        T value = std::get<T>(it->value);
        NLM_LOG_DEBUG("Retrieved configuration key: " + key + " = " + std::to_string(value));
        return value;
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARNING("Type mismatch for configuration key: " + key + 
                       " (expected " + typeid(T).name() + ")");
        return std::nullopt;
    }
}

/**
 * Get configuration value with default fallback.
 * 
 * Convenience method that returns the value if it exists and has the correct type,
 * or returns the provided default value if not found or type mismatch.
 * 
 * @tparam T Type to convert value to
 * @param key Configuration key to retrieve
 * @param defaultValue Value to return if key doesn't exist or type mismatch
 * @return Value from config or default if not found
 * @note Safer than get() for critical configuration parameters
 * @example auto count = config->getOr<size_t>("neuron_count", 1000);
 */
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    } else {
        NLM_LOG_DEBUG("Using default configuration value for key: " + key + 
                     " (default = " + std::to_string(defaultValue) + ")");
        return defaultValue;
    }
}

/**
 * Set configuration value.
 * 
 * Creates or updates a configuration entry with the given key, value, and source.
 * If the key already exists, both value and source are updated. If not, a new
 * entry is created.
 * 
 * @param key Configuration key
 * @param value Configuration value
 * @param source Origin of the configuration value
 * @note ConfigSource determines override priority: File < CommandLine < Runtime
 * @see ConfigSource enum for valid sources
 */
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_INFO("Updated configuration key: " + key + " (source: " + 
                    std::to_string(static_cast<int>(source)) + ")");
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_INFO("Added configuration key: " + key + " (source: " + 
                    std::to_string(static_cast<int>(source)) + ")");
    }
}

/**
 * Set configuration value with type conversion.
 * 
 * Convenience overload that converts string value to specified type T.
 * Used for command line argument parsing where all values start as strings.
 * 
 * @tparam T Type to convert string to
 * @param key Configuration key
 * @param value String value to convert
 * @param source Configuration source
 * @see set(const std::string&, const ConfigValue&, ConfigSource) for generic version
 */
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * Set integer configuration value.
 * 
 * @param key Configuration key
 * @param value Integer value
 * @param source Configuration source
 */
void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * Set double configuration value.
 * 
 * @param key Configuration key
 * @param value Double value
 * @param source Configuration source
 */
void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * Set boolean configuration value.
 * 
 * @param key Configuration key
 * @param value Boolean value
 * @param source Configuration source
 */
void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * Check if configuration key exists.
 * 
 * @param key Configuration key to check
 * @return true if key exists in configuration, false otherwise
 * @see remove for deleting keys
 * @see getKeys for listing all keys
 */
bool Config::has(const std::string& key) const {
    bool exists = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (!exists) {
        NLM_LOG_DEBUG("Configuration key not found: " + key);
    }
    
    return exists;
}

/**
 * Remove configuration key.
 * 
 * Deletes the configuration entry with the given key if it exists.
 * 
 * @param key Configuration key to remove
 * @note Safe to call even if key doesn't exist
 */
void Config::remove(const std::string& key) {
    auto originalSize = pImpl->entries.size();
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    if (pImpl->entries.size() < originalSize) {
        NLM_LOG_INFO("Removed configuration key: " + key);
    }
}

/**
 * Get all configuration keys.
 * 
 * Returns a vector containing all keys currently in the configuration.
 * 
 * @return Vector of all configuration keys
 * @note Order is not guaranteed
 * @see has for checking individual keys
 * @see remove for deleting keys
 */
std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

/**
 * Clear all configuration entries.
 * 
 * Removes all key-value pairs from the configuration, leaving it empty.
 * Useful for resetting configuration or starting fresh.
 * 
 * @note This only affects the current configuration object
 * @see set for adding entries
 */
void Config::clear() {
    auto originalSize = pImpl->entries.size();
    pImpl->entries.clear();
    
    if (originalSize > 0) {
        NLM_LOG_INFO("Cleared " + std::to_string(originalSize) + " configuration entries");
    }
}

/**
 * Generate human-readable configuration summary.
 * 
 * Creates a formatted string showing all configuration entries with their
 * values and sources. Useful for logging and debugging.
 * 
 * @return Formatted configuration summary string
 * @note Shows values in readable format with source indicators
 * @example std::cout << config->summary() << std::endl;
 */
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "=============================\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "Key:      " << entry.key << "\n";
        oss << "Source:   " << static_cast<int>(entry.source) << " (0=Default, 1=File, 2=CommandLine, 3=Runtime)\n";
        oss << "Description: " << (entry.description.empty() ? "(no description)" : entry.description) << "\n";
        oss << "Value:    ";
        
        // Format value based on type
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        
        oss << "\n" << std::string(50, '-') << "\n";
    }
    
    return oss.str();
}

/**
 * Trim whitespace from string.
 * 
 * Removes leading and trailing whitespace characters (spaces, tabs, newlines).
 * Used for parsing configuration files.
 * 
 * @param str Input string to trim
 * @return Trimmed string
 * @note Used internally by loadFromFile
 */
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

/**
 * Convert string to lowercase.
 * 
 * Case-insensitive string processing helper used for configuration key matching.
 * 
 * @param str Input string
 * @return Lowercase version of input string
 * @note Used for case-insensitive configuration key lookups
 */
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Explicit template instantiations for common types
// These ensure the template methods are compiled and available

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

} // namespace nlm

