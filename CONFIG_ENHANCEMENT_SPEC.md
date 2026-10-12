# NLM Configuration Enhancement - Technical Specification

## Document Overview

This document provides the technical specification for enhancing the NLM configuration system. It covers the current implementation, required improvements, and detailed implementation guidelines for all configuration-related enhancements.

## Current Configuration System

### Architecture
```mermaid
graph LR
    A[Config Class] --> B[ConfigEntry Struct]
    A --> C[ConfigSource Enum]
    B --> D[ConfigValue Variant]
    D --> E[int]
    D --> F[int64_t]
    D --> G[double]
    D --> H[bool]
    D --> I[string]
    D --> J[std::vector<int>]
    D --> K[std::vector<double>]
    D --> L[std::vector<string>]
    
    A --> M[loadFromFile()]
    A --> N[loadFromArgs()]
    A --> O[saveToFile()]
    A --> P[get<T>()]
    A --> Q[getOr<T>()]
    A --> R[set()]
    A --> S[has()]
    A --> T[remove()]
```

### Core Components

#### 1. Config Class (`src/core/Config/Config.hpp`)
```cpp
class Config {
public:
    // Construction and destruction
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Configuration loading
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    
    // Configuration saving
    bool saveToFile(const std::string& filepath) const;
    
    // Value access
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Value setting
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};
```

#### 2. Configuration Value Types
```cpp
// ConfigValue variant type definition
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
```

#### 3. Configuration Source Priority
```cpp
enum class ConfigSource {
    Default,      // Default configuration values
    File,         // Loaded from configuration files
    CommandLine,  // Set via command line arguments
    Runtime       // Set programmatically during execution
};
```

#### 4. Configuration Entry
```cpp
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};
```

## Current Implementation Analysis

### Strengths
1. **Modern C++20 Design**: Uses `std::variant`, `std::optional`, move semantics
2. **Type Safety**: Compile-time type checking with template methods
3. **Memory Efficiency**: Pimpl idiom for hiding implementation details
4. **Priority System**: Clear configuration priority hierarchy
5. **Flexible Value Types**: Supports basic types and containers

### Weaknesses
1. **Simple Parser**: Basic key=value parser with TODO for JSON/YAML
2. **No Validation**: No input validation or schema checking
3. **Placeholder Issues**: saveToFile uses placeholder values
4. **Limited Error Handling**: Basic error checking only
5. **No Advanced Features**: No environment variable support, profiles

## Required Enhancements

### 1. JSON/YAML Parser Implementation

#### Current Status
```cpp
// src/core/Config/Config.cpp line 21-22
bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    // ...
}
```

#### Implementation Requirements
1. **Dependency Management**: Add nlohmann/json or similar library
2. **Parser Implementation**: Replace key=value with proper JSON/YAML parsing
3. **Schema Validation**: Add configuration schema validation
4. **Format Detection**: Auto-detect file format based on extension

#### Implementation Plan
```cpp
class JSONConfigParser {
public:
    static bool parseFromFile(const std::string& filepath, Config& config);
    static bool parseFromString(const std::string& content, Config& config);
    static bool validateSchema(const Config& config);
};

class YAMLConfigParser {
public:
    static bool parseFromFile(const std::string& filepath, Config& config);
    static bool parseFromString(const std::string& content, Config& config);
};
```

#### Configuration Schema
```cpp
struct ConfigSchema {
    std::string key;
    ConfigValueType type;
    ConfigSource allowedSources;
    std::string description;
    std::any defaultValue;
    bool required;
    std::vector<std::string> validationRules;
    
    // Validation methods
    static bool validate(const ConfigSchema& schema, const ConfigEntry& entry);
    static std::vector<std::string> getValidationErrors(const ConfigSchema& schema, const ConfigEntry& entry);
};
```

### 2. Enhanced Error Handling

#### Custom Exception Hierarchy
```cpp
namespace nlm {

// Base exception class
class NLMException : public std::runtime_error {
public:
    enum class ErrorCode {
        ConfigError,
        InitializationError,
        MemoryError,
        PlasticityError,
        ValidationError,
        FileError,
        ParseError,
        NotFoundError
    };
    
    NLMException(const std::string& message, ErrorCode code)
        : std::runtime_error(message), code_(code) {}
    
    ErrorCode getCode() const { return code_; }
    
private:
    ErrorCode code_;
};

// Configuration-specific exceptions
class ConfigError : public NLMException {
public:
    ConfigError(const std::string& message, ErrorCode code = ErrorCode::ConfigError)
        : NLMException(message, code) {}
};

class ConfigFileError : public ConfigError {
public:
    ConfigFileError(const std::string& message, const std::string& filepath)
        : ConfigError(message, ErrorCode::FileError) {
        // Add filepath to error message
    }
};

class ConfigParseError : public ConfigError {
public:
    ConfigParseError(const std::string& message, const std::string& filepath, int line)
        : ConfigError(message, ErrorCode::ParseError) {
        // Add line information to error message
    }
};

class ConfigValidationError : public ConfigError {
public:
    ConfigValidationError(const std::string& message, const std::string& key)
        : ConfigError(message, ErrorCode::ValidationError) {
        // Add key information to error message
    }
};

} // namespace nlm
```

#### Enhanced Error Handling Patterns
```cpp
// Example: Safe configuration access with detailed error reporting
class SafeConfig {
public:
    template<typename T>
    std::optional<T> getWithValidation(const std::string& key, 
                                      const std::function<bool(const T&)>& validator = nullptr) {
        auto value = config_.get<T>(key);
        if (!value) {
            return std::nullopt;
        }
        
        if (validator && !validator(*value)) {
            throw nlm::ConfigValidationError(
                "Configuration validation failed for key: " + key,
                key
            );
        }
        
        return value;
    }
    
    // Configuration validation with detailed error reporting
    class ConfigValidator {
    public:
        static std::vector<std::string> validateAll(const Config& config) {
            std::vector<std::string> errors;
            
            // Check for required keys
            for (const auto& schema : getSchema()) {
                if (schema.required && !config.has(schema.key)) {
                    errors.push_back("Missing required configuration: " + schema.key);
                }
            }
            
            // Validate value ranges
            for (const auto& entry : config.getAllEntries()) {
                auto schemaIt = findSchema(entry.key);
                if (schemaIt != getSchema().end()) {
                    auto validationErrors = validateEntry(*schemaIt, entry);
                    errors.insert(errors.end(), validationErrors.begin(), validationErrors.end());
                }
            }
            
            return errors;
        }
        
    private:
        static std::vector<std::string> validateEntry(const ConfigSchema& schema, const ConfigEntry& entry) {
            std::vector<std::string> errors;
            
            // Type validation
            if (!isValidType(schema.type, entry.value)) {
                errors.push_back("Type mismatch for key " + schema.key + ": expected " + 
                               typeToString(schema.type) + ", got " + valueToString(entry.value));
            }
            
            // Range validation
            if (schema.validationRules.count("range")) {
                auto range = parseRange(schema.validationRules["range"]);
                if (!isInRange(entry.value, range)) {
                    errors.push_back("Value out of range for key " + schema.key + ": " + 
                                   valueToString(entry.value));
                }
            }
            
            // Enum validation
            if (!schema.allowedValues.empty() && !isAllowedValue(entry.value, schema.allowedValues)) {
                errors.push_back("Invalid value for key " + schema.key + ": " + 
                               valueToString(entry.value));
            }
            
            return errors;
        }
        
        // Helper methods for validation
        static bool isValidType(ConfigValueType type, const ConfigValue& value);
        static std::string typeToString(ConfigValueType type);
        static std::string valueToString(const ConfigValue& value);
        static std::pair<double, double> parseRange(const std::string& rangeStr);
        static bool isInRange(const ConfigValue& value, const std::pair<double, double>& range);
        static bool isAllowedValue(const ConfigValue& value, const std::vector<std::string>& allowedValues);
    };
};
```

### 3. Enhanced saveToFile Method

#### Current Implementation
```cpp
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}
```

#### Enhanced Implementation
```cpp
bool Config::saveToFile(const std::string& filepath, Format format = Format::Simple) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    switch (format) {
        case Format::Simple:
            saveAsSimple(file);
            break;
        case Format::JSON:
            saveAsJSON(file);
            break;
        case Format::YAML:
            saveAsYAML(file);
            break;
        case Format::TOML:
            saveAsTOML(file);
            break;
    }
    
    return true;
}

void Config::saveAsSimple(std::ostream& out) const {
    for (const auto& entry : pImpl->entries) {
        out << "# " << entry.description << "\n";
        out << entry.key << " = " << valueToString(entry.value) << "\n";
    }
}

void Config::saveAsJSON(std::ostream& out) const {
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        json[entry.key] = valueToJSON(entry.value);
    }
    
    out << json.dump(2); // Pretty print with 2-space indentation
}

void Config::saveAsYAML(std::ostream& out) const {
    // Implement YAML serialization
    // Could use YAML-cpp library or custom implementation
}

void Config::saveAsTOML(std::ostream& out) const {
    // Implement TOML serialization
    // Could use toml11 library or custom implementation
}
```

### 4. Environment Variable Support

#### Implementation
```cpp
class EnvironmentConfig {
public:
    static void loadFromEnvironment(Config& config) {
        // Map environment variables to configuration keys
        const std::unordered_map<std::string, std::string> envVarMap = {
            {"NLM_RANDOM_SEED", "random_seed"},
            {"NLM_NEURON_COUNT", "neuron_count"},
            {"NLM_SIMULATION_TIMESTEP", "simulation_timestep"},
            {"NLM_CONNECTION_PROBABILITY", "connection_probability"},
            {"NLM_PLATIVITY_LEARNING_RATE", "plasticity_learning_rate"},
            // ... more mappings
        };
        
        for (const auto& [envVar, configKey] : envVarMap) {
            const char* value = std::getenv(envVar.c_str());
            if (value != nullptr && value[0] != '\0') {
                config.set(configKey, std::string(value), ConfigSource::Environment);
            }
        }
    }
};
```

### 5. Configuration Profiles

#### Profile System
```cpp
enum class ConfigProfile {
    Development,
    Production,
    Research,
    Benchmark,
    Custom
};

class ProfileConfig {
public:
    static Config createProfile(ConfigProfile profile) {
        Config config;
        
        switch (profile) {
            case ConfigProfile::Development:
                setDevelopmentDefaults(config);
                break;
            case ConfigProfile::Production:
                setProductionDefaults(config);
                break;
            case ConfigProfile::Research:
                setResearchDefaults(config);
                break;
            case ConfigProfile::Benchmark:
                setBenchmarkDefaults(config);
                break;
            case ConfigProfile::Custom:
                // Use defaults, can be customized
                break;
        }
        
        return config;
    }
    
private:
    static void setDevelopmentDefaults(Config& config) {
        config.set("random_seed", 42);
        config.set("neuron_count", 100);
        config.set("simulation_timestep", 0.01);
        config.set("log_level", "DEBUG");
        config.set("max_simulation_steps", 1000);
    }
    
    static void setProductionDefaults(Config& config) {
        config.set("random_seed", 12345);
        config.set("neuron_count", 5000);
        config.set("simulation_timestep", 0.001);
        config.set("log_level", "INFO");
        config.set("max_simulation_steps", 100000);
    }
    
    // ... other profile methods
};
```

## Configuration Schema Definition

### Complete Configuration Schema
```cpp
struct ConfigSchema {
    // Core settings
    static constexpr struct {
        std::string key = "random_seed";
        ConfigValueType type = ConfigValueType::Int;
        ConfigSource allowedSources = ConfigSource::Default | ConfigSource::File | ConfigSource::CommandLine;
        std::string description = "Random seed for reproducible experiments";
        std::any defaultValue = 42;
        bool required = false;
        std::vector<std::string> validationRules = {"range:0,10000"};
    } RANDOM_SEED;
    
    static constexpr struct {
        std::string key = "simulation_timestep";
        ConfigValueType type = ConfigValueType::Double;
        ConfigSource allowedSources = ConfigSource::Default | ConfigSource::File | ConfigSource::CommandLine;
        std::string description = "Simulation timestep in seconds";
        std::any defaultValue = 0.001;
        bool required = false;
        std::vector<std::string> validationRules = {"range:0.0001,0.1"};
    } SIMULATION_TIMESTEP;
    
    // ... more schema definitions
    
    static std::vector<ConfigSchema> getAllSchemas() {
        return {
            RANDOM_SEED,
            SIMULATION_TIMESTEP,
            // ... all other schemas
        };
    }
};
```

## Implementation Dependencies

### Required Libraries
```cmake
# CMakeLists.txt additions
find_package(nlohmann_json REQUIRED)
find_package(CLI11 REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(toml11 REQUIRED)

# Add targets for configuration system
add_library(nlm_config STATIC
    src/core/Config/Config.cpp
    src/core/Config/Config.hpp
    src/core/Logger/Logger.cpp
    src/core/Logger/Logger.hpp
)

target_link_libraries(nlm_config PUBLIC
    nlohmann_json::nlohmann_json
    CLI11::CLI11
    yaml-cpp
    toml11
)

target_include_directories(nlm_config PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/core/Config>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/core/Logger>
)
```

### Package Management
```cmake
# Use Conan for dependency management
conan_configure()

# Or use vcpkg
vcpkg_install(nlohmann_json)
vcpkg_install(CLI11)
vcpkg_install(yaml-cpp)
vcpkg_install(toml11)
```

## Testing and Validation

### Unit Tests
```cpp
// test_config.cpp
class ConfigTests {
public:
    void testBasicConfig() {
        Config config;
        config.set("test_key", "test_value");
        assert(config.has("test_key"));
        assert(config.getOr<std::string>("test_key") == "test_value");
    }
    
    void testConfigValidation() {
        Config config;
        config.set("neuron_count", -5); // Invalid value
        
        // Validation should catch this
        auto errors = ConfigValidator::validateAll(config);
        assert(!errors.empty());
        assert(errors[0].find("neuron_count") != std::string::npos);
    }
    
    void testFileLoading() {
        Config config;
        bool loaded = config.loadFromFile("configs/default.cfg");
        assert(loaded);
        assert(config.has("random_seed"));
        assert(config.has("neuron_count"));
    }
    
    void testJSONLoading() {
        Config config;
        bool loaded = config.loadFromFile("configs/test.json");
        assert(loaded);
        // Validate JSON format
    }
};
```

### Integration Tests
```cpp
// Integration test for configuration system
class ConfigIntegrationTests {
public:
    void testCompleteWorkflow() {
        // 1. Create configuration
        Config config = ProfileConfig::createProfile(ConfigProfile::Development);
        
        // 2. Load from environment variables
        EnvironmentConfig::loadFromEnvironment(config);
        
        // 3. Load from file
        config.loadFromFile("configs/development.cfg");
        
        // 4. Load from command line
        std::string args[] = {"program", "--neuron_count=200", "--random_seed=123"};
        config.loadFromArgs(3, args);
        
        // 5. Set runtime values
        config.set("plasticity_learning_rate", 0.001);
        
        // 6. Validate configuration
        auto errors = ConfigValidator::validateAll(config);
        assert(errors.empty());
        
        // 7. Save configuration
        config.saveToFile("configs/output.json", Format::JSON);
        
        // 8. Load saved configuration
        Config config2;
        config2.loadFromFile("configs/output.json");
        
        // 9. Compare configurations
        assert(config.getAllEntries() == config2.getAllEntries());
    }
};
```

## Build System Integration

### CMake Integration
```cmake
# Add configuration library to the main build
add_subdirectory(src/core/Config)
add_subdirectory(src/core/Logger)

# Update main CMakeLists.txt
include(FetchContent)

# nlohmann/json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

# CLI11
FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.3.9
)
FetchContent_MakeAvailable(CLI11)

# yaml-cpp
find_package(yaml-cpp REQUIRED)

# toml11
FetchContent_Declare(
    toml11
    GIT_REPOSITORY https://github.com/ToruNiina/toml11.git
    GIT_TAG v3.8.0
)
FetchContent_MakeAvailable(toml11)
```

### Python Bindings Integration
```python
# Python configuration bindings
class PyConfig {
    def __init__(self):
        self._config = nlm.Config()
    
    def load_from_file(self, filepath):
        return self._config.loadFromFile(filepath)
    
    def load_from_env(self):
        EnvironmentConfig.loadFromEnvironment(self._config)
    
    def set(self, key, value, source=None):
        if source is None:
            source = nlm.ConfigSource.Runtime
        self._config.set(key, value, source)
    
    def get(self, key, default=None):
        value = self._config.getOr(key, default)
        return value
    
    def validate(self):
        return nlm.ConfigValidator::validateAll(self._config)
    
    def save(self, filepath, format="simple"):
        format_map = {
            "simple": nlm.Format::Simple,
            "json": nlm.Format::JSON,
            "yaml": nlm.Format::YAML,
            "toml": nlm.Format::TOML
        }
        return self._config.saveToFile(filepath, format_map.get(format, nlm.Format::Simple))
    
    def profile(self, profile_name):
        profile_map = {
            "development": nlm.ConfigProfile::Development,
            "production": nlm.ConfigProfile::Production,
            "research": nlm.ConfigProfile::Research,
            "benchmark": nlm.ConfigProfile::Benchmark
        }
        profile = profile_map.get(profile_name, nlm.ConfigProfile::Development)
        return ProfileConfig::createProfile(profile)
}
```

## Migration Guide

### From Version 0.1 to Version 1.0

#### Configuration Changes
```cpp
// Old configuration (Phase 1)
config->set("plasticity.stdp.learning_rate", 0.01);
config->set("plasticity.stdp.tau_plus", 20.0);
config->set("plasticity.stdp.tau_minus", 20.0);

// New configuration (Phase 2+)
config->set("plasticity_learning_rate", 0.01);
config->set("stdp_ltp_weight", 20.0);
config->set("stdp_ltd_weight", 20.0);
config->set("stdp_time_constant", 20.0); // New parameter
```

#### File Format Changes
```json
// Old format (configs/default.cfg)
random_seed = 42
simulation_timestep = 0.001
plasticity.stdp.learning_rate = 0.01

// New format (configs/default.json)
{
  "nlm": {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "plasticity": {
      "learning_rate": 0.01,
      "stdp": {
        "ltp_weight": 20.0,
        "ltd_weight": 20.0,
        "time_constant": 20.0
      }
    }
  }
}
```

#### Migration Utility
```cpp
class ConfigMigrationTool {
public:
    static Config migrate(const Config& oldConfig) {
        Config newConfig;
        
        // Map old keys to new keys
        std::unordered_map<std::string, std::string> keyMapping = {
            {"plasticity.stdp.learning_rate", "plasticity_learning_rate"},
            {"plasticity.stdp.tau_plus", "stdp_ltp_weight"},
            {"plasticity.stdp.tau_minus", "stdp_ltd_weight"},
            // ... more mappings
        };
        
        for (const auto& [oldKey, newKey] : keyMapping) {
            if (oldConfig.has(oldKey)) {
                auto value = oldConfig.getValue(oldKey);
                newConfig.set(newKey, value);
            }
        }
        
        return newConfig;
    }
};
```

## Performance Considerations

### Memory Usage
```cpp
// Memory-efficient configuration storage
class CompactConfig {
public:
    size_t getMemoryUsage() const {
        size_t total = sizeof(*this);
        
        for (const auto& entry : entries_) {
            total += entry.key.capacity();
            total += entry.description.capacity();
            total += estimateVariantSize(entry.value);
        }
        
        return total;
    }
    
private:
    size_t estimateVariantSize(const ConfigValue& value) const {
        size_t size = 0;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                size = arg.capacity();
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                size = arg.size() * sizeof(int);
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                size = arg.size() * sizeof(double);
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                for (const auto& str : arg) {
                    size += str.capacity();
                }
            } else {
                size = sizeof(T);
            }
        }, value);
        
        return size;
    }
    
    std::vector<ConfigEntry> entries_;
};
```

### CPU Performance
```cpp
// Performance-optimized configuration access
class FastConfig {
public:
    template<typename T>
    T getOptimized(const std::string& key) const {
        // Cache frequently accessed values
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return std::get<T>(it->second);
        }
        
        // Slow path
        auto value = config_.get<T>(key);
        if (value.has_value()) {
            cache_[key] = *value;
        }
        
        return *value;
    }
    
private:
    std::unordered_map<std::string, ConfigValue> cache_;
    Config config_;
};
```

## Security Considerations

### Secure Configuration File Handling
```cpp
class SecureConfig {
public:
    static bool loadFromSecureFile(const std::string& filepath) {
        // Check file permissions
        struct stat fileStat;
        if (stat(filepath.c_str(), &fileStat) != 0) {
            throw ConfigFileError("Cannot access configuration file", filepath);
        }
        
        // Check file ownership
        if (fileStat.st_uid != geteuid() && fileStat.st_gid != getegid()) {
            throw ConfigFileError("Insufficient permissions for configuration file", filepath);
        }
        
        // Check file permissions (read-only for owner)
        if ((fileStat.st_mode & S_IWUSR) || (fileStat.st_mode & S_IWGRP) || (fileStat.st_mode & S_IWOTH)) {
            throw ConfigFileError("Configuration file should not be writable", filepath);
        }
        
        return loadFromFile(filepath);
    }
};
```

### Sensitive Data Protection
```cpp
class SensitiveDataConfig {
public:
    void maskSensitiveValues(std::ostringstream& out) const {
        for (const auto& entry : entries_) {
            if (isSensitiveKey(entry.key)) {
                out << entry.key << " = [REDACTED]\n";
            } else {
                out << entry.key << " = " << valueToString(entry.value) << "\n";
            }
        }
    }
    
private:
    bool isSensitiveKey(const std::string& key) const {
        static const std::vector<std::string> sensitiveKeys = {
            "password",
            "secret",
            "token",
            "api_key",
            "auth",
            "credential"
        };
        
        std::string lowerKey = toLower(key);
        return std::any_of(sensitiveKeys.begin(), sensitiveKeys.end(),
            [&](const std::string& sensitiveKey) {
                return lowerKey.find(sensitiveKey) != std::string::npos;
            });
    }
};
```

## Documentation and Examples

### Configuration Examples
```cpp
// Basic configuration example
void basicConfigurationExample() {
    nlm::Config config;
    
    // Set values programmatically
    config.set("random_seed", 42);
    config.set("neuron_count", 1000);
    config.set("simulation_timestep", 0.001);
    
    // Load from file
    config.loadFromFile("configs/my_config.cfg");
    
    // Load from command line
    std::string args[] = {"program", "--neuron_count=500", "--random_seed=123"};
    config.loadFromArgs(3, args);
    
    // Create brain with configuration
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
}

// Advanced configuration example
void advancedConfigurationExample() {
    nlm::Config config = nlm::ProfileConfig::createProfile(nlm::ConfigProfile::Development);
    
    // Override with environment variables
    nlm::EnvironmentConfig::loadFromEnvironment(config);
    
    // Validate configuration
    auto errors = nlm::ConfigValidator::validateAll(config);
    if (!errors.empty()) {
        throw std::runtime_error("Configuration validation failed:\n" + 
                               join(errors, "\n"));
    }
    
    // Save configuration in different formats
    config.saveToFile("configs/development.json", nlm::Format::JSON);
    config.saveToFile("configs/development.yaml", nlm::Format::YAML);
    config.saveToFile("configs/development.toml", nlm::Format::TOML);
    
    // Run with profile
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
}
```

## Implementation Checklist

### Core Implementation
- [ ] Add nlohmann/json dependency
- [ ] Add CLI11 dependency
- [ ] Add yaml-cpp dependency
- [ ] Add toml11 dependency
- [ ] Implement JSON parser
- [ ] Implement YAML parser
- [ ] Implement TOML parser
- [ ] Add configuration schema definition
- [ ] Implement configuration validation
- [ ] Add custom exception hierarchy
- [ ] Add environment variable support
- [ ] Add configuration profiles
- [ ] Update saveToFile method
- [ ] Add configuration migration tool

### Build System Integration
- [ ] Update CMakeLists.txt
- [ ] Add dependency management
- [ ] Add cross-platform support
- [ ] Add installation packaging
- [ ] Add CI/CD integration

### Testing and Validation
- [ ] Add unit tests for configuration system
- [ ] Add integration tests
- [ ] Add performance tests
- [ ] Add security tests
- [ ] Add cross-platform tests
- [ ] Add regression tests

### Documentation
- [ ] Update configuration documentation
- [ ] Add API documentation
- [ ] Add examples
- [ ] Add migration guide
- [ ] Add troubleshooting guide

## Conclusion

This technical specification provides a comprehensive plan for enhancing the NLM configuration system. The improvements will:

1. **Modernize the Configuration System**: Move from simple key=value to JSON/YAML with schema validation
2. **Enhance Developer Experience**: Add comprehensive error handling, custom exceptions, and Pythonic interfaces
3. **Improve Reliability**: Add validation, environment variable support, and secure file handling
4. **Enhance Performance**: Optimize for memory usage and CPU performance
5. **Future-Proof**: Add extensible architecture for future features

The implementation follows best practices for C++ and Python development, with comprehensive testing, security considerations, and performance optimizations. The specification provides detailed implementation guidelines, examples, and migration paths to ensure a smooth transition from the current system to the enhanced configuration system.

The enhanced configuration system will significantly improve NLM's usability, reliability, and maintainability, making it more accessible to researchers, educators, and practitioners while preserving its scientific integrity and innovative capabilities.
