# NLM Configuration Schema

## Configuration Overview

NLM uses a structured configuration system to define brain parameters, simulation settings, and system behavior. The configuration supports multiple formats and provides validation for all parameters.

## Configuration Structure

### Configuration Parameters

| Parameter | Type | Default | Description | Range | Required |
|-----------|------|---------|-------------|-------|----------|
| `random_seed` | integer | 42 | Random seed for reproducible experiments | 0-2^31-1 | No |
| `simulation_timestep` | float | 0.001 | Simulation timestep in seconds | 0.0001-1.0 | Yes |
| `neuron_count` | integer | 1000 | Number of neurons in the brain | 1-100000 | Yes |
| `region_count` | integer | 2 | Number of brain regions | 1-100 | Yes |
| `connection_probability` | float | 0.1 | Connection probability for random connectivity | 0.0-1.0 | Yes |
| `stdp_ltp_weight` | float | 0.01 | STDP Long-Term Potentiation weight | 0.0-1.0 | No |
| `stdp_ltd_weight` | float | 0.012 | STDP Long-Term Depression weight | 0.0-1.0 | No |
| `stdp_time_constant` | float | 20.0 | STDP time constant in ms | 1.0-1000.0 | No |
| `development_synaptogenesis_rate` | float | 0.001 | Synaptogenesis rate during development | 0.0-0.1 | No |
| `development_pruning_rate` | float | 0.0001 | Pruning rate during development | 0.0-0.01 | No |
| `dopamine_baseline` | float | 0.1 | Baseline dopamine level | 0.0-1.0 | No |
| `reward_discount_factor` | float | 0.99 | Reward discount factor | 0.0-1.0 | No |
| `environment_name` | string | "GridWorld" | Environment type | - | No |
| `environment_width` | integer | 10 | Environment width | 1-1000 | No |
| `environment_height` | integer | 10 | Environment height | 1-1000 | No |
| `log_level` | string | "INFO" | Logging level | DEBUG, INFO, WARN, ERROR, CRITICAL | No |
| `log_to_file` | boolean | false | Enable file logging | true, false | No |
| `log_filename` | string | "nlm.log" | Log file name | - | No |
| `max_simulation_steps` | integer | 10000 | Maximum simulation steps | 1-1000000 | Yes |
| `simulation_time_limit` | float | 0.0 | Time limit in seconds (0 = unlimited) | 0.0-1000000.0 | Yes |
| `visualization_enabled` | boolean | false | Enable visualization | true, false | No |
| `visualization_update_rate` | float | 30.0 | Visualization update rate in Hz | 1.0-1000.0 | No |

### Configuration Sources (Priority Order)

1. **Command Line Arguments** (Highest priority)
2. **Configuration File** (Medium priority)
3. **Programmatic API** (Lowest priority)

## Configuration File Formats

### Key=Value Format (nlm.cfg)

```
# Example configuration file
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
log_level = INFO
log_to_file = false
environment_name = GridWorld
environment_width = 10
environment_height = 10
max_simulation_steps = 10000
```

### JSON Format (nlm.json)

```json
{
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "neuron_count": 1000,
  "region_count": 2,
  "connection_probability": 0.1,
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.012,
  "stdp_time_constant": 20.0,
  "development_synaptogenesis_rate": 0.001,
  "development_pruning_rate": 0.0001,
  "dopamine_baseline": 0.1,
  "reward_discount_factor": 0.99,
  "environment_name": "GridWorld",
  "environment_width": 10,
  "environment_height": 10,
  "log_level": "INFO",
  "log_to_file": false,
  "log_filename": "nlm.log",
  "max_simulation_steps": 10000,
  "simulation_time_limit": 0.0,
  "visualization_enabled": false,
  "visualization_update_rate": 30.0
}
```

### YAML Format (nlm.yaml)

```yaml
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
region_count: 2
connection_probability: 0.1
stdp_ltp_weight: 0.01
stdp_ltd_weight: 0.012
stdp_time_constant: 20.0
development_synaptogenesis_rate: 0.001
development_pruning_rate: 0.0001
dopamine_baseline: 0.1
reward_discount_factor: 0.99
environment_name: GridWorld
environment_width: 10
environment_height: 10
log_level: INFO
log_to_file: false
log_filename: nlm.log
max_simulation_steps: 10000
simulation_time_limit: 0.0
visualization_enabled: false
visualization_update_rate: 30.0
```

## Configuration Validation

### Parameter Validation Rules

1. **Positive Values**: All integer parameters must be >= 1
2. **Range Validation**: Float parameters must be within specified ranges
3. **Enum Validation**: String parameters must match allowed values
4. **Dependency Validation**: Some parameters require others to be set

### Validation Examples

```cpp
bool validateConfig(const Config& config) {
    // Check required parameters
    if (!config.has("simulation_timestep")) return false;
    if (!config.has("neuron_count")) return false;
    if (!config.has("max_simulation_steps")) return false;
    
    // Validate ranges
    if (config.getOr<float>("simulation_timestep", 0.0) <= 0.0) return false;
    if (config.getOr<int>("neuron_count", 1) < 1) return false;
    if (config.getOr<int>("max_simulation_steps", 0) < 1) return false;
    
    // Validate enum values
    std::string logLevel = config.getOr<std::string>("log_level", "");
    if (logLevel != "DEBUG" && logLevel != "INFO" && 
        logLevel != "WARN" && logLevel != "ERROR" && 
        logLevel != "CRITICAL") return false;
    
    return true;
}
```

## Configuration Examples

### Research Configuration

```cpp
Config createResearchConfig() {
    Config config;
    
    // Set research-specific parameters
    config.set("random_seed", 12345);
    config.set("neuron_count", 5000);
    config.set("region_count", 5);
    config.set("simulation_timestep", 0.0005);
    
    // Enable development
    config.set("development_synaptogenesis_rate", 0.002);
    config.set("development_pruning_rate", 0.0002);
    
    // Configure neuromodulation
    config.set("dopamine_baseline", 0.2);
    
    // Set up environment for testing
    config.set("environment_name", "GridWorld");
    config.set("environment_width", 20);
    config.set("environment_height", 20);
    
    // Enable detailed logging
    config.set("log_level", "INFO");
    config.set("log_to_file", true);
    config.set("log_filename", "research_run.log");
    
    // Set experiment duration
    config.set("max_simulation_steps", 50000);
    
    return config;
}
```

### Production Configuration

```cpp
Config createProductionConfig() {
    Config config;
    
    // Optimized for performance
    config.set("random_seed", 42);
    config.set("neuron_count", 10000);
    config.set("region_count", 3);
    config.set("simulation_timestep", 0.001);
    
    // Disable development for stable performance
    config.set("development_synaptogenesis_rate", 0.0);
    config.set("development_pruning_rate", 0.0);
    
    // Basic neuromodulation
    config.set("dopamine_baseline", 0.1);
    
    // Production environment
    config.set("environment_name", "SimpleWorld");
    config.set("environment_width", 16);
    config.set("environment_height", 16);
    
    // Minimal logging for production
    config.set("log_level", "WARN");
    config.set("log_to_file", true);
    config.set("log_filename", "production.log");
    
    // Standard experiment duration
    config.set("max_simulation_steps", 10000);
    
    return config;
}
```

### Rapid Prototyping Configuration

```cpp
Config createRapidPrototypeConfig() {
    Config config;
    
    // Small scale for quick testing
    config.set("random_seed", 999);
    config.set("neuron_count", 100);
    config.set("region_count", 1);
    config.set("simulation_timestep", 0.001);
    
    // Enable rapid development
    config.set("development_synaptogenesis_rate", 0.01);
    config.set("development_pruning_rate", 0.001);
    
    // Strong neuromodulation for exploration
    config.set("dopamine_baseline", 0.5);
    
    // Simple environment for debugging
    config.set("environment_name", "GridWorld");
    config.set("environment_width", 8);
    config.set("environment_height", 8);
    
    // Detailed logging for debugging
    config.set("log_level", "DEBUG");
    config.set("log_to_file", true);
    config.set("log_filename", "debug.log");
    
    // Short experiment for rapid iteration
    config.set("max_simulation_steps", 1000);
    
    return config;
}
```

## Command Line Interface

### Usage Examples

```bash
# Basic usage with configuration file
./nlm --config experiment.cfg

# Override specific parameters
./nlm --config experiment.cfg --neuron_count=2000 --simulation_timestep=0.0005

# Set random seed
./nlm --random_seed=12345 --max_simulation_steps=20000

# Environment configuration
./nlm --environment_name=GridWorld --environment_width=30 --environment_height=30

# Logging configuration
./nlm --log_level=DEBUG --log_to_file=true --log_filename=detailed.log

# Multiple parameter overrides
./nlm --config experiment.cfg \
     --neuron_count=5000 \
     --region_count=3 \
     --connection_probability=0.15 \
     --stdp_ltp_weight=0.02 \
     --stdp_ltd_weight=0.018
```

### Help System

```bash
./nlm --help

Available options:
  --help                      Show this help message
  --version                   Show version information
  --config <file>             Load configuration from file
  --random_seed <value>       Set random seed
  --simulation_timestep <val> Set simulation timestep
  --neuron_count <value>      Set number of neurons
  --region_count <value>      Set number of regions
  --connection_probability <val> Set connection probability
  --log_level <level>         Set logging level (DEBUG, INFO, WARN, ERROR, CRITICAL)
  --log_to_file <bool>         Enable/disable file logging
  --log_filename <file>       Set log filename
  --environment_name <name>   Set environment type
  --environment_width <value> Set environment width
  --environment_height <value> Set environment height
  --max_simulation_steps <val> Set maximum simulation steps

Configuration file formats supported:
  nlm.cfg    - Key=value format
  nlm.json   - JSON format
  nlm.yaml   - YAML format
```

## Configuration Validation Script

### Example validation script

```bash
#!/bin/bash

# Validate configuration file
validate_config() {
    local config_file="$1"
    
    if [[ ! -f "$config_file" ]]; then
        echo "Error: Configuration file not found: $config_file"
        return 1
    fi
    
    # Check file format
    if [[ "$config_file" == *.json ]]; then
        # Validate JSON format
        if ! jq empty "$config_file" 2>/dev/null; then
            echo "Error: Invalid JSON format in $config_file"
            return 1
        fi
    elif [[ "$config_file" == *.yaml || "$config_file" == *.yml ]]; then
        # Validate YAML format
        if ! yamllint "$config_file" 2>/dev/null; then
            echo "Error: Invalid YAML format in $config_file"
            return 1
        fi
    elif [[ "$config_file" == *.cfg ]]; then
        # Validate key=value format
        while IFS='=' read -r key value; do
            # Basic validation
            if [[ -z "$key" ]]; then
                echo "Error: Empty key in $config_file"
                return 1
            fi
        done < "$config_file"
    else
        echo "Error: Unsupported configuration file format: $config_file"
        return 1
    fi
    
    echo "Configuration validation passed: $config_file"
}

# Usage example
validate_config "experiment.cfg"
```

## Configuration Templates

### Template: Learning Experiment

```cfg
# Learning experiment configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 2000
region_count = 3
connection_probability = 0.15

# Enable development for learning
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Configure neuromodulation
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018
dopamine_baseline = 0.1

# Environment setup
environment_name = GridWorld
environment_width = 20
environment_height = 20

# Logging for analysis
log_level = INFO
log_to_file = true
log_filename = learning_experiment.log

# Extended learning session
max_simulation_steps = 50000
```

### Template: Damage Recovery Experiment

```cfg
# Damage recovery experiment configuration
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 3000
region_count = 4
connection_probability = 0.1

# Strong development for recovery
development_synaptogenesis_rate = 0.005
development_pruning_rate = 0.001

# Enhanced neuromodulation for recovery
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.022
dopamine_baseline = 0.3

# Complex environment for recovery testing
environment_name = GridWorld
environment_width = 30
environment_height = 30

# Detailed logging for analysis
log_level = DEBUG
log_to_file = true
log_filename = damage_recovery.log

# Long recovery session
max_simulation_steps = 100000
```

## Configuration Best Practices

### 1. Parameter Naming
- Use descriptive parameter names
- Use snake_case for multi-word parameters
- Avoid abbreviations unless well-documented

### 2. Parameter Ranges
- Validate all parameters before use
- Provide sensible defaults
- Document parameter ranges clearly

### 3. Configuration Sources
- Command line arguments override configuration files
- Programmatic API has lowest priority
- Document the override hierarchy

### 4. File Naming
- Use consistent naming conventions
- Include file extensions
- Document supported formats

### 5. Error Handling
- Provide clear error messages
- Validate configuration before use
- Log configuration warnings

### 6. Versioning
- Document configuration schema versions
- Handle backward compatibility
- Support configuration migration