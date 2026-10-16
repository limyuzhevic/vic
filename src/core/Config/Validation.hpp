// Configuration validation and error handling
class ConfigurationError : public std::runtime_error {
public:
    ConfigurationError(const std::string& message, const std::string& key = "", 
                      const std::string& value = "")
        : std::runtime_error("Configuration Error: " + message), 
          key(key), value(value) {}
    
    const std::string& getKey() const { return key; }
    const std::string& getValue() const { return value; }
    
private:
    std::string key;
    std::string value;
};

class ConfigurationValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        ValidationResult() : isValid(true) {}
        
        void addError(const std::string& error) {
            errors.push_back(error);
            isValid = false;
        }
        
        void addWarning(const std::string& warning) {
            warnings.push_back(warning);
        }
    };
    
    ValidationResult validate(const Config& config) const;
    
    ValidationResult validateParameter(const std::string& key, 
                                       const ConfigValue& value) const;
    
    static ValidationResult validateRange(const std::string& key, 
                                          const ConfigValue& value,
                                          double min, double max);
    
    static ValidationResult validateEnum(const std::string& key,
                                         const ConfigValue& value,
                                         const std::vector<std::string>& validValues);
    
    static ValidationResult validateConnection(const std::string& key,
                                               const ConfigValue& value);
};

// Enhanced Config class with validation and error handling
class ValidatedConfig : public Config {
public:
    ValidatedConfig();
    ~ValidatedConfig() override;
    
    // Override to enable validation
    bool loadFromFile(const std::string& filepath) override;
    bool loadFromArgs(int argc, char** argv) override;
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime) override;
    
    // Validation interface
    ValidationResult validate() const;
    ValidationResult validate(const ConfigurationValidator& validator) const;
    
    // Error handling
    void setErrorHandler(std::function<void(const ConfigurationError&)> handler);
    void setWarningHandler(std::function<void(const std::string&)> handler);
    
    // Configuration templates
    static ValidatedConfig createDevelopmentConfig();
    static ValidatedConfig createResearchConfig();
    static ValidatedConfig createProductionConfig();
    
    // Configuration search and suggestions
    std::vector<std::string> findSimilarKeys(const std::string& key) const;
    std::string getKeyDescription(const std::string& key) const;
    
    // Safe configuration getters with validation
    template<typename T>
    T getOrValidated(const std::string& key, const T& defaultValue, 
                     double min = std::numeric_limits<double>::lowest(),
                     double max = std::numeric_limits<double>::max()) const;
    
    // Configuration comparison
    bool matches(const ValidatedConfig& other, 
                 const std::vector<std::string>& ignoreKeys = {}) const;
    
private:
    ValidationResult currentValidationResult;
    std::function<void(const ConfigurationError&)> errorHandler;
    std::function<void(const std::string&)> warningHandler;
    
    void reportError(const std::string& message, const std::string& key = "",
                    const std::string& value = "");
    void reportWarning(const std::string& message);
    
    // Default validation rules
    static std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> 
        validationRules;
    
    static void initializeValidationRules();
};