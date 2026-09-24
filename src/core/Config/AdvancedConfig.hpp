// Advanced Configuration System for NLM
// This file contains enhanced configuration management with validation, presets, and advanced features

#ifndef ADVANCED_CONFIG_HPP
#define ADVANCED_CONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <unordered_set>
#include <chrono>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "core/Config/Config.hpp"

namespace nlm {

// Advanced configuration validation
class ConfigValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::map<std::string, std::string> suggestions;
    };
    
    struct ConfigSchema {
        std::string name;
        std::string description;
        std::map<std::string, std::pair<std::string, std::string>> typeRanges;
        std::vector<std::string> requiredKeys;
        std::vector<std::string> optionalKeys;
        std::map<std::string, std::vector<std::string>> dependencies;
    };
    
    class ValidationRule {
    public:
        enum class Type { Range, StringLength, Pattern, Value, Custom };
        
        ValidationRule(Type type, const std::string& key, const std::string& errorMsg);
        virtual ~ValidationRule() = default;
        virtual ValidationResult validate(const Config& config) const;
        Type getType() const { return type_; }
        const std::string& getKey() const { return key_; }
        
    protected:
        Type type_;
        std::string key_;
        std::string errorMsg_;
    };
    
    class RangeRule : public ValidationRule {
    public:
        RangeRule(const std::string& key, double min, double max, const std::string& errorMsg);
        ValidationResult validate(const Config& config) const override;
        
    private:
        double min_;
        double max_;
    };
    
    class StringLengthRule : public ValidationRule {
    public:
        StringLengthRule(const std::string& key, size_t min, size_t max, const std::string& errorMsg);
        ValidationResult validate(const Config& config) const override;
        
    private:
        size_t min_;
        size_t max_;
    };
    
    class PatternRule : public ValidationRule {
    public:
        PatternRule(const std::string& key, const std::string& pattern, const std::string& errorMsg);
        ValidationResult validate(const Config& config) const override;
        
    private:
        std::regex pattern_;
    };
    
    class ValueRule : public ValidationRule {
    public:
        ValueRule(const std::string& key, const std::string& allowedValue, const std::string& errorMsg);
        ValidationResult validate(const Config& config) const override;
        
    private:
        std::string allowedValue_;
    };
    
    ValidationResult validate(const Config& config) const;
    
    void addRule(std::unique_ptr<ValidationRule> rule) { rules_.push_back(std::move(rule)); }
    void setSchema(const ConfigSchema& schema) { schema_ = schema; }
    
private:
    std::vector<std::unique_ptr<ValidationRule>> rules_;
    std::optional<ConfigSchema> schema_;
};

// Advanced configuration manager with presets and validation
class AdvancedConfig {
public:
    struct Preset {
        std::string name;
        std::string description;
        std::shared_ptr<Config> config;
        std::vector<std::string> tags;
        double performanceScore;
    };
    
    struct ExportOptions {
        bool prettyPrint;
        bool includeComments;
        std::vector<std::string> keyFilter;
        std::string outputFormat;
    };
    
    struct ImportOptions {
        bool validateOnImport;
        bool mergeWithExisting;
        std::string configPath;
    };
    
    AdvancedConfig();
    ~AdvancedConfig() = default;
    
    // Configuration management
    bool setValue(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::User);
    bool setValue(const std::string& key, int64_t value, ConfigSource source = ConfigSource::User);
    bool setValue(const std::string& key, double value, ConfigSource source = ConfigSource::User);
    bool setValue(const std::string& key, float value, ConfigSource source = ConfigSource::User);
    bool setValue(const std::string& key, bool value, ConfigSource source = ConfigSource::User);
    
    template<typename T>
    bool setValue(const std::string& key, const T& value, ConfigSource source = ConfigSource::User) {
        std::stringstream ss;
        ss << value;
        return setValue(key, ss.str(), source);
    }
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int64_t getInt(const std::string& key, int64_t defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    float getFloat(const std::string& key, float defaultValue = 0.0f) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
    // Presets management
    void addPreset(const Preset& preset);
    bool removePreset(const std::string& name);
    Preset* getPreset(const std::string& name);
    const std::vector<Preset>& getPresets() const { return presets_; }
    
    // Configuration categories
    struct ConfigCategory {
        std::string name;
        std::string description;
        std::map<std::string, std::string> defaultValues;
        std::vector<std::string> keys;
    };
    
    void addCategory(const ConfigCategory& category) { categories_.push_back(category); }
    const std::vector<ConfigCategory>& getCategories() const { return categories_; }
    
    std::vector<std::string> getCategoryKeys(const std::string& categoryName) const;
    
    // Validation and analysis
    ConfigValidator::ValidationResult validate() const;
    std::map<std::string, std::string> analyzePerformance() const;
    std::vector<std::string> getSuggestions() const;
    
    // Export/import
    bool exportToFile(const std::string& filepath, const ExportOptions& options = {}) const;
    bool importFromFile(const std::string& filepath, const ImportOptions& options = {});
    std::string exportToJson(const ExportOptions& options = {}) const;
    bool importFromJson(const std::string& jsonStr, const ImportOptions& options = {});
    
    // Preset utilities
    std::vector<Preset> findPresetsByTag(const std::string& tag) const;
    std::vector<Preset> findPresetsByScore(double minScore) const;
    void updatePresetScores();
    
    // Configuration templates
    struct ConfigTemplate {
        std::string name;
        std::string description;
        std::shared_ptr<Config> templateConfig;
        std::map<std::string, std::string> variables;
    };
    
    void addTemplate(const ConfigTemplate& template_) { templates_.push_back(template_); }
    std::shared_ptr<Config> createFromTemplate(const std::string& templateName, 
                                               const std::map<std::string, std::string>& variables = {});
    
    // Statistics and monitoring
    struct ConfigStats {
        size_t totalKeys;
        size_t userKeys;
        size_t defaultKeys;
        size_t systemKeys;
        double validationScore;
        std::chrono::system_clock::time_point lastModified;
        std::map<std::string, size_t> keyUsageCount;
    };
    
    ConfigStats getStats() const;
    
    std::string getKeyType(const std::string& key) const;
    
    static bool typesAreCompatible(const std::string& type1, const std::string& type2);
    
    bool isCompatible(const AdvancedConfig& other) const;
    
    // Get the underlying config
    std::shared_ptr<Config> getInternalConfig() const { return config_; }
    
private:
    std::shared_ptr<Config> config_;
    std::vector<Preset> presets_;
    std::vector<ConfigCategory> categories_;
    std::vector<ConfigTemplate> templates_;
    ConfigValidator validator_;
    
    mutable ConfigStats stats_;
    
    void updateStats() const;
    std::string toString(const std::any& value) const;
    std::any parseValue(const std::string& value, const std::string& key) const;
};

// Pre-built advanced presets for common use cases
class AdvancedPresets {
public:
    static AdvancedConfig::Preset createResearchBrainPreset();
    static AdvancedConfig::Preset createProductionBrainPreset();
    static AdvancedConfig::Preset createDevelopmentBrainPreset();
    static AdvancedConfig::Preset createRealTimeBrainPreset();
    static AdvancedConfig::Preset createMemoryFocusedPreset();
    static AdvancedConfig::Preset createLearningFocusedPreset();
    static AdvancedConfig::Preset createVisualizationPreset();
    
    static std::vector<AdvancedConfig::Preset> getAllPresets();
    
    static AdvancedConfig::ConfigTemplate createCustomTemplate(const std::string& name,
                                                              const std::string& description,
                                                              const std::map<std::string, std::string>& variables);
};

// Configuration validation helper functions
namespace ConfigValidation {
    bool validateNeuronCount(int64_t count);
    bool validateConnectionProbability(float prob);
    bool validateTimestep(double timestep);
    bool validateLearningRate(float rate);
    bool validatePlasticityParameters(const Config& config);
    bool validateNeuromodulationParameters(const Config& config);
    
    std::string getValidationErrorMessage(const std::string& key, const std::string& error);
    
    // Configuration recommendations
    std::map<std::string, std::string> getRecommendations(const Config& config);
    std::string getPerformanceRecommendation(const Config& config);
}

// Configuration merge utilities
namespace ConfigMerge {
    struct MergeResult {
        std::shared_ptr<Config> mergedConfig;
        std::vector<std::string> conflicts;
        std::vector<std::string> changes;
        bool successful;
    };
    
    MergeResult mergeConfigs(const std::shared_ptr<Config>& base,
                            const std::shared_ptr<Config>& override,
                            bool overwriteConflicting = true);
    
    std::vector<MergeResult> mergeMultipleConfigs(const std::vector<std::shared_ptr<Config>>& configs);
}

} // namespace nlm

#endif // ADVANCED_CONFIG_HPP