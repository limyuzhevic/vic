# NLM User Guide: Neural Learning Machine Examples

This guide provides examples for getting started with NLM (Neural Learning Machine).

## Quick Start

### Running Basic Simulation

```bash
# Run default simulation
./nlm

# With verbose logging
./nlm -v

# With custom config file
./nlm --config my_config.cfg

# Run integration tests
./nlm --test
```

### Common Command Line Options

- `--config=file` - Specify configuration file
- `--config file` - Alternative syntax
- `-v, --verbose` - Enable verbose logging
- `-t, --test` - Run integration tests
- `-h, --help` - Show help message

## Configuration Examples

### Basic Configuration (default.cfg)

```
# Basic neural network setup
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15

# Plasticity rules
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015

# Neuromodulation
dopamine_baseline = 0.1
curiosity_baseline = 0.05
```

### Advanced Configuration

```
# Performance optimizations
use_simd_optimization = true
use_parallel_processing = false
memory_pool_size = 1000
event_queue_capacity = 5000

# Memory systems
working_memory_size = 50
episodic_memory_capacity = 1000
associative_memory_pattern_count = 500

# Sensory and motor parameters
sensory_noise_level = 0.01
motor_noise_level = 0.02

# Development parameters
development_maturation_rate = 0.1
development_plasticity_decay = 0.01
```

## Running Integration Tests

To run all integration tests:

```bash
./nlm --test
```

This will run:
- Configuration tests
- Random number generator tests
- Clock tests
- Neuron tests
- Synapse tests
- STDP plasticity tests
- Brain integration tests

## Example: Running with Custom Parameters

```bash
# Create a custom config file
cat > my_config.cfg << EOF
random_seed = 123
simulation_timestep = 0.002
neuron_count = 1000
region_count = 2
log_level = DEBUG
sensory_noise_level = 0.05
motor_noise_level = 0.03
EOF

# Run with custom config
./nlm --config my_config.cfg -v
```

## Technical Notes

### Configuration File Format

The configuration file uses a simple key=value format:

```
key = value
```

Values can be:
- Integers: `42`
- Floats: `3.14`
- Booleans: `true` or `false`
- Strings: `"hello"` (with quotes)
- Arrays: `[1, 2, 3]` or `["a", "b", "c"]`

### Command Line Arguments

Command line arguments override configuration file values:

```bash
# Override specific values
./nlm --neuron-count=2000 --region-count=3 --log-level=INFO

# Short form
./nlm -n 2000 -r 3 -v
```

### Advanced Usage

For advanced users, you can modify the configuration to:

1. **Enable visualization**: Set `visualization_enabled = true`
2. **Enable logging to file**: Set `log_to_file = true`
3. **Increase memory capacity**: Adjust memory pool sizes
4. **Enable parallel processing**: Set `use_parallel_processing = true`
5. **Adjust noise levels**: Modify sensory and motor noise parameters

## Troubleshooting

### Common Issues

1. **Configuration file not found**
   ```
   # Create the file or specify correct path
   ./nlm --config path/to/config.cfg
   ```

2. **Invalid configuration values**
   ```
   # Check data types in config file
   # Use getOr() for safe access with defaults
   ```

3. **Tests failing**
   ```
   # Run with verbose output to see failures
   ./nlm --test -v
   ```

### Getting Help

```bash
# Show help message
./nlm -h

# Show advanced help
./nlm --help
```

## API Reference

### Key Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `random_seed` | int | 42 | Seed for random number generator |
| `simulation_timestep` | double | 0.001 | Time step in seconds |
| `neuron_count` | int64 | 500 | Number of neurons |
| `region_count` | int64 | 1 | Number of brain regions |
| `connection_probability` | float | 0.15 | Probability of synapse creation |

## Next Steps

1. **Explore the source code** - Look at `src/` for implementation details
2. **Run the integration tests** - Verify everything works correctly
3. **Modify configurations** - Customize for your specific use case
4. **Extend the system** - Add new plasticity rules or neuromodulators

## Version Information

This is Phase 6 of the NLM project - Final Integration.
All major systems (memory, prediction, cognition, neuromodulation) are integrated and working.

## License

MIT License - See LICENSE file for details.