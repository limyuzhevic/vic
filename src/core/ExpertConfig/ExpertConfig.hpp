#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <regex>
#include <nlohmann/json.hpp>
#include "core/Config/Config.hpp"
#include "core/Validation/Validation.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Forward declarations
class ExpertConfig;
class ConfigTemplate;
class ConfigPreset;
class ConfigHistory;
class ConfigAuditor;
class ConfigDiff;

// Advanced configuration value types
using ExpertConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::map<std::string, nlohmann::json>, // Nested config
    std::map<std::string, ExpertConfigValue>, // Advanced nested config
    std::vector<std::map<std::string, ExpertConfigValue>> // Array of nested configs
>;

// Configuration template
struct ConfigTemplate {
    std::string name;
    std::string description;
    ExpertConfigValue defaultValue;
    std::string type; // "simple", "complex", "neural_network", "simulation", "experiment", etc.
    bool isRequired;
    std::unordered_map<std::string, std::string> metadata;
    std::vector<std::string> dependencies;
    std::string validationRule;
    std::string displayName;
    std::string category;
    std::string unit;
    double minValue;
    double maxValue;
    bool configurable;
    
    ConfigTemplate() : isRequired(false), minValue(0), maxValue(0), configurable(true) {}
    ConfigTemplate(const std::string& name, const std::string& description) 
        : name(name), description(description), isRequired(false), minValue(0), maxValue(0), configurable(true) {}
};

// Configuration preset
struct ConfigPreset {
    std::string name;
    std::string description;
    std::string author;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastModified;
    std::string tags;
    bool isPublic;
    std::unordered_map<std::string, ExpertConfigValue> values;
    std::vector<std::string> basePresets;
    std::string version;
    std::unordered_map<std::string, std::string> metadata;
    
    ConfigPreset() : isPublic(true) {
        created = std::chrono::system_clock::now();
        lastModified = created;
    }
    ConfigPreset(const std::string& name, const std::string& description) 
        : name(name), description(description), isPublic(true) {
        created = std::chrono::system_clock::now();
        lastModified = created;
    }
};

// Configuration diff result
struct ConfigDiffResult {
    std::string sourcePath;
    std::string targetPath;
    std::unordered_map<std::string, std::pair<ExpertConfigValue, ExpertConfigValue>> addedKeys;
    std::unordered_map<std::string, std::pair<ExpertConfigValue, ExpertConfigValue>> removedKeys;
    std::unordered_map<std::string, std::pair<ExpertConfigValue, ExpertConfigValue>> modifiedKeys;
    std::unordered_map<std::string, ExpertConfigValue> unchangedKeys;
    std::string diffString;
    bool isIdentical;
    double similarityScore;
    
    ConfigDiffResult() : isIdentical(true), similarityScore(1.0) {}
};

// Configuration history entry
struct ConfigHistoryEntry {
    std::string version;
    std::chrono::system_clock::time_point timestamp;
    std::string author;
    std::string operation; // "set", "load", "save", "merge", "validate"
    std::string description;
    std::string diff; // String representation of changes
    std::unordered_map<std::string, ExpertConfigValue> changes;
    std::unordered_map<std::string, ExpertConfigValue> snapshot; // Full configuration snapshot
    std::string checksum;
    
    ConfigHistoryEntry() {
        timestamp = std::chrono::system_clock::now();
    }
};

// Configuration audit entry
struct ConfigAuditEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string userId;
    std::string operation; // "read", "write", "execute", "validate", "export", "import"
    std::string target; // "config_key", "config_file", "config_preset"
    std::string action; // "view", "modify", "create", "delete", "execute"
    std::string ipAddress;
    std::string userAgent;
    std::string result; // "success", "failure", "partial"
    std::string details;
    std::string checksum;
    std::string sessionId;
    std::unordered_map<std::string, std::string> metadata;
    
    ConfigAuditEntry() {
        timestamp = std::chrono::system_clock::now();
    }
};

// Expert configuration management class
class ExpertConfig {
public:
    ExpertConfig();
    ~ExpertConfig();
    
    // Disable copying, enable moving
    ExpertConfig(const ExpertConfig&) = delete;
    ExpertConfig& operator=(const ExpertConfig&) = delete;
    ExpertConfig(ExpertConfig&&) noexcept;
    ExpertConfig& operator=(ExpertConfig&&) noexcept;
    
    // Configuration value management
    void set(const std::string& key, const ExpertConfigValue& value);
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, int value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);
    
    ExpertConfigValue get(const std::string& key) const;
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    std::vector<std::string> getKeysByType(const std::string& type) const;
    
    // Configuration templates
    std::string createTemplate(const ConfigTemplate& templateConfig);
    bool updateTemplate(const std::string& templateName, const ConfigTemplate& newTemplate);
    bool deleteTemplate(const std::string& templateName);
    bool addTemplate(const ConfigTemplate& templateConfig);
    std::optional<ConfigTemplate> getTemplate(const std::string& templateName) const;
    std::vector<ConfigTemplate> getAllTemplates() const;
    std::vector<ConfigTemplate> getTemplatesByCategory(const std::string& category) const;
    std::vector<ConfigTemplate> getTemplatesByType(const std::string& type) const;
    
    // Configuration presets
    std::string createPreset(const ConfigPreset& preset);
    bool updatePreset(const std::string& presetName, const ConfigPreset& newPreset);
    bool deletePreset(const std::string& presetName);
    bool addPreset(const ConfigPreset& preset);
    std::optional<ConfigPreset> getPreset(const std::string& presetName) const;
    std::vector<ConfigPreset> getAllPresets() const;
    std::vector<ConfigPreset> getPresetsByAuthor(const std::string& author) const;
    std::vector<ConfigPreset> getPresetsByTags(const std::string& tags) const;
    
    // Configuration inheritance
    bool applyPreset(const std::string& presetName);
    bool mergePreset(const std::string& presetName, bool overwrite = true);
    std::unordered_map<std::string, ExpertConfigValue> inheritFromPreset(const std::string& presetName) const;
    bool resolveDependencies(const std::string& templateName);
    
    // Configuration comparison and diffing
    ConfigDiffResult diffWith(const std::string& otherConfigPath) const;
    ConfigDiffResult diffWith(const ExpertConfig& otherConfig) const;
    double calculateSimilarity(const std::string& otherConfigPath) const;
    std::string generateDiffReport(const ConfigDiffResult& diff) const;
    
    // Configuration history and versioning
    bool enableHistory(bool enable);
    bool addHistoryEntry(const ConfigHistoryEntry& entry);
    bool addHistoryEntry(const std::string& version, const std::string& author, const std::string& operation, const std::string& description);
    std::optional<ConfigHistoryEntry> getHistoryEntry(const std::string& version) const;
    std::vector<ConfigHistoryEntry> getHistory() const;
    std::vector<ConfigHistoryEntry> getHistoryByOperation(const std::string& operation) const;
    std::vector<ConfigHistoryEntry> getHistoryByAuthor(const std::string& author) const;
    bool rollback(const std::string& version);
    bool createVersion(const std::string& version, const std::string& author, const std::string& description);
    bool compareVersions(const std::string& version1, const std::string& version2);
    
    // Configuration audit trail
    bool enableAudit(bool enable);
    bool logAudit(const ConfigAuditEntry& entry);
    bool logAudit(const std::string& userId, const std::string& operation, const std::string& target, const std::string& action, const std::string& result, const std::string& details = "");
    std::vector<ConfigAuditEntry> getAuditLogs() const;
    std::vector<ConfigAuditEntry> getAuditLogsByUser(const std::string& userId) const;
    std::vector<ConfigAuditEntry> getAuditLogsByOperation(const std::string& operation) const;
    std::vector<ConfigAuditEntry> getAuditLogsByTimeRange(const std::chrono::system_clock::time_point& start, const std::chrono::system_clock::time_point& end) const;
    bool exportAuditLogs(const std::string& filepath, const std::string& format = "json") const;
    
    // Configuration validation
    bool validate(const std::string& key) const;
    bool validateStrict(const std::string& key) const;
    bool validateAll() const;
    bool validateAllStrict() const;
    std::string validateAndReport() const;
    std::string validateStrictAndReport() const;
    
    // Configuration export/import
    bool exportConfig(const std::string& filepath, const std::string& format = "json", bool includeSecrets = false) const;
    bool importConfig(const std::string& filepath, const std::string& format = "auto", bool merge = true, bool validateOnImport = true);
    std::string exportConfigString(const std::string& format = "json", bool includeSecrets = false) const;
    bool importConfigString(const std::string& configString, const std::string& format = "auto", bool merge = true, bool validateOnImport = true);
    
    // Configuration comparison
    ConfigDiffResult compare(const std::string& otherConfigPath) const;
    ConfigDiffResult compare(const ExpertConfig& otherConfig) const;
    std::string compareAndReport(const std::string& otherConfigPath) const;
    std::string compareAndReport(const ExpertConfig& otherConfig) const;
    
    // Configuration profiling
    std::string profile() const;
    std::string profilePerformance() const;
    std::string profileMemory() const;
    std::string profileValidation() const;
    std::string profileDependencies() const;
    
    // Configuration optimization
    bool optimize();
    bool optimizeMemory();
    bool optimizePerformance();
    bool optimizeValidation();
    bool optimizeDependencies();
    std::string getOptimizationReport() const;
    
    // Configuration analysis
    std::string analyze() const;
    std::string analyzeUsage() const;
    std::string analyzeDependencies() const;
    std::string analyzePerformance() const;
    std::string analyzeMemory() const;
    std::string analyzeValidation() const;
    std::unordered_map<std::string, double> calculateMetrics() const;
    
    // Configuration scripting
    bool executeScript(const std::string& script, const std::string& language = "javascript");
    std::string evaluateScript(const std::string& script, const std::string& language = "javascript") const;
    bool loadScriptFromFile(const std::string& filepath, const std::string& language = "auto");
    bool registerCustomFunction(const std::string& name, const std::function<ExpertConfigValue(const std::vector<ExpertConfigValue>&)>& func);
    
    // Configuration macros and templates
    std::string defineMacro(const std::string& name, const std::string& templateString);
    ExpertConfigValue expandMacro(const std::string& macroName, const std::unordered_map<std::string, ExpertConfigValue>& context) const;
    bool loadMacroFile(const std::string& filepath);
    bool registerTemplateFunction(const std::string& name, const std::function<ExpertConfigValue(const std::vector<ExpertConfigValue>&)>& func);
    
    // Configuration secrets and environment variable handling
    bool setSecret(const std::string& key, const std::string& value, bool encrypt = true);
    std::string getSecret(const std::string& key) const;
    bool isSecret(const std::string& key) const;
    bool removeSecret(const std::string& key);
    bool setEnvironmentVariable(const std::string& key, const std::string& envVarName);
    bool isFromEnvironment(const std::string& key) const;
    std::string getEnvironmentVariable(const std::string& key) const;
    
    // Configuration hot-reloading and live updates
    bool enableHotReload(bool enable);
    bool addHotReloadListener(const std::function<void(const std::string&, const ExpertConfigValue&)>& listener);
    bool removeHotReloadListener(const std::string& listenerId);
    bool notifyHotReload(const std::string& key, const ExpertConfigValue& value);
    bool startHotReloadServer(int port = 8080);
    bool stopHotReloadServer();
    bool isHotReloadEnabled() const;
    
    // Configuration distribution
    bool distribute(const std::string& target, const std::string& protocol = "http");
    bool receiveFrom(const std::string& source, const std::string& protocol = "http");
    bool syncWith(const std::string& otherConfigPath);
    bool createCluster(const std::vector<std::string>& nodes);
    bool joinCluster(const std::string& nodeId);
    bool leaveCluster();
    bool isInCluster() const;
    std::vector<std::string> getClusterNodes() const;
    
    // Advanced configuration types
    // Neural network hyperparameter optimization
    bool addNeuralNetworkConfig(const std::string& name, const std::unordered_map<std::string, ExpertConfigValue>& params);
    std::optional<std::unordered_map<std::string, ExpertConfigValue>> getNeuralNetworkConfig(const std::string& name) const;
    std::vector<std::string> getNeuralNetworkConfigs() const;
    bool optimizeNeuralNetwork(const std::string& name, const std::string& objective = "accuracy");
    
    // Simulation parameter sets
    bool addSimulationConfig(const std::string& name, const std::unordered_map<std::string, ExpertConfigValue>& params);
    std::optional<std::unordered_map<std::string, ExpertConfigValue>> getSimulationConfig(const std::string& name) const;
    std::vector<std::string> getSimulationConfigs() const;
    bool optimizeSimulation(const std::string& name, const std::string& objective = "performance");
    
    // Experiment configuration bundles
    bool addExperimentConfig(const std::string& name, const std::unordered_map<std::string, ExpertConfigValue>& params);
    std::optional<std::unordered_map<std::string, ExpertConfigValue>> getExperimentConfig(const std::string& name) const;
    std::vector<std::string> getExperimentConfigs() const;
    bool optimizeExperiment(const std::string& name, const std::string& objective = "result");
    
    // Performance tuning profiles
    bool addPerformanceProfile(const std::string& name, const std::unordered_map<std::string, ExpertConfigValue>& params);
    std::optional<std::unordered_map<std::string, ExpertConfigValue>> getPerformanceProfile(const std::string& name) const;
    std::vector<std::string> getPerformanceProfiles() const;
    bool optimizePerformanceProfile(const std::string& name, const std::string& objective = "speed");
    
    // Memory management profiles
    bool addMemoryProfile(const std::string& name, const std::unordered_map<std::string, ExpertConfigValue>& params);
    std::optional<std::unordered_map<std::string, ExpertConfigValue>> getMemoryProfile(const std::string& name) const;
    std::vector<std::string> getMemoryProfiles() const;
    bool optimizeMemoryProfile(const std::string& name, const std::string& objective = "efficiency");
    
    // Configuration UI integration
    std::string generateWebUI() const;
    bool startWebUI(int port = 8080);
    bool stopWebUI();
    bool isWebUIEnabled() const;
    
    // Expert configuration commands (CLI integration)
    bool validateCommand(const std::string& format = "json") const;
    bool validateStrictCommand(const std::string& format = "json") const;
    bool exportCommand(const std::string& filepath, const std::string& format = "json") const;
    bool importCommand(const std::string& filepath, const std::string& format = "auto") const;
    bool diffCommand(const std::string& otherConfigPath) const;
    bool compareCommand(const std::string& otherConfigPath) const;
    bool profileCommand(const std::string& outputFormat = "text") const;
    bool optimizeCommand(const std::string& optimizationType = "all") const;
    bool analyzeCommand(const std::string& analysisType = "all") const;
    
    // Integration with existing systems
    bool integrateWithCLI();
    bool integrateWithValidation();
    bool integrateWithLogging();
    bool integrateWithSerialization();
    bool integrateWithAdvancedCLI();
    
    // Backward compatibility with existing Config API
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    template<typename T> std::optional<T> get(const std::string& key) const;
    template<typename T> T getOr(const std::string& key, const T& defaultValue) const;
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void setFromEnvironment(const std::string& key, const std::string& envVarName);
    bool isFromEnvironment(const std::string& key) const;
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;
    
    // Expert configuration statistics
    std::string getStatistics() const;
    std::string getHealthStatus() const;
    bool isHealthy() const;
    bool enablePerformanceMonitoring(bool enable);
    bool enableMemoryTracking(bool enable);
    bool enableAuditLogging(bool enable);
    bool enableHotReload(bool enable);
    
    // Configuration backup and restore
    bool createBackup(const std::string& name = "");
    bool restoreBackup(const std::string& name = "latest");
    std::vector<std::string> getBackups() const;
    bool deleteBackup(const std::string& name);
    
    // Configuration synchronization
    bool syncTo(const std::string& target, const std::string& protocol = "http");
    bool syncFrom(const std::string& source, const std::string& protocol = "http");
    bool isSynced() const;
    std::string getSyncStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    void initialize();
    void cleanup();
    void logOperation(const std::string& operation, const std::string& target, const std::string& result, const std::string& details = "");
    std::string generateChecksum(const std::unordered_map<std::string, ExpertConfigValue>& config) const;
    bool validateConfigStructure() const;
    std::unordered_map<std::string, ExpertConfigValue> convertToExpertConfig(const std::unordered_map<std::string, ConfigValue>& config) const;
    std::unordered_map<std::string, ConfigValue> convertFromExpertConfig(const std::unordered_map<std::string, ExpertConfigValue>& config) const;
};

// Expert configuration utility functions
namespace expert_config {
    // Script execution support
    bool executeJavaScript(const std::string& script, ExpertConfig& config);
    bool executeLua(const std::string& script, ExpertConfig& config);
    bool executePython(const std::string& script, ExpertConfig& config);
    
    // Configuration comparison utilities
    ConfigDiffResult compareConfigs(const ExpertConfig& config1, const ExpertConfig& config2);
    double calculateSimilarity(const ExpertConfig& config1, const ExpertConfig& config2);
    
    // Configuration analysis utilities
    std::unordered_map<std::string, double> analyzeConfig(const ExpertConfig& config);
    std::string generateConfigReport(const ExpertConfig& config);
    
    // Configuration optimization utilities
    bool optimizeConfig(ExpertConfig& config, const std::string& objective = "performance");
    std::unordered_map<std::string, ExpertConfigValue> getOptimalConfig(const ExpertConfig& config, const std::string& objective = "performance");
    
    // Configuration template utilities
    bool createTemplateFromConfig(const std::string& templateName, const ExpertConfig& config);
    ExpertConfig createConfigFromTemplate(const std::string& templateName);
    
    // Configuration history utilities
    bool createVersionFromConfig(ExpertConfig& config, const std::string& version, const std::string& author, const std::string& description);
    ExpertConfig restoreVersion(const ExpertConfig& config, const std::string& version);
    
    // Configuration audit utilities
    bool createAuditReport(const std::vector<ConfigAuditEntry>& auditLogs, const std::string& outputFile, const std::string& format = "json");
    bool createAuditSummary(const std::vector<ConfigAuditEntry>& auditLogs, const std::string& outputFile, const std::string& format = "text");
    
    // Configuration distribution utilities
    bool distributeConfig(const ExpertConfig& config, const std::string& target, const std::string& protocol = "http");
    ExpertConfig receiveConfig(const std::string& source, const std::string& protocol = "http");
    
    // Configuration validation utilities
    bool validateConfig(const ExpertConfig& config, const std::string& validationRule = "");
    bool validateConfigStrict(const ExpertConfig& config, const std::string& validationRule = "");
    std::string validateConfigAndReport(const ExpertConfig& config, const std::string& validationRule = "");
    
    // Configuration export/import utilities
    bool exportConfig(const ExpertConfig& config, const std::string& filepath, const std::string& format = "json", bool includeSecrets = false);
    ExpertConfig importConfig(const std::string& filepath, const std::string& format = "auto", bool merge = true, bool validateOnImport = true);
}

} // namespace nlm

// Convenience functions for ExpertConfig
namespace nlm {
    // Create ExpertConfig from existing Config
    ExpertConfig createExpertConfig(const Config& config);
    
    // Create ExpertConfig from file
    ExpertConfig loadExpertConfig(const std::string& filepath, const std::string& format = "auto");
    
    // Create ExpertConfig from JSON string
    ExpertConfig loadExpertConfigFromJSON(const std::string& jsonString);
    
    // Create ExpertConfig from YAML string
    ExpertConfig loadExpertConfigFromYAML(const std::string& yamlString);
    
    // Create ExpertConfig from key=value string
    ExpertConfig loadExpertConfigFromKeyValue(const std::string& keyValueString);
    
    // Export ExpertConfig to existing Config
    Config exportToConfig(const ExpertConfig& expertConfig);
    
    // Merge ExpertConfig into existing Config
    bool mergeIntoConfig(Config& config, const ExpertConfig& expertConfig, bool overwrite = true);
}