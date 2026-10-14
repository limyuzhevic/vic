// Configuration Schema Definition
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "double", "string", "bool", "vector"
    std::string description;
    std::string defaultValue;
    std::pair<double, double> range; // min, max (empty for no range)
    bool required;
};

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (supports JSON, key=value, and YAML)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON file
    bool loadFromJson(const std::string& filepath);
    
    // Load from key=value file (legacy format)
    bool loadFromKeyValue(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJsonString(const std::string& jsonStr);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to key=value file (legacy format)
    bool saveToKeyValue(const std::string& filepath) const;
    
    // Get values with type conversion and validation
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
    
    // Set value with validation
    bool setValidated(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence and validation
    bool has(const std::string& key) const;
    bool isValid(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Get schema information
    static std::vector<ConfigSchema> getDefaultSchema();
    
    // Load schema from JSON schema file
    bool loadSchemaFromJson(const std::string& filepath);
    
    // Validate all configuration values
    bool validateAll() const;
    
    // Export configuration to JSON
    std::string exportToJson() const;
    
    // Import configuration from JSON
    bool importFromJson(const std::string& jsonStr);
    
    // Merge configuration from another config object
    void mergeFrom(const Config& other);
    
    // Get configuration statistics
    struct Stats {
        size_t totalEntries;
        size_t validEntries;
        size_t invalidEntries;
        std::vector<std::string> sources; // List of config sources
    };
    
    Stats getStats() const;
    
    // Get configuration health score (0-100)
    int getHealthScore() const;
    
    // Check for deprecated keys
    std::vector<std::string> getDeprecatedKeys() const;
    
    // Get recommended replacements for deprecated keys
    std::vector<std::pair<std::string, std::string>> getDeprecatedKeyReplacements() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidJsonKey(const std::string& key);
    static std::optional<ConfigValue> parseJsonValue(const nlohmann::json& jsonValue);
    static nlohmann::json convertToJson(const ConfigValue& value) const;
    static std::string jsonToString(const nlohmann::json& jsonValue);
    
    // Schema validation
    static bool validateValue(const std::string& key, const ConfigValue& value, const ConfigSchema& schema);
    static std::string getTypeName(const std::type_info& typeInfo);
    
    // Configuration migration
    void migrateFromLegacy(const Config& legacyConfig);
    static bool isKeyDeprecated(const std::string& key);
    static std::string getReplacementKey(const std::string& deprecatedKey);
    
    // JSON parsing helpers
    bool parseJsonObject(const nlohmann::json& jsonObject);
    bool parseLegacyFormat(const nlohmann::json& jsonLegacy);
    
    // Validation helpers
    bool validateRange(const ConfigValue& value, const std::pair<double, double>& range) const;
    bool validateStringLength(const std::string& value, size_t minLength, size_t maxLength) const;
    
    // Error handling
    void addError(const std::string& key, const std::string& error);
    void clearErrors();
    
    // Configuration source tracking
    void addSource(const std::string& key, ConfigSource source);
    
    // Performance optimization
    void optimizeStorage();
    
    // Compression for large configurations
    bool compressConfiguration();
    bool decompressConfiguration();
    
    // Backup and restore
    bool backupConfiguration(const std::string& backupPath) const;
    bool restoreConfiguration(const std::string& backupPath);
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
    // Get configuration fingerprint for change detection
    std::string getFingerprint() const;
};