// Configuration Validator Header
#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

namespace nlm {

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

} // namespace nlm