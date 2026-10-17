# NLM Enhanced Argument Parser

The enhanced argument parsing system provides comprehensive command-line and configuration file support for the Neural Learning Machine (NLM).

## Categories

The argument parser organizes parameters into the following categories:

### Core Parameters
- `random_seed`: Random seed for reproducibility
- `simulation_timestep`: Time step for simulation
- `neuron_count`: Number of neurons in the brain
- `region_count`: Number of neural regions
- `connection_probability`: Probability of synaptic connections
- `enable_neuromodulation`: Enable neuromodulation system

### Brain Configuration
- `brain_type`: Type of brain architecture
- `enable_plasticity`: Enable synaptic plasticity

### Plasticity Settings
- `stdp_ltp_weight`: STDP long-term potentiation weight
- `stdp_ltd_weight`: STDP long-term depression weight
- `stdp_tau`: STDP time constant
- `synaptogenesis_rate`: Rate of new synapse formation
- `pruning_rate`: Rate of synapse pruning

### Neuromodulation Settings
- `curiosity_strength`: Strength of curiosity-driven learning
- `novelty_threshold`: Threshold for novelty detection
- `reward_decay`: Reward decay factor

### Simulation Parameters
- `max_steps`: Maximum simulation steps
- `enable_checkpointing`: Enable checkpoint saving

### Output Configuration
- `output_dir`: Directory for output files
- `save_weights`: Save neural network weights

### Debugging Options
- `log_level`: Logging level (debug, info, warning, error)
- `debug_neurons`: List of neuron IDs for debugging
- `enable_profiling`: Enable performance profiling

## Command Line Options

### Short Options (-s)
- `-n, --neuron-count`: Number of neurons
- `-p, --enable-plasticity`: Enable plasticity
- `-v, --verbose`: Enable verbose output
- `-h, --help`: Show help information

### Long Options (--long)
- `--random-seed=<value>`: Random seed
- `--simulation-timestep=<value>`: Simulation time step
- `--neuron-count=<value>`: Number of neurons
- `--region-count=<value>`: Number of regions
- `--connection-probability=<value>`: Connection probability
- `--brain-type=<value>`: Brain type
- `--enable-plasticity=<true|false>`: Enable plasticity
- `--stdp-ltp-weight=<value>`: STDP LTP weight
- `--stdp-ltd-weight=<value>`: STDP LTD weight
- `--stdp-tau=<value>`: STDP time constant
- `--synaptogenesis-rate=<value>`: Synaptogenesis rate
- `--pruning-rate=<value>`: Pruning rate
- `--enable-neuromodulation=<true|false>`: Enable neuromodulation
- `--curiosity-strength=<value>`: Curiosity strength
- `--novelty-threshold=<value>`: Novelty threshold
- `--reward-decay=<value>`: Reward decay
- `--max-steps=<value>`: Maximum steps
- `--enable-checkpointing=<true|false>`: Enable checkpointing
- `--output-dir=<path>`: Output directory
- `--save-weights=<true|false>`: Save weights
- `--log-level=<level>`: Log level
- `--debug-neurons=<list>`: Debug neurons
- `--enable-profiling=<true|false>`: Enable profiling

## Configuration File Format

The parser supports multiple configuration file formats:

### Plain Text (.cfg)
```
# Comment
key = value
```

### JSON (.json)
```json
{
    "key": "value"
}
```

### YAML (.yaml)
```yaml
key: value
```

### TOML (.toml)
```toml
key = "value"
```

### INI (.ini)
```ini
[section]
key = value
```

### Properties (.properties)
```properties
key = value
```

## Presets

The parser includes several predefined presets:

### Standard Preset
- Balanced configuration for general use
- Includes core brain functionality
- Moderate performance

### Fast Preset
- Optimized for speed
- Reduced accuracy
- Fewer plasticity rules

### High Precision Preset
- Optimized for accuracy
- Enhanced learning capabilities
- Slower performance

### Debug Preset
- Enhanced debugging output
- Detailed logging
- Performance profiling enabled

## Usage Examples

### Basic Usage
```bash
nlm --neuron-count=1000 --simulation-timestep=0.01
```

### With Configuration File
```bash
nlm --config=configs/basic.cfg
```

### With Preset
```bash
nlm --preset=debug --enable-profiling=true
```

### Override Configuration
```bash
nlm --config=configs/basic.cfg --neuron-count=2000
```

### Get Help
```bash
nlm --help
```

## Error Handling

The parser provides comprehensive error handling:

1. **Validation Errors**: Invalid values are caught and reported
2. **Missing Dependencies**: Missing required parameters are flagged
3. **File Errors**: Invalid or unreadable configuration files
4. **Format Errors**: Unsupported configuration file formats

## Type Safety

The parser provides type-safe argument parsing:

- Automatic type conversion
- Type validation
- Default values for missing parameters
- Configuration schema checking

## Command Completion

The parser supports command completion:

```bash
nlm --complete "--"
```

## Integration

The parsed arguments can be accessed programmatically:

```cpp
ArgsParser parser;
parser.parse(argc, argv);
auto neuronCount = parser.get<int64_t>("neuron_count");
```

## License

This enhanced argument parsing system is part of the NLM project.
