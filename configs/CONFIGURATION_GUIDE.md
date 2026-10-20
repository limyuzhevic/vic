# NLM Configuration System Documentation

## Overview

The NLM (Neural Learning Machine) configuration system provides flexible, multi-format configuration support for the neural network simulation framework. It supports three main formats:

1. **Simple key=value format** (backward compatible)
2. **JSON format** (structured, programmatic)
3. **YAML format** (human-readable, nested)

## Configuration File Formats

### 1. Simple key=value Format

```ini
# configs/default.cfg
brain.neuron_count = 1000
brain.region_count = 1
brain.connection_probability = 0.1
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
simulation.timestep = 0.001
```

**Pros:**
- Backward compatible with existing configurations
- Simple and easy to edit
- Works with minimal dependencies

**Cons:**
- Limited data types (only basic types)
- No nested structures
- Manual file creation required

### 2. JSON Format

```json
// configs/nlm_config.json
{
  "brain": {
    "neuron_count": 2000,
    "region_count": 2,
    "connection_probability": 0.08,
    "v_thresh": -50.0,
    "v_rest": -70.0,
    "v_reset": -75.0
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "enable": true,
      "learning_rate": 0.0005
    }
  },
  "simulation": {
    "timestep": 0.001,
    "max_steps": 10000,
    "random_seed": 42
  }
}
```

**Pros:**
- Full support for nested structures
- Rich data types (arrays, numbers, strings, booleans)
- Easy parsing with standard libraries
- Ideal for programmatic access

**Cons:**nformation processing and API interactions with comprehensive management capabilities.

### 3. YAML Format

```yaml
# configs/nlm_config.yaml
brain:
  neuron_count: 1000
  region_count: 1
  connection_probability: 0.1
  v_thresh: -50.0
  v_rest: -70.0
  v_reset: -75.0

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
  hebbian:
    enable: true
    learning_rate: 0.0005

simulation:
  timestep: 0.001
  max_steps: 10000
  random_seed: 42

environment:
  simple_world:
    width: 20
    height: 20
    vision_width: 8
    vision_height: 8
    max_energy: 100.0
    agent_start_x: 10.0
    agent_start_y: 10.0

metadata:
  version: "2.0"
  description: "NLM Neural Learning Machine Configuration"
  author: "NLM Research Team"
tags:
  - production
  - integration
  - research
```

**Pros:**
- Human-readable and well-structured
- Excellent for configuration files
- Supports complex nested structures
- Great for documentation

**Cons:**
- Requires additional parsing libraries
- Slightly more complex than JSON for some use cases

## File Format Detection

The configuration system automatically detects file formats:

1. **JSON Detection**: Files with `.json` extension or content that starts with `{` and contains valid JSON syntax
2. **YAML Detection**: Files with `.yaml` or `.yml` extension, or content that contains valid YAML syntax
3. **Simple Config Detection**: Files with `.cfg` extension or content with `key=value` pairs

The system will attempt to parse in order: JSON → YAML → Simple Config, providing clear error messages for malformed files.

## Configuration Access API

### C++ API Examples

#### Basic Access
```cpp
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    nlm::Config config;
    
    // Load from JSON file
    if (config.loadFromFile("configs/nlm_config.json")) {
        std::cout << "Loaded JSON configuration successfully!" << std::endl;
    }
    
    // Load from YAML file
    if (config.loadFromFile("configs/nlm_config.yaml")) {
        std::cout << "Loaded YAML configuration successfully!" << std::endl;
    }
    
    // Load from simple config file
    if (config.loadFromFile("configs/default.cfg")) {
        std::cout << "Loaded simple configuration successfully!" << std::endl;
    }
    
    // Access configuration values
    auto neuronCount = config.getOr<int>("brain.neuron_count", 1000);
    auto connectionProb = config.getOr<double>("brain.connection_probability", 0.1);
    auto stdpEnabled = config.getOr<bool>("plasticity.stdp.enable", true);
    
    std::cout << "Neuron count: " << neuronCount << std::endl;
    std::cout << "Connection probability: " << connectionProb << std::endl;
    std::cout << "STDP enabled: " << (stddpEnabled ? "Yes" : "No") << std::endl;
    
    // Set configuration values
    config.set("brain.neuron_count", 2000);
    config.set("simulation.max_steps", 5000);
    
    return 0;
}
```

#### Nested Configuration Access
```cpp
// Access nested values using dot notation
auto brainSection = config.getOr<std::string>("brain", "default");
auto plasticitySection = config.getOr<std::string>("plasticity", "default");

// Check if specific configuration exists
if (config.has("brain.neuron_count")) {
    std::cout << "Neuron count is configured!" << std::endl;
}

// Get all configuration keys
auto allKeys = config.getKeys();
for (const auto& key : allKeys) {
    std::cout << "Key: " << key << std::endl;
}
```

### Python API Examples

#### Basic Usage
```python
import pynlm

# Create configuration from JSON file
config = pynlm.createDefaultConfig()
config.loadFromFile("configs/nlm_config.json")

# Access configuration values
neuron_count = config.get("brain.neuron_count", 1000)
connection_prob = config.get("brain.connection_probability", 0.1)
stdp_enabled = config.get("plasticity.stdp.enable", True)

print(f"Neuron count: {neuron_count}")
print(f"Connection probability: {connection_prob}")
print(f"STDP enabled: {stdp_enabled}")

# Create brain with configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Command Line Configuration
```python
import pynlm

# Create configuration and override with command line
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Override default

# You can also load from command line arguments
# config.loadFromArgs(len(sys.argv), sys.argv)
```

## Configuration Schemas

The configuration system supports optional schema validation to ensure configuration consistency:

```cpp
// Create and configure a schema
nlm::ConfigSchema schema;

// Add properties with validation
nlm::ConfigSchema::ConfigProperty brainNeuronCountProp {
    "brain.neuron_count",
    "int",
    1000,  // default value
    [](const std::string& key, const nlm::ConfigValue& value) {
        auto val = std::get<int>(value);
        return val > 0 && val <= 10000;  // custom validator
    },
    "Number of neurons in the brain"
};

schema.addProperty(brainNeuronCountProp);

// Apply schema to configuration
config.setSchema(schema);
```

## Configuration Best Practices

### 1. File Organization

- Store configuration files in the `configs/` directory
- Use descriptive filenames: `production.cfg`, `development.cfg`, `research.cfg`
- Keep environment-specific configurations separate

### 2. Configuration Structure

#### Simple Configuration (configs/default.cfg)
```ini
# Use for quick prototyping
brain.neuron_count = 500
simulation.timestep = 0.001
```

#### Development Configuration (configs/development.cfg)
```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 2
  },
  "simulation": {
    "timestep": 0.001,
    "max_steps": 5000
  },
  "environment": {
    "simple_world": {
      "width": 50,
      "height": 50
    }
  },
  "metadata": {
    "version": "dev",
    "environment": "development"
  }
}
```

#### Production Configuration (configs/production.cfg)
```yaml
brain:
  neuron_count: 10000
  region_count: 4
  connection_probability: 0.05

simulation:
  timestep: 0.001
  max_steps: 100000
  checkpoint_interval: 10000
  enable_replay: true

environment:
  simple_world:
    width: 100
    height: 100
    vision_width: 16
    vision_height: 16

metadata:
  version: "prod"
  environment: "production"
  backup_strategy: "daily"
  monitoring: "enabled"
```

### 3. Configuration Validation

#### Runtime Validation
```cpp
bool validateConfig(const nlm::Config& config) {
    // Check required values
    if (!config.has("brain.neuron_count")) {
        throw nlm::ConfigValidationError("Missing brain.neuron_count");
    }
    
    auto neuronCount = config.getOr<int>("brain.neuron_count", 0);
    if (neuronCount <= 0) {
        throw nlm::ConfigValidationError("brain.neuron_count must be positive");
    }
    
    // Check for valid range
    auto timestep = config.getOr<double>("simulation.timestep", 0.001);
    if (timestep <= 0.0 || timestep > 1.0) {
        throw nlm::ConfigValidationError("simulation.timestep must be in range (0, 1]");
    }
    
    return true;
}
```

### 4. Configuration Management

#### Configuration Templates
```cpp
// Configuration template for different experiment types
struct ConfigTemplates {
    static nlm::Config getPhase3Template() {
        nlm::Config config;
        
        // Phase 3: Real neural computation
        config.set("brain.neuron_count", 500);
        config.set("brain.region_count", 1);
        config.set("brain.connection_probability", 0.15);
        config.set("plasticity.stdp.enable", true);
        config.set("plasticity.hebbian.enable", true);
        
        config.set("simulation.timestep", 0.001);
        config.set("simulation.max_steps", 5000);
        
        return config;
    }
    
    static nlm::Config getPhase4Template() {
        nlm::Config config;
        
        // Phase 4: Emerging cognition
        config.set("brain.neuron_count", 800);
        config.set("brain.region_count", 2);
        config.set("brain.connection_probability", 0.12);
        
        // Enable all cognitive systems
        config.set("cognition.attention.competition_strength", 1.0);
        config.set("cognition.planner.planning_horizon", 5);
        config.set("cognition.concept_formation.stability_threshold", 0.7);
        
        // Memory systems
        config.set("memory.working_memory.capacity", 200);
        config.set("memory.episodic_memory.max_episodes", 500);
        
        config.set("simulation.timestep", 0.001);
        config.set("simulation.max_steps", 8000);
        
        return config;
    }
};
```

## Error Handling

The configuration system provides detailed error messages:

### Common Error Scenarios

1. **File Not Found**
   ```
   Error: Config file not found: "configs/missing.cfg"
   ```

2. **Malformed JSON**
   ```
   Error: Invalid JSON in config file "configs/bad.json"
   Line 5: Expected property name or '}' in line 5, column 8
   ```

3. **Malformed YAML**
   ```
   Error: Invalid YAML in config file "configs/bad.yaml"
   Line 8: Mapping key already defined at line 8, column 2
   ```

4. **Invalid Configuration Values**
   ```
   Error: Configuration validation failed for "brain.neuron_count"
   Value must be between 1 and 10000, got: 50000
   ```

5. **Schema Validation Failure**
   ```
   Error: Schema validation failed for key "plasticity.stdp.learning_rate"
   Value must be between 0.0 and 1.0, got: 1.5
   ```

## Performance Considerations

### Large Configuration Files
- **JSON**: Use for large files with many nested properties
- **YAML**: Use for human-readable configuration with good formatting
- **Simple Config**: Use for small, simple configurations

### Memory Usage
- The configuration system caches loaded configurations in memory
- For very large configuration files, consider loading and processing in chunks
- Use `config.clear()` to free memory when configurations are no longer needed

### File I/O
- Configuration files are read once and cached
- For dynamic configuration changes, use runtime `set()` commands
- Consider using configuration templates for common setups

## Migration Guide

### From Simple Config to JSON/YAML

If you're migrating from simple key=value format:

1. **Manual Conversion**: Use the provided templates as starting points
2. **Tool-Assisted**: Write a simple script to convert your configs
3. **Gradual Migration**: Start with complex configurations first

#### Example Migration Script
```python
import json
import yaml

def convert_simple_config_to_json(simple_path, json_path):
    """Convert simple key=value config to JSON"""
    config = {}
    
    with open(simple_path, 'r') as f:
        for line in f:
            line = line.strip()
            if line and not line.startswith('#') and '=' in line:
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip()
                
                # Convert value to appropriate type
                if value.lower() == 'true':
                    value = True
                elif value.lower() == 'false':
                    value = False
                elif value.isdigit():
                    value = int(value)
                else:
                    try:
                        value = float(value)
                    except ValueError:
                        pass  # Keep as string
                
                # Handle nested keys
                keys = key.split('.')
                current = config
                for k in keys[:-1]:
                    if k not in current:
                        current[k] = {}
                    current = current[k]
                current[keys[-1]] = value
    
    with open(json_path, 'w') as f:
        json.dump(config, f, indent=2)
    
    print(f"Converted {simple_path} to {json_path}")

# Usage
convert_simple_config_to_json("default.cfg", "nlm_config.json")
```

### Troubleshooting

#### Common Issues and Solutions

1. **"File format not detected"**
   - **Solution**: Ensure the file contains valid JSON/YAML or key=value pairs
   - **Check**: File syntax and extension match content

2. **"Configuration key not found"**
   - **Solution**: Check the exact key name and format
   - **Tip**: Use `config.getKeys()` to list all available keys

3. **"JSON parsing failed"**
   - **Solution**: Fix the JSON syntax (missing braces, commas, etc.)
   - **Tip**: Use a JSON validator for large configurations

4. **"YAML parsing failed"**
   - **Solution**: Fix YAML syntax (indentation, formatting)
   - **Tip**: YAML is sensitive to indentation and special characters

5. **"Schema validation failed"**
   - **Solution**: Check validation rules and configuration values
   - **Tip**: Temporarily disable schema validation for testing

## Advanced Configuration Features

### Configuration Templates

The configuration system supports template inheritance and overriding:

```cpp
// Base template
nlm::Config baseTemplate;
baseTemplate.set("brain.neuron_count", 1000);
baseTemplate.set("simulation.timestep", 0.001);

// Derived template with overrides
nlm::Config derivedTemplate = baseTemplate;
derivedTemplate.set("brain.neuron_count", 2000); // Override
derivedTemplate.set("plasticity.stdp.enable", true); // Add
```

### Configuration Validation Rules

Create custom validation rules for critical parameters:

```cpp
auto validateNeuronCount = [](const std::string&, const nlm::ConfigValue& value) {
    int count = std::get<int>(value);
    return count >= 10 && count <= 100000;
};

auto validateTimestep = [](const std::string&, const nlm::ConfigValue& value) {
    double timestep = std::get<double>(value);
    return timestep > 0.0 && timestep <= 1.0;
};

auto validateConnectionProb = [](const std::string&, const nlm::ConfigValue& value) {
    double prob = std::get<double>(value);
    return prob >= 0.0 && prob <= 1.0;
};
```

### Configuration Export/Import

Export configurations to different formats:

```cpp
// Export to JSON string
std::string jsonString = config.saveToJsonString();

// Export to YAML string
std::string yamlString = config.saveToYamlString();

// Export to simple config format
std::string simpleString = config.saveToSimpleString();
```

## Conclusion

The NLM configuration system provides:

1. **Multiple format support** for different use cases
2. **Automatic format detection** for ease of use
3. **Backward compatibility** with existing configurations
4. **Schema validation** for configuration consistency
5. **Comprehensive error handling** with clear messages
6. **Flexible API** for programmatic access

This system ensures that you can configure your neural network simulations with confidence, whether you're developing, testing, or running production systems.

## See Also

- `configs/default.cfg` - Simple configuration example
- `configs/nlm_config.json` - JSON configuration example
- `configs/nlm_config.yaml` - YAML configuration example
- `Config.hpp` - Configuration API documentation
- `Config.cpp` - Configuration implementation details

## References

1. [JSON Specification](https://www.json.org/)
2. [YAML 1.2 Specification](https://yaml.org/spec/1.2/spec.html)
3. [NLM Documentation](https://nlm.readthedocs.io)
4. [Python Bindings API](https://pynlm.readthedocs.io)