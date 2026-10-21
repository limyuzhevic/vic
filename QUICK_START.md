# NLM Quick Start Guide

## Quick Start for Users

This guide covers the basic usage of NLM (Neural Learning Machine) with the improved configuration and command-line interface.

## Basic Usage

### Building and Running

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
# Use -DCMAKE_BUILD_TYPE=Debug for development
# Use -DCMAKE_BUILD_TYPE=Release for production
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with parallel jobs
make -j4
```

### Running Experiments

The main executable (`./nlm`) runs Phase 2 experiments by default:

```bash
# Run basic integration tests
./nlm
```

This will run:
1. Basic connectivity test
2. Plasticity learning experiment  
3. STDP verification

### Available Command-Line Options

#### General Options

```bash
# Show help information
./nlm --help
./nlm -h
```

```bash
# Set verbosity level
./nlm -v     # Basic info
./nlm -vv    # Detailed info
./nlm -vvv   # Debug info
```

#### Configuration Options

```bash
# Load custom configuration file
./nlm --config=myconfig.cfg

# Override configuration via command line
./nlm --random_seed=123 --neuron_count=1000

# Use short format
./nlm -random_seed 456 -neuron_count 2000
```

#### Help for Specific Options

```bash
# Get help for specific configuration options
./nlm --help-config
```

## Configuration Files

### Default Configuration

NLM uses several configuration sources (loaded in this order):

1. **Command Line Arguments** (highest priority)
2. **Config File** (default: `configs/default.cfg`)
3. **Built-in Defaults** (lowest priority)

### Configuration File Format

Configuration files use simple `key = value` format:

```ini
# Example config file (configs/default.cfg)
# Configuration settings for NLM simulation

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds  
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 500

# Number of neural regions
region_count = 1

# Probability of synaptic connections
connection_probability = 0.15

# STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

### Available Configuration Options

#### Simulation Parameters

- `random_seed`: Random seed for reproducible experiments
- `simulation_timestep`: Time step for neural simulation (seconds)
- `neuron_count`: Total number of neurons in the brain
- `region_count`: Number of neural regions
- `connection_probability`: Probability of synaptic connections

#### Plasticity Parameters

- `stdp_ltp_weight`: STDP Long-Term Potentiation weight
- `stdp_ltd_weight`: STDP Long-Term Depression weight
- `stdp_tau`: STDP time constant (ms)
- `synaptogenesis_rate`: Rate of new synapse formation
- `pruning_rate`: Rate of synapse elimination

#### Logging and Debugging

- `log_level`: Logging level (ERROR, WARN, INFO, DEBUG)
- `log_file`: Log output file path
- `verbose`: Verbosity level (0-3)

## Advanced Usage

### Running Different Experiment Modes

```bash
# Run with experiment mode
./nlm --mode=connect --config=experiments/connectivity_test.cfg

# Run with custom parameters
./nlm --mode=learning --timestep=0.01 --neurons=2000

# Profile performance
./nlm --profile --output=performance.json
```

### Configuration File Discovery

NLM automatically searches for configuration files in this order:

1. Command line `--config` parameter
2. `configs/default.cfg` (project root)
3. `configs/config.cfg` (project root)
4. `nlm.cfg` (project root)
5. `.nlm.cfg` (project root)

### Environment Variables

NLM supports environment variable overrides:

```bash
export NLM_LOG_LEVEL=INFO
export NLM_RANDOM_SEED=12345
./nlm --custom_param=value
```

### Experiment Results

NLM exports detailed results:

- **Console Output**: Detailed experiment logs
- **Configuration Summary**: All loaded configuration parameters
- **Performance Metrics**: Timing and resource usage
- **Brain State**: Neural connectivity and activity data

## Configuration File Examples

### Basic Configuration

```ini
# Basic simulation configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15
```

### Learning Experiment Configuration

```ini
# Configuration for learning experiments
mode = learning
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
epoch_count = 100
train_steps = 1000
```

### Debugging Configuration

```ini
# Configuration for debugging and development
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.5
log_level = DEBUG
verbose = 2
checkpoint_enabled = true
checkpoint_interval = 100
```

## Troubleshooting

### Common Issues

#### Configuration File Not Found
```bash
# Use absolute path or copy file to configs/ directory
./nlm --config=/full/path/to/config.cfg
```

#### Invalid Configuration Values
```bash
# Check configuration format and values
# Use --verbose for detailed error messages
./nlm --config=bad_config.cfg
```

#### Performance Issues
```bash
# Reduce neuron count for faster execution
./nlm --neuron_count=100

# Use Release build for better performance
mkdir build_release
cd build_release
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Getting Help

1. **General Help**: `./nlm --help`
2. **Configuration Help**: `./nlm --help-config`
3. **Troubleshooting**: Check the NLM documentation
4. **Bugs**: Report issues on the NLM GitHub repository

## Experiment Modes

NLM supports different experiment modes:

- `connect`: Basic connectivity tests
- `learning`: Plasticity learning experiments
- `stdp`: STDP verification
- `integration`: Full integration tests
- `custom`: User-defined experiment configurations

## Quick Tips

1. **Always specify configuration files**: Use `--config` to control experiments
2. **Use verbosity levels**: `-v`, `-vv`, `-vvv` for detailed information
3. **Reproducible experiments**: Set `random_seed` for consistent results
4. **Performance optimization**: Use Release builds for production
5. **Debugging**: Use DEBUG configurations for development

## Next Steps

1. **Read the Architecture Documentation**: `docs/ARCHITECTURE.md`
2. **Explore Advanced Features**: Learn about neuromodulation, memory, and prediction
3. **Create Custom Experiments**: Design your own experiment configurations
4. **Contribute**: Add new configuration options or features
5. **Research**: Use NLM for neuroscience research or AI development

## Related Resources

- **Documentation**: `docs/` directory
- **Configuration Examples**: `configs/` directory
- **Source Code**: `src/` directory
- **Tests**: `tests/` directory
- **Issues**: GitHub repository issues
- **Discussions**: GitHub discussions

## License

MIT License. See `LICENSE` file for details.

## Contact

For questions or support:
- GitHub repository: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Issues: Submit issues on GitHub

---

*Version: Phase 6 Final Integration*
*Last Updated: October 2026*