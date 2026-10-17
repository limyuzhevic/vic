#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <optional>
#include <stdexcept>
#include <functional>
#include <nlohmann/json.hpp>

namespace nlm {

// Configuration value types - enhanced with type safety
enum class ConfigType {
    Int,
    Int64,
    Double,
    Bool,
    String,
    IntArray,
    DoubleArray,
    StringArray,
    Vector  // Generic vector type for backward compatibility
};

// Configuration source enum
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Enhanced Configuration class with validation and better error handling
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from JSON file (recommended)
    bool loadFromFile(const std::string& filepath);
    
    // Load from config file (simple key=value format, for backward compatibility)
    bool loadFromSimpleFile(const std::string& filepath);
    
    // Load from command line arguments with enhanced support
    bool loadFromArgs(int argc, char** argv);
    
    // Save to JSON file
    bool saveToFile(const std::string& filepath) const;
    
    // Enhanced type-safe getters with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Enhanced set methods with validation and error handling
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Backward compatibility for variant-based access
    std::optional<std::string> getString(const std::string& key) const;
    std::optional<int64_t> getInt(const std::string& key) const;
    std::optional<double> getDouble(const std::string& key) const;
    std::optional<bool> getBool(const std::string& key) const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with validation info
    std::string summary() const;
    
    // Enhanced configuration validation with detailed error reporting
    bool validate(std::vector<std::string>& outErrors) const;
    
    // Load from JSON
    bool fromJson(const nlohmann::json& jsonData);
    
    // Get configuration value as JSON for saving
    nlohmann::json toJson() const;
    
    // Convenience methods for specific NLM configurations
    static Config createDefaultConfig();
    static Config createExperimentConfig(uint64_t maxSteps = 10000, 
                                         size_t neuronCount = 1000,
                                         size_t regionCount = 1);
    static Config createPerformanceConfig(size_t neuronCount = 10000,
                                          size_t regionCount = 4,
                                          size_t connectionProbability = 10);
    
    // Configuration parameter validation setup
    void addParameter(const std::string& key, ConfigType type, 
                     const std::string& defaultValue = "", 
                     const std::string& description = "",
                     std::function<bool(const std::string&, const std::string&)> validator = nullptr);
    
    // Parameter metadata access
    std::string getParameterDescription(const std::string& key) const;
    bool isParameterRequired(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Initialize with default parameters
    void initializeDefaults();
    
    // Enhanced validation with detailed error reporting
    bool validateKeyValue(const std::string& key, const std::string& value, std::string& error) const;
    
    // Type conversion utilities
    static std::string toString(int64_t value);
    static std::string toString(double value);
    static std::string toString(bool value);
    static std::string toString(const std::string& value);
    static std::string toString(const std::vector<int>& value);
    static std::string toString(const std::vector<double>& value);
    static std::string toString(const std::vector<std::string>& value);
    
    static bool fromString(const std::string& str, int64_t& outValue);
    static bool fromString(const std::string& str, double& outValue);
    static bool fromString(const std::string& str, bool& outValue);
    static bool fromString(const std::string& str, std::string& outValue);
    static bool fromString(const std::string& str, std::vector<int>& outValue);
    static bool fromString(const std::string& str, std::vector<double>& outValue);
    static bool fromString(const std::string& str, std::vector<std::string>& outValue);
    
    // Error handling
    void addError(const std::string& key, const std::string& error);
};

// Exception for configuration errors
class ConfigError : public std::runtime_error {
public:
    ConfigError(const std::string& message) : std::runtime_error(message) {}
};

} // namespace nlm