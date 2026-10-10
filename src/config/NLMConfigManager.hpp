// NLM Configuration Management Header
#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>

namespace nlm {

/**
 * Configuration manager for NLM
 */
class NLMConfigManager {
public:
    NLMConfigManager();
    ~NLMConfigManager();
    
    /**
     * Load configuration from file
     */
    bool loadFromFile(const std::string& filePath);
    
    /**
     * Save configuration to file
     */
    bool saveToFile(const std::string& filePath);
    
    /**
     * Validate current configuration
     */
    bool validateCurrentConfig();
    
    /**
     * Set configuration value
     */
    bool setConfigValue(const std::string& key, const std::string& value);
    
    /**
     * Get configuration value
     */
    std::string getConfigValue(const std::string& key) const;
    
    /**
     * Get all configuration keys
     */
    std::vector<std::string> getConfigKeys() const;
    
    /**
     * Clear configuration
     */
    void clearConfig();
    
    /**
     * Get configuration summary
     */
    std::string getConfigSummary() const;
    
    /**
     * Reset configuration to defaults
     */
    void resetToDefaults();
    
    /**
     * Import configuration from template
     */
    void importFromTemplate(const std::string& templateName);
    
    /**
     * Export configuration to template
     */
    void exportToTemplate(const std::string& templateName);
    
    /**
     * Get validator
     */
    ConfigurationValidator* getValidator() const;
    
    /**
     * Get schema
     */
    ConfigurationSchema* getSchema() const;

private:
    std::map<std::string, std::string> _currentConfig;
    std::shared_ptr<ConfigurationValidator> _validator;
    std::shared_ptr<ConfigurationSchema> _schema;
};

/**
 * Configuration validator
 */
class ConfigurationValidator {
public:
    ConfigurationValidator();
    ~ConfigurationValidator();
    
    /**
     * Set schema for validation
     */
    void setSchema(ConfigurationSchema* schema);
    
    /**
     * Validate configuration
     */
    bool validate(const std::map<std::string, std::string>& config);
    
private:
    std::vector<std::string> validateAgainstSchema(const std::map<std::string, std::string>& config);
    bool validateFieldType(const std::string& fieldName, const std::string& value, const std::string& type);
    
    ConfigurationSchema* _schema;
};

/**
 * Configuration schema
 */
class ConfigurationSchema {
public:
    ConfigurationSchema();
    ~ConfigurationSchema();
    
    /**
     * Initialize default schema
     */
    void initializeDefaultSchema();
    
    /**
     * Get field types
     */
    const std::map<std::string, std::string>& getFieldTypes() const;
    
    /**
     * Get required fields
     */
    const std::set<std::string>& getRequiredFields() const;
    
    /**
     * Add field to schema
     */
    bool addField(const std::string& name, const std::string& type, bool required = false);
    
    /**
     * Remove field from schema
     */
    bool removeField(const std::string& name);

private:
    std::map<std::string, std::string> _fields;
    std::set<std::string> _requiredFields;
};

/**
 * Configuration template system
 */
class ConfigurationTemplate {
public:
    ConfigurationTemplate();
    ~ConfigurationTemplate();
    
    /**
     * Create template
     */
    bool createTemplate(const std::string& name, const std::map<std::string, std::string>& config);
    
    /**
     * Load template
     */
    bool loadTemplate(const std::string& name);
    
    /**
     * Get all templates
     */
    const std::map<std::string, std::map<std::string, std::string>>& getAllTemplates() const;
    
    /**
     * Get template
     */
    const std::map<std::string, std::string>& getTemplate(const std::string& name) const;
    
    /**
     * Delete template
     */
    bool deleteTemplate(const std::string& name);
    
    /**
     * List templates
     */
    bool listTemplates() const;

private:
    std::map<std::string, std::map<std::string, std::string>> _templates;
};

} // namespace nlm