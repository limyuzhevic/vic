// NLM Configuration History and Hot-Reconfiguration
// Advanced configuration management with history and runtime changes

#ifndef NLM_CONFIG_HISTORY_H
#define NLM_CONFIG_HISTORY_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <functional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Configuration change tracking
struct ConfigurationChange {
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string key;
    std::string oldValue;
    std::string newValue;
    std::string source; // "command_line", "file", "runtime"
    std::string user;   // User who made the change
    std::string reason; // Why the change was made
    bool reverted;      // Whether the change was reverted
    std::string revertedBy; // Who reverted it
    std::chrono::high_resolution_clock::time_point revertTimestamp;
};

// Configuration version management
struct ConfigurationVersion {
    int versionNumber;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string description;
    std::string user; // User who created this version
    std::map<std::string, std::string> configurationValues;
    std::string snapshotFile; // File where this version is saved
};

// Hot-reconfiguration callback
using ConfigurationChangeCallback = std::function<void(const ConfigurationChange& change)>;
using ConfigurationValidationCallback = std::function<bool(const std::string&, const std::string&)>;
using ConfigurationRevertCallback = std::function<bool(const ConfigurationChange& change)>;

// Main configuration history manager
class ConfigurationHistory {
public:
    ConfigurationHistory(const std::string& configFile = "configs/default.cfg");
    ~ConfigurationHistory();
    
    // Load and save configurations
    bool loadConfiguration();
    bool saveConfiguration();
    bool saveConfigurationAs(const std::string& filename);
    
    // Get current configuration
    std::shared_ptr<Config> getCurrentConfig() const { return currentConfig_; }
    std::shared_ptr<Config> getBackupConfig() const { return backupConfig_; }
    
    // Configuration history
    bool saveVersion(const std::string& description, const std::string& user = "system");
    bool loadVersion(int versionNumber);
    std::vector<ConfigurationVersion> getVersionHistory() const { return versionHistory_; }
    
    // Configuration changes
    bool setValue(const std::string& key, const std::string& value, 
                  const std::string& source = "runtime",
                  const std::string& user = "system",
                  const std::string& reason = "");
    bool revertChange(size_t changeIndex);
    bool revertToVersion(int versionNumber);
    
    // Get change history
    std::vector<ConfigurationChange> getChanges() const { return changes_; }
    std::vector<ConfigurationChange> getChangesByKey(const std::string& key) const;
    std::vector<ConfigurationChange> getChangesBySource(const std::string& source) const;
    
    // Change analysis
    std::string getChangeSummary() const;
    std::string generateChangeReport() const;
    bool detectConfigurationDrift() const;
    
    // Configuration validation with history
    bool validateCurrentConfiguration(std::vector<std::string>& errors) const;
    bool validateVersion(int versionNumber, std::vector<std::string>& errors) const;
    
    // Configuration comparison
    std::string compareConfigurations(int version1, int version2) const;
    std::vector<std::string> findConfigurationDifferences(const std::map<std::string, std::string>& config1,
                                                           const std::map<std::string, std::string>& config2) const;
    
    // Callbacks and notifications
    void addChangeCallback(ConfigurationChangeCallback callback);
    void addValidationCallback(ConfigurationValidationCallback callback);
    void addRevertCallback(ConfigurationRevertCallback callback);
    
    void removeChangeCallback(ConfigurationChangeCallback callback);
    void removeValidationCallback(ConfigurationValidationCallback callback);
    void removeRevertCallback(ConfigurationRevertCallback callback);
    
    // Configuration snapshots
    bool createSnapshot(const std::string& label = "");
    bool restoreSnapshot(const std::string& label);
    std::vector<std::string> getAvailableSnapshots() const { return snapshots_; }
    
    // Configuration export/import
    bool exportConfiguration(const std::string& filename, const std::string& format = "json") const;
    bool importConfiguration(const std::string& filename, const std::string& format = "json",
                            const std::string& user = "system", const std::string& reason = "");
    
    // Configuration analysis
    double calculateConfigurationStability() const;
    std::string getMostCommonConfigurationValues() const;
    std::map<std::string, int> getValueFrequencyAnalysis() const;
    
    // Configuration security
    bool isReadOnlyMode() const { return readOnlyMode_; }
    void setReadOnlyMode(bool readOnly) { readOnlyMode_ = readOnly; }
    std::string getLastModifiedBy() const { return lastModifiedBy_; }
    std::chrono::high_resolution_clock::time_point getLastModifiedTime() const { return lastModifiedTime_; }
    
    // Configuration cleanup
    bool cleanupOldVersions(int keepVersions = 10);
    bool cleanupChanges(int maxChanges = 1000);
    bool cleanupSnapshots(int maxSnapshots = 50);
    
    // Statistics
    size_t getChangesCount() const { return changes_.size(); }
    size_t getVersionsCount() const { return versionHistory_.size(); }
    size_t getSnapshotsCount() const { return snapshots_.size(); }
    std::string getConfigurationStatistics() const;
    
private:
    std::shared_ptr<Config> currentConfig_;
    std::shared_ptr<Config> backupConfig_;
    
    std::vector<ConfigurationChange> changes_;
    std::vector<ConfigurationVersion> versionHistory_;
    std::vector<std::string> snapshots_;
    
    std::string configFile_;
    bool readOnlyMode_;
    
    std::string lastModifiedBy_;
    std::chrono::high_resolution_clock::time_point lastModifiedTime_;
    
    std::vector<ConfigurationChangeCallback> changeCallbacks_;
    std::vector<ConfigurationValidationCallback> validationCallbacks_;
    std::vector<ConfigurationRevertCallback> revertCallbacks_;
    
    std::mutex changesMutex_;
    
    // Internal helper methods
    bool loadFromFile(std::shared_ptr<Config> config, const std::string& filename);
    bool saveToFile(const std::shared_ptr<Config>& config, const std::string& filename) const;
    std::string getCurrentTimestamp() const;
    ConfigurationChange createChange(const std::string& key, const std::string& oldValue,
                                   const std::string& newValue, const std::string& source,
                                   const std::string& user, const std::string& reason);
    void notifyChangeCallbacks(const ConfigurationChange& change);
    void notifyValidationCallbacks(const std::string& key, const std::string& newValue);
    bool notifyRevertCallbacks(const ConfigurationChange& change);
    bool validateWithCallbacks(const std::string& key, const std::string& newValue) const;
    
    bool createSnapshotFile(const std::string& label);
    bool restoreSnapshotFile(const std::string& label);
    std::string generateSnapshotFilename(const std::string& label) const;
    
    std::string exportToJSON(const std::map<std::string, std::string>& config) const;
    std::string exportToINI(const std::map<std::string, std::string>& config) const;
    std::string exportToXML(const std::map<std::string, std::string>& config) const;
    
    bool importFromJSON(std::map<std::string, std::string>& config, const std::string& content);
    bool importFromINI(std::map<std::string, std::string>& config, const std::string& content);
    bool importFromXML(std::map<std::string, std::string>& config, const std::string& content);
    
    double calculateValueSimilarity(const std::string& value1, const std::string& value2) const;
    std::string normalizeValueForComparison(const std::string& value) const;
};

// Configuration delta (diff) tracking
class ConfigurationDelta {
public:
    struct DeltaEntry {
        std::string key;
        std::string oldValue;
        std::string newValue;
        std::string changeType; // "added", "removed", "modified"
        double similarity; // 0.0 to 1.0
    };
    
    ConfigurationDelta();
    ~ConfigurationDelta();
    
    // Compare configurations
    std::vector<DeltaEntry> compare(const std::map<std::string, std::string>& config1,
                                   const std::map<std::string, std::string>& config2) const;
    std::vector<DeltaEntry> compareConfigs(const std::shared_ptr<Config>& config1,
                                          const std::shared_ptr<Config>& config2) const;
    
    // Delta analysis
    std::string getDeltaSummary(const std::vector<DeltaEntry>& delta) const;
    std::string generateDeltaReport(const std::vector<DeltaEntry>& delta) const;
    double calculateDeltaSimilarity(const std::vector<DeltaEntry>& delta) const;
    
    // Delta filtering and categorization
    std::vector<DeltaEntry> filterByType(const std::vector<DeltaEntry>& delta,
                                        const std::string& type) const;
    std::vector<DeltaEntry> filterByKey(const std::vector<DeltaEntry>& delta,
                                       const std::string& keyPattern) const;
    std::vector<DeltaEntry> filterByImportance(const std::vector<DeltaEntry>& delta,
                                             double minImportance) const;
    
    // Delta export
    bool exportDelta(const std::vector<DeltaEntry>& delta, const std::string& filename,
                    const std::string& format = "txt") const;
    
    // Delta visualization
    std::string generateTextDiff(const std::vector<DeltaEntry>& delta) const;
    std::string generateHTMLDiff(const std::vector<DeltaEntry>& delta) const;
    std::string generateJSONDiff(const std::vector<DeltaEntry>& delta) const;
    
private:
    std::string calculateChangeType(const std::string& oldValue, const std::string& newValue) const;
    double calculateValueSimilarity(const std::string& value1, const std::string& value2) const;
    std::string formatDiffEntry(const DeltaEntry& entry) const;
};

// Configuration comparison utilities
class ConfigurationComparator {
public:
    struct ComparisonResult {
        std::string operation; // "equal", "similar", "different"
        double similarityScore; // 0.0 to 1.0
        std::vector<std::string> differences;
        std::vector<std::string> similarities;
        std::map<std::string, std::string> mergedConfiguration; // For similar configs
    };
    
    ConfigurationComparator();
    ~ConfigurationComparator();
    
    // Configuration comparison
    ComparisonResult compare(const std::map<std::string, std::string>& config1,
                           const std::map<std::string, std::string>& config2) const;
    ComparisonResult compareConfigs(const std::shared_ptr<Config>& config1,
                                   const std::shared_ptr<Config>& config2) const;
    
    // Similarity analysis
    double calculateSimilarity(const std::map<std::string, std::string>& config1,
                              const std::map<std::string, std::string>& config2) const;
    std::vector<std::string> findSimilarKeys(const std::map<std::string, std::string>& config1,
                                           const std::map<std::string, std::string>& config2,
                                           double minSimilarity = 0.8) const;
    
    // Difference analysis
    std::vector<std::string> findDifferences(const std::map<std::string, std::string>& config1,
                                            const std::map<std::string, std::string>& config2) const;
    std::vector<std::string> findDifferences(const std::shared_ptr<Config>& config1,
                                            const std::shared_ptr<Config>& config2) const;
    
    // Configuration merging
    std::map<std::string, std::string> mergeConfigurations(const std::map<std::string, std::string>& config1,
                                                           const std::map<std::string, std::string>& config2) const;
    
    // Configuration normalization
    std::map<std::string, std::string> normalizeConfiguration(const std::map<std::string, std::string>& config) const;
    
    // Comparison export
    bool exportComparison(const ComparisonResult& result, const std::string& filename,
                         const std::string& format = "txt") const;
    
    // Comparison visualization
    std::string generateComparisonReport(const ComparisonResult& result) const;
    std::string generateSimilarityMatrix(const std::vector<std::map<std::string, std::string>>& configs) const;
    
private:
    std::string normalizeKey(const std::string& key) const;
    std::string normalizeValue(const std::string& value, const std::string& type) const;
    bool areValuesSimilar(const std::string& value1, const std::string& value2, const std::string& type) const;
    double calculateKeySimilarity(const std::string& key1, const std::string& key2) const;
};

// Configuration validation with history
class HistoricalConfigurationValidator {
public:
    struct ValidationRule {
        std::string key;
        std::string type; // "int", "float", "bool", "string", "enum"
        std::string validation; // Range, enum list, pattern, etc.
        bool required = false;
        std::string defaultValue;
        std::function<bool(const std::string&)> customValidator;
        std::string description;
    };
    
    HistoricalConfigurationValidator();
    ~HistoricalConfigurationValidator();
    
    // Add validation rules
    void addRule(const ValidationRule& rule);
    void addIntRule(const std::string& key, bool required, int min = INT_MIN, int max = INT_MAX,
                   const std::string& defaultValue = "", const std::string& description = "");
    void addFloatRule(const std::string& key, bool required, float min = -FLT_MAX, float max = FLT_MAX,
                     const std::string& defaultValue = "", const std::string& description = "");
    void addBoolRule(const std::string& key, bool required, const std::string& defaultValue = "",
                    const std::string& description = "");
    void addStringRule(const std::string& key, bool required, size_t minLength = 0, size_t maxLength = 0,
                      const std::string& defaultValue = "", const std::string& pattern = "",
                      const std::string& description = "");
    
    // Validate configuration with history
    bool validateConfig(const std::map<std::string, std::string>& config, 
                       std::vector<std::string>& errors) const;
    bool validateConfig(const std::shared_ptr<Config>& config,
                       std::vector<std::string>& errors) const;
    
    // Version-specific validation
    bool validateVersion(const std::map<std::string, std::string>& config, int versionNumber,
                       std::vector<std::string>& errors) const;
    
    // Validation analysis
    std::string generateValidationReport(const std::map<std::string, std::string>& config) const;
    double calculateValidationScore(const std::map<std::string, std::string>& config) const;
    
    // Configuration comparison
    std::vector<std::string> compareWithRules(const std::map<std::string, std::string>& config) const;
    
private:
    std::vector<ValidationRule> rules_;
    
    bool validateValue(const std::string& value, const ValidationRule& rule) const;
    bool parseNumber(const std::string& str, double& result) const;
    std::string getRuleDescription(const ValidationRule& rule) const;
};

// Global configuration management instances
extern ConfigurationHistory globalConfigurationHistory;
extern ConfigurationDelta globalConfigurationDelta;
extern ConfigurationComparator globalConfigurationComparator;
extern HistoricalConfigurationValidator globalHistoricalValidator;

} // namespace nlm

#endif // NLM_CONFIG_HISTORY_H