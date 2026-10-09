# NLM Command-Line Interface Reference

## Overview

This document provides comprehensive reference documentation for the enhanced NLM Command-Line Interface (CLI). The CLI provides a powerful, user-friendly interface for controlling NLM brains, running experiments, and managing configurations.

## CLI Structure

### Main Command Syntax

```bash
nlm [OPTIONS] <COMMAND> [ARGS] [COMMAND_OPTIONS]
```

### Global Options

| Option | Short | Description | Example |
|--------|--------|-------------|---------|
| `--help` | `-h` | Show help information | `nlm help` |
| `--version` | `-v` | Show version information | `nlm version` |
| `--config` | -c | Configuration file path | `nlm --config myconfig.cfg brain create brain1` |

### Command Categories

The CLI is organized into the following command categories:

1. **Brain Commands** - Brain creation, initialization, and control
2. **Agent Commands** - Agent control and configuration
3. **Config Commands** - Configuration management
4. **Experiment Commands** - Experiment running and tracking
5. **Monitor Commands** - Real-time monitoring
6. **System Commands** - System operations and maintenance

---

## Brain Commands

### brain create

**Create a new brain instance**

#### Syntax
```bash
nlm brain create <name> [--neurons N] [--regions R] [--config FILE] [key=value...]
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `<name>` | string | Yes | - | Name of the brain to create |
| `--neurons N` | integer | No | 1000 | Number of neurons in the brain |
| `--regions R` | integer | No | 2 | Number of brain regions |
| `--config FILE` | string | No | `configs/default.cfg` | Configuration file path |
| `key=value...` | string | No | - | Configuration key-value pairs |

#### Examples

```bash
# Create brain with default settings
nlm brain create mybrain

# Create brain with custom neuron count
nlm brain create research_brain --neurons 2000 --regions 3

# Create brain from custom config file
nlm brain create simulation_brain --config configs/simulation.cfg

# Create brain with custom configuration
nlm brain create experiment_brain --neurons 500 --regions 1 --plasticity_learning_rate=0.02
```

#### Command Description

Creates a new NLM brain instance with specified configuration parameters. The brain is initialized with the specified number of neurons, regions, and configuration values. The brain can be configured with various parameters to control its behavior, learning capabilities, and simulation characteristics.

#### Return Values

- Returns `0` on success
- Returns non-zero on failure (e.g., invalid parameters, configuration errors)

#### Common Errors

- **Invalid neuron count**: Must be positive integer
- **Invalid region count**: Must be positive integer
- **File not found**: Configuration file does not exist
- **Invalid configuration**: Invalid key-value pairs

---

### brain initialize

**Initialize an existing brain**

#### Syntax
```bash
nlm brain initialize <name> [--current] [--reset]
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `<name>` | string | Yes | - | Name of the brain to initialize |
| `--current` | boolean | No | false | Use current configuration |
| `--reset` | boolean | No | false | Reset brain to initial state |

#### Examples

```bash
# Initialize brain with default settings
nlm brain initialize mybrain

# Initialize brain with reset
nlm brain initialize mybrain --reset

# Initialize brain with current configuration
nlm brain initialize mybrain --current
```

#### Command Description

Initializes the specified brain with the given configuration. If the `--reset` flag is used, the brain is reset to its initial state before initialization. The brain prepares itself for simulation with the specified configuration parameters.

#### Return Values

- Returns `0` on success
- Returns non-zero on failure (e.g., brain not found, initialization failed)

#### Common Errors

- **Brain not found**: Brain with specified name does not exist
- **Initialization failed**: Brain initialization failed due to configuration errors
- **Invalid flag**: Invalid combination of flags

---

### brain step

**Execute simulation steps**

#### Syntax
```bash
nlm brain step <name> <steps> [--verbose] [--stats] [--neuron's]
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `<name>` | string | Yes | - | Name of the brain to step |
| `<steps>` | integer | Yes | - | Number of simulation steps to execute |
| `--verbose` | boolean | No | false | Enable verbose output |
| `--stats` | boolean | No | false | Show step statistics |
| `--neurons` | boolean | No | false | Show neuron information |

#### Examples

```bash
# Run 10 simulation steps
nlm brain step mybrain 10

# Run 100 steps with verbose output
nlm brain step mybrain 100 --verbose

# Run 50 steps with statistics
nlm brain step mybrain 50 --stats

# Run 20 steps with neuron information
nlm brain step mybrain 20 --neurons
```

#### Command Description

Executes the specified number of simulation steps for the brain. Each step advances the brain's neural simulation by one time step, updating neuron states, processing connections, and applying learning rules. The `--verbose` flag provides detailed output for each step, while `--stats` shows summary statistics.

#### Return Values

- Returns `0` on success
- Returns non-zero on failure (e.g., brain not found, invalid steps)

#### Common Errors

- **Brain not found**: Brain with specified name does not exist
- **Invalid steps**: Steps must be positive integer
- **Simulation error**: Error during simulation execution

---

### brain save

**Save brain state to file**

#### Syntax
```bash
nlm brain save <name> <file> [--overwrite] [--format FORMAT]
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `<name>` | string | Yes | - | Name of the brain to save |
| `<file>` | string | Yes | - | File path to save brain state |
| `--overwrite` | boolean | No | false | Overwrite existing file |
| `--format FORMAT` | string | No | `json` | Output format (`json`, `binary`, `txt`) |

#### Examples

```bash
# Save brain state to JSON file
nlm brain save mybrain checkpoint.brain.json

# Save brain state with overwrite
nlm brain save mybrain checkpoint.brain.json --overwrite

# Save brain state in binary format
nlm brain save mybrain checkpoint.brain.bin --format binary

# Save brain state in text format
nlm brain save mybrain checkpoint.brain.txt --format txt
```

#### Command Description

Saves the current state of the brain to a file in the specified format. The saved state includes brain configuration, neuron states, synapse weights, and other internal state information. The brain can be restored later using the `brain load` command.

#### Return Values

- Returns `0` on success
- Returns non-zero on failure (e.g., brain not found, file error)

#### Common Errors

- **Brain not found**: Brain with specified name does not exist
- **File error**: Cannot write to specified file
- **Format error**: Invalid output format

---

### brain load

**Load brain state from file**

#### Syntax
```bash
nlm brain load <name> <file> [--merge] [--validate]
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `<name>` | string | Yes | - | Name of the brain to load |
| `<file>` | string | Yes | - | File path to load brain state from |
| `--merge` | boolean | No | false | Merge with existing brain |
| `--validate` | boolean | No | false | Validate loaded configuration |

#### Examples

```bash
# Load brain state from JSON file
nlm brain load mybrain checkpoint.brain.json

# Load and merge with existing brain
nlm brain load mybrain checkpoint.brain.json --merge

# Load and validate configuration
nlm brain load mybrain checkpoint.brain.json --validate
```

#### Command Description

Loads brain state from a file and initializes the brain with the saved state. The `--merge` flag merges the loaded state with the existing brain state, while the `--validate` flag validates the loaded configuration before applying it.

#### Return Values

- Returns `0` on success
- Returns non-zero on failure (e.g., file not found, invalid state)

#### Common Errors

- **File not found**: Specified file does not exist
- **Invalid state**: Loaded state is invalid or corrupted
- **Validation error**: Configuration validation failed

---

### brain status

**Show brain status information**

#### Syntax
```bash
nlm brain status <name> [--full] [--stats] [--connections] [--neurons] [--development]
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `<name>` | string | Yes | - | Name of the brain to check |
| `--full` | boolean | No | false | Show full status information |
| `--stats` | boolean | No | false | Show statistics |
| `--connections` | boolean | No | false | Show connection information |
| `--neurons` | boolean | No | false | Show neuron information |
| `--development` | boolean | No | false | Show development information |

#### Examples

```bash
# Show basic brain status
nlm brain status mybrain

# Show full status information
nlm brain status mybrain --full

# Show brain statistics
nlm brain status mybrain --stats

# Show connection information
nlm brain status mybrain --connections

# Show all information
nlm brain status mybrain --full --stats --connections --neurons --development
```

#### Command Description

Displays comprehensive status information about the specified brain. The `--full` flag shows complete status information, while other flags show specific categories of information (statistics, connections, neurons, development).

#### Return Values

- Returns `0` on success
- Returns non-zero on failure (e.g., brain not found)

#### Common Errors

- **Brain not found**: Brain with specified name does not exist
- **Permission error**: Cannot access brain information

---

### brain reset

**Reset brain to initial state**

#### Syntax
```bash
nlm brain reset <name>
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `<name>` | string | Yes | - | Name of the brain to reset |

#### Examples

```bash
# Reset brain to initial state
nlm brain reset mybrain
```

#### Command Description

Resets the specified brain to its initial state. All neuron states, synapse weights, and other internal state information are reset to their initial values. The brain is then reinitialized with its original configuration.

#### Return Values

- Returns `0` on success
- Returns non-zero on failure (e.g., brain not found)

#### Common Errors

- **Brain not found**: Brain with specified name does not exist
- **Reset error**: Error during brain reset

---

## Agent Commands

### agent enable

**Enable agent functionality**

#### Syntax
```bash
nlm agent enable [--curiosity] [--reward-modulation] [--neuromodulation] [--structural-plasticity] [--development]
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `--curiosity` | boolean | No | false | Enable curiosity-driven behavior |
| `--reward-modulation` | boolean | No | false | Enable reward modulation |
| `--neuromodulation` | boolean | No | false | Enable neuromodulation |
| `--structural-plasticity` | boolean | No | false | Enable structural plasticity |
| `--development` | boolean | No | false | Enable development |

#### Examples

```bash
# Enable all agent features
nlm agent enable --curiosity --reward-modulation --neuromodulation --structural-plasticity --development

# Enable only curiosity and reward modulation
nlm agent enable --curiosity --reward-modulation

# Enable neuromodulation only
nlm agent enable --neuromodulation
```

#### Command Description

Enables various agent functionality features. Curiosity drives exploration behavior, reward modulation enables learning from rewards, neuromodulation affects neural processing, structural plasticity allows connection growth, and development enables brain maturation.

#### Return Values

- Returns `0` on success
- Returns non-zero on failure (e.g., agent not found)

#### Common Errors

- **Agent not found**: Agent with specified name does not exist
- **Invalid flag**: Invalid flag combination

---

### agent disable

**Disable agent functionality**

#### Syntax
```bash
nlm agent disable [--all] [--curiosity] [--reward-modulation] [--neuromodulation] [--structural-plasticity] [--development]
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `--all` | boolean | No | false | Disable all agent features |
| `--curiosity` | boolean | No | false | Disable curiosity-driven behavior |
| `--reward-modulation` | boolean | No | false | Disable reward modulation |
| `--neuromodulation` | boolean | No | false | Disable neuromodulation |
| `--structural-plasticity` | boolean | No | false | Disable structural plasticity |
| `--development` | boolean | No | false | Disable development |

#### Examples

```bash
# Disable all agent features
nlm agent disable --all

# Disable only curiosity and reward modulation
nlm agent disable --curiosity --reward-modulation

# Disable neuromodulation only
nlm agent disable --neuromodulation
```

#### Command Description

Disables various agent functionality features. This command provides granular control over which agent features are active, allowing for fine-tuned control of agent behavior.

#### Return Values

- Returns `0` on success
- Returns non-zero on failure (e.g., agent not found)

#### Common Errors

- **Agent not found**: Agent with specified name does not exist
- **Invalid flag**: Invalid flag combination

---

### agent status

**Show agent status information**

#### Syntax
```bash
nlm agent status [--agent-name NAME]
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `--agent-name NAME` | string | No | current | Name of agent to check |

#### Examples

```bash
# Show current agent status
nlm agent status

# Show specific agent status
nlm agent status --agent-name myagent
```

#### Command Description

Displays status information about the agent. This includes current functionality status, enabled features, and performance metrics. The `--agent-name` flag allows checking status for specific agents.

#### Return Values

- Returns `0` on success
- Returns non-zero on failure (e.g., agent not found)

#### Common Errors

- **Agent not found**: Agent with specified name does not exist
- **Permission error**: Cannot access agent information

---

## Config Commands

### config show

**Show configuration values**

#### Syntax
```bash
nlm config show [--global] [--brain NAME] [--all] [--key KEY]
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `--global` | boolean | No | false | Show global configuration |
| `--brain NAME` | string | No | default | Name of brain for brain-specific config |
| `--all` | boolean | No | false | Show all configuration keys |
| `--key KEY` | string | No | - | Show specific configuration key |

#### Examples

```bash
# Show global configuration
nlm config show --global

# Show brain-specific configuration
nlm config show --brain mybrain

# Show all configuration keys
nlm config show --all

# Show specific configuration key
nlm config show --brain mybrain --key neuron_count
```

#### Command Description

Displays configuration values. The `--global` flag shows global configuration, while `--brain NAME` shows configuration for a specific brain. The `--all` flag shows all configuration keys, and `--key KEY` shows a specific configuration key.

#### Return Values

- Returns `0` on success
- Returns non-zero on failure (e.g., configuration not found)

#### Common Errors

- **Configuration not found**: Configuration with specified name does not exist
- **Invalid parameter**: Invalid flag combination

---

### config set

**Set configuration values**

#### Syntax
```bash
nlm config set [--global] [--brain NAME] <key> <value>
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `--global` | boolean | No | false | Set global configuration |
| `--brain NAME` | string | No | default | Name of brain for brain-specific config |
| `<key>` | string | Yes | - | Configuration key |
| `<value>` | string | Yes | - | Configuration value |

#### Examples

```bash
# Set global configuration
nlm config set --global random_seed 42

# Set brain-specific configuration
nlm config set --brain mybrain neuron_count 1000

# Set string value
nlm config set --brain mybrain brain_name "research_brain"

# Set numeric value
nlm config set --brain mybrain neuron_count 2000
```

#### Command Description

Sets configuration values. The `--global` flag sets global configuration, while `--brain NAME` sets configuration for a specific brain. The key and value parameters specify the configuration key and its value.

#### Return Values

- Returns `0` on success

#### Common Errors

- **Configuration not found**: Configuration with specified name does not exist
- **Invalid parameter**: Invalid parameter combination

---

### config validate

**Validate configuration**

#### Syntax
```bash
nlm config validate [--global] [--brain NAME] [--fix-duplicates] [--show-errors]
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `--global` | boolean | No | false | Validate global configuration |
| `--brain NAME` | string | No | default | Name of brain for brain-specific config |
| `--fix-duplicates` | boolean | No | false | Automatically fix duplicate entries |
| `--show-errors` | boolean | No | false | Show validation errors |

#### Examples

```bash
# Validate global configuration
nlm config validate --global

# Validate brain-specific configuration
nlm config validate --brain mybrain

# Validate and fix duplicates
nlm config validate --brain mybrain --fix-duplicates

# Validate and show errors
nlm config validate --brain mybrain --show-errors
```

#### Command Description

Validates configuration values and checks for errors. The `--fix-duplicates` flag automatically fixes duplicate configuration entries, while `--show-errors` displays validation errors. This command ensures that configuration files are valid and can be safely used.

#### Return Values

- Returns `0` on success

#### Common Errors

- **Configuration not found**: Configuration with specified name does not exist
- **Validation error**: Configuration validation failed

---

## Experiment Commands

### experiment run

**Run experiment**

#### Syntax
```bash
nlm experiment run [--brain NAME] <phase> <steps> [--continue] [--config CONFIG_FILE]
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `--brain NAME` | string | No | default | Name of brain to use |
| `<phase>` | string | Yes | - | Experiment phase |
| `<steps>` | integer | Yes | - | Number of steps to run |
| `--continue` | boolean | No | false | Continue from previous run |
| `--config CONFIG_FILE` | string | No | default | Configuration file path |

#### Examples

```bash
# Run experiment with default settings
nlm experiment run phase2 1000

# Run experiment with specific brain
nlm experiment run --brain mybrain phase2 1000

# Continue from previous experiment
nlm experiment run --continue phase2 500

# Run experiment with custom config
nlm experiment run phase2 1000 --config configs/experiment.cfg
```

#### Command Description

Runs experiment with the specified phase and number of steps. The `--brain NAME` flag specifies which brain to use, `--continue` allows continuing from a previous experiment run, and `--config CONFIG_FILE` specifies a custom configuration file.

#### Return Values

- Returns `0` on success
- Returns non-zero on failure (e.g., experiment not found, invalid parameters)

#### Common Errors

- **Experiment not found**: Experiment with specified phase does not exist
- **Invalid parameters**: Invalid parameter combination
- **Brain not found**: Brain with specified name does not exist

---

### experiment status

**Show experiment status**

#### Syntax
```bash
nlm experiment status [--brain NAME] [--all] [--running]
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `--brain NAME` | string | No | default | Name of brain to check |
| `--all` | boolean | No | false | Show all experiment information |
| `--running` | boolean | No | false | Show only running experiments |

#### Examples

```bash
# Show current experiment status
nlm experiment status

# Show experiments for specific brain
nlm experiment status --brain mybrain

# Show all experiment information
nlm experiment status --all

# Show only running experiments
nlm experiment status --running
```

#### Command Description

Displays experiment status information. The `--brain NAME` flag shows experiment status for a specific brain. The `--all` flag shows complete experiment information, while `--running` shows only currently running experiments.

#### Return Values

- Returns `0` on success
- Returns non-zero on failure (e.g., experiment not found)

#### Common Errors

- **Experiment not found**: No experiments found
- **Brain not found**: Brain with specified name does not exist

---

## Monitor Commands

### monitor brain

**Monitor brain activity**

#### Syntax
```bash
nlm monitor brain <name> [--stats] [--neuromodulation] [--spikes] [--activity] [--heatmap] [--performance]
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `<name>` | string | Yes | - | Name of brain to monitor |
| `--stats` | boolean | No | false | Show statistics |
| `--neuromodulation` | boolean | No | false | Show neuromodulation |
| `--spikes` | boolean | No | false | Show spike activity |
| `--activity` | boolean | No | false | Show general activity |
| `--heatmap` | boolean | No | false | Show activity heatmap |
| `--performance` | boolean | No | false | Show performance metrics |

#### Examples

```bash
# Monitor brain with statistics
nlm monitor brain mybrain --stats

# Monitor brain with neuromodulation
nlm monitor brain mybrain --neuromodulation

# Monitor brain with spike activity
nlm monitor brain mybrain --spikes

# Monitor brain with all metrics
nlm monitor brain mybrain --stats --neuromodulation --spikes --activity --heatmap --performance
```

#### Command Description

Monitors brain activity in real-time. The `--stats` flag shows statistics, `--neuromodulation` shows neuromodulation levels, `--spikes` shows spike activity, `--activity` shows general activity, `--heatmap` shows activity heatmap, and `--performance` shows performance metrics.

#### Return Values

- Returns `0` on success
- Returns non-zero on failure (e.g., brain not found)

#### Common Errors

- **Brain not found**: Brain with specified name does not exist
- **Monitoring error**: Error during monitoring

---

### monitor agent

**Monitor agent activity**

#### Syntax
```bash
nlm monitor agent [--curiosity] [--reward-modulation] [--neuromodulation] [--development]
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `--curiosity` | boolean | No | false | Show curiosity information |
| `--reward-modulation` | boolean | No | false | Show reward modulation |
| `--neuromodulation` | boolean | No | false | Show neuromodulation |
| `--development` | boolean | No | false | Show development information |

#### Examples

```bash
# Monitor agent with curiosity
nlm monitor agent --curiosity

# Monitor agent with reward modulation
nlm monitor agent --reward-modulation

# Monitor agent with neuromodulation
nlm monitor agent --neuromodulation

# Monitor agent with all features
nlm monitor agent --curiosity --reward-modulation --neuromodulation --development
```

#### Command Description

Monitors agent activity in real-time. The `--curiosity` flag shows curiosity information, `--reward-modulation` shows reward modulation, `--neuromodulation` shows neuromodulation, and `--development` shows development information.

#### Return Values

- Returns `0` on success
- Returns non-zero on failure (e.g., agent not found)

#### Common Errors

- **Agent not found**: Agent with specified name does not exist
- **Monitoring error**: Error during monitoring

---

## System Commands

### help

**Show help information**

#### Syntax
```bash
nlm help [command]
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `[command]` | string | No | - | Specific command to get help for |

#### Examples

```bash
# Show general help
nlm help

# Show specific command help
nlm help brain create

# Show experiment command help
nlm help experiment run
```

#### Command Description

Displays help information. Without arguments, shows general help for all commands. With a command argument, shows specific help for that command.

#### Return Values

- Returns `0` on success

#### Common Errors

- **Command not found**: Command with specified name does not exist

---

### version

**Show version information**

#### Syntax
```bash
nlm version
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| None | - | - | - | No parameters required |

#### Examples

```bash
# Show version information
nlm version
```

#### Command Description

Displays version information for the NLM CLI and related components.

#### Return Values

- Returns `0` on success

#### Common Errors

- None

---

### exit

**Exit the NLM CLI**

#### Syntax
```bash
nlm exit
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| None | - | - | - | No parameters required |

#### Examples

```bash
# Exit the NLM CLI
nlm exit
```

#### Command Description

Exits the NLM CLI and returns to the operating system.

#### Return Values

- Returns `0` on success

#### Common Errors

- None

---

### reset

**Reset all brains to default state**

#### Syntax
```bash
nlm reset
```

#### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| None | - | - | - | No parameters required |

#### Examples

```bash
# Reset all brains to default state
nlm reset
```

#### Command Description

Resets all brains to their default state. This command resets all saved brain configurations and initializes them with default settings.

#### Return Values

- Returns `0` on success

#### Common Errors

- None

---

## Command-Line Arguments

### Common Argument Patterns

#### Key-Value Arguments
```bash
# Set configuration value
nlm brain create mybrain --neurons=1000 --regions=2

# Set multiple values
nlm brain create experiment_brain --neurons=1500 --regions=3 --plasticity_learning_rate=0.02
```

#### Boolean Flags
```bash
# Enable features
nlm brain create mybrain --verbose --stats

# Disable features
nlm brain create mybrain --no-verbose --no-stats
```

#### File Path Arguments
```bash
# Use custom configuration
nlm brain create mybrain --config configs/custom.cfg

# Save to custom file
nlm brain save mybrain checkpoint.brain.json
```

### Argument Parsing Rules

1. **Order doesn't matter**: Arguments can be in any order
2. **Flags**: Start with `--` or `-`
3. **Values**: Can be attached to flags with `=` or as separate arguments
4. **Case sensitivity**: Command names and flags are case-sensitive
5. **Spaces in values**: Use quotes for values with spaces

### Example Usage

```bash
# Complex brain creation with multiple options
nlm brain create "My Research Brain" \
  --neurons 1000 \
  --regions 2 \
  --config configs/research.cfg \
  --plasticity_learning_rate 0.01 \
  --stdp_ltp_weight 0.02 \
  --curiosity \
  --reward-modulation

# Run experiment with monitoring
nlm experiment run --brain mybrain phase2 1000 \
  --continue \
  --verbose \
  --stats

# Monitor brain with all metrics
nlm monitor brain mybrain \
  --stats \
  --neuromodulation \
  --spikes \
  --activity \
  --heatmap \
  --performance
```

---

## Command Help

### General Help

```bash
nlm help
```

### Command-Specific Help

```bash
nlm help <command>
```

### Command Categories Help

```bash
# Brain commands
nlm help brain

# Agent commands
nlm help agent

# Config commands
nlm help config

# Experiment commands
nlm help experiment

# Monitor commands
nlm help monitor

# System commands
nlm help system
```

---

## Configuration Files

### Default Configuration File

**File**: `configs/default.cfg`

```ini
# NLM Configuration File
# This is the default configuration for NLM

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

### Configuration File Format

Configuration files use the `key = value` format:

```ini
# Key-value pairs
key = value
number_key = 123
boolean_key = true
float_key = 3.14
string_key = "hello"
```

### Configuration Override

Command-line arguments can override configuration file values:

```bash
# Override neuron_count from command line
nlm brain create mybrain --neurons 2000
```

---

## Error Handling

### Error Codes

| Code | Description |
|------|-------------|
| `0` | Success |
| `1` | General error |
| `2` | Invalid parameters |
| `3` | Brain not found |
| `4` | Configuration error |
| `5` | File not found |
| `6` | Permission error |

### Error Messages

Error messages provide detailed information about the error and suggested solutions:

```bash
$ nlm brain create invalid_brain
Error: Brain name cannot be empty
Usage: nlm brain create <name> [--neurons N] [--regions R] [--config FILE]

$ nlm brain step non_existent_brain 100
Error: Brain 'non_existent_brain' not found
Available brains: mybrain, experiment_brain
```

---

## Scripting and Automation

### Using CLI in Scripts

```bash
#!/bin/bash

# Create brain
nlm brain create experiment_brain --neurons 1000 --regions 2

# Run experiment
nlm experiment run experiment_brain phase2 1000

# Monitor results
nlm monitor brain experiment_brain --stats --neuromodulation

# Save results
nlm brain save experiment_brain results.brain

# Clean up (optional)
nlm brain reset experiment_brain
```

### Python Integration

```python
import subprocess
import json

# Run NLM CLI command
def run_nlm_command(command):
    try:
        result = subprocess.run(command, shell=True, capture_output=True, text=True)
        if result.returncode == 0:
            return result.stdout
        else:
            return f"Error: {result.stderr}"
    except Exception as e:
        return f"Exception: {e}"

# Example usage
output = run_nlm_command("nlm brain create test_brain --neurons 500")
print(output)
```

---

## Performance Considerations

### Memory Usage

- **Small brain** (100 neurons): ~10MB
- **Medium brain** (1000 neurons): ~50MB
- **Large brain** (10000 neurons): ~500MB

### CPU Usage

- **Simple simulation**: 1-5% per brain
- **Complex simulation**: 10-50% per brain
- **Monitoring overhead**: 1-2%

### Scaling Considerations

- **Multiple brains**: Each brain consumes memory proportional to its size
- **Concurrent operations**: Monitor can track multiple brains simultaneously
- **Resource limits**: System may throttle operations if resources are limited

---

## Advanced Features

### Command Composition

```bash
# Combine multiple commands in a pipeline
nlm brain create pipeline_brain --neurons 500 | nlm brain initialize pipeline_brain | nlm brain step pipeline_brain 100
```

### Configuration Inheritance

```bash
# Create brain with inherited configuration
nlm brain create child_brain --neurons 500 --regions 2 \
  --config configs/parent.cfg \
  --plasticity_learning_rate 0.02 \
  --stdp_ltp_weight 0.03
```

### Batch Operations

```bash
# Process multiple brains in sequence
for brain_name in brain1 brain2 brain3; do
    nlm brain status $brain_name --stats
    nlm brain step $brain_name 100
    nlm brain save $brain_name checkpoint_${brain_name}.brain
    echo "Processed $brain_name"
done
```

---

## Conclusion

The enhanced NLM Command-Line Interface provides a comprehensive, user-friendly interface for controlling NLM brains, running experiments, and managing configurations. The CLI is designed to be:

1. **Intuitive** for beginners
2. **Flexible** for advanced users
3. **Powerful** for complex operations
4. **Documented** with comprehensive help
5. **Extensible** for custom commands

**Start using the enhanced NLM CLI today and experience the power of unified brain control!** 🚀