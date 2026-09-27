#pragma once

/**
 * Configuration Templates
 * 
 * Configuration template system for predefined templates for common configuration scenarios,
 * template inheritance, template customization, and template validation for the NLM brain simulation project.
 * 
 * Features:
 * - Template definition and management
 * - Template inheritance
 * - Template customization
 * - Template validation
 * - Template application
 * - Template customization
 * - Template versioning
 * - Template performance optimization
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <functional>

namespace nlm {
namespace core {

class ConfigurationManager;

/**
 * Template category
 */
enum class TemplateCategory {
    PERFORMANCE,
    MEMORY,
    NETWORK,
    NEURAL,
    BEHAVIORAL,
    DEVELOPMENTAL,
    ENVIRONMENT,
    USER,
    SYSTEM,
    PLUGIN
};

/**
 * Template metadata
 */
struct TemplateMetadata {
    std::string name;
    std::string description;
    TemplateCategory category;
    std::string version;
    std::string author;
    double createdAt;
    double updatedAt;
    std::map<std::string, std::string> tags;
    
    TemplateMetadata(const std::string& n, const std::string& desc = "")
        : name(n), description(desc), category(TemplateCategory::SYSTEM),
          version("1.0"), author("System"), createdAt(0.0), updatedAt(0.0) {}
};

/**
 * Template parameter
 */
struct TemplateParameter {
    std::string name;
    std::string value;
    ConfigDataType dataType;
    std::string description;
    bool required;
    std::string defaultValue;
    std::vector<std::string> allowedValues;
    double minValue;
    double maxValue;
    
    TemplateParameter(const std::string& n, const std::string& v, ConfigDataType type)
        : name(n), value(v), dataType(type), required(false), minValue(-std::numeric_limits<double>::max()),
          maxValue(std::numeric_limits<double>::max()) {}
};

/**
 * Template inheritance
 */
struct TemplateInheritance {
    std::string parentName;
    std::string version;
    bool inheritFields;
    bool inheritRules;
    bool inheritMetadata;
    
    TemplateInheritance(const std::string& parent, const std::string& ver = "1.0")
        : parentName(parent), version(ver), inheritFields(true), inheritRules(true), inheritMetadata(true) {}
};

/**
 * Configuration template
 */
class ConfigurationTemplate {
public:
    ConfigurationTemplate();
    explicit ConfigurationTemplate(const TemplateMetadata& metadata);
    ~ConfigurationTemplate();
    
    // Disable copying, enable moving
    ConfigurationTemplate(const ConfigurationTemplate&) = delete;
    ConfigurationTemplate& operator=(const ConfigurationTemplate&) = delete;
    ConfigurationTemplate(ConfigurationTemplate&&) noexcept = default;
    ConfigurationTemplate& operator=(ConfigurationTemplate&&) noexcept = default;
    
    // Template metadata
    TemplateMetadata& getMetadata() { return metadata_; }
    const TemplateMetadata& getMetadata() const { return metadata_; }
    
    void setName(const std::string& name) { metadata_.name = name; }
    void setDescription(const std::string& desc) { metadata_.description = desc; }
    void setCategory(TemplateCategory category) { metadata_.category = category; }
    
    // Template parameters
    bool addParameter(const TemplateParameter& param);
    bool removeParameter(const std::string& paramName);
    TemplateParameter* getParameter(const std::string& paramName);
    const TemplateParameter* getParameter(const std::string& paramName) const;
    std::vector<std::string> getParameterNames() const;
    
    // Template inheritance
    bool addInheritance(const TemplateInheritance& inheritance);
    bool removeInheritance(const std::string& parentName);
    std::vector<TemplateInheritance> getInheritances() const { return inheritances_; }
    
    // Template validation
    bool validate() const;
    std::vector<ConfigError> getValidationErrors() const;
    bool validateParameter(const TemplateParameter& param) const;
    
    // Template application
    std::map<std::string, std::string> applyTo(ConfigurationManager& configManager) const;
    std::map<std::string, std::string> applyTo(const std::map<std::string, std::string>& baseConfig) const;
    
    // Template customization
    void customizeParameter(const std::string& paramName, const std::string& value);
    void removeCustomization(const std::string& paramName);
    bool hasCustomization(const std::string& paramName) const;
    std::map<std::string, std::string> getCustomizations() const { return customizations_; }
    
    // Template serialization
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    std::string toString() const;
    
    // Template management
    bool updateMetadata(const TemplateMetadata& metadata);
    bool incrementVersion();
    
    // Template statistics
    size_t getParameterCount() const { return parameters_.size(); }
    size_t getCustomizationCount() const { return customizations_.size(); }
    size_t getInheritanceCount() const { return inheritances_.size(); }
    double getApplicationTime() const { return application_time_; }
    
    // Template filtering
    bool matchesCategory(TemplateCategory category) const { return metadata_.category == category; }
    bool matchesTag(const std::string& tag) const;
    bool matchesVersion(const std::string& version) const;
    
    // Template comparison
    bool isCompatibleWith(const ConfigurationTemplate& other) const;
    std::vector<std::string> getCompatibilityIssues(const ConfigurationTemplate& other) const;
    
    // Template optimization
    void optimize() const;
    bool isOptimized() const { return optimized_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    TemplateMetadata metadata_;
    
    std::vector<TemplateParameter> parameters_;
    std::vector<TemplateInheritance> inheritances_;
    std::map<std::string, std::string> customizations_;
    mutable double application_time_ = 0.0;
    bool optimized_ = false;
    
    void resolveInheritances() const;
    TemplateParameter getInheritedParameter(const std::string& paramName) const;
    bool hasInheritanceFrom(const std::string& parentName) const;
    
    friend class ConfigurationTemplates;
};

/**
 * Configuration template manager
 */
class ConfigurationTemplates {
public:
    ConfigurationTemplates();
    ~ConfigurationTemplates();
    
    // Disable copying, enable moving
    ConfigurationTemplates(const ConfigurationTemplates&) = delete;
    ConfigurationTemplates& operator=(const ConfigurationTemplates&) = delete;
    ConfigurationTemplates(ConfigurationTemplates&&) noexcept = default;
    ConfigurationTemplates& operator=(ConfigurationTemplates&&) noexcept = default;
    
    // Template management
    bool addTemplate(const ConfigurationTemplate& template);
    bool removeTemplate(const std::string& templateName);
    ConfigurationTemplate* getTemplate(const std::string& templateName);
    const ConfigurationTemplate* getTemplate(const std::string& templateName) const;
    std::vector<std::string> getTemplateNames() const;
    
    // Template filtering
    std::vector<ConfigurationTemplate*> getTemplatesByCategory(TemplateCategory category);
    std::vector<ConfigurationTemplate*> getTemplatesByTag(const std::string& tag);
    std::vector<ConfigurationTemplate*> getTemplatesByAuthor(const std::string& author);
    
    // Template search
    std::vector<ConfigurationTemplate*> findTemplates(const std::string& pattern);
    ConfigurationTemplate* findBestMatch(const std::map<std::string, std::string>& configData);
    
    // Template operations
    bool applyTemplate(const std::string& templateName, ConfigurationManager& configManager) const;
    bool updateTemplate(const std::string& templateName, const ConfigurationTemplate& newTemplate);
    bool duplicateTemplate(const std::string& sourceName, const std::string& targetName);
    
    // Template statistics
    size_t getTotalTemplateCount() const { return templates_.size(); }
    size_t getTotalParameterCount() const;
    size_t getTotalInheritanceCount() const;
    
    // Template serialization
    bool saveAllToFile(const std::string& directory) const;
    bool loadAllFromFile(const std::string& directory);
    bool exportTemplatesToJson(const std::string& filepath) const;
    bool importTemplatesFromJson(const std::string& filepath);
    
    // Template optimization
    void optimizeAll() const;
    void cleanupUnusedTemplates();
    
    // Template validation
    bool validateAll() const;
    std::vector<ConfigError> getAllValidationErrors() const;
    
    // Template versioning
    bool incrementTemplateVersion(const std::string& templateName);
    std::vector<std::string> getTemplateVersions(const std::string& templateName) const;
    bool restoreTemplateVersion(const std::string& templateName, const std::string& version);
    
    // Template backup
    bool backupTemplate(const std::string& templateName, const std::string& backupFile) const;
    bool restoreTemplate(const std::string& backupFile);
    std::vector<std::string> getAvailableBackups() const { return available_backups_; }
    
    // Template monitoring
    double getAverageApplicationTime(const std::string& templateName) const;
    size_t getApplicationCount(const std::string& templateName) const;
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Debug and diagnostics
    std::string dump() const;
    std::string getStatisticsString() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    
    std::map<std::string, std::unique_ptr<ConfigurationTemplate>> templates_;
    std::vector<std::string> available_backups_;
    
    ConfigurationTemplate* findTemplate(const std::string& templateName);
    const ConfigurationTemplate* findTemplate(const std::string& templateName) const;
    
    friend class ConfigurationManager;
};

} // namespace core
} // namespace nlm
