#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <variant>
#include <cassert>

namespace nlm {

// Configuration schema documentation
// This file contains definitions for all valid configuration parameters
// organized by functional groups with their types, default values, and constraints.

// Type aliases for schema definitions
using StringList = std::vector<std::string>;
using DoubleList = std::vector<double>;
using IntList = std::vector<int64_t>;
using ConfigValueSchema = std::variant<
    int, int64_t, double, bool, std::string, IntList, DoubleList, StringList
>;

// Validation result structure
struct ValidationResult {
    bool isValid;
    std::string errorMessage;
    std::string parameterName;
    
    ValidationResult() : isValid(true), errorMessage(""), parameterName("") {}
    ValidationResult(bool valid, const std::string& error, const std::string& param)
        : isValid(valid), errorMessage(error), parameterName(param) {}
};

// Parameter constraint types
struct Constraint {
    enum class Type {
        None,
        Range,
        Enum,
        Pattern,
        Positive,
        NonNegative,
        Required,
        MaxLength,
        MinLength,
        GreaterThan,
        LessThan,
        Unique,
        Dependency,
        Consistency
    };
    
    Type type;
    std::string value1;  // For range: "min,max", for enum: values, for pattern: regex
    std::string value2;  // For range: "max"
    std::string message;
    
    Constraint(Type t, const std::string& v1, const std::string& v2 = "", const std::string& msg = "")
        : type(t), value1(v1), value2(v2), message(msg) {}
};

// Parameter metadata
struct ParameterInfo {
    std::string name;
    std::string displayName;
    std::string description;
    ConfigValueSchema defaultValue;
    std::string category;  // Brain, Plasticity, Neuromodulation, etc.
    std::string subcategory;
    std::vector<Constraint> constraints;
    std::string units;
    bool isAdvanced = false;
    bool isRequired = false;
    std::string validationGroup; // For cross-parameter validation
    
    // Parameter groups for organization
    static const std::string CATEGORY_BRAIN;
    static const std::string CATEGORY_PLASTICITY;
    static const std::string CATEGORY_NEUROMODULATION;
    static const std::string CATEGORY_DEVELOPMENT;
    static const std::string CATEGORY_ENVIRONMENT;
    static const std::string CATEGORY_LOGGING;
    static const std::string CATEGORY_SIMULATION;
    static const std::string CATEGORY_VISUALIZATION;
    static const std::string CATEGORY_PERFORMANCE;
    
    ParameterInfo(const std::string& n, const std::string& desc, const ConfigValueSchema& defaultVal,
                  const std::string& cat = "", const std::string& subcat = "")
        : name(n), displayName(n), description(desc), defaultValue(defaultVal),
          category(cat), subcategory(subcat), isAdvanced(false), isRequired(false) {}
};

// Configuration schema class
class ConfigurationSchema {
public:
    ConfigurationSchema();
    ~ConfigurationSchema();
    
    // Get parameter info by name
    const ParameterInfo* getParameter(const std::string& name) const;
    
    // Get all parameters in a category
    std::vector<const ParameterInfo*> getParametersInCategory(const std::string& category) const;
    
    // Get all parameters
    std::vector<const ParameterInfo*> getAllParameters() const;
    
    // Get parameter categories
    std::vector<std::string> getCategories() const;
    
    // Check if parameter exists
    bool hasParameter(const std::string& name) const;
    
    // Get default value as string
    std::string getDefaultValueString(const std::string& name) const;
    
    // Get category description
    std::string getCategoryDescription(const std::string& category) const;
    
    // Validate parameter exists
    bool validateParameterExists(const std::string& name) const;
    
    // Validate parameter type
    bool validateParameterType(const std::string& name, const ConfigValueSchema& value) const;
    
    // Validate parameter constraints
    bool validateParameterConstraints(const std::string& name, const ConfigValueSchema& value) const;
    
    // Get validation error message
    std::string getValidationError(const std::string& name, const ConfigValueSchema& value) const;
    
    // Register new parameter
    void registerParameter(const ParameterInfo& param);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Initialize schema with default parameters
void initializeConfigurationSchema();

} // namespace nlm

// External declaration for schema instance
extern nlm::ConfigurationSchema gConfigurationSchema;
