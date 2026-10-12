#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <chrono>

namespace nlm {

// Forward declarations
class Config;

// Plugin-related configuration
struct PluginConfig {
    std::string name;
    std::string version;
    std::string configFile;
    std::string parameters;
    bool enabled;
    std::vector<std::string> dependencies;
    
    PluginConfig() : enabled(true) {}
    PluginConfig(const std::string& n) : name(n), enabled(true) {}
};

// Main configuration class with plugin support
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
    
    // Enhanced CLI-specific options loading
    void loadAdvancedCLIOptions(int argc, char** argv);
    
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
    
    // Plugin-specific getters
    std::vector<PluginConfig> getPluginConfigs() const;
    PluginConfig getPluginConfig(const std::string& pluginName) const;
    bool hasPlugin(const std::string& pluginName) const;
    
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
    
    // CLI-specific getters
    bool getBool(const std::string& key, bool defaultValue) const;
    std::string getString(const std::string& key, const std::string& defaultValue) const;
    int getInt(const std::string& key, int defaultValue) const;
    double getDouble(const std::string& key, double defaultValue) const;
    LogLevel getLogLevel(const std::string& levelStr);
    
    // Plugin configuration helpers
    bool configurePlugin(const std::string& pluginName, const std::string& config);
    std::string getPluginConfig(const std::string& pluginName, const std::string& configKey) const;
    bool loadPluginConfig(const std::string& pluginName, const std::string& configFile = "");
    bool savePluginConfig(const std::string& pluginName, const std::string& configFile = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm