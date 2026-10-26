// Configuration validation rules
struct ValidationRule {
    std::string key;
    std::string type; // "int", "double", "bool", "string", "range"
    std::string min;
    std::string max;
    std::string description;
    bool required = false;
};

// Configuration validation interface
class IConfigValidator {
public:
    virtual ~IConfigValidator() = default;
    virtual bool validate(const std::string& key, const ConfigValue& value) const = 0;
    virtual std::string getError(const std::string& key, const ConfigValue& value) const = 0;
};

// Range validation for numeric types
class RangeValidator : public IConfigValidator {
public:
    RangeValidator(double min, double max, const std::string& errorMsg = "");
    
    bool validate(const std::string& key, const ConfigValue& value) const override;
    std::string getError(const std::string& key, const ConfigValue& value) const override;
    
    void setMin(double min);
    void setMax(double max);
    double getMin() const;
    double getMax() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Choices validator for string types
class ChoicesValidator : public IConfigValidator {
public:
    ChoicesValidator(const std::vector<std::string>& choices, const std::string& errorMsg = "");
    
    bool validate(const std::string& key, const ConfigValue& value) const override;
    std::string getError(const std::string& key, const ConfigValue& value) const override;
    
    void addChoice(const std::string& choice);
    bool hasChoice(const std::string& choice) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main configuration class with enhanced validation
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
    
    // Validation methods
    void addValidator(std::shared_ptr<IConfigValidator> validator);
    void removeValidator(const std::string& key);
    void clearValidators();
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Default management
    void setDefault(const std::string& key, const ConfigValue& value, const std::string& description = "");
    bool hasDefault(const std::string& key) const;
    ConfigValue getDefault(const std::string& key) const;
    void clearDefaults();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};
