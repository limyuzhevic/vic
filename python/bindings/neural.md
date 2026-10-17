# Neural Module - Core Neural Components

This module provides Python bindings for core neural components including neurons, synapses, and neural regions.
It offers a Pythonic interface to the C++ neural simulation framework with comprehensive error handling
and type safety.

## Overview

The Neural module contains the following key components:

1. **Neuron**: Single neuron with LIF dynamics
2. **Synapse**: Connection between neurons with plasticity
3. **NeuralRegion**: Collection of neurons and synapses
4. **NeuralPopulation**: Group of neurons with same type
5. **Type definitions**: All type enums and identifiers

## Basic Usage

```python
import nlm

# Create neurons
neuron1 = nlm.Neuron(nlm.create_neuron_id(1))
neuron2 = nlm.Neuron(nlm.create_neuron_id(2))

# Set neuron properties
neuron1.set_type(nlm.NeuronType.Excitatory)
neuron1.set_resting_potential(-70.0)
neuron1.set_threshold(-55.0)

# Create synapses
# Note: In practice, synapses are created by the brain system
# This is a simplified example

# Create a neural region
region_id = brain.add_region("Cortex")
region = brain.get_region(region_id)

# Add neurons to region
# region.add_neuron(neuron1)
# region.add_neuron(neuron2)

# Create a population
# population_id = region.add_population(10, nlm.NeuronType.Excitatory)
# population = region.get_population(population_id)
```

## Key Classes

### Neuron

The Neuron class represents a single biological neuron with Leaky Integrate-and-Fire dynamics.

**Attributes:**
- `id`: Unique identifier for the neuron
- `type`: Neuron type (Excitatory, Inhibitory, etc.)
- `membrane_potential`: Current membrane potential (mV)
- `threshold`: Firing threshold (mV)
- `firing_state`: Current firing state (Resting, Active, Refractory)

**Methods:**

#### `__init__(self, neuron_id)`
Create a new neuron with the specified ID.

```python
neuron = nlm.Neuron(nlm.create_neuron_id(1))
```

#### `set_type(self, neuron_type)`
Set the neuron type.

```python
neuron.set_type(nlm.NeuronType.Excitatory)
```

#### `set_membrane_potential(self, potential)`
Set the membrane potential.

```python
neuron.set_membrane_potential(-65.0)
```

#### `add_to_membrane_potential(self, delta)`
Add to the membrane potential.

```python
neuron.add_to_membrane_potential(5.0)
```

#### `set_threshold(self, threshold)`
Set the firing threshold.

```python
neuron.set_threshold(-55.0)
```

#### `set_firing_state(self, state)`
Set the firing state.

```python
neuron.set_firing_state(nlm.FiringState.Active)
```

#### `set_refractory_period(self, steps)`
Set the refractory period in simulation steps.

```python
neuron.set_refractory_period(10)
```

#### `decrement_refractory(self)`
Decrement the refractory period by one step.

```python
neuron.decrement_refractory()
```

#### `is_firing(self) -> bool`
Check if the neuron is currently firing.

```python
if neuron.is_firing():
    print("Neuron is firing!")
```

#### `is_refractory(self) -> bool`
Check if the neuron is in refractory period.

```python
if neuron.is_refractory():
    print("Neuron is in refractory period!")
```

#### `get_membrane_potential(self) -> float`
Get the current membrane potential.

```python
potential = neuron.get_membrane_potential()
print(f"Membrane potential: {potential} mV")
```

#### `get_threshold(self) -> float`
Get the firing threshold.

```python
threshold = neuron.get_threshold()
print(f"Threshold: {threshold} mV")
```

#### `get_last_spike_time(self) -> float`
Get the time of the last spike.

```python
last_spike = neuron.get_last_spike_time()
if last_spike >= 0:
    print(f"Last spike at {last_spike} ms")
```

#### `check_threshold(self) -> bool`
Check if the neuron has reached firing threshold.

```python
if neuron.check_threshold():
    print("Neuron reached threshold!")
```

#### `inject_current(self, current)`
Inject current into the neuron.

```python
neuron.inject_current(5.0)
```

#### `receive_excitatory_input(self, amplitude)`
Receive excitatory synaptic input.

```python
neuron.receive_excitatory_input(2.5)
```

#### `receive_inhibitory_input(self, amplitude)`
Receive inhibitory synaptic input.

```python
neuron.receive_inhibitory_input(1.5)
```

#### `record_spike(self, timestamp)`
Record a spike at the given timestamp.

```python
neuron.record_spike(10.5)
```

#### `get_spike_history(self) -> List[float]`
Get the history of spike timestamps.

```python
history = neuron.get_spike_history()
print(f"Spike history: {history}")
```

#### `clear_spike_history(self)`
Clear the spike history.

```python
neuron.clear_spike_history()
```

#### `step(self, current_time)`
Update the neuron for one simulation step.

```python
neuron.step(current_time)
```

#### `reset(self)`
Reset the neuron to its initial state.

```python
neuron.reset()
```

#### `initialize_random(self, random_generator)`
Initialize the neuron with random parameters.

```python
rng = nlm.RandomGenerator(42)
neuron.initialize_random(rng)
```

### Synapse

The Synapse class represents a connection between two neurons with synaptic transmission and plasticity.

**Attributes:**
- `id`: Unique identifier for the synapse
- `source_neuron`: Source neuron ID
- `destination_neuron`: Destination neuron ID
- `weight`: Synaptic weight
- `type`: Synapse type (Excitatory, Inhibitory, etc.)

**Methods:**

#### `__init__(self, synapse_id, source_neuron_id, destination_neuron_id)`
Create a new synapse.

```python
synapse = nlm.Synapse(nlm.create_synapse_id(1), source_id, dest_id)
```

#### `set_weight(self, weight)`
Set the synaptic weight.

```python
synapse.set_weight(0.5)
```

#### `add_to_weight(self, delta)`
Add to the synaptic weight.

```python
synapse.add_to_weight(0.1)
```

#### `get_weight(self) -> float`
Get the synaptic weight.

```python
weight = synapse.get_weight()
print(f"Synaptic weight: {weight}")
```

#### `set_type(self, synapse_type)`
Set the synapse type.

```python
synapse.set_type(nlm.SynapseType.Excitatory)
```

#### `get_type(self) -> SynapseType`
Get the synapse type.

```python
syn_type = synapse.get_type()
```

#### `is_excitatory(self) -> bool`
Check if the synapse is excitatory.

```python
if synapse.is_excitatory():
    print("This is an excitatory synapse")
```

#### `is_inhibitory(self) -> bool`
Check if the synapse is inhibitory.

```python
if synapse.is_inhibitory():
    print("This is an inhibitory synapse")
```

#### `record_pre_spike(self, timestamp)`
Record a pre-synaptic spike.

```python
synapse.record_pre_spike(10.0)
```

#### `record_post_spike(self, timestamp)`
Record a post-synaptic spike.

```python
synapse.record_post_spike(10.5)
```

#### `get_pre_spike_history(self) -> List[float]`
Get pre-synaptic spike history.

```python
pre_history = synapse.get_pre_spike_history()
```

#### `get_post_spike_history(self) -> List[float]`
Get post-synaptic spike history.

```python
post_history = synapse.get_post_spike_history()
```

#### `clear_history(self)`
Clear all spike history.

```python
synapse.clear_history()
```

#### `get_eligibility_trace(self) -> float`
Get the eligibility trace for reward-modulated learning.

```python
trace = synapse.get_eligibility_trace()
```

#### `set_eligibility_trace(self, trace)`
Set the eligibility trace.

```python
synapse.set_eligibility_trace(0.5)
```

#### `decay_eligibility_trace(self, decay_rate)`
Decay the eligibility trace.

```python
synapse.decay_eligibility_trace(0.01)
```

#### `get_efficacy(self) -> float`
Get the synaptic efficacy.

```python
efficacy = synapse.get_efficacy()
```

#### `set_efficacy(self, efficacy)`
Set the synaptic efficacy.

```python
synapse.set_efficacy(1.2)
```

#### `step(self, current_time)`
Update the synapse for one simulation step.

```python
synapse.step(current_time)
```

#### `reset(self)`
Reset the synapse to its initial state.

```python
synapse.reset()
```

#### `initialize_random(self, random_generator)`
Initialize the synapse with random parameters.

```python
synapse.initialize_random(rng)
```

### NeuralRegion

The NeuralRegion class represents a collection of neurons and their connections.

**Attributes:**
- `id`: Unique identifier for the region
- `name`: Name of the region
- `neuron_count`: Number of neurons in the region
- `synapse_count`: Number of synapses in the region

**Methods:**

#### `__init__(self, region_id, name)`
Create a new neural region.

```python
region = nlm.NeuralRegion(nlm.create_region_id(1), "Cortex")
```

#### `add_neuron(self, neuron)`
Add a neuron to the region.

```python
region.add_neuron(neuron)
```

#### `get_neuron(self, neuron_id)`
Get a neuron by its ID.

```python
neuron = region.get_neuron(neuron_id)
```

#### `get_all_neurons(self) -> List[Neuron]`
Get all neurons in the region.

```python
neurons = region.get_all_neurons()
```

#### `add_synapse(self, synapse)`
Add a synapse to the region.

```python
region.add_synapse(synapse)
```

#### `get_synapse(self, synapse_id)`
Get a synapse by its ID.

```python
synapse = region.get_synapse(synapse_id)
```

#### `get_all_synapses(self) -> List[Synapse]`
Get all synapses in the region.

```python
synapses = region.get_all_synapses()
```

#### `get_synapses_to(self, neuron_id) -> List[Synapse]`
Get all synapses targeting a specific neuron.

```python
synapses = region.get_synapses_to(target_neuron_id)
```

#### `get_synapses_from(self, neuron_id) -> List[Synapse]`
Get all synapses originating from a specific neuron.

```python
synapses = region.get_synapses_from(source_neuron_id)
```

#### `initialize_random_connectivity(self, random_generator, connection_probability, min_weight, max_weight)`
Initialize random connectivity and weights.

```python
region.initialize_random_connectivity(rng, 0.1, 0.2, 0.4)
```

#### `reset(self)`
Reset the region to its initial state.

```python
region.reset()
```

### NeuralPopulation

The NeuralPopulation class represents a group of neurons with the same type and characteristics.

**Attributes:**
- `id`: Unique identifier for the population
- `type`: Neuron type (Excitatory, Inhibitory, etc.)
- `neuron_count`: Number of neurons in the population

**Methods:**

#### `__init__(self, population_id, neuron_type)`
Create a new neural population.

```python
population = nlm.NeuralPopulation(nlm.create_population_id(1), nlm.NeuronType.Excitatory)
```

#### `get_id(self) -> PopulationId`
Get the population ID.

```python
population_id = population.get_id()
```

#### `get_neuron_type(self) -> NeuronType`
Get the neuron type.

```python
neuron_type = population.get_neuron_type()
```

#### `add_neuron(self, neuron)`
Add a neuron to the population.

```python
population.add_neuron(neuron)
```

#### `get_neuron(self, neuron_id)`
Get a neuron by its ID.

```python
neuron = population.get_neuron(neuron_id)
```

#### `get_all_neurons(self) -> List[Neuron]`
Get all neurons in the population.

```python
neurons = population.get_all_neurons()
```

#### `get_neuron_count(self) -> int`
Get the number of neurons.

```python
count = population.get_neuron_count()
```

#### `get_active_neuron_count(self) -> int`
Get the number of active neurons.

```python
active_count = population.get_active_neuron_count()
```

#### `get_average_firing_rate(self) -> float`
Get the average firing rate of neurons in the population.

```python
avg_rate = population.get_average_firing_rate()
```

## Type Definitions

### NeuronId

```python
# Create neuron IDs
neuron_id = nlm.create_neuron_id(1)
invalid_neuron_id = nlm.INVALID_NEURON_ID
```

### SynapseId

```python
# Create synapse IDs
synapse_id = nlm.create_synapse_id(1)
invalid_synapse_id = nlm.INVALID_SYNAPSE_ID
```

### RegionId

```python
# Create region IDs
region_id = nlm.create_region_id(1)
invalid_region_id = nlm.INVALID_REGION_ID
```

### PopulationId

```python
# Create population IDs
population_id = nlm.create_population_id(1)
invalid_population_id = nlm.INVALID_POPULATION_ID
```

### NeuronType

```python
# Neuron type enumeration
print(nlm.NeuronType.Excitatory)  # NeuronType.Excitatory
print(nlm.NeuronType.Inhibitory) # NeuronType.Inhibitory
print(nlm.NeuronType.Modulatory)  # NeuronType.Modulatory
print(nlm.NeuronType.Sensory)     # NeuronType.Sensory
print(nlm.NeuronType.Motor)       # NeuronType.Motor
print(nlm.NeuronType.Internal)    # NeuronType.Internal
```

### SynapseType

```python
# Synapse type enumeration
print(nlm.SynapseType.Excitatory)   # SynapseType.Excitatory
print(nlm.SynapseType.Inhibitory)  # SynapseType.Inhibitory
print(nlm.SynapseType.Modulatory)   # SynapseType.Modulatory
print(nlm.SynapseType.Electrical)   # SynapseType.Electrical
print(nlm.SynapseType.GapJunction) # SynapseType.GapJunction
```

### DevelopmentalStage

```python
# Developmental stage enumeration
print(nlm.DevelopmentalStage.Initial)       # DevelopmentalStage.Initial
print(nlm.DevelopmentalStage.CriticalPeriod) # DevelopmentalStage.CriticalPeriod
print(nlm.DevelopmentalStage.Maturation)   # DevelopmentalStage.Maturation
print(nlm.DevelopmentalStage.Adult)        # DevelopmentalStage.Adult
print(nlm.DevelopmentalStage.Aging)       # DevelopmentalStage.Aging
```

### FiringState

```python
# Firing state enumeration
print(nlm.FiringState.Resting)    # FiringState.Resting
print(nlm.FiringState.Active)     # FiringState.Active
print(nlm.FiringState.Refractory) # FiringState.Refractory
print(nlm.FiringState.Inhibited) # FiringState.Inhibited
```

### ActionType

```python
# Action type enumeration
print(nlm.ActionType.MoveForward)  # ActionType.MoveForward
print(nlm.ActionType.MoveBackward) # ActionType.MoveBackward
print(nlm.ActionType.MoveLeft)     # ActionType.MoveLeft
print(nlm.ActionType.MoveRight)    # ActionType.MoveRight
print(nlm.ActionType.TurnLeft)     # ActionType.TurnLeft
print(nlm.ActionType.TurnRight)    # ActionType.TurnRight
print(nlm.ActionType.Look)         # ActionType.Look
print(nlm.ActionType.LookUp)       # ActionType.LookUp
print(nlm.ActionType.LookDown)     # ActionType.LookDown
print(nlm.ActionType.Interact)     # ActionType.Interact
print(nlm.ActionType.Eat)          # ActionType.Eat
print(nlm.ActionType.Drink)        # ActionType.Drink
print(nlm.ActionType.Rest)         # ActionType.Rest
print(nlm.ActionType.Wait)         # ActionType.Wait
print(nlm.ActionType.Custom)       # ActionType.Custom
```

### MotorCommand

```python
# Motor command enumeration
print(nlm.MotorCommand.MoveForward)  # MotorCommand.MoveForward
print(nlm.MotorCommand.MoveBackward) # MotorCommand.MoveBackward
print(nlm.MotorCommand.TurnLeft)     # MotorCommand.TurnLeft
print(nlm.MotorCommand.TurnRight)    # MotorCommand.TurnRight
print(nlm.MotorCommand.LookLeft)     # MotorCommand.LookLeft
print(nlm.MotorCommand.LookRight)    # MotorCommand.LookRight
print(nlm.MotorCommand.Interact)     # MotorCommand.Interact
print(nlm.MotorCommand.Wait)         # MotorCommand.Wait
```

## Examples

### Basic Neuron Operations

```python
import nlm

# Create neurons
neuron1 = nlm.Neuron(nlm.create_neuron_id(1))
neuron2 = nlm.Neuron(nlm.create_neuron_id(2))

# Configure neurons
neuron1.set_type(nlm.NeuronType.Excitatory)
neuron1.set_resting_potential(-70.0)
neuron1.set_threshold(-55.0)
neuron1.set_firing_rate(10.0)

neuron2.set_type(nlm.NeuronType.Inhibitory)
neuron2.set_resting_potential(-65.0)
neuron2.set_threshold(-50.0)

# Set up random initialization
rng = nlm.RandomGenerator(42)
neuron1.initialize_random(rng)
neuron2.initialize_random(rng)

# Check initial state
print(f"Neuron 1: Type={neuron1.get_type()}, Potential={neuron1.get_membrane_potential():.2f}mV")
print(f"Neuron 2: Type={neuron2.get_type()}, Potential={neuron2.get_membrane_potential():.2f}mV")
```

### Synapse Operations

```python
import nlm

# Create synapses
synapse1 = nlm.Synapse(
    nlm.create_synapse_id(1),
    nlm.create_neuron_id(1),  # Source
    nlm.create_neuron_id(2)   # Destination
)

synapse2 = nlm.Synapse(
    nlm.create_synapse_id(2),
    nlm.create_neuron_id(2),  # Source
    nlm.create_neuron_id(1)   # Destination
)

# Configure synapses
synapse1.set_type(nlm.SynapseType.Excitatory)
synapse1.set_weight(0.5)

synapse2.set_type(nlm.SynapseType.Inhibitory)
synapse2.set_weight(-0.3)

# Set up random initialization
rng = nlm.RandomGenerator(42)
synapse1.initialize_random(rng)
synapse2.initialize_random(rng)

# Check synapse properties
print(f"Synapse 1: {synapse1.get_source_neuron().index()} -> {synapse1.get_destination_neuron().index()}")
print(f"  Type: {synapse1.get_type()}, Weight: {synapse1.get_weight():.3f}")
print(f"Synapse 2: {synapse2.get_source_neuron().index()} -> {synapse2.get_destination_neuron().index()}")
print(f"  Type: {synapse2.get_type()}, Weight: {synapse2.get_weight():.3f}")
```

### NeuralRegion Operations

```python
import nlm

# Create region
region_id = nlm.create_region_id(1)
region = nlm.NeuralRegion(region_id, "Cortex")

# Create and add neurons
neurons = []
for i in range(5):
    neuron = nlm.Neuron(nlm.create_neuron_id(i + 1))
    neuron.set_type(nlm.NeuronType.Excitatory)
    neuron.set_resting_potential(-70.0)
    neuron.set_threshold(-55.0)
    region.add_neuron(neuron)
    neurons.append(neuron)

# Create and add synapses
for i in range(len(neurons)):
    for j in range(i + 1, len(neurons)):
        # Create forward connection
        synapse = nlm.Synapse(
            nlm.create_synapse_id(i * len(neurons) + j + 1),
            neurons[i].get_id(),
            neurons[j].get_id()
        )
        synapse.set_type(nlm.SynapseType.Excitatory)
        synapse.set_weight(0.1 + nlm.random_uniform(0, 0.4))
        region.add_synapse(synapse)

# Set up random connectivity
rng = nlm.RandomGenerator(42)
region.initialize_random_connectivity(rng, 0.5, 0.1, 0.4)

# Check region properties
print(f"Region: {region.get_name()}")
print(f"  Neuron count: {region.get_neuron_count()}")
print(f"  Synapse count: {region.get_synapse_count()}")
```

### NeuralPopulation Operations

```python
import nlm

# Create population
population_id = nlm.create_population_id(1)
population = nlm.NeuralPopulation(population_id, nlm.NeuronType.Excitatory)

# Create and add neurons
for i in range(10):
    neuron = nlm.Neuron(nlm.create_neuron_id(i + 1))
    neuron.set_type(nlm.NeuronType.Excitatory)
    neuron.set_resting_potential(-70.0)
    neuron.set_threshold(-55.0)
    population.add_neuron(neuron)

# Check population properties
print(f"Population type: {population.get_neuron_type()}")
print(f"  Neuron count: {population.get_neuron_count()}")

# Create neurons for simulation
rng = nlm.RandomGenerator(42)
for neuron in population.get_all_neurons():
    neuron.initialize_random(rng)

# Check average firing rate
avg_rate = population.get_average_firing_rate()
print(f"  Average firing rate: {avg_rate:.2f} Hz")
```

## Advanced Usage

### Neural Dynamics Simulation

```python
import nlm

# Create brain and world for simulation
brain = nlm.create_brain()
world = nlm.create_simple_world()
brain.initialize()

# Create neural components
region_id = brain.add_region("SensoryCortex")
region = brain.get_region(region_id)

# Create neurons
neurons = []
for i in range(20):
    neuron = nlm.Neuron(nlm.create_neuron_id(i + 1))
    neuron.set_type(nlm.NeuronType.Sensory)
    neuron.set_resting_potential(-65.0)
    neuron.set_threshold(-50.0)
    region.add_neuron(neuron)
    neurons.append(neuron)

# Create synapses with random weights
rng = nlm.RandomGenerator(42)
for i in range(len(neurons)):
    for j in range(i + 1, len(neurons)):
        synapse = nlm.Synapse(
            nlm.create_synapse_id(i * len(neurons) + j + 1),
            neurons[i].get_id(),
            neurons[j].get_id()
        )
        synapse.set_type(nlm.SynapseType.Excitatory)
        synapse.set_weight(rng.uniform_real(0.1, 0.4))
        region.add_synapse(synapse)

# Set up random connectivity
region.initialize_random_connectivity(rng, 0.3, 0.1, 0.5)

# Run neural dynamics simulation
for step in range(100):
    # Get current time
    current_time = step * 0.001
    
    # Step all neurons
    for neuron in region.get_all_neurons():
        # Inject some current for demonstration
        neuron.inject_current(0.1)
        
        # Step the neuron
        neuron.step(current_time)
        
        # Check if neuron spiked
        if neuron.check_threshold():
            neuron.record_spike(current_time)
    
    # Update synapses
    for synapse in region.get_all_synapses():
        synapse.step(current_time)
    
    # Print progress
    if step % 10 == 0:
        active_count = region.get_active_neuron_count()
        print(f"Step {step}: {active_count} active neurons")

# Analyze results
print(f"Total spikes: {sum(len(neuron.get_spike_history()) for neuron in region.get_all_neurons())}")
print(f"Average firing rate: {region.get_average_firing_rate():.2f} Hz")
```

### Memory and Learning

```python
import nlm

# Create brain with memory systems
brain = nlm.create_brain()
brain.initialize()

# Enable memory systems
brain.get_working_memory().set_capacity(100)
brain.get_episodic_memory().set_max_episodes(1000)

# Create neural components for learning
region_id = brain.add_region("MemoryCortex")
region = brain.get_region(region_id)

# Create neurons with plasticity
neurons = []
for i in range(10):
    neuron = nlm.Neuron(nlm.create_neuron_id(i + 1))
    neuron.set_type(nlm.NeuronType.Internal)
    neuron.set_resting_potential(-70.0)
    neuron.set_threshold(-55.0)
    neuron.enable_plasticity(True, True, True)  # Hebbian, STDP, reward-modulated
    region.add_neuron(neuron)
    neurons.append(neuron)

# Create learning synapses
for i in range(len(neurons)):
    for j in range(i + 1, len(neurons)):
        synapse = nlm.Synapse(
            nlm.create_synapse_id(i * len(neurons) + j + 1),
            neurons[i].get_id(),
            neurons[j].get_id()
        )
        synapse.set_type(nlm.SynapseType.Excitatory)
        synapse.enable_plasticity(True, True, True)
        region.add_synapse(synapse)

# Set up random connectivity
rng = nlm.RandomGenerator(42)
region.initialize_random_connectivity(rng, 0.4, 0.1, 0.5)

# Run learning simulation
for step in range(500):
    current_time = step * 0.001
    
    # Random activity pattern
    for neuron in region.get_all_neurons():
        if rng.uniform_real(0, 1) < 0.1:  # 10% chance of firing
            neuron.inject_current(5.0)  # Strong input to induce firing
    
    # Step neurons
    for neuron in region.get_all_neurons():
        if neuron.step(current_time):
            neuron.record_spike(current_time)
    
    # Step synapses (plasticity updates)
    for synapse in region.get_all_synapses():
        synapse.step(current_time)
    
    # Store experience in working memory
    for neuron in region.get_all_neurons():
        if neuron.is_firing():
            activation = abs(neuron.get_membrane_potential() - neuron.get_resting_potential()) / 10.0
            brain.get_working_memory().store_to_neuron(neuron.get_id(), activation)
    
    # Store episode in episodic memory
    if step % 10 == 0:
        episode = nlm.EpisodicMemoryItem()
        episode.timestamp = step
        episode.active_neurons = [neuron.get_id() for neuron in region.get_all_neurons() if neuron.is_firing()]
        episode.reward = 0.0  # No reward in this example
        brain.get_episodic_memory().store_episode(episode)
    
    # Occasionally replay memories
    if step % 50 == 0:
        episodes = brain.get_episodic_memory().get_episodes_for_replay(3)
        for episode in episodes:
            # Replay the episode (simplified)
            pass

# Analyze learning
print(f"Working memory active traces: {brain.get_working_memory().get_active_traces()}")
print(f"Episodic memory episodes: {brain.get_episodic_memory().get_episode_count()}")
```

## Performance Considerations

### Large-Scale Simulations

```python
import nlm

# Create optimized brain for large simulations
brain = (nlm.create_brain()
         .with_neuron_count(10000)  # Large scale
         .with_development()       # Developmental effects
         .with_structural_plasticity())  # Structural plasticity

brain.initialize()

# Use efficient memory access
neurons = brain.get_all_neurons()
neurons.activate_all(1.0)  # Efficient batch activation

# Use batch operations for efficiency
working_memory = brain.get_working_memory()
working_memory.set_capacity(1000)  # Efficient memory management
```

### Real-time Applications

```python
import nlm
import time

# Create real-time optimized brain
brain = nlm.create_brain()
brain.initialize()

# Target 1kHz update rate
update_interval = 0.001  # 1ms

start_time = time.time()
step_count = 0

while time.time() - start_time < 60:  # Run for 60 seconds
    loop_start = time.time()
    
    # Simulation steps...
    
    elapsed = time.time() - loop_start
    sleep_time = max(0, update_interval - elapsed)
    
    if sleep_time > 0:
        time.sleep(sleep_time)

print(f"Completed {step_count} steps in 60 seconds")
print(f"Achieved {step_count / 60:.1f} Hz")
```

## Error Handling

### Common Exceptions

```python
import nlm

try:
    # Create neurons
    neuron1 = nlm.Neuron(nlm.create_neuron_id(1))
    neuron2 = nlm.Neuron(nlm.create_neuron_id(2))
    
    # Try to access invalid neuron
    invalid_neuron = nlm.Neuron(nlm.INVALID_NEURON_ID)
    
except nlm.NeuronException as e:
    print(f"Neuron error: {e}")
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Migration Guide

### From C++ API

If migrating from the C++ API to Python, note these key differences:

1. **Memory Management**: Python handles memory automatically
2. **Error Handling**: Use try-catch with NLM exceptions
3. **Type Hints**: Use type annotations for better IDE support
4. **Documentation**: Access docstrings with help()

### Performance Tips

1. **NumPy Integration**: Use NumPy for numerical operations
2. **Batch Processing**: Use batch operations for efficiency
3. **Caching**: Cache frequently accessed data
4. **Profiling**: Use Python profiling tools

## Conclusion

The Neural module provides a comprehensive Python interface to the core neural components of the NLM framework. With its modular design, type safety, and extensive documentation, it's ideal for both beginners and advanced users working with neural simulations.

Key features:
- **Biological Accuracy**: Implements LIF dynamics with realistic parameters
- **Learning Capabilities**: Supports Hebbian, STDP, and reward-modulated plasticity
- **Memory Systems**: Integrates with working memory and episodic memory
- **Extensive Documentation**: Comprehensive docstrings and examples
- **Type Safety**: Full type hints and validation
- **Performance Optimized**: Efficient implementations for large-scale simulations

Start exploring the power of neural computation with the NLM Neural module today!