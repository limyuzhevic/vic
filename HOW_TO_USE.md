# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the enhanced NLM Python API

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

---

## Part 3: Using the Enhanced NLM Python Library

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Creating a Simple World

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)
```

### Enhanced Usage Examples

#### Complete Agent Example

```python
import pynlm

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
        world.update(timestep=0.1)
        
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
        reward = world.getActionResult().reward
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

#### Enhanced Configuration Management

```python
import pynlm

# Create configuration with Pythonic API
config = pynlm.createDefaultConfig()

# Set values using intuitive Python syntax
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.001)

# Load from dictionary for easy configuration
config_dict = {
    "brain.neuron_count": 500,
    "brain.synapse_density": 0.1,
    "neuromod.curiosity.enable": True,
    "plasticity.hebbian.enable": True
}
config.setMultiple(config_dict)

# Convert back to dictionary for analysis
config_output = config.toDict()
print("Final configuration:", config_output)
```

#### Phase 3 Style Learning Experiment

```python
import pynlm

# Run a Phase 3 style learning experiment (plasticity demonstration)
print("Running Phase 3 Learning Experiment...")

results = pynlm.runPhase3StyleExperiment(1000)

print("Experiment Results:")
print(f"  Total spikes: {results['total_spikes']}")
print(f"  Final firing rate: {results['final_firing_rate']:.4f}")
print(f"  Final E/I ratio: {results['final_eir']:.4f}")
print(f"  Learning detected: {results['learning_detected']}")

if results['learning_detected']:
    print("✓ Plasticity learning was successful!")
else:
    print("! No significant learning detected - check parameters")
```

#### Phase 4 Style Cognitive Experiment

```python
import pynlm

# Run a Phase 4 style complete agent experiment (cognition demonstration)
print("Running Phase 4 Cognitive Experiment...")

results = pynlm.runPhase4StyleExperiment(1000)

print("Cognitive Experiment Results:")
print(f"  Episode score: {results['episode_score']:.2f}")
print(f"  Final curiosity: {results['final_curiosity']:.3f}")
print(f"  Final novelty: {results['final_novelty']:.3f}")
print(f"  Final prediction error: {results['final_prediction_error']:.4f}")
print(f"  Developmental stage: {results['final_developmental_stage']}")
print(f"  Brain complexity: {results['brain_neuron_count']} neurons")
```

#### Simple Simulation Runner

```python
import pynlm

# Quick simulation with activity analysis
print("Running simple simulation...")

stats = pynlm.runSimpleSimulation(2000)

print("Simulation Activity Analysis:")
for key, value in stats.items():
    if isinstance(value, dict):
        print(f"  {key}:")
        for subkey, subvalue in value.items():
            print(f"    {subkey}: {subvalue}")
    else:
        print(f"  {key}: {value}")
```

#### Agent-World Simulation with Environment

```python
import pynlm

# Create rich environment for agent learning
world = pynlm.createSimpleWorld()
world.configure(15, 15, 8, 8)
world.reset()
world.setAgentStart(7.5, 7.5)

# Add environmental features
world.addObject(pynlm.WorldObject(3, 3, pynlm.WorldObjectType.Resource, 10.0, 0.5))
world.addObject(pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Hazard, 2.0, 0.3))
world.addObject(pynlm.WorldObject(12, 8, pynlm.WorldObjectType.Marker, 1.0, 0.2))

# Run agent simulation with learning
print("Running agent in rich environment...")
results = pynlm.runAgentWorldSimulation(world, 500)

print("Environment Simulation Results:")
print(f"  Total reward collected: {results['total_reward']:.2f}")
print(f"  Exploration level (curiosity): {results['final_curiosity']:.3f}")
print(f"  Response to novelty (novelty): {results['final_novelty']:.3f}")
print(f"  Learning rate (neuromodulation): {results['learning_rate']:.3f}")
```

#### Complete Training Setup

```python
import pynlm

# Create a complete training setup with multiple configurations
print("Creating training setup...")

training_setup = pynlm.createCompleteTrainingSetup()

print("Training Setup Created:")
print(f"  Number of configurations: {len(training_setup['configs'])}")
print(f"  World size: {training_setup['world'].getWidth()}x{training_setup['world'].getHeight()}")
print(f"  Description: {training_setup['description']}")

# Examine configurations
for i, config in enumerate(training_setup['configs']):
    print(f"\nConfiguration {i+1}:")
    print(f"  Neurons: {config.get('brain.neuron_count', 'default')}")
    print(f"  Synapse density: {config.get('brain.synapse_density', 'default')}")
    print(f"  STDP enabled: {config.get('plasticity.stdp.enable', 'default')}")
```

#### Comprehensive Monitoring and Analysis

```python
import pynlm

# Create brain and enable monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get comprehensive activity statistics
stats = brain.getActivityStats()
print("Brain Activity Statistics:")
print(f"  Total neurons: {stats['total_neuron_count']}")
print(f"  Total synapses: {stats['total_synapse_count']}")
print(f"  Active neurons: {stats['active_neuron_count']}")
print(f"  Firing neurons: {stats['firing_neuron_count']}")
print(f"  Average firing rate: {stats['average_firing_rate']:.4f}")
print(f"  Excitation/Inhibition ratio: {stats['excitation_inhibition_ratio']:.4f}")
print(f"  Total spikes: {stats['total_spike_count']}")

# Run simulation to collect more data
for step in range(100):
    brain.step(step)

# Get updated statistics
updated_stats = brain.getActivityStats()
print("\nAfter 100 simulation steps:")
print(f"  Total spikes: {updated_stats['total_spike_count']}")
print(f"  Firing rate: {updated_stats['average_firing_rate']:.4f}")
```

#### Agent Neuroscience Metrics

```python
import pynlm

# Create and run agent to collect neuroscience data
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(10, 10, 6, 6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run learning episode
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(world.getActionResult().reward, 0.0)
    agent.updateDevelopment(0.1)

# Get neuroscience-inspired metrics
neuroscience_metrics = agent.getNeuroscienceMetrics()

print("Neuroscience Metrics Analysis:")
print(f"  Curiosity level (exploration drive): {neuroscience_metrics['curiosity']:.3f}")
print(f"  Novelty level (response to new things): {neuroscience_metrics['novelty']:.3f}")
print(f"  Prediction error (learning signal): {neuroscience_metrics['prediction_error']:.4f}")
print(f"  Neuromodulation level (learning rate): {neuroscience_metrics['neuromodulation_level']:.3f}")
print(f"  Developmental stage: {neuroscience_metrics['developmental_stage']}")
print(f"  Plasticity enabled: {neuroscience_metrics['plasticity_enabled']}")
print(f"  Reward learning enabled: {neuroscience_metrics['reward_modulation_enabled']}")
```

#### Development and Learning Analysis

```python
import pynlm

# Create agent and track development over time
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(8, 8, 6, 6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Track development over multiple episodes
development_history = []

for episode in range(5):
    # Run episode
    reward = pynlm.runAgentEpisode(agent, world, 100)
    
    # Record development metrics
    metrics = agent.getNeuroscienceMetrics()
    development_history.append({
        'episode': episode + 1,
        'reward': reward,
        'curiosity': metrics['curiosity'],
        'novelty': metrics['novelty'],
        'developmental_stage': metrics['developmental_stage'],
        'prediction_error': metrics['prediction_error']
    })

print("Development History:")
for record in development_history:
    print(f"  Episode {record['episode']}:")
    print(f"    Reward: {record['reward']:.2f}")
    print(f"    Curiosity: {record['curiosity']:.3f}")
    print(f"    Novelty: {record['novelty']:.3f}")
    print(f"    Developmental stage: {record['developmental_stage']}")
    print(f"    Prediction error: {record['prediction_error']:.4f}")

# Analyze development trends
print("\nDevelopment Analysis:")
final_curiosity = development_history[-1]['curiosity']
initial_curiosity = development_history[0]['curiosity']
curiosity_growth = final_curiosity - initial_curiosity

print(f"  Curiosity growth: {curiosity_growth:.3f}")
print(f"  Learning efficiency: {(development_history[-1]['reward'] / 100):.4f}")
print(f"  Developmental progression: {'Yes' if development_history[-1]['developmental_stage'] > development_history[0]['developmental_stage'] else 'No'}")
```

### Key Classes and Methods

#### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(input)  # Inject sensory data
action = brain.produceAction()    # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Development
brain.getDevelopmentalStage()     # Get current developmental stage
brain.setDevelopmentalStage(stage)  # Set developmental stage
```

#### Config Class (Enhanced)

```python
# Create
config = pynlm.createDefaultConfig()

# Enhanced Pythonic operations
config.set("brain.neuron_count", 1000)        # Set value
config.setMultiple({"key": "value", "num": 42})  # Set multiple
config.get("brain.neuron_count")              # Get value
config.has("brain.neuron_count")              # Check existence
config.remove("brain.neuron_count")           # Remove key
config.toDict()                               # Convert to Python dict
config.fromDict({"key": "value"})             # Load from Python dict
config.validate({"required_key": "type"})    # Validate configuration
```

#### AgentBrain Class (Enhanced)

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Enhanced simulation methods
agent.runEpisode(world, 500)                          # Run complete episode
agent.runEpisodeWithLearning(world, 300, True)        # Run with learning
agent.simulateWorldInteraction(world, config)         # Simulate with config

# Neuroscience metrics
agent.getNeuroscienceMetrics()                        # Get analysis metrics
agent.getCuriosityLevel()                             # Get curiosity
agent.getNoveltyLevel()                               # Get novelty
agent.getPredictionError()                            # Get prediction error
agent.getNeuromodulationLevel()                       # Get neuromodulation
```

#### SimpleWorld Class (Enhanced)

```python
# Creation
world = pynlm.createSimpleWorld()

# Enhanced world methods
world.runEnvironment(100)                            # Run simulation steps
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)  # Configure
world.addResources(10)                               # Add resources
world.addHazards(5)                                  # Add hazards
world.createTestEnvironment()                         # Create test world
world.getEnvironmentStats()                           # Get statistics
world.getAgentPosition()                             # Get agent state
world.getVisionData()                                # Get sensory data
world.getTouchData()                                 # Get touch data
world.getInternalData()                              # Get internal signals
```

### Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Rest

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

### Configuration Options

#### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

#### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

---

## Part 5: Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Part 6: Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

---

## Part 7: Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

---

## Version Information

- **Version**: NLM Phase 6 Enhanced Python API
- **Author**: NLM Project
- **Description**: Enhanced Python bindings with improved APIs and comprehensive examples
- **License**: MIT

The enhanced Python API provides significant improvements over the basic bindings, including:

1. **Pythonic APIs**: Intuitive method names and dictionary-based configuration
2. **Experiment Helpers**: Built-in simulations for Phase 3 and 4 experiments
3. **Enhanced Monitoring**: Comprehensive metrics and analysis tools
4. **Better Documentation**: Extensive examples and comprehensive docstrings
5. **Improved Error Handling**: More descriptive exceptions and validation feedback
6. **Advanced Features**: Neuroscience metrics, development tracking, and learning analysis

All existing code continues to work while providing powerful new tools for advanced research and experimentation.
