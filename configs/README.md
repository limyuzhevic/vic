# NLM Configuration File Examples

This directory contains example configuration files for the NLM (Neural Learning Machine) project.
Each file demonstrates different configuration scenarios for various use cases.

## Configuration Overview

NLM uses key-value pairs in plain text format for configuration. Each line should be:
    key = value  # optional comment

Keys include:
- random_seed: Random number generator seed (int64_t)
- simulation_timestep: Time step duration in seconds (double)
- neuron_count: Number of neurons in the brain (int64_t)
- region_count: Number of neural regions (int64_t)
- connection_probability: Probability of synapse creation (float)
- stdp_ltp_weight: STDP long-term potentiation weight (float)
- stdp_ltd_weight: STDP long-term depression weight (float)
- stdp_tau: STDP time constant (float)
- synaptogenesis_rate: Rate of new synapse creation (float)
- pruning_rate: Rate of synapse elimination (float)
- replay_interval: Steps between memory replay cycles (int64_t)
- consolidation_interval: Steps between memory consolidation (int64_t)
- checkpoint_dir: Directory for saving brain state (string)
- checkpoint_interval: Steps between automatic checkpoints (int64_t)

## Configuration Files

### configs/default.cfg - Standard Configuration

This is the recommended configuration for general-purpose NLM operation.

random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
replay_interval = 100
consolidation_interval = 1000
checkpoint_dir = ./checkpoints
checkpoint_interval = 10000

### configs/small.cfg - Resource-Constrained Configuration

Optimized for systems with limited memory and processing power.

random_seed = 123
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 15.0
synaptogenesis_rate = 0.00005
pruning_rate = 0.000005
replay_interval = 50
consolidation_interval = 500
checkpoint_dir = ./checkpoints_small
checkpoint_interval = 5000

### configs/large.cfg - High-Performance Configuration

Designed for systems with substantial computational resources and memory.

random_seed = 999
simulation_timestep = 0.0005
neuron_count = 2000
region_count = 2
connection_probability = 0.2
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
stdp_tau = 25.0
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002
replay_interval = 200
consolidation_interval = 2000
checkpoint_dir = ./checkpoints_large
checkpoint_interval = 20000

### configs/learning.cfg - Intensive Learning Configuration

Optimized for maximum learning and memory consolidation.

random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.18
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.025
stdp_tau = 30.0
synaptogenesis_rate = 0.0003
pruning_rate = 0.00001
replay_interval = 50
consolidation_interval = 200
checkpoint_dir = ./checkpoints_learning
checkpoint_interval = 2000

### configs/development.cfg - Development/Debug Configuration

Configuration optimized for development, debugging, and experimentation.

random_seed = 0  # 0 means random seed based on time
simulation_timestep = 0.001
neuron_count = 50
region_count = 1
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 10.0
synaptogenesis_rate = 0.00001
pruning_rate = 0.000001
replay_interval = 25
consolidation_interval = 100
checkpoint_dir = ./checkpoints_dev
checkpoint_interval = 1000

## Using Configuration Files

1. Place your configuration file in the `configs/` directory
2. Pass it to NLM using the `--config` flag:
   ```
   ./nlm --config configs/learning.cfg
   ```

3. Or run without a config file to use defaults.

## Advanced Configuration Options

### Command Line Overrides

Command line arguments override configuration file settings:

```
./nlm --config configs/small.cfg --neurons 200 --seed 777 --demo-mode plasticity
```

This creates a 200-neuron brain with seed 777 and runs the plasticity demo.

### Dynamic Configuration

Some configuration values can be dynamically adjusted during runtime by modifying the configuration object programmatically (for advanced use).

## Configuration Validation

NLM performs basic validation:
- Neuron count must be positive
- Simulation timestep must be positive
- Random seed can be 0 (time-based) or any positive value
- Connection probability should be between 0.0 and 1.0
- Plasticity rates should be small positive values

## Best Practices

1. Use `random_seed` values for reproducible experiments
2. Match `neuron_count` to your system's memory constraints
3. Adjust `connection_probability` to control network density
4. Use smaller configurations for debugging and rapid iteration
5. Use larger configurations for final experiments and production runs
6. Always test with `configs/development.cfg` before running large experiments
7. Save important brain states using checkpointing

## Additional Configuration Resources

- Documentation in the `docs/` directory
- Configuration options are documented in the source code
- Command-line help can be displayed with `--help`

## Migration Notes

When upgrading NLM versions:
1. Review configuration changes in documentation
2. Update configuration files if new options are required
3. Test configuration with `--demo-mode basic` before running full experiments
4. Use checkpoints to preserve learned states between versions

Configuration files are for quick setup, but fine-tuning should be done through code modification for maximum performance and correctness.
