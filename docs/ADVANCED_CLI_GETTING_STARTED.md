# NLM Advanced CLI - Getting Started Guide

## Overview

The NLM Advanced CLI provides an enhanced command-line interface for the Neural Learning Machine (NLM), offering advanced features for experienced users while maintaining full backward compatibility.

## Installation

### Prerequisites

- NLM Phase 6 (Final Integration) is required
- C++ compiler with C++20 support
- CMake 3.16 or higher

### Build Instructions

1. Build NLM core libraries (if not already built):
   ```bash
   cd /workspace/app
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. The advanced CLI is included in the main NLM build (see `src/main_advanced.cpp`).

## Basic Usage

### Launching the Advanced CLI

```bash
# Launch interactive advanced CLI
./nlm --advanced

# Launch with specific configuration
./nlm --advanced --config my_config.cfg

# Launch in quiet mode
./nlm --advanced --quiet

# Launch in verbose mode
./nlm --advanced --verbose
```

### Command Categories

The advanced CLI provides commands organized into logical categories:

- **Memory Management**: `working_memory`, `episodic_memory`, `prediction_analysis`
- **Neuromodulation Control**: `neuromodulation`
- **State Analysis**: `state_analysis`, `performance`
- **Simulation Control**: `simulation_control`, `visualization`
- **Configuration Management**: `configuration`
- **Scripting and Batch Processing**: `script`
- **Diagnostics**: `diagnostic`

### Help System

#### Show General Help
```bash
./nlm --advanced help
```

#### Show Command-Specific Help
```bash
./nlm --advanced help working_memory
./nlm --advanced help neuromodulation
./nlm --advanced help state_analysis
```

#### List All Commands
```bash
./nlm --advanced commands
```

## Quick Start Examples

### 1. Basic Brain Analysis
```bash
./nlm --advanced
>>> state_analysis info
>>> neuromodulation get_levels
>>> working_memory stats
>>> performance system_health
```

### 2. Working Memory Management
```bash
./nlm --advanced
>>> working_memory info
>>> working_memory store 42 0.75
>>> working_memory retrieve 42
>>> working_memory clear
>>> working_memory stats
```

### 3. Neuromodulation Control
```bash
./nlm --advanced
>>> neuromodulation info
>>> neuromodulation set_acetylcholine 0.8
>>> neuromodulation set_norepinephrine 0.3
>>> neuromodulation get_levels
```

### 4. Episodic Memory Queries
```bash
./nlm --advanced
>>> episodic_memory info
>>> episodic_memory recent 10
>>> episodic_memory consolidate 0.5
```

### 5. Prediction Analysis
```bash
./nlm --advanced
>>> prediction_analysis info
>>> prediction_analysis confidence
>>> prediction_analysis error_analysis
```

### 6. Simulation Control
```bash
./nlm --advanced
>>> simulation_control info
>>> simulation_control step 100
>>> simulation_control reset
```

### 7. Configuration Management
```bash
./nlm --advanced
>>> configuration show
>>> configuration set neuron_count 1000
>>> configuration get neuron_count
>>> configuration save my_config.cfg
```

### 8. Script-Based Control
```bash
./nlm --advanced
>>> script execute "working_memory info; neuromodulation get_levels; state_analysis info"
>>> script load experiment_script.txt
>>> script list
```

### 9. Diagnostics
```bash
./nlm --advanced
>>> diagnostic system
>>> diagnostic brain
>>> diagnostic full
```

## Advanced Features

### Command History

- Use up/down arrow keys to navigate command history
- Type `clear` or `cls` to clear the screen
- Commands are automatically saved to history file

### Auto-completion

- Press Tab key to auto-complete command names
- Press Tab key after partial command to see options

### Script Files

Create script files for batch processing:

```bash
# Create a script file
nano experiment_script.nlm

# Example script content
simulation_control reset
configuration set neuron_count 500
working_memory clear
state_analysis info
neuromodulation get_levels
performance system_health
```

Execute script:
```bash
./nlm --advanced script load experiment_script.nlm
```

### Configuration Files

Create custom configuration files:

```bash
cat > my_config.cfg <<EOF
# NLM Configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.1
random_seed = 42
simulation_timestep = 0.001
max_simulation_steps = 10000
visualization_enabled = false
log_level = INFO
EOF
```

Load custom configuration:
```bash
./nlm --advanced --config my_config.cfg
```

### Command Templates

Many commands support templates:

```bash
# Show configuration templates
./nlm --advanced configuration template

# Use configuration template
./nlm --advanced configuration load default_template.cfg
```

## Integration with Existing NLM

### Legacy Compatibility

The advanced CLI maintains full compatibility with existing NLM:

- Original command-line arguments still work:
  ```bash
  ./nlm --config my_config.cfg
  ```

- Legacy demos still run:
  ```bash
  ./nlm  # Runs Phase 2 demonstrations
  ```

- Existing configuration files are supported:
  ```bash
  # configs/default.cfg
  # configs/*.cfg
  ```

### Configuration System

The advanced CLI extends the existing NLM configuration system:

1. **File-based configuration**: Load from `.cfg` files
2. **Command-line configuration**: Override with `--key value` or `--key=value`
3. **Runtime configuration**: Set during interactive session
4. **Configuration validation**: Check configuration integrity

### Brain Integration

The advanced CLI integrates with all NLM brain systems:

- **Memory Systems**: Working memory, episodic memory, semantic memory
- **Neuromodulation**: Dopamine, acetylcholine, norepinephrine, serotonin
- **Prediction**: Prediction system, error signals, confidence
- **Cognition**: Neural planner, concept formation, attention
- **Development**: Developmental stages, plasticity modulation

## Technical Details

### Command Parsing

Commands use a flexible syntax:

```bash
command_name [subcommand] [options] [arguments]

Options:
  --key value        # With separate value
  --key=value        # With inline value

Arguments:
  arg1 arg2          # Positional arguments
```

### Error Handling

The CLI provides comprehensive error handling:

- Command validation before execution
- Detailed error messages with suggestions
- Graceful handling of invalid inputs
- Automatic recovery from common errors

### Performance Features

- Command execution timing
- Memory usage monitoring
- Configuration caching
- Efficient argument parsing

## Troubleshooting

### Common Issues

#### 1. Command Not Found
**Problem**: Command doesn't exist or has wrong name
**Solution**:
```bash
./nlm --advanced help
./nlm --advanced commands
```

#### 2. Permission Denied
**Problem**: Cannot access configuration files or write output
**Solution**:
```bash
Check file permissions and ensure write access
```

#### 3. Brain System Not Available
**Problem**: Brain system not initialized
**Solution**:
```bash
Ensure NLM is properly built and try again
```

#### 4. Configuration File Errors
**Problem**: Syntax or format errors in configuration files
**Solution**:
```bash
Check configuration file syntax
```

### Debugging Tips

1. **Verbose Mode**:
   ```bash
   ./nlm --advanced --verbose
   ```

2. **Quiet Mode**:
   ```bash
   ./nlm --advanced --quiet
   ```

3. **Save Output to File**:
   ```bash
   ./nlm --advanced command > output.txt
   ```

4. **Use Logging**:
   ```bash
   # Check NLM logs for detailed information
   ```

## Migration Guide

### From Basic NLM to Advanced CLI

#### What Changed

1. **Command-line interface**: Enhanced with `--advanced` flag
2. **Interactive mode**: Available when `--advanced` is used
3. **Advanced commands**: New commands for experienced users
4. **Configuration management**: Enhanced with validation and templates
5. **Scripting**: Batch processing capabilities

#### What Remained the Same

1. **Configuration files**: Still supported
2. **Brain simulation**: Core functionality unchanged
3. **Legacy demos**: Still available
4. **Basic commands**: Original commands still work

#### Migration Steps

1. **Test your existing scripts**:
   ```bash
   ./nlm --config my_config.cfg
   ```

2. **Try the advanced CLI**:
   ```bash
   ./nlm --advanced
   ```

3. **Explore new features**:
   ```bash
   ./nlm --advanced help
   ./nlm --advanced commands
   ```

4. **Use your existing configuration files**:
   ```bash
   ./nlm --advanced --config my_config.cfg
   ```

## Examples for Different User Levels

### Beginner Users

```bash
# Start with basic functionality
./nlm --advanced help
./nlm --advanced working_memory info
./nlm --advanced neuromodulation get_levels
```

### Intermediate Users

```bash
# Explore advanced features
./nlm --advanced state_analysis full_report
./nlm --advanced simulation_control step 100
./nlm --advanced configuration show
```

### Advanced Users

```bash
# Use all advanced features
./nlm --advanced script execute "state_analysis full_report; neuromodulation set_levels 0.5; performance system_health"
./nlm --advanced configuration load experiment_config.cfg
./nlm --advanced diagnostic full
```

## Best Practices

### For Script Authors

1. **Use descriptive command names**:
   ```bash
   # Better
   working_memory clear
   
   # Less clear
   wm clear
   ```

2. **Include error handling**:
   ```bash
   # Better
   ./nlm --advanced command || echo "Command failed"
   
   # Less safe
   ./nlm --advanced command
   ```

3. **Use configuration files**:
   ```bash
   # Better
   ./nlm --advanced --config experiment.cfg
   
   # Less flexible
   ./nlm --advanced --neuron_count 1000
   ```

### For Interactive Users

1. **Use command history**:
   ```bash
   # Navigate with up/down arrows
   # Type 'clear' to clear screen
   ```

2. **Use help effectively**:
   ```bash
   # Get help for specific commands
   ./nlm --advanced help command_name
   ```

3. **Save important sessions**:
   ```bash
   # Use script to save important command sequences
   ./nlm --advanced script save important_session.nlm
   ```

## Conclusion

The NLM Advanced CLI provides a powerful, flexible, and extensible interface for working with neural simulations. It maintains full backward compatibility while offering advanced features for experienced users. The modular architecture makes it easy to add new commands and extend functionality.

For the latest updates and additional documentation, visit the NLM documentation portal.
