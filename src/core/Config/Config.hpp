#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>

namespace nlm {

// Enhanced configuration schema for better type safety and validation
struct ConfigSchema {
    enum class Type {
        INT,
        INT64,
        DOUBLE,
        BOOL,
        STRING,
        VECTOR_INT,
        VECTOR_DOUBLE,
        VECTOR_STRING
    };
    
    std::string key;
    Type type;
    std::optional<std::string> description;
    std::optional<std::string> unit;
    std::variant<int, int64_t, double, bool, std::string> defaultValue;
    std::optional<std::pair<double, double>> range;  // for numeric types
    std::optional<size_t> minSize;  // for vector types
    bool required;  // whether this parameter is required
    
    ConfigSchema(const std::string& k, Type t, const std::string& desc = "",
                 const std::string& u = "")
        : key(k), type(t), description(desc), unit(u), required(false) {}
};

// Advanced configuration class with schema validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file with schema validation
    bool loadFromFile(const std::string& filepath, const std::vector<ConfigSchema>& schema = {});
    
    // Load from command line with schema validation
    bool loadFromArgs(int argc, char** argv, const std::vector<ConfigSchema>& schema = {});
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with schema validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set from string with type conversion and validation
    bool setFromString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get schema (if loaded)
    const std::vector<ConfigSchema>& getSchema() const { return schema_; }
    
    // Set schema
    void setSchema(const std::vector<ConfigSchema>& schema);
    
    // Validate current configuration against schema
    bool validate() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Get config source for a key
    ConfigSource getSource(const std::string& key) const;
    
    // Get description for a key
    std::string getDescription(const std::string& key) const;
    
    // Get unit for a key
    std::string getUnit(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<ConfigSchema> schema_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    template<typename T>
    std::optional<T> getFromString(const std::string& value) const;
    bool validateValue(const ConfigSchema& schema, const ConfigValue& value) const;
};

} // namespace nlm
