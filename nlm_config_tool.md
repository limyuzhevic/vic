# NLM Advanced Configuration Tool

A comprehensive tool for configuring, validating, and managing NLM simulations with advanced options for experienced users.

## Overview

The Advanced Configuration Tool provides:
- Sophisticated configuration validation and error checking
- Batch configuration management
- Advanced parameter tuning and optimization
- Configuration templates and presets
- Real-time configuration preview and testing
- Integration with checkpoint systems
- Export/import configuration utilities

## Usage

### Command Line Interface

```bash
# Show help
./nlm_config_tool --help

# Validate configuration
./nlm_config_tool validate configs/my_config.cfg

# Apply advanced tuning
./nlm_config_tool optimize configs/default.cfg --target-performance=high --learning-rate=adaptive

# Batch process multiple configs
./nlm_config_tool batch configs/ --output-dir=optimized_configs

# Export configuration
./nlm_config_tool export configs/default.cfg --format=json --output=nlm_config.json

# Import and validate configuration
./nlm_config_tool import nlm_config.json
```

### Python Interface

```python
import nlm_config_tool as nct

# Create advanced configuration
config = nct.AdvancedConfig()

# Set sophisticated parameters
config.set("neuron_count", 2000)
config.set("region_count", 3)
config.set("connection_probability", 0.15f)

# Configure advanced plasticity
config.set_advanced_plasticity(
    stdp_enabled=True,
    hebbian_enabled=True,
    structural_enabled=True,
    plasticity_modulation="adaptive"
)

# Configure neuromodulation
config.set_neuromodulation(
    dopamine_enabled=True,
    novelty_enabled=True,
    curiosity_enabled=True,
    neuromodulation_strength=1.5f
)

# Configure memory systems
config.set_memory_systems(
    working_memory_enabled=True,
    episodic_memory_enabled=True,
    episodic_memory_size=5000,
    replay_enabled=True,
    replay_interval=50
)

# Save configuration
config.save("advanced_config.cfg")
```

## Advanced Configuration Features

### 1. Configuration Validation

The tool validates configurations for:
- **Parameter ranges**: Ensures parameters are within valid ranges
- **Consistency checks**: Validates parameter interdependencies
- **Performance feasibility**: Estimates runtime and memory requirements
- **Convergence testing**: Tests configuration before full simulation

Example validation rules:
```python
# Parameter range validation
config.validate_range("neuron_count", 100, 10000)
config.validate_range("connection_probability", 0.01, 0.5)
config.validate_range("stdp_ltp_weight", 0.0, 0.1)

# Consistency validation
config.validate_consistency({
    "neuron_count": {"min": 100, "max": 10000},
    "region_count": {"min": 1, "max": 100},
    "stdp_ltp_weight": {"positive_only": True},
    "hebbian_enabled": {"requires_stdp": True}
})
```

### 2. Advanced Plasticity Configuration

Configure sophisticated plasticity rules:

```python
# STDP configuration
config.set_stdp_parameters(
    ltp_weight=0.02f,
    ltd_weight=0.015f,
    time_constant=20.0f,
    min_weight=-1.0f,
    max_weight=1.0f,
    weight_dependent=True,
    neuromodulation_sensitive=True
)

# Hebbian configuration
config.set_hebbian_parameters(
    learning_rate=0.01f,
    eligibility_trace_duration=100,
    max_eligibility_trace=10.0f,
    presynaptic_modulation=True
)

# Structural plasticity configuration
config.set_structural_plasticity(
    synaptogenesis_rate=0.0002f,
    pruning_rate=0.00005f,
    development_modulated=True,
    activity_dependent=True
)
```

### 3. Neuromodulation Configuration

Advanced neuromodulation setup:

```python
# Dopamine system
config.set_dopamine_parameters(
    baseline=0.1f,
    reward_sensitivity=2.0f,
    prediction_error_weight=1.5f,
    learning_modulation=True
)

# Curiosity system
config.set_curiosity_parameters(
    novelty_threshold=0.5f,
    exploration_bonus=0.5f,
    policy_gradient_weight=0.3f
)

# Novelty detection
config.set_novelty_parameters(
    detection_sensitivity=0.8f,
    pattern_change_threshold=0.3f,
    decay_rate=0.99f
)
```

### 4. Memory System Configuration

Configure sophisticated memory systems:

```python
# Working memory
config.set_working_memory(
    capacity=50,  # Number of traces
    trace_duration=100,  # Steps
    competition_strength=0.5f,
    reinstatement_probability=0.7f
)

# Episodic memory
config.set_episodic_memory(
    storage_capacity=5000,  # Number of episodes
    encoding_frequency=10,  # Encode every N steps
    replay_frequency=50,  # Replay every N steps
    consolidation_threshold=10,  # Minimum episodes for consolidation
    pattern_compression=True
)
```

### 5. Performance Optimization

Automatic performance optimization:

```python
# Performance mode configuration
config.set_performance_mode("balanced")  # options: speed, accuracy, balanced

# Resource allocation
config.set_resource_allocation(
    cpu_threads=4,
    memory_limit_gb=2.0,
    vectorization_enabled=True,
    parallelization_enabled=True
)

# Optimization presets
config.apply_preset("research_high_accuracy")
config.apply_preset("real_time_simulation")
config.apply_preset("large_scale_network")
```

### 6. Configuration Templates and Presets

Predefined configuration templates:

```python
# Research configurations
config.use_template("scientific_research", {
    neuron_count=5000,
    region_count=10,
    learning_rate=0.01,
    memory_capacity=10000
})

# Real-time configurations
config.use_template("real_time_behavior", {
    neuron_count=1000,
    region_count=3,
    timestep=0.01,
    memory_realtime=True
})

# Large-scale configurations
config.use_template("high_performance", {
    neuron_count=20000,
    region_count=50,
    parallel_processing=True,
    distributed_memory=True
})
```

## Configuration File Formats

### JSON Configuration

```json
{
    "nlm_version": "6.0",
    "config_name": "advanced_research_config",
    
    "neural_parameters": {
        "neuron_count": 5000,
        "region_count": 10,
        "connection_probability": 0.15,
        "simulation_timestep": 0.001
    },
    
    "plasticity_parameters": {
        "stdp": {
            "enabled": true,
            "ltp_weight": 0.02,
            "ltd_weight": 0.015,
            "time_constant": 20.0
        },
        "hebbian": {
            "enabled": true,
            "learning_rate": 0.01,
            "eligibility_trace": 100
        }
    },
    
    "neuromodulation": {
        "dopamine": {
            "enabled": true,
            "baseline": 0.1,
            "reward_sensitivity": 2.0
        },
        "curiosity": {
            "enabled": true,
            "novelty_threshold": 0.5
        }
    },
    
    "memory_systems": {
        "working_memory": {
            "enabled": true,
            "capacity": 50,
            "trace_duration": 100
        },
        "episodic_memory": {
            "enabled": true,
            "storage_capacity": 5000,
            "encoding_frequency": 10
        }
    }
}
```

### YAML Configuration

```yaml
nlm_version: "6.0"
config_name: "advanced_research_config"

neural_parameters:
  neuron_count: 5000
  region_count: 10
  connection_probability: 0.15
  simulation_timestep: 0.001

plasticity_parameters:
  stdp:
    enabled: true
    ltp_weight: 0.02
    ltd_weight: 0.015
    time_constant: 20.0
  hebbian:
    enabled: true
    learning_rate: 0.01
    eligibility_trace: 100

neuromodulation:
  dopamine:
    enabled: true
    baseline: 0.1
    reward_sensitivity: 2.0
  curiosity:
    enabled: true
    novelty_threshold: 0.5

memory_systems:
  working_memory:
    enabled: true
    capacity: 50
    trace_duration: 100
  episodic_memory:
    enabled: true
    storage_capacity: 5000
    encoding_frequency: 10
```

## Integration Features

### Checkpoint Integration

```python
# Integrate with checkpoint system
config.set_checkpoint_integration(
    enabled=True,
    auto_checkpoint=True,
    checkpoint_interval=1000,
    checkpoint_path="checkpoints/",
    compression_level="balanced"
)

# Validate checkpoint compatibility
config.validate_checkpoint_compatibility()
```

### Simulation Integration

```python
# Generate simulation configuration
brain_config = config.generate_simulation_config()
brain = nlm.create_brain(brain_config)

# Setup advanced simulation
config.setup_advanced_simulation(
    enable_visualization=True,
    visualization_output="real_time",
    enable_profiling=True,
    profiling_output="nlm_profile.json"
)
```

## Command Line Tools

### Configuration Validation

```bash
# Validate configuration file
./nlm_config_tool validate --config configs/advanced.cfg --verbose

# Check for parameter conflicts
./nlm_config_tool check-conflicts --config configs/advanced.cfg

# Estimate performance requirements
./nlm_config_tool estimate-performance --config configs/advanced.cfg
```

### Configuration Management

```bash
# Apply optimization presets
./nlm_config_tool apply-preset --config config.cfg --preset high-accuracy
./nlm_config_tool apply-preset --config config.cfg --preset real-time

# Batch process configurations
./nlm_config_tool batch-process --input-dir configs/ --output-dir optimized/

# Merge configurations
./nlm_config_tool merge --configs config1.cfg config2.cfg --output merged.cfg
```

### Export and Import

```bash
# Export to JSON
./nlm_config_tool export --config config.cfg --format json --output config.json

# Export to YAML
./nlm_config_tool export --config config.cfg --format yaml --output config.yaml

# Import from JSON
./nlm_config_tool import --format json --input config.json --output config.cfg

# Import from YAML
./nlm_config_tool import --format yaml --input config.yaml --output config.cfg
```

## API Reference

### Classes

#### AdvancedConfig
Main configuration class with comprehensive configuration management.

#### ConfigValidator
Validates configuration parameters and consistency.

#### ConfigOptimizer
Optimizes configuration for performance or accuracy.

#### ConfigExporter
Exports configuration to various formats.

### Methods

#### AdvancedConfig
- `AdvancedConfig()` - Constructor
- `void set(string key, T value)` - Set configuration parameter
- `T get(string key)` - Get configuration parameter
- `void validate()` - Validate entire configuration
- `void optimize(OptimizationMode mode)` - Optimize configuration
- `void save(string filename)` - Save configuration to file
- `void load(string filename)` - Load configuration from file
- `AdvancedConfig merge(AdvancedConfig other)` - Merge with another configuration
- `bool is_valid()` - Check if configuration is valid
- `vector<string> get_errors()` - Get validation errors

#### ConfigValidator
- `bool validate_range(string key, T min, T max)` - Validate parameter range
- `bool validate_consistency(map<string, ValidationRule> rules)` - Validate consistency
- `bool validate_dependencies(map<string, vector<string>> dependencies)` - Validate dependencies

#### ConfigOptimizer
- `void optimize_for_performance()` - Optimize for speed
- `void optimize_for_accuracy()` - Optimize for accuracy
- `void optimize_for_balance()` - Optimize for balance
- `double calculate_score()` - Calculate optimization score

#### ConfigExporter
- `void export_json(string filename)` - Export to JSON
- `void export_yaml(string filename)` - Export to YAML
- `void export_xml(string filename)` - Export to XML
- `void export_ini(string filename)` - Export to INI

## Installation

### From Source

```bash
# Clone repository
 git clone <repository-url>
 cd nlm

# Build configuration tool
 mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_CONFIG_TOOL=ON ..
make -j4

# Install
 sudo make install
```

### Using Package Manager

```bash
# Ubuntu/Debian
 sudo apt-get update
sudo apt-get install nlm-config-tool

# macOS
brew install nlm-config-tool

# Linux (generic)
curl -L https://github.com/nlm-project/nlm-config-tool/releases/download/v1.0.0/nlm-config-tool-1.0.0-linux-x86_64.tar.gz | tar xz
sudo mv nlm-config-tool /usr/local/bin/
```

### Python Package

```bash
pip install nlm-config-tool
```

## Configuration File Examples

See the `examples/` directory for complete configuration examples:

- `examples/basic_config.cfg` - Basic configuration
- `examples/research_config.cfg` - Research-oriented configuration
- `examples/real_time_config.cfg` - Real-time configuration
- `examples/large_scale_config.cfg` - Large-scale configuration

## Support

For support with the Advanced Configuration Tool:
- GitHub Issues: https://github.com/nlm-project/nlm-config-tool/issues
- Documentation: https://docs.nlm-project.org/config-tool
- Community Forum: https://forum.nlm-project.org

## License

MIT License - See LICENSE file for details.