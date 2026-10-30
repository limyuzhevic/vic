# Enhanced NLM Python API Documentation

This document provides comprehensive documentation for the enhanced NLM Python API, which significantly improves upon the original bindings with Pythonic interfaces and advanced features.

## Overview

The enhanced NLM Python API provides a more intuitive and powerful interface for working with neural simulations. It addresses many of the limitations in the original bindings by:

- Providing Pythonic abstractions and fluent interfaces
- Implementing context managers for resource management
- Adding comprehensive error handling with custom exception classes
- Offering factory methods for common setup patterns
- Including statistics collection and simulation monitoring
- Supporting advanced simulation control features

## Key Improvements

### 1. Pythonic Configuration (PythonConfig)

#### Builder Pattern
```python
import pynlm_enhanced as nlm

# Simple configuration
config = nlm.createConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Fluent interface for complex configurations
config = (nlm.createConfig()
    .withBrain(2000, density=0.15)
    .withLearning(stdp_rate=0.001, hebbian_rate=0.0005)
    .withNeuromodulation()
    .withDevelopment())

# Preset configurations
minimal_config = nlm.PythonConfig.createMinimal()
complex_config = nlm.PythonConfig.createComplex()
```

#### Type-Safe Access
```python
# Type-safe configuration access with error handling
try:
    neuron_count = config.getInt("brain.neuron_count")
    learning_rate = config.getDouble("plasticity.stdp.learning_rate")
    enable_curiosity = config.getBool("neuromod.curiosity.enable")
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
```

### 2. Enhanced Brain Interface (PythonBrain)

#### Basic Usage
```python
import pynlm_enhanced as nlm

# Create brain with configuration
config = nlm.createConfig()
config.withBrain(1000).withLearning().withNeuromodulation()
brain = nlm.createBrain(config)
brain.initialize()

# Run simulation with statistics
stats = brain.runSimulation(1000, collect_detailed_stats=True)
print(f"Steps completed: {stats['steps_completed']}")
print(f"Final neuron count: {stats['final_neuron_count']}")
print(f"Final firing rate: {stats['final_average_firing_rate']:.3f}")
```

#### Context Manager Support
```python
# Automatic resource management with context managers
with brain.createSimulationContext() as sim:
    sim.runSteps(500)
    print("Simulation completed automatically cleaned up")

# Advanced simulation with context
brain.reset()
with brain.createSimulationContext() as sim:
    sim.runSteps(100)
    # Brain state is automatically reset at context exit
```

#### Learning Control
```python
# Enable all learning systems
brain.enableAllLearning(True)

# Set specific learning rates
brain.setLearningRates(stdp_rate=0.001, hebbian_rate=0.0005)

# Development
brain.develop(timestep=0.1)
```

### 3. Enhanced Agent Interface (PythonAgent)

#### Basic Agent Setup
```python
import pynlm_enhanced as nlm

# Create complete simulation setup
brain, agent, world = nlm.NLMFactory.createSimpleSimulation()

# Run episode with automatic reset
results = agent.runEpisode(world, 500)
print(f"Episode completed: {results['steps_completed']} steps")
print(f"Total reward: {results['total_reward']:.3f}")
```

#### Training Loop
```python
import pynlm_enhanced as nlm

# Create training setup
brain, agent, world = nlm.NLMFactory.createTrainingSetup(
    episodes=100,
    steps_per_episode=100
)

# Run training with detailed statistics
training_results = agent.runTraining(world, 50)

# Process results
for episode_result in training_results:
    episode = episode_result['episode']
    total_reward = episode_result['total_reward']
    print(f"Episode {episode}: Total reward = {total_reward:.3f}")
```

#### Subsystem Control
```python
# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Get agent statistics
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
```

### 4. Enhanced World Interface (PythonWorld)

#### World Configuration
```python
world = nlm.createWorld()
world.configure(width=20.0, height=20.0, vision_width=8, vision_height=8)
world.setAgentStart(10.0, 10.0)
world.setSeed(42)  # For reproducibility
```

#### Command Execution
```python
# Apply motor command with result handling
world = nlm.createWorld()
world.configure(20.0, 20.0, 8, 8)

# Set up agent and brain
config = nlm.createConfig().withBrain(500)
brain = nlm.createBrain(config)
brain.initialize()
agent = nlm.createAgent(brain)
agent.initialize(world.getInternalWorld())

# Run simulation loop
for step in range(100):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes input
    brain.step(step)
    
    # Get and apply action
    action = agent.getAction()
    result = world.applyCommand(action, world.getSimulationTime())
    
    # Apply reward
    agent.applyReward(result['reward'])
    agent.updateDevelopment(0.1)
    
    # Optional: Print progress
    if step % 10 == 0:
        print(f"Step {step}: Reward = {result['reward']:.3f}")
```

### 5. Phase 6 Integration

#### Phase 6 Experiment Runner
```python
import pynlm_enhanced as nlm

# Create Phase 6 experiment
experiment = nlm.createPhase6Experiment()

# Run Phase 6 integration test
results = experiment.runPhase6Test(1000, detailed_stats=True)

# Analyze results
print(f"Test type: {results['test_type']}")
print(f"Steps completed: {results['steps_completed']}")
print(f"Final development stage: {results['final_development_stage']}")
print(f"Final curiosity level: {results['final_curiosity_level']:.3f}")

if 'neuron_counts' in results:
    import matplotlib.pyplot as plt
    plt.figure(figsize=(12, 8))
    
    plt.subplot(2, 2, 1)
    plt.plot(results['neuron_counts'])
    plt.title('Firing Neuron Count')
    plt.xlabel('Step')
    plt.ylabel('Count')
    
    plt.subplot(2, 2, 2)
    plt.plot(results['firing_rates'])
    plt.title('Average Firing Rate')
    plt.xlabel('Step')
    plt.ylabel('Hz')
    
    plt.subplot(2, 2, 3)
    plt.plot(results['curiosity_levels'])
    plt.title('Curiosity Level')
    plt.xlabel('Step')
    plt.ylabel('Level')
    
    plt.subplot(2, 2, 4)
    plt.plot(results['novelty_levels'])
    plt.title('Novelty Level')
    plt.xlabel('Step')
    plt.ylabel('Level')
    
    plt.tight_layout()
    plt.show()
```

#### Checkpoint Management
```python
experiment = nlm.createPhase6Experiment()

# Save checkpoint
success = experiment.saveCheckpoint("phase6_checkpoint.bin")
if success:
    print("Checkpoint saved successfully")

# Run simulation
results = experiment.runPhase6Test(500)

# Save intermediate checkpoint
experiment.saveCheckpoint("phase6_checkpoint_500.bin")

# Load checkpoint back
new_experiment = nlm.createPhase6Experiment()
new_experiment.loadCheckpoint("phase6_checkpoint_500.bin")
print("Checkpoint loaded successfully")
```

## Advanced Examples

### Example 1: Complete Agent Training
```python
import pynlm_enhanced as nlm
import time

def train_agent(episodes=100, steps_per_episode=100):
    """Complete agent training with monitoring"""
    start_time = time.time()
    
    # Create training setup
    brain, agent, world = nlm.NLMFactory.createTrainingSetup(
        episodes=episodes,
        steps_per_episode=steps_per_episode
    )
    
    # Training loop
    episode_results = []
    for episode in range(episodes):
        episode_start = time.time()
        
        world.reset()
        agent.reset()
        
        episode_reward = 0
        for step in range(steps_per_episode):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getPercept()
            agent.processSensoryInput(percept)
            
            # Brain processes
            brain.step(step)
            
            # Get and apply action
            action = agent.getAction()
            result = world.applyCommand(action, world.getSimulationTime())
            
            # Apply learning
            agent.applyReward(result['reward'])
            agent.updateDevelopment(0.1)
            
            episode_reward += result['reward']
        
        episode_time = time.time() - episode_start
        
        episode_result = {
            'episode': episode,
            'total_reward': episode_reward,
            'steps_per_episode': steps_per_episode,
            'episode_time': episode_time,
            'avg_reward_per_step': episode_reward / steps_per_episode,
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel()
        }
        
        episode_results.append(episode_result)
        
        # Print progress
        if (episode + 1) % 10 == 0:
            avg_reward = sum(r['total_reward'] for r in episode_results[-10:]) / 10
            print(f"Episode {episode + 1}/{episodes}, "
                  f"Avg Reward: {avg_reward:.3f}, "
                  f"Time: {episode_time:.2f}s")
    
    total_time = time.time() - start_time
    
    # Return training summary
    training_summary = {
        'total_episodes': episodes,
        'total_steps': episodes * steps_per_episode,
        'total_time': total_time,
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel(),
        'final_prediction_error': agent.getPredictionError(),
        'episode_results': episode_results
    }
    
    return training_summary

# Run training
training_results = train_agent(episodes=50, steps_per_episode=100)
print(f"Training completed in {training_results['total_time']:.2f}s")
print(f"Final curiosity level: {training_results['final_curiosity']:.3f}")
```

### Example 2: World Navigation Agent
```python
import pynlm_enhanced as nlm

def create_navigating_agent():
    """Create an agent that navigates a simple world"""
    # Create custom world with obstacles
    world = nlm.createWorld()
    world.configure(width=50.0, height=50.0, vision_width=10, vision_height=10)
    world.setAgentStart(5.0, 5.0)
    
    # Add some obstacles
    for x in range(20, 30):
        for y in range(20, 30):
            if x == y:  # Diagonal wall
                world.addObject(nlm.WorldObject(x, y, nlm.WorldObjectType.Wall, 1.0, 1.0))
    
    # Create agent with navigation capabilities
    config = nlm.createConfig()
    config.withBrain(800).withLearning(0.001, 0.0005).withNeuromodulation()
    
    brain = nlm.createBrain(config)
    agent = nlm.createAgent(brain)
    
    agent.initialize(world.getInternalWorld())
    agent.enableAllLearning(True)
    agent.enableCuriosity(True)
    
    return brain, agent, world

def simulate_navigation(brain, agent, world, max_steps=1000):
    """Simulate agent navigation"""
    episode_rewards = []
    collision_count = 0
    
    for step in range(max_steps):
        # Update world
        world.update(0.1)
        
        # Get percept
        percept = world.getPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Get action
        action = agent.getAction()
        result = world.applyCommand(action, world.getSimulationTime())
        
        # Apply learning
        agent.applyReward(result['reward'])
        agent.updateDevelopment(0.1)
        
        # Track statistics
        if result['success']:
            episode_rewards.append(result['reward'])
        else:
            collision_count += 1
        
        # Optional: Stop if agent is stuck
        if step > 100 and len(episode_rewards) == 0:
            break
    
    return {
        'total_steps': step + 1,
        'total_reward': sum(episode_rewards),
        'average_reward': sum(episode_rewards) / len(episode_rewards) if episode_rewards else 0,
        'collision_count': collision_count,
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel()
    }

# Create and simulate navigating agent
brain, agent, world = create_navigating_agent()
results = simulate_navigation(brain, agent, world, max_steps=500)

print(f"Navigation simulation results:")
print(f"  Steps completed: {results['total_steps']}")
print(f"  Total reward: {results['total_reward']:.3f}")
print(f"  Average reward: {results['average_reward']:.3f}")
print(f"  Collisions: {results['collision_count']}")
print(f"  Final curiosity: {results['final_curiosity']:.3f}")
```

## Migration Guide

### From Original NLM to Enhanced NLM

#### Configuration
```python
# OLD WAY (Original NLM)
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", True)

# NEW WAY (Enhanced NLM)
config = nlm.createConfig()
config.withBrain(1000).withLearning()
```

#### Brain Usage
```python
# OLD WAY
brain = pynlm.createBrain(config)
brain.initialize()
for i in range(100):
    brain.step(i)

# NEW WAY
brain = nlm.createBrain(config)
brain.initialize()
stats = brain.runSimulation(100, collect_detailed_stats=True)
```

#### Agent Usage
```python
# OLD WAY
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()

# NEW WAY
agent = nlm.createAgent(brain)
agent.initialize(world.getInternalWorld())
agent.processSensoryInput(percept)
action = agent.getAction()
```

## Error Handling

The enhanced API provides comprehensive error handling:

```python
import pynlm_enhanced as nlm

try:
    config = nlm.createConfig()
    config.set("brain.neuron_count", 1000)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    stats = brain.runSimulation(100)
    
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.ValidationError as e:
    print(f"Validation error: {e}")
except nlm.NLMException as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Tips

1. **Reuse Configurations**: Create configuration templates for common scenarios
2. **Batch Operations**: Use context managers for automatic resource management
3. **Statistics Collection**: Enable detailed statistics only when needed
4. **Memory Management**: Use `del` or context managers for object cleanup
5. **Parallel Processing**: Consider running multiple simulations in separate processes

## Best Practices

1. **Always initialize brains before use**
2. **Use context managers for resource management**
3. **Validate configuration before creating brains**
4. **Collect statistics early in simulation for better monitoring**
5. **Use preset configurations for common use cases**
6. **Handle exceptions gracefully in production code**
7. **Test with small simulations before scaling up**
8. **Monitor curiosity and novelty levels for exploration balance**

## Conclusion

The enhanced NLM Python API provides a significant improvement over the original bindings, offering:

- Pythonic interfaces with fluent APIs
- Comprehensive error handling and validation
- Context managers for resource management
- Factory methods for common setup patterns
- Statistics collection and monitoring
- Better documentation and examples
- Preset configurations for different use cases

This makes it easier to get started with NLM while providing advanced users with powerful tools for complex simulations and research.

For more advanced usage and research applications, refer to the original NLM documentation and the Phase 6 integration materials.
