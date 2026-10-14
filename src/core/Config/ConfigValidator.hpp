#pragma once

#include "ConfigSchema.hpp"
#include <vector>
#include <any>
#include <string>

namespace nlm {

// Configuration validator class
class ConfigValidator {
public:
    ConfigValidator();
    
    // Add a schema to the validator
    void addSchema(const std::string& name, const ConfigSchema& schema);
    
    // Remove a schema by name
    void removeSchema(const std::string& name);
    
    // Get all schema names
    std::vector<std::string> getSchemaNames() const;
    
    // Get a schema by name
    const ConfigSchema* getSchema(const std::string& name) const;
    
    // Validate a configuration against all registered schemas
    std::vector<ValidationError> validate(const std::unordered_map<std::string, std::any>& config) const;
    
    // Validate a single key-value pair against all schemas
    std::vector<ValidationError> validate(
        const std::string& key,
        const std::any& value
    ) const;
    
    // Clear all validation errors
    void clearValidationErrors();
    
    // Get all accumulated validation errors
    std::vector<ValidationError> getValidationErrors() const;
    
    // Get formatted validation report
    std::string getValidationReport() const;
    
    // Set strict mode - fail on first error vs collect all errors
    void setStrictMode(bool strict);
    
    // Get strict mode setting
    bool getStrictMode() const;
    
    // Reset validator to initial state
    void reset();
private:
    std::unordered_map<std::string, ConfigSchema> schemas_;
    std::vector<ValidationError> accumulatedErrors_;
    bool strictMode_ = false;
};