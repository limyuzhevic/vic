// Advanced Configuration Management System
// Configuration comparison, diffing, validation reports, templates, and inheritance

#pragma once

#include "Config.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>

namespace nlm {

// Configuration comparison and diffing
struct ConfigDiff {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    std::string sourceOld;
    std::string sourceNew;
    std::string changeType; // "added", "removed", "modified"
    std::string descriptionOld;
    std::string descriptionNew;
    bool isBreakingChange;
    double confidence;
};

struct ConfigValidationReport {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::vector<std::string> suggestions;
    double validationScore;
    size_t totalKeys;
    size_t validKeys;
    size_t errorCount;
    size_t warningCount;
    std::chrono::system_clock::time_point timestamp;
    std::string reportSummary;
    std::unordered_map<std::string, std::string> validationMetadata;
};

struct ConfigInheritance {
    std::string baseConfigPath;
    std::unordered_map<std::string, std::string> overrides;
    bool mergeArrays;
    bool mergeMaps;
    std::vector<std::string> excludedKeys;
    std::vector<std::string> priorityKeys;
    std::string inheritanceStrategy; // "override", "merge", "strict"
    bool resolveCircularReferences;
    bool validateAfterInheritance;
};

// Configuration templates for common setups
struct ConfigTemplate {
    std::string name;
    std::string description;
    std::string category; // "development", "production", "research", "benchmark"
    std::unordered_map<std::string, ConfigValue> baseConfig;
    std::vector<std::string> requiredParameters;
    std::unordered_map<std::string, std::string> parameterDescriptions;
    std::unordered_map<std::string, std::pair<ConfigValue, ConfigValue>> parameterRanges;
    double estimatedMemoryUsage;
    double estimatedPerformance;
    std::vector<std::string> tags;
    bool isCustomizable;
    bool isPublic;
    std::string author;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point lastUpdated;
};

// Configuration history tracking
struct ConfigHistoryEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string operation; // "create", "update", "save", "load", "merge"
    std::string user;
    std::string description;
    std::string checksum;
    size_t version;
    std::string notes;
};

// Main configuration management class
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager() = default;
    
    // Configuration comparison and diffing
    std::vector<ConfigDiff> compareConfigs(const Config& config1, const Config& config2) const;
    std::vector<ConfigDiff> diffConfig(const Config& original, const Config& modified) const;
    std::string generateDiffReport(const std::vector<ConfigDiff>& diffs) const;
    bool detectBreakingChanges(const std::vector<ConfigDiff>& diffs, std::vector<std::string>& breakingChanges) const;
    
    // Configuration validation reports
    ConfigValidationReport validateConfig(const Config& config) const;
    ConfigValidationReport validateConfigWithExtendedChecks(const Config& config) const;
    std::string generateValidationReport(const ConfigValidationReport& report) const;
    double calculateValidationScore(const Config& config) const;
    
    // Configuration templates
    bool createTemplate(const ConfigTemplate& template);
    bool updateTemplate(const ConfigTemplate& template);
    bool deleteTemplate(const std::string& name);
    std::optional<ConfigTemplate> getTemplate(const std::string& name) const;
    std::vector<ConfigTemplate> getTemplates(const std::string& category = "") const;
    std::vector<ConfigTemplate> searchTemplates(const std::string& query) const;
    Config applyTemplate(const ConfigTemplate& template, const std::unordered_map<std::string, ConfigValue>& overrides = {}) const;
    
    // Configuration inheritance
    Config resolveInheritance(const ConfigInheritance& inheritance, const std::shared_ptr<Config>& baseConfig = nullptr) const;
    std::vector<std::string> resolveInheritanceChain(const ConfigInheritance& inheritance) const;
    bool validateInheritance(const ConfigInheritance& inheritance) const;
    
    // Configuration history and version control
    void addToHistory(const std::string& configName, const std::string& operation,
                     const std::string& user = "", const std::string& description = "");
    std::vector<ConfigHistoryEntry> getHistory(const std::string& configName) const;
    std::string calculateChecksum(const Config& config) const;
    bool validateChecksum(const Config& config, const std::string& checksum) const;
    
    // Configuration export/import with compression
    bool exportConfig(const Config& config, const std::string& filepath,
                    const std::string& format = "json",
                    bool compress = false,
                    const std::string& compressionAlgorithm = "gzip") const;
    std::optional<Config> importConfig(const std::string& filepath,
                                      const std::string& format = "auto",
                                      bool decompress = false) const;
    bool createIncrementalBackup(const Config& config, const std::string& backupDir);
    std::vector<std::string> listBackups(const std::string& configName) const;
    std::optional<Config> restoreFromBackup(const std::string& configName, const std::string& backupId) const;
    
    // Configuration presets and environments
    bool createPreset(const std::string& name, const Config& config,
                     const std::string& description = "",
                     const std::vector<std::string>& tags = {});
    bool updatePreset(const std::string& name, const Config& config);
    bool deletePreset(const std::string& name);
    std::optional<Config> getPreset(const std::string& name) const;
    std::vector<std::string> getPresetNames() const;
    std::vector<Config> getPresetsByTag(const std::string& tag) const;
    
    // Configuration analysis and optimization
    std::unordered_map<std::string, double> analyzeConfigPerformance(const Config& config) const;
    std::vector<std::string> suggestOptimizations(const Config& config) const;
    std::unordered_map<std::string, ConfigValue> optimizeConfig(const Config& config) const;
    
    // Configuration validation rules and constraints
    bool addValidationRule(const std::string& key, const std::string& ruleType,
                          const std::string& ruleData,
                          const std::string& errorMessage);
    bool removeValidationRule(const std::string& key);
    std::vector<std::string> getValidationRules(const std::string& key = "") const;
    bool validateConfigRules(const Config& config) const;
    
    // Configuration comparison with advanced features
    std::string generateDetailedComparisonReport(const Config& config1, const Config& config2,
                                                 const std::string& reportType = "detailed") const;
    std::vector<ConfigDiff> findSimilarConfigs(const std::vector<Config>& configs, double threshold = 0.8) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration template manager
class ConfigTemplateManager {
public:
    ConfigTemplateManager();
    ~ConfigTemplateManager() = default;
    
    // Template lifecycle management
    bool createTemplate(const ConfigTemplate& template);
    bool updateTemplate(const ConfigTemplate& template);
    bool deleteTemplate(const std::string& name);
    bool importTemplate(const std::string& filepath);
    bool exportTemplate(const std::string& name, const std::string& filepath) const;
    
    // Template discovery and search
    std::vector<ConfigTemplate> getAllTemplates() const;
    std::vector<ConfigTemplate> getTemplatesByCategory(const std::string& category) const;
    std::vector<ConfigTemplate> searchTemplates(const std::string& query) const;
    std::vector<ConfigTemplate> filterTemplates(const std::function<bool(const ConfigTemplate&)>& predicate) const;
    
    // Template validation and quality checks
    bool validateTemplate(const ConfigTemplate& template) const;
    std::vector<std::string> getTemplateQualityIssues(const ConfigTemplate& template) const;
    double calculateTemplateScore(const ConfigTemplate& template) const;
    
    // Template recommendation system
    std::vector<ConfigTemplate> getRecommendedTemplates(const std::string& useCase,
                                                       const std::string& environment = "") const;
    std::string generateTemplateRecommendationReport(const std::string& useCase) const;
    
    // Template versioning
    bool createTemplateVersion(const std::string& templateName, const std::string& notes = "");
    std::vector<ConfigTemplate> getTemplateVersions(const std::string& templateName) const;
    std::optional<ConfigTemplate> getTemplateVersion(const std::string& templateName, size_t version) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration backup and recovery system
class ConfigBackupSystem {
public:
    ConfigBackupSystem();
    ~ConfigBackupSystem() = default;
    
    // Backup management
    bool createBackup(const std::string& configName, const Config& config,
                     const std::string& notes = "",
                     bool compress = true,
                     const std::string& compressionAlgorithm = "gzip");
    bool createIncrementalBackup(const std::string& configName, const Config& config);
    std::optional<Config> restoreBackup(const std::string& configName, const std::string& backupId);
    bool deleteBackup(const std::string& configName, const std::string& backupId);
    std::vector<std::string> listBackups(const std::string& configName) const;
    
    // Recovery and rollback
    bool canRollback(const std::string& configName, const std::string& backupId);
    bool rollbackToBackup(const std::string& configName, const std::string& backupId,
                         const std::string& notes = "");
    std::vector<std::string> getRollbackHistory(const std::string& configName) const;
    
    // Health and monitoring
    bool isBackupHealthy(const std::string& configName, const std::string& backupId) const;
    std::vector<std::string> getBackupHealthIssues(const std::string& configName, const std::string& backupId) const;
    double calculateBackupIntegrity(const std::string& configName, const std::string& backupId) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Helper functions for configuration management
namespace ConfigUtils {
    std::string generateChecksum(const std::string& data);
    std::string formatSize(size_t bytes);
    std::string formatDuration(double seconds);
    std::string escapeForJSON(const std::string& input);
    std::string escapeForYAML(const std::string& input);
    
    // Configuration analysis
    std::unordered_map<std::string, size_t> countConfigTypes(const Config& config);
    double calculateConfigComplexity(const Config& config);
    std::vector<std::string> findConfigConflicts(const Config& config);
    
    // Validation helpers
    bool validateJSONFile(const std::string& filepath);
    bool validateYAMLFile(const std::string& filepath);
    bool validateTOMLFile(const std::string& filepath);
    
    // Diff and comparison helpers
    std::string configValueToString(const ConfigValue& value);
    bool compareConfigValues(const ConfigValue& a, const ConfigValue& b);
    std::string getConfigChangeType(const ConfigEntry& oldEntry, const ConfigEntry& newEntry);
}

} // namespace nlm
