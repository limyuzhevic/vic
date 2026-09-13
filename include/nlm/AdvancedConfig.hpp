#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <cmath>
#include <random>
#include <chrono>
#include <iomanip>
#include <set>

#include "nlm/Constants.hpp"
#include "nlm/CLI.hpp"
#include "nlm/Exception.hpp"
#include "nlm/Config.hpp"
#include "nlm/Experiment.hpp"

namespace nlm {

/**
 * Advanced configuration system with schema validation and type safety
 */
class AdvancedConfig : public Config {
public:
    // Configuration schema validation result
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::map<std::string, std::string> suggestions;
    };

    // Configuration value validator function type
    using Validator = std::function<bool(const Config::Value& value, std::string& errorMessage)>;

    // Configuration constraint
    struct Constraint {
        std::string name;
        Validator validator;
        std::string errorMessage;
        std::string suggestion;
        
        Constraint(const std::string& name, Validator validator,
                  const std::string& errorMessage = "",
                  const std::string& suggestion = "")
            : name(name), validator(validator), errorMessage(errorMessage),
              suggestion(suggestion) {}
    };

    // Configuration parameter with constraints
    struct ParameterWithConstraints {
        std::string name;
        std::string type;
        std::string description;
        Config::Value defaultValue;
        std::vector<Constraint> constraints;
        bool required;
        std::string category; // For organizing parameters
        std::string subcategory;
        
        ParameterWithConstraints(const std::string& name, const std::string& type,
                                 const std::string& description,
                                 const Config::Value& defaultValue = Config::Value())
            : name(name), type(type), description(description), defaultValue(defaultValue),
              required(false), category("General"), subcategory("") {}
    };

    AdvancedConfig(const std::string& programName = "nlm");
    ~AdvancedConfig() override;

    // Load configuration from various sources
    bool loadFromJSON(const std::string& path);
    bool loadFromYAML(const std::string& path);
    bool loadFromINI(const std::string& path);
    
    // Save configuration to various formats
    bool saveToJSON(const std::string& path) const;
    bool saveToYAML(const std::string& path) const;
    bool saveToINI(const std::string& path) const;
    
    // Merge configurations
    bool merge(const Config& other);
    bool mergeFromFile(const std::string& path, const std::string& format = "auto");
    
    // Get parameter definitions
    const std::vector<ParameterWithConstraints>& getParameterDefinitions() const;
    
    // Get parameters by category
    std::vector<ParameterWithConstraints> getParametersByCategory(const std::string& category) const;
    
    // Get parameter by name
    const ParameterWithConstraints* getParameter(const std::string& name) const;
    
    // Add parameter definition
    void addParameter(const ParameterWithConstraints& param);
    
    // Validate configuration with detailed results
    ValidationResult validate() const;
    
    // Get configuration as JSON string
    std::string toJSON() const;
    
    // Get configuration as YAML string
    std::string toYAML() const;
    
    // Get configuration as INI string
    std::string toINI() const;
    
    // Initialize parameter schema
    void initializeParameterSchema();
    
    // Create example configuration
    static std::string createExampleConfig(const std::string& format = "json");

protected:
    std::vector<ParameterWithConstraints> parameterDefinitions;
    
    // Parse JSON configuration
    bool parseJSON(const std::string& jsonStr);
    
    // Parse YAML configuration
    bool parseYAML(const std::string& yamlStr);
    
    // Parse INI configuration
    bool parseINI(const std::string& iniStr);
    
    // Validate single parameter
    bool validateParameter(const ParameterWithConstraints& param, const Config::Value& value) const;
    
    // Validate constraint
    bool validateConstraint(const Constraint& constraint, const Config::Value& value) const;
    
    // Get data type name as string
    static std::string getTypeName(Config::Value::Type type);
    
    // Detect file format
    static std::string detectFileFormat(const std::string& path);
    
    // Convert to JSON
    void toJSONInternal(std::ostream& out) const;
    
    // Convert to YAML
    void toYAMLInternal(std::ostream& out) const;
    
    // Convert to INI
    void toINIInternal(std::ostream& out) const;
};

// Configuration file format detection
class ConfigFormatDetector {
public:
    enum class Format { Unknown, JSON, YAML, INI, CFG }
    
    static Format detect(const std::string& path);
    static Format detect(const std::string& content);
    
    static bool isJSON(const std::string& content);
    static bool isYAML(const std::string& content);
    static bool isINI(const std::string& content);
    static bool isCFG(const std::string& content);
    
    static std::string formatToString(Format format);
};

// Configuration schema validation
class ConfigValidator {
public:
    struct ValidationOptions {
        bool validateTypes = true;
        bool validateRanges = true;
        bool validateAllowedValues = true;
        bool validateRequiredFields = true;
        bool strictMode = false;
        std::vector<std::string> enabledValidators;
    };

    ConfigValidator(const ValidationOptions& options = ValidationOptions());
    
    ValidationResult validate(const AdvancedConfig& config) const;
    
    // Register custom validator
    void registerValidator(const std::string& name, 
                          std::function<bool(const Config::Value&, std::string&)> validator);
    
    // Get list of available validators
    std::vector<std::string> getAvailableValidators() const;
    
    // Set validation options
    void setOptions(const ValidationOptions& options) { this->options = options; }
    
    const ValidationOptions& getOptions() const { return options; }

private:
    ValidationOptions options;
    std::map<std::string, std::function<bool(const Config::Value&, std::string&)>> validators;
    
    ValidationResult validateType(const Config::Value& value, const std::string& type) const;
    ValidationResult validateRange(const Config::Value& value, const ParameterWithConstraints& param) const;
    ValidationResult validateAllowedValues(const Config::Value& value, const ParameterWithConstraints& param) const;
    ValidationResult validateRequired(const std::string& key, const Config::Value& value) const;
};

} // namespace nlm
