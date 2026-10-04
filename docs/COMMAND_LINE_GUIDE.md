# NLM Command-Line Interface Guide

## Overview

This document provides comprehensive documentation for the NLM command-line interface. The NLM executable offers a wide range of options for brain simulation, configuration management, experiment control, and performance monitoring.

## Basic Usage

### Help and Version Information

```bash
# Show help and available commands
./nlm --help

# Show version information
./nlm --version

# Show help for specific command
./nlm --help command_name
```

### Running Basic Simulations

```bash
# Run Phase 3 demo (world interaction)
./nlm_phase3_demo 5 200

# Run Phase 4 demo (cognitive experiments)
./nlm_phase4_demo 100

# Run Phase 6 demo (full integration test)
./nlm_phase6_demo
```

## Command Structure

The NLM command-line interface follows a hierarchical structure:

```
./nlm [global_options] [command] [command_options]
```

### Global Options

| Option | Description | Example |
|--------|-------------|---------|
| `--help` | Show help message | `./nlm --help` |
| `--version` | Show version information | `./nlm --version` |
| `--verbose` | Enable verbose output | `./nlm --verbose` |
| `--quiet` | Suppress output | `./nlm --quiet` |

### Command Categories

| Command | Description | Example |
|---------|-------------|---------|
| `brain` | Brain configuration and control | `./nlm brain --create --neurons=1000` |
| `simulation` | Simulation control and monitoring | `./nlm simulation --steps=1000` |
| `experiment` | Experiment management | `./nlm experiment --run --config=exp.json` |
| `performance` | Performance tuning and monitoring | `./nlm performance --monitor --level=3` |
| `export` | Export brain state and configurations | `./nlm export --brain=state.bin` |
| `import` | Import brain state and configurations | `./nlm import --brain=state.bin` |

## Brain Commands

### Brain Creation and Configuration

```bash
# Create brain with default configuration
./nlm brain create

# Create brain with custom configuration
./nlm brain create --neurons=2000 --plasticity=stdp --learning_rate=0.001

# Create brain from configuration file
./nlm brain create --config=custom_config.json
```

| Option | Description | Default | Example |
|--------|-------------|---------|---------|
| `--neurons` | Number of neurons | 1000 | `--neurons=5000` |
| `--plasticity` | Plasticity type (stdp, hebbian, both) | stdp | `--plasticity=both` |
| `--learning_rate` | STDP learning rate | 0.001 | `--learning_rate=0.0005` |
| `--tau_plus` | STDP tau_plus parameter | 20.0 | `--tau_plus=30.0` |
| `--tau_minus` | STDP tau_minus parameter | 20.0 | `--tau_minus=30.0` |
| `--config` | Configuration file path | - | `--config=my_config.json` |

### Brain Control

```bash
# Initialize brain
./nlm brain initialize

# Run simulation steps
./nlm brain step --steps=100

# Reset brain
./nlm brain reset

# Get brain statistics
./nlm brain stats

# Save brain state
./nlm brain save --file=checkpoint.bin

# Load brain state
./nlm brain load --file=checkpoint.bin
```

| Command | Description | Options |
|---------|-------------|---------|
| `initialize` | Initialize brain | `--config=file` |
| `step` | Run simulation steps | `--steps=N`, `--output=file` |
| `reset` | Reset brain to initial state | `--preserve=config` |
| `stats` | Display brain statistics | `--format=json`, `--verbose` |
| `save` | Save brain state | `--file=file`, `--compress=level` |
| `load` | Load brain state | `--file=file`, `--validate` |

### Brain Monitoring

```bash
# Monitor brain activity
./nlm brain monitor --duration=60 --interval=1

# Get brain metrics
./nlm brain metrics --output=metrics.json

# Analyze brain state
./nlm brain analyze --pattern=spike --threshold=0.5
```

## Simulation Commands

### Basic Simulation

```bash
# Run simple simulation
./nlm simulation run --steps=1000

# Run simulation with world interaction
./nlm simulation run --steps=500 --world=2d --vision=8

# Run simulation with custom timestep
./nlm simulation run --steps=1000 --timestep=0.05
```

| Option | Description | Default | Example |
|--------|-------------|---------|---------|
| `--steps` | Number of simulation steps | 100 | `--steps=5000` |
| `--world` | World type (2d, 3d) | 2d | `--world=3d` |
| `--timestep` | Simulation timestep (ms) | 0.1 | `--timestep=0.05` |
| `--vision` | Vision range (cells) | 8 | `--vision=16` |
| `--output` | Output file for results | - | `--output=results.json` |

### Simulation Control

```bash
# Pause simulation
./nlm simulation pause

# Resume simulation
./nlm simulation resume

# Stop simulation
./nlm simulation stop

# Get simulation status
./nlm simulation status
```

### Simulation Monitoring

```bash
# Monitor simulation performance
./nlm simulation monitor --duration=300 --interval=5

# Get real-time metrics
./nlm simulation metrics --realtime

# Set simulation alerts
./nlm simulation alert --metric=firing_rate --threshold=50
```

## Experiment Commands

### Running Experiments

```bash
# Run predefined experiment
./nlm experiment run --name=temporal_prediction --trials=100

# Run custom experiment from configuration
./nlm experiment run --config=exp_config.json

# Run experiment with specific parameters
./nlm experiment run --name=working_memory --capacity=50 --duration=1000
```

| Option | Description | Default | Example |
|--------|-------------|---------|---------|
| `--name` | Experiment name | - | `--name=temporal_prediction` |
| `--trials` | Number of trials | 1 | `--trials=100` |
| `--duration` | Experiment duration (steps) | 1000 | `--duration=5000` |
| `--config` | Experiment configuration file | - | `--config=exp_config.json` |
| `--output` | Output directory for experiment results | ./results | `--output=./my_results` |

### Experiment Management

```bash
# List available experiments
./nlm experiment list

# Get experiment details
./nlm experiment info --name=temporal_prediction

# Create new experiment
./nlm experiment create --name=custom_exp --type=learning

# Modify experiment
./nlm experiment modify --name=exp1 --parameter=learning_rate --value=0.01

# Delete experiment
./nlm experiment delete --name=exp1
```

### Experiment Results

```bash
# Analyze experiment results
./nlm experiment analyze --name=exp1 --metric=firing_rate

# Compare experiment results
./nlm experiment compare --exp1=exp1 --exp2=exp2

# Export experiment results
./nlm experiment export --name=exp1 --format=csv --file=results.csv
```

## Performance Commands

### Performance Tuning

```bash
# Set performance optimization level
./nlm performance set-level --level=3

# Get current performance settings
./nlm performance get-level

# Profile simulation performance
./nlm performance profile --duration=60 --output=profile.json

# Optimize brain configuration
./nlm performance optimize --target=firing_rate --value=50
```

| Option | Description | Default | Example |
|--------|-------------|---------|---------|
| `set-level` | Set optimization level (1-3) | 2 | `set-level 3` |
| `get-level` | Get current optimization level | - | - |
| `profile` | Profile performance | - | `profile --duration=60` |
| `optimize` | Optimize for specific metric | - | `optimize --target=firing_rate --value=50` |

### Performance Monitoring

```bash
# Start performance monitoring
./nlm performance monitor --interval=5 --duration=300

# Get real-time performance metrics
./nlm performance metrics --stream

# Set performance alerts
./nlm performance alert --cpu=80 --memory=90

# Generate performance report
./nlm performance report --output=performance_report.html
```

## Export Commands

### Brain Export

```bash
# Export brain state
./nlm export brain --file=brain_state.bin --compression=2

# Export brain configuration
./nlm export config --file=config.json

# Export brain statistics
./nlm export stats --file=stats.json
```

| Option | Description | Default | Example |
|--------|-------------|---------|---------|
| `--file` | Output file path | - | `--file=brain_state.bin` |
| `--compression` | Compression level (0-9) | 0 | `--compression=6` |
| `--format` | Output format (binary, json, text) | binary | `--format=json` |

### Configuration Export

```bash
# Export entire configuration
./nlm export config --output=config.json

# Export configuration subset
./nlm export config --include=plasticity --exclude=neuromodulation

# Import configuration
./nlm import config --file=config.json
```

## Import Commands

### Brain Import

```bash
# Import brain state
./nlm import brain --file=imported_brain.bin --validate

# Import and initialize
./nlm import brain --file=state.bin --initialize

# Compare imported brain
./nlm import brain --file=state1.bin --compare=state2.bin
```

| Option | Description | Default | Example |
|--------|-------------|---------|---------|
| `--file` | Input file path | - | `--file=state.bin` |
| `--validate` | Validate brain state before import | false | `--validate` |
| `--initialize` | Initialize brain after import | false | `--initialize` |
| `--compare` | Compare with existing brain | - | `--compare=current.bin` |

## Analysis Commands

### Brain Analysis

```bash
# Analyze brain connectivity
./nlm analyze connectivity --file=brain.bin --output=connectivity.json

# Analyze spike patterns
./nlm analyze spikes --file=brain.bin --pattern=temporal

# Analyze plasticity effects
./nlm analyze plasticity --file=brain.bin --metric=weight_change
```

### Experiment Analysis

```bash
# Analyze experiment results
./nlm analyze experiment --file=results.json --metric=firing_rate

# Generate performance statistics
./nlm analyze performance --file=metrics.json --summary

# Create comparison report
./nlm analyze compare --experiments=exp1.json,exp2.json
```

## Utility Commands

### Configuration Management

```bash
# Create configuration profile
./nlm config create --name=research --neurons=5000 --plasticity=both

# List configuration profiles
./nlm config list

# Get configuration details
./nlm config show --name=research

# Modify configuration profile
./nlm config modify --name=research --neurons=10000
```

### Batch Processing

```bash
# Run batch of simulations
./nlm batch run --config=parameters.json --output=batch_results

# Monitor batch processing
./nlm batch monitor --id=batch_123

# Stop batch processing
./nlm batch stop --id=batch_123
```

## Examples

### Example 1: Complete Brain Workflow

```bash
# Create and configure brain
./nlm brain create --neurons=2000 --plasticity=both --learning_rate=0.001

# Run simulation
./nlm simulation run --steps=1000 --world=2d --vision=8

# Save results
./nlm brain save --file=simulation_results.bin

# Export configuration
./nlm export config --file=simulation_config.json
```

### Example 2: Experiment Workflow

```bash
# Create experiment configuration
cat > exp_config.json << EOF
{
    "name": "working_memory_test",
    "type": "cognitive",
    "parameters": {
        "neuron_count": 3000,
        "plasticity": "stdp",
        "learning_rate": 0.0005
    },
    "duration": 2000,
    "trials": 50
}
EOF

# Run experiment
./nlm experiment run --config=exp_config.json

# Analyze results
./nlm experiment analyze --file=results.json --metric=firing_rate

# Export results
./nlm experiment export --name=working_memory_test --format=csv
```

### Example 3: Performance Monitoring

```bash
# Set up performance monitoring
./nlm performance monitor --interval=10 --duration=300

# Run simulation with monitoring
./nlm simulation run --steps=5000 --world=2d

# Check performance metrics
./nlm performance metrics --stream

# Generate performance report
./nlm performance report --output=performance_report.html
```

## Command Line Completion

### Bash Completion

```bash
# Add completion to ~/.bashrc
source <(nlm completion bash)

# Or use completion in your current session
nlm completion bash | source
```

### Fish Completion

```bash
# Add completion to ~/.config/fish/config.fish
source <(nlm completion fish)
```

## Environment Variables

| Variable | Description | Default | Example |
|----------|-------------|---------|---------|
| `NLM_HOME` | NLM installation directory | /usr/local/nlm | `/home/user/nlm` |
| `NLM_LOG_LEVEL` | Logging level (debug, info, warn, error) | info | `debug` |
| `NLM_OUTPUT_DIR` | Default output directory | ./results | `./simulations` |
| `NLM_CONFIG_PATH` | Default configuration file path | ./config.json | `/etc/nlm/config.json` |

## Troubleshooting

### Common Issues

#### "Command not found: nlm"

**Solution:**
```bash
# Add NLM to PATH
export PATH=$PATH:/usr/local/nlm/bin

# Or use absolute path
/usr/local/nlm/bin/nlm --help
```

#### "Permission denied: ./nlm"

**Solution:**
```bash
# Make executable
chmod +x ./nlm

# Or run with sudo (if needed)
sudo ./nlm --help
```

#### "Invalid command options"

**Solution:**
```bash
# Check help for available options
./nlm --help

# Or use specific command help
./nlm brain --help
```

### Error Messages

| Error Message | Possible Cause | Solution |
|---------------|----------------|----------|
| "Brain not initialized" | Brain not initialized before step | Call `./nlm brain initialize` first |
| "Invalid neuron count" | Neuron count out of range | Use `--neurons` with valid value (1-100000) |
| "File not found" | Configuration or data file not found | Check file path and permissions |
| "Memory allocation failed" | Insufficient memory | Increase system memory or reduce simulation size |

## Scripting

### Example Shell Script

```bash
#!/bin/bash

# NLM simulation script

# Configuration
NLM_EXEC="./nlm"
SIMULATION_STEPS=1000
NEURON_COUNT=2000

# Create brain
$nlm brain create --neurons=$NEURON_COUNT --plasticity=both

# Run simulation
$nlm simulation run --steps=$SIMULATION_STEPS --world=2d

# Save results
$nlm brain save --file=simulation_results.bin

# Export configuration
$nlm export config --file=simulation_config.json

# Analyze results
$nlm experiment analyze --file=results.json --metric=firing_rate

echo "Simulation completed successfully!"
```

### Example Python Script

```python
import subprocess
import json
import os

def run_nlm_simulation(neurons=2000, steps=1000):
    """Run NLM simulation using command line interface."""
    
    # Create brain
    cmd = [
        "./nlm",
        "brain", "create",
        "--neurons", str(neurons),
        "--plasticity", "both"
    ]
    
    subprocess.run(cmd, check=True)
    
    # Run simulation
    cmd = [
        "./nlm",
        "simulation", "run",
        "--steps", str(steps),
        "--world", "2d"
    ]
    
    subprocess.run(cmd, check=True)
    
    # Save results
    cmd = [
        "./nlm",
        "brain", "save",
        "--file", "simulation_results.bin"
    ]
    
    subprocess.run(cmd, check=True)
    
    # Load and return results
    with open("simulation_results.bin", "rb") as f:
        return f.read()

if __name__ == "__main__":
    try:
        results = run_nlm_simulation()
        print("Simulation completed successfully!")
        print(f"Results size: {len(results)} bytes")
    except subprocess.CalledProcessError as e:
        print(f"Simulation failed with error: {e}")
    except FileNotFoundError:
        print("NLM executable not found. Please check installation.")
```

## Advanced Features

### Custom Command Line Arguments

```bash
# Use custom configuration file
./nlm --config=/path/to/custom_config.json brain create

# Enable debug mode
./nlm --debug brain create

# Set output directory
./nlm --output=/custom/output brain create

# Configure logging
./nlm --log-level=debug --log-file=simulation.log brain create
```

### Parallel Processing

```bash
# Run multiple simulations in parallel
./nlm batch run --config=parallel_config.json --parallel=true

# Monitor parallel execution
./nlm batch monitor --id=parallel_batch
```

## Comparison with Python API

| Feature | Command Line | Python API |
|---------|--------------|------------|
| Basic simulation | `./nlm brain create --neurons=1000` | `brain = pynlm.createBrain(config)` |
| Configuration management | `./nlm export config --file=config.json` | `config.saveToFile("config.json")` |
| Monitoring | `./nlm brain monitor --duration=60` | Use Python monitoring libraries |
| Data analysis | `./nlm analyze connectivity --file=brain.bin` | Use Python data analysis libraries |
| Batch processing | `./nlm batch run --config=parameters.json` | Use Python loops and scripts |

## Best Practices

### For Command Line Usage

1. **Use configuration files** for complex setups
2. **Automate repetitive tasks** with scripts
3. **Monitor performance** during long simulations
4. **Validate inputs** before running simulations
5. **Backup important results** regularly

### For Scripting

1. **Use try-catch blocks** for error handling
2. **Log important steps** for debugging
3. **Use environment variables** for configuration
4. **Implement progress tracking** for long operations
5. **Set reasonable timeouts** for commands

## Future Enhancements

### Planned Features

1. **Interactive mode**: Enhanced command-line interface with tab completion
2. **Visual monitoring**: Real-time visualization of simulation progress
3. **Plugin system**: Extensible command-line interface
4. **Cloud integration**: Remote execution and monitoring
5. **API Gateway**: RESTful API for external control

### Known Limitations

1. **Limited scripting**: Basic shell scripting support
2. **No interactive editing**: Configuration must be set before execution
3. **Resource constraints**: Limited by system resources during long simulations
4. **Error recovery**: Limited automatic error recovery

## Support and Resources

### Getting Help

- **Documentation**: `nlm --help` or visit the NLM documentation website
- **Community**: Join the NLM user forum and Discord channel
- **Bug Reports**: Submit issues through the GitHub issue tracker
- **Feature Requests**: Submit feature requests through the GitHub issue tracker

### Additional Resources

- **API Reference**: `docs/API_REFERENCE.md`
- **Configuration Guide**: `docs/CONFIGURATION_GUIDE.md`
- **Performance Guide**: `docs/PERFORMANCE_GUIDE.md`
- **Experiment Guide**: `docs/EXPERIMENT_GUIDE.md`
- **Troubleshooting Guide**: `docs/TROUBLESHOOTING_GUIDE.md`

## Conclusion

The NLM command-line interface provides comprehensive control over brain simulations, experiment management, and performance monitoring. Whether you're running simple demonstrations or complex research experiments, the CLI offers powerful tools for controlling and analyzing NLM simulations.

For most users, the command-line interface provides an intuitive and efficient way to interact with NLM. However, for complex workflows or automated analysis, the Python API may provide more flexibility and integration with existing tools.

The choice between command-line and Python API depends on your specific needs:

- **Command Line**: Best for interactive use, one-time experiments, and simple automation
- **Python API**: Best for complex workflows, automated analysis, and integration with other tools

Both interfaces are fully equivalent and can be used together to leverage their respective strengths.