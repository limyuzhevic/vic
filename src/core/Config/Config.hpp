#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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
    Runtime,
    Environment
};

// Configuration diff
struct ConfigDiff {
    struct ValueDiff {
        std::string key;
        ConfigValue oldValue;
        ConfigValue newValue;
        
        ValueDiff() : key(), oldValue(), newValue() {}
        ValueDiff(const std::string& k, const ConfigValue& old, const ConfigValue& newVal)
            : key(k), oldValue(old), newValue(newVal) {}
    };
    
    std::vector<ValueDiff> differences;
    std::vector<std::string> onlyInFirst;
    std::vector<std::string> onlyInSecond;
    
    ConfigDiff() : differences(), onlyInFirst(), onlyInSecond() {}
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
    
    // Set default agent brain sensory dimensions
    void setVisionDimensions(size_t width, size_t height);
    void setTouchCount(size_t count);
    void setInternalSignalCount(size_t count);
    void setProprioceptionCount(size_t count);
    void setMotorActionCount(size_t count);
    
    // Get agent brain sensory dimensions
    size_t getVisionWidth() const;
    size_t getVisionHeight() const;
    size_t getTouchCount() const;
    size_t getInternalSignalCount() const;
    size_t getProprioceptionCount() const;
    size_t getMotorActionCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
