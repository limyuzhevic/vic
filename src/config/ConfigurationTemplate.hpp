// Configuration Template Header
#pragma once

#include <string>
#include <map>

namespace nlm {

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