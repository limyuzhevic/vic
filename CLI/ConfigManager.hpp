// NLM Configuration Manager
// Advanced configuration management with JSON/YAML/XML support

#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <any>
#include <functional>
#include <filesystem>
#include <variant>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// Configuration value types
using ConfigValue = std::variant<
    std::string,
    int,
    float,
    double,
    bool,
    std::vector<std::string>,
    std::vector<int>,
    std::vector<float>,
    std::vector<double>,
    std::vector<bool>
>;

// Configuration section
struct ConfigSection {
    std::string name;
    std::map<std::string, ConfigValue> values;
    std::map<std::string, ConfigSection> subsections;
    bool isValidated;
    std::chrono::system_clock::time_point lastModified;
    
    ConfigSection(const std::string& sectionName) : name(sectionName), isValidated(false) {
        lastModified = std::chrono::system_clock::now();
    }
};

// Configuration import/export flags
enum class ImportFormat {
    JSON,
    YAML,
    XML,
    INI,
    PLAIN_TEXT
};

// Configuration merge strategies
enum class MergeStrategy {
    REPLACE,      // Replace with imported values
    MERGE,        // Merge with existing values
    INHERIT,      // Only set if not already set
    OVERRIDE,     // Override existing values
    VALIDATE_ONLY  // Only validate, don't modify
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::chrono::system_clock::time_point timestamp;
    
    ValidationResult() : valid(true), timestamp(std::chrono::system_clock::now()) {}
};

// Configuration inheritance
struct InheritanceInfo {
    std::string parentPath;
    MergeStrategy strategy;
    bool inheritSections;
    bool inheritValues;
    std::vector<std::string> excludedSections;
    std::vector<std::string> excludedValues;
    
    InheritanceInfo() : strategy(MergeStrategy::MERGE), inheritSections(true), inheritValues(true) {}
};

// Main configuration manager class
class ConfigManager {
private:
    std::map<std::string, ConfigSection> configSections;
    std::map<std::string, ConfigSection> defaultSections;
    
    // Inheritance configuration
    std::map<std::string, InheritanceInfo> inheritanceConfigs;
    
    // Import/export configuration
    ImportFormat importFormat;
    std::string backupDirectory;
    bool autoBackup;
    int backupRetentionDays;
    
    // Validation
    std::vector<std::function<bool(const ConfigSection&)>> validators;
    std::vector<std::function<bool(const std::string&, const ConfigValue&)>> valueValidators;
    
    // Configuration history
    std::vector<ConfigSection> configHistory;
    bool historyEnabled;
    int maxHistorySize;
    
    // File system
    std::filesystem::path configDir;
    std::filesystem::path defaultConfigPath;
    
    // Error handling
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // Configuration change tracking
    std::chrono::system_clock::time_point lastChange;
    bool isModified;
    
public:
    ConfigManager();
    ~ConfigManager() = default;
    
    // Configuration management
    void setConfigDir(const std::string& dir) { configDir = dir; }
    void setDefaultConfig(const std::string& path) { defaultConfigPath = path; }
    void setAutoBackup(bool enabled, int retentionDays = 30) { 
        autoBackup = enabled; 
        backupRetentionDays = retentionDays;
    }
    void setHistoryEnabled(bool enabled, int maxSize = 100) { 
        historyEnabled = enabled; 
        maxHistorySize = maxSize;
    }
    
    // Section management
    ConfigSection& getSection(const std::string& name);
    const ConfigSection& getSection(const std::string& name) const;
    bool hasSection(const std::string& name) const;
    void createSection(const std::string& name);
    void removeSection(const std::string& name);
    std::vector<std::string> getSectionNames() const;
    
    // Value management
    template<typename T>
    void setValue(const std::string& section, const std::string& key, const T& value);
    
    template<typename T>
    T getValue(const std::string& section, const std::string& key, const T& defaultValue = T()) const;
    
    ConfigValue getValue(const std::string& section, const std::string& key) const;
    bool hasValue(const std::string& section, const std::string& key) const;
    void removeValue(const std::string& section, const std::string& key);
    
    // Subsections
    ConfigSection& getSubsection(const std::string& parentSection, const std::string& subsectionName);
    bool hasSubsection(const std::string& parentSection, const std::string& subsectionName) const;
    void createSubsection(const std::string& parentSection, const std::string& subsectionName);
    
    // Default values
    void setDefaultValue(const std::string& section, const std::string& key, const ConfigValue& value);
    void loadDefaults();
    
    // Import/export
    bool importFromFile(const std::string& filePath, ImportFormat format = ImportFormat::JSON,
                       MergeStrategy strategy = MergeStrategy::MERGE);
    bool importFromString(const std::string& content, ImportFormat format = ImportFormat::JSON,
                         MergeStrategy strategy = MergeStrategy::MERGE);
    bool exportToFile(const std::string& filePath, ImportFormat format = ImportFormat::JSON) const;
    std::string exportToString(ImportFormat format = ImportFormat::JSON) const;
    
    // Inheritance
    void setInheritance(const std::string& section, const InheritanceInfo& inheritance);
    bool applyInheritance(const std::string& section);
    
    // Validation
    void addValidator(std::function<bool(const ConfigSection&)> validator);
    void addValueValidator(std::function<bool(const std::string&, const ConfigValue&)> validator);
    ValidationResult validate() const;
    ValidationResult validateSection(const std::string& section) const;
    ValidationResult validateValue(const std::string& section, const std::string& key) const;
    
    // Session management
    bool saveSession(const std::string& sessionPath) const;
    bool loadSession(const std::string& sessionPath);
    bool compareSessions(const std::string& sessionPath1, const std::string& sessionPath2) const;
    std::string generateSessionChecksum(const std::string& sessionPath) const;
    
    // History
    void saveToHistory();
    std::vector<ConfigSection> getHistory() const { return configHistory; }
    bool restoreFromHistory(size_t index);
    
    // Configuration operations
    void clear();
    void resetToDefaults();
    bool isModified() const { return isModified; }
    std::chrono::system_clock::time_point getLastChange() const { return lastChange; }
    
    // Error handling
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    void clearErrors() { errors.clear(); }
    void clearWarnings() { warnings.clear(); }
    
    // System information
    size_t getSectionCount() const { return configSections.size(); }
    size_t getValueCount() const;
    size_t getSubsectionCount() const;
    
    // CLI commands
    bool saveConfig(const std::string& path) const;
    bool loadConfig(const std::string& path);
    bool listConfigs() const;
    bool validateConfig() const;
    bool mergeConfig(const std::string& sourcePath);
    bool diffConfig(const std::string& sourcePath) const;
    
    // Convenience methods for common configuration options
    void setupBrainOptions();
    void setupMemoryOptions();
    void setupPlasticityOptions();
    void setupDevelopmentOptions();
    void setupNeuromodulationOptions();
    void setupPerformanceOptions();
    void setupOutputOptions();
    void setupDebugOptions();
    void setupLoggingOptions();
    void setupExperimentOptions();
    
    // Configuration group helpers
    void setupBrainConfiguration(const std::string& section = "brain");
    void setupMemoryConfiguration(const std::string& section = "memory");
    void setupPlasticityConfiguration(const std::string& section = "plasticity");
    void setupDevelopmentConfiguration(const std::string& section = "development");
    void setupNeuromodulationConfiguration(const std::string& section = "neuromodulation");
    void setupPerformanceConfiguration(const std::string& section = "performance");
    void setupOutputConfiguration(const std::string& section = "output");
    void setupDebugConfiguration(const std::string& section = "debug");
    void setupLoggingConfiguration(const std::string& section = "logging");
    void setupExperimentConfiguration(const std::string& section = "experiment");
    
private:
    void markAsModified();
    ConfigValue convertToConfigValue(const std::any& value) const;
    std::any convertFromConfigValue(const ConfigValue& value) const;
    void addDefaultSection(const std::string& name, const ConfigSection& section);
    std::string configValueToString(const ConfigValue& value) const;
    bool stringToConfigValue(const std::string& str, ConfigValue& outValue) const;
};

} // namespace nlm

#endif // CONFIG_MANAGER_HPP
