# NLM Configuration Documentation
# ================================

This document provides comprehensive documentation for NLM (Neural Learning Machine) configuration templates and files. It covers configuration file formats, parameter descriptions, template selection, and configuration management.

## Overview

The NLM configuration system provides several pre-configured templates for different use cases and performance characteristics. These templates are designed to help users quickly get started with NLM simulations while providing options for different requirements such as performance, development, memory usage, and experimentation.

## Configuration Templates

NLM provides six configuration templates, each optimized for specific use cases:

### 1. default.cfg
**Purpose**: Balanced configuration for general use
**Use Cases**: General-purpose simulations, standard testing
**Key Characteristics**:
- Balanced settings
- Comprehensive feature set
- Reference configuration for customization

### 2. high_perf.cfg
**Purpose**: Optimized for speed and efficiency
**Use Cases**: Production environments, benchmarking, large-scale simulations
**Key Characteristics**:
- Large brain (2000 neurons)
- High connectivity and plasticity
- Fast simulation (2000 max steps)

### 3. development.cfg
**Purpose**: Optimized for learning and development
**Use Cases**: Research, development, studying developmental processes
**Key Characteristics**:
- Moderate brain size (1500 neurons)
- Moderate connectivity (0.12)
- Aggressive plasticity with development tracking
- Extended simulation time (5000 steps)

### 4. lightweight.cfg
**Purpose**: Optimized for minimal resource usage
**Use Cases**: Quick testing, resource-constrained environments, prototyping
**Key Characteristics**:
- Small brain (300 neurons)
- Minimal connectivity (0.05)
- Conservative plasticity
- Quick simulation (500 steps)

### 5. experimental.cfg
**Purpose**: Designed for testing specific hypotheses
**Use Cases**: Scientific experiments, parameter testing, novelty studies
**Key Characteristics**:
- Medium brain size (1000 neurons)
- Extreme parameter values
- Detailed recording and logging
- Extended tracking for experimental analysis

### 6. benchmark.cfg
**Purpose**: Designed for performance testing and comparison
**Use Cases**: Performance benchmarking, optimization studies, comparison testing
**Key Characteristics**:
- Standard brain size (1000 neurons)
- Consistent parameters for fair comparison
- Detailed performance metrics
- Configurable benchmark scenarios

## Template Selection Guide

| Use Case | Recommended Template | Reason |
|----------|---------------------|--------|
| Production | high_perf.cfg | Optimized for performance and reliability |
| Research | development.cfg | Supports extended developmental studies |
| Prototyping | lightweight.cfg | Quick iteration with minimal resources |
| Scientific experiments | experimental.cfg | Designed for hypothesis testing |
| Performance comparison | benchmark.cfg | Standardized for fair comparison |
| General purpose | default.cfg | Balanced settings for most scenarios |

## Configuration File Format

### Basic Syntax

NLM configuration files use a simple `key = value` format with the following characteristics:

- **Key**: Dot-separated path representing nested configuration (e.g., `brain.neuron_count`)
- **Value**: String, integer, float, or boolean (auto-detected)
- **Comments**: Lines starting with `#` are treated as comments
- **Case Sensitivity**: Configuration keys are case-sensitive

### Example Configuration File

```ini
# NLM Configuration File
# This is a sample configuration for NLM simulations

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
brain.neuron_count = 1000

# Number of brain regions
brain.region_count = 2

# Connection probability for random connectivity
brain.synapse_density = 0.1

# Plasticity settings
plasticity.stdp.learning_rate = 0.01
plasticity.stdp.ltp_weight = 0.01
plasticity.stdp.ltd_weight = 0.012
plasticity.stdp.time_constant = 20.0

# Development settings
development.initial_plasticity = 1.0
development.synaptogenesis_rate = 0.001
development.pruning_rate = 0.0001
development.critical_period_start = 100
development.critical_period_end = 500

# Neuromodulation settings
neuromodulation.dopamine_baseline = 0.1
neuromodulation.curiosity_baseline = 0.0
neuromodulation.novelty_threshold = 0.5

# Reward settings
reward.discount_factor = 0.99

# Environment settings
environment.name = GridWorld
environment.width = 10
environment.height = 10
environment.visionWidth = 8
environment.visionHeight = 8

# Logging settings
logging.level = INFO
logging.to_file = false
logging.filename = nlm.log

# Simulation settings
simulation.max_steps = 10000
simulation.time_limit = 0.0

# Visualization settings
visualization.enabled = false
visualization.update_rate = 30.0
```

## Configuration Categories

### Brain Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `brain.neuron_count` | Integer | 1000 | Number of neurons in the brain | 10-10000 |
| `brain.region_count` | Integer | 2 | Number of brain regions | 1-10 |
| `brain.synapse_density` | Float | 0.1 | Probability of connections between neurons | 0.0-1.0 |

### Plasticity Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `plasticity.stdp.learning_rate` | Float | 0.01 | STDP learning rate | 0.0-1.0 |
| `plasticity.stdp.ltp_weight` | Float | 0.01 | LTP weight for Hebbian plasticity | 0.0-1.0 |
| `plasticity.stdp.ltd_weight` | Float | 0.012 | LTD weight for Hebbian plasticity | 0.0-1.0 |
| `plasticity.stdp.time_constant` | Float | 20.0 | Time constant for STDP | 1.0-100.0 |

### Development Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `development.initial_plasticity` | Float | 1.0 | Initial plasticity level | 0.0-10.0 |
| `development.synaptogenesis_rate` | Float | 0.001 | Rate of new synapse formation | 0.0-0.01 |
| `development.pruning_rate` | Float | 0.0001 | Rate of synapse elimination | 0.0-0.001 |
| `development.critical_period_start` | Integer | 100 | Start of critical period | 0-1000 |
| `development.critical_period_end` | Integer | 500 | End of critical period | 100-2000 |

### Neuromodulation Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `neuromodulation.dopamine_baseline` | Float | 0.1 | Baseline dopamine level | 0.0-1.0 |
| `neuromodulation.curiosity_baseline` | Float | 0.0 | Baseline curiosity level | 0.0-1.0 |
| `neuromodulation.novelty_threshold` | Float | 0.5 | Novelty threshold for curiosity | 0.0-1.0 |

### Reward Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `reward.discount_factor` | Float | 0.99 | Discount factor for reward prediction | 0.9-1.0 |

### Environment Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `environment.name` | String | GridWorld | Name of environment | [See supported environments] |
| `environment.width` | Integer | 10 | Environment width | 5-100 |
| `environment.height` | Integer | 10 | Environment height | 5-100 |
| `environment.visionWidth` | Integer | 8 | Agent vision width | 4-20 |
| `environment.visionHeight` | Integer | 8 | Agent vision height | 4-20 |

### Simulation Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `simulation.max_steps` | Integer | 10000 | Maximum simulation steps | 100-100000 |
| `simulation.time_limit` | Float | 0.0 | Simulation time limit (0 = unlimited) | 0.0-1000.0 |

### Logging Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `logging.level` | String | INFO | Log level | DEBUG, INFO, WARNING, ERROR |
| `logging.to_file` | Boolean | false | Log to file | true, false |
| `logging.filename` | String | nlm.log | Log file name | [valid filename] |

### Visualization Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `visualization.enabled` | Boolean | false | Enable visualization | true, false |
| `visualization.update_rate` | Float | 30.0 | Visualization update rate (Hz) | 1.0-60.0 |

## Template Specifications

### high_perf.cfg

```ini
# NLM Configuration Template
# High Performance Configuration
# Optimized for speed and efficiency

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain (larger = more powerful)
brain.neuron_count = 2000

# Number of brain regions (larger = more complex)
brain.region_count = 4

# Connection probability for random connectivity (higher = more connections)
brain.synapse_density = 0.15

# Plasticity settings
plasticity.stdp.learning_rate = 0.005
plasticity.stdp.ltp_weight = 0.008
plasticity.stdp.ltd_weight = 0.009
plasticity.stdp.time_constant = 20.0

# Development settings (lower rates for larger brains)
development.initial_plasticity = 0.8
development.synaptogenesis_rate = 0.0005
development.pruning_rate = 0.00005

# Neuromodulation settings
dopamine_baseline = 0.0
dopamine_baseline = 0.2  # Increased baseline for better motivation

# Reward settings
reward.discount_factor = 0.95  # Faster learning

# Environment settings
environment.name = GridWorld
environment.width = 20
environment.height = 20

# Logging settings
log_level = INFO
log_to_file = true
log_filename = high_perf_nlm.log

# Simulation settings (fewer steps for faster benchmarking)
max_simulation_steps = 2000
simulation_time_limit = 0.0

# Visualization settings (disabled for performance)
visualization_enabled = false
visualization_update_rate = 30.0
```

### development.cfg

```ini
# NLM Configuration Template
# Development and Learning Configuration
# Optimized for learning and development

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
brain.neuron_count = 1500

# Number of brain regions
brain.region_count = 3

# Connection probability for random connectivity
brain.synapse_density = 0.12

# Plasticity settings
plasticity.stdp.learning_rate = 0.008
plasticity.stdp.ltp_weight = 0.01
plasticity.stdp.ltd_weight = 0.011
plasticity.stdp.time_constant = 20.0

# Development settings (enhanced for learning)
development.initial_plasticity = 1.5
development.synaptogenesis_rate = 0.002
development.pruning_rate = 0.0002
development.critical_period_start = 50
development.critical_period_end = 400

# Neuromodulation settings
dopamine_baseline = 0.15
curiosity_baseline = 0.1
novelty_threshold = 0.4

# Reward settings
reward.discount_factor = 0.98

# Environment settings
environment.name = GridWorld
environment.width = 15
environment.height = 15
environment.visionWidth = 10
environment.visionHeight = 10

# Logging settings
log_level = DEBUG
log_to_file = true
log_filename = development_nlm.log

# Simulation settings (extended for development studies)
max_simulation_steps = 5000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

### lightweight.cfg

```ini
# NLM Configuration Template
# Lightweight Configuration
# Optimized for minimal resource usage

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain (small = minimal resources)
brain.neuron_count = 300

# Number of brain regions
brain.region_count = 1

# Connection probability for random connectivity
brain.synapse_density = 0.05

# Plasticity settings
plasticity.stdp.learning_rate = 0.02
plasticity.stdp.ltp_weight = 0.015
plasticity.stdp.ltd_weight = 0.018
plasticity.stdp.time_constant = 20.0

# Development settings (conservative)
development.initial_plasticity = 0.5
development.synaptogenesis_rate = 0.0005
development.pruning_rate = 0.00005
development.critical_period_start = 0

# Neuromodulation settings
dopamine_baseline = 0.05
curiosity_baseline = 0.0
novelty_threshold = 0.6

# Reward settings
reward.discount_factor = 0.95

# Environment settings
environment.name = GridWorld
environment.width = 8
environment.height = 8
environment.visionWidth = 6
environment.visionHeight = 6

# Logging settings
log_level = WARNING
log_to_file = false
log_filename = lightweight_nlm.log

# Simulation settings (quick testing)
max_simulation_steps = 500
simulation_time_limit = 0.0

# Visualization settings (disabled)
visualization_enabled = false
visualization_update_rate = 30.0
```

### experimental.cfg

```ini
# NLM Configuration Template
# Experimental Configuration
# Designed for testing specific hypotheses

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
brain.neuron_count = 1000

# Number of brain regions
brain.region_count = 2

# Connection probability for random connectivity (high for maximum exploration)
brain.synapse_density = 0.25

# Plasticity settings (extreme values for experimental conditions)
plasticity.stdp.learning_rate = 0.05
plasticity.stdp.ltp_weight = 0.1
plasticity.stdp.ltd_weight = 0.12
plasticity.stdp.time_constant = 15.0

# Development settings (minimal development)
development.initial_plasticity = 2.0
development.synaptogenesis_rate = 0.01
development.pruning_rate = 0.001

# Neuromodulation settings (enhanced)
dopamine_baseline = 0.5
curiosity_baseline = 0.3
novelty_threshold = 0.8

# Reward settings (aggressive)
reward.discount_factor = 0.9

# Environment settings (challenging)
environment.name = GridWorld
environment.width = 25
environment.height = 25
environment.visionWidth = 12
environment.visionHeight = 12

# Logging settings (detailed)
log_level = INFO
log_to_file = true
log_filename = experimental_nlm.log

# Simulation settings (extended for experiments)
max_simulation_steps = 3000
simulation_time_limit = 0.0

# Visualization settings (enabled for observation)
visualization_enabled = true
visualization_update_rate = 30.0
```

### benchmark.cfg

```ini
# NLM Configuration Template
# Benchmark Configuration
# Designed for performance testing

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain (standard for comparison)
brain.neuron_count = 1000

# Number of brain regions (standard)
brain.region_count = 2

# Connection probability for random connectivity
brain.synapse_density = 0.1

# Plasticity settings (baseline)
plasticity.stdp.learning_rate = 0.01
plasticity.stdp.ltp_weight = 0.01
plasticity.stdp.ltd_weight = 0.012
plasticity.stdp.time_constant = 20.0

# Development settings (baseline)
development.initial_plasticity = 1.0
development.synaptogenesis_rate = 0.001
development.pruning_rate = 0.0001

# Neuromodulation settings (baseline)
dopamine_baseline = 0.1
curiosity_baseline = 0.0
novelty_threshold = 0.5

# Reward settings (baseline)
reward.discount_factor = 0.99

# Environment settings (standard)
environment.name = GridWorld
environment.width = 10
environment.height = 10
environment.visionWidth = 8
environment.visionHeight = 8

# Logging settings (performance-focused)
log_level = INFO
log_to_file = true
log_filename = benchmark_nlm.log

# Simulation settings (benchmark-focused)
max_simulation_steps = 1000
simulation_time_limit = 0.0

# Visualization settings (disabled for performance)
visualization_enabled = false
visualization_update_rate = 30.0
```

## Configuration Management

### Configuration Validation

NLM validates configuration parameters before simulation:

```python
import pynlm

# Validate configuration
config = pynlm.createDefaultConfig()

# Check if configuration is valid
if config.validate():
    print("Configuration is valid")
else:
    print("Configuration has errors:")
    for error in config.get_validation_errors():
        print(f"  - {error}")
```

### Configuration Copying and Modification

```python
import pynlm
import shutil
import os

# Copy configuration template
current_dir = os.path.dirname(os.path.abspath(__file__))

# Create custom configuration from template
config = pynlm.createDefaultConfig()

# Modify parameters
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("environment.width", 20)
config.set("environment.height", 20)

# Save custom configuration
config.save("my_custom_config.cfg")

# Or create configuration by copying and editing
if os.path.exists("configs/high_perf.cfg"):
    shutil.copy("configs/high_perf.cfg", "my_config.cfg")
    
    # Edit file directly (for complex modifications)
    with open("my_config.cfg", 'r') as f:
        content = f.read()
    
    # Replace parameters
    content = content.replace("neuron_count = 2000", "neuron_count = 1500")
    content = content.replace("plasticity.stdp.learning_rate = 0.005", "plasticity.stdp.learning_rate = 0.008")
    
    with open("my_config.cfg", 'w') as f:
        f.write(content)
```

### Configuration Comparison

```python
import pynlm

# Create two configurations
config1 = pynlm.createDefaultConfig()
config1.set("brain.neuron_count", 1000)
config1.set("environment.width", 10)

config2 = pynlm.createDefaultConfig()
config2.set("brain.neuron_count", 2000)
config2.set("environment.width", 20)

# Compare configurations
differences = config1.compare(config2)

print("Differences between configurations:")
for key, (value1, value2) in differences.items():
    print(f"  {key}: {value1} -> {value2}")
```

### Configuration Versioning

```python
import pynlm
import json
import os
from datetime import datetime

class ConfigurationVersionManager:
    """Manage configuration versions and changes."""
    
    def __init__(self, version_dir="config_versions/"):
        self.version_dir = Path(version_dir)
        os.makedirs(version_dir, exist_ok=True)
    
    def save_version(self, config, version_name=None):
        """Save configuration as new version."""
        if version_name is None:
            version_name = f"v_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
        
        # Capture configuration state
        config_data = {
            'timestamp': datetime.now().isoformat(),
            'version': version_name,
            'config': config.to_dict(),  # Convert to dictionary if available
            'description': f"Configuration version {version_name}"
        }
        
        # Save to file
        version_file = self.version_dir / f"{version_name}.json"
        with open(version_file, 'w') as f:
            json.dump(config_data, f, indent=2)
        
        print(f"Configuration saved as version: {version_name}")
        return version_name
    
    def load_version(self, version_name):
        """Load configuration from version."""
        version_file = self.version_dir / f"{version_name}.json"
        
        if not version_file.exists():
            raise FileNotFoundError(f"Version not found: {version_name}")
        
        with open(version_file, 'r') as f:
            config_data = json.load(f)
        
        # Reconstruct configuration
        config = pynlm.createDefaultConfig()
        
        # Apply configuration parameters
        if 'config' in config_data:
            for key, value in config_data['config'].items():
                config.set(key, value)
        
        print(f"Configuration loaded from version: {version_name}")
        return config
    
    def list_versions(self):
        """List available configuration versions."""
        versions = []
        
        for filename in os.listdir(self.version_dir):
            if filename.endswith('.json'):
                version_name = filename[:-5]  # Remove .json extension
                versions.append(version_name)
        
        versions.sort()  # Sort by name (timestamp)
        
        print("Available configuration versions:")
        for version in versions:
            print(f"  - {version}")
        
        return versions
```

## Configuration Best Practices

### Optimization Guidelines

1. **Performance Optimization**
   - Use `high_perf.cfg` for production environments
   - Adjust `brain.neuron_count` based on available memory
   - Use `lightweight.cfg` for quick testing
   - Balance between accuracy and performance

2. **Memory Management**
   - Monitor memory usage during simulation
   - Use smaller configurations for memory-constrained environments
   - Consider using compression for large simulations

3. **Simulation Efficiency**
   - Use appropriate `simulation.max_steps` for your use case
   - Balance between simulation depth and runtime
   - Use efficient integration methods for long simulations

### Validation Best Practices

1. **Pre-Simulation Validation**
   - Always validate configuration before running simulations
   - Check for invalid parameter combinations
   - Verify environment settings are compatible

2. **Post-Simulation Validation**
   - Check simulation results for validity
   - Verify numerical stability
   - Validate performance metrics

3. **Continuous Validation**
   - Implement automated validation in CI/CD pipelines
   - Set up continuous monitoring of configuration validity
   - Implement validation hooks for plugin systems

### Configuration File Management

1. **Version Control**
   - Keep configuration files under version control
   - Track configuration changes over time
   - Use meaningful version names

2. **Backup and Recovery**
   - Regular backup of important configurations
   - Maintain working configurations in multiple locations
   - Use checkpoints for long-running simulations

3. **Documentation**
   - Document configuration parameters and their effects
   - Provide examples of common configurations
   - Include best practices and recommendations

## Configuration File Reference

### File Formats

#### .cfg Files (Key-Value Format)
- **Purpose**: Primary configuration format
- **Syntax**: `key = value`
- **Example**: `brain.neuron_count = 1000`
- **Usage**: Most common configuration file format

#### .json Files (JSON Format)
- **Purpose**: Machine-readable configuration
- **Syntax**: JSON object with nested structure
- **Example**: `{"brain": {"neuron_count": 1000}}`
- **Usage**: API communication, configuration databases

#### .yaml Files (YAML Format)
- **Purpose**: Human-readable configuration
- **Syntax**: YAML key-value pairs
- **Example**: `brain:
  neuron_count: 1000`
- **Usage**: Documentation, complex configurations

### Configuration Migration

```python
import pynlm

# Migrate from old configuration format to new

def migrate_old_config(old_config):
    """Migrate configuration from old format to new format."""
    new_config = pynlm.createDefaultConfig()
    
    # Migrate brain settings
    if 'neuronCount' in old_config:
        new_config.set("brain.neuron_count", old_config['neuronCount'])
    
    # Migrate environment settings
    if 'envWidth' in old_config:
        new_config.set("environment.width", old_config['envWidth'])
    
    if 'envHeight' in old_config:
        new_config.set("environment.height", old_config['envHeight'])
    
    # Migrate other settings
    if 'plasticityLearningRate' in old_config:
        new_config.set("plasticity.stdp.learning_rate", old_config['plasticityLearningRate'])
    
    return new_config

# Example usage
old_config = {
    'neuronCount': 1500,
    'envWidth': 15,
    'envHeight': 15,
    'plasticityLearningRate': 0.008
}

new_config = migrate_old_config(old_config)
new_config.save("migrated_config.cfg")
```

## Configuration Troubleshooting

### Common Configuration Issues

1. **Parameter Validation Errors**
   - **Problem**: Invalid parameter values
   - **Solution**: Use configuration validation tools
   - **Prevention**: Implement parameter checking in code

2. **File Path Issues**
   - **Problem**: Configuration files not found
   - **Solution**: Check file paths and permissions
   - **Prevention**: Include configuration files in distribution packages

3. **Incompatible Parameter Combinations**
   - **Problem**: Parameters that conflict with each other
   - **Solution**: Use configuration templates
   - **Prevention**: Implement combination validation

4. **Memory Constraints**
   - **Problem**: System runs out of memory
   - **Solution**: Use smaller configurations
   - **Prevention**: Monitor memory usage

### Troubleshooting Commands

```bash
# Validate configuration file
./nlmc validate-config my_config.cfg

# Show configuration differences
./nlmc compare-configs configs/default.cfg my_config.cfg

# Run simulation with configuration
./nlmc run-sim my_config.cfg --steps 1000

# Check configuration syntax
python3 -c "import pynlm; config = pynlm.createDefaultConfig(); config.set('brain.neuron_count', 1000); print('Configuration valid')"

# Test configuration with quick simulation
./nlmc benchmark my_config.cfg --iterations 3
```

## Configuration Documentation

### Parameter Documentation

Each configuration parameter should be documented with:

1. **Description**: What the parameter does
2. **Type**: Data type (integer, float, boolean, string)
3. **Default Value**: Default setting
4. **Valid Range**: Acceptable values
5. **Effect**: How the parameter affects simulation behavior
6. **Recommendations**: When to use specific values

### Example Parameter Documentation

```ini
# brain.neuron_count = 1000
# Description: Number of neurons in the brain
# Type: Integer
# Default: 1000
# Valid Range: 10-10000
# Effect: Determines brain complexity and computational requirements
# Recommendations: Use 1000 for standard testing, 500 for quick tests, 5000 for research
```

## Configuration Examples

### Simple Configuration

```ini
# Simple configuration for basic testing
random_seed = 42
simulation_timestep = 0.001
brain.neuron_count = 500
brain.region_count = 2
environment.name = GridWorld
environment.width = 10
environment.height = 10
simulation.max_steps = 1000
```

### Complex Configuration

```ini
# Complex configuration for research
random_seed = 12345
simulation_timestep = 0.001
brain.neuron_count = 2000
brain.region_count = 4
brain.synapse_density = 0.15
plasticity.stdp.learning_rate = 0.005
plasticity.stdp.ltp_weight = 0.008
plasticity.stdp.ltd_weight = 0.009
plasticity.stdp.time_constant = 20.0
development.initial_plasticity = 1.0
development.synaptogenesis_rate = 0.0005
development.pruning_rate = 0.00005
development.critical_period_start = 100
development.critical_period_end = 500
neuromodulation.dopamine_baseline = 0.2
curiosity_baseline = 0.1
reward.discount_factor = 0.95
environment.name = GridWorld
environment.width = 20
environment.height = 20
environment.visionWidth = 8
environment.visionHeight = 8
simulation.max_steps = 5000
logging.level = INFO
logging.to_file = true
logging.filename = research_simulation.log
```

### Performance Configuration

```ini
# Performance-optimized configuration
random_seed = 42
simulation_timestep = 0.001
brain.neuron_count = 1500
brain.synapse_density = 0.12
plasticity.stdp.learning_rate = 0.01
plasticity.stdp.ltp_weight = 0.01
plasticity.stdp.ltd_weight = 0.012
development.initial_plasticity = 0.8
environment.name = GridWorld
environment.width = 15
environment.height = 15
simulation.max_steps = 2000
logging.level = WARNING
logging.to_file = false
```

## Conclusion

Configuration management is a critical aspect of NLM system operation. By following these guidelines and best practices, users can:

1. **Create Effective Configurations**: Use appropriate templates and parameter settings
2. **Validate Configurations**: Ensure configurations are valid before use
3. **Manage Configurations**: Track changes, versions, and backups
4. **Troubleshoot Issues**: Identify and resolve configuration-related problems
5. **Optimize Performance**: Balance performance, memory usage, and simulation requirements

By mastering NLM configuration management, users can build robust, efficient, and reliable neural simulation systems that meet their specific requirements and research goals.
