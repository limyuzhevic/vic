// Helper functions for common configuration patterns

namespace nlm {
    
    // Get a configuration value with type conversion
    template<typename T>
    std::optional<T> getConfig(const Config& config, const std::string& key) {
        return config.get<T>(key);
    }
    
    // Get a configuration value with default
    template<typename T>
    T getConfigOr(const Config& config, const std::string& key, const T& defaultValue) {
        return config.getOr<T>(key, defaultValue);
    }
    
    // Set a configuration value
    template<typename T>
    void setConfig(Config& config, const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime) {
        config.set(key, value, source);
    }
    
    // Check if a configuration value exists
    bool hasConfig(const Config& config, const std::string& key) {
        return config.has(key);
    }
    
    // Get all configuration keys
    std::vector<std::string> getConfigKeys(const Config& config) {
        return config.getKeys();
    }
    
    // Load configuration from JSON string
    bool loadJsonConfig(Config& config, const std::string& jsonString) {
        return config.loadFromJson(jsonString);
    }
    
    // Load configuration from YAML string
    bool loadYamlConfig(Config& config, const std::string& yamlString) {
        return config.loadFromYaml(yamlString);
    }
    
    // Load configuration from file (auto-detects format)
    bool loadConfigFile(Config& config, const std::string& filepath) {
        return config.loadFromFile(filepath);
    }
    
    // Save configuration to file (auto-detects format)
    bool saveConfigFile(const Config& config, const std::string& filepath) {
        return config.saveToFile(filepath);
    }
    
    // Get configuration summary
    std::string getConfigSummary(const Config& config) {
        return config.summary();
    }
    
    // Merge two configurations (right overwrites left)
    void mergeConfigs(Config& target, const Config& source) {
        for (const auto& key : source.getKeys()) {
            auto valueOpt = source.getConfigValue(key);
            if (valueOpt.has_value()) {
                // Try to set with the same source if possible
                target.set(key, valueOpt.value(), ConfigSource::Runtime);
            }
        }
    }
    
    // Get configuration value as string
    std::string configValueToString(const ConfigValue& value) {
        return ::configValueToString(value);
    }
    
    // Create a default configuration
    Config createDefaultConfig() {
        Config config;
        
        // Set default values based on DEFAULT_SCHEMA
        config.set("random_seed", 42, ConfigSource::Default);
        config.set("simulation_timestep", 0.001, ConfigSource::Default);
        config.set("neuron_count", 1000, ConfigSource::Default);
        config.set("region_count", 2, ConfigSource::Default);
        config.set("connection_probability", 0.1, ConfigSource::Default);
        config.set("plasticity_learning_rate", 0.01, ConfigSource::Default);
        config.set("stdp_ltp_weight", 0.01, ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.012, ConfigSource::Default);
        config.set("stdp_time_constant", 20.0, ConfigSource::Default);
        config.set("development_synaptogenesis_rate", 0.001, ConfigSource::Default);
        config.set("development_pruning_rate", 0.0001, ConfigSource::Default);
        config.set("dopamine_baseline", 0.0, ConfigSource::Default);
        config.set("reward_discount_factor", 0.99, ConfigSource::Default);
        config.set("environment_name", "GridWorld", ConfigSource::Default);
        config.set("environment_width", 10, ConfigSource::Default);
        config.set("environment_height", 10, ConfigSource::Default);
        config.set("log_level", std::string("INFO"), ConfigSource::Default);
        config.set("log_to_file", false, ConfigSource::Default);
        config.set("log_filename", std::string("nlm.log"), ConfigSource::Default);
        config.set("max_simulation_steps", 10000, ConfigSource::Default);
        config.set("simulation_time_limit", 0.0, ConfigSource::Default);
        config.set("visualization_enabled", false, ConfigSource::Default);
        config.set("visualization_update_rate", 30.0, ConfigSource::Default);
        
        return config;
    }
    
    // Validate configuration
    bool validateConfig(const Config& config) {
        return config.validate();
    }
    
    // Convert ConfigValue to JSON
    nlohmann::json configValueToJson(const ConfigValue& value) {
        return ::configValueToJson(value);
    }
    
    // Convert ConfigValue to YAML
    YAML::Node configValueToYaml(const ConfigValue& value) {
        return ::configValueToYaml(value);
    }
    
    // Generate JSON configuration from Config
    std::string generateJsonConfig(const Config& config) {
        nlohmann::json jsonData;
        for (const auto& entry : config.getKeys()) {
            auto valueOpt = config.getConfigValue(entry);
            if (valueOpt.has_value()) {
                jsonData[entry] = configValueToJson(valueOpt.value());
            }
        }
        return jsonData.dump(2);
    }
    
    // Generate YAML configuration from Config
    std::string generateYamlConfig(const Config& config) {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : config.getKeys()) {
            auto valueOpt = config.getConfigValue(entry);
            if (valueOpt.has_value()) {
                emitter << YAML::Key << entry;
                emitter << YAML::Value << configValueToYaml(valueOpt.value());
            }
        }
        
        emitter << YAML::EndMap;
        return std::string(emitter.GetLastModified().c_str());
    }
    
    // Export configuration to various formats
    enum class ConfigFormat {
        KEY_VALUE,
        JSON,
        YAML
    };
    
    std::string exportConfig(const Config& config, ConfigFormat format) {
        switch (format) {
            case ConfigFormat::KEY_VALUE:
                // For key=value format, we could return a string representation
                // For simplicity, we'll return JSON representation for now
                return generateJsonConfig(config);
            case ConfigFormat::JSON:
                return generateJsonConfig(config);
            case ConfigFormat::YAML:
                return generateYamlConfig(config);
            default:
                return "";
        }
    }

} // namespace nlm