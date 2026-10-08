// Config Parser - Handles loading configuration from files
class ConfigParser {
public:
    virtual ~ConfigParser() = default;
    
    // Parse file content and return key-value pairs
    virtual std::unordered_map<std::string, std::string> parse(const std::string& content) = 0;
    
    // Check if this parser can handle the given file extension
    virtual bool canParse(const std::string& filename) const = 0;
    
    virtual const char* getName() const = 0;
};

// JSON Parser - For JSON configuration files
class JsonConfigParser : public ConfigParser {
public:
    std::unordered_map<std::string, std::string> parse(const std::string& content) override;
    bool canParse(const std::string& filename) const override { 
        return filename.size() > 5 && filename.substr(filename.size() - 5) == ".json"; 
    }
    const char* getName() const override { return "JSON Parser"; }
};

// YAML Parser - For YAML configuration files  
class YamlConfigParser : public ConfigParser {
public:
    std::unordered_map<std::string, std::string> parse(const std::string& content) override;
    bool canParse(const std::string& filename) const override {
        return filename.size() > 5 && filename.substr(filename.size() - 5) == ".yaml"; 
    }
    const char* getName() const override { return "YAML Parser"; }
};

// Key-Value Parser - For simple key=value files
class KeyValueParser : public ConfigParser {
public:
    std::unordered_map<std::string, std::string> parse(const std::string& content) override;
    bool canParse(const std::string& filename) const override {
        return filename.size() > 4 && filename.substr(filename.size() - 4) == ".cfg"; 
    }
    const char* getName() const override { return "Key-Value Parser"; }
};

// Config Serializer - Handles saving configuration to files
class ConfigSerializer {
public:
    virtual ~ConfigSerializer() = default;
    
    // Serialize config entries to file content
    virtual std::string serialize(const std::vector<ConfigEntry>& entries) = 0;
    
    // Check if this serializer can handle the given file extension
    virtual bool canSerialize(const std::string& filename) const = 0;
    
    virtual const char* getName() const = 0;
};

// JSON Serializer - For JSON configuration files
class JsonConfigSerializer : public ConfigSerializer {
public:
    std::string serialize(const std::vector<ConfigEntry>& entries) override;
    bool canSerialize(const std::string& filename) const override {
        return filename.size() > 5 && filename.substr(filename.size() - 5) == ".json"; 
    }
    const char* getName() const override { return "JSON Serializer"; }
};

// YAML Serializer - For YAML configuration files
class YamlConfigSerializer : public ConfigSerializer {
public:
    std::string serialize(const std::vector<ConfigEntry>& entries) override;
    bool canSerialize(const std::string& filename) const override {
        return filename.size() > 5 && filename.substr(filename.size() - 5) == ".yaml"; 
    }
    const char* getName() const override { return "YAML Serializer"; }
};

// Key-Value Serializer - For simple key=value files
class KeyValueSerializer : public ConfigSerializer {
public:
    std::string serialize(const std::vector<ConfigEntry>& entries) override;
    bool canSerialize(const std::string& filename) const override {
        return filename.size() > 4 && filename.substr(filename.size() - 4) == ".cfg"; 
    }
    const char* getName() const override { return "Key-Value Serializer"; }
};