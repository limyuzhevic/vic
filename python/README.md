# NLM Python Bindings - Comprehensive User Guide

## Overview

This guide provides comprehensive documentation for the NLM (Neural Learning Machine) Python bindings. NLM is an experimental artificial developmental brain that implements real spiking neural computation with integrated memory, neuromodulation, prediction, and learning systems.

## Quick Start

### Installation

The NLM Python bindings are installed as part of the main package. You can install them using pip:

```bash
pip install pynlm
```

Or, if you're working with the source code:

```bash
pip install -e .
```

### Basic Usage Example

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain with configuration
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()

# Create simple world for simulation
world = pynlm.createSimpleWorld()
world.configure(width=80, height=60, visionWidth=16, visionHeight=16)

# Create agent brain interface
agent_brain = pynlm.createAgentBrain(brain)

print("NLM brain initialized successfully!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

## Core Concepts

### 1. Brain (Central Neural System)

The Brain class is the central coordinator of the neural system. It implements:

- **Real spiking neural computation** using Leaky Integrate-and-Fire (LIF) neurons
- **Event-driven dynamics** with synaptic delays
- **Integrated memory systems** (working, episodic, associative)
- **Neuromodulation** (dopamine, curiosity, novelty, prediction error)
- **Plasticity rules** (STDP, Hebbian, structural plasticity)
- **Predictive processing** and learning
- **Developmental stages** affecting plasticity

### 2. AgentBrain (World Interface)

The AgentBrain class connects the NLM brain to the world:

- **Sensory transduction**: Converts world inputs to neural currents
- **Motor decoding**: Interprets neural activity as motor commands
- **Neuromodulation**: Applies reward signals and learning
- **Development**: Manages developmental stages and plasticity

### 3. SimpleWorld (Environment)

The SimpleWorld provides a 2D simulated environment:

- **Sensory input**: Vision, touch, internal signals, proprioception
- **Action execution**: Motor commands affect world state
- **Feedback**: Returns rewards and observations
- **State management**: Handles agent body, objects, and interactions

## Detailed API Reference

### Configuration

```python
# Create empty configuration
config = pynlm.createDefaultConfig()

# Set various parameters
config.random_seed = 42
config.simulation_timestep = 0.001
config.neuron_count = 1000
config.region_count = 2
config.connection_probability = 0.15

# Save configuration to file
config.saveToFile("my_config.cfg")

# Load configuration from file
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.cfg")
```

### Brain Operations

```python
# Initialize brain
brain.initialize()

# Run simulation steps
for step in range(1000):
    brain.step(step, step * 0.001)

# Inject sensory input
vision_input = pynlm.Vision(16, 16, 3)
brain.receiveSensoryInput(vision_input)

# Get neural statistics
print(f"Firing rate: {brain.getAverageFiringRate()} Hz")
print(f"Spike count: {brain.getTotalSpikeCount()}")
print(f"Region count: {brain.getRegionCount()}")
```

### Agent Operations

```python
# Create agent components
world = pynlm.createSimpleWorld()
world.configure(width=80, height=60, visionWidth=16, visionHeight=16)
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Process sensory input
percept = world.getSensoryPercept()
agent_brain.processSensoryInput(percept)

# Decode motor command
command = agent_brain.decodeMotorCommand()
print(f"Decoded command: {command}")

# Apply reward
agent_brain.applyRewardModulation(reward=1.0, predicted_reward=0.5)

# Update development
agent_brain.updateDevelopment(0.001)
```

### Memory Systems

```python
# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Working memory operations
working_memory.store("key", "value", timestamp=100.0)
retrieved = working_memory.retrieve("key", 1.0)  # Within 1.0 time window

# Episodic memory operations
episode_id = episodic_memory.store_episode(
    percept=percept,
    action=action,
    reward=1.0,
    timestamp=100.0
)

# Replay episodes
episodic_memory.replay_episode(episode_id, callback_function)
```

### Neuromodulation

```python
# Get neuromodulation levels
dopamine = agent_brain.getNeuromodulationLevel()
curiosity = agent_brain.getCuriosityLevel()
novelty = agent_brain.getNoveltyLevel()
error = agent_brain.getPredictionError()

print(f"Neuromodulation levels:")
print(f"  Dopamine: {dopamine:.3f}")
print(f"  Curiosity: {curiosity:.3f}")
print(f"  Novelty: {novelty:.3f}")
print(f"  Prediction Error: {error:.3f}")
```

## Advanced Usage Examples

### 1. Complete Simulation Loop

```python
def run_simulation(config, duration=10.0):
    """Run a complete NLM simulation with world interaction."""
    
    # Initialize components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=100, height=100, visionWidth=20, visionHeight=20)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Set up agent in world
    world.setAgentStart(50, 50)
    
    print("Starting NLM simulation...")
    
    # Main simulation loop
    steps = int(duration / config.simulation_timestep)
    for step in range(steps):
        # Get current world state
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        result = world.applyMotorCommand(action, step * config.simulation_timestep)
        
        # Apply reward
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        # Update development
        agent.updateDevelopment(config.simulation_timestep)
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}: Action={action}, Reward={result.reward:.3f}")
            print(f"  Neuromodulation: Dopamine={agent.getNeuromodulationLevel():.3f}")
    
    print("Simulation complete!")
    
    # Return final statistics
    return {
        'total_reward': sum(world.getAgentBody().energy for _ in range(steps)),
        'final_firing_rate': brain.getAverageFiringRate(),
        'developmental_stage': agent.getDevelopmentalStage()
    }

# Example usage
config = pynlm.createDefaultConfig()
config.neuron_count = 2000
config.simulation_timestep = 0.001

results = run_simulation(config, duration=5.0)
print(f"Simulation results: {results}")
```

### 2. Learning Experiment

```python
def run_learning_experiment(config, episodes=50):
    """Run an experiment to demonstrate NLM learning capabilities."""
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=60, height=60, visionWidth=12, visionHeight=12)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    world.setAgentStart(30, 30)
    
    # Enable all plasticity systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Starting learning experiment...")
    print(f"Running {episodes} episodes")
    
    for episode in range(episodes):
        # Reset for new episode
        brain.reset()
        agent.reset()
        
        # Run episode
        episode_reward = 0.0
        steps_per_episode = 200
        
        for step in range(steps_per_episode):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process and act
            agent.processSensoryInput(percept)
            action = agent.decodeMotorCommand()
            
            # Execute action
            result = world.applyMotorCommand(action, step * config.simulation_timestep)
            episode_reward += result.reward
            
            # Learn from experience
            agent.applyRewardModulation(result.reward, agent.getPredictionError())
            agent.updateDevelopment(config.simulation_timestep)
        
        # Log episode results
        if episode % 10 == 0:
            print(f"Episode {episode}: Reward={episode_reward:.3f}, "
                  f"Dopamine={agent.getNeuromodulationLevel():.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    print("Learning experiment complete!")
    
    return {
        'total_episodes': episodes,
        'average_reward': episode_reward / episodes,
        'final_dopamine': agent.getNeuromodulationLevel(),
        'final_curiosity': agent.getCuriosityLevel()
    }

# Run learning experiment
config = pynlm.createDefaultConfig()
config.neuron_count = 1500
results = run_learning_experiment(config, episodes=100)
print(f"Learning results: {results}")
```

### 3. Developmental Stages

```python
def demonstrate_development(config, duration=30.0):
    """Demonstrate developmental stages affecting neural plasticity."""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=80, height=80, visionWidth=16, visionHeight=16)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    world.setAgentStart(40, 40)
    
    print("Demonstrating developmental stages...")
    print(f"Initial stage: {agent.getDevelopmentalStage()}")
    
    steps = int(duration / config.simulation_timestep)
    
    for step in range(steps):
        # Run one step
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * config.simulation_timestep)
        
        agent.applyRewardModulation(0.1, agent.getPredictionError())
        agent.updateDevelopment(config.simulation_timestep)
        
        # Log stage changes
        current_stage = agent.getDevelopmentalStage()
        if step % 300 == 0:
            print(f"Step {step}: Stage={current_stage}, "
                  f"Plasticity={agent.getBrain().getDevelopmentSystem().getPlasticityRate():.4f}")
    
    print("Development demonstration complete!")
    
    return agent.getDevelopmentalStage()

# Demonstrate development
config = pynlm.createDefaultConfig()
duration = demonstrate_development(config, duration=10.0)
print(f"Final developmental stage: {duration}")
```

## Configuration File Examples

### Basic Configuration

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001
}
```

### High-Performance Configuration

```json
{
    "random_seed": 12345,
    "simulation_timestep": 0.0005,
    "neuron_count": 2000,
    "region_count": 3,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_tau": 15.0,
    "synaptogenesis_rate": 0.00005,
    "pruning_rate": 0.000005,
    "enable_checkpointing": true,
    "checkpoint_interval": 1000
}
```

## Performance Tips

1. **Memory Management**: The NLM brain uses memory pools and event queues for efficient simulation
2. **Parallel Processing**: Utilize SIMD vectorization for faster computation
3. **Checkpointing**: Save and load brain states to resume long simulations
4. **Configuration Tuning**: Adjust parameters based on your hardware and requirements
5. **Batch Processing**: Process multiple steps before visualization for better performance

## Troubleshooting

### Common Issues and Solutions

1. **Brain initialization fails**
   - Check that configuration parameters are valid
   - Ensure random number generator is properly seeded
   - Verify file system permissions for checkpointing

2. **Python import errors**
   - Install the package: `pip install pynlm`
   - Check that all dependencies are installed
   - Verify Python version compatibility

3. **Simulation performance issues**
   - Reduce neuron count for faster testing
   - Increase simulation timestep
   - Use simpler world configurations
   - Check for memory leaks

4. **Parameter errors**
   - Use descriptive variable names
   - Check type consistency
   - Validate parameter ranges before use

## Further Reading

- **Architecture Documentation**: See `docs/ARCHITECTURE.md` for detailed technical information
- **Scientific Background**: See `docs/SCIENCE.md` for theoretical foundations
- **Phase Documentation**: See `docs/PHASE6_FINAL_AUDIT.md` for implementation details
- **Performance Analysis**: See `docs/LIMITATIONS.md` for performance considerations

## Support

For issues, questions, or contributions:

1. Check the documentation in the `docs/` directory
2. Review existing examples and tests
3. Consult the API reference in this guide
4. For technical issues, refer to the source code comments
5. For scientific questions, consult the scientific documentation

## License

MIT License - See LICENSE file for details.

## Copyright

NLM (熙然) - Neural Learning Machine Project

This package provides Python bindings for the NLM C++ neural simulation framework, enabling Python users to access advanced neural computation capabilities including real spiking neurons, learning, memory, and developmental systems.