# NLM Project Documentation

## Quick Start Guide

### Installation

```bash
# Install NLM from PyPI
pip install nlm

# Or install from source
pip install -e ./nlm/python
```

## Basic Usage

```python
import nlm

# Create NLM system
nlm_system = nlm.NLM()

# Create a simple brain
brain = nlm_system.create_brain({
    "regions": 3,
    "neurons_per_region": 50,
    "synaptic_connections": "auto"
})

# Run simulation
results = nlm_system.run_simulation(steps=100)

# Get results
print(f"Simulation completed with {results.get('steps', 0)} steps")
print(f"Average activity: {results.get('avg_activity', 0):.3f}")
```

## Next Steps

- Learn about configuration management
- Explore memory systems
- Set up learning environments
- Try advanced agent training

## Advanced User Guide

### Configuration Management

NLM provides a comprehensive configuration system with support for multiple formats:

```python
import nlm

# Create configuration manager
config = nlm.Config()

# Load from file (auto-detects format)
config.load_file("./configs/neural_network.json")

# Or load from plain text
config.load_text("""
# Neural network configuration
brain_regions = 5
neurons_per_region = 100
learning_rate = 0.01
plasticity_type = "hebbian"
""")

# Save with different formats
config.save("output/config.yaml", format="yaml")
config.save("output/config.json", format="json")

# Validate configuration
if config.validate():
    print("Configuration is valid!")
```

### Brain System Configuration

```python
# Advanced brain configuration
brain_config = nlm.BrainConfig()

# Set neuron parameters
brain_config.set("neuron_type", "lif")
brain_config.set("threshold", -55.0)
brain_config.set("resting_potential", -70.0)
brain_config.set("refractory_period", 5)

# Set synaptic parameters
brain_config.set("synaptic_strength", 0.1)
brain_config.set("synaptic_delay", 1.0)
brain_config.set("inhibition_strength", 0.05)

# Create brain with advanced configuration
brain = nlm.create_brain(brain_config)
```

### Memory System Management

```python
# Configure memory systems
memory_config = nlm.MemoryConfig()

# Episodic memory settings
memory_config.set("max_episodes", 1000)
memory_config.set("consolidation_threshold", 0.7)
memory_config.set("replay_frequency", 0.1)

# Working memory settings
memory_config.set("working_memory_capacity", 100)
memory_config.set("decay_rate", 0.01)
memory_config.set("update_frequency", 0.05)

# Semantic memory settings
memory_config.set("semantic_retention", 3600)  # seconds
memory_config.set("semantic_decay", 0.001)
memory_config.set("fact_limit", 10000)

# Create memory system
memory = nlm.create_memory_system(memory_config)
```
