// NLM Advanced CLI - Command Reference Manual
// Complete reference for all advanced commands

# NLM Advanced CLI Commands

This document provides comprehensive information about all advanced commands available in the NLM Advanced CLI interface.

## Overview

The NLM Advanced CLI provides powerful command-line control over the Neural Learning Machine, including:

- Memory management (working memory, episodic memory)
- Prediction analysis and confidence assessment
- Neuromodulation control (dopamine, acetylcholine, norepinephrine, serotonin)
- Brain state inspection and analysis
- Performance profiling and debugging
- Simulation control (pause, resume, step, jump)
- Real-time monitoring and visualization
- Advanced configuration management
- Script-based batch processing
- System diagnostics and health monitoring

## Command Categories

### 1. Memory Management Commands

#### `working_memory`
**Description**: Working memory analysis and management

**Subcommands**:
- `info`: Display working memory status
- `store <neuron> <value>`: Store a value for a neuron
- `retrieve <neuron>`: Retrieve value for a neuron
- `clear`: Clear all working memory
- `stats`: Show working memory statistics

**Examples**:
```bash
working_memory info
working_memory store 42 0.75
working_memory retrieve 42
working_memory clear
working_memory stats
```

#### `episodic_memory`
**Description**: Episodic memory queries and management

**Subcommands**:
- `info`: Display episodic memory status
- `store`: Store an episode (requires episode data)
- `retrieve <index>`: Retrieve an episode by index
- `recent <count>`: Get recent episodes
- `consolidate <threshold>`: Consolidate memories

**Examples**:
```bash
already_memory info
already_memory recent 10
already_memory consolidate 0.5
```

#### `prediction_analysis`
**Description**: Prediction confidence and error analysis

**Subcommands**:
- `info`: Display prediction system status
- `confidence`: Show prediction confidence metrics
- `error_analysis`: Analyze prediction errors
- `system_info`: Show prediction system integration status

**Examples**:
```bash
prediction_analysis info
prediction_analysis confidence
prediction_analysis error_analysis
prediction_analysis system_info
```

### 2. Neuromodulation Control Commands

#### `neuromodulation`
**Description**: Neuromodulation control and monitoring

**Subcommands**:
- `info`: Display neuromodulation system status
- `set_dopamine <level>`: Set dopamine level
- `set_acetylcholine <level>`: Set acetylcholine (ACh) level
- `set_norepinephrine <level>`: Set norepinephrine (NE) level
- `set_serotonin <level>`: Set serotonin (5-HT) level
- `set_curiosity <level>`: Set curiosity level
- `set_levels [level]`: Set all neuromodulators to specified level (default: 0.5)
- `get_levels`: Display current neuromodulator levels
- `status`: Show system integration status

**Examples**:
```bash
neuromodulation info
neuromodulation set_acetylcholine 0.8
neuromodulation set_norepinephrine 0.3
neuromodulation get_levels
neuromodulation status
```

### 3. State Analysis Commands

#### `state_analysis`
**Description**: Brain state inspection and analysis

**Subcommands**:
- `info`: Display basic brain state information
- `neurons <limit>`: Analyze neurons (specify limit)
- `synapses <limit>`: Analyze synapses (specify limit)
- `regions`: Display brain region information
- `connections`: Display inter-region connections
- `memory`: Show memory system status
- `cognition`: Show cognition system status
- `full_report`: Generate comprehensive brain state report

**Examples**:
```bash
state_analysis info
state_analysis neurons 10
state_analysis full_report
state_analysis cognition
```

#### `performance`
**Description**: Performance profiling and debugging

**Subcommands**:
- `info`: Display performance system status
- `memory_usage`: Analyze memory usage
- `cpu_profiling`: CPU performance profiling
- `checkpoint_info`: Show checkpoint system information
- `system_health`: Comprehensive system health monitoring

**Examples**:
```bash
performance info
performance memory_usage
performance system_health
```

### 4. Simulation Control Commands

#### `simulation_control`
**Description**: Extended simulation control (pause, resume, jump frames)

**Subcommands**:
- `pause`: Pause simulation
- `resume`: Resume simulation
- `step`: Advance one simulation step
- `step <count>`: Advance multiple simulation steps
- `jump <step>`: Jump to specific simulation step
- `reset`: Reset simulation to initial state
- `status`: Display simulation status
- `limits`: Show simulation configuration limits

**Examples**:
```bash
simulation_control pause
simulation_control step 100
simulation_control jump 1000
simulation_control reset
```

#### `visualization`
**Description**: Real-time monitoring and visualization control

**Subcommands**:
- `toggle [enable]`: Toggle visualization (specify true/false)
- `update_rate <rate>`: Set visualization update rate (Hz)
- `status`: Display visualization system status
- `data`: Export data for visualization
- `create_window`: Create visualization window

**Examples**:
```bash
visualization toggle true
visualization update_rate 30
visualization data
```

### 5. Configuration Commands

#### `configuration`
**Description**: Advanced configuration management

**Subcommands**:
- `show`: Display current configuration
- `set <key> <value>`: Set configuration value
- `get <key>`: Get configuration value
- `save <file>`: Save configuration to file
- `load <file>`: Load configuration from file
- `validate`: Validate configuration
- `template`: Show configuration templates

**Examples**:
```bash
configuration show
configuration set neuron_count 1000
configuration get neuron_count
configuration save my_config.cfg
configuration load my_config.cfg
```

### 6. Scripting and Batch Processing Commands

#### `script`
**Description**: Script-based control and batch processing

**Subcommands**:
- `execute <commands>`: Execute batch of commands (semicolon-separated)
- `load <file>`: Load script from file
- `save <file>`: Save script to file
- `list`: List available script templates

**Examples**:
```bash
script execute "working_memory info; neuromodulation get_levels; state_analysis info"
script load experiment_script.txt
script list
```

### 7. Diagnostic Commands

#### `diagnostic`
**Description**: Error detection and system health monitoring

**Subcommands**:
- `system`: System health check
- `brain`: Brain system check
- `neuromodulation`: Neuromodulation system check
- `memory`: Memory system check
- `prediction`: Prediction system check
- `full`: Comprehensive diagnostic report

**Examples**:
```bash
diagnostic system
diagnostic brain
diagnostic full
```

## Advanced Usage Examples

### Example 1: Complete Brain Analysis
```bash
# Run a comprehensive brain analysis
state_analysis full_report

# Check neuromodulation system
neuromodulation get_levels

# Analyze memory systems
working_memory stats
episodic_memory info

# Check performance
performance system_health
```

### Example 2: Experiment Setup Script
```bash
# Create a script for running experiments
script load experiment_setup.txt

# Execute experiment commands
script execute "simulation_control reset; configuration set neuron_count 1000; working_memory clear; state_analysis info"
```

### Example 3: Real-time Monitoring
```bash
# Start real-time monitoring
while true; do
    simulation_control status
    neuromodulation get_levels
    working_memory stats
    sleep 5
done
```

## Integration with Existing NLM

The advanced CLI maintains full backward compatibility with existing NLM functionality:

- Legacy command-line arguments (--config, etc.) still work
- Original Phase 2 demonstrations run when --advanced flag is not used
- All existing configuration file loading is preserved
- Original brain simulation functionality remains intact

## Getting Started

1. **Basic Usage**:
   ```bash
   ./nlm --advanced
   ```

2. **Help**:
   ```bash
   ./nlm --advanced help
   ./nlm --advanced help <command_name>
   ```

3. **List Commands**:
   ```bash
   ./nlm --advanced commands
   ```

4. **Examples**:
   ```bash
   ./nlm --advanced working_memory info
   ./nlm --advanced neuromodulation get_levels
   ./nlm --advanced state_analysis full_report
   ```

## Command History and Auto-completion

The advanced CLI supports:

- Command history (up arrow keys)
- Auto-completion (tab key)
- Command filtering (search with text)
- Session management (save/load sessions)

## Configuration File Support

Configuration can be specified in:

1. **Default configuration file**: `configs/default.cfg`
2. **Command-line arguments**: `--key value` or `--key=value`
3. **Session files**: Save/load CLI sessions

Example configuration file (`my_config.cfg`):
```ini
# NLM Configuration File
neuron_count = 1000
region_count = 2
connection_probability = 0.1
random_seed = 42
simulation_timestep = 0.001
max_simulation_steps = 10000
visualization_enabled = false
log_level = INFO
```

## Performance Considerations

- Commands are executed efficiently with minimal overhead
- Memory usage is optimized for large brain simulations
- Performance monitoring allows tracking of command execution times
- Configuration caching reduces file I/O operations

## Troubleshooting

### Common Issues and Solutions

1. **Command not found**:
   ```bash
   Check spelling and try: help command_name
   ```

2. **Permission denied**:
   ```bash
   Ensure you have write permissions to configuration files
   ```

3. **Brain system not available**:
   ```bash
   The brain system may not be fully initialized. Try running basic NLM first.
   ```

4. **Configuration file errors**:
   ```bash
   Check syntax in configuration file
   ```

## Technical Details

### Command Parsing

Commands are parsed using a flexible syntax:

```bash
command_name [subcommand] [options] [arguments]

Options can be specified in two formats:
  --option value
  --option=value

Arguments are specified without the '--' prefix:
  command arg1 arg2
```

### Error Handling

The CLI provides comprehensive error handling:

- Command validation before execution
- Detailed error messages with suggestions
- Graceful handling of invalid inputs
- Session recovery capabilities

### API Integration

The advanced CLI provides programmatic access through:

- Command execution APIs
- Configuration management APIs
- Event notification system
- Plugin architecture for custom commands

## Future Enhancements

Planned enhancements include:

1. **Enhanced visualization**: Real-time graphs and plots
2. **Machine learning integration**: AI-powered analysis
3. **Distributed computing**: Multi-machine simulation support
4. **Web interface**: Browser-based visualization
5. **Plugin system**: Custom command development

## Conclusion

The NLM Advanced CLI provides a comprehensive and powerful interface for controlling and analyzing neural simulations. It maintains backward compatibility while offering advanced features for experienced users. The modular architecture allows for easy extension and customization.

For the latest documentation and examples, visit the NLM documentation portal.
