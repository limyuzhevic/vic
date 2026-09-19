#pragma once

#include <string>
#include <vector>
#include <map>
#include <any>

namespace nlm {

// Forward declarations
enum class ParameterType;
struct RangeSpec;
struct ChoiceSpec;
struct VectorSpec;
struct MatrixSpec;
struct ParameterInfo;
class ConfigSchema;

// Configuration validator
class ConfigValidator {
public:
    ConfigValidator() = default;
    
    // Validate configuration against schema
    std::vector<ConfigError> validate(const std::map<std::string, std::any>& config,
                                      const ConfigSchema& schema) const;
    
    // Validate individual parameter constraints
    std::vector<ConfigError> validateRange(const std::string& key, const std::any& value,
                                           const RangeSpec& range) const;
    
    std::vector<ConfigError> validateChoice(const std::string& key, const std::string& value,
                                           const ChoiceSpec& choice) const;
    
    std::vector<ConfigError> validateVector(const std::string& key, const std::any& value,
                                           const VectorSpec& vector) const;
    
    std::vector<ConfigError> validateMatrix(const std::string& key, const std::any& value,
                                           const MatrixSpec& matrix) const;
    
    std::vector<ConfigError> validateDependency(const std::string& key, const std::any& value,
                                                const ParameterInfo& dependency,
                                                const std::map<std::string, std::any>& config) const;
    
    // Configuration analysis
    std::vector<std::string> getConflictingParameters(const ConfigSchema& schema) const;
    
    // Utility functions
    std::string formatChoices(const std::vector<std::string>& choices) const;
    std::string joinStrings(const std::vector<std::string>& strings, const std::string& delimiter) const;
    std::string getTypeName(const std::type_info& type) const;
    
    double getDoubleValue(const std::any& value) const;
    double parseDouble(const std::string& str) const;
};

} // namespace nlm