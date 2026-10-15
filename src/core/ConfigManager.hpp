// High-level configuration manager with validation
class ConfigManager {
public:
    ConfigManager() : config(std::make_shared<Config>()) {
        initializeValidator();
    }
    
    ConfigManager(const std::string& configFile) : config(std::make_shared<Config>()) {
        initializeValidator();
        loadFromFile(configFile);
    }
    
    // Load configuration from JSON file
    bool loadFromFile(const std::string& filepath) {
        if (config->loadFromFile(filepath)) {
            return validate();
        }
        return false;
    }
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv) {
        config->loadFromArgs(argc, argv);
        return validate();
    }
    
    // Apply default values
    void applyDefaults() {
        initializeDefaultValues();
        validate();
    }
    
    // Get configuration
    std::shared_ptr<Config> getConfig() const { return config; }
    
    // Configuration value access (convenience methods)
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T{}) const {
        return config->getOr<T>(key, defaultValue);
    }
    
    // Set value (convenience method)
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime) {
        config->set(key, value, source);
        validate();
    }
    
    // Check if configuration is valid
    bool isValid() const { return validationErrors.empty(); }
    
    // Get validation errors
    const std::vector<std::string>& getValidationErrors() const { return validationErrors; }
    
    // Get configuration summary
    std::string summary() const {
        std::string summary = config->summary();
        if (!validationErrors.empty()) {
            summary += "\nValidation Errors:";
            for (const auto& error : validationErrors) {
                summary += "\n  ERROR: " + error;
            }
        }
        return summary;
    }
    
    // Save configuration
    bool saveToFile(const std::string& filepath) const {
        return config->saveToFile(filepath);
    }
    
private:
    std::shared_ptr<Config> config;
    ConfigValidator validator;
    std::vector<std::string> validationErrors;
    
    void initializeValidator() {
        // Add standard validation rules for NLM configuration
        validator.addValidation(ConfigValidation::intRange("neuron_count", 1, 10000, 
                                      "Number of neurons must be between 1 and 10000", true));
        validator.addValidation(ConfigValidation::doubleRange("simulation_timestep", 0.0001, 0.1, 
                                      "Simulation timestep must be between 0.0001 and 0.1s", true));
        validator.addValidation(ConfigValidation::positive("region_count", 
                                      "Region count must be positive", true));
        validator.addValidation(ConfigValidation::doubleRange("connection_probability", 0.0, 1.0, 
                                      "Connection probability must be between 0.0 and 1.0", true));
        
        // STDP parameters
        validator.addValidation(ConfigValidation::doubleRange("stdp_ltp_weight", 0.0, 1.0, 
                                      "STDP LTP weight must be between 0.0 and 1.0", true));
        validator.addValidation(ConfigValidation::doubleRange("stdp_ltd_weight", 0.0, 1.0, 
                                      "STDP LTD weight must be between 0.0 and 1.0", true));
        validator.addValidation(ConfigValidation::doubleRange("stdp_tau", 1.0, 100.0, 
                                      "STDP time constant must be between 1.0 and 100.0", true));
        
        // Development parameters
        validator.addValidation(ConfigValidation::doubleRange("synaptogenesis_rate", 0.0, 1.0, 
                                      "Synaptogenesis rate must be between 0.0 and 1.0", true));
        validator.addValidation(ConfigValidation::doubleRange("pruning_rate", 0.0, 1.0, 
                                      "Pruning rate must be between 0.0 and 1.0", true));
        
        // Logging parameters
        std::vector<std::string> logLevels = {"OFF", "ERROR", "WARNING", "INFO", "DEBUG"};
        validator.addValidation(ConfigValidation::stringList("log_level", logLevels, 
                                      "Log level must be one of: OFF, ERROR, WARNING, INFO, DEBUG", true));
    }
    
    void initializeDefaultValues() {
        // Apply sensible defaults if values are missing
        if (!config->has("random_seed")) {
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        }
        if (!config->has("max_simulation_steps")) {
            config->set("max_simulation_steps", 10000, ConfigSource::Default);
        }
        if (!config->has("log_level")) {
            config->set("log_level", "INFO", ConfigSource::Default);
        }
    }
    
    bool validate() {
        validationErrors = validator.validate(*config);
        return validationErrors.empty();
    }
};