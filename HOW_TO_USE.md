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
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
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

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
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
# ... and more

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
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

```

---

## Part 5: Advanced Examples

The following examples demonstrate practical applications and advanced usage patterns for NLM.

### 5.1 Realistic Agent with Learning and Memory

This example shows how to create an agent that learns from experience, uses memory systems, and develops over time.

```python
import pynlm
import random
import statistics
import time
import json

def run_realistic_agent_simulation(episodes=1000):
    """
    Run a realistic agent simulation with learning, memory, and decision making.
    This example shows how to create an agent that learns from experience.
    """
    # 1. Configure the brain with realistic parameters
    config = pynlm.createDefaultConfig()
    
    # Advanced brain configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.12)
    config.set("brain.connection_probability", 0.04)
    config.set("brain.v_thresh", -50.0)
    config.set("brain.v_rest", -70.0)
    config.set("brain.tau_mem", 20.0)
    
    # Enable advanced plasticity
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.stdp.tau_plus", 15.0)
    config.set("plasticity.stdp.tau_minus", 15.0)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.strength", 0.3)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.rate", 0.01)
    
    # Advanced neuromodulation
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.sensitivity", 0.8)
    config.set("neuromod.curiosity.exploration_bonus", 0.4)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.threshold", 0.35)
    config.set("neuromod.development.enable", True)
    config.set("neuromod.development.boost", 1.2)
    
    # 2. Create the brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create the world environment
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30, 
        height=30, 
        visionWidth=10, 
        visionHeight=10
    )
    world.setMaxEnergy(1000.0)
    world.setRandomSeed(42)  # For reproducible experiments
    
    # Add environmental elements
    _add_environment_elements(world)
    
    # Set starting position
    world.setAgentStart(15.0, 15.0)
    world.reset()
    
    # 4. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 5. Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Starting Realistic Agent Simulation")
    print(f"=================================")
    print(f"Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"World: {world.getWidth()}x{world.getHeight()}, vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    print(f"Episodes: {episodes}")
    print()
    
    # 6. Simulation data collection
    episode_data = []
    learning_curve = []
    curiosity_over_time = []
    prediction_errors = []
    
    # 7. Run simulation
    for episode in range(episodes):
        episode_stats = {
            'episode': episode,
            'steps': 0,
            'total_reward': 0.0,
            'max_curiosity': 0.0,
            'avg_prediction_error': 0.0,
            'final_firing_rate': 0.0,
            'novel_events': 0
        }
        
        # Reset environment for new episode
        world.reset()
        world.setAgentStart(15.0, 15.0)
        
        # Run episode
        for step in range(50):  # 50 steps per episode
            # World update
            world.update(0.05)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            
            # Apply action and get results
            action_result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation with exploration bonus
            reward = action_result.reward
            if agent.getCuriosityLevel() > 0.5:  # Curiosity-driven exploration
                reward += 0.2
            
            agent.applyRewardModulation(reward, brain.getPredictionError())
            
            # Update development
            agent.updateDevelopment(0.05)
            
            # Collect statistics
            episode_stats['steps'] += 1
            episode_stats['total_reward'] += reward
            episode_stats['max_curiosity'] = max(episode_stats['max_curiosity'], agent.getCuriosityLevel())
            episode_stats['avg_prediction_error'] += agent.getPredictionError()
            episode_stats['final_firing_rate'] = brain.getAverageFiringRate()
            
            # Check for novel events
            if agent.getNoveltyLevel() > 0.7:
                episode_stats['novel_events'] += 1
        
        # Calculate episode averages
        episode_stats['avg_prediction_error'] /= episode_stats['steps']
        
        # Store episode data
        episode_data.append(episode_stats)
        learning_curve.append(episode_stats['total_reward'])
        curiosity_over_time.append(episode_stats['max_curiosity'])
        prediction_errors.append(episode_stats['avg_prediction_error'])
        
        # Print progress every 100 episodes
        if episode % 100 == 0:
            print(f"Episode {episode + 1}/{episodes}")
            print(f"  Total Reward: {episode_stats['total_reward']:.2f}")
            print(f"  Max Curiosity: {episode_stats['max_curiosity']:.3f}")
            print(f"  Avg Prediction Error: {episode_stats['avg_prediction_error']:.3f}")
            print(f"  Novel Events: {episode_stats['novel_events']}")
            print(f"  Firing Rate: {episode_stats['final_firing_rate']:.2f} Hz")
            print()
    
    # 8. Analyze results
    print(f"Simulation Complete!")
    print(f"==================")
    
    final_stats = episode_data[-1]
    
    print(f"Final Performance:")
    print(f"  Total Reward: {final_stats['total_reward']:.2f}")
    print(f"  Average Curiosity: {statistics.mean(curiosity_over_time):.3f}")
    print(f"  Average Prediction Error: {statistics.mean(prediction_errors):.3f}")
    print(f"  Final Firing Rate: {final_stats['final_firing_rate']:.2f} Hz")
    
    # Calculate learning metrics
    mid_point = len(learning_curve) // 2
    first_half_avg = statistics.mean(learning_curve[:mid_point])
    second_half_avg = statistics.mean(learning_curve[mid_point:])
    learning_improvement = ((second_half_avg - first_half_avg) / first_half_avg) * 100 if first_half_avg > 0 else 0
    
    print(f"\nLearning Analysis:")
    print(f"  First Half Average Reward: {first_half_avg:.2f}")
    print(f"  Second Half Average Reward: {second_half_avg:.2f}")
    print(f"  Learning Improvement: {learning_improvement:.1f}%")
    
    # Check for convergence
    last_10_episodes = learning_curve[-10:]
    episode_variance = statistics.stdev(last_10_episodes) if len(last_10_episodes) > 1 else 0
    
    print(f"\nConvergence Analysis:")
    print(f"  Last 10 Episodes Variance: {episode_variance:.3f}")
    print(f"  Performance Stable: {'Yes' if episode_variance < 0.5 else 'No'}")
    
    # Save results
    _save_simulation_results(episode_data, learning_curve, curiosity_over_time, prediction_errors)
    
    # Save final brain state
    brain.save("realistic_agent_final_brain.bin")
    print(f"\nFinal brain state saved to realistic_agent_final_brain.bin")
    
    return episode_data

# Helper functions for realistic simulation
def _add_environment_elements(world):
    """Add environmental elements like resources and hazards."""
    # Add resources
    for _ in range(8):
        x = random.uniform(5, 25)
        y = random.uniform(5, 25)
        resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=5.0)
        world.addObject(resource)
    
    # Add hazards
    for _ in range(3):
        x = random.uniform(25, 35)
        y = random.uniform(25, 35)
        hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=-3.0)
        world.addObject(hazard)
    
    # Add walls
    for x in [0, 40]:
        for y in range(0, 41, 2):
            wall = pynlm.WorldObject(float(x), float(y), pynlm.WorldObjectType.Wall, value=0.0)
            world.addObject(wall)
    
    for y in [0, 40]:
        for x in range(0, 41, 2):
            wall = pynlm.WorldObject(float(x), float(y), pynlm.WorldObjectType.Wall, value=0.0)
            world.addObject(wall)

def _save_simulation_results(episode_data, learning_curve, curiosity_over_time, prediction_errors):
    """Save simulation results to files."""
    results = {
        'episode_data': episode_data,
        'learning_curve': learning_curve,
        'curiosity_over_time': curiosity_over_time,
        'prediction_errors': prediction_errors,
        'metadata': {
            'simulation_type': 'realistic_agent',
            'total_episodes': len(episode_data),
            'timestamp': time.time()
        }
    }
    
    with open('realistic_agent_results.json', 'w') as f:
        json.dump(results, f, indent=2, default=str)
    
    print(f"Simulation results saved to realistic_agent_results.json")

# Run the realistic simulation (uncomment to execute)
# simulation_data = run_realistic_agent_simulation(1000)
```

### 5.2 Multi-Agent System

```python
import pynlm
import random
import statistics
import time
import json

def run_multi_agent_simulation(num_agents=3, episodes=500):
    """
    Run a multi-agent simulation where multiple agents learn together.
    This example shows interaction between multiple NLM brains.
    """
    print(f"Multi-Agent Simulation with {num_agents} Agents")
    print("=" * 45)
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=12, visionHeight=12)
    world.setMaxEnergy(5000.0)
    world.setRandomSeed(123)
    world.reset()
    
    # Add shared environmental elements
    for _ in range(15):
        x = random.uniform(10, 40)
        y = random.uniform(10, 40)
        resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=3.0)
        world.addObject(resource)
    
    # Create multiple agents with different configurations
    agents = []
    brains = []
    
    for agent_id in range(num_agents):
        # Each agent can have different characteristics
        config = pynlm.createDefaultConfig()
        
        # Vary parameters to create different agent types
        if agent_id == 0:
            # Fast learner
            config.set("brain.neuron_count", 1500)
            config.set("plasticity.stdp.learning_rate", 0.003)
            config.set("neuromod.curiosity.sensitivity", 0.9)
            agent_type = "Fast Learner"
        elif agent_id == 1:
            # Conservative learner
            config.set("brain.neuron_count", 3000)
            config.set("plasticity.stdp.learning_rate", 0.001)
            config.set("neuromod.curiosity.sensitivity", 0.5)
            agent_type = "Conservative"
        else:
            # Curious explorer
            config.set("brain.neuron_count", 2000)
            config.set("plasticity.stdp.learning_rate", 0.002)
            config.set("neuromod.curiosity.sensitivity", 0.8)
            config.set("neuromod.novelty.threshold", 0.2)
            agent_type = "Curious Explorer"
        
        # Set different starting positions
        start_x = 10 + agent_id * 15
        start_y = 10 + agent_id * 10
        
        # Create brain and agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        agents.append(agent)
        brains.append(brain)
        
        print(f"Agent {agent_id + 1} ({agent_type}): {brain.getTotalNeuronCount()} neurons")
    
    # Multi-agent simulation
    print(f"\nRunning Multi-Agent Simulation ({episodes} episodes)")
    
    # Track social learning
    social_interactions = []
    agent_performance = []
    
    for episode in range(episodes):
        episode_data = {
            'episode': episode,
            'total_reward': 0.0,
            'individual_rewards': [],
            'social_events': 0,
            'coordination_events': 0
        }
        
        # Reset world for new episode
        world.reset()
        
        # Reset all agents
        for agent in agents:
            agent.reset()
        
        # Run episode for all agents
        for step in range(30):
            # Update world
            world.update(0.04)
            
            # Each agent perceives the same world state
            percept = world.getSensoryPercept()
            
            # Process through all agents
            agent_rewards = []
            for agent_idx, agent in enumerate(agents):
                agent.processSensoryInput(percept)
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                action_result = world.applyMotorCommand(action, world.getSimulationTime())
                
                agent.applyRewardModulation(action_result.reward, brain.getPredictionError())
                agent.updateDevelopment(0.04)
                
                agent_rewards.append(action_result.reward)
            
            # Check for social interactions
            reward_variance = max(agent_rewards) - min(agent_rewards)
            if reward_variance > 2.0:  # High variance indicates potential social learning opportunity
                episode_data['social_events'] += 1
            
            if len(agent_rewards) > 1:
                # Simple coordination check (all agents getting similar rewards)
                avg_reward = sum(agent_rewards) / len(agent_rewards)
                coordination = all(abs(r - avg_reward) < 1.0 for r in agent_rewards)
                if coordination:
                    episode_data['coordination_events'] += 1
            
            episode_data['individual_rewards'].append(agent_rewards)
            episode_data['total_reward'] += sum(agent_rewards)
        
        agent_performance.append(episode_data)
        
        if episode % 100 == 0:
            print(f"Episode {episode + 1}/{episodes}")
            print(f"  Total Reward: {episode_data['total_reward']:.2f}")
            print(f"  Social Events: {episode_data['social_events']}")
            print(f"  Coordination Events: {episode_data['coordination_events']}")
    
    # Analyze multi-agent results
    print(f"\nMulti-Agent Analysis:")
    
    total_reward = sum(ep['total_reward'] for ep in agent_performance)
    avg_social_events = sum(ep['social_events'] for ep in agent_performance) / len(agent_performance)
    avg_coordination = sum(ep['coordination_events'] for ep in agent_performance) / len(agent_performance)
    
    print(f"  Total Reward: {total_reward:.2f}")
    print(f"  Average Social Events: {avg_social_events:.1f}")
    print(f"  Average Coordination Events: {avg_coordination:.1f}")
    
    # Individual agent performance
    print(f"\nIndividual Agent Performance:")
    for i, brain in enumerate(brains):
        final_firing_rate = brain.getAverageFiringRate()
        total_spikes = brain.getTotalSpikeCount()
        print(f"  Agent {i + 1}: {final_firing_rate:.2f} Hz, {total_spikes} total spikes")
    
    # Save multi-agent results
    multi_agent_results = {
        'agent_performance': agent_performance,
        'num_agents': num_agents,
        'episodes': episodes,
        'final_brain_states': [
            {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate()
            }
            for brain in brains
        ],
        'timestamp': time.time()
    }
    
    with open('multi_agent_results.json', 'w') as f:
        json.dump(multi_agent_results, f, indent=2, default=str)
    
    print(f"\nMulti-agent results saved to multi_agent_results.json")
    
    # Save all brain states
    for i, brain in enumerate(brains):
        brain.save(f"multi_agent_{i+1}_brain.bin")
    
    print(f"Individual brain states saved")
    
    print(f"\nMulti-agent simulation complete!")
    return agent_performance

# Run the multi-agent simulation (uncomment to execute)
# multi_agent_data = run_multi_agent_simulation(3, 500)
```

---

## Part 3: Advanced Examples

The following examples demonstrate practical applications and advanced usage patterns for NLM.

### 3.1 Realistic Agent with Learning and Memory

This example shows how to create an agent that learns from experience, uses memory systems, and develops over time.

```python
import pynlm
import random
import statistics
import time
import json

def run_realistic_agent_simulation(episodes=1000):
    """
    Run a realistic agent simulation with learning, memory, and decision making.
    This example shows how to create an agent that learns from experience.
    """
    # 1. Configure the brain with realistic parameters
    config = pynlm.createDefaultConfig()
    
    # Advanced brain configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.12)
    config.set("brain.connection_probability", 0.04)
    config.set("brain.v_thresh", -50.0)
    config.set("brain.v_rest", -70.0)
    config.set("brain.tau_mem", 20.0)
    
    # Enable advanced plasticity
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.stdp.tau_plus", 15.0)
    config.set("plasticity.stdp.tau_minus", 15.0)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.strength", 0.3)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.rate", 0.01)
    
    # Advanced neuromodulation
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.sensitivity", 0.8)
    config.set("neuromod.curiosity.exploration_bonus", 0.4)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.threshold", 0.35)
    config.set("neuromod.development.enable", True)
    config.set("neuromod.development.boost", 1.2)
    
    # 2. Create the brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create the world environment
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30, 
        height=30, 
        visionWidth=10, 
        visionHeight=10
    )
    world.setMaxEnergy(1000.0)
    world.setRandomSeed(42)  # For reproducible experiments
    
    # Add environmental elements
    _add_environment_elements(world)
    
    # Set starting position
    world.setAgentStart(15.0, 15.0)
    world.reset()
    
    # 4. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 5. Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Starting Realistic Agent Simulation")
    print(f"=================================")
    print(f"Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"World: {world.getWidth()}x{world.getHeight()}, vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    print(f"Episodes: {episodes}")
    print()
    
    # 6. Simulation data collection
    episode_data = []
    learning_curve = []
    curiosity_over_time = []
    prediction_errors = []
    
    # 7. Run simulation
    for episode in range(episodes):
        episode_stats = {
            'episode': episode,
            'steps': 0,
            'total_reward': 0.0,
            'max_curiosity': 0.0,
            'avg_prediction_error': 0.0,
            'final_firing_rate': 0.0,
            'novel_events': 0
        }
        
        # Reset environment for new episode
        world.reset()
        world.setAgentStart(15.0, 15.0)
        
        # Run episode
        for step in range(50):  # 50 steps per episode
            # World update
            world.update(0.05)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            
            # Apply action and get results
            action_result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation with exploration bonus
            reward = action_result.reward
            if agent.getCuriosityLevel() > 0.5:  # Curiosity-driven exploration
                reward += 0.2
            
            agent.applyRewardModulation(reward, brain.getPredictionError())
            
            # Update development
            agent.updateDevelopment(0.05)
            
            # Collect statistics
            episode_stats['steps'] += 1
            episode_stats['total_reward'] += reward
            episode_stats['max_curiosity'] = max(episode_stats['max_curiosity'], agent.getCuriosityLevel())
            episode_stats['avg_prediction_error'] += agent.getPredictionError()
            episode_stats['final_firing_rate'] = brain.getAverageFiringRate()
            
            # Check for novel events
            if agent.getNoveltyLevel() > 0.7:
                episode_stats['novel_events'] += 1
        
        # Calculate episode averages
        episode_stats['avg_prediction_error'] /= episode_stats['steps']
        
        # Store episode data
        episode_data.append(episode_stats)
        learning_curve.append(episode_stats['total_reward'])
        curiosity_over_time.append(episode_stats['max_curiosity'])
        prediction_errors.append(episode_stats['avg_prediction_error'])
        
        # Print progress every 100 episodes
        if episode % 100 == 0:
            print(f"Episode {episode + 1}/{episodes}")
            print(f"  Total Reward: {episode_stats['total_reward']:.2f}")
            print(f"  Max Curiosity: {episode_stats['max_curiosity']:.3f}")
            print(f"  Avg Prediction Error: {episode_stats['avg_prediction_error']:.3f}")
            print(f"  Novel Events: {episode_stats['novel_events']}")
            print(f"  Firing Rate: {episode_stats['final_firing_rate']:.2f} Hz")
            print()
    
    # 8. Analyze results
    print(f"Simulation Complete!")
    print(f"==================")
    
    final_stats = episode_data[-1]
    
    print(f"Final Performance:")
    print(f"  Total Reward: {final_stats['total_reward']:.2f}")
    print(f"  Average Curiosity: {statistics.mean(curiosity_over_time):.3f}")
    print(f"  Average Prediction Error: {statistics.mean(prediction_errors):.3f}")
    print(f"  Final Firing Rate: {final_stats['final_firing_rate']:.2f} Hz")
    
    # Calculate learning metrics
    mid_point = len(learning_curve) // 2
    first_half_avg = statistics.mean(learning_curve[:mid_point])
    second_half_avg = statistics.mean(learning_curve[mid_point:])
    learning_improvement = ((second_half_avg - first_half_avg) / first_half_avg) * 100 if first_half_avg > 0 else 0
    
    print(f"\nLearning Analysis:")
    print(f"  First Half Average Reward: {first_half_avg:.2f}")
    print(f"  Second Half Average Reward: {second_half_avg:.2f}")
    print(f"  Learning Improvement: {learning_improvement:.1f}%")
    
    # Check for convergence
    last_10_episodes = learning_curve[-10:]
    episode_variance = statistics.stdev(last_10_episodes) if len(last_10_episodes) > 1 else 0
    
    print(f"\nConvergence Analysis:")
    print(f"  Last 10 Episodes Variance: {episode_variance:.3f}")
    print(f"  Performance Stable: {'Yes' if episode_variance < 0.5 else 'No'}")
    
    # Save results
    _save_simulation_results(episode_data, learning_curve, curiosity_over_time, prediction_errors)
    
    # Save final brain state
    brain.save("realistic_agent_final_brain.bin")
    print(f"\nFinal brain state saved to realistic_agent_final_brain.bin")
    
    return episode_data

# Helper functions for realistic simulation
def _add_environment_elements(world):
    """Add environmental elements like resources and hazards."""
    # Add resources
    for _ in range(8):
        x = random.uniform(5, 25)
        y = random.uniform(5, 25)
        resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=5.0)
        world.addObject(resource)
    
    # Add hazards
    for _ in range(3):
        x = random.uniform(25, 35)
        y = random.uniform(25, 35)
        hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=-3.0)
        world.addObject(hazard)
    
    # Add walls
    for x in [0, 40]:
        for y in range(0, 41, 2):
            wall = pynlm.WorldObject(float(x), float(y), pynlm.WorldObjectType.Wall, value=0.0)
            world.addObject(wall)
    
    for y in [0, 40]:
        for x in range(0, 41, 2):
            wall = pynlm.WorldObject(float(x), float(y), pynlm.WorldObjectType.Wall, value=0.0)
            world.addObject(wall)

def _save_simulation_results(episode_data, learning_curve, curiosity_over_time, prediction_errors):
    """Save simulation results to files."""
    results = {
        'episode_data': episode_data,
        'learning_curve': learning_curve,
        'curiosity_over_time': curiosity_over_time,
        'prediction_errors': prediction_errors,
        'metadata': {
            'simulation_type': 'realistic_agent',
            'total_episodes': len(episode_data),
            'timestamp': time.time()
        }
    }
    
    with open('realistic_agent_results.json', 'w') as f:
        json.dump(results, f, indent=2, default=str)
    
    print(f"Simulation results saved to realistic_agent_results.json")

# Run the realistic simulation (uncomment to execute)
# simulation_data = run_realistic_agent_simulation(1000)
```

### 3.2 Multi-Agent System

```python
import pynlm
import random
import statistics
import time
import json

def run_multi_agent_simulation(num_agents=3, episodes=500):
    """
    Run a multi-agent simulation where multiple agents learn together.
    This example shows interaction between multiple NLM brains.
    """
    print(f"Multi-Agent Simulation with {num_agents} Agents")
    print("=" * 45)
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=12, visionHeight=12)
    world.setMaxEnergy(5000.0)
    world.setRandomSeed(123)
    world.reset()
    
    # Add shared environmental elements
    for _ in range(15):
        x = random.uniform(10, 40)
        y = random.uniform(10, 40)
        resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=3.0)
        world.addObject(resource)
    
    # Create multiple agents with different configurations
    agents = []
    brains = []
    
    for agent_id in range(num_agents):
        # Each agent can have different characteristics
        config = pynlm.createDefaultConfig()
        
        # Vary parameters to create different agent types
        if agent_id == 0:
            # Fast learner
            config.set("brain.neuron_count", 1500)
            config.set("plasticity.stdp.learning_rate", 0.003)
            config.set("neuromod.curiosity.sensitivity", 0.9)
            agent_type = "Fast Learner"
        elif agent_id == 1:
            # Conservative learner
            config.set("brain.neuron_count", 3000)
            config.set("plasticity.stdp.learning_rate", 0.001)
            config.set("neuromod.curiosity.sensitivity", 0.5)
            agent_type = "Conservative"
        else:
            # Curious explorer
            config.set("brain.neuron_count", 2000)
            config.set("plasticity.stdp.learning_rate", 0.002)
            config.set("neuromod.curiosity.sensitivity", 0.8)
            config.set("neuromod.novelty.threshold", 0.2)
            agent_type = "Curious Explorer"
        
        # Set different starting positions
        start_x = 10 + agent_id * 15
        start_y = 10 + agent_id * 10
        
        # Create brain and agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        agents.append(agent)
        brains.append(brain)
        
        print(f"Agent {agent_id + 1} ({agent_type}): {brain.getTotalNeuronCount()} neurons")
    
    # Multi-agent simulation
    print(f"\nRunning Multi-Agent Simulation ({episodes} episodes)")
    
    # Track social learning
    social_interactions = []
    agent_performance = []
    
    for episode in range(episodes):
        episode_data = {
            'episode': episode,
            'total_reward': 0.0,
            'individual_rewards': [],
            'social_events': 0,
            'coordination_events': 0
        }
        
        # Reset world for new episode
        world.reset()
        
        # Reset all agents
        for agent in agents:
            agent.reset()
        
        # Run episode for all agents
        for step in range(30):
            # Update world
            world.update(0.04)
            
            # Each agent perceives the same world state
            percept = world.getSensoryPercept()
            
            # Process through all agents
            agent_rewards = []
            for agent_idx, agent in enumerate(agents):
                agent.processSensoryInput(percept)
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                action_result = world.applyMotorCommand(action, world.getSimulationTime())
                
                agent.applyRewardModulation(action_result.reward, brain.getPredictionError())
                agent.updateDevelopment(0.04)
                
                agent_rewards.append(action_result.reward)
            
            # Check for social interactions
            reward_variance = max(agent_rewards) - min(agent_rewards)
            if reward_variance > 2.0:  # High variance indicates potential social learning opportunity
                episode_data['social_events'] += 1
            
            if len(agent_rewards) > 1:
                # Simple coordination check (all agents getting similar rewards)
                avg_reward = sum(agent_rewards) / len(agent_rewards)
                coordination = all(abs(r - avg_reward) < 1.0 for r in agent_rewards)
                if coordination:
                    episode_data['coordination_events'] += 1
            
            episode_data['individual_rewards'].append(agent_rewards)
            episode_data['total_reward'] += sum(agent_rewards)
        
        agent_performance.append(episode_data)
        
        if episode % 100 == 0:
            print(f"Episode {episode + 1}/{episodes}")
            print(f"  Total Reward: {episode_data['total_reward']:.2f}")
            print(f"  Social Events: {episode_data['social_events']}")
            print(f"  Coordination Events: {episode_data['coordination_events']}")
    
    # Analyze multi-agent results
    print(f"\nMulti-Agent Analysis:")
    
    total_reward = sum(ep['total_reward'] for ep in agent_performance)
    avg_social_events = sum(ep['social_events'] for ep in agent_performance) / len(agent_performance)
    avg_coordination = sum(ep['coordination_events'] for ep in agent_performance) / len(agent_performance)
    
    print(f"  Total Reward: {total_reward:.2f}")
    print(f"  Average Social Events: {avg_social_events:.1f}")
    print(f"  Average Coordination Events: {avg_coordination:.1f}")
    
    # Individual agent performance
    print(f"\nIndividual Agent Performance:")
    for i, brain in enumerate(brains):
        final_firing_rate = brain.getAverageFiringRate()
        total_spikes = brain.getTotalSpikeCount()
        print(f"  Agent {i + 1}: {final_firing_rate:.2f} Hz, {total_spikes} total spikes")
    
    # Save multi-agent results
    multi_agent_results = {
        'agent_performance': agent_performance,
        'num_agents': num_agents,
        'episodes': episodes,
        'final_brain_states': [
            {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate()
            }
            for brain in brains
        ],
        'timestamp': time.time()
    }
    
    with open('multi_agent_results.json', 'w') as f:
        json.dump(multi_agent_results, f, indent=2, default=str)
    
    print(f"\nMulti-agent results saved to multi_agent_results.json")
    
    # Save all brain states
    for i, brain in enumerate(brains):
        brain.save(f"multi_agent_{i+1}_brain.bin")
    
    print(f"Individual brain states saved")
    
    print(f"\nMulti-agent simulation complete!")
    return agent_performance

# Run the multi-agent simulation (uncomment to execute)
# multi_agent_data = run_multi_agent_simulation(3, 500)
```

---

## Part 4: Configuration Options

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
