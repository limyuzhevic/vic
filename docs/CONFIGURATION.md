# NLM Configuration

## Overview

This document provides comprehensive documentation for configuration management in the NLM project. It covers configuration file formats, parameter types, default values, and examples for different use cases. The configuration system is essential for controlling neural network behavior, simulation parameters, and system functionality.

## Configuration System Architecture

### Configuration Overview

The NLM configuration system provides a flexible and hierarchical approach to managing system parameters. It supports:

1. **Default Configuration**: Built-in sensible defaults for all system components
2. **File-based Configuration**: JSON configuration files for custom setups
3. **Runtime Configuration**: Programmatic configuration during execution
4. **Hierarchical Configuration**: Inheritance and overriding of configuration values
5. **Type Safety**: Strongly-typed configuration values with validation

### Configuration Components

#### 1. Core Configuration
- **Brain Configuration**: Neural network parameters
- **World Configuration**: Environment and simulation parameters
- **Agent Configuration**: Cognitive and behavioral parameters
- **Experiment Configuration**: Research and testing parameters
- **Visualization Configuration**: Display and output settings

#### 2. Configuration File Format
```json
{
    "nlm": {
        "version": "2.0",
        "simulation": {
            "max_steps": 1000,
            "timestep": 0.001,
            "random_seed": 42
        },
        "brain": {
            "neuron_count": 1000,
            "region_count": 4,
            "connection_probability": 0.1,
            "plasticity": {
                "stdp": {"enabled": true, "learning_rate": 0.001},
                "hebbian": {"enabled": true, "learning_rate": 0.01},
                "structural": {"enabled": true, "rate": 0.0001}
            }
        },
        "world": {
            "width": 20,
            "height": 20,
            "vision_range": 8,
            "enable_resources": true,
            "enable_hazards": true
        },
        "agent": {
            "enable_memory": true,
            "enable_prediction": true,
            "enable_cognition": true,
            "enable_development": true,
            "reward_mode": "prediction_error"
        },
        "visualization": {
            "enabled": true,
            "output_format": "png",
            "resolution": {"width": 1920, "height": 1080}
        }
    }
}
```

## Configuration File Types

### 1. JSON Configuration
**File Extension:** `.json`
**Use Case:** User customization and research experiments

#### JSON Schema
```json
{
    "$schema": "https://kilo.ai/nlm/config-schema.json",
    "nlm": {
        "simulation": {
            "type": "object",
            "properties": {
                "max_steps": {"type": "integer", "minimum": 1, "maximum": 1000000},
                "timestep": {"type": "number", "minimum": 0.0001, "maximum": 1.0},
                "random_seed": {"type": "integer", "minimum": 0},
                "enable_checkpoints": {"type": "boolean"},
                "checkpoint_interval": {"type": "integer", "minimum": 1}
            },
            "required": ["max_steps", "timestep"]
        },
        "brain": {
            "type": "object",
            "properties": {
                "neuron_count": {"type": "integer", "minimum": 10, "maximum": 100000},
                "region_count": {"type": "integer", "minimum": 1, "maximum": 100},
                "connection_probability": {"type": "number", "minimum": 0.0, "maximum": 1.0},
                "plasticity": {"$ref": "#/definitions/plasticity_config"}
            },
            "required": ["neuron_count", "region_count"]
        },
        "world": {
            "type": "object",
            "properties": {
                "width": {"type": "integer", "minimum": 1},
                "height": {"type": "integer", "minimum": 1},
                "vision_range": {"type": "integer", "minimum": 1},
                "enable_resources": {"type": "boolean"},
                "enable_hazards": {"type": "boolean"},
                "resource_difficulty": {"type": "number", "minimum": 0.0, "maximum": 10.0}
            },
            "required": ["width", "height", "vision_range"]
        },
        "agent": {
            "type": "object",
            "properties": {
                "enable_memory": {"type": "boolean"},
                "enable_prediction": {"type": "boolean"},
                "enable_cognition": {"type": "boolean"},
                "enable_development": {"type": "boolean"},
                "enable_curiosity": {"type": "boolean"},
                "reward_mode": {"type": "string", "enum": ["prediction_error", "simple", "surrogate"]}
            },
            "required": ["enable_memory", "enable_prediction", "enable_cognition"]
        }
    },
    "definitions": {
        "plasticity_config": {
            "type": "object",
            "properties": {
                "stdp": {
                    "type": "object",
                    "properties": {
                        "enabled": {"type": "boolean"},
                        "learning_rate": {"type": "number", "minimum": 0.0},
                        "tau_plus": {"type": "number", "minimum": 0.0},
                        "tau_minus": {"type": "number", "minimum": 0.0}
                    }
                },
                "hebbian": {
                    "type": "object",
                    "properties": {
                        "enabled": {"type": "boolean"},
                        "learning_rate": {"type": "number", "minimum": 0.0}
                    }
                },
                "structural": {
                    "type": "object",
                    "properties": {
                        "enabled": {"type": "boolean"},
                        "rate": {"type": "number", "minimum": 0.0}
                    }
                }
            }
        }
    }
}
```

### 2. TOML Configuration
**File Extension:** `.toml`
**Use Case:** Configuration management with hierarchical structure

#### TOML Example
```toml
[nlm.simulation]
max_steps = 5000
timestep = 0.001
random_seed = 12345
enable_checkpoints = true
checkpoint_interval = 100

[nlm.brain]
neuron_count = 2000
region_count = 6
connection_probability = 0.05

[nlm.brain.plasticity.stdp]
enabled = true
learning_rate = 0.001
tau_plus = 20.0
tau_minus = 20.0

[nlm.brain.plasticity.hebbian]
enabled = true
learning_rate = 0.01

[nlm.brain.plasticity.structural]
enabled = true
rate = 0.0005

[nlm.world]
width = 30
height = 30
vision_range = 12
enable_resources = true
enable_hazards = true
resource_difficulty = 1.5

[nlm.agent]
enable_memory = true
enable_prediction = true
enable_cognition = true
enable_development = true
enable_curiosity = true
reward_mode = "prediction_error"

[nlm.visualization]
enabled = true
output_format = "png"
resolution.width = 1920
resolution.height = 1080
```

### 3. YAML Configuration
**File Extension:** `.yaml` or `.yml`
**Use Case:** Documentation and collaborative configuration

#### YAML Example
```yaml
nlm:
  version: 2.0
  simulation:
    max_steps: 3000
    timestep: 0.001
    random_seed: 99
    enable_checkpoints: true
    checkpoint_interval: 200
  brain:
    neuron_count: 1500
    region_count: 5
    connection_probability: 0.08
    plasticity:
      stdp:
        enabled: true
        learning_rate: 0.001
        tau_plus: 15.0
        tau_minus: 15.0
      hebbian:
        enabled: true
        learning_rate: 0.008
      structural:
        enabled: true
        rate: 0.001
  world:
    width: 25
    height: 25
    vision_range: 10
    enable_resources: true
    enable_hazards: true
    resource_difficulty: 1.2
  agent:
    enable_memory: true
    enable_prediction: true
    enable_cognition: true
    enable_development: true
    enable_curiosity: true
    reward_mode: "prediction_error"
  visualization:
    enabled: true
    output_format: "png"
    resolution:
      width: 1280
      height: 720
```

## Configuration Parameters

### Simulation Parameters

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `nlm.simulation.max_steps` | Integer | 1000 | Maximum simulation steps | 1-1,000,000 |
| `nlm.simulation.timestep` | Float | 0.001 | Simulation timestep (seconds) | 0.0001-1.0 |
| `nlm.simulation.random_seed` | Integer | 0 | Random seed for reproducibility | 0-2^31-1 |
| `nlm.simulation.enable_checkpoints` | Boolean | false | Enable checkpoint saving | true/false |
| `nlm.simulation.checkpoint_interval` | Integer | 100 | Checkpoint saving interval | 1-10,000 |

### Brain Configuration

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `nlm.brain.neuron_count` | Integer | 1000 | Total number of neurons | 10-100,000 |
| `nlm.brain.region_count` | Integer | 4 | Number of neural regions | 1-100 |
| `nlm.brain.connection_probability` | Float | 0.1 | Connection density between neurons | 0.0-1.0 |
| `nlm.brain.plasticity.stdp.enabled` | Boolean | true | Enable spike-timing dependent plasticity | true/false |
| `nlm.brain.plasticity.stdp.learning_rate` | Float | 0.001 | STDP learning rate | 0.0-1.0 |
| `nlm.brain.plasticity.hebbian.enabled` | Boolean | true | Enable Hebbian plasticity | true/false |
| `nlm.brain.plasticity.hebbian.learning_rate` | Float | 0.01 | Hebbian learning rate | 0.0-1.0 |
| `nlm.brain.plasticity.structural.enabled` | Boolean | true | Enable structural plasticity | true/false |
| `nlm.brain.plasticity.structural.rate` | Float | 0.0001 | Structural plasticity rate | 0.0-0.01 |

### World Configuration

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `nlm.world.width` | Integer | 20 | World width in cells | 1-1000 |
| `nlm.world.height` | Integer | 20 | World height in cells | 1-1000 |
| `nlm.world.vision_range` | Integer | 8 | Sensory perception range | 1-100 |
| `nlm.world.enable_resources` | Boolean | true | Enable resource objects | true/false |
| `nlm.world.enable_hazards` | Boolean | true | Enable hazard objects | true/false |
| `nlm.world.resource_difficulty` | Float | 1.0 | Resource scarcity factor | 0.0-10.0 |

### Agent Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `nlm.agent.enable_memory` | Boolean | true | Enable memory systems |
| `nlm.agent.enable_prediction` | Boolean | true | Enable prediction systems |
| `nlm.agent.enable_cognition` | Boolean | true | Enable cognitive systems |
| `nlm.agent.enable_development` | Boolean | true | Enable developmental systems |
| `nlm.agent.enable_curiosity` | Boolean | true | Enable curiosity-driven exploration |
| `nlm.agent.reward_mode` | String | "prediction_error" | Reward calculation mode |

### Visualization Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `nlm.visualization.enabled` | Boolean | true | Enable visualization output |
| `nlm.visualization.output_format` | String | "png" | Output image format |
| `nlm.visualization.resolution.width` | Integer | 1920 | Output image width |
| `nlm.visualization.resolution.height` | Integer | 1080 | Output image height |

## Configuration Loading and Validation

### Configuration Loading Priority

1. **Command Line Arguments** (highest priority)
2. **Configuration File** (medium priority)
3. **Default Configuration** (lowest priority)

### Configuration Validation

The configuration system validates all parameters according to schema constraints:

```cpp
class ConfigValidator {
public:
    static bool validate(const json& config, std::vector<std::string>& errors);
    static json mergeConfigs(const json& default_config, const json& user_config);
    static json applyCommandLineArgs(const json& base_config, const std::vector<std::string>& args);
};
```

### Configuration Validation Examples

#### Valid Configuration
```json
{
    "nlm": {
        "brain": {
            "neuron_count": 500,
            "connection_probability": 0.15
        }
    }
}
```

#### Invalid Configurations

1. **Out of Range Values**
```json
{
    "nlm": {
        "brain": {
            "neuron_count": 200000
        }
    }
}
// Error: neuron_count exceeds maximum of 100,000
```

2. **Missing Required Parameters**
```json
{
    "nlm": {
        "brain": {
            "connection_probability": 0.1
        }
    }
}
// Error: neuron_count is required
```

3. **Invalid Data Types**
```json
{
    "nlm": {
        "brain": {
            "neuron_count": "five hundred"
        }
    }
}
// Error: neuron_count must be an integer
```

## Configuration API

### Core Configuration API

```cpp
class Config {
public:
    // Configuration loading
    static Config fromFile(const std::string& filepath);
    static Config fromJSON(const std::string& json_str);
    static Config fromCommandLine(int argc, char** argv);
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration access
    template<typename T>
    T get(const std::string& path) const;
    
    template<typename T>
    T get(const std::string& path, const T& default_value) const;
    
    bool has(const std::string& path) const;
    
    // Configuration modification
    void set(const std::string& path, const json& value);
    void merge(const Config& other);
    
    // Configuration export
    std::string toJSON() const;
    std::string toYAML() const;
    std::string toTOML() const;
    
    // Configuration utilities
    void save(const std::string& filepath) const;
    void load(const std::string& filepath);
    
    // Configuration cloning
    Config clone() const;
};
```

### Configuration Builder API

```cpp
class ConfigBuilder {
public:
    ConfigBuilder& set(const std::string& path, const json& value);
    ConfigBuilder& setBrain(const BrainConfig& config);
    ConfigBuilder& setWorld(const WorldConfig& config);
    ConfigBuilder& setAgent(const AgentConfig& config);
    ConfigBuilder& setSimulation(const SimulationConfig& config);
    
    Config build() const;
    
    // Presets
    static ConfigBuilder createDefault();
    static ConfigBuilder createResearch();
    static ConfigBuilder createProduction();
    static ConfigBuilder createDebug();
};
```

### Configuration Templates

#### Research Configuration Template
```json
{
    "nlm": {
        "simulation": {
            "max_steps": 5000,
            "timestep": 0.001,
            "random_seed": 12345
        },
        "brain": {
            "neuron_count": 2000,
            "region_count": 8,
            "connection_probability": 0.08
        },
        "world": {
            "width": 30,
            "height": 30,
            "vision_range": 12,
            "enable_resources": true,
            "enable_hazards": true
        },
        "agent": {
            "enable_memory": true,
            "enable_prediction": true,
            "enable_cognition": true,
            "enable_development": true,
            "enable_curiosity": true
        }
    }
}
```

#### Production Configuration Template
```json
{
    "nlm": {
        "simulation": {
            "max_steps": 1000,
            "timestep": 0.001,
            "enable_checkpoints": true
        },
        "brain": {
            "neuron_count": 1000,
            "region_count": 4,
            "connection_probability": 0.1
        },
        "world": {
            "width": 20,
            "height": 20,
            "vision_range": 8,
            "enable_resources": true
        },
        "agent": {
            "enable_memory": true,
            "enable_prediction": true,
            "enable_cognition": true,
            "reward_mode": "surrogate"
        }
    }
}
```

#### Debug Configuration Template
```json
{
    "nlm": {
        "simulation": {
            "max_steps": 100,
            "timestep": 0.01,
            "random_seed": 42
        },
        "brain": {
            "neuron_count": 100,
            "region_count": 2,
            "connection_probability": 0.2
        },
        "world": {
            "width": 10,
            "height": 10,
            "vision_range": 4,
            "enable_resources": true,
            "enable_hazards": true
        },
        "agent": {
            "enable_memory": false,
            "enable_prediction": false,
            "enable_cognition": false,
            "enable_development": false
        }
    }
}
```

## Configuration Command Line Interface

### Command Line Arguments

```bash
# Help
./nlm --help

# Configuration file
./nlm --config my_config.json

# Override parameters
./nlm --neuron-count 500 --world-width 30 --world-height 30

# Save configuration
./nlm --save-config my_config.json

# Load configuration
./nlm --load-config my_config.json

# Print current configuration
./nlm --print-config

# Validate configuration
./nlm --validate my_config.json
```

### Configuration Command Line Examples

```bash
# Basic configuration
./nlm --neuron-count 1000 --world-width 20 --world-height 20 --steps 1000

# Research configuration with visualization
./nlm --config research_config.json --enable-visualization --output-dir=./results/viz

# Production configuration with checkpointing
./nlm --load production_config.json --enable-checkpoints --checkpoint-interval 500

# Custom configuration with parameters
./nlm \
    --neuron-count 2000 \
    --region-count 6 \
    --connection-probability 0.08 \
    --world-width 30 \
    --world-height 30 \
    --steps 5000 \
    --random-seed 12345
```

## Configuration Best Practices

### 1. Configuration Management

1. **Version Control**: Keep configuration files in version control
2. **Environment Variables**: Use environment variables for system-specific settings
3. **Secrets Management**: Use secure storage for sensitive configuration values
4. **Configuration Validation**: Validate configuration before use

### 2. Configuration Testing

1. **Default Configuration**: Ensure default configuration is valid
2. **Custom Configuration**: Test custom configuration thoroughly
3. **Edge Cases**: Test boundary values and error conditions
4. **Integration Testing**: Test configuration with different system components

### 3. Configuration Documentation

1. **Configuration Schema**: Document configuration schema with examples
2. **Parameter Descriptions**: Provide clear descriptions for all parameters
3. **Default Values**: Document default values and their rationale
4. **Validation Rules**: Document validation rules and constraints

## Configuration Tools

### Configuration Editor

```bash
# Install configuration editor
pip install nlm-config-editor

# Launch configuration editor
nlm-config-editor

# Import/export configuration
nlm-config-editor --import my_config.json
nlm-config-editor --export my_config.json
```

### Configuration Validator

```bash
# Install configuration validator
pip install nlm-config-validator

# Validate configuration file
nlm-config-validator validate my_config.json

# Validate all configuration files in directory
nlm-config-validator validate-dir ./configs
```

### Configuration Generator

```bash
# Install configuration generator
pip install nlm-config-generator

# Generate configuration from template
nlm-config-generator generate --template research --output research_config.json

# Generate configuration for specific platform
nlm-config-generator generate --platform production --output prod_config.json
```

## Configuration Summary

The NLM configuration system provides a comprehensive approach to managing system parameters through:

1. **Multiple File Formats**: JSON, TOML, and YAML support
2. **Hierarchical Configuration**: Inheritance and overriding capabilities
3. **Type Safety**: Strongly-typed parameters with validation
4. **Extensive API**: Rich set of configuration methods and utilities
5. **Validation System**: Comprehensive validation with detailed error messages
6. **Command Line Integration**: Seamless integration with command line interface
7. **Configuration Templates**: Presets for common use cases
8. **Configuration Tools**: Editor, validator, and generator tools

This configuration system enables users to easily customize NLM behavior for research, production, and development environments while ensuring type safety and validation.
