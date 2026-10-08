// Config Manager - Manages configuration parsing and serialization
class ConfigManager {
public:
    ConfigManager() = default;
    ~ConfigManager() = default;
    
    // Add a parser to the manager
    void addParser(std::unique_ptr<ConfigParser> parser);
    
    // Add a serializer to the manager
    void addSerializer(std::unique_ptr<ConfigSerializer> serializer);
    
    // Parse content using the appropriate parser
    std::unordered_map<std::string, std::string> parse(const std::string& filename, const std::string& content);
    
    // Serialize entries using the appropriate serializer
    std::string serialize(const std::vector<ConfigEntry>& entries, const std::string& filename);
    
    // Get available parser names
    std::vector<std::string> getAvailableParsers() const;
    
    // Get available serializer names
    std::vector<std::string> getAvailableSerializers() const;
    
private:
    std::vector<std::unique_ptr<ConfigParser>> parsers_;
    std::vector<std::unique_ptr<ConfigSerializer>> serializers_;
};

// Config implementation with improved modularity
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (supports JSON, YAML, and simple key=value formats)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with better error handling
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with type safety
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, std::vector<int> value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, std::vector<double> value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, std::vector<std::string> value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence and type
    bool has(const std::string& key) const;
    
    // Get the type of a config value
    std::string getValueType(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with more details
    std::string summary() const;
    
    // Export config to string for debugging
    std::string exportToString() const;
    
    // Import config from string
    bool importFromString(const std::string& content);
    
private:
    // Internal implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Config manager for parsing/serialization
    static ConfigManager& getConfigManager();
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isComment(const std::string& line);
    static std::pair<std::string, std::string> parseKeyValue(const std::string& line);
    
    // Validate value type
    static bool isValidType(const ConfigValue& value, const std::type_info& typeInfo) const;
    
    // Convert string to appropriate type
    template<typename T>
    static T convertStringToType(const std::string& value);
    
    // Convert value to string representation
    static std::string convertValueToString(const ConfigValue& value);
};

// Implementation details for ConfigParser subclasses
namespace parsers {
    std::unordered_map<std::string, std::string> JsonConfigParser::parse(const std::string& content) {
        // TODO PHASE 3: Implement proper JSON parsing
        // For now, return empty map
        return {};
    }
    
    std::unordered_map<std::string, std::string> YamlConfigParser::parse(const std::string& content) {
        // TODO PHASE 3: Implement proper YAML parsing
        // For now, return empty map
        return {};
    }
    
    std::unordered_map<std::string, std::string> KeyValueParser::parse(const std::string& content) {
        std::unordered_map<std::string, std::string> result;
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || isComment(line)) {
                continue;
            }
            
            auto [key, value] = parseKeyValue(line);
            if (!key.empty()) {
                result[key] = value;
            }
        }
        
        return result;
    }
}

// Implementation details for ConfigSerializer subclasses
namespace serializers {
    std::string JsonConfigSerializer::serialize(const std::vector<ConfigEntry>& entries) {
        // TODO PHASE 3: Implement proper JSON serialization
        // For now, return a placeholder JSON
        std::ostringstream oss;
        oss << "{\n";
        for (size_t i = 0; i < entries.size(); ++i) {
            const auto& entry = entries[i];
            oss << "  \"" << entry.key << "\": " << convertValueToString(entry.value);
            if (i < entries.size() - 1) oss << ",\n";
        }
        oss << "\n}";
        return oss.str();
    }
    
    std::string YamlConfigSerializer::serialize(const std::vector<ConfigEntry>& entries) {
        // TODO PHASE 3: Implement proper YAML serialization
        // For now, return a placeholder YAML
        std::ostringstream oss;
        for (const auto& entry : entries) {
            oss << "# " << entry.description << "\n";
            oss << entry.key << " = " << convertValueToString(entry.value) << "\n\n";
        }
        return oss.str();
    }
    
    std::string KeyValueSerializer::serialize(const std::vector<ConfigEntry>& entries) {
        std::ostringstream oss;
        for (const auto& entry : entries) {
            oss << "# " << entry.description << "\n";
            oss << entry.key << " = " << convertValueToString(entry.value) << "\n\n";
        }
        return oss.str();
    }
}