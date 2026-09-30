#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <mutex>
#include <algorithm>
#include <cmath>

using json = nlohmann::json;

namespace nlm {

/**
 * Experiment configuration management with JSON/YAML support
 * and parameter validation for neural network experiments.
 */
class ExperimentConfiguration {
public:
    // Configuration value types
    enum class ValueType {
        Int,
        Float,
        String,
        Bool,
        IntArray,
        FloatArray,
        StringArray,
        Invalid
    };
    
    // Configuration entry with validation
    struct ConfigEntry {
        std::string key;
        std::string value;
        ValueType type;
        std::string description;
        bool required;
        std::string defaultValue;
        
        // Validation ranges
        double minValue;
        double maxValue;
        std::vector<std::string> allowedValues;
        
        // Min/max for numeric types
        bool hasMin;
        bool hasMax;
        
        ConfigEntry() : type(ValueType::Invalid), required(false), 
                       minValue(0.0), maxValue(0.0), hasMin(false), hasMax(false) {}
        
        ConfigEntry(const std::string& k, const std::string& v, ValueType t, 
                   const std::string& desc = "", bool req = false, 
                   const std::string& def = "")
            : key(k), value(v), type(t), description(desc), required(req), 
              defaultValue(def), minValue(0.0), maxValue(0.0), hasMin(false), hasMax(false) {}
    };
    
    // Configuration template for experiment types
    struct ConfigTemplate {
        std::string name;
        std::string description;
        std::vector<ConfigEntry> parameters;
        std::string experimentType;
    };
    
    ExperimentConfiguration();
    ~ExperimentConfiguration() = default;
    
    // Load/save methods
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    bool loadFromJSON(const std::string& filepath);
    bool saveToJSON(const std::string& filepath) const;
    bool loadFromYAML(const std::string& filepath);
    bool saveToYAML(const std::string& filepath) const;
    
    // Get/set values
    bool has(const std::string& key) const;
    std::string getValue(const std::string& key) const;
    
    // Template versions for specific types
    int getOr<int>(const std::string& key, int defaultValue) const;
    float getOr<float>(const std::string& key, float defaultValue) const;
    double getOr<double>(const std::string& key, double defaultValue) const;
    bool getOr<bool>(const std::string& key, bool defaultValue) const;
    std::string getOr<std::string>(const std::string& key, const std::string& defaultValue) const;
    
    // Set values with type conversion
    template<typename T>
    void set(const std::string& key, const T& value);
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration comparison and merging
    bool isSameAs(const ExperimentConfiguration& other) const;
    void merge(const ExperimentConfiguration& other, bool overwrite = true);
    
    // Template management
    static std::vector<ConfigTemplate> getAvailableTemplates();
    static ExperimentConfiguration createFromTemplate(const std::string& templateName);
    
    // Configuration conversion
    json toJSON() const;
    void fromJSON(const json& j);
    
    // Configuration management
    void clear();
    size_t size() const;
    std::vector<std::string> getKeys() const;
    
    // Statistics
    std::string summary() const;
    
private:
    // Configuration storage
    std::map<std::string, ConfigEntry> configData;
    mutable std::mutex configMutex;
    
    // File format detection
    static ConfigFormat detectFileFormat(const std::string& filepath);
    
    // Helper methods
    ValueType inferType(const std::string& value) const;
    bool validateEntry(const ConfigEntry& entry) const;
    std::string valueToString(const std::string& value, ValueType type) const;
};

// File format enum (declared here for use in ExperimentConfiguration)
enum class ConfigFormat {
    Unknown,
    JSON,
    YAML,
    KeyValue
};

} // namespace nlm