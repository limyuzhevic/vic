# NLM CLI Usage Examples

## Overview

Practical examples for using the NLM command-line interface. These examples demonstrate common usage patterns and advanced command-line techniques.

## Quick Start

### Basic Command Line Usage
```bash
# Show help information
./nlm help

# Run basic neural simulation
./nlm brain simulate

# Load brain from file
./nlm brain load my_brain.nml

# Save brain to file
./nlm brain save current_brain.nml

# Reset brain
./nlm brain reset
```

### Configuration Management
```bash
# List all configuration parameters
./nlm config list

# Get specific parameter value
./nlm config get neuron_count

# Set parameter value
./nlm config set neuron_count 1000

# Validate configuration
./nlm config validate
```

### Debug and Analysis
```bash
# Enable debug logging
./nlm debug log --level=DEBUG

# Show brain information
./nlm brain info

# Run debug validation
./nlm debug validate
```

## Configuration Examples

### 1. Basic Configuration Management

#### List All Configuration Parameters
```bash
# Basic configuration listing
./nlm config list

# Filter for specific parameter types
./nlm config list | grep int64
./nlm config list | grep "string"
```

#### Get and Set Configuration Values
```bash
# Get current configuration values
./nlm config get random_seed
./nlm config get neuron_count
./nlm config get simulation_timestep

# Set configuration values (with validation)
./nlm config set random_seed 42
./nlm config set neuron_count 2000
./nlm config set simulation_timestep 0.0005

# Check configuration sources
./nlm config get neuron_count
# Shows: neuron_count = 2000 (from command line)
```

### 2. Configuration File Management

#### Create and Use Configuration Files
```bash
# Create a basic configuration file
cat > my_config.cfg << 'EOF'
# Basic NLM configuration
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
log_level = INFO

# Advanced settings
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.02
environment_name = GridWorld
EOF

# Use configuration file
./nlm --config my_config.cfg brain simulate

# Override configuration values
./nlm --config my_config.cfg \
    --config.set neuron_count=1500 \
    --config.set plasticity_learning_rate=0.01 \
    brain simulate
```

#### Export and Import Configurations
```bash
# Export current configuration to file
./nlm config export current_config.json

# Import configuration from file
./nlm config set config_file imported_config.json
./nlm brain simulate

# Create example configurations
./nlm config export research_config.json
./nlm config export production_config.json
```

### 3. Brain Operation Examples

#### Basic Brain Operations
```bash
# Reset brain to default state
./nlm brain reset

# Save current brain state
./nlm brain save brain_checkpoint.nml

# Load saved brain state
./nlm brain load brain_checkpoint.nml

# Run simulation with saved brain
./nlm brain simulate --config custom_config.cfg
```

#### Brain Analysis
```bash
# Show detailed brain information
./nlm brain info

# Run analysis with specific parameters
./nlm brain info --verbose

# Show brain status with current configuration
./nconfig get neuron_count
./nlm brain info
```

### 4. Debug and Validation Examples

#### Debug Configuration
```bash
# Validate current configuration
./nlm config validate

# Validate with detailed output
./nlm debug validate --verbose

# Fix configuration issues
./nlm debug validate --fix

# Check configuration file syntax
./nlm config validate --config-file my_config.cfg
```

#### Enable Debug Logging
```bash
# Enable basic debug logging
./nlm debug log --level=DEBUG

# Enable file logging
./nlm debug log --level=DEBUG --file=nlm_debug.log

# Configure logging for specific components
./nlm debug log --level=DEBUG --component=plasticity
```

### 5. Experiment Examples

#### Run Experiments with Different Configurations
```bash
# Run experiment with custom configuration
./nlm experiment run --config configs/experimental.cfg

# Run experiment with output file
./nlm experiment run --config configs/experimental.cfg --output results.json

# Run experiment with parallel processing
./nlm experiment run --config configs/experimental.cfg --parallel 4

# List available experiments
./nlm experiment list
```

#### Batch Processing
```bash
# Run multiple simulations with different configurations
for config in configs/*.cfg; do
    echo "Running simulation with $config"
    ./nlm --config "$config" brain simulate --steps 1000
    ./nlm brain save "results/$(basename $config .cfg)_final.nml"
done

# Run batch experiments
./nlm experiment run \
    --config configs/experimental.cfg \
    --output experiment_results.json \
    --parallel 2 \
    --iterations 10
```

### 6. System Management Examples

#### System Control
```bash
# Check system status
./nlm system status

# Stop system gracefully
./nlm system stop

# Restart system
./nlm system restart

# Show system information
./nlm debug info
```

#### Help System
```bash
# Show general help
./nlm help

# Show brain command help
./nlm brain help

# Show config command help
./nlm config help

# Show debug command help
./nlm debug help

# Show experiment command help
./nlm experiment help

# Show system command help
./nlm system help
```

### 7. Advanced CLI Usage

#### Command Line Options
```bash
# Use multiple options in one command
./nlm --verbose --config my_config.cfg brain simulate

# Use long options with equals sign
./nlm --config=my_config.cfg --verbose brain simulate

# Use short options where available
./nlm -c my_config.cfg -v brain simulate
```

#### Argument Management
```bash
# Commands with positional arguments
./nlm brain load brain_state.nml
./nlm brain save my_brain.nml

# Commands with optional arguments
./nlm brain simulate --steps 1000
./nlm brain simulate --timestep 0.0005

# Commands with flag arguments
./nlm debug log --level=DEBUG
./nlm debug log --file=debug.log
```

#### Complex Command Examples
```bash
# Complex command with multiple options
./nlm --verbose --config configs/experimental.cfg \
    --config.set neuron_count=1500 \
    --config.set plasticity_learning_rate=0.01 \
    brain simulate --steps 5000 --save-interval 1000

# Command with file operations
./nlm brain load initial_brain.nml
./nlm brain simulate --steps 2000
./nlm brain save final_brain.nml
./nlm config export final_config.json
```

## Common Patterns and Workflows

### Pattern 1: Experiment Development Workflow
```bash
# 1. Initialize with default configuration
./nlm brain reset

# 2. Quick test with small simulation
./nlm brain simulate --steps 100

# 3. Save test results
./nlm brain save test_brain.nml

# 4. Create new configuration for research
nano configs/research_config.cfg

# 5. Run experiment with new configuration
./nlm --config configs/research_config.cfg brain simulate --steps 1000

# 6. Compare results
./nlm brain load test_brain.nml
./nlm brain load final_brain.nml
./nlm brain info
```

### Pattern 2: Configuration Testing Workflow
```bash
# 1. Validate configuration before use
./nlm config validate

# 2. Test configuration with small simulation
./nlm --config configs/test_config.cfg brain simulate --steps 50

# 3. Compare results with different configurations
for config in configs/test_*.cfg; do
    echo "Testing $config"
    ./nlm --config "$config" brain simulate --steps 100
    ./nlm brain info | head -20
done
```

### Pattern 3: Production Workflow
```bash
# 1. Validate configuration
./nlm debug validate

# 2. Run production simulation
./nlm brain simulate --steps 10000

# 3. Save checkpoint
./nlm brain save production_checkpoint.nml

# 4. Generate report
./nlm brain info > production_report.txt
./nlm config export production_config.json
```

### Pattern 4: Development Workflow
```bash
# 1. Reset and initialize
./nlm brain reset

# 2. Quick test
./nlm brain simulate --steps 100

# 3. Check current configuration
./nlm config list | grep -E "(neuron|step|log)"

# 4. Modify configuration
./nlm config set neuron_count 500

# 5. Test with new configuration
./nlm brain simulate --steps 500

# 6. Save results
./nlm brain save development_brain.nml
```

## CLI Command Reference

### Config Commands
```bash
# List configuration parameters
./nlm config list

# Get configuration value
./nlm config get <key>

# Set configuration value
./nlm config set <key> <value>

# Validate configuration
./nlm config validate

# Export configuration
./nlm config export <file>
```

### Brain Commands
```bash
# Run brain simulation
./nlm brain simulate
./nlm brain simulate --steps <num>

# Load brain state
./nlm brain load <path>

# Save brain state
./nlm brain save <path>

# Reset brain
./nlm brain reset

# Show brain information
./nlm brain info
```

### Debug Commands
```bash
# Validate configuration
./nlm debug validate
./nlm debug validate --verbose

# Configure logging
./nlm debug log --level=<level>
./nlm debug log --file=<file>

# Show system information
./nlm debug info
```

### Experiment Commands
```bash
# Run experiment
./nlm experiment run --config <config>
./nlm experiment run --config <config> --output <file>

# List experiments
./nlm experiment list
```

### System Commands
```bash
# System control
./nlm system status
./nlm system stop
./nlm system restart
```

### Help Commands
```bash
# Help system
./nlm help
./nlm <command> help
```

## Troubleshooting

### Common CLI Issues

#### "Command not found"
**Solution**: Check command spelling and ensure correct syntax:
```bash
./nlm brain  # Correct: brain is a subcommand
./nlm brain simulate  # Correct: simulate is a brain subcommand
./nlm brain invalid_command  # Wrong: invalid_command doesn't exist
```

#### "Missing required arguments"
**Solution**: Check command documentation and provide required arguments:
```bash
# Wrong (missing required arguments)
./nlm brain load

# Correct (with required arguments)
./nlm brain load brain_state.nml
```

#### "Invalid option"
**Solution**: Use valid CLI options:
```bash
# Wrong
./nlm brain simulate --invalid-option

# Correct
./nlm brain simulate --steps 100
```

#### "Configuration file not found"
**Solution**: Check configuration file location:
```bash
# Wrong (file doesn't exist)
./nlm --config non_existent_config.cfg brain simulate

# Correct (file exists)
./nlm --config my_config.cfg brain simulate
```

#### "Command execution failed"
**Solution**: Check error messages and logs:
```bash
# Check for verbose output
./nlm --verbose brain simulate

# Check system information
./nlm debug info
```

### Debug CLI Usage

#### Enable Verbose Output
```bash
# Enable verbose logging for detailed information
./nlm --verbose brain simulate

# Enable verbose output for specific commands
./nlm --verbose config set neuron_count 1000
```

#### Check Command Syntax
```bash
# Show help for specific commands
./nlm help
./nlm brain help
./nlm config help

# Show usage examples
./nlm brain simulate --help
./nlm config set --help
```

#### Validate Commands
```bash
# Validate configuration before execution
./nlm config validate

# Test command syntax without execution
./nlm --dry-run brain simulate

# Check command availability
./nlm brain list 2>/dev/null || echo "Command brain list failed"
```

## Advanced CLI Techniques

### Command Composition
```bash
# Compose multiple commands
./nlm config set neuron_count 1000
./nlm brain simulate --steps 500
./nlm brain save test_brain.nml

# Use command output as input
brain_state=$(./nlm brain info --json 2>/dev/null | grep "brain_state" || echo "default")
echo "Brain state: $brain_state"
```

### File Operations
```bash
# Combine CLI with file operations
./nlm brain save brain_checkpoint.nml
./nlm config export config_backup.json
cat brain_checkpoint.nml >> brain_archive.nml
```

### Conditional Execution
```bash
# Execute commands conditionally
if [ -f "configs/experimental.cfg" ]; then
    ./nlm --config configs/experimental.cfg brain simulate
else
    echo "Experimental config not found"
fi
```

## CLI Performance Tips

### Optimize Command Execution
```bash
# Use appropriate options for performance
./nlm --verbose brain simulate

# Avoid unnecessary output
./nlm --quiet brain simulate

# Use appropriate timestep for performance
./nlm brain simulate --timestep 0.001
```

### Command Caching
```bash
# Cache configuration validation results
if [ ! -f "validation_cache.txt" ] || [ "$config_file" -nt "validation_cache.txt" ]; then
    ./nlm config validate --cache > validation_cache.txt
fi
```

## Script Examples

### Script 1: Configuration Validator
```bash
#!/bin/bash
# validate_config.sh - Validate configuration files

CONFIG_DIR="configs/"
LOG_FILE="validation.log"

echo "Validating configuration files in $CONFIG_DIR"

for config in $CONFIG_DIR*.cfg; do
    echo "Validating $config"
    ./nlm --config "$config" config validate >> $LOG_FILE 2>&1
    if [ $? -eq 0 ]; then
        echo "✓ $config is valid"
    else
        echo "✗ $config has validation errors"
    fi
done

echo "Validation complete. See $LOG_FILE for details."
```

### Script 2: Batch Processor
```bash
#!/bin/bash
# batch_processor.sh - Process multiple configurations

BATCH_SIZE=100
CONFIG_PATTERN="configs/batch_*.cfg"
OUTPUT_DIR="batch_results"

mkdir -p $OUTPUT_DIR

echo "Processing batch configurations"

for config in $CONFIG_PATTERN; do
    base_name=$(basename "$config" .cfg)
    echo "Processing $base_name"
    
    ./nlm --config "$config" brain simulate --steps $BATCH_SIZE
    ./nlm brain save "$OUTPUT_DIR/${base_name}_final.nml"
    ./nlm config export "$OUTPUT_DIR/${base_name}_config.json"
    
    echo "Completed $base_name"
done

echo "Batch processing complete. Results in $OUTPUT_DIR"
```

### Script 3: Experiment Runner
```bash
#!/bin/bash
# experiment_runner.sh - Run experiments with different parameters

EXPERIMENT_CONFIG="configs/experiment.cfg"
PARAMETER_FILE="experiment_parameters.txt"

readarray -t parameters < $PARAMETER_FILE

echo "Running experiments with ${#parameters[@]} parameter combinations"

for param in "${parameters[@]}"; do
    echo "Running experiment: $param"
    
    # Create experiment-specific configuration
    ./nlm --config "$EXPERIMENT_CONFIG" \
          --config.set "experiment_name=$param" \
          experiment run --output "results/${param}_results.json"
    
    echo "Completed experiment: $param"
done

echo "All experiments completed"
```

## Conclusion

This CLI usage guide provides comprehensive examples for using the NLM command-line interface. Whether you're running basic simulations, managing configurations, debugging issues, or running complex experiments, these examples provide the foundation for effective CLI usage.

For more advanced usage and scripting, refer to the script examples and advanced techniques sections.

---
*CLI usage guide v1.0.0*
