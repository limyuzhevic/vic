# NLM Advanced Development CLI

## Overview

This module provides comprehensive command-line and programmatic interfaces for advanced development operations on the NLM (Neural Learning Machine) system. The Advanced CLI extends the basic NLM functionality with specialized commands organized into functional categories:

## Key Features

### 1. Configuration Management Commands
- List all configuration parameters with descriptions
- Get/set individual parameters
- Load/save configuration files
- Validate configuration
- Create configuration templates

### 2. Checkpoint Management Commands
- List available checkpoints
- Create manual checkpoints
- Restore from checkpoints
- Delete checkpoints
- Checkpoint statistics

### 3. Brain State Inspection Commands
- Detailed brain state dump (neurons, synapses, regions)
- Memory system statistics (working, episodic, semantic, procedural, associative)
- Neuromodulation system status (dopamine, curiosity, novelty, prediction error)
- Development system status (current stage, plasticity rates)
- Prediction system status (prediction error, confidence)

### 4. Development System Commands
- Advance development stage
- Modify plasticity rates
- Trigger specific development events
- Set developmental goals

### 5. Memory System Commands
- Inspect memory contents (working, episodic)
- Search memories by criteria
- Consolidate memories
- Replay specific memories

### 6. Simulation Control Commands
- Speed control (slow motion, fast forward)
- Pause/resume simulation
- Jump to specific steps
- Record simulation data
- Replay recorded simulations

### 7. Debugging and Monitoring Commands
- Real-time system monitoring
- Performance profiling
- Memory usage tracking
- Checkpoint validation
- Network analysis

### 8. World Management Commands
- Add/remove world objects
- Modify world parameters
- Set agent start positions
- World validation

## Usage

### Command Line Interface

The Advanced CLI can be accessed via the `nlm` command with the `--advanced` flag:

```bash
nlm --advanced command [options]
```

#### Available Command Categories

1. **Config** - Configuration management commands
   - `config list` - List all configuration parameters
   - `config get <key>` - Get individual parameter
   - `config set <key> <value>` - Set individual parameter
   - `config load <source> [--target <target>]` - Load configuration from file
   - `config save <target> [--source <source>]` - Save configuration to file
   - `config validate` - Validate configuration
   - `config template <path>` - Create configuration template

2. **Checkpoint** - Checkpoint management commands
   - `checkpoint list` - List available checkpoints
   - `checkpoint create <name> [--config <config>]` - Create manual checkpoint
   - `checkpoint restore <name> [--config <config>]` - Restore from checkpoint
   - `checkpoint delete <name>` - Delete checkpoint
   - `checkpoint stats` - Get checkpoint statistics

3. **Brain** - Brain state inspection commands
   - `brain dump [--output <output>]` - Detailed brain state dump
   - `brain memory-stats` - Memory system statistics
   - `brain neuromodulation` - Neuromodulation system status
   - `brain development` - Development system status
   - `brain prediction` - Prediction system status

4. **Development** - Development system commands
   - `development advance` - Advance development stage
   - `development plasticity [--rates <rates>]` - Modify plasticity rates
   - `development event <type>` - Trigger specific development event
   - `development goals <goals>` - Set developmental goals

5. **Memory** - Memory system commands
   - `memory inspect <type>` - Inspect memory contents
   - `memory search [--criteria <criteria>]` - Search memories by criteria
   - `memory consolidate` - Consolidate memories
   - `memory replay <id>` - Replay specific memory

6. **Simulation** - Simulation control commands
   - `simulation speed <mode> [--factor <factor>]` - Control simulation speed
   - `simulation pause` - Pause simulation
   - `simulation resume` - Resume simulation
   - `simulation jump <step>` - Jump to specific step
   - `simulation record <output>` - Record simulation data
   - `simulation replay <input>` - Replay recorded simulation

7. **Debug** - Debugging and monitoring commands
   - `debug monitor` - Real-time system monitoring
   - `debug profile` - Performance profiling
   - `debug memory` - Memory usage tracking
   - `debug validate` - Checkpoint validation
   - `debug network` - Network analysis

8. **World** - World management commands
   - `world add <type> <x> <y> [--properties <properties>]` - Add world object
   - `world modify [--parameters <parameters>]` - Modify world parameters
   - `world start <x> <y>` - Set agent start position
   - `world validate` - World validation

#### Generic Command

The generic `run` command allows running any advanced command:

```bash
nlm --advanced run <category> <command> [args]
```

### Python API

The Advanced API provides programmatic access to all advanced functionality:

```python
from nlm_cli.advanced import AdvancedAPI

# Initialize API
api = AdvancedAPI()

# Configuration management
print(api.list_config_params())
print(api.get_config_param("random_seed"))
print(api.set_config_param("neuron_count", 2000))

# Checkpoint management
print(api.list_checkpoints())
print(api.create_checkpoint("experiment_1"))

# Brain state inspection
print(api.brain_state_dump("brain_state.json"))
print(api.memory_system_statistics())

# Development system
print(api.advance_development_stage())
print(api.modify_plasticity_rates({"rate": 0.02, "time_constant": 30.0}))

# Memory system
print(api.inspect_memory_contents("working"))
print(api.search_memories({"type": "episodic", "time_range": "last_24h"}))

# Simulation control
print(api.control_simulation_speed("fast", 2.0))
print(api.pause_simulation())
print(api.resume_simulation())

# Debugging and monitoring
print(api.real_time_monitoring())
print(api.performance_profiling())

# World management
print(api.add_world_object("resource", 10.5, 20.3, {"value": 100.0}))
print(api.set_agent_start_position(5.0, 5.0))
```

## Installation

### Prerequisites

1. **NLM System**: Ensure NLM (Neural Learning Machine) system is installed and configured
2. **Python Environment**: Python 3.8+
3. **Dependencies**: Install required Python packages

### Installation Steps

1. **Clone the repository** (if not already done):
   ```bash
   git clone <repository-url>
   cd nlm
   ```

2. **Install the NLM Python package**:
   ```bash
   pip install -e .
   ```

3. **Set up the Advanced CLI**:
   ```bash
   # The advanced CLI is included in the NLM package
   # No additional setup required
   ```

## Configuration

### Configuration File Format

NLM uses configuration files to store system parameters. The default configuration file is `configs/default.cfg` with the following format:

```
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
dopamine_baseline = 0.1
reward_discount_factor = 0.99
environment_name = GridWorld
environment_width = 10
environment_height = 10
log_level = INFO
log_to_file = false
log_filename = nlm.log
```

### Configuration Template

To create a configuration template:

```bash
nlm --advanced config template config_template.cfg
```

### Configuration Validation

To validate the current configuration:

```bash
nlm --advanced config validate
```

## Examples

### Example 1: Running a Simulation

```bash
# Run a simulation with 2000 steps and save checkpoint
nlm run 2000 --save

# Load configuration from file
nlm load my_config.cfg

# Show current configuration
nlm config show
```

### Example 2: Working with Checkpoints

```bash
# List available checkpoints
nlm --advanced checkpoint list

# Create a new checkpoint
nlm --advanced checkpoint create experiment_1

# Restore from checkpoint
nlm --advanced checkpoint restore experiment_1

# Get checkpoint statistics
nlm --advanced checkpoint stats
```

### Example 3: Brain State Inspection

```bash
# Get detailed brain state dump
nlm --advanced brain dump --output brain_state.json

# Get memory system statistics
nlm --advanced brain memory-stats

# Get neuromodulation system status
nlm --advanced brain neuromodulation
```

### Example 4: Development System

```bash
# Advance development stage
nlm --advanced development advance

# Modify plasticity rates
nlm --advanced development plasticity --rates rate=0.02 time_constant=30.0

# Set developmental goals
nlm --advanced development goals "maturation,adaptation,learning"
```

### Example 5: Memory System

```bash
# Inspect working memory
nlm --advanced memory inspect working

# Search memories
nlm --advanced memory search --criteria type=episodic time_range=last_24h

# Consolidate memories
nlm --advanced memory consolidate
```

### Example 6: Simulation Control

```bash
# Control simulation speed
nlm --advanced simulation speed fast --factor 2.0

# Pause and resume simulation
nlm --advanced simulation pause
nlm --advanced simulation resume

# Jump to specific step
nlm --advanced simulation jump 500
```

### Example 7: Debugging and Monitoring

```bash
# Get real-time monitoring data
nlm --advanced debug monitor

# Get performance profiling
nlm --advanced debug profile

# Get memory usage tracking
nlm --advanced debug memory

# Validate checkpoints
nlm --advanced debug validate

# Get network analysis
nlm --advanced debug network
```

### Example 8: World Management

```bash
# Add a resource object
nlm --advanced world add resource 10.5 20.3 --properties value=100.0

# Modify world parameters
nlm --advanced world modify --parameters width=20 height=20

# Set agent start position
nlm --advanced world start 5.0 5.0

# Validate world
nlm --advanced world validate
```

## File Structure

The Advanced CLI is organized into the following directories:

```
nlm_cli/
├── __main__.py              # Entry point
├── basic/                  # Basic CLI interface
│   └── cli.py             # Basic CLI implementation
├── advanced/               # Advanced CLI interface
│   ├── __init__.py        # Advanced module initialization
│   ├── cli.py             # Advanced CLI implementation
│   ├── api.py             # Advanced API implementation
│   └── commands/          # Advanced command implementations
│       ├── config.py      # Configuration commands
│       ├── checkpoint.py  # Checkpoint commands
│       ├── brain_state.py # Brain state inspection commands
│       ├── development.py # Development system commands
│       ├── memory.py     # Memory system commands
│       ├── simulation.py # Simulation control commands
│       ├── debug.py      # Debugging and monitoring commands
│       └── world.py       # World management commands
└── utils/                  # Utility functions
    └── helpers.py         # Helper functions
```

## Advanced API Reference

The Advanced API provides programmatic access to all advanced functionality through a unified interface. Each category of functionality is exposed as a class method:

### Configuration Management

- `list_config_params()`: List all configuration parameters
- `get_config_param(key)`: Get individual parameter
- `set_config_param(key, value)`: Set individual parameter
- `load_config_file(source, target)`: Load configuration from file
- `save_config_file(target, source)`: Save configuration to file
- `validate_config()`: Validate configuration
- `create_config_template(path)`: Create configuration template

### Checkpoint Management

- `list_checkpoints()`: List available checkpoints
- `create_checkpoint(name, config_path)`: Create manual checkpoint
- `restore_checkpoint(name, config_path)`: Restore from checkpoint
- `delete_checkpoint(name)`: Delete checkpoint
- `checkpoint_statistics()`: Get checkpoint statistics

### Brain State Inspection

- `brain_state_dump(output_path)`: Detailed brain state dump
- `memory_system_statistics()`: Memory system statistics
- `neuromodulation_system_status()`: Neuromodulation system status
- `development_system_status()`: Development system status
- `prediction_system_status()`: Prediction system status

### Development System

- `advance_development_stage()`: Advance development stage
- `modify_plasticity_rates(rates)`: Modify plasticity rates
- `trigger_development_event(event_type)`: Trigger specific development event
- `set_developmental_goals(goals)`: Set developmental goals

### Memory System

- `inspect_memory_contents(memory_type)`: Inspect memory contents
- `search_memories(criteria)`: Search memories by criteria
- `consolidate_memories()`: Consolidate memories
- `replay_memory(memory_id)`: Replay specific memory

### Simulation Control

- `control_simulation_speed(mode, factor)`: Control simulation speed
- `pause_simulation()`: Pause simulation
- `resume_simulation()`: Resume simulation
- `jump_to_step(step)`: Jump to specific step
- `record_simulation(output_path)`: Record simulation data
- `replay_simulation(input_path)`: Replay recorded simulation

### Debugging and Monitoring

- `real_time_monitoring()`: Real-time system monitoring
- `performance_profiling()`: Performance profiling
- `memory_usage_tracking()`: Memory usage tracking
- `checkpoint_validation()`: Checkpoint validation
- `network_analysis()`: Network analysis

### World Management

- `add_world_object(obj_type, x, y, properties)`: Add/remove world objects
- `modify_world_parameters(parameters)`: Modify world parameters
- `set_agent_start_position(x, y)`: Set agent start positions
- `world_validation()`: World validation

## Development

### Adding New Commands

To add a new command category, create a new file in the `nlm_cli/advanced/commands/` directory:

```python
# File: nlm_cli/advanced/commands/<category>.py
from ..api import AdvancedAPI
from typing import Dict, Any, List, Optional

class <Category>Commands:
    """<Category> commands."""
    
    def __init__(self, api: AdvancedAPI):
        self.api = api
    
    # Command methods here
```

Update the `AdvancedCLI` class in `nlm_cli/advanced/cli.py` to include the new command category.

### Running Tests

To run the tests for the Advanced CLI:

```bash
pytest nlm_cli/
```

### Documentation

For detailed documentation on NLM system and its API, refer to:
- `README.md`: Main project documentation
- `HOW_TO_USE.md`: Usage instructions
- `docs/`: Additional documentation

## Support

For issues, questions, or feature requests:
- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Documentation](https://nlm.readthedocs.io)
- [Contact](mailto:support@nlm-project.org)

## License

This project is licensed under the MIT License. See `LICENSE` file for details.

## Acknowledgments

This Advanced CLI is built upon the NLM (Neural Learning Machine) framework, an experimental artificial developmental brain system implementing real spiking neural computation with Leaky Integrate-and-Fire (LIF) neurons, event-driven spike propagation, STDP and Hebbian plasticity, and structural plasticity.

Special thanks to the NLM development team and contributors.
