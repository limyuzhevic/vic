# NLM Enhanced Python API Guide

This document provides comprehensive documentation for the enhanced Python bindings of the Neural Learning Machine (NLM) framework.

## Overview

The enhanced bindings now provide access to the full neural architecture, including:

- **Core Neural Components**: Neuron, Synapse, NeuralRegion, NeuralPopulation
- **Plasticity Mechanisms**: STDP, Hebbian learning, PlasticityRule
- **Neuromodulation**: Dopamine, Curiosity, Novelty, PredictionError
- **Research APIs**: Factory functions, batch operations, utilities

This enables advanced neuroscience research capabilities that were previously unavailable in the Python API.

## Quick Start Examples

### 1. Create a Basic Research Brain

```python
import pynlm

# Create a research brain with 1000 neurons and 0.1 connection density
brain = pynlm.createResearchBrain(1000, 0.1)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### 2. Create Complete Research Setup

```python
import pynlm

# Create complete setup with all subsystems
brain, world, agent = pynlm.createCompleteResearchSetup()

# Run simulation for 1000 steps
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Check results
print(f"Simulation complete!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
```

### 3. Working with Individual Neurons

```python
import pynlm

# Create a brain and access individual neurons
brain = pynlm.createResearchBrain(100, 0.05)
brain.initialize()

# Get all neurons in the brain
neurons = pynlm.getAllNeuronsInBrain(brain)
print(f"Total neurons: {len(neurons)}")

# Access first 10 neurons
for i, neuron in enumerate(neurons[:10]):
    print(f"Neuron {i}: ID={neuron.getId()}, Type={neuron.getType()}")
    print(f"  Membrane potential: {neuron.getMembranePotential():.2f}mV")
    print(f"  Firing rate: {neuron.getFiringRate():.2f}Hz")
    print(f"  Firing: {neuron.isFiring()}")
```

### 4. Creating and Managing Neural Populations

```python
import pynlm

# Create a brain and region
brain = pynlm.createResearchBrain(500, 0.1)
brain.initialize()

# Create a region
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)

# Create multiple populations
populations = pynlm.createNeuralPopulations(region, 50, pynlm.NeuronType.Excitatory)
print(f"Created {len(populations)} neural populations")

# Analyze population statistics
for i, pop in enumerate(populations):
    print(f"Population {i}: {pop.getSize()} neurons, Type={pop.getNeuronType()}")
    print(f"  Active: {pop.getActiveCount()}")
    print(f"  Firing: {pop.getFiringCount()}")
    print(f"  Average firing rate: {pop.getAverageFiringRate():.2f}Hz")
```

### 5. Plasticity and Neuromodulation Control

```python
import pynlm

# Create setup with learning systems
brain, world, agent = pynlm.createCompleteResearchSetup()

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation with learning
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Check learning metrics
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Prediction Error: {agent.getPredictionError():.3f}")
        print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
```

### 6. Advanced Brain Analysis

```python
import pynlm

# Create multiple brains for comparison
brains = []
for i in range(3):
    brain = pynlm.createResearchBrain(1000, 0.1)
    brain.initialize()
    brains.append(brain)

# Initialize all brains
pynlm.initializeAllBrains(brains, 100)

# Collect statistics
stats = pynlm.collectBrainStatistics(brains)
for i, stat in enumerate(stats):
    print(f"Brain {i} statistics:")
    print(f"  Total neurons: {stat['total_neurons']}")
    print(f"  Total synapses: {stat['total_synapses']}")
    print(f"  Firing neurons: {stat['firing_neurons']}")
    print(f"  Average firing rate: {stat['average_firing_rate']:.2f}Hz")
    print(f"  E/I ratio: {stat['excitation_inhibition_ratio']:.2f}")
    print(f"  Total spikes: {stat['total_spikes']}")
```

## Class Reference

### Neuron

```python
# Properties
neuron.id          # Neuron ID
neuron.type        # Neuron type (Excitatory, Inhibitory, etc.)
neuron.membrane_potential  # Current membrane potential (mV)
neuron.firing_rate # Current firing rate (Hz)
neuron.is_firing() # Is neuron currently firing?
neuron.is_refractory() # Is neuron in refractory period?

# Methods
neuron.record_spike(timestamp)  # Record a spike
neuron.get_spike_history()       # Get spike history timestamps
neuron.inject_current(current)    # Inject external current
neuron.reset()                    # Reset to initial state
neuron.enable_plasticity(hebbian, stdp, reward_modulated)
```

### Synapse

```python
# Properties
synapse.id              # Synapse ID
synapse.weight          # Synaptic weight
synapse.type            # Synapse type (Excitatory, Inhibitory, etc.)
synapse.delay           # Transmission delay

# Methods
synapse.record_pre_spike(timestamp)   # Record pre-synaptic spike
synapse.record_post_spike(timestamp)  # Record post-synaptic spike
synapse.set_weight(weight)             # Set weight
synapse.add_to_weight(delta)           # Add to weight
synapse.enable_plasticity(hebbian, stdp, reward_modulated)
```

### Brain

```python
# Properties (from original API)
brain.total_neuron_count
brain.total_synapse_count
brain.firing_neuron_count
brain.average_firing_rate
brain.excitation_inhibition_ratio
brain.total_spike_count

# New methods
brain.get_all_neurons()              # Get all neurons in brain
brain.get_brain_stats()              # Get comprehensive statistics
brain.add_region(name)                # Add new region
brain.get_region(region_id)           # Get region by ID
brain.get_all_regions()               # Get all regions
brain.save("checkpoint.bin")          # Save brain state
brain.load("checkpoint.bin")          # Load brain state
```

### Plasticity Rules

```python
# STDP - Spike-Timing Dependent Plasticity
stdp = STDP()
stdp.setLTPWeight(0.5)      # Long-term potentiation weight
stdp.setLTDWeight(-0.5)     # Long-term depression weight
stdp.setTimeConstant(10.0)  # Time constant (ms)

# Hebbian - Hebbian learning
hebbian = Hebbian()
hebbian.setLearningRate(0.01)    # Learning rate
hebbian.setMaxWeight(1.0)        # Maximum weight
```

### Neuromodulators

```python
# Dopamine - reward signaling
dopamine = Dopamine()
dopamine.signal_reward(1.0)           # Signal reward
dopamine.signal_reward_prediction_error(0.1)

# Curiosity - exploration motivation
curiosity = Curiosity()
curiosity.setLevel(0.5)                # Set curiosity level

# Novelty - novelty detection
novelty = Novelty()
novelty.setLevel(0.3)                  # Set novelty level

# PredictionError - prediction error signal
pred_error = PredictionError()
pred_error.setLevel(0.2)                # Set prediction error level
```

## Advanced Research APIs

### Factory Functions

```python
# Quick creation of common setups
brain = pynlm.createResearchBrain(1000, 0.1)
world = pynlm.createSimpleResearchWorld(20, 20)
agent = pynlm.createAgentWithDefaults(brain, world)
brain, world, agent = pynlm.createCompleteResearchSetup()
```

### Batch Operations

```python
# Work with multiple brains simultaneously
brains = [pynlm.createResearchBrain(1000, 0.1) for _ in range(5)]
pynlm.initializeAllBrains(brains, 100)
stats = pynlm.collectBrainStatistics(brains)

# Create multiple populations
populations = pynlm.createNeuralPopulations(region, 50, pynlm.NeuronType.Excitatory)
```

### Analysis Utilities

```python
# Analyze connectome
analysis = pynlm.analyzeConnectome(region)
print(f"Neuron count: {analysis['neuron_count']}")
print(f"Synaptic density: {analysis['synaptic_density']:.3f}")

# Visualize activity
activity_map = pynlm.visualizeNeuralActivity(region)
print(f"Activity map dimensions: {len(activity_map)}x{len(activity_map[0])}")

# Analyze memory
memory_analysis = pynlm.analyzeWorkingMemory(memory)
print(f"Active traces: {memory_analysis['active_traces']}")
print(f"Memory activity: {memory_analysis['memory_activity']:.3f}")

# Analyze prediction
prediction_analysis = pynlm.analyzePredictionSystem(prediction)
print(f"Prediction error: {prediction_analysis['prediction_error']:.3f}")
print(f"Prediction confidence: {prediction_analysis['confidence']:.3f}")
```

## Migration Guide

### From Original to Enhanced API

#### Original API (Limited)

```python
import pynlm

# Limited to high-level classes only
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)

# No access to neural components
# No plasticity control
# No neuromodulation
```

#### Enhanced API (Full access)

```python
import pynlm

# Full access to neural components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access neurons and synapses
neurons = pynlm.getAllNeuronsInBrain(brain)
stats = pynlm.getBrainStats(brain)

# Enable advanced learning
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Full neural architecture access
region = brain.addRegion("research")
populations = pynlm.createNeuralPopulations(region, 50, pynlm.NeuronType.Excitatory)

# Advanced analysis
analysis = pynlm.analyzeConnectome(region)
```

## Performance Considerations

1. **Batch Operations**: Use factory functions and batch operations for efficiency
2. **Memory Management**: Be mindful of Python object overhead for large networks
3. **Simulation Loops**: For high-performance simulations, use the C++ API directly
4. **Data Size**: Large arrays (sensory inputs, state vectors) should be managed carefully

## Troubleshooting

### Common Issues and Solutions

**Issue**: `ImportError: cannot import name 'X'`
**Solution**: Ensure you're using the enhanced bindings. The bindings module is `pynlm`.

**Issue**: `AttributeError: 'Y' object has no attribute 'Z'`
**Solution**: Check the method names in the documentation above. Some methods may have different names.

**Issue**: Slow performance with large networks
**Solution**: Use `createResearchBrain()` with appropriate parameters, and consider batch operations.

**Issue**: Memory issues with many neurons
**Solution**: Use `createResearchBrain()` with memory-efficient configurations, and clear unused objects.

## Future Enhancements

The following features are planned for future releases:

1. **Advanced Visualization**: Integration with matplotlib and other visualization libraries
2. **Distributed Computing**: Support for multi-GPU and distributed training
3. **Export/Import**: Support for importing/exporting neural networks
4. **Advanced Learning**: Integration with modern deep learning frameworks
5. **Cloud Integration**: Support for cloud-based simulations

## License and Attribution

This enhanced bindings implementation is provided under the same license as the original NLM framework. Credits go to the original NLM development team and all contributors to the project.

For questions, issues, or feature requests, please refer to the NLM project documentation and issue tracker.
