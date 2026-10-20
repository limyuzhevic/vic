# NLM (Neural Learning Machine) - Python Bindings Documentation

## Overview

This documentation provides comprehensive information about the NLM Python bindings, which offer a high-level Python interface to the NLM (Neural Learning Machine) framework. The Python bindings enable users to create, configure, and simulate neural brain models using Python, making it easier to integrate NLM with existing Python workflows and tools.

## Installation

### Prerequisites

- Python 3.8 or higher
- CMake 3.16 or higher
- Build system (make, ninja, or Visual Studio)

### Installation Methods

#### 1. Using pip (Recommended for Production)

```bash
# Install the package with all dependencies
pip install pynlm
```

#### 2. From Source (Recommended for Development)

```bash
# Clone the repository
git clone https://github.com/yourusername/NLM.git
cd NLM

# Install the package in development mode
pip install -e .

# Or build and install separately
pip install build
python -m build
pip install dist/*.whl
```

#### 3. Manual Build

```bash
# Create build directory
mkdir build
cd build

# Configure with Python bindings
 cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_BINDINGS=ON

# Build the project
make -j$(nproc)

# Install Python bindings
pip install -e ..
```

### Verification

After installation, verify that the package is correctly installed:

```python
import pynlm
print(f"NLM Python bindings version: {pynlm.__version__}")

# Create a simple brain to test
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Successfully created brain with {brain.getTotalNeuronCount()} neurons")
```

## Python API

### Core Classes

#### Brain

The `Brain` class represents the central neural processor in the NLM framework. It implements spiking neural computation with event-driven dynamics and integrated memory, prediction, cognition, and neuromodulation systems.

**Constructor**:

```python
brain = pynlm.Brain(config)
```

**Parameters**:
- `config`: Configuration object (`pynlm.Config`)

**Methods**:

##### Initialization

```python
brain.initialize()
brain.initialize(True)  # Enable detailed logging
brain.initialize(False) # Disable logging
```

**Description**: Initialize the brain with the given configuration.

##### Simulation

```python
brain.step(step_number)
brain.step(step_number, timestamp)
```

**Parameters**:
- `step_number`: Current simulation step
- `timestamp`: Current simulation timestamp (optional)

**Description**: Execute one simulation step.

##### Input Processing

```python
brain.receiveSensoryInput(sensory_input)
brain.injectCurrent(neuron_id, current)
brain.injectCurrentToNeurons(neuron_type, current)
```

**Parameters**:
- `sensory_input`: Sensory input object (`pynlm.SensoryInput`)
- `neuron_id`: Neuron identifier (`pynlm.NeuronId`)
- `current`: Membrane current (float)
- `neuron_type`: Neuron type (`pynlm.NeuronType`)

**Description**: Inject external current into neurons.

##### Output Production

```python
action = brain.produceAction()
```

**Returns**:
- `action`: Generated action (`pynlm.Action`)

**Description**: Produce motor/action output based on motor neuron activity.

##### Statistics and Diagnostics

```python
excitatory_inhibition_ratio = brain.getExcitationInhibitionRatio()
total_spike_count = brain.getTotalSpikeCount()
pending_spike_event_count = brain.getPendingSpikeEventCount()
average_firing_rate = brain.getAverageFiringRate()
total_neuron_count = brain.getTotalNeuronCount()
total_synapse_count = brain.getTotalSynapseCount()
firing_neuron_count = brain.getFiringNeuronCount()
```

**Returns**:
- Various statistics about brain activity

**Description**: Get statistics and diagnostic information.

##### State Management

```python
brain.save(filename)
brain.load(filename)
brain.reset()
```

**Parameters**:
- `filename`: File path for checkpoint

**Description**: Save/load brain state, or reset to initial state.

##### Region Management

```python
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

**Parameters**:
- `name`: Region name (string)

**Returns**:
- `region_id`: Region identifier (`pynlm.RegionId`)
- `region`: Neural region object (`pynlm.NeuralRegion`)

**Description**: Add or get brain regions.

##### Neural System Access

```python
stdp = brain.getSTDP()
hebbian = brain.getHebbian()
structural_plasticity = brain.getStructuralPlasticity()
spike_system = brain.getSpikeSystem()
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()
prediction_system = brain.getPredictionSystem()
planner = brain.getPlanner()
attention = brain.getAttention()
concept_formation = brain.getConceptFormation()
dopamine = brain.getDopamine()
novelty = brain.getNovelty()
curiosity = brain.getCuriosity()
prediction_error = brain.getPredictionError()
development_system = brain.getDevelopmentSystem()
```

**Returns**:
- Neural system objects (`pynlm.*`)

**Description**: Access individual neural systems.

#### Config

The `Config` class manages configuration parameters for the brain system with validation, history tracking, and session management.

**Constructor**:

```python
config = pynlm.Config()
config = pynlm.createDefaultConfig()
```

**Methods**:

##### Parameter Management

```python
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp_learning_rate", 0.001)

neurons = config.get("brain.neuron_count")
rate = config.get("plasticity.stdp_learning_rate")

if config.has("brain.neuron_count"):
    # Parameter exists
    pass

keys = config.getKeys()
category_keys = config.getKeys("brain")
```

**Parameters**:
- `key`: Configuration parameter key (e.g., "brain.neuron_count")
- `value`: Parameter value

**Returns**:
- Parameter value or list of keys

**Description**: Get/set configuration parameters.

##### File I/O

```python
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

**Parameters**:
- `filename`: Configuration file path

**Returns**:
- `bool`: True if operation successful

**Description**: Load/save configuration from/to files.

##### Session Management

```python
config.startNewSession("experiment_1")
config.endSession()
current_session = config.getCurrentSessionId()

config.undoLastChange()
history_size = config.getHistorySize()
```

**Parameters**:
- `session_id`: Session identifier

**Returns**:
- `bool`: Success status
- `int`: History size

**Description**: Manage configuration sessions and history.

#### AgentBrain

The `AgentBrain` class provides the interface between the NLM brain and the external world, handling sensory transduction, motor decoding, and neuromodulation.

**Constructor**:

```python
agent_brain = pynlm.AgentBrain(brain)
```

**Parameters**:
- `brain`: Brain instance (`pynlm.Brain`)

**Methods**:

##### Initialization

```python
agent_brain.initialize(world)
```

**Parameters**:
- `world`: World interface (`pynlm.SimpleWorld`)

**Description**: Initialize the agent brain with a world interface.

##### Subsystem Control

```python
agent_brain.enableRewardModulation(True)
agent_brain.enableStructuralPlasticity(True)
agent_brain.enableDevelopment(True)
agent_brain.enableCuriosity(True)
```

**Parameters**:
- `enabled`: Boolean flag

**Description**: Enable/disable agent subsystems.

##### Sensory Processing

```python
agent_brain.processSensoryInput(percept)
```

**Parameters**:
- `percept`: Sensory perception (`pynlm.SensoryPercept`)

**Description**: Process sensory input from the environment.

##### Motor Decoding

```python
action = agent_brain.decodeMotorCommand()
```

**Returns**:
- `action`: Generated action (`pynlm.Action`)

**Description**: Decode motor command from brain activity.

##### Neuromodulation

```python
agent_brain.applyRewardModulation(reward, predicted_reward)
```

**Parameters**:
- `reward`: Actual reward received
- `predicted_reward`: Predicted reward

**Description**: Apply reward modulation to plasticity.

##### Development Updates

```python
agent_brain.updateDevelopment(timestep)
stage = agent_brain.getDevelopmentalStage()
```

**Parameters**:
- `timestep`: Time step for development update

**Returns**:
- `stage`: Developmental stage (`pynlm.DevelopmentalStage`)

**Description**: Update developmental stage and get current stage.

##### Statistics

```python
curiosity_level = agent_brain.getCuriosityLevel()
novelty_level = agent_brain.getNoveltyLevel()
prediction_error = agent_brain.getPredictionError()
neuromodulation_level = agent_brain.getNeuromodulationLevel()
```

**Returns**:
- Various neuromodulation levels

**Description**: Get neuromodulation statistics.

##### State Management

```python
agent_brain.save("checkpoint.bin")
agent_brain.load("checkpoint.bin")
```

**Parameters**:
- `filename`: Checkpoint file path

**Description**: Save/load agent brain state.

#### SimpleWorld

The `SimpleWorld` class provides a basic 2D environment for agent interaction and testing.

**Constructor**:

```python
world = pynlm.SimpleWorld()
world = pynlm.createSimpleWorld()
```

**Methods**:

##### Configuration

```python
world.configure(width=100, height=100, visionWidth=8, visionHeight=8)
world.setMaxEnergy(100.0)
world.setTemperature(25.0)
```

**Parameters**:
- `width`: World width
- `height`: World height
- `visionWidth`: Vision sensor width
- `visionHeight`: Vision sensor height
- `max_energy`: Maximum agent energy
- `temperature`: World temperature

**Description**: Configure world parameters.

##### Control

```python
world.reset()
world.update(timestep)
world.setRandomSeed(42)
```

**Parameters**:
- `timestep`: Time step for update
- `seed`: Random seed for reproducibility

**Description**: Control world simulation.

##### Agent Interaction

```python
world.setAgentStart(x, y)
world.applyMotorCommand(action, current_time)
```

**Parameters**:
- `x`, `y`: Agent start position
- `action`: Action to apply
- `current_time`: Current simulation time

**Description**: Set agent start position or apply motor command.

##### State Access

```python
percept = world.getSensoryPercept()
body = world.getAgentBody()
```

**Returns**:
- `percept`: Sensory perception
- `body`: Agent body state

**Description**: Get current world state.

##### Object Management

```python
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
is_valid = world.isValidPosition(x, y)
```

**Parameters**:
- `x`, `y`: Object position
- `object`: World object
- `value`: Object value (for resources)

**Returns**:
- `bool`: Position validity

**Description**: Add/remove world objects.

##### Properties

```python
world_width = world.getWidth()
world_height = world.getHeight()
simulation_time = world.getSimulationTime()
max_energy = world.getMaxEnergy()
```

**Returns**:
- World properties

**Description**: Get world properties.

#### SensoryInput

Base class for sensory data from the environment.

**Subclasses**:

##### Vision

```python
visual_input = pynlm.VisionInput(width, height, data)
```

**Parameters**:
- `width`: Input width
- `height`: Input height
- `data`: Input data (list or array)

**Methods**:

```python
visual_input.getWidth()
visual_input.getHeight()
visual_input.getData()
visual_input.getNormalized()
```

##### Proprioception

```python
proprioceptive_input = pynlm.ProprioceptiveInput(position, velocity)
```

**Parameters**:
- `position`: Agent position
- `velocity`: Agent velocity

**Methods**:

```python
proprioceptive_input.getPosition()
proprioceptive_input.getVelocity()
```

##### Internal Signals

```python
internal_input = pynlm.InternalInput(internal_state)
```

**Parameters**:
- `internal_state`: Internal neural state

**Methods**:

```python
internal_input.getInternalState()
internal_input.getEmotionalState()
```

#### SensoryPercept

Represents combined sensory input from multiple modalities.

**Constructor**:

```python
percept = pynlm.SensoryPercept(visual, proprioceptive, internal)
```

**Parameters**:
- `visual`: Visual input
- `proprioceptive`: Proprioceptive input
- `internal`: Internal signals

**Methods**:

```python
visual = percept.getVisual()
proprioceptive = percept.getProprioceptive()
internal = percept.getInternal()

percept.getTotalSignalStrength()
percept.getDominantModality()
```

#### Action

Base class for motor commands and actions.

**Subclasses**:

##### MoveForward

```python
action = pynlm.MoveForward(direction, speed)
```

**Parameters**:
- `direction`: Movement direction
- `speed`: Movement speed

**Methods**:

```python
action.getDirection()
action.getSpeed()
action.execute()
```

##### TurnLeft

```python
action = pynlm.TurnLeft(angle)
```

**Parameters**:
- `angle`: Turn angle (degrees)

**Methods**:

```python
action.getAngle()
```

##### Interact

```python
action = pynlm.Interact(object_type, object_id)
```

**Parameters**:
- `object_type`: Type of object to interact with
- `object_id`: Object identifier

##### Eat

```python
action = pynlm.Eat(resource_type, amount)
```

**Parameters**:
- `resource_type`: Type of resource
- `amount`: Amount to consume

#### WorldObject

Represents objects in the world environment.

**Constructor**:

```python
world_object = pynlm.WorldObject(x, y, object_type, value=1.0)
```

**Parameters**:
- `x`, `y`: Object position
- `object_type`: Object type (`pynlm.WorldObjectType`)
- `value`: Object value (for resources)

**Properties**:

```python
world_object.x
world_object.y
world_object.type
world_object.value
```

**Enumeration**:

- `pynlm.WorldObjectType.Empty`
- `pynlm.WorldObjectType.Resource`
- `pynlm.WorldObjectType.Hazard`
- `pynlm.WorldObjectType.Wall`
- `pynlm.WorldObjectType.Marker`

#### NeuronType

Enumeration of neuron types:

- `pynlm.NeuronType.Excitatory`
- `pynlm.NeuronType.Inhibitory`
- `pynlm.NeuronType.Sensory`
- `pynlm.NeuronType.Motor`
- `pynlm.NeuronType.Modulatory`
- `pynlm.NeuronType.Internal`

#### DevelopmentalStage

Enumeration of developmental stages:

- `pynlm.DevelopmentalStage.Initial`
- `pynlm.DevelopmentalStage.CriticalPeriod`
- `pynlm.DevelopmentalStage.Maturation`
- `pynlm.DevelopmentalStage.Adult`
- `pynlm.DevelopmentalStage.Aging`

#### ActionType

Enumeration of action types:

- `pynlm.ActionType.MoveForward`
- `pynlm.ActionType.TurnLeft`
- `pynlm.ActionType.Interact`
- `pynlm.ActionType.Eat`

#### WorldObjectType

Enumeration of world object types:

- `pynlm.WorldObjectType.Empty`
- `pynlm.WorldObjectType.Resource`
- `pynlm.WorldObjectType.Hazard`
- `pynlm.WorldObjectType.Wall`
- `pynlm.WorldObjectType.Marker`

## Python-Specific Features

### Enhanced Error Handling

The Python bindings provide enhanced error handling with custom exception classes:

```python
try:
    brain.initialize()
except pynlm.InitializationError as e:
    print(f"Brain initialization failed: {e}")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
```

### Advanced Configuration

#### Configuration Templates

```python
# Create different configuration templates
default_config = pynlm.createDefaultConfig()
optimized_config = pynlm.createOptimizedConfig(2000, True)
development_config = pynlm.createDevelopmentConfig()
production_config = pynlm.createProductionConfig()
```

#### Configuration Validation

```python
# Validate configuration
if not config.validate():
    print("Configuration validation failed")
    print("Errors:", config.getValidationErrors())

# Validate specific parameters
if config.validateParameter("brain.neuron_count"):
    print("Neuron count is valid")
```

### Performance Optimization

#### Multi-threading Support

```python
# Enable multi-threading
config.set("performance.parallel_threads", 4)

# Use thread pool for simulation
with pynlm.ThreadPoolExecutor(max_workers=4) as executor:
    futures = []
    for step in range(100):
        future = executor.submit(brain.step, step)
        futures.append(future)
    
    # Wait for all futures to complete
    for future in futures:
        future.result()
```

#### Memory Management

```python
# Use memory pools for large simulations
brain.enableMemoryPool(1024)  # 1GB memory pool

# Monitor memory usage
memory_usage = brain.getMemoryUsage()
peak_memory = brain.getPeakMemoryUsage()

print(f"Current memory usage: {memory_usage / 1024 / 1024:.2f} MB")
print(f"Peak memory usage: {peak_memory / 1024 / 1024:.2f} MB")
```

### Advanced Simulation Features

#### Simulation Monitoring

```python
import time

# Monitor simulation performance
start_time = time.time()
steps_completed = 0

for step in range(1000):
    brain.step(step)
    steps_completed += 1
    
    # Log progress every 100 steps
    if step % 100 == 0:
        elapsed = time.time() - start_time
        fps = steps_completed / elapsed
        print(f"Step {step}: {fps:.2f} FPS")

# Get final statistics
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

#### Checkpoint Management

```python
# Create checkpoints
brain.save("checkpoint_step_100.bin")
brain.save("checkpoint_step_500.bin")

# Restore from checkpoint
brain.load("checkpoint_step_100.bin")

# Automatic checkpointing
brain.enableAutomaticCheckpointing("checkpoints/", 100)  # Every 100 steps
```

#### Visualization Support

```python
# Get neural activity data for visualization
neural_activity = brain.getNeuralActivityData()
spike_times = brain.getSpikeTimes()

# Export for visualization tools
import numpy as np
np.save("neural_activity.npy", neural_activity)
np.save("spike_times.npy", spike_times)
```

### Integration with Python Ecosystem

#### NumPy Integration

```python
import numpy as np

# Convert Python lists to numpy arrays
sensor_data = np.array([1.0, 2.0, 3.0, 4.0, 5.0])

# Create sensory input from numpy array
sensory_input = pynlm.VisionInput(5, 1, sensor_data)

# Get neural data as numpy array
neural_data = brain.getNeuralData()
spike_data = np.array(brain.getSpikeData())
```

#### Matplotlib Integration

```python
import matplotlib.pyplot as plt

# Plot simulation results
steps = list(range(100))
spike_counts = [brain.getSpikeCountAtStep(step) for step in steps]

plt.figure(figsize=(10, 6))
plt.plot(steps, spike_counts)
plt.xlabel('Step')
plt.ylabel('Spike Count')
plt.title('Neural Activity Over Time')
plt.grid(True)
plt.show()
```

#### Pandas Integration

```python
import pandas as pd

# Create DataFrame for simulation results
results = {
    'step': list(range(100)),
    'spike_count': [brain.getSpikeCountAtStep(step) for step in range(100)],
    'firing_rate': [brain.getFiringRateAtStep(step) for step in range(100)],
    'energy': [brain.getEnergyAtStep(step) for step in range(100)]
}

df = pd.DataFrame(results)
df.to_csv('simulation_results.csv', index=False)
```

## Advanced Usage Examples

### Example 1: Complete Agent Simulation

```python
import pynlm
import time

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent_brain = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent_brain.initialize(world)
    
    # 6. Enable subsystems
    agent_brain.enableRewardModulation(True)
    agent_brain.enableStructuralPlasticity(True)
    agent_brain.enableDevelopment(True)
    agent_brain.enableCuriosity(True)
    
    # 7. Run simulation loop
    start_time = time.time()
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent_brain.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        action = agent_brain.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent_brain.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent_brain.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            elapsed = time.time() - start_time
            print(f"Step {step} ({elapsed:.2f}s):")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent_brain.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent_brain.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {agent_brain.getDevelopmentalStage()}")
    
    # 8. Final statistics
    elapsed = time.time() - start_time
    print(f"\nSimulation completed in {elapsed:.2f} seconds")
    print(f"Average steps per second: {num_steps / elapsed:.2f}")
    
    return {
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'final_curiosity': agent_brain.getCuriosityLevel(),
        'final_novelty': agent_brain.getNoveltyLevel(),
        'final_development_stage': agent_brain.getDevelopmentalStage()
    }

# Run the simulation
results = run_agent_simulation(1000)
print("\nSimulation Results:", results)
```

### Example 2: Learning with Curiosity

```python
import pynlm
import random

def curiosity_driven_learning(num_steps=500):
    """Run a simulation where curiosity drives exploration."""
    
    # Create configuration with high curiosity
    config = pynlm.createDefaultConfig()
    config.set("neuromod.curiosity.threshold", 0.1)
    config.set("neuromod.curiosity.learning_rate", 0.05)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent_brain = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    agent_brain.initialize(world)
    
    # Enable curiosity and exploration
    agent_brain.enableCuriosity(True)
    agent_brain.enableNovelty(True)
    
    # Track novel experiences
    novel_experiences = []
    exploration_count = 0
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        
        # Check for novelty
        novelty = agent_brain.getNoveltyLevel()
        if novelty > 0.5:
            novel_experiences.append(step)
            exploration_count += 1
        
        # Run brain
        brain.step(step)
        
        # Decode action
        action = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply curiosity-driven exploration
        curiosity = agent_brain.getCuriosityLevel()
        if curiosity > 0.3:
            # Add random exploration action
            random_action = pynlm.MoveForward(random.uniform(0, 360), 0.5)
            world.applyMotorCommand(random_action, world.getSimulationTime())
        
        # Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent_brain.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent_brain.updateDevelopment(0.1)
        
        # Print exploration statistics
        if step % 100 == 0:
            print(f"Step {step}: Novelty={novelty:.3f}, Curiosity={curiosity:.3f}, "
                  f"Exploration={exploration_count}, Novel Experiences={len(novel_experiences)}")
    
    print(f"\nCuriosity-driven learning results:")
    print(f"  Total novel experiences: {len(novel_experiences)}")
    print(f"  Total exploration actions: {exploration_count}")
    print(f"  Novelty at final step: {agent_brain.getNoveltyLevel():.3f}")
    print(f"  Curiosity at final step: {agent_brain.getCuriosityLevel():.3f}")
    
    return {
        'novel_experiences': novel_experiences,
        'exploration_count': exploration_count,
        'final_novelty': agent_brain.getNoveltyLevel(),
        'final_curiosity': agent_brain.getCuriosityLevel()
    }

# Run curiosity-driven learning
results = curiosity_driven_learning(500)
```

### Example 3: Development Simulation

```python
import pynlm
import matplotlib.pyplot as plt

def simulate_development(num_steps=1000):
    """Simulate neural development over time."""
    
    # Create configuration with development enabled
    config = pynlm.createDefaultConfig()
    config.set("development.enabled", True)
    config.set("development.stage_duration", 100.0)
    config.set("development.critical_period_start", 20.0)
    config.set("development.critical_period_end", 80.0)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent_brain = pynlm.createAgentBrain(brain)
    
    # Create simple world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
    world.reset()
    agent_brain.initialize(world)
    
    # Enable development
    agent_brain.enableDevelopment(True)
    
    # Track developmental metrics
    developmental_stages = []
    plasticity_levels = []
    learning_rates = []
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Decode action
        action = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent_brain.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent_brain.updateDevelopment(0.1)
        
        # Record developmental metrics
        stage = agent_brain.getDevelopmentalStage()
        developmental_stages.append(stage.value)
        
        plasticity = brain.getPlasticityLevel()
        plasticity_levels.append(plasticity)
        
        learning_rate = brain.getLearningRate()
        learning_rates.append(learning_rate)
        
        # Print developmental stage every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: Development Stage={stage.name}, "
                  f"Plasticity={plasticity:.3f}, Learning Rate={learning_rate:.3f}")
    
    # Create plots
    fig, axes = plt.subplots(3, 1, figsize=(12, 10))
    
    # Plot developmental stages
    axes[0].plot(range(num_steps), developmental_stages)
    axes[0].set_ylabel('Developmental Stage')
    axes[0].set_title('Neural Development Over Time')
    axes[0].grid(True)
    
    # Plot plasticity levels
    axes[1].plot(range(num_steps), plasticity_levels)
    axes[1].set_ylabel('Plasticity Level')
    axes[1].set_title('Plasticity During Development')
    axes[1].grid(True)
    
    # Plot learning rates
    axes[2].plot(range(num_steps), learning_rates)
    axes[2].set_ylabel('Learning Rate')
    axes[2].set_xlabel('Step')
    axes[2].set_title('Learning Rate During Development')
    axes[2].grid(True)
    
    plt.tight_layout()
    plt.savefig('development_plots.png')
    plt.show()
    
    # Find critical period
    critical_period_steps = [i for i, stage in enumerate(developmental_stages) 
                           if stage == pynlm.DevelopmentalStage.CriticalPeriod.value]
    
    print(f"\nDevelopment simulation results:")
    print(f"  Total steps: {num_steps}")
    print(f"  Critical period duration: {len(critical_period_steps)} steps")
    print(f"  Average plasticity: {np.mean(plasticity_levels):.3f}")
    print(f"  Average learning rate: {np.mean(learning_rates):.3f}")
    
    return {
        'developmental_stages': developmental_stages,
        'plasticity_levels': plasticity_levels,
        'learning_rates': learning_rates,
        'critical_period_steps': critical_period_steps
    }

# Run development simulation
results = simulate_development(1000)
```

## Performance Considerations

### Memory Management

1. **Memory Pooling**: Use `brain.enableMemoryPool(size_mb)` for large-scale simulations
2. **Checkpointing**: Regularly save brain state to reduce memory usage
3. **Cleanup**: Call `brain.reset()` to clear unused memory

```python
# Enable memory pooling
brain.enableMemoryPool(1024)  # 1GB memory pool

# Monitor memory usage
memory_usage = brain.getMemoryUsage()
peak_memory = brain.getPeakMemoryUsage()

print(f"Current memory usage: {memory_usage / 1024 / 1024:.2f} MB")
print(f"Peak memory usage: {peak_memory / 1024 / 1024:.2f} MB")
```

### Simulation Speed

1. **Multi-threading**: Use `config.set("performance.parallel_threads", 4)` for parallel processing
2. **Optimization**: Use `brain.enableOptimizedMode(True)` for performance-critical applications
3. **Batch Processing**: Process multiple steps in batch for better performance

```python
# Configure for performance
config.set("performance.parallel_threads", 4)
config.set("performance.enable_optimization", True)

# Batch processing
brain.step(0, 100)  # Process steps 0-100 in one call
```

### Scaling Considerations

1. **Large Networks**: Use `OptimizedBrain` for networks with >5000 neurons
2. **Memory Efficiency**: Monitor `brain.getTotalSynapseCount() / brain.getTotalNeuronCount()`
3. **Simulation Speed**: Use `brain.getAverageFiringRate()` to optimize simulation parameters

```python
# For large networks
if brain.getTotalNeuronCount() > 5000:
    optimized_brain = pynlm.OptimizedBrain(brain)
    optimized_brain.initialize()
```

## Error Handling

### Common Exceptions

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.InitializationError as e:
    print(f"Brain initialization failed: {e}")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
```

### Error Recovery

```python
# Save checkpoint before risky operations
brain.save("safe_checkpoint.bin")

# Attempt risky operation
try:
    brain.step(step)
except Exception as e:
    print(f"Error occurred: {e}")
    # Restore from checkpoint
    brain.load("safe_checkpoint.bin")
    print("Restored from checkpoint")
```

## Troubleshooting

### Common Issues

**Issue 1: Brain initialization fails**

```python
# Check configuration
config = pynlm.createDefaultConfig()
if not config.validate():
    print("Configuration validation failed:", config.getValidationErrors())

# Try to get detailed error information
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.InitializationError as e:
    print(f"Initialization error: {e}")
```

**Issue 2: Simulation runs too slowly**

```python
# Enable optimizations
config.set("performance.enable_optimization", True)
config.set("performance.parallel_threads", 4)

# Reduce simulation complexity
config.set("brain.synapse_density", 0.05)
config.set("brain.neuron_count", 500)
```

**Issue 3: Memory usage is too high**

```python
# Monitor memory usage
memory_usage = brain.getMemoryUsage()
print(f"Memory usage: {memory_usage / 1024 / 1024:.2f} MB")

# Enable memory pooling
brain.enableMemoryPool(512)  # 512MB pool

# Clear unused memory
brain.reset()
```

## Integration Examples

### Integration with External Libraries

```python
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get neural data as numpy array
neural_data = brain.getNeuralData()  # Returns numpy array

# Create DataFrame for analysis
df = pd.DataFrame({
    'step': np.arange(len(neural_data)),
    'neural_activity': neural_data.flatten()
})

# Plot using matplotlib
plt.figure(figsize=(12, 6))
plt.plot(df['step'], df['neural_activity'])
plt.xlabel('Step')
plt.ylabel('Neural Activity')
plt.title('Neural Activity Over Time')
plt.grid(True)
plt.show()
```

### Integration with Machine Learning Libraries

```python
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA

# Get neural data
neural_data = brain.getNeuralData()

# Preprocess data
scaler = StandardScaler()
scaled_data = scaler.fit_transform(neural_data)

# Reduce dimensionality
pca = PCA(n_components=2)
pca_data = pca.fit_transform(scaled_data)

# Plot results
plt.figure(figsize=(10, 8))
plt.scatter(pca_data[:, 0], pca_data[:, 1])
plt.xlabel('PC1')
plt.ylabel('PC2')
plt.title('Neural Data PCA')
plt.grid(True)
plt.show()
```

### Integration with Web Frameworks

```python
import flask
from flask import Flask, request, jsonify

app = Flask(__name__)

# Global brain instance
brain = None

@app.route('/initialize', methods=['POST'])
def initialize_brain():
    global brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    return jsonify({'status': 'success', 'neuron_count': brain.getTotalNeuronCount()})

@app.route('/step', methods=['POST'])
def simulate_step():
    global brain
    if brain is None:
        return jsonify({'error': 'Brain not initialized'}), 400
    
    step = request.json.get('step', 0)
    brain.step(step)
    
    return jsonify({
        'step': step,
        'spike_count': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate()
    })

@app.route('/status', methods=['GET'])
def get_status():
    global brain
    if brain is None:
        return jsonify({'error': 'Brain not initialized'}), 400
    
    return jsonify({
        'neuron_count': brain.getTotalNeuronCount(),
        'synapse_count': brain.getTotalSynapseCount(),
        'spike_count': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate()
    })
```

## Advanced Python Features

### Context Managers

```python
# Use brain as context manager
with pynlm.BrainContext(config) as brain:
    brain.initialize()
    for step in range(100):
        brain.step(step)
    # Brain automatically reset when exiting context
```

### Decorators

```python
# Create decorator for simulation monitoring
import time

def monitor_simulation(func):
    def wrapper(*args, **kwargs):
        start_time = time.time()
        result = func(*args, **kwargs)
        elapsed = time.time() - start_time
        print(f"Simulation completed in {elapsed:.2f} seconds")
        return result
    return wrapper

@monitor_simulation
def run_simulation(steps):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    for step in range(steps):
        brain.step(step)
    return brain.getTotalSpikeCount()

# Run monitored simulation
total_spikes = run_simulation(1000)
```

### Generator Functions

```python
# Create generator for step-by-step simulation
def simulate_steps(start_step, num_steps):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for step in range(start_step, start_step + num_steps):
        brain.step(step)
        yield {
            'step': step,
            'spike_count': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate()
        }

# Use generator
for result in simulate_steps(0, 100):
    print(f"Step {result['step']}: {result['spike_count']} spikes")
```

### Asynchronous Programming

```python
import asyncio
import websockets

# Async simulation
async def async_simulation(brain, steps):
    for step in steps:
        brain.step(step)
        await asyncio.sleep(0)  # Yield to event loop
    return brain.getTotalSpikeCount()

# WebSocket server for remote simulation
async def handle_client(websocket, path):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    async for message in websocket:
        if message == 'step':
            brain.step(0)
            await websocket.send(f"Spike count: {brain.getTotalSpikeCount()}")
        elif message == 'status':
            await websocket.send(f"Status: {brain.getTotalNeuronCount()} neurons")

start_server = websockets.serve(handle_client, "localhost", 8765)
asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
```

## Conclusion

The NLM Python bindings provide a powerful and flexible interface to the NLM framework. This documentation covers the core functionality, advanced features, and integration examples for using NLM in Python applications.

The Python bindings enable users to:

1. **Create and configure brains** using Python API
2. **Run simulations** with high-level Python interface
3. **Analyze results** using Python data science libraries
4. **Integrate with external systems** using Python ecosystems
5. **Build complex applications** with Python's flexibility and power

For more information and advanced usage, refer to the API reference and other documentation files in the NLM documentation set.

## References

- [NLM API Reference](API_REFERENCE.md)
- [NLM User Guide](USER_GUIDE.md)
- [NLM Configuration Documentation](CONFIGURATION.md)
- [NLM Architecture Documentation](ARCHITECTURE.md)
- [NLM Testing Documentation](TESTING.md)
- [NLM Examples and Tutorials](EXAMPLES.md)
- [NLM Upgrading Guide](UPGRADING_GUIDE.md)

## See Also

- NLM GitHub repository: https://github.com/yourusername/NLM
- NLM documentation: https://nlm.readthedocs.io/
- NLM Python package: https://pypi.org/project/pynlm/
- NLM discussion forum: https://github.com/yourusername/NLM/discussions

## License

MIT License - See LICENSE file for details.