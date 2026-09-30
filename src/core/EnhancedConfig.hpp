// Configuration validation and preset system
#ifndef NLM_ENHANCED_CONFIG_HPP
#define NLM_ENHANCED_CONFIG_HPP

#include "core/Config/Config.hpp"
#include <map>
#include <functional>
#include <string>
#include <vector>
#include <unordered_set>

namespace nlm {

// Configuration validation rules
struct ValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    bool required = false;
    std::string category;
};

// Preset configurations for different use cases
struct PresetConfig {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> values;
    std::vector<std::string> tags;
};

// Configuration schema definition
struct ConfigSchema {
    std::string name;
    std::string description;
    std::vector<ValidationRule> validationRules;
    std::vector<PresetConfig> presets;
    
    // Default values for all configurable keys
    std::map<std::string, ConfigValue> defaults;
    
    // Configuration categories for better organization
    std::vector<std::string> categories;
};

// Enhanced configuration manager with validation and presets
class EnhancedConfigManager {
public:
    EnhancedConfigManager();
    ~EnhancedConfigManager();
    
    // Load and validate configuration
    bool loadConfig(const std::string& filepath);
    
    // Apply preset configurations
    bool applyPreset(const std::string& presetName);
    bool applyPreset(const PresetConfig& preset);
    
    // Get all available presets
    std::vector<std::string> getAvailablePresets() const;
    std::vector<PresetConfig> getPresetsByTag(const std::string& tag) const;
    
    // Validate current configuration
    std::vector<std::string> validate() const;
    bool isValid() const;
    
    // Get configuration suggestions for fixing issues
    std::vector<std::string> getValidationSuggestions() const;
    
    // Runtime configuration updates
    void enableRuntimeUpdates(bool enable);
    bool isRuntimeUpdatesEnabled() const;
    
    // Configuration backup and restore
    bool backupConfig(const std::string& backupPath);
    bool restoreConfig(const std::string& backupPath);
    
    // Get configuration summary with validation info
    std::string getDetailedSummary() const;
    
    // Get the underlying config
    std::shared_ptr<Config> getConfig() const;
    
    // Set validation callback for custom validation logic
    void setValidationCallback(std::function<bool(const std::string&, const ConfigValue&)> callback);
    
    // Configuration comparison for debugging
    std::string diffWith(const EnhancedConfigManager& other) const;
    
    // Export configuration
    bool exportConfig(const std::string& filepath, const std::string& format = "text") const;
    bool importConfig(const std::string& filepath, const std::string& format = "text");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Initialize with default schema
    void initializeDefaultSchema();
    
    // Apply validation rules
    void applyValidationRules(const ConfigSchema& schema) const;
    
    // Check if value meets validation rule
    bool validateValue(const ValidationRule& rule, const ConfigValue& value) const;
    
    // Get error message for validation failure
    std::string getValidationError(const ValidationRule& rule, const ConfigValue& value) const;
};

// Pre-built configuration schemas
namespace ConfigSchemas {
    
    // Standard NLM configuration schema
    ConfigSchema getStandardSchema();
    
    // Development configuration schema
    ConfigSchema getDevelopmentSchema();
    
    // Production configuration schema
    ConfigSchema getProductionSchema();
    
    // Performance testing configuration schema
    ConfigSchema getPerformanceSchema();
    
    // Research/experiment configuration schema
    ConfigSchema getResearchSchema();
}

// Configuration presets
namespace ConfigPresets {
    
    // Standard presets
    PresetConfig getQuickStartPreset();
    PresetConfig getMinimalPreset();
    PresetConfig getFullPreset();
    PresetConfig getResearchPreset();
    
    // Performance presets
    PresetConfig getPerformancePreset();
    PresetConfig getMemoryEfficientPreset();
    PresetConfig getSpeedOptimizedPreset();
    
    // Development presets
    PresetConfig getDebugPreset();
    PresetConfig getValidationPreset();
    
    // Research-specific presets
    PresetConfig getLongRunningPreset();
    PresetConfig getHighPrecisionPreset();
    PresetConfig getLargeScalePreset();
}

// Configuration validation utilities
namespace ValidationUtils {
    
    // Range validation
    bool validateIntRange(const ConfigValue& value, int min, int max);
    bool validateDoubleRange(const ConfigValue& value, double min, double max);
    
    // String validation
    bool validateStringLength(const ConfigValue& value, size_t minLength, size_t maxLength);
    bool validateStringPattern(const ConfigValue& value, const std::string& pattern);
    
    // Collection validation
    bool validateVectorSize(const ConfigValue& value, size_t minSize, size_t maxSize);
    
    // Dependency validation
    bool validateDependency(const std::map<std::string, ConfigValue>& config, 
                          const std::vector<std::pair<std::string, std::string>>& dependencies);
    
    // File path validation
    bool validateFilePath(const ConfigValue& value, bool mustExist = false);
    
    // Port validation
    bool validatePort(const ConfigValue& value);
}

} // namespace nlm

#endif // NLM_ENHANCED_CONFIG_HPP