// Advanced Configuration System
// Support for JSON, YAML and other modern configuration formats

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <variant>
#include <stdexcept>

namespace nlm {

// Configuration value types
using ConfigValue = std::variant<
    std::monostate,      // null
    bool,                // boolean
    int64_t,             // integer
    double,              // floating point
    std::string,         // string
    std::vector<bool>,   // bool array
    std::vector<int64_t>, // integer array
    std::vector<double>, // float array
    std::vector<std::string>, // string array
    std::unordered_map<std::string, ConfigValue> // object
>;

// Configuration file format enum
enum class ConfigFormat {
    Auto,    // Auto-detect format
    Text,    // Original key=value format
    JSON,    // JSON format
    YAML,    // YAML format
    TOML     // TOML format
};

// Advanced configuration interface
class AdvancedConfig {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath, ConfigFormat format = ConfigFormat::Auto);
    
    // Load configuration from JSON string
    bool loadFromJSON(const std::string& jsonString);
    
    // Load configuration from YAML string
    bool loadFromYAML(const std::string& yamlString);
    
    // Load configuration from original text format
    bool loadFromText(const std::string& textString);
    
    // Get configuration value with type checking
    template<typename T>
    T getValue(const std::string& key) const {
        auto it = values.find(key);
        if (it == values.end()) {
            throw std::runtime_error("Configuration key not found: " + key);
        }
        
        if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(it->second)) {
                return std::get<std::string>(it->second);
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(it->second)) {
                return std::get<double>(it->second);
            } else if (std::holds_alternative<int64_t>(it->second)) {
                return static_cast<double>(std::get<int64_t>(it->second));
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<int64_t>(it->second)) {
                return std::get<int64_t>(it->second);
            } else if (std::holds_alternative<double>(it->second)) {
                return static_cast<int64_t>(std::get<double>(it->second));
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(it->second)) {
                return std::get<bool>(it->second);
            }
        }
        
        throw std::runtime_error("Configuration value type mismatch for key: " + key);
    }
    
    // Get optional value (returns default if not found)
    template<typename T>
    T getOptional(const std::string& key, const T& defaultValue) const {
        try {
            return getValue<T>(key);
        } catch (const std::runtime_error&) {
            return defaultValue;
        }
    }
    
    // Set configuration value
    template<typename T>
    void setValue(const std::string& key, const T& value) {
        values[key] = value;
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return values.find(key) != values.end();
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(values.size());
        for (const auto& pair : values) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // Export configuration to string
    std::string exportToJSON() const;
    std::string exportToYAML() const;
    std::string exportToText() const;
    
    // Merge with another configuration
    void merge(const AdvancedConfig& other);
    
    // Validate configuration
    std::vector<std::string> validate() const;
    
    // Get configuration summary
    std::string getSummary() const;
    
    // Configuration schema validation
    struct ConfigSchema {
        std::string name;
        ConfigValue defaultValue;
        std::string description;
        bool required = false;
        
        bool validate(const ConfigValue& value) const {
            // Basic type validation
            if (required && value.index() == 0) { // null
                return false;
            }
            return true;
        }
    };
    
    // Set configuration schema
    void setSchema(const std::vector<ConfigSchema>& schema);
    
    // Validate against schema
    std::vector<std::string> validateSchema() const;
    
    // Configuration command interface for power users
    class CommandInterface {
    public:
        virtual ~CommandInterface() = default;
        
        // Run configuration command
        virtual bool runCommand(const std::vector<std::string>& args) = 0;
        
        // Get command help
        virtual std::string getHelp() const = 0;
        
        // Get command name
        virtual std::string getName() const = 0;
    };
    
    // Configuration commands for power users
    class ConfigCommandLoader : public CommandInterface {
    public:
        bool runCommand(const std::vector<std::string>& args) override;
        std::string getHelp() const override;
        std::string getName() const override { return "load"; }
    };
    
    class ConfigCommandSave : public CommandInterface {
    public:
        bool runCommand(const std::vector<std::string>& args) override;
        std::string getHelp() const override;
        std::string getName() const override { return "save"; }
    };
    
    class ConfigCommandExport : public CommandInterface {
    public:
        bool runCommand(const std::vector<std::string>& args) override;
        std::string getHelp() const override;
        std::string getName() const override { return "export"; }
    };
    
    class ConfigCommandValidate : public CommandInterface {
    public:
        bool runCommand(const std::vector<std::string>& args) override;
        std::string getHelp() const override;
        std::string getName() const override { return "validate"; }
    };
    
    class ConfigCommandMerge : public CommandInterface {
    public:
        bool runCommand(const std::vector<std::string>& args) override;
        std::string getHelp() const override;
        std::string getName() const override { return "merge"; }
    };
    
    class ConfigCommandInfo : public CommandInterface {
    public:
        bool runCommand(const std::vector<std::string>& args) override;
        std::string getHelp() const override;
        std::string getName() const override { return "info"; }
    };
    
    // Execute configuration command
    bool executeCommand(const std::string& commandName, const std::vector<std::string>& args);
    
    // List available commands
    std::vector<std::string> listCommands() const;
    
    // Get command help
    std::string getCommandHelp(const std::string& commandName) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Factory function for creating advanced configuration
inline std::unique_ptr<AdvancedConfig> createAdvancedConfig() {
    return std::make_unique<AdvancedConfig>();
}

} // namespace nlm
