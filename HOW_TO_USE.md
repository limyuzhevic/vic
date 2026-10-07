# NLM - Comprehensive Usage Guide

This guide provides comprehensive documentation for using NLM (Neural Learning Machine) beyond the basic examples.

## Prerequisites

- **Python 3.8 or higher**
- **pynlm** package installed (pip install pynlm)
- **NLM** library compiled and accessible

## Installation

### Build NLM from source:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
cd ..
pip install -e .
```

### Quick install (if NLM already built):
```bash
pip install -e .
```

## Basic Usage (Quick Start)

The easiest way to get started is with the simple brain example:

```python
import pynlm

# Create a simple brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run the brain for 100 simulation steps
for i in range(100):
    brain.step(i)

print(f"Brain ran successfully! Spike count: {brain.getTotalSpikeCount()}")
```

## Advanced Usage Examples

### Example 1: Complete Agent Simulation

```python
import pynlm
import time

def run_complete_simulation():
    """Run a complete simulation with brain, world, and agent."""
    
    # 1. Setup the brain with custom configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Setup the world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20, 
        height=20, 
        visionWidth=8, 
        visionHeight=8
    )
    world.reset()
    
    # 3. Create the agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 5. Run the simulation
    print("Starting simulation...")
    start_time = time.time()
    
    for step in range(1000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress periodically
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Firing neurons: {brain.getFiringNeuronCount()}, "
                  f"Total spikes: {brain.getTotalSpikeCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Reward: {agent.getNeuromodulationLevel():.3f}")
    
    end_time = time.time()
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    
    # Final brain status
    print(f"Final brain status:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_complete_simulation()
```

### Example 2: Learning Experiment

```python
import pynlm
import matplotlib.pyplot as plt

def run_learning_experiment():
    """Demonstrate learning through reward modulation and curiosity."""
    
    # Create brain with learning enabled
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world with rewards
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    # Create agent with all learning systems enabled
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Track learning metrics
    curiosity_levels = []
    neuromodulation_levels = []
    spike_counts = []
    reward_history = []
    
    print("Running learning experiment...")
    print("This demonstrates how curiosity drives exploration and reward enables learning")
    
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action (curiosity may cause exploration)
        action = agent.decodeMotorCommand()
        
        # Execute action and get reward
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation for learning
        if action_result.success:
            agent.applyRewardModulation(
                reward=action_result.reward,
                predictedReward=agent.getPredictionError()
            )
        
        # Record metrics
        curiosity_levels.append(agent.getCuriosityLevel())
        neuromodulation_levels.append(agent.getNeuromodulationLevel())
        spike_counts.append(brain.getTotalSpikeCount())
        reward_history.append(action_result.reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Exploration behavior: occasionally try new actions when curiosity is high
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Curiosity: {curiosity_levels[-1]:.3f}, "
                  f"Dopamine: {neuromodulation_levels[-1]:.3f}, "
                  f"Total spikes: {spike_counts[-1]}, "
                  f"Action: {action}")
    
    # Analyze learning patterns
    print("\nLearning Analysis:")
    print(f"Final curiosity level: {curiosity_levels[-1]:.3f}")
    print(f"Average neuromodulation: {sum(neuromodulation_levels)/len(neuromodulation_levels):.3f}")
    print(f"Peak curiosity: {max(curiosity_levels):.3f}")
    print(f"Peak neuromodulation: {max(neuromodulation_levels):.3f}")
    
    # Plot learning curves
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(12, 10))
    
    ax1.plot(curiosity_levels)
    ax1.set_title('Curiosity Level Over Time')
    ax1.set_xlabel('Step')
    ax1.set_ylabel('Curiosity')
    
    ax2.plot(neuromodulation_levels)
    ax2.set_title('Neuromodulation Level (Dopamine) Over Time')
    ax2.set_xlabel('Step')
    ax2.set_ylabel('Dopamine Level')
    
    ax3.plot(spike_counts)
    ax3.set_title('Total Spike Count Over Time')
    ax3.set_xlabel('Step')
    ax3.set_ylabel('Spike Count')
    
    ax4.plot(reward_history)
    ax4.set_title('Reward History')
    ax4.set_xlabel('Step')
    ax4.set_ylabel('Reward')
    
    plt.tight_layout()
    plt.savefig('learning_experiment.png')
    plt.show()
    
    return brain, world, agent

# Run learning experiment
brain, world, agent = run_learning_experiment()
```

### Example 3: Configuration Management

```python
import pynlm
import json

def demonstrate_configuration():
    """Show how to create and use custom configurations."""
    
    print("=== Configuration Examples ===\n")
    
    # 1. Create default configuration
    print("1. Default Configuration:")
    config1 = pynlm.createDefaultConfig()
    print(f"   Neuron count: {config1.getOr('neuron_count', 'Not set')}")
    print(f"   Region count: {config1.getOr('region_count', 'Not set')}")
    print(f"   Random seed: {config1.getOr('random_seed', 'Not set')}")
    
    # 2. Custom configuration
    print("\n2. Custom Configuration:")
    config2 = pynlm.createDefaultConfig()
    config2.set("neuron_count", 5000)
    config2.set("region_count", 5)
    config2.set("connection_probability", 0.05)
    config2.set("random_seed", 123)
    config2.set("simulation_timestep", 0.01)
    
    print(f"   Neuron count: {config2.getOr('neuron_count', 'Not set')}")
    print(f"   Region count: {config2.getOr('region_count', 'Not set')}")
    print(f"   Connection probability: {config2.getOr('connection_probability', 'Not set')}")
    
    # 3. Save configuration to file
    print("\n3. Saving configuration to file:")
    config2.saveToFile("my_config.json")
    print("   Configuration saved to 'my_config.json'")
    
    # 4. Load configuration from file
    print("\n4. Loading configuration from file:")
    config3 = pynlm.createDefaultConfig()
    if config3.loadFromFile("my_config.json"):
        print(f"   Successfully loaded configuration from file")
        print(f"   Neuron count: {config3.getOr('neuron_count', 'Not set')}")
    else:
        print("   Failed to load configuration from file")
    
    # 5. Command line configuration
    print("\n5. Command line configuration:")
    print("   Usage: python your_script.py --neuron_count=10000 --random_seed=42")
    
    # 6. Configuration summary
    print("\n6. Configuration Summary:")
    print(config2.summary())
    
    # 7. Configuration validation
    print("\n7. Configuration Validation:")
    print(f"   Has 'neuron_count': {config2.has('neuron_count')}")
    print(f"   Has 'invalid_key': {config2.has('invalid_key')}")
    
    # 8. All configuration keys
    print("\n8. All Configuration Keys:")
    keys = config2.getKeys()
    for key in keys:
        value = config2.getOr(key, "N/A")
        print(f"   {key}: {value}")
    
    return config2

# Demonstrate configuration
config = demonstrate_configuration()
```

### Example 4: Advanced Python Features

```python
import pynlm

def advanced_python_features():
    """Demonstrate advanced Python features and API capabilities."""
    
    print("=== Advanced Python Features ===\n")
    
    # 1. Brain with custom neuron types
    print("1. Custom Neuron Type Configuration:")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get all regions
    regions = brain.getRegions()
    print(f"   Brain has {len(regions)} regions")
    
    # Analyze neuron types
    neuron_counts = {}
    for region in regions:
        for pop in region.getPopulations():
            neuron_type = pop.getNeuronType()
            if neuron_type in neuron_counts:
                neuron_counts[neuron_type] += pop.getNeuronCount()
            else:
                neuron_counts[neuron_type] = pop.getNeuronCount()
    
    print("   Neuron type distribution:")
    for neuron_type, count in neuron_counts.items():
        print(f"     {neuron_type}: {count}")
    
    # 2. Sensory processing
    print("\n2. Sensory Processing:")
    # Create a simple world for testing
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    print(f"   Vision dimensions: {percept.getVisionWidth()} x {percept.getVisionHeight()}")
    print(f"   Vision data shape: {len(percept.getVision()) if percept.getVision() else 0}")
    print(f"   Touch data: {len(percept.getTouch()) if percept.getTouch() else 0}")
    print(f"   Internal data: {len(percept.getInternal()) if percept.getInternal() else 0}")
    
    # 3. Action production
    print("\n3. Action Production:")
    action = brain.produceAction()
    if action:
        print(f"   Produced action: {action.getName()}")
        print(f"   Action type: {action.getType()}")
        print(f"   Action parameters: {action.getParameters()}")
    
    # 4. Memory systems
    print("\n4. Memory Systems:")
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print(f"   Working memory capacity: {working_memory.getCapacity()}")
        print(f"   Working memory items: {working_memory.getItemCount()}")
    
    # 5. Performance monitoring
    print("\n5. Performance Monitoring:")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   Active neurons: {brain.getActiveNeuronCount()}")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"   Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # 6. Development tracking
    print("\n6. Development Tracking:")
    print(f"   Current developmental stage: {brain.getDevelopmentalStage()}")
    
    # 7. Neuromodulation levels
    print("\n7. Neuromodulation Levels:")
    # This would require access to internal brain components
    print("   (Access to neuromodulation requires brain introspection)")
    
    # 8. Checkpoint operations
    print("\n8. Checkpoint Operations:")
    print("   Checkpoint system available for saving/loading brain state")
    
    return brain, world, percept

# Demonstrate advanced features
brain, world, percept = advanced_python_features()
```

### Example 5: Error Handling and Validation

```python
import pynlm

def error_handling_example():
    """Demonstrate error handling and validation in NLM."""
    
    print("=== Error Handling and Validation ===\n")
    
    # 1. Invalid configuration
    print("1. Testing Error Handling:")
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test invalid operations
        print("   Performing operations...")
        
        # This should handle errors gracefully
        action = brain.produceAction()
        print(f"   Action produced: {action.getName() if action else 'None'}")
        
    except Exception as e:
        print(f"   Error caught: {e}")
    
    # 2. World configuration validation
    print("\n2. World Configuration Validation:")
    world = pynlm.createSimpleWorld()
    
    # Test valid configuration
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    print("   Valid world configuration successful")
    
    # 3. Agent initialization validation
    print("\n3. Agent Initialization Validation:")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Initialize agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print("   Agent initialization successful")
    
    # 4. Learning system validation
    print("\n4. Learning System Validation:")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    print("   Learning systems enabled successfully")
    
    # 5. Performance validation
    print("\n5. Performance Validation:")
    for i in range(10):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(i)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if i % 5 == 0:
            print(f"   Step {i}: "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Spikes: {brain.getTotalSpikeCount()}")
    
    return brain, world, agent

# Demonstrate error handling
brain, world, agent = error_handling_example()
```

## Reference Guide

### Core Classes

| Class | Description | Key Methods |
|-------|-------------|-------------|
| `Brain` | Neural simulation engine | `initialize()`, `step()`, `produceAction()` |
| `Config` | Configuration manager | `loadFromFile()`, `saveToFile()`, `set()` |
| `AgentBrain` | Agent interface | `processSensoryInput()`, `decodeMotorCommand()` |
| `SimpleWorld` | Environment simulation | `update()`, `getSensoryPercept()` |

### Common Configuration Options

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `neuron_count` | int | 1000 | Number of neurons in brain |
| `region_count` | int | 2 | Number of brain regions |
| `connection_probability` | float | 0.1 | Probability of synapse formation |
| `random_seed` | int | 42 | Random seed for reproducibility |
| `simulation_timestep` | float | 0.001 | Simulation time step |

### Common Errors and Solutions

| Error | Solution |
|-------|----------|
| Configuration not found | Check file path, use default configuration |
| Brain already initialized | Call `reset()` before re-initialization |
| Invalid parameters | Validate inputs before use |
| Memory issues | Monitor memory usage, use smaller configurations |

### Troubleshooting

#### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of simulation steps

#### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

#### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Ensure sensory input is connected and being provided

## Best Practices

### 1. Configuration Management
```python
# Always validate configuration before use
config = pynlm.createDefaultConfig()
if not config.has('neuron_count'):
    print("Warning: Using default neuron count")
    config.set('neuron_count', 1000)
```

### 2. Error Handling
```python
# Always handle errors gracefully
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except Exception as e:
    print(f"Failed to initialize brain: {e}")
```

### 3. Performance Monitoring
```python
# Monitor performance during simulation
import time
start_time = time.time()

for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        elapsed = time.time() - start_time
        print(f"Step {step}: {elapsed:.2f}s elapsed, "
              f"{brain.getTotalSpikeCount()} total spikes")
```

### 4. Memory Management
```python
# Use context managers for resources
with pynlm.createBrain(config) as brain:
    brain.initialize()
    for step in range(100):
        brain.step(step)
```

### 5. Testing and Validation
```python
# Always validate behavior
def test_brain():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    assert brain.getFiringNeuronCount() >= 0
    
    # Run simulation
    for step in range(10):
        brain.step(step)
    
    print("All tests passed!")
```

## Advanced Topics

### 1. Custom Development Stages
```python
# Modify development progression
agent.updateDevelopment(0.5)  # Fast-forward development
print(f"New developmental stage: {agent.getDevelopmentalStage()}")
```

### 2. Neuromodulation Control
```python
# Control neuromodulation levels
agent.applyRewardModulation(reward=1.0, predictedReward=0.5)
print(f"Current neuromodulation level: {agent.getNeuromodulationLevel()}")
```

### 3. Structural Plasticity
```python
# Enable structural changes (growing new connections)
agent.enableStructuralPlasticity(True)
print("Structural plasticity enabled")
```

### 4. Checkpoint System
```python
# Save and load brain state
brain.save("my_brain_checkpoint")
# ... later ...
brain.load("my_brain_checkpoint")
```

## Performance Tips

1. **Use appropriate timestep**: Use 0.001 for biological accuracy or larger for speed
2. **Adjust neuron count**: Start small for testing, scale up for realism
3. **Enable learning**: Use learning systems only when needed (performance impact)
4. **Monitor memory**: Large brains require more memory, monitor usage
5. **Batch operations**: Process multiple steps before checking status

## Conclusion

You now have a comprehensive guide to using NLM effectively. The key takeaways are:

1. **Start simple**: Use basic examples to understand the framework
2. **Validate configuration**: Always check your configuration settings
3. **Handle errors**: Use try-catch blocks for robust operation
4. **Monitor performance**: Track metrics to optimize your simulations
5. **Use learning systems wisely**: Enable only when needed for your application

The NLM framework provides powerful capabilities for creating artificial developmental brains. With these examples and guides, you should be able to:

- Create complex brain simulations
- Implement learning through reward and curiosity
- Manage configuration efficiently
- Debug and optimize your simulations
- Build advanced applications using the brain architecture

Good luck with your NLM development!
