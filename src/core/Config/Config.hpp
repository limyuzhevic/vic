// Configuration schema validation
enum class ConfigValidationError {
    None,
    InvalidType,
    OutOfRange,
    MissingRequired,
    InvalidValue
};

// Configuration schema definition
struct ConfigSchemaEntry {
    std::string key;
    std::type_index type;
    std::any defaultValue;
    std::string description;
    bool required;
    ConfigValidationError error;
    
    ConfigSchemaEntry() : key(), type(typeid(void)), defaultValue(), description(), required(false), error(ConfigValidationError::None) {}
    ConfigSchemaEntry(const std::string& k, const std::any& def, const std::string& desc, bool req = false)
        : key(k), type(def.type()), defaultValue(def), description(desc), required(req), error(ConfigValidationError::None) {}
};

// Configuration validator interface
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    virtual bool validate(const std::string& key, const ConfigValue& value, ConfigValidationError& error) = 0;
    virtual std::string getErrorMessage(ConfigValidationError error) const = 0;
};

// Advanced configuration class with schema validation and runtime features
class AdvancedConfig {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Disable copying, enable moving
    AdvancedConfig(const AdvancedConfig&) = delete;
    AdvancedConfig& operator=(const AdvancedConfig&) = delete;
    AdvancedConfig(AdvancedConfig&&) noexcept;
    AdvancedConfig& operator=(AdvancedConfig&&) noexcept;
    
    // Schema management
    void registerSchema(const std::string& key, const std::any& defaultValue, 
                       const std::string& description, bool required = false);
    void setValidator(std::unique_ptr<ConfigValidator> validator);
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv, bool validate = true);
    
    // Save to file
    bool saveToFile(const std::string& filepath, bool includeComments = true) const;
    
    // Get values with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Safe set values with validation
    bool set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Runtime configuration adjustment
    bool adjust(const std::string& key, const std::string& operation, const std::string& parameter);
    
    // Configuration validation
    bool validateAll() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Advanced configuration features
    void enableAutoSave(const std::string& filepath, double interval = 60.0);
    void disableAutoSave();
    bool isAutoSaveEnabled() const;
    
    // Configuration profiles
    bool saveProfile(const std::string& profileName) const;
    bool loadProfile(const std::string& profileName);
    std::vector<std::string> getAvailableProfiles() const;
    
    // Configuration comparison
    bool equals(const AdvancedConfig& other) const;
    std::string diff(const AdvancedConfig& other) const;
    
    // Get configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    double getMemoryUsage() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseJSONValue(const std::string& str, ConfigValue& outValue);
    static std::string configValueToString(const ConfigValue& value);
};
