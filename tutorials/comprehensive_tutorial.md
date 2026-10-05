# NLM (熙然) - Neural Learning Machine
# Comprehensive Tutorial Examples and Workflows

This document provides comprehensive tutorial examples and complete workflows for using NLM at different skill levels, from complete beginners to advanced users.

## Table of Contents

1. [Quick Start for Beginners](#quick-start-for-beginners)
2. [Basic Brain Simulation](#basic-brain-simulation)
3. [Brain in a World](#brain-in-a-world)
4. [Agent-Based Learning](#agent-based-learning)
5. [Advanced Workflows](#advanced-workflows)
6. [Performance Optimization](#performance-optimization)
7. [Advanced Features](#advanced-features)
8. [Troubleshooting](#troubleshooting)

---

# Quick Start for Beginners

## 1.1 The Three Fundamental Components

NLM is built around three core concepts:

1. **Brain** - The virtual brain that thinks and learns
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

Think of it like this:
```
WORLD → SENSORY INPUT → BRAIN → ACTION → WORLD
```

## 1.2 First Example: Silent Brain

```python
import pynlm

# Step 1: Create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 10 steps
for i in range(10):
    brain.step(i)

# Step 4: Check results
print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

## 1.3 Understanding the Output

- **Total Spike Count**: Number of neurons that fired
- **Average Firing Rate**: How often neurons fire per step
- **Neurons/Firing**: Active neurons during simulation

## 1.4 Common Mistakes to Avoid

1. **Forgetting to initialize()**: Always call `brain.initialize()` before `brain.step()`
2. **Using uninitialized variables**: All components need proper setup
3. **Ignoring error handling**: Check return values where appropriate

---

# Basic Brain Simulation

## 2.1 Creating and Configuring a Brain

### Example 2.1: Custom Brain Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Customize settings
config.set("brain.neuron_count", 500)  # Fewer neurons for faster testing
config.set("brain.v_thresh", -45.0)    # Higher threshold
config.set("plasticity.stdp.enable", True)  # Enable learning

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Example 2.2: Brain Development Simulation

```python
import pynlm
import time

def simulateDevelopment():
    # Setup
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    config.set("development.enable", True)
    config.set("development.critical_period", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Simulate development over time
    for step in range(300):
        brain.step(step)
        
        # Check development stage every 50 steps
        if step % 50 == 0:
            stage = brain.getDevelopmentalStage()
            print(f"Step {step}: Developmental stage {stage}")
            
            # Show learning progress
            print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    return brain

brain = simulateDevelopment()
```

## 2.2 Monitoring Brain Activity

### Example 2.3: Real-time Brain Monitoring

```python
import pynlm

def monitorBrain(brain, duration=1000):
    """Monitor brain activity over time"""
    print("Starting brain monitoring...")
    print("=" * 50)
    
    metrics = {
        'spike_counts': [],
        'firing_rates': [],
        'stages': [],
        'time_points': []
    }
    
    for step in range(duration):
        brain.step(step)
        
        # Record metrics
        metrics['spike_counts'].append(brain.getTotalSpikeCount())
        metrics['firing_rates'].append(brain.getAverageFiringRate())
        metrics['stages'].append(brain.getDevelopmentalStage())
        metrics['time_points'].append(step)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print()
    
    return metrics

# Create brain and monitor
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
metrics = monitorBrain(brain, 500)
```

### Example 2.4: Brain State Management

```python
import pynlm
import os

def brainStateManagement():
    """Demonstrate brain checkpointing and state management"""
    
    # Create and initialize brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run some simulation
    for i in range(100):
        brain.step(i)
    
    print("Running simulation...")
    print(f"Brain state: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSpikeCount()} spikes")
    
    # Save checkpoint
    checkpoint_file = "test_checkpoint.bin"
    brain.save(checkpoint_file)
    print(f"Saved checkpoint to {checkpoint_file}")
    
    # Simulate some more learning
    for i in range(100, 200):
        brain.step(i)
    
    print(f"Continued simulation: {brain.getTotalSpikeCount()} total spikes")
    
    # Reset and restore from checkpoint
    brain.reset()
    brain.load(checkpoint_file)
    
    print("Restored from checkpoint")
    print(f"Brain state after restore: {brain.getTotalSpikeCount()} spikes")
    
    # Clean up
    if os.path.exists(checkpoint_file):
        os.remove(checkpoint_file)
    
    return brain

brain = brainStateManagement()
```

---

# Brain in a World

## 3.1 World Simulation

### Example 3.1: Simple World Setup

```python
import pynlm

def createSimpleWorld():
    """Create and configure a simple world"""
    # Create world
    world = pynlm.createSimpleWorld()
    
    # Configure dimensions
    world.configure(
        width=30,      # World width
        height=30,     # World height  
        visionWidth=8, # Agent vision width
        visionHeight=8 # Agent vision height
    )
    
    # Reset world to initial state
    world.reset()
    
    # Set agent starting position
    world.setAgentStart(15.0, 15.0)
    
    return world

world = createSimpleWorld()
print(f"Created world: {world.getWidth()}x{world.getHeight()}")
print(f"Agent start: {world.getAgentBody().x}, {world.getAgentBody().y}")
```

### Example 3.2: World Dynamics

```python
import pynlm

def simulateWorld():
    """Demonstrate world simulation with agent movement"""
    
    # Create components
    brain = pynlm.createQuickBrain(100, enablePlasticity=True)
    world = pynlm.createQuickWorld(20, 20, 6, 6)
    agent = pynlm.createQuickAgent(brain, world)
    
    # Enable agent capabilities
    agent.enableCuriosity(True)      # Explore new areas
    agent.enableRewardModulation(True) # Learn from rewards
    agent.enableDevelopment(True)     # Mature over time
    
    print("Starting world simulation...")
    
    for step in range(200):
        # Update world state
        world.update(0.1)  # 0.1 time units per step
        
        # Get what agent sees
        percept = world.getSensoryPercept()
        
        # Tell brain what it sees
        agent.processSensoryInput(percept)
        
        # Brain thinks and decides
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Show progress
        if step % 20 == 0:
            body = world.getAgentBody()
            percept = world.getSensoryPercept()
            
            print(f"Step {step}:")
            print(f"  Position: ({body.x:.1f}, {body.y:.1f})")
            print(f"  Direction: {body.orientation:.1f}°")
            print(f"  Velocity: ({body.velocityX:.2f}, {body.velocityY:.2f})")
            print(f"  Energy: {body.energy:.1f}/{body.health}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print()
    
    return brain, world, agent

brain, world, agent = simulateWorld()
```

### Example 3.3: World Objects and Interactions

```python
import pynlm

def worldObjectsDemo():
    """Demonstrate world objects and agent interactions"""
    
    world = pynlm.createQuickWorld(15, 15, 5, 5)
    world.reset()
    
    # Set up agent
    agent_brain = pynlm.createQuickBrain(50)
    agent = pynlm.createQuickAgent(agent_brain, world)
    
    # Add objects to the world
    # Resource (food)
    world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0))
    
    # Wall
    world.addObject(pynlm.WorldObject(10.0, 5.0, pynlm.WorldObjectType.Wall, radius=0.8))
    
    # Hazard
    world.addObject(pynlm.WorldObject(8.0, 8.0, pynlm.WorldObjectType.Hazard, value=5.0))
    
    print("World setup complete:")
    print(f"  World size: {world.getWidth()}x{world.getHeight()}")
    print(f"  Agent start: ({agent.getBrain().getTotalNeuronCount()} neurons)")
    print(f"  Objects: Resource at (5,5), Wall at (10,5), Hazard at (8,8)")
    
    # Run simulation
    for step in range(50):
        world.update(0.1)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        agent_brain.step(step)
        
        # Make decision
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check position
        body = world.getAgentBody()
        print(f"Step {step:2d}: Pos({body.x:4.1f}, {body.y:4.1f}) Action:{action}")
    
    return brain, world, agent

brain, world, agent = worldObjectsDemo()
```

---

# Agent-Based Learning

## 4.1 Reinforcement Learning Framework

### Example 4.1: Basic Reinforcement Learning

```python
import pynlm

def basicRL():
    """Basic reinforcement learning example"""
    
    # Setup NLM components
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createQuickWorld(10, 10, 4, 4)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Running Basic Reinforcement Learning...")
    print("=" * 50)
    
    episode_reward = 0.0
    episode_steps = 50
    
    for step in range(episode_steps):
        # Environment update
        world.update(0.1)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Action selection
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        
        # Learn from experience
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        episode_reward += reward
        
        if step % 10 == 0:
            print(f"  Step {step}: reward={reward:.3f}, "
                  f"curiosity={agent.getCuriosityLevel():.3f}, "
                  f"stage={brain.getDevelopmentalStage()}")
    
    print(f"Episode complete:")
    print(f"  Total reward: {episode_reward:.3f}")
    print(f"  Average reward per step: {episode_reward/episode_steps:.3f}")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Final development stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

brain, world, agent = basicRL()
```

### Example 4.2: Multi-Episode Learning

```python
import pynlm
import time

def multiEpisodeLearning(num_episodes=20):
    """Run multiple learning episodes and track performance"""
    
    # Setup
    brain = pynlm.createQuickBrain(200, enablePlasticity=True)
    world = pynlm.createQuickWorld(12, 12, 6, 6)
    agent = pynlm.createQuickAgent(brain, world)
    
    # Track performance
    episode_rewards = []
    episode_lengths = []
    curiosity_levels = []
    
    print(f"Running {num_episodes} learning episodes...")
    print("=" * 60)
    
    for episode in range(num_episodes):
        print(f"\nEpisode {episode + 1}/{num_episodes}")
        
        # Reset for new episode
        brain.reset()
        brain.initialize()
        world.reset()
        agent.reset()
        
        episode_reward = 0.0
        
        # Run episode
        for step in range(100):
            # Update world
            world.update(0.1)
            
            # Get percept and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processing
            brain.step(step)
            
            # Action selection
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Reward
            reward = 0.0
            if percept.getInternal():
                reward = percept.getInternal()[0]
            
            # Learn
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            episode_reward += reward
            
            # Show progress
            if step % 20 == 0:
                print(f"    Step {step:3d}: reward={reward:6.3f}, "
                      f"curiosity={agent.getCuriosityLevel():5.3f}")
        
        # Record episode results
        episode_rewards.append(episode_reward)
        episode_lengths.append(100)
        curiosity_levels.append(agent.getCuriosityLevel())
        
        print(f"  Episode complete:")
        print(f"    Total reward: {episode_reward:8.3f}")
        print(f"    Avg reward/step: {episode_reward/100:8.4f}")
        print(f"    Final curiosity: {agent.getCuriosityLevel():5.3f}")
        print(f"    Development stage: {brain.getDevelopmentalStage()}")
    
    # Calculate summary statistics
    avg_reward = sum(episode_rewards) / len(episode_rewards)
    avg_curiosity = sum(curiosity_levels) / len(curiosity_levels)
    
    print("\n" + "=" * 60)
    print("LEARNING SUMMARY")
    print("=" * 60)
    print(f"Average reward per episode: {avg_reward:.3f}")
    print(f"Average curiosity level: {avg_curiosity:.3f}")
    print(f"Best episode reward: {max(episode_rewards):.3f}")
    print(f"Average episode length: {sum(episode_lengths)/len(episode_lengths):.1f} steps")
    
    return {
        'rewards': episode_rewards,
        'lengths': episode_lengths,
        'curiosity': curiosity_levels,
        'avg_reward': avg_reward,
        'avg_curiosity': avg_curiosity
    }

results = multiEpisodeLearning(15)
```

### Example 4.3: Advanced Agent Capabilities

```python
import pynlm

def advancedAgentDemo():
    """Demonstrate advanced agent capabilities"""
    
    # Setup with full capabilities
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createQuickWorld(25, 25, 10, 10)
    world.reset()
    world.setAgentStart(12.5, 12.5)
    world.setMaxEnergy(200.0)  # Higher energy capacity
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable ALL subsystems
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Mature over time
    agent.enableCuriosity(True)             # Explore new things
    
    print("Advanced Agent Demo")
    print("=" * 50)
    print("Enabled subsystems:")
    print("  ✓ Reward modulation (learning from experience)")
    print("  ✓ Structural plasticity (growing new synapses)")
    print("  ✓ Development (aging and maturation)")
    print("  ✓ Curiosity (exploration and discovery)")
    print()
    
    # Run extended simulation
    for episode in range(5):
        print(f"Episode {episode + 1}:")
        
        # Run episode
        for step in range(80):
            world.update(0.1)
            
            # Get all sensory inputs
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Complex decision making
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Complex learning
            if step % 20 == 0:
                # Get reward and predicted reward
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                predicted = reward * 0.5  # Simplified prediction
                
                agent.applyRewardModulation(reward, predicted)
                agent.updateDevelopment(0.1)
        
        # Show episode results
        body = world.getAgentBody()
        print(f"  Final position: ({body.x:.1f}, {body.y:.1f})")
        print(f"  Final energy: {body.energy:.1f}/{body.health}")
        print(f"  Final age: {body.age:.1f}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  Prediction error: {agent.getPredictionError():.3f}")
        print()
    
    return brain, world, agent

brain, world, agent = advancedAgentDemo()
```

---

# Advanced Workflows

## 5.1 Experiment Framework

### Example 5.1: Running Experiments

```python
import pynlm

def runExperiment(experiment_name, config, duration=500):
    """Run a structured experiment with NLM"""
    
    print(f"Starting experiment: {experiment_name}")
    print("=" * 50)
    
    # Create brain with experiment config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create environment
    world = pynlm.createQuickWorld(20, 20, 8, 8)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Configure based on experiment
    if "reward" in experiment_name.lower():
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
    elif "development" in experiment_name.lower():
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    else:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
    
    # Run experiment
    start_time = time.time()
    
    for step in range(duration):
        # Update environment
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Action selection
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning updates
        if step % 50 == 0:
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
    
    elapsed_time = time.time() - start_time
    
    # Experiment results
    results = {
        'experiment': experiment_name,
        'duration': duration,
        'time_elapsed': elapsed_time,
        'final_reward': agent.getNeuromodulationLevel(),
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel(),
        'development_stage': brain.getDevelopmentalStage(),
        'total_neurons': brain.getTotalNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate()
    }
    
    print(f"Experiment '{experiment_name}' completed:")
    for key, value in results.items():
        if isinstance(value, float):
            print(f"  {key}: {value:.3f}")
        else:
            print(f"  {key}: {value}")
    
    return results

# Experiment configurations
experiments = {
    "reward_learning": {
        "brain.neuron_count": 200,
        "neuromod.dopamine.scale": 1.5,
        "neuromod.curiosity.enable": True
    },
    "development_study": {
        "brain.neuron_count": 150,
        "development.enable": True,
        "development.critical_period": 200
    },
    "curiosity_experiment": {
        "brain.neuron_count": 180,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.enable": True
    }
}

# Run experiments
for exp_name, exp_config in experiments.items():
    # Create config
    config = pynlm.createDefaultConfig()
    for key, value in exp_config.items():
        config.set(key, value)
    
    results = runExperiment(exp_name, config, 300)
    print()
```

### Example 5.2: Parameter Sweeps

```python
import pynlm

def parameterSweep(param_name, param_values, base_config):
    """Run parameter sweep experiment"""
    
    results = {}
    
    print(f"Parameter sweep for {param_name}:")
    print(f"Values: {param_values}")
    print("=" * 60)
    
    for value in param_values:
        # Create modified config
        config = pynlm.createDefaultConfig()
        
        # Copy base config values
        for key, val in base_config.items():
            config.set(key, val)
        
        # Set sweep parameter
        config.set(param_name, value)
        
        # Run experiment
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createQuickWorld(10, 10, 4, 4)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        total_reward = 0.0
        for step in range(100):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            total_reward += reward
        
        results[value] = total_reward
        print(f"  {param_name}={value:6.3f}: reward={total_reward:8.3f}")
    
    # Find optimal value
    optimal_value = max(results, key=results.get)
    optimal_reward = results[optimal_value]
    
    print(f"\nOptimal {param_name}: {optimal_value:.3f} (reward={optimal_reward:.3f})")
    
    return results

# Parameter sweep example
base_config = {
    "brain.neuron_count": 100,
    "neuromod.dopamine.scale": 1.0,
    "neuromod.curiosity.enable": True
}

learning_rates = [0.001, 0.005, 0.01, 0.05, 0.1]
sweep_results = parameterSweep("neuromod.dopamine.scale", learning_rates, base_config)
```

---

# Performance Optimization

## 6.1 Memory and Performance Tips

### Example 6.1: Efficient Brain Configuration

```python
import pynlm
import time

def optimizeConfiguration():
    """Demonstrate configuration optimization techniques"""
    
    print("Configuration Optimization Examples")
    print("=" * 50)
    
    # Method 1: Minimal configuration (fast startup)
    print("\n1. Minimal Configuration (Fast Startup):")
    start_time = time.time()
    
    minimal_config = pynlm.createDefaultConfig()
    minimal_config.set("brain.neuron_count", 50)  # Small brain
    minimal_config.set("plasticity.stdp.enable", False)  # No learning
    minimal_config.set("neuromod.curiosity.enable", False)  # No curiosity
    
    brain1 = pynlm.createBrain(minimal_config)
    brain1.initialize()
    
    init_time1 = time.time() - start_time
    print(f"  Initialization time: {init_time1:.3f}s")
    print(f"  Neurons: {brain1.getTotalNeuronCount()}")
    
    # Method 2: Learning-focused configuration
    print("\n2. Learning-Focused Configuration:")
    start_time = time.time()
    
    learning_config = pynlm.createDefaultConfig()
    learning_config.set("brain.neuron_count", 200)
    learning_config.set("plasticity.stdp.enable", True)
    learning_config.set("plasticity.stdp.learning_rate", 0.01)
    learning_config.set("plasticity.hebbian.enable", True)
    learning_config.set("neuromod.curiosity.enable", True)
    learning_config.set("neuromod.novelty.enable", True)
    
    brain2 = pynlm.createBrain(learning_config)
    brain2.initialize()
    
    init_time2 = time.time() - start_time
    print(f"  Initialization time: {init_time2:.3f}s")
    print(f"  Neurons: {brain2.getTotalNeuronCount()}")
    print(f"  Plasticity enabled: {learning_config.has('plasticity.stdp.enable')}")
    
    # Method 3: Development simulation
    print("\n3. Development Simulation:")
    start_time = time.time()
    
    dev_config = pynlm.createDefaultConfig()
    dev_config.set("brain.neuron_count", 150)
    dev_config.set("development.enable", True)
    dev_config.set("development.critical_period", 100)
    dev_config.set("neuromod.curiosity.enable", True)
    
    brain3 = pynlm.createBrain(dev_config)
    brain3.initialize()
    
    init_time3 = time.time() - start_time
    print(f"  Initialization time: {init_time3:.3f}s")
    print(f"  Neurons: {brain3.getTotalNeuronCount()}")
    print(f"  Development enabled: {dev_config.has('development.enable')}")
    
    return brain1, brain2, brain3

brain1, brain2, brain3 = optimizeConfiguration()
```

### Example 6.2: Batch Processing

```python
import pynlm
import time

def batchProcessingExample():
    """Demonstrate efficient batch processing"""
    
    print("Batch Processing Example")
    print("=" * 50)
    
    # Create multiple brains with different configurations
    brains = []
    configs = []
    
    # Different configurations
    config_templates = [
        {"neurons": 50, "learning": False, "curiosity": False},
        {"neurons": 100, "learning": True, "curiosity": False},
        {"neurons": 200, "learning": True, "curiosity": True},
        {"neurons": 300, "learning": True, "curiosity": True, "development": True}
    ]
    
    print("Creating brain configurations...")
    start_time = time.time()
    
    for i, template in enumerate(config_templates):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", template["neurons"])
        config.set("plasticity.stdp.enable", template.get("learning", False))
        config.set("neuromod.curiosity.enable", template.get("curiosity", False))
        config.set("development.enable", template.get("development", False))
        
        configs.append(config)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
        
        print(f"  Brain {i+1}: {template['neurons']} neurons, "
              f"learning={template.get('learning', False)}, "
              f"curiosity={template.get('curiosity', False)}")
    
    creation_time = time.time() - start_time
    print(f"Total creation time: {creation_time:.3f}s")
    
    # Run batch simulation
    print("\nRunning batch simulation...")
    batch_start = time.time()
    
    for episode in range(5):
        print(f"\nBatch Episode {episode + 1}:")
        
        for brain_idx, brain in enumerate(brains):
            # Create fresh world for each brain
            world = pynlm.createQuickWorld(10, 10, 4, 4)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Run episode
            episode_reward = 0.0
            for step in range(50):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                episode_reward += reward
            
            print(f"  Brain {brain_idx+1}: reward={episode_reward:7.3f}, "
                  f"curiosity={agent.getCuriosityLevel():5.3f}")
    
    batch_time = time.time() - batch_start
    print(f"\nBatch simulation time: {batch_time:.3f}s")
    print(f"Average time per brain: {batch_time/len(brains):.3f}s")
    
    return brains, configs

brains, configs = batchProcessingExample()
```

---

# Advanced Features

## 7.1 System Integration

### Example 7.1: Complete System Integration

```python
import pynlm
import time

def completeSystemIntegration():
    """Demonstrate complete system integration with all features"""
    
    print("Complete System Integration")
    print("=" * 50)
    
    # Create sophisticated configuration
    config = pynlm.createDefaultConfig()
    
    # Brain configuration
    config.set("brain.neuron_count", 400)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.v_thresh", -50.0)
    config.set("brain.v_reset", -75.0)
    
    # Plasticity configuration (advanced learning)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.005)
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0001)
    config.set("plasticity.structural.pruning_rate", 0.00001)
    
    # Neuromodulation configuration
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.prediction.error_weight", 0.3)
    
    # Development configuration
    config.set("development.enable", True)
    config.set("development.critical_period", 150)
    config.set("development.maturation_rate", 0.02)
    config.set("development.pruning_rate", 0.01)
    
    # Memory configuration
    config.set("memory.working_capacity", 100)
    config.set("memory.episodic_capacity", 500)
    config.set("memory.associative_strength", 0.8)
    
    # Create complete system
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createQuickWorld(30, 30, 12, 12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    world.setMaxEnergy(300.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("System Configuration:")
    print(f"  Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"  World: {world.getWidth()}x{world.getHeight()} with agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"  Learning: STDP={config.has('plasticity.stdp.enable')}, Hebbian={config.has('plasticity.hebbian.enable')}")
    print(f"  Neuromodulation: Dopamine={config.has('neuromod.dopamine.scale')}, Curiosity={config.has('neuromod.curiosity.enable')}")
    print(f"  Development: Enabled={config.has('development.enable')}, Critical period={config.has('development.critical_period')}")
    print()
    
    # Run comprehensive experiment
    print("Running comprehensive integration experiment...")
    
    experiment_results = {
        'rewards': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'development_stages': [],
        'spike_counts': [],
        'memory_usage': []
    }
    
    for episode in range(10):
        print(f"Episode {episode + 1}/10")
        
        # Reset for new episode
        brain.reset()
        brain.initialize()
        world.reset()
        agent.reset()
        
        # Run episode
        episode_reward = 0.0
        episode_steps = 100
        
        for step in range(episode_steps):
            # World dynamics
            world.update(0.1)
            
            # Sensory processing
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            
            # Decision making
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Learning
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            predicted = reward * 0.5
            
            agent.applyRewardModulation(reward, predicted)
            agent.updateDevelopment(0.1)
            
            episode_reward += reward
            
            # Record metrics
            experiment_results['rewards'].append(reward)
            experiment_results['curiosity_levels'].append(agent.getCuriosityLevel())
            experiment_results['novelty_levels'].append(agent.getNoveltyLevel())
            experiment_results['development_stages'].append(brain.getDevelopmentalStage())
            experiment_results['spike_counts'].append(brain.getTotalSpikeCount())
        
        print(f"  Total reward: {episode_reward:8.3f}")
        print(f"  Avg curiosity: {sum(experiment_results['curiosity_levels'][-episode_steps:]) / episode_steps:.3f}")
        print(f"  Avg novelty: {sum(experiment_results['novelty_levels'][-episode_steps:]) / episode_steps:.3f}")
        print(f"  Final development stage: {brain.getDevelopmentalStage()}")
        print()
    
    # Calculate summary statistics
    def calculate_stats(data):
        if not data:
            return 0.0, 0.0
        return sum(data) / len(data), max(data) - min(data)
    
    print("INTEGRATION SUMMARY")
    print("=" * 50)
    print(f"Learning System:")
    print(f"  Average reward per step: {calculate_stats(experiment_results['rewards'])[0]:.4f}")
    print(f"  Reward range: {calculate_stats(experiment_results['rewards'])[1]:.3f}")
    print()
    print(f"Curiosity System:")
    print(f"  Average curiosity level: {calculate_stats(experiment_results['curiosity_levels'])[0]:.3f}")
    print(f"  Curiosity range: {calculate_stats(experiment_results['curiosity_levels'])[1]:.3f}")
    print()
    print(f"Novelty System:")
    print(f"  Average novelty level: {calculate_stats(experiment_results['novelty_levels'])[0]:.3f}")
    print(f"  Novelty range: {calculate_stats(experiment_results['novelty_levels'])[1]:.3f}")
    print()
    print(f"Development System:")
    print(f"  Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  Average stage: {calculate_stats(experiment_results['development_stages'])[0]:.1f}")
    print()
    print(f"Performance:")
    print(f"  Average firing rate: {calculate_stats(experiment_results['spike_counts'])[0]:.1f}")
    print(f"  Spike count range: {calculate_stats(experiment_results['spike_counts'])[1]:.0f}")
    
    return brain, world, agent, experiment_results

brain, world, agent, results = completeSystemIntegration()
```

### Example 7.2: Advanced Monitoring and Visualization

```python
import pynlm
import matplotlib.pyplot as plt

def advancedMonitoring(brain, world, agent, duration=200):
    """Advanced monitoring with data collection and visualization"""
    
    print("Advanced Monitoring System")
    print("=" * 50)
    
    # Monitoring data collection
    monitoring_data = {
        'time': [],
        'position_x': [],
        'position_y': [],
        'curiosity': [],
        'novelty': [],
        'reward': [],
        'spike_count': [],
        'firing_rate': [],
        'development_stage': [],
        'energy': [],
        'orientation': []
    }
    
    print("Collecting monitoring data...")
    
    for step in range(duration):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Action selection
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Collect data
        body = world.getAgentBody()
        monitoring_data['time'].append(step)
        monitoring_data['position_x'].append(body.x)
        monitoring_data['position_y'].append(body.y)
        monitoring_data['curiosity'].append(agent.getCuriosityLevel())
        monitoring_data['novelty'].append(agent.getNoveltyLevel())
        monitoring_data['reward'].append(reward)
        monitoring_data['spike_count'].append(brain.getTotalSpikeCount())
        monitoring_data['firing_rate'].append(brain.getAverageFiringRate())
        monitoring_data['development_stage'].append(brain.getDevelopmentalStage())
        monitoring_data['energy'].append(body.energy)
        monitoring_data['orientation'].append(body.orientation)
        
        # Print real-time updates
        if step % 20 == 0:
            print(f"Step {step:3d}: "
                  f"Pos({body.x:5.1f},{body.y:5.1f}) "
                  f"Curiosity:{agent.getCuriosityLevel():5.3f} "
                  f"Novelty:{agent.getNoveltyLevel():5.3f} "
                  f"Reward:{reward:6.3f} "
                  f"Energy:{body.energy:5.1f}")
    
    # Generate visualizations
    print("\nGenerating visualizations...")
    
    try:
        # Create figures
        fig, axes = plt.subplots(3, 2, figsize=(15, 12))
        fig.suptitle('NLM Advanced Monitoring Dashboard', fontsize=16)
        
        # Position trajectory
        axes[0, 0].plot(monitoring_data['position_x'], 
                       monitoring_data['position_y'], 'b-', alpha=0.7, linewidth=2)
        axes[0, 0].plot(monitoring_data['position_x'][0], 
                       monitoring_data['position_y'][0], 'go', markersize=10, label='Start')
        axes[0, 0].plot(monitoring_data['position_x'][-1], 
                       monitoring_data['position_y'][-1], 'ro', markersize=10, label='End')
        axes[0, 0].set_xlabel('X Position')
        axes[0, 0].set_ylabel('Y Position')
        axes[0, 0].set_title('Agent Trajectory')
        axes[0, 0].legend()
        axes[0, 0].grid(True, alpha=0.3)
        
        # Curiosity over time
        axes[0, 1].plot(monitoring_data['time'], 
                       monitoring_data['curiosity'], 'r-', linewidth=2)
        axes[0, 1].set_xlabel('Time Step')
        axes[0, 1].set_ylabel('Curiosity Level')
        axes[0, 1].set_title('Curiosity Dynamics')
        axes[0, 1].grid(True, alpha=0.3)
        axes[0, 1].set_ylim(0, 1)
        
        # Novelty over time
        axes[1, 0].plot(monitoring_data['time'], 
                       monitoring_data['novelty'], 'g-', linewidth=2)
        axes[1, 0].set_xlabel('Time Step')
        axes[1, 0].set_ylabel('Novelty Level')
        axes[1, 0].set_title('Novelty Dynamics')
        axes[1, 0].grid(True, alpha=0.3)
        axes[1, 0].set_ylim(0, 1)
        
        # Reward over time
        axes[1, 1].plot(monitoring_data['time'], 
                       monitoring_data['reward'], 'b-', linewidth=1, alpha=0.7)
        axes[1, 1].set_xlabel('Time Step')
        axes[1, 1].set_ylabel('Reward')
        axes[1, 1].set_title('Reward Accumulation')
        axes[1, 1].grid(True, alpha=0.3)
        
        # Firing rate over time
        axes[2, 0].plot(monitoring_data['time'], 
                       monitoring_data['firing_rate'], 'm-', linewidth=2)
        axes[2, 0].set_xlabel('Time Step')
        axes[2, 0].set_ylabel('Firing Rate')
        axes[2, 0].set_title('Neural Firing Rate')
        axes[2, 0].grid(True, alpha=0.3)
        
        # Development stage
        axes[2, 1].plot(monitoring_data['time'], 
                       monitoring_data['development_stage'], 'c-', linewidth=3, marker='o', markersize=2)
        axes[2, 1].set_xlabel('Time Step')
        axes[2, 1].set_ylabel('Developmental Stage')
        axes[2, 1].set_title('Development Progress')
        axes[2, 1].grid(True, alpha=0.3)
        
        # Save figure
        plt.tight_layout()
        plt.savefig('nlm_monitoring_dashboard.png', dpi=150, bbox_inches='tight')
        print(f"Dashboard saved to: nlm_monitoring_dashboard.png")
        
        # Show summary statistics
        print("\nMONITORING STATISTICS")
        print("=" * 50)
        print(f"Trajectory:")
        print(f"  Start: ({monitoring_data['position_x'][0]:.1f}, {monitoring_data['position_y'][0]:.1f})")
        print(f"  End:   ({monitoring_data['position_x'][-1]:.1f}, {monitoring_data['position_y'][-1]:.1f})")
        print(f"  Distance traveled: {((monitoring_data['position_x'][-1] - monitoring_data['position_x'][0])**2 + 
                                (monitoring_data['position_y'][-1] - monitoring_data['position_y'][0])**2)**0.5:.1f} units")
        
        print(f"\nLearning Metrics:")
        avg_curiosity = sum(monitoring_data['curiosity']) / len(monitoring_data['curiosity'])
        avg_novelty = sum(monitoring_data['novelty']) / len(monitoring_data['novelty'])
        avg_reward = sum(monitoring_data['reward']) / len(monitoring_data['reward'])
        
        print(f"  Average curiosity: {avg_curiosity:.3f}")
        print(f"  Average novelty: {avg_novelty:.3f}")
        print(f"  Average reward per step: {avg_reward:.3f}")
        print(f"  Total reward: {sum(monitoring_data['reward']):.3f}")
        
        print(f"\nNeural Metrics:")
        avg_firing_rate = sum(monitoring_data['firing_rate']) / len(monitoring_data['firing_rate'])
        avg_spike_count = sum(monitoring_data['spike_count']) / len(monitoring_data['spike_count'])
        
        print(f"  Average firing rate: {avg_firing_rate:.1f} Hz")
        print(f"  Average spike count: {avg_spike_count:.0f}")
        
        print(f"\nDevelopment:")
        final_stage = monitoring_data['development_stage'][-1]
        print(f"  Final developmental stage: {final_stage}")
        print(f"  Time in each stage:")
        for stage in range(6):
            stage_time = sum(1 for s in monitoring_data['development_stage'] if s == stage)
            print(f"    Stage {stage}: {stage_time} steps ({stage_time/len(monitoring_data['time'])*100:.1f}%)")
        
        return monitoring_data
        
    except ImportError:
        print("matplotlib not available. Skipping visualization.")
        print("Install matplotlib with: pip install matplotlib")
        
        # Return basic statistics
        return monitoring_data

monitoring_data = advancedMonitoring(brain, world, agent, 300)
```

---

# Troubleshooting

## 8.1 Common Issues and Solutions

### 8.1.1 Brain Initialization Problems

**Problem:** Brain initialization fails or takes too long.

**Solutions:**

```python
# Solution 1: Check configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 50)  # Reduce neuron count
config.set("plasticity.stdp.enable", False)  # Disable learning
config.set("neuromod.curiosity.enable", False)  # Disable curiosity

brain = pynlm.createBrain(config)
if (!brain->initialize()) {
    std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
    return 1;
}

# Solution 2: Check available memory
// Monitor memory usage before initialization
```

### 8.1.2 Performance Issues

**Problem:** Simulation is too slow or runs out of memory.

**Solutions:**

```python
# Solution 1: Optimize configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)  // Fewer neurons
config.set("plasticity.stdp.enable", False)  // No STDP
config.set("neuromod.curiosity.enable", False)  // No curiosity
config.set("development.enable", False)  // No development

# Solution 2: Batch processing for multiple simulations
brains = []
for i in range(10):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 50)  // Small brains
    brain = pynlm.createBrain(config)
    brain->initialize()
    brains.append(brain)

// Process each brain individually to manage memory
```

### 8.1.3 Learning System Issues

**Problem:** No learning occurs or learning is too slow.

**Solutions:**

```python
// Solution 1: Check plasticity parameters
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.01)  // Higher learning rate
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.005)

// Solution 2: Ensure proper sensory input
world->update(0.1);
SensoryPercept percept = world->getSensoryPercept();
agent->processSensoryInput(percept);

// Solution 3: Check reward signals
float reward = world->getSensoryPercept().getInternal()[0];
agent->applyRewardModulation(reward, 0.0);
```

### 8.1.4 Agent Behavior Issues

**Problem:** Agent doesn't move or learn properly.

**Solutions:**

```python
// Solution 1: Check action decoding
agent->processSensoryInput(percept);
Action action = agent->decodeMotorCommand();
if (action.getType() == ActionType::None) {
    std::cout << "WARNING: No action selected!" << std::endl;
}

// Solution 2: Ensure world updates
world->update(0.1);  // Must update world before getting percepts

// Solution 3: Check agent initialization
agent->initialize(world);  // Re-initialize if needed
agent->enableRewardModulation(true);  // Ensure learning is enabled
```

### 8.1.5 Configuration Problems

**Problem:** Configuration file not found or invalid.

**Solutions:**

```python
// Solution 1: Create default config
config = pynlm.createDefaultConfig();

// Solution 2: Save working config as backup
config->saveToFile("working_config.json");

// Solution 3: Load config with error handling
try {
    if (!config->loadFromFile("my_config.json")) {
        std::cout << "Using default configuration (file not found)" << std::endl;
    }
} catch (const std::exception& e) {
    std::cout << "Error loading config: " << e.what() << std::endl;
    std::cout << "Using default configuration" << std::endl;
}
```

## 8.2 Debugging Techniques

### 8.2.1 Logging and Output

```python
import pynlm

// Enable verbose logging
brain = pynlm.createBrain(config)
brain->logStatus();  // Log detailed brain status

// Enable console logging
// (Configure in C++ code or through config file)

// Log specific events
for (step = 0; step < max_steps; ++step) {
    if (step % 100 == 0) {
        brain->logStatus();  // Log every 100 steps
    }
    world->update(0.1);
    // ... rest of simulation
}
```

### 8.2.2 Step-by-Step Debugging

```python
import pynlm

// Debug individual components
brain = pynlm.createBrain(config)
brain->initialize()

// Check brain components
std::cout << "Brain components:" << std::endl;
std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
std::cout << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;

// Test individual neurons
for (int i = 0; i < 5; ++i) {
    NeuronId neuron_id(i);
    if (brain->getNeuron(neuron_id)) {
        std::cout << "  Neuron " << i << ": "
                  << "V=" << brain->getNeuron(neuron_id)->getMembranePotential()
                  << ", firing=" << brain->getNeuron(neuron_id)->isFiring()
                  << std::endl;
    }
}
```

### 8.2.3 Validation Scripts

```python
import pynlm

def validateNLMSetup():
    """Validate NLM setup before running simulations"""
    
    print("Validating NLM Setup...")
    
    // Test 1: Configuration validation
    config = pynlm.createDefaultConfig()
    if (!pynlm::validateBrainConfiguration(config)) {
        std::cout << "ERROR: Invalid default configuration!" << std::endl;
        return false;
    }
    
    // Test 2: Brain initialization
    brain = pynlm::createQuickBrain(10, false);  // Small brain, no learning
    if (!brain->initialize()) {
        std::cout << "ERROR: Brain initialization failed!" << std::endl;
        return false;
    }
    
    // Test 3: Basic functionality
    for (step = 0; step < 10; ++step) {
        brain->step(step);
        std::cout << "  Step " << step << ": "
                  << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
    }
    
    // Test 4: Component availability
    world = pynlm::createQuickWorld(5, 5);
    agent = pynlm::createQuickAgent(brain, world);
    
    if (!agent->getBrain() || agent->getBrain() != brain) {
        std::cout << "ERROR: Agent-brain connection failed!" << std::endl;
        return false;
    }
    
    std::cout << "✓ NLM setup validation successful!" << std::endl;
    return true;
}

bool setup_valid = validateNLMSetup();
if (!setup_valid) {
    std::cout << "Setup failed. Please check configuration and try again." << std::endl;
}
```

### 8.2.4 Error Handling

```python
import pynlm
import traceback

def safeSimulationStep(brain, world, agent, step) {
    """Safely execute a simulation step with error handling"""
    
    try {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        SensoryPercept percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain processing
        brain->step(step);
        
        // Get action
        Action action = agent->decodeMotorCommand();
        
        // Apply action
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Get reward
        float reward = 0.0;
        if (percept.getInternal() && !percept.getInternal()->empty()) {
            reward = (*percept.getInternal())[0];
        }
        
        // Learn
        agent->applyRewardModulation(reward, 0.0);
        
        return true; // Success
        
    } catch (const std::exception& e) {
        std::cout << "ERROR in simulation step " << step << ": " << e.what() << std::endl;
        std::cout << "Stack trace:" << std::endl;
        std::cout << traceback.format_exc() << std::endl;
        
        // Attempt recovery
        try {
            brain->reset();
            brain->initialize();
            return false; // Recovery attempted
        } catch (...) {
            std::cout << "ERROR: Recovery failed!" << std::endl;
            return false;
        }
    }
}
```

## 8.3 Getting Help

### 8.3.1 Documentation Resources

- **NLM Documentation:** https://github.com/nlm-project/nlm
- **API Reference:** Python bindings documentation
- **Tutorials:** Examples and workflows
- **Examples:** Complete working examples

### 8.3.2 Community Support

- **GitHub Issues:** Report bugs and request features
- **Discussions:** Ask questions and share experiences
- **Email:** Direct support for technical issues

### 8.3.3 Troubleshooting Checklist

```
□ Check configuration files
□ Verify brain initialization
□ Test sensory input processing
□ Validate action selection
□ Monitor reward signals
□ Check learning subsystem activation
□ Review performance metrics
□ Validate state management
```

---

# Conclusion

This comprehensive tutorial provides everything you need to get started with NLM, from basic concepts to advanced system integration. Whether you're a beginner looking to understand the fundamentals or an experienced researcher implementing complex neural architectures, NLM offers the flexibility and power needed for advanced brain simulation research.

The examples and workflows provided here demonstrate:

1. **Quick Start:** Get up and running in minutes
2. **Basic Usage:** Understand core concepts and operations
3. **Advanced Features:** Leverage all NLM capabilities
4. **Performance Optimization:** Efficient simulation strategies
5. **Real-World Applications:** Complete system integration
6. **Troubleshooting:** Effective debugging and problem-solving

Start with the Quick Start section, then progress through the tutorials at your own pace. NLM's modular design allows you to learn as you go, building expertise while exploring the full potential of this experimental neural learning machine.

Happy simulating! 🚀
EOF