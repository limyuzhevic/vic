#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <stdexcept>

namespace nlm {

/**
 * Advanced configuration manager with support for multiple formats
 * and validation capabilities
 */
class AdvancedConfigManager {
public:
    AdvancedConfigManager() = default;
    ~AdvancedConfigManager() = default;
    
    /**
     * Load configuration from file (.cfg or .json format)
     * 
     * @param filepath Path to configuration file
     * @param validate If true, validates configuration after loading
     * @return true if loading succeeded
     */
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    /**
     * Save configuration to file
     * 
     * @param filepath Output file path
     * @param format Output format ("cfg" or "json")
     * @return true if saving succeeded
     */
    bool saveToFile(const std::string& filepath, const std::string& format = "cfg") const;
    
    /**
     * Merge command-line arguments into configuration
     * 
     * @param argc Command line argument count
     * @param argv Command line argument values
     * @param prefix Prefix for config keys
     */
    void applyCommandLineArgs(int argc, char* argv[], const std::string& prefix = "");
    
    /**
     * Export configuration for use by other components
     * 
     * @param configName Name of the config section
     * @return Map of configuration values
     */
    std::map<std::string, std::string> exportConfig(const std::string& configName = "") const;
    
    /**
     * Import configuration from another source
     * 
     * @param configData Configuration data map
     * @param configName Name of the config section to import
     */
    void importConfig(const std::map<std::string, std::string>& configData, const std::string& configName = "");
    
    /**
     * Validate configuration against rules
     * 
     * @return List of validation errors, empty if valid
     */
    std::vector<std::string> validate() const;
    
    /**
     * Get the underlying config object
     * 
     * @return Reference to internal Config object
     */
    const std::shared_ptr<Config>& getConfig() const { return config_; }
    
    /**
     * Set the underlying config object
     * 
     * @param config Config object to use
     */
    void setConfig(const std::shared_ptr<Config>& config) { config_ = config; }
    
    /**
     * Get configuration value with type conversion
     * 
     * @tparam T Type to convert to
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value
     */
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue = T()) const {
        return config_->getOr<T>(key, defaultValue);
    }
    
    /**
     * Set configuration value
     * 
     * @tparam T Type of value
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of configuration
     */
    template<typename T>
    void setValue(const std::string& key, const T& value, const std::string& source = "user") {
        config_->set(key, value, ConfigSource::fromString(source));
    }
    
    /**
     * Check if configuration key exists
     * 
     * @param key Configuration key
     * @return true if key exists
     */
    bool hasKey(const std::string& key) const {
        return config_->has(key);
    }
    
    /**
     * Get all configuration keys
     * 
     * @return Vector of configuration keys
     */
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    /**
     * Create a backup of current configuration
     * 
     * @param backupPath Path to backup file
     * @return true if backup created successfully
     */
    bool createBackup(const std::string& backupPath) const;
    
    /**
     * Restore configuration from backup
     * 
     * @param backupPath Path to backup file
     * @return true if restoration succeeded
     */
    bool restoreFromBackup(const std::string& backupPath);
    
    /**
     * Get configuration statistics
     * 
     * @return Map containing configuration statistics
     */
    std::map<std::string, size_t> getStats() const;
    
    /**
     * Clear all configuration
     */
    void clear() {
        config_->clear();
    }
    
    /**
     * Get configuration format (based on file extension)
     * 
     * @param filepath File path to analyze
     * @return Format string ("cfg", "json", or "unknown")
     */
    static std::string getFormatFromFile(const std::string& filepath);

private:
    std::shared_ptr<Config> config_ = std::make_shared<Config>();
    
    /**
     * Load .cfg format file
     * 
     * @param filepath File path
     * @param config Config object to populate
     * @return true if successful
     */
    bool loadCfgFormat(const std::string& filepath, const std::shared_ptr<Config>& config);
    
    /**
     * Save .cfg format file
     * 
     * @param filepath File path
     * @param config Config object to save
     * @return true if successful
     */
    bool saveCfgFormat(const std::string& filepath, const std::shared_ptr<Config>& config) const;
    
    /**
     * Load .json format file
     * 
     * @param filepath File path
     * @param config Config object to populate
     * @return true if successful
     */
    bool loadJsonFormat(const std::string& filepath, const std::shared_ptr<Config>& config);
    
    /**
     * Save .json format file
     * 
     * @param filepath File path
     * @param config Config object to save
     * @return true if successful
     */
    bool saveJsonFormat(const std::string& filepath, const std::shared_ptr<Config>& config) const;
    
    /**
     * Parse command line argument string
     * 
     * @param arg Argument string (e.g., "--neuron_count=500")
     * @param prefix Prefix for config keys
     */
    void parseCommandLineArg(const std::string& arg, const std::string& prefix = "");
    
    /**
     * Convert string to appropriate type
     * 
     * @tparam T Type to convert to
     * @param value String value
     * @param defaultValue Default value if conversion fails
     * @return Converted value
     */
    template<typename T>
    T stringToValue(const std::string& value, const T& defaultValue = T()) const;
};

} // namespace nlm
