#pragma once

#include <string>
#include <map>
#include <vector>
#include <variant>
#include <stdexcept>
#include <typeinfo>
#include <limits>

namespace nlm {

// Configuration schema validation
class ConfigSchema {
public:
    using ConfigValue = std::variant<int, int64_t, double, bool, std::string>;
    
    struct SchemaEntry {
        std::string key;
        std::string type;  // "int", "double", "bool", "string"
        ConfigValue defaultValue;
        std::string description;
        bool required = false;
        std::vector<std::string> allowedValues;
        double minValue = std::numeric_limits<double>::lowest();
        double maxValue = std::numeric_limits<double>::max();
    };
    
    ConfigSchema() = default;
    
    // Add a schema entry
    void addEntry(const SchemaEntry& entry);
    
    // Validate configuration against schema
    bool validate(const std::string& key, const ConfigValue& value) const;
    
    // Get schema entry for a key
    const SchemaEntry* getEntry(const std::string& key) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get default value for a key
    ConfigValue getDefault(const std::string& key) const;
    
    // Clear all entries
    void clear();
    
    // Merge with another schema
    void merge(const ConfigSchema& other);
    
    // Get schema summary
    std::string summary() const;
    
private:
    std::map<std::string, SchemaEntry> entries_;
};

// Global configuration schema instance
ConfigSchema& getGlobalConfigSchema();

// Helper functions to add common configuration entries
void registerDefaultConfigSchema();

} // namespace nlm