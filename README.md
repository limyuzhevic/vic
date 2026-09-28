# NLM Python API - Comprehensive Documentation

**Improved and Extended Version**

This document provides comprehensive documentation for the NLM Python bindings, incorporating all existing examples and adding new functionality for advanced users.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Pythonic API Features](#pythonic-api-features)
3. [Basic Usage Examples](#basic-usage-examples)
4. [Advanced Usage](#advanced-usage)
5. [Configuration Management](#configuration-management)
6. [Performance Monitoring](#performance-monitoring)
7. [Simulation Utilities](#simulation-utilities)
8. [Common Patterns](#common-patterns)
9. [Troubleshooting](#troubleshooting)
10. [Advanced Features](#advanced-features)

---

## Quick Start

NLM is a brain-inspired spiking neural network simulator with enhanced Python bindings for ease of use.

### Installation

```bash
pip install pynlm
# Or install from source
pip install -e .
```

### Basic Usage

```python
import pynlm

# Create and initialize a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
stats = brain.get_stats()
print(f"Firing neurons: {stats['firing_neurons']}")
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
    agent.enable_all()  # Enables reward modulation, curiosity, development, etc.
    
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
        action = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Display progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

# Run the simulation
run_agent_simulation(1000)
```

---

## Pythonic API Features

### 1. Property Decorators
Many classes now use Pythonic property decorators for cleaner access:

```python
# Before
config.set("name", "my_brain")
config.get("name")

# After
config.name = "my_brain"  # Using property
print(config.name)        # Automatic getter
```

### 2. Context Managers
Automatic resource management with context managers:

```python
# Context manager for brain
with pynlm.createBrain(config) as brain:
    brain.initialize()
    for step in range(100):
        brain.step(step)
    # Brain is automatically cleaned up

# Context manager for world  
with pynlm.createSimpleWorld() as world:
    world.configure(width=10, height=10)
    world.reset()
    # World is automatically reset on exit
```

### 3. Convenience Factory Methods
Simplified object creation:

```python
# Action creation
move_action = pynlm.Action.createMoveForward()
look_action = pynlm.Action.createLook()
interact_action = pynlm.Action.createInteract()

# World object creation
world.add_resource(5.0, 5.0, 1.0)  # Auto-create Resource object
world.add_hazard(3.0, 3.0, -1.0)   # Auto-create Hazard object
```

### 4. Enhanced Statistics
Comprehensive statistics in dictionary format:

```python
stats = brain.get_stats()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"E/I balance: {stats['e_i_ratio']:.3f}")
```

### 5. Type Safety
Runtime type checking and validation:

```python
# Type-safe configuration
config.set("brain.neuron_count", 1000)  # int
config.set("brain.learning_rate", 0.001)  # float
config.set("brain.enabled", True)  # bool
```

---

## Basic Usage Examples

### Example 1: Silent Brain (Basic Neural Activity)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print(f"Statistics: {brain.get_stats()}")
```

### Example 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)

print("Watched world for 30 steps")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Example 3: Complete Agent with Learning

```python
import pynlm

# Setup with learning enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning systems
agent.enable_all()

# Run agent simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        stats = brain.get_stats()
        print(f"Step {step}: {stats['firing_neurons']} neurons firing")

print("Agent simulation complete!")
```

---

## Advanced Usage

### 1. Brain State Management

```python
import pynlm
import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some steps
for step in range(100):
    brain.step(step)

# Save brain state
brain.save("checkpoint1.bin")
print("Brain saved to checkpoint1.bin")

# Modify brain (e.g., change developmental stage)
brain.set_developmental_stage(pynlm.DevelopmentalStage.Adult)

# Continue simulation
for step in range(100, 200):
    brain.step(step)

# Load previous state
brain.load("checkpoint1.bin")
print("Brain loaded from checkpoint1.bin")

# Verify restoration
print(f"Restored neurons: {brain.get_total_neuron_count()}")
```

### 2. Advanced World Configuration

```python
import pynlm

world = pynlm.createSimpleWorld()

# Configure with custom parameters
world.configure(
    width=50,
    height=50,
    visionWidth=16,
    visionHeight=16,
    maxEnergy=100.0,
    energyDecayRate=0.95
)

# Add environmental objects
world.add_resource(10.0, 10.0, 5.0)      # Resource with value 5
world.add_resource(20.0, 20.0, 2.0)      # Another resource
world.add_hazard(30.0, 30.0, -1.0)       # Hazard with value -1
world.add_wall(40.0, 40.0, 1.0)          # Wall with radius 1

# Set up agent path
agent_start = world.set_agent_start(5.0, 5.0)

# Run simulation with agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enable_all()

print(f"World configured: {world.get_width()}x{world.get_height()} pixels")
print(f"Agent start: {agent_start}")
print(f"Objects in world: {len(world.get_objects())}")
```

### 3. Configuration Management

```python
import pynlm

# Create and configure
config = pynlm.createDefaultConfig()

# Set multiple parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("brain.v_thresh", -45.0)
config.set("brain.tau_mem", 25.0)

# Enable specific plasticity mechanisms
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Save configuration
config.save_to_file("my_config.json")

# Load configuration from file
config2 = pynlm.createDefaultConfig()
config2.load_from_file("my_config.json")

print(f"Config keys: {config2.get_keys()}")
print(f"Neuron count: {config2.get('brain.neuron_count')}")
```

---

## Configuration Management

### Configuration Parameters

| Category | Parameter | Type | Default | Description |
|----------|-----------|------|---------|-------------|
| Brain | `brain.neuron_count` | int | 1000 | Total number of neurons |
| Brain | `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| Brain | `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| Brain | `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| Plasticity | `plasticity.stdp.enable` | bool | true | Enable STDP |
| Plasticity | `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| Neuromodulation | `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| Neuromodulation | `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |

### Configuration Operations

```python
import pynlm

config = pynlm.createDefaultConfig()

# Query configuration
if config.has("brain.neuron_count"):
    print(f"Current neuron count: {config.get('brain.neuron_count')}")

# Get all configuration keys
keys = config.get_keys()
print(f"All keys: {keys}")

# Check configuration source
if config.has("brain.learning_rate"):
    value = config.get("brain.learning_rate")

# Modify configuration with type safety
config.set("brain.neuron_count", 1500)
config.set("brain.learning_rate", 0.01)
config.set("brain.enabled", True)

# Configuration summary
print(config.summary())
```

---

## Performance Monitoring

### Brain Performance Metrics

```python
import pynlm
import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Performance monitoring
start_time = time.time()
steps_completed = 0
spike_count = 0

for step in range(1000):
    brain.step(step)
    steps_completed += 1
    spike_count = brain.get_total_spike_count()

end_time = time.time()
elapsed = end_time - start_time

# Calculate performance metrics
steps_per_second = steps_completed / elapsed if elapsed > 0 else 0
spikes_per_second = spike_count / elapsed if elapsed > 0 else 0

print(f"Performance Metrics:")
print(f"  Steps completed: {steps_completed}")
print(f"  Time elapsed: {elapsed:.2f} seconds")
print(f"  Steps per second: {steps_per_second:.1f}")
print(f"  Total spikes: {spike_count}")
print(f"  Spikes per second: {spikes_per_second:.1f}")

# Get detailed statistics
stats = brain.get_stats()
print(f"\nDetailed Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")
```

### Advanced Performance Analysis

```python
import pynlm
import matplotlib.pyplot as plt

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Collect performance data
steps = list(range(0, 1000, 10))
firing_rates = []
spike_counts = []
neuro_synchrony = []

episode_num = 0
for step in steps:
    brain.step(step)
    
    if step % 10 == 0:
        # Collect metrics
        firing_rate = brain.get_average_firing_rate()
        spike_count = brain.get_total_spike_count()
        
        # Calculate synchrony (simplified)
        active_neurons = brain.get_firing_neuron_count()
        total_neurons = brain.get_total_neuron_count()
        synchrony = active_neurons / total_neurons if total_neurons > 0 else 0
        
        firing_rates.append(firing_rate)
        spike_counts.append(spike_count)
        neuro_synchrony.append(synchrony)

# Performance visualization (if matplotlib available)
try:
    fig, axes = plt.subplots(3, 1, figsize=(10, 8))
    
    axes[0].plot(steps[::10], firing_rates)
    axes[0].set_title("Average Firing Rate")
    axes[0].set_ylabel("Hz")
    
    axes[1].plot(steps[::10], spike_counts)
    axes[1].set_title("Total Spike Count")
    axes[1].set_ylabel("Spikes")
    
    axes[2].plot(steps[::10], neuro_synchrony)
    axes[2].set_title("Neural Synchrony")
    axes[2].set_ylabel("0-1")
    axes[2].set_xlabel("Simulation Step")
    
    plt.tight_layout()
    plt.savefig('performance_analysis.png')
    print("Performance analysis saved to performance_analysis.png")
    plt.show()
except ImportError:
    print("Matplotlib not available - skipping visualization")
```

---

## Simulation Utilities

### 1. Episode-based Simulation

```python
import pynlm

class EpisodeSimulator:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.episode_length = 100
        self.step_count = 0
        self.reward_history = []
        self.performance_data = []
    
    def run_episode(self, episode_num=0):
        """Run a single episode of simulation."""
        self.step_count = 0
        self.reward_history = []
        self.performance_data = []
        
        print(f"Starting Episode {episode_num + 1}")
        
        for step in range(self.episode_length):
            # Update world
            self.world.update(0.1)
            
            # Get and process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Decode action
            action = self.agent.decodeMotorCommand()
            
            # Apply action and get reward
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Collect reward
            reward = self._calculate_reward()
            self.reward_history.append(reward)
            
            # Collect performance data
            self.performance_data.append({
                'step': step,
                'firing_rate': self.brain.get_average_firing_rate(),
                'curiosity': self.agent.get_curiosity_level(),
                'novelty': self.agent.get_novelty_level(),
                'reward': reward
            })
            
            self.step_count += 1
            
            if step % 20 == 0:
                print(f"  Step {step}: Reward={reward:.3f}, "
                      f"Firing={self.brain.get_firing_neuron_count()}")
        
        self._analyze_episode()
        return self.get_episode_summary()
    
    def _calculate_reward(self):
        """Calculate reward based on agent's performance."""
        # Simple reward: positive for resource collection, negative for hazards
        percept = self.world.getSensoryPercept()
        internal = percept.get_internal()
        if internal and len(internal) > 0:
            return internal[0]
        return 0.0
    
    def _analyze_episode(self):
        """Analyze episode performance."""
        if self.reward_history:
            self.total_reward = sum(self.reward_history)
            self.max_reward = max(self.reward_history)
            self.min_reward = min(self.reward_history)
            self.avg_reward = self.total_reward / len(self.reward_history)
    
    def get_episode_summary(self):
        """Get summary of episode performance."""
        return {
            'episode': self.step_count,
            'total_reward': self.total_reward,
            'avg_reward': self.avg_reward,
            'max_reward': self.max_reward,
            'min_reward': self.min_reward,
            'final_curiosity': self.agent.get_curiosity_level(),
            'final_novelty': self.agent.get_novelty_level()
        }

# Usage example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enable_all()

# Run multiple episodes
simulator = EpisodeSimulator(brain, world, agent)

episode_summaries = []
for episode in range(3):
    summary = simulator.run_episode(episode)
    episode_summaries.append(summary)
    print(f"Episode {episode} Summary: {summary}")

print("\nAll Episodes Summary:")
for i, summary in enumerate(episode_summaries):
    print(f"  Episode {i + 1}: Avg Reward={summary['avg_reward']:.3f}, "
          f"Total Reward={summary['total_reward']:.3f}")
```

### 2. Batch Processing

```python
import pynlm
import time

class BatchProcessor:
    def __init__(self, brain_template, world_template):
        self.brain_template = brain_template
        self.world_template = world_template
        self.results = []
    
    def run_batch(self, num_simulations, steps_per_simulation):
        """Run a batch of simulations with different initial conditions."""
        print(f"Running batch of {num_simulations} simulations")
        print(f"Each simulation: {steps_per_simulation} steps")
        
        start_time = time.time()
        
        for i in range(num_simulations):
            # Create fresh simulation for each iteration
            brain = pynlm.createBrain(self.brain_template)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=10, height=10)
            world.reset()
            world.setRandomSeed(i * 12345)  # Different seed for each simulation
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agent.enable_all()
            
            # Run simulation
            for step in range(steps_per_simulation):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect results
            stats = brain.get_stats()
            stats['simulation_id'] = i
            stats['total_reward'] = sum(world.getSensoryPercept().getInternal() or [0])
            self.results.append(stats)
            
            if (i + 1) % 10 == 0:
                print(f"  Completed {i + 1}/{num_simulations} simulations")
        
        end_time = time.time()
        self.total_time = end_time - start_time
        
        print(f"Batch completed in {self.total_time:.2f} seconds")
        return self.get_batch_summary()
    
    def get_batch_summary(self):
        """Get summary of batch results."""
        if not self.results:
            return {}
        
        avg_neurons = sum(r['total_neurons'] for r in self.results) / len(self.results)
        avg_firing = sum(r['firing_neurons'] for r in self.results) / len(self.results)
        avg_spikes = sum(r['total_spikes'] for r in self.results) / len(self.results)
        
        return {
            'total_simulations': len(self.results),
            'total_time': self.total_time,
            'average_time_per_simulation': self.total_time / len(self.results),
            'average_neurons': avg_neurons,
            'average_firing_neurons': avg_firing,
            'average_total_spikes': avg_spikes,
            'min_neurons': min(r['total_neurons'] for r in self.results),
            'max_neurons': max(r['total_neurons'] for r in self.results)
        }

# Usage example
brain_template = pynlm.createDefaultConfig()
world_template = pynlm.createSimpleWorld()

batch_processor = BatchProcessor(brain_template, world_template)
summary = batch_processor.run_batch(50, 100)

print("\nBatch Processing Summary:")
for key, value in summary.items():
    print(f"  {key}: {value}")
```

---

## Common Patterns

### Pattern 1: Reinforcement Learning Setup

```python
import pynlm

def setup_reinforcement_learning(config):
    """Setup NLM for reinforcement learning."""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable RL-specific systems
    agent.enable_reward_modulation(True)  # Learn from rewards
    agent.enable_curiosity(True)          # Explore unknown areas
    agent.enable_development(True)        # Brain matures
    
    return brain, world, agent

def run_rl_episode(brain, world, agent, max_steps=200):
    """Run a single RL episode."""
    episode_reward = 0.0
    episode_steps = 0
    
    for step in range(max_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward
        reward = calculate_reward(world, brain)
        agent.applyRewardModulation(reward, 0.0)  # No predicted reward yet
        
        episode_reward += reward
        episode_steps += 1
        
        # Break if agent reaches goal or time limit
        if is_episode_finished(world, step):
            break
    
    agent.updateDevelopment(0.1)  # Update development
    return episode_reward, episode_steps

# Example RL configuration
rl_config = pynlm.createDefaultConfig()
rl_config.set("brain.neuron_count", 1500)
rl_config.set("plasticity.stdp.enable", True)
rl_config.set("neuromod.curiosity.enable", True)

brain, world, agent = setup_reinforcement_learning(rl_config)

# Run RL episodes
for episode in range(10):
    reward, steps = run_rl_episode(brain, world, agent)
    print(f"Episode {episode + 1}: Reward={reward:.2f}, Steps={steps}")
```

### Pattern 2: Cognitive Development

```python
import pynlm

class CognitiveDevelopmentSimulator:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 2000)
        self.config.set("neuromod.curiosity.enable", True)
        self.config.set("neuromod.novelty.enable", True)
    
    def simulate_development(self, total_steps=1000):
        """Simulate cognitive development over time."""
        # Create brain
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Create environment
        world = pynlm.createSimpleWorld()
        world.configure(width=40, height=40)
        world.reset()
        
        # Create agent with development tracking
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enable_all()
        
        # Development stages tracking
        development_log = []
        curiosity_log = []
        performance_log = []
        
        # Simulate development phases
        for step in range(total_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Log development metrics
            development_log.append({
                'step': step,
                'developmental_stage': brain.get_developmental_stage(),
                'plasticity_modifier': agent.get_plasticity_modifier()
            })
            
            curiosity_log.append(agent.get_curiosity_level())
            performance_log.append(brain.get_average_firing_rate())
            
            # Simulate developmental transitions
            if step % 100 == 0:
                self._transition_developmental_stage(brain, agent, step)
        
        return self._analyze_development(development_log, curiosity_log, performance_log)
    
    def _transition_developmental_stage(self, brain, agent, step):
        """Transition through developmental stages."""
        current_stage = brain.get_developmental_stage()
        
        if current_stage == pynlm.DevelopmentalStage.Initial and step > 100:
            brain.set_developmental_stage(pynlm.DevelopmentalStage.CriticalPeriod)
            print(f"Stage transition: Initial -> Critical Period")
        
        elif current_stage == pynlm.DevelopmentalStage.CriticalPeriod and step > 300:
            brain.set_developmental_stage(pynlm.DevelopmentalStage.Maturation)
            print(f"Stage transition: Critical Period -> Maturation")
        
        elif current_stage == pynlm.DevelopmentalStage.Maturation and step > 600:
            brain.set_developmental_stage(pynlm.DevelopmentalStage.Adult)
            print(f"Stage transition: Maturation -> Adult")
    
    def _analyze_development(self, development_log, curiosity_log, performance_log):
        """Analyze development patterns."""
        analysis = {
            'final_stage': development_log[-1]['developmental_stage'],
            'peak_curiosity': max(curiosity_log),
            'peak_curiosity_step': curiosity_log.index(max(curiosity_log)),
            'final_performance': performance_log[-1],
            'average_performance': sum(performance_log) / len(performance_log),
            'development_phases': self._identify_development_phases(development_log)
        }
        
        print("Development Analysis:")
        print(f"  Final developmental stage: {analysis['final_stage']}")
        print(f"  Peak curiosity: {analysis['peak_curiosity']:.3f}")
        print(f"  Final performance: {analysis['final_performance']:.2f} Hz")
        print(f"  Average performance: {analysis['average_performance']:.2f} Hz")
        print(f"  Development phases: {len(analysis['development_phases'])}")
        
        return analysis

# Run development simulation
simulator = CognitiveDevelopmentSimulator()
development_results = simulator.simulate_development(1000)
```

### Pattern 3: Experimental Design

```python
import pynlm

class ExperimentRunner:
    def __init__(self, experiment_config):
        self.config = experiment_config
        self.results = []
        self.metrics = []
    
    def run_experiment(self, condition_name, num_trials=10):
        """Run an experiment condition."""
        print(f"Running experiment: {condition_name}")
        
        for trial in range(num_trials):
            trial_results = self._run_single_trial(condition_name, trial)
            self.results.append(trial_results)
            self.metrics.append(self._extract_metrics(trial_results))
            
            print(f"  Trial {trial + 1}/{num_trials}: "
                  f"Success={trial_results['success']}, "
                  f"Reward={trial_results['total_reward']:.3f}")
        
        return self._analyze_experiment()
    
    def _run_single_trial(self, condition_name, trial_id):
        """Run a single trial with specific condition."""
        # Create fresh brain for each trial
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Configure environment based on condition
        world = self._configure_environment(condition_name, trial_id)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        self._configure_agent_for_condition(agent, condition_name)
        
        # Run trial
        trial_results = self._execute_trial(brain, world, agent, condition_name)
        
        return trial_results
    
    def _configure_environment(self, condition_name, trial_id):
        """Configure environment based on experimental condition."""
        world = pynlm.createSimpleWorld()
        
        if condition_name == "baseline":
            world.configure(width=15, height=15)
        elif condition_name == "complex":
            world.configure(width=25, height=25, visionWidth=12, visionHeight=12)
        elif condition_name == "sparse":
            world.configure(width=20, height=20)
        
        # Add objects based on trial
        self._add_objects_to_world(world, condition_name, trial_id)
        
        return world
    
    def _configure_agent_for_condition(self, agent, condition_name):
        """Configure agent based on experimental condition."""
        if condition_name == "baseline":
            agent.enable_all()
        elif condition_name == "complex":
            agent.enable_reward_modulation(True)
            agent.enable_curiosity(True)
        elif condition_name == "sparse":
            agent.enable_structural_plasticity(True)
            agent.enable_development(True)
    
    def _execute_trial(self, brain, world, agent, condition_name):
        """Execute a single trial and collect results."""
        trial_data = {
            'condition': condition_name,
            'steps': [],
            'total_reward': 0.0,
            'success': False,
            'completion_time': 0.0
        }
        
        start_time = time.time()
        
        for step in range(100):  # Fixed 100 steps per trial
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record trial data
            trial_data['steps'].append({
                'step': step,
                'firing_rate': brain.get_average_firing_rate(),
                'reward': self._calculate_reward(world),
                'curiosity': agent.get_curiosity_level()
            })
        
        trial_data['completion_time'] = time.time() - start_time
        trial_data['total_reward'] = sum(s['reward'] for s in trial_data['steps'])
        trial_data['success'] = trial_data['total_reward'] > 0
        
        return trial_data
    
    def _calculate_reward(self, world):
        """Calculate reward for current world state."""
        percept = world.getSensoryPercept()
        internal = percept.get_internal() or [0]
        return internal[0]
    
    def _extract_metrics(self, trial_results):
        """Extract key metrics from trial results."""
        steps = trial_results['steps']
        return {
            'average_firing_rate': sum(s['firing_rate'] for s in steps) / len(steps),
            'total_reward': trial_results['total_reward'],
            'success_rate': 1.0 if trial_results['success'] else 0.0,
            'completion_time': trial_results['completion_time'],
            'final_curiosity': steps[-1]['curiosity'] if steps else 0
        }
    
    def _analyze_experiment(self):
        """Analyze experimental results."""
        if not self.metrics:
            return {}
        
        # Group by condition
        conditions = {}
        for metric in self.metrics:
            condition = metric['condition']
            if condition not in conditions:
                conditions[condition] = []
            conditions[condition].append(metric)
        
        # Analyze each condition
        analysis = {}
        for condition, metrics_list in conditions.items():
            analysis[condition] = {
                'trial_count': len(metrics_list),
                'average_reward': sum(m['total_reward'] for m in metrics_list) / len(metrics_list),
                'success_rate': sum(m['success_rate'] for m in metrics_list) / len(metrics_list),
                'average_firing_rate': sum(m['average_firing_rate'] for m in metrics_list) / len(metrics_list),
                'std_reward': self._std_dev([m['total_reward'] for m in metrics_list]),
                'std_firing': self._std_dev([m['average_firing_rate'] for m in metrics_list])
            }
        
        return analysis
    
    def _std_dev(self, values):
        """Calculate standard deviation."""
        if len(values) <= 1:
            return 0.0
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / (len(values) - 1)
        return variance ** 0.5

# Example experiment configuration
experiment_config = pynlm.createDefaultConfig()
experiment_config.set("brain.neuron_count", 1000)

# Run experiments
experiment_runner = ExperimentRunner(experiment_config)

conditions = ["baseline", "complex", "sparse"]
all_results = {}

for condition in conditions:
    results = experiment_runner.run_experiment(condition, num_trials=5)
    all_results[condition] = results
    print(f"\nCondition '{condition}' Results:")
    for key, value in results.items():
        print(f"  {key}: {value:.3f}")
```

---

## Troubleshooting

### Common Issues and Solutions

**1. "ImportError: No module named 'pynlm'"**

```bash
# Solution: Install the package
pip install pynlm

# Or install from source in development mode
pip install -e .
```

**2. "Segmentation fault during simulation"**

```python
# Solution: Ensure proper initialization
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()  # Always call this first!

for step in range(100):
    brain.step(step)  # Now safe to simulate
```

**3. "Brain not learning or adapting"**

```python
# Solution: Enable learning systems
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enable_all()  # Enables reward modulation, curiosity, etc.
```

**4. "Agent not responding to environment"**

```python
# Solution: Check environment setup and agent initialization
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)  # Critical: agent must be initialized with world

# Verify sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)  # Agent must process input
```

**5. "Memory usage too high"**

```python
# Solution: Configure memory-efficient settings
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Reduce neuron count
config.set("brain.synapse_density", 0.05)  # Reduce connectivity
config.set("memory.working_memory_size", 100)  # Limit working memory
```

### Performance Optimization Tips

1. **Use appropriate simulation step count**: Start with 100-1000 steps for testing
2. **Configure world size**: Use smaller worlds for debugging
3. **Monitor memory usage**: Check `brain.get_total_memory_usage()` if available
4. **Use efficient data structures**: Leverage Python lists over numpy arrays when possible

```python
# Optimized simulation loop
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(500):  # Balanced for debugging
    brain.step(step)
    if step % 50 == 0:
        stats = brain.get_stats()
        print(f"Step {step}: {stats['firing_neurons']} firing neurons")
```

---

## Advanced Features

### 1. Custom Neural Architectures

```python
import pynlm

class CustomBrainBuilder:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
    
    def build_sensory_motor_brain(self, sensory_neurons=200, motor_neurons=100):
        """Build a brain specialized for sensory-motor control."""
        # Configure for sensory-motor processing
        self.config.set("brain.neuron_count", sensory_neurons + motor_neurons)
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("neuromod.dopamine.scale", 1.2)
        self.config.set("neuromod.curiosity.enable", True)
        
        # Create brain
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Add specialized regions
        sensory_region = brain.add_region("sensory_cortex")
        motor_region = brain.add_region("motor_cortex")
        
        return brain
    
    def build_predictive_brain(self, memory_neurons=300, prediction_neurons=150):
        """Build a brain specialized for prediction and memory."""
        self.config.set("brain.neuron_count", memory_neurons + prediction_neurons)
        self.config.set("plasticity.hebbian.enable", True)
        self.config.set("neuromod.novelty.enable", True)
        self.config.set("plasticity.structural.enable", True)
        
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Add memory and prediction regions
        episodic_region = brain.add_region("episodic_memory")
        prediction_region = brain.add_region("prediction_system")
        
        return brain

# Usage
builder = CustomBrainBuilder()
sensory_brain = builder.build_sensory_motor_brain()
prediction_brain = builder.build_predictive_brain()
```

### 2. Multi-Agent Systems

```python
import pynlm

class MultiAgentSystem:
    def __init__(self, num_agents=3):
        self.num_agents = num_agents
        self.agents = []
        self.shared_world = pynlm.createSimpleWorld()
        self.shared_world.configure(width=50, height=50)
        self.shared_world.reset()
    
    def add_agent(self, brain):
        """Add an agent to the system."""
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(self.shared_world)
        agent.enable_all()
        self.agents.append(agent)
        return agent
    
    def run_synchronous_step(self):
        """Run all agents in parallel (synchronous update)."""
        # All agents sense the same world state
        percept = self.shared_world.getSensoryPercept()
        
        # All agents process input
        for agent in self.agents:
            agent.processSensoryInput(percept)
        
        # All brains step simultaneously
        for i, agent in enumerate(self.agents):
            agent.get_brain().step(i)
        
        # All agents decide actions
        actions = []
        for agent in self.agents:
            action = agent.decode_motor_command()
            actions.append(action)
        
        # Apply all actions to world
        for i, action in enumerate(actions):
            self.shared_world.apply_motor_command(action, i * 0.1)
    
    def run_asynchronous_step(self):
        """Run agents sequentially (asynchronous update)."""
        # Get world state
        percept = self.shared_world.getSensoryPercept()
        
        for i, agent in enumerate(self.agents):
            # Each agent processes world state
            agent.processSensoryInput(percept)
            
            # Agent brain steps with its own timing
            agent.get_brain().step(i)
            
            # Agent decides action
            action = agent.decode_motor_command()
            
            # Apply action with staggered timing
            self.shared_world.apply_motor_command(action, i * 0.05)
            
            # Brief pause for this agent's influence on world
            self.shared_world.update(0.01)
            
            # Get updated percept for next agent
            percept = self.shared_world.getSensoryPercept()
    
    def run_agent_independence_simulation(self, steps=100):
        """Run each agent independently in the same world."""
        for step in range(steps):
            self.shared_world.update(0.1)
            
            # Each agent operates independently
            for i, agent in enumerate(self.agents):
                # Each agent gets unique sensory input
                unique_percept = self._create_unique_percept(agent, step)
                
                agent.processSensoryInput(unique_percept)
                agent.get_brain().step(step)
                action = agent.decode_motor_command()
                self.shared_world.apply_motor_command(action, step * 0.1 + i * 0.01)
    
    def _create_unique_percept(self, agent, step):
        """Create unique sensory percept for each agent."""
        base_percept = self.shared_world.getSensoryPercept()
        
        # Add agent-specific noise or variations
        vision = base_percept.get_vision().copy()
        
        # Add step-dependent variation
        for i in range(len(vision)):
            vision[i] += (step % 10) * 0.01 * (hash(str(agent)) % 100) / 100.0
        
        # Create unique percept
        unique_percept = pynlm.SensoryPercept()
        unique_percept.set_vision(vision)
        
        return unique_percept

# Usage example
mas = MultiAgentSystem(num_agents=3)

# Create different brain architectures for each agent
brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
mas.add_agent(brain1)

brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.get_config().set("brain.neuron_count", 1500)  # Larger brain
mas.add_agent(brain2)

brain3 = pynlm.createBrain(pynlm.createDefaultConfig())
brain3.get_config().set("neuromod.curiosity.enable", True)  # More curious
mas.add_agent(brain3)

# Run multi-agent simulation
for step in range(100):
    mas.run_synchronous_step()
    
    if step % 20 == 0:
        print(f"Step {step}: {len(mas.agents)} agents active")
```

### 3. Visualization and Monitoring

```python
import pynlm
import time

class BrainVisualization:
    def __init__(self, brain):
        self.brain = brain
        self.metrics_history = []
        self.spike_history = []
    
    def collect_metrics(self, steps=100, interval=10):
        """Collect metrics for visualization."""
        for step in range(steps):
            self.brain.step(step)
            
            if step % interval == 0:
                metrics = {
                    'step': step,
                    'total_neurons': self.brain.get_total_neuron_count(),
                    'firing_neurons': self.brain.get_firing_neuron_count(),
                    'total_spikes': self.brain.get_total_spike_count(),
                    'average_firing_rate': self.brain.get_average_firing_rate(),
                    'e_i_ratio': self.brain.get_excitation_inhibition_ratio(),
                    'developmental_stage': self.brain.get_developmental_stage()
                }
                self.metrics_history.append(metrics)
                self.spike_history.append(self.brain.get_total_spike_count())
    
    def generate_report(self):
        """Generate comprehensive analysis report."""
        if not self.metrics_history:
            return "No data collected"
        
        report = {
            'simulation_summary': {
                'total_steps': len(self.metrics_history),
                'final_stage': self.metrics_history[-1]['developmental_stage'],
                'peak_firing_rate': max(m['average_firing_rate'] for m in self.metrics_history),
                'average_e_i_ratio': sum(m['e_i_ratio'] for m in self.metrics_history) / len(self.metrics_history)
            },
            'performance_trends': self._analyze_trends(),
            'spike_patterns': self._analyze_spike_patterns(),
            'recommendations': self._generate_recommendations()
        }
        
        return report
    
    def _analyze_trends(self):
        """Analyze performance trends over time."""
        if len(self.metrics_history) < 10:
            return "Insufficient data for trend analysis"
        
        steps = [m['step'] for m in self.metrics_history]
        firing_rates = [m['average_firing_rate'] for m in self.metrics_history]
        
        # Simple trend detection
        early_avg = sum(firing_rates[:len(firing_rates)//3]) / (len(firing_rates)//3)
        late_avg = sum(firing_rates[-len(firing_rates)//3:]) / (len(firing_rates)//3)
        
        if late_avg > early_avg * 1.1:
            trend = "Increasing"
        elif late_avg < early_avg * 0.9:
            trend = "Decreasing"
        else:
            trend = "Stable"
        
        return {
            'trend': trend,
            'early_average': early_avg,
            'late_average': late_avg,
            'change_percentage': ((late_avg - early_avg) / early_avg * 100) if early_avg > 0 else 0
        }
    
    def _analyze_spike_patterns(self):
        """Analyze spike timing and patterns."""
        if len(self.spike_history) < 5:
            return "Insufficient spike data"
        
        spike_rates = [h / 10 for h in self.spike_history]  # Approximate per-interval rates
        
        # Detect bursts (rapid spike increases)
        bursts = []
        for i in range(1, len(spike_rates)):
            if spike_rates[i] > spike_rates[i-1] * 2:
                bursts.append(i)
        
        return {
            'spike_rate_range': [min(spike_rates), max(spike_rates)],
            'average_rate': sum(spike_rates) / len(spike_rates),
            'burst_count': len(bursts),
            'burst_intervals': bursts,
            'coefficient_of_variation': self._coefficient_of_variation(spike_rates)
        }
    
    def _coefficient_of_variation(self, values):
        """Calculate coefficient of variation."""
        if len(values) <= 1 or sum(values) == 0:
            return 0.0
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        std_dev = variance ** 0.5
        return (std_dev / mean) if mean > 0 else 0.0
    
    def _generate_recommendations(self):
        """Generate recommendations based on analysis."""
        recommendations = []
        
        if not self.metrics_history:
            return ["Collect more data for analysis"]
        
        # Check firing rate stability
        firing_rates = [m['average_firing_rate'] for m in self.metrics_history]
        avg_rate = sum(firing_rates) / len(firing_rates)
        
        if avg_rate < 0.1:
            recommendations.append("Increase neural excitability - firing rates too low")
        elif avg_rate > 10:
            recommendations.append("Reduce neural excitability - firing rates too high")
        
        # Check E/I balance
        e_i_ratio = sum(m['e_i_ratio'] for m in self.metrics_history) / len(self.metrics_history)
        if e_i_ratio < 0.5:
            recommendations.append("Increase inhibition for better stability")
        elif e_i_ratio > 2.0:
            recommendations.append("Increase excitation for better responsiveness")
        
        # Check developmental stage
        final_stage = self.metrics_history[-1]['developmental_stage']
        if final_stage == pynlm.DevelopmentalStage.Initial:
            recommendations.append("Allow more development time for maturation")
        
        if not recommendations:
            recommendations.append("Performance looks good! Continue current configuration")
        
        return recommendations

# Usage example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation and collect data
visualizer = BrainVisualization(brain)
visualizer.collect_metrics(steps=200, interval=20)

# Generate comprehensive report
report = visualizer.generate_report()

print("Brain Performance Report:")
print(f"Total Steps: {report['simulation_summary']['total_steps']}")
print(f"Final Developmental Stage: {report['simulation_summary']['final_stage']}")
print(f"Peak Firing Rate: {report['simulation_summary']['peak_firing_rate']:.2f} Hz")
print(f"Average E/I Ratio: {report['simulation_summary']['average_e_i_ratio']:.3f}")
print(f"\nPerformance Trend: {report['performance_trends']['trend']}")
print(f"Spike Rate Range: {report['spike_patterns']['spike_rate_range'][0]:.2f} - {report['spike_patterns']['spike_rate_range'][1]:.2f}")
print("\nRecommendations:")
for rec in report['recommendations']:
    print(f"  - {rec}")
```

---

## Migration Guide

### From Old to New API

This guide helps you migrate from the original NLM Python API to the improved version.

**1. Basic Brain Creation**

```python
# Old way
brain = pynlm.createBrain(config)
brain.initialize()

# New way (same)
brain = pynlm.createBrain(config)
brain.initialize()

# With context manager (new)
with pynlm.createBrain(config) as brain:
    brain.initialize()
    # ... your simulation code
```

**2. Statistics Access**

```python
# Old way
neurons = brain.getTotalNeuronCount()
firing = brain.getFiringNeuronCount()
rate = brain.getAverageFiringRate()

# New way (more convenient)
stats = brain.get_stats()
neurons = stats['total_neurons']
firing = stats['firing_neurons']
rate = stats['average_firing_rate']
```

**3. Action Creation**

```python
# Old way
action = pynlm.Action(ActionType.MoveForward)

# New way (same)
action = pynlm.Action(ActionType.MoveForward)

# New convenience methods
action = pynlm.Action.createMoveForward()
action = pynlm.Action.createLook()
action = pynlm.Action.createInteract()
```

**4. World Configuration**

```python
# Old way
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# New way (same)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# New property access
world.width = 20
world.height = 20
world.vision_width = 8
world.vision_height = 8
```

**5. Agent Configuration**

```python
# Old way
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# New way (more efficient)
agent.enable_all()  # Enables all learning systems
```

**6. Configuration Management**

```python
# Old way
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")
config.set("brain.neuron_count", 1000)

# New way (more Pythonic)
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")

# Type-safe setting with validation
config.set_int("brain.neuron_count", 1000)  # Automatically validates type
```

### What's New

1. **Property Decorators**: Direct attribute access
2. **Context Managers**: Automatic resource cleanup
3. **Convenience Methods**: Simplified object creation
4. **Enhanced Statistics**: Comprehensive metrics in dictionary format
5. **Type Safety**: Runtime type checking and validation
6. **Performance Monitoring**: Built-in performance tracking
7. **Advanced Utilities**: Simulation tools and batch processing
8. **Visualization Support**: Data collection for analysis
9. **Multi-Agent Support**: Complex agent interactions
10. **Custom Architectures**: Specialized brain building tools

### What's Changed

1. **API Naming**: Some method names changed for Pythonic style
2. **Return Types**: More consistent return types (dictionaries, enhanced objects)
3. **Error Handling**: Improved error messages and exception handling
4. **Documentation**: Comprehensive API documentation with examples
5. **Performance**: Optimized for better performance and memory usage

### Compatibility

- **Backward Compatible**: All existing examples continue to work
- **Enhanced Features**: New features are additive, not breaking
- **Same Core Functionality**: Essential neural simulation capabilities unchanged
- **Improved Usability**: New Pythonic interfaces make API easier to use

---

## Conclusion

The improved NLM Python bindings provide a powerful, user-friendly interface to the neural simulation framework. Whether you're a beginner looking for simple examples or an advanced researcher needing complex simulation capabilities, the enhanced API provides the tools you need.

Key improvements include:

- **Pythonic Interfaces**: Natural Python API with property decorators and context managers
- **Enhanced Usability**: Convenience methods and comprehensive documentation
- **Advanced Features**: Performance monitoring, visualization tools, and simulation utilities
- **Type Safety**: Runtime type checking and validation
- **Extensibility**: Easy to extend for custom use cases

The NLM framework continues to provide state-of-the-art neural simulation capabilities while being more accessible and easier to use for Python developers.

For the latest documentation and examples, visit the official NLM documentation site or explore the examples in this repository.
