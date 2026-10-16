// src/core/Config/ConfigurationSchema.hpp
#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <variant>
#include <typeindex>
#include <functional>
#include <nlohmann/json.hpp>

namespace nlm {

// Type for schema validation functions
using ValidationFunction = std::function<bool(const nlohmann::json&, std::string& error)>;

// Schema validation types
enum class ValidationLevel {
    Optional,
    Required,
    ConditionalRequired
};

// Type information for schema values
struct ConfigTypeInfo {
    std::type_index type;
    std::string name;
    bool isArray;
    std::optional<ConfigTypeInfo> elementType;
    
    ConfigTypeInfo(std::type_index t, const std::string& n, bool arr = false, 
                   const std::optional<ConfigTypeInfo>& elem = std::nullopt)
        : type(t), name(n), isArray(arr), elementType(elem) {}
    
    // Helper methods
    static ConfigTypeInfo ofInt() { return ConfigTypeInfo(typeid(int), "int"); }
    static ConfigTypeInfo ofInt64() { return ConfigTypeInfo(typeid(int64_t), "int64"); }
    static ConfigTypeInfo ofDouble() { return ConfigTypeInfo(typeid(double), "double"); }
    static ConfigTypeInfo ofBool() { return ConfigTypeInfo(typeid(bool), "bool"); }
    static ConfigTypeInfo ofString() { return ConfigTypeInfo(typeid(std::string), "string"); }
    static ConfigTypeInfo ofIntArray() { return ConfigTypeInfo(typeid(std::vector<int>), "int_array", true, ofInt()); }
    static ConfigTypeInfo ofDoubleArray() { return ConfigTypeInfo(typeid(std::vector<double>), "double_array", true, ofDouble()); }
    static ConfigTypeInfo ofStringArray() { return ConfigTypeInfo(typeid(std::vector<std::string>), "string_array", true, ofString()); }
    
    bool matchesType(const std::type_index& other) const;
    bool isNumeric() const;
};

// Configuration schema parameter
struct ConfigParameter {
    std::string name;
    ConfigTypeInfo type;
    ValidationLevel required;
    std::any defaultValue;
    std::string description;
    ValidationFunction validator;
    std::optional<std::string> parentSection;
    
    ConfigParameter(const std::string& n, const ConfigTypeInfo& t, ValidationLevel req,
                   const std::any& def = std::nullopt, const std::string& desc = "",
                   const ValidationFunction& val = nullptr,
                   const std::optional<std::string>& parent = std::nullopt)
        : name(n), type(t), required(req), defaultValue(def), description(desc),
          validator(val), parentSection(parent) {}
};

// Configuration section
struct ConfigSection {
    std::string name;
    std::vector<ConfigParameter> parameters;
    std::string description;
    
    ConfigSection(const std::string& n, const std::string& desc = "")
        : name(n), description(desc) {}
    
    void addParameter(const ConfigParameter& param) {
        parameters.push_back(param);
    }
};

// Main configuration schema class
class ConfigurationSchema {
private:
    std::vector<ConfigSection> sections;
    std::unordered_map<std::string, ConfigParameter> parameterMap;
    std::unordered_map<std::string, std::string> sectionMap;
    
public:
    ConfigurationSchema() = default;
    
    // Add a section
    void addSection(const ConfigSection& section) {
        sections.push_back(section);
        sectionMap[section.name] = section.description;
        for (const auto& param : section.parameters) {
            parameterMap[param.name] = param;
        }
    }
    
    // Get parameter by name
    const ConfigParameter* getParameter(const std::string& name) const {
        auto it = parameterMap.find(name);
        return it != parameterMap.end() ? &it->second : nullptr;
    }
    
    // Get section by name
    const ConfigSection* getSection(const std::string& name) const {
        auto it = sectionMap.find(name);
        if (it == sectionMap.end()) return nullptr;
        
        for (const auto& section : sections) {
            if (section.name == name) {
                return &section;
            }
        }
        return nullptr;
    }
    
    // Validate configuration against schema
    bool validate(const nlohmann::json& config, std::string& error) const;
    
    // Get all sections
    const std::vector<ConfigSection>& getSections() const { return sections; }
    
    // Get all parameter names
    std::vector<std::string> getParameterNames() const {
        std::vector<std::string> names;
        names.reserve(parameterMap.size());
        for (const auto& pair : parameterMap) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Check if schema is defined
    bool isEmpty() const { return sections.empty(); }
    
    // Get schema description
    std::string getDescription() const {
        std::string desc = "Configuration Schema\n";
        for (const auto& section : sections) {
            desc += "  Section: " + section.name + "\n";
            if (!section.description.empty()) {
                desc += "    Description: " + section.description + "\n";
            }
            desc += "    Parameters: " + std::to_string(section.parameters.size()) + "\n";
        }
        return desc;
    }
};

// Helper functions for creating schemas
namespace Schema {
    // Create basic validation functions
    ValidationFunction isNotEmpty();
    ValidationFunction isWithinRange(double min, double max);
    ValidationFunction isPositive();
    ValidationFunction isNonNegative();
    ValidationFunction isInteger();
    ValidationFunction matchesPattern(const std::string& pattern);
    
    // Create predefined schemas
    ConfigurationSchema createDefaultSchema();
    ConfigurationSchema createExperimentalSchema();
    ConfigurationSchema createPerformanceSchema();
}
}