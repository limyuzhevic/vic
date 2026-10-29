# NLM Python API Documentation

**Complete Reference for the pynlm Python bindings**

## Overview

The NLM Python API provides access to the Neural Learning Machine (NLM) neural simulation framework. This API allows you to:

- Create and configure virtual brains
- Simulate neural networks in different environments
- Enable learning and development capabilities
- Access detailed brain state information
- Run comprehensive experiments

## Installation

### From Source

```bash
# Clone the repository
cd /path/to/nlm

# Build with Python bindings
mkdir build
cd build
cmake ..
make -j4

# Install the Python module
cd python
pip install .
```

### Using pip (if available)

```bash
pip install pynlm
```

## Basic Usage

### Quick Start (see `easy_usage.md` for more details)

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
```

## API Reference

### Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify configuration values
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)
config.set("connection_probability", 0.1)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")
```

### Brain Class

#### Basic Brain Operations

```python
# Create brain with configuration
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run simulation step
brain.step(0)  # Current step
brain.step(0, 0.001)  # Current step and time

# Get brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

#### Sensory and Motor Functions

```python
# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent
agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Main simulation loop
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Simulate brain
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Advanced Brain Features

```python
# Set developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Get current developmental stage
stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {stage}")

# Save brain state
brain.save("brain_checkpoint.bin")

# Load brain state
brain.load("brain_checkpoint.bin")

# Add new region
region_id = brain.addRegion("new_region")
region = brain.getRegion(region_id)
```

### AgentBrain Class

#### Sensory Processing

```python
# Create sensory percept
sensor_percept = pynlm.SensoryPercept()

# Set vision data
vision_data = [0.1, 0.5, 0.9, 0.2]  # Example vision data
sensor_percept.setVision(pynlm.Vision(8, 8, 3, vision_data))

# Set touch data
touch_data = [0.0, 1.0, 0.5]  # Example touch data
sensor_percept.setTouch(pynlm.Audio(100, touch_data))

# Process sensory input
agent.processSensoryInput(sensor_percept)
```

#### Motor Output

```python
# Get motor command
command = agent.decodeMotorCommand()

# Command types:
# pynlm.MotorCommand.MoveForward
# pynlm.MotorCommand.MoveBackward
# pynlm.MotorCommand.TurnLeft
# pynlm.MotorCommand.TurnRight
# pynlm.MotorCommand.Interact
# pynlm.MotorCommand.Wait

print(f"Selected action: {command}")
```

#### Neuromodulation

```python
# Apply reward modulation
agent.applyRewardModulation(reward=1.0, predicted_reward=0.5)

# Get neuromodulation levels
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.2f}")
print(f"Novelty level: {agent.getNoveltyLevel():.2f}")
print(f"Prediction error: {agent.getPredictionError():.2f}")
```

### World Class

#### Simple World

```python
# Create simple world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)

# Set agent start position
world.setAgentStart(5, 5)

# Get agent body state
agent_body = world.getAgentBody()
print(f"Agent position: ({agent_body.x}, {agent_body.y})")
print(f"Agent energy: {agent_body.energy}")
print(f"Agent health: {agent_body.health}")
print(f"Agent age: {agent_body.age}")
```

#### World Objects

```python
# Create world objects
resource = pynlm.WorldObject(
    x=10, y=10, 
    type=pynlm.WorldObjectType.Resource, 
    value=1.0
)

wall = pynlm.WorldObject(
    x=15, y=5, 
    type=pynlm.WorldObjectType.Wall, 
    radius=0.5
)

# Add objects to world
world.addObject(resource)
world.addObject(wall)

# Check if position is valid
is_valid = world.isValidPosition(12, 12)
print(f"Position (12, 12) is valid: {is_valid}")
```

## Advanced Examples

### Example 1: Learning in a Simple Environment

```python
import pynlm

def run_learning_simulation():
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("vision_width", 8)
    config.set("vision_height", 8)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setAgentStart(10, 10)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("=== Learning Simulation ===")
    print("Initial brain state:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation
    for episode in range(5):
        print(f"\n--- Episode {episode} ---")
        world.reset()
        agent.reset()
        
        for step in range(200):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processing
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Learn from result
            agent.applyRewardModulation(result.reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            if step % 50 == 0:
                print(f"  Step {step}: Reward {result.reward:.2f}, "
                      f"Firing {brain.getFiringNeuronCount()}, "
                      f"Energy {world.getAgentBody().energy:.1f}")
    
    print("\n=== Simulation Complete ===")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Final curiosity: {agent.getCuriosityLevel():.2f}")

run_learning_simulation()
```

### Example 2: Brain Checkpointing and Loading

```python
import pynlm
import os

def save_and_load_brain():
    # Create and configure brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some simulation
    for step in range(100):
        brain.step(step)
    
    # Save brain state
    checkpoint_file = "learning_checkpoint.bin"
    print(f"Saving brain to {checkpoint_file}...")
    brain.save(checkpoint_file)
    
    # Run more simulation
    for step in range(100, 200):
        brain.step(step)
    
    print("Brain state after additional simulation:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    
    # Save brain state again
    final_file = "final_checkpoint.bin"
    brain.save(final_file)
    
    # Create new brain and load checkpoint
    brain2 = pynlm.createBrain(config)
    print(f"\nLoading brain from {checkpoint_file}...")
    
    if brain2.load(checkpoint_file):
        print("Brain loaded successfully!")
        print(f"Loaded brain state:")
        print(f"  Total spikes: {brain2.getTotalSpikeCount()}")
        print(f"  Firing neurons: {brain2.getFiringNeuronCount()}")
        
        # Continue simulation from loaded state
        for step in range(200, 250):
            brain2.step(step)
        
        print(f"After additional simulation:")
        print(f"  Total spikes: {brain2.getTotalSpikeCount()}")
        print(f"  Firing neurons: {brain2.getFiringNeuronCount()}")
        
        # Clean up
        os.remove(checkpoint_file)
        os.remove(final_file)
    else:
        print("Failed to load brain checkpoint")

save_and_load_brain()
```

### Example 3: Multiple Brain Configuration

```python
import pynlm

def compare_brain_configurations():
    # Define different configurations
    configs = []
    
    # Configuration 1: Small and fast
    config1 = pynlm.createDefaultConfig()
    config1.set("neuron_count", 100)
    config1.set("region_count", 1)
    configs.append(("Small Fast", config1))
    
    # Configuration 2: Medium with learning
    config2 = pynlm.createDefaultConfig()
    config2.set("neuron_count", 500)
    config2.set("region_count", 2)
    config2.set("connection_probability", 0.15)
    configs.append(("Medium Learning", config2))
    
    # Configuration 3: Large and complex
    config3 = pynlm.createDefaultConfig()
    config3.set("neuron_count", 1000)
    config3.set("region_count", 3)
    config3.set("connection_probability", 0.2)
    configs.append(("Large Complex", config3))
    
    print("=== Comparing Brain Configurations ===")
    
    for name, config in configs:
        print(f"\n{name} Configuration:")
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(50):
            brain.step(step)
        
        # Record results
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Synapses: {brain.getTotalSynapseCount()}")
        print(f"  Spikes: {brain.getTotalSpikeCount()}")
        print(f"  Firing rate: {brain.getAverageFiringRate():.2f}")
        
        # Store brain for comparison
        if not hasattr(compare_brain_configurations, 'brains'):
            compare_brain_configurations.brains = []
        compare_brain_configurations.brains.append((name, brain))

compare_brain_configurations()
```

## Common Patterns

### Pattern 1: Standard Simulation Loop

```python
def standard_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from result
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress periodically
        if step % 50 == 0:
            print(f"Step {step}: Reward {result.reward:.2f}, "
                  f"Firing {brain.getFiringNeuronCount()}, "
                  f"Energy {world.getAgentBody().energy:.1f}")
```

### Pattern 2: Learning with Memory

```python
def learning_with_memory(brain, world, agent, num_episodes):
    for episode in range(num_episodes):
        world.reset()
        agent.reset()
        
        episode_reward = 0.0
        
        for step in range(100):
            world.update(0.1)
            
            # Get sensory input and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processing
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action and get reward
            result = world.applyMotorCommand(action, world.getSimulationTime())
            episode_reward += result.reward
            
            # Learn from result
            agent.applyRewardModulation(result.reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Print episode summary
        print(f"Episode {episode}: Total reward {episode_reward:.2f}, "
              f"Final curiosity {agent.getCuriosityLevel():.2f}")
```

### Pattern 3: Visualization and Monitoring

```python
def monitor_brain(brain, world, agent, monitor_interval=10):
    metrics = {
        'steps': [],
        'rewards': [],
        'firing_rates': [],
        'curiosity_levels': [],
        'energy_levels': []
    }
    
    for step in range(500):
        world.update(0.1)
        
        # Record metrics
        metrics['steps'].append(step)
        metrics['rewards'].append(world.getAgentBody().energy if hasattr(world, 'getAgentBody') else 0)
        metrics['firing_rates'].append(brain.getAverageFiringRate())
        metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        metrics['energy_levels'].append(world.getAgentBody().energy if hasattr(world, 'getAgentBody') else 0)
        
        # Standard simulation steps
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Print monitoring data
        if step % monitor_interval == 0:
            print(f"Step {step}:")
            print(f"  Firing rate: {brain.getAverageFiringRate():.2f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")
            print(f"  Energy: {metrics['energy_levels'][-1]:.1f}")
            print(f"  Prediction error: {agent.getPredictionError():.2f}")
    
    return metrics
```

## Troubleshooting

### Common Issues and Solutions

#### "Brain isn't doing anything"

**Problem**: The brain simulation isn't producing any activity.

**Solution**:
```python
# Check if brain was initialized
if not brain.initialize():
    print("Brain initialization failed!")
    # Check configuration
    print(f"Neuron count: {config.getOr<int64_t>('neuron_count', 500)}")
    print(f"Connection probability: {config.getOr<float>('connection_probability', 0.1f)}")

# Make sure to call brain.step()
for step in range(100):
    brain.step(step)  # This is essential!
```

#### "Agent isn't moving"

**Problem**: The agent isn't producing motor commands.

**Solution**:
```python
# Check agent initialization
agent.initialize(world)

# Enable the agent brain interface
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

# Make sure to get motor commands
action = agent.decodeMotorCommand()
print(f"Selected action: {action}")

# Apply the action
world.applyMotorCommand(action, world.getSimulationTime())
```

#### "Everything is 0"

**Problem**: All values are zero (neurons firing, rewards, etc.).

**Solution**:
```python
# Brains need time to "warm up"
for step in range(100):
    brain.step(step)

# Make sure sensory input is connected
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Check if world objects are set up
world.addObject(pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Resource, 1.0))
```

#### "Memory usage too high"

**Problem**: High memory usage during simulation.

**Solutions**:
```python
# Use smaller brain configuration
config.set("neuron_count", 100)
config.set("region_count", 1)

# Reduce simulation steps
for step in range(100):  # Instead of 1000
    brain.step(step)

# Use more efficient data structures
# (This depends on the specific NLM implementation)
```

## Performance Tips

### Optimization Strategies

1. **Use Appropriate Brain Size**
   - Start with smaller configurations for development
   - Increase size gradually for production

2. **Configure Simulation Parameters**
   ```python
   # Use appropriate timestep for your simulation
   config.set("simulation_timestep", 0.001)
   
   # Adjust neuron count based on available memory
   config.set("neuron_count", 500)
   ```

3. **Enable Efficient Learning**
   ```python
   # Enable learning only if needed
   agent.enableRewardModulation(True)   # If you want reward learning
   agent.enableCuriosity(True)          # If you want exploration
   agent.enableDevelopment(True)        # If you want development
   ```

4. **Use Batch Processing**
   ```python
   # For long simulations, save checkpoints
   if step % 100 == 0:
       brain.save(f"checkpoint_step_{step}.bin")
   ```

5. **Monitor Performance**
   ```python
   import time
   start_time = time.time()
   
   for step in range(1000):
       brain.step(step)
   
   end_time = time.time()
   print(f"Simulation completed in {end_time - start_time:.2f} seconds")
   ```

### Memory Management

1. **Clean Up References**
   ```python
   # Delete large objects when no longer needed
   del brain
   del world
   del agent
   ```

2. **Use Configuration Files**
   ```python
   # Save configuration for later use
   config.saveToFile("optimized_config.json")
   
   # Load configuration when needed
   config.loadFromFile("optimized_config.json")
   ```

## API Compatibility

### Python Version Requirements

- Python 3.8 or higher
- NumPy (optional, for advanced operations)

### Installation

```bash
# Install with pip
pip install pynlm

# Or install from source
pip install -e .
```

## License

This API is part of the NLM (Neural Learning Machine) project, licensed under the MIT License.

## Support

For support and questions:
1. Check the documentation in `docs/`
2. Visit the NLM project website
3. Report issues on the GitHub repository

## Future Enhancements

### Planned Features

1. **Advanced World Types**: More sophisticated environments
2. **Enhanced Learning**: New learning algorithms and mechanisms
3. **Visualization Tools**: Real-time visualization capabilities
4. **Distributed Computing**: Support for multi-machine simulations
5. **Cloud Integration**: Deployment on cloud platforms

### Contribution Guidelines

1. **Code Quality**: Follow PEP 8 and NLM coding standards
2. **Documentation**: Update documentation for new features
3. **Testing**: Add comprehensive tests for new functionality
4. **Performance**: Profile and optimize performance as needed

## Conclusion

The NLM Python API provides a comprehensive interface for creating and simulating neural networks. Whether you're building simple proof-of-concepts or complex learning systems, the API offers flexibility and power while maintaining ease of use.

Start with the basic examples and gradually explore the advanced features as you become more familiar with the NLM framework.