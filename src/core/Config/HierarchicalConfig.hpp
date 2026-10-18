// HierarchicalConfig.hpp - Hierarchical configuration system
#pragma once

/**
 * HierarchicalConfig - Hierarchical configuration system
 * 
 * This class provides a hierarchical configuration system that supports
 * nested values, validation, and persistence. It's designed to handle
 * complex configuration needs for the NLM project.
 */

#include "Config.hpp"
#include "../Error/NLMErrors.hpp"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <unordered_map>
#include <set>

namespace nlm {

/**
 * Hierarchical configuration value
 * 
 * Supports basic types: string, integer, double, boolean, and nested configurations
 */
class ConfigValue {
public:
    enum class Type {
        STRING,
        INTEGER,
        DOUBLE,
        BOOLEAN,
        ARRAY,
        OBJECT
    };
    
    ConfigValue() : type_(Type::STRING) {}
    ConfigValue(const std::string& value) : type_(Type::STRING), stringValue_(value) {}
    ConfigValue(int64_t value) : type_(Type::INTEGER), intValue_(value) {}
    ConfigValue(double value) : type_(Type::DOUBLE), doubleValue_(value) {}
    ConfigValue(bool value) : type_(Type::BOOLEAN), boolValue_(value) {}
    ConfigValue(const std::vector<ConfigValue>& value) : type_(Type::ARRAY), arrayValue_(value) {}
    ConfigValue(const std::map<std::string, ConfigValue>& value) : type_(Type::OBJECT), objectValue_(value) {}
    
    Type getType() const { return type_; }
    
    // Type-specific getters
    const std::string& getString() const { return stringValue_; }
    int64_t getInt() const { return intValue_; }
    double getDouble() const { return doubleValue_; }
    bool getBool() const { return boolValue_; }
    const std::vector<ConfigValue>& getArray() const { return arrayValue_; }
    const std::map<std::string, ConfigValue>& getObject() const { return objectValue_; }
    
    // Type-specific setters
    void setString(const std::string& value) { type_ = Type::STRING; stringValue_ = value; }
    void setInt(int64_t value) { type_ = Type::INTEGER; intValue_ = value; }
    void setDouble(double value) { type_ = Type::DOUBLE; doubleValue_ = value; }
    void setBool(bool value) { type_ = Type::BOOLEAN; boolValue_ = value; }
    void setArray(const std::vector<ConfigValue>& value) { type_ = Type::ARRAY; arrayValue_ = value; }
    void setObject(const std::map<std::string, ConfigValue>& value) { type_ = Type::OBJECT; objectValue_ = value; }
    
    // Conversion to basic types
    std::string toString() const;
    int64_t toInt() const;
    double toDouble() const;
    bool toBool() const;
    
    // Equality comparison
    bool operator==(const ConfigValue& other) const;
    bool operator!=(const ConfigValue& other) const { return !(*this == other); }
    
    // String representation
    std::string toStringRepresentation() const;
    
private:
    Type type_;
    std::variant<std::string, int64_t, double, bool, std::vector<ConfigValue>, std::map<std::string, ConfigValue>> value_;
};

/**
 * Hierarchical configuration system
 * 
 * Provides hierarchical configuration with support for nested values,
 * validation, and persistence. Uses a tree structure for configuration
 * management.
 */
class HierarchicalConfig : public Config {
public:
    HierarchicalConfig();
    ~HierarchicalConfig() override = default;
    
    // Core configuration interface
    bool loadFromFile(const std::string& filepath) override;
    bool saveToFile(const std::string& filepath) const override;
    bool loadFromArgs(int argc, char** argv) override;
    bool has(const std::string& key) const override;
    std::optional<std::string> get(const std::string& key) const override;
    std::optional<int64_t> getInt(const std::string& key) const override;
    std::optional<double> getDouble(const std::string& key) const override;
    std::optional<bool> getBool(const std::string& key) const override;
    void set(const std::string& key, const std::string& value) override;
    void set(const std::string& key, int64_t value) override;
    void set(const std::string& key, double value) override;
    void set(const std::string& key, bool value) override;
    std::vector<std::string> getKeys() const override;
    std::string summary() const override;
    bool validate() const override;
    void clear() override;
    
    // Hierarchical configuration methods
    void setNested(const std::string& path, const ConfigValue& value);
    std::optional<ConfigValue> getNested(const std::string& path) const;
    bool hasNested(const std::string& path) const;
    void removeNested(const std::string& path);
    std::vector<std::string> getNestedKeys(const std::string& path) const;
    
    // Configuration merging
    void merge(const HierarchicalConfig& other);
    void mergeFromFile(const std::string& filepath);
    
    // Configuration validation
    bool validate(const std::string& path = "") const;
    std::vector<std::string> getValidationErrors(const std::string& path = "") const;
    
    // Default values
    void setDefault(const std::string& key, const std::string& value);
    void setDefault(const std::string& key, int64_t value);
    void setDefault(const std::string& key, double value);
    void setDefault(const std::string& key, bool value);
    
    // Configuration persistence
    bool saveToJson(const std::string& filepath) const;
    bool loadFromJson(const std::string& filepath);
    bool saveToBinary(const std::string& filepath) const;
    bool loadFromBinary(const std::string& filepath);
    
    // Configuration serialization
    std::string toJson() const;
    bool fromJson(const std::string& json);
    
private:
    std::unordered_map<std::string, ConfigValue> config_;
    std::unordered_map<std::string, ConfigValue> defaults_;
    std::vector<std::string> validationErrors_;
    
    // Helper methods
    std::string normalizePath(const std::string& path) const;
    std::string getParentPath(const std::string& path) const;
    std::string getKeyFromPath(const std::string& path) const;
    bool validateValue(const ConfigValue& value) const;
    std::string valueToString(const ConfigValue& value) const;
};

} // namespace nlm
