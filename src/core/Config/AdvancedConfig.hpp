#ifndef NLM_ADVANCED_CONFIG_HPP
#define NLM_ADVANCED_CONFIG_HPP

#include "core/Config/Config.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace nlm {

// Advanced configuration system with schema validation
class AdvancedConfig {
public:
    // Configuration schema validation result
    struct ValidationResult {
        bool isValid;
        std::string message;
        std::unordered_map<std::string, std::string> warnings;
        std::unordered_map<std::string, std::string> errors;
        
        ValidationResult() : isValid(true) {}
    };

    // Configuration inheritance
    struct InheritanceConfig {
        std::string baseConfigFile;
        std::vector<std::string> overrideKeys;
        bool inheritDefaults = true;
        std::unordered_map<std::string, json> inheritanceRules;
    };

    // Real-time logging configuration
    struct LoggingConfig {
        std::string level = "INFO";
        bool console = true;
        bool file = false;
        std::string filename = "nlm.log";
        bool append = true;
        size_t maxSize = 10 * 1024 * 1024; // 10MB
        bool colored = true;
        bool timestamps = true;
    };

    // Profiling and debugging configuration
    struct DebugConfig {
        bool enabled = false;
        bool profile = false;
        bool breakpoints = false;
        bool validate = false;
        bool verbose = false;
        std::string profileOutput = "profile.json";
        double timeLimit = 0.0; // seconds
    };

    // Configuration templates
    enum class TemplateType {
        PHASE1_BASIC,
        PHASE2_NEURAL_COMPUTATION,
        PHASE3_AGENT_WORLD_INTERACTION,
        PHASE4_COGNITIVE_DEVELOPMENT,
        PHASE5_PERFORMANCE_OPTIMIZATION,
        PHASE6_FULL_INTEGRATION
    };

    AdvancedConfig();
    ~AdvancedConfig();
    
    // Load configuration from file (JSON/YAML/key=value)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Process configuration with inheritance
    bool process(const InheritanceConfig& inheritance = InheritanceConfig());
    
    // Validate configuration against schema
    ValidationResult validate() const;
    
    // Apply configuration to base Config system
    void applyToBaseConfig(std::shared_ptr<Config> baseConfig) const;
    
    // Get raw configuration values
    const json& getRaw() const { return configData; }
    
    // Get configuration value with type conversion
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T()) const;
    
    // Get raw value
    json getRaw(const std::string& key) const;
    
    // Set configuration value
    void set(const std::string& key, const json& value, ConfigSource source = ConfigSource::Runtime);
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Export configuration to JSON
    std::string exportToJson() const;
    
    // Load template configuration
    bool loadTemplate(TemplateType type);
    
    // Get template JSON
    static json getTemplateJson(TemplateType type);
    
    // Batch mode processing
    bool processBatch(const std::vector<std::string>& configFiles);
    
    // Interactive mode setup
    void interactiveSetup();
    
    // Real-time logging configuration
    void configureLogging(const LoggingConfig& logging);
    
    // Debug configuration
    void configureDebug(const DebugConfig& debug);
    
    // Get current configurations
    const LoggingConfig& getLoggingConfig() const { return loggingConfig; }
    const DebugConfig& getDebugConfig() const { return debugConfig; }
    
    // Parallel execution support
    bool executeParallel(const std::vector<std::string>& configFiles);
    
    // Validation and export options
    ValidationResult validateAndExport(const std::string& outputPath) const;

private:
    json configData;
    LoggingConfig loggingConfig;
    DebugConfig debugConfig;
    std::shared_ptr<Config> baseConfig;
    
    // Internal helper functions
    bool loadJsonFile(const std::string& filepath);
    bool loadYamlFile(const std::string& filepath);
    bool loadKeyValueFile(const std::string& filepath);
    
    ValidationResult validateSchema() const;
    ValidationResult validateCrossComponent() const;
    
    void mergeConfigs(json& target, const json& source, const std::unordered_map<std::string, json>& inheritanceRules = {});
    
    static std::string detectFileFormat(const std::string& filepath);
};

} // namespace nlm

#endif // NLM_ADVANCED_CONFIG_HPP
