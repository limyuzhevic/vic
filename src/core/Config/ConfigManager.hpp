#ifndef NLM_CONFIG_MANAGER_H
#define NLM_CONFIG_MANAGER_H

#include "NLMConstants.hpp"
#include "Config.hpp"
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

namespace nlm {

class ConfigManagerError : public std::runtime_error {
public:
    explicit ConfigManagerError(const std::string& message) 
        : std::runtime_error("ConfigManager: " + message) {}
};

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager() = default;
    
    // Prevent copying
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    // Singleton pattern
    static ConfigManager& getInstance();
    
    // Configuration management
    void loadFromFile(const std::string& filename);
    void loadFromArgs(int argc, char* argv[]);
    void saveToFile(const std::string& filename) const;
    
    // Configuration access with validation
    template<typename T>
    T get(const std::string& key) const {
        auto it = config_.find(key);
        if (it == config_.end()) {
            throw ConfigManagerError("Configuration key not found: " + key);
        }
        return it->second;
    }
    
    template<typename T>
    T get(const std::string& key, const T& defaultValue) const {
        auto it = config_.find(key);
        if (it == config_.end()) {
            return defaultValue;
        }
        return it->second;
    }
    
    // Configuration modification
    template<typename T>
    void set(const std::string& key, const T& value) {
        validateKey(key);
        validateValue(key, value);
        config_[key] = value;
    }
    
    // Configuration validation
    bool validateConfig() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration enumeration
    std::vector<std::string> getKeys() const;
    bool hasKey(const std::string& key) const;
    
    // Schema management
    void registerSchema(const std::string& key, const std::string& type, 
                       const std::string& description);
    bool validateKey(const std::string& key) const;
    bool validateValue(const std::string& key, const std::string& value) const;
    
    // Serialization
    std::string toJSON() const;
    void fromJSON(const std::string& json);
    
    // Environment variable support
    void loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Configuration templates for common settings
    void setupDefaultBrainConfiguration();
    void setupPhase2Configuration();
    void setupPhase3Configuration();
    void setupPhase4Configuration();
    void setupPhase5Configuration();
    void setupPhase6Configuration();
    
    // Configuration monitoring
    void enableMonitoring(bool enable);
    double getLastValidationTime() const;
    size_t getValidationCount() const;
    
    // Statistics
    size_t getKeyCount() const;
    double getMemoryUsage() const;
    
protected:
    struct ConfigEntry {
        std::string key;
        std::string value;
        std::string type;
        std::string description;
        bool required;
        std::string defaultValue;
        std::vector<std::string> allowedValues;
        double minValue;
        double maxValue;
        bool isValidated;
        
        ConfigEntry() : required(false), isValidated(false), minValue(-std::numeric_limits<double>::max()), 
                       maxValue(std::numeric_limits<double>::max()) {}
    };
    
    std::map<std::string, ConfigEntry> schema_;
    std::map<std::string, std::string> config_;
    bool monitoringEnabled_;
    double lastValidationTime_;
    size_t validationCount_;
    size_t keyCount_;
    
private:
    ConfigManager(const std::string& filename = "");
};

} // namespace nlm

#endif // NLM_CONFIG_MANAGER_H
