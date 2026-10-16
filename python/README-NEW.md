# NLM Python API - New Convenience Functions

This document describes the new convenience functions added to the NLM Python API in version 0.1.0.

## Overview

The NLM Python bindings now include several convenience functions designed to make it easier to get started with common use cases and to run standard simulation patterns.

## New Convenience Functions

### 1. `createQuickBrain()`

**Description**: Creates a brain with optimized defaults for quick testing and basic simulation.

**Returns**: `Brain` object

**Example**:
```python
import pynlm

brain = pynlm.createQuickBrain()
brain.initialize()
for i in range(100):
    brain.step(i)
```

**Default Configuration**:
- 500 neurons
- 2 regions
- Connection probability: 0.15
- Learning rate: 0.01
- Time step: 0.01
- Max steps: 1000

### 2. `createTrainingConfig()`

**Description**: Creates a configuration optimized for learning/training simulations.

**Returns**: `Config` object

**Example**:
```python
import pynlm

training_config = pynlm.createTrainingConfig()
brain = pynlm.createBrain(training_config)
```

**Features**:
- 1000 neurons
- 3 regions
- Reward modulation enabled
- Curiosity enabled
- Development enabled
- Structural plasticity enabled
- Checkpointing enabled

### 3. `createExplorationConfig()`

**Description**: Creates a configuration optimized for exploration and curiosity-driven learning simulations.

**Returns**: `Config` object

**Example**:
```python
import pynlm

exploration_config = pynlm.createExplorationConfig()
brain = pynlm.createBrain(exploration_config)
```

**Features**:
- 800 neurons
- 2 regions
- Novelty detection enabled
- Reward prediction error enabled
- Exploration bonus: 1.0
- Curiosity strength: 2.0

### 4. `runBasicSimulation(brain, world, steps, verbose=False)`

**Description**: Runs a basic simulation with the given brain and world for specified steps.

**Parameters**:
- `brain`: Brain object
- `world`: SimpleWorld object
- `steps`: Number of simulation steps to run
- `verbose`: Whether to print progress information

**Returns**: Tuple of `(total_neurons, average_firing_rate)`

**Example**:
```python
import pynlm

brain = pynlm.createQuickBrain()
world = pynlm.createQuickWorld()

neurons, avg_rate = pynlm.runBasicSimulation(brain, world, 1000, verbose=True)
print(f"Simulation complete: {neurons} neurons, avg firing rate: {avg_rate}")
```

### 5. `validateConfig(config)`

**Description**: Validates configuration parameters and provides feedback.

**Parameters**:
- `config`: Config object to validate

**Returns**: Tuple of `(issues, warnings, suggestions)`

**Example**:
```python
import pynlm

config = pynlm.createQuickBrain().getConfig()
issues, warnings, suggestions = pynlm.validateConfig(config)

if issues:
    print("Config issues found:", issues)
if warnings:
    print("Warnings:", warnings)
if suggestions:
    print("Suggestions:", suggestions)
```

## Additional Helper Functions

### 6. `createQuickWorld()`
Creates a simple world with reasonable default configuration.

### 7. `createAgentSetup(brain, world)`
Creates and initializes an agent brain with the given world.

### 8. `runTrainingEpisode(brain, world, agent, steps, enable_learning=True, verbose=False)`
Runs a complete training episode with reward-based learning.

### 9. `isBrainReady(brain)`
Checks if a brain is properly initialized and ready for simulation.

### 10. `saveBrainCheckpoint(brain, filepath)`
Saves brain state to a file.

### 11. `loadBrainCheckpoint(brain, filepath)`
Loads brain state from a file.

### 12. `createExampleConfig()`
Creates a minimal functional configuration for basic simulations.

### 13. `getBrainStats(brain)`
Gets brain statistics as a dictionary of key metrics.

## Quick Start Guide

### Basic Usage

```python
import pynlm

# Create everything with sensible defaults
brain = pynlm.createQuickBrain()
world = pynlm.createQuickWorld()
agent = pynlm.createAgentSetup(brain, world)

# Run a training episode
neuron_count, avg_rate, total_reward = pynlm.runTrainingEpisode(
    brain, world, agent, steps=500, enable_learning=True, verbose=True
)

print(f"Training complete: {neuron_count} neurons, avg rate: {avg_rate:.4f}")
```

### Configuration Validation

```python
import pynlm

# Create a custom config
config = pynlm.createExampleConfig()

# Validate and get feedback
issues, warnings, suggestions = pynlm.validateConfig(config)

# Fix any issues found
if issues:
    for issue in issues:
        print("Fix:", issue)
```

### Simulation Patterns

#### Pattern 1: Quick Test
```python
brain = pynlm.createQuickBrain()
brain.initialize()
for step in range(100):
    brain.step(step)
```

#### Pattern 2: Training
```python
config = pynlm.createTrainingConfig()
brain = pynlm.createBrain(config)
world = pynlm.createQuickWorld()
agent = pynlm.createAgentSetup(brain, world)

for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Pattern 3: Exploration
```python
config = pynlm.createExplorationConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(500):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: firing rate = {brain.getAverageFiringRate():.4f}")
```

## Error Handling

The new convenience functions include comprehensive error handling with informative messages:

- Null object checks (e.g., "Brain is null - cannot run simulation")
- Invalid file path checks (e.g., "Filepath is empty - cannot save checkpoint")
- Parameter validation (e.g., in `validateConfig`)

## Backward Compatibility

All new functions are additive and maintain backward compatibility with existing code. The original API functions remain unchanged:

- `pynlm.createDefaultConfig()`
- `pynlm.createBrain(config)`
- `pynlm.createSimpleWorld()`
- `pynlm.createAgentBrain(brain)`

## Building and Installation

The new convenience functions are automatically included in the Python module when built with scikit-build-core. No additional steps are required.

```bash
# Build from source
pip install .

# Install in development mode
pip install -e .
```

## Testing

Example usage is available in the `examples/` directory. The best way to test the new convenience functions is to build the module and run the example scripts.

```bash
cd /path/to/nlm
mkdir -p build
cd build
cmake ..
make -j4
```

Then run the examples:
```python
python examples/demo_new_api.py
```

## Future Enhancements

The convenience functions provide a foundation for future enhancements such as:
- More specialized configuration templates
- Advanced simulation patterns
- Cross-validation utilities
- Performance profiling tools
- Visualization helpers

---

**Version**: 0.1.0  
**Released**: Phase 6 (Final Integration)  
**License**: MIT
