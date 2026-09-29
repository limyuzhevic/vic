#pragma once

#include "Config.hpp"
#include <map>
#include <string>
#include <vector>
#include <set>
#include <unordered_set>

namespace nlm {

// Parameter validation rules
struct ParameterRule {
    std::string name;
    std::string type;  // "int", "float", "bool", "string"
    std::string description;
    bool required;
    
    // Range validation
    std::optional<std::pair<double, double>> numericRange;
    std::set<std::string> validValues;
    
    // Validation callback
    std::function<bool(const Config&, const std::string&)> validator;
};

// Configuration schema for validation
class ConfigSchema {
public:
    void addParameter(const ParameterRule& rule);
    bool validate(const Config& config, std::vector<std::string>& errors) const;
    const std::vector<ParameterRule>& getParameters() const { return parameters_; }
    
    // Predefined schema validation rules
    static ConfigSchema createDefaultSchema();
    static ConfigSchema createAdvancedSchema();
    
private:
    std::vector<ParameterRule> parameters_;
};

// Configuration presets for common use cases
class ConfigPresets {
public:
    static std::shared_ptr<Config> createDefaultConfig();
    static std::shared_ptr<Config> createDevelopmentConfig();
    static std::shared_ptr<Config> createLearningConfig();
    static std::shared_ptr<Config> createExplorationConfig();
    static std::shared_ptr<Config> createPerformanceConfig();
    
    // Load preset from file
    static std::shared_ptr<Config> loadPreset(const std::string& presetName);
    
    // Save preset to file
    static bool savePreset(const std::string& presetName, const Config& config);
    
    // List available presets
    static std::vector<std::string> listPresets();
    
private:
    static std::map<std::string, std::shared_ptr<Config>> presets_;
    static const std::string PRESETS_DIR;
};

// Configuration validator
class ConfigValidator {
public:
    ConfigValidator();
    
    // Validate configuration against schema
    bool validate(const Config& config, std::vector<std::string>& errors) const;
    
    // Get validation warnings (non-fatal issues)
    bool validateWithWarnings(const Config& config, std::vector<std::string>& warnings) const;
    
    // Apply smart defaults for missing parameters
    void applySmartDefaults(Config& config) const;
    
    // Check for parameter conflicts
    bool checkConflicts(const Config& config, std::vector<std::string>& conflicts) const;
    
    // Validate parameter ranges
    bool validateRanges(const Config& config, std::vector<std::string>& errors) const;
    
    // Get validation statistics
    struct ValidationStats {
        size_t totalParameters;
        size_t validParameters;
        size_t invalidParameters;
        size_t warningsGenerated;
    };
    
    ValidationStats getValidationStats(const Config& config) const;
    
private:
    ConfigSchema schema_;
    void initializeSchema();
};

} // namespace nlm
