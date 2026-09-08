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
    bool validate(bool treatWarningsAsErrors = false);
    std::vector<ValidationError> getValidationErrors() const;
    void addValidationRule(const ValidationRule& rule);
    void clearValidationRules();
    
    // Range validation helpers
    static bool validateRange(const ConfigValue& value, double min, double max);
    
    // Pattern validation helpers
    static bool validatePattern(const std::string& value, const std::string& pattern);
    
    // Conditional validation
    void addConditionalValidation(const std::string& conditionKey, const std::string& dependentKey,
                                  std::function<bool(const ConfigValue&, const ConfigValue&)> validator);
    
    // Default configuration templates
    static ConfigTemplate getPhase2Template();
    static ConfigTemplate getPhase4Template();
    static ConfigTemplate getPhase6Template();
    void applyTemplate(const ConfigTemplate& template);
    
    // Configuration inheritance/substitution
    void applyInheritance(const Config& parentConfig);
    bool resolveSubstitutions();
    
    // Configuration merge strategies
    enum class MergeStrategy {
        Override,
        Merge,
        KeepExisting,
        DeepMerge
    };
    void merge(const Config& other, MergeStrategy strategy = MergeStrategy::Override);
    
    // Serialization enhancements
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    bool saveToBinary(const std::string& filepath) const;
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromBinary(const std::string& filepath);
    
    // Configuration versioning
    void incrementVersion(const std::string& description = "");
    std::string getVersion() const;
    std::vector<ConfigVersion> getVersionHistory() const;
    
    // Configuration comparison utilities
    bool hasDiff(const Config& other) const;
    std::vector<ConfigDiff> getDiff(const Config& other) const;
    
    // Configuration patch system
    bool applyPatch(const std::vector<ConfigDiff>& diff);
    bool exportDiff(const std::string& filepath) const;
    bool importDiff(const std::string& filepath);
    
    // Configuration macro substitution
    bool substituteMacros(const std::map<std::string, std::string>& macros);
    
    // Advanced features
    bool exportConfig(const std::string& filepath, const std::string& format = "auto") const;
    bool importConfig(const std::string& filepath, const std::string& format = "auto");
    
    // Configuration monitoring
    void setMonitorEnabled(bool enabled);
    bool isMonitorEnabled() const;
    std::vector<ConfigError> getRecentErrors() const;
    std::vector<ConfigError> getRecentWarnings() const;
    
    // Get detailed validation report
    std::string getValidationReport() const;
    
    // File format detection
    static std::string detectFileFormat(const std::string& filepath);
    
    // Error reporting
    static ConfigError createError(ConfigErrorType type, const std::string& message, 
                                  const std::string& key = "", const std::string& file = "",
                                  int line = 0, int column = 0, bool isWarning = false);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Internal validation implementation
    void validateEntry(const ConfigEntry& entry, std::vector<ValidationError>& errors) const;
    void validateConditionalDependencies(std::vector<ValidationError>& errors) const;
    
    // Error handling helpers
    void reportError(const ConfigError& error);
    void reportWarning(const ConfigError& error);
};

} // namespace nlm
