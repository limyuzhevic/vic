#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "Config.hpp"
#include "ConfigValidator.hpp"
#include "ConfigLoader.hpp"
#include "ConfigManager.hpp"
#include "ConfigBuilder.hpp"

namespace nlm {

// Forward declarations
class ConfigManager;

// Enhanced configuration system entry point
class ConfigSystem {
public:
    // Initialize with default parameters
    static bool initialize(const std::string& configFile = "configs/default.cfg", 
                          int argc = 0, char** argv = nullptr);
    
    // Get the global configuration manager
    static std::shared_ptr<ConfigManager> getManager() { return manager_; }
    
    // Get the current configuration
    static std::shared_ptr<Config> getConfig() {
        return manager_ ? manager_->getConfig() : nullptr;
    }
    
    // Get the validator
    static std::shared_ptr<ConfigValidator> getValidator() {
        return manager_ ? manager_->getValidator() : nullptr;
    }
    
    // Validate current configuration
    static std::vector<ConfigValidator::ValidationResult> validate() {
        return manager_ ? manager_->validate() : std::vector<ConfigValidator::ValidationResult>();
    }
    
    // Save configuration to file
    static bool saveToFile(const std::string& filepath, const std::string& description = "") {
        return manager_ ? manager_->saveToFile(filepath, description) : false;
    }
    
    // Load configuration from file
    static bool loadFromFile(const std::string& filepath) {
        return manager_ ? manager_->loadFromFile(filepath) : false;
    }
    
    // Create backup
    static bool createBackup(const std::string& description = "Manual backup") {
        return manager_ ? manager_->createBackup(description) : false;
    }
    
    // Restore from backup
    static bool restoreFromBackup(size_t backupIndex = 0) {
        return manager_ ? manager_->restoreFromBackup(backupIndex) : false;
    }
    
    // Reset to defaults
    static bool resetToDefaults() {
        return manager_ ? manager_->resetToDefaults() : false;
    }
    
    // Print help information
    static void printHelp() {
        ConfigLoader::printHelp();
    }
    
    // Initialize with config builder
    static bool initializeWithBuilder(const ConfigBuilder& builder, 
                                      const std::map<std::string, ConfigValue>& customValues = {});
    
    // Export configuration
    static std::string exportToJson() {
        return manager_ && manager_->getConfig() ? 
            nlohmann::json(*manager_->getConfig()).dump(2) : "{}";
    }
    
    // Import configuration
    static bool importFromJson(const std::string& jsonString) {
        return manager_ ? manager_->loadFromString(jsonString, "json") : false;
    }
    
private:
    static std::shared_ptr<ConfigManager> manager_;
    
    // Initialize singleton manager
    static void initializeManager(const std::string& configFile, int argc, char** argv);
};

// Global configuration manager instance
inline std::shared_ptr<ConfigManager> ConfigSystem::manager_ = nullptr;

// Factory function to create enhanced config system
inline std::shared_ptr<ConfigManager> createEnhancedConfigSystem(const std::string& configFile = "configs/default.cfg",
                                                                int argc = 0, char** argv = nullptr) {
    auto manager = std::make_shared<ConfigManager>();
    if (manager->initialize(configFile, argc, argv)) {
        return manager;
    }
    return nullptr;
}

} // namespace nlm