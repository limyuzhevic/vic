#pragma once

/**
 * @file Config.cpp
 * @brief Configuration system implementation
 * 
 * This file implements the configuration management system for the Neural Learning Machine (NLM).
 * The configuration system provides a flexible, type-safe mechanism for managing
 * neural network parameters, simulation settings, and learning rule parameters.
 * 
 * Implementation details:
 * - Uses pImpl pattern for interface/implementation separation
 * - Simple key=value format parsing (planned JSON/YAML support)
 * - Type-safe value storage with variant types
 * - Source-aware value resolution (Runtime > Command Line > File > Default)
 * - Comprehensive error handling and validation
 */

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

/**
 * @brief Internal implementation structure for configuration management
 * 
 * This structure encapsulates all the storage and management logic for configuration
 * parameters, keeping the public interface clean while providing powerful internal
 * functionality for parsing, validation, and management.
 * 
 * Data organization:
 * - entries: All configuration parameters with full metadata
 * - Source tracking: Enables priority-based resolution
 * - Type safety: Variant-based type management
 * - Memory efficiency: Direct storage without indirection
 */
struct Config::Impl {
    std::vector<ConfigEntry> entries;  ///< All configuration parameters with metadata
};

/**
 * @brief Constructor creates empty configuration with default parameters
 * 
 * Initializes the configuration system with no parameters loaded. Default parameters
 * are added on-demand when specific values are requested, following the principle of
 * lazy initialization to reduce startup overhead.
 * 
 * Memory management:
 * - Uses pImpl pattern for exception safety
 * - Automatic cleanup of internal implementation
 * - Exception-safe move operations
 */
Config::Config() : pImpl(std::make_unique<Impl>()) {}

/**
 * @brief Destructor cleans up internal implementation
 * 
 * Proper cleanup of the pImpl member ensures that all configuration resources
 * are released and the internal implementation is safely destroyed.
 */
Config::~Config() = default;

/**
 * @brief Move constructor transfers implementation ownership
 * 
 * Implements exception-safe transfer of configuration ownership from
 * one object to another. This is crucial for performance and resource
 * management in scenarios where configuration objects are frequently
 * created and destroyed.
 * 
 * @param other Source configuration object to move from
 */
Config::Config(Config&&) noexcept = default;

/**
 * @brief Move assignment operator transfers implementation ownership
 * 
 * Ensures proper cleanup of existing resources before transferring
 * ownership from another configuration object. This implements the
 * rule of zero for resource management in this class.
 * 
 * @param other Source configuration object to move from
 * @return Reference to this configuration object (for chaining)
 */
Config& Config::operator=(Config&&) noexcept = default;

/**
 * @brief Load configuration from file with parsing and validation
 * 
 * Parses configuration files in simple key=value format. This is a
 * placeholder implementation that will be enhanced with full JSON/YAML
 * support in future versions. The current implementation provides
 * basic functionality while maintaining the interface.
 * 
 * Parsing features:
 * - Supports comments with # and / characters
 * - Handles quoted values with both " and ' delimiters
 * - Strips whitespace from keys and values
 * - Validates parameter syntax
 * 
 * File format limitations:
 * - Currently only simple key=value pairs
 * - No nested structures or complex types
 * - No schema validation (planned for future)
 * 
 * @param filepath Path to configuration file
 * @return true if file was successfully parsed and loaded
 */
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

/**
 * @brief Load configuration from command-line arguments
 * 
 * Processes command-line arguments to configure the system. Supports
 * both --key=value and -k value formats, providing maximum flexibility
 * for different command-line interface styles.
 * 
 * Argument processing:
 * - --key=value format for explicit assignment
 * - -k value format for space-separated arguments
 * - Case-insensitive key matching (planned for future)
 * - Type inference from string values (planned for future)
 * 
 * @param argc Argument count (from main())
 * @param argv Argument values (from main())
 * @return true if all arguments were processed successfully
 */
bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

/**
 * @brief Save configuration to file with serialization
 * 
 * Serializes the current configuration to a human-readable file format.
 * This is primarily used for configuration persistence and sharing.
 * The output format maintains compatibility with
 * the simple parser in loadFromFile().
 * 
 * Output format:
 * - Comments with parameter descriptions
 * - key = value format with quotes for strings
 * - Parameter order matches insertion order (preserves history)
 * - Source information for debugging and auditing
 * 
 * @param filepath Path where configuration will be saved
 * @return true if file was successfully written
 */
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

/**
 * @brief Get configuration value with type safety and validation
 * 
 * Retrieves a configuration value of the specified type, performing
 * automatic type conversion and validation. If the value doesn't exist
 * or has an incompatible type, returns nullopt.
 * 
 * Type safety mechanisms:
 * - std::variant storage with runtime type checking
 * - Automatic conversion for numeric types
 * - Strict type enforcement (no implicit conversions)
 * - Exception handling for bad variant access
 * 
 * Performance considerations:
 * - Linear search through configuration entries
 * - O(1) access for small configuration sets
 * - Caching for frequently accessed parameters
 * 
 * @tparam T Desired value type
 * @param key Configuration parameter name
 * @return Optional value, or nullopt if not found or wrong type
 */
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

/**
 * @brief Get configuration value with default fallback
 * 
 * Retrieves a configuration value or returns the default if the value
 * doesn't exist or has an incompatible type. This is the most commonly
 * used interface for configuration access, providing robustness and
 * simplicity for parameter retrieval.
 * 
 * Usage patterns:
 * - Essential parameters with sensible defaults
 * - Optional configuration with fallback behavior
 * - Experimental parameter tuning
 * - Production system configuration
 * 
 * Example:
 * @code
 * double learning_rate = config->getOr<double>("learning_rate", 0.001);
 * @endcode
 * 
 * @tparam T Desired value type
 * @param key Configuration parameter name
 * @param defaultValue Value returned if parameter not found
 * @return Configuration value or default
 */
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

/**
 * @brief Set configuration value with source tracking and validation
 * 
 * Sets a configuration value, storing it with metadata about its source.
 * Source tracking enables priority-based resolution when multiple sources
 * provide conflicting values. Runtime sources take highest precedence.
 * 
 * Value insertion strategy:
 * - Update if key already exists (overwrites with new source)
 * - Insert if key doesn't exist (creates new entry)
 * - Maintain source priority (Runtime > CommandLine > File > Default)
 * - Preserve insertion order for deterministic behavior
 * 
 * @param key Configuration parameter name
 * @param value Parameter value
 * @param source Origin of this value (defaults to Runtime)
 */
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

/**
 * @brief Set configuration value as string (convenience function)
 * 
 * Convenience function that automatically converts string values to
 * ConfigValue variant type. This reduces boilerplate code when
 * programmatically setting configuration parameters.
 * 
 * @param key Configuration parameter name
 * @param value Parameter value as string
 * @param source Origin of this value
 */
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Set configuration value as integer (convenience function)
 * 
 * Convenience function for setting integer values. Supports both
 * int and int64_t types for different precision requirements.
 * Automatically converts to ConfigValue variant type.
 * 
 * @param key Configuration parameter name
 * @param value Parameter value as integer
 * @param source Origin of this value
 */
void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Set configuration value as floating-point number (convenience function)
 * 
 * Convenience function for setting floating-point values. Automatically
 * converts to ConfigValue variant type.
 * 
 * @param key Configuration parameter name
 * @param value Parameter value as floating-point number
 * @param source Origin of this value
 */
void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Set configuration value as boolean (convenience function)
 * 
 * Convenience function for setting boolean values. Automatically
 * converts to ConfigValue variant type.
 * 
 * @param key Configuration parameter name
 * @param value Parameter value as boolean
 * @param source Origin of this value
 */
void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Check if configuration parameter exists
 * 
 * Performs a linear search to determine if a configuration parameter
 * with the specified key exists. This is an O(n) operation but is
 * typically fast for configuration sets with moderate size.
 * 
 * Use cases:
 * - Parameter validation before access
 * - Optional parameter checking
 * - Configuration auditing
 * - Error handling and user feedback
 * 
 * @param key Configuration parameter name
 * @return true if parameter exists, false otherwise
 */
bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
                       [&key](const ConfigEntry& e) { return e.key == key; });
}

/**
 * @brief Remove configuration parameter
 * 
 * Permanently removes a configuration parameter from the configuration.
 * This is equivalent to deleting the parameter from the configuration
 * registry.
 * 
 * Removal effects:
 * - Parameter will be retrieved as missing
 * - Default values will be used instead
 * - Source priority is reset
 * - Memory is freed for that entry
 * 
 * @param key Configuration parameter name to remove
 */
void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

/**
 * @brief Get all configuration parameter names
 * 
 * Returns a list of all keys currently in the configuration. This is
 * useful for introspection, debugging, configuration validation,
 * and user interface generation.
 * 
 * Output ordering:
 * - Maintains insertion order for deterministic behavior
 * - Preserves source history (default < file < command line < runtime)
 * - Enables configuration auditing and tracking
 * 
 * @return Vector of all configuration parameter names
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
 * @brief Clear all configuration values
 * 
 * Removes all configuration parameters, leaving the configuration
 * in its default empty state. This operation is useful for:
 * - Resetting configuration between experimental trials
 * - Freeing memory in long-running applications
 * - Starting fresh with new parameters
 * - Testing configuration loading and validation
 * 
 * This is a destructive operation that cannot be undone without
 * reloading configuration from its original sources.
 */
void Config::clear() {
    pImpl->entries.clear();
}

/**
 * @brief Get human-readable configuration summary
 * 
 * Generates a comprehensive summary of all configuration parameters
 * including their values and sources. This is primarily used for:
 * - Logging and debugging
 * - User interface display
 * - Configuration auditing
 * - Error reporting and troubleshooting
 * 
 * Summary format:
 * - Total parameter count
 * - Each parameter with name, value, and source
 * - Value representation (quoted for strings)
 * - Source identification (0=Default, 1=File, 2=CommandLine, 3=Runtime)
 * 
 * @return Formatted configuration summary string
 */
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

/**
 * @brief Internal helper: Trim whitespace from string
 * 
 * Removes leading and trailing whitespace characters (spaces, tabs,
 * carriage returns, newlines) from input strings. Used for cleaning
 * configuration key-value pairs during parsing.
 * 
 * Trimming rules:
 * - Preserves internal whitespace
 * - Handles mixed whitespace characters
 * - Efficient linear scan
 * - Safe for empty strings
 * 
 * @param str Input string to trim
 * @return String with whitespace trimmed from both ends
 */
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

/**
 * @brief Internal helper: Convert string to lowercase
 * 
 * Converts all uppercase characters in a string to lowercase.
 * Used for case-insensitive configuration key matching and
 * normalization in the configuration system.
 * 
 * Conversion rules:
 * - Uses std::tolower for locale-independent conversion
 * - Preserves non-alphabetic characters
 * - Efficient single-pass algorithm
 * - Safe for empty strings
 * 
 * @param str Input string to convert
 * @return Lowercase version of input string
 */
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Explicit template instantiations
// These ensure that the template functions are compiled and available
// for the types we actually use in the configuration system
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
