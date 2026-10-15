// Basic Configuration Validator Header

#pragma once

#include "ConfigValidator.cpp"
#include <vector>
#include <string>
#include <any>
#include <typeindex>

namespace nlm {

// Basic configuration validator with common validation rules
class BasicConfigValidator : public ConfigValidator {
public:
    BasicConfigValidator();
    
    bool validate(const std::string& key, const ConfigValue& value, ConfigValidationError& error) override;
    std::string getErrorMessage(ConfigValidationError error) const override;
    
    void setSchema(const std::vector<ConfigSchemaEntry>& schema);
    void addSchemaEntry(const ConfigSchemaEntry& entry);
    
protected:
    virtual bool validateType(const std::string& key, const ConfigValue& value,
                           const std::type_index& expectedType, ConfigValidationError& error);
    virtual bool validateRange(const std::string& key, const ConfigValue& value,
                             const ConfigSchemaEntry& schema, ConfigValidationError& error);
    virtual bool validateCustomRules(const std::string& key, const ConfigValue& value,
                                   const ConfigSchemaEntry& schema, ConfigValidationError& error);
    
private:
    std::vector<ConfigSchemaEntry> schema;
    
    void initializeDefaultRules();
};

} // namespace nlm