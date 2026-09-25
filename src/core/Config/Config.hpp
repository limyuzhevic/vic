#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>

namespace nlm {

// Forward declarations
class Component;
class PlasticityRule;
class Neuromodulator;
class MemorySystem;
class DevelopmentSystem;

// Component type enumeration for configuration
enum class ComponentType {
    Unknown,
    PlasticityRule,
    Neuromodulator,
    MemorySystem,
    DevelopmentSystem,
    Brain,
    SpikeSystem,
    NeuralRegion,
    Neuron,
    Synapse
};

// Component configuration for plugin-based architecture
struct ComponentConfig {
    std::string name;
    ComponentType type;
    std::string implementation;  // Plugin name or class name
    std::unordered_map<std::string, std::string> parameters;
    bool enabled;
    std::string pluginName;
    int priority;
    
    ComponentConfig()
        : name(), type(ComponentType::Unknown), implementation(), parameters(), enabled(true), pluginName(), priority(0) {}
    
    ComponentConfig(const std::string& n, ComponentType t, const std::string& impl)
        : name(n), type(t), implementation(impl), enabled(true), pluginName(), priority(0) {}
};

// Configuration value types for component configuration
using ComponentConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::unordered_map<std::string, std::string>
>;

// Configuration entry for component configuration
struct ComponentEntry {
    std::string key;
    ComponentConfigValue value;
    std::string description;
    std::vector<std::string> dependencies;
    
    ComponentEntry() : key(), value(), description() {}
    ComponentEntry(const std::string& k, const ComponentConfigValue& v, const std::string& desc = "")
        : key(k), value(v), description(desc) {}
};

// Main configuration class extended for component management
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
    bool load_from_file(const std::string& filepath);
    
    // Load from command line arguments
    bool load_from_args(int argc, char** argv);
    
    // Save to file
    bool save_to_file(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T get_or(const std::string& key, const T& default_value) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Component configuration methods
    bool registerComponent(const ComponentConfig& component);
    bool unregisterComponent(const std::string& name);
    bool hasComponent(const std::string& name) const;
    ComponentConfig getComponent(const std::string& name) const;
    std::vector<std::string> getComponentNames() const;
    std::vector<ComponentConfig> getComponentsOfType(ComponentType type) const;
    
    // Component parameter management
    void setComponentParameter(const std::string& componentName, 
                              const std::string& paramName, 
                              const ComponentConfigValue& value);
    ComponentConfigValue getComponentParameter(const std::string& componentName,
                                              const std::string& paramName) const;
    std::unordered_map<std::string, ComponentConfigValue> getComponentParameters(const std::string& componentName) const;
    
    // Plugin architecture
    bool loadPlugin(const std::string& pluginPath, const std::string& componentName = "");
    bool unloadPlugin(const std::string& componentName);
    bool hasPlugin(const std::string& componentName) const;
    std::vector<std::string> getAvailablePlugins() const;
    
    // Configuration validation
    bool validateComponent(const ComponentConfig& component) const;
    std::vector<std::string> validateAllComponents() const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> get_keys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    std::string componentSummary() const;
    
    // Component lifecycle
    bool enableComponent(const std::string& componentName);
    bool disableComponent(const std::string& componentName);
    bool isComponentEnabled(const std::string& componentName) const;
    
    // Runtime configuration updates
    bool updateComponentRuntime(const std::string& componentName, 
                               const std::unordered_map<std::string, ComponentConfigValue>& parameters);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string to_lower(const std::string& str);
    ComponentType stringToComponentType(const std::string& typeStr) const;
    std::string componentTypeToString(ComponentType type) const;
};

} // namespace nlm

