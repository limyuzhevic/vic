# Enhanced Python Bindings Documentation and API Improvements

## Overview

The NLM Python bindings (pynlm) provide a comprehensive interface for working with the Neural Learning Machine simulation framework. This documentation includes the existing API plus enhancements for better usability and functionality.

## Getting Started

### Installation

```bash
# Install the NLM Python package
pip install .  # Install from current directory

# For development mode (with editable files)
pip install -e .

# Install with specific build options
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
pip install .
```

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(1000):
    brain.step(step)
    
# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

## Enhanced API Documentation

### Brain Class

The `Brain` class is the core neural simulation component. Here's the enhanced API:

#### Core Methods

```python
# Creation and initialization
brain = pynlm.createBrain(config)      # Create brain with config
brain.initialize()                     # Initialize simulation
brain.step(step)                       # Single simulation step
brain.step(step, time)                 # Step with timestamp
brain.reset()                          # Reset brain state
brain.save(filepath)                   # Save state to file
brain.load(filepath)                   # Load state from file

# Neural operations
brain.receiveSensoryInput(percept)    # Inject sensory data
brain.injectCurrent(neuron, current)  # Inject to specific neuron
brain.injectCurrentToNeurons(type, current)  # Inject to neuron type
brain.produceAction()                  # Get motor action

# Query methods
brain.getTotalNeuronCount()            # Get total neurons
brain.getTotalSynapseCount()           # Get total synapses
brain.getActiveNeuronCount()           # Get active neurons
brain.getFiringNeuronCount()           # Get firing neurons
brain.getAverageFiringRate()           # Get firing rate
brain.getExcitationInhibitionRatio()  # Get E/I balance
brain.getTotalSpikeCount()             # Get total spikes
brain.getDevelopmentalStage()         # Get development stage
brain.getConfig()                      # Get configuration
brain.logStatus()                      # Log brain status

# Region management
brain.addRegion(name)                  # Add neural region
brain.getRegion(id)                    # Get region by ID
brain.getRegionCount()                 # Get region count
brain.getRegionIds()                   # Get all region IDs
```

#### Enhanced Configuration Support

```python
# Create and configure brain
config = pynlm.createDefaultConfig()

# Query configuration
if config.has("brain.neuron_count"):
    neuron_count = config.get("brain.neuron_count")

# Modify configuration
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

# File operations
config.loadFromFile("config.json")     # Load JSON config
config.saveToFile("config.json")       # Save JSON config
config.getKeys()                       # Get all keys
config.remove("key")                  # Remove key
config.clear()                         # Clear all settings
```

### AgentBrain Class

The `AgentBrain` class provides the interface between the brain and the world:

#### Core Methods

```python
# Creation and initialization
agent = pynlm.createAgentBrain(brain)    # Create agent with brain
agent.initialize(world)                   # Initialize with world

# Sensory processing
agent.processSensoryInput(percept)      # Process sensory data

# Motor decoding
motor_cmd = agent.decodeMotorCommand()  # Get motor command

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)  # Apply reward
agent.updateDevelopment(timestep)       # Update development

# Query methods
agent.getDevelopmentalStage()           # Get development stage
agent.getNeuromodulationLevel()          # Get neuromodulation level
agent.getCuriosityLevel()                # Get curiosity level
agent.getNoveltyLevel()                  # Get novelty level
agent.getPredictionError()               # Get prediction error

# Subsystem controls
agent.enableRewardModulation(True)      # Enable/disable reward modulation
agent.enableStructuralPlasticity(True)  # Enable/disable structural plasticity
agent.enableDevelopment(True)           # Enable/disable development
agent.enableCuriosity(True)             # Enable/disable curiosity
```

### SimpleWorld Class

The `SimpleWorld` class represents the simulation environment:

#### Core Methods

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width, height, visionWidth, visionHeight)  # Set world dimensions
world.setAgentStart(x, y)                              # Set agent starting position
world.setRandomSeed(seed)                              # Set random seed
world.setMaxEnergy(e)                                  # Set max energy
world.setEnergyDecayRate(r)                            # Set energy decay rate

# Control
world.reset()                                          # Reset world to initial state
world.update(timestep)                                 # Update world simulation
world.applyMotorCommand(cmd, current_time)            # Apply agent action

# Access methods
percept = world.getSensoryPercept()                   # Get what the agent sees
body = world.getAgentBody()                          # Get agent body state

# Object management
world.addObject(obj)                                 # Add world object
world.removeObject(x, y)                             # Remove object at position
world.isValidPosition(x, Sade positionx, y)           # Check valid position

# Query methods
world.getWidth()                                     # Get world width
world.getHeight()                                    # Get world height
world.getMaxEnergy()                                 # Get max energy
world.getEnergyDecayRate()                           # Get energy decay rate
world.getSimulationTime()                            # Get simulation time
world.getRandomSeed()                                # Get random seed
```

### Enum Classes

#### NeuronType Enum

```python
# Neuron type constants
pynlm.NeuronType.Excitatory    # Excitatory neuron
pynlm.NeuronType.Inhibitory   # Inhibitory neuron
pynlm.NeuronType.Modulatory    # Modulatory neuron
pynlm.NeuronType.Sensory       # Sensory neuron
pynlm.NeuronType.Motor         # Motor neuron
pynlm.NeuronType.Internal     # Internal neuron
```

#### DevelopmentalStage Enum

```python
# Development stage constants
pynlm.DevelopmentalStage.Initial      # Initial stage
pynlm.DevelopmentalStage.CriticalPeriod # Critical period
pynlm.DevelopmentalStage.Maturation    # Maturation stage
pmlpym.DevelopmentalStage.Adult        # Adult stage
pynlm.DevelopmentalStage.Aging        # Aging stage
```

#### ActionType Enum

```python
# Action type constants
pynlm.ActionType.MoveForward   # Move forward
pynlm.ActionType.TurnLeft      # Turn left
pynlm.ActionType.TurnRight     # Turn right
pynlm.ActionType.Interact      # Interact with object
pynlm.ActionType.Eat           # Eat food
```

#### WorldObjectType Enum

```python
# World object type constants
pynlm.WorldObjectType.Empty      # Empty space
pynlm.WorldObjectType.Resource    # Resource
pynlm.WorldObjectType.Hazard     # Hazard/danger
pynlm.WorldObjectType.Wall        # Wall/obstacle
pynlm.WorldObjectType.Marker     # Marker/landmark
```

## Advanced Usage Examples

### Complete Simulation Example

```python
import pynlm

def run_simulation(config_path, steps=1000):
    """Run a complete agent simulation with world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 1. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Configure brain features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 6. Run simulation loop
    for step in range(steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Print progress
        if step % 100 == ورية:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    # 7. Print final statistics
    print("\nSimulation Results:")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run the simulation
run_simulation("configs/default.json", 1000)
```

### Configuration-Based Simulation

```python
import pynlm
import json

def load_config_from_json(json_file):
    """Load configuration from JSON file."""
    with open(json_file, 'r') as f:
        data = json.load(f)
    
    config = pynlm.createDefaultConfig()
    
    # Apply JSON configuration
    if "brain" in data:
        brain_config = data["brain"]
        if "neuron_count" in brain_config:
            config.set("brain.neuron_count", brain_config["neuron_count"])
        if "region_count" in brain_config:
            config.set("brain.region_count", brain_config["region_count"])
        if "connection_probability" in brain_config:
            config.set("brain.connection_probability", brain_config["connection_probability"])
    
    if "simulation" in data:
        sim_config = data["simulation"]
        if "timestep" in sim_config:
            config.set("simulation.timestep", sim_config["timestep"])
        if "max_steps" in sim_config:
            config.set("simulation.max_steps", sim_config["max_steps"])
    
    return config

def export_config_to_json(config, json_file):
    """Export configuration to JSON file."""
    config_dict = {}
    
    # Extract brain configuration
    brain_keys = ["neuron_count", "region_count", "connection_probability",
                  "v_thresh", "v_rest", "v_reset", "tau_mem", "tau_ref"]
    brain_config = {}
    for key in brain_keys:
        if config.has(f"brain.{key}"):
            brain_config[key] = config.get(f"brain.{key}")
    if brain_config:
        config_dict["brain"] = brain_config
    
    # Extract simulation configuration
    sim_keys = ["timestep", "max_steps", "checkpoint_path"]
    sim_config = {}
    for key in sim_keys:
        if config.has(f"simulation.{key}"):
            sim_config[key] = config.get(f"simulation.{key}")
    if sim_config:
        config_dict["simulation"] = sim_config
    
    # Write to JSON file
    with open(json_file, 'w') as f:
        json.dump(config_dict, f, indent=2)

# Example usage
config = load_config_from_json("my_config.json")
brain = pynlm.createBrain(config)
brain.initialize()

# Export modified config
config.set("brain.neuron_count", 2000)
export_config_to_json(config, "modified_config.json")
```

### Batch Processing

```python
import pynlm
from multiprocessing import Pool
import time

def run_single_simulation(params):
    """Run a single simulation with given parameters."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", params['neurons'])
    config.set("simulation.max_steps", params['steps'])
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    for step in range(params['steps']):
        brain.step(step)
    
    end_time = time.time()
    
    return {
        'neurons': params['neurons'],
        'steps': params['steps'],
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'time_seconds': end_time - start_time
    }

def run_batch_simulation(param_list):
    """Run multiple simulations in parallel."""
    with Pool(4) as pool:
        results = pool.map(run_single_simulation, param_list)
    
    return results

# Define simulation parameters
params = [
    {'neurons': 500, 'steps': 1000},
    {'neurons': 1000, 'steps': 1000},
    {'neurons': 2000, 'steps': 1000},
    {'neurons': 500, 'steps': 2000},
    {'neurons': 1000, 'steps': 2000},
]

# Run batch simulation
results = run_batch_simulation(params)

# Print results
print("Simulation Results:")
for result in results:
    print(f"Neurons: {result['neurons']:4d} | Steps: {result['steps']:4d} | "
          f"Spikes: {result['total_spikes']:6d} | "
          f"Firing rate: {result['firing_rate']:6.3f} | "
          f"Time: {result['time_seconds']:5.2f}s")
```

## Error Handling and Best Practices

### Error Handling

```python
import pynlm

def safe_simulation_run(config_path):
    """Run simulation with proper error handling."""
    try:
        # Load configuration
        config = pynlm.createDefaultConfig()
        if not config.loadFromFile(config_path):
            print(f"Error: Could not load configuration from {config_path}")
            return False
        
        # Create brain
        brain = pynlm.createBrain(config)
        if not brain.initialize():
            print("Error: Failed to initialize brain")
            return False
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
            
            # Check for errors
            if brain.getTotalSpikeCount() > 1000000:  # Safety check
                print("Warning: High spike count, consider reducing simulation time")
        
        print("Simulation completed successfully")
        return True
        
    except Exception as e:
        print(f"Simulation error: {e}")
        return False

# Run safe simulation
if not safe_simulation_run("my_config.json"):
    print("Simulation failed, exiting...")
    exit(1)
```

### Best Practices

1. **Always initialize before using**: Call `brain.initialize()` after creating a brain.

2. **Use try-catch for error handling**: Wrap simulation code in try-catch blocks.

3. **Check configuration**: Validate configuration before running simulations.

4. **Use appropriate simulation steps**: Start with smaller simulations for testing.

5. **Monitor system resources**: Be mindful of memory and CPU usage for large networks.

6. **Use checkpoints for long simulations**: Save intermediate state for long-running simulations.

7. **Validate inputs**: Check all input parameters before processing.

8. **Handle exceptions gracefully**: Provide meaningful error messages to users.

## Performance Tips

### Optimizing Simulations

```python
import pynlm

# Create optimized configuration
def create_optimized_config():
    config = pynlm.createDefaultConfig()
    
    # Optimize for different scenarios
    # For speed:
    config.set("brain.neuron_count", 500)
    config.set("simulation.max_steps", 500)
    
    # For realism:
    config.set("brain.neuron_count", 5000)
    config.set("brain.connection_probability", 0.05)
    config.set("plasticity.stdp.learning_rate", 0.0001)
    
    # For research:
    config.set("brain.neuron_count", 10000)
    config.set("plasticity.structural.enable", True)
    config.set("memory.episodic.capacity", 5000)
    
    return config

# Use checkpointing for long simulations
def run_with_checkpoints(config, checkpoint_interval=1000):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(config.get("simulation.max_steps")):
        brain.step(step)
        
        # Save checkpoint periodically
        if step > 0 and step % checkpoint_interval == 0:
            checkpoint_file = f"checkpoint_step_{step}.bin"
            if brain.save(checkpoint_file):
                print(f"Checkpoint saved: {checkpoint_file}")
            else:
                print(f"Warning: Failed to save checkpoint at step {step}")

# Run optimized simulation
config = create_optimized_config()
run_with_checkpoints(config, checkpoint_interval=500)
```

## API Reference Summary

### Core Classes

| Class | Purpose | Key Methods |
|-------|---------|-------------|
| `Brain` | Neural simulation engine | `initialize()`, `step()`, `save()`, `load()` |
| `AgentBrain` | Agent-world interface | `initialize()`, `processSensoryInput()`, `decodeMotorCommand()` |
| `SimpleWorld` | Simulation environment | `configure()`, `reset()`, `update()`, `applyMotorCommand()` |
| `Config` | Configuration management | `loadFromFile()`, `saveToFile()`, `set()`, `get()` |

### Enums

| Enum | Values | Description |
|------|--------|-------------|
| `NeuronType` | Excitatory, Inhibitory, etc. | Neuron type classifications |
| `DevelopmentalStage` | Initial, CriticalPeriod, etc. | Brain development stages |
| `ActionType` | MoveForward, TurnLeft, etc. | Possible actions |
| `WorldObjectType` | Empty, Resource, Hazard, etc. | World object types |

## Migration Guide

### From Previous Versions

1. **Configuration format**: Changed from key=value to nested JSON/YAML format.
2. **Method names**: Some methods have been standardized (e.g., `getTotalNeuronCount()` instead of `getNeuronCount()`).
3. **Error handling**: More robust error handling with meaningful error messages.
4. **Advanced features**: New configuration and simulation options.

### Upgrading Examples

```python
# Old way
old_brain = OldBrainClass()
old_brain.Init()
old_brain.Simulate(steps)

# New way
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(steps):
    brain.step(step)
```

## Troubleshooting

### Common Issues

1. **Brain doesn't initialize**
   - Check that configuration is valid
   - Ensure required libraries are installed
   - Verify file permissions

2. **Simulation crashes**
   - Start with smaller network sizes
   - Use try-catch blocks around simulation code
   - Check for memory issues

3. **Performance issues**
   - Optimize configuration parameters
   - Use checkpointing for long simulations
   - Consider running on larger hardware

4. **Python bindings not working**
   - Ensure C++ library is built with Python bindings
   - Check that pynlm module is in Python path
   - Verify all dependencies are installed

### Debug Commands

```bash
# Check if pynlm is installed
python -c "import pynlm; print(pynlm.__version__)"

# Test basic functionality
python -c "
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print(f'Brain created with {brain.getTotalNeuronCount()} neurons')
"

# Run a simple test
python -c "
import pynlm
config = pynlm.createDefaultConfig()
config.set('brain.neuron_count', 100)
brain = pynlm.createBrain(config)
brain.initialize()
for i in range(10):
    brain.step(i)
print(f'Simulation completed: {brain.getTotalSpikeCount()} spikes')
"
```

## Future Enhancements

The Python bindings will continue to evolve with:

1. **Enhanced visualization**: Built-in plotting and visualization tools
2. **Advanced machine learning**: Integration with ML libraries
3. **Distributed computing**: Parallel processing support
4. **Cloud integration**: Cloud-based simulation capabilities
5. **Web API**: REST API for remote access
6. **Mobile support**: iOS and Android bindings

## Conclusion

The enhanced NLM Python bindings provide a comprehensive, user-friendly interface for working with the Neural Learning Machine simulation framework. With improved documentation, better error handling, and advanced features, users can now easily create, configure, and run complex neural simulations.

For the most up-to-date information and additional examples, visit the NLM documentation website.
