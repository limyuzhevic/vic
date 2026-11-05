# NLM Command Line Interface Reference

## Overview

NLM command-line interface provides control over brain operations, configuration management, and system debugging. The CLI follows a hierarchical command structure with multiple categories for different operations.

## Command Categories

### Config Commands

Configure NLM parameters and settings:

#### `nlm config list`
List all configuration parameters:
```bash
nlm config list
```

**Output**:
Shows all available configuration keys with their current values and sources.

#### `nlm config get <key>`
Get configuration value:
```bash
nlm config get random_seed
nlm config get neuron_count
nlm config get plasticity_learning_rate
```

**Output**:
Shows the value of the specified configuration parameter.

#### `nlm config set <key> <value>`
Set configuration value:
```bash
nlm config set random_seed 42
nlm config set neuron_count 1000
nlm config set plasticity_learning_rate 0.02
```

**Output**:
Confirms the configuration value has been set.

#### `nlm config validate`
Validate current configuration:
```bash
nlm config validate
```

**Output**:
Shows validation errors if any, or confirms configuration is valid.

#### `nlm config export <file>`
Export configuration to file:
```bash
nlm config export my_config.cfg
```

**Output**:
Saves current configuration to specified file.

### Brain Commands

Brain operation and management commands:

#### `nlm brain load <path>`
Load brain state from file:
```bash
nlm brain load brain_state.nml
nlm brain load my_brain_checkpoint.nml
```

**Output**:
Shows loading progress and confirms successful load.

#### `nlm brain save <path>`
Save brain state to file:
```bash
nlm brain save brain_state.nml
nlm brain save current_brain.nml
```

**Output**:
Shows save progress and confirms successful save.

#### `nlm brain reset`
Reset brain to initial state:
```bash
nlm brain reset
```

**Output**:
Resets brain and initializes with current configuration.

#### `nlm brain info`
Show brain status information:
```bash
nlm brain info
```

**Output**:
Displays brain statistics including neuron count, synapse count, firing rates, etc.

#### `nlm brain simulate`
Run brain simulation:
```bash
nlm brain simulate
nlm brain simulate --steps 1000
nlm brain simulate --config experimental.cfg
```

**Output**:
Runs the brain simulation with specified parameters and shows progress.

### Debug Commands

Debug and analysis tools:

#### `nlm debug validate`
Validate current configuration:
```bash
nlm debug validate
```

**Output**:
Shows configuration validation errors if any.

#### `nlm debug log`
Enable detailed logging:
```bash
nlm debug log --level=DEBUG
nlm debug log --file=nlm_debug.log
```

**Output**:
Configures logging level and file output.

#### `nlm debug info`
Show system information:
```bash cautionedbash
nlm debug info
```

**Output**:
Displays system and brain information for debugging.

### Experiment Commands

Run experimental scenarios:

#### `nlm experiment run <config>`
Run complete experiment:
```bash
nlm experiment run --config phase6_integrated.cfg
nlm experiment run --config experimental.cfg --output results.json
```

**Output**:
Runs experiment with specified configuration and saves results.

#### `nlm experiment list`
List available experiments:
```bash
nlm experiment list
```

**Output**:
Shows all available experiment configurations.

### System Commands

System control and maintenance:

#### `nlm system status`
Show system status:
```bash
nlm system status
```

**Output**:
Displays system resource usage and brain status.

#### `nlm system stop`
Stop system:
```bash
nlm system stop
```

**Output**:
Gracefully stops the running system.

#### `nlm system restart`
Restart system:
```bash
nlm system restart
```

**Output**:
Restarts the system with current configuration.

### Help Commands

Help and documentation:

#### `nlm help`
Show general help:
```bash
nlm help
```

**Output**:
Displays all available commands and basic usage.

#### `nlm <command> help`
Show specific command help:
```bash
nlm brain help
nlm config help
nlm debug help
```

**Output**:
Displays detailed help for the specified command.

## Command Line Options

### Global Options

#### `--verbose, -v`
Enable verbose output:
```bash
nlm --verbose brain simulate
```

#### `--help, -h`
Show help message:
```bash
nlm help
nlm brain help
```

#### `--version`
Show version information:
```bash
nlm --version
```

#### `--config <file>, -c <file>`
Load configuration from file:
```bash
nlm --config my_config.cfg brain simulate
nlm -c experimental.cfg brain simulate
```

#### `--output <file>, -o <file>`
Output file for results:
```bash
nlm experiment run --config cfg --output results.json
```

## Configuration File Options

#### `--config.set <key> <value>`
Set configuration value via command line:
```bash
nlm --config.set neuron_count 2000 --config.set plasticity_learning_rate 0.02 brain simulate
```

#### `--config-file <file>`
Alternative configuration file option:
```bash
nlm --config-file configs/experimental.cfg brain simulate
```

## Command-Specific Options

### Brain Commands Options

#### `nlm brain simulate` options:

##### `--steps <num>`
Number of simulation steps:
```bash
nlm brain simulate --steps 5000
```

##### `--timestep <float>`
Simulation timestep:
```bash
nlm brain simulate --timestep 0.0005
```

##### `--save-interval <num>`
Checkpoint save interval:
```bash
nlm brain simulate --save-interval 1000
```

### Experiment Commands Options

#### `nlm experiment run` options:

##### `--output <file>`
Output results file:
```bash
nlm experiment run --config cfg --output results.json
```

##### `--parallel <num>`
Number of parallel experiments:
```bash
nlm experiment run --config cfg --parallel 4
```

##### `--iterations <num>`
Number of iterations per experiment:
```bash
nlm experiment run --config cfg --iterations 10
```

## Examples

### Basic Usage

#### Basic Brain Simulation
```bash
# Run simple brain simulation
./nlm brain simulate

# Use custom configuration
./nlm --config experimental.cfg brain simulate

# Save current brain state
./nlm brain save current_brain.nml

# Load and continue from saved state
./nlm brain load saved_brain.nml
```

#### Configuration Management
```bash
# List all configuration parameters
./nlm config list

# Get specific parameter value
./nlm config get neuron_count

# Set parameter value
./nlm config set neuron_count 1000

# Validate configuration
./nlm config validate

# Export configuration to file
./nlm config export my_config.cfg
```

#### Brain Operations
```bash
# Reset brain to initial state
./nlm brain reset

# Load brain from file
./nlm brain load brain_state.nml

# Save brain to file
./nlm brain save brain_state.nml

# Get brain information
./nlm brain info
```

#### Debug and Analysis
```bash
# Validate configuration
./nlm debug validate

# Enable detailed logging
./nlm debug log --level=DEBUG

# Show system information
./nlm debug info
```

### Advanced Usage

#### Complete Experiment
```bash
# Run complete experiment with multiple phases
./nlm experiment run \
  --config phase6_integrated.cfg \
  --output results.json \
  --verbose \
  --parallel 2
```

#### Configuration Override
```bash
# Command that uses config file
./nlm --config configs/experimental.cfg brain simulate

# Override config values
./nlm --config configs/experimental.cfg \
    --config.set neuron_count=2000 \
    --config.set plasticity_learning_rate=0.02 \
    brain simulate
```

#### Batch Processing
```bash
# Run multiple simulations with different configurations
for config in configs/*.cfg; do
    echo "Running simulation with $config"
    ./nlm --config "$config" brain simulate --steps 1000
    ./nlm brain save "results/$(basename $config .cfg)_final.nml"
done
```

#### Development Workflow
```bash
# Development workflow
./nlm brain reset                    # Reset brain
cat configs/experimental.cfg | head -20  # Check initial config
./nlm brain simulate --steps 100      # Quick test
./nlm brain save test_brain.nml       # Save test state
cat configs/test_config.cfg >> configs/experimental.cfg  # Test new config
./nlm brain load test_brain.nml       # Continue from test
```

## Common Patterns and Workflows

### 1. Experimentation Workflow
```bash
# 1. Reset and initialize
./nlm brain reset

# 2. Run quick test
./nlm brain simulate --steps 100

# 3. Save test results
./nlm brain save test_brain.nml

# 4. Create new configuration
nano configs/test_config.cfg

# 5. Run experiment with new config
./nlm --config configs/test_config.cfg brain simulate --steps 1000

# 6. Compare results
./nlm brain load test_brain.nml
./nlm brain load final_brain.nml
./nlm debug info
```

### 2. Research Workflow
```bash
# Start with Phase 2 demo
./nlm

# Run Phase 3 world interaction
./nlm_phase3_demo

# Run Phase 6 integration test
./nlm_phase6_demo

# Run custom research experiment
./nlm experiment run --config research_config.cfg
```

### 3. Production Workflow
```bash
# 1. Validate configuration
./nlm debug validate

# 2. Run simulation
./nlm brain simulate --steps 10000

# 3. Save checkpoint
./nlm brain save final_results.nml

# 4. Generate report
./nlm brain info > simulation_report.txt
```

## Error Messages and Solutions

### Common Errors

#### "Command not found"
**Solution**: Check command spelling and ensure you're using correct command syntax.

#### "Invalid configuration parameter"
**Solution**: Use `nlm config list` to see all available parameters, or check documentation.

#### "Cannot open configuration file"
**Solution**: Verify file exists and has correct permissions.

#### "Brain initialization failed"
**Solution**: Check configuration and ensure all required parameters are set.

#### "Simulation step failed"
**Solution**: Check log output for specific error details, or try smaller simulation.

## Command Help Examples

### Get Help for Specific Commands
```bash
# General help
./nlm help

# Brain command help
./nlm brain help

# Config command help
./nlm config help

# Debug command help
./nlm debug help
```

### Get Full Command Syntax
```bash
# Show complete command syntax
./nlm brain simulate --help
./nlm config set --help
./nlm debug validate --help
```

## Command Documentation Index

### By Category
- [Config Commands](#config-commands)
- [Brain Commands](#brain-commands)
- [Debug Commands](#debug-commands)
- [Experiment Commands](#experiment-commands)
- [System Commands](#system-commands)
- [Help Commands](#help-commands)

### By Functionality
- [Configuration Management](#configuration-management)
- [Brain Operations](#brain-operations)
- [Debug and Analysis](#debug-and-analysis)
- [Experiment Running](#experiment-running)
- [System Control](#system-control)

### Common Usage Patterns
- [Basic Brain Simulation](#basic-brain-simulation)
- [Configuration Testing](#configuration-testing)
- [Experiment Runs](#experiment-runs)
- [Development Workflow](#development-workflow)

## Command Reference Summary

### Quick Reference
```bash
# Configuration
./nlm config list
./nlm config get <key>
./nlm config set <key> <value>
./nlm config validate

# Brain Operations
./nlm brain simulate
./nlm brain load <path>
./nlm brain save <path>
./nlm brain reset
./nlm brain info

# Debug
./nlm debug validate
./nlm debug log
./nlm debug info

# Global Options
--verbose, -v
--help, -h
--version
--config <file>, -c <file>
```

---
*CLI reference generated from source code analysis*
