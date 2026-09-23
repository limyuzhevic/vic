# NLM Python Bindings

This document describes the Python bindings for the NLM (Neural Learning Machine) C++ neural simulation framework.

## Installation

The Python bindings are built using scikit-build-core and pybind11. After building the C++ library:

```bash
pip install -e .
```

## API Overview

The Python bindings provide access to NLM's neural simulation capabilities through a Python-friendly interface:

### Core Classes

- `Brain`: Main neural system with neurons, synapses, and memory
- `Neuron`: Individual neuron model with LIF dynamics
- `Region`: Neural population with organized connectivity
- `Population`: Group of neurons with shared properties
- `Memory`: Collection of memory systems
- `WorkingMemory`: Active memory storage
- `EpisodicMemory`: Experience storage and replay
- `SemanticMemory`: Fact and knowledge storage
- `ProceduralMemory`: Skill and habit storage
- `AssociativeMemory`: Relationship storage
- `Config`: Configuration management
- `RandomGenerator`: Random number generation

### Basic Usage Example

```python
from pynlm import Brain, Neuron, Memory

# Create a brain system
brain = Brain()

# Create neurons
neuron1 = brain.create_neuron(neuron_type=NeuronType.EXCITATORY)
neuron2 = brain.create_neuron(neuron_type=NeuronType.INHIBITORY)

# Create memory systems
brain.create_working_memory(capacity=100)
brain.create_episodic_memory(max_episodes=1000)
brain.create_semantic_memory()
brain.create_procedural_memory()
brain.create_associative_memory()

# Configure neuron parameters
neuron1.set_leak_conductance(10.0)
neuron1.set_threshold(-55.0)
neuron1.set_resting_potential(-70.0)

# Add synaptic connection
brain.create_synapse(neuron1, neuron2, weight=0.5)

# Run simulation
for step in range(1000):
    brain.step()
    if step % 100 == 0:
        print(f"Step {step}, spike rate: {brain.get_spike_rate():.2f} Hz")
```

### Memory System Usage

```python
# Working Memory
working_mem = brain.get_working_memory()
working_mem.store(neuron_id, value=0.7)
retrieved_value = working_mem.retrieve(neuron_id)

# Episodic Memory
episodic_mem = brain.get_episodic_memory()
# Store an experience
episode = EpisodicMemoryItem(
    timestamp=step,
    neurons=[neuron1.get_id()],
    values=[0.8],
    metadata="test experience",
    action=ActionType.MOVE,
    reward=1.0
)
episodic_mem.store_episode(episode)

# Semantic Memory
semantic_mem = brain.get_semantic_memory()
semantic_mem.store_fact("concept", "definition")
definition = semantic_mem.retrieve_fact("concept")

# Procedural Memory
procedural_mem = brain.get_procedural_memory()
procedural_mem.learn_skill("walk", [neuron1.get_id(), neuron2.get_id()])
skill = procedural_mem.get_skill("walk")

# Associative Memory
assoc_mem = brain.get_associative_memory()
assoc_mem.associate(neuron1, neuron2, strength=0.8)
associations = assoc_mem.get_associations(neuron1.get_id())
```

### Configuration

```python
# Load configuration from file
config = Config()
config.load_from_file("config.yaml")

# Access configuration values
learning_rate = config.get("learning_rate", 0.01)
max_steps = config.get_or("max_steps", 1000, default=1000)

# Save configuration
config.save_to_file("config.yaml")
```

### Advanced Features

```python
# Neural working memory with persistent activity
from pynlm import NeuralWorkingMemory
nwm = NeuralWorkingMemory()
nwm.initialize(brain)
nwm.store([0.1, 0.5, 0.9])
pattern = nwm.retrieve()

# Attentional selection
from pynlm import AttentionalSelection
attention = AttentionalSelection()
attention.initialize(brain)
winners = attention.process_competition([neuron1, neuron2])

# Neural episodic memory replay
from pynlm import NeuralEpisodicMemory
nem = NeuralEpisodicMemory()
nem.initialize(brain)
episode = nem.get_episode(0)
nem.replay_episode(episode)

# Neural associative memory
from pynlm import NeuralAssociativeMemory
nam = NeuralAssociativeMemory()
nam.initialize(brain)
nam.associate([0.1, 0.2], [0.3, 0.4])
retrieved = nam.retrieve([0.1, 0.2])
```

### Error Handling

```python
try:
    # Create a new brain
    brain = Brain()
    
    # Simulate with error handling
    for step in range(1000):
        brain.step()
        if step % 100 == 0:
            print(f"Simulation step {step}")
            
except NLMException as e:
    print(f"NLM Error: {e}")
except MemoryError as e:
    print(f"Memory Error: {e}")
except SimulationError as e:
    print(f"Simulation Error: {e}")
```

### Performance Monitoring

```python
# Get simulation statistics
stats = brain.get_statistics()
print(f"Average firing rate: {stats.average_firing_rate:.2f} Hz")
print(f"Total spikes: {stats.total_spikes}")
print(f"Simulation time: {stats.simulation_time:.2f} ms")

# Memory statistics
mem_stats = brain.get_memory_statistics()
print(f"Working memory usage: {mem_stats.working_memory_usage}/{mem_stats.working_memory_capacity}")
print(f"Episodic memory episodes: {mem_stats.episodic_memory_episodes}")
```

### Full Example Script

```python
#!/usr/bin/env python3
"""Example NLM simulation script"""

from pynlm import Brain, Config, NeuronType, ActionType, EpisodicMemoryItem
import matplotlib.pyplot as plt

def main():
    # Load configuration
    config = Config()
    config.load_from_file("default_config.yaml")
    
    # Create brain
    brain = Brain()
    
    # Configure brain with simulation parameters
    brain.set_max_steps(config.get_or("max_steps", 10000))
    brain.set_time_step(config.get_or("time_step", 0.001))
    brain.set_leak_conductance(config.get_or("leak_conductance", 10.0))
    
    # Create neural populations
    brain.create_region("sensory", 10, neuron_type=NeuronType.EXCITATORY)
    brain.create_region("motor", 5, neuron_type=NeuronType.EXCITATORY)
    brain.create_region("associative", 20, neuron_type=NeuronType.EXCITATORY)
    
    # Create memory systems
    brain.create_working_memory(capacity=200)
    brain.create_episodic_memory(max_episodes=5000)
    brain.create_semantic_memory()
    brain.create_procedural_memory()
    brain.create_associative_memory()
    
    # Record initial statistics
    initial_stats = brain.get_statistics()
    
    # Run simulation
    spike_history = []
    for step in range(brain.get_max_steps()):
        # Get sensory input (simulated)
        sensory_input = brain.get_region("sensory").get_average_activation()
        
        # Apply sensory input
        brain.set_region_input("sensory", sensory_input)
        
        # Step brain simulation
        brain.step()
        
        # Record spike rate
        spike_rate = brain.get_spike_rate()
        spike_history.append(spike_rate)
        
        # Store experience every 100 steps
        if step % 100 == 0:
            episode = EpisodicMemoryItem(
                timestamp=step,
                neurons=brain.get_active_neuron_ids(),
                values=brain.get_neuron_activations(),
                action=ActionType.WAIT,
                reward=spike_rate * 0.1
            )
            brain.get_episodic_memory().store_episode(episode)
        
        # Print progress
        if step % 1000 == 0:
            print(f"Step {step}, spike rate: {spike_rate:.2f} Hz")
    
    # Plot results
    plt.figure(figsize=(12, 4))
    plt.subplot(1, 2, 1)
    plt.plot(spike_history)
    plt.xlabel("Step")
    plt.ylabel("Spike Rate (Hz)")
    plt.title("Neuron Activity")
    
    # Plot memory statistics
    plt.subplot(1, 2, 2)
    mem_stats = brain.get_memory_statistics()
    categories = ['Working', 'Episodic', 'Semantic', 'Procedural', 'Associative']
    usages = [
        mem_stats.working_memory_usage,
        mem_stats.episodic_memory_episodes,
        len(brain.get_semantic_memory().get_all_facts()),
        len(brain.get_procedural_memory().get_all_skills()),
        mem_stats.associative_memory_associations
    ]
    plt.bar(categories, usages)
    plt.ylabel("Usage Count")
    plt.title("Memory System Usage")
    
    plt.tight_layout()
    plt.savefig("nlm_simulation_results.png")
    plt.close()
    
    # Save final configuration
    config.save_to_file("final_config.yaml")
    
    # Display final statistics
    final_stats = brain.get_statistics()
    print("\n=== Simulation Complete ===")
    print(f"Total simulation time: {final_stats.simulation_time:.2f} ms")
    print(f"Total spikes: {final_stats.total_spikes}")
    print(f"Average firing rate: {final_stats.average_firing_rate:.2f} Hz")
    print(f"Max firing rate: {final_stats.max_firing_rate:.2f} Hz")
    
    print(f"\nMemory Statistics:")
    print(f"Working memory usage: {mem_stats.working_memory_usage}/{mem_stats.working_memory_capacity}")
    print(f"Episodic memory episodes: {mem_stats.episodic_memory_episodes}")
    print(f"Semantic memory facts: {len(brain.get_semantic_memory().get_all_facts())}")
    print(f"Procedural memory skills: {len(brain.get_procedural_memory().get_all_skills())}")
    print(f"Associative memory associations: {mem_stats.associative_memory_associations}")

if __name__ == "__main__":
    main()
```

## Development

### Adding New Python Bindings

To add new Python bindings, create a new .pyx file in the `src/python/` directory and add it to the CMakeLists.txt:

```cmake
pybind11_add_module(pynlm
    src/python/bindings.cpp
    src/python/neuron.cpp
    src/python/brain.cpp
    # ... other binding files
)
```

### Running Tests

```bash
pytest tests/
```

### Building Documentation

```bash
mkdocs build
```

## Troubleshooting

### Build Errors

If you encounter build errors:

1. Ensure you have the latest version of scikit-build-core and pybind11
2. Check that all C++ dependencies are installed
3. Verify your build directory is clean

### Import Errors

If you cannot import pynlm:

1. Ensure the package is installed in development mode: `pip install -e .`
2. Check that all dependencies are installed
3. Verify the build was successful

### Runtime Errors

For runtime errors:

1. Check for memory leaks
2. Verify that all required neurons and regions are created
3. Ensure simulation parameters are properly configured

## License

MIT