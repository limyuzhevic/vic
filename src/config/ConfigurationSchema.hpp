// Configuration Schema Header
#pragma once

#include <string>
#include <map>
#include <set>

namespace nlm {

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

} // namespace nlm