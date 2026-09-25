#pragma once

#include <string>
#include <memory>
#include <map>
#include <variant>
#include <vector>
#include <functional>
#include "Config.hpp"
#include "ConfigValidator.hpp"
#include "ConfigLoader.hpp"

namespace nlm {

class ConfigManager {
public:
    // Config version for compatibility
    struct ConfigVersion {
        int major;
        int minor;
        int patch;
        
        ConfigVersion() : major(1), minor(0), patch(0) {}
        ConfigVersion(int maj, int min, int patch) : major(maj), minor(min), patch(patch) {}
        
        bool operator==(const ConfigVersion& other) const {
            return major == other.major && minor == other.minor && patch == other.patch;
        }
        
        bool operator!=(const ConfigVersion& other) const {
            return !(*this == other);
        }
    };
    
    // Config backup information
    struct ConfigBackup {
        std::string timestamp;
        std::string description;
        std::string filepath;
        ConfigVersion version;
        
        ConfigBackup() = default;
        ConfigBackup(const std::string& ts, const std::string& desc, const std::string& fp)
            : timestamp(ts), description(desc), filepath(fp), version(1, 0, 0) {}
    };
    
    ConfigManager();
    ~ConfigManager() = default;
    
    // Initialize with configuration source (file, command line, defaults)
    bool initialize(const std::string& configFile = "configs/default.cfg", int argc = 0, char** argv = nullptr);
    
    // Get the current configuration
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    // Get validator reference
    std::shared_ptr<ConfigValidator> getValidator() const { return validator_; }
    
    // Validate current configuration
    std::vector<ConfigValidator::ValidationResult> validate() const;
    
    // Save current configuration to file
    bool saveToFile(const std::string& filepath, const std::string& description = "") const;
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from string (JSON/YAML)
    bool loadFromString(const std::string& configString, const std::string& format = "auto");
    
    // Create backup of current configuration
    bool createBackup(const std::string& description = "Manual backup");
    
    // Restore configuration from backup
    bool restoreFromBackup(size_t backupIndex = 0);
    
    // Get backup list
    std::vector<ConfigBackup> getBackups() const { return backups_; }
    
    // Clear current configuration
    void clear() { if (config_) config_->clear(); }
    
    // Reset to default configuration
    bool resetToDefaults();
    
    // Merge configuration from another source (file, string, etc.)
    bool mergeFromFile(const std::string& filepath);
    
    // Get configuration summary for logging
    std::string getSummary() const;
    
    // Set custom configuration directory
    void setConfigDirectory(const std::string& directory) { configDirectory_ = directory; }
    
    // Get configuration directory
    const std::string& getConfigDirectory() const { return configDirectory_; }
    
    // Get configuration version
    ConfigVersion getVersion() const { return version_; }
    
    // Set configuration version
    void setVersion(const ConfigVersion& version) { version_ = version; }
    
    // Check if configuration has been modified
    bool isModified() const { return modified_; }
    
    // Get modification timestamp
    const std::string& getLastModified() const { return lastModified_; }
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<ConfigValidator> validator_;
    std::shared_ptr<ConfigLoader> loader_;
    
    // Backup management
    std::vector<ConfigBackup> backups_;
    
    // Configuration metadata
    std::string configDirectory_ = "configs";
    ConfigVersion version_ = ConfigVersion(1, 0, 0);
    bool modified_ = false;
    std::string lastModified_;
    
    // Helper methods
    std::string generateTimestamp() const;
    bool loadDefaultConfig();
    bool loadDefaultValues();
    void updateLastModified();
};

} // namespace nlm