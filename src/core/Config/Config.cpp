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

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

/**
 * @class Config::Impl
 * @brief Private implementation details for the Config class
 * 
 * Encapsulates the internal storage of configuration entries and provides
 * the pImpl pattern to hide implementation details from the header file.
 * This allows for forward declarations and reduces compilation dependencies.
 */
struct Config::Impl {
    std::vector<ConfigEntry> entries;  /**< Vector storing all configuration entries */
};

/**
 * @brief Constructs a new Config object with empty configuration
 * 
 * Initializes the private implementation with an empty entries vector.
 * The config starts with default configuration (none loaded).
 * 
 * @note Uses std::make_unique for automatic memory management and exception safety.
 *       Follows the pImpl pattern for encapsulation and forward declarations.
 */
Config::Config() : pImpl(std::make_unique<Impl>()) {}

/**
 * @brief Destroys the Config object and cleans up resources
 * 
 * Automatically called when Config object goes out of scope.
 * The unique_ptr pImpl ensures proper cleanup of the implementation.
 */
Config::~Config() = default;

/**
 * @brief Move constructor - transfers ownership of the Config object
 * 
 * Enables efficient transfer of configuration data without copying.
 * The source object's pImpl is reset to nullptr to prevent double deletion.
 * 
 * @param other The Config object to move from
 */
Config::Config(Config&&) noexcept = default;

/**
 * @brief Move assignment operator - transfers ownership of the Config object
 * 
 * Enables efficient assignment by transferring ownership of the internal
 * configuration data without copying. Follows the Rule of Five.
 * 
 * @param other The Config object to move from
 * @return Config& Reference to this Config object for method chaining
 * 
 * @note Self-assignment is handled (returns *this), safe and efficient.
 */
Config& Config::operator=(Config&&) noexcept = default;

/**
 * @brief Loads configuration from a file
 * 
 * Parses configuration files using a simple key=value format (Phase 1 implementation).
 * Future phases will implement full JSON/YAML parsing via the TODO comment.
 * 
 * File format:
 *   # Comment lines start with # or /
 *   key = value
 *   "quoted value" can be used for strings
 *   Empty lines are skipped
 * 
 * @param filepath Path to the configuration file to load
 * 
 * @return bool true if file was successfully read and parsed, false on error
 * 
 * @note This is a placeholder implementation. Phase 2 will add proper
 *       JSON/YAML support with type validation and schema checking.
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
 * @brief Loads configuration from command line arguments
 * 
 * Parses standard command-line configuration formats:
 * --key=value or -key value
 * 
 * Command-line arguments take highest priority and override file-based
 * configuration. Supports common CLI conventions for flexibility.
 * 
 * @param argc Standard C-style argc parameter from main()
 * @param argv Standard C-style argv parameter from main()
 * 
 * @return bool true if arguments were successfully parsed (always returns true
 *              unless there's an actual parsing error, which is unlikely with
 *              the simple parsing algorithm).
 * 
 * @note Arguments are processed in order, with later arguments overriding
 *       earlier ones for the same keys. This allows for flexible configuration
 *       overrides.
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
 * @brief Saves configuration to a file
 * 
 * Writes the current configuration to a file in the simple key=value format.
 * Note: This is a simplified implementation that writes placeholder values
 * instead of the actual stored values. Phase 2 will implement proper serialization.
 * 
 * @param filepath Path to the configuration file to save
 * 
 * @return bool true if file was successfully written, false on error
 * 
 * @note Currently writes placeholder values ("PLACEHOLDER_VALUE") instead of
 *       actual configuration values. This is a Phase 1 limitation. Phase 2
 *       will implement proper serialization of ConfigValue variants.
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
 * @brief Retrieves a configuration value with type checking
 * 
 * Extracts and returns the configuration value for a given key, performing
 * type-safe retrieval with automatic type checking via std::variant.
 * 
 * @tparam T Target type to extract (must match stored type)
 * @param key Configuration key to look up
 * 
 * @return std::optional<T> The configuration value if found and type matches,
 *         std::nullopt otherwise (key not found or type mismatch)
 * 
 * @note Uses std::any_of with lambda to find the configuration entry.
 *       If found, attempts to extract the type using std::get<T>.
 *       Catches std::bad_variant_access for type mismatches and returns
 *       std::nullopt.
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
 * @brief Retrieves a configuration value with a default fallback
 * 
 * Attempts to get a configuration value, returning the provided default
 * if the key is not found or the value cannot be cast to the requested type.
 * This is a convenience method for safe configuration access.
 * 
 * @tparam T Target type to retrieve
 * @param key Configuration key to look up
 * @param defaultValue Value to return if key not found or type mismatch
 * 
 * @return T The configuration value if found and type matches, otherwise
 *         the default value
 * 
 * @note This method is preferred over get() when you want to handle
 *       missing configuration values gracefully without throwing exceptions.
 *       The default value is returned as-is without type checking.
 */
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

/**
 * @brief Sets a configuration value with explicit source tracking
 * 
 * Internal implementation method that allows setting configuration values
 * with source tracking (File, CommandLine, Runtime, etc.). This is the
 * canonical setter method called by the public set() overloads.
 * 
 * @param key Configuration key to set
 * @param value Configuration value to store
 * @param source Source of this configuration (indicates priority)
 * 
 * @return void
 * 
 * @note If the key already exists, updates both value and source.
 *       If the key doesn't exist, creates a new entry.
 *       ConfigSource::Runtime is the default for programmatic access.
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
 * @brief Sets a configuration value as a string
 * 
 * Convenience wrapper for setting string values with source tracking.
 * Converts the string to ConfigValue and stores it.
 * 
 * @param key Configuration key to set
 * @param value String value to store
 * @param source Source of this configuration (default: Runtime)
 * 
 * @return void
 */
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Sets a configuration value as an integer
 * 
 * Convenience wrapper for setting integer values with source tracking.
 * Converts the integer to ConfigValue and stores it.
 * 
 * @param key Configuration key to set
 * @param value Integer value to store
 * @param source Source of this configuration (default: Runtime)
 * 
 * @return void
 */
void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Sets a configuration value as a double
 * 
 * Convenience wrapper for setting double values with source tracking.
 * Converts the double to ConfigValue and stores it.
 * 
 * @param key Configuration key to set
 * @param value Double value to store
 * @param source Source of this configuration (default: Runtime)
 * 
 * @return void
 */
void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Sets a configuration value as a boolean
 * 
 * Convenience wrapper for setting boolean values with source tracking.
 * Converts the boolean to ConfigValue and stores it.
 * 
 * @param key Configuration key to set
 * @param value Boolean value to store
 * @param source Source of this configuration (default: Runtime)
 * 
 * @return void
 */
void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Checks if a configuration key exists
 * 
 * Performs a linear search through configuration entries to check for
 * the presence of a given key. Uses std::any_of with a lambda for the
 * search operation.
 * 
 * @param key Configuration key to check for existence
 * 
 * @return bool true if the key exists in the configuration, false otherwise
 * 
 * @note This is an O(n) operation where n is the number of configuration
 *       entries. For frequently accessed keys, consider maintaining an
 *       auxiliary data structure for faster lookups.
 */
bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

/**
 * @brief Removes a configuration key
 * 
 * Removes all configuration entries with the specified key from the
 * configuration. Uses std::remove_if to filter out matching entries,
 * then erases them in linear time.
 * 
 * @param key Configuration key to remove
 * 
 * @return void
 * 
 * @note If multiple entries exist for the same key (unlikely in normal use),
 *       all are removed. The operation preserves the order of remaining entries.
 */
void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

/**
 * @brief Retrieves all configuration keys
 * 
 * Returns a vector containing all keys currently stored in the configuration.
 * This is useful for introspection, documentation generation, or when you
 * need to iterate over all configured settings.
 * 
 * @return std::vector<std::string> Vector containing all configuration keys
 * 
 * @note The returned vector is in the same order as entries in the
 *       internal pImpl::entries vector. This maintains insertion order.
 *       The operation is O(n) where n is the number of configuration entries.
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
 * @brief Clears all configuration entries
 * 
 * Removes all configuration entries from the Config object, leaving it
 * in the same state as after construction (empty configuration).
 * 
 * @return void
 * 
 * @note This operation resets the configuration to its default empty state.
 *       All source tracking information is lost.
 */
void Config::clear() {
    pImpl->entries.clear();
}

/**
 * @brief Generates a human-readable configuration summary
 * 
 * Creates a formatted string describing the current configuration state,
 * including the number of entries and the value/source for each entry.
 * This is useful for debugging, logging, and configuration validation.
 * 
 * @return std::string Formatted configuration summary string
 * 
 * @note The output format is:
 *       Configuration (X entries):
 *         key = [value] (source)
 *       Where source is shown as an integer (ConfigSource enum value).
 *       String values are quoted, other types shown directly.
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
 * @brief Trims whitespace from both ends of a string
 * 
 * Removes leading and trailing whitespace characters (spaces, tabs, newlines,
 * carriage returns) from the input string. Used for normalizing configuration
 * file parsing and command-line argument processing.
 * 
 * @param str Input string to trim
 * 
 * @return std::string Trimmed string with whitespace removed from both ends
 * 
 * @note Empty strings or strings containing only whitespace return an empty
 *       string. This implementation is simple but effective for typical
 *       configuration file formats.
 */
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

/**
 * @brief Converts a string to lowercase
 * 
 * Performs case-insensitive string conversion by transforming all uppercase
 * characters to lowercase. Useful for case-insensitive configuration key
 * comparisons and normalization.
 * 
 * @param str Input string to convert
 * 
 * @return std::string Lowercase version of the input string
 * 
 * @note Uses std::tolower from the C standard library. Only affects ASCII
 *       characters. Non-alphabetic characters are unchanged.
 */
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

} // namespace nlm
