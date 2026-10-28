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

### Enhanced Basic Usage

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

# Get comprehensive brain statistics
stats = pynlm.get_simulation_stats(brain)
print("Brain statistics:", stats)
```

#### Creating a Configuration from Dictionary

```python
import pynlm

# Create configuration from Python dictionary
config_dict = {
    "brain.neuron_count": 5000,
    "brain.synapse_density": 0.15,
    "plasticity.stdp.learning_rate": 0.002,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True,
    "development.enable": True
}

config = pynlm.createConfigFromDict(config_dict)
brain = pynlm.createBrain(config)
brain.initialize()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

#### Creating Specialized Worlds

```python
import pynlm

# Create a simple world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Reset world to initial state
world.reset()

# Run world update
world.update(timestep=0.1)

# Print world information
pynlm.print_world_info(world)
```

#### Adding Objects to World

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Add various objects to the world
pynlm.add_resource_to_world(world, 5.0, 5.0, 10.0)  # Resource at (5,5) with value 10
pynlm.add_resource_to_world(world, 15.0, 15.0, 5.0, radius=0.3)  # Smaller resource

print("Added objects to world")
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

#### Enhanced Agent Simulation

```python
import pynlm

def run_complete_simulation(num_steps=1000, use_enhanced_config=False):
    """Run a complete enhanced agent simulation."""
    
    # Create appropriate configuration
    if use_enhanced_config:
        # Use exploratory configuration for learning and adaptation
        config = pynlm.create_exploratory_config()
    else:
        # Use default configuration
        config = pynlm.createDefaultConfig()
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionWidth=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
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
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Dev stage: {agent.getDevelopmentalStage()}")
    
    # Get final statistics
    stats = pynlm.get_simulation_stats(brain)
    formatted_stats = pynlm.format_stats(stats)
    
    print("\n=== Simulation Complete ===")
    print(formatted_stats)
    
    return stats

# Run the enhanced simulation
result = run_complete_simulation(1000, use_enhanced_config=True)
```

#### Run Agent Episode with Statistics

```python
import pynlm

def run_agent_episode_with_stats(num_steps=500, enable_learning=True):
    """Run a complete agent episode and return detailed statistics."""
    
    # Create configuration and systems
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Initialize
    brain.initialize()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    agent.initialize(world)
    
    # Enable learning if requested
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
    
    # Run episode
    episode_stats = pynlm.run_agent_episode(brain, world, agent, num_steps, enable_learning)
    
    print(f"Episode completed in {num_steps} steps")
    print(f"Total reward: {episode_stats['total_reward']:.2f}")
    print(f"Novel actions: {episode_stats['novel_actions']}")
    print(f"Final curiosity: {episode_stats['final_curiosity']:.3f}")
    print(f"Final developmental stage: {episode_stats['final_development_stage']}")
    
    return episode_stats

# Run episode
stats = run_agent_episode_with_stats(800, enable_learning=True)
```

#### Configuration Templates

```python
import pynlm

# List all available configuration templates
templates = pynlm.list_configuration_templates()
print("Available configuration templates:", templates)

# Get a specific template
exploratory_config = pynlm.get_configuration_template("exploratory")
predictive_config = pynlm.get_configuration_template("predictive")
social_config = pynlm.get_configuration_template("social")

# Create brains with different templates
def create_brain_with_template(template_name):
    config = pynlm.get_configuration_template(template_name)
    config_obj = pynlm.createConfigFromDict(config)
    brain = pynlm.createBrain(config_obj)
    brain.initialize()
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons using '{template_name}' template")
    return brain

# Test different templates
brain1 = create_brain_with_template("default")
brain2 = create_brain_with_template("exploratory")
brain3 = create_brain_with_template("predictive")
brain4 = create_brain_with_template("social")
```

#### Visualization and Benchmarking

```python
import pynlm

# Create a simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

brain.initialize()
world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
world.reset()
agent.initialize(world)

# Run simulation for visualization
pynlm.run_simulation(brain, world, agent, 500, progress_interval=100)

# Create visualization script
pynlm.create_visualization_script("nlm_visualization.py", brain, world, agent)

# Run benchmark
brain2 = pynlm.createBrain(config)
benchmark_results = pynlm.benchmark_brain_performance(brain2, 2000)

print("Benchmark results:", benchmark_results)
```

#### Advanced Simulation Control

```python
import pynlm

# Create systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

brain.initialize()
world.configure(width=12, height=12, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation with progress reporting
pynlm.simulate_with_progress(brain, world, agent, 1500, progress_interval=200)

# Save brain state
pynlm.save_brain_checkpoint(brain, "brain_checkpoint.pkl")

# Create brain from checkpoint
loaded_brain = pynlm.create_brain_from_checkpoint("brain_checkpoint.pkl")

print("Advanced simulation completed successfully!")
```

### Enhanced Brain Class

```python
# Creation and Configuration
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

# Enhanced Statistics
stats = pynlm.get_simulation_stats(brain)  # Get comprehensive stats
formatted = pynlm.format_stats(stats)      # Format stats for display
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getActiveNeuronCount()      # Currently active
brain.getFiringNeuronCount()     # Currently firing
brain.getTotalSpikeCount()        # Total spikes
brain.getPendingSpikeEventCount() # Pending spike events
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State Management
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state
brain.logStatus()                  # Print brain status

# Memory Systems
brain.getWorkingMemory()          # Get working memory
brain.getEpisodicMemory()         # Get episodic memory
brain.getAssociativeMemory()      # Get associative memory
brain.getPredictionSystem()       # Get prediction system
brain.getPlanner()                # Get neural planner
brain.getConceptFormation()       # Get concept formation
brain.getAttention()              # Get attention system
brain.getDevelopmentSystem()      # Get development system
brain.getDopamine()               # Get dopamine system
brain.getCuriosity()              # Get curiosity system
brain.getNovelty()                # Get novelty system
brain.getPredictionErrorSignal()  # Get prediction error

# Development and Neuromodulation
brain.getDevelopmentalStage()    # Get current development stage
brain.setDevelopmentalStage(stage)  # Set development stage
brain.getConfig()                 # Get configuration
brain.getRandomGenerator()        # Get random number generator
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

### Enhanced AgentBrain Class

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

# Enhanced Statistics
agent.getNeuromodulationLevel()   # Get current neuromodulation level
agent.getCuriosityLevel()        # Get curiosity level
agent.getNoveltyLevel()          # Get novelty level
agent.getPredictionError()       # Get prediction error

# Subsystem Configuration (all now with getter methods)
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Status Checks
agent.isRewardModulationEnabled()     # Check if reward modulation is enabled
agent.isStructuralPlasticityEnabled() # Check if structural plasticity is enabled
agent.isDevelopmentEnabled()          # Check if development is enabled
agent.isCuriosityEnabled()            # Check if curiosity is enabled

# Additional Processing
agent.applyRewardModulation(reward, predicted_reward)  # Apply reward-based learning
agent.getBrain()                      # Get underlying brain reference
```

### Enhanced SimpleWorld Class

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

# Enhanced Utilities
pynlm.print_world_info(world)                                    # Print detailed world information
pynlm.add_resource_to_world(world, x, y, value, radius=0.5)     # Add resource objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))  # Add world objects
world.removeObject(x, y)                                        # Remove object
world.isValidPosition(x, y)                                      # Check if position is valid

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getVisionWidth()
world.getVisionHeight()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
world.getEnergyDecayRate()
world.setEnergyDecayRate(0.1)
world.getRandomSeed()
```

### Enumerations

#### Neuron Types

```python
pynlm.NeuronType.Excitatory    # Excitatory neurons (standard)
pynlm.NeuronType.Inhibitory    # Inhibitory neurons
pynlm.NeuronType.Modulatory    # Modulatory neurons (neuromodulation)
pynlm.NeuronType.Sensory       # Sensory neurons (input processing)
pynlm.NeuronType.Motor         # Motor neurons (output control)
pynlm.NeuronType.Internal      # Internal neurons (cognitive processing)
```

#### Synapse Types

```python
pynlm.SynapseType.Excitatory     # Excitatory synapses
pynlm.SynapseType.Inhibitory     # Inhibitory synapses
pynlm.SynapseType.Modulatory     # Modulatory synapses
pynlm.SynapseType.Electrical     # Electrical synapses
pynlm.SynapseType.GapJunction   # Gap junction synapses
```

#### Developmental Stages

```python
pynlm.DevelopmentalStage.Initial         # Embryonic/early development
pynlm.DevelopmentalStage.CriticalPeriod  # Sensitive learning period
pynlm.DevelopmentalStage.Maturation       # Growth and maturation
pynlm.DevelopmentalStage.Adult           # Fully developed
pynlm.DevelopmentalStage.Aging           # Aging and decline
```

#### Action Types (Motor Commands)

```python
pynlm.ActionType.MoveForward    # Move forward
pynlm.ActionType.MoveBackward   # Move backward
pynlm.ActionType.MoveLeft       # Move left
pynlm.ActionType.MoveRight      # Move right
pynlm.ActionType.TurnLeft       # Turn left
pynlm.ActionType.TurnRight      # Turn right
pynlm.ActionType.Look           # Look (scan environment)
pynlm.ActionType.LookUp         # Look up
pynlm.ActionType.LookDown       # Look down
pynlm.ActionType.Interact       # Interact with objects
pynlm.ActionType.Eat           # Consume food/resources
pynlm.ActionType.Drink         # Drink/water
pynlm.ActionType.Rest          # Rest/sleep
pynlm.ActionType.Wait          # Wait/idle
pynlm.ActionType.Custom        # Custom action
```

#### Motor Commands (Low-level)

```python
pynlm.MotorCommand.MoveForward  # Move forward
pynlm.MotorCommand.MoveBackward # Move backward
pynlm.MotorCommand.TurnLeft     # Turn left
pynlm.MotorCommand.TurnRight    # Turn right
pynlm.MotorCommand.LookLeft     # Look left
pynlm.MotorCommand.LookRight    # Look right
pynlm.MotorCommand.Interact     # Interact
pynlm.MotorCommand.Wait        # Wait
```

#### World Object Types

```python
pynlm.WorldObjectType.Empty     # Empty space
pynlm.WorldObjectType.Resource  # Resource/food/water
pynlm.WorldObjectType.Hazard    # Hazardous object
pynlm.WorldObjectType.Wall      # Wall/obstacle
pynlm.WorldObjectType.Marker    # Marked location
```

---

## Part 4: Enhanced Configuration Options

### Brain Configuration

```python
# Create configuration from dictionary
config_dict = {
    "brain.neuron_count": 5000,
    "brain.synapse_density": 0.15,
    "brain.connection_probability": 0.08,
    "brain.v_thresh": -45.0,  # Higher threshold for stability
    "brain.v_rest": -65.0,
    "brain.tau_mem": 25.0,   # Slower membrane dynamics
    "brain.tau_ref": 3.0     # Longer refractory period
}

config = pynlm.createConfigFromDict(config_dict)
```

### Configuration Templates

#### Default Configuration (pynlm.createDefaultConfig())

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `development.enable` | bool | true | Enable developmental changes |

#### Exploratory Configuration (pynlm.create_exploratory_config())

Optimized for exploration and learning:
- Higher neuron count (5000)
- Strong neuromodulation (curiosity, novelty)
- Enhanced plasticity (both STDP and Hebbian)
- Developmental focus for adaptation

#### Predictive Configuration (pynlm.create_predictive_config())

Optimized for planning and prediction:
- Moderate neuron count (3000)
- Integrated prediction system
- Strong cognitive planning
- Balanced neuromodulation
- Working memory focus

#### Social Configuration (pynlm.create_social_config())

Optimized for cooperative behavior:
- Higher connectivity (8000 neurons)
- Social learning mechanisms
- Cooperation and imitation
- Social memory systems
- Enhanced neuromodulation

#### Adaptive Configuration (pynlm.create_adaptive_config())

Optimized for flexible adaptation:
- High plasticity with adaptive mechanisms
- Enhanced memory adaptation
- Strong neuromodulation
- Dynamic development
- Performance optimized

#### Minimal Configuration (pynlm.create_minimal_config())

For testing and rapid prototyping:
- Low neuron count (100)
- Basic STDP only
- Disabled heavy subsystems
- Fast execution
- Small memory footprint

### Advanced Configuration Options

#### Configuration File Loading

```python
# Load configuration from JSON file (with proper JSON parser)
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")

# Save configuration to JSON file
config.saveToFile("my_config.json")

# Load from command line arguments
config.loadFromArgs(3, ["--brain.neuron_count=2000", "--neuromod.curiosity.enable=true"])
```

#### Configuration Validation

```python
# Validate configuration
config = pynlm.createConfigFromDict(config_dict)
if pynlm.validate_config(config):
    print("Configuration is valid!")
else:
    print("Configuration has issues!")
```

#### Default Configuration Dictionary

```python
# Get default values as dictionary
defaults = pynlm.create_default_config_dict()
print(defaults["brain.neuron_count"])  # 1000
print(defaults["plasticity.stdp.enable"])  # True
```

#### Configuration Templates Listing

```python
# Get all available templates
templates = pynlm.list_configuration_templates()
print("Available templates:", templates)
# Returns: ['default', 'exploratory', 'predictive', 'minimal', 'social', 'adaptive']

# Get a specific template
template = pynlm.get_configuration_template("social")
```

---

### Enhanced Example Scripts

#### Minimal Example (Using Enhanced API)

```python
import pynlm

# Create and initialize with enhanced configuration
config = pynlm.create_default_config_dict()
config_obj = pynlm.createConfigFromDict(config)

brain = pynlm.createBrain(config_obj)
brain.initialize()

# Simulate with enhanced statistics
for i in range(100):
    brain.step(i)
    
    # Get comprehensive statistics every 20 steps
    if i % 20 == 0:
        stats = pynlm.get_simulation_stats(brain)
        print(f"Step {i}: {stats['total_spikes']} spikes, {stats['firing_neurons']} firing neurons")

print("Enhanced simulation complete!")
```

#### Environment Interaction (Enhanced)

```python
import pynlm

# Setup with enhanced world utilities
config = pynlm.create_default_config_dict()
config_obj = pynlm.createConfigFromDict(config)

brain = pynlm.createBrain(config_obj)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create and configure enhanced world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Use enhanced world utilities
pynlm.print_world_info(world)
pynlm.add_resource_to_world(world, 2.0, 2.0, 10.0)  # Add food
pynlm.add_resource_to_world(world, 8.0, 8.0, 5.0, radius=0.3)  # Add smaller resource

# Initialize agent with enhanced world
agent.initialize(world)

# Run enhanced episode
episode_stats = pynlm.run_agent_episode_with_stats(500, enable_learning=True)

# Log episode results
print(f"Episode completed with total reward: {episode_stats['total_reward']:.2f}")
print(f"Novel actions experienced: {episode_stats['novel_actions']}")
```

#### Advanced Agent with Learning Configuration

```python
import pynlm

def run_advanced_agent_simulation(template="exploratory", num_steps=1000):
    """Run an advanced agent simulation with different learning configurations."""
    
    # Get configuration template
    template_config = pynlm.get_configuration_template(template)
    
    # Create configuration and systems
    config = pynlm.createConfigFromDict(template_config)
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Initialize all systems
    brain.initialize()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    agent.initialize(world)
    
    # Enable learning subsystems based on template
    if template in ["exploratory", "adaptive", "social"]:
        agent.enableCuriosity(True)      # For exploration
        agent.enableDevelopment(True)    # For adaptation
    
    if template in ["exploratory", "adaptive"]:
        agent.enableRewardModulation(True)  # For reinforcement learning
        agent.enableStructuralPlasticity(True)  # For neural growth
    
    if template == "social":
        agent.enableCuriosity(True)      # Observe other agents
        agent.enableDevelopment(True)    # Social development
    
    # Run advanced simulation with progress
    stats = pynlm.simulate_with_progress(brain, world, agent, num_steps, progress_interval=200)
    
    # Get final comprehensive statistics
    final_stats = pynlm.get_simulation_stats(brain)
    formatted_stats = pynlm.format_stats(final_stats)
    
    print(f"\n=== {template.title()} Agent Simulation Complete ===")
    print(formatted_stats)
    
    return final_stats

# Test different configurations
templates = ["default", "exploratory", "predictive", "social", "adaptive"]
for template in templates:
    print(f"\nRunning {template} configuration...")
    run_advanced_agent_simulation(template, 500)
    break  # Just run one for demo
```

#### Configuration Template Comparison

```python
import pynlm

# Compare different configuration templates
configs = {}
for template_name in pynlm.list_configuration_templates():
    config = pynlm.get_configuration_template(template_name)
    config_obj = pynlm.createConfigFromDict(config)
    brain = pynlm.createBrain(config_obj)
    brain.initialize()
    
    stats = pynlm.get_simulation_stats(brain)
    configs[template_name] = {
        "neurons": stats["total_neurons"],
        "plasticity": "enabled" if config.get("plasticity.stdp.enable") else "disabled",
        "curiosity": "enabled" if config.get("neuromod.curiosity.enable") else "disabled",
        "development": "enabled" if config.get("development.enable") else "disabled"
    }

print("Configuration Template Comparison:")
for name, props in configs.items():
    print(f"\n{name.title()}:")
    print(f"  Neurons: {props['neurons']}")
    print(f"  Plasticity: {props['plasticity']}")
    print(f"  Curiosity: {props['curiosity']}")
    print(f"  Development: {props['development']}")
```

#### Benchmarking and Performance Analysis

```python
import pynlm

# Benchmark different configurations
print("Benchmarking NLM Configurations")
print("=" * 40)

configurations = [
    ("Minimal", pynlm.create_minimal_config()),
    ("Default", pynlm.createDefaultConfig()),
    ("Exploratory", pynlm.create_exploratory_config())
]

for name, config in configurations:
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    brain.initialize()
    world.configure(width=8, height=8, visionWidth=4, visionHeight=4)
    world.reset()
    agent.initialize(world)
    
    # Run performance benchmark
    results = pynlm.benchmark_brain_performance(brain, 1000)
    
    print(f"\n{name} Configuration:")
    print(f"  Steps per second: {results['steps_per_second']:.1f}")
    print(f"  Total time (ms): {results['total_time_ms']:.1f}")
    print(f"  Neurons per step: {results['neurons_per_step']}")
    print(f"  Spikes per step: {results['spikes_per_step']:.2f}")
```

#### Visualization and Data Export

```python
import pynlm

# Create comprehensive simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

brain.initialize()
world.configure(width=12, height=12, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run simulation with data collection
print("Running simulation with data collection...")
for step in range(800):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 200 == 0:
        print(f"Progress: {step}/800 steps")

# Create visualization script
pynlm.create_visualization_script("nlm_simulation.py", brain, world, agent)

# Get comprehensive statistics for visualization
stats = pynlm.get_simulation_stats(brain)
formatted_stats = pynlm.format_stats(stats)

print("\n" + "=" * 50)
print("SIMULATION VISUALIZATION DATA")
print("=" * 50)
print(formatted_stats)
print(f"Visualization script created: nlm_simulation.py")
```

#### Advanced Error Handling and Configuration Validation

```python
import pynlm

# Test configuration validation
print("Testing Configuration Validation")
print("=" * 30)

# Valid configuration
valid_config = {
    "brain.neuron_count": 1000,
    "plasticity.stdp.enable": True
}
config_obj = pynlm.createConfigFromDict(valid_config)
is_valid = pynlm.validate_config(config_obj)
print(f"Valid configuration: {is_valid}")  # Should be True

# Invalid configuration (too many neurons)
invalid_config = {
    "brain.neuron_count": 1000000,  # Unrealistic value
    "plasticity.stdp.enable": True
}
config_obj2 = pynlm.createConfigFromDict(invalid_config)
is_valid2 = pynlm.validate_config(config_obj2)
print(f"Invalid configuration: {is_valid2}")  # Should be False

# Get default values for troubleshooting
defaults = pynlm.create_default_config_dict()
print(f"\nDefault neuron count: {defaults['brain.neuron_count']}")
print(f"Default STDP enabled: {defaults['plasticity.stdp.enable']}")
```

#### Complete System Integration Example

```python
import pynlm

print("NLM Complete System Integration Demo")
print("=" * 40)

# Create brain with exploratory configuration
exploratory_config = pynlm.create_exploratory_config()
brain = pynlm.createBrain(exploratory_config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Initialize all systems
brain.initialize()
world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
world.reset()
world.setAgentStart(10.0, 10.0)
agent.initialize(world)

# Enable learning and development
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run complete episode with enhanced reporting
print("\nStarting enhanced agent episode...")
episode_stats = pynlm.run_agent_episode_with_stats(1000, enable_learning=True)

# Create comprehensive visualization
print("\nGenerating visualization script...")
pynlm.create_visualization_script("complete_agent_demo.py", brain, world, agent)

# Get final system information
system_info = pynlm.get_system_info()
print(f"\nSystem: {system_info['name']} v{system_info['version']}")
print(f"Phase: {system_info['phase']}")
print(f"Python API Version: {system_info['python_api_version']}")

# Show enhanced features
compat = pynlm.check_python_compatibility()
enhanced_features = [k for k, v in compat.items() if v]
print(f"\nEnhanced Features Available: {len(enhanced_features)}")
for feature in enhanced_features:
    print(f"  ✓ {feature.replace('_', ' ').title()}")

print(f"\nComplete system integration successful!")
print(f"Visualization script created: complete_agent_demo.py")
print(f"Episode statistics saved and accessible")
```

## Troubleshooting

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

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
