# =============================================================================
# NLM (Neural Learning Machine) - Getting Started Guide
# =============================================================================

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that creates a neural system beginning in a primitive developmental state and acquiring increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Status: Phase 6 - Final Integration

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

## Installation and Building

### Prerequisites
- C++ compiler with C++20 support
- CMake 3.16 or higher
- Python 3.8+ (for bindings)

### Quick Build

1. Clone the repository
2. Create build directory:
```bash
mkdir build && cd build
```

3. Configure and build:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

4. Run Phase 6 integration demo:
```bash
./nlm_phase6_demo
```

### Building with Python Bindings

To build the Python bindings (recommended for programmatic access):

```bash
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON -DPYTHON_EXECUTABLE=$(which python3)
make -j4
```

The Python bindings will be installed to:
- `pynlm_original`: Original comprehensive bindings
- `pynlm_enhanced`: Enhanced Pythonic bindings with better API

## Using NLM

### Basic Usage Examples

#### 1. Running the Built-in Demo

The simplest way to see NLM in action is to run the Phase 6 integration demo:

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

#### 2. Using Python API

```python
import pynlm_enhanced as nlm

# Create configuration
config = nlm.Config()

# Set simulation parameters
config.set("neuron_count", 1000)
config.set("random_seed", 42)

# Create brain
brain = nlm.create_brain(config)

# Initialize brain
if brain.initialize():
    print("Brain initialized successfully")
    
    # Run simulation
    for step in range(1000):
        brain.step(step, step * 0.001)
    
    print(f"Total spikes: {brain.get_total_spike_count()}")
    print(f"Active neurons: {brain.get_active_neuron_count()}")
    
    # Save brain state
    brain.save("nlm_brain_checkpoint.pkl")
else:
    print("Failed to initialize brain")
```

#### 3. Using Simple World and Agent

```python
import pynlm_enhanced as nlm

# Create world
world = nlm.create_simple_world()

# Configure world (width, height, vision dimensions)
world.configure(100.0, 100.0, 20, 20)

# Create brain and agent
brain = nlm.create_brain()
agent = nlm.create_agent_brain(brain)

# Initialize
agent.initialize(world)

# Main simulation loop
for step in range(1000):
    # Get sensory input from world
    percept = world.get_sensory_percept()
    
    # Process sensory input
    agent.process_sensory_input(percept)
    
    # Step brain
    brain.step(step, step * 0.1)
    
    # Get motor command
    motor_cmd = agent.decode_motor_command()
    
    # Apply to world
    result = world.apply_motor_command(motor_cmd, step * 0.1)
    
    if step % 100 == 0:
        print(f"Step {step}: Reward={result.reward}, Energy={world.get_agent_body().energy}")
```

### Advanced Usage

#### 1. Custom Simulation Configuration

```python
config = nlm.Config()

# Override with custom parameters
config.set("neuron_count", 5000)
config.set("region_count", 4)
config.set("connection_probability", 0.05)
config.set("random_seed", 12345)
config.set("simulation_timestep", 0.01)

# STDP parameters
config.set("stdp_ltp_weight", 0.03)
config.set("stdp_ltd_weight", 0.03)
config.set("stdp_tau", 20.0)

# Structural plasticity
config.set("synaptogenesis_rate", 0.0001)
config.set("pruning_rate", 0.00001)

# Create brain with custom config
brain = nlm.create_brain(config)
```

#### 2. Memory System Access

```python
# Access working memory
working_memory = brain.get_working_memory()
if working_memory:
    print(f"Working memory capacity: {working_memory.get_capacity()}")
    print(f"Active traces: {working_memory.get_active_traces()}")

# Access episodic memory
episodic_memory = brain.get_episodic_memory()
if episodic_memory:
    print(f"Episodic memory episodes: {episodic_memory.get_episode_count()}")
    print(f"Max episodes: {episodic_memory.get_max_episodes()}")

# Access prediction system
prediction_system = brain.get_prediction_system()
if prediction_system:
    print("Prediction system active")
```

#### 3. Neuromodulation Control

```python
agent_brain = nlm.create_agent_brain(brain)

# Enable/disable neuromodulation
agent_brain.enable_reward_modulation(True)
agent_brain.enable_curiosity(True)
agent_brain.enable_structural_plasticity(True)

# Apply reward
agent_brain.apply_reward_modulation(reward=1.0, predicted_reward=0.8)

# Monitor neuromodulation levels
print(f"Neuromodulation level: {agent_brain.get_neuromodulation_level()}")
print(f"Curiosity level: {agent_brain.get_curiosity_level()}")
print(f"Novelty level: {agent_brain.get_novelty_level()}")
print(f"Prediction error: {agent_brain.get_prediction_error()}")
```

### Working with Different Brain Types

#### 1. Basic Brain
```python
# Simple brain with default configuration
brain = nlm.create_default_config()
brain = nlm.create_brain(brain)
```

#### 2. Agent Brain
```python
# Brain with agent interface for world interaction
brain = nlm.create_brain()
agent_brain = nlm.create_agent_brain(brain)
agent_brain.initialize(world)
```

### Simulation Control and Data Collection

#### 1. Checkpointing and Saving
```python
# Save brain state for later use
brain.save("my_brain_checkpoint.pkl")

# Load saved brain state
brain.load("my_brain_checkpoint.pkl")
```

#### 2. Real-time Monitoring
```python
# Get brain status information
brain.log_status()

# Get statistics
print(f"Total neurons: {brain.get_total_neuron_count()}")
print(f"Total synapses: {brain.get_total_synapse_count()}")
print(f"Active neurons: {brain.get_active_neuron_count()}")
print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
print(f"E/I ratio: {brain.get_excitation_inhibition_ratio():.3f}")
```

## Development Features

### 1. Developmental Stages
```python
# Set developmental stage
brain.set_developmental_stage(nlm.DevelopmentalStage.Maturation)

# Monitor development
stage = brain.get_developmental_stage()
print(f"Developmental stage: {stage}")
```

### 2. Plasticity Control
```python
# Get plasticity systems
stdp = brain.get_stdp()
hebbian = brain.get_hebbian()
structural_plasticity = brain.get_structural_plasticity()

# Configure STDP parameters
if stdp:
    stdp.configure(0.02, 0.015, 20.0)
```

### 3. Sensory and Motor Systems
```python
# Create sensory input
vision = nlm.Vision(width=20, height=20)
touch = nlm.Audio(sample_rate=44100, num_samples=1024)

# Create action
action = nlm.Action(nlm.ActionType.MoveForward)
action.set_parameters([1.0, 0.5, 0.0])

# Get sensory percept from world
sensory_percept = world.get_sensory_percept()
```

## Configuration Management

### 1. Configuration File

The default configuration file `configs/default.cfg` contains:

```
# Default configuration for NLM simulation
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
replay_interval = 100
consolidation_interval = 1000
```

### 2. Command Line Configuration

NLM supports command-line configuration:

```bash
./nlm_phase6_demo --config configs/custom.cfg
./nlm_phase6_demo --neuron_count 5000 --random_seed 123
```

## Advanced Python Features

### 1. Python Callbacks and Extensions

The Python bindings support custom extensions:

```python
def custom_plasticity_callback(synapse, pre_spike_time, post_spike_time):
    # Custom plasticity implementation
    pass

# Register custom callback
# (See enhanced bindings for details)
```

### 2. Visualization Support

The enhanced bindings include visualization utilities:

```python
# Create simple visualization (requires matplotlib/pil)
import pynlm_enhanced as nlm
from nlm_viz import visualize_brain_activity

brain = nlm.create_brain()
brain.initialize()

# Visualize neuron activity
visualize_brain_activity(brain, duration=1000)
```

## Common Issues and Solutions

### 1. "Failed to initialize brain"

**Solution:** Check that your configuration is valid and all dependencies are available.

### 2. Python Import Errors

**Solution:** Ensure Python bindings were built and installed:

```bash
python3 -c "import pynlm_enhanced; print('Import successful')"
```

### 3. Memory Issues

**Solution:** Reduce `neuron_count` and `region_count` in your configuration for smaller systems.

## Further Reading

- **Documentation:** See the `docs/` directory for comprehensive technical documentation
- **Source Code:** All source files are documented with Doxygen-style comments
- **Examples:** Look in `examples/` for additional usage patterns
- **Community:** Join the NLM discussion forums for questions and collaboration

## License

MIT License - See LICENSE file for details.

## Contributing

NLM welcomes contributions from the community. Please see CONTRIBUTING.md for guidelines on:
- Code style
- Testing
- Documentation
- Feature requests

## Support

For questions, issues, or discussions, visit the NLM GitHub repository.
