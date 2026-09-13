// Configuration registry for config presets
class ConfigRegistry {
public:
    static ConfigRegistry& getInstance() {
        static ConfigRegistry instance;
        return instance;
    }
    
    void registerConfig(const std::string& name, const std::function<void(Config&)>& builder) {
        ConfigFactory::getInstance().registerConfig(name, builder);
    }
    
    std::unique_ptr<Config> create(const std::string& name) {
        return ConfigFactory::getInstance().createConfig(name);
    }
    
    bool has(const std::string& name) const {
        return ConfigFactory::getInstance().hasConfig(name);
    }
    
    std::vector<std::string> getNames() const {
        return ConfigFactory::getInstance().getConfigNames();
    }
    
private:
    ConfigRegistry() = default;
};

// Factory function for config creation
inline std::unique_ptr<Config> makeConfig(const std::string& name) {
    return Config::create(name);
}

// Convenience config loaders
inline bool loadConfigFromFile(Config& config, const std::string& filepath) {
    return config.loadFromFile(filepath);
}

inline bool loadConfigFromArgs(Config& config, int argc, char** argv) {
    return config.loadFromArgs(argc, argv);
}

inline bool loadConfigFromEnvironment(Config& config, const std::string& prefix = "") {
    return config.loadFromEnvironment(prefix);
}

inline bool loadConfigFromAll(Config& config, const std::string& filepath, int argc, char** argv, const std::string& envPrefix = "") {
    bool success = true;
    success &= loadConfigFromFile(config, filepath);
    success &= loadConfigFromArgs(config, argc, argv);
    success &= loadConfigFromEnvironment(config, envPrefix);
    return success;
}

// Config utilities
inline std::string getConfigValueAsString(const Config& config, const std::string& key, const std::string& defaultValue = "") {
    auto val = config.get<std::string>(key);
    return val.has_value() ? val.value() : defaultValue;
}

inline int getConfigValueAsInt(const Config& config, const std::string& key, int defaultValue = 0) {
    auto val = config.get<int>(key);
    return val.has_value() ? val.value() : defaultValue;
}

inline double getConfigValueAsDouble(const Config& config, const std::string& key, double defaultValue = 0.0) {
    auto val = config.get<double>(key);
    return val.has_value() ? val.value() : defaultValue;
}

inline bool getConfigValueAsBool(const Config& config, const std::string& key, bool defaultValue = false) {
    auto val = config.get<bool>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Config validation utilities
inline bool isConfigKeyValid(const Config& config, const std::string& key) {
    return config.isValid(key);
}

inline bool hasConfigKey(const Config& config, const std::string& key) {
    return config.has(key);
}

// Config serialization/deserialization helpers
inline std::string serializeConfig(const Config& config) {
    return config.summary();
}

inline bool deserializeConfig(Config& config, const std::string& data) {
    // Basic implementation for simple config data
    // In a real implementation, this would parse the string representation
    return true;
}

// Helper to check if a string starts with prefix
inline bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

} // namespace nlm