#pragma once

#include <string>
#include <unordered_map>
#include <any>
#include <vector>
#include <memory>
#include "Config.hpp"

namespace nlm {

// Configuration template for reusable configuration sets
class ConfigTemplate {
public:
    struct TemplateInfo {
        std::string name;
        std::string description;
        std::string author;
        std::string version;
        std::unordered_map<std::string, std::any> defaultValues;
        std::vector<std::string> tags;
        bool isPublic;
        
        TemplateInfo() : name(), description(), author(), version("1.0"), isPublic(true) {}
        TemplateInfo(const std::string& name, const std::string& description = "")
            : name(name), description(description), author(), version("1.0"), isPublic(true) {}
    };

    ConfigTemplate() = default;
    ~ConfigTemplate() = default;

    // Add a template field
    template<typename T>
    void addField(const std::string& name, const T& defaultValue,
                  const std::string& description = "");

    // Apply template to a configuration
    void applyTo(Config& config) const;

    // Get template info
    const TemplateInfo& getInfo() const { return info; }
    TemplateInfo& getInfo() { return info; }

    // Get all field names
    std::vector<std::string> getFieldNames() const;

    // Get field value
    template<typename T>
    std::optional<T> getFieldValue(const std::string& name) const;

    // Check if field exists
    bool hasField(const std::string& name) const;

    // Get default value as any
    const std::any* getDefaultValue(const std::string& name) const;

    // Export template to file
    bool exportToFile(const std::string& filepath) const;

    // Import template from file
    static std::unique_ptr<ConfigTemplate> importFromFile(const std::string& filepath);

    // Clone template
    std::unique_ptr<ConfigTemplate> clone() const;

private:
    TemplateInfo info;
    std::unordered_map<std::string, std::any> fields;
};

// Template specializations for different types
// These provide type-safe implementations for adding and retrieving template fields

} // namespace nlm