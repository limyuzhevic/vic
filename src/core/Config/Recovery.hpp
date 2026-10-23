#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <stdexcept>
#include <sstream>

namespace nlm {

/**
 * @brief Exception thrown when configuration operations fail
 * 
 * This exception provides detailed information about configuration operation
 * failures, including error messages and context about what went wrong.
 */
class ConfigOperationError : public std::runtime_error {
public:
    ConfigOperationError(const std::string& message, const std::string& context = "")
        : std::runtime_error(message), context(context) {}
    
    std::string getContext() const { return context; }
    
private:
    std::string context;
};

/**
 * @brief Configuration recovery operation for handling errors
 * 
 * ConfigRecovery provides mechanisms for handling configuration errors
 * and recovering from problematic states. It includes error handling,
 * fallback mechanisms, and validation strategies.
 */
class ConfigRecovery {
public:
    ConfigRecovery();
    ~ConfigRecovery();
    
    // Disable copying, enable moving
    ConfigRecovery(const ConfigRecovery&) = delete;
    ConfigRecovery& operator=(const ConfigRecovery&) = delete;
    ConfigRecovery(ConfigRecovery&&) noexcept;
    ConfigRecovery& operator=(ConfigRecovery&&) noexcept;
    
    /**
     * @brief Set error handler for configuration operations
     * 
     * @param handler Function that handles configuration errors
     */
    void setErrorHandler(std::function<void(const ConfigOperationError&)> handler);
    
    /**
     * @brief Get current error handler
     * 
     * @return Current error handler function
     */
    std::function<void(const ConfigOperationError&)> getErrorHandler() const;
    
    /**
     * @brief Enable automatic recovery from configuration errors
     * 
     * @param enable Whether to enable automatic recovery
     */
    void setAutoRecovery(bool enable);
    
    /**
     * @brief Check if automatic recovery is enabled
     * 
     * @return true if automatic recovery is enabled, false otherwise
     */
    bool isAutoRecoveryEnabled() const;
    
    /**
     * @brief Handle configuration error with recovery
     * 
     * @param error Configuration error to handle
     * @return true if error was handled successfully, false otherwise
     */
    bool handleError(const ConfigOperationError& error);
    
    /**
     * @brief Validate configuration and recover from errors
     * 
     * @param config Configuration to validate and recover
     * @return true if configuration is valid or was successfully recovered, false otherwise
     */
    bool validateAndRecover(class Config& config);
    
    /**
     * @brief Get recovery statistics
     * 
     * @return String containing recovery statistics
     */
    std::string getStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Configuration backup utility for saving and restoring states
 * 
 * ConfigBackup provides functionality to create backups of configuration
 * states and restore from backups when needed. This is useful for recovery
 * from corrupted configurations or for rolling back changes.
 */
class ConfigBackup {
public:
    ConfigBackup();
    ~ConfigBackup();
    
    // Disable copying, enable moving
    ConfigBackup(const ConfigBackup&) = delete;
    ConfigBackup& operator=(const ConfigBackup&) = delete;
    ConfigBackup(ConfigBackup&&) noexcept;
    ConfigBackup& operator=(ConfigBackup&&) noexcept;
    
    /**
     * @brief Create a backup of the current configuration
     * 
     * @param config Configuration to backup
     * @param label Label for the backup
     */
    void createBackup(class Config& config, const std::string& label);
    
    /**
     * @brief Restore configuration from backup
     * 
     * @param config Configuration to restore
     * @param label Label of the backup to restore
     * @return true if backup was found and restored successfully, false otherwise
     */
    bool restoreBackup(class Config& config, const std::string& label);
    
    /**
     * @brief Remove backup by label
     * 
     * @param label Label of the backup to remove
     */
    void removeBackup(const std::string& label);
    
    /**
     * @brief List all available backups
     * 
     * @return Vector of backup labels
     */
    std::vector<std::string> listBackups() const;
    
    /**
     * @brief Clear all backups
     */
    void clearBackups();
    
    /**
     * @brief Get backup information
     * 
     * @param label Label of the backup
     * @return String containing backup information
     */
    std::string getBackupInfo(const std::string& label) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Configuration validator for comprehensive validation
 * 
 * ConfigValidator provides comprehensive validation of configuration
 * parameters including type checking, constraint validation, and
 * cross-parameter validation. It also provides suggestions for fixing
 * validation errors.
 */
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Disable copying, enable moving
    ConfigValidator(const ConfigValidator&) = delete;
    ConfigValidator& operator=(const ConfigValidator&) = delete;
    ConfigValidator(ConfigValidator&&) noexcept;
    ConfigValidator& operator=(ConfigValidator&&) noexcept;
    
    /**
     * @brief Validate configuration with detailed error reporting
     * 
     * @param config Configuration to validate
     * @return Vector of validation errors (empty if valid)
     */
    std::vector<ConfigOperationError> validate(class Config& config);
    
    /**
     * @brief Validate configuration parameter
     * 
     * @param config Configuration to validate
     * @param key Parameter key to validate
     * @param value Parameter value to validate
     * @return Vector of validation errors for this parameter
     */
    std::vector<ConfigOperationError> validateParameter(class Config& config, const std::string& key, const ConfigValue& value);
    
    /**
     * @brief Get validation suggestions for a parameter
     * 
     * @param config Configuration to validate
     * @param key Parameter key
     * @param value Parameter value
     * @return Vector of suggestions for fixing validation errors
     */
    std::vector<std::string> getValidationSuggestions(class Config& config, const std::string& key, const ConfigValue& value);
    
    /**
     * @brief Set validation strictness level
     * 
     * @param strict Whether to use strict validation (true) or lenient validation (false)
     */
    void setStrictValidation(bool strict);
    
    /**
     * @brief Check if strict validation is enabled
     * 
     * @return true if strict validation is enabled, false otherwise
     */
    bool isStrictValidation() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
