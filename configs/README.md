# NLM Configuration File Examples

This directory contains example configuration files for the NLM (熙然) project.

## Default Configuration (configs/default.cfg)

```ini
; NLM - Neural Learning Machine Configuration
; Phase 2: Real Neural Computation

; Basic simulation parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15

; STDP (Spike-Timing-Dependent Plasticity) parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

; Structural plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

; Development system parameters
development_rate = 0.001
maturation_rate = 0.0005

; Performance settings
max_steps = 2000
enable_checkpointing = false
checkpoint_path = /tmp/nlm_checkpoint.bin

; Neuromodulation settings
enable_reward_modulation = true
enable_curiosity = true
enable_novelty_detection = true

; Logging configuration
log_level = info
log_file = nlm.log
console_output = true

; World and environment settings
world_size = 16,16,16,16
enable_graphics = false
enable_sensor_simulation = true

; Memory system parameters
working_memory_capacity = 100
episodic_memory_capacity = 1000
replay_frequency = 0.1

; Learning parameters
hebbian_learning_rate = 0.01
stdp_learning_rate = 0.02

; Development parameters
developmental_stages = 10
development_plasticity_multiplier = 1.5
age_based_pruning = true
```

## Performance Configuration (configs/performance.cfg)

```ini
; High-performance NLM configuration optimized for large-scale simulations

; Performance-critical parameters
random_seed = 12345
simulation_timestep = 0.0005  ; Smaller timestep for accuracy
neuron_count = 2000            ; Larger network
region_count = 4               ; Multiple regions for parallel processing
connection_probability = 0.20  ; Higher connectivity

; Aggressive learning parameters
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.025
stdp_tau = 30.0

; Fast structural plasticity
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002

; Development enabled for plasticity
sydevelopment_rate = 0.002
maturation_rate = 0.001

; Performance optimizations
enable_checkpointing = true
checkpoint_path = /tmp/nlm_perf_checkpoint.bin
max_steps = 10000
enable_threads = true
thread_count = 8

; Logging for performance analysis
log_level = debug
log_file = nlm_performance.log
console_output = false
```

## Research Configuration (configs/research.cfg)

```ini
; Research configuration for studying specific neural phenomena

; Controlled experimental parameters
random_seed = 999
simulation_timestep = 0.002
neuron_count = 100              ; Smaller for controlled experiments
region_count = 1                 ; Single region for isolation
connection_probability = 0.10   ; Lower connectivity for study

; Precise STDP parameters for research
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 50.0                 ; Longer timescale for research

; Development system enabled
development_rate = 0.001
maturation_rate = 0.0003
enable_development = true

; Memory system focused on specific behaviors
enable_replay = false           ; Disable for clean experimental conditions
replay_frequency = 0.0
working_memory_capacity = 50
episodic_memory_capacity = 200

; Research-specific logging
log_level = info
log_file = nlm_research.log
console_output = true
export_metrics = true
metrics_format = json

; Experimental control
max_steps = 5000
checkpoint_path = /tmp/nlm_research_checkpoint.bin
enable_checkpointing = true
```

## Quick Configuration (configs/quick.cfg)

```ini
; Quick test configuration for fast validation

random_seed = 42
simulation_timestep = 0.001
neuron_count = 100              ; Small for speed
region_count = 1
connection_probability = 0.15

; Basic plasticity
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

; Fast simulation
max_steps = 500
enable_checkpointing = false

; Minimal logging for quick runs
log_level = error
log_file = nlm_quick.log
console_output = false
```

## Configuration File Format

NLM configuration files use INI format with the following sections:

### Required Parameters
- `random_seed`: Integer for reproducible results
- `simulation_timestep`: Float for integration timestep
- `neuron_count`: Integer for network size
- `region_count`: Integer for number of neural regions

### Optional Parameters
All other parameters have sensible defaults and can be omitted.

## Command Line Overrides

Command line arguments override configuration file settings:

```bash
./nlm --config configs/default.cfg --seed 123 --neurons 1000 --steps 5000 --checkpoint-enable
```

## Configuration Validation

To validate a configuration file:

```bash
./nlm --dry-run --config configs/my_config.cfg
```

This will check for:
- Valid parameter types and ranges
- File accessibility
- Parameter consistency
- Resource availability

## Configuration Management

### Loading Order
1. Default configuration
2. Configuration file (if specified)
3. Command line arguments (highest priority)

### Configuration Sources
- `configs/default.cfg`: Built-in defaults
- `configs/*.cfg`: User-defined configurations
- Command line: Runtime overrides

### Configuration Variables

#### Basic Simulation
- `random_seed`: Random number generator seed
- `simulation_timestep`: Integration timestep (seconds)
- `neuron_count`: Total neurons in network
- `region_count`: Number of neural regions
- `connection_probability`: Synapse formation probability

#### Plasticity Systems
- `stdp_ltp_weight`: STDP long-term potentiation weight
- `stdp_ltd_weight`: STDP long-term depression weight
- `stdp_tau`: STDP timescale (ms)
- `synaptogenesis_rate`: New synapse formation rate
- `pruning_rate`: Synapse elimination rate

#### Development System
- `development_rate`: Overall development rate
- `maturation_rate`: Neural maturation rate
- `enable_development`: Enable/disable development

#### Memory Systems
- `working_memory_capacity`: Working memory size
- `episodic_memory_capacity`: Episodic memory size
- `replay_frequency`: Memory replay frequency

#### Performance
- `max_steps`: Maximum simulation steps
- `enable_checkpointing`: Enable checkpoint saving
- `checkpoint_path`: Checkpoint file path

#### Neuromodulation
- `enable_reward_modulation`: Reward-based learning
- `enable_curiosity`: Curiosity-driven exploration
- `enable_novelty_detection`: Novelty-based learning

#### Logging
- `log_level`: Logging verbosity (trace, debug, info, warning, error, critical)
- `log_file`: Log file path
- `console_output`: Enable/disable console output

## Configuration Templates

### For Research
Use `configs/research.cfg` for controlled experiments with precise parameters.

### For Performance
Use `configs/performance.cfg` for large-scale simulations with optimized settings.

### For Development
Use `configs/default.cfg` for general development and testing.

### For Quick Testing
Use `configs/quick.cfg` for fast validation and prototyping.

## Configuration Examples

### Example 1: Basic Phase 2 Testing
```bash
./nlm --phase2-complete --config configs/default.cfg --seed 42 --neurons 500
```

### Example 2: Phase 6 Integration
```bash
./nlm --phase6 --config configs/research.cfg --checkpoint-enable --verbose
```

### Example 3: Benchmark with Custom Settings
```bash
./nlm --benchmark --config configs/performance.cfg --threads 4 --output benchmark.json
```

### Example 4: Dry Run Validation
```bash
./nlm --dry-run --config configs/my_config.cfg --neurons 1000 --steps 10000
```

### Example 5: Auto Mode with Progress
```bash
./nlm --auto --verbose --output results.json --metrics metrics.csv
```

## Configuration Troubleshooting

### Common Issues

#### Configuration File Not Found
```bash
# Check if file exists
ls -la configs/

# Or provide full path
./nlm --config /path/to/config.cfg
```

#### Invalid Parameter Values
```bash
# Check parameter types and ranges
# Random seed must be integer
./nlm --seed abc  # Error: invalid seed

# Neuron count must be positive
./nlm --neurons -100  # Error: invalid neuron count
```

#### File Permission Issues
```bash
# Check write permissions
./nlm --config configs/default.cfg --checkpoint-enable
# Error: Cannot write to /tmp/nlm_checkpoint.bin
```

### Configuration Validation Script

Create a validation script for consistent configuration:

```bash
#!/bin/bash

CONFIG_FILE="$1"

if [ ! -f "$CONFIG_FILE" ]; then
    echo "Error: Configuration file not found: $CONFIG_FILE"
    exit 1
fi

echo "Validating configuration: $CONFIG_FILE"
./nlm --dry-run --config "$CONFIG_FILE"

if [ $? -eq 0 ]; then
    echo "Configuration validation successful"
else
    echo "Configuration validation failed"
    exit 1
fi
```

## Configuration Best Practices

1. **Version Control**: Keep configuration files in version control
2. **Environment-Specific**: Use different configs for development, testing, production
3. **Parameter Documentation**: Document all configuration parameters
4. **Validation**: Always validate configurations before running
5. **Backup**: Regularly backup configuration files
6. **Performance**: Profile configurations for optimal performance
7. **Testing**: Test configurations with representative workloads

## Configuration File Schema

```json
{
    "nlm_config": {
        "version": "2.0",
        "parameters": {
            "random_seed": {"type": "integer", "min": 0},
            "simulation_timestep": {"type": "float", "min": 0.0001, "max": 0.1},
            "neuron_count": {"type": "integer", "min": 1, "max": 10000},
            "region_count": {"type": "integer", "min": 1, "max": 100},
            "connection_probability": {"type": "float", "min": 0.0, "max": 1.0},
            "stdp_ltp_weight": {"type": "float", "min": 0.0},
            "stdp_ltd_weight": {"type": "float", "min": 0.0},
            "stdp_tau": {"type": "float", "min": 1.0},
            "synaptogenesis_rate": {"type": "float", "min": 0.0},
            "pruning_rate": {"type": "float", "min": 0.0},
            "development_rate": {"type": "float", "min": 0.0},
            "maturation_rate": {"type": "float", "min": 0.0},
            "max_steps": {"type": "integer", "min": 1},
            "enable_checkpointing": {"type": "boolean"},
            "log_level": {"type": "enum", "values": ["trace", "debug", "info", "warning", "error", "critical"]}
        }
    }
}
```

## Configuration File Migration

### From Version 1.0 to 2.0

- New parameter `development_rate` added
- New parameter `maturation_rate` added
- Parameter `max_steps` moved from required to optional
- Parameter `enable_checkpointing` moved from optional to commonly used
- Parameter `checkpoint_path` added for custom checkpoint locations

### Migration Script

```bash
#!/bin/bash

# Convert old config to new format

OLD_CONFIG="$1"
NEW_CONFIG="${2:-$OLD_CONFIG.new}"

if [ ! -f "$OLD_CONFIG" ]; then
    echo "Error: Old configuration file not found: $OLD_CONFIG"
    exit 1
fi

cat "$OLD_CONFIG" > "$NEW_CONFIG"

# Add missing parameters if needed
sed -i 's/^[[:space:]]*$/random_seed = 42\n&/' "$NEW_CONFIG"

# Set defaults for new parameters
grep -q "development_rate" "$NEW_CONFIG" || echo -e "\ndevelopment_rate = 0.001\nmaturation_rate = 0.0005" >> "$NEW_CONFIG"

echo "Migration complete: $OLD_CONFIG -> $NEW_CONFIG"
```

## Configuration File Security

### Sensitive Parameters
Do not commit the following to version control:
- `random_seed` (can affect experimental reproducibility)
- File paths in `checkpoint_path`
- Log file paths

### Secure Configuration
```ini
; Development configuration
random_seed = 123456

; Production configuration (do not commit)
# random_seed = 999999
# checkpoint_path = /secure/nlm_checkpoint.bin
# log_file = /secure/nlm.log
```

## Configuration Monitoring

### Configuration Change Detection
```bash
# Monitor configuration changes
git diff --name-only HEAD~1 -- configs/

# Alert on configuration changes
# (add to CI/CD pipeline)
```

### Performance Monitoring
```bash
# Monitor configuration performance
./nlm --benchmark --config configs/performance.cfg --output benchmark.json

# Analyze configuration impact
cat benchmark.json | python analyze_config.py
```

This directory provides comprehensive configuration management for the NLM project, supporting different use cases from quick testing to large-scale research simulations.