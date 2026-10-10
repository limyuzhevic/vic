# Comprehensive Python API Documentation

This document provides a complete reference for the NLM Python API, designed to be accessible to beginners while providing advanced functionality for experienced users.

## Overview

NLM (Neural Learning Machine) is a Python library for simulating brain-like neural networks using the PyNLM C++ framework. The API provides high-level abstractions for creating and running neural simulations while maintaining access to low-level control.

## Installation

```bash
# Install from source (recommended for development)
pip install -e .

# Or install from PyPI (if available)
pip install pynlm
```

## Getting Started

### Basic Brain Simulation

This example shows the simplest way to create and run a brain simulation:

```python
import pynlm
import time

print("=== NLM Basic Brain Simulation ===")

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run a simple simulation
print("\nRunning simulation...")
start_time = time.time()
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")

end_time = time.time()
print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Complete Agent in World

This example demonstrates a complete agent-environment interaction:

```python
import pynlm
import math

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration and brain
    print("=== Creating Agent Brain ===")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # 2. Create world and agent interface
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 3. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Enabled learning subsystems:")
    print("  - Reward modulation")
    print("  - Structural plasticity")
    print("  - Developmental learning")
    print("  - Curiosity-driven exploration")
    
    # 4. Run simulation loop
    print(f"\n=== Running {num_steps} simulation steps ===")
    
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
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    print(f"\n=== Simulation Complete ===")
    print(f"Final metrics:")
    print(f"  Total reward: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty: {agent.getNoveltyLevel():.3f}")
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_agent_simulation(1000)
```

### Interactive Exploration

This example allows you to interactively explore the brain's behavior:

```python
import pynlm
import numpy as np

def explore_brain_behavior():
    """Interactive exploration of brain behavior."""
    
    print("=== NLM Brain Behavior Explorer ===")
    
    # Create brain with specific configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Number of regions: {brain.getRegionCount()}")
    
    # Explore different sensory inputs
    print("\n=== Exploring Sensory Inputs ===")
    
    # Create different types of sensory inputs
    from pynlm import Vision, InternalSignals, SensoryPercept
    
    # Vision input (256 values)
    vision_data = np.random.rand(16, 16) * 2 - 1  # Normalized to [-1, 1]
    vision = Vision(16, 16)
    vision.setData(vision_data.flatten())
    
    # Internal signals
    internal = InternalSignals()
    for i in range(4):
        internal.addSignal(np.random.randn() * 0.5)
    
    # Combine into a percept
    percept = SensoryPercept()
    percept.setVision(vision)
    percept.setInternal(internal)
    
    # Inject into brain
    brain.receiveSensoryInput(percept)
    
    # Run brain to process input
    brain.step(0)
    
    print(f"After sensory input:")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Active regions: {brain.getActiveRegionCount()}")
    
    # Explore motor output
    print("\n=== Exploring Motor Output ===")
    action = brain.produceAction()
    print(f"Produced action: {action.getType()}")
    print(f"Action parameters: {action.getParameters()}")
    
    # Explore memory systems
    print("\n=== Exploring Memory Systems ===")
    if brain.getWorkingMemory():
        traces = brain.getWorkingMemory().getActiveTraces()
        print(f"Working memory active traces: {traces}")
    
    if brain.getEpisodicMemory():
        episodes = brain.getEpisodicMemory().getEpisodeCount()
        print(f"Episodic memory stored episodes: {episodes}")
    
    # Explore neuromodulation
    print("\n=== Exploring Neuromodulation ===")
    if hasattr(brain, 'getDopamine'):
        dopamine = brain.getDopamine()
        print(f"Dopamine level: {dopamine.getLevel():.3f}")
    
    if hasattr(brain, 'getCuriosity'):
        curiosity = brain.getCuriosity()
        print(f"Curiosity level: {curiosity.getLevel():.3f}")
    
    return brain

brain = explore_brain_behavior()
```

## Configuration Management

### Creating and Modifying Configurations

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify configuration parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.dopamine.scale", 1.5)

# Check configuration
if config.has("brain.neuron_count"):
    count = config.get("brain.neuron_count")
    print(f"Neuron count: {count.value}")

# Get values with defaults
neuron_count = config.getOr("brain.neuron_count", 1000)
synapse_density = config.getOr("brain.synapse_density", 0.1)

print(f"Using {neuron_count} neurons and {synapse_density} synapse density")

# Save configuration
config.saveToFile("my_config.cfg")

# Load configuration from file
new_config = pynlm.createDefaultConfig()
if new_config.loadFromFile("my_config.cfg"):
    print("Configuration loaded successfully")

# Get all keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Configuration summary
print("\nConfiguration summary:")
print(config.summary())
```

## Agent API Reference

The AgentBrain class provides a high-level interface for brain-world interaction:

### Core Methods

```python
# Initialize agent with world
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Process sensory input from world
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Get motor command for action
motor_cmd = agent.decodeMotorCommand()
world.applyMotorCommand(motor_cmd, current_time)

# Apply reward-based learning
reward = world.getReward()
agent.applyRewardModulation(reward, predicted_reward)

# Update developmental state
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Get neuromodulation levels
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()

# Check subsystem status
agent.isRewardModulationEnabled()
agent.isStructuralPlasticityEnabled()
agent.isDevelopmentEnabled()
agent.isCuriosityEnabled()
```

### Advanced Agent Usage

```python
def advanced_agent_usage(brain, world):
    """Advanced agent usage patterns."""
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable specific subsystems based on task requirements
    agent.enableRewardModulation(True)  # For goal-directed behavior
    agent.enableCuriosity(True)        # For exploration
    agent.enableDevelopment(True)      # For adaptation
    # agent.enableStructuralPlasticity(False)  # Disable for stability
    
    # Custom learning rate adjustment
    def adaptive_learning(reward, predicted):
        error = abs(reward - predicted)
        # Adjust learning based on prediction error
        learning_rate = min(1.0, error * 2.0)
        agent.applyRewardModulation(reward, predicted * learning_rate)
    
    # Run episode with custom logic
    episode_reward = 0.0
    for step in range(100):
        # Get state
        percept = world.getSensoryPercept()
        
        # Process
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Act
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.1)
        
        # Learn
        reward = world.getReward()
        adaptive_learning(reward, 0.0)
        episode_reward += reward
        
        # Develop
        agent.updateDevelopment(0.1)
    
    return episode_reward

# Use the advanced agent
reward = advanced_agent_usage(brain, world)
```

## World Simulation

### SimpleWorld Usage

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=30.0,
    height=30.0,
    visionWidth=16,
    visionHeight=16
)

# Set agent starting position
world.setAgentStart(15.0, 15.0)

# Reset world to initial state
world.reset()

# Add objects to world
world.addObject(pynlm.WorldObject(
    x=5.0, y=5.0, 
    type=pynlm.WorldObjectType.Resource, 
    value=10.0
))

world.addObject(pynlm.WorldObject(
    x=25.0, y=25.0,
    type=pynlm.WorldObjectType.Hazard,
    value=-5.0
))

# Run world update
world.update(timestep=0.1)

# Get sensory percept for agent
percept = world.getSensoryPercept()
vision = percept.getVision()
touch = percept.getTouch()

# Get agent state
agent_body = world.getAgentBody()
print(f"Agent at ({agent_body.x:.1f}, {agent_body.y:.1f})")
print(f"Agent energy: {agent_body.energy:.1f}")
print(f"Agent health: {agent_body.health:.1f}")

# Check position validity
if world.isValidPosition(10.0, 10.0):
    print("Position (10.0, 10.0) is valid")
else:
    print("Position (10.0, 10.0) is invalid")

# Set random seed for reproducible simulations
world.setRandomSeed(42)

# Get world statistics
print(f"World width: {world.getWidth()}")
print(f"World height: {world.getHeight()}")
print(f"Max energy: {world.getMaxEnergy()}")
print(f"Simulation time: {world.getSimulationTime():.2f}")
```

## Advanced Usage Patterns

### Pattern 1: Training with Experience Replay

```python
def train_with_experience_replay(brain, world, agent, num_episodes, replay_interval=100):
    """Train using experience replay for stable learning."""
    
    episodes = []  # Store experiences for replay
    
    for episode in range(num_episodes):
        print(f"Episode {episode + 1}/{num_episodes}")
        
        # Reset environment
        world.reset()
        brain.reset()
        
        episode_reward = 0.0
        steps_in_episode = 0
        
        # Run episode
        for step in range(1000):
            # Get observation
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get motor command
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Compute reward
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            episode_reward += reward
            steps_in_episode += 1
            
            # Store experience for replay
            if episode % replay_interval == 0:
                experience = {
                    'percept': percept,
                    'action': motor_cmd,
                    'reward': reward,
                    'step': step
                }
                episodes.append(experience)
        
        # Periodically replay experiences
        if len(episodes) > 0 and episode % replay_interval == 0:
            print(f"  Replaying {len(episodes)} stored experiences...")
            replay_experiences(brain, agent, episodes)
            episodes.clear()  # Clear after replay
        
        print(f"  Episode reward: {episode_reward:.2f}")
        print(f"  Steps: {steps_in_episode}")
    
    return brain, world, agent

def replay_experiences(brain, agent, experiences):
    """Replay stored experiences for stable learning."""
    for exp in experiences[:10]:  # Replay last 10 experiences
        # Process stored percept
        agent.processSensoryInput(exp['percept'])
        
        # Run brain step with stored context
        brain.step(exp['step'] + 1000)  # Offset steps to avoid overlap
        
        # Apply reward modulation
        agent.applyRewardModulation(exp['reward'], 0.0)
```

### Pattern 2: Developmental Learning

```python
def developmental_training(brain, world, agent, num_stages=5):
    """Train with developmental stages."""
    
    print("=== Developmental Training ===")
    
    for stage in range(num_stages):
        print(f"\nDevelopmental Stage {stage + 1}/{num_stages}")
        
        # Set developmental stage
        brain.setDevelopmentalStage(stage)
        agent.updateDevelopment(0.1)
        
        # Adjust learning rates based on developmental stage
        if stage < 2:  # Early stages: exploration-focused
            agent.enableCuriosity(True)
            agent.enableStructuralPlasticity(True)
        elif stage < 4:  # Middle stages: balanced
            agent.enableCuriosity(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
        else:  # Later stages: exploitation-focused
            agent.enableCuriosity(False)
            agent.enableDevelopment(True)
        
        # Train for this stage
        stage_reward = 0.0
        for step in range(500):
            # Get observation
            percept = world.getSensoryPercept()
            
            # Process and act
            agent.processSensoryInput(percept)
            brain.step(step + stage * 1000)
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Learn
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            stage_reward += reward
        
        print(f"  Stage reward: {stage_reward:.2f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    # Set to adult stage
    brain.setDevelopmentalStage(num_stages - 1)
    return brain, world, agent
```

### Pattern 3: Multi-objective Optimization

```python
def multi_objective_training(brain, world, agent, objectives):
    """Train for multiple objectives simultaneously."""
    
    print("=== Multi-Objective Training ===")
    
    for objective_name, objective_params in objectives.items():
        print(f"\nTraining for: {objective_name}")
        
        # Configure for this objective
        if objective_name == "exploration":
            agent.enableCuriosity(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
        elif objective_name == "exploitation":
            agent.enableCuriosity(False)
            agent.enableDevelopment(True)
        elif objective_name == "conservation":
            agent.enableCuriosity(False)
            agent.enableDevelopment(True)
            # Reduce plasticity to conserve energy
            config = brain.getConfig()
            config.set("synaptogenesis_rate", 0.00001)
        
        # Train for this objective
        objective_reward = 0.0
        for step in range(500):
            # Get observation
            percept = world.getSensoryPercept()
            
            # Process
            agent.processSensoryInput(percept)
            brain.step(step)
            
            # Act based on objective
            if objective_name == "exploration":
                # Act randomly for exploration
                action = pynlm.Action(pynlm.ActionType.Custom)
                action.setParameters([np.random.randn() for _ in range(6)])
            else:
                # Act normally for exploitation
                action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Compute multi-objective reward
            reward = compute_multi_objective_reward(world, agent, objective_params)
            agent.applyRewardModulation(reward, 0.0)
            
            objective_reward += reward
        
        print(f"  Objective reward: {objective_reward:.2f}")
    
    return brain, world, agent

def compute_multi_objective_reward(world, agent, params):
    """Compute reward based on multiple objectives."""
    
    reward = 0.0
    
    # Reward components
    for i, (component, weight) in enumerate(params.items()):
        if component == "energy":
            agent_body = world.getAgentBody()
            reward += agent_body.energy * weight
        elif component == "health":
            agent_body = world.getAgentBody()
            reward += agent_body.health * weight
        elif component == "novelty":
            novelty = agent.getNoveltyLevel()
            reward += novelty * weight
        elif component == "curiosity":
            curiosity = agent.getCuriosityLevel()
            reward += curiosity * weight
        elif component == "goal_progress":
            # Custom goal progress calculation
            progress = calculate_goal_progress(world)
            reward += progress * weight
    
    return reward
```

## Error Handling and Troubleshooting

### Common Issues and Solutions

```python
def common_troubleshooting():
    """Common troubleshooting patterns."""
    
    print("=== NLM Troubleshooting Guide ===")
    
    # Issue 1: Brain not responding to input
    print("\n1. Brain not responding to input:")
    print("   - Ensure brain.initialize() is called before step()")
    print("   - Check that sensory input is being properly connected")
    print("   - Verify neuron parameters (threshold, membrane potential)")
    
    # Issue 2: Zero firing rate
    print("\n2. Zero firing rate:")
    print("   - Increase injected current: brain.injectCurrentToNeurons(type, amount)")
    print("   - Check neuron excitability parameters")
    print("   - Verify synaptic connectivity")
    
    # Issue 3: Memory systems not working
    print("\n3. Memory systems not working:")
    print("   - Ensure memory systems are enabled in agent")
    print("   - Check brain.getWorkingMemory() and brain.getEpisodicMemory()")
    print("   - Verify memory system initialization")
    
    # Issue 4: Performance issues
    print("\n4. Performance issues:")
    print("   - Reduce neuron count for debugging")
    print("   - Check for infinite loops in simulation")
    print("   - Monitor memory usage")
    
    # Issue 5: Configuration errors
    print("\n5. Configuration errors:")
    print("   - Validate all configuration keys before use")
    print("   - Check type compatibility (int vs float vs string)")
    print("   - Use config.has() to check for key existence")

# Run troubleshooting guide
troubleshooting = common_troubleshooting()
```

## Performance Tips

### Optimizing Simulation Speed

```python
import pynlm
import time

def performance_comparison():
    """Compare different performance optimizations."""
    
    print("=== NLM Performance Comparison ===")
    
    # Configuration 1: Small, simple brain
    config1 = pynlm.createDefaultConfig()
    config1.set("brain.neuron_count", 100)
    config1.set("brain.synapse_density", 0.05)
    
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    
    # Configuration 2: Medium, balanced brain
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 1000)
    config2.set("brain.synapse_density", 0.1)
    
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    # Configuration 3: Large, complex brain
    config3 = pynlm.createDefaultConfig()
    config3.set("brain.neuron_count", 5000)
    config3.set("brain.synapse_density", 0.15)
    
    brain3 = pynlm.createBrain(config3)
    brain3.initialize()
    
    # Benchmark each configuration
    steps = 100
    
    # Benchmark brain1
    start = time.time()
    for step in range(steps):
        brain1.step(step)
    time1 = time.time() - start
    
    # Benchmark brain2
    start = time.time()
    for step in range(steps):
        brain2.step(step)
    time2 = time.time() - start
    
    # Benchmark brain3
    start = time.time()
    for step in range(steps):
        brain3.step(step)
    time3 = time.time() - start
    
    print(f"\nPerformance Results ({steps} steps):")
    print(f"  Brain 1 (100 neurons):  {time1:.3f}s = {steps/time1:.0f} steps/sec")
    print(f"  Brain 2 (1000 neurons): {time2:.3f}s = {steps/time2:.0f} steps/sec")
    print(f"  Brain 3 (5000 neurons): {time3:.3f}s = {steps/time3:.0f} steps/sec")
    
    # Performance per neuron
    perf1 = steps / time1 / 100  # per neuron
    perf2 = steps / time2 / 1000
    perf3 = steps / time3 / 5000
    
    print(f"\nPerformance per neuron:")
    print(f"  Brain 1: {perf1:.2f} steps/sec/neuron")
    print(f"  Brain 2: {perf2:.2f} steps/sec/neuron")
    print(f"  Brain 3: {perf3:.2f} steps/sec/neuron")
    
    return brain1, brain2, brain3

brains = performance_comparison()
```

### Memory Management

```python
import pynlm
import gc

def memory_efficiency_test():
    """Test memory efficiency of different approaches."""
    
    print("=== NLM Memory Efficiency Test ===")
    
    import psutil
    import os
    
    process = psutil.Process(os.getpid())
    
    # Test 1: Single large brain
    print("\nTest 1: Single large brain (5000 neurons)")
    brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain1.initialize()
    
    mem_before = process.memory_info().rss / 1024 / 1024  # MB
    for step in range(100):
        brain1.step(step)
    mem_after = process.memory_info().rss / 1024 / 1024
    
    print(f"  Memory usage: {mem_before:.1f} MB -> {mem_after:.1f} MB")
    print(f"  Memory growth: {mem_after - mem_before:.1f} MB")
    
    del brain1
    gc.collect()
    
    # Test 2: Multiple small brains
    print("\nTest 2: Multiple small brains (100 neurons each)")
    mem_before = process.memory_info().rss / 1024 / 1024
    
    brains = []
    for i in range(10):
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        brains.append(brain)
        
        for step in range(10):
            brain.step(step)
    
    mem_after = process.memory_info().rss / 1024 / 1024
    print(f"  Memory usage: {mem_before:.1f} MB -> {mem_after:.1f} MB")
    print(f"  Memory growth: {mem_after - mem_before:.1f} MB")
    
    # Clean up
    for brain in brains:
        del brain
    gc.collect()
    
    return brain1

memory_efficiency_test()
```

## Research and Development Tools

### Advanced Research Functions

```python
def research_mode(brain, world, agent, research_protocol):
    """Run advanced research protocols."""
    
    print(f"=== Running Research Protocol: {research_protocol} ===")
    
    # Save initial state
    initial_checkpoint = brain.save("research_initial.bin")
    
    # Run research-specific training
    if research_protocol == "developmental_stages":
        run_developmental_research(brain, world, agent)
    elif research_protocol == "social_learning":
        run_social_learning_research(brain, world, agent)
    elif research_protocol == "memory_consolidation":
        run_memory_research(brain, world, agent)
    elif research_protocol == "neuromodulation_dynamics":
        run_neuromodulation_research(brain, world, agent)
    
    # Save final state
    final_checkpoint = brain.save("research_final.bin")
    
    # Compare states
    if initial_checkpoint and final_checkpoint:
        print("  Research completed successfully")
        print(f"  Checkpoint saved: {final_checkpoint}")
    
    return brain, world, agent

def run_developmental_research(brain, world, agent):
    """Research on developmental processes."""
    
    for stage in range(5):
        print(f"  Testing developmental stage: {stage}")
        
        brain.setDevelopmentalStage(stage)
        agent.updateDevelopment(0.1)
        
        # Enable/disable systems based on stage
        if stage <= 1:
            agent.enableCuriosity(True)
            agent.enableStructuralPlasticity(True)
        else:
            agent.enableCuriosity(False)
            agent.enableDevelopment(True)
        
        # Run simulation
        for step in range(200):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            # Custom reward for developmental research
            reward = calculate_development_reward(world, agent)
            agent.applyRewardModulation(reward, 0.0)
        
        print(f"    Developmental metrics: {agent.getCuriosityLevel():.3f}, {agent.getNoveltyLevel():.3f}")

def run_social_learning_research(brain, world, agent):
    """Research on social learning and interaction."""
    
    print("  Testing social learning mechanisms...")
    
    # Create multiple agents (simulating social context)
    agents = [agent]  # Start with single agent
    for i in range(2):  # Add 2 "social partners"
        partner_agent = pynlm.createAgentBrain(brain)
        partner_agent.initialize(world)
        agents.append(partner_agent)
    
    # Run social interaction simulation
    for step in range(300):
        # Each agent processes shared percept
        shared_percept = world.getSensoryPercept()
        
        for i, social_agent in enumerate(agents):
            # Agents can learn from each other
            if i > 0:  # Only social partners (not focal agent)
                # Social learning: copy successful behaviors
                social_reward = social_agent.getNeuromodulationLevel()
                agent.applyRewardModulation(social_reward * 0.5, 0.0)
            
            # Process shared percept
            social_agent.processSensoryInput(shared_percept)
            social_agent.step(step)
        
        # Continue with normal social learning
        agent.step(step)
    
    print("  Social learning research complete")

def run_neuromodulation_research(brain, world, agent):
    """Research on neuromodulation dynamics."""
    
    print("  Testing neuromodulation dynamics...")
    
    # Record baseline neuromodulation levels
    baseline_dopamine = agent.getNeuromodulationLevel()
    baseline_curiosity = agent.getCuriosityLevel()
    baseline_novelty = agent.getNoveltyLevel()
    
    # Manipulate neuromodulation
    print(f"  Baseline - Dopamine: {baseline_dopamine:.3f}, Curiosity: {baseline_curiosity:.3f}")
    
    # Test different neuromodulation levels
    for test in range(3):
        # Set specific neuromodulation levels
        agent.applyRewardModulation(1.0, 0.0)  # High reward
        print(f"    Test {test + 1}: Dopamine after high reward: {agent.getNeuromodulationLevel():.3f}")
        
        # Reset and test curiosity
        agent.applyRewardModulation(0.0, 0.0)
        print(f"    Test {test + 1}: Curiosity after neutral: {agent.getCuriosityLevel():.3f}")
```

## Complete Research Example

Here's a complete research script that combines multiple advanced techniques:

```python
import pynlm
import numpy as np

def advanced_research_protocol():
    """Complete advanced research protocol."""
    
    print("=== Advanced NLM Research Protocol ===")
    
    # 1. Create optimized research brain
    print("\n1. Setting up research brain...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.12)
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.018)
    
    research_brain = pynlm.createBrain(config)
    research_brain.initialize()
    
    # 2. Create world with research stimuli
    print("\n2. Setting up research environment...")
    research_world = pynlm.createSimpleWorld()
    research_world.configure(
        width(40.0),
        height(40.0),
        visionWidth=16,
        visionHeight=16
    )
    
    # Add research-specific objects
    research_world.addObject(pynlm.WorldObject(
        x=20.0, y=20.0,
        type=pynlm.WorldObjectType.Resource,
        value=20.0
    ))
    
    # 3. Create research agent
    print("\n3. Creating research agent...")
    research_agent = pynlm.createAgentBrain(research_brain)
    research_agent.initialize(research_world)
    
    # Enable research-specific learning
    research_agent.enableRewardModulation(True)
    research_agent.enableCuriosity(True)
    research_agent.enableDevelopment(True)
    research_agent.enableStructuralPlasticity(True)
    
    # 4. Run research phases
    print("\n4. Running research phases...")
    
    # Phase 1: Baseline measurement
    print("   Phase 1: Baseline measurement...")
    run_baseline_phase(research_brain, research_world, research_agent)
    
    # Phase 2: Developmental research
    print("   Phase 2: Developmental research...")
    run_developmental_research_phase(research_brain, research_world, research_agent)
    
    # Phase 3: Social learning research
    print("   Phase 3: Social learning research...")
    run_social_research_phase(research_brain, research_world, research_agent)
    
    # Phase 4: Neuromodulation research
    print("   Phase 4: Neuromodulation research...")
    run_neuromodulation_research_phase(research_brain, research_world, research_agent)
    
    # 5. Save research results
    print("\n5. Saving research results...")
    research_brain.save("research_results.bin")
    
    # 6. Generate research report
    print("\n6. Generating research report...")
    report = generate_research_report(research_brain, research_world, research_agent)
    
    print(f"\n=== Research Complete ===")
    print(f"Research brain saved to: research_results.bin")
    print(f"Report generated with {len(report)} metrics")
    
    return research_brain, research_world, research_agent, report

def run_baseline_phase(brain, world, agent):
    """Baseline measurement phase."""
    
    baseline_reward = 0.0
    baseline_curiosity = 0.0
    baseline_novelty = 0.0
    
    for step in range(200):
        # Normal operation
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, step * 0.1)
        
        # Collect baseline metrics
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        baseline_reward += reward
        baseline_curiosity += agent.getCuriosityLevel()
        baseline_novelty += agent.getNoveltyLevel()
    
    print(f"   Baseline metrics:")
    print(f"     Average reward: {baseline_reward/200:.3f}")
    print(f"     Average curiosity: {baseline_curiosity/200:.3f}")
    print(f"     Average novelty: {baseline_novelty/200:.3f}")

def run_developmental_research_phase(brain, world, agent):
    """Developmental research phase."""
    
    for stage in range(3):
        print(f"     Developmental stage {stage}:")
        
        brain.setDevelopmentalStage(stage)
        agent.updateDevelopment(0.1)
        
        # Modify development parameters
        if stage == 0:
            agent.enableCuriosity(True)
            agent.enableStructuralPlasticity(True)
        elif stage == 1:
            agent.enableDevelopment(True)
        else:
            agent.enableCuriosity(False)
        
        # Run developmental simulation
        for step in range(200):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step + 200)
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            if step % 50 == 0:
                print(f"       Stage {stage}, Step {step}: Curiosity={agent.getCuriosityLevel():.3f}")

def run_social_research_phase(brain, world, agent):
    """Social learning research phase."""
    
    print("     Social learning research...")
    
    # Create social partners
    social_agents = []
    for i in range(3):
        partner = pynlm.createAgentBrain(brain)
        partner.initialize(world)
        social_agents.append(partner)
    
    # Run social interaction
    for step in range(300):
        shared_percept = world.getSensoryPercept()
        
        # Shared learning
        for social_agent in social_agents:
            social_agent.processSensoryInput(shared_percept)
            social_agent.step(step)
        
        # Main agent continues
        agent.processSensoryInput(shared_percept)
        agent.step(step)
        
        # Social reinforcement
        social_reward = agent.getNeuromodulationLevel()
        agent.applyRewardModulation(social_reward * 0.3, 0.0)

def run_neuromodulation_research_phase(brain, world, agent):
    """Neuromodulation research phase."""
    
    print("     Neuromodulation research...")
    
    # Test different neuromodulation scenarios
    scenarios = [
        ("high_curiosity", 1.0, 0.5),
        ("low_curiosity", 0.1, 0.1),
        ("balanced", 0.5, 0.5)
    ]
    
    for scenario_name, curiosity_level, novelty_level in scenarios:
        print(f"       Testing {scenario_name}...")
        
        # Modify neuromodulation
        agent.enableCuriosity(True)
        
        for step in range(100):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step + 500)
            
            # Simulate neuromodulation manipulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            
            # Apply custom neuromodulation
            if scenario_name == "high_curiosity":
                agent.applyRewardModulation(reward, 0.8)
            elif scenario_name == "low_curiosity":
                agent.applyRewardModulation(reward, 0.2)
            else:
                agent.applyRewardModulation(reward, 0.5)
            
            if step % 25 == 0:
                print(f"         {scenario_name}: Cur={agent.getCuriosityLevel():.3f}, Nov={agent.getNoveltyLevel():.3f}")

def generate_research_report(brain, world, agent):
    """Generate comprehensive research report."""
    
    report = {
        "brain_metrics": {
            "neuron_count": brain.getTotalNeuronCount(),
            "synapse_count": brain.getTotalSynapseCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "total_spikes": brain.getTotalSpikeCount()
        },
        "agent_metrics": {
            "final_curiosity": agent.getCuriosityLevel(),
            "final_novelty": agent.getNoveltyLevel(),
            "final_reward_prediction": agent.getNeuromodulationLevel(),
            "development_stage": brain.getDevelopmentalStage()
        },
        "world_metrics": {
            "final_position": (world.getAgentBody().x, world.getAgentBody().y),
            "final_energy": world.getAgentBody().energy,
            "final_health": world.getAgentBody().health
        },
        "research_findings": []
    }
    
    # Add research findings based on results
    if agent.getCuriosityLevel() > 0.7:
        report["research_findings"].append(
            "High curiosity levels promote exploratory behavior"
        )
    
    if brain.getAverageFiringRate() > 2.0:
        report["research_findings"].append(
            "Network achieved high firing rates through plasticity"
        )
    
    if agent.getNoveltyLevel() > 0.5:
        report["research_findings"].append(
            "Novelty detection effectively drives learning"
        )
    
    return report

# Run the complete research protocol
if __name__ == "__main__":
    brain, world, agent, report = advanced_research_protocol()
    
    print(f"\n=== Research Protocol Summary ===")
    print(f"Final brain: {brain.getTotalNeuronCount()} neurons")
    print(f"Final agent: Curiosity={agent.getCuriosityLevel():.3f}")
    print(f"Research findings: {len(report['research_findings'])}")
    
    for finding in report["research_findings"]:
        print(f"  - {finding}")
```

## Conclusion

This comprehensive Python API documentation provides everything needed to work with the NLM framework, from simple examples for beginners to advanced research protocols for experienced users. The API is designed to be intuitive yet powerful, supporting everything from basic neural simulations to sophisticated research experiments.

The NLM Python API enables:

- **Simple neural simulations** - Quick brain testing and visualization
- **Complete agent-world interactions** - Full embodied AI simulations
- **Advanced research protocols** - Custom experimental setups
- **Performance optimization** - Efficient large-scale simulations
- **Developmental learning** - Age-dependent learning processes
- **Multi-objective training** - Balanced learning of multiple goals
- **Experience replay** - Stable lifelong learning
- **Neuromodulation research** - Advanced reward and curiosity systems
- **Social learning** - Multi-agent interaction studies
- **Memory system research** - Comprehensive memory studies

The framework is continuously evolving with new features and improvements based on research feedback and user needs.
