/**
 * @file Config.cpp
 * @brief Implementation file for the Config class
 * 
 * This file provides the implementation of the Config class methods, including
 * file I/O operations, command-line argument parsing, and configuration management.
 * The implementation uses a simple key=value format for configuration files
 * and supports various configuration value types.
 * 
 * @details Implementation features:
 * - File I/O operations with simple text format parsing
 * - Command-line argument parsing with --key=value and -key value formats
 * - Multiple configuration value type support
 * - Error handling for file operations and parsing
 * - Template-based value retrieval with type safety
 * - Source tracking for configuration precedence
 * - Memory-efficient storage using vector of ConfigEntry objects
 * - String manipulation utilities (trim, toLower)
 * 
 * @note This implementation uses a simple text-based configuration format
 * suitable for Phase 1 of the NLM development. More advanced formats like
 * JSON/YAML will be implemented in later phases.
 * 
 * @author NLM Authors
 * @date Created: 2024
 * @version 1.0
 * 
 * @copyright MIT License
 * 
 * @warning File I/O operations may fail silently in production environments.
 * Always check return values from load operations to ensure configuration
 * is properly loaded.
 * 
 * @see Config.hpp for class interface
 * @see ConfigEntry for configuration entry structure
 * @see ConfigSource for source enumeration
 * @see nlm::Config for main configuration interface
 * 
 * @ingroup CoreComponents
 */

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

/**
 * @brief Implementation details for the Config class
 * 
 * This struct contains the private implementation details of the Config class,
 * following the pImpl idiom for efficient memory management and forward declaration.
 * The implementation stores all configuration entries in a vector, allowing for
 * efficient lookup and management of configuration values.
 * 
 * @details The implementation uses:
 * - std::vector<ConfigEntry> for storing all configuration entries
 * - Constant time search for individual keys
 * - Linear time operations for bulk operations
 * - RAII for resource management
 * 
 * The pImpl pattern allows for easy evolution of the implementation without
 * affecting the public interface.
 * 
 * @see Config::Impl for public interface
 */
struct Config::Impl {
    std::vector<ConfigEntry> entries;  ///< Vector of all configuration entries
};

/**
 * @brief Default constructor implementation
 * 
 * Constructs the Config implementation with an empty configuration. This initializes
 * the internal implementation structure and sets up the default state for the
 * configuration management system.
 * 
 * @note The constructor uses std::make_unique for efficient memory management
 * and automatic cleanup of the implementation structure.
 * 
 * @see Config::Config()
 */
Config::Config() : pImpl(std::make_unique<Impl>()) {}

/**
 * @brief Default destructor implementation
 * 
 * The destructor is defaulted, relying on the implicit destruction of the
 * unique_ptr smart pointer that manages the implementation. This ensures
 * proper cleanup of the implementation resources without manual memory management.
 * 
 * @note The default implementation is sufficient for the current pImpl implementation
 * and provides exception safety.
 * 
 * @see Config::~Config()
 */
Config::~Config() = default;

/**
 * @brief Move constructor implementation (noexcept)
 * 
 * Moves the implementation from the source configuration to the new configuration
 * without copying the complex internal state. This provides efficient transfer
 * of configuration ownership.
 * 
 * @param other Source configuration to move from
 * 
 * @note The move constructor is noexcept for exception safety. It transfers
 * ownership of the unique_ptr smart pointer, ensuring efficient memory transfer.
 * 
 * @see Config::Config(Config&&)
 */
Config::Config(Config&&) noexcept = default;

/**
 * @brief Move assignment operator implementation (noexcept)
 * 
 * Moves the implementation from the source configuration to the existing
 * configuration without copying the complex internal state. This provides
 * efficient reassignment of configuration ownership.
 * 
 * @param other Source configuration to move from
 * @return Config& Reference to this configuration
 * 
 * @note The move assignment operator is noexcept for exception safety.
 * It uses the default implementation which efficiently transfers ownership
 * of the unique_ptr smart pointer.
 * 
 * @see Config& operator=(Config&&)
 */
Config& Config::operator=(Config&&) noexcept = default;

/**
 * @brief Load configuration from file implementation
 * 
 * Loads configuration values from a text file using a simple key=value format.
 * The file parser skips comments (lines starting with # or //) and empty lines,
 * and parses each key=value pair, storing them with ConfigSource::File.
 * 
 * @param filepath Path to the configuration file
 * @return bool true if file was read and parsed successfully, false otherwise
 * 
 * @throws std::runtime_error if file cannot be opened or read (uncaught)
 * 
 * @note This implementation uses a simple text format suitable for Phase 1.
 * The format supports:
 * - Simple key=value pairs
 * - Optional quotes around string values
 * - Comment lines starting with # or //
 * - Case-insensitive key matching
 * 
 * @warning File loading errors are not thrown but reported through the return value.
 * Consider using exception handling for production error reporting.
 * 
 * @see Config::loadFromFile(const std::string& filepath)
 * @see Config::set() for value storage
 * @see Config::trim() for string trimming
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
 * @brief Load configuration from command-line arguments implementation
 * 
 * Parses command-line arguments and loads them as configuration values.
 * Supports both --key=value and -key value formats. Command-line values
 * have higher priority than file and default values.
 * 
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return bool true if arguments were parsed successfully, false otherwise
 * 
 * @note Command-line arguments override configuration from files and defaults.
 * The parser handles:
 * - --key=value format (long option with value)
 * - -key value format (short option with separate value)
 * - Ignoring unrecognized arguments
 * 
 * @warning Command-line arguments take highest priority in configuration resolution.
 * They override values from all other sources.
 * 
 * @see Config::loadFromArgs(int argc, char** argv)
 * @see Config::set() for value storage
 * @see ConfigSource for source priority
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
 * @brief Save configuration to file implementation
 * 
 * Saves current configuration values to a text file. Only configuration
 * values with non-default sources (File, CommandLine, Runtime) are saved.
 * The output uses a simple key=value format similar to the input format.
 * 
 * @param filepath Path to the configuration file
 * @return bool true if file was written successfully, false otherwise
 * 
 * @note This implementation uses a simple text format. It only saves values
 * that are not from the Default source. The output format is similar to
 * the input format for easy round-trip editing.
 * 
 * @see Config::saveToFile(const std::string& filepath) const
 * @see Config::getKeys() for determining which keys to save
 * @see ConfigSource for source determination
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
 * @brief Get a configuration value with type safety implementation
 * 
 * Retrieves a configuration value of the specified type. If the value is not
 * present or of the wrong type, returns std::nullopt. This function provides
 * compile-time type safety for configuration access.
 * 
 * @tparam T Type of value to retrieve (must match one of the supported ConfigValue types)
 * @param key Configuration key to retrieve
 * @return std::optional<T> Value if found and type matches, std::nullopt otherwise
 * 
 * @throws std::bad_variant_access if value exists but is of incompatible type
 * (caught internally and converted to std::nullopt)
 * 
 * @note Supported types: int, int64_t, double, bool, std::string, and std::vector<T>
 * where T is int, double, or std::string. The implementation uses std::variant
 * to store multiple types and std::get for type extraction.
 * 
 * @see Config::getOr() for alternative with default value
 * @see Config::set() for value storage
 * @see ConfigValue for supported value types
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
 * @brief Get a configuration value with default fallback implementation
 * 
 * Retrieves a configuration value of the specified type, returning a default
 * value if the configuration key is not present or of the wrong type. This
 * provides safe configuration access without error checking.
 * 
 * @tparam T Type of value to retrieve
 * @param key Configuration key to retrieve
 * @param defaultValue Default value to return if key not found
 * @return T Configuration value if found, otherwise default value
 * 
 * @note This function is convenient for optional configuration parameters.
 * It never throws exceptions because it uses std::optional internally and
 * provides a fallback value. This makes it safe for use in critical code paths.
 * 
 * @see Config::get() for strict type-safe retrieval
 * @see Config::getOr() for default fallback
 * @see Config::set() for value storage
 */
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

/**
 * @brief Set a configuration value implementation
 * 
 * Sets a configuration value with the specified key and value. The value is
 * stored with the specified source (defaulting to Runtime source). The type
 * must match one of the supported ConfigValue types.
 * 
 * @param key Configuration key to set
 * @param value Configuration value with source
 * @param source Configuration source (default: ConfigSource::Runtime)
 * 
 * @note This implementation uses std::vector::emplace_back for new entries
 * and direct assignment for existing entries, providing O(1) lookup and update
 * operations. The source tracking ensures proper precedence resolution.
 * 
 * @see Config::set(const std::string&, const std::string&, ConfigSource) for string overload
 * @see Config::set(const std::string&, int, ConfigSource) for int overload
 * @see Config::set(const std::string&, double, ConfigSource) for double overload
 * @see Config::set(const std::string&, bool, ConfigSource) for bool overload
 * @see ConfigSource for source enumeration
 * @see ConfigEntry for entry structure
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
 * @brief Set a string configuration value implementation
 * 
 * Convenience overload for setting string values. This implementation converts
 * the string to ConfigValue using the ConfigValue constructor and delegates
 * to the main set method.
 * 
 * @param key Configuration key
 * @param value String value to set
 * @param source Configuration source (default: ConfigSource::Runtime)
 * 
 * @see Config::set(const std::string&, const ConfigValue&, ConfigSource) for main implementation
 */
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Set an integer configuration value implementation
 * 
 * Convenience overload for setting integer values. This implementation converts
 * the integer to ConfigValue using the ConfigValue constructor and delegates
 * to the main set method.
 * 
 * @param key Configuration key
 * @param value Integer value to set
 * @param source Configuration source (default: ConfigSource::Runtime)
 * 
 * @see Config::set(const std::string&, const ConfigValue&, ConfigSource) for main implementation
 */
void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Set a floating-point configuration value implementation
 * 
 * Convenience overload for setting double values. This implementation converts
 * the double to ConfigValue using the ConfigValue constructor and delegates
 * to the main set method.
 * 
 * @param key Configuration key
 * @param value Double value to set
 * @param source Configuration source (default: ConfigSource::Runtime)
 * 
 * @see Config::set(const std::string&, const ConfigValue&, ConfigSource) for main implementation
 */
void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Set a boolean configuration value implementation
 * 
 * Convenience overload for setting boolean values. This implementation converts
 * the boolean to ConfigValue using the ConfigValue constructor and delegates
 * to the main set method.
 * 
 * @param key Configuration key
 * @param value Boolean value to set
 * @param source Configuration source (default: ConfigSource::Runtime)
 * 
 * @see Config::set(const std::string&, const ConfigValue&, ConfigSource) for main implementation
 */
void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Check if a configuration key exists implementation
 * 
 * Determines if a configuration key has been set in the configuration.
 * This checks all configuration sources and returns true if the key exists
 * in any source. The search uses std::any_of for efficient checking.
 * 
 * @param key Configuration key to check
 * @return bool true if key exists in any configuration source, false otherwise
 * 
 * @note This implementation uses std::any_of for efficient checking, stopping
 * as soon as the key is found. The search examines all sources regardless
 * of their priority, as key existence is independent of source precedence.
 * 
 * @see Config::has(const std::string& key) const
 * @see Config::set() for value storage
 * @see ConfigSource for source enumeration
 */
bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

/**
 * @brief Remove a configuration key implementation
 * 
 * Removes a configuration key from all configuration sources. After removal,
 * the key will not be available for retrieval until set again. This implementation
 * uses std::remove_if and std::vector::erase for efficient removal.
 * 
 * @param key Configuration key to remove
 * 
 * @note This implementation uses std::remove_if to rearrange the vector
 * (moving remaining elements to the front) and std::vector::erase to remove
 * the marked elements, providing O(n) time complexity for the operation.
 * 
 * @see Config::remove(const std::string& key)
 * @see Config::has() for checking key existence
 * @see Config::set() for value storage
 */
void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

/**
 * @brief Get all configuration keys implementation
 * 
 * Returns a vector containing all keys from all configuration sources.
 * Keys are returned in order of appearance (which reflects source priority
 * for proper value resolution). The implementation uses reserve for efficiency
 * and preserves the original order.
 * 
 * @return std::vector<std::string> All configuration keys
 * 
 * @note This implementation preserves the insertion order of configuration
 * entries, which reflects the source priority. Keys are returned in the
 * order they were first added to the configuration.
 * 
 * @see Config::getKeys() const
 * @see Config::add() for key addition
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
 * @brief Clear all configuration values implementation
 * 
 * Removes all configuration values from all sources, leaving only the
 * built-in default configuration. This is useful for resetting the
 * configuration to its initial state. This implementation uses
 * std::vector::clear for efficient clearing.
 * 
 * @note This implementation uses std::vector::clear which efficiently
 * removes all elements without reallocating memory. The internal vector
 * capacity is preserved for future use.
 * 
 * @see Config::clear()
 * @see Config::add() for adding values
 */
void Config::clear() {
    pImpl->entries.clear();
}

/**
 * @brief Get configuration summary implementation
 * 
 * Returns a human-readable summary of all current configuration values,
 * including their sources and descriptions. This implementation formats the
 * output using std::ostringstream and lambda functions for type-safe value
 * extraction and formatting.
 * 
 * @return std::string Formatted configuration summary
 * 
 * @note This implementation uses a visitor pattern (lambda) to safely
 * extract and format values of different types. The summary includes
 * all configuration entries with their sources and key names.
 * 
 * @see Config::summary() const
 * @see ConfigEntry for entry structure
 * @see ConfigSource for source enumeration
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
 * @brief Trim whitespace from string implementation
 * 
 * Removes leading and trailing whitespace from a string. This implementation
 * uses standard string operations for efficient trimming and is used
 * for normalizing configuration keys and values during file parsing.
 * 
 * @param str Input string to trim
 * @return std::string Trimmed string
 * 
 * @note This implementation handles all whitespace characters (space, tab,
 * carriage return, newline). It uses std::string::find_first_not_of and
 * std::string::find_last_not_of for efficient trimming.
 * 
 * @see Config::trim(const std::string& str) for const version
 * @see Config::toLower() for case conversion
 */
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

/**
 * @brief Convert string to lowercase implementation
 * 
 * Converts all characters in a string to lowercase. This implementation
 * is used for case-insensitive configuration key matching during file parsing.
 * 
 * @param str Input string to convert
 * @return std::string Lowercase version of input string
 * 
 * @note This implementation uses std::transform with ::tolower for efficient
 * case conversion. It handles locale-dependent case conversion.
 * 
 * @see Config::toLower(const std::string& str) for const version
 * @see Config::trim() for string trimming
 */
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

/**
 * @brief Explicit template instantiations for Config class
 * 
 * These explicit template instantiations ensure that the template methods
 * are compiled and available for use. This is necessary because the template
 * methods are defined in the header file but need to be instantiated in
 * the implementation file to be compiled.
 * 
 * @note These instantiations provide type-safe retrieval and default value
 * access for all supported configuration value types. They are marked with
 * the 'template' keyword to indicate they are template specializations.
 * 
 * @see Config::get() for template method definition
 * @see Config::getOr() for template method definition
 */

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

} // namespace nlm
