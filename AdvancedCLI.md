# NLM Advanced CLI

The NLM (Neural Learning Machine) Advanced CLI provides comprehensive command-line interfaces for power users working with the NLM brain simulation system.

## Overview

This advanced CLI extends the basic NLM interface with specialized commands for:

1. **Advanced Configuration Management**
   - Import/export configurations in multiple formats (JSON, YAML, binary)
   - Configuration validation and schema checking
   - Configuration comparison and diffing

2. **Enhanced Simulation Control**
   - Custom simulation parameters
   - Real-time simulation monitoring
   - Simulation checkpoint/restart controls
   - Custom learning rate scheduling

3. **Power User Features**
   - Advanced debugging tools
   - Performance profiling integration
   - Neuron-by-neuron debugging
   - Synapse-level monitoring
   - Network-wide statistics

4. **Brain State Management**
   - Manual brain state manipulation
   - Pattern injection for testing
   - Network topology exploration
   - Weight visualization and analysis

## Installation and Building

### Build from Source

```bash
mkdir build
cd build
cmake ..
cmake --build . -- -j$(nproc)
```

### Install

```bash
cd build
sudo make install
```

## Usage

### Basic Commands

```bash
# Show help information
nlm-cli --help

# Show version information
nlm-cli --version

# Enable verbose output
nlm-cli --verbose

# Suppress output
nlm-cli --quiet
```

### Configuration Management

```bash
# Export configuration to file
nlm-cli config export --file config.json --format json
nlm-cli config export --file config.yaml --format yaml
nlm-cli config export --file config.txt --format table

# Import configuration from file
nlm-cli config import --file config.json --format json
nlm-cli config import --file config.yaml --format yaml
nlm-cli config import --file config.txt --format table

# Validate configuration schema
nlm-cli config validate

# Compare configuration versions
nlm-cli config diff --source current --target default

# List all configuration keys
nlm-cli config list
```

### Simulation Control

```bash
# Run custom simulation
nlm-cli simulate custom --timestep 0.001 --neurons 1000 --regions 2 --plasticity

# Run in real-time with monitoring
nlm-cli simulate realtime --speed 2.0 --monitoring --checkpoint-interval 100

# Start simulation monitoring
nlm-cli simulate monitor --log-level INFO --metrics --performance

# Create checkpoint
nlm-cli simulate checkpoint --directory checkpoints --compress --keep 5

# Restart from checkpoint
nlm-cli simulate restart --checkpoint checkpoints/brain_checkpoint_20250101_120000.bin

# Configure learning rate schedule
nlm-cli simulate schedule --type exponential_decay --initial 0.01 --decay 0.001 --steps 1000
```

### Power User Features

```bash
# Advanced debugging
nlm-cli debug advanced --neuron-debug --synapse-monitor --memory-leak

# Performance profiling
nlm-cli profiler start --output profile.json --duration 60 --detailed
nlm-cli profiler report --input profile.json --format html

# Neuron-by-neuron debugging
nlm-cli neuron debug --id 1 --show-potential --show-spikes --show-synapses
nlm-cli neuron debug --population sensory --show-activations --history-size 100

# Synapse-level monitoring
nlm-cli synapse monitor --show-weights --plasticity-events --delays
nlm-cli synapse monitor --filter type excitatory --history 50

# Network-wide statistics
nlm-cli network stats --summary --detailed --performance
```

### Brain State Management

```bash
# Manual brain state manipulation
nlm-cli state manual --command inject --neuron-id 1 --current 50.0
nlm-cli state manual --command set-weight --source 1 --dest 2 --weight 0.5
nlm-cli state manual --command set-development-stage --stage maturation

# Pattern injection for testing
nlm-cli state pattern --type pre-post --pre-neuron 1 --post-neuron 2 --repetitions 10
nlm-cli state pattern --type random --neurons 20 --current-range 10-50 --steps 100

# Network topology exploration
nlm-cli state topology --measure clustering --measure shortest-path
nlm-cli state topology --identify hubs --threshold 0.8

# Weight visualization
nlm-cli state visualize --type network --output network_graph.png
nlm-cli state visualize --type distribution --output weight_histogram.png

# Brain state analysis
nlm-cli state analysis --complexity --information-bottleneck --generalization
```

## Configuration File Formats

### JSON Format (`config.json`)

```json
{
  "neuron_count": 1000,
  "region_count": 1,
  "simulation_timestep": 0.001,
  "random_seed": 42,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.015,
  "stdp_tau": 20.0,
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001,
  "connection_probability": 0.15,
  "replay_interval": 100,
  "consolidation_interval": 1000
}
```

### YAML Format (`config.yaml`)

```yaml
# NLM Configuration
neuron_count: 1000
region_count: 1
simulation_timestep: 0.001
random_seed: 42

# Plasticity Parameters
stdp_ltp_weight: 0.02
stdp_ltd_weight: 0.015
stdp_tau: 20.0
synaptogenesis_rate: 0.0001
pruning_rate: 0.00001

# Network Properties
connection_probability: 0.15
replay_interval: 100
consolidation_interval: 1000
```

### Table Format (`config.txt`)

```text
# Configuration File
# Format: key = value
neuron_count = 1000
region_count = 1
simulation_timestep = 0.001
random_seed = 42
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
connection_probability = 0.15
replay_interval = 100
consolidation_interval = 1000
```

## Workflow Examples

### Complete Development Workflow

```bash
# 1. Initialize with configuration
nlm-cli config import --file config.yaml --format yaml
nlm-cli config validate

# 2. Run experiment with monitoring
nlm-cli simulate realtime --speed 1.0 --monitoring

# 3. Create checkpoint
nlm-cli simulate checkpoint --directory checkpoints

# 4. Analyze results
nlm-cli profiler start --output profile.json --duration 60
nlm-cli network stats --detailed

# 5. Inspect network topology
nlm-cli state topology --identify hubs --measure clustering

# 6. Export final configuration
nlm-cli config export --file final_config.json --format json

# 7. Generate documentation
nlm-cli help export --format markdown > documentation.md
```

### Production Workflow

```bash
# 1. Run simulation with production settings
nlm-cli simulate custom --timestep 0.0005 --neurons 5000 --regions 3

# 2. Enable comprehensive monitoring
nlm-cli simulate monitor --log-level INFO --metrics --performance

# 3. Create periodic checkpoints
nlm-cli simulate checkpoint --directory prod_checkpoints --compress --keep 10

# 4. Analyze production results
nlm-cli network stats --summary --detailed
nlm-cli state analysis --complexity --generalization

# 5. Validate results
nlm-cli config validate

# 6. Export production configuration
nlm-cli config export --file production_config.json --format json
```

## Integration with Existing Systems

The Advanced CLI integrates seamlessly with the following NLM modules:

- **Config**: Full configuration management and validation
- **Brain**: Brain state manipulation and simulation control
- **Logger**: Comprehensive logging support with multiple output formats
- **CheckpointSystem**: Simulation checkpoint/restart capabilities
- **Performance**: Performance profiling and metrics collection
- **Visualization**: Network visualization and analysis tools

## Command Categories

### Configuration Management

| Command | Description | Examples |
|---------|-------------|----------|
| `config export` | Export configuration to file | `nlm-cli config export --file config.json --format json` |
| `config import` | Import configuration from file | `nlm-cli config import --file config.yaml --format yaml` |
| `config validate` | Validate configuration schema | `nlm-cli config validate` |
| `config diff` | Compare configuration versions | `nlm-cli config diff --source current --target default` |
| `config list` | List all configuration keys | `nlm-cli config list` |

### Simulation Control

| Command | Description | Examples |
|---------|-------------|----------|
| `simulate custom` | Run custom simulation | `nlm-cli simulate custom --timestep 0.001 --neurons 1000` |
| `simulate realtime` | Real-time simulation | `nlm-cli simulate realtime --speed 2.0 --monitoring` |
| `simulate monitor` | Simulation monitoring | `nlm-cli simulate monitor --log-level INFO` |
| `simulate checkpoint` | Create checkpoint | `nlm-cli simulate checkpoint --directory checkpoints` |
| `simulate restart` | Restart from checkpoint | `nlm-cli simulate restart --checkpoint file.bin` |
| `simulate schedule` | Learning rate schedule | `nlm-cli simulate schedule --type exponential_decay` |

### Power User Features

| Command | Description | Examples |
|---------|-------------|----------|
| `debug advanced` | Advanced debugging tools | `nlm-cli debug advanced --neuron-debug` |
| `profiler start` | Start performance profiler | `nlm-cli profiler start --output profile.json` |
| `profiler report` | Generate profiler report | `nlm-cli profiler report --input profile.json` |
| `neuron debug` | Neuron-by-neuron debugging | `nlm-cli neuron debug --id 1` |
| `synapse monitor` | Synapse-level monitoring | `nlm-cli synapse monitor --show-weights` |
| `network stats` | Network-wide statistics | `nlm-cli network stats --summary` |

### Brain State Management

| Command | Description | Examples |
|---------|-------------|----------|
| `state manual` | Manual brain state manipulation | `nlm-cli state manual --command inject --neuron-id 1` |
| `state pattern` | Pattern injection for testing | `nlm-cli state pattern --type pre-post --repetitions 10` |
| `state topology` | Network topology exploration | `nlm-cli state topology --identify hubs` |
| `state visualize` | Weight visualization | `nlm-cli state visualize --type network --output file.png` |
| `state analysis` | Brain state analysis | `nlm-cli state analysis --complexity` |

## Error Handling

The CLI uses the following error codes:

- `0`: Success
- `1`: General error
- `2`: Configuration error
- `3`: Brain initialization error
- `4`: Simulation error
- `5`: Debug error
- `6`: Profiling error
- `7`: State management error
- `8`: I/O error

## Advanced Usage Tips

### Command Line Parsing

The CLI uses argparse-style command line parsing. For detailed help on any command:

```bash
nlm-cli config export --help
nlm-cli simulate custom --help
```

### Environment Variables

Set environment variables for configuration:

```bash
export NLM_CONFIG_FILE=config.yaml
export NLM_LOG_LEVEL=DEBUG
export NLM_LOG_FILE=/var/log/nlm.log
```

### Configuration File Locations

The CLI looks for configuration files in the following order:

1. Current directory (`./config.json`, `./config.yaml`, `./config.txt`)
2. Home directory (`~/.config/nlm/config.json`)
3. Environment variable (`$NLM_CONFIG_FILE`)

### Performance Optimization

For large-scale simulations:

```bash
# Use checkpointing for long runs
nlm-cli simulate checkpoint --directory checkpoints --compress --keep 10

# Monitor performance during simulation
nlm-cli simulate monitor --performance --metrics

# Use parallel processing
nlm-cli simulate custom --parallel --num-threads 8

# Optimize memory usage
nlm-cli debug advanced --memory-analysis
```

## Development

### Adding New Commands

To add a new command:

1. Add the command to `src/cli/AdvancedCLI.cpp` in the `registerCommands()` method
2. Implement the command handler
3. Update this documentation
4. Rebuild the CLI

### Command Structure

Each command follows this pattern:

```cpp
commands["command-name"] = {
    "Description of the command",
    [this]() { handleCommand(); },
    "command usage pattern"
};
```

### Testing the CLI

```bash
# Test basic functionality
nlm-cli --help
nlm-cli --version

# Test configuration management
nlm-cli config list
nlm-cli config validate

# Test brain operations
nlm-cli brain info
nlm-cli brain status
```

## Known Issues and Limitations

1. Some advanced debugging features require additional compilation flags
2. Real-time simulation requires adequate system resources
3. Large network topology analysis may be memory-intensive
4. Checkpoint files can be large (use compression for production)

## Future Enhancements

Planned improvements include:

1. **Interactive mode**: TUI for real-time brain control
2. **Web interface**: REST API for remote brain management
3. **Advanced visualization**: 3D neural network visualization
4. **Batch processing**: Support for automated experiment pipelines
5. **Distributed computing**: Scale across multiple machines

## License

This project is licensed under the MIT License. See the LICENSE file for details.

---

*Document generated automatically by NLM Advanced CLI*
*Last updated: $(date)*
