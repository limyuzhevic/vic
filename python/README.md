# NLM Python Bindings - User Guide

## Quick Start

The NLM Python bindings provide access to the Neural Learning Machine (熙然) neural simulation framework. This guide covers the essential functionality for getting started with the Python API.

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create a default brain configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

## Configuration

### Config Class

The `Config` class manages brain configuration parameters. It provides methods for:

- Creating default configurations
- Setting and getting configuration values
- Loading from files
- Validating configuration

#### Key Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |

#### Usage Examples

```python
import pynlm

# Create default config
config = pynlm.createDefaultConfig()

# Check if config has a value
if config.has("brain.neuron_count"):
    print(f"Current neuron count: {config.get('brain.neuron_count')}")

# Set custom values
config.set("brain.neuron_count", 2000)
config.set("learning.rate", 0.001)

# Get all keys
keys = config.getKeys()
print(f"Available keys: {keys}")

# Load from file
config.loadFromFile("config.json")

# Save to file
config.saveToFile("my_config.json")
```

## Brain Simulation

### Brain Class

The `Brain` class represents the neural simulation system. Key methods include:

- `initialize()`: Start the brain with configuration
- `step()`: Run one simulation step
- `receiveSensoryInput()`: Inject sensory data
- `produceAction()`: Get motor output
- `save()`/`load()`: Checkpoint brain state

#### Complete Brain Example

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with monitoring
for step in range(500):
    brain.step(step)
    
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")

# Save brain state for later
brain.save("checkpoint_brain.bin")
```

## Agent Integration

### AgentBrain Class

The `AgentBrain` class bridges the brain and the world, handling:

- Sensory input processing
- Motor command decoding
- Neuromodulation (reward learning)
- Developmental updates

#### Agent Integration Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
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
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

## World Simulation

### SimpleWorld Class

The `SimpleWorld` class provides a 2D simulation environment with:

- Grid-based world representation
- Agent body state (position, velocity, energy)
- Sensory perception (vision, touch, internal signals)
- Object management (resources, hazards, walls)

#### World Usage Example

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
world.reset()
world.setAgentStart(10.0, 10.0)
world.setMaxEnergy(100.0)
world.setRandomSeed(42)

# Add objects to the world
world.addObject(pynlm.WorldObject(
    x=15.0, y=15.0, 
    type=pynlm.WorldObjectType.Resource, 
    value=10.0
))

world.addObject(pynlm.WorldObject(
    x=5.0, y=5.0, 
    type=pynlm.WorldObjectType.Hazard, 
    value=-5.0
))

# Run simulation loop
for step in range(200):
    # Update world state
    world.update(timestep=0.1)
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Check agent status
    body = world.getAgentBody()
    print(f"Step {step}: Agent at ({body.x:.1f}, {body.y:.1f}), "
          f"Energy: {body.energy:.1f}")
    
    # Example: Check if agent is near a resource
    vision = percept.getVision()
    if any(v > 0.5 for v in vision):
        print("  Agent detecting something in vision")

# Check valid positions
is_valid = world.isValidPosition(10.0, 10.0)
print(f"Position (10, 10) is valid: {is_valid}")
```

## Enums

### NeuronType

```python
# Neuron types in the brain
pynlm.NeuronType.Excitatory   # Neuron that excites other neurons
pynlm.NeuronType.Inhibitory   # Neuron that inhibits other neurons
pynlm.NeuronType.Modulatory   # Neuron that modulates other neurons
pynlm.NeuronType.Sensory      # Neuron that processes sensory input
pynlm.NeuronType.Motor        # Neuron that produces motor output
pynlm.NeuronType.Internal     # Internal state neuron
```

### DevelopmentalStage

```python
# Brain development stages
pynlm.DevelopmentalStage.Initial         # Embryonic stage
pynlm.DevelopmentalStage.CriticalPeriod  # Rapid development phase
pynlm.DevelopmentalStage.Maturation       # Refinement phase
pynlm.DevelopmentalStage.Adult           # Mature brain
pynlm.DevelopmentalStage.Aging           # Declining performance
```

### ActionType

```python
# Action types for motor output
pynlm.ActionType.MoveForward  # Move forward
pynlm.ActionType.TurnLeft     # Turn left
pynlm.ActionType.Interact     # Interact with object
pynlm.ActionType.Eat         # Consume resource
# ... and more
```

### WorldObjectType

```python
# Object types in the world
pynlm.WorldObjectType.Empty    # Empty space
pynlm.WorldObjectType.Resource  # Resource (positive reward)
pynlm.WorldObjectType.Hazard   # Hazard (negative reward)
pynlm.WorldObjectType.Wall     # Wall/obstacle
pynlm.WorldObjectType.Marker   # Navigation marker
```

## Error Handling

The module provides comprehensive error handling:

```python
import pynlm

try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Your simulation code here
    
except pynlm.PyNLMInitializationError as e:
    print(f"Brain initialization failed: {e}")
except pynlm.PyNLMConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.PyNLMRuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Tips

### For Long Simulations

1. **Checkpoint frequently**:
```python
# Save every 1000 steps
if step % 1000 == 0:
    brain.save(f"checkpoint_step_{step}.bin")
```

2. **Monitor memory usage**:
```python
neurons = brain.getTotalNeuronCount()
synapses = brain.getTotalSynapseCount()
print(f"Brain size: {neurons} neurons, {synapses} synapses")
```

3. **Use appropriate timestep**:
```python
# Smaller timestep for accuracy, larger for performance
brain.step(step, time=step * 0.01)  # 10ms timestep
```

### For Multiple Agents

```python
import threading

def run_agent(agent_id, config, world_config):
    """Run an agent in its own thread."""
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(**world_config)
    world.reset()
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    print(f"Agent {agent_id} completed simulation")

# Run multiple agents in parallel
threads = []
for i in range(4):
    thread = threading.Thread(target=run_agent, args=(i, config, world_config))
    threads.append(thread)
    thread.start()

for thread in threads:
    thread.join()
```

## Advanced Features

### Custom Configuration

Create custom brain configurations:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Override specific parameters
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("neuromod.dopamine.scale", 2.0)

# Create brain with custom config
brain = pynlm.createBrain(config)
```

### Development Monitoring

Monitor brain development:

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Track development over time
for step in range(1000):
    brain.step(step)
    
    stage = brain.getDevelopmentalStage()
    print(f"Step {step}: {stage}")
    
    if stage == pynlm.DevelopmentalStage.Adult:
        print("Brain has reached adulthood!")
        break
```

### Object Interaction

Interact with world objects:

```python
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)

# Place objects
world.addObject(pynlm.WorldObject(3, 3, pynlm.WorldObjectType.Resource, 5.0))
world.addObject(pynlm.WorldObject(7, 7, pynlm.WorldObjectType.Hazard, -2.0))

# Check agent position
agent_body = world.getAgentBody()
obj_at_pos = world.getObjectAt(agent_body.x, agent_body.y)

if obj_at_pos and obj_at_pos.type == pynlm.WorldObjectType.Resource:
    print(f"Found resource worth {obj_at_pos.value}")
elif obj_at_pos and obj_at_pos.type == pynlm.WorldObjectType.Hazard:
    print(f"Warning: Hazard at ({obj_at_pos.x}, {obj_at_pos.y})")
```

## Troubleshooting

### Common Issues

**Issue: "ImportError: No module named 'pynlm'"**
- Solution: Ensure the module is installed. Try `pip install pynlm`

**Issue: "Segmentation fault during simulation"**
- Solution: Check that `brain.initialize()` is called before `brain.step()`
- Ensure configuration values are valid

**Issue: "Brain not learning"**
- Solution: Enable learning subsystems
```python
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
```

**Issue: "Agent not moving"**
- Solution: Check that `world.update()` and `world.applyMotorCommand()` are called
- Verify that `agent.decodeMotorCommand()` returns a valid command

### Debug Mode

Enable debug logging:

```python
import logging
logging.basicConfig(level=logging.DEBUG)
import pynlm

# Your simulation code here
```

## API Reference

### Config Class Methods

- `createDefaultConfig()`: Create default configuration
- `has(key)`: Check if key exists
- `get(key)`: Get value by key
- `getOr(key, default)`: Get value with default
- `set(key, value)`: Set value
- `remove(key)`: Remove key
- `getKeys()`: Get all keys
- `clear()`: Clear all settings
- `loadFromFile(filepath)`: Load from JSON file
- `saveToFile(filepath)`: Save to JSON file

### Brain Class Methods

- `createBrain(config)`: Create brain instance
- `initialize()`: Initialize brain
- `step(currentStep)`: Run simulation step
- `step(currentStep, currentTime)`: Run step with timestamp
- `receiveSensoryInput(input)`: Inject sensory input
- `produceAction()`: Get motor action
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state
- `addRegion(name)`: Add neural region
- `getRegion(id)`: Get region by ID
- `getTotalNeuronCount()`: Get total neuron count
- `getTotalSynapseCount()`: Get total synapse count
- `getFiringNeuronCount()`: Get currently firing neurons
- `getAverageFiringRate()`: Get average firing rate
- `getExcitationInhibitionRatio()`: Get E/I balance
- `getTotalSpikeCount()`: Get total spike count
- `getDevelopmentalStage()`: Get current developmental stage

### AgentBrain Class Methods

- `createAgentBrain(brain)`: Create agent brain interface
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command
- `applyRewardModulation(reward, predictedReward)`: Apply reward modulation
- `updateDevelopment(timestep)`: Update development system
- `getCuriosityLevel()`: Get curiosity level
- `getNoveltyLevel()`: Get novelty level
- `getPredictionError()`: Get prediction error
- `getNeuromodulationLevel()`: Get neuromodulation level
- `reset()`: Reset for new episode
- `enableRewardModulation(bool)`: Enable/disable reward modulation
- `enableStructuralPlasticity(bool)`: Enable/disable structural plasticity
- `enableDevelopment(bool)`: Enable/disable development
- `enableCuriosity(bool)`: Enable/disable curiosity

### SimpleWorld Class Methods

- `createSimpleWorld()`: Create simple world instance
- `configure(width, height, visionWidth, visionHeight)`: Configure world
- `reset()`: Reset world state
- `setAgentStart(x, y)`: Set agent starting position
- `update(timestep)`: Update world state
- `applyMotorCommand(cmd, currentTime)`: Apply motor command
- `getSensoryPercept()`: Get sensory percept
- `getAgentBody()`: Get agent body state
- `addObject(obj)`: Add object to world
- `removeObject(x, y)`: Remove object at position
- `isValidPosition(x, y)`: Check if position is valid
- `getWidth()`, `getHeight()`: Get world dimensions
- `getSimulationTime()`: Get simulation time
- `getMaxEnergy()`, `setMaxEnergy(e)`: Get/set max energy

## License

MIT License. See LICENSE file for details.

## Copyright

© 2024 NLM Project Authors. All rights reserved.

---

For more information, visit: https://github.com/nlm-project/nlm
