#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <thread>
#include <filesystem>
#include <optional>
#include <variant>
#include <typeindex>
#include "../core/Config/Config.hpp"

namespace nlm {

// Advanced configuration types
enum class ConfigChangeType {
    Added,
    Modified,
    Removed,
    Reordered,
    Validated
};

enum class ConfigInheritanceType {
    Override,
    Merge,
    Extend,
    Substitute
};

enum class ConfigValidationLevel {
    None,
    Basic,
    Strict,
    Comprehensive
};

// Configuration change tracking
struct ConfigChange {
    std::string key;
    Config oldValue;
    Config newValue;
    ConfigChangeType type;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string changedBy;
    bool isValid;
    std::string errorMessage;
    
    ConfigChange() : type(ConfigChangeType::Added), isValid(true) {}
    ConfigChange(const std::string& k, const Config& oldVal, const Config& newVal, ConfigChangeType t, const std::string& by)
        : key(k), oldValue(oldVal), newValue(newVal), type(t), changedBy(by), isValid(true) {
        timestamp = std::chrono::high_resolution_clock::now();
    }
};

// Configuration template
struct ConfigTemplate {
    std::string name;
    std::string description;
    std::vector<ConfigSchemaEntry> schema;
    Config defaultValues;
    Config inheritanceRules;
    std::string baseTemplate;
    bool isAbstract;
    std::vector<std::string> tags;
    std::string version;
    
    ConfigTemplate()
        : isAbstract(false), version("1.0") {}
    
    bool isValid() const {
        // Basic validation - check for duplicate keys
        std::unordered_set<std::string> keys;
        for (const auto& entry : schema) {
            if (keys.find(entry.key) != keys.end()) {
                return false;
            }
            keys.insert(entry.key);
        }
        return true;
    }
};

// Configuration inheritance rule
struct ConfigInheritanceRule {
    std::string key;
    std::string sourceTemplate;
    ConfigInheritanceType inheritanceType;
    bool allowOverride;
    std::function<bool(const ConfigValue&, const ConfigValue&)> mergeRule;
    
    ConfigInheritanceRule() : inheritanceType(ConfigInheritanceType::Override), allowOverride(true) {}
    ConfigInheritanceRule(const std::string& k, const std::string& source, ConfigInheritanceType type, bool override = true)
        : key(k), sourceTemplate(source), inheritanceType(type), allowOverride(override) {}
};

// Advanced validation rule
struct AdvancedValidationRule {
    std::string ruleId;
    std::string description;
    ConfigValidationLevel level;
    std::function<bool(const Config& config, const std::string& key, const ConfigValue& value)> validator;
    std::string errorMessage;
    bool isCrossValidation; // Validation between multiple keys
    std::vector<std::string> dependentKeys; // Keys this rule depends on
    std::string severity; // "error", "warning", "info"
    
    AdvancedValidationRule()
        : level(ConfigValidationLevel::Basic), isCrossValidation(false), severity("error") {}
    
    AdvancedValidationRule(const std::string& id, const std::string& desc, 
                          ConfigValidationLevel lvl,
                          const std::function<bool(const Config&, const std::string&, const ConfigValue&)>& validator,
                          const std::string& errorMsg = "Validation failed")
        : ruleId(id), description(desc), level(lvl), validator(validator), 
          errorMessage(errorMsg), isCrossValidation(false), severity("error") {}
};

// Configuration scripting
struct ConfigScript {
    std::string name;
    std::string description;
    std::string language; // "lua", "python", "javascript", "expression"
    std::string code;
    bool enabled;
    std::vector<std::string> triggers; // When to execute
    std::map<std::string, std::string> variables;
    
    ConfigScript()
        : enabled(true), language("expression") {}
    
    virtual Config execute(const Config& currentConfig) const {
        // Default expression evaluation
        // This is a placeholder - real implementation would parse the language
        return currentConfig;
    }
};

// Configuration history entry
struct ConfigHistoryEntry {
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string sessionId;
    Config config;
    std::string description;
    std::string changedBy;
    
    ConfigHistoryEntry() {}
    ConfigHistoryEntry(const Config& cfg, const std::string& desc = "", const std::string& by = "system")
        : config(cfg), description(desc), changedBy(by) {
        timestamp = std::chrono::high_resolution_clock::now();
    }
};

// Advanced configuration manager
class AdvancedConfig {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Disable copying
    AdvancedConfig(const AdvancedConfig&) = delete;
    AdvancedConfig& operator=(const AdvancedConfig&) = delete;
    
    // Singleton access
    static AdvancedConfig& getInstance() {
        static AdvancedConfig instance;
        return instance;
    }
    
    // Configuration templates management
    bool createTemplate(const ConfigTemplate& template);
    bool updateTemplate(const ConfigTemplate& template);
    bool deleteTemplate(const std::string& templateName);
    bool loadTemplate(const std::string& templateName, Config& outConfig) const;
    std::vector<ConfigTemplate> getAllTemplates() const;
    ConfigTemplate* getTemplate(const std::string& templateName);
    const ConfigTemplate* getTemplate(const std::string& templateName) const;
    
    // Configuration inheritance
    bool applyInheritance(const std::string& targetTemplate, const std::string& sourceTemplate);
    bool applyInheritanceChain(const std::string& targetTemplate, const std::vector<std::string>& inheritanceChain);
    Config resolveInheritance(const std::string& templateName) const;
    
    // Advanced validation
    bool addValidationRule(const AdvancedValidationRule& rule);
    bool removeValidationRule(const std::string& ruleId);
    bool updateValidationRule(const AdvancedValidationRule& rule);
    std::vector<AdvancedValidationRule> getValidationRules() const;
    AdvancedValidationRule* getValidationRule(const std::string& ruleId);
    bool validateConfig(const Config& config, ConfigValidationLevel level = ConfigValidationLevel::Strict) const;
    
    // Runtime configuration modification
    bool setValue(const std::string& key, const ConfigValue& value, const std::string& description = "");
    bool setValue(const std::string& key, int value, const std::string& description = "");
    bool setValue(const std::string& key, double value, const std::string& description = "");
    bool setValue(const std::string& key, bool value, const std::string& description = "");
    bool setValue(const std::string& key, const std::string& value, const std::string& description = "");
    bool setValue(const std::string& key, const std::vector<int>& value, const std::string& description = "");
    bool setValue(const std::string& key, const std::vector<double>& value, const std::string& description = "");
    bool setValue(const std::string& key, const std::vector<std::string>& value, const std::string& description = "");
    
    // Configuration scripting
    bool addScript(const ConfigScript& script);
    bool removeScript(const std::string& scriptName);
    bool updateScript(const ConfigScript& script);
    std::vector<ConfigScript> getScripts() const;
    bool executeScript(const std::string& scriptName, const Config& baseConfig, Config& outConfig) const;
    Config executeAllScripts(const Config& baseConfig) const;
    
    // Change tracking and history
    std::vector<ConfigChange> getRecentChanges(size_t maxChanges = 100) const;
    ConfigHistoryEntry getHistoryEntry(const std::string& sessionId, size_t index) const;
    std::vector<ConfigHistoryEntry> getHistory(const std::string& sessionId) const;
    void clearHistory(const std::string& sessionId = "");
    void startSession(const std::string& sessionId = "default");
    void endSession(const std::string& sessionId = "default");
    
    // Configuration management
    bool loadFromFile(const std::string& filepath, const std::string& templateName = "");
    bool saveToFile(const std::string& filepath) const;
    bool saveToFileWithHistory(const std::string& filepath, const std::string& description = "") const;
    bool loadFromJson(const std::string& jsonString, const std::string& templateName = "");
    std::string toJson() const;
    bool fromJson(const std::string& jsonString);
    
    // Configuration queries
    std::optional<ConfigValue> get(const std::string& key) const;
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        auto value = get(key);
        if (value.has_value() && std::holds_alternative<T>(*value)) {
            return std::get<T>(*value);
        }
        return std::nullopt;
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto value = get(key);
        if (value.has_value() && std::holds_alternative<T>(*value)) {
            return std::get<T>(*value);
        }
        return defaultValue;
    }
    
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    Config getSection(const std::string& sectionPrefix) const;
    
    // Configuration rollback and validation
    bool rollbackToChange(const ConfigChange& change);
    bool validateSingleKey(const std::string& key, const ConfigValue& value) const;
    std::vector<std::string> validateAllKeys() const;
    
    // Configuration templates
    bool createTemplateFromConfig(const std::string& templateName, const Config& config, 
                                 const std::string& description = "");
    Config exportTemplate(const std::string& templateName) const;
    bool importTemplate(const std::string& templateName, const Config& config);
    
    // Configuration comparison
    static ConfigDifference calculateDifference(const Config& oldConfig, const Config& newConfig);
    static ConfigSimilarity calculateSimilarity(const Config& config1, const Config& config2);
    
    // Session management
    void setCurrentSession(const std::string& sessionId);
    std::string getCurrentSession() const;
    std::vector<std::string> getAllSessions() const;
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Statistics and monitoring
    struct ConfigStats {
        size_t totalChanges{0};
        size_t totalValidations{0};
        size_t validationErrors{0};
        size_t templatesCount{0};
        size_t scriptsCount{0};
        uint64_t totalConfigSize{0};
        double averageValidationTime{0.0};
        std::map<std::string, size_t> changesByKey;
        std::map<std::string, size_t> validationErrorsByRule;
        
        void reset() {
            totalChanges = totalValidations = validationErrors = 0;
            templatesCount = scriptsCount = 0;
            totalConfigSize = 0;
            averageValidationTime = 0.0;
            changesByKey.clear();
            validationErrorsByRule.clear();
        }
    };
    
    ConfigStats getStats() const;
    
    // Error handling
    std::string getLastError() const { return lastError_; }
    void setLastError(const std::string& error) { lastError_ = error; }
    
    // Configuration comparison structures
    struct ConfigDifference {
        std::string key;
        ConfigChangeType type;
        ConfigValue oldValue;
        ConfigValue newValue;
        bool isSignificant;
    };
    
    struct ConfigSimilarity {
        double similarityScore{0.0};
        size_t similarKeys{0};
        size_t totalKeys{0};
        std::vector<std::string> similarKeysList;
        std::vector<std::string> differentKeysList;
    };
    
    // Legacy compatibility
    bool loadFromFileLegacy(const std::string& filepath) {
        return loadFromFile(filepath);
    }
    
    bool saveToFileLegacy(const std::string& filepath) const {
        return saveToFile(filepath);
    }
    
    std::optional<ConfigValue> getLegacy(const std::string& key) const {
        return get(key);
    }
    
    bool setLegacy(const std::string& key, const ConfigValue& value) {
        return setValue(key, value);
    }
    
private:
    // Internal helper functions
    ConfigChange recordChange(const std::string& key, const ConfigValue& oldValue, 
                             const ConfigValue& newValue, ConfigChangeType type, 
                             const std::string& description = "");
    bool validateChange(const ConfigChange& change) const;
    Config applyInheritanceToConfig(const Config& baseConfig, const Config& derivedConfig) const;
    bool executeConfigScripts(const std::vector<ConfigScript>& scripts, const Config& baseConfig, 
                            Config& outConfig) const;
    
    // Session management
    void cleanupOldSessions();
    
    // File operations
    bool loadConfigFromDirectory(const std::string& directory);
    bool saveConfigToDirectory(const std::string& directory) const;
    
    // Validation helpers
    bool validateAllRules(const Config& config, ConfigValidationLevel level) const;
    bool validateCrossValidationRules(const Config& config) const;
    
    // Change tracking
    std::vector<ConfigChange> recentChanges_;
    mutable std::mutex changesMutex_;
    
    // Template management
    std::unordered_map<std::string, ConfigTemplate> templates_;
    mutable std::mutex templatesMutex_;
    
    // Inheritance management
    std::unordered_map<std::string, std::vector<std::string>> inheritanceChains_;
    mutable std::mutex inheritanceMutex_;
    
    // Validation rules
    std::unordered_map<std::string, AdvancedValidationRule> validationRules_;
    mutable std::mutex validationMutex_;
    
    // Scripts
    std::unordered_map<std::string, ConfigScript> scripts_;
    mutable std::mutex scriptsMutex_;
    
    // Configuration history
    std::unordered_map<std::string, std::vector<ConfigHistoryEntry>> configHistory_;
    mutable std::mutex historyMutex_;
    
    // Main configuration
    Config currentConfig_;
    mutable std::mutex configMutex_;
    
    // Session management
    std::string currentSession_;
    mutable std::mutex sessionMutex_;
    
    // Statistics
    ConfigStats stats_;
    mutable std::mutex statsMutex_;
    
    // Error handling
    std::string lastError_;
    mutable std::mutex errorMutex_;
    
    // Thread synchronization
    mutable std::mutex systemMutex_;
    std::atomic<bool> isLocked_;
};

// Configuration difference and similarity calculation helper functions
inline ConfigAdvancedConfig::ConfigDifference calculateDifference(const Config& oldConfig, const Config& newConfig) {
    ConfigAdvancedConfig::ConfigDifference diff;
    
    // Get keys from both configurations
    auto oldKeys = oldConfig.getKeys();
    auto newKeys = newConfig.getKeys();
    
    // Find added keys
    for (const auto& key : newKeys) {
        if (!oldConfig.has(key)) {
            ConfigAdvancedConfig::ConfigDifference entry;
            entry.key = key;
            entry.type = ConfigChangeType::Added;
            entry.newValue = *newConfig.get(key);
            entry.isSignificant = true;
            diff.push_back(entry);
        }
    }
    
    // Find modified keys
    for (const auto& key : oldKeys) {
        if (newConfig.has(key) && oldConfig.get(key) != newConfig.get(key)) {
            ConfigAdvancedConfig::ConfigDifference entry;
            entry.key = key;
            entry.type = ConfigChangeType::Modified;
            entry.oldValue = *oldConfig.get(key);
            entry.newValue = *newConfig.get(key);
            entry.isSignificant = true;
            diff.push_back(entry);
        }
    }
    
    // Find removed keys
    for (const auto& key : oldKeys) {
        if (!newConfig.has(key)) {
            ConfigAdvancedConfig::ConfigDifference entry;
            entry.key = key;
            entry.type = ConfigChangeType::Removed;
            entry.oldValue = *oldConfig.get(key);
            entry.isSignificant = true;
            diff.push_back(entry);
        }
    }
    
    return diff;
}

inline ConfigAdvancedConfig::ConfigSimilarity calculateSimilarity(const Config& config1, const Config& config2) {
    ConfigAdvancedConfig::ConfigSimilarity similarity;
    
    auto keys1 = config1.getKeys();
    auto keys2 = config2.getKeys();
    
    similarity.totalKeys = std::max(keys1.size(), keys2.size());
    
    if (similarity.totalKeys == 0) return similarity;
    
    for (const auto& key : keys1) {
        if (config2.has(key) && config1.get(key) == config2.get(key)) {
            similarity.similarKeys++;
            similarity.similarKeysList.push_back(key);
        } else {
            similarity.differentKeysList.push_back(key);
        }
    }
    
    for (const auto& key : keys2) {
        if (!config1.has(key)) {
            similarity.differentKeysList.push_back(key);
        }
    }
    
    similarity.similarityScore = static_cast<double>(similarity.similarKeys) / similarity.totalKeys * 100.0;
    
    return similarity;
}

} // namespace nlm