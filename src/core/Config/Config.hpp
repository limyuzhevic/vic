// Configuration schema definition for better validation and organization
struct ConfigSchema {
    enum class ValidationType {
        INT,
        FLOAT,
        BOOL,
        STRING,
        VECTOR_INT,
        VECTOR_FLOAT,
        VECTOR_STRING,
        RANGE,
        CHOICES,
        REGEX
    };
    
    struct ValidationRule {
        ValidationType type;
        std::string errorMessage;
        std::string min;  // For ranges and comparisons
        std::string max;  // For ranges
        std::string choices;  // For CHOICES type
        std::string pattern;  // For REGEX type
    };
    
    struct Parameter {
        std::string key;
        std::string description;
        std::string defaultValue;
        std::string type;  // "int", "float", "bool", "string"
        std::vector<ValidationRule> validations;
        std::string category;  // "core", "neural", "plasticity", etc.
        bool required;
    };
    
    static std::vector<Parameter> getDefaultSchema() {
        return {
            // Core simulation parameters
            {"neuron_count", "Number of neurons in the network", "500", "int", {}, "core", false},
            {"region_count", "Number of neural regions", "1", "int", {}, "core", false},
            {"simulation_timestep", "Simulation timestep in seconds", "0.001", "float", {}, "core", false},
            {"random_seed", "Random seed for reproducible simulations", "42", "int", {}, "core", false},
            {"connection_probability", "Probability of connections between neurons", "0.15", "float", {}, "neural", false},
            
            // Plasticity parameters
            {"stdp_ltp_weight", "STDP long-term potentiation weight", "0.02", "float", {}, "plasticity", false},
            {"stdp_ltd_weight", "STDP long-term depression weight", "0.015", "float", {}, "plasticity", false},
            {"stdp_tau", "STDP time constant", "20.0", "float", {}, "plasticity", false},
            {"synaptogenesis_rate", "Rate of new synapse formation", "0.0001", "float", {}, "development", false},
            {"pruning_rate", "Rate of synapse elimination", "0.00001", "float", {}, "development", false},
            
            // Memory parameters
            {"working_memory_size", "Size of working memory buffer", "100", "int", {}, "memory", false},
            
            // Sensory parameters
            {"vision_width", "Vision sensor width", "16", "int", {}, "sensory", false},
            {"vision_height", "Vision sensor height", "16", "int", {}, "sensory", false},
            
            // Debug and performance parameters
            {"enable_debug_logging", "Enable debug logging output", "false", "bool", {}, "debug", false},
            {"enable_performance_profiling", "Enable performance profiling", "false", "bool", {}, "performance", false},
            {"max_simulation_steps", "Maximum number of simulation steps", "1000", "int", {}, "performance", false},
            {"checkpoint_interval", "Interval for checkpoint saving", "100", "int", {}, "performance", false},
        };
    }
    
    // Enhanced validation function
    static bool validateParameter(const Parameter& param, const std::string& value, std::string& error) {
        // Basic type validation
        if (param.type == "int") {
            try {
                size_t pos;
                int val = std::stoi(value, &pos);
                if (pos != value.size()) {
                    error = "Value must be an integer for parameter: " + param.key;
                    return false;
                }
            } catch (const std::exception&) {
                error = "Invalid integer value for parameter: " + param.key;
                return false;
            }
        } else if (param.type == "float") {
            try {
                size_t pos;
                float val = std::stof(value, &pos);
                if (pos != value.size()) {
                    error = "Value must be a floating point number for parameter: " + param.key;
                    return false;
                }
            } catch (const std::exception&) {
                error = "Invalid float value for parameter: " + param.key;
                return false;
            }
        } else if (param.type == "bool") {
            if (value != "true" && value != "false" && value != "1" && value != "0") {
                error = "Value must be a boolean (true/false) for parameter: " + param.key;
                return false;
            }
        } else if (param.type == "string") {
            if (value.empty() && param.required) {
                error = "Value is required for parameter: " + param.key;
                return false;
            }
        }
        
        // Range validation if specified
        for (const auto& validation : param.validations) {
            if (validation.type == ValidationType::RANGE) {
                try {
                    double val = std::stod(value);
                    double min = std::stod(validation.min);
                    double max = std::stod(validation.max);
                    if (val < min || val > max) {
                        error = "Value for " + param.key + " must be between " + 
                                validation.min + " and " + validation.max;
                        return false;
                    }
                } catch (...) {
                    error = "Invalid range value for parameter: " + param.key;
                    return false;
                }
            } else if (validation.type == ValidationType::CHOICES) {
                if (!validation.choices.empty()) {
                    if (value.find(validation.choices) == std::string::npos) {
                        error = "Value for " + param.key + " must be one of: " + validation.choices;
                        return false;
                    }
                }
            }
        }
        
        return true;
    }
};

// Enhanced configuration manager with schema validation and environment variable support
class Config {
public:
    struct ConfigEntry {
        std::string key;
        std::string value;
        ConfigSource source;
        std::string description;
        
        ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
        ConfigEntry(const std::string& k, const std::string& v, ConfigSource s, const std::string& desc = "")
            : key(k), value(v), source(s), description(desc) {}
    };
    
    Config() {
        // Initialize with default schema
        initializeFromSchema(ConfigSchema::getDefaultSchema());
    }
    
    ~Config() = default;
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept = default;
    Config& operator=(Config&&) noexcept = default;
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables (optional)
    bool loadFromEnv(const std::string& prefix = "NLM_");
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type conversion and validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    bool set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Get parameter information
    std::optional<ConfigSchema::Parameter> getParameterInfo(const std::string& key) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by category
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    
    // Get configuration summary
    std::string summary() const;
    
    // Clear all
    void clear();
    
    // Remove key
    void remove(const std::string& key);
    
private:
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigSchema::Parameter> parameterInfo;
    
    void initializeFromSchema(const std::vector<ConfigSchema::Parameter>& schema);
    bool validateParameter(const std::string& key, const std::string& value, std::string& error) const;
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};