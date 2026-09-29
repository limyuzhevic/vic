# NLM Command-Line Interface Documentation

## Overview

This document describes the command-line interface (CLI) for the NLM (Neural Learning Machine) project. The CLI provides various options for controlling simulation behavior, configuration loading, and experiment execution.

## Command Structure

### Basic Usage
```bash
./nlm [options] [arguments]
```

### Executable Names
NLM provides several executable entry points:

- `nlm`: Main simulation executable (Phase 2/3/4/5/6)
- `nlm_phase3_demo`: Phase 3 demonstration
- `nlm_phase4_demo`: Phase 4 demonstration  
- `nlm_phase5_demo`: Phase 5 demonstration (if available)
- `nlm_phase6_demo`: Phase 6 integration demonstration
- `nlm_test`: Test suite runner

## Common Options

### Configuration Options
```bash
--config <file>           Load configuration from file
-c <file>                 Short form of --config
```

### Simulation Options
```bash
--help                    Show help information and exit
--version                 Show version information and exit
--verbose                 Enable verbose output
--quiet                   Disable most output (errors only)
```

### Experiment Options
```bash
--steps <n>               Number of simulation steps to run
--max_steps <n>           Same as --steps (alternative syntax)
--time <s>                Maximum simulation time in seconds
--duration <s>            Same as --time (alternative syntax)
```

### Debugging Options
```bash
--debug                   Enable debug mode
--trace <level>           Set trace level (0-3)
--log_level <level>       Set logging level (DEBUG, INFO, WARN, ERROR)
```

### Output Options
```bash
--output <file>           Write output to file
--log_file <file>         Write log to file
--checkpoint <file>       Save checkpoint to file
--save_checkpoint         Enable automatic checkpoint saving
```

## Configuration File Options

### Simple Format Options
```bash
--set <key>=<value>        Set configuration key to value
--set <key> <value>        Set configuration key to value (alternative syntax)
--remove <key>            Remove configuration key
--clear                   Clear all configuration
```

### JSON Format Options
```bash
--json <json_string>      Parse JSON configuration string
--json_file <file>        Load configuration from JSON file
```

## Command Line Argument Examples

### Basic Simulation
```bash
# Run with default configuration
./nlm

# Run with custom configuration file
./nlm --config my_config.cfg

# Run with specific number of steps
./nlm --steps 1000

# Run with verbose output and custom config
./nlm --verbose --config research.cfg
```

### Configuration Management
```bash
# Create new configuration with defaults
./nlm --config empty.cfg --clear

# Set specific configuration values
./nlm --set neuron_count=500 --set simulation_timestep=0.001

# Load and modify JSON configuration
./nlm --json_file config.json --set development_plasticity_factor=0.8

# Remove configuration keys
./nlm --config temp.cfg --remove working_memory_capacity_factor
```

### Experiment Control
```bash
# Run specific experiment
./nlm --config experiment.cfg --steps 5000 --verbose

# Run with checkpointing
./nlm --config exp.cfg --steps 10000 --save_checkpoint

# Run debug mode
./nlm --config debug.cfg --debug --log_level DEBUG
```

### Output Control
```bash
# Run and save output
./nlm --steps 2000 --output results.txt --log_file nlm.log

# Run with minimal output
./nlm --steps 1000 --quiet

# Run with detailed logging
./nlm --steps 500 --verbose --log_level INFO
```

## Detailed Option Descriptions

### --config <file> / -c <file>

**Description:** Load configuration from a file.

**Default:** `configs/default.cfg` (if file doesn't exist, defaults are used)

**Supported Formats:**
- Simple key=value format (`.cfg`, `.txt`)
- JSON format (`.json`)

**Usage Examples:**
```bash
./nlm --config my_settings.cfg
./nlm -c experiment.json
```

**Configuration Loading Order:**
1. Default configuration (built-in)
2. Configuration file (if specified)
3. Command line arguments (if any)

### --steps <n> / --max_steps <n>

**Description:** Set the number of simulation steps to run.

**Default:** 10000

**Constraints:**
- Must be a positive integer
- Values less than 1000 may use different default settings
- Large values may affect performance

**Usage Examples:**
```bash
./nlm --steps 1000      # Run 1000 steps
./nlm --max_steps 5000   # Alternative syntax
```

**Related Configuration Key:**
- `max_simulation_steps`

### --time <s> / --duration <s>

**Description:** Set maximum simulation time in seconds.

**Default:** 100.0 (100 seconds)

**Constraints:**
- Must be a positive float
- Time is converted to steps based on `simulation_timestep`

**Usage Examples:**
```bash
./nlm --time 10.0      # Run for 10 seconds
./nlm --duration 30.0   # Alternative syntax
```

**Calculation:**
```
Steps = Time / simulation_timestep
```

### --help

**Description:** Show help information and exit.

**Usage Examples:**
```bash
./nlm --help           # Show all options
./nlm_phase3_demo --help  # Show Phase 3 demo options
```

**Output:** Displays help text with:
- All available options
- Option descriptions
- Usage examples
- Configuration file information

### --version

**Description:** Show version information and exit.

**Usage Examples:**
```bash
./nlm --version
./nlm_phase6_demo --version
```

**Output:** Displays version information including:
- NLM version number
- Build information
- Compilation date

### --verbose / --quiet

**Description:** Control output verbosity.

**Default:** INFO level

**Usage Examples:**
```bash
./nlm --verbose      # Show detailed progress
./nlm --quiet         # Show only errors
```

**Related Configuration Key:**
- `log_level`

### --debug

**Description:** Enable debug mode.

**Effects:**
- Enable debug logging
- Increase trace output
- Show additional internal state information
- Disable some optimizations

**Usage Examples:**
```bash
./nlm --config debug.cfg --debug
./nlm --debug --log_level DEBUG
```

### --log_level <level>

**Description:** Set logging level.

**Valid Levels:**
- `DEBUG`: Detailed debug information
- `INFO`: Normal operation information
- `WARN`: Warning messages
- `ERROR`: Error messages only

**Default:** `INFO`

**Usage Examples:**
```bash
./nlm --log_level DEBUG
./nlm --log_level ERROR
```

**Related Configuration Key:**
- `log_level`

### --output <file>

**Description:** Write output to a file.

**Default:** Console output

**Usage Examples:**
```bash
./nlm --steps 1000 --output results.txt
./nlm --output /tmp/experiment_results.txt
```

### --log_file <file>

**Description:** Write log information to a file.

**Default:** Console logging

**Usage Examples:**
```bash
./nlm --log_file nlm.log
./nlm --log_file /var/log/nlm_simulation.log
```

**Related Configuration Key:**
- `log_filename`
- `log_to_file`

### --checkpoint <file>

**Description:** Specify checkpoint file path.

**Default:** `./checkpoints/checkpoint.bin`

**Usage Examples:**
```bash
./nlm --checkpoint ./my_checkpoints/checkpoint.bin
./nlm --checkpoint /tmp/checkpoint.dat
```

**Related Configuration Keys:**
- `checkpoint_dir`
- `checkpoint_max_saves`

### --save_checkpoint

**Description:** Enable automatic checkpoint saving during simulation.

**Default:** Disabled

**Usage Examples:**
```bash
./nlm --steps 10000 --save_checkpoint
./nlm --config exp.cfg --save_checkpoint --steps 5000
```

**Related Configuration Keys:**
- `enableCheckpointing`
- `checkpoint_save_interval`

### --set <key>=<value> / --set <key> <value>

**Description:** Set configuration key to a value at runtime.

**Syntax:**
- `key=value` (equals sign)
- `key value` (space separated, alternative)

**Value Types:**
- **Strings:** `"value"` or `value`
- **Numbers:** `42`, `3.14`, `-2.5`
- **Booleans:** `true`, `false`
- **Vectors:** `1 2 3` (space-separated)

**Usage Examples:**
```bash
./nlm --set neuron_count=500
./nlm --set simulation_timestep 0.001
./nlm --set log_level=DEBUG
./nlm --set development_plasticity_factor=0.8
```

**Command Line Priority:**
Command line `--set` options override configuration file values.

### --remove <key>

**Description:** Remove a configuration key.

**Usage Examples:**
```bash
./nlm --config temp.cfg --remove neuron_count
./nlm --remove working_memory_capacity_factor
```

### --clear

**Description:** Clear all configuration settings.

**Usage Examples:**
```bash
./nlm --config empty.cfg --clear
./nlm --clear --set neuron_count=100
```

### --json <json_string>

**Description:** Parse JSON configuration string.

**Usage Examples:**
```bash
./nlm --json '{"neuron_count": 500, "simulation_timestep": 0.001}'
./nlm --json "{\"log_level\": \"DEBUG\", \"verbose\": true}"
```

### --json_file <file>

**Description:** Load configuration from a JSON file.

**Usage Examples:**
```bash
./nlm --json_file config.json
./nlm --json_file /path/to/experiment_config.json
```

## Configuration File vs Command Line Priority

### Priority Order (Highest to Lowest):

1. **Command Line `--set` options**: Override everything
2. **Command line `--json`/ `--json_file`**: Override config file but use defaults for unspecified
3. **Configuration file**: Override defaults
4. **Built-in defaults**: Final fallback

### Priority Examples:

**Scenario:** All configuration sources specify different values

1. **Built-in defaults:** `neuron_count = 1000`
2. **Config file:** `neuron_count = 500`
3. **Command line:** `neuron_count = 200`

**Result:** `neuron_count = 200` (command line wins)

## Special Syntax

### Vector Values
```bash
# Vector syntax for numeric arrays
./nlm --set vector_key=1 2 3 4 5

# Vector syntax for string arrays
./nlm --set string_vector=hello world test
```

### Quoted Values
```bash
# Quoted strings with spaces
./nlm --set description="Research experiment with neurons"

# Escaped quotes in strings
./nlm --set path="C:\\config\\file.cfg"
```

### Scientific Notation
```bash
# Scientific notation for floats
./nlm --set learning_rate=1.23e-4
./nlm --set small_value=5.6e-7
```

## Error Handling

### Invalid Options
```
$ ./nlm --invalid_option
sh: Invalid option
./nlm: unrecognized option '--invalid_option'

$ ./nlm --help
Usage: ./nlm [options]
  --config <file>           Load configuration from file
  --steps <n>               Number of simulation steps to run
  --time <s>                Maximum simulation time in seconds
  --help                    Show help information and exit
  --version                 Show version information and exit
  ... (other options)
```

### Invalid Values
```
$ ./nlm --steps -100
./nlm: error: Invalid value for --steps: -100 (must be positive integer)
```

### Missing Files
```
$ ./nlm --config nonexistent.cfg
./nlm: warning: Configuration file 'nonexistent.cfg' not found, using defaults
```

### Configuration Validation
```bash
# Validate configuration before running
./nlm --config test.cfg --validate

# Test configuration loading
./nlm --config test.cfg --test_config
```

## Environment Variables

NLM supports the following environment variables:

### NLM_CONFIG_PATH
```bash
export NLM_CONFIG_PATH=/path/to/configs
./nlm --config default.cfg  # Uses /path/to/configs/default.cfg
```

### NLM_LOG_LEVEL
```bash
export NLM_LOG_LEVEL=DEBUG
./nlm --verbose  # Debug level from environment
```

### NLM_CHECKPOINT_DIR
```bash
export NLM_CHECKPOINT_DIR=/tmp/checkpoints
./nlm --save_checkpoint  # Uses /tmp/checkpoints
```

## Completion Scripts

### Bash Completion
```bash
# Add to ~/.bashrc
source <(nlm --bash-completion)

# Use completion
./nlm --config <TAB>
```

### Zsh Completion
```bash
# Add to ~/.zshrc
source <(nlm --zsh-completion)

# Use completion
./nlm --config <TAB>
```

## Examples by Use Case

### 1. Basic Testing
```bash
# Quick test of basic functionality
./nlm --steps 100 --quiet
```

### 2. Research Experiment
```bash
# Configure and run a research experiment
./nlm --config research.json --steps 50000 --verbose --save_checkpoint
```

### 3. Performance Analysis
```bash
# Run with minimal output for performance analysis
./nlm --config perf.cfg --quiet --steps 20000 --output perf_results.txt
```

### 4. Debugging
```bash
# Debug a specific issue
./nlm --config debug.cfg --debug --log_level DEBUG --verbose
```

### 5. Configuration Development
```bash
# Develop and test configuration
./nlm --config new_config.cfg --test_config --validate

# Test configuration before running experiment
./nlm --config new_config.cfg --steps 100 --verbose
```

## Automation Examples

### Shell Script
```bash
#!/bin/bash

# Run multiple experiments with different configurations
for seed in 42 123 456; do
    ./nlm --config template.cfg --set random_seed=$seed --steps 1000
    ./nlm --config template.cfg --set random_seed=$seed --steps 1000 --save_checkpoint
    mv checkpoint.bin "checkpoint_seed_${seed}.bin"
done
```

### Python Wrapper
```python
import subprocess
import json
import sys

def run_experiment(config_file, steps, options=None):
    cmd = ['./nlm', '--config', config_file, '--steps', str(steps)]
    if options:
        for key, value in options.items():
            cmd.extend(['--set', f'{key}={value}'])
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    return result

# Run experiment
config = {'neuron_count': 500, 'simulation_timestep': 0.001}
result = run_experiment('experiment.json', 10000, config)
print(result.stdout)
if result.returncode != 0:
    print(result.stderr)
    sys.exit(1)
```

### Makefile
```makefile
.PHONY: run-experiment run-benchmark clean

EXPERIMENT_CONFIG = configs/experiment.cfg
EXPERIMENT_STEPS = 10000
BENCHMARK_CONFIG = configs/benchmark.cfg
BENCHMARK_STEPS = 50000

run-experiment:
	./nlm --config $(EXPERIMENT_CONFIG) --steps $(EXPERIMENT_STEPS) --verbose

run-benchmark:
	./nlm --config $(BENCHMARK_CONFIG) --steps $(BENCHMARK_STEPS) --quiet

clean:
	rm -f results.txt
	rm -rf checkpoints
	rm -f *.log
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: Configuration file not found
**Solution:**
```bash
# Check if file exists
ls -la configs/my_config.cfg

# Use absolute path
./nlm --config /path/to/configs/my_config.cfg

# Create directory if needed
mkdir -p configs
```

#### Issue: Invalid configuration values
**Solution:**
```bash
# Check configuration
./nlm --config my_config.cfg --test_config

# Use defaults for invalid values
./nlm --config my_config.cfg --set invalid_key=100
```

#### Issue: Command line argument parsing errors
**Solution:**
```bash
# Use help to see all options
./nlm --help

# Check for typos
./nlm --config my_config.cfg --set neuron_count=500
```

#### Issue: Permission errors
**Solution:**
```bash
# Check file permissions
chmod 644 configs/my_config.cfg

# Ensure executable has permissions
chmod +x ./nlm
```

## Advanced Usage

### Configuration Template System
```bash
# Create template configuration
template.cfg:
  random_seed = 42
  simulation_timestep = 0.001
  neuron_count = 1000

# Generate experiment configurations
for seed in 42 123 456; do
    cp template.cfg experiment_$seed.cfg
    ./nlm --config experiment_$seed.cfg --set random_seed=$seed
    ./nlm --config experiment_$seed.cfg --set neuron_count=$((1000 + seed % 1000))
done
```

### Multi-Environment Configuration
```bash
# Environment variables for different environments
export NLM_ENV=production
export NLM_LOG_LEVEL=INFO
export NLM_CHECKPOINT_DIR=/var/checkpoints

# Run with environment-specific configuration
./nlm --config configs/${NLM_ENV}.cfg
```

### Configuration Comparison
```bash
# Compare configurations
./nlm --config config1.cfg --config config2.cfg --compare

# Find differences
./nlm --config config1.cfg --config config2.cfg --diff
```

## Version Notes

### Version 0.1.0 (Current)
- Basic CLI argument parsing
- Simple configuration file loading
- Limited option set

### Version 1.0.0 (Future)
- Advanced argument parsing
- Configuration schema validation
- Autocomplete support
- Configuration history

### Version 2.0.0 (Future)
- Natural language arguments
- Configuration suggestions
- Machine learning based configuration
- Interactive configuration

## References

1. NLM Documentation: https://github.com/nlm/docs
2. Configuration System Design: docs/CONFIGURATION.md
3. Advanced CLI Usage: docs/ADVANCED_CLI.md
4. Script Integration: docs/SCRIPTING.md

---
*Last updated: September 2026*
*NLM Command-Line Interface Documentation*
*Phase 6: Final Integration*
