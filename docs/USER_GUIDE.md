# NLM Python Bindings User Guide

This comprehensive user guide provides step-by-step instructions for using
the NLM Python bindings to create neural simulations, learning experiments,
and cognitive models.

## Table of Contents

1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Quick Start](#quick-start)
4. [Basic Usage](#basic-usage)
5. [Advanced Topics](#advanced-topics)
6. [Troubleshooting](#troubleshooting)
7. [References](#references)

## Introduction

The NLM (Neural Learning Machine) Python bindings provide a complete interface
to the NLM C++ neural simulation framework. This allows users to create,
runtime, and analyze biologically plausible neural simulations using Python,
without needing to understand the underlying C++ implementation.

### What is NLM?

NLM is a comprehensive neural simulation framework that implements:

- **Neural Dynamics**: Leaky Integrate-and-Fire (LIF) neuron models
- **Learning Algorithms**: STDP, Hebbian, and reward-modulated plasticity
- **Memory Systems**: Working, episodic, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error
- **Cognitive Functions**: Neural planning, attention, and concept formation
- **Developmental Processes**: Critical periods and maturation
- **Prediction Systems**: Forward models and error computation
- **Visualization**: Comprehensive plotting and analysis tools

### When to Use NLM Python Bindings

Use NLM Python bindings when you need:

- Biologically realistic neural simulations
- Learning and plasticity research
- Cognitive modeling
- Neuromodulation studies
- Memory system analysis
- Experimental design and execution
- Visualization of neural data
- Integration with Python-based workflows

## Installation

### Prerequisites

Before installing NLM Python bindings, ensure you have:

- Python 3.8 or higher
- Virtual environment (recommended)
- Basic Python packages (numpy, matplotlib for full functionality)

### Installation from Source

1. Clone the repository:
```bash
https://github.com/your-repo/nlm-python-bindings.git
cd nlm-python-bindings
```

2. Install the package in development mode:
```bash
pip install -e .
```

3. Install optional dependencies for full functionality:
```bash
pip install pynlm[all]
```

### Installation from PyPI

For users who prefer pre-built packages:

```bash
pip install pynlm
```

### Installation with Development Dependencies

For development and testing:

```bash
pip install -e .[dev]
```

This installs the package in development mode along with development tools
like pytest, black, flake8, and other linting tools.

## Quick Start

This section provides a minimal working example to get you started with NLM
Python bindings quickly.

### Step 1: Import and Setup

```python
import pynlm
import matplotlib.pyplot as plt

# Create a default configuration
config = pynlm.Config.create_default()
```

### Step 2: Create and Initialize Brain

```python
# Create brain instance
brain = pynlm.create_brain(config)

# Initialize the brain (required before simulation)
brain.initialize()
print("Brain initialized successfully!")
```

### Step 3: Run a Simple Simulation

```python
# Run a simulation for 100 steps
num_steps = 100

print(f"Running simulation for {num_steps} steps...")

for step in range(num_steps):
    brain.step(step)
    
    # Print progress every 20 steps
    if step % 20 == 0:
        stats = brain.get_stats()
        print(f"Step {step}: {stats['total_spikes']} spikes, "
              f"Firing rate: {stats['firing_rate']:.2f} Hz")

print("Simulation completed!")
```

### Step 4: Analyze Results

```python
# Get final statistics
final_stats = brain.get_stats()

print("\n=== Final Statistics ===")
print(f"Total simulation steps: {final_stats['step']}")
print(f"Total spikes: {final_stats['total_spikes']}")
print(f"Average firing rate: {final_stats['firing_rate']:.3f} Hz")
print(f"Excitation/Inhibition ratio: {final_stats['e_i_ratio']:.3f}")
print(f"Active neurons: {final_stats['active_neurons']}")
print(f"Working memory usage: {final_stats['working_memory_usage']:.3f}")
print(f"Prediction error: {final_stats['prediction_error']:.3f}")
print(f"Neuromodulation level: {final_stats['neuromodulation_level']:.3f}")
print(f"Developmental stage: {final_stats['developmental_stage']}")
```

### Step 5: Visualize Results

```python
# Create visualizer and generate plots
visualizer = pynlm.Visualizer(save_plots=True)

print("\nCreating visualizations...")
visualizer.plot_all(brain)
print("Visualizations saved to 'visualizations/' directory")
```

### Step 6: Save and Load

```python
# Save brain state for later use
brain.save("my_simulation_checkpoint.json")
print("Brain state saved to 'my_simulation_checkpoint.json'")

# Load brain state (in a new session)
loaded_brain = pynlm.create_brain(config)
loaded_brain.load("my_simulation_checkpoint.json")
print("Brain state loaded successfully")
```

## Basic Usage

### 1. Understanding the Brain Architecture

The Brain class is the central coordinator of neural simulation. It manages
multiple subsystems through a manager-based architecture:

```python
# Brain subsystems
brain = pynlm.create_brain(config)

# Access individual subsystems
brain.memory     # Working memory system
brain.plasticity # Plasticity system
brain.neuromodulation # Neuromodulation system
brain.cognition  # Cognitive system
brain.prediction # Prediction system
brain.development # Development system
brain.visualizer # Visualization system
```

### 2. Configuring the Simulation

Configuration is managed through the Config class, which uses a flexible
key-value system with nested structures.

#### Basic Configuration

```python
config = pynlm.Config.create_default()

# Set individual parameters
config.set("simulation.neuron_count", 1000)
config.set("simulation.duration", 500.0)
config.set("simulation.timestep", 0.001)

# Configure plasticity
config.set("plasticity.learning_rate", 0.01)
config.set("plasticity.stdp_learning_rate", 0.02)

# Configure neuromodulation
config.set("neuromodulation.reward_sensitivity", 1.0)
config.set("neuromodulation.curiosity_sensitivity", 0.5)
```

#### Advanced Configuration

```python
# Create complex configuration from dictionary
config_dict = {
    "simulation": {
        "neuron_count": 2000,
        "timestep": 0.0005,
        "duration": 1000.0
    },
    "plasticity": {
        "learning_rate": 0.05,
        "stdp_learning_rate": 0.01,
        "reward_modulated": True
    },
    "neuromodulation": {
        "reward_sensitivity": 1.5,
        "curiosity_sensitivity": 0.8,
        "novelty_threshold": 0.4
    }
}

config = pynlm.Config.from_dict(config_dict)
```

### 3. Neuron Management

Neurons are created automatically based on configuration. You can access
individual neurons for detailed analysis:

```python
brain = pynlm.create_brain(config)
brain.initialize()

# Access neurons
neuron_count = len(brain.neurons)
print(f"Total neurons: {neuron_count}")

# Analyze individual neuron (example)
for i, neuron in enumerate(brain.neurons[:10]):  # First 10 neurons
    state = neuron.get_state_dict()
    print(f"Neuron {state['id']}: Type={state['type']}, "
          f"State={state['firing_state']}, "
          f"Potential={state['membrane_potential']:.2f}mV")
```

### 4. Memory System Usage

The memory system integrates three types of memory: working, episodic, and
associative.

#### Working Memory

```python
# Store activation in working memory
brain.memory.store_working_memory(neuron_id=1, activation=0.8, timestamp=10.0)
brain.memory.store_working_memory(neuron_id=2, activation=0.6, timestamp=11.0)

# Get working memory usage
usage = brain.memory.get_working_memory_usage()
print(f"Working memory traces: {usage}")
```

#### Episodic Memory

```python
# Store an episode (experience)
brain.memory.store_episode(
    sensory_input=[0.1, 0.2, 0.3, 0.4, 0.5],
    action_taken={"type": "move", "direction": "forward"},
    reward=1.0,
    neural_pattern=[0.8, 0.6, 0.9],
    timestamp=15.0
)

# Get episode count
episode_count = brain.memory.get_episode_count()
print(f"Episodic memory episodes: {episode_count}")
```

#### Associative Memory

```python
# Form associations between patterns
brain.memory.associate(pattern1_key=1, pattern2_key=2, strength=1.0)
brain.memory.store_pattern(key=1, pattern=[0.1, 0.2, 0.3])
brain.memory.store_pattern(key=2, pattern=[0.4, 0.5, 0.6])

# Retrieve associations
associations = brain.memory.retrieve_associations([0.1, 0.2, 0.3], top_k=5)
print(f"Retrieved associations: {associations}")
```

### 5. Plasticity System

Plasticity rules determine how synaptic weights change over time based on
neural activity.

```python
brain = pynlm.create_brain(config)
brain.initialize()

# Initialize plasticity system
brain.plasticity.initialize()

# Record spike pairs for STDP
# (In a real simulation, this would be triggered by actual spike events)
synapse = None  # This would be a real synapse object
brain.plasticity.record_spike_pair(synapse, pre_spike=100.0, post_spike=101.0)

# Get plasticity rule instances
stdp = brain.plasticity.get_stdp()
hebbian = brain.plasticity.get_hebbian()

print(f"STDP LTP weight: {stdp.get_ltp_weight():.4f}")
print(f"Hebbian learning rate: {hebbian.get_state()['learning_rate']:.4f}")
```

### 6. Neuromodulation

Neuromodulators (dopamine, novelty, curiosity) regulate learning and behavior.

```python
brain = pynlm.create_brain(config)
brain.initialize()

# Apply reward signal
brain.neuromodulation.apply_reward(1.0)

# Process novelty based on sensory input
brain.neuromodulation.update_novelty([0.1, 0.5, 0.9])

# Set neuromodulator levels
brain.neuromodulation.set_dopamine_level(0.7)
brain.neuromodulation.set_curiosity_level(0.4)

# Get neuromodulator state
state = brain.neuromodulation.get_state()
print(f"Dopamine: {state['dopamine']:.3f}, "
      f"Novelty: {state['novelty']:.3f}, "
      f"Curiosity: {state['curiosity']:.3f}")
```

### 7. Cognitive System

The cognitive system handles planning, concept formation, and attention.

```python
brain = pynlm.create_brain(config)
brain.initialize()

# Configure cognitive parameters
brain.cognition.set_planning_depth(3)
brain.cognition.set_attention_inhibition(0.7)
brain.cognition.set_attention_excitation(1.2)

# Get cognitive statistics
stats = brain.cognition.update(0.001)
cognitive_load = stats['cognitive_load']
active_concepts = brain.cognition.get_active_concept_count()

print(f"Cognitive load: {cognitive_load:.3f}")
print(f"Active concepts: {active_concepts}")
```

### 8. Prediction System

The prediction system generates predictions and computes prediction errors.

```python
brain = pynlm.create_brain(config)
brain.initialize()

# Process sensory input
sensory_input = [0.1, 0.2, 0.3, 0.4, 0.5]
brain.prediction.process_sensory_input(sensory_input)

# Generate prediction from current state
predicted_state = brain.prediction.generate_prediction(sensory_input)

# Calculate prediction error
error = brain.prediction.calculate_error(sensory_input, predicted_state)

print(f"Prediction error: {error:.3f}")
print(f"Prediction confidence: {brain.prediction.get_confidence():.3f}")
```

### 9. Development System

The development system manages developmental stages and critical periods.

```python
brain = pynlm.create_brain(config)
brain.initialize()

# Check current developmental stage
stage = brain.development.get_stage()
age = brain.development.get_age()
plasticity_mod = brain.development.get_plasticity_modulator()

print(f"Developmental stage: {stage}")
print(f"Age: {age:.1f}")
print(f"Plasticity modulator: {plasticity_mod:.3f}")

# Advance development
brain.development.advance_stage()
```

### 10. Experiment Management

The Experiment class provides a framework for controlled simulations.

```python
# Create experiment
brain = pynlm.create_brain(config)
brain.initialize()

experiment = pynlm.Experiment("my_experiment", brain)

# Add parameter sweep
experiment.add_parameter_sweep("learning_rate", [0.01, 0.05, 0.1])
experiment.add_parameter_sweep("novelty_threshold", [0.1, 0.3, 0.5])

# Run experiment
result = experiment.run("experiment_1")
print(f"Experiment completed in {result.duration:.2f} seconds")

# Run parameter sweep
results = experiment.run_parameter_sweep()
print(f"Completed parameter sweep with {len(results)} combinations")
```

## Advanced Topics

### 1. Custom Neuron Models

While NLM uses LIF neurons by default, you can extend the system with custom
neuron models by modifying the neuron module.

```python
# Example: Custom neuron with additional properties
class CustomNeuron:
    def __init__(self, neuron_id, neuron_type="Internal"):
        self.neuron_id = neuron_id
        self.neuron_type = neuron_type
        self.custom_property = 0.0
        # ... custom implementation
```

### 2. Advanced Plasticity Rules

You can implement custom plasticity rules by extending the PlasticityRule class.

```python
class CustomPlasticity(PlasticityRule):
    def update(self, dt: float):
        # Custom plasticity implementation
        pass
```

### 3. Multi-Region Simulations

Configure simulations with multiple brain regions:

```python
config = pynlm.Config.create_default()
config.set("regions.sensory.neuron_count", 200)
config.set("regions.cortex.neuron_count", 500)
config.set("regions.motor.neuron_count", 300)

brain = pynlm.create_brain(config)
brain.initialize()
```

### 4. Real-time Control

Use the CLI interface for real-time control:

```bash
# Run simulation
pynlm create-brain run-simulation --steps 1000 --dt 0.001

# Create visualizations
pynlm create-visualizations --brain-file checkpoint.json

# Run experiment
pynlm run-experiment --experiment-id my_exp --run-id run1
```

### 5. Performance Optimization

For large-scale simulations:

```python
# Use smaller timestep for better accuracy
config.set("simulation.timestep", 0.0001)

# Limit neuron count for testing
config.set("simulation.neuron_count", 100)

# Use sparse connectivity
config.set("connectivity.sparse", True)
config.set("connectivity.sparsity", 0.95)
```

## Troubleshooting

### Common Issues and Solutions

#### 1. "Brain not initialized"

**Error:** `RuntimeError: Brain not initialized. Call initialize() first.`

**Solution:** Always call `brain.initialize()` before running simulation.

```python
brain = pynlm.create_brain(config)
brain.initialize()  # This is required!
for step in range(100):
    brain.step(step)  # This will work now
```

#### 2. Missing Dependencies

**Error:** ImportError for numpy or matplotlib

**Solution:** Install optional dependencies:

```bash
pip install pynlm[all]
```

Or install individually:
```bash
pip install numpy matplotlib seaborn
```

#### 3. Configuration Errors

**Error:** Invalid configuration values

**Solution:** Validate configuration before use:

```python
config = pynlm.Config.create_default()
validation = config.validate()
if not validation.is_valid:
    print(f"Configuration errors: {validation.errors}")
```

#### 4. Simulation Memory Issues

**Problem:** Simulation crashes due to memory constraints

**Solution:**

```python
# Reduce simulation size
config.set("simulation.neuron_count", 100)
config.set("simulation.duration", 100.0)

# Use sparse connectivity
config.set("connectivity.sparse", True)
```

#### 5. Visualization Issues

**Problem:** Plots not generated or saving fails

**Solution:**

```python
# Check if matplotlib is available
if not pynlm.MATPLOTLIB_AVAILABLE:
    print("Matplotlib not available. Install with: pip install matplotlib")

# Try different save formats
visualizer = pynlm.Visualizer(save_plots=True)
visualizer.plot_all(brain, output_prefix="test", format="png")
```

### Debug Mode

For debugging simulations:

```python
import logging

# Set up logging
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')

# Create brain and run with debug output
brain = pynlm.create_brain(config)
brain.initialize()

# Run a few steps with detailed logging
for step in range(10):
    brain.step(step)
    stats = brain.get_stats()
    print(f"Step {step}: {stats}")
```

## References

### Key Papers and Resources

1. **Neural Dynamics and Learning**
   - Gerstner, W., Kistler, W., Naud, R., & Paninski, L. (2014). *Neuronal Dynamics*. Cambridge University Press.

2. **Spike-Timing-Dependent Plasticity**
   - Song, S., Miller, K. D., & Abbott, L. F. (2000). "Competitive Hebbian learning through spike-timing-dependent synaptic plasticity". *Neural Computation*, 12(3), 529-541.

3. **Neuromodulation**
   - Robinson, T. E., & Berridge, K. C. (1995). "The role of dopamine in reward and learning". *Handbook of Chemical Neuroanatomy*, 15, 371-414.

4. **Prediction Error**
   - Schultz, W., Dayan, P., & Montague, P. R. (1997). "A neural substrate of prediction errors in the monkey striatum". *Journal of Neurophysiology*, 80(1), 289-304.

5. **Memory Systems**
   - Squire, L. R., & Zola-Morgan, S. (1996). "Memory systems in the brain". *Annual review of neuroscience*, 19(1), 217-242.

### Online Resources

- **NLM Documentation**: https://nlm-project.org/documentation
- **Python Neuroscience Ecosystem**: https://python-neuroscience.org
- **Machine Learning for Neuroscience**: https://ml4neuro.org

### Related Projects

- **Brian2**: https://brian2.org
- **NEURON**: https://neuron.yale.edu
- **PyNEST**: https://nest-simulator.org
- **Nengo**: https://www.nengo.ai

## License

This project is licensed under the MIT License.

## Copyright

Copyright (c) 2024 NLM Project. All rights reserved.

---

**Need Help?**

- Visit the [NLM Documentation](https://nlm-project.org/documentation)
- Check the [Examples](examples/) directory
- Report issues at [GitHub Issues](https://github.com/your-repo/nlm-python-bindings/issues)
- Join the discussion on [Discord](https://discord.gg/nlm)

**Acknowledgments**

Special thanks to the contributors and maintainers who have helped develop and maintain this framework.

---

*Generated by Kilo AI. Visit https://kilo.ai for more information.*
