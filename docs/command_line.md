# NLM Command Line Interface Reference

## Overview

The NLM (Neural Learning Machine) project now features an enhanced command-line interface with advanced configuration options for performance tuning, debugging, experimentation, validation, and output formatting.

## Installation

Build the project with CMake:
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Run the executable:
```bash
./nlm
```

## Basic Usage

```bash
# Basic help
./nlm --help

# Advanced help (shows all options)
./nlm --advanced-help

# Load configuration from file
./nlm --config=my_config.cfg

# Set individual configuration options
./nlm --key1=value1 --key2=value2

# Short format
./nlm -key1 value1 -key2 value2
```

## Advanced Configuration Options

### Performance Options

| Option | Short Form | Description | Default | Type |
|--------|------------|-------------|---------|------|
| `--threads=N` | `-t` | Number of worker threads | CPU cores | integer |
| `--simd` | `-s` | Enable SIMD acceleration | disabled | flag |
| `--performance-mode=mode` | `-pm` | Performance optimization mode (balanced/speed/power/memory) | balanced | string |
| `--checkpoint-interval=N` | `-ci` | Checkpoint frequency in simulation steps | 100 | integer |
| `--memory-pool-size=N` | `-mps` | Memory pool size in bytes | 1MB | integer |

**Examples:**
```bash
./nlm --threads=8 --simd --performance-mode=speed
./nlm --checkpoint-interval=500 --memory-pool-size=4MB
```

### Debugging Options

| Option | Short Form | Description | Default | Type |
|--------|------------|-------------|---------|------|
| `--debug-level=N` | `-dl` | Debug verbosity level (0-5) | 0 | integer |
| `--debug-neurons` | `-dn` | Debug specific neuron groups | disabled | flag |
| `--debug-synapses` | `-ds` | Debug specific synapse groups | disabled | flag |
| `--debug-plasticity` | `-dp` | Debug plasticity operations | disabled | flag |
| `--debug-validation` | `-dv` | Enable validation logging | disabled | flag |
| `--trace-events` | `-te` | Enable event tracing | disabled | flag |
| `--profile-execution` | `-pe` | Enable performance profiling | disabled | flag |

**Examples:**
```bash
./nlm --debug-level=3 --debug-neurons --debug-plasticity
./nlm --trace-events --profile-execution
```

### Experiment Options

| Option | Short Form | Description | Default | Type |
|--------|------------|-------------|---------|------|
| `--experiment-mode=mode` | `-em` | Predefined experiment configurations | standard | string |
| `--experiment-config=file` | `-ec` | Load experiment configuration | none | file |
| `--observation-interval=N` | `-oi` | Observation frequency in steps | 10 | integer |
| `--reward-mode=mode` | `-rm` | Reward calculation mode (sparse/dense/shaped) | sparse | string |
| `--environment-config=file` | `-env` | Environment configuration | none | file |

**Examples:**
```bash
./nlm --experiment-mode=stress_test --observation-interval=5
./nlm --reward-mode=dense --environment-config=env_config.json
```

### Validation Options

| Option | Short Form | Description | Default | Type |
|--------|------------|-------------|---------|------|
| `--validate-config` | `-vc` | Validate configuration | disabled | flag |
| `--strict-validation` | `-sv` | Enable strict validation | disabled | flag |
| `--validation-threshold=N` | `-vt` | Validation threshold (0.0-1.0) | 0.5 | float |
| `--skip-invalid` | `-si` | Skip invalid configurations | disabled | flag |
| `--fix-invalid` | `-fi` | Attempt to fix invalid configurations | disabled | flag |

**Examples:**
```bash
./nlm --validate-config --strict-validation --validation-threshold=0.8
./nlm --skip-invalid --fix-invalid
```

### Output Options

| Option | Short Form | Description | Default | Type |
|--------|------------|-------------|---------|------|
| `--output-format=mode` | `-of` | Output format (text/json/csv/binary) | text | string |
| `--output-prefix=path` | `-op` | Output file prefix | ./output | string |
| `--output-interval=N` | `-oi` | Output frequency in steps | 100 | integer |
| `--verbose` | `-v` | Verbose output | disabled | flag |
| `--quiet` | `-q` | Minimal output | disabled | flag |
| `--progress` | `-pg` | Progress reporting | enabled | flag |

**Examples:**
```bash
./nlm --output-format=json --output-prefix=./results --verbose
./nlm --output-format=csv --output-interval=50 --progress
```

### Neural Network Options

| Option | Short Form | Description | Default | Type |
|--------|------------|-------------|---------|------|
| `--neuron-type=type` | `-nt` | Neuron type (excitatory/inhibitory/sensory/motor) | excitatory | string |
| `--synapse-type=type` | `-st` | Synapse type (static/dynamic/plastic) | static | string |
| `--connectivity-density=N` | `-cd` | Network connectivity density (0.0-1.0) | 0.15 | float |
| `--delay-range=min,max` | `-dr` | Synaptic delay range | 0,5 | pair |
| `--weight-range=min,max` | `-wr` | Synaptic weight range | 0,1 | pair |

**Examples:**
```bash
./nlm --neuron-type=inhibitory --synapse-type=plastic
./nlm --connectivity-density=0.2 --delay-range=0,10 --weight-range=-1,1
```

### Memory Management Options

| Option | Short Form | Description | Default | Type |
|--------|------------|-------------|---------|------|
| `--memory-optimization=mode` | `-mo` | Memory optimization strategy | standard | string |
| `--enable-memory-pool` | `-emp` | Enable memory pools | disabled | flag |
| `--memory-pool-size=N` | `-mps` | Memory pool size in bytes | 64MB | integer |
| `--enable-vm` | `-evm` | Enable virtual memory | disabled | flag |
| `--mmap-threshold=N` | `-mmt` | Memory mapping threshold | 1MB | integer |

**Examples:**
```bash
./nlm --memory-optimization=aggressive --enable-memory-pool
./nlm --enable-vm --mmap-threshold=10MB
```

### Checkpoint Options

| Option | Short Form | Description | Default | Type |
|--------|------------|-------------|---------|------|
| `--checkpoint=file` | `-cp` | Load checkpoint | none | file |
| `--save-checkpoint=file` | `-scp` | Save checkpoint | disabled | flag |
| `--checkpoint-interval=N` | `-ci` | Checkpoint interval | 100 | integer |
| `--checkpoint-format=mode` | `-cpf` | Checkpoint format (binary/json) | binary | string |
| `--checkpoint-compression=level` | `-ccl` | Checkpoint compression level (0-9) | 6 | integer |

**Examples:**
```bash
./nlm --checkpoint=checkpoint.bin --save-checkpoint=checkpoint_backup.bin
./nlm --checkpoint-interval=200 --checkpoint-format=json --checkpoint-compression=9
```

## Configuration File Format

### JSON Format
```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 500,
    "performance": {
        "threads": 4,
        "simd": true,
        "optimization_level": "balanced"
    },
    "debug": {
        "level": 2,
        "neuron_ids": [1, 2, 3],
        "log_validation": true
    },
    "experiment": {
        "mode": "standard",
        "duration": 1000,
        "objective": "maximize"
    }
}
```

### Key-Value Format (Legacy)
```
# NLM Configuration File
# Comments start with # or /
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
performance.threads = 4
debug.level = 2
experiment.mode = standard
```

## Common Patterns

### Pattern 1: High-Performance Learning
```bash
./nlm \
  --performance-mode=speed \
  --threads=8 \
  --enable-memory-pool \
  --output-format=json \
  --debug-level=2 \
  --validate-config
```

### Pattern 2: Experiment with Detailed Logging
```bash
./nlm \
  --experiment-mode=stress_test \
  --observation-interval=5 \
  --reward-mode=dense \
  --trace-events \
  --progress \
  --verbose
```

### Pattern 3: Debugging and Validation
```bash
./nlm \
  --debug-level=4 \
  --debug-plasticity \
  --debug-validation \
  --strict-validation \
  --skip-invalid
```

### Pattern 4: Memory-Optimized Execution
```bash
./nlm \
  --memory-optimization=aggressive \
  --enable-vm \
  --mmap-threshold=10MB \
  --memory-pool-size=128MB
```

### Pattern 5: Checkpoint-based Execution
```bash
./nlm \
  --checkpoint=checkpoint.bin \
  --checkpoint-interval=500 \
  --save-checkpoint=checkpoint_backup.bin \
  --checkpoint-compression=9
```

## Troubleshooting

### Common Issues and Solutions

**Issue: Command-line parsing fails**
- **Cause**: Incorrect option format
- **Solution**: Check option syntax. Use `--help` for basic usage, `--advanced-help` for all options

**Issue: Configuration file not found**
- **Cause**: Invalid `--config` path
- **Solution**: Ensure the file exists and provide absolute path if needed

**Issue: Invalid configuration values**
- **Cause**: Out-of-range or invalid values
- **Solution**: Use `--validate-config` to check configuration, `--fix-invalid` to auto-fix

**Issue: Memory usage too high**
- **Cause**: Not using memory optimization
- **Solution**: Use `--memory-optimization=aggressive` and `--enable-memory-pool`

**Issue: No output generated**
- **Cause**: Output format or interval not set
- **Solution**: Set `--output-format` and `--output-interval`

### Validation Error Codes

- **1**: Command-line parsing error
- **2**: Configuration file not found or invalid
- **3**: Command-line validation failed
- **4**: Brain initialization failed
- **5**: Simulation execution error

## Integration with External Tools

### Environment Variables

The following environment variables can be used to set default values:

```bash
export NLM_THREADS=4
export NLM_PERFORMANCE_MODE="speed"
export NLM_DEBUG_LEVEL=2
```

These will be used unless overridden by command-line arguments.

### Configuration Management

Create configuration files for different use cases:

- `configs/development.cfg` - Development settings with debugging
- `configs/production.cfg` - Production settings with optimizations
- `configs/research.cfg` - Research settings with experiment focus

Load them with:
```bash
./nlm --config=configs/development.cfg
```

## Advanced Features

### Command-Line Validation

The enhanced system includes comprehensive validation:

1. **Type validation**: Options are validated against their expected types
2. **Range validation**: Numeric options are checked for valid ranges
3. **Dependency validation**: Options that require other options are checked
4. **Mutual exclusion validation**: Conflicting options are detected
5. **Schema validation**: Configuration can be validated against a schema

### Configuration Precedence

Configuration values are applied in this order (highest precedence first):

1. Command-line arguments
2. Configuration file settings
3. Default values

Command-line arguments override both file and default settings.

### Performance Considerations

- **Parsing**: Optimized for speed with minimal overhead
- **Memory**: Thread-safe parsing with minimal memory allocation
- **Validation**: Lazy validation to avoid unnecessary computation
- **Output**: Configurable logging levels to reduce I/O

## Example Configuration Workflow

```bash
# 1. Start with basic configuration
./nlm --config=configs/base.cfg

# 2. Add performance tuning
./nlm --config=configs/base.cfg --performance-mode=speed --threads=8

# 3. Enable debugging for troubleshooting
./nlm --config=configs/base.cfg --debug-level=3 --trace-events

# 4. Run experiment with detailed logging
./nlm --config=configs/base.cfg \
     --experiment-mode=stress_test \
     --observation-interval=5 \
     --output-format=json \
     --verbose

# 5. Validate and optimize
./nlm --config=configs/base.cfg \
     --validate-config \
     --fix-invalid \
     --memory-optimization=aggressive
```

## Conclusion

The enhanced command-line interface provides comprehensive control over NLM's behavior, from basic configuration to advanced performance tuning, debugging, and experimentation. The system maintains backward compatibility while adding powerful new features for research, development, and production use.

For more information about specific options or advanced features, refer to the individual option documentation above or consult the NLM documentation.