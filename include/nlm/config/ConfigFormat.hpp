#ifndef NLM_CONFIG_FORMAT_HPP
#define NLM_CONFIG_FORMAT_HPP

#include <string>
#include <map>
#include <vector>
#include <variant>
#include <memory>
#include <functional>

namespace nlm {
namespace config {

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    JSON,
    YAML
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// JSON/YAML parser interface
class ConfigParser {
public:
    virtual ~ConfigParser() = default;
    
    virtual bool canParse(const std::string& content) const = 0;
    virtual std::vector<ConfigEntry> parse(const std::string& content) = 0;
    virtual std::string getFormatName() const = 0;
};

// JSON parser implementation
class JSONParser : public ConfigParser {
public:
    bool canParse(const std::string& content) const override;
    std::vector<ConfigEntry> parse(const std::string& content) override;
    std::string getFormatName() const override { return "JSON"; }
};

// YAML parser implementation
class YAMLParser : public ConfigParser {
public:
    bool canParse(const std::string& content) const override;
    std::vector<ConfigEntry> parse(const std::string& content) override;
    std::string getFormatName() const override { return "YAML"; }
};

// Configuration manager with JSON/YAML support
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    // Parser registration
    void registerParser(std::unique_ptr<ConfigParser> parser);
    
    // Configuration loading
    bool loadFromFile(const std::string& filepath);
    bool loadFromString(const std::string& content, const std::string& format = "auto");
    
    // Configuration saving
    bool saveToFile(const std::string& filepath) const;
    bool saveToString(std::string& output) const;
    
    // Configuration format detection
    std::string detectFormat(const std::string& content) const;
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration access
    const std::vector<ConfigEntry>& getEntries() const { return entries_; }
    ConfigEntry* findEntry(const std::string& key);
    const ConfigEntry* findEntry(const std::string& key) const;
    
    // Configuration manipulation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool has(const std::string& key) const;
    
    // Configuration statistics
    size_t getEntryCount() const { return entries_.size(); }
    std::vector<std::string> getKeys() const;
    std::string getFormatSummary() const;
    
private:
    std::vector<std::unique_ptr<ConfigParser>> parsers_;
    std::vector<ConfigEntry> entries_;
    std::vector<std::string> validationErrors_;
    
    // Helper functions
    static std::string trim(const std::string& str);
    static bool isWhitespace(char c);
    static bool isDigit(char c);
    static bool isAlpha(char c);
    static bool isAlphanumeric(char c);
    static std::string normalizeKey(const std::string& key);
};

// Configuration schema validation
class ConfigSchema {
public:
    struct Field {
        std::string name;
        std::string type; // "int", "double", "string", "bool", "vector"
        bool required;
        ConfigValue defaultValue;
        std::string description;
        std::vector<std::string> allowedValues;
        
        Field() : name(), type("string"), required(false), defaultValue(), description(), allowedValues() {}
    };
    
    ConfigSchema() = default;
    ~ConfigSchema() = default;
    
    void addField(const Field& field);
    bool validate(const ConfigManager& config) const;
    std::vector<std::string> getValidationErrors(const ConfigManager& config) const;
    std::string getSchemaDescription() const;
    
private:
    std::vector<Field> fields_;
    mutable std::vector<std::string> errors_;
};

// Configuration profile system
class ConfigProfile {
public:
    struct Profile {
        std::string name;
        std::string description;
        std::vector<ConfigEntry> entries;
        std::string parentProfile; // Inherit from another profile
        
        Profile() : name(), description(), parentProfile() {}
    };
    
    ConfigProfile();
    ~ConfigProfile() = default;
    
    // Profile management
    void addProfile(const Profile& profile);
    bool loadProfilesFromFile(const std::string& filepath);
    bool saveProfilesToFile(const std::string& filepath) const;
    
    // Profile operations
    Profile* getProfile(const std::string& name);
    const Profile* getProfile(const std::string& name) const;
    bool hasProfile(const std::string& name) const;
    std::vector<std::string> getProfileNames() const;
    
    // Profile merging and inheritance
    std::vector<ConfigEntry> resolveProfile(const std::string& profileName) const;
    
private:
    std::map<std::string, Profile> profiles_;
};

// Configuration template system
class ConfigTemplate {
public:
    struct Template {
        std::string name;
        std::string description;
        std::vector<ConfigEntry> entries;
        std::string category;
        
        Template() : name(), description(), category() {}
    };
    
    ConfigTemplate();
    ~ConfigTemplate() = default;
    
    // Template management
    void addTemplate(const Template& template);
    bool loadTemplatesFromFile(const std::string& filepath);
    bool saveTemplatesToFile(const std::string& filepath) const;
    
    // Template operations
    Template* getTemplate(const std::string& name);
    const Template* getTemplate(const std::string& name) const;
    bool hasTemplate(const std::string& name) const;
    std::vector<std::string> getTemplateNames() const;
    std::vector<std::string> getTemplateCategories() const;
    
private:
    std::map<std::string, Template> templates_;
    std::map<std::string, std::vector<std::string>> categoryMap_;
};

// Factory functions
inline std::unique_ptr<ConfigParser> createJSONParser() {
    return std::make_unique<JSONParser>();
}

inline std::unique_ptr<ConfigParser> createYAMLParser() {
    return std::make_unique<YAMLParser>();
}

// Default schema definitions
namespace schemas {
    ConfigSchema getDefaultSchema();
    ConfigSchema getPlasticitySchema();
    ConfigSchema getPerformanceSchema();
    ConfigSchema getDevelopmentSchema();
    ConfigSchema getLearningSchema();
}

} // namespace config
} // namespace nlm

#endif // NLM_CONFIG_FORMAT_HPP