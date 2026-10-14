# NLM Command Line Interface

## Overview

The NLM (熙然) Neural Learning Machine now includes an enhanced command-line interface (CLI) that provides comprehensive control over neural network experiments and simulations. The CLI replaces the basic main.cpp with a more powerful and flexible interface.

## Features

### Basic Options
- `--help, -h`: Show this help message
- `--version`: Show version information
- `--config <file>`: Load configuration from file
- `--mode <mode>`: Experiment mode (phase2, phase4, phase6, custom)
- `--preset <preset>`: Configuration preset (default, performance, research, minimal)
- `--log-level <level>`: Logging level (debug, info, warning, error, critical)
- `--verbose`: Enable verbose output
- `--quiet`: Suppress output
- `--max-steps <n>`: Maximum simulation steps
- `--timestep <ms>`: Simulation timestep in milliseconds
- `--neuron-count <n>`: Number of neurons
- `--region-count <n>`: Number of brain regions
- `--experiment-name <name>`: Name for the experiment

### Configuration Presets

#### Default
- Random seed: 42
- Simulation timestep: 0.001s
- Neurons: 500
- Regions: 1
- Connection probability: 0.15

#### Performance
- Random seed: 123
- Simulation timestep: 0.0005s
- Neurons: 1000
- Regions: 2
- Connection probability: 0.1
- Checkpointing: enabled
- Replay: enabled

#### Research
- Random seed: 456
- Simulation timestep: 0.001s
- Neurons: 2000
- Regions: 4
- Connection probability: 0.05
- Development: enabled
- Plasticity: enabled

#### Minimal
- Random seed: 789
- Simulation timestep: 0.002s
- Neurons: 100
- Regions: 1
- Connection probability: 0.2
- Checkpointing: disabled
- Replay: disabled

### Experiment Modes

#### Phase 2 Demo
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity
- Basic connectivity tests

#### Phase 6 Demo
- Full integration test
- All brain systems verification
- Memory systems integration
- Neuromodulation integration
- Prediction system integration
- Development integration
- Checkpoint and replay functionality

#### Custom Experiment
- User-defined parameters
- Flexible configuration
- Advanced customization options

## Usage Examples

### Phase 6 Performance Demo
```bash
./nlm --mode phase6 --preset performance --max-steps 5000 --log-level info
```

### Phase 2 Basic Test
```bash
./nlm --mode phase2 --config my_config.cfg --verbose
```

### Custom Research Experiment
```bash
./nlm --mode custom --neuron-count 100 --region-count 2 --preset research
```

### Minimal Demo
```bash
./nlm --mode phase6 --preset minimal --quiet
```

## Configuration File Format

The configuration file uses a simple key=value format with comments:

```
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
learning_rate = 0.01
max_weight = 1.0
min_weight = -1.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
enable_checkpointing = true
enable_replay = true
enable_development = true
```

## Logging Levels

- **Debug**: Detailed debugging information
- **Info**: General information messages
- **Warning**: Warning messages
- **Error**: Error messages
- **Critical**: Critical error messages

## Advanced Features

### Colorized Logging
When logging is enabled, output includes color codes for better readability:
- Different colors for different log levels
- Color support can be disabled with `--no-colors`

### Output Formats
Future versions will support additional output formats:
- JSON
- CSV
- XML
- Human-readable

### Performance Monitoring
The CLI includes performance monitoring capabilities:
- Timing information
- Resource usage statistics
- Performance profiling

## Integration

The CLI is designed to work seamlessly with the existing NLM architecture:

1. **Backward Compatibility**: The legacy main.cpp executable (`nlm_legacy`) is still available
2. **Enhanced Functionality**: The new CLI provides all features of the old main.cpp plus many more
3. **Plugin Architecture**: The CLI can be extended with additional experiment types and output formats
4. **Configuration Management**: Advanced configuration management with file and command-line support

## Building

To build with the new CLI support:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

The build will produce:
- `nlm`: Enhanced command-line interface (new default)
- `nlm_legacy`: Legacy main.cpp for backward compatibility
- `nlm_cli`: Dedicated CLI executable
- `nlm_phase3_demo`: Phase 3 demo
- `nlm_phase4_demo`: Phase 4 demo

## Troubleshooting

### Common Issues

1. **Configuration File Not Found**
   ```bash
   # Check if the config file exists
   ls -la configs/
   
   # Create a default config file if needed
   ./nlm --mode phase6  # Creates default config
   ```

2. **Permission Denied**
   ```bash
   # Ensure execute permissions
   chmod +x nlm nlm_cli
   ```

3. **Missing Dependencies**
   ```bash
   # Check for required libraries
   make nlm_cli
   ```

### Help and Support

For more information:
- Run `./nlm --help` for command-line options
- Run `./nlm --version` for version information
- Check the NLM documentation in the docs/ directory

## Future Enhancements

The CLI is designed for extensibility. Future enhancements may include:

1. **Web Interface**: REST API for remote control
2. **Visualization**: Built-in visualization tools
3. **Distributed Computing**: Support for distributed experiments
4. **Machine Learning Integration**: Integration with external ML frameworks
5. **Cloud Support**: Deployment to cloud platforms

## License

MIT License
Copyright (c) 2026 NLM Project
