#ifndef NLM_CONFIG_MANAGER_HPP
#define NLM_CONFIG_MANAGER_HPP

#include "core/Config/Config.hpp"
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace nlm {

/**
 * @brief Central configuration management system for NLM
 * 
 * Provides a unified interface for configuration across all NLM components
 * with validation, documentation, and environment-specific settings.
 */
class ConfigManager {
public:
    /**
     * @brief Get the singleton instance
     * 
     * @return Reference to the ConfigManager instance
     */
    static ConfigManager& getInstance();
    
    /**
     * @brief Initialize the configuration system
     * 
     * @param configFile Path to configuration file (optional)
     * @param cliArgs Command line arguments (argc, argv)
     */
    void initialize(const std::string& configFile = "", int argc = 0, char** argv = nullptr);
    
    /**
     * @brief Get the configuration instance
     * 
     * @return Shared pointer to the Config instance
     */
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    /**
     * @brief Set a configuration value with validation
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param description Human-readable description of the parameter
     * @return bool true if valid and set, false otherwise
     */
    bool setValidated(const std::string& key, const std::string& value, 
                     const std::string& description = "");
    
    /**
     * @brief Get a configuration value with type safety
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Retrieved value
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        return config_->getOr<T>(key, defaultValue);
    }
    
    /**
     * @brief Check if a configuration key exists
     * 
     * @param key Configuration key
     */
    bool has(const std::string& key) const { return config_->has(key); }
    
    /**
     * @brief Get all configuration keys
     * 
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getAllKeys() const { return config_->getKeys(); }
    
    /**
     * @brief Validate all configuration values
     * 
     * @return bool true if all values are valid, false otherwise
     */
    bool validateAll() const;
    
    /**
     * @brief Get configuration summary for logging
     * 
     * @return String summary of configuration
     */
    std::string getSummary() const { return config_->summary(); }
    
    /**
     * @brief Load default configuration for NLM phases
     * 
     * @param phase Phase number (2-6)
     */
    void loadPhaseDefaults(int phase);
    
    /**
     * @brief Save configuration to file
     * 
     * @param filepath Path to save configuration
     * @return bool true if saved successfully, false otherwise
     */
    bool saveToFile(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    /**
     * @brief Reset configuration to defaults
     */
    void reset() { config_->clear(); }

private:
    ConfigManager() : config_(std::make_shared<Config>()) {}
    ~ConfigManager() = default;
    
    // Disable copying and moving
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    ConfigManager(ConfigManager&&) noexcept = delete;
    ConfigManager& operator=(ConfigManager&&) noexcept = delete;
    
    // Configuration schemas for validation
    struct ConfigSchema {
        std::string type; // "int", "double", "bool", "string", "vector<int>", etc.
        std::string description;
        std::string defaultValue;
        std::map<std::string, std::string> constraints; // min, max, allowed_values
    };
    
    static std::map<std::string, ConfigSchema> createConfigurationSchema();
    
    std::shared_ptr<Config> config_;
    static ConfigManager instance_;
};

} // namespace nlm

#endif // NLM_CONFIG_MANAGER_HPP
