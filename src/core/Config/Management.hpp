#pragma once

#include "Config.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <fstream>
#include <sstream>

namespace nlm {

// Configuration migration utilities
class ConfigMigrator {
public:
    struct MigrationStep {
        std::string version;
        std::function<void(Config&)> migration_function;
        std::string description;
    };
    
    ConfigMigrator() = default;
    explicit ConfigMigrator(const std::string& from_version, const std::string& to_version);
    
    std::string from_version;
    std::string to_version;
    std::vector<MigrationStep> steps;
    
    void addMigrationStep(const std::string& version, 
                         const std::function<void(Config&)>& migration_function,
                         const std::string& description);
    bool migrate(Config& config) const;
    std::vector<std::string> getMigrationSteps() const;
};

// Configuration comparison utilities
class ConfigComparator {
public:
    struct ComparisonResult {
        std::string key;
        ConfigValue old_value;
        ConfigValue new_value;
        ConfigSource source;
        bool has_changed() const {
            return old_value != new_value;
        }
    };
    
    ConfigComparator() = default;
    explicit ConfigComparator(const std::string& comparison_type);
    
    std::string comparison_type;
    bool include_defaults = false;
    bool include_environment = false;
    bool include_command_line = false;
    
    std::vector<ComparisonResult> compare(const Config& old_config, const Config& new_config) const;
    double getSimilarityScore(const Config& old_config, const Config& new_config) const;
    std::vector<std::string> getChangedKeys(const Config& old_config, const Config& new_config) const;
    std::vector<std::string> getNewKeys(const Config& old_config, const Config& new_config) const;
    std::vector<std::string> getRemovedKeys(const Config& old_config, const Config& new_config) const;
};

// Configuration validation utilities
class ConfigValidationReport {
public:
    struct Issue {
        enum class Severity {
            Info,
            Warning,
            Error,
            Critical
        };
        
        std::string key;
        std::string message;
        Severity severity;
        std::string recommendation;
        
        Issue(const std::string& k, const std::string& msg, Severity sev = Severity::Warning,
              const std::string& rec = "") : key(k), message(msg), severity(sev), recommendation(rec) {}
    };
    
    ConfigValidationReport() = default;
    explicit ConfigValidationReport(const std::string& report_name) : name(report_name) {}
    
    std::string name;
    std::vector<Issue> issues;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    bool is_valid() const { return errors.empty(); }
    bool has_warnings() const { return !warnings.empty(); }
    
    void addIssue(const std::string& key, const std::string& message,
                  Issue::Severity severity = Issue::Severity::Warning,
                  const std::string& recommendation = "");
    void addWarning(const std::string& message);
    void addError(const std::string& message);
    std::string generateReport() const;
    std::string generateJSONReport() const;
    std::string generateHTMLReport() const;
};

// Configuration template manager
class ConfigTemplateManager {
public:
    struct Template {
        std::string name;
        std::string description;
        std::string version;
        std::string author;
        std::map<std::string, std::string> tags;
        std::map<std::string, ConfigValue> defaults;
        std::string file_path;
        bool is_builtin = false;
        
        Template(const std::string& n, const std::string& desc = "", 
                 const std::string& ver = "1.0", const std::string& auth = "")
            : name(n), description(desc), version(ver), author(auth), is_builtin(true) {}
    };
    
    ConfigTemplateManager();
    
    std::vector<Template> getBuiltInTemplates() const;
    Template loadTemplate(const std::string& name) const;
    bool saveTemplate(const Template& template);
    bool deleteTemplate(const std::string& name);
    std::vector<std::string> getAvailableTemplates() const;
    
    // Built-in template loaders
    static Config loadDevelopmentTemplate(const Config& base_config = Config());
    static Config loadTestingTemplate(const Config& base_config = Config());
    static Config loadProductionTemplate(const Config& base_config = Config());
    static Config loadMinimalTemplate();
    static Config loadOptimizedTemplate();
};

// Configuration history manager
class ConfigHistoryManager {
public:
    struct ConfigSnapshot {
        std::string timestamp;
        std::string version;
        Config config;
        std::map<std::string, std::string> metadata;
        
        ConfigSnapshot(const Config& c, const std::string& ver = "1.0")
            : version(ver), config(c) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
            timestamp = ss.str();
        }
    };
    
    ConfigHistoryManager() = default;
    explicit ConfigHistoryManager(const std::string& history_file);
    
    std::string history_file;
    std::vector<ConfigSnapshot> snapshots;
    size_t max_history_size = 100;
    bool auto_save = true;
    
    void saveSnapshot(const Config& config, const std::string& version = "1.0",
                      const std::map<std::string, std::string>& metadata = {});
    ConfigSnapshot getSnapshot(size_t index) const;
    ConfigSnapshot getLatestSnapshot() const;
    std::vector<ConfigSnapshot> getHistory() const { return snapshots; }
    
    Config rollbackToSnapshot(size_t index);
    Config getDiffBetweenSnapshots(size_t index1, size_t index2) const;
    
    void saveHistory() const;
    bool loadHistory();
    
    void clearHistory();
    size_t getHistorySize() const { return snapshots.size(); }
};

// Configuration monitor
class ConfigMonitor {
public:
    struct MonitorRule {
        std::string key_pattern;
        std::function<bool(const ConfigValue&)> condition;
        std::function<void(const ConfigValue&, ConfigSource)> callback;
        std::string description;
    };
    
    ConfigMonitor() = default;
    explicit ConfigMonitor(const std::string& monitor_name) : name(monitor_name) {}
    
    std::string name;
    std::vector<MonitorRule> rules;
    bool active = true;
    
    void addRule(const std::string& key_pattern,
                 const std::function<bool(const ConfigValue&)>& condition,
                 const std::function<void(const ConfigValue&, ConfigSource)>& callback,
                 const std::string& description = "");
    void checkConfig(const Config& config) const;
    void setActive(bool state) { active = state; }
    bool isActive() const { return active; }
    
    void registerConfig(Config& config);
    void unregisterConfig();
};

// Configuration factory
class ConfigFactory {
public:
    struct FactoryConfig {
        std::string name;
        std::string description;
        std::string author;
        std::string version;
        std::map<std::string, ConfigValue> defaults;
        std::vector<std::string> tags;
        std::map<std::string, std::string> metadata;
    };
    
    ConfigFactory() = default;
    explicit ConfigFactory(const std::string& name) : name(name) {}
    
    std::string name;
    std::vector<FactoryConfig> recipes;
    
    Config create(const std::string& recipe_name) const;
    bool addRecipe(const FactoryConfig& recipe);
    bool removeRecipe(const std::string& recipe_name);
    std::vector<std::string> getAvailableRecipes() const;
    FactoryConfig getRecipe(const std::string& recipe_name) const;
    
    // Common factory methods
    static Config createDevelopmentConfig();
    static Config createTestingConfig();
    static Config createProductionConfig();
    static Config createMinimalConfig();
    static Config createOptimizedConfig();
};

// Configuration exporter
class ConfigExporter {
public:
    enum class ExportFormat {
        Text,
        JSON,
        YAML,
        TOML,
        XML,
        CSV,
        SQL
    };
    
    ConfigExporter() = default;
    explicit ConfigExporter(const std::string& export_name) : name(export_name) {}
    
    std::string name;
    ExportFormat format = ExportFormat::Text;
    bool pretty_print = false;
    bool include_metadata = false;
    std::vector<std::string> include_keys;
    std::vector<std::string> exclude_keys;
    
    std::string exportConfig(const Config& config) const;
    bool exportToFile(const Config& config, const std::string& filepath) const;
    
    // Format-specific exporters
    std::string exportToText(const Config& config) const;
    std::string exportToJSON(const Config& config) const;
    std::string exportToYAML(const Config& config) const;
    std::string exportToTOML(const Config& config) const;
    std::string exportToXML(const Config& config) const;
    std::string exportToCSV(const Config& config) const;
    std::string exportToSQL(const Config& config) const;
    
    void setFormat(ExportFormat fmt) { format = fmt; }
    void setPrettyPrint(bool enable) { pretty_print = enable; }
    void setIncludeMetadata(bool include) { include_metadata = include; }
    void setIncludeKeys(const std::vector<std::string>& keys) { include_keys = keys; }
    void setExcludeKeys(const std::vector<std::string>& keys) { exclude_keys = keys; }
    
    static std::string exportFormatToString(ExportFormat format);
    static ExportFormat parseFormatFromString(const std::string& format_str);
};

// Configuration importer
class ConfigImporter {
public:
    struct ImportOptions {
        bool override_existing = true;
        ConfigSource source = ConfigSource::File;
        bool validate_during_import = true;
        std::vector<std::string> required_keys;
        std::map<std::string, std::string> key_mappings;
        bool merge_hierarchical = true;
    };
    
    ConfigImporter() = default;
    explicit ConfigImporter(const std::string& import_name) : name(import_name) {}
    
    std::string name;
    ImportOptions options;
    
    Config importFromFile(const std::string& filepath) const;
    Config importFromText(const std::string& text) const;
    Config importFromJSON(const std::string& json_text) const;
    Config importFromYAML(const std::string& yaml_text) const;
    Config importFromTOML(const std::string& toml_text) const;
    
    void setOptions(const ImportOptions& opts) { options = opts; }
    ImportOptions getOptions() const { return options; }
    
    // Format detection
    static ExportFormat detectFormat(const std::string& content);
    static bool isJSON(const std::string& content);
    static bool isYAML(const std::string& content);
    static bool isTOML(const std::string& content);
    static bool isText(const std::string& content);
};

// Configuration diff tool
class ConfigDiffTool {
public:
    struct DiffOptions {
        bool ignore_order = true;
        bool ignore_metadata = true;
        bool ignore_source = true;
        bool hierarchical_mode = false;
        std::string key_prefix = "";
    };
    
    ConfigDiffTool() = default;
    explicit ConfigDiffTool(const std::string& diff_name) : name(diff_name) {}
    
    std::string name;
    DiffOptions options;
    
    std::vector<std::pair<std::string, std::string>> diff(const Config& config1, const Config& config2) const;
    Config unifiedDiff(const Config& config1, const Config& config2) const;
    double similarity(const Config& config1, const Config& config2) const;
    
    void setOptions(const DiffOptions& opts) { options = opts; }
    DiffOptions getOptions() const { return options; }
    
    // Common diff scenarios
    static std::vector<std::string> findBreakingChanges(const Config& old_config, const Config& new_config);
    static std::vector<std::string> findDeprecatedKeys(const Config& old_config, const Config& new_config);
    static std::vector<std::string> findNewKeys(const Config& old_config, const Config& new_config);
    static std::vector<std::string> findRemovedKeys(const Config& old_config, const Config& new_config);
};

} // namespace nlm