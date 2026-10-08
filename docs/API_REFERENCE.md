# NLM API Reference Guide

This document provides comprehensive API documentation for the NLM Python bindings, organized by class and module.

---

## Overview

NLM provides a Python interface to the C++ neural learning machine through pybind11 bindings. The API exposes the core brain simulation components, allowing users to create, control, and analyze artificial neural systems.

---

## Core Classes

### Config Class

The `Config` class manages NLM system configuration parameters.

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `loadFromFile(filepath)` | Load configuration from a JSON file | `filepath: str` | `bool` |
| `loadFromArgs(argc, argv)` | Load configuration from command line arguments | `argc: int, argv: char**` | `bool` |
| `saveToFile(filepath)` | Save configuration to a JSON file | `filepath: str` | `bool` |
| `has(key)` | Check if a configuration key exists | `key: str` | `bool` |
| `getKeys()` | Get all configuration keys | - | `List[str]` |
| `clear()` | Clear all configuration entries | - | `None` |
| `summary()` | Get a summary string of the configuration | - | `str` |

**Example:**
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Set basic configuration
config.set("brain.region_count", 4)
config.set("brain.neurons_per_region", 200)
config.set("memory.working_memory_size", 50)
config.set("visualization.enabled", True)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")
```

### Brain Class

The `Brain` class represents the central neural simulation brain with integrated cognitive systems.

**Constructor:**
```python
Brain(config: Config)
```

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize()` | Initialize the brain with configuration | - | `bool` |
| `step(currentStep)` | Perform a simulation step | `currentStep: SimulationStep` | `None` |
| `step(currentStep, currentTime)` | Perform a simulation step with timestamp | `currentStep: SimulationStep, currentTime: Timestamp` | `None` |
| `receiveSensoryInput(input)` | Inject sensory input into the brain | `input: SensoryInput` | `None` |
| `injectCurrent(neuron, current)` | Inject current into a specific neuron | `neuron: NeuronId, current: MembranePotential` | `None` |
| `injectCurrentToNeurons(type, current)` | Inject current into all neurons of a specific type | `type: NeuronType, current: MembranePotential` | `None` |
| `produceAction()` | Produce motor action based on neural activity | - | `Action` |
| `reset()` | Reset brain state | - | `None` |
| `save(filepath)` | Save brain state to file | `filepath: str` | `bool` |
| `load(filepath)` | Load brain state from file | `filepath: str` | `bool` |

**Properties:**

| Property | Type | Description |
|----------|------|-------------|
| `regions` | `List[NeuralRegion]` | All neural regions in the brain |
| `totalNeuronCount` | `int` | Total neuron count across all regions |
| `totalSynapseCount` | `int` | Total synapse count across all regions |
| `activeNeuronCount` | `int` | Count of active neurons |
| `firingNeuronCount` | `int` | Count of currently firing neurons |
| `averageFiringRate` | `float` | Average firing rate across all neurons |
| `excitatoryInhibitionRatio` | `float` | Excitation/inhibition balance ratio |
| `totalSpikeCount` | `int` | Total spike count |
| `developmentalStage` | `DevelopmentalStage` | Current developmental stage |
| `config` | `Config` | The brain's configuration |

**Example:**
```python
import pynlm

# Create brain with configuration
config = pynlm.createDefaultConfig()
config.set("brain.region_count", 4)
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()

# Perform simulation steps
for i in range(100):
    brain.step(i)
    if i % 10 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
```

### AgentBrain Class

The `AgentBrain` class connects the NLM brain to the world, handling sensory transduction and motor decoding.

**Constructor:**
```python
AgentBrain(brain: Brain)
```

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize(world)` | Initialize with world | `world: SimpleWorld` | `None` |
| `getSensoryInputSize()` | Get expected sensory input size | - | `int` |
| `getMotorOutputSize()` | Get expected motor output size | - | `int` |
| `processSensoryInput(percept)` | Process sensory percept and inject into brain | `percept: SensoryPercept` | `None` |
| `decodeMotorCommand()` | Decode brain motor activity into motor command | - | `MotorCommand` |
| `applyRewardModulation(reward, predictedReward)` | Apply reward-based neuromodulation | `reward: float, predictedReward: float` | `None` |
| `updateDevelopment(timestep)` | Update development system | `timestep: double` | `None` |
| `getDevelopmentalStage()` | Get current developmental stage | - | `DevelopmentalStage` |
| `getNeuromodulationLevel()` | Get current neuromodulation level | - | `float` |
| `getCuriosityLevel()` | Get curiosity level | - | `float` |
| `getNoveltyLevel()` | Get novelty level | - | `float` |
| `getPredictionError()` | Get prediction error | - | `float` |
| `reset()` | Reset agent for new episode | - | `None` |
| `getBrain()` | Get the underlying brain | - | `Brain` |

**Configuration Methods:**

| Method | Description | Parameters |
|--------|-------------|------------|
| `enableRewardModulation(enable)` | Enable/disable reward modulation | `enable: bool` |
| `enableStructuralPlasticity(enable)` | Enable/disable structural plasticity | `enable: bool` |
| `enableDevelopment(enable)` | Enable/disable development | `enable: bool` |
| `enableCuriosity(enable)` | Enable/disable curiosity | `enable: bool` |

**Example:**
```python
import pynlm

# Setup brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run agent-world interaction loop
for step in range(100):
    world.update(0.1)
    
    # Get what agent sees
    percept = world.getSensoryPercept()
    
    # Tell the brain
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Brain decides action
    action = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward information
    reward = world.getAgentBody().energy
    predicted = agent.getPredictionError()
    agent.applyRewardModulation(reward, predicted)
```

### SimpleWorld Class

The `SimpleWorld` class represents a 2D environment for NLM simulation.

**Constructor:**
```python
SimpleWorld()
```

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `configure(width, height, visionWidth, visionHeight)` | Configure world parameters | `width: int, height: int, visionWidth: int, visionHeight: int` | `None` |
| `reset()` | Reset world to initial state | - | `None` |
| `setAgentStart(x, y)` | Set agent starting position | `x: float, y: float` | `None` |
| `update(timestep)` | Update world by timestep seconds | `timestep: float` | `None` |
| `applyMotorCommand(cmd, currentTime)` | Apply motor command to world | `cmd: MotorCommand, currentTime: float` | `None` |
| `getSensoryPercept()` | Get sensory percept from world | - | `SensoryPercept` |
| `getAgentBody()` | Get agent body state | - | `AgentBody` |
| `addObject(obj)` | Add object to world | `obj: WorldObject` | `None` |
| `removeObject(x, y)` | Remove object at position | `x: float, y: float` | `None` |
| `isValidPosition(x, y)` | Check if position is valid | `x: float, y: float` | `bool` |
| `getWidth()` | Get world width | - | `int` |
| `getHeight()` | Get world height | - | `int` |
| `getMaxEnergy()` | Get maximum energy | - | `float` |
| `setMaxEnergy(e)` | Set maximum energy | `e: float` | `None` |
| `getEnergyDecayRate()` | Get energy decay rate | - | `float` |
| `setEnergyDecayRate(r)` | Set energy decay rate | `r: float` | `None` |
| `getSimulationTime()` | Get current simulation time | - | `float` |
| `setRandomSeed(seed)` | Set random seed | `seed: int` | `None` |
| `getRandomSeed()` | Get random seed | - | `int` |

**Example:**
```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(15, 15)

# Add objects
obj = pynlm.WorldObject()
obj.x = 10
obj.y = 10
obj.type = pynlm.WorldObjectType.Resource
obj.value = 10.0
world.addObject(obj)

# Update world
for step in range(50):
    world.update(0.1)
    
    # Get agent perception
    percept = world.getSensoryPercept()
    vision = percept.getVision()
    
    # Check what's visible
    for y in range(len(vision)):
        for x in range(len(vision[0])):
            if vision[y][x] == pynlm.WorldObjectType.Resource:
                print(f"Resource at relative position ({x}, {y})")
    
    # Move agent based on action
    action = pynlm.MotorCommand.MoveForward
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Neural Region Class

The `NeuralRegion` class represents a brain region containing multiple neuron populations and local connectivity.

**Constructor:**
```python
NeuralRegion(id: RegionId)
NeuralRegion(id: RegionId, name: str)
```

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `getId()` | Get region ID | - | `RegionId` |
| `getName()` | Get region name | - | `str` |
| `setName(name)` | Set region name | `name: str` | `None` |
| `addPopulation(size, type)` | Add neuron population | `size: int, type: NeuronType` | `PopulationId` |
| `getPopulation(id)` | Get population by ID | `id: PopulationId` | `NeuralPopulation` |
| `getPopulationCount()` | Get population count | - | `int` |
| `getPopulations()` | Get all populations | - | `List[NeuralPopulation]` |
| `getAllPopulations()` | Get all populations (non-const) | - | `List[NeuralPopulation*]` |
| `addSynapse(source, destination, weight, delay)` | Add synapse | `source: NeuronId, destination: NeuronId, weight: float, delay: Delay` | `SynapseId` |
| `getSynapse(id)` | Get synapse by ID | `id: SynapseId` | `Synapse` |
| `getSynapseCount()` | Get synapse count | - | `int` |
| `getSynapses()` | Get all synapses | - | `List[Synapse]` |
| `getSynapsesFrom(neuron)` | Get synapses originating from neuron | `neuron: NeuronId` | `List[Synapse*>` |
| `getSynapsesTo(neuron)` | Get synapses terminating at neuron | `neuron: NeuronId` | `List<Synapse*>` |
| `getTotalNeuronCount()` | Get total neuron count | - | `int` |
| `getActiveNeuronCount()` | Get active neuron count | - | `int` |
| `getFiringNeuronCount()` | Get firing neuron count | - | `int` |
| `getAverageFiringRate()` | Get average firing rate | - | `float` |
| `getAverageSynapticWeight()` | Get average synaptic weight | - | `float` |
| `getSynapticDensity()` | Get synaptic density | - | `float` |
| `step(currentTime)` | Step all populations and synapses | `currentTime: Timestamp` | `None` |
| `reset()` | Reset all neurons and synapses | - | `None` |
| `initializeRandomConnectivity(rng, connectionProbability, meanWeight, weightVariance)` | Initialize random connectivity | `rng: RandomGenerator, connectionProbability: float, meanWeight: float, weightVariance: float` | `None` |
| `getAllNeurons()` | Get all neurons | - | `List[Neuron*>]` |
| `getAllNeurons() const` | Get all neurons (const) | - | `List<const Neuron*>` |

### Neuron and Synapse Classes

**Neuron Class:**
The `Neuron` class represents individual spiking neurons.

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `injectCurrent(current)` | Inject current into neuron | `current: MembranePotential` | `None` |
| `getId()` | Get neuron ID | - | `NeuronId` |
| `getType()` | Get neuron type | - | `NeuronType` |
| `getState()` | Get neuron state | - | `FiringState` |
| `getMembranePotential()` | Get membrane potential | - | `MembranePotential` |
| `getFiringRate()` | Get firing rate | - | `float` |
| `isActive()` | Check if neuron is active | - | `bool` |
| `isFiring()` | Check if neuron is firing | - | `bool` |

**Synapse Class:**
The `Synapse` class represents connections between neurons.

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `getId()` | Get synapse ID | - | `SynapseId` |
| `getSource()` | Get source neuron ID | - | `NeuronId` |
| `getTarget()` | Get target neuron ID | - | `NeuronId` |
| `getWeight()` | Get synaptic weight | - | `float` |
| `setWeight(weight)` | Set synaptic weight | `weight: float` | `None` |
| `getDelay()` | Get synaptic delay | - | `Delay` |
| `getType()` | Get synapse type | - | `SynapseType` |
| `getState()` | Get synapse state | - | `SynapseState` |

### SensoryInput Classes

**Vision Class:**
Represents visual sensory input (image data).

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `Vision()` | Default constructor | - | `Vision` |
| `Vision(width, height, channels)` | Parameterized constructor | `width: size_t, height: size_t, channels: size_t = 3` | `Vision` |
| `setData(data)` | Set visual data | `data: List[float]` | `None` |
| `getWidth()` | Get image width | - | `size_t` |
| `getHeight()` | Get image height | - | `size_t` |
| `getChannels()` | Get number of channels | - | `size_t` |

**Audio Class:**
Represents audio sensory input.

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `Audio()` | Default constructor | - | `Audio` |
| `Audio(sampleRate, numSamples)` | Parameterized constructor | `sampleRate: size_t, numSamples: size_t` | `Audio` |
| `setData(data)` | Set audio data | `data: List[float]` | `None` |
| `setSampleRate(sampleRate)` | Set sample rate | `sampleRate: size_t` | `None` |
| `getSampleRate()` | Get sample rate | - | `size_t` |
| `getNumSamples()` | Get number of samples | - | `size_t` |

### Action Class

Represents motor actions/output from the brain.

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `Action()` | Default constructor | - | `Action` |
| `Action(type)` | Constructor with type | `type: ActionType` | `Action` |
| `Action(type, parameters)` | Constructor with type and parameters | `type: ActionType, parameters: List[float]` | `Action` |
| `getType()` | Get action type | - | `ActionType` |
| `setType(type)` | Set action type | `type: ActionType` | `None` |
| `getParameters()` | Get action parameters | - | `List[float]` |
| `setParameters(params)` | Set action parameters | `params: List[float]` | `None` |
| `getName()` | Get action name | - | `str` |
| `clone()` | Create a copy | - | `Action` |

### Neuromodulator Classes

**Dopamine Class:**
Reward and reinforcement signaling.

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `Dopamine()` | Default constructor | - | `Dopamine` |
| `getCurrentLevel()` | Get current dopamine level | - | `float` |
| `setLevel(level)` | Set dopamine level | `level: float` | `None` |
| `getPlasticityModulator()` | Get plasticity modulation factor | - | `float` |

**Curiosity Class:**
Exploration motivation and novelty detection.

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `Curiosity()` | Default constructor | - | `Curiosity` |
| `getCurrentLevel()` | Get current curiosity level | - | `float` |
| `setLevel(level)` | Set curiosity level | `level: float` | `None` |
| `getExplorationDrive()` | Get exploration drive | - | `float` |
| `getNoveltyThreshold()` | Get novelty threshold | - | `float` |

**Novelty Class:**
Novelty detection and surprise computation.

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `Novelty()` | Default constructor | - | `Novelty` |
| `getCurrentLevel()` | Get current novelty level | - | `float` |
| `setLevel(level)` | Set novelty level | `level: float` | `None` |
| `getSurpriseScore()` | Get surprise score | - | `float` |

### Memory System Classes

**NeuralWorkingMemory Class:**
Transient working memory with active information maintenance.

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `getActiveMemoryCount()` | Get count of active memories | - | `int` |
| `getMemoryStrength(memoryId)` | Get memory strength | `memoryId: MemoryId` | `float` |
| `recordMemory(pattern, strength)` | Record new memory | `pattern: List<float>, strength: float` | `MemoryId` |
| `retrieveMemory(pattern)` | Retrieve memory by pattern | `pattern: List<float>` | `MemoryId` |
| `decayMemory(memoryId, rate)` | Decay memory | `memoryId: MemoryId, rate: float` | `None` |

**NeuralEpisodicMemory Class:**
Experience storage with episodic memory consolidation.

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `getMemoryCount()` | Get total memory count | - | `int` |
| `recordExperience(percept, step)` | Record experience | `percept: SensoryPercept, step: SimulationStep` | `MemoryId` |
| `triggerReplay()` | Trigger memory replay | - | `None` |
| `shouldConsolidate()` | Check if consolidation needed | - | `bool` |
| `consolidateRecentMemories(count)` | Consolidate recent memories | `count: int` | `None` |
| `attemptRetrieval(percept)` | Attempt memory retrieval | `percept: SensoryPercept` | `bool` |
| `getAverageMemoryStrength()` | Get average memory strength | - | `float` |

**NeuralAssociativeMemory Class:**
Pattern association and Hebbian learning.

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `getAssociationStrength(sourcePattern, targetPattern)` | Get association strength | `sourcePattern: List<float>, targetPattern: List<float>` | `float` |
| `formAssociation(sourcePattern, targetPattern, strength)` | Form new association | `sourcePattern: List<float>, targetPattern: List<float>, strength: float` | `None` |
| `getInterferenceLevel()` | Get interference level | - | `float` |
| `getAverageAssociationStrength()` | Get average association strength | - | `float` |

### Prediction System Classes

**PredictionSystem Class:**
Forward model for sensory prediction and error computation.

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `getPredictionError()` | Get prediction error | - | `float` |
| `updatePredictions(percept)` | Update predictions based on percept | `percept: SensoryPercept` | `None` |
| `getPredictedSensory()` | Get predicted sensory input | - | `SensoryPercept` |
| `getConfidence()` | Get prediction confidence | - | `float` |

### Development System Classes

**DevelopmentSystem Class:**
Multi-stage developmental progression and maturation.

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `getDevelopmentalStage()` | Get current developmental stage | - | `DevelopmentalStage` |
| `setDevelopmentalStage(stage)` | Set developmental stage | `stage: DevelopmentalStage` | `None` |
| `getAge()` | Get developmental age | - | `float` |
| `getPlasticityRate()` | Get current plasticity rate | - | `float` |
| `updateDevelopment(timestep)` | Update development | `timestep: double` | `None` |
| `isCriticalPeriod()` | Check if in critical period | - | `bool` |

### Enumeration Types

**NeuronType:**
```python
Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
```

**SynapseType:**
```python
Excitatory, Inhibitory, Modulatory, Electrical, GapJunction
```

**DevelopmentalStage:**
```python
Initial, CriticalPeriod, Maturation, Adult, Aging
```

**FiringState:**
```python
Resting, Active, Refractory, Inhibited
```

**ActionType:**
```python
MoveForward, MoveBackward, MoveLeft, MoveRight, TurnLeft, TurnRight,
Look, LookUp, LookDown, Interact, Eat, Drink, Rest, Wait, Custom
```

**MotorCommand:**
```python
MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait
```

**WorldObjectType:**
```python
Empty, Resource, Hazard, Wall, Marker
```

### Utility Classes

**NeuronId, SynapseId, RegionId, PopulationId:**
Unique identifiers with integer values and indexing methods.

**SimulationClock:**
Manages simulation time and timestep calculations.

**RandomGenerator:**
Pseudo-random number generation with reproducible seeds.

---

## Memory Management

NLM uses C++ smart pointers (std::shared_ptr, std::unique_ptr) for automatic memory management. Python bindings automatically handle ownership, so users don't need to manually delete objects.

**Important Notes:**

1. **Object Ownership**: Python bindings use reference counting. Objects are automatically deleted when no Python references remain.

2. **Lifetime Considerations**: Avoid keeping long-lived references to temporary objects. Use `getBrain()` and similar methods for safe access.

3. **Memory Pools**: For performance-critical applications, consider reusing brain instances rather than creating new ones frequently.

---

## Performance Considerations

1. **Batch Operations**: Group related operations to minimize Python-C++ boundary crossings.

2. **Memory Efficiency**: Monitor memory usage with `getWorkingMemory().getActiveMemoryCount()`.

3. **Parallel Processing**: Use multiple brain instances for independent simulations.

4. **Configuration Optimization**: Optimize configuration parameters for your specific use case.

---

## Error Handling

NLM uses standard C++ exceptions that are translated to Python exceptions:

- `RuntimeError`: For runtime errors and invalid operations
- `ValueError`: For invalid parameter values
- `TypeError`: For type mismatches

**Example Error Handling:**
```python
import pynlm

try:
    config = pynlm.createDefaultConfig()
    config.set("invalid.key", "invalid_value")
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.RuntimeError as e:
    print(f"Configuration error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

---

## Migration from Easy Usage

This API reference extends the concepts from `easy_usage.md` with:

1. **Advanced Configuration**: Detailed configuration options for fine-tuning
2. **Complex Brain Architectures**: Multi-region and specialized neuron populations
3. **Integrated Cognitive Systems**: Full memory, prediction, and neuromodulation integration
4. **Production Features**: Real-time visualization, performance monitoring, and reporting

Users familiar with the basic patterns in `easy_usage.md` can progressively advance to these more sophisticated usage patterns.

---

## License

NLM is licensed under the MIT License. See `LICENSE` file for details.