# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

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

## Python API Overview

NLM provides a comprehensive Python interface (`pynlm`) for simulating and experimenting with artificial developmental brains. The enhanced Phase 6 API includes:

### Key Python Features:

1. **Enhanced Configuration Management**
   - Pythonic configuration setters and getters
   - Dictionary-based configuration
   - Validation helpers

2. **Simplified Simulation Control**
   - High-level simulation runners
   - Episode management
   - Learning loop automation

3. **Comprehensive Agent-Environment Integration**
   - Complete agent-world interaction APIs
   - Reward tracking and neuromodulation
   - Developmental stage management

4. **Advanced Experimentation Tools**
   - Phase 3/4 experiment helpers
   - Training setup utilities
   - Data collection and analysis

5. **Improved Error Handling**
   - Pythonic exception types
   - Descriptive error messages
   - Validation feedback

## Python API Usage Examples

### Basic Brain Simulation

```python
import pynlm

# Create and initialize a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
print(f"Simulation complete. Firing neurons: {brain.getFiringNeuronCount()}")
```

### Enhanced Configuration Management

```python
import pynlm

# Create Pythonic configuration
config = pynlm.createDefaultConfig()

# Set values using Pythonic API
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.5)

# Load configuration from dictionary
config_dict = {
    "brain.neuron_count": 500,
    "brain.synapse_density": 0.1,
    "plasticity.stdp.learning_rate": 0.001,
    "plasticity.hebbian.enable": True
}
config.setMultiple(config_dict)

# Get values with defaults
neuron_count = config.get("brain.neuron_count", 1000)
learning_rate = config.get("plasticity.stdp.learning_rate", 0.001)

# Convert to dictionary for inspection
config_dict = config.toDict()
```

### Complete Agent-World Simulation

```python
import pynlm

# Create complete simulation setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run complete episode
episode_reward = 0.0
for step in range(500):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if hasattr(world, 'getActionResult'):
        result = world.getActionResult()
        agent.applyRewardModulation(result.reward, 0.0)
        episode_reward += result.reward

print(f"Episode completed. Total reward: {episode_reward:.2f}")
print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
```

### Phase 3 Style Learning Experiment

```python
import pynlm

# Run a Phase 3 style learning experiment (plasticity demonstration)
results = pynlm.runPhase3StyleExperiment(1000)

print("Phase 3 Learning Experiment Results:")
print(f"  Total spikes: {results['total_spikes']}")
print(f"  Final firing rate: {results['final_firing_rate']:.4f}")
print(f"  Final E/I ratio: {results['final_eir']:.4f}")
print(f"  Learning detected: {results['learning_detected']}")
```

### Phase 4 Style Agent Experiment

```python
import pynlm

# Run a Phase 4 style complete agent experiment
results = pynlm.runPhase4StyleExperiment(1000)

print("Phase 4 Agent Experiment Results:")
print(f"  Episode score: {results['episode_score']:.2f}")
print(f"  Final curiosity: {results['final_curiosity']:.3f}")
print(f"  Final novelty: {results['final_novelty']:.3f}")
print(f"  Final prediction error: {results['final_prediction_error']:.4f}")
print(f"  Developmental stage: {results['final_developmental_stage']}")
print(f"  Brain neurons: {results['brain_neuron_count']}")
```

### Simple Simulation Runner

```python
import pynlm

# Run a simple simulation with activity analysis
stats = pynlm.runSimpleSimulation(2000)

print("Simulation Activity Analysis:")
for key, value in stats.items():
    print(f"  {key}: {value}")
```

### Agent-World Simulation

```python
import pynlm

# Create environment and run agent simulation
world = pynlm.createSimpleWorld()
world.configure(15, 15, 8, 8)
world.reset()
world.setAgentStart(7.5, 7.5)

# Add some resources and hazards
world.addObject(pynlm.WorldObject(3, 3, pynlm.WorldObjectType.Resource, 5.0, 0.5))
world.addObject(pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Hazard, 2.0, 0.3))

results = pynlm.runAgentWorldSimulation(world, 500)

print("Agent-World Simulation Results:")
print(f"  Total reward: {results['total_reward']:.2f}")
print(f"  Final curiosity: {results['final_curiosity']:.3f}")
print(f"  Final novelty: {results['final_novelty']:.3f}")
```

### Training Setup

```python
import pynlm

# Create complete training setup
training_setup = pynlm.createCompleteTrainingSetup()

print("Training Setup Created:")
print(f"  Number of configurations: {len(training_setup['configs'])}")
print(f"  World size: {training_setup['world'].getWidth()}x{training_setup['world'].getHeight()}")
print(f"  Description: {training_setup['description']}")
```

## Advanced Usage

### Neural Activity Monitoring

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Monitor neuron activity by type
monitoring = brain.monitorNeurons(pynlm.NeuronType.Sensory, 20)
print("Neuron Monitoring:", monitoring)

# Get comprehensive activity statistics
stats = brain.getActivityStats()
print("Activity Statistics:", stats)
```

### Development Management

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Advance development stages
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
print(f"Brain developmental stage: {brain.getDevelopmentalStage()}")

# Run development simulation
for step in range(100):
    brain.step(step, step * 0.1)
    brain.developStage(0.1)
```

### Data Collection and Analysis

```python
import pynlm

# Create environment and collect training data
world = pynlm.createSimpleWorld()
world.configure(10, 10, 6, 6)
world.reset()

# Collect training data
training_data = world.createTrainingData(5, 200)

print(f"Training data collected:")
print(f"  Episodes: {training_data['total_episodes']}")
print(f"  Steps per episode: {training_data['steps_per_episode']}")
```

## Configuration Options

The following configuration parameters can be set using the Python API:

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Constants and Configuration

The module provides useful constants:

```python
import pynlm

print(f"Default simulation timestep: {pynlm.DEFAULT_SIMULATION_TIMESTEP}")
print(f"Default brain neurons: {pynlm.DEFAULT_BRAIN_NEURONS}")
print(f"Default world size: {pynlm.DEFAULT_WORLD_WIDTH}x{pynlm.DEFAULT_WORLD_HEIGHT}")
```

## Version Information

- **Version**: Phase 6 Enhanced Python API
- **Author**: NLM Project
- **Description**: Enhanced Python bindings for NLM with improved APIs and documentation
- **License**: MIT

## Requirements

- Python 3.8+
- C++20 compiler
- pybind11 2.11.0+
- scikit-build-core
- NumPy (optional, for advanced data handling)

## Building from Source

See the main README for C++ build instructions, or use the Python setup:

```bash
pip install -e .
```

## Phase 6 Integration

Phase 6 represents the final integration of all NLM systems into a coherent artificial brain. The enhanced Python API provides:

1. **Complete System Integration**: All brain systems (memory, prediction, cognition, neuromodulation) working together
2. **Developmental Integration**: Developmental stages affecting plasticity rates and learning
3. **Persistence**: Checkpoint save/load capabilities
4. **Replay and Consolidation**: Episodic memory replay during simulation
5. **Real Neural Computation**: LIF neuron dynamics with event-driven spike propagation

The Python API maintains the full complexity and capabilities of the C++ implementation while providing Pythonic interfaces for easier use and experimentation.

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── python/              # Python bindings
│   ├── bindings.cpp     # Enhanced Python bindings
│   └── CMakeLists.txt   # Python module build
├── src/                 # C++ source code
│   ├── brain/           # Neural components
│   ├── memory/          # Memory systems
│   ├── neuromodulation/  # Neuromodulators
│   ├── prediction/      # Prediction systems
│   ├── cognition/       # Cognitive mechanisms
│   └── ...              # Other systems
├── tests/               # Unit tests
└── docs/                # Documentation
    ├── ARCHITECTURE.md
    ├── SCIENCE.md
    └── ...
```

## License

MIT

## Authors

Research project — See docs for scientific background.
