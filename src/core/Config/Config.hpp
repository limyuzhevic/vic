// Configuration validation
struct ConfigValidationRule {
    std::string key;
    std::string description;
    std::string type;
    bool required;
    std::string defaultValue;
    std::string minValue;
    std::string maxValue;
    std::string allowedValues;
    
    ConfigValidationRule() : required(false) {}
    ConfigValidationRule(const std::string& k, const std::string& d, bool req = false)
        : key(k), description(d), required(req) {}
};

// Configuration templates
struct ConfigTemplate {
    std::string name;
    std::string description;
    std::vector<ConfigValidationRule> rules;
    std::map<std::string, std::string> defaults;
    
    ConfigTemplate(const std::string& n, const std::string& d) : name(n), description(d) {}
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
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath);
    
    // Load from JSON file
    bool loadFromJSON(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to YAML file
    bool saveToYAML(const std::string& filepath) const;
    
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
    bool validate(const ConfigTemplate& template) const;
    std::string validateAll(const std::vector<ConfigTemplate>& templates) const;
    
    // Environment variable support
    void loadFromEnvironment(const std::map<std::string, std::string>& envVars = {});
    void setEnvironmentPrefix(const std::string& prefix);
    
    // Configuration templates
    void addTemplate(const ConfigTemplate& template);
    bool applyTemplate(const std::string& templateName);
    std::vector<std::string> getAvailableTemplates() const;
    
    // Error handling
    std::string getLastError() const;
    bool hasError() const;
    void clearError();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Error handling
    mutable std::string lastError;
    
    // Environment variable support
    std::string envPrefix;
    
    // Template support
    std::map<std::string, ConfigTemplate> templates;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringToBool(const std::string& str);
    static std::string boolToString(bool value);
    static std::string doubleToString(double value);
    static std::string int64ToString(int64_t value);
    
    // Validation helpers
    static bool validateValue(const ConfigValidationRule& rule, const ConfigValue& value) const;
    static std::string valueToString(const ConfigValue& value) const;
    
    // Environment variable helpers
    static std::string getEnvVar(const std::string& name);
    static std::string envVarToKey(const std::string& envName, const std::string& prefix = "");
};
