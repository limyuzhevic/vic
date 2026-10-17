// Enhanced Configuration Management System for NLM
// Advanced configuration management with validation, inheritance, observers, and analytics

#ifndef NLM_CONFIG_MANAGER_H
#define NLM_CONFIG_MANAGER_H

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <filesystem>

namespace nlm {

// Enhanced configuration value types with better type safety
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>,
    std::unordered_map<std::string, std::string>
>;

// Configuration validation rules
struct ValidationRule {
    enum class Type {
        Range,
        AllowedValues,
        Pattern,
        Custom
    };
    
    Type type;
    std::string errorMessage;
    std::string param1;
    std::string param2;
    std::string param3;
    
    ValidationRule(Type t, const std::string& msg) 
        : type(t), errorMessage(msg) {}
    ValidationRule(Type t, const std::string& msg, const std::string& p1) 
        : type(t), errorMessage(msg), param1(p1) {}
    ValidationRule(Type t, const std::string& msg, const std::string& p1, const std::string& p2) 
        : type(t), errorMessage(msg), param1(p1), param2(p2) {}
    ValidationRule(Type t, const std::string& msg, const std::string& p1, const std::string& p2, const std::string& p3) 
        : type(t), errorMessage(msg), param1(p1), param2(p2), param3(p3) {}
};

// Configuration change event
struct ConfigChangeEvent {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource source;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigChangeEvent(const std::string& k, const ConfigValue& oldVal, const ConfigValue& newVal, 
                     ConfigSource src)
        : key(k), oldValue(oldVal), newValue(newVal), source(src), 
          timestamp(std::chrono::system_clock::now()) {}
};

// Configuration template for predefined settings
struct ConfigTemplate {
    std::string name;
    std::string description;
    std::unordered_map<std::string, ConfigValue> defaults;
    std::vector<ValidationRule> validations;
    
    ConfigTemplate(const std::string& n, const std::string& desc) 
        : name(n), description(desc) {}
    
    void addDefault(const std::string& key, const ConfigValue& value) {
        defaults[key] = value;
    }
    
    void addValidation(const ValidationRule& rule) {
        validations.push_back(rule);
    }
    
    bool validate(const ConfigValue& value) const;
};

// Configuration observer interface
class ConfigObserver {
public:
    virtual ~ConfigObserver() = default;
    
    virtual void onConfigChanged(const ConfigChangeEvent& event) = 0;
    virtual void onConfigLoaded(const std::string& source) = 0;
    virtual void onConfigError(const std::string& error) = 0;
};

// Configuration statistics
struct ConfigStatistics {
    size_t totalEntries;
    size_t fileEntries;
    size_t commandLineEntries;
    size_t runtimeEntries;
    double validationSuccessRate;
    std::chrono::duration<double> totalLoadTime;
    size_t getUniqueValueCount() const;
    std::string getMostCommonType() const;
};

// Enhanced configuration manager with advanced features
class ConfigManager {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Thread safety
    mutable std::mutex mutex;
    
    // Observer management
    std::vector<std::shared_ptr<ConfigObserver>> observers;
    
    // Configuration history
    std::vector<std::unordered_map<std::string, ConfigValue>> configHistory;
    
    // Template management
    std::unordered_map<std::string, ConfigTemplate> templates;
    
    // Backup management
    std::string backupDirectory;
    size_t maxBackups;
    
    // Statistics
    mutable ConfigStatistics stats;
    
    // Internal helpers
    static bool validateValue(const ConfigValue& value, const ValidationRule& rule);
    static std::string valueToString(const ConfigValue& value);
    static std::string typeToString(const ConfigValue& value);
    
public:
    ConfigManager();
    ~ConfigManager();
    
    // Disable copying, enable moving
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    ConfigManager(ConfigManager&&) noexcept;
    ConfigManager& operator=(ConfigManager&&) noexcept;
    
    // Load from various sources
    bool loadFromFile(const std::string& filepath, bool validate = true);
    bool loadFromJSON(const std::string& filepath, bool validate = true);
    bool loadFromINI(const std::string& filepath, bool validate = true);
    bool loadFromYAML(const std::string& filepath, bool validate = true);
    bool loadFromArgs(int argc, char** argv);
    
    // Load from string
    bool loadFromString(const std::string& content, const std::string& format = "auto");
    
    // Save to file
    bool saveToFile(const std::string& filepath, const std::string& format = "ini") const;
    bool saveToJSON(const std::string& filepath) const;
    bool saveToINI(const std::string& filepath) const;
    
    // Configuration management
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration operations
    void remove(const std::string& key);
    void clear();
    void merge(const ConfigManager& other, ConfigSource source = ConfigSource::Runtime);
    void applyTemplate(const std::string& templateName);
    
    // Query methods
    std::optional<ConfigValue> get(const std::string& key) const;
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    std::vector<std::string> getKeys() const;
    std::vector<std::string> findKeys(const std::string& pattern) const;
    
    // Configuration validation
    bool validate() const;
    bool validateKey(const std::string& key) const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    
    // Statistics and analytics
    ConfigStatistics getStatistics() const;
    std::string analyzeConfiguration() const;
    double calculateConfigurationScore() const;
    
    // Observer management
    void addObserver(std::shared_ptr<ConfigObserver> observer);
    void removeObserver(std::shared_ptr<ConfigObserver> observer);
    
    // Configuration history
    void snapshot(const std::string& description);
    std::vector<std::unordered_map<std::string, ConfigValue>> getHistory() const;
    void restoreFromSnapshot(size_t index);
    
    // Backup management
    void setBackupDirectory(const std::string& directory, size_t maxBackups = 10);
    bool createBackup(const std::string& description = "");
    bool restoreBackup(const std::string& backupName);
    std::vector<std::string> getBackups() const;
    
    // Template management
    void addTemplate(const ConfigTemplate& template);
    void removeTemplate(const std::string& name);
    bool hasTemplate(const std::string& name) const;
    std::vector<std::string> getTemplateNames() const;
    
    // Configuration export/import
    bool exportToPython(const std::string& filepath) const;
    bool importFromPython(const std::string& filepath);
    bool exportToJSONSchema(const std::string& filepath) const;
    bool importFromJSONSchema(const std::string& filepath);
    
    // Comparison and diff
    std::unordered_map<std::string, std::pair<ConfigValue, ConfigValue>> diff(const ConfigManager& other) const;
    bool equals(const ConfigManager& other) const;
    
    // Configuration search
    std::vector<std::string> search(const std::string& query) const;
    std::vector<std::pair<std::string, float>> suggestKeys(const std::string& partialKey) const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    size_t getValueCount(const std::string& type) const;
    
    // Configuration information
    std::string getSummary() const;
    std::string getSourceSummary() const;
    std::string getValidationSummary() const;
    
    // Error handling
    std::string getLastError() const;
    void clearError();
    
    // Configuration validation rules management
    void addValidationRule(const std::string& key, const ValidationRule& rule);
    void removeValidationRule(const std::string& key);
    std::vector<ValidationRule> getValidationRules(const std::string& key) const;
    
    // Configuration change events
    std::vector<ConfigChangeEvent> getChangeEvents() const;
    void clearChangeEvents();
    
    // Configuration monitoring
    bool isMonitoring() const;
    void startMonitoring(size_t intervalSeconds);
    void stopMonitoring();
    
    // Configuration backup
    bool createAutoBackup();
    bool createScheduledBackup();
    
    // Configuration validation
    bool runComprehensiveValidation() const;
    std::unordered_map<std::string, std::vector<std::string>> getValidationErrors() const;
    
    // Configuration search
    struct SearchResult {
        std::string key;
        ConfigValue value;
        float score;
        std::string context;
    };
    
    std::vector<SearchResult> searchAdvanced(const std::string& query) const;
    
    // Configuration optimization
    bool optimizeConfiguration();
    bool removeUnusedEntries();
    bool consolidateSimilarEntries();
    
    // Configuration backup
    struct BackupInfo {
        std::string name;
        std::chrono::system_clock::time_point timestamp;
        size_t size;
        std::string description;
    };
    
    std::vector<BackupInfo> getBackupInfo() const;
    
    // Configuration statistics
    struct UsageStats {
        std::chrono::duration<double> totalLoadTime;
        size_t totalAccesses;
        double averageLoadTime;
        size_t getMaxMemoryUsage() const;
        double getMemoryEfficiency() const;
    };
    
    UsageStats getUsageStatistics() const;
    
    // Configuration validation
    struct ValidationReport {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        double score;
    };
    
    ValidationReport validateComprehensive() const;
    
    // Configuration comparison
    struct ComparisonReport {
        bool identical;
        std::vector<std::string> addedKeys;
        std::vector<std::string> removedKeys;
        std::vector<std::string> modifiedKeys;
        double similarity;
    };
    
    ComparisonReport compare(const ConfigManager& other) const;
    
    // Configuration export/import
    bool exportToArchive(const std::string& filepath) const;
    bool importFromArchive(const std::string& filepath);
    
    // Configuration encryption (basic)
    bool encrypt(const std::string& password);
    bool decrypt(const std::string& password);
    
    // Configuration compression
    bool compress();
    bool decompress();
    
    // Configuration recovery
    bool attemptRecovery();
    bool validateIntegrity();
    
    // Configuration monitoring
    void setMonitoringCallback(std::function<void(const ConfigChangeEvent&)> callback);
    void setValidationCallback(std::function<bool(const std::string&, const ConfigValue&)> callback);
    
    // Configuration statistics
    double calculateCohesion() const;
    double calculateCoupling() const;
    
    // Configuration validation
    bool validateCrossDependencies() const;
    bool validateTypeConsistency() const;
    
    // Configuration optimization
    bool removeRedundantEntries();
    bool optimizeOrdering();
    bool consolidateSimilarTypes();
    
    // Configuration backup
    bool createCheckpoint(const std::string& description);
    bool rollbackToCheckpoint(size_t index);
    std::vector<std::string> getCheckpoints() const;
    
    // Configuration statistics
    struct PerformanceMetrics {
        std::chrono::duration<double> loadTime;
        std::chrono::duration<double> saveTime;
        size_t memoryUsage;
        double cpuUsage;
        size_t accessCount;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Configuration validation
    struct Constraint {
        std::string name;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
    };
    
    void addConstraint(const Constraint& constraint);
    bool validateConstraints() const;
    
    // Configuration search
    struct FuzzyResult {
        std::string key;
        float similarity;
        ConfigValue value;
    };
    
    std::vector<FuzzyResult> fuzzySearch(const std::string& pattern) const;
    
    // Configuration analysis
    struct AnalysisResult {
        std::string type;
        double score;
        std::string recommendation;
    };
    
    std::vector<AnalysisResult> analyzeConfiguration() const;
    
    // Configuration validation
    bool validateSecurityConstraints() const;
    bool validatePerformanceConstraints() const;
    
    // Configuration optimization
    bool optimizeMemoryUsage();
    bool optimizeDiskUsage();
    bool optimizeCPUUsage();
    
    // Configuration backup
    bool createSnapshotBackup();
    bool createIncrementalBackup();
    bool restoreFromBackup(const std::string& name);
    
    // Configuration monitoring
    void enableMonitoring() { monitoringEnabled = true; }
    void disableMonitoring() { monitoringEnabled = false; }
    bool isMonitoringEnabled() const { return monitoringEnabled; }
    
    // Configuration statistics
    struct AdvancedStats {
        size_t totalEntries;
        size_t uniqueKeys;
        double valueVariety;
        double complexityScore;
        double stabilityScore;
        std::chrono::duration<double> uptime;
        size_t getAverageValueSize() const;
        std::string getDominantDataType() const;
    };
    
    AdvancedStats getAdvancedStatistics() const;
    
    // Configuration validation
    bool validateEnterpriseConstraints() const;
    bool validateProductionConstraints() const;
    
    // Configuration optimization
    bool runOptimizationRoutine();
    bool applyBestPractices();
    
    // Configuration backup
    bool createHealthCheckBackup();
    bool validateBackupIntegrity();
    
    // Configuration monitoring
    void setPerformanceThresholds(double maxLoadTime, size_t maxMemoryUsage);
    bool checkPerformanceThresholds() const;
    
    // Configuration validation
    bool validateSecurityModel() const;
    bool validateAccessControls() const;
    
    // Configuration analysis
    struct TrendAnalysis {
        std::string metric;
        std::vector<std::pair<std::chrono::system_clock::time_point, double>> dataPoints;
        double trend;
        std::string forecast;
    };
    
    std::vector<TrendAnalysis> analyzeTrends() const;
    
    // Configuration optimization
    bool autoOptimize();
    bool applyMachineLearningRecommendations();
    
    // Configuration backup
    bool createMaintenanceBackup();
    bool scheduleRegularBackups();
    
    // Configuration monitoring
    void logConfigurationEvent(const std::string& event, const std::string& details = "");
    void setLogLevel(int level);
    
    // Configuration validation
    bool validateSchema() const;
    bool validateDocumentation() const;
    
    // Configuration optimization
    bool cleanupUnusedEntries();
    bool compressConfiguration();
    bool optimizeIndexes();
    
    // Configuration backup
    bool createEmergencyBackup();
    bool validateConfigurationHealth();
    
protected:
    // Internal helper methods
    void notifyObservers(const ConfigChangeEvent& event);
    void logError(const std::string& error);
    void validateWithTemplates();
    void applyInheritance();
    
    // Configuration loading helpers
    bool loadFromFileInternal(const std::string& filepath, const std::string& format);
    bool loadFromJSONInternal(const std::string& content);
    bool loadFromINIInternal(const std::string& content);
    
    // Configuration saving helpers
    bool saveToFileInternal(const std::string& filepath, const std::string& format) const;
    bool saveToJSONInternal(std::ostream& out) const;
    bool saveToINIInternal(std::ostream& out) const;
    
    // Configuration validation helpers
    void validateConstraints() const;
    void validateDependencies() const;
    
    // Configuration backup helpers
    bool createBackupInternal(const std::string& name);
    bool restoreBackupInternal(const std::string& name);
    
    // Configuration monitoring helpers
    void updateStatistics();
    void checkPerformance();
    
private:
    bool monitoringEnabled;
    size_t monitoringInterval;
    std::chrono::system_clock::time_point lastMonitorCheck;
    std::function<void(const ConfigChangeEvent&)> monitoringCallback;
    std::function<bool(const std::string&, const ConfigValue&)> validationCallback;
};

// Global configuration manager instance
extern std::shared_ptr<ConfigManager> globalConfig;

// Convenience functions
void initializeGlobalConfig(const std::string& configFile = "configs/default.cfg");
std::shared_ptr<ConfigManager> getGlobalConfig();
void setGlobalConfig(std::shared_ptr<ConfigManager> config);

// Configuration utilities
std::string generateConfigID();
std::string generateConfigChecksum(const ConfigManager& config);
bool validateConfigIntegrity(const ConfigManager& config);

} // namespace nlm

#endif // NLM_CONFIG_MANAGER_H
