#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_set>
#include <nlohmann/json.hpp>

namespace nlm {

// Configuration schema validation
class ConfigSchema {
public:
    struct ValidationRule {
        std::string name;
        std::function<bool(const nlohmann::json&)> validator;
        std::string errorMessage;
    };
    
    struct SchemaField {
        std::string name;
        nlohmann::json::value_t type;
        bool required = false;
        nlohmann::json defaultValue;
        std::string description;
        std::vector<ValidationRule> validationRules;
    };
    
    ConfigSchema(const std::string& name, const std::string& description);
    ~ConfigSchema();
    
    std::string getName() const;
    std::string getDescription() const;
    
    void addField(const SchemaField& field);
    bool validate(const nlohmann::json& config) const;
    std::vector<std::string> getErrors(const nlohmann::json& config) const;
    
    void addInheritance(const ConfigSchema& parent);
    
    nlohmann::json getDefaultConfig() const;
    
    // Schema management
    void clear();
    size_t getFieldCount() const;
    std::vector<std::string> getFieldNames() const;

private:
    std::string name_;
    std::string description_;
    std::vector<SchemaField> fields_;
    std::vector<const ConfigSchema*> inheritance_;
};

// Configuration inheritance manager
class ConfigInheritanceManager {
public:
    struct InheritanceNode {
        std::string name;
        nlohmann::json config;
        std::vector<std::string> parentNames;
        float mergeWeight = 1.0f;
    };
    
    ConfigInheritanceManager();
    ~ConfigInheritanceManager();
    
    // Node management
    void addNode(const InheritanceNode& node);
    void removeNode(const std::string& name);
    bool hasNode(const std::string& name) const;
    
    // Inheritance resolution
    nlohmann::json resolveInheritance(const std::string& name) const;
    std::vector<std::string> getInheritanceChain(const std::string& name) const;
    
    // Merge operations
    nlohmann::json mergeConfigs(const std::vector<nlohmann::json>& configs) const;
    nlohmann::json mergeConfigs(const nlohmann::json& base,
                               const nlohmann::json& override) const;
    
    // Graph operations
    void printInheritanceGraph() const;
    void exportInheritanceGraph(const std::string& filename) const;

private:
    std::map<std::string, InheritanceNode> nodes_;
    bool initialized_ = false;
};

// Configuration templates for common setups
class ConfigTemplates {
public:
    ConfigTemplates();
    ~ConfigTemplates();
    
    // Template management
    void addTemplate(const std::string& name, const nlohmann::json& config,
                    const std::string& description = "");
    void removeTemplate(const std::string& name);
    bool hasTemplate(const std::string& name) const;
    
    // Template application
    nlohmann::json applyTemplate(const std::string& name,
                               const nlohmann::json& baseConfig = nlohmann::json()) const;
    
    // Template discovery
    std::vector<std::string> getTemplateNames() const;
    nlohmann::json getTemplate(const std::string& name) const;
    
    // Template comparison
    nlohmann::json diffTemplates(const std::string& name1,
                                const std::string& name2) const;
    bool areTemplatesCompatible(const std::string& name1,
                               const std::string& name2) const;
    
    // Template validation
    bool validateTemplate(const std::string& name) const;
    std::vector<std::string> getTemplateErrors(const std::string& name) const;

private:
    struct TemplateInfo {
        nlohmann::json config;
        std::string description;
        nlohmann::json schema;
    };
    
    std::map<std::string, TemplateInfo> templates_;
    std::unique_ptr<ConfigSchema> defaultSchema_;
};

// Dynamic configuration manager for runtime changes
class DynamicConfigManager {
public:
    struct ConfigChange {
        std::string keyPath;
        nlohmann::json oldValue;
        nlohmann::json newValue;
        auto timestamp;
        std::string source;
    };
    
    DynamicConfigManager();
    ~DynamicConfigManager();
    
    // Configuration updates
    void updateConfig(const std::string& keyPath, const nlohmann::json& value);
    void updateConfig(const nlohmann::json& config);
    void resetConfig();
    
    // Change tracking
    std::vector<ConfigChange> getChanges() const;
    std::vector<ConfigChange> getChangesSince(const auto& timestamp) const;
    
    // Configuration snapshots
    void createSnapshot(const std::string& name);
    nlohmann::json restoreSnapshot(const std::string& name);
    std::vector<std::string> getSnapshotNames() const;
    
    // Configuration history
    void enableHistory(bool enable, size_t maxHistorySize = 100);
    bool isHistoryEnabled() const;
    std::vector<nlohmann::json> getHistory() const;
    
    // Event callbacks
    void addChangeListener(std::function<void(const ConfigChange&)> listener);
    void removeChangeListener(std::function<void(const ConfigChange&)> listener);

private:
    nlohmann::json currentConfig_;
    std::vector<ConfigChange> changes_;
    std::map<std::string, nlohmann::json> snapshots_;
    std::vector<nlohmann::json> history_;
    bool historyEnabled_ = false;
    size_t maxHistorySize_ = 100;
    
    std::vector<std::function<void(const ConfigChange&)>> changeListeners_;
};

// Configuration comparison and diff tools
class ConfigDiff {
public:
    struct DiffResult {
        std::string operation; // "add", "remove", "modify"
        std::string keyPath;
        nlohmann::json oldValue;
        nlohmann::json newValue;
        std::string description;
    };
    
    ConfigDiff();
    ~ConfigDiff();
    
    // Diff computation
    std::vector<DiffResult> computeDiff(const nlohmann::json& config1,
                                       const nlohmann::json& config2) const;
    
    // Path resolution
    std::vector<std::string> getConfigKeys(const nlohmann::json& config) const;
    std::string getKeyPath(const std::vector<std::string>& path) const;
    
    // Diff formatting
    std::string formatDiff(const std::vector<DiffResult>& diff) const;
    nlohmann::json serializeDiff(const std::vector<DiffResult>& diff) const;
    
    // Diff analysis
    size_t countChanges(const std::vector<DiffResult>& diff) const;
    size_t countAdds(const std::vector<DiffResult>& diff) const;
    size_t countRemoves(const std::vector<DiffResult>& diff) const;
    size_t countModifications(const std::vector<DiffResult>& diff) const;
    
    // Merge analysis
    nlohmann::json analyzeMergeConflict(const std::vector<DiffResult>& diff) const;
    bool canMerge(const std::vector<DiffResult>& diff) const;

private:
    static std::string normalizeKey(const std::string& key);
    static std::string getOperationSymbol(const std::string& operation);
};

// Configuration documentation generator
class ConfigDocumentation {
public:
    ConfigDocumentation();
    ~ConfigDocumentation();
    
    // Documentation generation
    std::string generateMarkdown(const nlohmann::json& config) const;
    std::string generateHTML(const nlohmann::json& config) const;
    std::string generateJSONSchema(const nlohmann::json& config) const;
    std::string generateTypeScriptInterface(const nlohmann::json& config) const;
    
    // Documentation structure
    struct DocumentationSection {
        std::string title;
        std::string content;
        std::vector<DocumentationSection> subsections;
    };
    
    void addSection(const DocumentationSection& section);
    std::vector<DocumentationSection> getDocumentation() const;
    
    // Example generation
    nlohmann::json generateExamples(const std::string& configName) const;
    std::string generateExampleMarkdown(const nlohmann::json& example) const;
    
    // Validation
    bool validateDocumentation() const;
    std::vector<std::string> getDocumentationErrors() const;

private:
    std::vector<DocumentationSection> documentation_;
    bool initialized_ = false;
};

// Advanced configuration system - main class
class AdvancedConfig {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Configuration management
    void loadConfig(const std::string& filename);
    void saveConfig(const std::string& filename) const;
    void loadConfig(const nlohmann::json& config);
    nlohmann::json exportConfig() const;
    
    // Schema management
    void registerSchema(const ConfigSchema& schema);
    void unregisterSchema(const std::string& name);
    bool hasSchema(const std::string& name) const;
    
    // Inheritance management
    void setupInheritance(const std::string& child, const std::string& parent);
    void removeInheritance(const std::string& child);
    std::vector<std::string> getInheritanceChain(const std::string& name) const;
    
    // Template management
    void registerTemplate(const std::string& name, const nlohmann::json& config,
                         const std::string& description = "");
    nlohmann::json applyTemplate(const std::string& name) const;
    
    // Dynamic configuration
    void enableDynamicUpdates(bool enable);
    void updateConfig(const std::string& keyPath, const nlohmann::json& value);
    nlohmann::json getConfig(const std::string& keyPath = "") const;
    
    // Validation
    bool validateConfig() const;
    std::vector<std::string> getConfigErrors() const;
    
    // Diff and merge
    std::vector<ConfigDiff::DiffResult> diffConfig(const nlohmann::json& other) const;
    nlohmann::json mergeConfig(const nlohmann::json& other) const;
    
    // Documentation
    void enableDocumentation(bool enable);
    std::string generateDocumentation() const;
    
    // Statistics
    struct ConfigStats {
        size_t totalConfigs = 0;
        size_t validConfigs = 0;
        size_t invalidConfigs = 0;
        double averageConfigSize = 0.0;
        std::chrono::steady_clock::time_point lastModified;
    };
    
    ConfigStats getStats() const;
    
    // Performance optimization
    void enableCaching(bool enable, size_t cacheSize = 100);
    void clearCache();
    bool isCached(const std::string& key) const;
    nlohmann::json getCachedValue(const std::string& key) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
