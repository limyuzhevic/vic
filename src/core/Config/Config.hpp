#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>

namespace nlm {

using json = nlohmann::json;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration entry with structured path
struct ConfigEntry {
    std::vector<std::string> path;  // e.g., {"brain", "neuron_count"}
    json value;                    // JSON value for the configuration
    ConfigSource source;            // Where this config came from
    std::string description;        // Human-readable description
    
    ConfigEntry() : source(ConfigSource::Default) {}
    ConfigEntry(const std::vector<std::string>& p, const json& v, ConfigSource s, const std::string& desc = "")
        : path(p), value(v), source(s), description(desc) {}
};

// Main configuration class with JSON support
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from JSON file
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromString(const std::string& jsonString);
    
    // Load from command line arguments (supports --section.key=value)
    bool loadFromArgs(int argc, char** argv);
    
    // Save to JSON file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with template support for basic types
    template<typename T>
    std::optional<T> get(const std::vector<std::string>& path) const;
    
    template<typename T>
    T getOr(const std::vector<std::string>& path, const T& defaultValue) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        // Legacy support for dot-separated keys
        std::vector<std::string> path;
        size_t pos = 0;
        std::string token;
        std::string remaining = key;
        while ((pos = remaining.find('.')) != std::string::npos) {
            token = remaining.substr(0, pos);
            path.push_back(token);
            remaining.erase(0, pos + 1);
        }
        path.push_back(remaining);
        return getOr<T>(path, defaultValue);
    }
    
    // Set values with structured path
    template<typename T>
    void set(const std::vector<std::string>& path, const T& value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    
    // Convenience methods for legacy compatibility
    void set(const std::string& key, const json& value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    
    // Check existence
    bool has(const std::vector<std::string>& path) const;
    bool has(const std::string& key) const {
        std::vector<std::string> path;
        size_t pos = 0;
        std::string token;
        std::string remaining = key;
        while ((pos = remaining.find('.')) != std::string::npos) {
            token = remaining.substr(0, pos);
            path.push_back(token);
            remaining.erase(0, pos + 1);
        }
        path.push_back(remaining);
        return has(path);
    }
    
    // Remove key
    void remove(const std::vector<std::string>& path);
    void remove(const std::string& key) {
        std::vector<std::string> path;
        size_t pos = 0;
        std::string token;
        std::string remaining = key;
        while ((pos = remaining.find('.')) != std::string::npos) {
            token = remaining.substr(0, pos);
            path.push_back(token);
            remaining.erase(0, pos + 1);
        }
        path.push_back(remaining);
        remove(path);
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get the underlying JSON for advanced access
    json& getRoot() { return root_; }
    const json& getRoot() const { return root_; }
    
    // Merge another configuration
    void merge(const Config& other);
    
private:
    json root_;
    std::vector<ConfigEntry> entries_;
    
    // Internal helpers
    std::vector<std::string> parseKey(const std::string& key) const;
    std::string joinPath(const std::vector<std::string>& path) const;
};

} // namespace nlm
