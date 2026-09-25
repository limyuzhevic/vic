# NLM (Neural Learning Machine) - Configuration System

## Overview

The NLM configuration system has been enhanced with comprehensive schema validation, type safety, and support for modern configuration formats (JSON/YAML). This document describes the available configuration options and how to use them.

## Configuration File Format

### JSON Format (Recommended)

The configuration system now supports JSON as the primary format. This provides:
- Type safety
- Rich validation
- Dependency checking
- Version support

Example JSON configuration:

```json
{
    "version": "2.0",
    "config": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000,
        "log_level": "INFO",
        "environment_name": "GridWorld"
    }
}
```

### YAML Format

For compatibility with existing tools, YAML format is also supported. Basic YAML:

```yaml
version: 2.0
config:
  random_seed: 42
  simulation_timestep: 0.001
  neuron_count: 1000
  log_level: INFO
  environment_name: GridWorld
```

## Default Configuration

The default configuration file is located at `configs/default.cfg` and includes:

### Random Seed Configuration

- `random_seed` (int64_t, default: 42): Random seed for reproducible experiments

### Simulation Configuration

- `simulation_timestep` (double, default: 0.001): Simulation timestep in seconds
- `max_simulation_steps` (int64_t, default: 10000): Maximum number of simulation steps
- `simulation_time_limit` (double, default: 0.0): Simulation time limit (0 = no limit)

### Brain Configuration

- `neuron_count` (int64_t, default: 1000): Number of neurons in the brain
- `region_count` (int64_t, default: 2): Number of brain regions
- `connection_probability` (double, default: 0.1): Connection probability for random connectivity

### Plasticity Settings

- `plasticity_learning_rate` (double, default: 0.01): Plasticity learning rate
- `stdp_ltp_weight` (double, default: 0.01): STDP LTP weight
- `stdp_ltd_weight` (double, default: 0.012): STDP LTD weight
- `stdp_time_constant` (double, default: 20.0): STDP time constant

### Development Settings

- `development_synaptogenesis_rate` (double, default: 0.001): Development synaptogenesis rate
- `development_pruning_rate` (double, default: 0.0001): Development pruning rate

### Neuromodulation Settings

- `dopamine_baseline` (double, default: 0.1): Dopamine baseline value

### Reward Settings

- `reward_discount_factor` (double, default: 0.99): Reward discount factor

### Environment Settings

- `environment_name` (string, default: "GridWorld"): Name of the environment
- `environment_width` (int64_t, default: 10): Environment width in cells
- `environment_height` (int64_t, default: 10): Environment height in cells

### Logging Configuration

- `log_level` (string, default: "INFO"): Logging level (DEBUG, INFO, WARN, ERROR, CRITICAL)
- `log_to_file` (bool, default: false): Whether to log to file
- `log_filename` (string, default: "nlm.log"): Log filename when log_to_file is true

### Visualization Settings

- `visualization_enabled` (bool, default: false): Whether visualization is enabled
- `visualization_update_rate` (double, default: 30.0): Visualization update rate in Hz

## Command Line Configuration

You can override configuration values using command line arguments:

### Key-Value Pairs

```bash
./nlm --random_seed=123 --neuron_count=500 --log_level=DEBUG
```

### Individual Keys

```bash
./nlm -random_seed 123 -neuron_count 500 -log_level DEBUG
```

## Configuration Validation

The enhanced configuration system includes comprehensive validation:

1. **Type Validation**: Each configuration value is validated against its expected type
2. **Range Validation**: Numeric values are validated for valid ranges
3. **Dependency Validation**: Some configurations require other values to be present
4. **Schema Validation**: All values must conform to the configuration schema

### Validation Examples

```cpp
// Configuration with validation
nlm::Config config;
config.set("random_seed", 42);
config.set("neuron_count", 1000);
config.set("simulation_timestep", 0.001);

// Validate configuration
if (config.has("simulation_timestep")) {
    double dt = config.get<double>("simulation_timestep").value();
    if (dt <= 0.0) {
        throw nlm::ConfigException("Invalid simulation timestep");
    }
}
```

## API Usage

### C++ API

```cpp
#include "nlm/Config.hpp"

namespace nlm {
    class Config {
    public:
        // Load from JSON file
        bool loadFromFile(const std::string& filepath);
        
        // Load from command line
        bool loadFromArgs(int argc, char** argv);
        
        // Get values with type safety
        std::optional<int64_t> get<int64_t>("random_seed");
        std::optional<double> get<double>("simulation_timestep");
        
        // Get values with defaults
        int64_t getOr<int64_t>("neuron_count", 1000);
        
        // Set values with validation
        void set(const std::string& key, int64_t value);
        void set(const std::string& key, double value);
        
        // Configuration schema access
        const ConfigSchema& getSchema() const;
        
        // Summary and debugging
        std::string summary() const;
    };
}
```

### Python API

```python
from pynlm import Config

# Load configuration
config = Config()
config.loadFromFile("config.json")

# Get values
seed = config.get("random_seed")
neuron_count = config.get("neuron_count")

# Set values
config.set("random_seed", 42)
config.set("neuron_count", 1000)

# Configuration schema
schema = config.getSchema()
print(schema.summary())
```

## Configuration Versioning

The configuration system supports versioning:

### Version 1.0 (Legacy)
- Simple key=value pairs
- No validation
- Limited type support

### Version 2.0 (Enhanced)
- JSON/YAML format support
- Comprehensive schema validation
- Type safety
- Dependency checking
- Rich error messages

The `configs/default.cfg` file is currently at version 2.0. You can specify configuration version when loading:

```cpp
Config config;
config.setVersion("2.0");
config.loadFromFile("config.json");
```

## Best Practices

1. **Use JSON format**: JSON provides better type safety and tooling support
2. **Validate configuration**: Always validate critical configuration values
3. **Use schema for defaults**: Let the schema system handle default values
4. **Check dependencies**: Ensure required configurations are present
5. **Version your configuration**: Specify configuration version for compatibility

## Error Handling

The configuration system provides comprehensive error reporting:

```cpp
try {
    config.loadFromFile("config.json");
} catch (const nlm::ConfigException& e) {
    std::cerr << "Configuration error: " << e.what() << std::endl;
    std::cerr << "Error type: " << static_cast<int>(e.getErrorType()) << std::endl;
}
```

## Testing

The configuration system includes comprehensive tests:

```bash
# Run configuration tests
./build/tests/test_config

# Test with sample configurations
./nlm --config=config.json
```

## Additional Resources

- Configuration schema documentation
- Command line reference
- Python API documentation
- Performance considerations for large configurations

For more information, refer to the NLM documentation website or the individual component documentation.
