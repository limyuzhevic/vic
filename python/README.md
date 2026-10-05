# NLM Python Bindings - Comprehensive API Documentation

## Overview

The NLM Python bindings provide a complete Python interface to the neural simulation framework, allowing users to create and simulate neural networks with advanced Python features and comprehensive API coverage.

## Installation

### Using pip (with pre-built wheels)
```bash
pip install pynlm
```

### Building from source
```bash
# Clone the repository
cd nlm-project

# Build and install
pip install -e .
```

### Dependencies
- C++ compiler with C++20 support
- scikit-build-core
- pybind11
- Python 3.8+

## Quick Start

### Basic Neural Network Example

```python
from pynlm import Neuron, Synapse, NeuralRegion, NeuralPopulation
from pynlm import NeuronType, SynapseType

# Create neural region
with NeuralRegion(1, "hippocampus") as region:
    # Create neurons
    neuron1 = Neuron(1)
    neuron1.type = NeuronType.Excitatory
    neuron1.V = -65.0
    neuron1.threshold = -55.0
    
    neuron2 = Neuron(2)
    neuron2.type = NeuronType.Inhibitory
    neuron2.V = -70.0
    
    # Create synapse
    synapse = region.add_synapse(neuron1, neuron2, weight=0.5)
    
    # Simulate activity
    for t in range(100):
        neuron1.stepLIF(t * 0.001, 0.001)
        neuron2.stepLIF(t * 0.001, 0.001)
        region.step(t * 0.001)
```

### Population Dynamics Example

```python
from pynlm import NeuralRegion, NeuralPopulation, NeuronType

# Create population with 1000 neurons
with NeuralRegion(2, "cortex") as region:
    population = region.addPopulation(1000, NeuronType.Excitatory)
    
    # Simulate and track activity
    for t in range(500):
        population.step(t * 0.001)
        
        # Access statistics
        print(f"Step {t}: Active {population.active_count}, Firing {population.firing_count}")
```

### Neuromodulation Example

```python
from pynlm import Dopamine, Acetylcholine, Norepinephrine, Serotonin

# Create neuromodulators
dopamine = Dopamine()
acetylcholine = Acetylcholine()
norepinephrine = Norepinephrine()
serotonin = Serotonin()

# Apply reward signal
dopamine.signal_reward(1.0)

# Set neuromodulation levels
acetylcholine.set_level(0.7)
norepinephrine.set_level(0.5)
serotonin.set_level(0.3)
```

## Core Classes

### Neuron Class

The `Neuron` class represents a single LIF (Leaky Integrate-and-Fire) neuron.

#### Properties

- `id`: Neuron identifier
- `type`: Neuron type (excitatory, inhibitory, etc.)
- `V`: Membrane potential (mV)
- `threshold`: Firing threshold (mV)
- `resting_potential`: Resting potential (mV)
- `reset_potential`: Reset potential (mV)
- `leak_conductance`: Leak conductance (nS)
- `refractory_period`: Refractory period (steps)
- `firing_rate`: Firing rate (Hz)
- `firing_state`: Current firing state

#### Methods

- `get_id()`: Get neuron identifier
- `get_type()`: Get neuron type
- `set_type(type)`: Set neuron type
- `getMembranePotential()`: Get membrane potential
- `setMembranePotential(potential)`: Set membrane potential
- `addToMembranePotential(delta)`: Add to membrane potential
- `checkThreshold()`: Check if neuron has reached firing threshold
- `stepLIF(current_time, dt)`: Step LIF dynamics, returns True if fired
- `step(current_time)`: Step neuron with default timestep
- `recordSpike(timestamp)`: Record spike at timestamp
- `getSpikeHistory()`: Get spike history vector
- `getSpikeCount()`: Get number of recorded spikes
- `clearSpikeHistory()`: Clear spike history
- `addIncomingSynapse(handle)`: Add incoming synapse
- `addOutgoingSynapse(handle)`: Add outgoing synapse
- `getIncomingSynapses()`: Get incoming synapse handles
- `getOutgoingSynapses()`: Get outgoing synapse handles
- `enablePlasticity(hebbian, stdp, reward_modulated)`: Enable plasticity
- `setRegionId(region)`: Set region ID
- `getRegionId()`: Get region ID
- `setPopulationId(population)`: Set population ID
- `getPopulationId()`: Get population ID
- `getState()`: Get neuron state (mutable)
- `reset()`: Reset neuron to initial state
- `initializeRandom(rng)`: Initialize with random parameters

#### Pythonic Features

```python
# Property-based access
neuron.V = -65.0
neuron.threshold = -55.0
neuron.refractory_period = 10

# Pythonic operators
potential = neuron["V"]  # Get property
neuron["V"] = -65.0     # Set property

# Vector conversion
vector = neuron.to_vector()  # [V, threshold, firing_rate]

# Dictionary representation
dict_repr = neuron.as_dict()

# Iteration
for spike in neuron.spike_history:
    print(f"Spike at {spike}ms")
```

### Synapse Class

The `Synapse` class represents a connection between neurons with plasticity.

#### Properties

- `id`: Synapse identifier
- `source`: Source neuron ID
- `destination`: Destination neuron ID
- `weight`: Synaptic weight
- `delay`: Synaptic delay (steps)
- `type`: Synapse type
- `eligibility_trace`: Eligibility trace for reward-modulated learning
- `efficacy`: Synaptic efficacy

#### Methods

- `getId()`: Get synapse identifier
- `getSourceNeuron()`: Get source neuron ID
- `getDestinationNeuron()`: Get destination neuron ID
- `getWeight()`: Get synaptic weight
- `setWeight(weight)`: Set synaptic weight
- `addToWeight(delta)`: Add to synaptic weight
- `getDelay()`: Get synaptic delay
- `setDelay(delay)`: Set synaptic delay
- `getType()`: Get synapse type
- `setType(type)`: Set synapse type
- `isExcitatory()`: Check if excitatory
- `isInhibitory()`: Check if inhibitory
- `recordPreSpike(timestamp)`: Record pre-synaptic spike
- `recordPostSpike(timestamp)`: Record post-synaptic spike
- `getPreSpikeHistory()`: Get pre-synaptic spike history
- `getPostSpikeHistory()`: Get post-synaptic spike history
- `getPreSpikeCount()`: Get pre-synaptic spike count
- `getPostSpikeCount()`: Get post-synaptic spike count
- `clearHistory()`: Clear spike history
- `getPlasticityFlags()`: Get plasticity flags
- `enablePlasticity(hebbian, stdp, reward_modulated)`: Enable plasticity
- `getEligibilityTrace()`: Get eligibility trace
- `setEligibilityTrace(trace)`: Set eligibility trace
- `decayEligibilityTrace(decay_rate)`: Decay eligibility trace
- `getEfficacy()`: Get synaptic efficacy
- `setEfficacy(efficacy)`: Set synaptic efficacy
- `step(current_time)`: Step synapse dynamics
- `reset()`: Reset synapse to initial state
- `initializeRandom(rng)`: Initialize with random parameters

#### Pythonic Features

```python
# Property-based access
synapse.weight = 0.5
synapse.delay = 2
synapse.type = SynapseType.Excitatory

# Pythonic operators
weight = synapse["weight"]
synapse["weight"] = 0.5

# History access
pre_spikes = synapse.getPreSpikeHistory()
post_spikes = synapse.getPostSpikeHistory()

# Batch operations
batch_step_synapses(synapses, current_time)
```

### NeuralRegion Class

The `NeuralRegion` class represents a brain region containing populations and local connectivity.

#### Properties

- `id`: Region identifier
- `name`: Region name

#### Methods

- `getId()`: Get region identifier
- `getName()`: Get region name
- `setName(name)`: Set region name
- `addPopulation(size, type)`: Add new population
- `getPopulation(id)`: Get population by ID
- `getPopulationCount()`: Get number of populations
- `getPopulations()`: Get all populations (read-only)
- `addSynapse(source, destination, weight, delay)`: Add synapse
- `getSynapse(id)`: Get synapse by ID
- `getSynapseCount()`: Get number of synapses
- `getSynapses()`: Get all synapses (read-only)
- `getSynapsesFrom(neuron)`: Get synapses originating from neuron
- `getSynapsesTo(neuron)`: Get synapses targeting neuron
- `getTotalNeuronCount()`: Get total neuron count
- `getActiveNeuronCount()`: Get active neuron count
- `getFiringNeuronCount()`: Get firing neuron count
- `getAverageFiringRate()`: Get average firing rate
- `getAverageSynapticWeight()`: Get average synaptic weight
- `getSynapticDensity()`: Get synaptic density
- `step(current_time)`: Step all neurons and synapses
- `reset()`: Reset all neurons and synapses
- `initializeRandomConnectivity(rng, connection_probability, mean_weight, weight_variance)`: Initialize random connectivity
- `getAllNeurons()`: Get all neurons across populations

#### Pythonic Features

```python
# Property-based access
region.name = "hippocampus"

# Pythonic operators
population_count = region["population_count"]
synapse_count = region["synapse_count"]

# Dictionary representation
region_info = region.as_dict()

# Context manager
with NeuralRegion(1, "my_region") as region:
    # Use region safely
    pass
```

### NeuralPopulation Class

The `NeuralPopulation` class represents a group of neurons with shared properties.

#### Properties

- `id`: Population identifier
- `size`: Population size
- `neuron_type`: Neuron type (all neurons share this)

#### Methods

- `getId()`: Get population identifier
- `getSize()`: Get population size
- `isEmpty()`: Check if population is empty
- `getNeuron(index)`: Get neuron by index
- `getNeuronIds()`: Get all neuron IDs
- `getNeuronType()`: Get neuron type
- `setNeuronType(type)`: Set neuron type for all neurons
- `getActiveCount()`: Get active neuron count
- `getFiringCount()`: Get firing neuron count
- `getAverageFiringRate()`: Get average firing rate
- `getActivityLevel()`: Get activity level (fraction active)
- `recordActivity()`: Record current activity state
- `getMeanMembranePotential()`: Get mean membrane potential
- `getVarianceMembranePotential()`: Get variance of membrane potentials
- `step(current_time)`: Step all neurons
- `reset()`: Reset all neurons
- `initializeRandom(rng)`: Initialize all neurons randomly

#### Pythonic Features

```python
# Iteration
for neuron in population:
    print(f"Neuron potential: {neuron.V}mV")

# Pythonic indexing
neuron = population[0]

# Dictionary representation
population_info = population.as_dict()

# Batch operations
population.step(current_time)
```

### Neuromodulator Classes

#### Neuromodulator (Base Class)

- `getName()`: Get modulator name
- `getLevel()`: Get current concentration/level
- `setLevel(level)`: Set concentration/level
- `getPlasticityFactor()`: Get plasticity factor
- `update(dt)`: Update neuromodulator state

#### Dopamine

- `signalReward(reward)`: Signal reward
- `signalRewardPredictionError(error)`: Signal reward prediction error

#### Acetylcholine, Norepinephrine, Serotonin

These classes have default implementations with minimal plasticity effects.

## Advanced Python Features

### Context Managers

```python
# Automatic resource management
with NeuralRegion(1, "my_region") as region:
    # Use region safely
    neuron = region.add_neuron()
    # Resources cleaned up automatically
```

### Pythonic Operators

```python
# Property-based access using dictionary-like syntax
neuron["V"] = -65.0  # Set membrane potential
neuron["threshold"] = -55.0  # Set threshold

# Iteration over population neurons
for neuron in population:
    print(f"Neuron {neuron.id} potential: {neuron.V}mV")

# Length for populations
print(f"Population size: {len(population)}")
```

### Property-Based Access

All key attributes are accessible as properties:

```python
# Read-write properties
neuron.V = -65.0
neuron.threshold = -55.0
neuron.refractory_period = 10
neuron.firing_rate = 50.0

# Read-only properties
potential = neuron.V
firing_state = neuron.firing_state
spike_count = neuron.spike_count
```

### Vector Conversions

Convert neural properties to standard Python containers:

```python
# Neuron to vector
vector = neuron.to_vector()  # [V, threshold, firing_rate]

# Synapse to vector
vector = synapse.to_vector()  # [weight, delay, efficacy]

# From array to list
values = from_array([1.0, 2.0, 3.0])  # [1.0, 2.0, 3.0]
```

### Batch Operations

Process multiple neurons and synapses efficiently:

```python
# Step multiple neurons
batch_step_neurons(neurons, current_time)

# Get membrane potentials for multiple neurons
potentials = batch_get_potential(neurons)

# Get spike counts for multiple neurons
spike_counts = batch_get_spike_counts(neurons)

# Step multiple synapses
batch_step_synapses(synapses, current_time)

# Simulate batch
simulate_batch(neurons, synapses, current_time)
```

### Statistics Utilities

Calculate statistical measures:

```python
from pynlm import calculate_mean, calculate_variance, calculate_stddev

# Calculate statistics
values = [1.0, 2.0, 3.0, 4.0, 5.0]
mean = calculate_mean(values)
variance = calculate_variance(values)
stddev = calculate_stddev(values)

print(f"Mean: {mean}, Variance: {variance}, Stddev: {stddev}")
```

## Convenience Factory Functions

### Creation Functions

```python
from pynlm import create_neuron, create_synapse, create_region, create_population

# Create neural components
neuron = create_neuron(1, NeuronType.Excitatory)
synapse = create_synapse(1, neuron1, neuron2, weight=0.5)
region = create_region(1, "hippocampus")
population = create_population(1, 100, NeuronType.Excitatory)
```

## Type Definitions

### Neuron Types

- `NeuronType.Excitatory`: Excitatory neuron
- `NeuronType.Inhibitory`: Inhibitory neuron
- `NeuronType.Modulatory`: Modulatory neuron
- `NeuronType.Sensory`: Sensory neuron
- `NeuronType.Motor`: Motor neuron
- `NeuronType.Internal`: Internal neuron

### Synapse Types

- `SynapseType.Excitatory`: Excitatory synapse
- `SynapseType.Inhibitory`: Inhibitory synapse
- `SynapseType.Modulatory`: Modulatory synapse
- `SynapseType.Electrical`: Electrical synapse
- `SynapseType.GapJunction`: Gap junction

### Firing States

- `FiringState.Resting`: Resting state
- `FiringState.Active`: Active state
- `FiringState.Refractory`: Refractory state
- `FiringState.Inhibited`: Inhibited state

### Developmental Stages

- `DevelopmentalStage.Initial`: Initial stage
- `DevelopmentalStage.CriticalPeriod`: Critical period
- `DevelopmentalStage.Maturation`: Maturation stage
- `DevelopmentalStage.Adult`: Adult stage
- `DevelopmentalStage.Aging`: Aging stage

### ID Types

- `NeuronId`: Unique identifier for a neuron
- `SynapseId`: Unique identifier for a synapse
- `RegionId`: Unique identifier for a brain region
- `PopulationId`: Unique identifier for a neuron population

## Constants

The following constants are available:

```python
from pynlm import INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID

# Invalid ID sentinels
invalid_neuron_id = INVALID_NEURON_ID
invalid_synapse_id = INVALID_SYNAPSE_ID
invalid_region_id = INVALID_REGION_ID
invalid_population_id = INVALID_POPULATION_ID
```

## Enum Values

```python
from pynlm import (
    NeuronType, SynapseType, FiringState, DevelopmentalStage,
    NEURON_TYPE_EXCITATORY, NEURON_TYPE_INHIBITORY,
    NEURON_TYPE_MODULATORY, NEURON_TYPE_SENSORY,
    NEURON_TYPE_MOTOR, NEURON_TYPE_INTERNAL,
    SYNAPSE_TYPE_EXCITATORY, SYNAPSE_TYPE_INHIBITORY,
    SYNAPSE_TYPE_MODULATORY, SYNAPSE_TYPE_ELECTRICAL,
    SYNAPSE_TYPE_GAP_JUNCTION,
    FIRING_STATE_RESTING, FIRING_STATE_ACTIVE,
    FIRING_STATE_REFRACTORY, FIRING_STATE_INHIBITED,
    DEVELOPMENTAL_STAGE_INITIAL, DEVELOPMENTAL_STAGE_CRITICAL_PERIOD,
    DEVELOPMENTAL_STAGE_MATURATION, DEVELOPMENTAL_STAGE_ADULT,
    DEVELOPMENTAL_STAGE_AGING
)
```

## Module Information

### Version

```python
from pynlm import __version__
print(f"NLM Python Bindings v{__version__}")
```

### Available Features

```python
from pynlm import features
for i, feature in enumerate(features(), 1):
    print(f"{i}. {feature}")
```

### Example Scripts

The following example scripts are included:

1. `basic_example.py` - Basic neural simulation
2. `neuromodulator_example.py` - Neuromodulation demonstration
3. `population_example.py` - Population dynamics
4. `batch_operations_example.py` - Batch operations

```python
from pynlm import examples
for i, example in enumerate(examples(), 1):
    print(f"{i}. {example}")
```

## Error Handling

The bindings include comprehensive error handling:

```python
import pybind11.exceptions

# Catch binding errors
try:
    neuron = Neuron(1)
    neuron.V = -65.0
except pybind11.exceptions.runtime_error as e:
    print(f"Error: {e}")
```

## Performance Considerations

### Large-Scale Simulation

For simulations with many neurons or long durations:

1. **Use batch operations**: `batch_step_neurons()`, `batch_step_synapses()`
2. **Monitor memory usage**: Large populations consume significant memory
3. **Use checkpointing**: Save/load brain state periodically
4. **Optimize timestep**: Use appropriate timestep for your simulation

### Memory Management

```python
# Clear spike history regularly to save memory
neuron.clearSpikeHistory()

# Reset populations when no longer needed
population.reset()
```

## Migration Guide

### From C++ to Python

If you're migrating from the C++ API:

#### Neuron

C++:
```cpp
Neuron neuron(1);
neuron.setMembranePotential(-65.0f);
neuron.setThreshold(-55.0f);
```

Python:
```python
neuron = Neuron(1)
neuron.V = -65.0
neuron.threshold = -55.0
```

#### Synapse

C++:
```cpp
Synapse synapse(1, source, destination);
synapse.setWeight(0.5f);
```

Python:
```python
synapse = Synapse(1, source, destination)
synapse.weight = 0.5
```

#### NeuralRegion

C++:
```cpp
NeuralRegion region(1);
PopulationId popId = region.addPopulation(100);
```

Python:
```python
region = NeuralRegion(1)
population = region.addPopulation(100)
```

## Troubleshooting

### Common Issues

#### ModuleNotFoundError
If you get "ModuleNotFoundError: No module named 'pynlm'":

```bash
# Install in development mode
pip install -e .

# Or install globally
pip install pynlm
```

#### ImportError
If you get "ImportError":

```bash
# Check if dependencies are installed
pip install scikit-build-core pybind11

# Rebuild if needed
pip install --no-deps -e .
```

#### Build Errors
If you encounter build errors:

1. Ensure you have a C++ compiler with C++20 support
2. Install dependencies: `pip install scikit-build-core pybind11`
3. Check for required headers in include paths
4. Ensure pybind11 is installed: `pip install pybind11`

### Performance Issues

#### Slow Performance
```python
# Use batch operations instead of individual calls
batch_step_neurons(neurons, current_time)

# Reduce simulation timestep if possible
current_time += dt  # Use smaller dt
```

#### Memory Issues
```python
# Clear spike history regularly
neuron.clearSpikeHistory()

# Reset populations when no longer needed
population.reset()
```

## Future Enhancements

The Python bindings will be enhanced with:

1. **Advanced Visualization**: Plotting neural activity
2. **Parallel Processing**: Multi-core simulation capabilities
3. **Machine Learning Integration**: TensorFlow/PyTorch interoperability
4. **Web API**: REST interface for remote access
5. **GUI Tools**: Interactive visualization and control
6. **Distributed Computing**: Multi-machine simulation support

## License

MIT License

## Authors

NLM Research Team

## Contact

For questions, issues, or suggestions:
- File issues on GitHub
- Check documentation for technical details
- Visit the NLM project website

---

*Generated with comprehensive Python bindings for NLM neural components.*
*Version 0.1.0 - Phase 6 Integration Complete*