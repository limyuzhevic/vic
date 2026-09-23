// NLM Configuration Helper
// Phase 2: Real Neural Computation
//
// This file provides helper classes for configuration loading and validation

#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

namespace nlm {

/**
 * @class ConfigHelper
 * @brief Helper class for configuration loading, validation, and command-line parsing
 * 
 * This class encapsulates all configuration-related operations including:
 * - Command-line argument parsing
 * - Configuration file loading
 * - Configuration parameter validation
 * - Default configuration setup
 * - Configuration logging
 * 
 * Follows Single Responsibility Principle by focusing solely on configuration management
 */
class ConfigHelper {
public:
    /**
     * @brief Exception thrown when configuration errors occur
     */
    class ConfigError : public std::runtime_error {
    public:
        explicit ConfigError(const std::string& message) : std::runtime_error(message) {}
    };
    
    /**
     * @brief Configuration parameter validation result
     */
    struct ValidationResult {
        bool isValid;
        std::string errorMessage;
        std::vector<std::string> warnings;
        
        ValidationResult() : isValid(true), errorMessage(), warnings() {}
        ValidationResult(bool valid, const std::string& error) 
            : isValid(valid), errorMessage(error), warnings() {}
    };
    
    /**
     * @brief Parse command-line arguments for configuration options
     * 
     * @param argc Number of command-line arguments
     * @param argv Array of command-line argument strings
     * @return Configuration object with parsed values
     * 
     * @throws ConfigError if parsing fails
     */
    static std::shared_ptr<Config> parseCommandLineArgs(int argc, char** argv);
    
    /**
     * @brief Load configuration from file with error handling
     * 
     * @param configFilePath Path to configuration file
     * @param config Configuration object to populate
     * @return bool true if successfully loaded, false otherwise
     */
    static bool loadConfigurationFile(const std::string& configFilePath, 
                                     std::shared_ptr<Config> config);
    
    /**
     * @brief Apply Phase 2 specific default configuration
     * 
     * @param config Configuration object to configure
     */
    static void applyPhase2Defaults(std::shared_ptr<Config> config);
    
    /**
     * @brief Validate configuration parameters for Phase 2 requirements
     * 
     * @param config Configuration to validate
     * @return ValidationResult with validation details
     */
    static ValidationResult validateConfiguration(const std::shared_ptr<Config>& config);
    
    /**
     * @brief Log configuration summary for debugging and verification
     * 
     * @param config Configuration object to log
     */
    static void logConfigurationSummary(const std::shared_ptr<Config>& config);
    
    /**
     * @brief Get the default configuration file path
     * 
     * @return Default configuration file path
     */
    static std::string getDefaultConfigFilePath() {
        return "configs/default.cfg";
    }
    
    /**
     * @brief Get help text for configuration options
     */
    static void printConfigurationHelp();
    
    /**
     * @brief Get configuration source for a specific key
     * 
     * @param config Configuration object
     * @param key Key to look up
     * @return Configuration source, or ConfigSource::Default if not found
     */
    static ConfigSource getSource(const std::shared_ptr<Config>& config, const std::string& key) {
        if (!config) {
            return ConfigSource::Default;
        }
        return config->getSource(key);
    }
    
    /**
     * @brief Get configuration value for a specific key
     * 
     * @param config Configuration object
     * @param key Key to look up
     * @return Configuration value variant, or empty variant if not found
     */
    static ConfigValue getValue(const std::shared_ptr<Config>& config, const std::string& key) {
        if (!config) {
            return ConfigValue();
        }
        return config->getValue(key);
    }
    
private:
    /**
     * @brief Validate a single configuration parameter
     * 
     * @param key Parameter key
     * @param value Parameter value
     * @return ValidationResult for the specific parameter
     */
    static ValidationResult validateParameter(const std::string& key, const ConfigValue& value);
    
    /**
     * @brief Apply command-line overrides to configuration
     * 
     * @param argc Number of command-line arguments
     * @param argv Array of command-line argument strings
     * @param config Configuration object to modify
     */
    static void applyCommandLineOverrides(int argc, char** argv, 
                                          std::shared_ptr<Config> config);
    
    /**
     * @brief Show configuration file loading errors with detailed information
     * 
     * @param configFilePath Path that failed to load
     * @param error Error message from file loading attempt
     */
    static void showConfigurationLoadError(const std::string& configFilePath, 
                                         const std::string& error);
};

} // namespace nlm

// Helper function to trim whitespace (used by ConfigHelper)
inline std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}
