# NLM Python API Reference - Brain Class

## Overview

The Brain class is the central neural simulation component in NLM. It implements a spiking neural network with Leaky Integrate-and-Fire (LIF) neurons, supporting complex dynamics including synaptic plasticity, neuromodulation, and developmental changes.

## Key Features

- **Event-driven spike propagation** with precise timing
- **Multiple plasticity mechanisms** (STDP, Hebbian, structural)
- **Neuromodulation** (dopamine, curiosity, novelty)
- **Developmental stages** affecting plasticity and dynamics
- **Memory integration** (working, episodic, semantic)
- **Prediction systems** for forward modeling
- **Checkpoint save/load** functionality
- **Real-time statistics** and monitoring

## Class Definition

```python
brain = pynlm.createBrain(config)
```

## Core Methods

### Initialization and Configuration

#### `brain.initialize()`
```python
brain.initialize()
```

**Description**: Initialize the brain with configuration parameters. Must be called before any simulation steps.

**Parameters**:
- None

**Returns**:
- None

**Example**:
```python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()  # Essential first step
```

#### `brain.reset()`
```python
brain.reset()
```

**Description**: Reset the brain to its initial state. Clears all spike history and internal state.

**Parameters**:
- None

**Returns**:
- None

**Example**:
```python
brain.initialize()
# Run simulation
brain.reset()  # Reset for new episode
```

### Simulation Control

#### `brain.step(step_number)`
```python
brain.step(step_number)
```

**Description**: Execute one simulation step. Advances all neurons, processes spikes, updates synaptic weights, and handles plasticity.

**Parameters**:
- `step_number` (int): Simulation step number (used for plasticity timing)

**Returns**:
- None

**Overloads**:
```python
brain.step(step_number, timestamp)  # With timestamp
```

**Example**:
```python
for step in range(1000):
    brain.step(step)
```

#### `brain.step(step_number, timestamp)`
```python
brain.step(step_number, timestamp)
```

**Description**: Execute one simulation step with precise timestamp. Useful for modeling time-dependent processes.

**Parameters**:
- `step_number` (int): Simulation step number
- `timestamp` (float): Current simulation time in milliseconds

**Returns**:
- None

**Example**:
```python
for step in range(100):
    brain.step(step, step * 10.0)  # Each step is 10ms
```

### Neural Input/Output

#### `brain.receiveSensoryInput(sensory_input)`
```python
brain.receiveSensoryInput(sensory_input)
```

**Description**: Inject sensory input into the brain. Modulates neural activity based on external stimuli.

**Parameters**:
- `sensory_input` (SensoryInput): Sensory data object (Vision, Audio, InternalSignals, etc.)

**Returns**:
- None

**Example**:
```python
from pynlm import SensoryInput
percept = world.getSensoryPercept()
brain.receiveSensoryInput(percept)
```

#### `brain.produceAction()`
```python
action = brain.produceAction()
```

**Description**: Generate motor action based on current neural activity. Decodes brain state into executable commands.

**Parameters**:
- None

**Returns**:
- `action` (Action): Motor action with type and parameters

**Example**:
```python
action = brain.produceAction()
world.applyMotorCommand(action, world.getSimulationTime())
```

### Neural Statistics

#### `brain.getTotalNeuronCount()`
```python
neuron_count = brain.getTotalNeuronCount()
```

**Description**: Get total number of neurons in the brain.

**Parameters**:
- None

**Returns**:
- `neuron_count` (int): Total neuron count

#### `brain.getTotalSynapseCount()`
```python
synapse_count = brain.getTotalSynapseCount()
```

**Description**: Get total number of synapses in the brain.

**Parameters**:
- None

**Returns**:
- `synapse_count` (int): Total synapse count

#### `brain.getFiringNeuronCount()`
```python
firing_count = brain.getFiringNeuronCount()
```

**Description**: Get count of currently firing neurons.

**Parameters**:
- None

**Returns**:
- `firing_count` (int): Number of firing neurons

#### `brain.getActiveNeuronCount()`
```python
active_count = brain.getActiveNeuronCount()
```

**Description**: Get count of active neurons (including those in refractory period).

**Parameters**:
- None

**Returns**:
- `active_count` (int): Number of active neurons

#### `brain.getAverageFiringRate()`
```python
firing_rate = brain.getAverageFiringRate()
```

**Description**: Get average firing rate across all neurons.

**Parameters**:
- None

**Returns**:
- `firing_rate` (float): Average firing rate in Hz

#### `brain.getExcitationInhibitionRatio()`
```python
ei_ratio = brain.getExcitationInhibitionRatio()
```

**Description**: Get excitation/inhibition balance ratio.

**Parameters**:
- None

**Returns**:
- `ei_ratio` (float): E/I balance ratio

#### `brain.getTotalSpikeCount()`
```python
spike_count = brain.getTotalSpikeCount()
```

**Description**: Get total number of spikes generated by all neurons.

**Parameters**:
- None

**Returns**:
- `spike_count` (int): Total spike count

#### `brain.getFiringNeuronCount()`
```python
firing_count = brain.getFiringNeuronCount()
```

**Description**: Get count of neurons currently firing (synapsing).

**Parameters**:
- None

**Returns**:
- `firing_count` (int): Number of neurons firing

#### `brain.getAverageFiringRate()`
```python
firing_rate = brain.getAverageFiringRate()
```

**Description**: Get average firing rate across all neurons.

**Parameters**:
- None

**Returns**:
- `firing_rate` (float): Average firing rate in Hz

#### `brain.getExcitationInhibitionRatio()`
```python
ei_ratio = brain.getExcitationInhibitionRatio()
```

**Description**: Get excitation/inhibition balance ratio.

**Parameters**:
- None

**Returns**:
- `ei_ratio` (float): E/I balance ratio

#### `brain.getTotalSpikeCount()`
```python
spike_count = brain.getTotalSpikeCount()
```

**Description**: Get total number of spikes generated by all neurons.

**Parameters**:
- None

**Returns**:
- `spike_count` (int): Total spike count

### Developmental Control

#### `brain.getDevelopmentalStage()`
```python
stage = brain.getDevelopmentalStage()
```

**Description**: Get current developmental stage of the brain.

**Parameters**:
- None

**Returns**:
- `stage` (DevelopmentalStage): Current developmental stage

#### `brain.setDevelopmentalStage(stage)`
```python
brain.setDevelopmentalStage(stage)
```

**Description**: Set developmental stage manually (overrides natural progression).

**Parameters**:
- `stage` (DevelopmentalStage): Target developmental stage

**Returns**:
- None

#### `brain.updateDevelopment(timestep)`
```python
brain.updateDevelopment(timestep)
```

**Description**: Advance developmental stage based on elapsed time.

**Parameters**:
- `timestep` (float): Time elapsed since last update in milliseconds

**Returns**:
- None

**Example**:
```python
brain.updateDevelopment(100.0)  # Advance by 100ms
```

### Memory Systems

#### `brain.getWorkingMemory()`
```python
memory = brain.getWorkingMemory()
```

**Description**: Get working memory object for persistent neural activity patterns.

**Parameters**:
- None

**Returns**:
- `memory` (WorkingMemory): Working memory object, or None if not implemented

#### `brain.getEpisodicMemory()`
```python
memory = brain.getEpisodicMemory()
```

**Description**: Get episodic memory system for storing experience sequences.

**Parameters**:
- None

**Returns**:
- `memory` (EpisodicMemory): Episodic memory object, or None if not implemented

#### `brain.getSemanticMemory()`
```python
memory = brain.getSemanticMemory()
```

**Description**: Get semantic memory system for storing abstract knowledge.

**Parameters**:
- None

**Returns**:
- `memory` (SemanticMemory): Semantic memory object, or None if not implemented

#### `brain.getPredictionSystem()`
```python
prediction = brain.getPredictionSystem()
```

**Description**: Get prediction system for forward modeling and expectation generation.

**Parameters**:
- None

**Returns**:
- `prediction` (PredictionSystem): Prediction system object, or None if not implemented

### State Management

#### `brain.save(filepath)`
```python
brain.save(filepath)
```

**Description**: Save current brain state to file. Includes neural activity, synaptic weights, and internal state.

**Parameters**:
- `filepath` (str): Path to save file

**Returns**:
- `success` (bool): True if save successful, False otherwise

**Example**:
```python
saved = brain.save("checkpoint.brain")
if saved:
    print("Brain state saved successfully")
```

#### `brain.load(filepath)`
```python
brain.load(filepath)
```

**Description**: Load brain state from file. Restores neural activity, synaptic weights, and internal state.

**Parameters**:
- `filepath` (str): Path to load file

**Returns**:
- `success` (bool): True if load successful, False otherwise

**Example**:
```python
loaded = brain.load("checkpoint.brain")
if loaded:
    print("Brain state loaded successfully")
```

### Neural Region Management

#### `brain.addRegion(name="")`
```python
region_id = brain.addRegion(name="")
```

**Description**: Add a new neural region to the brain.

**Parameters**:
- `name` (str): Optional region name (default: "")

**Returns**:
- `region_id` (RegionId): Unique identifier for the new region

#### `brain.getRegion(region_id)`
```python
region = brain.getRegion(region_id)
```

**Description**: Get neural region by its ID.

**Parameters**:
- `region_id` (RegionId): Region identifier

**Returns**:
- `region` (NeuralRegion): Neural region object

#### `brain.getRegionCount()`
```python
count = brain.getRegionCount()
```

**Description**: Get number of neural regions in the brain.

**Parameters**:
- None

**Returns**:
- `count` (int): Number of regions

#### `brain.getRegionIds()`
```python
region_ids = brain.getRegionIds()
```

**Description**: Get all region IDs in the brain.

**Parameters**:
- None

**Returns**:
- `region_ids` (list[RegionId]): List of all region IDs

#### `brain.getRegions()`
```python
regions = brain.getRegions()
```

**Description**: Get all neural regions in the brain.

**Parameters**:
- None

**Returns**:
- `regions` (list[NeuralRegion]): List of all neural regions

### System Configuration

#### `brain.getConfig()`
```python
config = brain.getConfig()
```

**Description**: Get the configuration object associated with this brain.

**Parameters**:
- None

**Returns**:
- `config` (Config): Configuration object

### Monitoring and Logging

#### `brain.logStatus()`
```python
brain.logStatus()
```

**Description**: Log current brain status and statistics. Useful for debugging and monitoring.

**Parameters**:
- None

**Returns**:
- None

### Advanced Neural Manipulation

#### `brain.injectCurrent(neuron_id, current)`
```python
brain.injectCurrent(neuron_id, current)
```

**Description**: Inject direct current into a specific neuron. Useful for testing and controlling neural activity.

**Parameters**:
- `neuron_id` (NeuronId): Target neuron identifier
- `current` (float): Current to inject (positive/negative)

**Returns**:
- None

#### `brain.injectCurrentToNeurons(neuron_type, current)`
```python
brain.injectCurrentToNeurons(neuron_type, current)
```

**Description**: Inject current into all neurons of a specific type. Useful for stimulating neural populations.

**Parameters**:
- `neuron_type` (NeuronType): Type of neurons to target
- `current` (float): Current to inject

**Returns**:
- None

### Debug and Analysis

#### `brain.get_neuron_activity(step_range=None)`
```python
activity = brain.get_neuron_activity(step_range=None)
```

**Description**: Get neural activity patterns over time. Returns firing rates or spike counts.

**Parameters**:
- `step_range` (tuple[int, int]): Optional range of steps to analyze

**Returns**:
- `activity` (list[float]): Neural activity data

#### `brain.get_spike_trains(neuron_id=None)`
```python
spikes = brain.get_spike_trains(neuron_id=None)
```

**Description**: Get spike train data for specific neurons or the whole brain.

**Parameters**:
- `neuron_id` (NeuronId): Optional specific neuron ID

**Returns**:
- `spikes` (list[list[float]]): Spike times for each neuron

## Configuration Integration

### Brain Configuration Parameters

The Brain class uses configuration parameters set through the Config object:

#### Neural Parameters
- `brain.neuron_count`: Total number of neurons (default: 1000)
- `brain.synapse_density`: Synapse connectivity density (default: 0.1)
- `brain.connection_probability`: Probability of connection between neurons (default: 0.05)
- `brain.v_thresh`: Neuron threshold potential (mV, default: -50.0)
- `brain.v_rest`: Resting potential (mV, default: -70.0)
- `brain.v_reset`: Reset potential after spike (mV, default: -75.0)
- `brain.tau_mem`: Membrane time constant (ms, default: 20.0)
- `brain.tau_ref`: Refractory period (ms, default: 2.0)

#### Plasticity Parameters
```
plasticity.stdp.enable: Enable STDP (default: true)
plasticity.stdp.learning_rate: STDP learning rate (default: 0.001)
plasticity.stdp.tau_plus: STDP time constant plus (ms, default: 20.0)
plasticity.stdp.tau_minus: STDP time constant minus (ms, default: 20.0)

plasticity.hebbian.enable: Enable Hebbian learning (default: true)
plasticity.hebbian.learning_rate: Hebbian learning rate (default: 0.0005)

plasticity.structural.enable: Enable structural plasticity (default: true)
plasticity.structural.synapse_creation_rate: Rate of new synapse creation (default: 0.001)
plasticity.structural.synapse_removal_rate: Rate of synapse removal (default: 0.001)
```

#### Neuromodulation Parameters
```
neuromod.dopamine.scale: Dopamine modulation scale (default: 1.0)
neuromod.curiosity.enable: Enable curiosity-driven exploration (default: true)
neuromod.novelty.enable: Enable novelty detection (default: true)
neuromod.prediction_error.enable: Enable prediction error signaling (default: true)
```

#### Development Parameters
```
development.initial_plasticity_rate: Initial plasticity rate (default: 1.0)
development.critical_period_duration: Critical period duration (ms, default: 1000.0)
development.maturation_rate: Maturation rate (default: 0.001)
```

## Error Handling

The Brain class raises the following exceptions:

### Configuration Errors
- **ConfigurationError**: Invalid or missing configuration parameters

### Simulation Errors
- **SimulationError**: Errors during simulation execution
- **RuntimeError**: General runtime errors during neural computation

### Memory Errors
- **RuntimeError**: Memory system not properly initialized or configured

## Performance Considerations

### Memory Usage
- Each neuron requires approximately 100-200 bytes
- Synapse storage scales with `neuron_count * synapse_density`
- Working memory additional overhead: O(neuron_count)

### Computational Complexity
- **Single step**: O(neuron_count + synapse_count)
- **STDP updates**: O(synapse_count)
- **Spike propagation**: Depends on network connectivity

### Optimization Strategies
1. **Adjust neuron count**: Fewer neurons = faster simulation
2. **Configure sparsity**: Reduce synapse density for faster computation
3. **Limit plasticity**: Disable structural plasticity for speed
4. **Use release build**: Optimized compilation with compiler optimizations

## Compatibility Notes

### Forward Compatibility
- Method signatures are stable across minor versions
- New features added as optional parameters
- Default behavior preserved unless explicitly configured

### Backward Compatibility
- Removed methods: `getWorkingMemory()`, `getEpisodicMemory()` 
  (returning None indicates disconnection)
- Changed behavior: Save/load operations now return success flags
- Updated parameter names: Some configuration keys renamed for consistency

### Platform Considerations
- Cross-platform compatible (Windows, Linux, macOS)
- Python bindings rely on pybind11 standard library
- No external dependencies beyond C++ standard library

## Example Usage Patterns

### Basic Simulation
```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Get final statistics
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

### Advanced Simulation with State Management
```python
def run_advanced_simulation(steps, save_path=None, load_path=None):
    """Advanced simulation with state management"""
    
    # Load existing state if available
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if load_path and os.path.exists(load_path):
        if brain.load(load_path):
            print(f"Loaded brain state from {load_path}")
        else:
            print(f"Failed to load {load_path}, initializing new brain")
            brain.initialize()
    else:
        brain.initialize()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        
        # Optional: Log progress
        if step % 100 == 0:
            brain.logStatus()
        
        # Optional: Save checkpoint
        if step % 500 == 0 and save_path:
            brain.save(f"{save_path}_step_{step}.brain")
    
    # Save final state
    if save_path:
        brain.save(f"{save_path}_final.brain")
    
    return brain
```

## Future Extensions

### Planned Enhancements
1. **Real-time spike monitoring**: Access to individual neuron spike times
2. **Custom neural types**: User-defined neuron models and properties
3. **Advanced plasticity rules**: Custom learning rules beyond STDP/Hebbian
4. **Multi-scale simulation**: Hierarchical neural models
5. **Hardware acceleration**: GPU support for large-scale simulations

### Research Applications
- **Studying neural development**: Tracking changes over time
- **Testing plasticity mechanisms**: Evaluating different learning rules
- **Modeling cognitive functions**: Simulating perception, memory, decision-making
- **Neuroscientific validation**: Comparing with biological data

## Conclusion

The Brain class provides a comprehensive neural simulation framework combining biological plausibility with computational efficiency. Its event-driven architecture, multiple plasticity mechanisms, and developmental systems make it suitable for a wide range of applications from basic education to advanced research.

For more information, see:
- `docs/USAGE/BASIC.md` - Basic usage examples
- `docs/USAGE/INTERMEDIATE.md` - Intermediate usage patterns
- `docs/USAGE/ADVANCED.md` - Advanced features and customization
- `examples/` - Complete working examples
