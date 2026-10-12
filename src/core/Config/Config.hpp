#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <filesystem>
#include <map>

// pybind11 forward declaration
namespace pybind11 {
    template<typename T> class arg_t;
    template<typename... Args> class class_;
    template<typename T> class module_attr_t;
    class module_;
    
    template<typename... Args>
    class class_;
    
    template<typename... Args>
    class module_;

    template<typename T>
    class arg_t {
    public:
        arg_t() = default;
        arg_t(const T&) {}
    };

    template<typename T>
    T arg(const T& value) {
        return value;
    }

    template<typename T>
    T arg(const std::string&) {
        return T{};
    }

    template<typename T, typename = void>
    struct type_caster;

    template<typename T>
    struct type_caster<T, std::void_t<decltype(std::declval<T>().value)>> {
        using type = T;
        template<typename Getter>
        static void load(Getter&& getter, type* t) {
            if (getter().has_value()) {
                *t = getter().value();
            }
        }
    };
}

// Forward declarations for external libraries
#ifdef HAS_NLOHMANN_JSON
#include "nlohmann/json.hpp"
#endif

#ifdef HAS_YAML_CPP
#include "yaml-cpp/yaml.h"
#endif

namespace nlm {

// Forward declarations
class Config;
class NLMRuntimeError;
class NLMValidationError;
class NLMConfigurationError;
class NLMSerializationError;
class SimpleWorld;
class AgentBrain;
class Brain;

// Forward declarations for pybind11
namespace py = pybind11;

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
    Runtime
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

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Helper functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Helper function to convert YAML node to ConfigValue
    static ConfigValue yamlToConfigValue(const YAML::Node& node);
    
    // Helper function to convert ConfigValue to YAML node
    static YAML::Node configValueToYAML(const ConfigValue& value);
    
    // Internal helper methods (not exposed to Python)
    bool validate() const;
    bool validateKeyValue(const std::string& key, const ConfigValue& value) const;
    bool validateRequiredKeys() const;
    bool validateKeyDependencies() const;
    void expandEnvironmentVariables(ConfigValue& value);
    void applyPreset(const std::string& presetName);
    bool loadFromSimpleFile(const std::string& filepath);
    bool loadFromJSONFile(const std::string& filepath);
    bool loadFromYAMLFile(const std::string& filepath);
    bool saveToJSONFile(const std::string& filepath) const;
    bool saveToYAMLFile(const std::string& filepath) const;
    bool loadFromArgsEnhanced(int argc, char** argv);
    std::string getFormatExtension() const;
    
    // Factory methods for Python integration
    static void exportToPython(py::module& m);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Python-specific factory functions
inline std::shared_ptr<Config> createConfig() {
    return std::make_shared<Config>();
}

inline std::shared_ptr<Config> createConfigFromPreset(const std::string& presetName) {
    auto config = std::make_shared<Config>();
    config->applyPreset(presetName);
    return config;
}

inline bool validateConfig(std::shared_ptr<Config> config) {
    return config->validate();
}

} // namespace nlm
