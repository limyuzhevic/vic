# NLM User Guide: A Comprehensive Learning Path

**Progressive Guide for Users of All Skill Levels**

## Welcome to NLM

This guide is designed to help you master NLM at your own pace. Whether you're just starting out or looking to deepen your expertise, we've structured the content to build your skills progressively.

## Learning Path Overview

1. **Beginner Stage** - Get started quickly with simple examples
2. **Intermediate Stage** - Understand core concepts and workflows
3. **Advanced Stage** - Master complex simulations and optimizations

Let's begin your journey!

## Stage 1: Beginner - Getting Started

### Why NLM?

NLM is a brain simulator that creates virtual brains capable of learning, memory, and decision-making. Think of it as teaching a computer basic neuroscience.

### Quick Start Project: "Hello Brain"

This project teaches the fundamentals of brain simulation.

#### The Three Components You Need

1. **Brain** - The virtual thinking engine
2. **World** - The environment the brain lives in  
3. **Agent** - The bridge connecting brain to world

#### Step 1: Install NLM

```bash
# Build from source
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
make install
```

#### Step 2: Your First Brain

```python
import pynlm

print("=== Stage 1: Hello Brain ===")

# 1. Make a brain (like giving it a body)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# 2. Check it worked
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# 3. Make it think
brain.step(0)

# 4. See what happened
print(f"After first step: {brain.getFiringNeuronCount()} neurons firing")
print("✓ Your brain is alive and thinking!\n")
```

#### Stage 1 Project: Silent Brain

```python
import pynlm

print("=== Stage 1 Project: Silent Brain ===")

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Let it think for a bit
for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
```

#### Stage 1 Project: Brain Watching a World

```python
import pynlm

print("=== Stage 1 Project: Brain Watching a World ===")

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
print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
```

## Stage 2: Intermediate - Core Concepts

### Understanding the Brain-World-Agent Loop

The NLM system operates through a continuous loop:

```
WORLD (Environment)
  ↓
SENSORY PERCEPT (What the agent sees)
  ↓
AGENT (Processing)
  ↓
BRAIN (Thinking)
  ↓
MOTOR COMMAND (What to do)
  ↓
WORLD (Environment updates)
  ↓
REWARD/ERROR (Learning signal)
  ↓
BRAIN (Plasticity)
```

### The Three Key Components Explained

#### 1. Brain

**What it does:** Computes, learns, and makes decisions

**Key actions:**
- Receives sensory input
- Processes information through neurons
- Generates motor commands
- Learns from feedback

**When to use:** When you need intelligent computation

#### 2. World

**What it does:** Provides the environment and sensory input

**Key actions:**
- Updates its state over time
- Provides visual, audio, and internal signals
- Applies motor commands
- Generates reward signals

**When to use:** When you need environmental interaction

#### 3. Agent

**What it does:** Bridges the brain and world

**Key actions:**
- Transduces sensory input to brain
- Decodes motor commands from brain
- Applies neuromodulation (reward signals)
- Manages developmental stages

**When to use:** When you need the brain-world connection

### Intermediate Project: Complete Agent Simulation

```python
import pynlm

print("=== Stage 2 Project: Complete Agent Simulation ===")

def run_agent_simulation(num_steps=100):
    """Complete agent simulation demonstrating all components"""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    
    # 2. Create brain and initialize  
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)  # Start in center
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Step brain
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress occasionally
        if step % 20 == 0:
            print(f"Step {step}: Firing={brain.getFiringNeuronCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}, "
                  f"Novelty={agent.getNoveltyLevel():.2f}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation(100)

print("✓ Agent simulation complete!")
print(f"Final firing rate: {brain.getAverageFiringRate():.3f}")
```

### Understanding Configuration

The configuration system uses a hierarchical structure. Common keys include:

#### Brain Configuration
- `brain.neuron_count` - Number of neurons (default: 1000)
- `brain.synapse_density` - Connection probability (default: 0.1)
- `brain.connection_probability` - Specific connection chance (default: 0.05)

#### Plasticity Configuration
- `plasticity.enabled` - Enable/disable learning (default: True)
- `plasticity.stdp.enable` - Spike-timing dependent plasticity (default: True)
- `plasticity.hebbian.enable` - Hebbian learning (default: True)

#### Neuromodulation Configuration
- `neuromod.dopamine.scale` - Reward signal strength (default: 1.0)
- `neuromod.curiosity.enable` - Enable curiosity-driven exploration (default: True)

#### Development Configuration
- `development.critical_period` - Sensitive learning period (default: 0.5)
- `development.maturation_rate` - Development speed (default: 0.8)

### Common Intermediate Patterns

#### Pattern 1: Running a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

# Usage:
run_simulation(brain, world, agent, 1000)
```

#### Pattern 2: Monitoring Brain Statistics

```python
# Add these to your simulation loop:
print("Brain Stats:")
print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"  Development stage: {brain.getDevelopmentalStage()}")
```

#### Pattern 3: Enabling Features

```python
# Enable optional learning features:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections  
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

## Stage 3: Advanced - Complex Systems

### Advanced Project: Learning Environment Navigation

This project creates an agent that learns to navigate a complex environment.

```python
import pynlm
import numpy as np

print("=== Stage 3 Project: Learning Environment Navigation ===")

def create_navigating_agent(neuron_count=500, world_size=30):
    """Create an agent that learns to navigate"""
    
    # Create complex configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.08)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)  # Stronger rewards
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("development.critical_period", 0.3)  # Longer learning phase
    config.set("development.maturation_rate", 0.6)  # Slower maturation
    
    # Create brain with advanced configuration
    brain = pynlm.Brain(config)
    brain.initialize()
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
    
    # Create sophisticated world
    world = pynlm.createSimpleWorld()
    world.configure(width=world_size, height=world_size, visionWidth=7, visionHeight=7)
    world.setMaxEnergy(50.0)  # Limited resources
    world.reset()
    world.setAgentStart(world_size//2, world_size//2)
    
    # Add resources to world
    np.random.seed(42)
    for _ in range(20):
        x = np.random.randint(0, world_size)
        y = np.random.randint(0, world_size)
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
    
    # Create agent with advanced capabilities
    agent = pynlm.AgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)  # Can grow new connections
    agent.enableDevelopment(True)           # Developmental changes
    agent.enableCuriosity(True)             # Drive to explore
    agent.enableNovelty(True)               # Respond to new patterns
    
    return brain, agent, world

def train_navigator(brain, agent, world, training_steps=500):
    """Train agent to navigate and learn"""
    
    # Track learning progress
    curiosity_history = []
    firing_history = []
    reward_history = []
    
    for step in range(training_steps):
        # Update world
        world.update(0.05)
        
        # Get current state
        percept = world.getSensoryPercept()
        current_reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        
        # Store statistics
        curiosity_history.append(agent.getCuriosityLevel())
        firing_history.append(brain.getFiringNeuronCount())
        reward_history.append(current_reward)
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Step brain
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print training progress
        if step % 50 == 0:
            avg_curiosity = np.mean(curiosity_history[-50:])
            avg_firing = np.mean(firing_history[-50:])
            avg_reward = np.mean(reward_history[-50:])
            
            print(f"Step {step}:")
            print(f"  Development: {brain.getDevelopmentalStage().name}")
            print(f"  Learning: curiosity={avg_curiosity:.3f}, firing={avg_firing:.1f}")
            print(f"  Performance: reward={avg_reward:.3f}")
    
    return curiosity_history, firing_history, reward_history

# Create and train the agent
brain, agent, world = create_navigating_agent()
print("Created advanced navigating agent")

curiosity, firing, rewards = train_navigator(brain, agent, world, 300)

print("\n✓ Training complete!")
print(f"Final development stage: {brain.getDevelopmentalStage().name}")
print(f"Final curiosity level: {curiosity[-1]:.3f}")
print(f"Final firing rate: {brain.getAverageFiringRate():.3f}")
```

### Advanced Configuration Management

```python
def apply_advanced_configuration(config, params):
    """Apply advanced configuration with validation"""
    
    # Validate neuron count
    if params.get("neuron_count", 0) <= 0:
        raise ValueError("Neuron count must be positive")
    
    # Validate synapse density
    density = params.get("synapse_density", 0)
    if density < 0 or density > 1:
        raise ValueError("Synapse density must be between 0 and 1")
    
    # Apply configuration
    config.set("brain.neuron_count", params["neuron_count"])
    config.set("brain.synapse_density", params["synapse_density"])
    config.set("brain.connection_probability", params.get("connection_probability", 0.05))
    
    # Advanced plasticity settings
    if "plasticity" in params:
        for key, value in params["plasticity"].items():
            config.set(f"plasticity.{key}", value)
    
    # Advanced neuromodulation settings
    if "neuromod" in params:
        for key, value in params["neuromod"].items():
            config.set(f"neuromod.{key}", value)
    
    return config

# Example usage
advanced_params = {
    "neuron_count": 1000,
    "synapse_density": 0.15,
    "connection_probability": 0.08,
    "plasticity": {
        "stdp": {"enable": True, "learning_rate": 0.002, "tau_plus": 25.0, "tau_minus": 25.0},
        "hebbian": {"enable": True, "learning_rate": 0.001},
        "structural": {"enable": True, "rate": 0.001}
    },
    "neuromod": {
        "dopamine": {"scale": 1.2, "enable": True},
        "curiosity": {"enable": True, "scale": 1.0},
        "novelty": {"enable": True, "threshold": 0.1}
    }
}

config = pynlm.createDefaultConfig()
advanced_config = apply_advanced_configuration(config, advanced_params)
```

### Performance Optimization

#### Large-Scale Configuration

```python
def optimize_for_performance(config, target_neurons=100000):
    """Optimize configuration for large-scale simulation"""
    
    # Calculate optimal parameters
    neuron_count = target_neurons
    synapse_density = min(0.05, 0.001 * np.sqrt(neuron_count / 1000))
    connection_probability = synapse_density * 0.8
    
    # Apply performance-friendly settings
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", synapse_density)
    config.set("brain.connection_probability", connection_probability)
    
    # Disable expensive features for performance
    config.set("plasticity.structural.enable", False)  # Expensive
    config.set("neuromod.novelty.enable", False)       # Expensive
    config.set("memory.episodic_memory_size", 100)    # Limit memory
    
    # Enable performance optimizations
    config.set("brain.use_vectorization", True)
    config.set("brain.use_multithreading", True)
    
    return config

# Usage
large_config = pynlm.createDefaultConfig()
performance_config = optimize_for_performance(large_config, 10000)
```

### Debugging Advanced Systems

#### Monitoring and Logging

```python
import logging
import time
from typing import Dict, List

class AdvancedMonitor:
    def __init__(self):
        self.logging_enabled = True
        self.stats_buffer = {}
        self.start_time = time.time()
    
    def log_step(self, step: int, brain: pynlm.Brain, 
                 agent: pynlm.AgentBrain, world: pynlm.SimpleWorld):
        """Log detailed statistics for debugging"""
        
        step_stats = {
            "step": step,
            "time_elapsed": time.time() - self.start_time,
            "neurons_firing": brain.getFiringNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "development_stage": brain.getDevelopmentalStage().name,
            "curiosity_level": agent.getCuriosityLevel(),
            "novelty_level": agent.getNoveltyLevel(),
            "neuromodulation_level": agent.getNeuromodulationLevel(),
            "prediction_error": agent.getPredictionError(),
            "world_time": world.getSimulationTime(),
            "energy": world.getAgentBody().getEnergy() if world.getAgentBody() else 0.0
        }
        
        self.stats_buffer[step] = step_stats
        
        if self.logging_enabled:
            print(f"Step {step}: "
                  f"Firing={step_stats['neurons_firing']}, "
                  f"Curiosity={step_stats['curiosity_level']:.3f}, "
                  f"Dev={step_stats['development_stage']}")
    
    def analyze_learning(self) -> Dict:
        """Analyze learning patterns from logged data"""
        
        if not self.stats_buffer:
            return {}
        
        steps = list(self.stats_buffer.keys())
        curiosity_levels = [self.stats_buffer[s]['curiosity_level'] for s in steps]
        firing_rates = [self.stats_buffer[s]['average_firing_rate'] for s in steps]
        development_stages = [self.stats_buffer[s]['development_stage'] for s in steps]
        
        # Find when learning stabilizes
        curiosity_trend = np.diff(curiosity_levels)
        learning_stabilization_step = np.argmax(np.abs(curiosity_trend) < 0.01)
        
        return {
            "total_steps": len(steps),
            "final_curiosity": curiosity_levels[-1],
            "final_firing_rate": firing_rates[-1],
            "development_stage": development_stages[-1],
            "learning_stabilization": learning_stabilization_step,
            "average_curiosity": np.mean(curiosity_levels),
            "curiosity_variance": np.var(curiosity_levels)
        }

# Usage in your simulation
monitor = AdvancedMonitor()

# Inside your simulation loop:
# monitor.log_step(step, brain, agent, world)

# After simulation:
# analysis = monitor.analyze_learning()
# print(f"Learning analysis: {analysis}")
```

## Quick Reference Guide

### Essential Commands

| Action | Command |
|--------|---------|
| Create brain | `brain = pynlm.createBrain(config)` |
| Initialize brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `world = pynlm.createSimpleWorld()` |
| Create agent | `agent = pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

### Common Configuration Keys

#### Brain
- `brain.neuron_count` - Number of neurons
- `brain.synapse_density` - Connection probability

#### Learning
- `plasticity.enabled` - Enable/disable learning
- `plasticity.stdp.enable` - Spike-timing plasticity

#### Motivation
- `neuromod.dopamine.scale` - Reward strength
- `neuromod.curiosity.enable` - Exploration drive

### Error Messages and Solutions

#### "Brain not doing anything"
**Problem:** Brain has zero firing neurons

**Solutions:**
1. Ensure `brain.initialize()` is called
2. Provide input current: `brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)`
3. Check configuration parameters
4. Run more simulation steps for brain to "warm up"

#### "Agent isn't moving"
**Problem:** Agent generates zero motor commands

**Solutions:**
1. Ensure `world.update()` is being called
2. Check sensory input is available
3. Verify brain is receiving percepts
4. Debug motor decoding with print statements

#### "Everything is 0"
**Problem:** All values showing zero

**Solutions:**
1. Brains need time to "warm up" - try more steps
2. Some neurons need input to fire - connect sensory input
3. Check random seed issues
4. Verify parameter types match expectations

## Next Steps

### For Continued Learning:

1. **Read `API_REFERENCE.md`** - Complete API documentation
2. **Try examples in `python/examples/`** - See working code
3. **Read `docs/ARCHITECTURE.md`** - Understand system design
4. **Read `docs/SCIENCE.md`** - Learn scientific background
5. **Experiment with different configurations** - Discover parameter effects

### Advanced Topics:

1. **Performance optimization** - Scale to millions of neurons
2. **Memory management** - Handle large simulations
3. **Distributed computing** - Multi-machine scaling
4. **Custom plasticity rules** - Design new learning algorithms
5. **Integration with other systems** - Connect to external environments

### Community and Support:

1. **Check `docs/EXPERIMENTS.md`** - See how others use NLM
2. **Read `docs/LIMITATIONS.md`** - Understand constraints
3. **Try `./nlm_phase3_demo`** - See complete integration example
4. **Build with `-DNLM_PERFORMANCE_ENABLED=ON`** - Get performance benchmarks
5. **Report issues and suggestions** - Help improve the project

This comprehensive user guide provides everything you need to master NLM at any skill level. Start with the beginners section, progress through intermediate concepts, and tackle advanced challenges as you grow confident with the system.