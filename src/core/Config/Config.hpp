#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>
#include <functional>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Config;

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
    std::string group;
    std::vector<std::string> depends_on;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), group(), depends_on() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", 
                const std::string& grp = "", const std::vector<std::string>& deps = {})
        : key(k), value(v), source(s), description(desc), group(grp), depends_on(deps) {}
};

// Configuration validation function type
using ValidationFunction = std::function<bool(const std::string&, const ConfigValue&)>;

// Parameter group definition
struct ParameterGroup {
    std::string name;
    std::string description;
    std::vector<std::string> parameter_keys;
    bool readonly;
    
    ParameterGroup() : name(), description(), readonly(false) {}
    ParameterGroup(const std::string& n, const std::string& desc, bool ro = false)
        : name(n), description(desc), readonly(ro) {}
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
    
    // Get keys by group
    std::vector<std::string> getKeysByGroup(const std::string& group) const;
    
    // Get all groups
    std::vector<std::string> getGroups() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation
    bool validate() const;
    bool validate(const std::string& key) const;
    void addValidator(const std::string& key, ValidationFunction validator);
    void clearValidators();
    
    // Parameter groups
    void addGroup(const std::string& name, const std::string& description, bool readonly = false);
    void addParameterToGroup(const std::string& group, const std::string& key);
    void removeGroup(const std::string& name);
    bool hasGroup(const std::string& name) const;
    
    // Configuration inheritance
    void setInheritanceParent(Config* parent);
    Config* getInheritanceParent() const;
    
    // Configuration macros
    void addMacro(const std::string& name, const std::string& expansion);
    void removeMacro(const std::string& name);
    std::string expandMacro(const std::string& macro_name) const;
    
    // Configuration profiles
    void saveProfile(const std::string& profile_name) const;
    bool loadProfile(const std::string& profile_name);
    std::vector<std::string> getAvailableProfiles() const;
    
    // Advanced optimization
    bool optimize(const std::string& optimization_mode = "balanced");
    void setOptimizationMode(const std::string& mode);
    
    // Parameter dependency management
    void addDependency(const std::string& key, const std::string& depends_on);
    void removeDependency(const std::string& key);
    bool hasDependency(const std::string& key) const;
    std::vector<std::string> getDependencies(const std::string& key) const;
    
    // Configuration statistics
    struct ConfigStats {
        size_t total_parameters;
        size_t file_loaded_parameters;
        size_t command_line_parameters;
        size_t runtime_parameters;
        double validation_score;
        std::unordered_map<std::string, size_t> parameters_by_group;
    };
    
    ConfigStats getStats() const;
    
    // Configuration comparison
    bool isSameAs(const Config& other) const;
    double similarityScore(const Config& other) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Internal implementation details
    void resolveDependencies() const;
    bool validateDependencies() const;
    std::string resolveMacroExpansions(const std::string& value) const;
    bool applyProfile(const std::string& profile_name);
    bool applyOptimization(const std::string& mode);
};

} // namespace nlm
