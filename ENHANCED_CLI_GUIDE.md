# NLM Enhanced CLI - Installation and Usage Guide

## Overview

The NLM (Neural Learning Machine) now features an enhanced Command-Line Interface (CLI) system that provides comprehensive brain manipulation, analysis, and control capabilities. This represents Phase 7: Enhanced User Interface and Control Systems for the Phase 6 integrated brain.

## Installation

### Build from Source

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the enhanced CLI
make -j4
```

### Quick Start

After building, you can use the enhanced NLM CLI in two modes:

#### Interactive Mode (Default)
```bash
./nlm --interactive
```

#### Headless Mode with Command
```bash
./nlm brain init configs/default.cfg
./nlm simulation run 100
./nlm neurons list
./nlm stats
```

## Command Structure

The enhanced CLI provides a modular command structure organized into categories:

### Core Commands

- **help, -h, --help**: Show help message with all available commands
- **version**: Show version information
- **quit, exit**: Exit CLI

### Brain Control Commands

- **brain init [config]**: Initialize brain with configuration
- **brain reset**: Reset brain to initial state
- **brain status**: Show brain status and statistics

### Neuron Commands

- **neurons list [region] [type]**: List all neurons with filtering
- **neurons inject <id> <current> [region]**: Inject current into neuron
- **neurons type <id> <type> [region]**: Set neuron type
- **neurons spike <id> [region]**: Trigger spike in neuron

### Synapse Commands

- **synapses list [region]**: List all synapses
- **synapses enable <id> [region]**: Enable plasticity on synapse
- **synapses disable <id> [region]**: Disable plasticity on synapse
- **synapses set-weight <id> <weight> [region]**: Set synapse weight

### Memory Commands

- **memory working**: Show working memory state
- **memory episodic**: Show episodic memory state
- **memory associative**: Show associative memory state
- **memory clear**: Clear all memory systems

### Neuromodulation Commands

- **neuromodulation status**: Show neuromodulator levels
- **neuromodulation set <dopamine|curiosity|novelty> <level>**: Set neuromodulator level

### Simulation Commands

- **simulation run <steps> [record]**: Run simulation for specified steps
- **simulation step [<step_number>]**: Run single simulation step
- **simulation status**: Show simulation status

### Plasticity Commands

- **plasticity enable [region]**: Enable plasticity on all synapses
- **plasticity disable [region]**: Disable plasticity on all synapses
- **plasticity update [region]**: Update plasticity rules

### Save/Load Commands

- **save <filepath>**: Save brain state to file
- **load <filepath>**: Load brain state from file
- **replay <episodes>**: Replay last N episodes

### Analysis Commands

- **analyze network**: Analyze network connectivity
- **analyze learning**: Analyze learning patterns
- **analyze plasticity**: Analyze plasticity effects
- **stats**: Show comprehensive statistics

### Visualization Commands

- **visualization enable [window_size]**: Enable visualization
- **visualization disable**: Disable visualization

### Utility Commands

- **benchmark <steps>**: Run performance benchmark
- **reset**: Reset brain to initial state

## Configuration Management

### Configuration File Formats

NLM supports multiple configuration file formats:

#### 1. Plain Text Format (configs/default.cfg)
```
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
log_level = INFO
```

#### 2. Command Line Arguments
```bash
./nlm brain init --config custom_config.cfg
./nlm brain init --random_seed=123 --neuron_count=2000
./nlm brain init -t 0.0005 -n 500
```

### Configuration Key Types

Configuration values support multiple data types:

- **Integers**: `neuron_count = 1000`
- **Floats**: `simulation_timestep = 0.001`
- **Booleans**: `log_to_file = true`
- **Strings**: `environment_name = "GridWorld"`
- **Lists**: `supported_actions = ["move_forward", "turn_left", "turn_right"]`

## Quick Examples

### Example 1: Initialize and Run Basic Simulation
```bash
# Initialize brain with default configuration
./nlm brain init

# Run 100 simulation steps
./nlm simulation run 100

# Show brain statistics
./nlm stats

# Save brain state for later analysis
./nlm save brain_state.checkpoint
```

### Example 2: Interactive Exploration
```bash
# Start interactive mode
./nlm --interactive

# Inside interactive mode:
NLM> brain init configs/performance.cfg
NLM> neurons list
NLM> simulation run 50
NLM> neurons inject 42 50.0
NLM> stats
NLM> save test_brain.checkpoint
NLM> quit
```

### Example 3: Advanced Brain Manipulation
```bash
# Initialize with custom settings
./nlm brain init --neuron_count=2000 --region_count=4 --connection_probability=0.2

# Enable plasticity and run learning experiment
./nlm plasticity enable
./nlm simulation run 1000 record

# Analyze learning results
./nlm analyze learning
./nlm analyze plasticity

# Reset and start fresh
./nlm reset
```

### Example 4: Performance Benchmarking
```bash
# Run benchmark with 1000 steps
./nlm benchmark 1000

# Compare performance with different configurations
./nlm brain init configs/optimistic.cfg
./nlm benchmark 500
./nlm brain init configs/conservative.cfg
./nlm benchmark 500
```

## Advanced Features

### Command Scripts

Create script files for complex sequences:

```bash
#!/bin/bash
# learning_experiment.sh

./nlm brain init configs/learning.cfg
./nlm plasticity enable

for i in {1..10}; do
    ./nlm simulation run 100
    ./nlm neurons list > step_$(printf "%03d" $i).txt
    ./nlm analyze learning
    ./nlm stats >> analysis.log
    ./nlm save checkpoint_$i.checkpoint
    ./nlm reset
    sleep 1
    echo "Completed iteration $i"
done

./nlm brain save final_learning.checkpoint
```

### Configuration Templates

NLM includes configuration templates in the configs/ directory:

- **configs/default.cfg**: Basic configuration for testing
- **configs/performance.cfg**: Optimized for performance
- **configs/learning.cfg**: Optimized for learning experiments
- **configs/debug.cfg**: Detailed logging for debugging

## API for Extensions

The enhanced CLI is designed for extensibility:

### Adding New Commands

```cpp
class CustomCommand : public CLICommand {
public:
    std::string getName() const override { return "custom"; }
    std::string getDescription() const override { return "Custom command for special functionality"; }
    std::string getUsage() const override { return "custom <arg1> [arg2]"; }
    bool execute(NLMBrainCLI& cli, const std::vector<std::string>& args) override {
        // Implement custom functionality
        return true;
    }
};
```

### Command Registration

Commands can be registered dynamically:

```cpp
void NLMBrainCLI::initializeCommands() {
    // ... existing commands ...
    
    // Add custom command
    addCommand(std::make_unique<CustomCommand>());
}
```

## Troubleshooting

### Common Issues

#### 1. "Brain not initialized"
```bash
# Solution: Initialize brain first
./nlm brain init
```

#### 2. Configuration file not found
```bash
# Solution: Use absolute path or place config in working directory
./nlm brain init /path/to/config.cfg
```

#### 3. Permission denied
```bash
# Solution: Check file permissions
chmod 644 configs/*.cfg
```

#### 4. Memory issues with large networks
```bash
# Solution: Reduce neuron count
./nlm brain init --neuron_count=100 --region_count=1
```

### Debug Mode

Run with verbose logging:
```bash
./nlm --verbose brain init
```

## Integration with Other Tools

### Jupyter Notebook Integration
```python
import subprocess
import json

# Run NLM simulation and capture output
result = subprocess.run([
    './nlm', 'brain', 'init', '--neuron_count=100',
    'simulation', 'run', '50'
], capture_output=True, text=True)

# Parse statistics
print(result.stdout)
```

### Python Bindings
```python
from nlm import Brain, Config

# Create brain with custom configuration
config = Config()
config.loadFromFile('configs/learning.cfg')
brain = Brain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step, step * 0.001)

# Save brain state
brain.save('python_brain.checkpoint')
```

## Performance Tips

1. **Large Networks**: Use `configs/performance.cfg` for optimized settings
2. **Memory Usage**: Monitor with `stats` command regularly
3. **Plasticity**: Disable for benchmarking (`plasticity disable`)
4. **Simulation Speed**: Adjust timestep in configuration
5. **Parallel Processing**: Enable multithreading in config

## Development

### Adding New Command Categories

1. Create new command class implementing `CLICommand`
2. Implement execute() method with argument parsing
3. Add command to NLMBrainCLI::initializeCommands()
4. Update help system

### Configuration Schema

To add new configuration parameters:

1. Update config file templates in configs/
2. Add default values in CLI initialization
3. Update documentation
4. Test with various configurations

### Testing Commands

Test individual commands:
```bash
./nlm neurons list
./nlm synapses list
./nlm memory working
./nlm stats
./nlm benchmark 10
```

## Migration Guide

### From Basic CLI to Enhanced CLI

If you were using the basic NLM CLI (./nlm), migrate to the enhanced version with these changes:

#### Old Commands (./nlm)
```bash
# Phase 2 basic usage
./nlm --config test.cfg
```

#### New Commands (./nlm enhanced CLI)
```bash
# Enhanced usage with brain control
./nlm brain init test.cfg
./nlm brain status
./nlm simulation run 100
```

### Backward Compatibility

The enhanced CLI maintains backward compatibility for basic usage patterns while providing additional functionality for advanced users.

## Conclusion

The NLM Enhanced CLI represents a significant advancement in brain simulation control and analysis capabilities. It provides:

- **Comprehensive Control**: Full control over brain components
- **Advanced Analysis**: Detailed statistics and performance metrics
- **Flexible Configuration**: Multiple configuration formats
- **Extensible Design**: Easy to add new commands and features
- **Interactive Mode**: Rich interactive experience for exploration
- **Script Support**: Command-line scripting for automation

This enhanced CLI makes NLM more accessible to researchers, developers, and users while maintaining the powerful neural simulation capabilities that make NLM unique.
