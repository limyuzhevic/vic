#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <any>
#include <functional>
#include <memory>
#include <variant>
#include <optional>
#include "Config.hpp"

namespace nlm {

// Forward declarations for dependencies
class ConfigValidator;
class ConfigGroup;
class ConfigProfile;

// Configuration schema definition
class ConfigSchema {
public:
    struct Field {
        std::string name;
        std::string description;
        std::any defaultValue;
        bool required;
        std::vector<std::shared_ptr<ConfigValidator>> validators;
        std::vector<std::string> dependsOn;
        std::string group;
        int priority;
        bool readOnly;
        std::function<void(ConfigChangeEvent&)> onValidate;
        
        Field() : name(), description(), required(false), priority(0), readOnly(false) {}
        Field(const std::string& name, const std::any& defaultValue, 
              bool required = false, const std::string& description = "")
            : name(name), description(description), defaultValue(defaultValue), 
              required(required), priority(0), readOnly(false) {}
    };

    ConfigSchema() = default;
    ~ConfigSchema() = default;

    // Add a configuration field with type
    template<typename T>
    void add(const std::string& name, 
             const std::any& defaultValue = std::any(),
             bool required = false,
             const std::string& description = "",
             const std::string& group = "",
             int priority = 0,
             bool readOnly = false);

    // Add a configuration field with validation
    template<typename T>
    void add(const std::string& name,
             const std::any& defaultValue,
             std::shared_ptr<ConfigValidator> validator,
             const std::string& description = "",
             const std::string& group = "",
             int priority = 0,
             bool readOnly = false);

    // Get field by name
    const Field* getField(const std::string& name) const;
    Field* getField(const std::string& name);

    // Get all fields
    const std::vector<Field>& getFields() const { return fields; }
    std::vector<Field>& getFields() { return fields; }

    // Get fields by group
    std::vector<const Field*> getFieldsByGroup(const std::string& group) const;

    // Validate all fields
    bool validate(const Config& config) const;

    // Get required field names
    std::vector<std::string> getRequiredFieldNames() const;

    // Get field names by group
    std::vector<std::string> getFieldNamesByGroup(const std::string& group) const;

    // Clear all fields
    void clear() { fields.clear(); }

    // Check if schema is empty
    bool empty() const { return fields.empty(); }

private:
    std::vector<Field> fields;
};

// Template specialization for add() to handle specific types
// Specializations for common types

// Template function specializations for different types
// These provide type-specific implementations for adding fields to the schema

} // namespace nlm