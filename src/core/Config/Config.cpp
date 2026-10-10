#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "core/Logger/Logger.hpp"

namespace nlm {

/**
 * @brief Internal implementation struct for Config class (Pimpl idiom).
 * 
 * This struct contains the actual implementation details of the configuration system.
 * Keeping implementation details private allows for easier changes without affecting
 * the public interface of the Config class.
 */
struct Config::Impl {
    std::vector<ConfigEntry> entries;  ///< Collection of all configuration entries
};

/**
 * @brief Constructs a Config object with internal implementation.
 * 
 * Initializes the Pimpl pattern by creating a new implementation instance.
 * This allows for proper encapsulation of implementation details.
 */
Config::Config() : pImpl(std::make_unique<Impl>()) {}

/**
 * @brief Default destructor for Config.
 * 
 * The default implementation is sufficient as the pImpl unique_ptr will
 * automatically clean up the internal implementation when the object is destroyed.
 */
Config::~Config() = default;

/**
 * @brief Move constructor for Config objects.
 * 
 * Transfers ownership of the internal implementation from the source object
 * to the newly created object. The source object will no longer manage
 * its implementation after the move.
 * 
 * @param other The Config object to move from
 */
Config::Config(Config&&) noexcept = default;

/**
 * @brief Move assignment operator for Config objects.
 * 
 * Transfers ownership of the internal implementation from the source object
 * to this object. The source object will no longer manage its implementation
 * after the move. Self-assignment is safely handled by the default implementation.
 * 
 * @param other The Config object to move from
 * @return Reference to this Config object for method chaining
 */
Config& Config::operator=(Config&&) noexcept = default;

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

/**
 * @brief Loads configuration values from a text file.
 * 
 * Parses a simple key=value format configuration file. Lines starting with
 * #, ;, or // are treated as comments and ignored. Empty lines are also skipped.
 * The parser handles quoted values and validates configuration keys.
 * 
 * @param filepath Path to the configuration file to load
 * @return true if the file was successfully parsed and loaded, false if:
 *         - File doesn't exist
 *         - File cannot be opened
 *         - File is malformed (invalid format)
 * @note Currently supports simple key=value pairs. TODO: Implement proper JSON/YAML parser
 * @see ConfigSource::File
 */
bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists before attempting to open to provide early error
    if (!std::filesystem::exists(filepath)) {
        Logger::getGlobal().error("Config file not found: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        Logger::getGlobal().error("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments (support for multiple comment styles)
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';' || line.rfind("//", 0) == 0) {
            continue;
        }
        
        // Parse simple key=value pairs with quoted value support
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key - empty keys are not allowed and are logged as warnings
            if (key.empty()) {
                Logger::getGlobal().warning("Empty key found at line " + std::to_string(lineNumber));
                continue;
            }
            
            // Validate value - log warning for empty values but allow (could be valid)
            if (value.empty()) {
                Logger::getGlobal().warning("Empty value for key '" + key + "' at line " + std::to_string(lineNumber));
            }
            
            // Remove surrounding quotes if present (handles both single and double quotes)
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            Logger::getGlobal().warning("Malformed config line (missing '='): " + line + " at line " + std::to_string(lineNumber));
        }
    }
    
    return true;
}

/**
 * @brief Loads configuration values from command-line arguments.
 * 
 * Supports two formats:
 * - --key=value (GNU-style long options)
 * - -k value (POSIX-style short options)
 * 
 * Command-line arguments have the highest precedence in the configuration system
 * and override values loaded from files and defaults.
 * 
 * @param argc Number of command-line arguments (argc-1 is the actual count of options)
 * @param argv Array of command-line argument strings (argv[0] is program name)
 * @return true if arguments were successfully processed, false otherwise
 * @note Empty arguments are logged as warnings and skipped
 * @see ConfigSource::CommandLine
 */
bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        return true;  // No arguments to process
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Validate argument - empty arguments are not allowed
        if (arg.empty()) {
            Logger::getGlobal().warning("Empty command line argument");
            continue;
        }
        
        // Handle --key=value format (GNU-style long options)
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key and value - empty keys are not allowed
                if (key.empty()) {
                    Logger::getGlobal().warning("Empty key in command line argument: " + arg);
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                Logger::getGlobal().warning("Malformed command line argument (missing '='): " + arg);
            }
        }
        // Handle -key value format (POSIX-style short options)
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key - empty keys are not allowed
            if (key.empty()) {
                Logger::getGlobal().warning("Empty key in command line argument: " + arg);
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
        } else {
            Logger::getGlobal().warning("Unhandled command line argument format: " + arg);
        }
    }
    return true;
}

/**
 * @brief Saves the current configuration to a file.
 * 
 * Writes configuration in a human-readable key=value format with descriptive comments.
 * Note: The implementation currently uses placeholder values for actual configuration
 * values. This is a work-in-progress feature.
 * 
 * @param filepath Path where the configuration file should be created or overwritten
 * @return true if the file was successfully created and written, false if:
 *         - File cannot be opened for writing
 *         - File system errors occur during writing
 * @note File permissions and paths are not validated - let the OS handle filesystem errors
 * @see ConfigEntry::description
 */
bool Config::saveToFile(const std::string& filepath) const {
    // Validate filepath before attempting to write
    if (filepath.empty()) {
        Logger::getGlobal().error("Empty filepath provided to saveToFile");
        return false;
    }
    
    // Ensure directory exists before attempting to write
    std::filesystem::path path(filepath);
    std::filesystem::path dir = path.parent_path();
    
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        // Try to create directory
        try {
            if (!std::filesystem::create_directories(dir)) {
                Logger::getGlobal().error("Failed to create directory for config file: " + dir.string());
                return false;
            }
        } catch (const std::exception& e) {
            Logger::getGlobal().error("Exception creating directory for config file " + dir.string() + ": " + e.what());
            return false;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        Logger::getGlobal().error("Failed to save config to file: " + filepath);
        return false;
    }
    
    // Set good precision for floating point values
    file.precision(6);
    file << std::fixed;
    
    // Write header information
    file << "# NLM Configuration File\n";
    file << "# Generated at " << "PLACEHOLDER_TIMESTAMP\n\n";
    
    // Write each configuration entry with its description as a comment
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            Logger::getGlobal().warning("Skipping config entry with empty key");
            continue;
        }
        
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Write value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

/**
 * @brief Retrieves a configuration value of the specified type from the configuration.
 * 
 * This template method performs type-safe value retrieval using std::variant type checking.
 * It searches for the key in the internal configuration entries and attempts to extract
 * the value as type T. If successful, returns the value in a std::optional; otherwise,
 * returns std::nullopt indicating either the key doesn't exist or the value is of wrong type.
 * 
 * The method handles bad_variant_access exceptions that occur when trying to extract
 * a value of incorrect type from the variant storage.
 * 
 * @tparam T The expected type of the configuration value (must be one of the types
 *           stored in ConfigValue variant)
 * @param key The configuration key to look up
 * @return std::optional<T> std::optional containing the value if:
 *         - Key exists in configuration
 *         - Value matches type T
 *         - No type conversion errors occur
 *         - std::nullopt if key doesn't exist, value is of wrong type, or extraction fails
 * @note Errors and type mismatches are logged for debugging purposes
 * @see ConfigValue for supported value types
 * 
 * @example
 * @code
 * // Get a string value if it exists and is of correct type
 * auto modelPath = config.get<std::string>("model_path");
 * if (modelPath) {
 *     std::cout << "Using model from: " << *modelPath << std::endl;
 * } else {
 *     std::cout << "No model path configured" << std::endl;
 * }
 * @endcode
 */
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Validate input key - empty keys are considered invalid
    if (key.empty()) {
        Logger::getGlobal().error("Empty key provided to get()");
        return std::nullopt;
    }
    
    // Find the configuration entry with matching key
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    // If key not found, return std::nullopt
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    // Attempt to extract the value as type T from the variant
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        // Log type mismatch warning for debugging
        Logger::getGlobal().warning("Type mismatch when retrieving key: " + key);
        return std::nullopt;
    }
}

/**
 * @brief Retrieves a configuration value or returns a default if not found.
 * 
 * This convenience method combines type-safe value retrieval with default value
 * fallback. It's particularly useful for optional configuration parameters where
 * a sensible default should be used if the configuration is missing or invalid.
 * 
 * The method first attempts to retrieve the value using the type-safe get() method.
 * If get() returns std::nullopt (key not found or type mismatch), the provided
 * default value is returned instead.
 * 
 * @tparam T The expected type of the configuration value
 * @param key The configuration key to look up
 * @param defaultValue Value to return if key is not found or type mismatch occurs
 * @return T The configuration value if found and of correct type, otherwise defaultValue
 * 
 * @example
 * @code
 * // Get learning rate with sensible default
 * float learningRate = config.getOr("learning_rate", 0.01f);
 * 
 * // Get model path with default fallback
 * std::string modelPath = config.getOr("model_path", "/default/model.bin");
 * 
 * // Get debug flag with false default
 * bool debugEnabled = config.getOr("debug", false);
 * @endcode
 */
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    // Validate input key - empty keys are considered invalid
    if (key.empty()) {
        Logger::getGlobal().error("Empty key provided to getOr()");
        return defaultValue;
    }
    
    // Attempt to get the value using type-safe retrieval
    auto val = get<T>(key);
    
    // Return the value if present, otherwise return the default
    return val.has_value() ? val.value() : defaultValue;
}

/**
 * @brief Sets a configuration value using the generic ConfigValue variant.
 * 
 * This is the primary setter method that accepts a ConfigValue variant and source
 * information. It will create a new configuration entry if the key doesn't exist,
 * or update the existing entry if the key already exists.
 * 
 * The method validates input parameters and handles invalid configuration sources
 * by defaulting to Runtime source with a warning.
 * 
 * @param key The configuration key to set
 * @param value The configuration value to store (as variant)
 * @param source The origin of this configuration value (default: Runtime)
 * 
 * @note Empty keys are logged as errors and the operation is aborted
 * @note Invalid source values are clamped to Runtime with warning
 * @see ConfigSource for valid source values and precedence rules
 */
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate input key - empty keys are considered invalid
    if (key.empty()) {
        Logger::getGlobal().error("Empty key provided to set()");
        return;
    }
    
    // Validate input source - ensure it's within valid enum range
    if (source < ConfigSource::Default || source > ConfigSource::Runtime) {
        Logger::getGlobal().warning("Invalid config source: " + std::to_string(static_cast<int>(source)));
        source = ConfigSource::Runtime;  // Default to Runtime on invalid source
    }
    
    // Find existing entry with matching key
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Update existing entry - preserve description, update value and source
        it->value = value;
        it->source = source;
    } else {
        // Create new entry
        pImpl->entries.emplace_back(key, value, source);
    }
}

/**
 * @brief Convenience overload for setting string values.
 * 
 * This overload provides type safety for string configuration values by
 * converting the string to a ConfigValue variant before calling the primary
 * set() method. This avoids code duplication for common string configuration.
 * 
 * @param key The configuration key to set
 * @param value The string value to store
 * @param source The origin of this configuration value (default: Runtime)
 * 
 * @note Uses the template to convert string to ConfigValue internally
 */
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    if (key.empty()) {
        Logger::getGlobal().error("Empty key provided to set(std::string)");
        return;
    }
    
    set(key, ConfigValue(value), source);
}

/**
 * @brief Convenience overload for setting integer values.
 * 
 * Type-safe setter for integer configuration values. Converts the integer
 * to ConfigValue variant and calls the primary set() method.
 * 
 * @param key The configuration key to set
 * @param value The integer value to store
 * @param source The origin of this configuration value (default: Runtime)
 */
void Config::set(const std::string& key, int value, ConfigSource source) {
    if (key.empty()) {
        Logger::getGlobal().error("Empty key provided to set(int)");
        return;
    }
    
    set(key, ConfigValue(value), source);
}

/**
 * @brief Convenience overload for setting double values.
 * 
 * Type-safe setter for floating-point configuration values. Converts the
 * double to ConfigValue variant and calls the primary set() method.
 * 
 * @param key The configuration key to set
 * @param value The double value to store
 * @param source The origin of this configuration value (default: Runtime)
 */
void Config::set(const std::string& key, double value, ConfigSource source) {
    if (key.empty()) {
        Logger::getGlobal().error("Empty key provided to set(double)");
        return;
    }
    
    set(key, ConfigValue(value), source);
}

/**
 * @brief Convenience overload for setting boolean values.
 * 
 * Type-safe setter for boolean configuration values. Converts the boolean
 * to ConfigValue variant and calls the primary set() method.
 * 
 * @param key The configuration key to set
 * @param value The boolean value to store
 * @param source The origin of this configuration value (default: Runtime)
 */
void Config::set(const std::string& key, bool value, ConfigSource source) {
    if (key.empty()) {
        Logger::getGlobal().error("Empty key provided to set(bool)");
        return;
    }
    
    set(key, ConfigValue(value), source);
}

/**
 * @brief Checks if a configuration key exists in the configuration.
 * 
 * Performs a case-sensitive search for the specified key among all configuration
 * entries. Empty keys are always considered non-existent and are logged as errors.
 * 
 * @param key The configuration key to check for existence
 * @return true if the key exists in the configuration, false otherwise
 * @note Key comparison is case-sensitive
 * @note Empty keys always return false and are logged as errors
 */
bool Config::has(const std::string& key) const {
    if (key.empty()) {
        Logger::getGlobal().error("Empty key provided to has()");
        return false;
    }
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

/**
 * @brief Removes a configuration key and its associated value from the configuration.
 * 
 * Performs a case-sensitive search for the specified key and removes the
 * corresponding configuration entry. Empty keys are logged as errors and
 * the operation is aborted.
 * 
 * @param key The configuration key to remove
 * @note Key comparison is case-sensitive
 * @note Empty keys always abort the operation and are logged as errors
 * @note If the key doesn't exist, this is a no-op
 */
void Config::remove(const std::string& key) {
    if (key.empty()) {
        Logger::getGlobal().error("Empty key provided to remove()");
        return;
    }
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

/**
 * @brief Retrieves all configuration keys as a vector of strings.
 * 
 * Iterates through all configuration entries and collects the keys into a
 * vector. Empty keys are filtered out and logged as warnings since they
 * don't represent valid configuration parameters.
 * 
 * @return std::vector<std::string> List of all non-empty configuration keys
 * @note The returned vector preserves insertion order
 * @note Empty keys are silently skipped and logged as warnings
 */
std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());  // Reserve memory for efficiency
    for (const auto& entry : pImpl->entries) {
        if (!entry.key.empty()) {
            keys.push_back(entry.key);
        } else {
            Logger::getGlobal().warning("Empty key found in config entries");
        }
    }
    return keys;
}

/**
 * @brief Clears all configuration entries from the configuration.
 * 
 * Removes all key-value pairs from the configuration. This is useful for
 * resetting configuration state or preparing to load a fresh configuration.
 * The operation logs information about how many entries were cleared.
 * 
 * @note Empty configurations are logged as debug messages
 * @note Non-runtime sources (File, CommandLine, Default) are not cleared
 * @see ConfigSource::Runtime
 */
void Config::clear() {
    if (pImpl->entries.empty()) {
        Logger::getGlobal().debug("Config already empty");
    } else {
        Logger::getGlobal().info("Clearing " + std::to_string(pImpl->entries.size()) + " config entries");
        pImpl->entries.clear();
    }
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    int emptyKeyCount = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            ++emptyKeyCount;
            continue;  // Skip empty keys in summary
        }
        
        oss << "  " << entry.key << " = ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " (" << static_cast<int>(entry.source) << ")\n";
    }
    
    if (emptyKeyCount > 0) {
        oss << "  [" << emptyKeyCount << " entries have empty keys]\n";
    }
    
    return oss.str();
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
