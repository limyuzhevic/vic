// Advanced configuration validation and preset system
#include "Config.hpp"
#include <map>
#include <stdexcept>

namespace nlm {

// Configuration validation rules
struct ValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
};

// Advanced configuration with validation and presets
class AdvancedConfig {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Load from various sources with validation
    bool loadFromFile(const std::string& filepath, bool validate = true);
    bool loadFromArgs(int argc, char** argv, bool validate = true);
    bool loadFromEnvironment(bool validate = true);
    
    // Configuration presets
    enum class Preset {
        Default,
        Phase2,
        Phase3,
        Phase4,
        Phase5,
        Phase6,
        Performance,
        Development,
        Production
    };
    
    // Load a preset configuration
    bool loadPreset(Preset preset, bool validate = true);
    
    // Merge configurations (source overrides base)
    void merge(const AdvancedConfig& other, ConfigSource source = ConfigSource::Runtime);
    
    // Validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration key ranges and validation
    static const std::map<std::string, std::pair<double, double>> getKeyRanges();
    static const std::map<std::string, std::vector<std::string>> getAllowedValues();
    
    // Get validation rules
    static std::vector<ValidationRule> getValidationRules();
    
    // Environment variable prefix
    static constexpr const char* ENV_PREFIX = "NLM_";
    
    // Default configuration file
    static constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";
    
    // Get the underlying config for compatibility
    const Config& getCoreConfig() const { return coreConfig_; }
    Config& getCoreConfig() { return coreConfig_; }
    
private:
    Config coreConfig_;
    std::vector<std::string> validationErrors_;
    
    // Helper functions
    bool loadFromEnvironmentVariable(const std::string& envVar, bool validate);
    bool validateValue(const std::string& key, const ConfigValue& value, std::string& error) const;
    bool applyPreset(const std::map<std::string, ConfigValue>& presetValues, bool validate);
    std::string envVarToKey(const std::string& envVar) const;
};

} // namespace nlm
