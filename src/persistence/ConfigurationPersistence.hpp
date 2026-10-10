// Configuration persistence - configuration persistence
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <mutex>
#include "../core/Config/Config.hpp"

namespace nlm {

// Configuration persistence result
struct ConfigurationPersistenceResult {
    bool success;
    std::string message;
    uint64_t totalKeys;
    uint64_t totalSize;
    double elapsedTime;
    time_t timestamp;
    
    ConfigurationPersistenceResult() : success(false), totalKeys(0), totalSize(0),
                                       elapsedTime(0.0), timestamp(0) {}
};

// Configuration persistence options
struct ConfigurationPersistenceOptions {
    std::string outputPath;
    std::string format; // json, binary, xml
    bool compress;
    bool encrypt;
    std::vector<std::string> keyPaths; // Specific configuration keys to save
    bool validateAfterSave;
    std::chrono::seconds timeout;
    
    ConfigurationPersistenceOptions() : format("json"), compress(true), encrypt(false),
                                        validateAfterSave(true),
                                        timeout(std::chrono::seconds(30)) {}
};

// Configuration persistence statistics
struct ConfigurationPersistenceStats {
    uint64_t configurationsSaved;
    uint64_t configurationsLoaded;
    uint64_t configurationsExported;
    uint64_t configurationsImported;
    uint64_t totalKeysSaved;
    uint64_t totalSizeSaved;
    double totalTimeSpent;
    uint32_t errorCount;
    
    ConfigurationPersistenceStats() : configurationsSaved(0), configurationsLoaded(0),
                                     configurationsExported(0), configurationsImported(0),
                                     totalKeysSaved(0), totalSizeSaved(0),
                                     totalTimeSpent(0.0), errorCount(0) {}
};

// Main configuration persistence class
class ConfigurationPersistence {
public:
    ConfigurationPersistence();
    ~ConfigurationPersistence();
    
    // Configuration
    bool configure(const std::shared_ptr<Config>& config);
    void shutdown();
    
    // Persistence operations
    bool saveConfiguration(const std::string& name, const ConfigurationPersistenceOptions& options = ConfigurationPersistenceOptions());
    bool loadConfiguration(const std::string& name);
    bool exportToJSON(const std::string& path);
    bool importFromJSON(const std::string& path);
    
    // Configuration management
    bool saveKey(const std::string& name, const std::string& key, const std::string& value);
    std::string loadKey(const std::string& name, const std::string& key);
    bool deleteKey(const std::string& name, const std::string& key);
    std::vector<std::string> listKeys(const std::string& name);
    
    // Configuration system integration
    void setConfig(std::shared_ptr<Config> config) {
        config_ = config;
    }
    
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    // Statistics and monitoring
    ConfigurationPersistenceStats getStats() const { return stats_; }
    
    bool isHealthy() const { return errorCount_ == 0; }
    
    // Event handling
    using ConfigurationEventCallback = std::function<void(const std::string&, const std::string&, const ConfigurationPersistenceResult&)>;
    void setConfigurationEventCallback(ConfigurationEventCallback callback) { 
        configurationEventCallback_ = callback; 
    }
    
    // Validation
    bool validateConfiguration() const;
    bool validateConfigurationIntegrity() const;
    std::vector<std::string> getValidationErrors() const { return validationErrors_; }
    
    // Thread safety
    void lock() const { mutex_.lock(); }
    void unlock() const { mutex_.unlock(); }
    bool tryLock() const { return mutex_.try_lock(); }

private:
    bool initialize();
    void cleanup();
    
    ConfigurationPersistenceResult executeConfigurationOperation(const std::string& operation,
                                                                 const std::string& name,
                                                                 const ConfigurationPersistenceOptions& options);
    
    void updateStats(const ConfigurationPersistenceResult& result);
    void logEvent(const std::string& operation, const std::string& name,
                  const ConfigurationPersistenceResult& result);
    
    std::string getConfigurationPath(const std::string& name) const;
    
    // Component references
    std::shared_ptr<Config> config_;
    
    // Configuration
    std::string configDir_;
    
    // State
    mutable std::mutex mutex_;
    uint32_t errorCount_;
    
    // Statistics
    ConfigurationPersistenceStats stats_;
    
    // Events
    ConfigurationEventCallback configurationEventCallback_;
    
    // Validation
    std::vector<std::string> validationErrors_;
};

} // namespace nlm
