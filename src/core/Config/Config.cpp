#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

/**
 * @brief Implementation of Config class methods
 * 
 * This file provides the complete implementation of the configuration management system.
 * It includes file I/O, parsing, type conversion, and all utility methods for the Config class.
 * 
 * The implementation follows the pimpl idiom to maintain encapsulation and reduce
 * compilation dependencies. All public interface is declared in Config.hpp.
 */

struct Config::Impl {
    /**
     * @brief Internal storage for configuration entries
     * 
     * Contains the vector of ConfigEntry structures that store all configuration
     * values along with their source information and descriptions.
     */
    std::vector<ConfigEntry> entries;
};

/**
 * @brief Default constructor implementation
 * 
 * Creates a new configuration object and initializes its internal storage.
 * The configuration starts empty, with no entries defined.
 */
Config::Config() : pImpl(std::make_unique<Impl>()) {}

/**
 * @brief Destructor implementation
 * 
 * Cleans up the internal implementation structure. The pimpl idiom ensures
 * that the implementation details are properly destroyed when the Config object
 * is no longer needed.
 */
Config::~Config() = default;

/**
 * @brief Move constructor implementation
 * 
 * Enables efficient transfer of resources between Config objects.
 * The implementation is moved rather than copied, avoiding deep copy costs.
 * 
 * @param other Source configuration to move from
 */
Config::Config(Config&&) noexcept = default;

/**
 * @brief Move assignment operator implementation
 * 
 * Enables efficient assignment between Config objects.
 * Uses move semantics to transfer resources efficiently.
 * 
 * @param other Source configuration to move from
 * @return Reference to this configuration object
 */
Config& Config::operator=(Config&&) noexcept = default;

/**
 * @brief Load configuration from file implementation
 * 
 * Parses configuration from a text file using a simple key=value format.
 * Supports comments (lines starting with # or /), handles quoted values,
 * and manages source tracking for each configuration entry.
 * 
 * @param filepath Path to the configuration file
 * @return true if file loaded successfully, false if file not found, invalid, or parsing error
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
 * @brief Load configuration from command line arguments implementation
 * 
 * Parses command line arguments in the format --key=value or -key value.
 * Arguments are processed in order, with later arguments taking precedence
 * over earlier ones for the same configuration key.
 * 
 * @param argc Number of command line arguments
 * @param argv Command line argument array (argv[0] is program name)
 * @return true if parsing succeeded, false otherwise
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
 * Writes current configuration to a text file. This is currently a placeholder
 * implementation that outputs "PLACEHOLDER_VALUE" for all values and includes
 * comments with descriptions. Phase 2+ should implement proper JSON/YAML serialization
 * with proper value preservation.
 * 
 * @param filepath Path where configuration should be saved
 * @return true if file saved successfully, false otherwise
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
 * @brief Set configuration value implementation (generic variant overload)
 * 
 * Sets a configuration value using the generic ConfigValue variant type.
 * Manages the entry list, either updating an existing entry or adding a new one.
 * Handles source priority and maintains the order of configuration sources.
 * 
 * @param key Configuration key
 * @param value New value (stored as ConfigValue variant)
 * @param source Source of this configuration value
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
 * @brief Set configuration value implementation (string overload)
 * 
 * Convenience overload for setting string values. Converts the string to
 * ConfigValue variant before delegating to the generic set() method.
 * 
 * @param key Configuration key
 * @param value String value
 * @param source Source of this configuration value
 */
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Set configuration value implementation (int overload)
 * 
 * Convenience overload for setting integer values. Converts the int to
 * ConfigValue variant before delegating to the generic set() method.
 * 
 * @param key Configuration key
 * @param value Integer value
 * @param source Source of this configuration value
 */
void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Set configuration value implementation (int64_t overload)
 * 
 * Convenience overload for setting int64_t values. Converts the value to
 * ConfigValue variant before delegating to the generic set() method.
 * 
 * @param key Configuration key
 * @param value int64_t value
 * @param source Source of this configuration value
 */
void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Set configuration value implementation (double overload)
 * 
 * Convenience overload for setting double values. Converts the value to
 * ConfigValue variant before delegating to the generic set() method.
 * 
 * @param key Configuration key
 * @param value double value
 * @param source Source of this configuration value
 */
void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Set configuration value implementation (bool overload)
 * 
 * Convenience overload for setting bool values. Converts the value to
 * ConfigValue variant before delegating to the generic set() method.
 * 
 * @param key Configuration key
 * @param value bool value
 * @param source Source of this configuration value
 */
void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Check if configuration key exists implementation
 * 
 * Searches through all configuration entries to determine if a key is present.
 * Uses linear search which is efficient for the typically small number of
 * configuration entries in the NLM system.
 * 
 * @param key Configuration key to check
 * @return true if key exists, false otherwise
 */
bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

/**
 * @brief Remove configuration key implementation
 * 
 * Removes the configuration entry for the specified key from the entries list.
 * Uses efficient erase-remove idiom for optimal performance.
 * 
 * @param key Configuration key to remove
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
 * Returns a vector containing all configuration keys currently stored.
 * Pre-allocates memory to avoid reallocations during population.
 * 
 * @return Vector containing all configuration keys
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
 * @brief Clear all configuration entries implementation
 * 
 * Removes all configuration entries, resetting the configuration object
 * to its default empty state. This effectively clears all configuration
 * values, regardless of their source.
 */
void Config::clear() {
    pImpl->entries.clear();
}

/**
 * @brief Get configuration summary implementation
 * 
 * Generates a human-readable summary of all configuration entries, including
 * their values, types, and sources. This is useful for debugging and inspection.
 * The output format is designed to be both machine-readable and human-friendly.
 * 
 * @return String representation of all configuration entries with metadata
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
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

/**
 * @brief Trim whitespace implementation
 * 
 * Removes leading and trailing whitespace characters (spaces, tabs, newlines)
 * from a string. Used for parsing configuration file entries and cleaning
 * up user input.
 * 
 * @param str Input string to trim
 * @return Trimmed string with no leading/trailing whitespace
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
 * Converts all uppercase characters in a string to lowercase.
 * Used for case-insensitive configuration key matching and comparison.
 * 
 * @param str Input string to convert
 * @return Lowercase version of input string
 */
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

/**
 * @brief Get implementation (int template specialization)
 * 
 * Retrieves an integer configuration value. Handles automatic conversion
 * from other numeric types (double, int64_t) for flexibility while maintaining
 * type safety through the variant system.
 * 
 * @param key Configuration key to look up
 * @return std::optional<int> Value if key exists and type matches, nullopt otherwise
 */
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

/**
 * @brief Get or implementation (int template specialization)
 * 
 * Retrieves a configuration value with fallback to a default value.
 * If the key doesn't exist or has an incompatible type, returns the
 * provided default value. This is useful for optional configuration
 * parameters with sensible defaults.
 * 
 * @tparam T Target type
 * @param key Configuration key to look up
 * @param defaultValue Default value to return if key not found
 * @return Retrieved value or default if key doesn't exist/incompatible
 */
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
