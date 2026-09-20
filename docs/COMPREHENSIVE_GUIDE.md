# NLM - Comprehensive User Guide

This guide provides a complete introduction to NLM (Neural Learning Machine), covering everything from basic setup to advanced usage and configuration.

## Table of Contents

1. [What is NLM?](#what-is-nlm)
2. [Getting Started](#getting-started)
   - [Installation](#installation)
   - [Building from Source](#building-from-source)
   - [Running Your First Simulation](#running-your-first-simulation)
3. [Core Concepts](#core-concepts)
   - [Brain Architecture](#brain-architecture)
   - [Memory Systems](#memory-systems)
   - [Neuromodulation](#neuromodulation)
   - [Development](#development)
4. [Using the Python API](#using-the-python-api)
   - [Basic Operations](#basic-operations)
   - [Complete Agent Example](#complete-agent-example)
   - [Configuration Management](#configuration-management)
5. [Advanced Features](#advanced-features)
   - [Memory Management](#memory-management)
   - [Plasticity Rules](#plasticity-rules)
   - [Development Stages](#development-stages)
   - [Checkpointing and Saving](#checkpointing-and-saving)
6. [Configuration Reference](#configuration-reference)
7. [Performance Optimization](#performance-optimization)
8. [Troubleshooting](#troubleshooting)
9. [Experiment API](#experiment-api)
10. [Extending NLM](#extending-nlm)

---

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Key Features

- **Experimental Platform**: A research project investigating brain-like systems
- **Spiking Neural Network**: Implements Leaky Integrate-and-Fire (LIF) neurons with real neural dynamics
- **Event-Driven Processing**: Efficient spike propagation with synaptic delays
- **Integrated Systems**: Memory, prediction, cognition, and neuromodulation systems
- **Developmental Stages**: Brain development from initial to adult stages
- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, and novelty detection
- **Plasticity**: STDP and Hebbian learning rules

### What NLM IS

- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

### What NLM IS NOT

- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

---

## Getting Started

### Installation

NLM can be used in two ways:

1. **Install from PyPI**: `pip install pynlm`
2. **Build from Source**: Clone the repository and build with CMake

### Building from Source

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4
```

### Running Your First Simulation

There are several ways to get started:

#### Using Python (Easiest)

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for 100 steps
for i in range(100):
    brain.step(i)

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

#### Using C++ CLI

```bash
# Run the basic demo
./nlm --phase2

# Run integration demo
./nlm_phase6_demo
```

#### Using NLM CLI

```bash
# Install NLM CLI
chmod +x scripts/nlm
./scripts/nlm demo --phase2
```

---

## Core Concepts

### Brain Architecture

NLM's brain is organized into regions, populations, and neurons:

- **Regions**: Functional brain regions (Cortex, Hippocampus, Amygdala, etc.)
- **Populations**: Groups of neurons with similar types and functions
- **Neurons**: Individual processing units (LIF neurons)

Neuron types:
- **Sensory**: Receive input from environment
- **Internal**: Process and integrate information
- **Motor**: Generate actions/output
- **Modulatory**: Regulate neural excitability

### Memory Systems

NLM has three integrated memory systems:

1. **Working Memory**: Maintains active neural activity patterns
2. **Episodic Memory**: Stores experiences as episodes
3. **Associative Memory**: Forms pattern associations

### Neuromodulation

Neuromodulation affects neural excitability and plasticity:

- **Dopamine**: Reward prediction and reinforcement
- **Curiosity**: Drives exploration and novelty seeking
- **Novelty**: Detects unexpected events
- **Prediction Error**: Learning from prediction mismatches

### Development

NLM brains develop through stages:

1. **Initial**: High plasticity, rapid learning
2. **Critical Period**: Sensitive period for learning
3. **Maturation**: Stabilizing connections
4. **Adult**: Refined, stable network
5. **Aging**: Gradual decline in plasticity

---

## Using the Python API

### Basic Operations

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Optional: Modify configuration
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Example

This example demonstrates a complete agent using NLM to interact with an environment:

```python
import pynlm
import time

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

### Configuration Management

```python
import pynlm
import json

# Create configuration
config = pynlm.createDefaultConfig()

# Check for key
if config.has("brain.neuron_count"):
    neurons = config.getOr<int64_t>("brain.neuron_count", 1000)
    print(f"Current neuron count: {neurons}")

# Get all keys
keys = config.getKeys()
print(f"Available keys: {keys}")

# Save configuration
config.saveToFile("my_config.json")

# Load configuration from file
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")
```

---

## Advanced Features

### Memory Management

Working memory maintains active neural traces:

```python
# Get working memory traces
working_memory = brain.getWorkingMemory()
if working_memory:
    traces = working_memory.getActiveTraces()
    print(f"Working memory traces: {traces}")
```

Episodic memory stores experiences:

```python
# Get episodic memory
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    episodes = episodic_memory.getEpisodeCount()
    print(f"Episodic memory episodes: {episodes}")
    
    # Get episodes for replay
    to_replay = episodic_memory.getEpisodesForReplay(5)
```

### Plasticity Rules

STDP (Spike-Timing-Dependent Plasticity):

```python
# Configure STDP
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)  # LTP time constant
config.set("plasticity.stdp.tau_minus", 20.0) # LTD time constant
```

Hebbian learning:

```python
# Configure Hebbian
config.set("plasticity.hebbian.enable", True)
```

Structural plasticity:

```python
# Configure structural changes
config.set("plasticity.structural.enable", True)
config.set("synaptogenesis_rate", 0.0001f)
config.set("pruning_rate", 0.00001f)
```

### Development Stages

Development affects plasticity rates over time:

```python
# Check developmental stage
stage = brain.getDevelopmentalStage()
if stage == pynlm.DevelopmentalStage::Initial:
    print("Brain in high plasticity stage")
elif stage == pynlm.DevelopmentalStage::Adult:
    print("Brain in stable adult stage")
```

### Checkpointing and Saving

Save brain state for later restoration:

```python
# Save brain state
brain.save("checkpoint.bin")

# Load brain state
brain2 = pynlm.createBrain(config)
brain2.initialize()
brain2.load("checkpoint.bin")
```

---

## Configuration Reference

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Simulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation_timestep` | float | 0.001 | Simulation time step (s) |
| `random_seed` | int64 | 42 | Random seed for reproducibility |
| `replay_interval` | size_t | 100 | Replay interval (steps) |
| `consolidation_interval` | size_t | 1000 | Memory consolidation interval |

---

## Performance Optimization

### Memory Management

NLM uses efficient memory pools and event queues:

- **Memory Pools**: Pre-allocated memory for neurons and synapses
- **Event Queues**: Asynchronous spike and event processing
- **Sparse Connectivity**: Only store actual connections

### Parallel Processing

NLM supports parallel computation:

- **SIMD Vectorization**: Parallel processing of neuron dynamics
- **Multi-threading**: Parallel simulation steps
- **Event-driven**: Only process events that occur

### Caching and Optimization

- **Neuron Caching**: Frequently accessed neuron data cached
- **Synaptic Compression**: Compressed synapse representations
- **Lazy Evaluation**: Compute only when needed

---

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"**

- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that neurons have proper input

**"The agent isn't moving"**

- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure motor neurons are firing

**"Everything is 0"**

- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that configuration values are reasonable

### Error Messages

**CMake cannot find Python:**

```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**ImportError: No module named 'pynlm':**

```bash
pip install --force-reinstall .
```

**Segmentation faults:**

- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

---

## Experiment API

NLM provides an experiment framework for systematic testing:

### Phase 6 Integration Experiment

The Phase 6 integration experiment tests all systems:

```python
import pynlm

# Create experiment
experiment = pynlm.createPhase6Experiment()

# Run integration verification
if experiment.verifyIntegration():
    print("Integration successful!")
else:
    print("Integration failed!")

# Run full experiment
config = pynlm.createPhase6Config()
config.neuronCount = 500
config.maxSteps = 1000

result = experiment.run(config)

print(f"Experiment completed!")
print(f"Total reward: {result.totalReward}")
print(f"Integration status: {result.predictionIntegrated}")
```

### Experiment Results

The `ExperimentResult` struct contains:

- **success**: Whether experiment succeeded
- **totalReward**: Total reward accumulated
- **averageFiringRate**: Average neural firing rate
- **memoryEpisodesStored**: Number of episodes stored
- **dopamineLevel**: Current dopamine level
- **Integration flags**: Whether each system integrated properly

---

## Extending NLM

### Adding New Neuron Types

```cpp
// In Neuron.hpp
enum class NeuronType {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal,
    // Add new type here
    Custom
};

// In Neuron.cpp
// Add handling for new type in initialization and dynamics
```

### Adding New Plasticity Rules

```cpp
// Create new plasticity class inheriting from PlasticityRule
class MyPlasticityRule : public PlasticityRule {
public:
    void update(Synapse* syn, const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes, double timestep) override {
        // Implement custom plasticity rule
    }
};

// Register in Brain.cpp
pImpl->myPlasticity = std::make_unique<MyPlasticityRule>();
```

### Adding Custom Sensory Modalities

```cpp
// Create new SensoryInput subclass
class CustomSensory : public SensoryInput {
public:
    void setData(const std::vector<float>& data) override {
        // Handle custom sensory data
    }
    
    SensorType getType() const override {
        return SensorType::Custom;
    }
};
```

---

## License

MIT License

---

## Authors

Research project — See docs/ for scientific background.
