# NLM Python Bindings - API Reference

This document provides a comprehensive reference for the NLM (Neural Learning Machine) Python bindings.

## Overview

The `pynlm` Python package provides bindings to the NLM C++ neural simulation framework. It exposes classes and functions for building, simulating, and controlling artificial developmental brains.

## Installation

```bash
# Install the package
pip install pynlm

# For development, build from source
mkdir build
cd build
cmake ..
make -j4
python setup.py install
```

## Core Classes

### Brain

The central neural simulation brain class that implements real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

#### Initialization
```python
import pynlm as nlm

# Create a brain with default configuration
brain = nlm.createBrainWithDefaultConfig()

# Create a brain with custom configuration
config = nlm.Config()
brain = nlm.createBrain(config)
```

#### Basic Operations
```python
# Initialize the brain
brain.initialize()

# Perform a simulation step
brain.step(step_id)
brain.step(step_id, timestamp)

# Inject sensory input
vision = nlm.Vision(64, 64, 3)
brain.receiveSensoryInput(vision)

# Inject current
brain.injectCurrent(neuron_id, current_value)
brain.injectCurrentToNeurons(neuron_type, current_value)

# Get actions
action = brain.produceAction()

# Save and load brain state
brain.save("brain_state.pkl")
brain.load("brain_state.pkl")

# Reset brain state
brain.reset()
```

#### Regions and Statistics
```python
# Add a region
region_id = brain.addRegion("visual_cortex")

# Get region information
region = brain.getRegion(region_id)
region_count = brain.getRegionCount()
region_ids = brain.getRegionIds()

# Get system statistics
stats = {
    "total_neurons": brain.getTotalNeuronCount(),
    "total_synapses": brain.getTotalSynapseCount(),
    "active_neurons": brain.getActiveNeuronCount(),
    "firing_neurons": brain.getFiringNeuronCount(),
    "average_firing_rate": brain.getAverageFiringRate(),
    "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
    "total_spikes": brain.getTotalSpikeCount(),
    "pending_spike_events": brain.getPendingSpikeEventCount(),
}

# Get developmental stage
development_stage = brain.getDevelopmentalStage()
brain.setDevelopmentalStage(development_stage)
```

#### Memory Systems
```python
# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Prediction system
prediction_system = brain.getPredictionSystem()

# Cognition systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()

# Development system
development_system = brain.getDevelopmentSystem()

# Neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()

# Plasticity systems
stdp = brain.getSTDP()
hebbian = brain.getHebbian()
structural_plasticity = brain.getStructuralPlasticity()

# Spike system
spike_system = brain.getSpikeSystem()

# Random generator
random_gen = brain.getRandomGenerator()
```

#### Plasticity and Development
```python
# Update plasticity rules (called automatically in step)
brain.updatePlasticity()

# Apply developmental changes (called automatically in step)
brain.develop()

# Apply neuromodulatory signals
neuromodulator = nlm.Dopamine()
brain.applyNeuromodulation(neuromodulator)
```

### AgentBrain

The AgentBrain class connects the NLM brain to the world, handling sensory transduction and motor decoding.

#### Initialization and Processing
```python
import pynlm as nlm

# Create a complete agent system
brain, world, agent_brain = nlm.createCompleteAgentSystem()

# Initialize agent brain
agent_brain.initialize(world)

# Process sensory input
sensory_percept = world.getSensoryPercept()
agent_brain.processSensoryInput(sensory_percept)

# Get motor command
motor_command = agent_brain.decodeMotorCommand()

# Apply reward modulation
agent_brain.applyRewardModulation(reward=1.0, predicted_reward=0.5)

# Update development
agent_brain.updateDevelopment(timestep=0.1)
```

#### State Management
```python
# Get and set agent state
state = agent_brain.getState()
print(f"Developmental stage: {state['developmental_stage']}")
print(f"Curiosity level: {state['curiosity_level']}")
print(f"Reward modulation enabled: {state['reward_modulation_enabled']}")

# Update agent state from dictionary
new_state = {
    "developmental_stage": nlm.DevelopmentalStage.Maturation,
    "reward_modulation_enabled": True,
    "curiosity_enabled": False
}
agent_brain.updateState(new_state)
```

#### Configuration Flags
```python
# Enable/disable systems
agent_brain.setRewardModulationEnabled(True)
agent_brain.setStructuralPlasticityEnabled(True)
agent_brain.setDevelopmentEnabled(True)
agent_brain.setCuriosityEnabled(True)

# Check if systems are enabled
if agent_brain.isRewardModulationEnabled():
    print("Reward modulation is enabled")

if agent_brain.isCuriosityEnabled():
    print("Curiosity is enabled")
```

### SimpleWorld

The SimpleWorld class provides a 2D simulation environment for the NLM agent.

#### World Configuration
```python
import pynlm as nlm

# Create world
world = nlm.createSimpleWorld()

# Configure world dimensions and vision
world.configure(width=100.0, height=100.0, vision_width=20, vision_height=20)

# Set agent starting position
world.setAgentStart(x=50.0, y=50.0)
```

#### World Operations
```python
# Reset world
world.reset()

# Update world state
world.update(timestep=0.1)

# Get sensory percept (for agent brain)
percept = world.getSensoryPercept()

# Get agent body state
agent_body = world.getAgentBody()
print(f"Agent position: ({agent_body.x}, {agent_body.y})")
print(f"Agent orientation: {agent_body.orientation}")
print(f"Agent energy: {agent_body.energy}")
```

#### Object Management
```python
# Add objects to world
world_object = nlm.WorldObject(x=10.0, y=10.0, type=nlm.WorldObjectType.Resource, value=10.0)
world.addObject(world_object)

# Remove object
world.removeObject(x=10.0, y=10.0)

# Check if position is valid
if world.isValidPosition(x=50.0, y=50.0):
    print("Position is valid")
else:
    print("Position is invalid (e.g., wall)")

# Get object at position
obj = world.getObjectAt(x=50.0, y=50.0)
if obj:
    print(f"Object at position: type={obj.type}, value={obj.value}")

# List all objects
objects = world.listObjects()
for obj in objects:
    print(f"Object at ({obj['x']}, {obj['y']}): {obj['type']}")
```

#### World State
```python
# Get world state as dictionary
world_state = world.getState()
print(f"World width: {world_state['width']}")
print(f"World height: {world_state['height']}")
print(f"Agent X: {world_state['agent_x']}")
print(f"Agent Y: {world_state['agent_y']}")
print(f"Agent energy: {world_state['agent_energy']}")
print(f"Simulation time: {world_state['simulation_time']}")

# Update world state from dictionary
new_world_state = {
    "agent_x": 30.0,
    "agent_y": 70.0,
    "max_energy": 2.0,
    "energy_decay_rate": 0.01
}
world.updateState(new_world_state)
```

#### Sensory and Motor Systems
```python
# Vision sensory input
vision = nlm.Vision(width=64, height=64, channels=3)

# Audio sensory input  
sample_rate = 44100
num_samples = 1024
audio = nlm.Audio(sample_rate=sample_rate, num_samples=num_samples)

# Internal signals
internal_signals = nlm.InternalSignals()
internal_signals.addSignal(0.5)
internal_signals.addSignal(0.7)

# Actions and motor commands
action = nlm.Action(action_type=nlm.ActionType.MoveForward, parameters=[1.0])
motor_command = nlm.MotorCommand.MoveForward

# Action results
action_result = nlm.ActionResult(reward=1.0, success=True, message="Moved successfully")
print(f"Action reward: {action_result.reward}")
print(f"Action success: {action_result.success}")
print(f"Action message: {action_result.message}")
```

### SensoryPercept

The SensoryPercept class contains all sensory information from the environment.

#### Accessing Sensory Data
```python
import pynlm as nlm

percept = nlm.SensoryPercept()

# Get vision data
vision = percept.getVision()
width = percept.getVisionWidth()
height = percept.getVisionHeight()

# Get touch data
touch = percept.getTouch()

# Get internal signals
internal = percept.getInternal()

# Get audio data
audio = percept.getAudio()

# Get all signals
signals = percept.getAllSignals()

# Get and set timestamps
timestamp = percept.getTimestamp()
percept.setTimestamp(timestamp)
```

### Config

The Config class manages configuration settings for the NLM system.

#### Configuration Access
```python
import pynlm as nlm

# Create configuration
config = nlm.Config()

# Set configuration values
config.setValue("learning_rate", "0.01")
config.setInt("max_steps", 1000)
config.setFloat("vision_fov", 90.0)
config.setBool("enable_curiosity", True)

# Get configuration values
value = config.getValue("learning_rate")  # Returns string
int_val = config.getInt("max_steps")      # Returns int
float_val = config.getFloat("vision_fov") # Returns float
bool_val = config.getBool("enable_curiosity") # Returns bool

# Check if key exists
if config.has("learning_rate"):
    print("Learning rate is set")

# Get all keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Save and load configuration
config.saveToFile("config.json")

# Load from command line
# config.loadFromArgs(len(sys.argv), sys.argv)
```

## Factory Functions

### Core Factories
```python
import pynlm as nlm

# Create basic components
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()
agent_brain = nlm.createAgentBrain(brain)

# Create complete systems
brain, world, agent_brain = nlm.createCompleteAgentSystem(
    worldWidth=100.0,
    worldHeight=100.0,
    visionWidth=20,
    visionHeight=20
)

# Create demo environment
brain, world, agent_brain = nlm.createDemoEnvironment()
```

### Simulation Helper
```python
import pynlm as nlm

# Perform a complete simulation step
state, world_state = nlm.simulateStep(agent_brain, world, timestep=0.1)

# Run simulation loop
for step in range(100):
    # Get current state
    current_state = agent_brain.getState()
    current_world_state = world.getState()
    
    # Process simulation step
    new_state, new_world_state = nlm.simulateStep(agent_brain, world, timestep=0.1)
    
    # Log progress (could integrate with actual logging system)
    print(f"Step {step}: Agent at ({new_world_state['agent_x']}, {new_world_state['agent_y']})")
```

## Convenience Functions

The module provides convenience functions for common operations:

### Complete Agent System
```python
def createCompleteAgentSystem(worldWidth=100.0, worldHeight=100.0, visionWidth=20, visionHeight=20):
    """Create a complete agent system with brain, world, and agent brain."""
```

### Demo Environment
```python
def createDemoEnvironment():
    """Create a demo environment with default settings."""
```

### Simulation Step
```python
def simulateStep(agentBrain, world, timestep=0.1):
    """Perform a complete simulation step with agent brain and world."""
```

## Constants

The module provides constants for invalid IDs:

```python
import pynlm as nlm

# Invalid IDs for reference
invalid_neuron_id = nlm.INVALID_NEURON_ID
invalid_synapse_id = nlm.INVALID_SYNAPSE_ID
invalid_region_id = nlm.INVALID_REGION_ID
invalid_population_id = nlm.INVALID_POPULATION_ID
```

## Type Definitions

The following enumerations are exposed:

- `NeuronType`: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
- `SynapseType`: Excitatory, Inhibitory, Modulatory, Electrical, GapJunction
- `DevelopmentalStage`: Initial, CriticalPeriod, Maturation, Adult, Aging
- `FiringState`: Resting, Active, Refractory, Inhibited
- `ActionType`: MoveForward, MoveBackward, MoveLeft, MoveRight, TurnLeft, TurnRight, Look, LookUp, LookDown, Interact, Eat, Drink, Rest, Wait, Custom
- `MotorCommand`: MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait
- `WorldObjectType`: Empty, Resource, Hazard, Wall, Marker

## Error Handling

The bindings use standard Python exceptions:

- `RuntimeError` for runtime errors
- `ValueError` for invalid values or arguments
- `KeyError` for missing configuration keys (when accessing via `config.getValue`)

## Performance Tips

1. **Reuse objects**: Reuse Brain, World, and AgentBrain instances when possible to avoid overhead from initialization and cleanup.

2. **Batch operations**: When processing multiple sensory inputs or actions, create them in batches rather than one at a time.

3. **Memory management**: Be aware of C++ object lifetimes. Returned objects (e.g., regions, neurons) should not be deleted explicitly.

4. **Simulation efficiency**: Use the `step()` method with timestamps for more efficient time-based simulations.

## Example: Complete Simulation

Here's a complete example demonstrating how to use the NLM Python bindings:

```python
import pynlm as nlm
import time

# Create agent system
brain, world, agent_brain = nlm.createCompleteAgentSystem(
    worldWidth=200.0,
    worldHeight=200.0,
    visionWidth=32,
    visionHeight=32
)

print("Created agent system successfully")
print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"World dimensions: {world.getWidth()}x{world.getHeight()}")

# Run simulation
for episode in range(5):
    print(f"\n=== Episode {episode} ===")
    
    # Reset for new episode
    brain.reset()
    world.reset()
    agent_brain.reset()
    
    # Initial sensory input
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)
    
    # Run simulation steps
    for step in range(50):
        # Get current state
        agent_state = agent_brain.getState()
        world_state = world.getState()
        
        # Perform simulation step
        new_agent_state, new_world_state = nlm.simulateStep(
            agent_brain, world, timestep=0.1
        )
        
        # Log progress
        if step % 10 == 0:
            print(f"Step {step}: Agent at ({new_world_state['agent_x']:.1f}, {new_world_state['agent_y']:.1f})")
            print(f"  Developmental stage: {new_agent_state['developmental_stage']}")
            print(f"  Curiosity level: {new_agent_state['curiosity_level']:.3f}")
            print(f"  Novelty level: {new_agent_state['novelty_level']:.3f}")
    
    # End of episode statistics
    final_state = agent_brain.getState()
    print(f"Episode {episode} completed:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Final developmental stage: {final_state['developmental_stage']}")

print("\nSimulation completed successfully!")
```

## Migration Guide

### From C++ to Python

If you're migrating from C++ to Python:

1. **Memory management**: In Python, you don't need to explicitly delete objects. Python's garbage collector handles cleanup.

2. **Exception handling**: Use try/except blocks to catch runtime errors.

3. **Pointer to reference**: Instead of using raw pointers, Python bindings typically return references or smart pointers.

4. **Type conversions**: Use the provided convenience methods (e.g., `config.getInt()`, `config.getFloat()`) for type-safe access.

### Upgrading from Older Versions

- **Version 0.1.0**: Initial release with basic bindings. See API reference for available functionality.

This documentation covers the complete API available in the current version. For additional functionality or advanced usage patterns, please refer to the specific class and function documentation.

## References

- NLM (Neural Learning Machine) Project Documentation
- Pybind11 Documentation
- C++ NLM Framework Documentation

For more detailed information about the underlying C++ implementation, refer to the NLM project documentation in the `docs/` directory.

---

*This documentation is automatically generated from the NLM Python bindings source code.*