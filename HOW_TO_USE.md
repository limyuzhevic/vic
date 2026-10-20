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

### Memory System Examples

#### Working Memory (Short-term Information Holding)

```python
import pynlm

# Create a configured brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
brain = pynlm.createBrain(config)
brain.initialize()

# Get working memory system
working_mem = brain.getWorkingMemory()
working_mem.setCapacity(100)  # Store up to 100 active patterns

# Store current sensory patterns in working memory
agent = pynlm.createAgentBrain(brain)
for step in range(100):
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Process sensory input and store in working memory
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Store active neuron patterns
    active_neurons = brain.getWorkingMemory().getMemoryNeurons()
    working_mem.store(f"step_{step}", active_neurons)
    
    brain.step(step)
    
    # Retrieve recent patterns for planning
    recent_patterns = working_mem.recallPattern(0.7)  # Patterns with similarity > 0.7
    if recent_patterns:
        print(f"Retrieved {len(recent_patterns)} similar patterns from working memory")

# Clear working memory when no longer needed
working_mem.clear()
```

#### Episodic Memory (Event Storage and Replay)

```python
import pynlm
import time

# Create brain with episodic memory capabilities
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
brain = pynlm.createBrain(config)
brain.initialize()

# Get episodic memory system
episodic_mem = brain.getEpisodicMemory()
episodic_mem.setMaxEpisodes(1000)

# Simulate an agent learning from experiences
agent = pynlm.createAgentBrain(brain)
agent.enableCuriosity(True)  # Enable exploration

for episode in range(50):
    # Create environment
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    agent.initialize(world)
    
    # Run episode
    episode_start_time = time.time()
    for step in range(100):
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Make decisions
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record reward
        reward = 0.0  # Would be computed from world state
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    # Store episode in episodic memory
    episode_data = {
        'start_pos': (7.5, 7.5),
        'duration': time.time() - episode_start_time,
        'reward_sum': sum(reward),
        'final_state': 'agent_position'
    }
    episodic_mem.encode(episode_data, episode)
    
    print(f"Stored episode {episode + 1} in episodic memory")

# Replay important episodes (e.g., successful learning experiences)
print("\nReplaying successful episodes:")
successful_episodes = episodic_mem.getEpisodesForReplay(5)
for i, episode in enumerate(successful_episodes):
    print(f"Episode {i + 1}: replay with consolidation")
    episodic_mem.replayEpisode(episode)

# Consolidate weak memories to strengthen important ones
episodic_mem.consolidate(0.3)  # Keep memories with activation > 0.3
```

#### Associative Memory (Pattern Association)

```python
import pynlm

# Create brain with associative memory
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get associative memory system
assoc_mem = brain.getAssociativeMemory()

# Store associations between patterns
# Example: associate 'red' with 'stop', 'blue' with 'go'
assoc_mem.associate("red", [0.9, 0.1, 0.05, 0.05])
assoc_mem.associate("blue", [0.05, 0.1, 0.9, 0.05])
assoc_mem.associate("green", [0.1, 0.05, 0.9, 0.05])

# Test pattern completion
patterns_to_complete = ["red", "blu", "gren"]
for pattern in patterns_to_complete:
    completed = assoc_mem.complete(pattern)
    if completed:
        print(f"Pattern '{pattern}' completed as: {completed[:2]}...")
    else:
        print(f"Pattern '{pattern}' not recognized")

# Store sequences for procedural memory
assoc_mem.storeSequence("approach", [1.0, 0.8, 0.6, 0.4, 0.2, 0.0])
retrieved_sequence = assoc_mem.getSequence("approach")
print(f"Retrieved sequence 'approach': {retrieved_sequence}")

# Use Hebbian learning for pattern formation
assoc_mem.enableHebbianLearning(True)
assoc_mem.setHebbianLearningRate(0.01)
```

### Neuromodulation System Examples

#### Comprehensive Neuromodulation Example

```python
import pynlm

# Create agent with full neuromodulation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Enable all neuromodulation systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Create environment
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)
agent.initialize(world)

print("=== Running Neuromodulation Simulation ===\n")

# Run extended simulation with neuromodulation
for step in range(500):
    # Update environment
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Neural processing
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation (simplified reward function)
    # Reward is based on proximity to center
    agent_body = world.getAgentBody()
    distance_from_center = ((agent_body.x - 10.0) ** 2 + (agent_body.y - 10.0) ** 2) ** 0.5
    reward = max(0.0, 1.0 - distance_from_center / 15.0)
    
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Log neuromodulation state
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Curiosity Level:     {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty Level:       {agent.getNoveltyLevel():.3f}")
        print(f"  Neuromodulation:     {agent.getNeuromodulationLevel():.3f}")
        print(f"  Prediction Error:    {agent.getPredictionError():.3f}")
        print(f"  Developmental Stage: {agent.getDevelopmentalStage()}")
        print(f"  Brain Stage:         {brain.getDevelopmentalStage()}")
        print()

print("=== Neuromodulation Simulation Complete ===")
```

#### Dopamine-Driven Learning Example

```python
import pynlm

# Setup dopamine system for reward-based learning
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableDevelopment(True)

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

print("\n=== Dopamine-Driven Learning Simulation ===\n")

# Learning simulation with dopamine modulation
learning_sessions = []

for session in range(20):
    session_rewards = []
    session_errors = []
    
    for step in range(200):
        # Explore environment
        world.update(0.1)
        
        # Process new sensory information
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Make decisions
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Compute reward based on reaching goals
        agent_body = world.getAgentBody()
        goal_pos = (25.0, 25.0)  # Target location
        distance_to_goal = ((agent_body.x - goal_pos[0]) ** 2 + 
                          (agent_body.y - goal_pos[1]) ** 2) ** 0.5
        
        reward = max(0.0, 1.0 - distance_to_goal / 40.0)
        predicted_reward = 0.0  # Simple prediction model
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, predicted_reward)
        session_rewards.append(reward)
        session_errors.append(agent.getPredictionError())
        
        # Update development
        agent.updateDevelopment(0.1)
    
    learning_sessions.append({
        'session': session + 1,
        'mean_reward': sum(session_rewards) / len(session_rewards),
        'mean_error': sum(session_errors) / len(session_errors),
        'dopamine_level': agent.getNeuromodulationLevel()
    })
    
    print(f"Session {session + 1}: Mean Reward = {learning_sessions[-1]['mean_reward']:.3f}, "
          f"Dopamine = {learning_sessions[-1]['dopamine_level']:.3f}")

print("\n=== Learning Performance Summary ===")
for session in learning_sessions:
    print(f"Session {session['session']}: Reward={session['mean_reward']:.3f}, "
          f"Error={session['mean_error']:.3f}, Dopamine={session['dopamine_level']:.3f}")
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

---

### Batch Processing Utilities

NLM provides several utility functions for efficient batch processing and advanced usage patterns:

#### Batch Simulation Runner

```python
import pynlm
import numpy as np

class BatchSimulationRunner:
    """Efficient batch simulation for multiple experiments."""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.results = []
    
    def run_batch_simulation(self, num_experiments=10, steps_per_experiment=1000, 
                           random_seed=None):
        """Run multiple experiments in batch mode."""
        
        for exp in range(num_experiments):
            # Set unique seed for reproducibility
            if random_seed is not None:
                self.config.set("random_seed", random_seed + exp)
            
            # Create fresh brain for each experiment
            brain = pynlm.createBrain(self.config)
            brain.initialize()
            
            # Create agent with learning capabilities
            agent = pynlm.createAgentBrain(brain)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            
            # Create environment
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            world.setAgentStart(10.0, 10.0)
            agent.initialize(world)
            
            # Run experiment
            experiment_results = self._run_experiment(brain, agent, world, steps_per_experiment)
            self.results.append(experiment_results)
            
            print(f"Completed experiment {exp + 1}/{num_experiments}")
        
        return self.results
    
    def _run_experiment(self, brain, agent, world, steps):
        """Run a single experiment and collect results."""
        results = {
            'total_spikes': 0,
            'firing_rate': 0.0,
            'curiosity_levels': [],
            'novelty_levels': [],
            'prediction_errors': [],
            'rewards': []
        }
        
        for step in range(steps):
            # Update environment
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Neural processing
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect metrics
            results['total_spikes'] += brain.getFiringNeuronCount()
            results['curiosity_levels'].append(agent.getCuriosityLevel())
            results['novelty_levels'].append(agent.getNoveltyLevel())
            results['prediction_errors'].append(agent.getPredictionError())
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Calculate averages
        results['firing_rate'] = results['total_spikes'] / steps if steps > 0 else 0.0
        
        return results
    
    def get_batch_summary(self):
        """Get summary statistics across all batch experiments."""
        if not self.results:
            return None
        
        avg_spikes = np.mean([r['total_spikes'] for r in self.results])
        avg_firing_rate = np.mean([r['firing_rate'] for r in self.results])
        avg_curiosity = np.mean([np.mean(r['curiosity_levels']) for r in self.results])
        avg_novelty = np.mean([np.mean(r['novelty_levels']) for r in self.results])
        
        return {
            'num_experiments': len(self.results),
            'avg_total_spikes': avg_spikes,
            'avg_firing_rate': avg_firing_rate,
            'avg_curiosity_level': avg_curiosity,
            'avg_novelty_level': avg_novelty
        }

# Example usage:
# runner = BatchSimulationRunner()
# results = runner.run_batch_simulation(num_experiments=5, steps_per_experiment=1000)
# summary = runner.get_batch_summary()
# print(f"Batch results: {summary}")
```

#### Multi-Agent Batch Processing

```python
import pynlm

class MultiAgentBatchSimulator:
    """Simulate multiple brains/agents in parallel for comparative analysis."""
    
    def __init__(self):
        self.configs = []
        self.brains = []
        self.agents = []
        self.worlds = []
    
    def create_experiment_set(self, num_agents=5, base_neuron_count=1000):
        """Create a set of agents with different configurations for comparison."""
        
        for i in range(num_agents):
            # Create unique configuration for each agent
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", base_neuron_count + i * 200)
            config.set("brain.synapse_density", 0.05 + i * 0.01)
            config.set("random_seed", 42 + i)  # Different seed for each agent
            
            self.configs.append(config)
        
        return self.configs
    
    def run_comparison_experiment(self, steps=500):
        """Run the same environment with multiple agents for comparison."""
        
        # Create environments
        for i in range(len(self.configs)):
            brain = pynlm.createBrain(self.configs[i])
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            world.setAgentStart(10.0, 10.0)
            agent.initialize(world)
            
            self.brains.append(brain)
            self.agents.append(agent)
            self.worlds.append(world)
        
        # Run comparative experiment
        comparative_results = {}
        
        for agent_idx, (brain, agent, world) in enumerate(zip(self.brains, self.agents, self.worlds)):
            print(f"\nRunning agent {agent_idx + 1}/{len(self.brains)}...")
            
            agent_results = {
                'agent_id': agent_idx,
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'total_spikes': 0,
                'curiosity_levels': [],
                'performance_metrics': []
            }
            
            for step in range(steps):
                # Same environment state for all agents
                world.update(0.1)
                
                # Each agent processes the same percept
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Agent-specific neural processing
                brain.step(step)
                
                # Collect metrics
                agent_results['total_spikes'] += brain.getFiringNeuronCount()
                agent_results['curiosity_levels'].append(agent.getCuriosityLevel())
                
                # Calculate performance metric (distance to goal)
                agent_body = world.getAgentBody()
                goal_pos = (15.0, 15.0)
                distance = ((agent_body.x - goal_pos[0]) ** 2 + 
                          (agent_body.y - goal_pos[1]) ** 2) ** 0.5
                performance = max(0.0, 1.0 - distance / 20.0)
                agent_results['performance_metrics'].append(performance)
                
                # Update agent development
                agent.updateDevelopment(0.1)
            
            comparative_results[agent_idx] = agent_results
        
        return comparative_results
    
    def analyze_differences(self, results):
        """Analyze differences between agents and identify optimal configuration."""
        
        print("\n=== Comparative Analysis ===")
        
        # Extract metrics
        metrics = {}
        for agent_id, result in results.items():
            metrics[agent_id] = {
                'final_firing_rate': result['total_spikes'] / len(result['curiosity_levels']),
                'avg_curiosity': sum(result['curiosity_levels']) / len(result['curiosity_levels']),
                'avg_performance': sum(result['performance_metrics']) / len(result['performance_metrics']),
                'neuron_count': result['neuron_count']
            }
        
        # Find best performing agent
        best_agent = max(metrics.items(), key=lambda x: x[1]['avg_performance'])
        
        print("Agent Performance Summary:")
        for agent_id, metric in metrics.items():
            print(f"  Agent {agent_id}: "
                  f"Performance={metric['avg_performance']:.3f}, "
                  f"Firing={metric['final_firing_rate']:.1f}, "
                  f"Curiosity={metric['avg_curiosity']:.3f}, "
                  f"Neurons={metric['neuron_count']}")
        
        print(f"\nBest performing agent: {best_agent[0]} "
              f"(Performance: {best_agent[1]['avg_performance']:.3f})")
        
        return best_agent

# Example usage:
# simulator = MultiAgentBatchSimulator()
# configs = simulator.create_experiment_set(num_agents=3)
# results = simulator.run_comparison_experiment(steps=300)
# best = simulator.analyze_differences(results)
```

#### Advanced Checkpoint and Replay Utilities

```python
import pynlm
import os

class AdvancedCheckpointManager:
    """Advanced checkpoint management for long-running experiments."""
    
    def __init__(self, checkpoint_dir="./nlm_checkpoints"):
        self.checkpoint_dir = checkpoint_dir
        os.makedirs(checkpoint_dir, exist_ok=True)
        self.checkpoint_files = []
    
    def save_agent_state(self, agent, world, experiment_id, step):
        """Save detailed agent state including neural activity and environment."""
        
        checkpoint_data = {
            'experiment_id': experiment_id,
            'step': step,
            'brain_state': self._serialize_brain(agent.getBrain()),
            'agent_metrics': {
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'neuromodulation': agent.getNeuromodulationLevel(),
                'developmental_stage': agent.getDevelopmentalStage()
            },
            'world_state': self._serialize_world(world),
            'timestamp': step * 0.1  # Assuming 0.1 timestep
        }
        
        # Create checkpoint filename
        checkpoint_filename = f"experiment_{experiment_id}_step_{step:06d}.nlm"
        filepath = os.path.join(self.checkpoint_dir, checkpoint_filename)
        
        # Save checkpoint
        import json
        with open(filepath, 'w') as f:
            json.dump(checkpoint_data, f, indent=2, default=str)
        
        self.checkpoint_files.append(filepath)
        print(f"Saved checkpoint: {filepath}")
        
        return filepath
    
    def _serialize_brain(self, brain):
        """Serialize brain state for checkpointing."""
        return {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'total_spike_count': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'developmental_stage': brain.getDevelopmentalStage()
        }
    
    def _serialize_world(self, world):
        """Serialize world state for checkpointing."""
        return {
            'width': world.getWidth(),
            'height': world.getHeight(),
            'simulation_time': world.getSimulationTime(),
            'max_energy': world.getMaxEnergy(),
            'agent_position': (world.getAgentBody().x, world.getAgentBody().y),
            'agent_orientation': world.getAgentBody().orientation
        }
    
    def load_checkpoint(self, checkpoint_filepath):
        """Load agent state from checkpoint."""
        import json
        
        with open(checkpoint_filepath, 'r') as f:
            checkpoint_data = json.load(f)
        
        print(f"Loaded checkpoint: {checkpoint_filepath}")
        print(f"  Experiment ID: {checkpoint_data['experiment_id']}")
        print(f"  Step: {checkpoint_data['step']}")
        print(f"  Neurons: {checkpoint_data['brain_state']['total_neurons']}")
        print(f"  Synapses: {checkpoint_data['brain_state']['total_synapses']}")
        
        return checkpoint_data
    
    def find_checkpoints(self, experiment_id=None, min_step=None):
        """Find checkpoints matching criteria."""
        checkpoints = []
        
        for checkpoint_file in self.checkpoint_files:
            filename = os.path.basename(checkpoint_file)
            
            # Parse filename
            import re
            match = re.match(r'experiment_(\d+)_step_(\d+)\.nlm', filename)
            if match:
                exp_id = int(match.group(1))
                step = int(match.group(2))
                
                # Apply filters
                if experiment_id is not None and exp_id != experiment_id:
                    continue
                if min_step is not None and step < min_step:
                    continue
                
                checkpoints.append({
                    'filepath': checkpoint_file,
                    'experiment_id': exp_id,
                    'step': step
                })
        
        return sorted(checkpoints, key=lambda x: x['step'])
    
    def cleanup_old_checkpoints(self, experiment_id, keep_last_n=5):
        """Remove old checkpoints, keeping only the last N for each experiment."""
        checkpoints = self.find_checkpoints(experiment_id=experiment_id)
        
        if len(checkpoints) <= keep_last_n:
            print(f"No cleanup needed for experiment {experiment_id}")
            return
        
        # Remove oldest checkpoints
        for checkpoint in checkpoints[:-keep_last_n]:
            try:
                os.remove(checkpoint['filepath'])
                self.checkpoint_files.remove(checkpoint['filepath'])
                print(f"Removed old checkpoint: {checkpoint['filepath']}")
            except Exception as e:
                print(f"Error removing checkpoint {checkpoint['filepath']}: {e}")

# Example usage:
# manager = AdvancedCheckpointManager()
# 
# # During experiment run
# for step in range(0, 5000, 100):
#     manager.save_agent_state(agent, world, experiment_id=1, step=step)
# 
# # Load specific checkpoint
# checkpoint = manager.load_checkpoint("./nlm_checkpoints/experiment_1_step_010000.nlm")
# 
# # Cleanup old checkpoints
# manager.cleanup_old_checkpoints(experiment_id=1, keep_last_n=10)
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
