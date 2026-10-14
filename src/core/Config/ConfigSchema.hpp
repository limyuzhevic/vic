#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <optional>
#include <variant>
#include <any>

namespace nlm {

// Configuration parameter metadata
struct ConfigParameter {
    std::string name;
    std::string description;
    std::variant<int, int64_t, double, bool, std::string> defaultValue;
    std::function<bool(const std::any&)> validator;
    std::optional<std::pair<double, double>> numericRange; // min, max
    bool required = false;
    std::string category;
    
    ConfigParameter(
        const std::string& n,
        const std::string& desc,
        const std::variant<int, int64_t, double, bool, std::string>& def,
        std::function<bool(const std::any&)> val = nullptr,
        const std::optional<std::pair<double, double>>& range = std::nullopt,
        bool req = false,
        const std::string& cat = "General"
    ) : name(n), description(desc), defaultValue(def), 
        validator(val), numericRange(range), required(req), category(cat) {}
};

// Configuration validation error
struct ValidationError {
    std::string parameter;
    std::string message;
    std::string severity; // "error", "warning", "info"
    
    ValidationError(const std::string& param, const std::string& msg, const std::string& sev = "error")
        : parameter(param), message(msg), severity(sev) {}
};

// Configuration schema containing all valid parameters
class ConfigSchema {
public:
    ConfigSchema();
    
    // Register a parameter with the schema
    void registerParameter(const ConfigParameter& param);
    
    // Get parameter metadata by name
    const ConfigParameter* getParameter(const std::string& name) const;
    
    // Validate a configuration value against schema
    std::vector<ValidationError> validate(
        const std::string& key,
        const std::any& value
    ) const;
    
    // Get all parameter names
    std::vector<std::string> getParameterNames() const;
    
    // Get parameters by category
    std::vector<const ConfigParameter*> getParametersByCategory(
        const std::string& category
    ) const;
    
    // Get schema description
    std::string getSchemaDescription() const;
    
    // Get validation summary
    std::string getValidationSummary() const;
    
    // Check if parameter exists
    bool hasParameter(const std::string& name) const;
private:
    std::unordered_map<std::string, ConfigParameter> parameters_;
    std::vector<ValidationError> validationErrors_;
};