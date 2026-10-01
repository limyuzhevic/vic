// Advanced configuration options and presets for NLM

#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <fstream>
#include <sstream>

namespace nlm {

// Configuration preset categories
enum class ConfigPresetCategory {
    Development,
    Performance,
    Research,
    Educational,
    Custom,
    Standard
};

// Configuration preset
struct ConfigPreset {
    std::string name;                          // Preset name
    std::string description;                   // Preset description
    ConfigPresetCategory category;              // Preset category
    double version;                             // Preset version
    std::map<std::string, std::string> settings; // Configuration settings
    std::map<std::string, std::string> notes;    // Additional notes
    
    ConfigPreset(const std::string& name, const std::string& description,
                 ConfigPresetCategory category = ConfigPresetCategory::Standard)
        : name(name), description(description), category(category), 
          version(1.0), notes() {}
    
    // Add setting
    void addSetting(const std::string& key, const std::string& value, 
                   const std::string& note = "") {
        settings[key] = value;
        if (!note.empty()) {
            notes[key] = note;
        }
    }
    
    // Get setting with default
    std::string getSetting(const std::string& key, 
                          const std::string& defaultValue = "") const {
        auto it = settings.find(key);
        return it != settings.end() ? it->second : defaultValue;
    }
    
    // Check if setting exists
    bool hasSetting(const std::string& key) const {
        return settings.find(key) != settings.end();
    }
    
    // Get all settings
    std::vector<std::string> getAllKeys() const {
        std::vector<std::string> keys;
        keys.reserve(settings.size());
        for (const auto& pair : settings) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // Export to string
    std::string toString() const {
        std::ostringstream oss;
        oss << "Preset: " << name << "\n";
        oss << "Description: " << description << "\n";
        oss << "Category: " << static_cast<int>(category) << "\n";
        oss << "Version: " << version << "\n";
        oss << "\nSettings:\n";
        for (const auto& pair : settings) {
            oss << "  " << pair.first << " = \"" << pair.second << "\"";
            if (notes.find(pair.first) != notes.end()) {
                oss << "  # " << notes.at(pair.first);
            }
            oss << "\n";
        }
        return oss.str();
    }
};

// Advanced configuration manager
class AdvancedConfigManager {
public:
    AdvancedConfigManager();
    ~AdvancedConfigManager();
    
    // Load presets from file
    bool loadPresets(const std::string& filepath);
    
    // Save presets to file
    bool savePresets(const std::string& filepath) const;
    
    // Add preset
    void addPreset(const ConfigPreset& preset);
    
    // Get preset
    ConfigPreset getPreset(const std::string& name) const;
    
    // Check if preset exists
    bool hasPreset(const std::string& name) const;
    
    // Remove preset
    void removePreset(const std::string& name);
    
    // Get all preset names
    std::vector<std::string> getPresetNames() const;
    
    // Get presets by category
    std::vector<ConfigPreset> getPresetsByCategory(ConfigPresetCategory category) const;
    
    // Create configuration from preset
    std::shared_ptr<class Config> createConfigFromPreset(const std::string& presetName) const;
    
    // Merge configuration with preset
    void mergeWithPreset(std::shared_ptr<class Config> config, const std::string& presetName) const;
    
    // Compare two presets
    std::map<std::string, std::pair<std::string, std::string>> comparePresets(
        const std::string& preset1, const std::string& preset2) const;
    
    // Get preset differences
    std::vector<std::string> getPresetDifferences(const std::string& preset1,
                                                  const std::string& preset2) const;
    
    // Validate preset
    bool validatePreset(const ConfigPreset& preset) const;
    
    // Generate preset name
    std::string generatePresetName(const std::string& baseName) const;
    
    // Export all presets to string
    std::string exportAllPresets() const;
    
    // Import presets from string
    bool importPresets(const std::string& presetsString);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration template system
class ConfigTemplate {
public:
    ConfigTemplate();
    ~ConfigTemplate();
    
    // Set template name
    void setName(const std::string& name);
    
    // Set template description
    void setDescription(const std::string& description);
    
    // Add parameter definition
    void addParameter(const std::string& name, const std::string& type,
                     const std::string& defaultValue, const std::string& description,
                     bool required = false, const std::string& validation = "");
    
    // Get parameter definition
    struct ParameterDefinition {
        std::string name;
        std::string type;
        std::string defaultValue;
        std::string description;
        bool required;
        std::string validation;
        
        ParameterDefinition() : name(), type(), defaultValue(), description(), 
                               required(false), validation() {}
    };
    
    ParameterDefinition getParameter(const std::string& name) const;
    
    // Check if parameter exists
    bool hasParameter(const std::string& name) const;
    
    // Get all parameters
    std::vector<ParameterDefinition> getAllParameters() const;
    
    // Validate parameter values
    bool validateParameter(const std::string& name, const std::string& value) const;
    
    // Convert parameter to ConfigValue
    ConfigValue parameterToConfigValue(const std::string& name, const std::string& value) const;
    
    // Generate configuration from template
    std::shared_ptr<class Config> generateConfig(const std::map<std::string, std::string>& parameters) const;
    
    // Export template
    std::string exportTemplate() const;
    
    // Import template
    bool importTemplate(const std::string& templateString);
    
    // Get template name
    std::string getName() const;
    
    // Get template description
    std::string getDescription() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration preset factory
class ConfigPresetFactory {
public:
    ConfigPresetFactory();
    ~ConfigPresetFactory();
    
    // Create development preset
    static ConfigPreset createDevelopmentPreset();
    
    // Create performance preset
    static ConfigPreset createPerformancePreset();
    
    // Create research preset
    static ConfigPreset createResearchPreset();
    
    // Create educational preset
    static ConfigPreset createEducationalPreset();
    
    // Create standard preset
    static ConfigPreset createStandardPreset();
    
    // Create custom preset
    static ConfigPreset createCustomPreset(const std::string& name, const std::string& description);
    
    // Create specialized presets
    static ConfigPreset createHighPerformancePreset();
    static ConfigPreset createMemoryEfficientPreset();
    static ConfigPreset createAccuratePreset();
    static ConfigPreset createFastPreset();
    
    // Get all built-in presets
    static std::vector<std::string> getBuiltInPresetNames();
    
    // Validate preset compatibility
    static bool validatePresetCompatibility(const ConfigPreset& preset, const std::string& targetVersion);
    
    // Get preset recommendations
    static std::vector<std::string> getPresetRecommendations(const std::string& useCase);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration comparison utility
class ConfigComparison {
public:
    ConfigComparison();
    ~ConfigComparison();
    
    // Compare two configurations
    struct ComparisonResult {
        std::map<std::string, std::pair<std::string, std::string>> different;     // Key-value pairs for differences
        std::map<std::string, std::string> onlyInFirst;                           // Only in first config
        std::map<std::string, std::string> onlyInSecond;                         // Only in second config
        std::map<std::string, std::pair<std::string, std::string>> similar;        // Key-value pairs with similar values
        std::vector<std::string> warnings;                                      // Comparison warnings
        double similarityScore;                                                // Similarity score (0-1)
        
        ComparisonResult() : similarityScore(0.0) {}
    };
    
    ComparisonResult compareConfigs(const std::shared_ptr<class Config>& config1,
                                    const std::shared_ptr<class Config>& config2) const;
    
    // Compare configuration with preset
    ComparisonResult compareConfigWithPreset(const std::shared_ptr<class Config>& config,
                                             const ConfigPreset& preset) const;
    
    // Generate diff report
    std::string generateDiffReport(const ComparisonResult& result, 
                                   const std::string& config1Name = "Config1",
                                   const std::string& config2Name = "Config2") const;
    
    // Check if configurations are compatible
    bool areCompatible(const std::shared_ptr<class Config>& config1,
                       const std::shared_ptr<class Config>& config2) const;
    
    // Get compatibility warnings
    std::vector<std::string> getCompatibilityWarnings(const std::shared_ptr<class Config>& config1,
                                                       const std::shared_ptr<class Config>& config2) const;
    
    // Merge compatible configurations
    std::shared_ptr<class Config> mergeConfigs(const std::shared_ptr<class Config>& config1,
                                              const std::shared_ptr<class Config>& config2) const;
    
    // Resolve configuration conflicts
    std::shared_ptr<class Config> resolveConflicts(const std::shared_ptr<class Config>& config1,
                                                  const std::shared_ptr<class Config>& config2,
                                                  const std::function<bool(const std::string&, const std::string&, const std::string&)>& resolver) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration validation system
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Validate configuration
    bool validate(const std::shared_ptr<class Config>& config) const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors(const std::shared_ptr<class Config>& config) const;
    
    // Get validation warnings
    std::vector<std::string> getValidationWarnings(const std::shared_ptr<class Config>& config) const;
    
    // Add custom validator
    void addValidator(const std::string& name, 
                     std::function<bool(const std::shared_ptr<class Config>&)> validator,
                     const std::string& errorMessage = "Validation failed");
    
    // Remove validator
    void removeValidator(const std::string& name);
    
    // Get all validator names
    std::vector<std::string> getValidatorNames() const;
    
    // Validate specific parameter
    bool validateParameter(const std::shared_ptr<class Config>& config,
                          const std::string& key, const std::string& expectedType = "") const;
    
    // Get parameter validation rules
    std::string getParameterValidationRules(const std::string& key) const;
    
    // Check if parameter is valid
    bool isValidParameter(const std::string& key, const std::string& value) const;
    
    // Generate validation report
    std::string generateValidationReport(const std::shared_ptr<class Config>& config) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced configuration utilities
namespace config_utils {
    
    // Generate configuration hash
    std::string generateConfigHash(const std::shared_ptr<class Config>& config);
    
    // Check if configuration is optimized
    bool isOptimized(const std::shared_ptr<class Config>& config);
    
    // Get optimization recommendations
    std::vector<std::string> getOptimizationRecommendations(const std::shared_ptr<class Config>& config);
    
    // Apply optimization
    void applyOptimization(std::shared_ptr<class Config>& config);
    
    // Calculate configuration complexity
    double calculateComplexity(const std::shared_ptr<class Config>& config);
    
    // Get configuration size in bytes
    size_t getConfigSize(const std::shared_ptr<class Config>& config);
    
    // Estimate configuration memory usage
    uint64_t estimateMemoryUsage(const std::shared_ptr<class Config>& config);
    
    // Check configuration consistency
    bool checkConsistency(const std::shared_ptr<class Config>& config);
    
    // Fix common configuration issues
    bool fixCommonIssues(std::shared_ptr<class Config>& config);
    
    // Generate configuration summary
    std::string generateSummary(const std::shared_ptr<class Config>& config);
    
    // Format configuration for display
    std::string formatConfig(const std::shared_ptr<class Config>& config, 
                           const std::string& format = "text");
    
    // Convert configuration to JSON
    std::string toJSON(const std::shared_ptr<class Config>& config);
    
    // Convert configuration from JSON
    std::shared_ptr<class Config> fromJSON(const std::string& json);
    
    // Convert configuration to YAML
    std::string toYAML(const std::shared_ptr<class Config>& config);
    
    // Convert configuration from YAML
    std::shared_ptr<class Config> fromYAML(const std::string& yaml);
    
    // Convert configuration to XML
    std::string toXML(const std::shared_ptr<class Config>& config);
    
    // Convert configuration from XML
    std::shared_ptr<class Config> fromXML(const std::string& xml);
    
    // Clone configuration
    std::shared_ptr<class Config> cloneConfig(const std::shared_ptr<class Config>& config);
    
    // Merge configurations
    std::shared_ptr<class Config> mergeConfigs(const std::vector<std::shared_ptr<class Config>>& configs);
    
    // Apply preset to configuration
    std::shared_ptr<class Config> applyPreset(const std::string& presetName,
                                             const std::shared_ptr<class Config>& config = nullptr);
    
    // Get default configuration for preset
    std::shared_ptr<class Config> getDefaultConfig(const std::string& presetName);
    
    // Check if configuration exists
    bool configExists(const std::string& presetName);
    
    // List all available configurations
    std::vector<std::string> listAvailableConfigs();
    
    // Get configuration metadata
    struct ConfigMetadata {
        std::string name;
        std::string description;
        std::string version;
        std::string category;
        std::string author;
        std::string date;
        std::string tags;
    };
    
    ConfigMetadata getConfigMetadata(const std::string& presetName);
    
    // Update configuration metadata
    bool updateConfigMetadata(const std::string& presetName, const ConfigMetadata& metadata);
    
    // Delete configuration
    bool deleteConfig(const std::string& presetName);
    
    // Backup configuration
    bool backupConfig(const std::string& presetName, const std::string& backupPath);
    
    // Restore configuration from backup
    bool restoreConfigFromBackup(const std::string& presetName, const std::string& backupPath);
    
    // Get configuration backup list
    std::vector<std::string> listConfigBackups(const std::string& presetName);
    
    // Remove configuration backup
    bool removeConfigBackup(const std::string& presetName, const std::string& backupName);
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalConfigs;
        size_t totalParameters;
        size_t totalFiles;
        uint64_t totalSize;
        std::map<std::string, size_t> presetsByCategory;
        std::vector<std::string> recentConfigs;
        std::chrono::system_clock::time_point lastModified;
    };
    
    ConfigStats getConfigStats();
    
    // Clear all configuration caches
    void clearConfigCache();
    
    // Reload configuration files
    bool reloadConfigFiles();
    
    // Export all configurations
    bool exportAllConfigs(const std::string& outputDirectory);
    
    // Import all configurations
    bool importAllConfigs(const std::string& inputDirectory);
    
    // Validate configuration compatibility
    bool validateCompatibility(const std::string& presetName, const std::string& targetVersion);
    
    // Get configuration dependencies
    std::vector<std::string> getConfigDependencies(const std::string& presetName);
    
    // Check if configuration has dependencies
    bool hasDependencies(const std::string& presetName);
    
    // Get configuration dependents
    std::vector<std::string> getConfigDependents(const std::string& presetName);
    
    // Check if configuration is used by others
    bool isUsedByOthers(const std::string& presetName);
    
    // Get configuration usage count
    size_t getConfigUsageCount(const std::string& presetName);
    
    // Record configuration usage
    void recordConfigUsage(const std::string& presetName);
    
    // Get configuration usage statistics
    std::map<std::string, size_t> getConfigUsageStats();
    
    // Clean up unused configurations
    void cleanupUnusedConfigs();
    
    // Archive old configurations
    bool archiveConfigs(const std::string& archivePath, const std::string& filter = "");
    
    // Restore archived configurations
    bool restoreArchivedConfigs(const std::string& archivePath);
    
    // Get configuration history
    std::vector<std::pair<std::string, std::chrono::system_clock::time_point>> getConfigHistory(
        const std::string& presetName, size_t count = 10);
    
    // Get configuration changes
    std::vector<std::pair<std::chrono::system_clock::time_point, std::string>> getConfigChanges(
        const std::string& presetName);
    
    // Rollback configuration
    bool rollbackConfig(const std::string& presetName, 
                       const std::chrono::system_clock::time_point& timestamp);
    
    // Get configuration version history
    std::vector<std::string> getConfigVersionHistory(const std::string& presetName);
    
    // Check if configuration version exists
    bool hasConfigVersion(const std::string& presetName, const std::string& version);
    
    // Get configuration version
    std::string getConfigVersion(const std::string& presetName);
    
    // Set configuration version
    bool setConfigVersion(const std::string& presetName, const std::string& version);
    
    // Increment configuration version
    bool incrementConfigVersion(const std::string& presetName);
    
    // Get configuration author
    std::string getConfigAuthor(const std::string& presetName);
    
    // Set configuration author
    bool setConfigAuthor(const std::string& presetName, const std::string& author);
    
    // Get configuration date
    std::chrono::system_clock::time_point getConfigDate(const std::string& presetName);
    
    // Set configuration date
    bool setConfigDate(const std::string& presetName, const std::chrono::system_clock::time_point& date);
    
    // Add configuration tag
    bool addConfigTag(const std::string& presetName, const std::string& tag);
    
    // Remove configuration tag
    bool removeConfigTag(const std::string& presetName, const std::string& tag);
    
    // Get configuration tags
    std::vector<std::string> getConfigTags(const std::string& presetName);
    
    // Search configurations by tag
    std::vector<std::string> searchConfigsByTag(const std::string& tag);
    
    // Get configuration size
    size_t getConfigSize(const std::string& presetName);
    
    // Estimate configuration memory usage
    uint64_t estimateConfigMemoryUsage(const std::string& presetName);
    
    // Get configuration statistics
    struct DetailedConfigStats {
        std::string name;
        std::string description;
        std::string category;
        std::string version;
        std::string author;
        std::chrono::system_clock::time_point created;
        std::chrono::system_clock::time_point lastModified;
        size_t parameterCount;
        size_t fileSize;
        uint64_t memoryUsage;
        std::vector<std::string> tags;
        std::map<std::string, size_t> dependencies;
    };
    
    DetailedConfigStats getDetailedConfigStats(const std::string& presetName);
    
    // Get all configuration statistics
    std::vector<DetailedConfigStats> getAllConfigStats();
    
    // Export configuration to file
    bool exportConfigToFile(const std::string& presetName, const std::string& filepath);
    
    // Import configuration from file
    bool importConfigFromFile(const std::string& presetName, const std::string& filepath);
    
    // Validate configuration file
    bool validateConfigFile(const std::string& presetName, const std::string& filepath);
    
    // Generate configuration report
    std::string generateConfigReport(const std::string& presetName);
    
    // Get configuration health status
    struct ConfigHealthStatus {
        bool isHealthy;
        std::vector<std::string> issues;
        std::vector<std::string> warnings;
        double healthScore;
    };
    
    ConfigHealthStatus getConfigHealthStatus(const std::string& presetName);
    
    // Fix configuration issues
    bool fixConfigIssues(const std::string& presetName);
    
    // Validate configuration integrity
    bool validateConfigIntegrity(const std::string& presetName);
    
    // Get configuration dependencies
    std::vector<std::string> getConfigDependencies(const std::string& presetName);
    
    // Add configuration dependency
    bool addConfigDependency(const std::string& presetName, const std::string& dependency);
    
    // Remove configuration dependency
    bool removeConfigDependency(const std::string& presetName, const std::string& dependency);
    
    // Check if configuration is valid
    bool isValid(const std::string& presetName);
    
    // Validate configuration
    bool validate(const std::string& presetName);
    
    // Get validation errors
    std::vector<std::string> getValidationErrors(const std::string& presetName);
    
    // Get validation warnings
    std::vector<std::string> getValidationWarnings(const std::string& presetName);
    
    // Add custom validation rule
    void addValidationRule(const std::string& name,
                          std::function<bool(const std::shared_ptr<class Config>&)> rule,
                          const std::string& errorMessage = "Validation failed");
    
    // Remove validation rule
    void removeValidationRule(const std::string& name);
    
    // Get all validation rule names
    std::vector<std::string> getValidationRuleNames();
    
    // Execute validation rules
    std::vector<std::string> executeValidationRules(const std::shared_ptr<class Config>& config);
    
    // Get validation summary
    std,color getValidationSummary(const std::shared_ptr<class Config>& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace config_utils

} // namespace nlm
