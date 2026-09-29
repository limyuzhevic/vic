#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <any>
#include <typeinfo>

namespace nlm {

// Parameter container for plasticity rules
// Provides type-safe parameter storage and access
class PlasticityParameters {
public:
    // Parameter types
    enum class ParameterType {
        Int,
        Float,
        Double,
        String,
        Bool,
        VectorFloat,
        VectorString
    };
    
    // Parameter metadata for type validation
    struct ParameterInfo {
        ParameterType type;
        std::string name;
        bool required;
        std::any defaultValue;
        std::string description;
        
        ParameterInfo(ParameterType t, const std::string& n, bool req = false,
                     const std::any& defaultVal = nullptr, const std::string& desc = "")
            : type(t), name(n), required(req), defaultValue(defaultVal), description(desc) {}
    };
    
    // Default parameter definitions for different plasticity rule types
    struct RuleParameters {
        std::vector<ParameterInfo> parameters;
        std::string ruleName;
        std::string description;
        
        RuleParameters(const std::string& name, const std::string& desc = "")
            : ruleName(name), description(desc) {}
    };
    
public:
    // Constructors
    PlasticityParameters() = default;
    explicit PlasticityParameters(const std::unordered_map<std::string, std::any>& data);
    
    // Parameter setting methods
    void set(const std::string& name, int value);
    void set(const std::string& name, float value);
    void set(const std::string& name, double value);
    void set(const std::string& name, const std::string& value);
    void set(const std::string& name, bool value);
    void set(const std::string& name, const std::vector<float>& values);
    void set(const std::string& name, const std::vector<std::string>& values);
    
    // Parameter getting methods
    bool hasParameter(const std::string& name) const;
    int getInt(const std::string& name, int defaultValue = 0) const;
    float getFloat(const std::string& name, float defaultValue = 0.0f) const;
    double getDouble(const std::string& name, double defaultValue = 0.0) const;
    std::string getString(const std::string& name, const std::string& defaultValue = "") const;
    bool getBool(const std::string& name, bool defaultValue = false) const;
    std::vector<float> getVectorFloat(const std::string& name, 
                                      const std::vector<float>& defaultValue = {}) const;
    std::vector<std::string> getVectorString(const std::string& name,
                                             const std::vector<std::string>& defaultValue = {}) const;
    
    // Parameter type checking
    ParameterType getParameterType(const std::string& name) const;
    bool isParameterValid(const std::string& name) const;
    
    // Parameter management
    void removeParameter(const std::string& name);
    void clear();
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    // Serialization and validation
    std::string toString() const;
    bool validate() const;
    std::vector<std::string> getErrors() const { return errors_; }
    
    // Conversion to other formats
    std::unordered_map<std::string, std::any> toMap() const;
    static PlasticityParameters fromMap(const std::unordered_map<std::string, std::any>& data);
    
    // Predefined parameter sets for common configurations
    static PlasticityParameters getDefaultSTDPParameters();
    static PlasticityParameters getDefaultHebbianParameters();
    static PlasticityParameters getDefaultR_STDPParameters();
    static PlasticityParameters getDefaultTripletSTDPParameters();
    
    // Validation function for specific rule types
    static bool validateForRule(const std::string& ruleName, 
                               const PlasticityParameters& params);
    
private:
    // Internal storage
    std::unordered_map<std::string, std::any> data_;
    std::vector<std::string> errors_;
    
    // Parameter metadata for validation
    static const std::unordered_map<std::string, RuleParameters>& getRuleParameterDefinitions();
    
    // Type conversion utilities
    template<typename T>
    T getParameter(const std::string& name, const std::any& defaultValue) const;
    
    // Error handling
    void addError(const std::string& error) { errors_.push_back(error); }
    bool hasErrors() const { return !errors_.empty(); }
};

} // namespace nlm