# NLM API Reference Guide

## Core Brain API

This section provides detailed documentation for the main NLM Python API classes and functions.

## nlm.Brain

### Overview
The `Brain` class is the central component of NLM, representing a simulated neural network that processes information and produces motor outputs.

### Creation and Initialization

#### `createBrain(config)`
Creates a new brain instance with the given configuration.

**Parameters:**
- `config` (nlm.Config): Configuration object defining brain parameters. Use `createDefaultConfig()` for default settings.

**Returns:**
- `nlm.Brain`: A new brain instance

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
```

#### `brain.initialize()`
Initializes the brain, allocating memory and setting up initial state.

**Parameters:**
- None

**Returns:**
- None

**Example:**
```python
brain.initialize()
print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
```

### Simulation Control

#### `brain.step(step_number)`
Executes one simulation step, processing neural dynamics and plasticity.

**Parameters:**
- `step_number` (int): Step number for logging and development tracking

**Returns:**
- None

**Example:**
```python
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### `brain.step(step_number, time)`
Executes one simulation step with a specific time value.

**Parameters:**
- `step_number` (int): Step number for logging
- `time` (float): Current simulation time in milliseconds

**Returns:**
- None

**Example:**
```python
brain.step(step, current_time)
```

#### `brain.reset()`
Resets the brain to its initial state.

**Parameters:**
- None

**Returns:**
- None

**Example:**
```python
brain.reset()
print("Brain reset to initial state")
```

### Sensory and Motor Processing

#### `brain.receiveSensoryInput(sensory_input)`
Receives sensory data from the world and processes it through sensory neurons.

**Parameters:**
- `sensory_input` (nlm.SensoryPercept): Sensory observation containing vision, touch, and internal signals

**Returns:**
- None

**Example:**
```python
percept = world.getSensoryPercept()
brain.receiveSensoryInput(percept)
```

#### `brain.produceAction()`
Decodes motor neuron activity into motor commands.

**Parameters:**
- None

**Returns:**
- `nlm.Action`: Motor command for the agent

**Example:**
```python
action = brain.produceAction()
world.applyMotorCommand(action, current_time)
```

### Memory Systems

#### `brain.getWorkingMemory()`
Retrieves the working memory system.

**Parameters:**
- None

**Returns:**
- `nlm.NeuralWorkingMemory`: Working memory instance or None if not available

**Example:**
```python
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory capacity: {working_memory.getMaxCapacity()}")
```

#### `brain.getEpisodicMemory()`
Retrieves the episodic memory system.

**Parameters:**
- None

**Returns:**
- `nlm.NeuralEpisodicMemory`: Episodic memory instance or None if not available

**Example:**
```python
episodic_memory = brain.getEpisodicMemory()
```

### Statistics and Monitoring

#### `brain.getTotalNeuronCount()`
Gets the total number of neurons in the brain.

**Parameters:**
- None

**Returns:**
- int: Total neuron count

#### `brain.getTotalSynapseCount()`
Gets the total number of synapses in the brain.

**Parameters:**
- None

**Returns:**
- int: Total synapse count

#### `brain.getFiringNeuronCount()`
Gets the number of neurons currently firing.

**Parameters:**
- None

**Returns:**
- int: Number of firing neurons

#### `brain.getAverageFiringRate()`
Gets the average firing rate across all neurons.

**Parameters:**
- None

**Returns:**
- float: Average firing rate (spikes per second)

#### `brain.getExcitationInhibitionRatio()`
Gets the E/I balance of the network.

**Parameters:**
- None

**Returns:**
- float: Excitatory to inhibitory neuron ratio

### State Management

#### `brain.save(filepath)`
Saves the brain state to a file.

**Parameters:**
- `filepath` (str): Path to save the brain state

**Returns:**
- bool: True if save successful, False otherwise

**Example:**
```python
success = brain.save("brain_checkpoint.bin")
if success:
    print("Brain state saved successfully")
```

#### `brain.load(filepath)`
Loads the brain state from a file.

**Parameters:**
- `filepath` (str): Path to load the brain state from

**Returns:**
- bool: True if load successful, False otherwise

**Example:**
```python
success = brain.load("brain_checkpoint.bin")
if success:
    print("Brain state loaded successfully")
```

### Developmental Stages

#### `brain.getDevelopmentalStage()`
Gets the current developmental stage of the brain.

**Parameters:**
- None

**Returns:**
- nlm.DevelopmentalStage: Current developmental stage

**Example:**
```python
stage = brain.getDevelopmentalStage()
print(f"Current developmental stage: {stage}")
```

## nlm.Config

### Overview
The `Config` class manages brain configuration parameters.

### Configuration Methods

#### `createDefaultConfig()`
Creates a configuration with default values.

**Parameters:**
- None

**Returns:**
- nlm.Config: Default configuration

#### `config.set(key, value)`
Sets a configuration parameter.

**Parameters:**
- `key` (str): Configuration path (e.g., "brain.neuron_count")
- `value`: Configuration value

**Returns:**
- None

**Example:**
```python
config.set("brain.neuron_count", 2000)
config.set("brain.v_thresh", -45.0)
config.set("plasticity.stdp.learning_rate", 0.001)
```

#### `config.get(key, default=None)`
Gets a configuration parameter value.

**Parameters:**
- `key` (str): Configuration path
- `default`: Default value if key not found

**Returns:**
- Configuration value

#### `config.has(key)`
Checks if a configuration key exists.

**Parameters:**
- `key` (str): Configuration path to check

**Returns:**
- bool: True if key exists, False otherwise

#### `config.loadFromFile(filepath)`
Loads configuration from a file.

**Parameters:**
- `filepath` (str): Path to configuration file

**Returns:**
- None

#### `config.saveToFile(filepath)`
Saves configuration to a file.

**Parameters:**
- `filepath` (str): Path to save configuration

**Returns:**
- None

#### `config.getKeys()`
Gets all configuration keys.

**Parameters:**
- None

**Returns:**
- list: List of all configuration keys

## nlm.AgentBrain

### Overview
The `AgentBrain` class bridges the NLM brain with the environment, handling sensory processing and motor decoding.

### Creation and Initialization

#### `createAgentBrain(brain)`
Creates an agent brain interface for the given brain.

**Parameters:**
- `brain` (nlm.Brain): Brain instance to interface with

**Returns:**
- nlm.AgentBrain: Agent brain interface

#### `agentBrain.initialize(world)`
Initializes the agent with a world instance.

**Parameters:**
- `world` (nlm.SimpleWorld): World instance to interact with

**Returns:**
- None

### Sensory Processing

#### `agentBrain.processSensoryInput(percept)`
Processes sensory input from the world.

**Parameters:**
- `percept` (nlm.SensoryPercept): Sensory observation from world

**Returns:**
- None

#### `agentBrain.decodeMotorCommand()`
Decodes motor command from brain activity.

**Parameters:**
- None

**Returns:**
- nlm.Action: Motor command for world interaction

### Neuromodulation

#### `agentBrain.applyRewardModulation(reward, predicted_reward)`
Applies reward-based neuromodulation.

**Parameters:**
- `reward` (float): Actual reward received
- `predicted_reward` (float): Predicted reward (for prediction error)

**Returns:**
- None

#### `agentBrain.enableRewardModulation(enabled)`
Enables or disables reward modulation.

**Parameters:**
- `enabled` (bool): Whether to enable reward modulation

**Returns:**
- None

#### `agentBrain.enableDevelopment(enabled)`
Enables or disables development.

**Parameters:**
- `enabled` (bool): Whether to enable development

**Returns:**
- None

#### `agentBrain.enableCuriosity(enabled)`
Enables or disables curiosity-driven exploration.

**Parameters:**
- `enabled` (bool): Whether to enable curiosity

**Returns:**
- None

### Statistics and Monitoring

#### `agentBrain.getCuriosityLevel()`
Gets current curiosity level.

**Parameters:**
- None

**Returns:**
- float: Curiosity level (0.0 to 1.0)

#### `agentBrain.getNoveltyLevel()`
Gets current novelty level.

**Parameters:**
- None

**Returns:**
- float: Novelty level (0.0 to 1.0)

#### `agentBrain.getPredictionError()`
Gets prediction error signal.

**Parameters:**
- None

**Returns:**
- float: Prediction error

#### `agentBrain.getNeuromodulationLevel()`
Gets current neuromodulation level.

**Parameters:**
- None

**Returns:**
- float: Neuromodulation level

#### `agentBrain.updateDevelopment(dt)`
Updates developmental stage.

**Parameters:**
- `dt` (float): Time step for development update

**Returns:**
- None

## nlm.SimpleWorld

### Overview
The `SimpleWorld` class represents a simple 2D environment for agent interaction.

### World Creation and Configuration

#### `createSimpleWorld()`
Creates a new simple world instance.

**Parameters:**
- None

**Returns:**
- nlm.SimpleWorld: New world instance

#### `world.configure(width, height, visionWidth, visionHeight)`
Configures world dimensions and sensory parameters.

**Parameters:**
- `width` (int): World width in cells
- `height` (int): World height in cells
- `visionWidth` (int): Agent vision width
- `visionHeight` (int): Agent vision height

**Returns:**
- None

#### `world.reset()`
Resets world to initial state.

**Parameters:**
- None

**Returns:**
- None

#### `world.update(dt)`
Updates world state.

**Parameters:**
- `dt` (float): Time step for update

**Returns:**
- None

#### `world.setAgentStart(x, y)`
Sets agent starting position.

**Parameters:**
- `x` (float): X coordinate
- `y` (float): Y coordinate

**Returns:**
- None

#### `world.applyMotorCommand(action, time)`
Applies motor command to world.

**Parameters:**
- `action` (nlm.Action): Motor command
- `time` (float): Current simulation time

**Returns:**
- None

### State Access

#### `world.getSensoryPercept()`
Gets current sensory percept.

**Parameters:**
- None

**Returns:**
- nlm.SensoryPercept: Current sensory observation

#### `world.getAgentBody()`
Gets agent body information.

**Parameters:**
- None

**Returns:**
- nlm.AgentBody: Agent body information

#### `world.getWidth()`, `world.getHeight()`
Gets world dimensions.

**Parameters:**
- None

**Returns:**
- int: World dimension

#### `world.getSimulationTime()`
Gets current simulation time.

**Parameters:**
- None

**Returns:**
- float: Current simulation time

### Object Management

#### `world.addObject(object)`
Adds an object to the world.

**Parameters:**
- `object` (nlm.WorldObject): Object to add

**Returns:**
- None

#### `world.removeObject(x, y)`
Removes object at position.

**Parameters:**
- `x` (float): X coordinate
- `y` (float): Y coordinate

**Returns:**
- None

#### `world.isValidPosition(x, y)`
Checks if position is valid.

**Parameters:**
- `x` (float): X coordinate
- `y` (float): Y coordinate

**Returns:**
- bool: True if position is valid

## nlm.Enumerations

### NeuronType
- `NeuronType.Excitatory`: Excitatory neuron
- `NeuronType.Inhibitory`: Inhibitory neuron
- `NeuronType.Sensory`: Sensory neuron
- `NeuronType.Motor`: Motor neuron
- `NeuronType.Modulatory`: Modulatory neuron
- `NeuronType.Internal`: Internal neuron

### DevelopmentalStage
- `DevelopmentalStage.Initial`: Initial stage
- `DevelopmentalStage.CriticalPeriod`: Critical period
- `DevelopmentalStage.Maturation`: Maturation stage
- `DevelopmentalStage.Adult`: Adult stage
- `DevelopmentalStage.Aging`: Aging stage

### ActionType
- `ActionType.MoveForward`: Move forward
- `ActionType.TurnLeft`: Turn left
- `ActionType.TurnRight`: Turn right
- `ActionType.Interact`: Interact with object
- `ActionType.Eat`: Consume resource
- `ActionType.Sleep`: Rest

### WorldObjectType
- `WorldObjectType.Empty`: Empty cell
- `WorldObjectType.Resource`: Resource object
- `WorldObjectType.Hazard`: Hazard object
- `WorldObjectType.Wall`: Wall object
- `WorldObjectType.Marker`: Marker object

## Example Configuration Parameters

### Brain Configuration
- `brain.neuron_count`: Total number of neurons
- `brain.synapse_density`: Synapse connectivity density
- `brain.connection_probability`: Probability of connection between neurons
- `brain.initial_weight_mean`: Mean initial synaptic weight
- `brain.initial_weight_std`: Standard deviation of initial weights
- `brain.v_thresh`: Neuron threshold potential (mV)
- `brain.v_rest`: Resting potential (mV)
- `brain.v_reset`: Reset potential after spike (mV)
- `brain.tau_mem`: Membrane time constant (ms)
- `brain.tau_ref`: Refractory period (ms)

### Plasticity Configuration
- `plasticity.stdp.enable`: Enable STDP
- `plasticity.stdp.learning_rate`: STDP learning rate
- `plasticity.stdp.tau_plus`: STDP time constant (ms)
- `plasticity.stdp.tau_minus`: STDP time constant (ms)
- `plasticity.hebbian.enable`: Enable Hebbian learning
- `plasticity.structural.enable`: Enable structural plasticity

### Neuromodulation Configuration
- `neuromod.dopamine.scale`: Dopamine modulation scale
- `neuromod.curiosity.enable`: Enable curiosity-driven exploration
- `neuromod.novelty.enable`: Enable novelty detection
