# NLM Configuration Guide

## Overview

The Neural Learning Machine (NLM) provides a sophisticated configuration system that supports multiple formats, inheritance, templates, and validation. This guide covers all aspects of configuration management.

## Configuration Formats

### JSON Format (`.cfg`)
JSON is the primary configuration format. Example:

```json
{
  "simulation": {
    "random_seed": 42,
    "max_steps": 10000,
    "timestep": 0.001,
    "neuron_count": 500,
    "region_count": 1
  },
  "plasticity": {
    "learning_rate": 0.01,
    "enable_stdp": true,
    "weight_decay": 0.001
  },
  "neuromodulation": {
    "enable_novelty": true,
    "enable_reward": false,
    "novelty_threshold": 0.1
  },
  "development": {
    "enable_critical_period": true,
    "critical_period_strength": 0.8,
    "enable_maturation": true
  },
  "performance": {
    "parallel_execution": true,
    "num_processes": 4,
    "enable_vectorization": true,
    "enable_gpu": false
  }
}
```

### YAML Format (`.yml`, `.yaml`)
YAML configuration files:

```yaml
simulation:
  random_seed: 42
  max_steps: 10000
  timestep: 0.001
  neuron_count: 500
  region_count: 1

plasticity:
  learning_rate: 0.01
  enable_stdp: true
  weight_decay: 0.001

neuromodulation:
  enable_novelty: true
  enable_reward: false
  novelty_threshold: 0.1

development:
  enable_critical_period: true
  critical_period_strength: 0.8
  enable_maturation: true

performance:
  parallel_execution: true
  num_processes: 4
  enable_vectorization: true
  enable_gpu: false
```

### INI Format (`.ini`)
INI-style configuration:

```ini
[simulation]
random_seed = 42
max_steps = 10000
timestep = 0.001
neuron_count = 500
region_count = 1

[plasticity]
learning_rate = 0.01
enable_stdp = true
weight_decay = 0.001

[neuromodulation]
enable_novelty = true
enable_reward = false
novelty_threshold = 0.1

[development]
enable_critical_period = true
critical_period_strength = 0.8
enable_maturation = true

[performance]
parallel_execution = true
num_processes = 4
enable_vectorization = true
enable_gpu = false
```

## Configuration File Locations

### Default Locations
1. **Current directory**: `config.cfg`, `configs/config.cfg`, `config.json`
2. **User directory**: `~/.nlm/config.cfg`
3. **System directory**: `/etc/nlm/config.cfg`
4. **Project-specific**: `./project/.nlm/config.cfg`

### Command Line Overrides
```bash
# Specify configuration file
nlm run --config my_config.cfg
nlm config load my_config.json

# Environment variable
export NLM_CONFIG_DIR=/path/to/configs
nlm run
```

## Configuration Schema

### Core Configuration Categories

#### 1. Simulation Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `random_seed` | int64 | 42 | Random seed for reproducibility |
| `max_steps` | int64 | 10000 | Maximum simulation steps |
| `timestep` | double | 0.001 | Simulation timestep |
| `neuron_count` | int64 | 500 | Number of neurons in network |
| `region_count` | int64 | 1 | Number of neural regions |
| `enable_interactive` | bool | false | Enable interactive mode |

#### 2. Neural Network Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `learning_rate` | double | 0.01 | Learning rate for plasticity |
| `enable_stdp` | bool | true | Enable Spike-Timing Dependent Plasticity |
| `weight_decay` | double | 0.001 | Weight decay factor |
| `max_weight` | double | 1.0 | Maximum synaptic weight |
| `min_weight` | double | -1.0 | Minimum synaptic weight |
| `connection_probability` | double | 0.1 | Probability of synapse formation |

#### 3. Neuromodulation Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `enable_novelty` | bool | true | Enable novelty detection |
| `enable_reward` | bool | false | Enable reward signals |
| `novelty_threshold` | double | 0.1 | Threshold for novelty detection |
| `reward_scale` | double | 1.0 | Scale factor for reward signals |
| `enable_curiosity` | bool | true | Enable curiosity-driven learning |

#### 4. Development Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `enable_critical_period` | bool | true | Enable critical period for learning |
| `critical_period_strength` | double | 0.8 | Strength of critical period effects |
| `enable_maturation` | bool | true | Enable neural maturation |
| `enable_pruning` | bool | true | Enable synaptic pruning |
| `pruning_threshold` | double | 0.01 | Threshold for pruning synapses |

#### 5. Performance Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `parallel_execution` | bool | true | Enable parallel execution |
| `num_processes` | int64 | 4 | Number of parallel processes |
| `enable_vectorization` | bool | true | Enable SIMD vectorization |
| `enable_gpu` | bool | false | Enable GPU acceleration |
| `gpu_device` | int64 | 0 | GPU device index |
| `memory_limit` | int64 | 8192 | Memory limit in MB |

#### 6. Monitoring Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `enable_logging` | bool | true | Enable logging |
| `log_level` | string | "Info" | Log level (Debug, Info, Warning, Error) |
| `log_file` | string | "" | Log file path |
| `enable_profiling` | bool | false | Enable profiling |
| `profile_interval` | double | 1.0 | Profiling interval in seconds |

## Configuration Templates

### Standard Template
Creates a standard configuration suitable for general use:

```bash
nlm config template standard
```

**Standard Template Configuration**:
```json
{
  "simulation": {
    "random_seed": 42,
    "max_steps": 10000,
    "timestep": 0.001,
    "neuron_count": 500,
    "region_count": 1,
    "enable_interactive": false
  },
  "plasticity": {
    "learning_rate": 0.01,
    "enable_stdp": true,
    "weight_decay": 0.001,
    "max_weight": 1.0,
    "min_weight": -1.0
  },
  "performance": {
    "parallel_execution": true,
    "num_processes": 4,
    "enable_vectorization": true,
    "enable_gpu": false
  },
  "monitoring": {
    "enable_logging": true,
    "log_level": "Info",
    "enable_profiling": false
  }
}
```

### Research Template
Creates a configuration optimized for research with more neurons and detailed logging:

```bash
nlm config template research
```

**Research Template Configuration**:
```json
{
  "simulation": {
    "random_seed": 12345,
    "max_steps": 100000,
    "timestep": 0.0001,
    "neuron_count": 2000,
    "region_count": 2,
    "enable_interactive": false
  },
  "plasticity": {
    "learning_rate": 0.001,
    "enable_stdp": true,
    "weight_decay": 0.0001,
    "max_weight": 2.0,
    "min_weight": -2.0
  },
  "development": {
    "enable_critical_period": true,
    "critical_period_strength": 1.0,
    "enable_maturation": true,
    "enable_pruning": true,
    "pruning_threshold": 0.001
  },
  "neuromodulation": {
    "enable_novelty": true,
    "enable_reward": true,
    "novelty_threshold": 0.05,
    "enable_curiosity": true
  },
  "performance": {
    "parallel_execution": true,
    "num_processes": 8,
    "enable_vectorization": true,
    "enable_gpu": true,
    "gpu_device": 0
  },
  "monitoring": {
    "enable_logging": true,
    "log_level": "Debug",
    "enable_profiling": true,
    "log_file": "research_simulation.log"
  }
}
```

### Performance Template
Creates a configuration optimized for high performance:

```bash
nlm config template performance
```

**Performance Template Configuration**:
```json
{
  "simulation": {
    "random_seed": 999,
    "max_steps": 5000,
    "timestep": 0.001,
    "neuron_count": 5000,
    "region_count": 1,
    "enable_interactive": false
  },
  "plasticity": {
    "learning_rate": 0.05,
    "enable_stdp": false,
    "weight_decay": 0.01,
    "max_weight": 5.0,
    "min_weight": -5.0
  },
  "performance": {
    "parallel_execution": true,
    "num_processes": 16,
    "enable_vectorization": true,
    "enable_gpu": true,
    "gpu_device": 0,
    "memory_limit": 32768
  },
  "monitoring": {
    "enable_logging": true,
    "log_level": "Warning",
    "enable_profiling": true
  }
}
```

## Configuration Inheritance

### Parent-Child Configuration
Child configurations inherit from parent configurations:

```bash
# Create child configuration from parent
nlm config load parent_config.json
nlm config save child_config.json --inherit parent_config.json

# Merge configurations
nlm config merge --file1 config1.json --file2 config2.json --output merged.json
```

### Inheritance Rules
1. **Child overrides parent** for same keys
2. **Parent values** used if child doesn't specify
3. **Deep inheritance** supported (child of child)
4. **Override markers** for selective inheritance

### Example Inheritance Structure
```json
// parent_config.json
{
  "simulation": {
    "random_seed": 42,
    "max_steps": 10000
  },
  "plasticity": {
    "learning_rate": 0.01,
    "enable_stdp": true
  }
}

// child_config.json (inherits from parent)
{
  "inherit": "parent_config.json",
  "simulation": {
    "max_steps": 20000  // Override parent
  },
  "plasticity": {
    "weight_decay": 0.001  // New key
  }
}
```

After inheritance, child resolves to:
```json
{
  "simulation": {
    "random_seed": 42,
    "max_steps": 20000
  },
  "plasticity": {
    "learning_rate": 0.01,
    "enable_stdp": true,
    "weight_decay": 0.001
  }
}
```

## Configuration Validation

### Schema Validation
Configuration can be validated against a predefined schema:

```bash
# Validate against standard schema
nlm config validate

# Validate with custom schema
nlm config validate --schema custom_schema.json

# List available schemas
nlm config list-schemas
```

### Required Keys
For a configuration to be valid, it must contain these required keys:

```json
{
  "simulation": {
    "random_seed": 42,
    "max_steps": 10000,
    "timestep": 0.001,
    "neuron_count": 500
  }
}
```

### Constraint Validation
Configuration values can be constrained:

```bash
# Set constraints
nlm config set-constraints --max-neuron-count 10000 --min-learning-rate 0.0001

# Validate with constraints
nlm config validate --constraints
```

## Configuration Export/Import

### Export Formats
```bash
# Export to JSON
nlm config export --format json --output config.json

# Export to YAML
nlm config export --format yaml --output config.yaml

# Export to INI
nlm config export --format ini --output config.ini

# Export environment variables
nlm config export --format env --output .env
```

### Import Formats
```bash
# Import from JSON
nlm config import --format json < config.json

# Import from YAML
nlm config import --format yaml < config.yaml

# Import from INI
nlm config import --format ini < config.ini
```

### Data Transformation
During import/export, configuration can be transformed:

```bash
# Convert to scientific notation
nlm config export --format json --transform scientific

# Normalize values
nlm config export --format yaml --transform normalize

# Scale numeric values
nlm config export --format ini --transform scale --from 0.0 --to 1.0
```

## Configuration Comparison

### Compare Configurations
```bash
# Compare two configuration files
nlm config compare --file1 config1.json --file2 config2.json

# Generate diff report
nlm config compare --file1 config1.json --file2 config2.json --output diff_report.txt

# Check if configurations are compatible
nlm config compare --file1 config1.json --file2 config2.json --check-compatibility
```

### Comparison Results

```text
Configuration Comparison Report
===============================

File 1: config1.json
File 2: config2.json

Differences:
  simulation.max_steps: 10000 -> 20000 (DIFFERENT)
  plasticity.learning_rate: 0.01 -> 0.05 (DIFFERENT)
  new_key: added
  simulation.old_key: removed

Summary:
  Total Changes: 3
  Added Keys: 1
  Removed Keys: 1
  Modified Keys: 1

Compatibility: INCOMPATIBLE
  Reason: Different learning rates may cause unstable training
```

## Configuration Version Control

### Version History
Configuration versions are tracked:

```bash
# Get version history
nlm config history --file config.json

# Get specific version
nlm config checkout --file config.json --version 3

# List available versions
nlm config list-versions --file config.json
```

### Branching Support
```bash
# Create a new branch
nlm config branch --file config.json --name experiment-a

# Switch branches
nlm config checkout --file config.json --branch experiment-a

# Merge branches
nlm config merge --source experiment-a --target main --file config.json
```

### Diff Between Versions
```bash
# Show differences between versions
nlm config diff --file config.json --version1 1 --version2 3

# Create patch between versions
nlm config patch --file config.json --version1 1 --version2 3 --output patch.txt
```

## Configuration Validation with Neural Constraints

### Neural Constraint Checking
Configuration can be validated against neural system constraints:

```bash
# Check if configuration is biologically plausible
nlm config validate --neural-constraints

# Get neural constraint violations
nlm config validate --neural-constraints --verbose
```

### Constraint Examples
1. **Neuron density constraint**: Max 10 neurons per mm³
2. **Connection probability constraint**: Between 0.01 and 0.5
3. **Learning rate constraint**: Between 0.0001 and 0.1
4. **Memory usage constraint**: Max 8 GB
5. **Temporal resolution constraint**: Min 1 ms timestep

```bash
# Set neural constraints
nlm config set-constraints --neuron-density-max 10 --connection-probability-min 0.01 --learning-rate-max 0.1

# Validate neural constraints
nlm config validate --neural-constraints
```

## Configuration Examples

### Example 1: Basic Learning Simulation
```bash
nlm config save basic.cfg <<EOF
{
  "simulation": {
    "random_seed": 42,
    "max_steps": 5000,
    "timestep": 0.001,
    "neuron_count": 200,
    "region_count": 1
  },
  "plasticity": {
    "learning_rate": 0.01,
    "enable_stdp": true
  }
}
EOF

nlm run --config basic.cfg
```

### Example 2: Pattern Recognition
```bash
nlm config save pattern_recognition.cfg <<EOF
{
  "simulation": {
    "random_seed": 12345,
    "max_steps": 20000,
    "timestep": 0.0005,
    "neuron_count": 1000,
    "region_count": 2
  },
  "plasticity": {
    "learning_rate": 0.005,
    "enable_stdp": true,
    "weight_decay": 0.0001
  },
  "neuromodulation": {
    "enable_novelty": true,
    "novelty_threshold": 0.05
  }
}
EOF

nlm experiment run --experiment phase4 --config pattern_recognition.cfg --steps 20000
```

### Example 3: GPU-Accelerated Training
```bash
nlm config save gpu_training.cfg <<EOF
{
  "simulation": {
    "random_seed": 999,
    "max_steps": 10000,
    "timestep": 0.001,
    "neuron_count": 2000,
    "region_count": 1
  },
  "plasticity": {
    "learning_rate": 0.01,
    "enable_stdp": false
  },
  "performance": {
    "parallel_execution": true,
    "enable_gpu": true,
    "gpu_device": 0,
    "num_processes": 4
  },
  "monitoring": {
    "enable_logging": true,
    "log_file": "gpu_training.log"
  }
}
EOF

nlm run --config gpu_training.cfg
```

### Example 4: Research with Detailed Logging
```bash
nlm config save research.cfg <<EOF
{
  "simulation": {
    "random_seed": 123456,
    "max_steps": 100000,
    "timestep": 0.0001,
    "neuron_count": 5000,
    "region_count": 3
  },
  "development": {
    "enable_critical_period": true,
    "critical_period_strength": 1.2,
    "enable_maturation": true,
    "enable_pruning": true
  },
  "neuromodulation": {
    "enable_novelty": true,
    "enable_reward": true,
    "enable_curiosity": true
  },
  "performance": {
    "parallel_execution": true,
    "num_processes": 8,
    "enable_gpu": true
  },
  "monitoring": {
    "enable_logging": true,
    "log_level": "Debug",
    "log_file": "research_simulation.log",
    "enable_profiling": true
  }
}
EOF

nlm run --config research.cfg
```

## Configuration Command Line Reference

### Configuration Commands

```bash
nlm config <command> [options]

Commands:
  show          Show current configuration
  set <key> <value>  Set configuration value
  load <file>   Load configuration from file
  save <file>   Save configuration to file
  validate      Validate configuration against schema
  export <format>  Export configuration to file
  import        Import configuration from stdin
  list          List configuration keys and sources
  template      Create configuration template
  compare       Compare two configuration files
  merge         Merge configuration files
  diff          Generate configuration diff
  history       Show version history
  branch        Create configuration branch
  checkout      Switch configuration version
  constraints   Set or check constraints
  neural        Validate neural constraints
  help          Show configuration help
```

### Configuration Flags

```bash
Common flags for configuration commands:
  --file <path>      Configuration file path
  --format <format>   Output format (json, yaml, ini)
  --output <file>     Output file
  --verbose           Enable verbose output
  --quiet             Suppress output
  --force             Overwrite existing files
  --backup            Create backup before writing
```

## Configuration Best Practices

### 1. Start with a Template
Always start with a template and customize:

```bash
nlm config template standard
# Modify as needed
nlm config save my_config.cfg
```

### 2. Use Version Control
Track configuration changes:

```bash
# Add to git
nlm config save config.cfg
git add config.cfg
git commit -m "Update configuration"
```

### 3. Validate Before Running
Always validate before running experiments:

```bash
nlm config validate
nlm run --config my_config.cfg
```

### 4. Profile Performance
Profile configuration performance:

```bash
nlm config save performance_profile.cfg
nlm config export --format json --output perf_profile.json
nlm profile cpu --output cpu_profile.json
```

### 5. Document Configuration
Document important configuration decisions:

```bash
# Add comments to configuration file
nlm config save documented_config.cfg <<EOF
{
  "simulation": {
    "random_seed": 42,
    // Use seed 42 for reproducible experiments
    "max_steps": 10000,
    // 10,000 steps for complete learning
    ...
  }
}
EOF
```

### 6. Test Configuration
Test configuration before production:

```bash
# Create test configuration
nlm config template research
# Modify for test
nlm config save test_config.cfg

# Run test experiment
nlm run --config test_config.cfg --steps 100
```

### 7. Monitor Configuration
Monitor configuration effectiveness:

```bash
# Run with monitoring
nlm config save monitored.cfg
nlm run --config monitored.cfg --profile memory

# Check results
nlm profile report --output profile_report.txt
```

## Configuration Troubleshooting

### Common Issues

1. **Invalid JSON**:
   ```bash
   nlm config validate --format json --file broken.json
   ```

2. **Missing Required Keys**:
   ```bash
   nlm config validate --check-required
   ```

3. **Constraint Violations**:
   ```bash
   nlm config validate --neural-constraints
   ```

4. **Inheritance Errors**:
   ```bash
   nlm config load --inherit parent.json child.json
   ```

5. **Export/Import Failures**:
   ```bash
   nlm config export --format json --file config.json
   nlm config import --format json < config.json
   ```

### Getting Help

```bash
# Configuration help
nlm config help

# Command line help
nlm help config

# General help
nlm help
```

## Configuration Integration

### Integration with Other Commands

Configuration files can be integrated with other NLM commands:

```bash
# Run simulation with configuration
nlm run --config sim.cfg --experiment phase3 --steps 10000

# Save experiment results with configuration
nlm experiment run --experiment phase4 --output results.json --config exp.cfg

# Monitor system with configuration
nlm monitor --interval 0.1 --duration 3600 --config monitor.cfg
```

### Configuration in Scripts

```bash
#!/bin/bash
# Run NLM with configuration
nlm run --config simulation.cfg

# Check configuration
nlm config show --config simulation.cfg

# Profile configuration
nlm profile memory --config simulation.cfg

# Debug configuration
nlm debug neural --config simulation.cfg
```

## Configuration Migration

### Migrate from Old Format
```bash
# Migrate from JSON 1.0 to JSON 2.0
nlm config convert --from-format json1 --to-format json2 --input old_config.json --output new_config.json

# Migrate from YAML to JSON
nlm config convert --from-format yaml --to-format json --input config.yaml --output config.json
```

### Migrate from INI
```bash
# Migrate from INI to JSON
nlm config convert --from-format ini --to-format json --input config.ini --output config.json

# Migrate from Windows INI to Unix YAML
nlm config convert --from-format ini --to-format yaml --input windows.ini --output unix_config.yaml
```

## Configuration Security

### Secure Configuration Files
```bash
# Encrypt configuration file
nlm config encrypt --input config.json --output config.json.enc --key mykey

# Decrypt configuration file
nlm config decrypt --input config.json.enc --output config.json --key mykey

# Set file permissions
chmod 600 config.json
```

### Environment Variables
```bash
# Set configuration via environment variable
export NLM_CONFIG="{\"random_seed\": 42, \"neuron_count\": 500}"
nlm run

# Load configuration from environment
nlm config load --env NLM_CONFIG
```

## Configuration Best Practices Summary

1. **Start with templates** and customize
2. **Validate before running** experiments
3. **Use version control** for configuration changes
4. **Document important decisions** in configuration files
5. **Profile performance** regularly
6. **Test configurations** before production use
7. **Monitor configuration effectiveness**
8. **Use inheritance** for common configurations
9. **Validate neural constraints** for biological plausibility
10. **Backup configurations** regularly

## Configuration Examples Gallery

The `configs/` directory contains example configurations:

- `default.cfg` - Basic default configuration
- `research.cfg` - Research-optimized configuration
- `performance.cfg` - Performance-optimized configuration
- `gpu.cfg` - GPU-accelerated configuration
- `demo.cfg` - Demo/configuration for tutorials

## Configuration File Format Reference

### JSON
```json
{
  "simulation": { ... },
  "plasticity": { ... },
  "neuromodulation": { ... },
  "development": { ... },
  "performance": { ... },
  "monitoring": { ... }
}
```

### YAML
```yaml
simulation:
  random_seed: 42
  max_steps: 10000
  timestep: 0.001

plasticity:
  learning_rate: 0.01
  enable_stdp: true
```

### INI
```ini
[simulation]
random_seed = 42
max_steps = 10000
timestep = 0.001

[plasticity]
learning_rate = 0.01
enable_stdp = true
```

Configuration files should always be saved with appropriate file extensions (`.cfg`, `.json`, `.yaml`, `.yml`, `.ini`).

## Configuration Integration with Documentation

Configuration settings should be documented in:

1. **CLI User Guide** (`docs/cli_user_guide.md`) - Command-line usage
2. **Configuration Guide** (`docs/configuration_guide.md`) - This file
3. **Experiment Protocol** (`docs/experiment_protocol.md`) - Protocol specifications
4. **API Reference** - Programmatic API documentation

This comprehensive configuration system makes NLM a flexible, powerful, and production-ready computational neuroscience platform for both beginners and power users.