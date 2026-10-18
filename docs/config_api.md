# NLM Configuration System API

## Overview

The Enhanced Configuration System provides a comprehensive, type-safe, and flexible mechanism for managing simulation parameters and runtime settings in the NLM neural simulation framework. This system supports multiple configuration sources, advanced validation, hierarchical organization, and extensive management capabilities.

## Key Features

- **Type-safe value access**: Strong typing with compile-time type checking using templates
- **Multiple configuration sources**: Default, Environment, File, Command Line, and Runtime
- **Multiple file format support**: JSON, YAML, TOML, and simple key=value format
- **Hierarchical configuration**: Dot notation for nested configuration groups
- **Configuration validation**: Type checking, range validation, and custom validators
- **Configuration schema support**: Define structure and validation rules
- **Environment variable integration**: Automatic mapping from environment variables
- **Configuration management utilities**: Templates, migration tools, comparison, validation reports
- **Runtime configuration updates**: Hot-swapping of configuration during simulation
- **Backward compatibility**: Existing configuration files continue to work

## Configuration Value Types

### Supported Types

The configuration system supports the following value types:

| Type | C++ Type | Description | Example |
|------|----------|-------------|---------|
| Integer | `int` | 32-bit signed integer | `learning_rate = 0.01` |
| 64-bit Integer | `int64_t` | 64-bit signed integer | `neuron_count = 1000` |
| Floating Point | `double` | 64-bit floating point | `timestep = 0.001` |
| Boolean | `bool` | Boolean value | `enable_learning = true` |
| String | `std::string` | String value | `environment_name = "GridWorld"` |
| Integer Array | `std::vector<int>` | Array of integers | `hidden_sizes = [128, 64, 32]` |
| Double Array | `std::vector<double>` | Array of floats | `weights = [0.1, 0.5, 0.9]` |
| String Array | `std::vector<std::string>` | Array of strings | `tags = ["visual", "auditory", "motor"]` |
| Nested Object | `std::map<std::string, std::any>` | Key-value pairs for complex structures | `model = {layers = 3, units = [128, 64, 32]}` |

### Type Aliases

```cpp
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::map<std::string, std::any>
>;
```

## Configuration Sources

### ConfigSource Enum

```cpp
enum class ConfigSource {
    Default,    // Built-in default values (lowest priority)
    Environment, // Environment variables (medium priority)
    File,       // Loaded from configuration file
    CommandLine, // Parsed from command line arguments
    Runtime     // Set during runtime (highest priority)
};
```

### Source Priority

Configuration sources are applied in order of increasing priority (later sources override earlier ones):

1. **Default**: Initial configuration values loaded from hardcoded defaults
2. **Environment**: Override defaults with environment variable values
3. **File**: Override with values from configuration file
4. **Command Line**: Override with command line arguments
5. **Runtime**: Final override with runtime changes

## Core Classes

### Config Class

The main configuration manager for the NLM simulation.

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
    
    // Environment variable configuration
    static void setEnvironmentPrefix(const std::string& prefix);
    static std::string getEnvironmentPrefix();
    
    // Load from different sources
    bool loadFromFile(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromTOML(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool loadFromEnvironment();
    bool loadFromDefaults();
    
    // Save to different formats
    bool saveToFile(const std::string& filepath, bool pretty = false) const;
    bool saveToYAML(const std::string& filepath, bool pretty = false) const;
    bool saveToJSON(const std::string& filepath, bool pretty = false) const;
    bool saveToTOML(const std::string& filepath, bool pretty = false) const;
    
    // Get values with enhanced type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getRequired(const std::string& key) const;
    
    // Hierarchical access with dot notation
    template<typename T>
    std::optional<T> getHierarchical(const std::string& dot_key) const;
    
    // Set values with type safety
    void set(const std::string& key, const ConfigValue& value,
             ConfigSource source = ConfigSource::Runtime,
             const ConfigMetadata& metadata = ConfigMetadata());
    void set(const std::string& key, const std::string& value,
             ConfigSource source = ConfigSource::Runtime,
             const ConfigMetadata& metadata = ConfigMetadata());
    // ... other set overloads for different types
    
    // Hierarchical set
    void setHierarchical(const std::string& dot_key, const ConfigValue& value,
                        ConfigSource source = ConfigSource::Runtime,
                        const ConfigMetadata& metadata = ConfigMetadata());
    
    // Configuration metadata
    const ConfigMetadata& getMetadata(const std::string& key) const;
    void setMetadata(const std::string& key, const ConfigMetadata& metadata);
    
    // Validation
    bool validate(const ConfigSchema& schema) const;
    bool validate(const ConfigValidator& validator) const;
    std::vector<std::string> getValidationErrors(const ConfigSchema& schema) const;
    std::vector<std::string> getValidationErrors(const ConfigValidator& validator) const;
    
    // Check existence
    bool has(const std::string& key) const;
    bool hasHierarchical(const std::string& dot_key) const;
    
    // Remove key
    void remove(const std::string& key);
    void removeHierarchical(const std::string& dot_key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getHierarchicalKeys() const;
    
    // Configuration hierarchy management
    ConfigNode* getRootNode() { return root_node.get(); }
    const ConfigNode* getRootNode() const { return root_node.get(); }
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    std::string summaryHierarchical() const;
    
    // Configuration management utilities
    void merge(const Config& other, bool override_source = true);
    Config diff(const Config& other) const;
    bool isSubset(const Config& other) const;
    
    // Schema and template management
    void addSchema(const std::string& name, std::unique_ptr<ConfigSchema> schema);
    ConfigSchema* getSchema(const std::string& name);
    const ConfigSchema* getSchema(const std::string& name) const;
    
    void addValidator(const std::string& name, std::unique_ptr<ConfigValidator> validator);
    ConfigValidator* getValidator(const std::string& name);
    const ConfigValidator* getValidator(const std::string& name) const;
};
```

### ConfigMetadata Struct

Represents configuration metadata including type information, validation rules, and descriptions.

```cpp
struct ConfigMetadata {
    std::string description;
    std::string type;
    std::any default_value;
    std::optional<std::pair<double, double>> range;
    bool required = false;
    std::vector<std::string> tags;
    std::map<std::string, std::string> metadata;
    
    ConfigMetadata() : description(), type(), required(false) {}
};
```

### ConfigNode Class

Hierarchical configuration node for nested configuration structures.

```cpp
class ConfigNode {
public:
    ConfigNode() = default;
    explicit ConfigNode(const std::string& name);
    
    std::string name;
    std::any value;
    std::map<std::string, std::unique_ptr<ConfigNode>> children;
    std::map<std::string, ConfigMetadata> metadata;
    
    ConfigNode* getChild(const std::string& name);
    const ConfigNode* getChild(const std::string& name) const;
    ConfigNode* addChild(const std::string& name);
    void setValue(const std::any& val);
    
    template<typename T>
    T getValue() const;
    
    bool hasValue() const;
    bool hasChild(const std::string& name) const;
};
```

### ConfigSchema Class

Configuration schema for defining structure and validation rules.

```cpp
class ConfigSchema {
public:
    using ValidationFunction = std::function<bool(const ConfigValue&)>;
    using TransformationFunction = std::function<ConfigValue(const ConfigValue&)>;
    
    struct FieldSchema {
        std::string name;
        std::string type;
        std::any default_value;
        std::optional<std::pair<double, double>> range;
        bool required = false;
        std::vector<std::string> tags;
        ValidationFunction validator;
        TransformationFunction transformer;
        std::string description;
    };
    
    std::string name;
    std::vector<FieldSchema> fields;
    std::map<std::string, std::unique_ptr<ConfigSchema>> sub_schemas;
    
    void addField(const FieldSchema& field);
    void addSubSchema(std::unique_ptr<ConfigSchema> schema);
    bool validate(const Config& config) const;
    ConfigValue transform(const ConfigValue& value, const std::string& field) const;
    bool hasField(const std::string& field_name) const;
};
```

### ConfigValidator Class

Configuration validator for custom validation rules.

```cpp
class ConfigValidator {
public:
    using ValidationRule = std::function<bool(const ConfigValue&)>;
    using ValidationMessage = std::function<std::string(const ConfigValue&)>;
    
    struct ValidationRuleEntry {
        std::string field_name;
        ValidationRule rule;
        ValidationMessage error_message;
        bool is_range_check = false;
    };
    
    std::string name;
    std::vector<ValidationRuleEntry> rules;
    
    void addRule(const std::string& field_name,
                 const ValidationRule& rule,
                 const ValidationMessage& error_msg = nullptr,
                 bool is_range_check = false);
    bool validate(const Config& config) const;
    std::vector<std::string> getValidationErrors(const Config& config) const;
};
```

## Usage Examples

### Basic Configuration

```cpp
#include <nlm/core/Config.hpp>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Set default values
    config->set("neuron_count", 1000LL);
    config->set("simulation_timestep", 0.001);
    config->set("plasticity_learning_rate", 0.01);
    config->set("environment_name", std::string("GridWorld"));
    
    // Set metadata
    nlm::ConfigMetadata neuron_metadata;
    neuron_metadata.description = "Number of neurons in the brain";
    neuron_metadata.type = "int64_t";
    neuron_metadata.default_value = 1000LL;
    neuron_metadata.range = std::make_pair(1, 1000000);
    neuron_metadata.required = true;
    neuron_metadata.tags = {"simulation", "brain"};
    config->setMetadata("neuron_count", neuron_metadata);
    
    // Override with file configuration
    if (!config->loadFromFile("configs/default.cfg")) {
        std::cerr << "Failed to load configuration file" << std::endl;
        return 1;
    }
    
    // Override with command line
    config->loadFromArgs(argc, argv);
    
    // Get values with type safety
    int neurons = config->getOr<int64_t>("neuron_count", 1000);
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    bool learning = config->getOr<bool>("enable_learning", false);
    
    return 0;
}
```

### Hierarchical Configuration

```cpp
// Using dot notation for nested configuration
config->set("simulation.neuron_count", 1000LL);
config->set("simulation.timestep", 0.001);
config->set("simulation.region_count", 2);

config->set("development.synaptogenesis_rate", 0.001);
config->set("development.pruning_rate", 0.0001);

config->set("neuromodulation.dopamine_baseline", 0.1);
config->set("neuromodulation.curiosity_rate", 0.05);

// Get hierarchical values
auto neuron_count = config->getHierarchical<int64_t>("simulation.neuron_count");
auto dopamine_baseline = config->getHierarchical<double>("neuromodulation.dopamine_baseline");
```

### Configuration with Validation

```cpp
// Define validation schema
nlm::ConfigSchema simulation_schema("simulation");
simulation_schema.addField({"neuron_count", "int64_t", 1000LL,
                            std::make_pair(1, 1000000), true,
                            {"simulation", "brain"},
                            [](const ConfigValue& v) {
                                return std::get<int64_t>(v) > 0;
                            },
                            nullptr, "Number of neurons in the brain"});

// Validate configuration
if (!config->validate(simulation_schema)) {
    auto errors = config->getValidationErrors(simulation_schema);
    for (const auto& error : errors) {
        std::cerr << "Validation error: " << error << std::endl;
    }
}
```

### Environment Variable Configuration

```cpp
// Configure environment prefix
nlm::Config::setEnvironmentPrefix("NLM_");

// Set environment variable
// In bash: export NLM_SIMULATION_NEURON_COUNT=2000

// Load from environment
config->loadFromEnvironment();

// Get values (will read from environment if set)
int neurons = config->getOr<int64_t>("simulation.neuron_count", 1000);
```

### Configuration Templates

```cpp
// Load development template
auto dev_config = nlm::ConfigTemplateManager::loadDevelopmentTemplate();

// Load testing template
auto test_config = nlm::ConfigTemplateManager::loadTestingTemplate();

// Load production template
auto prod_config = nlm::ConfigTemplateManager::loadProductionTemplate();

// Merge with base configuration
Config base_config;
base_config.set("simulation.timestep", 0.001);
base_config.merge(dev_config, true);
```

### Configuration Export/Import

```cpp
// Export to different formats
nlm::ConfigExporter exporter("my_export");
exporter.setFormat(nlm::ConfigExporter::ExportFormat::JSON);
exporter.setPrettyPrint(true);

std::string json_output = exporter.exportConfig(config);
exporter.exportToFile(config, "exported_config.json");

// Import from different formats
nlm::ConfigImporter importer("my_import");
Config imported_config = importer.importFromFile("imported_config.json");
```

### Configuration Comparison and Diff

```cpp
// Compare two configurations
nlm::ConfigComparator comparator("detailed");
auto changes = comparator.compare(old_config, new_config);

// Find breaking changes
auto breaking_changes = nlm::ConfigDiffTool::findBreakingChanges(old_config, new_config);

// Get similarity score
double similarity = comparator.getSimilarityScore(old_config, new_config);
```

## Type Safety

### Template Get Methods

```cpp
template<typename T>
std::optional<T> Config::get(const std::string& key) const;

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const;

template<typename T>
T Config::getRequired(const std::string& key) const;
```

### Type Handling Examples

```cpp
// Example of type-specific access
auto neuronCount = config->get<int64_t>("neuron_count");
if (neuronCount.has_value()) {
    std::cout << "Neuron count: " << neuronCount.value() << std::endl;
}

// Default value access
int count = config->getOr<int64_t>("neuron_count", 1000);

// Required value access (throws if not found)
try {
    int required_value = config->getRequired<int>("required_parameter");
} catch (const nlm::ConfigError& e) {
    std::cerr << "Missing required configuration: " << e.what() << std::endl;
}
```

## Configuration File Formats

### JSON Format

```json
{
  "simulation": {
    "neuron_count": 1000,
    "simulation_timestep": 0.001,
    "region_count": 2,
    "enable_plasticity": true,
    "plasticity_learning_rate": 0.01
  },
  "environment": {
    "name": "GridWorld",
    "width": 10,
    "height": 10
  }
}
```

### YAML Format

```yaml
simulation:
  neuron_count: 1000
  simulation_timestep: 0.001
  region_count: 2
  enable_plasticity: true
  plasticity_learning_rate: 0.01

environment:
  name: GridWorld
  width: 10
  height: 10
```

### TOML Format

```toml
simulation.neuron_count = 1000
simulation.simulation_timestep = 0.001
simulation.region_count = 2
simulation.enable_plasticity = true
simulation.plasticity_learning_rate = 0.01

[environment]
name = "GridWorld"
width = 10
height = 10
```

## Error Handling

### Configuration Errors

```cpp
enum class ConfigError {
    Success = 0,
    FileNotFound = 1,
    InvalidFormat = 2,
    TypeMismatch = 3,
    InvalidValue = 4,
    SaveFailed = 5,
    LoadFailed = 6
};
```

### Error Handling Pattern

```cpp
auto config = std::make_shared<nlm::Config>();

// Load with error handling
try {
    if (!config->loadFromFile("config.json")) {
        std::cerr << "Failed to load configuration file" << std::endl;
        return 1;
    }
} catch (const nlm::ConfigFileError& e) {
    std::cerr << "File error: " << e.what() << std::endl;
    return 1;
} catch (const nlm::ConfigParseError& e) {
    std::cerr << "Parse error: " << e.what() << std::endl;
    return 1;
}

// Set values with validation
try {
    config->set("neuron_count", 1000);
    config->set("invalid_parameter", "bad_value"); // This may cause type errors
} catch (const std::exception& e) {
    std::cerr << "Configuration error: " << e.what() << std::endl;
    return 1;
}
```

## Configuration Management

### Templates

Built-in templates available:

- **development**: Optimized for development and debugging
- **testing**: Optimized for automated testing
- **production**: Optimized for deployment
- **minimal**: Only essential settings
- **optimized**: Performance-focused

### Validation Reports

```cpp
nlm::ConfigValidationReport report("Production Validation");
report.addIssue("neuron_count", "Neuron count is below recommended minimum", 
                nlm::ConfigValidationReport::Issue::Severity::Warning,
                "Consider increasing neuron count for better performance");

std::cout << report.generateReport() << std::endl;
```

### Configuration History

```cpp
nlm::ConfigHistoryManager history("config_history.json");

// Save snapshot
history.saveSnapshot(config, "v1.0", {{ "environment", "production" }});

// Rollback to previous version
Config restored_config = history.rollbackToSnapshot(0);
```

### Configuration Monitoring

```cpp
callback = [](const ConfigValue& value, ConfigSource source) {
    std::cout << "Configuration changed: " << value << " (source: " << 
        static_cast<int>(source) << ")" << std::endl;
};

nlm::ConfigMonitor monitor("performance_monitor");
monitor.addRule("performance.*", 
                 [](const ConfigValue& v) { return std::get<double>(v) > 100.0; },
                 callback, "Alert when performance metrics exceed threshold");
```

## Performance Considerations

### Memory Efficiency

```cpp
// Use const references for large value accesses
const auto& config = brain->getConfig();
int neurons = config->getOr<int64_t>("neuron_count", 1000);
```

### Lookup Optimization

```cpp
// Frequently accessed values - cache locally
const int64_t neuronCount = config->getOr<int64_t>("neuron_count", 1000);
const double timestep = config->getOr<double>("simulation_timestep", 0.001);

// Use in simulation loop
for (int step = 0; step < maxSteps; ++step) {
    brain->step(step, step * timestep);
}
```

### Thread Safety

```cpp
// Configuration objects should not be accessed concurrently
// unless proper synchronization is implemented
auto config1 = std::make_shared<nlm::Config>();
auto config2 = std::make_shared<nlm::Config>();

// Thread-safe operations (single thread per config)
config1->loadFromFile("config1.json");
config2->loadFromFile("config2.json");
```

## Integration Examples

### With Brain Simulation

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config.hpp>

int main() {
    // Create and configure brain
    auto config = std::make_shared<nlm::Config>();
    
    // Load experiment configuration
    config->loadFromFile("experiments/large_scale.cfg");
    
    // Command line overrides
    config->loadFromArgs(argc, argv);
    
    // Create brain with configuration
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Run simulation with configuration
    config->set("max_simulation_steps", 10000);
    
    for (nlm::SimulationStep step = 0; step < 10000; ++step) {
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
    }
    
    return 0;
}
```

### Configuration Factory Pattern

```cpp
// Create specialized configurations
Config dev_config = nlm::ConfigFactory::createDevelopmentConfig();
Config test_config = nlm::ConfigFactory::createTestingConfig();
Config prod_config = nlm::ConfigFactory::createProductionConfig();

// Extend base configuration
Config base = nlm::ConfigFactory::createMinimalConfig();
base.merge(dev_config, true);
```

## Migration Guide

### From Earlier Versions

Configuration system changes:

- **Version 0.1.0**: Simple key-value pairs only
- **Version 0.2.0**: Added type safety and multiple sources
- **Version 0.3.0**: Added validation and error handling
- **Version 0.4.0**: Added file format support (JSON/YAML/TOML)
- **Version 0.5.0**: Added runtime configuration updates
- **Version 0.6.0**: Enhanced type safety and template-based access
- **Version 0.7.0**: Added hierarchical configuration with dot notation
- **Version 0.8.0**: Added comprehensive management utilities

### Migration Examples

#### From Simple Key-Value to Hierarchical

Old format (`config.cfg`):
```
neuron_count = 1000
simulation_timestep = 0.001
plasticity_learning_rate = 0.01
```

New format (`config.json`):
```json
{
  "simulation": {
    "neuron_count": 1000,
    "simulation_timestep": 0.001
  },
  "plasticity": {
    "learning_rate": 0.01
  }
}
```

Migration code:
```cpp
// Convert old format to new format
Config new_config;
for (const auto& key : old_config.getKeys()) {
    auto value = old_config.getHierarchical(key);
    if (value.has_value()) {
        // Simple key becomes top-level category
        if (key == "neuron_count") {
            new_config.setHierarchical("simulation.neuron_count", value.value());
        } else if (key == "simulation_timestep") {
            new_config.setHierarchical("simulation.timestep", value.value());
        } else if (key == "plasticity_learning_rate") {
            new_config.setHierarchical("plasticity.learning_rate", value.value());
        }
    }
}
```

## Configuration Templates

See the `configs/` directory for available templates:

- `configs/development.json`, `development.yaml`, `development.toml`
- `configs/testing.json`, `testing.yaml`, `testing.toml`
- `configs/production.json`, `production.yaml`, `production.toml`
- `configs/default.cfg` (legacy format)

## References

1. [Configuration Design Patterns](https://en.wikipedia.org/wiki/Configuration_management)
2. [Type Safety in C++](https://isocpp.org/wiki/faq/const-correctness#const-overloaded-functions)
3. [YAML for Configuration](https://yaml.org/)
4. [JSON for Data Exchange](https://www.json.org/)
5. [TOML for Configuration](https://toml.io/)

## See Also

- [Logger API](logging_api.md) - Logging system integration
- [Brain API](brain_api.md) - Brain simulation with configuration
- [Experiment API](experiments_api.md) - Experimental configuration management

**Last updated:** October 18, 2026
**Version:** 0.8.0
