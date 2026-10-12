// Forward declarations
class ConfigValidator;

namespace nlm {

// Configuration validation result
struct ConfigValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ConfigValidationResult() : valid(true) {}
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getSummary() const {
        std::ostringstream oss;
        if (!valid) {
            oss << "Configuration validation failed:\n";
            for (const auto& error : errors) {
                oss << "  ERROR: " << error << "\n";
            }
        }
        if (!warnings.empty()) {
            oss << "Warnings:\n";
            for (const auto& warning : warnings) {
                oss << "  WARNING: " << warning << "\n";
            }
        }
        if (valid && warnings.empty()) {
            oss << "Configuration is valid.\n";
        }
        return oss.str();
    }
};

// Required configuration keys validation
class ConfigValidator {
public:
    // Validate configuration against a set of required keys
    static ConfigValidationResult validateRequiredKeys(const Config& config, 
        const std::vector<std::string>& requiredKeys);
    
    // Validate configuration value types against schema
    static ConfigValidationResult validateTypes(const Config& config,
        const std::unordered_map<std::string, std::type_index>& schema);
    
    // Validate configuration values against constraints
    static ConfigValidationResult validateConstraints(const Config& config,
        const std::unordered_map<std::string, std::function<bool(const ConfigValue&)>>& constraints);
    
    // Validate configuration values range
    static ConfigValidationResult validateRanges(const Config& config,
        const std::unordered_map<std::string, std::pair<double, double>>& ranges);
    
    // Validate that configuration has no missing dependencies
    static ConfigValidationResult validateDependencies(const Config& config,
        const std::unordered_map<std::string, std::vector<std::string>>& dependencies);
    
    // Comprehensive validation
    static ConfigValidationResult validate(const Config& config,
        const std::vector<std::string>& requiredKeys = {},
        const std::unordered_map<std::string, std::type_index>& schema = {},
        const std::unordered_map<std::string, std::function<bool(const ConfigValue&)>>& constraints = {});
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Configuration validation
    ConfigValidationResult validate(const std::vector<std::string>& requiredKeys = {}) const;
    ConfigValidationResult validateTypes(const std::unordered_map<std::string, std::type_index>& schema) const;
    ConfigValidationResult validateConstraints(const std::unordered_map<std::string, std::function<bool(const ConfigValue&)>>& constraints) const;
    
    // Merge configuration from another config (runtime overrides)
    void merge(const Config& other);
    
    // Get configuration by source
    std::vector<ConfigEntry> getEntries(ConfigSource source) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

// Validation helper functions
namespace ConfigValidation {
    // Validate configuration type
    bool validateType(const ConfigValue& value, const std::type_index& expectedType);
    
    // Validate string configuration
    bool validateString(const std::string& value, size_t minLength = 0, size_t maxLength = std::numeric_limits<size_t>::max());
    
    // Validate numeric configuration
    bool validateNumber(const ConfigValue& value, double min = std::numeric_limits<double>::lowest(), 
                       double max = std::numeric_limits<double>::max());
    
    // Validate boolean configuration
    bool validateBoolean(const ConfigValue& value);
    
    // Validate array configuration
    bool validateArray(const ConfigValue& value, size_t minSize = 0, size_t maxSize = std::numeric_limits<size_t>::max());
    
    // Validate configuration path
    bool validatePath(const std::string& path);
    
    // Sanitize configuration key
    std::string sanitizeKey(const std::string& key);
}

} // namespace nlm
