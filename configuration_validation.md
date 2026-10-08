// NLM Configuration Validation and Schema Checking
// Advanced configuration validation for NLM brain simulations

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <chrono>
#include <regex>

// Forward declarations
namespace nlm {
    class Config;
}

namespace config {
    
    // Configuration validation errors
    class ConfigurationError : public std::runtime_error {
    public:
        ConfigurationError(const std::string& message, const std::string& parameter = "")
            : std::runtime_error(message), parameterName(parameter) {}
        
        std::string getParameterName() const { return parameterName; }
    
    private:
        std::string parameterName;
    };
    
    class ConfigurationWarning : public std::runtime_error {
    public:
        ConfigurationWarning(const std::string& message, const std::string& parameter = "")
            : std::runtime_error(message), parameterName(parameter) {}
        
        std::string getParameterName() const { return parameterName; }
    
    private:
        std::string parameterName;
    };
    
    // Configuration parameter types
    enum class ParameterType {
        INTEGER,
        FLOAT,
        DOUBLE,
        STRING,
        BOOLEAN,
        VECTOR_INT,
        VECTOR_FLOAT,
        VECTOR_DOUBLE,
        VECTOR_STRING,
        MAP_STRING_DOUBLE,
        MAP_STRING_STRING
    };
    
    // Configuration schema validation rules
    struct ValidationRule {
        std::string parameterName;
        ParameterType type;
        bool required;
        std::string description;
        std::vector<std::string> allowedValues;
        
        // Range validation for numeric types
        double minValue;
        double maxValue;
        bool hasMin;
        bool hasMax;
        
        // Validation functions
        std::function<bool(const std::string&)> customValidator;
        std::function<std::string(const std::string&)> transformer;
        
        ValidationRule()
            : required(false), minValue(0.0), maxValue(0.0), hasMin(false), hasMax(false) {}
    };
    
    // Configuration schema
    class ConfigurationSchema {
    public:
        ConfigurationSchema();
        ~ConfigurationSchema();
        
        void addParameter(const std::string& name, ParameterType type, bool required = false,
                         const std::string& description = "",
                         const std::vector<std::string>& allowedValues = {});
        
        void addParameter(const std::string& name, ParameterType type, double min, double max,
                         bool required = false, const std::string& description = "");
        
        void addParameter(const std::string& name, ParameterType type,
                         std::function<bool(const std::string&)> validator,
                         const std::string& description = "");
        
        bool validateParameter(const std::string& name, const std::string& value) const;
        bool validateParameter(const std::string& name, double value) const;
        bool validateParameter(const std::string& name, int value) const;
        bool validateParameter(const std::string& name, bool value) const;
        
        std::string getParameterDescription(const std::string& name) const;
        ParameterType getParameterType(const std::string& name) const;
        bool isParameterRequired(const std::string& name) const;
        
        std::vector<std::string> getAllParameterNames() const;
        std::vector<ValidationRule> getAllRules() const;
        
        bool validateAll(const std::map<std::string, std::string>& parameters) const;
        std::vector<ConfigurationError> validateWithErrors(const std::map<std::string, std::string>& parameters) const;
        
        std::string generateSchemaReport() const;
        
        void saveToFile(const std::string& filename) const;
        bool loadFromFile(const std::string& filename);
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Advanced configuration manager
    class AdvancedConfigurationManager {
    public:
        AdvancedConfigurationManager();
        ~AdvancedConfigurationManager();
        
        void setSchema(const ConfigurationSchema& schema);
        void setDefaultValues(const std::map<std::string, std::string>& defaults);
        void setValidationEnabled(bool enabled);
        void setWarningEnabled(bool enabled);
        
        bool loadFromFile(const std::string& filename);
        bool saveToFile(const std::string& filename) const;
        
        bool validate(const std::map<std::string, std::string>& parameters) const;
        std::vector<ConfigurationError> validateWithErrors(const std::map<std::string, std::string>& parameters) const;
        
        bool applyConfig(const std::map<std::string, std::string>& parameters, nlm::Config& config) const;
        std::map<std::string, std::string> getEffectiveConfig(
            const std::map<std::string, std::string>& userConfig) const;
        
        std::vector<std::string> getRecommendedParameters() const;
        std::map<std::string, std::string> getParameterMetadata(const std::string& name) const;
        
        std::string generateValidationReport(const std::map<std::string, std::string>& parameters) const;
        std::string generateConfigSummary(const std::map<std::string, std::string>& parameters) const;
        
        void clear();
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Configuration template system
    class ConfigurationTemplate {
    public:
        struct TemplateParameter {
            std::string name;
            std::string defaultValue;
            std::string description;
            ParameterType type;
            bool editable;
            std::vector<std::string> possibleValues;
        };
        
        ConfigurationTemplate(const std::string& name, const std::string& description);
        ~ConfigurationTemplate();
        
        void addParameter(const std::string& name, const std::string& defaultValue,
                         const std::string& description, ParameterType type,
                         bool editable = true, const std::vector<std::string>& possibleValues = {});
        
        std::string getName() const;
        std::string getDescription() const;
        std::vector<TemplateParameter> getParameters() const;
        
        std::map<std::string, std::string> getDefaultConfig() const;
        bool validateConfig(const std::map<std::string, std::string>& config) const;
        
        void saveToFile(const std::string& filename) const;
        bool loadFromFile(const std::string& filename);
        
        std::vector<std::string> getSimilarTemplates(const std::string& keyword) const;
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Configuration comparison and diff
    class ConfigurationDiff {
    public:
        struct DiffEntry {
            enum class Type {
                ADDED,
                REMOVED,
                MODIFIED,
                UNCHANGED
            };
            
            std::string parameterName;
            Type type;
            std::string oldValue;
            std::string newValue;
            std::string description;
            
            DiffEntry(const std::string& name, Type t, const std::string& oldVal = "",
                     const std::string& newVal = "", const std::string& desc = "")
                : parameterName(name), type(t), oldValue(oldVal), newValue(newVal), description(desc) {}
        };
        
        ConfigurationDiff(const std::map<std::string, std::string>& original,
                         const std::map<std::string, std::string>& modified);
        
        std::vector<DiffEntry> getDiff() const;
        std::vector<DiffEntry> getDiffForParameter(const std::string& parameterName) const;
        
        std::map<std::string, std::string> applyDiff(const std::map<std::string, std::string>& original) const;
        
        std::string generateDiffReport() const;
        std::string generateDetailedDiffReport() const;
        
        bool isIdentical(const std::map<std::string, std::string>& other) const;
        
        std::vector<std::string> getChangedParameters() const;
        std::vector<std::string> getUnchangedParameters() const;
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Configuration validator utilities
    class ConfigurationValidator {
    public:
        ConfigurationValidator();
        ~ConfigurationValidator();
        
        static bool isValidParameterName(const std::string& name);
        static bool isValidParameterValue(const std::string& value, ParameterType type);
        static bool isValidParameterValue(double value, ParameterType type);
        static bool isValidParameterValue(int value, ParameterType type);
        static bool isValidParameterValue(bool value, ParameterType type);
        
        static std::string normalizeParameterName(const std::string& name);
        static std::string normalizeParameterValue(const std::string& value, ParameterType type);
        
        static bool checkRange(double value, double min, double max);
        static bool checkAllowedValues(const std::string& value, const std::vector<std::string>& allowedValues);
        
        static std::vector<std::string> getCommonParameterNames();
        static std::map<std::string, ParameterType> getStandardParameterTypes();
        static std::map<std::string, std::string> getParameterDescriptions();
        
        static std::string generateValidationSummary(const std::vector<ConfigurationError>& errors);
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Configuration exporter/importer
    class ConfigurationIO {
    public:
        ConfigurationIO();
        ~ConfigurationIO();
        
        static bool exportToJSON(const std::map<std::string, std::string>& config, const std::string& filename);
        static bool importFromJSON(const std::string& filename, std::map<std::string, std::string>& config);
        
        static bool exportToINI(const std::map<std::string, std::string>& config, const std::string& filename);
        static bool importFromINI(const std::string& filename, std::map<std::string, std::string>& config);
        
        static bool exportToXML(const std::map<std::string, std::string>& config, const std::string& filename);
        static bool importFromXML(const std::string& filename, std::map<std::string, std::string>& config);
        
        static bool exportToCSV(const std::map<std::string, std::string>& config, const std::string& filename);
        static bool importFromCSV(const std::string& filename, std::map<std::string, std::string>& config);
        
        static std::string generateDocumentation(const std::map<std::string, std::string>& config);
        static std::string generateUsageExample(const std::map<std::string, std::string>& config);
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Configuration template library
    namespace templates {
        
        // Brain configuration templates
        ConfigurationTemplate createBasicBrainTemplate();
        ConfigurationTemplate createHighPerformanceBrainTemplate();
        ConfigurationTemplate createLearningFocusedBrainTemplate();
        ConfigurationTemplate createDevelopmentBrainTemplate();
        ConfigurationTemplate createSocialLearningBrainTemplate();
        ConfigurationTemplate createCuriosityDrivenBrainTemplate();
        
        // Experiment configuration templates
        ConfigurationTemplate createBasicExperimentTemplate();
        ConfigurationTemplate createLongTermExperimentTemplate();
        ConfigurationTemplate createComparativeExperimentTemplate();
        ConfigurationTemplate createAdaptiveExperimentTemplate();
        
        // Performance configuration templates
        ConfigurationTemplate createPerformanceTestTemplate();
        ConfigurationTemplate createScalabilityTestTemplate();
        ConfigurationTemplate createMemoryTestTemplate();
        ConfigurationTemplate createSpeedTestTemplate();
        
        // Environment configuration templates
        ConfigurationTemplate createSimpleEnvironmentTemplate();
        ConfigurationTemplate createComplexEnvironmentTemplate();
        ConfigurationTemplate createDynamicEnvironmentTemplate();
        ConfigurationTemplate createSocialEnvironmentTemplate();
        
        // Integration test templates
        ConfigurationTemplate createUnitTestTemplate();
        ConfigurationTemplate createIntegrationTestTemplate();
        ConfigurationTemplate createPerformanceTestTemplate();
        ConfigurationTemplate createEndToEndTestTemplate();
        
        std::vector<ConfigurationTemplate> getAllAvailableTemplates();
        ConfigurationTemplate findTemplateByName(const std::string& name);
        std::vector<ConfigurationTemplate> findTemplatesByKeyword(const std::string& keyword);
        
        void saveAllTemplates(const std::string& directory);
        bool loadAllTemplates(const std::string& directory);
        
    }
    
    // Configuration analysis utilities
    class ConfigurationAnalyzer {
    public:
        ConfigurationAnalyzer();
        ~ConfigurationAnalyzer();
        
        struct AnalysisResult {
            std::string parameterName;
            double currentValue;
            double recommendedValue;
            double difference;
            double impactScore;
            std::string recommendation;
            std::string category; // "performance", "learning", "development", "neuromodulation"
        };
        
        std::vector<AnalysisResult> analyzeConfiguration(
            const std::map<std::string, std::string>& config,
            const std::string& experimentType = "general") const;
        
        std::vector<AnalysisResult> optimizeConfiguration(
            const std::map<std::string, std::string>& currentConfig,
            const std::string& objective = "performance") const;
        
        double calculateConfigurationScore(
            const std::map<std::string, std::string>& config,
            const std::string& experimentType = "general") const;
        
        std::string generateOptimizationRecommendations(
            const std::map<std::string, std::string>& config,
            const std::string& experimentType = "general") const;
        
        std::map<std::string, double> calculateCorrelations(
            const std::vector<std::map<std::string, std::string>>& configs,
            const std::vector<double>& results) const;
        
        std::string generateAnalysisReport(
            const std::map<std::string, std::string>& config,
            const std::string& experimentType = "general") const;
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Configuration migration utilities
    class ConfigurationMigration {
    public:
        struct MigrationRule {
            std::string fromParameter;
            std::string toParameter;
            std::function<std::string(const std::string&)> transform;
            std::string description;
        };
        
        ConfigurationMigration();
        ~ConfigurationMigration();
        
        void addMigrationRule(const std::string& from, const std::string& to,
                             std::function<std::string(const std::string&)> transform,
                             const std::string& description = "");
        
        std::map<std::string, std::string> migrate(
            const std::map<std::string, std::string>& oldConfig) const;
        
        bool validateMigrationRules(const std::map<std::string, std::string>& config) const;
        
        std::string generateMigrationReport() const;
        
        void saveMigrationRules(const std::string& filename) const;
        bool loadMigrationRules(const std::string& filename);
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Main configuration management application
    class ConfigurationManagementApp {
    public:
        ConfigurationManagementApp();
        ~ConfigurationManagementApp();
        
        void initialize();
        
        void loadDefaultSchemas();
        void loadDefaultTemplates();
        
        void runConfigurationValidationTests();
        void runTemplateTests();
        void runMigrationTests();
        void runAnalysisTests();
        
        void generateComprehensiveReport();
        
        void saveTestResults();
        void exportConfigurationExamples();
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Configuration test functions
    void runConfigurationSchemaTests();
    void runConfigurationValidationTests();
    void runTemplateTests();
    void runMigrationTests();
    void runAnalysisTests();
    void runIOOperationsTests();
    
    // Configuration main function
    int main(int argc, char* argv[]);
}

// Configuration testing namespace
namespace config_tests {
    void runAllConfigurationTests();
    void runSchemaValidationTests();
    void runTemplateTests();
    void runMigrationTests();
    void runAnalysisTests();
    void runIOOperationsTests();
}

// Configuration demonstration
namespace config_demo {
    void demonstrateBasicConfiguration();
    void demonstrateAdvancedValidation();
    void demonstrateTemplateUsage();
    void demonstrateMigration();
    void demonstrateAnalysis();
    void demonstrateIOOperations();
}