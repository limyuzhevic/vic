#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class Config;

// Configuration validation function type
using ValidationFunction = std::function<bool(const std::string&, const ConfigValue&)>;

// Configuration schema
struct ConfigSchema {
    std::string key;
    std::string type;  // "int", "float", "bool", "string", "list"
    std::string description;
    std::optional<ConfigValue> defaultValue;
    ValidationFunction validator;
    bool required = false;
    
    ConfigSchema() = default;
    ConfigSchema(const std::string& k, const std::string& t, const std::string& desc = "",
                const std::optional<ConfigValue>& def = std::nullopt, bool req = false)
        : key(k), type(t), description(desc), defaultValue(def), required(req) {}
};

// Configuration value types (already defined in Config.hpp)
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

// Configuration source enum
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string filePath;  // Track which file loaded this value
    
    ConfigEntry() : source(ConfigSource::Default) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& file = "")
        : key(k), value(v), source(s), description(desc), filePath(file) {}
};

// Enhanced configuration class with JSON/YAML support and validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from different formats
    bool loadFromFile(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromString(const std::string& content, const std::string& format = "auto");
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnv(const std::map<std::string, std::string>& envVars);
    
    // Save to different formats
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    std::string saveToString(const std::string& format = "json") const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Validation and schema
    bool validate() const;
    bool validate(const std::string& key) const;
    void addSchema(const ConfigSchema& schema);
    bool removeSchema(const std::string& key);
    void clearSchemas();
    bool hasSchema(const std::string& key) const;
    
    // Configuration keys and metadata
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    std::vector<std::string> getKeysByType(const std::string& type) const;
    
    ConfigSource getSource(const std::string& key) const;
    std::string getFilePath(const std::string& key) const;
    
    // Remove and reset
    void remove(const std::string& key);
    void clear();
    
    // Configuration metadata
    std::string summary() const;
    std::string toString(const std::string& format = "json") const;
    
    // Statistics and validation
    size_t getEntryCount() const;
    size_t getSchemaCount() const;
    bool isValid(const std::string& key) const;
    std::string getValidationError(const std::string& key) const;
    
    // Schema helpers
    void setDefault(const std::string& key, const ConfigValue& defaultValue);
    void setRequired(const std::string& key, bool required);
    void setValidator(const std::string& key, const ValidationFunction& validator);
    
    // Configuration types
    static std::string typeToString(const ConfigValue& value);
    static std::string detectType(const ConfigValue& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool validateValueType(const std::string& expectedType, const ConfigValue& value);
    static ConfigValue parseJSONValue(const nlohmann::json& jsonValue);
    static nlohmann::json toJSON(const ConfigValue& value);
    
    // JSON/YAML parsing helpers
    bool loadFromJSONInternal(const nlohmann::json& jsonData, ConfigSource source, const std::string& filepath = "");
    nlohmann::json toJSONInternal() const;
};

} // namespace nlm

#endif // CONFIG_HPP