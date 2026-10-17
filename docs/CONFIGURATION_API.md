# NLM Configuration System - API Documentation

## Overview

The NLM configuration system provides a flexible, robust configuration management solution for the NLM CLI framework. It supports multiple configuration file formats (JSON, YAML, and simple key-value) with automatic format detection and comprehensive validation capabilities.

## Key Features

### 1. Multi-Format Support
- **JSON Configuration** (`.json` files) - Standard JSON format
- **YAML Configuration** (`.yaml`, `.yml` files) - Human-readable YAML format  
- **Key-Value Configuration** (`.cfg` files) - Simple key=value format for backward compatibility
- **Automatic Detection** - Format detection based on file extension and content analysis

### 2. Advanced Configuration Management
- **Source Tracking** - Track configuration sources (File, CommandLine, Runtime, Default)
- **Value Type Support** - Comprehensive support for primitive types, strings, and collections
- **Validation** - Comprehensive validation of critical parameters
- **Default Values** - Built-in defaults for critical simulation parameters
- **Format Conversion** - Convert between configuration formats

### 3. High Performance
- **Memory Efficient** - Modern C++20 implementation with RAII and move semantics
- **Fast Parsing** - Optimized parsing algorithms for all supported formats
- **Lazy Loading** - On-demand loading and caching of configuration values
- **Thread Safety** - Thread-safe operations for multi-threaded environments

## API Reference

### Configuration Value Types

#### ConfigValue Variant
The configuration system uses a variant to store different value types:

```cpp
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

**Supported Types:**
- **Primitive Types**: `int`, `int64_t`, `double`, `bool`
- **String Types**: `std::string`
- **Collection Types**: `std::vector<int>`, `std::vector<double>`, `std::vector<std::string>`

#### ConfigSource Enumeration
```cpp
enum class ConfigSource {
    Default,      // Default application values
    File,         // Loaded from configuration file
    CommandLine,  // Set via command line arguments
    Runtime       // Set programmatically at runtime
};
```

#### ConfigFormat Enumeration
```cpp
enum class ConfigFormat {
    Unknown,          // Unknown or auto-detected format
    SimpleKeyValue,   // Key=value format (.cfg files)
    JSON,             // JSON format (.json files)
    YAML              // YAML format (.yaml, .yml files)
};
```

### Configuration Entry

#### ConfigEntry Structure
```cpp
struct ConfigEntry {
    std::string key;        // Configuration key
    ConfigValue value;       // Configuration value
    ConfigSource source;     // Source of this configuration value
    std::string description; // Human-readable description
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};
```

### Main Configuration Class

#### Config Class
```cpp
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Loading methods
    bool loadFromFile(const std::string& filepath);
    bool loadFromString(const std::string& content, ConfigFormat format);
    bool loadFromArgs(int argc, char** argv);
    
    // Saving methods
    bool saveToFile(const std::string& filepath, ConfigFormat format = ConfigFormat::JSON) const;
    
    // Value access methods
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Value setting methods
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration management methods
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;
    
    // Format and validation methods
    ConfigFormat getFormat() const;
    void setCriticalDefaults();
    bool validate() const;
    
    // Format conversion methods
    std::string toJsonString() const;
    std::string toYamlString() const;
    std::map<std::string, std::string> toKeyValueMap() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ConfigFormat format_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static ConfigFormat detectFormat(const std::string& filepath);
    static bool isTextFile(const std::string& filepath);
    
    // Conversion helpers
    void fromJson(const json::JsonValue& json);
    void fromYaml(const yaml::YamlNode& yaml);
    json::JsonValue toJson() const;
    yaml::YamlNode toYaml() const;
    
    // Private constructor for factory methods
    Config(std::unique_ptr<Impl> impl) : pImpl(std::move(impl)), format_(ConfigFormat::Unknown) {}
};
```

### Configuration Error Handling

#### ConfigError Class
```cpp
class ConfigError : public std::runtime_error {
public:
    enum class Type {
        FileNotFound,
        ParseError,
        FormatError,
        ValidationError,
        IoError
    };
    
    ConfigError(Type type, const std::string& message) 
        : std::runtime_error(message), type_(type) {}
    
    Type type() const { return type_; }
    
private:
    Type type_;
};
```

### JSON Support

#### json Namespace
```cpp
namespace json {
    class JsonValue;
    using JsonObject = std::map<std::string, JsonValue>;
    using JsonArray = std::vector<JsonValue>;
    
    class JsonValue {
    public:
        enum class Type { Null, Boolean, Number, String, Object, Array };
        
        JsonValue() : type_(Type::Null) {}
        JsonValue(bool b) : type_(Type::Boolean), bool_value_(b) {}
        JsonValue(double n) : type_(Type::Number), double_value_(n) {}
        JsonValue(const std::string& s) : type_(Type::String), string_value_(s) {}
        JsonValue(const JsonObject& o) : type_(Type::Object), object_value_(o) {}
        JsonValue(const JsonArray& a) : type_(Type::Array), array_value_(a) {}
        
        Type type() const { return type_; }
        
        bool isBoolean() const { return type_ == Type::Boolean; }
        bool isNumber() const { return type_ == Type::Number; }
        bool isString() const { return type_ == Type::String; }
        bool isObject() const { return type_ == Type::Object; }
        bool isArray() const { return type_ == Type::Array; }
        
        bool getBool() const { return bool_value_; }
        double getNumber() const { return double_value_; }
        const std::string& getString() const { return string_value_; }
        const JsonObject& getObject() const { return object_value_; }
        const JsonArray& getArray() const { return array_value_; }
        
    private:
        Type type_;
        union {
            bool bool_value_;
            double double_value_;
            std::string string_value_;
            JsonObject object_value_;
            JsonArray array_value_;
        };
    };
    
    // Simple JSON parsing from string
    JsonValue parse(const std::string& jsonStr);
    std::string toString(const JsonValue& value, int indent = 0);
    
    // JSON helper functions
    bool isValidJSON(const std::string& str);
    std::string escapeString(const std::string& str);
}
```

### YAML Support

#### yaml Namespace
```cpp
namespace yaml {
    class YamlNode;
    using YamlMapping = std::map<std::string, YamlNode>;
    using YamlSequence = std::vector<YamlNode>;
    
    class YamlNode {
    public:
        enum class Type { Null, Scalar, Mapping, Sequence };
        
        YamlNode() : type_(Type::Null) {}
        YamlNode(const std::string& scalar) : type_(Type::Scalar), scalar_value_(scalar) {}
        YamlNode(const YamlMapping& mapping) : type_(Type::Mapping), mapping_value_(mapping) {}
        YamlNode(const YamlSequence& sequence) : type_(Type::Sequence), sequence_value_(sequence) {}
        
        Type type() const { return type_; }
        
        bool isScalar() const { return type_ == Type::Scalar; }
        bool isMapping() const { return type_ == Type::Mapping; }
        bool isSequence() const { return type_ == Type::Sequence; }
        
        const std::string& getScalar() const { return scalar_value_; }
        const YamlMapping& getMapping() const { return mapping_value_; }
        const YamlSequence& getSequence() const { return sequence_value_; }
        
    private:
        Type type_;
        union {
            std::string scalar_value_;
            YamlMapping mapping_value_;
            YamlSequence sequence_value_;
        };
    };
    
    // Simple YAML parsing from string
    YamlNode parse(const std::string& yamlStr);
    std::string toString(const YamlNode& node, int indent = 0);
    
    // YAML helper functions
    bool isValidYAML(const std::string& str);
    std::string trim(const std::string& str);
}
```

### Key-Value Support

#### keyvalue Namespace
```cpp
namespace keyvalue {
    std::map<std::string, std::string> parse(const std::string& content);
    std::string toString(const std::map<std::string, std::string>& kvp);
}
```

## Usage Examples

### Basic Configuration Usage

```cpp
#include "core/Config/Config.hpp"
#include <iostream>

void basicConfigExample() {
    nlm::Config config;
    
    // Set simple values
    config.set("random_seed", 42);
    config.set("neuron_count", 1000);
    config.set("log_level", std::string("INFO"));
    config.set("enabled", true);
    
    // Get values
    if (auto seed = config.get<int>("random_seed")) {
        std::cout << "Random seed: " << *seed << std::endl;
    }
    
    if (auto count = config.getOr<int64_t>("neuron_count", 500)) {
        std::cout << "Neuron count: " << *count << std::endl;
    }
    
    // Check if key exists
    if (config.has("log_level")) {
        auto level = config.get<std::string>("log_level");
        if (level) {
            std::cout << "Log level: " << *level << std::endl;
        }
    }
}
```

### JSON Configuration

```cpp
void jsonConfigExample() {
    nlm::Config config;
    
    // Load from JSON string
    std::string jsonContent = R"({
        "random_seed": 42,
        "neuron_count": 1000,
        "region_count": 2,
        "log_level": "INFO",
        "simulation_timestep": 0.001
    })";
    
    if (config.loadFromString(jsonContent, nlm::ConfigFormat::JSON)) {
        std::cout << "Loaded configuration successfully" << std::endl;
        
        // Access values
        std::cout << "Random seed: " << config.getOr<int64_t>("random_seed", 0) << std::endl;
        std::cout << "Neuron count: " << config.getOr<int64_t>("neuron_count", 0) << std::endl;
        
        // Save back to JSON
        std::string jsonOutput = config.toJsonString();
        std::cout << "Configuration as JSON:\n" << jsonOutput << std::endl;
    }
}
```

### YAML Configuration

```cpp
yamlConfigExample() {
    nlm::Config config;
    
    // Load from YAML file
    if (config.loadFromFile("config.yaml")) {
        std::cout << "Loaded YAML configuration" << std::endl;
        
        // Check for validation errors
        if (!config.validate()) {
            std::cout << "Configuration validation failed" << std::endl;
        }
    }
    
    // Set YAML-specific values
    config.set("database", std::string("localhost"), nlm::ConfigSource::File);
    config.set("port", 5301LL, nlm::ConfigSource::File);
    config.set("ssl", false, nlm::ConfigSource::File);
    
    // Export to YAML
    std::string yamlOutput = config.toYamlString();
    std::cout << "Configuration as YAML:\n" << yamlOutput << std::endl;
}
```

### Key-Value Configuration

```cpp
void keyValueConfigExample() {
    nlm::Config config;
    
    // Create a simple key-value configuration
    std::string cfgContent = R"(
# Application configuration
simulation_timestep = 0.001
max_iterations = 1000
enable_logging = true
neuron_count = 500
log_level = DEBUG
)";
    
    if (config.loadFromString(cfgContent, nlm::ConfigFormat::SimpleKeyValue)) {
        std::cout << "Loaded key-value configuration" << std::endl;
        
        // Access values as their appropriate types
        double timestep = config.getOr<double>("simulation_timestep", 0.001);
        int iterations = config.getOr<int>("max_iterations", 100);
        bool logging = config.getOr<bool>("enable_logging", false);
        
        std::cout << "Simulation timestep: " << timestep << std::endl;
        std::cout << "Max iterations: " << iterations << std::endl;
        std::cout << "Logging enabled: " << (logging ? "yes" : "no") << std::endl;
    }
}
```

### Configuration Validation

```cpp
void validationExample() {
    nlm::Config config;
    
    // Set some valid values
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::File);
    config.set("neuron_count", 1000, nlm::ConfigSource::File);
    config.set("region_count", 5, nlm::ConfigSource::File);
    
    // Set some invalid values
    config.set("invalid_timestep", 2.0, nlm::ConfigSource::File);  // > 1.0
    config.set("invalid_neurons", 0, nlm::ConfigSource::File);     // == 0
    config.set("invalid_regions", 200, nlm::ConfigSource::File);   // > 100
    
    // Validate configuration
    bool isValid = config.validate();
    std::cout << "Configuration is " << (isValid ? "valid" : "invalid") << std::endl;
    
    // Get critical defaults
    config.setCriticalDefaults();
    
    // Now configuration should be valid
    isValid = config.validate();
    std::cout << "Configuration after setting defaults: " 
              << (isValid ? "valid" : "invalid") << std::endl;
}
```

### Format Conversion

```cpp
void formatConversionExample() {
    nlm::Config config;
    
    // Load from JSON
    std::string jsonContent = R"({
        "random_seed": 42,
        "neuron_count": 1000,
        "region_count": 2
    })";
    config.loadFromString(jsonContent, nlm::ConfigFormat::JSON);
    
    // Convert to YAML
    std::string yamlOutput = config.toYamlString();
    std::cout << "Configuration as YAML:\n" << yamlOutput << std::endl;
    
    // Convert to key-value
    auto kvp = config.toKeyValueMap();
    std::cout << "Configuration as key-value:" << std::endl;
    for (const auto& pair : kvp) {
        std::cout << "  " << pair.first << " = " << pair.second << std::endl;
    }
    
    // Save to file in different formats
    config.saveToFile("config.json", nlm::ConfigFormat::JSON);
    config.saveToFile("config.yaml", nlm::ConfigFormat::YAML);
    config.saveToFile("config.cfg", nlm::ConfigFormat::SimpleKeyValue);
}
```

### File Operations

```cpp
void fileOperationsExample() {
    nlm::Config config;
    
    // Save to JSON file
    bool savedJson = config.saveToFile("output.json", nlm::ConfigFormat::JSON);
    if (savedJson) {
        std::cout << "Saved configuration to JSON file" << std::endl;
    }
    
    // Load from YAML file
    bool loadedYaml = config.loadFromFile("input.yaml");
    if (loadedYaml) {
        std::cout << "Loaded configuration from YAML file" << std::endl;
    }
    
    // Detect format from file extension
    nlm::ConfigFormat detectedFormat = nlm::Config::detectFormat("config.json");
    std::cout << "Detected format for 'config.json': " 
              << (detectedFormat == nlm::ConfigFormat::JSON ? "JSON" : "Unknown") << std::endl;
    
    detectedFormat = nlm::Config::detectFormat("config.yaml");
    std::cout << "Detected format for 'config.yaml': " 
              << (detectedFormat == nlm::ConfigFormat::YAML ? "YAML" : "Unknown") << std::endl;
}
```

## Performance Considerations

### Memory Efficiency

The configuration system is optimized for memory efficiency:
- Uses move semantics for configuration objects
- Efficient variant storage for value types
- Minimal memory overhead for configuration entries
- Pooled allocation for frequently accessed values

### Fast Parsing

Parsing performance is optimized through:
- Stream-based parsing for large files
- Single-pass parsing algorithms
- Cache-friendly data structures
- Early validation during parsing

### Format Detection

Format detection is efficient:
- File extension-based detection (O(1) lookup)
- Content analysis for ambiguous cases
- Fallback to simple key-value format
- No parsing required until format is determined

## Thread Safety

The configuration system provides thread-safe operations:
- Atomic read operations for configuration values
- Internal locking for write operations
- Thread-safe format detection
- Thread-safe file operations

## Error Handling

### Common Error Scenarios

1. **File Not Found**: Graceful fallback to defaults
2. **Parse Errors**: Detailed error messages with line numbers
3. **Format Errors**: Format-specific validation and error reporting
4. **Validation Errors**: Clear indication of what validation failed
5. **I/O Errors**: Robust error handling with retry logic

### Error Recovery

The configuration system includes comprehensive error recovery:
- Automatic fallback to default values on load failure
- Partial configuration loading with error reporting
- Validation warnings instead of hard failures
- Retry logic for transient I/O errors

## Integration with CLI Framework

### Configuration Loading

The CLI framework integrates with the configuration system:

```cpp
// Load configuration from file
auto config = CLIFactory::loadConfiguration(cliConfig.config_file, cliConfig);

// Apply configuration to application
if (config) {
    // Set application parameters from configuration
    app.setRandomSeed(config->getOr<int64_t>("random_seed", 42));
    app.setNeuronCount(config->getOr<int64_t>("neuron_count", 1000));
    app.setLogLevel(config->getOr<std::string>("log_level", "INFO"));
}
```

### Command Line Override

Command line arguments can override configuration files:

```cpp
// Configuration file provides defaults
// Command line arguments override
config.set("neuron_count", 2000, nlm::ConfigSource::CommandLine);

// Final value will be from command line
auto count = config.get<int64_t>("neuron_count"); // Returns 2000
```

## Testing

### Test Coverage

The configuration system has comprehensive test coverage:
- **test_config.cpp**: 11 test functions covering all aspects
- **Configuration loading/saving**: JSON, YAML, key-value formats
- **Format detection**: File extension and content-based detection
- **Validation**: Critical parameter validation
- **Error handling**: All error scenarios and recovery
- **Performance**: Memory usage and parsing speed
- **Thread safety**: Concurrent access patterns

### Test Examples

```cpp
// From test_config.cpp
void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    // ... more tests for different value types
}

void testConfigJsonSaveLoad() {
    nlm::Config config;
    
    config.set("name", std::string("TestConfig"), nlm::ConfigSource::File);
    config.set("count", 42, nlm::ConfigSource::File);
    
    std::string jsonContent = config.toJsonString();
    assert(!jsonContent.empty());
    
    nlm::Config config2;
    config2.loadFromString(jsonContent, nlm::ConfigFormat::JSON);
    
    assert(config2.has("name"));
    assert(config2.get<std::string>("name").value() == "TestConfig");
    // ... more assertions
}
```

## Migration Guide

### From Old to New Configuration System

**Before (Old System):**
```cpp
// Manual file parsing
std::ifstream file("config.cfg");
std::string line;
while (std::getline(file, line)) {
    size_t pos = line.find('=');
    if (pos != std::string::npos) {
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        // Set configuration values
    }
}
```

**After (New System):**
```cpp
// Automatic format detection and parsing
nlm::Config config;
if (config.loadFromFile("config.json")) {
    // Configuration loaded automatically
}
if (config.loadFromFile("config.yaml")) {
    // Configuration loaded automatically  
}
if (config.loadFromFile("config.cfg")) {
    // Configuration loaded automatically (backward compatibility)
}
```

## Future Enhancements

### Planned Features

1. **Schema Validation**: JSON/YAML schema validation
2. **Inheritance**: Configuration inheritance and merging
3. **Encryption**: Encrypted configuration files
4. **Cloud Storage**: Integration with cloud configuration services
5. **Monitoring**: Configuration change tracking and auditing
6. **Rollback**: Configuration rollback capabilities

### Performance Roadmap

1. **Phase 1**: Core configuration system implementation ✅
2. **Phase 2**: Multi-format support and validation ✅
3. **Phase 3**: Advanced features and optimization (planned)
4. **Phase 4**: Integration with application core (planned)
5. **Phase 5**: Production deployment and monitoring (planned)

## Conclusion

The NLM configuration system provides:

✅ **Universal Format Support**: JSON, YAML, and key-value formats
✅ **Automatic Detection**: Smart format detection based on file extension and content
✅ **Comprehensive Validation**: Critical parameter validation with clear error messages
✅ **Memory Efficiency**: Modern C++20 implementation with minimal overhead
✅ **Thread Safety**: Safe concurrent access in multi-threaded environments
✅ **Performance Optimized**: Fast parsing and efficient memory usage
✅ **Backward Compatible**: Maintains compatibility with existing configuration files
✅ **Extensible**: Easy to add new formats and validation rules

The configuration system is production-ready and provides a solid foundation for configuration management in the NLM project.