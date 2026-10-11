# Phase 6: Advanced CLI Examples

This document provides examples for using the NLM Advanced CLI system.

## Quick Start

The NLM Advanced CLI provides comprehensive command-line access to the NLM brain system. Here are some quick examples to get you started.

### Basic Operations

#### 1. Get Started with Help
```bash
# Show basic help
./nlm_cli --help

# Show advanced help
./nlm_cli --advanced --help

# Show help for specific command
./nlm_cli --advanced --brain-info --help
```

#### 2. Check Brain Status
```bash
# Display current brain status
./nlm_cli --advanced --brain-status

# Display detailed brain information
./nlm_cli --advanced --brain-info

# Reset brain to initial state
./nlm_cli --advanced --brain-reset
```

#### 3. Run Simulation
```bash
# Run simulation for 100 steps
./nlm_cli --advanced --simulate 100

# Continue simulation from current state
./nlm_cli --advanced --simulate-continue 50

# Execute single simulation step
./nlm_cli --advanced --simulate-step

# Run simulation in loop (10 iterations of 20 steps each)
./nlm_cli --advanced --simulate-loop 20

# Run simulation with profiling
./nlm_cli --advanced --simulate 1000 --simulate-profile
```

#### 4. Inspect Brain Components
```bash
# Show neuron information (first 10 neurons)
./nlm_cli --advanced --inspect-neurons 10

# Show detailed information for specific neuron
./nlm_cli --advanced --inspect-neuron 42

# Show synaptic information (first 20 synapses)
./nlm_cli --advanced --inspect-synapses 20

# Show region information
./nlm_cli --advanced --inspect-regions 5

# Show population information
./nlm_cli --advanced --inspect-populations 10

# Check memory system status
./nlm_cli --advanced --inspect-memory

# Check episodic memory status
./nlm_cli --advanced --inspect-episodic

# Check neuromodulation system
./nlm_cli --advanced --inspect-neuromodulation
```

#### 5. Save and Load Brain States
```bash
# Save brain state to file
./nlm_cli --advanced --brain-save my_brain_state.nml

# Load brain state from file
./nlm_cli --advanced --brain-load my_brain_state.nml

# Load brain configuration
./nlm_cli --advanced --brain-config configs/my_config.cfg

# Compare two brain states
./nlm_cli --advanced --brain-diff state1.nml state2.nml

# Dump brain state in binary format
./nlm_cli --advanced --brain-dump brain_dump.bin
```

#### 6. Analyze Neural Activity
```bash
# Analyze neural activity patterns
./nlm_cli --advanced --analyze-neural-activity

# Analyze plasticity patterns
./nlm_cli --advanced --analyze-plasticity

# Analyze memory system usage
./nlm_cli --advanced --analyze-memory-usage

# Analyze system performance
./nlm_cli --advanced --analyze-performance

# Analyze STDP timing patterns
./nlm_cli --advanced --analyze-stdp-timing

# Analyze Hebbian co-activity patterns
./nlm_cli --advanced --analyze-hebbian-coactivity
```

#### 7. Debug and Diagnostics
```bash
# Show recent spike events (100 events)
./nlm_cli --advanced --debug-spike-events 100

# Show spike history for specific neuron
./nlm_cli --advanced --debug-spike-history 123

# Show plasticity history for specific synapse
./nlm_cli --advanced --debug-plasticity-history 456

# Show recent memory traces (50 traces)
./nlm_cli --advanced --debug-memory-traces 50

# Show recent plastic changes for synapse
./nlm_cli --advanced --debug-plastic-change 789

# Check simulation clock state
./nlm_cli --advanced --debug-simulation-clock

# Check checkpoint system status
./nlm_cli --advanced --debug-checkpoint-status

# Check memory system consistency
./nlm_cli --advanced --debug-memory-consistency

# Check plasticity system consistency
./nlm_cli --advanced --debug-plasticity-consistency

# Check neural system consistency
./nlm_cli --advanced --debug-neural-consistency
```

#### 8. Export and Import Data
```bash
# Export neural connectivity graph
./nlm_cli --advanced --export-neural-graph neural_graph.json

# Export synaptic weight matrix
./nlm_cli --advanced --export-synaptic-matrix synaptic_matrix.csv

# Export memory trace data
./nlm_cli --advanced --export-memory-traces memory_traces.json

# Export plasticity history data
./nlm_cli --advanced --export-plasticity-data plasticity_data.json

# Export behavioral log data
./nlm_cli --advanced --export-behavior-log behavior_log.json

# Import neural connectivity graph
./nlm_cli --advanced --import-neural-graph neural_graph.json

# Import synaptic weight matrix
./nlm_cli --advanced --import-synaptic-matrix synaptic_matrix.csv

# Import behavioral log data
./nlm_cli --advanced --import-behavior-log behavior_log.json
```

#### 9. Experiment Management
```bash
# Create new experiment
./nlm_cli --advanced --experiment-create my_experiment

# List all experiments
./nlm_cli --advanced --experiment-list

# Run experiment
./nlm_cli --advanced --experiment-run my_experiment

# Show experiment status
./nlm_cli --advanced --experiment-status my_experiment

# Show experiment results
./nlm_cli --advanced --experiment-results my_experiment

# Export experiment data
./nlm_cli --advanced --experiment-export my_experiment

# Import experiment data
./nlm_cli --advanced --experiment-import my_experiment
```

#### 10. Advanced Features
```bash
# Run parameter sweep experiment
./nlm_cli --advanced --parameter-sweep params.cfg

# Run Monte Carlo simulation (1000 runs)
./nlm_cli --advanced --monte-carlo 1000

# Analyze parameter sensitivity
./nlm_cli --advanced --sensitivity-analysis learning_rate

# Enable online learning mode
./nlm_cli --advanced --online-learning

# Enable batch learning mode
./nlm_cli --advanced --batch-learning

# Enable curriculum learning mode
./nlm_cli --advanced --curriculum-learning
```

## Working with the Advanced CLI

### Command Groups

The advanced CLI is organized into 7 command groups:

1. **Brain Management** - Load, save, and manage brain states
2. **Simulation Control** - Control simulation execution and timing
3. **Real-time Inspection** - Inspect brain components and systems
4. **Analysis** - Analyze neural activity and system performance
5. **Debugging** - Debug and diagnose system issues
6. **Export/Import** - Export and import brain data
7. **Advanced Features** - Experiments and parameter analysis

### Command Structure

Most advanced commands follow this pattern:
```bash
./nlm_cli --advanced --<command-group> <options>
```

### Command Options

#### Brain Management Commands
- `--brain-config <file>`: Load brain configuration
- `--brain-save <file>`: Save brain state to file
- `--brain-load <file>`: Load brain state from file
- `--brain-info`: Display detailed brain information
- `--brain-reset`: Reset brain to initial state
- `--brain-dump <file>`: Dump brain state in binary format
- `--brain-diff <file1> <file2>`: Compare two brain states
- `--brain-status`: Display current brain status
- `--brain-validate`: Validate brain state consistency

#### Simulation Control Commands
- `--simulate <steps>`: Run simulation for specified steps
- `--simulate-continue <steps>`: Continue from current state
- `--simulate-pause`: Pause simulation
- `--simulate-resume`: Resume simulation
- `--simulate-step`: Execute single simulation step
- `--simulate-rewind <steps>`: Rewind simulation
- `--simulate-fastforward <factor>`: Run simulation at accelerated speed
- `--simulate-loop <steps>`: Run simulation in loop
- `--simulate-profile`: Profile simulation performance
- `--simulate-benchmark`: Run performance benchmark

#### Real-time Inspection Commands
- `--inspect-neurons <count>`: Show neuron information
- `--inspect-neuron <id>`: Show detailed neuron information
- `--inspect-synapses <count>`: Show synapse information
- `--inspect-synapse <id>`: Show detailed synapse information
- `--inspect-regions <count>`: Show region information
- `--inspect-region <id>`: Show detailed region information
- `--inspect-populations <count>`: Show population information
- `--inspect-population <id>`: Show detailed population information
- `--inspect-memory`: Show memory system status
- And more...

#### Analysis Commands
- `--analyze-neural-activity`: Analyze neural activity patterns
- `--analyze-plasticity`: Analyze plasticity patterns
- `--analyze-memory-usage`: Analyze memory system usage
- `--analyze-performance`: Analyze system performance
- `--analyze-stdp-timing`: Analyze STDP timing patterns
- `--analyze-hebbian-coactivity`: Analyze Hebbian co-activity patterns
- And more...

#### Debugging Commands
- `--debug-spike-events <count>`: Show recent spike events
- `--debug-spike-history <neuron>`: Show spike history for neuron
- `--debug-plasticity-history <synapse>`: Show plasticity history for synapse
- `--debug-memory-traces <count>`: Show recent memory traces
- `--debug-plastic-change <synapse>`: Show recent plastic changes
- `--debug-neuromodulation`: Show neuromodulation levels
- And more...

#### Export/Import Commands
- `--export-neural-graph <file>`: Export neural connectivity graph
- `--export-synaptic-matrix <file>`: Export synaptic weight matrix
- `--export-memory-traces <file>`: Export memory trace data
- `--export-plasticity-data <file>`: Export plasticity history data
- `--export-behavior-log <file>`: Export behavioral log data
- And more...

#### Advanced Features
- `--experiment-create <name>`: Create new experiment
- `--experiment-run <name>`: Run experiment
- `--experiment-list`: List all experiments
- `--experiment-status <name>`: Show experiment status
- And more...

### Command Completion

The advanced CLI supports command completion to help you discover available commands:

```bash
# Get completion suggestions for a partial command
./nlm_cli --advanced --b

# Show all available commands
./nlm_cli --advanced --help
```

### Command History

The advanced CLI maintains a history of executed commands:
```bash
# Check command history (not implemented yet)
./nlm_cli --advanced --command-history
```

### Error Handling

The advanced CLI provides detailed error messages:
```bash
# Error for invalid command
./nlm_cli --advanced --invalid-command
# Error: Unknown command: --invalid-command

# Error for missing required argument
./nlm_cli --advanced --brain-save
# Error: Missing required argument for --brain-save
```

### Output Formatting

The advanced CLI provides formatted output:
```bash
# Progress indicators for long-running commands
./nlm_cli --advanced --simulate 1000
# [=====>                                     ] 30% Step 300/1000

# Formatted information for inspection commands
./nlm_cli --advanced --inspect-neuron 42
# Neuron 42:
#   Type: Sensory
#   Membrane Potential: 0.234
#   Threshold: 0.250
#   Is Firing: false
```

## Integration with Original CLI

The advanced CLI maintains full backward compatibility with the original NLM CLI:

### Original CLI Usage
```bash
# Original CLI
./nlm_cli --config configs/default.cfg

# Advanced CLI with original functionality
./nlm_cli --advanced --config configs/default.cfg --brain-info
```

### Automatic Detection
```bash
# Without --advanced flag: Uses original CLI
./nlm_cli --config configs/my_config.cfg

# With --advanced flag: Uses advanced CLI
./nlm_cli --advanced --config configs/my_config.cfg
```

## Configuration

The advanced CLI uses the same configuration system as the original NLM:

### Configuration Files
```bash
# Default configuration
./nlm_cli --advanced --config configs/default.cfg

# Advanced configuration
./nlm_cli --advanced --config configs/advanced.cfg

# Debug configuration
./nlm_cli --advanced --config configs/debug.cfg
```

### Command Line Configuration
```bash
# Override configuration options via command line
./nlm_cli --advanced --brain-config configs/my_config.cfg --brain-info
```

## Performance Tips

### For Large Brain Systems
```bash
# Use batch operations for large datasets
./nlm_cli --advanced --inspect-neurons 1000

# Use sampling for large datasets
./nlm_cli --advanced --inspect-synapses 100
```

### For Long-Running Commands
```bash
# Save intermediate results
./nlm_cli --advanced --brain-save intermediate_state.nml

# Use simulation loops for repetitive tasks
./nlm_cli --advanced --simulate-loop 100
```

### For Debugging
```bash
# Run with debug mode
./nlm_cli --advanced --simulate 100 --debug

# Check system consistency
./nlm_cli --advanced --debug-memory-consistency
```

## Troubleshooting

### Common Issues

#### Issue: Command not found
**Solution**: Use `--help` to see all available commands
```bash
./nlm_cli --advanced --help
```

#### Issue: Missing required argument
**Solution**: Check command syntax
```bash
./nlm_cli --advanced --brain-save my_brain_state.nml
```

#### Issue: File not found
**Solution**: Check file path and permissions
```bash
# Check if file exists
./nlm_cli --advanced --brain-load my_brain_state.nml
```

#### Issue: Memory usage too high
**Solution**: Use sampling or reduce count
```bash
./nlm_cli --advanced --inspect-neurons 10
```

### Getting Help

#### General Help
```bash
# Show all available commands
./nlm_cli --advanced --help

# Show help for specific command group
./nlm_cli --advanced brain --help

# Show help for specific command
./nlm_cli --advanced --brain-info --help
```

#### Command Examples
```bash
# See examples for specific command
./nlm_cli --advanced --brain-info --help

# See examples for inspection commands
./nlm_cli --advanced --inspect-neurons --help
```

## Best Practices

### Command Organization
- Group related commands together
- Use descriptive command names
- Use consistent option formats (--option format)

### Output Management
- Use quiet mode for scripts
- Redirect output to files for logging
- Use progress indicators for long-running commands

### Configuration Management
- Use configuration files for complex setups
- Use command line arguments for quick tests
- Backup important brain states before modifications

### Performance
- Use sampling for large datasets
- Monitor memory usage
- Use appropriate simulation speeds

### Debugging
- Use debug commands to check system state
- Check command history for patterns
- Use consistent naming conventions

## Conclusion

The NLM Advanced CLI provides a comprehensive command-line interface for the NLM brain system. It maintains full backward compatibility while adding 60+ advanced commands organized into logical groups. The advanced CLI transforms the NLM project from a basic research prototype into a powerful production tool with extensive command-line capabilities for advanced users.

Use the examples above to get started, and explore the full command set to discover all the capabilities of the NLM Advanced CLI system.
