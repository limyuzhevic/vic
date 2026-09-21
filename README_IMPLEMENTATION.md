# Improved Documentation for NLM (Phase 6)

## Quick Start for Users

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create a default brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Advanced Agent Example

```python
import pynlm

# Create agent with learning capabilities
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable advanced learning
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True) 
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run learning episode
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing, "
              f"curiosity={agent.getCuriosityLevel():.2f}, "
              f"novelty={agent.getNoveltyLevel():.2f}")
```

### Configuration Examples

```python
import pynlm

# Custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("region_count", 2)
config.set("connection_probability", 0.2)

# Small brain for testing
small_config = pynlm.createDefaultConfig()
small_config.set("neuron_count", 100)
config.set("simulation_timestep", 0.01)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Monitoring and Debugging

```python
import pynlm
import matplotlib.pyplot as plt

# Create brain with monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Track metrics
firing_counts = []
spike_counts = []

for step in range(1000):
    brain.step(step)
    firing_counts.append(brain.getFiringNeuronCount())
    spike_counts.append(brain.getTotalSpikeCount())

# Simple plotting
plt.figure(figsize=(12, 4))
plt.subplot(121)
plt.plot(firing_counts)
plt.title('Firing Neurons Over Time')
plt.xlabel('Step')
plt.ylabel('Count')

plt.subplot(122)
plt.plot(spike_counts)
plt.title('Total Spikes Over Time')
plt.xlabel('Step')
plt.ylabel('Count')
plt.tight_layout()
plt.show()
```

## API Reference

### Core Classes

#### Brain
The central neural processing unit with spiking neurons and learning capabilities.

**Key Methods:**
- `initialize()` - Setup brain with configuration
- `step(step)` - Run one simulation step
- `runSimulation(steps, dt)` - Run multiple steps at once
- `getTotalNeuronCount()` - Get total neurons
- `getTotalSynapseCount()` - Get total synapses
- `getAverageFiringRate()` - Get average firing rate
- `getDevelopmentalStage()` - Get current developmental stage

#### Config
Brain configuration management.

**Key Methods:**
- `has(key)` - Check if key exists
- `get(key)` - Get configuration value
- `getOr(key, default)` - Get with default
- `set(key, value, source)` - Set configuration value

#### AgentBrain
Bridge between brain and world, handling perception and action.

**Key Methods:**
- `initialize(world)` - Setup with world
- `processSensoryInput(percept)` - Process sensory data
- `decodeMotorCommand()` - Generate motor command
- `applyRewardModulation(reward, predicted)` - Apply reward learning
- `runEpisode(world, brain, steps)` - Run complete episode

#### SimpleWorld
2D simulation environment with agent and objects.

**Key Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Setup dimensions
- `update(timestep)` - Advance simulation
- `applyMotorCommand(cmd, time)` - Execute action
- `getSensoryPercept()` - Get sensory input
- `getAgentBody()` - Get agent state

### Neuron Type Enumeration
```python
pynlm.NeuronType.Excitatory    # Excitatory neurons
pynlm.NeuronType.Inhibitory    # Inhibitory neurons  
pynlm.NeuronType.Sensory       # Sensory neurons
pynlm.NeuronType.Motor         # Motor neurons
pynlm.NeuronType.Internal     # Internal neurons
```

### Developmental Stage Enumeration
```python
pynlm.DevelopmentalStage.Initial         # Newborn phase
pynlm.DevelopmentalStage.CriticalPeriod  # Learning window
pynlm.DevelopmentalStage.Maturation       # Maturing phase
pynlm.DevelopmentalStage.Adult           # Adult phase
pynlm.DevelopmentalStage.Aging           # Aging phase
```

### Action Type Enumeration
```python
pynlm.ActionType.MoveForward    # Move forward
pynlm.ActionType.MoveBackward   # Move backward
pynlm.ActionType.TurnLeft       # Turn left
pynlm.ActionType.TurnRight      # Turn right
pynlm.ActionType.Look           # Look around
pynlm.ActionType.Interact       # Interact with objects
pynlm.ActionType.Eat            # Eat/food
pynlm.ActionType.Drink          # Drink/water
pynlm.ActionType.Rest           # Rest/sleep
pynlm.ActionType.Wait           # Wait/idle
```

## Advanced Features

### 1. Learning Mechanisms

The NLM brain supports multiple learning paradigms:

```python
# Enable specific learning systems
agent.enableRewardModulation(True)      # Reward prediction error
agent.enableStructuralPlasticity(True)  # Grow new connections  
agent.enableDevelopment(True)           # Stage-dependent learning
agent.enableCuriosity(True)             # Novelty-based exploration

# These affect synaptic plasticity through neuromodulation
```

### 2. Memory Systems

```python
# Access memory systems for advanced behavior
brain = pynlm.createBrain(config)
brain.initialize()

working_memory = brain.getWorkingMemory()
if working_memory:
    # Working memory for immediate information
    traces = working_memory.getActiveTraces()
    
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    # Store experiences for later recall
    episodes = episodic_memory.getEpisodeCount()
```

### 3. Neuromodulation

```python
# Monitor and use neuromodulation signals
brain = pynlm.createBrain(config)
brain.initialize()

# Get neuromodulation levels
# These reflect internal brain state and affect behavior
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
```

### 4. Development

```python
# Observe developmental progression
brain = pynlm.createBrain(config)
brain.initialize()

# Brain goes through developmental stages
# Each stage has different plasticity characteristics
for step in range(10000):
    brain.step(step)
    stage = brain.getDevelopmentalStage()
    
    if step % 1000 == 0:
        print(f"Step {step}: {stage}")
```

### 5. Event-Driven Processing

```python
# Use callback systems for efficient processing
brain = pynlm.createBrain(config)
brain.initialize()

# Register spike handlers for custom processing
spike_system = brain.getSpikeSystem()
if spike_system:
    spike_system.registerHandler([brain](const SpikeEvent& event) {
        // Handle spikes as they occur
        // More efficient than polling
    })
```

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"**
```python
# Solution: Check initialization and enable features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Make sure to initialize
brain.initialize()

# Run enough steps for activity
for step in range(1000):
    brain.step(step)
```

**"The agent isn't moving"**
```python
# Solution: Check world and agent setup
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Configure world properly
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent
agent.initialize(world)

# Enable motor capabilities
agent.enableRewardModulation(True)
```

**"Everything is 0"**
```python
# Solution: Give brain time and input
brain = pynlm.createBrain(config)
brain.initialize()

# Brains need time to "warm up"
for step in range(5000):  # More steps
    brain.step(step)
    
# Provide sensory input for neurons to respond to
# Add agent and world interaction
```

### Performance Tips

1. **For large simulations:** Use batch processing
2. **For detailed analysis:** Sample metrics instead of logging every step
3. **For development:** Start with small brain (100 neurons) and scale up
4. **For real-time:** Use event-driven systems instead of polling

## Example: Complete Learning Scenario

```python
import pynlm

def create_learning_agent(config=None):
    """Create a complete learning agent with all features enabled"""
    if config is None:
        config = pynlm.createDefaultConfig()
    
    # Configure for balanced learning
    config.set("neuron_count", 500)
    config.set("region_count", 2)
    config.set("connection_probability", 0.15)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Setup world
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Initialize with learning capabilities
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return agent, brain, world

def run_learning_experiment(agent, brain, world, steps=2000):
    """Run a complete learning experiment"""
    print("Starting learning experiment...")
    print("This will demonstrate neural development and adaptive behavior")
    
    for step in range(steps):
        # Environment interaction
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Neural processing
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning
        reward = 0.0  # Would be computed from world dynamics
        agent.applyRewardModulation(reward, 0.0)
        
        # Development
        agent.updateDevelopment(0.1)
        
        # Monitoring
        if step % 200 == 0:
            stage = brain.getDevelopmentalStage()
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            firing = brain.getFiringNeuronCount()
            
            print(f"Step {step:4d} | "
                  f"Stage: {str(stage):15s} | "
                  f"Curiosity: {curiosity:.2f} | "
                  f"Novelty: {novelty:.2f} | "
                  f"Firing: {firing:4d}")
    
    print("\nExperiment completed!")
    print("The brain has developed through stages and learned from experience.")

# Example usage
if __name__ == "__main__":
    # Create learning agent
    agent, brain, world = create_learning_agent()
    
    # Run learning experiment
    run_learning_experiment(agent, brain, world, 2000)
    
    # Show final statistics
    print(f"\nFinal Statistics:")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
```

## File Downloads

### Complete Example Scripts

- `minimal_brain.py` - Smallest possible brain
- `basic_agent.py` - Simple agent-world interaction  
- `learning_experiment.py` - Complete learning demonstration
- `debug_monitor.py` - Brain monitoring tools

### Configuration Templates

- `config_small.py` - Configuration for testing
- `config_medium.py` - Balanced configuration
- `config_large.py` - High-capacity configuration
- `config_debug.py` - Debug configuration

### Documentation

- `USAGE.md` - Complete API documentation
- `TUTORIAL.md` - Step-by-step tutorials
- `EXAMPLES.md` - Practical examples
- `API_REFERENCE.md` - Detailed method descriptions

## Future Improvements

The documentation system is actively being improved:

1. **Interactive Examples:** Live code execution in documentation
2. **Visualizations:** Neural network diagrams and simulations
3. **API Autocompletion:** Full IDE support with type hints
4. **Performance Profiling:** Built-in profiling tools
5. **Export/Import:** Configuration sharing between projects
6. **Community Examples:** User-contributed examples and tutorials

## Support

For questions, issues, or feature requests:

- **GitHub Issues:** Report bugs and request features
- **Documentation Issues:** Report documentation errors
- **Community Forum:** Share experiences and get help
- **Discord Channel:** Real-time support and discussions

## Citation

If you use NLM in your research or project, please cite:

```bibtexn
@article{nlm2024,
    title={NLM (Neural Learning Machine): An Experimental Artificial Developmental Brain},
    author={NLM Authors},
    year={2024},
    journal={Neural Simulation Systems},
    url={https://github.com/nlm-project/nlm}
}
```

## License

NLM is released under the MIT License. See LICENSE file for details.