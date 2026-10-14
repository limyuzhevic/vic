#include "ConfigValidator.hpp"
#include <sstream>
#include <algorithm>

namespace nlm {

ConfigValidator::ConfigValidator() {}

void ConfigValidator::addSchema(const std::string& name, const ConfigSchema& schema) {
    schemas_[name] = schema;
}

void ConfigValidator::removeSchema(const std::string& name) {
    schemas_.erase(name);
}

std::vector<std::string> ConfigValidator::getSchemaNames() const {
    std::vector<std::string> names;
    names.reserve(schemas_.size());
    for (const auto& pair : schemas_) {
        names.push_back(pair.first);
    }
    return names;
}

const ConfigSchema* ConfigValidator::getSchema(const std::string& name) const {
    auto it = schemas_.find(name);
    if (it != schemas_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<ValidationError> ConfigValidator::validate(
    const std::unordered_map<std::string, std::any>& config
) const {
    std::vector<ValidationError> allErrors;
    
    for (const auto& pair : config) {
        const std::string& key = pair.first;
        const std::any& value = pair.second;
        
        std::vector<ValidationError> errors = validate(key, value);
        allErrors.insert(allErrors.end(), errors.begin(), errors.end());
        
        if (strictMode_ && !errors.empty()) {
            break; // Stop at first error if in strict mode
        }
    }
    
    return allErrors;
}

std::vector<ValidationError> ConfigValidator::validate(
    const std::string& key,
    const std::any& value
) const {
    std::vector<ValidationError> errors;
    
    for (const auto& schemaPair : schemas_) {
        const ConfigSchema& schema = schemaPair.second;
        std::vector<ValidationError> schemaErrors = schema.validate(key, value);
        errors.insert(errors.end(), schemaErrors.begin(), schemaErrors.end());
        
        if (strictMode_ && !schemaErrors.empty()) {
            break; // Stop at first error if in strict mode
        }
    }
    
    return errors;
}

void ConfigValidator::clearValidationErrors() {
    accumulatedErrors_.clear();
}

std::vector<ValidationError> ConfigValidator::getValidationErrors() const {
    return accumulatedErrors_;
}

std::string ConfigValidator::getValidationReport() const {
    std::ostringstream oss;
    oss << "=== Configuration Validation Report ===\n";
    oss << "Schemas registered: " << schemas_.size() << "\n";
    oss << "Accumulated errors: " << accumulatedErrors_.size() << "\n";
    oss << "Strict mode: " << (strictMode_ ? "enabled" : "disabled") << "\n\n";
    
    if (accumulatedErrors_.empty()) {
        oss << "✓ No validation errors found.\n";
    } else {
        oss << "Validation Errors:\n";
        for (const auto& error : accumulatedErrors_) {
            oss << "  " << error.severity << ": " << error.parameter << " - " << error.message << "\n";
        }
    }
    
    return oss.str();
}

void ConfigValidator::setStrictMode(bool strict) {
    strictMode_ = strict;
}

bool ConfigValidator::getStrictMode() const {
    return strictMode_;
}

void ConfigValidator::reset() {
    schemas_.clear();
    accumulatedErrors_.clear();
    strictMode_ = false;
}

} // namespace nlm