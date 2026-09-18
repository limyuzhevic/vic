# NLM Python Usage Guide for Advanced Users

This guide provides comprehensive documentation for using the NLM Python bindings at an advanced level, including:

## Advanced API Reference

### Core Brain Operations

#### Brain Creation and Configuration
```python
import pynlm

# Method 1: Quick brain creation (recommended for most users)
brain = pynlm.createQuickBrain()

# Method 2: Full control with custom configuration
config = pynlm.loadConfig("config.json")  # Load from JSON/YAML
brain = pynlm.createBrain(config)

# Method 3: Advanced configuration with pre-sets
config = pynlm.createAdvancedConfig()  # High-performance settings
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()
```

#### Complex Agent Setup
```python
# Create complete agent environment with custom settings
brain, world, agent = pynlm.createQuickAgent(
    width=30, 
    height=30,
    visionWidth=12, 
    visionHeight=12
)

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

### Learning and Simulation Patterns

#### Pattern 1: Learning Agent with Environment Interaction
```python
def run_learning_agent_simulation(num_steps=5000):
    """Run a complete learning agent with environment interaction."""
    
    # Setup agent environment
    brain, world, agent = pynlm.createQuickAgent(25, 25, 10, 10)
    
    # Add world objects (resources, hazards, etc.)
    from pynlm import WorldObject, WorldObjectType
    
    # Add resource
    resource = WorldObject(5, 5, WorldObjectType.Resource, value=10.0)
    world.addObject(resource)
    
    # Add hazard  
    hazard = WorldObject(20, 20, WorldObjectType.Hazard, value=5.0)
    world.addObject(hazard)
    
    # Run learning simulation
    for step in range(num_steps):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes and decides
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning from experience
        if world.getSensoryPercept().getInternal().size() > 0:
            reward = world.getSensoryPercept().getInternal()[0]
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 500 == 0:
            print(f"Step {step}: "
                  f"{brain.getFiringNeuronCount()} firing, "
                  f"{brain.getTotalSpikeCount()} total spikes, "
                  f"curiosity: {agent.getCuriosityLevel():.2f}")

# Run the simulation
run_learning_agent_simulation(10000)
```

#### Pattern 2: Complex Agent with Multiple Features
```python
def run_complex_experiment(num_steps=20000):
    """Run complex experiments with advanced features."""
    
    # Create complex agent with enhanced settings
    brain = pynlm.createQuickBrain()
    
    # Create larger world with better sensors
    brain, world, agent = pynlm.createQuickAgent(
        width=50, height=50, 
        visionWidth=16, visionHeight=16
    )
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)  # Synapses grow/break
    agent.enableDevelopment(True)           # Brain matures
    agent.enableCuriosity(True)            # Explores novel stimuli
    
    print("=== Complex Experiment Started ===")
    print(f"Brain: {brain.getTotalNeuronCount()} neurons, "
          f"{brain.getTotalSynapseCount()} synapses")
    print(f"World: {world.getWidth()}x{world.getHeight()}")
    print(f"Learning: Reward, Plasticity, Development, Curiosity")
    print()
    
    # Track experiment metrics
    metrics = {
        'total_spikes': 0,
        'max_firing': 0,
        'curiosity_history': [],
        'novelty_history': [],
        'reward_history': []
    }
    
    for step in range(num_steps):
        # Update world dynamics
        world.update(0.05)
        
        # Get current perception
        percept = world.getSensoryPercept()
        
        # Process sensory information
        agent.processSensoryInput(percept)
        
        # Brain computes next action
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Execute action in environment
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from consequences
        if percept.getInternal().size() > 0:
            reward = percept.getInternal()[0]
            predicted = 0.0  # Could implement prediction system
            
            agent.applyRewardModulation(reward, predicted)
            metrics['reward_history'].append(reward)
            
            agent.updateDevelopment(0.05)
        
        # Track curiosity and novelty
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        metrics['curiosity_history'].append(curiosity)
        metrics['novelty_history'].append(novelty)
        
        # Update spike counts
        firing = brain.getFiringNeuronCount()
        spikes = brain.getTotalSpikeCount()
        metrics['total_spikes'] = spikes
        metrics['max_firing'] = max(metrics['max_firing'], firing)
        
        # Print progress every 1000 steps
        if step % 1000 == 0:
            avg_curiosity = sum(metrics['curiosity_history'][-1000:]) / 1000
            print(f"Step {step:5d}: "
                  f"{firing:4d} firing, {spikes:6d} total, "
                  f"curiosity: {curiosity:.3f}, novelty: {novelty:.3f}, "
                  f"avg curiosity: {avg_curiosity:.3f}")
    
    print()
    print("=== Experiment Complete ===")
    print(f"Total spikes generated: {metrics['total_spikes']}")
    print(f"Maximum simultaneous firing: {metrics['max_firing']}")
    print(f"Final curiosity level: {metrics['curiosity_history'][-1]:.3f}")
    print(f"Final novelty level: {metrics['novelty_history'][-1]:.3f}")
    print(f"Total reward received: {sum(metrics['reward_history']):.2f}")
    
    return metrics
```

### File I/O and Persistence

#### Save/Load Brain States
```python
# Save brain state (checkpoint)
brain.save("my_checkpoint.bin")
print("Brain saved to checkpoint")

# Load brain state
brain.load("my_checkpoint.bin")
print("Brain loaded from checkpoint")

# Save configuration
config = pynlm.createDefaultConfig()
config.saveToFile("my_config.json")

# Load configuration  
config = pynlm.loadConfig("my_config.json")
```

#### Advanced Configuration Management
```python
# Create configuration with default values
config = pynlm.createDefaultConfig()

# Modify specific parameters
config.set("brain.neuron_count", 3000)
config.set("brain.synapse_density", 0.12)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.dopamine.scale", 1.5)
config.set("development.enable", True)

# Save custom configuration
config.saveToFile("experiment_config.json")

# Load and verify configuration
config = pynlm.loadConfig("experiment_config.json")
print(config.summary())  # Print configuration details
```

### System Information and Debugging

#### Get System Information
```python
# Get comprehensive system information
info = pynlm.getSystemInfo()
print(info)

# This includes:
# - NLM Python Bindings version
# - Build configuration (C++20 with Pybind11)
# - Phase information (Final Integration)
# - Available features
```

#### Debug and Troubleshooting
```python
# Print detailed help
pynlm.printHelp()

# Run Phase 2 demo (LIF neuron dynamics)
demo_brain = pynlm.runPhase2Demo()

# Create and run simple test simulation
neurons, spikes = pynlm.runSilentSimulation(100)
print(f"Test: {neurons} neurons, {spikes} spikes")
```

### Utility Functions

#### Type and System Information
```python
# Get all available neuron types
neuron_types = pynlm.getNeuronTypes()
print("Available neuron types:", neuron_types)

# Get all action types  
action_types = pynlm.getActionTypes()
print("Available action types:", action_types)

# Get world object types
object_types = pynlm.getWorldObjectTypes()
print("Available world object types:", object_types)
```

#### Quick Test Functions
```python
# Create test environments quickly
# Small world for testing
brain1, world1, agent1 = pynlm.createQuickAgent(10, 10, 4, 4)

# Medium world for experimentation  
brain2, world2, agent2 = pynlm.createQuickAgent(20, 20, 8, 8)

# Large world for advanced experiments
brain3, world3, agent3 = pynlm.createQuickAgent(40, 40, 16, 16)

# Run learning experiments
pynlm.runLearningSimulation(1000)   # Basic learning
pynlm.runLearningSimulation(5000, verbose=True)  # Extended learning with verbose output
```

### Advanced Examples

#### Example 1: Multi-Agent Environment
```python
def run_multi_agent_simulation(num_agents=3, steps=2000):
    """Run simulation with multiple agents."""
    
    # Create world for multiple agents
    brain, world, agent = pynlm.createQuickAgent(60, 40, 12, 8)
    
    # Add multiple agents with different starting positions
    agents = []
    for i in range(num_agents):
        # Create separate brain for each agent
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent interface
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Set starting position for each agent
        world.setAgentStart(10 + i*15, 20 + i*5)
        
        agents.append(agent)
    
    print(f"Multi-agent simulation with {num_agents} agents")
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        
        # Each agent perceives and acts independently
        for i, agent in enumerate(agents):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 500 == 0:
            print(f"Step {step}")
    
    return agents
```

#### Example 2: Parameter Sweep
```python
def run_parameter_sweep(neuron_counts=[500, 1000, 2000], 
                         learning_rates=[0.0001, 0.001, 0.01]):
    """Run parameter sweep to find optimal settings."""
    
    results = []
    
    for neuron_count in neuron_counts:
        for learning_rate in learning_rates:
            print(f"Testing: {neuron_count} neurons, LR={learning_rate}")
            
            # Create experiment with specific parameters
            config = pynlm.createAdvancedConfig()
            config.set("brain.neuron_count", neuron_count)
            config.set("plasticity.stdp.learning_rate", learning_rate)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            spikes = []
            for step in range(1000):
                brain.step(step)
                spikes.append(brain.getTotalSpikeCount())
            
            # Calculate metrics
            avg_spikes = sum(spikes) / len(spikes)
            final_spikes = spikes[-1]
            spike_growth = final_spikes - spikes[0]
            
            result = {
                'neurons': neuron_count,
                'learning_rate': learning_rate,
                'avg_spikes': avg_spikes,
                'final_spikes': final_spikes,
                'spike_growth': spike_growth
            }
            
            results.append(result)
            print(f"  Result: avg={avg_spikes:.1f}, growth={spike_growth:.1f}")
    
    return results
```

#### Example 3: Real-time Visualization Interface
```python
import time
import matplotlib.pyplot as plt

def run_visualization_demo(duration_seconds=30):
    """Run simulation with real-time visualization."""
    
    # Setup visualization
    plt.ion()  # Interactive mode
    fig, ax = plt.subplots(2, 2, figsize=(12, 10))
    fig.suptitle("NLM Real-time Visualization", fontsize=16)
    
    # Create agent
    brain, world, agent = pynlm.createQuickAgent(30, 30, 10, 10)
    
    # Enable all learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Starting visualization demo (30 seconds)...")
    
    start_time = time.time()
    step = 0
    
    while time.time() - start_time < duration_seconds:
        # Update world
        world.update(0.05)
        
        # Process and act
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update visualization (simplified)
        ax[0,0].clear()
        ax[0,0].set_title(f"Brain Activity")
        ax[0,0].plot([brain.getTotalNeuronCount()] * 10)  # Placeholder
        
        ax[0,1].clear()
        ax[0,1].set_title(f"Spike Count")
        ax[0,1].plot([brain.getTotalSpikeCount()])
        
        ax[1,0].clear()
        ax[1,0].set_title(f"Curiosity Level")
        ax[1,0].plot([agent.getCuriosityLevel()])
        
        ax[1,1].clear()
        ax[1,1].set_title(f"Novelty Level")
        ax[1,1].plot([agent.getNoveltyLevel()])
        
        plt.tight_layout()
        plt.pause(0.01)
        
        step += 1
        
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"{brain.getFiringNeuronCount()} firing, "
                  f"{agent.getCuriosityLevel():.3f} curiosity")
    
    plt.ioff()
    plt.close()
    print("Visualization demo complete!")
```

## Getting Started Summary

### For Beginners:
1. **Quick Start**: Use `pynlm.createQuickBrain()` for basic simulations
2. **Simple Learning**: Use `pynlm.runLearningSimulation(1000)` for learning agents
3. **Help**: Use `pynlm.printHelp()` for comprehensive documentation

### For Intermediate Users:
1. **Custom Agents**: Use `pynlm.createQuickAgent()` for brain-world-agent setups
2. **File I/O**: Use `brain.save()` and `brain.load()` for persistence
3. **Configuration**: Use `pynlm.loadConfig()` and `config.saveToFile()` for advanced settings

### For Advanced Users:
1. **Complex Experiments**: Create parameter sweeps and multi-agent systems
2. **Real-time Visualization**: Use matplotlib integration for monitoring
3. **Custom Functions**: Use the factory functions to create specialized setups

## Next Steps

1. **Read Phase 6 Documentation**: Understand the complete integration
2. **Explore Advanced Features**: Try complex agent simulations
3. **Contribute**: Add new convenience functions or examples
4. **Report Issues**: Help improve the documentation and API

The NLM Python bindings provide a comprehensive interface for experimenting with artificial neural systems, from simple single-brain simulations to complex multi-agent learning environments. Use the examples above as starting points for your own research and experiments.