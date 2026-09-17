// NLM Configuration Management and Debugging Tools
// Enhanced configuration management with debugging capabilities

#ifndef NLM_CONFIG_TOOLS_HPP
#define NLM_CONFIG_TOOLS_HPP

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <algorithm>
#include <filesystem>

namespace nlm {

/**
 * Configuration Error with actionable guidance
 */
struct ConfigErrorWithGuidance {
    std::string message;
    std::string key;
    std::string suggestedFix;
    std::string helpUrl;
    enum class Severity { Info, Warning, Error, Critical } severity;
};

/**
 * Configuration Diff tool for comparing configurations
 */
class ConfigDiff {
public:
    struct Change {
        std::string key;
        std::string oldValue;
        std::string newValue;
        ConfigSource source;
        std::string description;
    };

    static std::vector<Change> compare(const Config& config1, const Config& config2);
    static std::string generateDiffReport(const std::vector<Change>& changes);
};

/**
 * Configuration Validator with suggestions
 */
class ConfigValidator {
public:
    struct ValidationIssue {
        std::string key;
        std::string message;
        std::string suggestion;
        bool isWarning;
    };

    static std::vector<ValidationIssue> validate(const Config& config);
    static std::string generateValidationReport(const std::vector<ValidationIssue>& issues);
    static bool canAutoFix(const ValidationIssue& issue);
};

/**
 * Configuration Hot-swapping manager
 */
class RuntimeConfigManager {
private:
    std::shared_ptr<Config> baseConfig_;
    std::shared_ptr<Config> activeConfig_;
    std::shared_ptr<Config> overlayConfig_;
    bool autoSaveEnabled_;
    std::string backupDirectory_;
    std::map<std::string, std::chrono::steady_clock::time_point> configHistory_;

public:
    RuntimeConfigManager(std::shared_ptr<Config> baseConfig);

    // Configuration operations
    bool applyOverlay(const std::string& key, const ConfigValue& value);
    bool removeOverlay(const std::string& key);
    bool clearOverlays();

    // Get current configuration
    std::shared_ptr<Config> getActiveConfig() const;
    std::shared_ptr<Config> getBaseConfig() const;
    std::shared_ptr<Config> getOverlayConfig() const;

    // Validation and checking
    std::vector<ConfigError> validate() const;
    bool isValid() const;

    // Save and backup
    bool saveToFile(const std::string& filepath, bool asBase = false);
    bool backupCurrent(const std::string& name = "");
    bool restoreBackup(const std::string& name);
    std::vector<std::string> getAvailableBackups() const;

    // Configuration profiling
    struct ConfigProfile {
        double memoryUsage;
        size_t entryCount;
        size_t keyCount;
        std::chrono::milliseconds loadTime;
        std::chrono::milliseconds accessTime;
    };

    ConfigProfile profile() const;

    // Configuration history
    bool enableHistory(bool enable);
    std::vector<std::string> getConfigHistory() const;
    bool revertToSnapshot(const std::string& timestamp);
};

/**
 * Configuration migration utility
 */
class ConfigMigration {
public:
    struct MigrationRule {
        std::string oldKeyPattern;
        std::string newKey;
        std::function<ConfigValue(const ConfigValue&)> converter;
        std::string description;
    };

    static bool migrate(std::shared_ptr<Config> config, 
                       const std::vector<MigrationRule>& rules);
    static std::vector<std::string> detectConfigFormat(const std::string& filepath);
    static bool convertFromLegacy(std::shared_ptr<Config> config, 
                                  const std::string& legacyFormat);
};

/**
 * Configuration diagnostics
 */
class ConfigDiagnostics {
public:
    struct DiagnosticResult {
        std::string component;
        bool isHealthy;
        std::string status;
        std::vector<std::string> details;
        double performanceScore;
    };

    static std::vector<DiagnosticResult> runDiagnostics(const Config& config);
    static std::string generateDiagnosticReport(const std::vector<DiagnosticResult>& results);
    static bool checkCorruption(const Config& config);
    static std::map<std::string, double> analyzeMemoryUsage(const Config& config);
};

/**
 * Configuration template system
 */
class ConfigTemplate {
private:
    struct TemplateEntry {
        std::string key;
        ConfigValue defaultValue;
        std::string description;
        bool isRequired;
        std::vector<std::string> dependsOn;
    };

    std::string name_;
    std::string description_;
    std::vector<TemplateEntry> entries_;

public:
    ConfigTemplate(const std::string& name, const std::string& description);

    void addEntry(const std::string& key, const ConfigValue& defaultValue,
                  const std::string& description = "",
                  bool isRequired = false,
                  const std::vector<std::string>& dependsOn = {});

    std::string getName() const { return name_; }
    std::string getDescription() const { return description_; }

    bool validate(const Config& config) const;
    std::shared_ptr<Config> createConfig() const;
    static std::vector<std::string> getAvailableTemplates();
    static std::shared_ptr<ConfigTemplate> loadTemplate(const std::string& name);
    static bool saveTemplate(const ConfigTemplate& template);
};

/**
 * Configuration backup manager
 */
class ConfigBackupManager {
private:
    std::string backupDirectory_;
    size_t maxBackups_;
    bool compressionEnabled_;

public:
    ConfigBackupManager(const std::string& directory, 
                       size_t maxBackups = 10,
                       bool compressionEnabled = true);

    bool createBackup(const Config& config, const std::string& name = "");
    bool restoreBackup(const std::string& name, std::shared_ptr<Config> config);
    std::vector<std::string> listBackups() const;
    bool deleteBackup(const std::string& name);
    bool cleanupOldBackups();

    struct BackupInfo {
        std::string name;
        std::chrono::system_clock::time_point timestamp;
        size_t size;
        std::string checksum;
    };

    std::vector<BackupInfo> getBackupInfo() const;
};

/**
 * Configuration profiler
 */
class ConfigProfiler {
public:
    struct ProfilingSession {
        std::chrono::steady_clock::time_point startTime;
        std::vector<std::pair<std::string, std::chrono::milliseconds>> operationTimes;
        size_t totalAccesses;
        size_t successfulAccesses;
        size_t failedAccesses;
    };

    ConfigProfiler();

    void startProfiling();
    void stopProfiling();
    void recordAccess(const std::string& operation, 
                     const std::chrono::milliseconds& duration,
                     bool success = true);
    std::string generateProfileReport() const;
    std::map<std::string, double> getPerformanceStats() const;

private:
    ProfilingSession session_;
    bool isProfiling_;
    std::mutex mutex_;
};

/**
 * Interactive configuration editor
 */
class ConfigEditor {
public:
    struct EditSession {
        std::shared_ptr<Config> config;
        std::vector<std::string> commandHistory;
        size_t currentPosition;
    };

    static bool startInteractiveEditor(std::shared_ptr<Config> config);
    static std::string generateConfigEditorHelp();
    static bool exportConfig(const Config& config, const std::string& filepath);
    static bool importConfig(std::shared_ptr<Config> config, const std::string& filepath);
    static std::string formatConfig(const Config& config, const std::string& format = "json");
    static bool parseConfig(std::shared_ptr<Config> config, const std::string& content, const std::string& format = "auto");
};

} // namespace nlm

#endif // NLM_CONFIG_TOOLS_HPP
