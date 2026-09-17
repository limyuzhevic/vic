#pragma once

// Config.hpp - Core configuration management
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <type_traits>
#include <any>
#include <optional>
#include <variant>
#include <stdexcept>

namespace nlm {

// Configuration parameter type
using ConfigValue = std::variant<
    bool,
    int,
    float,
    double,
    std::string,
    std::vector<int>,
    std::vector<float>,
    std::vector<double>,
    std::vector<std::string>
>;

class Config {
public:
    Config() = default;
    explicit Config(const std::string& filepath);
    ~Config() = default;
    
    // File operations
    bool load(const std::string& filepath);
    bool save(const std::string& filepath) const;
    
    // Parameter management
    template<typename T>
    void set(const std::string& key, const T& value);
    
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Parameter groups
    void createParameterGroup(const std::string& groupName, 
                            const std::string& description = "");
    void addToGroup(const std::string& groupName, const std::string& key);
    
    // Validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Serialization
    void serialize(std::ostream& out) const;
    void deserialize(std::istream& in);
    
    // Parameter info
    bool hasParameter(const std::string& key) const;
    std::vector<std::string> getAllParameterKeys() const;
    std::string getParameterDescription(const std::string& key) const;
    
private:
    struct Parameter {
        std::string key;
        ConfigValue value;
        std::string description;
        bool required = false;
        std::string groupName;
        std::function<bool(const ConfigValue&)> validator;
        
        template<typename T>
        Parameter(const std::string& k, const T& v, const std::string& desc = "")
            : key(k), value(v), description(desc), required(false) {}
    };
    
    std::vector<Parameter> parameters_;
    std::vector<std::string> parameterGroups_;
    mutable std::vector<std::string> validationErrors_;
    
    void validateParameter(const Parameter& param) const;
    static bool validateTypeMatch(const ConfigValue& value, const std::type_info& type);
};
