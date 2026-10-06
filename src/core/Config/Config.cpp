/**
  * @file Config.cpp
  * @brief Implementation of the configuration management system
  * 
  * This file contains the implementation of the Config class, which provides
  * a comprehensive configuration management system supporting multiple formats,
  * type-safe access, and multi-source configuration with precedence resolution.
  * 
  * The implementation includes:
  * - File format detection and parsing (JSON, YAML, simple text)
  * - Command-line argument parsing
  * - Type-safe configuration value storage and retrieval
  * - String conversion utilities for complex types
  * - Comprehensive error handling and logging
  * - Configuration validation with comprehensive checking
  * 
  * @note This file implements the Config class declared in Config.hpp
  * @see Config.hpp for the public interface
  * @see ConfigEntry for internal storage structure
  * @see Validation/ for the validation framework
  * 
  * @since Version 1.0.0
  */

#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>
#include "Validation/ValidationException.hpp"
#include "Validation/ConfigValidator.hpp"
#include "Validation/SchemaValidator.hpp"

namespace nlm {

/**
 * @brief Internal implementation structure for Config class
 * 
 * Contains the actual storage for configuration entries. Using a separate
 * structure with the pImpl idiom allows us to keep the implementation details
 * private from the header file while providing efficient access to configuration data.
 * 
 * @note This structure is an implementation detail and should not be accessed
 *       directly by users of the Config class.
 */
struct Config::Impl {
    /** @brief Vector of all configuration entries */
    std::vector<ConfigEntry> entries;
};

/**
 * @brief Default constructor - initializes the implementation
 * 
 * Creates a new implementation structure with an empty entries vector,
 * effectively starting with an empty configuration.
 */
Config::Config() : pImpl(std::make_unique<Impl>()) {}

/**
 * @brief Destructor - cleans up implementation resources
 * 
 * The destructor is defaulted and automatically cleans up the unique_ptr
 * to the implementation structure, freeing any allocated memory.
 */
Config::~Config() = default;

/**
 * @brief Move constructor - transfers ownership of implementation
 * 
 * Efficient move constructor that transfers ownership of the internal
 * implementation without performing a deep copy. Provides exception safety
 * and enables moving Config objects efficiently.
 */
Config::Config(Config&&) noexcept = default;

/**
 * @brief Move assignment operator - transfers ownership of implementation
 * 
 * Efficient move assignment that transfers ownership of the internal
 * implementation without performing a deep copy. Provides exception safety
 * and enables moving assignment of Config objects efficiently.
 */
Config& Config::operator=(Config&&) noexcept = default;

/**
 * @brief Load configuration from file with automatic format detection
 * 
 * Determines file format by examining the file extension:
 * - .json files are parsed as JSON format
 * - .yaml/.yml files are parsed as YAML format
 * - All other formats fall back to simple key=value format
 * 
 * @param filepath Path to the configuration file
 * 
 * @return true if the file was successfully loaded and parsed, false otherwise
 * 
 * @throws std::filesystem::filesystem_error if file operations fail (file not found,
 *         permissions error, etc.)
 * @throws nlohmann::json::exception if JSON parsing fails
 * @throws YAML::Exception if YAML parsing fails
 * 
 * @note The simple format fallback provides backward compatibility with
 *       older configuration files. The detected format determines which
 *       internal parsing method is used.
 * 
 * @see loadFromJSON() for JSON-specific implementation
 * @see loadFromYAML() for YAML-specific implementation
 * @see loadFromSimpleFormat() for basic key=value parsing
 */
bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Check if file exists first
        if (!std::filesystem::exists(filepath)) {
            auto& errorManager = getErrorManager();
            errorManager.handleError(
                errorManager.configFileError(
                    __FILE__, __LINE__, __func__,
                    "File not found",
                    filepath
                )
            );
            return false;
        }
        
        if (!std::filesystem::is_regular_file(filepath)) {
            auto& errorManager = getErrorManager();
            errorManager.handleError(
                errorManager.configFileError(
                    __FILE__, __LINE__, __func__,
                    "Path is not a regular file",
                    filepath
                )
            );
            return false;
        }
        
        // Determine file format based on extension
        std::string ext = std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".json") {
            return loadFromJSON(filepath);
        } else if (ext == ".yaml" || ext == ".yml") {
            return loadFromYAML(filepath);
        } else {
            // Backward compatibility with simple key=value format
            return loadFromSimpleFormat(filepath);
        }
    } catch (const std::filesystem::filesystem_error& e) {
        auto& errorManager = getErrorManager();
        errorManager.handleError(
            errorManager.configFileError(
                __FILE__, __LINE__, __func__,
                "Filesystem error: " + std::string(e.what()),
                filepath
            )
        );
        return false;
    } catch (const std::exception& e) {
        auto& errorManager = getErrorManager();
        errorManager.handleError(
            errorManager.configFileError(
                __FILE__, __LINE__, __func__,
                "Error loading configuration: " + std::string(e.what()),
                filepath
            )
        );
        return false;
    }
}

/**
 * @brief Load configuration from command-line arguments
 * 
 * Parses command-line arguments in two supported formats:
 * --key=value or -key value
 * 
 * @param argc Number of command-line arguments (argc from main())
 * @param argv Array of command-line argument strings (argv from main())
 * 
 * @return true if parsing completed (even with invalid entries), false on critical errors
 * 
 * @note Invalid arguments are silently ignored; only valid --key=value or -key value pairs
 *       are processed. This allows applications to use other command-line options.
 * 
 * @see set() for the method that actually sets the values
 * @see ConfigSource::CommandLine for marking values from command line
 */
bool Config::loadFromArgs(int argc, char** argv) {
    try {
        // Validate input parameters
        if (argc < 0) {
            auto& errorManager = getErrorManager();
            errorManager.handleError(
                errorManager.configFileError(
                    __FILE__, __LINE__, __func__,
                    "Invalid argument count",
                    "",
                    "argc = " + std::to_string(argc)
                )
            );
            return false;
        }
        
        for (int i = 1; i < argc; ++i) {
            if (!argv[i]) {
                auto& errorManager = getErrorManager();
                errorManager.handleError(
                    errorManager.configFileError(
                        __FILE__, __LINE__, __func__,
                        "NULL argument pointer",
                        "",
                        "Argument index: " + std::to_string(i)
                    )
                );
                continue; // Skip invalid argument
            }
            
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    if (key.empty()) {
                        auto& errorManager = getErrorManager();
                        errorManager.handleError(
                            errorManager.configFileError(
                                __FILE__, __LINE__, __func__,
                                "Empty key in --key=value format",
                                "",
                                "Argument: " + arg
                            )
                        );
                        continue;
                    }
                    
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    auto& errorManager = getErrorManager();
                    errorManager.handleError(
                        errorManager.configFileError(
                            __FILE__, __LINE__, __func__,
                            "Invalid --key=value format (missing '=')",
                            "",
                            "Argument: " + arg
                        )
                    );
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                if (key.empty()) {
                    auto& errorManager = getErrorManager();
                    errorManager.handleError(
                        errorManager.configFileError(
                            __FILE__, __LINE__, __func__,
                            "Empty key in -key value format",
                            "",
                            "Argument: " + arg
                        )
                    );
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            }
            else if (arg[0] == '-') {
                auto& errorManager = getErrorManager();
                errorManager.handleError(
                    errorManager.configFileError(
                        __FILE__, __LINE__, __func__,
                        "Argument with '-' prefix requires a value",
                        "",
                        "Argument: " + arg
                    )
                );
            }
        }
        return true;
    } catch (const std::exception& e) {
        auto& errorManager = getErrorManager();
        errorManager.handleError(
            errorManager.configFileError(
                __FILE__, __LINE__, __func__,
                "Error parsing command line arguments: " + std::string(e.what()),
                "",
                "argc = " + std::to_string(argc)
            )
        );
        return false;
    }
}

/**
 * @brief Save current configuration to a file
 * 
 * Saves configuration in simple key=value format for maximum compatibility.
 * Note: Only the most recent value for each key is saved; previous values
 * from different sources may be lost.
 * 
 * @param filepath Path where the configuration file should be saved
 * 
 * @return true if the file was successfully saved, false otherwise
 * 
 * @throws std::filesystem::filesystem_error if file operations fail
 * 
 * @note This method saves in the simple key=value format, not JSON or YAML.
 *       For complex types (arrays, nested structures), only the string
 *       representation is saved.
 * 
 * @see ConfigSource for information about value origins
 */
bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Check if directory exists
        std::filesystem::path path(filepath);
        if (!path.parent_path().empty()) {
            if (!std::filesystem::exists(path.parent_path())) {
                auto& errorManager = getErrorManager();
                errorManager.handleError(
                    errorManager.ioError(
                        __FILE__, __LINE__, __func__,
                        "Directory does not exist",
                        filepath,
                        "save"
                    )
                );
                return false;
            }
            
            if (!std::filesystem::is_directory(path.parent_path())) {
                auto& errorManager = getErrorManager();
                errorManager.handleError(
                    errorManager.ioError(
                        __FILE__, __LINE__, __func__,
                        "Path is not a directory",
                        filepath,
                        "save"
                    )
                );
                return false;
            }
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            // Get more detailed error information
            std::error_code ec;
            std::filesystem::file_status status = std::filesystem::status(filepath, ec);
            
            if (ec) {
                auto& errorManager = getErrorManager();
                errorManager.handleError(
                    errorManager.ioError(
                        __FILE__, __LINE__, __func__,
                        "Cannot open file for writing: " + ec.message(),
                        filepath,
                        "save"
                    )
                );
            } else {
                auto& errorManager = getErrorManager();
                errorManager.handleError(
                    errorManager.ioError(
                        __FILE__, __LINE__, __func__,
                        "Cannot open file for writing (permission or other error)",
                        filepath,
                        "save"
                    )
                );
            }
            return false;
        }
        
        // Write header comment
        file << "# NLM Configuration File\n";
        file << "# Generated by Config::saveToFile()\n";
        file << "# Format: key = value\n\n";
        
        // Write configuration entries with error checking
        for (const auto& entry : pImpl->entries) {
            // Write description comment
            if (!entry.description.empty()) {
                file << "# " << entry.description << "\n";
            }
            
            // Write key-value pair with quotes for strings
            file << entry.key << " = ";
            
            try {
                if (std::holds_alternative<std::string>(entry.value)) {
                    const std::string& strVal = std::get<std::string>(entry.value);
                    // Check for special characters that need escaping
                    if (strVal.find(' ') != std::string::npos || 
                        strVal.find('\t') != std::string::npos ||
                        strVal.find('#') != std::string::npos ||
                        strVal.find('=') != std::string::npos) {
                        file << "\"" << escapeString(strVal) << "\"";
                    } else {
                        file << strVal;
                    }
                } else {
                    file << entry.value; // Use visitor pattern or to_string
                }
            } catch (const std::exception& e) {
                auto& errorManager = getErrorManager();
                errorManager.handleError(
                    errorManager.configFileError(
                        __FILE__, __LINE__, __func__,
                        "Error writing value for key: " + entry.key + ": " + std::string(e.what()),
                        filepath,
                        entry.key
                    )
                );
                // Skip this entry and continue
                continue;
            }
            
            file << "\n\n";
        }
        
        // Check for write errors
        if (file.fail()) {
            auto& errorManager = getErrorManager();
            errorManager.handleError(
                errorManager.ioError(
                    __FILE__, __LINE__, __func__,
                    "Error during file write operations",
                    filepath,
                    "save"
                )
            );
            return false;
        }
        
        file.flush();
        if (file.fail()) {
            auto& errorManager = getErrorManager();
            errorManager.handleError(
                errorManager.ioError(
                    __FILE__, __LINE__, __func__,
                    "Error flushing file buffer",
                    filepath,
                    "save"
                )
            );
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        auto& errorManager = getErrorManager();
        errorManager.handleError(
            errorManager.configFileError(
                __FILE__, __LINE__, __func__,
                "Error saving configuration: " + std::string(e.what()),
                filepath
            )
        );
        return false;
    }
}

/**
 * @brief Template method to retrieve a configuration value with type safety
 * 
 * Extracts a configuration value by key and returns it as the requested type.
 * If the value doesn't exist or can't be converted to the requested type,
 * returns std::nullopt. This method provides compile-time type checking.
 * 
 * @tparam T Type to retrieve (must match one of ConfigValue supported types)
 * @param key Configuration key to look up
 * 
 * @return std::optional<T> The value if found and of correct type, std::nullopt otherwise
 * 
 * @note The supported types T are exactly those supported by ConfigValue:
 *       - int, int64_t
 *       - double
 *       - bool
 *       - std::string
 *       - std::vector<int>, std::vector<double>, std::vector<std::string>
 * 
 * @see getOr() for a version that returns a default value
 * @see set() for storing values of the same types
 * @see ConfigValue for the complete list of supported types
 * 
 * @warning Calling get with a type that doesn't match the stored type will return
 *          std::nullopt, not throw an exception.
 * 
 * @since Version 1.0.0
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
 * @brief Template method to retrieve a configuration value with default fallback
 * 
 * Retrieves a configuration value by key, returning a default value if the
 * key doesn't exist or can't be converted to the requested type. This provides
 * a convenient way to specify default values directly in the call.
 * 
 * @tparam T Type to retrieve (must match one of ConfigValue supported types)
 * @param key Configuration key to look up
 * @param defaultValue Value to return if the key doesn't exist or conversion fails
 * 
 * @return T The retrieved value, or defaultValue if not found
 * 
 * @note This is a convenience wrapper around get() that provides a default
 *       fallback. The default value is returned as-is without any type checking.
 * 
 * @see get() for the version that returns std::nullopt on failure
 * @see set() for storing values of the same types
 * 
 * @since Version 1.0.0
 */
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

/**
 * @brief Store a configuration value (internal version)
 * 
 * Stores a configuration value using the generic ConfigValue type. This is the
 * core implementation method used by all other set() overloads.
 * 
 * @param key Configuration parameter key
 * @param value Configuration value (any type supported by ConfigValue)
 * @param source Origin/source of this configuration value
 * 
 * @note If a key already exists, its value and source will be updated.
 *       If it doesn't exist, a new entry will be created.
 * 
 * @see ConfigValue for supported value types
 * @see ConfigSource for source specification
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
 * @brief Store a string configuration value
 * 
 * @param key Configuration parameter key
 * @param value String value to store
 * @param source Origin/source of this configuration value (default: Runtime)
 * 
 * @see set(const std::string&, const ConfigValue&, ConfigSource) for the generic version
 */
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Store an integer configuration value
 * 
 * @param key Configuration parameter key
 * @param value Integer value to store
 * @param source Origin/source of this configuration value (default: Runtime)
 * 
 * @see set(const std::string&, const ConfigValue&, ConfigSource) for the generic version
 */
void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Store a floating-point configuration value
 * 
 * @param key Configuration parameter key
 * @param value Double value to store
 * @param source Origin/source of this configuration value (default: Runtime)
 * 
 * @see set(const std::string&, const ConfigValue&, ConfigSource) for the generic version
 */
void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Store a boolean configuration value
 * 
 * @param key Configuration parameter key
 * @param value Boolean value to store
 * @param source Origin/source of this configuration value (default: Runtime)
 * 
 * @see set(const std::string&, const ConfigValue&, ConfigSource) for the generic version
 */
void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Check if a configuration key exists
 * 
 * @param key Configuration key to check
 * 
 * @return true if the key exists in the configuration, false otherwise
 * 
 * @complexity O(n) where n is the number of configuration entries
 * 
 * @see get() for retrieving the value associated with a key
 * @see remove() for removing a configuration entry
 */
bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

/**
 * @brief Remove a configuration entry
 * 
 * Removes all entries matching the specified key. This operation cannot
 * be undone.
 * 
 * @param key Configuration key to remove
 * 
 * @note This permanently removes the configuration entry from memory.
 *       It does not modify the original file (if loaded from file).
 * 
 * @see has() for checking key existence
 * @see clear() for removing all entries at once
 */
void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

/**
 * @brief Get all configuration keys
 * 
 * @return std::vector<std::string> List of all configuration keys currently stored
 * 
 * @note The order of returned keys is not guaranteed (depends on internal storage)
 * 
 * @complexity O(n) where n is the number of configuration entries
 * 
 * @see has() for checking individual key existence
 * @see get() for retrieving values for specific keys
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
 * @brief Clear all configuration entries
 * 
 * Removes all configuration entries from memory. This operation cannot
 * be undone. After calling clear(), the configuration is empty.
 * 
 * @note This does not modify the original file (if loaded from file).
 *       To reload from file, call loadFromFile() again.
 * 
 * @see remove() for removing individual entries
 * @see loadFromFile() for repopulating the configuration
 */
void Config::clear() {
    pImpl->entries.clear();
}

/**
 * @brief Generate a human-readable summary of the current configuration
 * 
 * Creates a formatted string showing all configuration entries with their
 * values, types, and source information. Useful for debugging and logging.
 * 
 * @return std::string Formatted configuration summary
 * 
 * @note The output format is designed for human readability:
 *       - Shows key = value [source] for each entry
 *       - Converts values to human-readable strings
 *       - Displays source as integer code (ConfigSource enum value)
 * 
 * @see ConfigSource for source code meanings
 * @see getKeys() for getting just the key names
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
 * @brief Remove leading and trailing whitespace from a string
 * 
 * @param str String to trim
 * @return std::string Trimmed string with no leading/trailing whitespace
 * 
 * @note Removes spaces, tabs, carriage returns, and newlines
 * 
 * @see toLower() for case conversion
 * @see loadFromSimpleFormat() for usage in file parsing
 */
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

/**
 * @brief Convert a string to lowercase
 * 
 * @param str String to convert
 * @return std::string Lowercase version of the input string
 * 
 * @note Uses std::transform with ::tolower for efficient conversion
 * 
 * @see trim() for whitespace removal
 * @see loadFromFile() for format detection
 */
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string Config::escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size() * 2); // Reserve space for escapes
    
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            default:
                if (c < 32 || c > 126) {
                    result += "\\x" + stringToHex(c);
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

std::string Config::stringToHex(char c) {
    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c));
    return ss.str();
}
/**
  * @brief Validate the configuration against a schema
  * 
  * Validates the current configuration against a JSON schema-like definition.
  * Provides detailed error messages for validation failures.
  * 
  * @param schema JSON schema object to validate against
  * @return bool true if configuration is valid, false otherwise
  * 
  * @throws ValidationException if schema is invalid
  * 
  * @note Schema format supports type, required, min, max, enum, pattern, and format constraints
  * @see SchemaValidator for schema validation details
  */
bool Config::validate(const nlohmann::json& schema) {
    try {
        SchemaValidator validator;
        ValidationResult result = validator.validate(*this, schema);
        
        // Convert validation result to return value
        if (!result.valid) {
            std::string errorMsg = "Schema validation failed: ";
            for (size_t i = 0; i < result.errors.size(); ++i) {
                if (i > 0) errorMsg += ", ";
                errorMsg += result.errors[i];
            }
            throw ValidationException(errorMsg, ValidationErrorCategory::Schema);
        }
        
        return result.valid;
    } catch (const ValidationException& e) {
        throw;
    } catch (const std::exception& e) {
        throw ValidationException(std::string("Schema validation error: ") + e.what(), 
                                 ValidationErrorCategory::Schema);
    }
}

/**
  * @brief Validate the configuration using the built-in validator
  * 
  * Performs comprehensive validation including type checking, range validation,
  * dependency validation, and consistency checking.
  * 
  * @return bool true if configuration is valid, false otherwise
  * 
  * @see ConfigValidator for validation details
  */
bool Config::validate() const {
    try {
        ConfigValidator validator;
        validator.setConfig(*this);
        ValidationResult result = validator.validateAll();
        
        return result.valid;
    } catch (const std::exception& e) {
        // Log validation error but don't throw
        return false;
    }
}

/**
  * @brief Get validation errors for the configuration
  * 
  * @return ValidationResult structure containing validation errors and warnings
  */
ValidationResult Config::getValidationResult() const {
    ValidationResult result;
    
    try {
        ConfigValidator validator;
        validator.setConfig(*this);
        result = validator.validateAll();
    } catch (const std::exception& e) {
        result.valid = false;
        result.errors.push_back(std::string("Validation error: ") + e.what());
    }
    
    return result;
}

/**
  * @brief Validate configuration before saving to file
  * 
  * Ensures configuration integrity before writing to file. Can be called
  * as a pre-validation step or integrated into saveToFile().
  * 
  * @throws ValidationException if configuration is invalid
  * 
  * @note This method is called automatically when saveToFile() is invoked
  *       if autoValidation is enabled
  */
void Config::validateBeforeSave() const {
    try {
        if (!validate()) {
            ValidationResult result = getValidationResult();
            std::string errorMsg = "Configuration validation failed before save: ";
            for (size_t i = 0; i < result.errors.size(); ++i) {
                if (i > 0) errorMsg += ", ";
                errorMsg += result.errors[i];
            }
            throw ValidationException(errorMsg, ValidationErrorCategory::Consistency);
        }
    } catch (const ValidationException& e) {
        throw;
    } catch (const std::exception& e) {
        throw ValidationException(std::string("Pre-save validation error: ") + e.what(),
                                 ValidationErrorCategory::Consistency);
    }
}

/**
  * @brief Check if a configuration key is valid
  * 
  * @param key Configuration key to check
  * @return bool true if key is valid, false otherwise
  * 
  * @note This method performs basic validation of configuration keys
  */
bool Config::isValidKey(const std::string& key) const {
    // Basic key validation
    if (key.empty()) {
        return false;
    }
    
    // Check for invalid characters in key
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '.' && c != '-') {
            return false;
        }
    }
    
    // Check if key already exists
    return !has(key);
}

/**
  * @brief Get a list of valid configuration keys
  * 
  * @return std::vector<std::string> List of valid configuration keys
  */
std::vector<std::string> Config::getValidKeys() {
    // Return common valid configuration keys
    std::vector<std::string> validKeys = {
        "neuron_count",
        "region_count",
        "simulation_timestep",
        "random_seed",
        "connection_probability",
        "learning_rate",
        "batch_size",
        "enable_structural_plasticity",
        "enable_development",
        "enable_reward_modulation",
        "enable_curiosity",
        "stdp_ltp_weight",
        "stdp_ltd_weight",
        "stdp_tau",
        "synaptogenesis_rate",
        "pruning_rate"
    };
    
    return validKeys;
}

/**
  * @brief Explicit template instantiations for get() method
  * 
  * These instantiations ensure that the get() method is compiled for all
  * supported types at link time, preventing linker errors when these types
  * are used in user code.
  */
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

/**
  * @brief Explicit template instantiations for getOr() method
  * 
  * These instantiations ensure that the getOr() method is compiled for all
  * supported types at link time, preventing linker errors when these types
  * are used in user code.
  */
template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
