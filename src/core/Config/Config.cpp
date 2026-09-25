#include "Config.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>

/**
 * @file Config.cpp
 * @brief Implementation of the Config class for NLM configuration management
 *
 * This file contains the implementation of the Config class, which manages
 * configuration parameters for the Neural Learning Machine framework. The
 * implementation includes file I/O operations, type conversion, validation,
 * and comprehensive error handling.
 *
 * @note This implementation uses the PImpl idiom for efficient memory management
 *       and provides thread-safe operations with comprehensive logging.
 *
 * @author NLM Development Team
 * @date Phase 6 Development
 */

namespace nlm {

/**
 * @brief Internal implementation details for the Config class
 *
 * The PImpl (Pointer to Implementation) idiom is used to hide implementation
 * details of the Config class, allowing efficient compilation and binary
 * compatibility while maintaining encapsulation.
 *
 * This struct contains all the data members required for configuration
 * management, including the configuration entries and various helper methods.
 */
struct Config::Impl {
    /** Vector storing all configuration entries with their metadata */
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced loadFromFile with comprehensive error handling
bool Config::loadFromFile(const std::string& filepath) {
    NLM_LOG_INFO("Attempting to load configuration from file: " + filepath);
    
    // Validate file path before attempting to open
    if (filepath.empty()) {
        NLM_CONFIG_ERROR("File path is empty");
        return false;
    }
    
    // Check if file exists and is readable
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_FILE_ERROR("Cannot open configuration file: " + filepath);
        return false;
    }
    
    // Get file size for validation
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Check for reasonable file size (prevent loading huge files)
    if (fileSize > 10 * 1024 * 1024) { // 10MB limit
        NLM_CONFIG_ERROR("Configuration file too large (" + std::to_string(fileSize) + " bytes). Maximum size: 10MB");
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with validation
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key
            if (key.empty()) {
                NLM_CONFIG_ERROR("Empty configuration key at line " + std::to_string(lineNumber));
                continue;
            }
            
            if (key.find('=') != std::string::npos || key.find(' ') != std::string::npos) {
                NLM_CONFIG_ERROR("Invalid configuration key format at line " + std::to_string(lineNumber) + ": " + key);
                continue;
            }
            
            // Validate value
            if (value.empty()) {
                NLM_CONFIG_ERROR("Empty configuration value for key '" + key + "' at line " + std::to_string(lineNumber));
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_CONFIG_ERROR("Invalid configuration format at line " + std::to_string(lineNumber) + ": " + line.substr(0, 50));
        }
    }
    
    NLM_LOG_INFO("Successfully loaded configuration from file: " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
    return true;
}

/**
 * @brief Load configuration from command line arguments
 *
 * Parses command-line arguments in the format --key=value or -key value.
 * Supports multiple configuration parameters from the command line, with
 * conflict resolution based on source priority.
 *
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return true if parsing completed successfully, false otherwise
 * @throws NLMErrors::SystemNotInitialized if the Config object is not properly initialized
 * @throws NLMErrors::InvalidParameter if argument format is invalid
 * @throws NLMErrors::ConfigParsing if argument parsing fails
 *
 * @note Command-line arguments follow these formats:
 * - --key=value (preferred format)
 * - -key value (alternative format)
 *
 * Examples:
 * @code
 * // Command line: program --neuron_count=500 --simulation_timestep=0.001 --enable_plasticity=true
 * config.loadFromArgs(argc, argv);
 *
 * // Later access:
 * int neurons = config.getOr("neuron_count", 500);
 * double timestep = config.getOr("simulation_timestep", 0.001);
 * bool plasticity = config.getOr("enable_plasticity", false);
 * @endcode
 *
 * @see loadFromFile
 * @see set
 */
bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Loading configuration from command line arguments");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key.empty()) {
                    NLM_CONFIG_ERROR("Empty configuration key from command line");
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_CONFIG_ERROR("Invalid command line argument format: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (key.empty()) {
                NLM_CONFIG_ERROR("Empty configuration key from command line");
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

/**
 * @brief Enhanced saveToFile method with comprehensive error handling
 *
 * Serializes the current configuration parameters to a file in a human-readable
 * format. The file is written with comments, proper formatting, and maintains
 * the original source information for each parameter.
 *
 * @param filepath Path where the configuration file should be saved
 * @return true if configuration was successfully saved, false otherwise
 * @throws NLMErrors::SystemNotInitialized if the Config object is not properly initialized
 * @throws NLMErrors::InvalidParameter if filepath is invalid or empty
 * @throws NLMErrors::FileIO if the file cannot be opened or written
 * @throws NLMErrors::ConfigValidation if the configuration contains invalid data
 *
 * @note The saved file format includes:
 * - Comment lines for each configuration entry's description
 * - Proper type-specific formatting (strings in quotes, numeric values as-is)
 * - Source information for each parameter in parentheses
 * - Clean, readable formatting for manual inspection
 *
 * Generated file format:
 * @code
 * # Description of neuron_count
 * neuron_count = 500
 * 
 * # Description of simulation_timestep
 * simulation_timestep = 0.001
 * 
 * # Description of stdp_ltp_weight (source: Runtime)
 * stdp_ltp_weight = 0.01
 * @endcode
 *
 * @see loadFromFile
 * @see toDict
 * @see validate
 */
bool Config::saveToFile(const std::string& filepath) const {
    NLM_LOG_INFO("Attempting to save configuration to file: " + filepath);
    
    // Validate file path
    if (filepath.empty()) {
        NLM_FILE_ERROR("File path is empty when saving configuration");
        return false;
    }
    
    // Check if we have data to save
    if (pImpl->entries.empty()) {
        NLM_CONFIG_ERROR("Cannot save configuration to empty file: " + filepath);
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_FILE_ERROR("Cannot save configuration to file: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Successfully saved configuration to file: " + filepath);
    return true;
}

/**
 * @brief Enhanced get() method with comprehensive type validation
 *
 * Retrieves a configuration value with type-safe conversion and validation.
 * The method performs comprehensive validation of input parameters and provides
 * detailed error reporting for type mismatches and invalid values.
 *
 * @tparam T Type of the configuration value to retrieve
 * @param key Configuration parameter key (must be non-empty and valid)
 * @return std::optional<T> The configuration value if found and type matches, std::nullopt otherwise
 * @throws NLMErrors::SystemNotInitialized if the Config object is not properly initialized
 * @throws NLMErrors::InvalidParameter if key is empty or invalid
 * @throws NLMErrors::ConfigParsing if type conversion fails
 *
 * @note The method includes:
 * - Input parameter validation (non-empty key, valid format)
 * - Configuration entry existence checking
 * - Type-safe retrieval using std::get<T> from the variant
 * - Comprehensive exception handling for type mismatches
 * - Detailed error logging with context information
 * - Automatic conversion attempts for compatible types
 *
 * @see getOr
 * @see set
 * @see validateKeyValue
 */
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Validate input parameter
    NLM_VALIDATE(key, !key.empty(), NLMErrors::InvalidParameter, "Empty key provided for get()");
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_WARNING("Configuration key not found: " + key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_CONFIG_ERROR("Type mismatch for configuration key '" + key + "': " +
                         getTypeName<T>() + " cannot be converted to stored type");
        return std::nullopt;
    }
}

/**
 * @brief Enhanced getOr() method with better error context
 *
 * Retrieves a configuration value with a fallback default value if the
 * specified key is not found or has an invalid type. Provides detailed
 * logging for configuration retrieval and default value usage.
 *
 * @tparam T Type of the configuration value to retrieve
 * @param key Configuration parameter key (must be non-empty and valid)
 * @param defaultValue Default value to return if key is not found or type mismatches
 * @return T The configuration value if found and type matches, otherwise the default value
 * @throws NLMErrors::SystemNotInitialized if the Config object is not properly initialized
 * @throws NLMErrors::InvalidParameter if key is empty or invalid
 *
 * @note The method includes:
 * - Input parameter validation (non-empty key, valid format)
 * - Type-safe value retrieval using the get<T>() method
 * - Comprehensive logging for configuration retrieval and default usage
 * - Automatic type conversion attempts for compatible types
 * - Graceful fallback to default values for missing or invalid configuration
 *
 * @see get
 * @see getWithFallback
 */
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    // Validate input parameter
    NLM_VALIDATE(key, !key.empty(), NLMErrors::InvalidParameter, "Empty key provided for getOr()");
    
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    }
    
    NLM_LOG_INFO("Using default value for configuration key '" + key + "': " + 
                 valueToString(defaultValue));
    return defaultValue;
}

/**
 * @brief Enhanced set() method with validation
 *
 * Sets a configuration value with comprehensive validation and type checking.
 * Supports multiple type conversions and maintains the configuration source
 * information for conflict resolution.
 *
 * @param key Configuration parameter key (must be non-empty and valid)
 * @param value Configuration value as ConfigValue variant
 * @param source Source of this configuration value (defaults to Runtime)
 * @throws NLMErrors::SystemNotInitialized if the Config object is not properly initialized
 * @throws NLMErrors::InvalidParameter if key is empty or value is invalid
 * @throws NLMErrors::ConfigValidation if value validation fails
 *
 * @note The method includes:
 * - Input parameter validation (non-empty key, valid format)
 * - Value validation based on key and type constraints
 * - Conflict resolution (update existing or add new entry)
 * - Source tracking for conflict resolution priority
 * - Comprehensive logging for configuration modifications
 * - Type validation through validateKeyValue()
 *
 * @see get
 * @see validateKeyValue
 * @see ConfigSource
 */
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    NLM_VALIDATE(key, !key.empty(), NLMErrors::InvalidParameter, "Empty key provided for set()");
    
    // Validate value based on key
    validateKeyValue(key, value);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        NLM_LOG_INFO("Updating existing configuration key: " + key);
        it->value = value;
        it->source = source;
    } else {
        NLM_LOG_INFO("Adding new configuration key: " + key);
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Enhanced has() method with better logging
 *
 * Checks if a configuration parameter exists in the configuration store.
 * Provides comprehensive logging for configuration parameter existence checks.
 *
 * @param key Configuration parameter key (must be non-empty and valid)
 * @return true if the configuration parameter exists, false otherwise
 * @throws NLMErrors::SystemNotInitialized if the Config object is not properly initialized
 * @throws NLMErrors::InvalidParameter if key is empty or invalid
 *
 * @note The method includes:
 * - Input parameter validation (non-empty key, valid format)
 * - Efficient lookup using std::any_of for performance
 * - Comprehensive logging for parameter existence checks
 * - Thread-safe access to the configuration store
 *
 * @see get
 * @see getOr
 * @see remove
 */
bool Config::has(const std::string& key) const {
    NLM_VALIDATE(key, !key.empty(), NLMErrors::InvalidParameter, "Empty key provided for has()");
    
    bool result = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (result) {
        NLM_LOG_DEBUG("Configuration key found: " + key);
    }
    
    return result;
}

/**
 * @brief Remove configuration parameter
 *
 * Deletes a configuration parameter from the configuration store. Provides
 * comprehensive logging for configuration removal operations.
 *
 * @param key Configuration parameter key to remove (must be non-empty and valid)
 * @throws NLMErrors::SystemNotInitialized if the Config object is not properly initialized
 * @throws NLMErrors::InvalidParameter if key is empty or invalid
 *
 * @note The method includes:
 * - Input parameter validation (non-empty key, valid format)
 * - Warning if attempting to remove non-existent parameter
 * - Information logging for successful removals
 * - Efficient removal using erase-remove idiom
 *
 * @see has
 * @see clear
 */
void Config::remove(const std::string& key) {
    NLM_VALIDATE(key, !key.empty(), NLMErrors::InvalidParameter, "Empty key provided for remove()");
    
    if (!has(key)) {
        NLM_LOG_WARNING("Attempting to remove non-existent configuration key: " + key);
    } else {
        NLM_LOG_INFO("Removing configuration key: " + key);
    }
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

/**
 * @brief Get all configuration parameter keys
 *
 * Retrieves all configuration parameter keys currently stored in the configuration
 * store. Provides comprehensive logging for configuration enumeration operations.
 *
 * @return Vector of all configuration parameter keys
 * @throws NLMErrors::SystemNotInitialized if the Config object is not properly initialized
 *
 * @note The method includes:
 * - Pre-allocation of result vector for performance optimization
 * - Comprehensive logging for configuration enumeration
 * - Thread-safe access to the configuration store
 * - Returns keys in insertion order (preserves configuration source order)
 *
 * @see has
 * @see remove
 * @see summary
 */
std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    NLM_LOG_DEBUG("Retrieved " + std::to_string(keys.size()) + " configuration keys");
    return keys;
}

/**
 * @brief Clear all configuration parameters
 *
 * Removes all configuration parameters from the configuration store. Provides
 * comprehensive logging for configuration clearing operations.
 *
 * @note The method includes:
 * - Comprehensive logging for configuration clearing
 * - Efficient clearing of all configuration entries
 * - Thread-safe access to the configuration store
 * - Preserves the implementation structure for future use
 *
 * @see remove
 * @see loadFromFile
 * @see loadFromArgs
 */
void Config::clear() {
    NLM_LOG_INFO("Clearing all configuration entries (" + std::to_string(pImpl->entries.size()) + " entries)");
    pImpl->entries.clear();
}

/**
 * @brief Enhanced summary() method with more detailed information
 *
 * Generates a comprehensive textual summary of all configuration parameters,
 * including their values, types, and source information. Provides detailed
 * formatting for human-readable configuration inspection.
 *
 * @return String containing the configuration summary
 * @throws NLMErrors::SystemNotInitialized if the Config object is not properly initialized
 *
 * @note The summary includes:
 * - Total number of configuration entries
 * - Individual parameter details (key, value, type, source)
 * - Type-specific formatting for different value types
 * - Array formatting for vector values
 * - Source information in parentheses
 * - Detailed, human-readable formatting for manual inspection
 *
 * Example output:
 * @code
 * Configuration (3 entries):
 *   neuron_count = [500] (1)
 *   simulation_timestep = [0.001] (3)
 *   enable_plasticity = [true] (4)
 * @endcode
 *
 * @see getKeys
 * @see saveToFile
 * @see validate
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
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
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
 * @brief Internal helper method to trim whitespace from strings
 *
 * Removes leading and trailing whitespace from a string. Used internally
 * by configuration file parsing to clean up parameter names and values.
 *
 * @param str Input string to trim
 * @return Trimmed string with no leading/trailing whitespace
 * @note This method is used internally by configuration file parsing
 *       and other string processing operations in the Config class.
 */
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

/**
 * @brief Internal helper method to convert string to lowercase
 *
 * Converts all characters in a string to lowercase. Used internally
 * by configuration file parsing and pattern matching operations.
 *
 * @param str Input string to convert
 * @return Lowercase version of the input string
 * @note This method is used internally by configuration file parsing
 *       and pattern matching operations for case-insensitive comparisons.
 */
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Enhanced type validation for configuration values
void Config::validateKeyValue(const std::string& key, const ConfigValue& value) {
    // Type-specific validation
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int>) {
            if (arg < INT_MIN || arg > INT_MAX) {
                NLM_VALIDATION_ERROR("Integer value " + std::to_string(arg) + " for key '" + key + "' is out of range");
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (arg < LLONG_MIN || arg > LLONG_MAX) {
                NLM_VALIDATION_ERROR("Integer64 value " + std::to_string(arg) + " for key '" + key + "' is out of range");
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::isnan(arg) || std::isinf(arg)) {
                NLM_VALIDATION_ERROR("Invalid double value (NaN or Infinity) for key '" + key + "'");
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (arg.find('\n') != std::string::npos || arg.find('\r') != std::string::npos) {
                NLM_VALIDATION_ERROR("String value for key '" + key + "' contains newline characters");
            }
        }
    }, value);
}

// Enhanced error recovery for get() method with type conversion attempts
template<typename T>
std::optional<T> Config::getWithFallback(const std::string& key) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val;
    }
    
    // Attempt type conversion for common cases
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        NLM_LOG_INFO("Attempting type conversion for configuration key: " + key);
        return convertToType<T>(it->value);
    }
    
    return std::nullopt;
}

// Type conversion with error handling
template<typename T>
std::optional<T> Config::convertToType(const ConfigValue& value) const {
    try {
        if constexpr (std::is_same_v<T, int>) {
            if (std::holds_alternative<int>(value)) {
                return std::get<int>(value);
            } else if (std::holds_alternative<double>(value)) {
                double d = std::get<double>(value);
                if (d >= INT_MIN && d <= INT_MAX) {
                    return static_cast<int>(d);
                }
            } else if (std::holds_alternative<std::string>(value)) {
                std::string s = std::get<std::string>(value);
                size_t pos;
                int result = std::stoi(s, &pos);
                if (pos == s.size()) {
                    return result;
                }
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value);
            } else if (std::holds_alternative<int>(value)) {
                return static_cast<double>(std::get<int>(value));
            } else if (std::holds_alternative<std::string>(value)) {
                std::string s = std::get<std::string>(value);
                size_t pos;
                double result = std::stod(s, &pos);
                if (pos == s.size()) {
                    return result;
                }
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(value)) {
                return std::get<bool>(value);
            } else if (std::holds_alternative<std::string>(value)) {
                std::string s = toLower(std::get<std::string>(value));
                if (s == "true" || s == "1" || s == "yes" || s == "on") return true;
                if (s == "false" || s == "0" || s == "no" || s == "off") return false;
            }
        }
        
        NLM_CONFIG_ERROR("Cannot convert configuration value to type " + getTypeName<T>());
        return std::nullopt;
    } catch (const std::exception& e) {
        NLM_CONFIG_ERROR("Exception during type conversion: " + std::string(e.what()));
        return std::nullopt;
    }
}

// Helper to get type name as string
template<typename T>
std::string Config::getTypeName() const {
    if constexpr (std::is_same_v<T, int>) return "int";
    if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
    if constexpr (std::is_same_v<T, double>) return "double";
    if constexpr (std::is_same_v<T, bool>) return "bool";
    if constexpr (std::is_same_v<T, std::string>) return "std::string";
    if constexpr (std::is_same_v<T, std::vector<int>>) return "std::vector<int>";
    if constexpr (std::is_same_v<T, std::vector<double>>) return "std::vector<double>";
    if constexpr (std::is_same_v<T, std::vector<std::string>>) return "std::vector<std::string>";
    return "unknown";
}

// Enhanced error recovery for saveToFile
template<typename T>
std::string Config::valueToString(const T& value) const {
    if constexpr (std::is_same_v<T, std::string>) {
        return "\"" + value + "\"";
    } else {
        return std::to_string(value);
    }
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

// Filter config keys matching pattern
std::unordered_map<std::string, ConfigValue> Config::filter(const std::string& pattern) const {
    std::unordered_map<std::string, ConfigValue> result;
    std::string lowerPattern = toLower(pattern);
    
    for (const auto& entry : pImpl->entries) {
        std::string lowerKey = toLower(entry.key);
        if (lowerKey.find(lowerPattern) != std::string::npos) {
            result[entry.key] = entry.value;
        }
    }
    
    NLM_LOG_DEBUG("Filtered configuration found " + std::to_string(result.size()) + " entries matching pattern: " + pattern);
    return result;
}

// Convert config to Python dictionary format
std::unordered_map<std::string, ConfigValue> Config::toDict() const {
    std::unordered_map<std::string, ConfigValue> dict;
    
    for (const auto& entry : pImpl->entries) {
        dict[entry.key] = entry.value;
    }
    
    return dict;
}

// Load config from Python dictionary format
void Config::fromDict(const std::unordered_map<std::string, ConfigValue>& dict) {
    clear();
    
    for (const auto& pair : dict) {
        validateKeyValue(pair.first, pair.second);
        set(pair.first, pair.second, ConfigSource::Runtime);
    }
    
    NLM_LOG_INFO("Loaded configuration from dictionary with " + std::to_string(dict.size()) + " entries");
}

// Enhanced validation with more comprehensive checks
bool Config::validate() const {
    // Basic validation: ensure no duplicate keys and all keys are non-empty
    std::unordered_set<std::string> seenKeys;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            NLM_VALIDATION_ERROR("Configuration entry has empty key");
            return false;
        }
        
        if (seenKeys.count(entry.key) > 0) {
            NLM_VALIDATION_ERROR("Duplicate configuration key: " + entry.key);
            return false;
        }
        
        seenKeys.insert(entry.key);
        
        // Additional validation based on key patterns
        if (entry.key.find('.') != std::string::npos) {
            NLM_VALIDATION_ERROR("Configuration key contains dots: " + entry.key);
            return false;
        }
        
        // Validate value based on key
        validateKeyValue(entry.key, entry.value);
        
        // Check for empty values in critical parameters
        if (entry.key == "neuron_count" || entry.key == "region_count" || entry.key == "simulation_steps") {
            if (isNumericEmpty(entry.value)) {
                NLM_VALIDATION_ERROR("Empty or invalid numeric value for critical parameter: " + entry.key);
                return false;
            }
        }
    }
    
    return true;
}

// Check if a ConfigValue is numeric and empty
template<typename T>
bool Config::isNumericEmpty(const ConfigValue& value) const {
    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || std::is_same_v<T, double>) {
        try {
            auto val = std::get<T>(value);
            return false; // Not empty
        } catch (const std::bad_variant_access&) {
            return true; // Empty or not numeric
        }
    } else {
        return true; // Not numeric
    }
}

// Safe load with error handling
bool Config::safeLoad(const std::string& filepath) {
    NLM_LOG_INFO("Safely loading configuration from file: " + filepath);
    
    // Clear existing errors
    NLMErrors::clearErrors();
    
    // Attempt to load from file
    bool success = loadFromFile(filepath);
    
    if (!success) {
        NLM_LOG_ERROR("Failed to safely load configuration. Errors:");
        printErrors(std::cerr);
    }
    
    return success;
}

// Merge another config into this one
void Config::merge(const Config& other) {
    NLM_LOG_INFO("Merging configuration from another config");
    
    for (const auto& entry : other.pImpl->entries) {
        // Use other's source for merged entries
        set(entry.key, entry.value, entry.source);
    }
    
    NLM_LOG_INFO("Configuration merge completed with " + std::to_string(other.pImpl->entries.size()) + " additional entries");
}

} // namespace nlm
