# NLM Configuration File Documentation

## Overview

This document describes all configuration parameters for the NLM (Neural Learning Machine) neural simulation framework. Configuration files can be in JSON, YAML, or simple key=value format.

## Configuration Structure

NLM uses a key-value configuration system with support for multiple data types:
- **Integer** (int, int64_t)
- **Floating-point** (double)
- **Boolean** (bool)
- **String** (std::string)
- **Arrays** (std::vector<int>, std::vector<double>, std::vector<std::string>)

## Core Configuration Parameters

### Simulation Parameters

#### `random_seed` (int64_t, default: 42)
**Description:** Random seed for reproducible experiments
**Range:** Any 64-bit integer
**Example:** `random_seed = 12345`

#### `simulation_timestep` (double, default: 0.001)
**Description:** Time step for simulation in seconds
**Range:** (0.0, 1.0]
**Example:** `simulation_timestep = 0.0001`

#### `max_simulation_steps` (int64_t, default: 10000)
**Description:** Maximum number of simulation steps
**Range:** ≥ 0
**Example:** `max_simulation_steps = 50000`

### Network Architecture

#### `neuron_count` (int64_t, default: 1000)
**Description:** Total number of neurons in the brain
**Range:** ≥ 1
**Example:** `neuron_count = 5000`

#### `region_count` (int64_t, default: 2)
**Description:** Number of brain regions
**Range:** ≥ 1
**Example:** `region_count = 4`

#### `connection_probability` (double, default: 0.1)
**Description:** Probability of synaptic connections between neurons
**Range:** [0.0, 1.0]
**Example:** `connection_probability = 0.15`

### Plasticity Parameters

#### `plasticity_learning_rate` (double, default: 0.01)
**Description:** General learning rate for plasticity
**Range:** [0.0, 1.0]
**Example:** `plasticity_learning_rate = 0.05`

#### `stdp_ltp_weight` (double, default: 0.01)
**Description:** Weight for Spike-Timing-Dependent Plasticity Long-Term Potentiation
**Range:** [0.0, 1.0]
**Example:** `stdp_ltp_weight = 0.02`

#### `stdp_ltd_weight` (double, default: 0.012)
**Description:** Weight for STDP Long-Term Depression
**Range:** [0.0, 1.0]
**Example:** `stdp_ltd_weight = 0.018`

#### `stdp_tau` (double, default: 20.0)
**Description:** Time constant for STDP in milliseconds
**Range:** > 0.0
**Example:** `stdp_tau = 25.0`

#### `synaptogenesis_rate` (double, default: 0.001)
**Description:** Rate of new synapse formation
**Range:** [0.0, 1.0]
**Example:** `synaptogenesis_rate = 0.002`

#### `pruning_rate` (double, default: 0.0001)
**Description:** Rate of synapse elimination
**Range:** [0.0, 1.0]
**Example:** `pruning_rate = 0.0002`

### Neuromodulation Parameters

#### `dopamine_baseline` (double, default: 0.1)
**Description:** Baseline dopamine level
**Range:** [0.0, 10.0]
**Example:** `dopamine_baseline = 0.5`

#### `reward_discount_factor` (double, default: 0.99)
**Description:** Discount factor for reward prediction
**Range:** (0.0, 1.0]
**Example:** `reward_discount_factor = 0.95`

### Development Parameters

#### `developmental_stage` (string, default: "Maturation")
**Description:** Current developmental stage
**Valid values:** "Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"
**Example:** `developmental_stage = "Adult"`

#### `neural_excitability_change_rate` (double, default: 0.0)
**Description:** Rate of change in neural excitability during development
**Range:** [-1.0, 1.0]
**Example:** `neural_excitability_change_rate = 0.1`

### Environment Parameters

#### `environment_name` (string, default: "GridWorld")
**Description:** Name of the simulation environment
**Example:** `environment_name = "OpenArena"`

#### `environment_width` (int64_t, default: 10)
**Description:** Width of the simulation environment
**Range:** ≥ 1
**Example:** `environment_width = 20`

#### `environment_height` (int64_t, default: 10)
**Description:** Height of the simulation environment
**Range:** ≥ 1
**Example:** `environment_height = 20`

### Logging Parameters

#### `log_level` (string, default: "INFO")
**Description:** Logging level
**Valid values:** "ERROR", "WARNING", "INFO", "DEBUG"
**Example:** `log_level = "DEBUG"`

#### `log_to_file` (bool, default: false)
**Description:** Whether to log to file
**Example:** `log_to_file = true`

#### `log_filename` (string, default: "nlm.log")
**Description:** Log file name
**Example:** `log_filename = "brain_simulation.log"`

### Experimental Parameters

#### `experiment_name` (string, default: "default")
**Description:** Name of the experiment
**Example:** `experiment_name = "learning_study"`

#### `record_every_n_steps` (int64_t, default: 100)
**Description:** Record data every N simulation steps
**Range:** ≥ 1
**Example:** `record_every_n_steps = 50`

## Command Line Options

### General Options

#### `--config=<file>` or `-config <file>`
**Description:** Load configuration from file
**Example:** `--config=my_config.cfg`

#### `--help` or `-h`
**Description:** Show help message
**Example:** `nlm --help`

### Configuration Overrides

#### `--neuron-count=<num>` or `-neuron-count <num>`
**Description:** Override neuron count
**Example:** `--neuron-count=2000`

#### `--region-count=<num>` or `-region-count <num>`
**Description:** Override region count
**Example:** `--region-count=3`

#### `--connection-probability=<float>` or `-connection-probability <float>`
**Description:** Override connection probability
**Example:** `--connection-probability=0.2`

#### `--random-seed=<num>` or `-random-seed <num>`
**Description:** Override random seed
**Example:** `--random-seed=42`

#### `--simulation-timestep=<float>` or `-simulation-timestep <float>`
**Description:** Override simulation timestep
**Example:** `--simulation-timestep=0.0005`

### Boolean Flags

#### `--enable-plasticity` or `--plasticity`
**Description:** Enable plasticity
**Example:** `nlm --enable-plasticity`

#### `--disable-plasticity` or `--no-plasticity`
**Description:** Disable plasticity
**Example:** `nlm --disable-plasticity`

#### `--enable-neuromodulation` or `--neuromodulation`
**Description:** Enable neuromodulation
**Example:** `nlm --enable-neuromodulation`

#### `--enable-development` or `--development`
**Description:** Enable developmental processes
**Example:** `nlm --enable-development`

#### `--enable-curiosity` or `--curiosity`
**Description:** Enable curiosity-driven exploration
**Example:** `nlm --enable-curiosity`

#### `--enable-reward-modulation` or `--reward-modulation`
**Description:** Enable reward-based modulation
**Example:** `nlm --enable-reward-modulation`

## Configuration File Examples

### Simple Key=Value Format

```
# NLM Configuration File
# Basic settings
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018

# Neuromodulation
dopamine_baseline = 0.5
reward_discount_factor = 0.95

# Environment
environment_name = GridWorld
environment_width = 20
environment_height = 20

# Logging
log_level = INFO
log_to_file = true
log_filename = simulation.log
```

### JSON Format

```json
{
    "random_seed": 12345,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.018,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.001,
    "pruning_rate": 0.0001,
    "dopamine_baseline": 0.5,
    "reward_discount_factor": 0.95,
    "environment_name": "GridWorld",
    "environment_width": 20,
    "environment_height": 20,
    "log_level": "INFO",
    "log_to_file": true,
    "log_filename": "simulation.log",
    "max_simulation_steps": 10000
}
```

### YAML Format

```yaml
# NLM Configuration
random_seed: 12345
simulation_timestep: 0.001
neuron_count: 1000
region_count: 2

# Plasticity
plasticity:
  learning_rate: 0.01
  stdp:
    ltp_weight: 0.02
    ltd_weight: 0.018
    tau: 20.0

# Neuromodulation
neuromodulation:
  dopamine_baseline: 0.5
  reward_discount_factor: 0.95

# Environment
environment:
  name: GridWorld
  width: 20
  height: 20

# Logging
logging:
  level: INFO
  to_file: true
  filename: simulation.log

# Experimental
experimental:
  max_steps: 10000
```

## Configuration Validation

NLM provides configuration validation for common parameter types:

1. **Bounds checking:** Ensures values are within acceptable ranges
2. **Type checking:** Validates that values match expected types
3. **Dependency checking:** Ensures dependent parameters are consistent
4. **Uniqueness checking:** Ensures unique names where required

## Configuration Saving

Configuration can be saved using the `saveToFile()` method, which always saves in JSON format for compatibility. Example:

```cpp
Config config;
config.set("neuron_count", 2000);
config.set("connection_probability", 0.15);
config.saveToFile("my_config.json");
```

## Error Handling

Configuration errors are logged with descriptive messages:
- **Missing required keys:** When validation fails
- **Invalid values:** When values don't match expected types or ranges
- **File errors:** When config files cannot be read/written
- **Parse errors:** When configuration files are malformed

## Best Practices

1. **Use JSON for configuration files** - JSON is more robust and widely supported
2. **Store secrets in environment variables** - Never hardcode sensitive values in config files
3. **Use descriptive keys** - Use clear, self-documenting configuration keys
4. **Validate critical parameters** - Always validate essential parameters like neuron_count > 0
5. **Test configurations** - Validate configuration files before running simulations
6. **Use version control** - Track configuration changes in version control
7. **Document parameters** - Maintain documentation of all configuration parameters

## File Extensions

- **Simple format:** `.cfg`, `.ini`, `.properties`
- **JSON format:** `.json`
- **YAML format:** `.yaml`, `.yml`

NLM automatically detects the format based on file extension and content.
