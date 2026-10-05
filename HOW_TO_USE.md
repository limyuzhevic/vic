# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API

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

## Part 3: Using the Python Library

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration using the factory
config = pynlm.create_default_config()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.create_brain(config)
brain.initialize()

print(f"Total neurons: {brain.total_neuron_count}")
print(f"Total synapses: {brain.total_synapse_count}")
```

### Enhanced Python Interface

The improved bindings provide a more Pythonic interface:

```python
import pynlm

# Create brain using improved factory functions
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Use Pythonic property access
print(f"Brain has {brain.total_neuron_count} neurons")
print(f"Currently firing: {brain.firing_neuron_count} neurons")
print(f"Average firing rate: {brain.average_firing_rate:.2f} Hz")

# Enhanced ID representation
neuron_id = pynlm.NeuronId(42)
print(str(neuron_id))  # "<NeuronId: 42>"
print(repr(neuron_id))  # "<NeuronId object at 0x...>"

# Enum utilities with string conversion
stage = brain.developmental_stage
print(stage.to_string())  # "DevelopmentalStage: 2"
stage_str = "Adult"
stage_from_str = pynlm.enums.string_to_enum(stage_str)  # Convert string to enum
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

### Complete Agent Example

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
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")      # Save state
brain.load("checkpoint.bin")      # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)

# Pythonic properties (new in improved bindings)
brain.total_neuron_count          # Property access
brain.firing_neuron_count         # Property access
brain.average_firing_rate         # Property access
brain.spike_count                 # Property access
brain.developmental_stage         # Property access
```

#### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()

# Pythonic properties (new in improved bindings)
agent.brain                    # Property access to underlying brain
agent.sensory_input_size        # Property access
agent.motor_output_size         # Property access
agent.get_subsystem_status()    # Get status of all subsystems
```

#### SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Pythonic properties (new in improved bindings)
world.width                       # Property access
world.height                      # Property access
world.vision_width                # Property access
world.vision_height               # Property access
world.simulation_time             # Property access
world.get_agent_position()        # Method for position
world.set_agent_position()        # Method for position
world.get_active_objects()        # Get active objects
world.clear_objects()             # Clear objects
world.is_position_valid()         # Check position
```

### Enumerations

#### Neuron Types

```python
# Enhanced enum access with Pythonic methods
pynlm.NeuronType.Excitatory.to_string()      # "NeuronType(0)"
pynlm.enums.string_to_enum("Excitatory")    # neuron_type = pynlm.enums.string_to_enum("Excitatory")
```

#### Developmental Stages

```python
# Enhanced developmental stage handling
pynlm.DevelopmentalStage.Maturation.to_string()    # "DevelopmentalStage: 2"
pynlm.enums.string_to_enum("Maturation")          # Convert string to enum
```

#### Action Types

```python
# Enhanced action type handling
pynlm.ActionType.MoveForward.to_string()          # "ActionType: 0"
pynlm.enums.string_to_enum("MoveForward")        # Convert string to enum
```

---

## Part 4: Configuration Options

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

---

## Part 5: Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize using improved factory functions
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

# Simulate using Pythonic interface
for i in range(100):
    brain.step(i)
    
print("Simulation complete!")
print(f"Firing neurons: {brain.firing_neuron_count}")
```

### Environment Interaction with Enhanced Features

```python
import pynlm

# Setup using improved factory functions
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()
agent = pynlm.create_agent_brain(brain)
world = pynlm.create_simple_world()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning subsystems
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# Run episode using Pythonic interface
for step in range(500):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step)
    world.apply_motor_command(agent.decode_motor_command(), world.simulation_time)
    
    # Get internal signals for reward
    internal = world.get_sensory_percept().internal
    reward = internal[0] if internal else 0.0
    agent.apply_reward_modulation(reward, 0.0)

print(f"Episode completed: {step} steps")
print(f"Final curiosity level: {agent.get_curiosity_level():.3f}")
print(f"Final novelty level: {agent.get_novelty_level():.3f}")
```

#### Advanced Configuration and Monitoring

```python
import pynlm

# Create brain with enhanced monitoring
config = pynlm.create_default_config()

# Configure advanced settings
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.5)

brain = pynlm.create_brain(config)
brain.initialize()

# Create enhanced agent
agent = pynlm.create_agent_brain(brain)
world = pynlm.create_simple_world()
world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
world.reset()
agent.initialize(world)

# Enable all subsystems for advanced simulation
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

# Run advanced simulation with monitoring
print("Starting advanced simulation...")
for step in range(1000):
    world.update(0.05)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.simulation_time)
    
    # Monitor brain health and development
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.firing_neuron_count}")
        print(f"  Average firing rate: {brain.average_firing_rate:.2f} Hz")
        print(f"  E/I ratio: {brain.get_excitation_inhibition_ratio():.3f}")
        print(f"  Development stage: {brain.get_developmental_stage().to_string()}")
        print(f"  Curiosity level: {agent.get_curiosity_level():.3f}")
        print(f"  Novelty level: {agent.get_novelty_level():.3f}")
        print(f"  Prediction error: {agent.get_prediction_error():.3f}")
        print()

print("Advanced simulation complete!")
```

---

## Part 6: Advanced Features and Troubleshooting

### Enhanced Installation (Improved Bindings)

To use the improved Python bindings with enhanced features:

```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build with improved bindings (default)
pip install .  # or pip install -e . for development

# Build with original bindings (if needed)
pip install --config-option="build.cmake_args=-DPYNLM_USE_IMPROVED_BINDINGS=OFF" .
```

### Key Improvements in Enhanced Bindings

#### 1. Eliminated Code Duplication
- Template-based ID management reduces code by ~70%
- Single source of truth for all identifier types
- Easier maintenance and updates

#### 2. Pythonic Interface
- Natural property access (`brain.total_neuron_count`)
- Pythonic method names (`to_string()`, `repr()`)
- Enhanced error handling with Python-style exceptions
- Better serialization support

#### 3. Factory Pattern
- Type-safe component creation
- Validation and error checking
- Consistent interface across all components
- Improved debugging support

#### 4. Enhanced Documentation
- Comprehensive docstrings
- Pythonic examples throughout
- Advanced usage patterns documented
- Clear separation of basic and advanced features

### Configuration Options (Advanced)

The configuration system now supports:

#### Enhanced Config Methods
```python
# Type-safe value access
value = config.get_int("brain.neuron_count")
value = config.get_float("brain.synapse_density")
value = config.get_bool("plasticity.stdp.enable")

# Path-based access
if config.has("neuromod.dopamine.scale"):
    scale = config.get_float("neuromod.dopamine.scale")

# Configuration copying and merging
config2 = config.copy()
config2.merge(config)
```

### Performance Monitoring

The improved bindings include:

```python
# Brain performance monitoring
brain.get_neuron_density()           # Neurons per unit area
brain.get_health_score()             # Overall brain health metric
brain.get_firing_efficiency()        # Efficiency of firing patterns
brain.get_plasticity_level()         # Current plasticity activity

# Agent performance monitoring
agent.get_subsystem_status()         # Status of all subsystems
agent.get_learning_progress()        # Learning curve tracking
agent.get_exploration_rate()         # How much the agent explores
```

### Error Handling and Debugging

#### Enhanced Error Messages
```python
try:
    brain = pynlm.create_brain(config)
    brain.initialize()
except pynlm.ValueError as e:
    print(f"Configuration error: {e}")
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except pynlm.IndexError as e:
    print(f"Index error: {e}")
```

#### Debugging Aids
```python
# Enhanced ID representation
neuron_id = pynlm.NeuronId(42)
print(f"Neuron ID: {neuron_id}")  # Uses __str__
print(f"Debug info: {repr(neuron_id)}")  # Uses __repr__

# Brain state debugging
brain.log_status()  # Detailed brain state information
print(f"Brain region count: {brain.region_count}")  # Property access
print(f"Total synapses: {brain.total_synapse_count}")  # Property access
```

### Advanced Examples

#### 1. Learning Brain with Enhanced Monitoring

```python
import pynlm

def run_learning_experiment(config_overrides=None):
    """Run a learning experiment with enhanced monitoring."""
    
    # Create default config
    config = pynlm.create_default_config()
    
    # Apply overrides if provided
    if config_overrides:
        for key, value in config_overrides.items():
            config.set(key, value)
    
    # Create brain with monitoring
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Create agent with world
    agent = pynlm.create_agent_brain(brain)
    world = pynlm.create_simple_world()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_development(True)
    
    # Run experiment with monitoring
    experiment_data = {
        'steps': [],
        'rewards': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'brain_health': []
    }
    
    for step in range(500):
        # Environment interaction
        world.update(0.1)
        agent.process_sensory_input(world.get_sensory_percept())
        brain.step(step)
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.simulation_time)
        
        # Get reward and apply learning
        percept = world.get_sensory_percept()
        reward = percept.internal[0] if percept.internal else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        
        # Record data
        if step % 10 == 0:  # Record every 10 steps
            experiment_data['steps'].append(step)
            experiment_data['rewards'].append(reward)
            experiment_data['curiosity_levels'].append(agent.get_curiosity_level())
            experiment_data['novelty_levels'].append(agent.get_novelty_level())
            experiment_data['brain_health'].append(brain.get_health_score())
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Reward: {reward:.3f}")
            print(f"  Curiosity: {agent.get_curiosity_level():.3f}")
            print(f"  Novelty: {agent.get_novelty_level():.3f}")
            print(f"  Brain Health: {brain.get_health_score():.3f}")
            print()
    
    return experiment_data

# Run the experiment
results = run_learning_experiment({
    "brain.neuron_count": 1000,
    "plasticity.stdp.enable": True,
    "neuromod.curiosity.enable": True
})

# Analyze results
final_curiosity = results['curiosity_levels'][-1]
final_novelty = results['novelty_levels'][-1]
avg_reward = sum(results['rewards']) / len(results['rewards'])

print(f"Experiment completed!")
print(f"Average reward: {avg_reward:.3f}")
print(f"Final curiosity level: {final_curiosity:.3f}")
print(f"Final novelty level: {final_novelty:.3f}")
```

#### 2. Multi-Agent Simulation

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor

def create_agent(config):
    """Create a single agent with given config."""
    brain = pynlm.create_brain(config)
    brain.initialize()
    return pynlm.create_agent_brain(brain)

def agent_episode(agent, world, steps):
    """Run one episode for an agent."""
    for step in range(steps):
        world.update(0.05)
        agent.process_sensory_input(world.get_sensory_percept())
        brain = agent.get_brain()
        brain.step(step)
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.simulation_time)
        
        # Apply learning
        percept = world.get_sensory_percept()
        reward = percept.internal[0] if percept.internal else 0.0
        agent.apply_reward_modulation(reward, 0.0)
    
    return agent

def run_multi_agent_simulation(num_agents=3, steps=200):
    """Run multiple agents in the same world."""
    
    # Create shared world
    world = pynlm.create_simple_world()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agents with different configurations
    configs = []
    for i in range(num_agents):
        config = pynlm.create_default_config()
        config.set("brain.neuron_count", 500 + i * 100)  # Different sizes
        config.set("plasticity.stdp.enable", True)
        configs.append(config)
    
    # Create agents
    agents = [create_agent(config) for config in configs]
    
    # Initialize agents with different starting positions
    positions = [(5, 5), (15, 15), (25, 5)]
    for agent, (x, y) in zip(agents, positions):
        agent.initialize(world)
        world.set_agent_start(x, y)
    
    # Run simulation
    print(f"Running multi-agent simulation with {num_agents} agents...")
    
    for step in range(steps):
        world.update(0.05)
        
        # All agents process same world state
        percept = world.get_sensory_percept()
        
        # Each agent processes independently
        for agent in agents:
            agent.process_sensory_input(percept)
            brain = agent.get_brain()
            brain.step(step)
            action = agent.decode_motor_command()
            world.apply_motor_command(action, world.simulation_time)
            
            # Apply learning
            reward = percept.internal[0] if percept.internal else 0.0
            agent.apply_reward_modulation(reward, 0.0)
        
        if step % 50 == 0:
            print(f"Step {step}:")
            for i, agent in enumerate(agents):
                brain = agent.get_brain()
                print(f"  Agent {i+1}: {brain.firing_neuron_count} firing neurons")
                print(f"    Curiosity: {agent.get_curiosity_level():.3f}")
    
    return agents

# Run multi-agent simulation
agents = run_multi_agent_simulation()
print("Multi-agent simulation completed!")
```

#### 3. Save and Load Brain States

```python
import pynlm

# Create and train a brain
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

# Train the brain
for i in range(100):
    brain.step(i)

# Save brain state
brain.save("trained_brain.bin")
print("Brain state saved!")

# Create new brain and load saved state
config2 = pynlm.create_default_config()
brain2 = pynlm.create_brain(config2)
brain2.initialize()
brain2.load("trained_brain.bin")
print("Brain state loaded!")

# Verify loaded state
print(f"Original brain neurons: {brain.total_neuron_count}")
print(f"Loaded brain neurons: {brain2.total_neuron_count}")
print(f"States match: {brain.total_neuron_count == brain2.total_neuron_count}")
```

#### 4. Configuration Serialization

```python
import pynlm
import json

# Create configuration
config = pynlm.create_default_config()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", True)

# Save to JSON
config.save_to_file("config.json")

# Load from JSON
config2 = pynlm.create_default_config()
config2.load_from_file("config.json")

# Verify configuration
print("Original config keys:", config.get_keys())
print("Loaded config keys:", config2.get_keys())
print("Configs match:", config.get_keys() == config2.get_keys())

# Display configuration summary
print("Original config summary:", config.summary())
print("Loaded config summary:", config2.summary())
```

#### 5. Complete Agent Lifecycle

```python
import pynlm

def run_complete_agent_lifecycle(episode_length=100):
    """Complete lifecycle of an agent from creation to shutdown."""
    
    print("=== Agent Lifecycle Demo ===\n")
    
    # Phase 1: Creation and Initialization
    print("Phase 1: Creating and initializing agent...")
    config = pynlm.create_default_config()
    
    # Configure agent with advanced settings
    config.set("brain.neuron_count", 800)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create brain and agent using factory pattern
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    agent = pynlm.create_agent_brain(brain)
    world = pynlm.create_simple_world()
    world.configure(width=12, height=12, visionWidth=6, visionHeight=6)
    world.reset()
    agent.initialize(world)
    
    print(f"  Brain created: {brain.total_neuron_count} neurons")
    print(f"  World configured: {world.width}x{world.height}")
    print(f"  Agent initialized at position {world.get_agent_position()}")
    
    # Phase 2: Development and Learning
    print("\nPhase 2: Development and learning...")
    
    # Enable all learning subsystems
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    print("  Learning subsystems enabled")
    print(f"  Initial developmental stage: {brain.get_developmental_stage().to_string()}")
    print(f"  Initial curiosity level: {agent.get_curiosity_level():.3f}")
    
    # Phase 3: Main simulation loop
    print("\nPhase 3: Main simulation loop...")
    
    episode_data = {
        'steps': [],
        'rewards': [],
        'curiosity': [],
        'novelty': [],
        'development_stage': [],
        'firing_rate': []
    }
    
    for step in range(episode_length):
        # Update world
        world.update(0.05)
        
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process sensory input
        agent.process_sensory_input(percept)
        
        # Brain computation
        brain.step(step)
        
        # Decode action
        action = agent.decode_motor_command()
        
        # Apply action in world
        world.apply_motor_command(action, world.simulation_time)
        
        # Apply reward-based learning
        reward = percept.internal[0] if percept.internal else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        
        # Apply development
        agent.update_development(0.05)
        
        # Record data
        episode_data['steps'].append(step)
        episode_data['rewards'].append(reward)
        episode_data['curiosity'].append(agent.get_curiosity_level())
        episode_data['novelty'].append(agent.get_novelty_level())
        episode_data['development_stage'].append(brain.get_developmental_stage())
        episode_data['firing_rate'].append(brain.get_average_firing_rate())
        
        # Print progress
        if step % 20 == 0:
            print(f"    Step {step}:")
            print(f"      Reward: {reward:.3f}")
            print(f"      Curiosity: {agent.get_curiosity_level():.3f}")
            print(f"      Novelty: {agent.get_novelty_level():.3f}")
            print(f"      Development: {brain.get_developmental_stage().to_string()}")
            print(f"      Firing rate: {brain.get_average_firing_rate():.2f} Hz")
    
    # Phase 4: Shutdown and cleanup
    print("\nPhase 4: Shutdown and cleanup...")
    
    # Reset agent
    agent.reset()
    
    # Save final state
    brain.save("agent_final_state.bin")
    
    print(f"  Agent reset completed")
    print(f"  Final developmental stage: {brain.get_developmental_stage().to_string()}")
    print(f"  Final curiosity level: {agent.get_curiosity_level():.3f}")
    print(f"  Final novelty level: {agent.get_novelty_level():.3f}")
    
    # Phase 5: Analysis
    print("\nPhase 5: Analysis...")
    
    final_stage = episode_data['development_stage'][-1]
    max_curiosity = max(episode_data['curiosity'])
    max_novelty = max(episode_data['novelty'])
    avg_reward = sum(episode_data['rewards']) / len(episode_data['rewards'])
    
    print("  Episode Analysis:")
    print(f"    Total steps: {episode_length}")
    print(f"    Average reward: {avg_reward:.3f}")
    print(f"    Max curiosity level: {max_curiosity:.3f}")
    print(f"    Max novelty level: {max_novelty:.3f}")
    print(f"    Final developmental stage: {final_stage.to_string()}")
    
    # Determine success based on metrics
    success_criteria = {
        'positive_rewards': sum(1 for r in episode_data['rewards'] if r > 0),
        'curiosity_growth': max_curiosity - episode_data['curiosity'][0],
        'development_progress': int(final_stage) - int(episode_data['development_stage'][0]),
        'average_firing': sum(episode_data['firing_rate']) / len(episode_data['firing_rate'])
    }
    
    print(f"    Success criteria met:")
    for criterion, value in success_criteria.items():
        print(f"      {criterion}: {value:.3f}")
    
    print(f"\n=== Agent Lifecycle Complete ===")
    return episode_data

# Run complete lifecycle
lifecycle_data = run_complete_agent_lifecycle(150)
```
