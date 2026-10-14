# NLM Enhanced Setup Guide

## Welcome to the Enhanced NLM (熙然) Neural Learning Machine!

This guide will help you get started with the enhanced version of NLM, featuring a new command-line interface, advanced configuration options, and comprehensive testing capabilities.

## Quick Start

### 1. Build the Project

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### 2. Run the Enhanced CLI

The enhanced CLI (`./nlm`) is now the default executable:

```bash
# Run Phase 6 integration demo (recommended)
./nlm --mode phase6 --preset performance --max-steps 5000

# Run Phase 2 basic demo
./nlm --mode phase2 --preset minimal

# Run with custom parameters
./nlm --mode custom --neuron-count 100 --region-count 2 --preset research
```

### 3. Get Help

```bash
# Show all command-line options
./nlm --help

# Show version information
./nlm --version

# Run with verbose output
./nlm --mode phase6 --verbose
```

## Key Features

### Enhanced Command-Line Interface

- **Mode Selection**: Choose from phase2, phase4, phase6, or custom experiments
- **Configuration Presets**: default, performance, research, minimal
- **Advanced Configuration**: Fine-tune parameters via command line or config files
- **Colorized Logging**: Enhanced output with color coding
- **Multiple Output Formats**: Support for JSON, CSV, XML, and human-readable formats

### Advanced Configuration

The enhanced configuration system includes:

**Basic Neural Parameters**
- Random seed
- Simulation timestep
- Number of neurons and regions
- Connection probability

**Plasticity Parameters**
- STDP LTP/LTD weights
- STDP time constant
- Hebbian learning rate
- Weight bounds (max/min)

**Structural Plasticity**
- Synaptogenesis rate
- Pruning rate
- Maximum synapses per neuron

**Memory Systems**
- Working memory capacity
- Episodic memory capacity
- Associative memory capacity

**Neuromodulation**
- Dopamine sensitivity
- Curiosity strength
- Novelty threshold
- Reward prediction error gain

**Development**
- Development stage duration
- Plasticity decay rate
- Synaptogenesis/pruning activation

**Prediction & Cognition**
- Prediction horizon
- Confidence thresholds
- Planning horizons
- Attention focus sizes

### Available Executables

After building, you'll have:

- **`./nlm`**: Enhanced command-line interface (NEW!)
- **`./nlm_legacy`**: Legacy main.cpp (backward compatibility)
- **`./nlm_cli`**: Dedicated CLI executable
- **`./nlm_phase3_demo`**: Phase 3 demo executable
- **`./nlm_phase4_demo`**: Phase 4 demo executable

## Configuration Presets

### Performance Preset
Optimized for research with good performance:
- 1000 neurons, 2 regions
- Faster timestep (0.0005s)
- Checkpointing and replay enabled
- Suitable for medium-sized experiments

### Research Preset
Optimized for detailed research:
- 2000 neurons, 4 regions
- Development and plasticity enabled
- Maximum exploration capabilities
- Suitable for large-scale experiments

### Minimal Preset
Optimized for quick testing:
- 100 neurons, 1 region
- Slower timestep (0.002s)
- Minimal overhead
- Suitable for prototyping and testing

### Default Preset
Balanced configuration:
- 500 neurons, 1 region
- Standard timestep (0.001s)
- All features enabled
- Good starting point for most users

## Experiment Modes

### Phase 2 Demo
- Demonstrates basic neural computation
- Shows LIF neuron dynamics
- Tests STDP and Hebbian plasticity
- Validates structural plasticity

### Phase 6 Demo
- Comprehensive integration test
- Verifies all brain systems
- Tests memory, neuromodulation, prediction
- Validates development and learning capabilities

### Custom Experiment
- User-defined parameters
- Flexible configuration
- Advanced customization
- Perfect for research experiments

## Configuration File

NLM supports configuration files (`.cfg` or `.json` format):

### Basic Configuration File
```
# NLM Configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15

# Plasticity
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
learning_rate = 0.01

# Structural Plasticity
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory
working_memory_capacity = 1000
episodic_memory_capacity = 5000
associative_memory_capacity = 2000

# Neuromodulation
dopamine_sensitivity = 1.0
curiosity_strength = 0.5
novelty_threshold = 0.1
reward_prediction_error_gain = 0.5

# Development
development_stage_duration = 1000
plasticity_decay_rate = 0.99
```

### Loading Configuration
```bash
./nlm --config my_config.cfg --mode phase6
```

## Advanced Usage

### Combining Configuration Options
```bash
# Use performance preset with custom timestep
./nlm --mode phase6 --preset performance --timestep 0.002 --max-steps 10000

# Use research preset with custom neuron count
./nlm --mode custom --preset research --neuron-count 2500 --region-count 8

# Use minimal preset for quick testing
./nlm --mode phase2 --preset minimal --quiet
```

### Logging Levels
```bash
# Show only errors
./nlm --log-level error --mode phase6

# Show everything with colors
./nlm --log-level debug --verbose --mode phase6

# Log to file
./nlm --log-level info --output-file experiment.log --mode phase6
```

## Performance Tips

### For Large Experiments
- Use the `performance` preset for better throughput
- Consider `--quiet` mode to reduce I/O overhead
- Use `--preset minimal` for initial testing

### For Research
- Use the `research` preset for maximum exploration
- Enable verbose logging for debugging
- Use detailed configuration files for reproducibility

### For Development
- Use `--mode custom` for flexible testing
- Combine presets with custom parameters
- Use the `minimal` preset for quick iterations

## Troubleshooting

### Common Issues

**"Command not found"**
```bash
# Make sure executables are in your PATH
chmod +x nlm nlm_legacy nlm_cli
mv nlm /usr/local/bin/  # or wherever you prefer
```

**Configuration file errors**
```bash
# Check file format and permissions
nano configs/my_config.cfg
./nlm --config configs/my_config.cfg --mode phase6
```

**Missing dependencies**
```bash
# Rebuild if dependencies change
make clean
make -j4
```

**Permission denied**
```bash
# Fix permissions
chmod +x nlm nlm_legacy nlm_cli
```

### Getting Help

```bash
# Show help for all options
./nlm --help

# Show version information
./nlm --version

# Get detailed documentation
./nlm --mode phase6 --verbose
```

## Migration Guide

### From Old Version
If you're upgrading from an older version of NLM:

1. **Run `./nlm --help`** to see all new options
2. **Use `./nlm_legacy`** if you need the old behavior
3. **Use `./nlm --mode phase6`** for the new enhanced demo
4. **Create configuration files** for better reproducibility

### Key Changes

- **New Default**: `./nlm` is now the enhanced CLI (replaces old main.cpp)
- **Backward Compatible**: `./nlm_legacy` provides the old interface
- **Enhanced Features**: Much more configuration options and presets
- **Better Logging**: Colorized and configurable log levels
- **Advanced Configuration**: JSON and file-based configuration support

## Future Enhancements

The enhanced NLM CLI is designed for extensibility. Future features may include:

- Web interface for remote control
- Real-time visualization
- Distributed computing support
- Cloud deployment capabilities
- Integration with external ML frameworks

## Getting Involved

### Report Issues
- Use GitHub issues for bug reports
- Check documentation for known limitations
- Join the NLM community for support

### Contribute
- Fork the repository and submit pull requests
- Add new configuration options
- Implement new experiment types
- Improve documentation

### Support
- Check the documentation for examples
- Use the issue tracker for problems
- Join the community for discussions

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Contact

For questions or support:
- Check the documentation in the docs/ directory
- Use the issue tracker on GitHub
- Contact the NLM project team

---

Enjoy exploring the enhanced NLM neural learning machine!
