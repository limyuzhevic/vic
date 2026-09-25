#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

class Config;
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Validate a configuration against a schema
    bool validate(const Config& config, const json& schema);
    
    // Validate individual field
    bool validateField(const std::string& key, const ConfigValue& value, const json& fieldSchema);
    
    // Get validation errors
    const std::vector<ConfigValidationError>& getErrors() const;
    
    // Set schema
    void setSchema(const json& schema);
    
    // Get schema
    const json& getSchema() const;
    
    // Clear errors
    void clearErrors();
    
    // Validate configuration for specific system
    bool validateSystemConfig(const Config& config, const std::string& systemName);
    
    // Check compatibility between configurations
    bool checkCompatibility(const Config& config1, const Config& config2);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
