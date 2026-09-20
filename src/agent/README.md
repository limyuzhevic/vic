# NLM Command-Line Interface
# Command-line tool for NLM runtime control

__author__ = "NLM Development Team"
__version__ = "1.0.0"

## Overview

This project provides a comprehensive command-line interface for controlling NLM (Neural Learning Machine) brain simulation runtime. It enables real-time control, monitoring, and experimentation with NLM brains through a rich set of commands.

## Features

### Core Commands

1. **Brain Control**
   - `help` - Show available commands
   - `status` - Show brain state and agent status
   - `step [count]` - Run simulation steps
   - `reset` - Reset brain state
   - `save [file]` - Save brain state
   - `load [file]` - Load brain state

2. **Configuration Management**
   - `config view [key]` - View configuration values
   - `config modify <key> <value>` - Modify configuration
   - `config save <file>` - Save configuration

3. **Memory Inspection**
   - `memory working` - Show working memory details
   - `memory episodic` - Show episodic memory details

4. **State Control**
   - `save [file]` - Save brain state
   - `load [file]` - Load brain state

5. **Statistics and Monitoring**
   - `stats` - Show neural network statistics
   - `monitoring` - Advanced monitoring

6. **Experiment Control**
   - `experiment run` - Run experiments
   - `experiment status` - Show experiment status

### Advanced Features

- **Batch Processing** - Execute multiple commands in sequence
- **Script Support** - Run commands from script files
- **Event-Driven Handling** - Respond to real-time events

## Installation

The command-line interface is built as part of the NLM project. Ensure the NLM library is properly installed and configured.

## Usage

### Basic Usage

```bash
./nlm_command [options]
```

### Interactive Mode

```bash
./nlm_command
```

### Command Examples

```bash
# Show help
./nlm_command help

# Check brain status
./nlm_command status

# Run 10 simulation steps
./nlm_command step 10

# Save brain state
./nlm_command save brain_state.bin

# Load brain state
./nlm_command load brain_state.bin

# View configuration
./nlm_command config view

# Check memory
./nlm_command memory working
```

### Script Mode

```bash
# Run commands from a script file
./nlm_command script commands.txt

# Run batch of commands
./nlm_command batch "step 5; save test.bin; status"
```

## Configuration

### Default Configuration

- **Working Directory**: Current directory
- **Default Save Path**: brain_state.bin
- **Configuration File**: configs/default.cfg

### Command Options

- `--help` - Show help message
- `--version` - Show version
- `--config <file>` - Load configuration from file
- `--interactive` - Run in interactive mode (default)
- `--batch` - Run in batch mode
- `--script <file>` - Run commands from script file

## Command Reference

### help

Shows all available commands with brief descriptions.

**Usage**: `help`

**Example**: `./nlm_command help`

### status

Displays current brain state including neuron counts, synaptic activity, and agent brain status.

**Usage**: `status`

**Example**: `./nlm_command status`

### step [count]

Runs simulation steps. If count is not specified, defaults to 1.

**Usage**: `step [count]`

**Examples**:
- `./nlm_command step` - Run 1 step
- `./nlm_command step 10` - Run 10 steps

### reset

Resets the brain to its initial state, clearing all accumulated activity.

**Usage**: `reset`

**Example**: `./nlm_command reset`

### save [file]

Saves the current brain state to a file for later restoration.

**Usage**: `save [file]`

**Examples**:
- `./nlm_command save brain_state.bin`
- `./nlm_command save ./saves/my_brain.bin`

### load [file]

Loads a previously saved brain state from file.

**Usage**: `load [file]`

**Example**: `./nlm_command load brain_state.bin`

### config

Configuration management commands.

**Subcommands**:
- `view [key]` - View configuration values (all if key not specified)
- `modify <key> <value>` - Modify configuration value
- `save <file>` - Save configuration to file

**Examples**:
- `./nlm_command config view`
- `./nlm_command config view random_seed`
- `./nlm_command config modify random_seed 123`
- `./nlm_command config save config_backup.cfg`

### memory

Memory inspection commands.

**Subcommands**:
- `working` - Show working memory details
- `episodic` - Show episodic memory details

**Examples**:
- `./nlm_command memory working`
- `./nlm_command memory episodic`

### stats

Shows comprehensive neural network statistics including neuron counts, spike activity, and performance metrics.

**Usage**: `stats`

**Example**: `./nlm_command stats`

### experiment

Experiment control commands.

**Subcommands**:
- `run` - Run an experiment
- `status` - Show experiment status

**Examples**:
- `./nlm_command experiment run`
- `./nlm_command experiment status`

## Advanced Features

### Batch Commands

Execute multiple commands in sequence using semicolons:

```bash
./nlm_command batch "step 5; save test.bin; status"
```

### Script Files

Run commands from a script file:

```bash
./nlm_command script my_commands.txt
```

Script file format (one command per line):
```
step 10
memory working
stats
save test.bin
```

### Event-Driven Commands

Respond to specific events in real-time:

```bash
./nlm_command event spike_count >100
./nlm_command event learning_detected
```

## Configuration File Format

Configuration files use key=value format:

```
# Example config file
random_seed=42
simulation_timestep=0.001
neuron_count=500
region_count=1
connection_probability=0.15
stdp_ltp_weight=0.02
stdp_ltd_weight=0.015
```

## File Formats

### Brain State Files

Binary files containing serialized brain state for persistence and backup.

### Configuration Files

Text files with key=value pairs for NLM configuration.

## Error Handling

The command-line interface provides clear error messages for invalid commands and operations. Common errors include:

- "Unknown command: <command>"
- "Missing required arguments"
- "Invalid argument value"
- "File not found"
- "Permission denied"

## Integration with AgentBrain

The command-line interface integrates with AgentBrain through:

1. **Real-time Control**: Commands can manipulate brain simulation state
2. **State Management**: Save/load operations preserve brain state
3. **Monitoring**: Commands provide insights into brain activity
4. **Experiment Control**: Commands enable automated experimentation

## Building the CLI

To build the command-line interface:

```bash
cd /path/to/nlm/project
mkdir build
cd build
cmake ..
make nlm_command
```

## License

This project is part of the NLM (Neural Learning Machine) project. See LICENSE file for details.

## Contact

For issues or questions about the NLM Command-Line Interface, please refer to the NLM documentation or contact the development team.