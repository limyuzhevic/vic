# NLM High-Level API Documentation

The NLM (Neural Learning Machine) High-Level API provides convenient functions that simplify common operations when working with the NLM framework. These functions are designed to make the powerful C++ neural simulation framework more accessible to Python users while maintaining all the underlying functionality.

## Overview

The high-level API addresses common patterns shown in the documentation examples, providing:

1. **Simplified brain creation** with configuration
2. **Streamlined simulation loops** without repetitive code
3. **Complete agent simulation** with all setup steps
4. **Easy statistics formatting** for reading and processing
5. **Batch feature management** for brain subsystems
6. **Safe state management** with automatic backups
7. **Flexible configuration handling** from dictionaries
8. **World creation** from configuration dictionaries

## Getting Started

### Installation

The high-level API is included with the main pynlm installation. If you need additional dependencies for the high-level functions:

```bash
# Install with high-level API support
pip install -e .[highlevel]

# Or install with all optional dependencies
pip install -e .[dev,cli,highlevel]
```

### Basic Usage

```python
import pynlm
from python.high_level_api import create_agent_brain_with_config, get_brain_stats

# Create a brain with default configuration
brain = create_agent_brain_with_config()
brain.initialize()

# Get statistics as a dictionary
stats = get_brain_stats(brain)
print(f"Neurons: {stats['neuron_count']}")
print(f"Firing rate: {stats['average_firing_rate']:.2f}")
```

## Core Functions

### 1. `create_agent_brain_with_config(config_dict=None)`

Creates a brain with configuration in a single step, combining config creation and brain initialization.

**Simple Usage:**
```python
# Use defaults
brain = create_agent_brain_with_config()
```

**Advanced Usage:**
```python
# Custom configuration
config = {
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.15
    },
    "plasticity": {
        "stdp": {"enable": True, "learning_rate": 0.002}
    }
}
brain = create_agent_brain_with_config(config)
```

**Parameters:**
- `config_dict`: Optional dictionary with configuration values (default: None)

**Returns:**
- `Brain`: Initialized brain object

### 2. `run_simulation_loop(brain, world, agent, steps, verbose=False)`

Runs a complete simulation loop with brain, world, and agent, handling the common workflow.

**Usage:**
```python
# Setup
brain = create_agent_brain_with_config()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)

# Run simulation
results = run_simulation_loop(brain, world, agent, 1000, verbose=True)
print(f"Completed {results['steps']} steps with {results['spike_increase']} spikes")
```

**Parameters:**
- `brain`: Brain object
- `world`: World object  
- `agent`: AgentBrain object
- `steps`: Number of simulation steps to run
- `verbose`: Enable verbose output (default: False)

**Returns:**
- `dict`: Simulation results including statistics

### 3. `run_agent_simulation(config=None, world_config=None, steps=1000, enable_features=True)`

Runs a complete agent simulation with all configuration and setup, replicating the documentation example.

**Basic Usage:**
```python
# Run with defaults
results = run_agent_simulation(steps=500)
```

**Advanced Usage:**
```python
# Full customization
config = {"brain": {"neuron_count": 3000}}
world_config = {"width": 30, "height": 30, "agent_start": (15.0, 15.0)}
results = run_agent_simulation(
    config=config,
    world_config=world_config,
    steps=2000,
    enable_features=True
)
```

**Parameters:**
- `config`: Optional brain configuration dictionary (default: None)
- `world_config`: Optional world configuration dictionary (default: None)
- `steps`: Number of simulation steps (default: 1000)
- `enable_features`: Enable agent features (default: True)

**Returns:**
- `dict`: Complete simulation results and final state

### 4. `get_brain_stats(brain) -> dict`

Gets formatted brain statistics as a dictionary for easy reading and programmatic access.

**Usage:**
```python
brain = create_agent_brain_with_config()

# Get statistics
stats = get_brain_stats(brain)

# Access specific values
print(f"Total neurons: {stats['neuron_count']}")
print(f"Firing neurons: {stats['firing_neuron_count']}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f}")

# Iterate over all statistics
for key, value in stats.items():
    print(f"{key}: {value}")
```

**Parameters:**
- `brain`: Brain object

**Returns:**
- `dict`: Dictionary containing all brain statistics

### 5. `enable_brain_features(agent, **features)`

Enables/disables brain subsystems in batch, avoiding repetitive enable/disable calls.

**Usage:**
```python
agent = pynlm.createAgentBrain(brain)

# Enable all features
enable_brain_features(agent,
    reward_modulation=True,
    structural_plasticity=True,
    development=True,
    curiosity=True
)

# Disable specific features
enable_brain_features(agent, development=False, curiosity=False)
```

**Available Features:**
- `reward_modulation`: Reward modulation subsystem
- `structural_plasticity`: Structural plasticity subsystem
- `development`: Development system
- `curiosity`: Curiosity-driven exploration

**Parameters:**
- `agent`: AgentBrain object
- `**features`: Feature names and their enable/disable state

### 6. `save_brain_with_timestamp(brain, filepath=None)`

Saves brain state with automatic timestamp in filename for easy backup and organization.

**Usage:**
```python
brain = create_agent_brain_with_config()

# Auto-generated filename with timestamp
filepath = save_brain_with_timestamp(brain)
print(f"Brain saved to: {filepath}")

# Custom filename
filepath = save_brain_with_timestamp(brain, "my_brain_state.bin")
```

**Parameters:**
- `brain`: Brain object to save
- `filepath`: Optional output filepath (default: None, uses timestamped default)

**Returns:**
- `str`: Actual filepath where brain state was saved

### 7. `load_brain_with_backup(brain, filepath, backup_if_failed=True)`

Loads brain state with automatic backup on failure, preventing data loss.

**Usage:**
```python
brain = create_agent_brain_with_config()

# Safe load with backup
success = load_brain_with_backup(brain, "checkpoint.bin")

if success:
    print("Brain state loaded successfully")
else:
    print("Brain state load failed, backup created")
```

**Parameters:**
- `brain`: Brain object to load state into
- `filepath`: Path to brain state file
- `backup_if_failed`: Create backup of current brain state if load fails (default: True)

**Returns:**
- `bool`: True if load successful, False if failed (and backup was created)

### 8. `configure_brain_from_dict(brain, config_dict)`

Configures an existing brain from dictionary configuration, enabling dynamic configuration changes.

**Usage:**
```python
brain = create_agent_brain_with_config()

# Update configuration
config_updates = {
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.25
    },
    "neuromod": {
        "dopamine": {"scale": 1.5}
    }
}

configure_brain_from_dict(brain, config_updates)
```

**Parameters:**
- `brain`: Brain object to configure
- `config_dict`: Configuration dictionary with nested sections

### 9. `create_world_from_config(world_config_dict)`

Creates and configures a world from dictionary configuration, simplifying world setup.

**Usage:**
```python
world_config = {
    "width": 40,
    "height": 40,
    "visionWidth": 16,
    "visionHeight": 16,
    "agent_start": (20.0, 20.0)
}

world = create_world_from_config(world_config)
```

**Parameters:**
- `world_config_dict`: World configuration dictionary

**Returns:**
- `SimpleWorld`: Configured world object

## Example Usage

### Complete Example

Here's a complete example showing how to use the high-level API:

```python
#!/usr/bin/env python3
"""
Example: Complete NLM Agent Simulation using High-Level API
"""

import pynlm
from python.high_level_api import (
    create_agent_brain_with_config,
    create_world_from_config,
    enable_brain_features,
    run_agent_simulation,
    get_brain_stats,
    save_brain_with_timestamp
)

def main():
    print("=== NLM High-Level API Example ===\n")
    
    # 1. Create brain with configuration
    print("1. Creating brain with configuration...")
    config = {
        "brain": {
            "neuron_count": 2000,
            "synapse_density": 0.1
        }
    }
    brain = create_agent_brain_with_config(config)
    print(f"   ✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # 2. Create world from configuration
    print("\n2. Creating world from configuration...")
    world_config = {
        "width": 30,
        "height": 30,
        "visionWidth": 12,
        "visionHeight": 12,
        "agent_start": (15.0, 15.0)
    }
    world = create_world_from_config(world_config)
    print(f"   ✓ World created: {world.getWidth()}x{world.getHeight()}")
    
    # 3. Create agent
    print("\n3. Creating agent...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print("   ✓ Agent initialized")
    
    # 4. Enable brain features
    print("\n4. Enabling brain features...")
    enable_brain_features(agent, 
                         reward_modulation=True,
                         structural_plasticity=True,
                         development=True,
                         curiosity=True)
    print("   ✓ All features enabled")
    
    # 5. Get initial statistics
    print("\n5. Getting initial statistics...")
    initial_stats = get_brain_stats(brain)
    print(f"   Initial neurons: {initial_stats['neuron_count']}")
    print(f"   Initial firing rate: {initial_stats['average_firing_rate']:.2f}")
    
    # 6. Run simulation
    print("\n6. Running simulation (500 steps)...")
    results = run_simulation_loop(brain, world, agent, 500, verbose=True)
    print(f"   ✓ Simulation complete: {results['steps']} steps")
    print(f"   Spikes generated: {results['spike_increase']}")
    print(f"   Average firing rate: {results['average_firing_rate']:.3f}")
    
    # 7. Get final statistics
    print("\n7. Getting final statistics...")
    final_stats = get_brain_stats(brain)
    print(f"   Final neurons: {final_stats['neuron_count']}")
    print(f"   Final firing neurons: {final_stats['firing_neuron_count']}")
    print(f"   Final spike count: {final_stats['total_spike_count']}")
    print(f"   Final developmental stage: {final_stats['developmental_stage']}")
    
    # 8. Save brain state
    print("\n8. Saving brain state...")
    save_path = save_brain_with_timestamp(brain)
    print(f"   ✓ Brain saved to: {save_path}")
    
    print("\n=== Example Complete ===")

if __name__ == "__main__":
    main()
```

## Migration Guide

### From Documentation to High-Level API

**Before (from HOW_TO_USE.md):**
```python
# Manual steps from documentation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

**After (using high-level API):**
```python
# Simplified with high-level API
brain = create_agent_brain_with_config()
```

**Before (simulation loop from documentation):**
```python
# Repetitive simulation code
for step in range(1000):
    world.update(timestep=0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    motor_cmd = agent.decodeMotorCommand()
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
```

**After (using high-level API):**
```python
# Simplified simulation loop
results = run_simulation_loop(brain, world, agent, 1000, verbose=True)
```

## Error Handling

The high-level API includes robust error handling for common scenarios:

1. **Import errors**: Clear messages when pynlm is not available
2. **Configuration errors**: Descriptive error messages for invalid configurations
3. **File I/O errors**: Graceful handling of save/load operations with backups
4. **Type validation**: Automatic type conversion for configuration values

## Performance Considerations

- The high-level API adds minimal overhead compared to direct C++ calls
- Functions are optimized to minimize unnecessary object creation
- Memory management is handled efficiently through shared pointers
- No performance impact when not using the convenience functions

## Testing

Run the high-level API tests to ensure everything works correctly:

```bash
python -m python.test_high_level_api
```

Or run the example scripts:

```bash
python -m python.api_examples
```

## Integration

The high-level API seamlessly integrates with existing NLM functionality:

- Works with all existing pynlm functions and classes
- Compatible with CLI tools and configurations
- Follows the same naming conventions as the underlying framework
- Provides backward compatibility with existing code

## Future Enhancements

The high-level API can be extended with:

- Additional convenience functions for common operations
- Configuration file templates and presets
- Visualization utilities for brain statistics
- Automated experiment setup and parameter tuning
- Integration with machine learning workflows

## Support

For issues with the high-level API:
1. Check the test output for detailed error messages
2. Review the examples in `api_examples.py`
3. Consult the full documentation in `HOW_TO_USE.md`
4. Report issues through the NLM issue tracker