# NLM - Advanced Usage Guide for Developers and Researchers

## Overview

This guide provides comprehensive examples and advanced patterns for using NLM beyond the basic usage. It covers configuration customization, advanced learning algorithms, memory systems, neuromodulation, development, and more sophisticated agent behaviors.

## Advanced Configuration

### Custom Brain Configuration

```python
import pynlm
import json

# Create a custom configuration
config = pynlm.createDefaultConfig()

# Customize brain parameters
config.set("brain.neuron_count", 2000)  # More neurons for complex behavior
config.set("brain.synapse_density", 0.15)  # Higher connectivity
config.set("brain.v_thresh", -55.0)  # Different threshold

# Configure plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

### Loading Configuration from File

```python
import pynlm
import os

# Save a custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.learning_rate", 0.001)
config.saveToFile("my_brain_config.json")

# Load configuration from file
config2 = pynlm.createDefaultConfig()
if config2.loadFromFile("my_brain_config.json"):
    print("Configuration loaded successfully")
else:
    print("Using default configuration")

brain = pynlm.createBrain(config2)
brain.initialize()
```

## Advanced Simulation Patterns

### Pattern 1: Learning Agent with Memory

```python
import pynlm
import time

def run_learning_agent(num_steps=1000):
    """Run an agent that learns through experience and memory."""
    
    # Create brain with learning capabilities
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with all subsystems enabled
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    # Initialize agent
    agent.initialize(world)
    
    # Run simulation
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action and execute it
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getEnergyDecayRate()  # Example reward calculation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Agent position: ({agent.getBrain().getAgentBody().x:.1f}, "
                  f"{agent.getBrain().getAgentBody().y:.1f})")
    
    return brain, agent, world

# Run the learning agent
brain, agent, world = run_learning_agent(500)
```

### Pattern 2: Developmental Learning

```python
import pynlm

def run_developmental_simulation():
    """Run a simulation where the brain develops over time."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Starting developmental simulation...")
    print("Development stages:")
    
    # Run simulation with developmental stages
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Check developmental stage changes
        if step % 50 == 0:
            stage = brain.getDevelopmentalStage()
            print(f"  Step {step}: {stage}")
            
            # Different behaviors at different stages
            if stage.name == "CriticalPeriod":
                # Enhanced learning during critical period
                agent.enableStructuralPlasticity(True)
            elif stage.name == "Adult":
                # More stable behavior in adult stage
                agent.enableStructuralPlasticity(False)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.updateDevelopment(0.1)
    
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    return brain

brain = run_developmental_simulation()
```

### Pattern 3: Advanced Memory Systems

```python
import pynlm

def run_memory_agent():
    """Run an agent that uses working memory and episodic memory."""
    
    # Create brain with memory capabilities
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 4000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Create world with resources
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=8, visionWidth=8)
    world.reset()
    
    # Add some resources to the world
    for i in range(10):
        x = i * 2.0 + 1.0
        y = 12.5
        obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
        world.addObject(obj)
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Running memory agent simulation...")
    
    for step in range(300):
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Check for nearby resources
        body = world.getAgentBody()
        percept = world.getSensoryPercept()
        vision = percept.getVision()
        
        # Simple resource detection (in real implementation, would use vision processing)
        if step % 50 == 0:
            print(f"  Step {step}: Position ({body.x:.1f}, {body.y:.1f})")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Memory neurons: {brain.getActiveNeuronCount()}")
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward based on resource collection
        reward = world.getAgentBody().energy * 0.1
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    return brain

brain = run_memory_agent()
```

### Pattern 4: Neuromodulation and Learning

```python
import pynlm

def run_neuromodulation_experiment():
    """Run an experiment demonstrating neuromodulation effects."""
    
    # Create two identical brains for comparison
    config1 = pynlm.createDefaultConfig()
    config2 = pynlm.createDefaultConfig()
    
    # Enable neuromodulation only in brain 1
    config1.set("neuromod.curiosity.enable", True)
    config1.set("neuromod.novelty.enable", True)
    
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    # Create agents
    agent1 = pynlm.createAgentBrain(brain1)
    agent2 = pynlm.createAgentBrain(brain2)
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent1.initialize(world)
    agent2.initialize(world)
    
    agent1.enableCuriosity(True)
    agent1.enableRewardModulation(True)
    
    # agent2 has no neuromodulation for comparison
    
    print("Running neuromodulation experiment...")
    print("Brain 1: With curiosity and reward modulation")
    print("Brain 2: No neuromodulation (baseline)")
    print()
    
    for step in range(100):
        world.update(0.1)
        
        # Both agents see the same world
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent1.processSensoryInput(percept)
        agent2.processSensoryInput(percept)
        
        # Run brain steps
        brain1.step(step)
        brain2.step(step)
        
        # Get actions
        action1 = agent1.decodeMotorCommand()
        action2 = agent2.decodeMotorCommand()
        
        # Execute actions (brain1 gets reward, brain2 doesn't)
        world.applyMotorCommand(action1, world.getSimulationTime())
        
        reward = 1.0 if step % 20 == 0 else 0.1  # Variable reward
        agent1.applyRewardModulation(reward, 0.0)
        
        # Check neuromodulation levels
        if step % 20 == 0:
            print(f"  Step {step}:")
            print(f"    Brain 1 - Curiosity: {agent1.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent1.getNoveltyLevel():.3f}")
            print(f"    Brain 2 - Curiosity: {agent2.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent2.getNoveltyLevel():.3f}")
            print(f"    Brain 1 spikes: {brain1.getTotalSpikeCount()}, "
                  f"Brain 2 spikes: {brain2.getTotalSpikeCount()}")
        
        agent1.updateDevelopment(0.1)
        agent2.updateDevelopment(0.1)
    
    print(f"\nFinal comparison:")
    print(f"Brain 1 (with neuromodulation): {brain1.getTotalSpikeCount()} spikes")
    print(f"Brain 2 (without neuromodulation): {brain2.getTotalSpikeCount()} spikes")
    
    return brain1, brain2

brain1, brain2 = run_neuromodulation_experiment()
```

## Error Handling and Robustness

### Pattern 5: Robust Simulation with Error Handling

```python
import pynlm
import traceback

def run_robust_simulation():
    """Run a simulation with comprehensive error handling."""
    
    brains = []
    agents = []
    worlds = []
    
    try:
        # Create multiple simulation instances
        for i in range(3):
            print(f"Creating simulation instance {i+1}...")
            
            # Create brain with error handling
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 1000 + i * 500)
            
            brain = pynlm.createBrain(config)
            if not brain.initialize():
                print(f"Warning: Failed to initialize brain {i+1}")
                continue
                
            brains.append(brain)
            
            # Create agent
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(pynlm.createSimpleWorld())
            
            agents.append(agent)
            worlds.append(agent.getBrain().getWorld())
        
        print(f"Successfully created {len(brains)} simulation instances")
        
        # Run simulations
        for sim_idx, (brain, agent, world) in enumerate(zip(brains, agents, worlds)):
            print(f"\nRunning simulation {sim_idx + 1}...")
            
            try:
                # Configure world
                world.configure(10 + sim_idx * 5, 10 + sim_idx * 5, 
                             6 + sim_idx * 2, 6 + sim_idx * 2)
                world.reset()
                
                # Enable features
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
                
                # Run simulation
                for step in range(100):
                    try:
                        world.update(0.1)
                        agent.processSensoryInput(world.getSensoryPercept())
                        brain.step(step)
                        
                        action = agent.decodeMotorCommand()
                        world.applyMotorCommand(action, world.getSimulationTime())
                        
                        # Occasionally print progress
                        if step % 25 == 0:
                            print(f"  Simulation {sim_idx+1}, Step {step}: "
                                  f"{brain.getFiringNeuronCount()} firing neurons")
                        
                    except Exception as e:
                        print(f"  Error at step {step}: {e}")
                        break
                
            except Exception as e:
                print(f"Error in simulation {sim_idx + 1}: {e}")
                traceback.print_exc()
        
        # Compare results
        print(f"\nSimulation Results:")
        for i, brain in enumerate(brains):
            print(f"  Simulation {i+1}: {brain.getTotalNeuronCount()} neurons, "
                  f"{brain.getTotalSpikeCount()} spikes")
        
    except Exception as e:
        print(f"Fatal error in simulation: {e}")
        traceback.print_exc()
    
    return brains

brains = run_robust_simulation()
```

## Performance Optimization

### Pattern 6: Performance Monitoring and Optimization

```python
import pynlm
import time
import statistics

def monitor_simulation_performance():
    """Monitor and optimize simulation performance."""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent.initialize(world)
    
    print("Performance Monitoring Simulation")
    print("=" * 40)
    
    step_times = []
    spike_counts = []
    firing_rates = []
    
    # Run performance test
    for step in range(200):
        start_time = time.time()
        
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        step_time = end_time - start_time
        step_times.append(step_time)
        
        firing_rate = brain.getFiringNeuronCount() / brain.getTotalNeuronCount()
        firing_rates.append(firing_rate)
        
        spike_counts.append(brain.getTotalSpikeCount())
        
        # Print progress every 50 steps
        if step % 50 == 0:
            avg_time = statistics.mean(step_times[-50:])
            print(f"  Step {step}: {avg_time*1000:.2f}ms/step, "
                  f"{firing_rate*100:.1f}% firing rate")
    
    # Performance summary
    print(f"\nPerformance Summary:")
    print(f"  Average step time: {statistics.mean(step_times)*1000:.2f}ms")
    print(f"  Median step time: {statistics.median(step_times)*1000:.2f}ms")
    print(f"  Min step time: {min(step_times)*1000:.2f}ms")
    print(f"  Max step time: {max(step_times)*1000:.2f}ms")
    print(f"  Total simulation time: {sum(step_times):.2f}s")
    print(f"  Steps per second: {len(step_times)/sum(step_times):.1f}")
    print(f"  Average firing rate: {statistics.mean(firing_rates)*100:.1f}%")
    print(f"  Final spike count: {brain.getTotalSpikeCount()}")
    
    # Determine performance tier
    avg_step_time_ms = statistics.mean(step_times) * 1000
    if avg_step_time_ms < 10:
        performance = "Excellent"
    elif avg_step_time_ms < 50:
        performance = "Good"
    elif avg_step_time_ms < 100:
        performance = "Acceptable"
    else:
        performance = "Needs Optimization"
    
    print(f"  Performance rating: {performance}")
    
    return step_times, spike_counts

step_times, spike_counts = monitor_simulation_performance()
```

## Advanced Configuration Options

### Complete Configuration Guide

```python
import pynlm

def explore_all_configurations():
    """Demonstrate all available configuration options."""
    
    config = pynlm.createDefaultConfig()
    
    print("Available Configuration Categories:")
    print("=" * 40)
    
    # Brain configuration
    print("\n1. Brain Configuration:")
    brain_keys = [k for k in config.getKeys() if k.startswith("brain.")]
    for key in sorted(brain_keys)[:5]:  # Show first 5 as examples
        print(f"   {key}")
    
    # Plasticity configuration
    print("\n2. Plasticity Configuration:")
    plastic_keys = [k for k in config.getKeys() if k.startswith("plasticity.")]
    for key in sorted(plastic_keys):
        print(f"   {key}")
    
    # Neuromodulation configuration
    print("\n3. Neuromodulation Configuration:")
    neuromod_keys = [k for k in config.getKeys() if k.startswith("neuromod.")]
    for key in sorted(neuromod_keys):
        print(f"   {key}")
    
    # Development configuration
    print("\n4. Development Configuration:")
    dev_keys = [k for k in config.getKeys() if k.startswith("development.")]
    for key in sorted(dev_keys):
        print(f"   {key}")
    
    # Memory configuration
    print("\n5. Memory Configuration:")
    mem_keys = [k for k in config.getKeys() if k.startswith("memory.")]
    for key in sorted(mem_keys):
        print(f"   {key}")
    
    # Prediction configuration
    print("\n6. Prediction Configuration:")
    pred_keys = [k for k in config.getKeys() if k.startswith("prediction.")]
    for key in sorted(pred_keys):
        print(f"   {key}")
    
    # Cognitive configuration
    print("\n7. Cognitive Configuration:")
    cog_keys = [k for k in config.getKeys() if k.startswith("cognition.")]
    for key in sorted(cog_keys):
        print(f"   {key}")
    
    print(f"\nTotal configuration options: {len(config.getKeys())}")
    
    # Show current configuration summary
    print(f"\nCurrent Configuration Summary:")
    print(config.summary())
    
    return config

config = explore_all_configurations()
```

## Advanced API Usage

### Pattern 7: Custom Action Types

```python
import pynlm

def create_custom_actions():
    """Demonstrate custom action creation and usage."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create custom actions
    actions = []
    
    # Standard action
    move_action = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 0.0, 0.0])
    actions.append(move_action)
    
    # Rotation action
    turn_action = pynlm.Action(pynlm.ActionType.TurnLeft, [0.5])
    actions.append(turn_action)
    
    # Interaction action
    interact_action = pynlm.Action(pynlm.ActionType.Interact, [0.0, 1.0, 0.0])
    actions.append(interact_action)
    
    # Custom action with parameters
    custom_action = pynlm.Action(pynlm.ActionType.Custom, 
                                [0.1, 0.2, 0.3, 0.4, 0.5])
    actions.append(custom_action)
    
    print("Created Custom Actions:")
    for i, action in enumerate(actions):
        print(f"  {i+1}. {action.getName()} (Type: {action.getType().name})")
        print(f"     Parameters: {action.getParameters()}")
    
    return actions

actions = create_custom_actions()
```

### Pattern 8: World Object Manipulation

```python
import pynlm

def advanced_world_manipulation():
    """Demonstrate advanced world object manipulation."""
    
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 6, 6)
    world.reset()
    
    print("Advanced World Object Manipulation")
    print("=" * 40)
    
    # Create various objects
    objects = []
    
    # Add resources
    resource1 = pynlm.WorldObject(3.0, 4.0, pynlm.WorldObjectType.Resource, value=2.0)
    objects.append(resource1)
    
    resource2 = pynlm.WorldObject(8.0, 12.0, pynlm.WorldObjectType.Resource, value=1.5)
    objects.append(resource2)
    
    # Add hazards
    hazard1 = pynlm.WorldObject(10.0, 8.0, pynlm.WorldObjectType.Hazard, value=0.5)
    objects.append(hazard1)
    
    # Add walls
    wall1 = pynlm.WorldObject(2.0, 2.0, pynlm.WorldObjectType.Wall, radius=0.8)
    objects.append(wall1)
    
    wall2 = pynlm.WorldObject(12.0, 10.0, pynlm.WorldObjectType.Wall, radius=1.0)
    objects.append(wall2)
    
    # Add markers
    marker1 = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Marker, radius=0.3)
    objects.append(marker1)
    
    # Add all objects to world
    for obj in objects:
        world.addObject(obj)
    
    print("Objects added to world:")
    for obj in objects:
        print(f"  {obj.type.name} at ({obj.x:.1f}, {obj.y:.1f}), "
              f"radius: {obj.radius:.1f}, active: {obj.active}")
    
    # Demonstrate object manipulation
    print(f"\nObject manipulation examples:")
    
    # Modify object properties
    resource1.value = 3.0
    resource1.active = False
    print(f"Modified resource1: value={resource1.value}, active={resource1.active}")
    
    # Check positions
    body = world.getAgentBody()
    print(f"Agent body: ({body.x:.1f}, {body.y:.1f}), "
          f"energy: {body.energy:.1f}, health: {body.health:.1f}")
    
    # Test world validation
    test_positions = [(1.0, 1.0), (14.0, 14.0), (3.0, 4.0), (8.0, 12.0)]
    for x, y in test_positions:
        is_valid = world.isValidPosition(x, y)
        print(f"Position ({x:.1f}, {y:.1f}): {'Valid' if is_valid else 'Invalid'}")
    
    # Demonstrate object removal
    if world.removeObject(3.0, 4.0):
        print(f"\nSuccessfully removed object at (3.0, 4.0)")
    else:
        print(f"\nFailed to remove object at (3.0, 4.0)")
    
    # Show remaining objects
    print(f"\nRemaining objects after removal:")
    percept = world.getSensoryPercept()
    vision = percept.getVision()
    print(f"Vision dimensions: {vision.getWidth()}x{vision.getHeight()}")
    
    return world

world = advanced_world_manipulation()
```

## Summary

This advanced usage guide provides comprehensive examples for:

1. **Configuration Customization** - Creating and loading custom brain configurations
2. **Complex Simulations** - Learning agents with memory, development, and neuromodulation
3. **Error Handling** - Robust simulation with comprehensive error management
4. **Performance Monitoring** - Optimizing and monitoring simulation performance
5. **API Exploration** - Understanding all available configuration options
6. **Custom Implementation** - Creating custom actions and world objects
7. **Advanced Patterns** - Neuromodulation experiments and developmental learning

These patterns enable developers and researchers to create sophisticated NLM simulations for various research applications and experimental setups.

---

## Quick Reference for Advanced Users

| Advanced Pattern | Use Case | Key Features |
|-----------------|----------|-------------|
| Learning Agent | Research on learning algorithms | Reward modulation, plasticity, memory |
| Developmental Learning | Study brain development | Developmental stages, maturation |
| Neuromodulation | Study motivation and exploration | Curiosity, novelty, reward |
| Performance Monitoring | Optimize simulation efficiency | Timing, statistics, profiling |
| Custom Actions | Implement specialized behaviors | Custom action types, parameters |
| World Manipulation | Complex environment simulation | Object management, validation |
| Error Handling | Production-ready simulations | Robust error management |

These advanced patterns build upon the basic NLM API to provide the flexibility and power needed for complex neural simulation research and applications.

---

## Further Reading

For more advanced topics:

1. **Phase 6 Integration** - Read `docs/PHASE6_FINAL_AUDIT.md` for complete system integration details
2. **Architecture Deep Dive** - Read `docs/ARCHITECTURE.md` for technical implementation details
3. **Scientific Background** - Read `docs/SCIENCE.md` for theoretical foundations
4. **Experiment Framework** - Read `docs/EXPERIMENTS.md` for experimental design
5. **Phase 5 Performance** - Review `docs/PHASE5_PERFORMANCE_AUDIT.md` for optimization insights

The advanced patterns in this guide provide a foundation for creating sophisticated NLM simulations that can model complex brain functions and behaviors.
