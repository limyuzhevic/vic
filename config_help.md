# NLM Configuration System Help

## Overview

The NLM (Neural Learning Machine) configuration system provides a flexible way to configure brain simulation parameters, including JSON/YAML support, validation, and extensive command-line options.

## Configuration File Formats

### Simple Key-Value Format (default.cfg)
```
# This is a comment
key = value
another_key = "quoted value"
boolean_flag = true
number = 42.5
list = [1, 2, 3, 4]
```

### JSON Format
```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012
    },
    "logging": {
        "log_level": "INFO",
        "log_to_file": false
    }
}
```

### YAML Format
```yaml
brain:
    neuron_count: 1000
    region_count: 2
    connection_probability: 0.1

plasticity:
    stdp_ltp_weight: 0.01
    stdp_ltd_weight: 0.012

logging:
    log_level: INFO
    log_to_file: false
```

## Available Configuration Options

### Brain Configuration
- `brain.neuron_count` - Total number of neurons (int, default: 1000)
- `brain.region_count` - Number of brain regions (int, default: 1)
- `brain.connection_probability` - Connection probability (float, default: 0.1)
- `brain.initial_weight_mean` - Mean initial synaptic weight (float, default: 0.5)
- `brain.initial_weight_std` - Standard deviation of initial weights (float, default: 0.1)

### Plasticity Configuration
- `plasticity.stdp.enable` - Enable STDP (bool, default: true)
- `plasticity.stdp.learning_rate` - STDP learning rate (float, default: 0.001)
- `plasticity.stdp.tau_plus` - STDP time constant (float, default: 20.0)
- `plasticity.stdp.tau_minus` - STDP time constant (float, default: 20.0)
- `plasticity.hebbian.enable` - Enable Hebbian learning (bool, default: true)
- `plasticity.structural.enable` - Enable structural plasticity (bool, default: true)

### Neuromodulation Configuration
- `neuromod.dopamine.scale` - Dopamine modulation scale (float, default: 1.0)
- `neuromod.curiosity.enable` - Enable curiosity-driven exploration (bool, default: true)
- `neuromod.novelty.enable` - Enable novelty detection (bool, default: true)

### Logging Configuration
- `log.level` - Log level (string: DEBUG, INFO, WARNING, ERROR, default: INFO)
- `log.to_file` - Log to file (bool, default: false)
- `log.filename` - Log filename (string, default: nlm.log)

### Simulation Configuration
- `simulation.timestep` - Simulation timestep (float, default: 0.001)
- `simulation.max_steps` - Maximum simulation steps (int, default: 10000)
- `simulation.time_limit` - Simulation time limit (float, default: 0.0)

### Development Configuration
- `development.synaptogenesis_rate` - Synaptogenesis rate (float, default: 0.0001)
- `development.pruning_rate` - Pruning rate (float, default: 0.00001)

## Command Line Interface

### Basic Usage
```bash
# Run with default configuration
./nlm

# Load custom configuration file
./nlm --config custom.cfg

# Override specific parameters
./nlm --brain.neuron_count=2000 --plasticity.stdp.learning_rate=0.01
```

### Available Command Line Options
- `--help, -h` - Display help information
- `--version` - Display version information
- `--config <file>` - Load configuration from file
- `--list-configs` - List available configuration templates
- `--validate <file>` - Validate configuration file
- `--benchmark <mode>` - Run benchmark mode
- `--verbose, -v` - Enable verbose logging
- `--log-level <level>` - Set logging level
- `--log-file <file>` - Set log file

### Example Command Line Usage
```bash
./nlm \
    --config production.cfg \
    --brain.neuron_count=5000 \
    --plasticity.stdp.learning_rate=0.005 \
    --log-level=DEBUG \
    --log-file=simulation.log
```

## Configuration Validation

The configuration system supports schema validation to ensure all required parameters are present and have valid values.

### Validation Examples
```cpp
// Define configuration schema
std::vector<ConfigOption> schema = {
    {"brain.neuron_count", ConfigValue(100), "Number of neurons", 
     [](const ConfigValue& v) { return std::get<int>(v) > 0; }},
    {"plasticity.stdp.learning_rate", ConfigValue(0.001), "STDP learning rate",
     [](const ConfigValue& v) { return std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; }},
    {"log.level", ConfigValue("INFO"), "Log level",
     [](const ConfigValue& v) { 
         const std::vector<std::string> levels = {"DEBUG", "INFO", "WARNING", "ERROR"};
         return std::find(levels.begin(), levels.end(), std::get<std::string>(v)) != levels.end();
     }},
    {"simulation.timestep", ConfigValue(0.001), "Simulation timestep",
     [](const ConfigValue& v) { return std::get<double>(v) > 0.0; }}
};

// Validate configuration
bool isValid = config.validate(schema);
if (!isValid) {
    auto result = config.validateWithDetails(schema);
    for (const auto& error : result.errors) {
        std::cerr << "ERROR: " << error << std::endl;
    }
}
```

## Configuration Comparison

The system can compare two configurations to identify differences:

```cpp
// Compare two configurations
std::string diff = Config::diff(config1, config2);
std::cout << "Configuration differences:" << std::endl;
std::cout << diff << std::endl;
```

## Performance Monitoring

The configuration system includes performance monitoring capabilities:

```cpp
// Get performance metrics
Config::PerformanceMetrics metrics = config.getPerformanceMetrics();
std::cout << "Total accesses: " << metrics.totalAccesses << std::endl;
std::cout << "Successful accesses: " << metrics.successfulAccesses << std::endl;
std::cout << "Access time: " << metrics.accessTime << " ms" << std::endl;

// Reset performance metrics
config.resetPerformanceMetrics();
```

## Configuration Templates

Available configuration templates:

### Development Template
```
// For rapid development and testing
brain.neuron_count = 100
brain.region_count = 1
connection_probability = 0.5
log_level = DEBUG
log_to_file = true
simulation.max_steps = 1000
```

### Production Template
```
// For production simulations
brain.neuron_count = 5000
brain.region_count = 2
connection_probability = 0.1
log_level = INFO
log_to_file = true
simulation.max_steps = 100000
plasticity.stdp.enable = true
neuromod.dopamine.scale = 1.0
```

### Research Template
```
// For scientific research
brain.neuron_count = 10000
brain.region_count = 3
connection_probability = 0.05
log_level = INFO
log_to_file = true
simulation.max_steps = 1000000
plasticity.stdp.enable = true
plasticity.hebbian.enable = true
neuromod.curiosity.enable = true
neuromod.novelty.enable = true
```

## Error Handling

The configuration system provides comprehensive error handling:

### Common Error Messages
- `"Configuration file not found: <filepath>"`
- `"Invalid configuration format in <file> at line <line>"`
- `"Unknown configuration key: <key>"`
- `"Invalid value type for <key>: expected <type>"`
- `"Configuration validation failed"`

### Error Recovery
The system provides graceful fallback for invalid configurations:
- Missing values fall back to defaults
- Invalid values are rejected with clear error messages
- File parsing errors provide detailed location information

## Integration with Main Application

The configuration system integrates seamlessly with the main NLM application:

### Command Line Argument Parsing
The main application accepts the following configuration-related arguments:

```bash
./nlm \
    --config custom.cfg \
    --brain.neuron_count=2000 \
    --plasticity.stdp.learning_rate=0.01 \
    --benchmark speed \
    --validate \
    --verbose
```

### Configuration Override Chain
1. Default values
2. Configuration file values
3. Command line arguments (highest priority)

### Saving Configuration Results
Configuration results can be saved in various formats:
- Original file format (JSON/YAML/simple)
- Configuration comparison reports
- Performance metrics
- Validation reports

## Migration Guide

### From Simple to JSON/YAML
If you're migrating from the simple key=value format to JSON/YAML:

1. Convert existing config files:
   ```bash
   # Convert to JSON
   ./nlm --validate old.cfg --output old.json
   
   # Convert to YAML
   ./nlm --validate old.cfg --output old.yaml
   ```

2. Update configuration references in code:
   ```cpp
   // Old way
   config->set("brain.neuron_count", 1000);
   
   // New way (same syntax)
   config->set("brain.neuron_count", 1000);
   ```

### Version Migration
Configuration format changes are backward compatible with proper validation.

## Advanced Usage

### Programmatic Configuration
```cpp
#include "core/Config/Config.hpp"

namespace nlm {
    class AdvancedConfigurator {
    public:
        static Config createDevelopmentConfig() {
            Config config;
            
            // Set multiple values efficiently
            config.set("brain.neuron_count", 100);
            config.set("brain.region_count", 1);
            config.set("connection_probability", 0.5);
            config.set("log.level", "DEBUG");
            
            return config;
        }
        
        static Config createResearchConfig(int neuronCount = 10000) {
            Config config;
            
            // Use efficient batch operations
            config.set("brain.neuron_count", neuronCount);
            config.set("brain.region_count", 3);
            config.set("connection_probability", 0.05);
            config.set("log.level", "INFO");
            config.set("log.to_file", true);
            config.set("simulation.max_steps", 1000000);
            
            return config;
        }
    };
}
```

### Configuration Templates
```cpp
// Configuration template system
class ConfigTemplates {
public:
    static std::vector<Config> getStandardTemplates() {
        std::vector<Config> templates;
        
        // Add predefined templates
        templates.push_back(getDevelopmentConfig());
        templates.push_back(getProductionConfig());
        templates.push_back(getResearchConfig());
        templates.push_back(getPerformanceConfig());
        
        return templates;
    }
    
private:
    static Config getDevelopmentConfig() {
        Config config;
        config.set("brain.neuron_count", 100);
        config.set("brain.region_count", 1);
        config.set("log.level", "DEBUG");
        config.set("simulation.max_steps", 1000);
        return config;
    }
    
    static Config getProductionConfig() {
        Config config;
        config.set("brain.neuron_count", 5000);
        config.set("brain.region_count", 2);
        config.set("log.level", "INFO");
        config.set("simulation.max_steps", 100000);
        return config;
    }
    
    // ... more templates
};
```

## Troubleshooting

### Common Issues and Solutions

**Issue: Configuration file not loading**
```
Solution: Check file permissions and path
./nlm --config ./configs/default.cfg
```

**Issue: Invalid configuration values**
```
Solution: Use validation mode to check errors
./nlm --validate config.cfg
```

**Issue: Missing required keys**
```
b Solution: Provide default configuration
./nlm --config --use-defaults
```

**Issue: Performance issues with large configs**
```
b Solution: Use optimized configuration access
// Use getOr() with defaults
int neurons = config->getOr<int>("brain.neuron_count", 1000);
```

### Debug Commands
```bash
# List all available configuration keys
./nlm --list-configs

# Validate configuration with detailed errors
./nlm --validate config.cfg --verbose

# Show configuration differences
./nlm --compare config1.cfg config2.cfg

# Check configuration performance
./nlm --benchmark performance
```

## Conclusion

The NLM configuration system provides a robust, flexible, and user-friendly way to configure brain simulations. It supports multiple formats, comprehensive validation, detailed error reporting, and performance monitoring. The system is designed to be both powerful for advanced users and accessible for beginners, with clear documentation and examples to guide users through the configuration process.

Key features:
- Multi-format support (simple, JSON, YAML)
- Schema validation with detailed error reporting
- Command-line interface with extensive options
- Performance monitoring and metrics
- Configuration comparison and difference analysis
- Template system for common configurations
- Comprehensive error handling and recovery
- Integration with main application