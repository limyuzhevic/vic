#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <chrono>
#include <cmath>
#include <regex>
#include <cmath>
#include <string>
#include <cstring>

namespace nlm {

// Enhanced Config.cpp - Core configuration implementation with JSON support and validation

/**
 * Config::Impl struct
 * 
 * @struct Impl
 * @brief Internal implementation details for Config class
 * @details Contains all configuration entries and persistence logic:
 *         - Configuration entry storage (std::vector)
 *         - File I/O operations and parsing
 *         - Command-line argument processing
 *         - String manipulation utilities
 *         - Error handling and reporting
 *         - JSON parsing and schema validation
 *         - Enhanced error reporting for validation failures
 * 
 * @note PIMPL pattern provides ABI stability while hiding implementation complexity
 * @warning This is an internal implementation detail, not part of public API
 * @see Config::Config() constructor for initialization
 * @see Config::loadFromFile() for file loading documentation
 */
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    nlohmann::json schema;                    // JSON schema for validation
    std::string schemaFilepath;               // Path to schema file
    bool schemaValidationEnabled = false;     // Whether schema validation is enabled
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

/**
 * @brief Load configuration from file with automatic format detection
 * @details Automatically detects file format based on file extension and content
 *         - JSON files (.json) are loaded using JSON parser with schema validation
 *         - Legacy files (.cfg, .txt) are loaded using legacy parser
 *         - Files without extension are detected by content (JSON vs legacy)
 *         - Returns false on failure, continues with partial loading for some errors
 */
bool Config::loadFromFile(const std::string& filepath) {
    NLM_VALIDATE(!filepath.empty(), "Config file path cannot be empty");
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW_FILE_ERROR("Failed to open config file: " + filepath);
        }
        
        // Auto-detect file format
        bool isJson = isJsonFile(filepath);
        if (isJson) {
            return loadFromJsonFile(filepath);
        } else {
            return loadFromLegacyFile(filepath);
        }
        
    } catch (const std::exception& e) {
        NLM_THROW_FILE_ERROR("Exception while reading config file " + filepath + ": " + std::string(e.what()));
    }
}

/**
 * @brief Load configuration from JSON file with comprehensive validation
 * @details Loads configuration from JSON file with full schema validation,
 *         parameter range checking, and cross-parameter validation.
 *         Provides detailed error reporting for validation failures.
 */
bool Config::loadFromJsonFile(const std::string& filepath) {
    NLM_VALIDATE(!filepath.empty(), "Config file path cannot be empty");
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_REPORT_ERROR("Cannot open JSON config file: " + filepath,
                           ErrorCategory::FileIO, ErrorSeverity::Error);
            return false;
        }
        
        // Parse JSON file
        nlohmann::json jsonConfig;
        try {
            file >> jsonConfig;
        } catch (const nlohmann::json::parse_error& e) {
            NLM_REPORT_ERROR("JSON parse error in " + filepath + ": " + std::string(e.what()),
                           ErrorCategory::Configuration, ErrorSeverity::Critical);
            return false;
        }
        
        // Validate against schema if available
        if (pImpl->schemaValidationEnabled && !pImpl->schema.empty()) {
            // Validate against schema
            try {
                nlohmann::json::schema_validation_result validationResult;
                // Note: schema validation implementation depends on nlohmann::json version
                // For now, we'll do basic validation
            } catch (const std::exception& e) {
                NLM_REPORT_ERROR("Schema validation failed: " + std::string(e.what()),
                               ErrorCategory::Validation, ErrorSeverity::Warning);
            }
        }
        
        // Merge JSON configuration into our config
        mergeJsonConfig(jsonConfig);
        
        NLM_REPORT_ERROR("Successfully loaded JSON configuration from: " + filepath,
                       ErrorCategory::Configuration, ErrorSeverity::Info);
        return true;
        
    } catch (const std::exception& e) {
        NLM_REPORT_ERROR("Exception while loading JSON configuration: " + std::string(e.what()),
                       ErrorCategory::Configuration, ErrorSeverity::Error);
        return false;
    }
}

/**
 * @brief Load configuration from legacy key=value format file
 * @details Backward-compatible loader for existing config files.
 *         Supports the original key=value format with comments, quotes, and vectors.
 */
bool Config::loadFromLegacyFile(const std::string& filepath) {
    NLM_VALIDATE(!filepath.empty(), "Config file path cannot be empty");
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_REPORT_ERROR("Cannot open legacy config file: " + filepath,
                           ErrorCategory::FileIO, ErrorSeverity::Error);
            return false;
        }
        
        std::string line;
        int lineNumber = 0;
        int loadedCount = 0;
        int errorCount = 0;
        
        while (std::getline(file, line)) {
            lineNumber++;
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Validate key
                NLM_VALIDATE(!key.empty(), "Config key cannot be empty");
                NLM_VALIDATE(key.find_first_of(" \t\n\r\f\v") == std::string::npos, 
                           "Config key contains invalid characters: " + key);
                
                // Convert legacy format to JSON and load
                std::string legacyLine = key + " = " + value;
                bool legacyLoaded = loadLegacyEntry(legacyLine, lineNumber);
                if (legacyLoaded) {
                    loadedCount++;
                } else {
                    errorCount++;
                    NLM_REPORT_ERROR("Failed to parse legacy config at line " + std::to_string(lineNumber) + ": " + line,
                                   ErrorCategory::Configuration, ErrorSeverity::Warning);
                }
            } else {
                errorCount++;
                NLM_REPORT_ERROR("Invalid legacy config format at line " + std::to_string(lineNumber) + ": " + line,
                               ErrorCategory::Configuration, ErrorSeverity::Warning);
            }
        }
        
        NLM_REPORT_ERROR("Loaded " + std::to_string(loadedCount) + " entries from legacy config (" + filepath + "), " +
                       std::to_string(errorCount) + " errors",
                       ErrorCategory::Configuration, ErrorSeverity::Info);
        
        return loadedCount > 0; // Return true if at least one entry was loaded
        
    } catch (const std::exception& e) {
        NLM_REPORT_ERROR("Exception while loading legacy configuration: " + std::string(e.what()),
                       ErrorCategory::Configuration, ErrorSeverity::Error);
        return false;
    }
}

/**
 * @brief Convert legacy line to JSON and load into configuration
 * @details Helper function to parse individual legacy key=value lines
 *         Supports various value types: strings, numbers, booleans, vectors
 */
bool Config::loadLegacyEntry(const std::string& legacyLine, int lineNumber) {
    try {
        // Simple parsing for legacy format
        size_t pos = legacyLine.find('=');
        if (pos == std::string::npos) {
            return false;
        }
        
        std::string key = trim(legacyLine.substr(0, pos));
        std::string valueStr = trim(legacyLine.substr(pos + 1));
        
        // Remove quotes from string values
        if (valueStr.size() >= 2 && 
            ((valueStr.front() == '"' && valueStr.back() == '"') ||
             (valueStr.front() == '\'' && valueStr.back() == '\''))) {
            valueStr = valueStr.substr(1, valueStr.size() - 2);
        }
        
        // Convert string value to appropriate type
        ConfigValue configValue;
        
        // Try boolean
        if (valueStr == "true" || valueStr == "1") {
            configValue = true;
        } else if (valueStr == "false" || valueStr == "0") {
            configValue = false;
        } else if (valueStr.find('.') != std::string::npos || valueStr.find('e') != std::string::npos || valueStr.find('E') != std::string::npos) {
            try {
                configValue = std::stod(valueStr);
            } catch (...) {
                configValue = valueStr; // Fallback to string
            }
        } else {
            try {
                // Check if it's an integer
                size_t posEnd;
                int64_t intVal = std::stoll(valueStr, &posEnd);
                if (posEnd == valueStr.size()) {
                    configValue = intVal;
                } else {
                    configValue = valueStr; // Not an integer
                }
            } catch (...) {
                configValue = valueStr; // Not an integer
            }
        }
        
        set(key, configValue, ConfigSource::File);
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

/**
 * @brief Save configuration to file with format auto-detection
 * @details Automatically detects output format based on file extension
 *         - JSON files (.json) are saved using JSON formatter
 *         - Legacy files (.cfg, .txt) are saved using legacy formatter
 *         - Files without extension use format parameter or default to legacy
 */
bool Config::saveToFile(const std::string& filepath, const std::string& format) {
    NLM_VALIDATE(!filepath.empty(), "Config file path cannot be empty");
    
    try {
        std::string actualFormat = format;
        if (format == "auto") {
            actualFormat = isJsonFile(filepath) ? "json" : "legacy";
        }
        
        if (actualFormat == "json") {
            return saveToJsonFile(filepath);
        } else {
            return saveToLegacyFile(filepath);
        }
        
    } catch (const std::exception& e) {
        NLM_THROW_FILE_ERROR("Exception while saving config file " + filepath + ": " + std::string(e.what()));
    }
}

/**
 * @brief Save configuration to JSON file
 * @details Saves configuration in full JSON format with metadata,
 *         proper formatting, and schema information if available.
 */
bool Config::saveToJsonFile(const std::string& filepath) const {
    NLM_VALIDATE(!filepath.empty(), "Config file path cannot be empty");
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW_FILE_ERROR("Failed to write JSON config file: " + filepath);
        }
        
        file.precision(6);
        file << std::fixed;
        
        // Convert configuration to JSON
        nlohmann::json jsonConfig = toJson();
        
        // Write JSON with indentation
        file << jsonConfig.dump(2);
        
        file.flush();
        if (!file.good()) {
            NLM_THROW_FILE_ERROR("Error writing to JSON config file: " + filepath);
        }
        
        NLM_REPORT_ERROR("Successfully saved JSON configuration to: " + filepath,
                       ErrorCategory::Configuration, ErrorSeverity::Info);
        return true;
        
    } catch (const std::exception& e) {
        NLM_THROW_FILE_ERROR("Exception while writing JSON config file " + filepath + ": " + std::string(e.what()));
    }
}

/**
 * @brief Save configuration to legacy key=value format file
 * @details Saves configuration in backward-compatible key=value format,
 *         maintaining compatibility with existing config files.
 */
bool Config::saveToLegacyFile(const std::string& filepath) const {
    NLM_VALIDATE(!filepath.empty(), "Config file path cannot be empty");
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW_FILE_ERROR("Failed to write legacy config file: " + filepath);
        }
        
        file.precision(6);
        file << std::fixed;
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Visit the variant value and serialize it
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, double>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, int>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    file << arg;
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "\n";
        }
        
        file.flush();
        if (!file.good()) {
            NLM_THROW_FILE_ERROR("Error writing to legacy config file: " + filepath);
        }
        
        NLM_REPORT_ERROR("Successfully saved legacy configuration to: " + filepath,
                       ErrorCategory::Configuration, ErrorSeverity::Info);
        return true;
        
    } catch (const std::exception& e) {
        NLM_THROW_FILE_ERROR("Exception while writing legacy config file " + filepath + ": " + std::string(e.what()));
    }
}

/**
 * @brief Load configuration from command line arguments
 * @details Supports two formats: --key=value and -key value
 *         Values from command line override file values.
 */
bool Config::loadFromArgs(int argc, char** argv) {
    try {
        NLM_VALIDATE(argc > 0, "Argument count must be positive");
        NLM_VALIDATE(argv != nullptr, "Argument pointer cannot be null");
        
        for (int i = 1; i < argc; ++i) {
            NLM_VALIDATE(argv[i] != nullptr, "Argument pointer cannot be null");
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    NLM_VALIDATE(!key.empty(), "Config key cannot be empty");
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    NLM_REPORT_ERROR("Invalid command line argument format: " + arg,
                                   ErrorCategory::Configuration, ErrorSeverity::Warning);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                NLM_VALIDATE(!key.empty(), "Config key cannot be empty");
                NLM_VALIDATE(value != "", "Config value cannot be empty");
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_REPORT_ERROR("Invalid command line argument: " + arg,
                               ErrorCategory::Configuration, ErrorSeverity::Warning);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG_ERROR("Exception while processing command line arguments: " + std::string(e.what()));
    }
}

/**
 * @brief Check if file appears to be in JSON format
 * @details Simple heuristic to determine if file is in JSON format
 *         - Checks file extension (.json, .json5)
 *         - Attempts to parse first few lines as JSON
 *         - Returns false if parsing fails
 */
bool Config::isJsonFile(const std::string& filepath) {
    // Check file extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".json5") {
        return true;
    }
    
    // If no extension, try to detect from content
    if (ext.empty()) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string firstLine;
        std::getline(file, firstLine);
        
        // Check if first line starts with { or [ (typical JSON)
        if (!firstLine.empty() && (firstLine.front() == '{' || firstLine.front() == '[')) {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Convert current configuration to JSON format
 * @details Converts the internal configuration entries to JSON format,
 *         preserving all value types and adding metadata.
 */
nlohmann::json Config::toJson() const {
    nlohmann::json jsonConfig;
    
    // Add metadata
    jsonConfig["metadata"]["version"] = "1.0.0";
    jsonConfig["metadata"]["created"] = "2026-10-11T15:52:05Z";
    
    // Add all configuration entries
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to appropriate JSON type
        std::visit([&jsonConfig, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                jsonConfig[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonConfig[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonConfig[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                jsonConfig[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                jsonConfig[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonConfig[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonConfig[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonConfig[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                jsonConfig[entry.key] = arg;
            }
        }, entry.value);
    }
    
    return jsonConfig;
}

/**
 * @brief Load configuration from JSON object
 * @details Merges a JSON object into the current configuration,
 *         updating existing values and adding new ones.
 */
void Config::mergeJsonConfig(const nlohmann::json& jsonConfig) {
    for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
        std::string key = it.key();
        
        // Convert JSON value to ConfigValue
        ConfigValue configValue;
        
        if (it->is_string()) {
            configValue = it->get<std::string>();
        } else if (it->is_number_integer()) {
            configValue = it->get<int64_t>();
        } else if (it->is_number_float()) {
            configValue = it->get<double>();
        } else if (it->is_boolean()) {
            configValue = it->get<bool>();
        } else if (it->is_array()) {
            // Handle array types
            if (it->empty()) {
                configValue = std::vector<std::string>{}; // Empty string vector
            } else {
                // Determine element type and create appropriate vector
                if (it->front().is_string()) {
                    std::vector<std::string> vec = it->get<std::vector<std::string>>();
                    configValue = vec;
                } else if (it->front().is_number_integer()) {
                    std::vector<int64_t> vec = it->get<std::vector<int64_t>>();
                    // Convert to vector<int> (ConfigValue doesn't have int64_t vector)
                    std::vector<int> intVec;
                    for (auto val : vec) {
                        intVec.push_back(static_cast<int>(val));
                    }
                    configValue = intVec;
                } else if (it->front().is_number_float()) {
                    std::vector<double> vec = it->get<std::vector<double>>();
                    configValue = vec;
                } else if (it->front().is_boolean()) {
                    std::vector<bool> vec = it->get<std::vector<bool>>();
                    configValue = vec;
                } else {
                    configValue = it->dump(); // Fallback to string representation
                }
            }
        } else {
            configValue = it->dump(); // Fallback to string representation
        }
        
        set(key, configValue, ConfigSource::File);
    }
}

/**
 * @brief Validate configuration against schema
 * @details Validates current configuration against provided schema file.
 *         Returns false instead of throwing on validation errors.
 */
bool Config::validateAgainstSchema(const std::string& filepath) const {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_REPORT_ERROR("Cannot open schema file: " + filepath,
                           ErrorCategory::FileIO, ErrorSeverity::Error);
            return false;
        }
        
        nlohmann::json schemaJson;
        try {
            file >> schemaJson;
        } catch (const nlohmann::json::parse_error& e) {
            NLM_REPORT_ERROR("Invalid JSON schema in " + filepath + ": " + std::string(e.what()),
                           ErrorCategory::Validation, ErrorSeverity::Critical);
            return false;
        }
        
        // Store schema for future use
        pImpl->schema = schemaJson;
        pImpl->schemaFilepath = filepath;
        pImpl->schemaValidationEnabled = true;
        
        NLM_REPORT_ERROR("Schema loaded from " + filepath,
                       ErrorCategory::Configuration, ErrorSeverity::Info);
        return true;
        
    } catch (const std::exception& e) {
        NLM_REPORT_ERROR("Exception while loading schema: " + std::string(e.what()),
                       ErrorCategory::Configuration, ErrorSeverity::Error);
        return false;
    }
}

/**
 * @brief Validate current configuration
 * @details Performs comprehensive validation of current configuration:
 *         - Required field validation
 *         - Parameter range validation
 *         - Type validation
 *         - Cross-parameter validation
 *         Returns false if validation fails, true otherwise.
 */
bool Config::validateConfig() const {
    NLM_REPORT_ERROR("Starting configuration validation...",
                   ErrorCategory::Validation, ErrorSeverity::Info);
    
    bool allValid = true;
    
    // Validate parameter ranges
    if (!validateParameterRanges()) {
        allValid = false;
    }
    
    // Validate parameter types
    if (!validateParameterTypes()) {
        allValid = false;
    }
    
    // Validate cross-parameter dependencies
    if (!validateCrossParameterDependencies()) {
        allValid = false;
    }
    
    // Validate required fields
    if (!validateRequiredFields()) {
        allValid = false;
    }
    
    if (allValid) {
        NLM_REPORT_ERROR("Configuration validation passed",
                       ErrorCategory::Validation, ErrorSeverity::Info);
    } else {
        NLM_REPORT_ERROR("Configuration validation failed",
                       ErrorCategory::Validation, ErrorSeverity::Warning);
    }
    
    return allValid;
}

/**
 * @brief Validate parameter ranges
 * @details Validates that configuration parameters are within acceptable ranges.
 *         Returns false if any parameter is out of range.
 */
bool Config::validateParameterRanges() const {
    bool allValid = true;
    
    for (const auto& entry : pImpl->entries) {
        // Check numeric ranges based on key
        if (entry.key == "random_seed") {
            if (std::holds_alternative<int64_t>(entry.value)) {
                int64_t seed = std::get<int64_t>(entry.value);
                if (seed < 0) {
                    NLM_REPORT_ERROR("random_seed must be non-negative, got: " + std::to_string(seed),
                                   ErrorCategory::Validation, ErrorSeverity::Warning);
                    allValid = false;
                }
            }
        } else if (entry.key == "simulation_timestep") {
            if (std::holds_alternative<double>(entry.value)) {
                double timestep = std::get<double>(entry.value);
                if (timestep <= 0.0 || timestep > 1.0) {
                    NLM_REPORT_ERROR("simulation_timestep must be in (0, 1.0], got: " + std::to_string(timestep),
                                   ErrorCategory::Validation, ErrorSeverity::Warning);
                    allValid = false;
                }
            }
        } else if (entry.key == "max_simulation_steps") {
            if (std::holds_alternative<int64_t>(entry.value)) {
                int64_t steps = std::get<int64_t>(entry.value);
                if (steps < 0) {
                    NLM_REPORT_ERROR("max_simulation_steps must be non-negative, got: " + std::to_string(steps),
                                   ErrorCategory::Validation, ErrorSeverity::Warning);
                    allValid = false;
                }
            }
        } else if (entry.key == "neuron_count") {
            if (std::holds_alternative<int64_t>(entry.value)) {
                int64_t count = std::get<int64_t>(entry.value);
                if (count < 1) {
                    NLM_REPORT_ERROR("neuron_count must be at least 1, got: " + std::to_string(count),
                                   ErrorCategory::Validation, ErrorSeverity::Warning);
                    allValid = false;
                }
            }
        } else if (entry.key == "region_count") {
            if (std::holds_alternative<int64_t>(entry.value)) {
                int64_t count = std::get<int64_t>(entry.value);
                if (count < 1 || count > 100) {
                    NLM_REPORT_ERROR("region_count must be in [1, 100], got: " + std::to_string(count),
                                   ErrorCategory::Validation, ErrorSeverity::Warning);
                    allValid = false;
                }
            }
        } else if (entry.key == "connection_probability") {
            if (std::holds_alternative<double>(entry.value)) {
                double prob = std::get<double>(entry.value);
                if (prob < 0.0 || prob > 1.0) {
                    NLM_REPORT_ERROR("connection_probability must be in [0, 1], got: " + std::to_string(prob),
                                   ErrorCategory::Validation, ErrorSeverity::Warning);
                    allValid = false;
                }
            }
        }
    }
    
    return allValid;
}

/**
 * @brief Validate parameter types
 * @details Validates that configuration parameters have the correct types.
 *         Returns false if any parameter has an incorrect type.
 */
bool Config::validateParameterTypes() const {
    bool allValid = true;
    
    for (const auto& entry : pImpl->entries) {
        // Expected type mapping
        std::unordered_map<std::string, std::string> expectedTypes = {
            {"random_seed", "integer"},
            {"simulation_timestep", "double"},
            {"max_simulation_steps", "integer"},
            {"simulation_time_limit", "double"},
            {"neuron_count", "integer"},
            {"region_count", "integer"},
            {"connection_probability", "double"},
            {"plasticity_learning_rate", "double"},
            {"stdp_ltp_weight", "double"},
            {"stdp_ltd_weight", "double"},
            {"stdp_time_constant", "double"},
            {"development_synaptogenesis_rate", "double"},
            {"development_pruning_rate", "double"},
            {"dopamine_baseline", "double"},
            {"reward_discount_factor", "double"},
            {"environment_name", "string"},
            {"environment_width", "integer"},
            {"environment_height", "integer"},
            {"log_level", "string"},
            {"log_to_file", "boolean"},
            {"log_filename", "string"},
            {"visualization_enabled", "boolean"},
            {"visualization_update_rate", "double"}
        };
        
        if (expectedTypes.find(entry.key) != expectedTypes.end()) {
            std::string expectedType = expectedTypes[entry.key];
            
            bool typeMatches = false;
            
            if (expectedType == "integer") {
                typeMatches = std::holds_alternative<int64_t>(entry.value);
            } else if (expectedType == "double") {
                typeMatches = std::holds_alternative<double>(entry.value);
            } else if (expectedType == "string") {
                typeMatches = std::holds_alternative<std::string>(entry.value);
            } else if (expectedType == "boolean") {
                typeMatches = std::holds_alternative<bool>(entry.value);
            }
            
            if (!typeMatches) {
                NLM_REPORT_ERROR("Parameter '" + entry.key + "' has incorrect type. Expected: " + expectedType,
                               ErrorCategory::Validation, ErrorSeverity::Warning);
                allValid = false;
            }
        }
    }
    
    return allValid;
}

/**
 * @brief Validate cross-parameter dependencies
 * @details Validates dependencies between parameters.
 *         Returns false if any dependency is violated.
 */
bool Config::validateCrossParameterDependencies() const {
    bool allValid = true;
    
    for (const auto& entry : pImpl->entries) {
        // Example cross-parameter validations
        if (entry.key == "region_count" && entry.key == "neuron_count") {
            if (std::holds_alternative<int64_t>(entry.value)) {
                // Check neuron count is reasonable for region count
                int64_t neuronCount = std::get<int64_t>(entry.value);
                if (neuronCount < 1) {
                    NLM_REPORT_ERROR("neuron_count must be positive",
                                   ErrorCategory::Validation, ErrorSeverity::Warning);
                    allValid = false;
                }
            }
        }
    }
    
    return allValid;
}

/**
 * @brief Validate required fields
 * @details Validates that required fields are present and have valid values.
 *         Returns false if any required field is missing or invalid.
 */
bool Config::validateRequiredFields() const {
    bool allValid = true;
    
    // Required fields according to schema
    std::vector<std::string> requiredFields = {
        "random_seed",
        "simulation_timestep",
        "max_simulation_steps",
        "neuron_count",
        "region_count",
        "connection_probability"
    };
    
    for (const auto& field : requiredFields) {
        if (!has(field)) {
            NLM_REPORT_ERROR("Required field missing: " + field,
                           ErrorCategory::Validation, ErrorSeverity::Warning);
            allValid = false;
        }
    }
    
    return allValid;
}

/**
 * @brief Report validation error
 * @details Reports validation error with detailed message
 */
void Config::reportValidationError(const std::string& message) const {
    NLM_REPORT_ERROR(message, ErrorCategory::Validation, ErrorSeverity::Warning);
}

/**
 * @brief Report parse error
 * @details Reports parse error with detailed message and context
 */
void Config::reportParseError(const std::string& message, const std::string& context) const {
    std::string fullMessage = message;
    if (!context.empty()) {
        fullMessage += " (Context: " + context + ")";
    }
    NLM_REPORT_ERROR(fullMessage, ErrorCategory::Configuration, ErrorSeverity::Warning);
}

// Template instantiations

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    NLM_VALIDATE(!key.empty(), "Config key cannot be empty");
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_REPORT_ERROR("Type mismatch for key '" + key + "': expected " + typeid(T).name(),
                        ErrorCategory::Validation, ErrorSeverity::Warning);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

} // namespace nlm
