# Quick Start Guide: Enhanced NLM Agent

This guide provides a quick overview of the enhanced NLM (Neural Learning Machine) agent with new features and improved functionality.

## What Changed

The enhanced NLM agent includes the following major improvements:

1. **Comprehensive Command-Line Interface** - Advanced options for configuration and control
2. **Enhanced Configuration System** - JSON/YAML support with validation
3. **Advanced Logging** - Detailed runtime monitoring and debugging
4. **Performance Monitoring** - Comprehensive metrics and tracking
5. **AgentBrain Improvements** - Configurable parameters and reduced magic numbers
6. **Extensibility Framework** - Support for custom agents and commands
7. **Better Error Handling** - Robust validation and recovery mechanisms

## Installation

The NLM agent is already installed and ready to use. If you need to build from source:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Quick Usage Examples

### Basic Usage

```bash
# Run the basic Phase 2 experiment
./nlm_agent

# Get help information
./nlm_agent --help

# See version information
./nlm_agent --version
```

### Configure the Agent

```bash
# Set neuron count to 1000
./nlm_agent --neuron 1000

# Set connection probability to 0.25
./nlm_agent --connections 0.25

# Enable development mode
./nlm_agent --development true

# Enable reward modulation
./nlm_agent --reward true
```

### Advanced Configuration

```bash
# Load configuration from file
./nlm_agent --config configs/production.json

# Export current configuration
./nlm_agent export myconfig.json

# Import configuration from file
./nlm_agent import saved_config.yaml

# Reset to defaults
./nlm_agent reset
```

### Performance Monitoring

```bash
# Enable performance monitoring
./nlm_agent --performance true

# Run benchmark and save results
./nlm_agent --performance true --benchmark benchmark.json

# Custom timestep and max steps
./nlm_agent --performance true 0.002 20000
```

### Debug Mode

```bash
# Enable debug mode with detailed logging
./nlm_agent --debug true

# Set log level to trace
./nlm_agent --log-level trace
```

## Configuration File Examples

### JSON Configuration

Create a file named `config.json`:

```json
{
    "neural_network": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.25
    },
    "agent_behavior": {
        "curiosity_threshold": 0.4,
        "exploration_chance_max": 0.5,
        "enable_reward_modulation": true,
        "enable_development": true,
        "enable_curiosity": true
    },
    "performance": {
        "simulation_timestep": 0.002,
        "max_steps": 20000,
        "enable_performance_monitoring": true
    },
    "advanced": {
        "output_format": "json",
        "validate_config": true,
        "enable_debug_mode": true,
        "enable_benchmark": true,
        "benchmark_file": "benchmark.json"
    }
}
```

### YAML Configuration

Create a file named `config.yaml`:

```yaml
neural_network:
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.25

agent_behavior:
  curiosity_threshold: 0.4
  exploration_chance_max: 0.5
  enable_reward_modulation: true
  enable_development: true
  enable_curiosity: true

performance:
  simulation_timestep: 0.002
  max_steps: 20000
  enable_performance_monitoring: true

advanced:
  output_format: json
  validate_config: true
  enable_debug_mode: true
  enable_benchmark: true
  benchmark_file: "benchmark.json"
```

### Simple Configuration

Create a file named `config.cfg`:

```
# NLM Agent Configuration
# Generated on: 2024-01-15

neural_network.neuron_count = 1000
neural_network.region_count = 2
neural_network.connection_probability = 0.25

agent_behavior.curiosity_threshold = 0.4
agent_behavior.exploration_chance_max = 0.5
agent_behavior.enable_reward_modulation = true
agent_behavior.enable_development = true
agent_behavior.enable_curiosity = true

performance.simulation_timestep = 0.002
performance.max_steps = 20000
performance.enable_performance_monitoring = true

advanced.output_format = json
advanced.validate_config = true
advanced.enable_debug_mode = true
advanced.enable_benchmark = true
advanced.benchmark_file = benchmark.json
```

## Command Reference

### Basic Commands

| Command | Description | Example |
|---------|-------------|---------|
| `--help` | Show all available commands | `--help` |
| `--version` | Show version information | `--version` |
| `--list` | List all commands with descriptions | `--list` |

### Configuration Commands

| Command | Description | Example |
|---------|-------------|---------|
| `--neuron <count>` | Set neuron count | `--neuron 1000` |
| `--region <count>` | Set region count | `--region 2` |
| `--connections <prob>` | Set connection probability | `--connections 0.25` |
| `--curiosity <thresh> [max]` | Set curiosity parameters | `--curiosity 0.4 0.5` |
| `--plasticity [factor] [rate]` | Set plasticity parameters | `--plasticity 0.7 0.15` |

### Feature Toggles

| Command | Description | Example |
|---------|-------------|---------|
| `--reward [true|false]` | Enable/disable reward modulation | `--reward true` |
| `--development [true|false]` | Enable/disable development | `--development true` |
| `--performance [enabled]` | Enable/disable performance monitoring | `--performance true` |
| `--debug [true|false]` | Enable/disable debug mode | `--debug true` |

### File Operations

| Command | Description | Example |
|---------|-------------|---------|
| `--config <file>` | Load configuration from file | `--config configs/myconfig.json` |
| `export <filename>` | Export configuration to file | `export myconfig.json` |
| `import <filename>` | Import configuration from file | `import saved_config.yaml` |
| `reset` | Reset to default configuration | `reset` |

### Advanced Commands

| Command | Description | Example |
|---------|-------------|---------|
| `--benchmark [filename]` | Run benchmark test | `--benchmark results.json` |

## Configuration Validation

The enhanced configuration system validates all values:

- **Neuron count**: Must be positive integer
- **Region count**: Must be positive integer
- **Connection probability**: Must be between 0.0 and 1.0
- **Curiosity threshold**: Must be between 0.0 and 1.0
- **Exploration chance max**: Must be between 0.0 and 1.0
- **Motor activation threshold**: Must be between 0.0 and 1.0
- **Simulation timestep**: Must be positive
- **Max steps**: Must be positive

If validation fails, the system will report the error and continue with default values.

## Logging and Debugging

### Enable Debug Mode

```bash
# Enable full debug mode
./nlm_agent --debug true

# Set log level to trace
./nlm_agent --log-level trace
```

### Log Output Format

Enhanced logging includes:

- **Timestamp**: Date and time of log entry
- **Log level**: DEBUG, INFO, WARNING, ERROR, etc.
- **Context**: Custom context identifier
- **Source**: File, line number, and function
- **Message**: Log message
- **Performance**: Duration information for operations

### Performance Tracking

Performance metrics are automatically tracked for:

- Operation timing
- Average durations
- Performance statistics
- Benchmark analysis

## Examples

### Example 1: Production Configuration

```bash
# Run with production configuration
./nlm_agent \
    --config configs/production.json \
    --connections 0.3 \
    --development true \
    --performance true \
    --benchmark production_benchmark.json
```

### Example 2: Development Configuration

```bash
# Run with development configuration
./nlm_agent \
    --neuron 100 \
    --connections 0.1 \
    --curiosity 0.5 \
    --plasticity 0.8 \
    --debug true \
    --development true
```

### Example 3: Custom Configuration

```bash
# Create custom config
cat > myconfig.json << EOF
{
    "neural_network": {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.15
    },
    "agent_behavior": {
        "curiosity_threshold": 0.3,
        "enable_reward_modulation": true
    }
}
EOF

# Run with custom config
./nlm_agent --config myconfig.json
```

## Performance Tips

### Optimize Configuration

1. **Reduce neuron count for testing**: `--neuron 100`
2. **Increase simulation timestep**: `--performance true 0.01 5000`
3. **Disable performance monitoring for production**: `--performance false`

### Memory Management

1. **Limit memory logger entries**: `--log-entries 1000`
2. **Disable memory logging for production**: `--memory-logging false`
3. **Use file logging for large deployments**: `--output-file app.log`

### Debugging Tips

1. **Enable debug mode for troubleshooting**: `--debug true`
2. **Export logs for analysis**: `--export-logs debug.log`
3. **Use memory logger for complex scenarios**: `--memory-logging true`

## Troubleshooting

### Common Issues

**Issue**: Configuration file not found
**Solution**: Ensure file exists or use defaults:
```bash
# Create config directory and file
mkdir -p configs
echo 'neuron_count = 500' > configs/default.cfg

# Run with explicit config
./nlm_agent --config configs/default.cfg
```

**Issue**: High memory usage
**Solution**: Limit memory logger or increase system resources:
```bash
# Limit memory logger entries
./nlm_agent --log-entries 1000

# Disable memory logging
./nlm_agent --memory-logging false
```

**Issue**: Performance bottlenecks
**Solution**: Adjust configuration:
```bash
# Reduce neuron count
./nlm_agent --neuron 100

# Increase timestep
./nlm_agent --performance true 0.01 5000
```

## Summary

The enhanced NLM agent provides a comprehensive set of tools for neural simulation and development. Key improvements include:

- **Flexible configuration** with JSON/YAML/simple format support
- **Advanced command-line interface** for fine-grained control
- **Enhanced logging** with performance tracking
- **Robust validation** and error handling
- **Memory and file logging** options
- **Benchmark and performance monitoring**
- **Extensible architecture** for custom agents

For more detailed information, refer to the main documentation file `ENHANCED_FEATURES.md`.
