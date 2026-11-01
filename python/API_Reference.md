# NLM - Comprehensive Python API Reference

## Overview

NLM (Neural Learning Machine) provides a complete Python interface to the C++ neural simulation framework through the `pynlm` module. This reference covers all available classes, functions, and usage patterns.

## Module Overview

```python
import pynlm

# Core classes
brain = pynlm.Brain(config)
agent = pynlm.AgentBrain(brain)
world = pynlm.SimpleWorld()
config = pynlm.Config()

# Create utilities
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
config = pynlm.createDefaultConfig()
```

## Classes

### Brain
Represents the central neural simulation brain with neurons, synapses, and learning capabilities.

#### Constructor
```python
Brain(config: Config) -> Brain
```

#### Methods
- `initialize()` - Initialize the brain with configuration
- `step(step: int)` - Perform a simulation step
- `step(step: int, time: float)` - Perform a simulation step with timestamp
- `receiveSensoryInput(input: SensoryInput)` - Inject sensory input
- `injectCurrent(neuron: NeuronId, current: float)` - Inject current into specific neuron
- `injectCurrentToNeurons(neuronType: NeuronType, current: float)` - Inject current to all neurons of type
- `produceAction()` -> Action - Produce motor action based on neural activity
- `reset()` - Reset brain state
- `save(filepath: str)` - Save brain state to file
- `load(filepath: str)` - Load brain state from file
- `addRegion(name: str)` -> None - Add a new neural region
- `getRegion(id: RegionId)` -> NeuralRegion - Get region by ID
- `getRegionCount()` -> int - Get number of regions
- `getRegionIds()` -> List[RegionId] - Get all region IDs
- `getRegions()` -> List[NeuralRegion] - Get all regions
- `getTotalNeuronCount()` -> int - Get total neuron count
- `getTotalSynapseCount()` -> int - Get total synapse count
- `getActiveNeuronCount()` -> int - Get count of active neurons
- `getFiringNeuronCount()` -> int - Get count of currently firing neurons
- `getAverageFiringRate()` -> float - Get average firing rate
- `getExcitationInhibitionRatio()` -> float - Get E/I balance ratio
- `getTotalSpikeCount()` -> int - Get total spike count
- `getDevelopmentalStage()` -> DevelopmentalStage - Get developmental stage
- `setDevelopmentalStage(stage: DevelopmentalStage)` - Set developmental stage
- `getConfig()` -> Config - Get the configuration
- `logStatus()` - Log brain status

### AgentBrain
Connects NLM brain to world environment with sensory processing and motor output.

#### Constructor
```python
AgentBrain(brain: Brain) -> AgentBrain
```

#### Methods
- `initialize(world: SimpleWorld)` - Initialize with world
- `getSensoryInputSize()` -> int - Get expected sensory input size
- `getMotorOutputSize()` -> int - Get expected motor output size
- `processSensoryInput(percept: SensoryPercept)` - Process sensory percept
- `decodeMotorCommand()` -> Action - Decode into motor command
- `applyRewardModulation(reward: float, predictedReward: float)` - Apply reward modulation
- `updateDevelopment(timestep: float)` - Update development system
- `getDevelopmentalStage()` -> DevelopmentalStage - Get developmental stage
- `getNeuromodulationLevel()` -> float - Get neuromodulation level
- `getCuriosityLevel()` -> float - Get curiosity level
- `getNoveltyLevel()` -> float - Get novelty level
- `getPredictionError()` -> float - Get prediction error
- `reset()` - Reset agent for new episode
- `getBrain()` -> Brain - Get underlying brain
- `enableRewardModulation(enable: bool)` - Enable/disable reward modulation
- `enableStructuralPlasticity(enable: bool)` - Enable/disable structural plasticity
- `enableDevelopment(enable: bool)` - Enable/disable development
- `enableCuriosity(enable: bool)` - Enable/disable curiosity
- `isRewardModulationEnabled()` -> bool - Check if reward modulation enabled
- `isStructuralPlasticityEnabled()` -> bool - Check if structural plasticity enabled
- `isDevelopmentEnabled()` -> bool - Check if development enabled
- `isCuriosityEnabled()` -> bool - Check if curiosity enabled

### SimpleWorld
2D simulation world with agents, objects, and environmental dynamics.

#### Constructor
```python
SimpleWorld() -> SimpleWorld
```

#### Methods
- `configure(width: int, height: int, visionWidth: int, visionHeight: int)` - Configure world dimensions
- `reset()` - Reset world state
- `setAgentStart(x: float, y: float)` - Set agent starting position
- `update(timestep: float)` - Update world by timestep
- `applyMotorCommand(cmd: Action, currentTime: float)` -> ActionResult - Apply motor command
- `getSensoryPercept()` -> SensoryPercept - Get sensory percept
- `getAgentBody()` -> AgentBody - Get agent body state
- `addObject(obj: WorldObject)` - Add world object
- `removeObject(x: float, y: float)` - Remove object at position
- `isValidPosition(x: float, y: float)` -> bool - Check valid position
- `getWidth()` -> int - Get world width
- `getHeight()` -> int - Get world height
- `getMaxEnergy()` -> float - Get max energy
- `setMaxEnergy(e: float)` - Set max energy
- `getEnergyDecayRate()` -> float - Get energy decay rate
- `setEnergyDecayRate(r: float)` - Set energy decay rate
- `getSimulationTime()` -> float - Get simulation time
- `setRandomSeed(seed: int)` - Set random seed
- `getRandomSeed()` -> int - Get random seed

### Config
Configuration class for NLM system settings.

#### Constructor
```python
Config() -> Config
```

#### Methods
- `loadFromFile(filepath: str)` -> bool - Load configuration from JSON file
- `saveToFile(filepath: str)` -> bool - Save configuration to JSON file
- `has(key: str)` -> bool - Check if configuration key exists
- `getKeys()` -> List[str] - Get all configuration keys
- `clear()` - Clear all configuration entries
- `summary()` -> str - Get configuration summary

### Vision
Vision sensory input representing visual information.

#### Constructor
```python
Vision() -> Vision
Vision(width: int, height: int, channels: int = 3) -> Vision
```

#### Methods
- `setData(data: List[float])` - Set visual data
- `getWidth()` -> int - Get width
- `getHeight()` -> int - Get height
- `getChannels()` -> int - Get channels

### Action
Action representation for motor output.

#### Constructor
```python
Action() -> Action
Action(type: ActionType) -> Action
Action(type: ActionType, parameters: List[float]) -> Action
```

#### Methods
- `getType()` -> ActionType - Get action type
- `setType(type: ActionType)` - Set action type
- `getParameters()` -> List[float] - Get parameters
- `setParameters(params: List[float])` - Set parameters
- `getName()` -> str - Get action name
- `clone()` -> Action - Clone action

### ActionResult
Result from world action execution.

#### Constructor
```python
ActionResult() -> ActionResult
ActionResult(reward: float, success: bool, message: str = "") -> ActionResult
```

#### Properties
- `reward: float` - Reward value
- `success: bool` - Success status
- `message: str` - Message

### SensoryPercept
Container for all sensory input types.

#### Constructor
```python
SensoryPercept() -> SensoryPercept
```

#### Methods
- `getVision()` -> Vision - Get vision data
- `setVision(vision: Vision)` - Set vision data
- `getVisionWidth()` -> int - Get vision width
- `getVisionHeight()` -> int - Get vision height
- `getTouch()` -> List[float] - Get touch data
- `setTouch(touch: List[float])` - Set touch data
- `getInternal()` -> List[float] - Get internal signals
- `setInternal(internal: List[float])` - Set internal signals
- `getProprioception()` -> List[float] - Get proprioception
- `setProprioception(proprioception: List[float])` - Set proprioception
- `getAudio()` -> Audio - Get audio data
- `setAudio(audio: Audio)` - Set audio data
- `getAllSignals()` -> List[List[float]] - Get all signals
- `getTimestamp()` -> float - Get timestamp
- `setTimestamp(timestamp: float)` - Set timestamp

### AgentBody
Agent body state for movement and physics simulation.

#### Constructor
```python
AgentBody() -> AgentBody
```

#### Properties
- `x: float` - X position
- `y: float` - Y position
- `orientation: float` - Orientation
- `velocityX: float` - X velocity
- `velocityY: float` - Y velocity
- `angularVelocity: float` - Angular velocity
- `energy: float` - Energy level
- `health: float` - Health
- `age: float` - Age
- `isMoving: bool` - Moving state
- `isTurning: bool` - Turning state
- `lastActionTime: float` - Last action time
- `reset()` - Reset body state

### WorldObject
World object representation.

#### Constructor
```python
WorldObject() -> WorldObject
WorldObject(x: float, y: float, type: WorldObjectType, value: float = 0.0, radius: float = 0.5) -> WorldObject
```

#### Properties
- `x: float` - X position
- `y: float` - Y position
- `radius: float` - Radius
- `type: WorldObjectType` - Object type
- `value: float` - Value
- `active: bool` - Active state

### SensoryInput
Base class for all sensory input.

#### Methods
- `getType()` -> NeuronType - Get input type
- `getData()` -> List[float] - Get data
- `getDimensions()` -> int - Get dimensions
- `getTimestamp()` -> float - Get timestamp
- `setTimestamp(timestamp: float)` - Set timestamp

### Audio
Audio sensory input.

#### Constructor
```python
Audio() -> Audio
Audio(sampleRate: int, numSamples: int) -> Audio
```

#### Methods
- `setData(data: List[float])` - Set audio data
- `setSampleRate(sampleRate: int)` - Set sample rate
- `getSampleRate()` -> int - Get sample rate
- `getNumSamples()` -> int - Get number of samples

### InternalSignals
Internal signals sensory input.

#### Constructor
```python
InternalSignals() -> InternalSignals
```

#### Methods
- `addSignal(value: float)` - Add signal
- `clearSignals()` - Clear signals

## Utility Functions

### Factory Functions
```python
createDefaultConfig() -> Config
    Create a default configuration with reasonable values.

createBrain(config: Config) -> Brain
    Create a new brain with configuration.

createSimpleWorld() -> SimpleWorld
    Create a new simple world.

createAgentBrain(brain: Brain) -> AgentBrain
    Create a new agent brain interface.

createEmptyConfig() -> Config
    Create an empty configuration with default values.

createSimpleBrain(neuron_count: int) -> Brain
    Create a simple brain with specified neuron count.

createAgentWithDefaults(brain: Brain, world: SimpleWorld) -> AgentBrain
    Create an agent with common defaults enabled.

createPerformanceConfig() -> Config
    Create a performance-optimized configuration.

createResearchConfig() -> Config
    Create a research-oriented configuration.
```

### Advanced Factory Functions
```python
createBrainEnsemble(neuronCounts: List[int]) -> List[Brain]
    Create multiple brains with different sizes for ensemble experiments.

runBrainSimulation(brain: Brain, steps: int, world: Optional[SimpleWorld] = None, 
                  agent: Optional[AgentBrain] = None, randomSeed: int = 42) -> Dict[str, float]
    Run a complete brain-world-agent simulation with statistics collection.
```

### Diagnostic Functions
```python
brainHealthReport(brain: Brain) -> str
    Generate a detailed health report for a brain.

validateConfig(config: Config) -> bool
    Validate that a configuration has all required fields.

printConfigHelp()
    Print configuration field documentation.
```

## Enumerations

### NeuronType
- `Excitatory` - Excitatory neuron
- `Inhibitory` - Inhibitory neuron
- `Modulatory` - Modulatory neuron
- `Sensory` - Sensory neuron
- `Motor` - Motor neuron
- `Internal` - Internal neuron

### SynapseType
- `Excitatory` - Excitatory synapse
- `Inhibitory` - Inhibitory synapse
- `Modulatory` - Modulatory synapse
- `Electrical` - Electrical synapse
- `GapJunction` - Gap junction

### DevelopmentalStage
- `Initial` - Initial development stage
- `CriticalPeriod` - Critical period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

### ActionType
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `MoveLeft` - Move left
- `MoveRight` - Move right
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Look` - Look
- `LookUp` - Look up
- `LookDown` - Look down
- `Interact` - Interact
- `Eat` - Eat
- `Drink` - Drink
- `Rest` - Rest
- `Wait` - Wait
- `Custom` - Custom action

### MotorCommand
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `LookLeft` - Look left
- `LookRight` - Look right
- `Interact` - Interact
- `Wait` - Wait

### WorldObjectType
- `Empty` - Empty space
- `Resource` - Resource object
- `Hazard` - Hazard object
- `Wall` - Wall object
- `Marker` - Marker object

### FiringState
- `Resting` - Resting state
- `Active` - Active state
- `Refractory` - Refractory state
- `Inhibited` - Inhibited state

## Constants

### Invalid IDs
- `INVALID_NEURON_ID` - Invalid neuron ID
- `INVALID_SYNAPSE_ID` - Invalid synapse ID
- `INVALID_REGION_ID` - Invalid region ID
- `INVALID_POPULATION_ID` - Invalid population ID

## Usage Examples

### Example 1: Simple Brain Simulation
```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
```

### Example 2: Complete Agent-World Simulation
```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Example 3: Advanced Brain with Statistics
```python
import pynlm

# Create performance-optimized configuration
config = pynlm.createPerformanceConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with defaults
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentWithDefaults(brain, world)

# Run simulation with statistics
stats = pynlm.runBrainSimulation(
    brain=brain,
    steps=1000,
    world=world,
    agent=agent,
    randomSeed=42
)

print("Simulation Results:")
for key, value in stats.items():
    print(f"  {key}: {value}")

# Generate health report
report = pynlm.brainHealthReport(brain)
print("\n" + report)
```

### Example 4: Ensemble Learning
```python
import pynlm

# Create ensemble of brains with different sizes
neuron_counts = [100, 500, 1000, 2000]
brains = pynlm.createBrainEnsemble(neuron_counts)

# Run different experiments with each brain
for i, brain in enumerate(brains):
    print(f"Running experiment with {brain.getTotalNeuronCount()} neurons...")
    
    # Simple plasticity experiment
    for step in range(200):
        # Record initial state
        if step == 0:
            initial_synapses = brain.getTotalSynapseCount()
        
        # Inject stimulating current
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
        brain.step(step)
        
        # Record final state
        if step == 199:
            final_synapses = brain.getTotalSynapseCount()
            growth = final_synapses - initial_synapses
            print(f"  Synapse growth: {growth}")
            print(f"  Firing rate: {brain.getAverageFiringRate()} Hz")
```

## Performance Tips

1. **For Large Networks**: Use `createPerformanceConfig()` for optimized settings
2. **For Research**: Use `createResearchConfig()` for advanced plasticity features
3. **For Multiple Experiments**: Use `createBrainEnsemble()` for parallel testing
4. **For Monitoring**: Use `brainHealthReport()` for regular diagnostics
5. **For Validation**: Use `validateConfig()` to ensure proper configuration

## Troubleshooting

**Common Issues:**

1. "My brain isn't doing anything"
   - Did you call `brain.initialize()`?
   - Try increasing the number of steps

2. "The agent isn't moving"
   - Did you call `world.applyMotorCommand()`?
   - Check that `world.update()` is being called

3. "Everything is 0"
   - Brains need time to "warm up" - try more steps
   - Some neurons need input to fire - make sure sensory input is connected

**Configuration Help:**
Call `pynlm.printConfigHelp()` for detailed configuration field documentation.

## Advanced Features

The enhanced Python API now includes:

- **Convenience Factory Methods**: Easy creation of common brain configurations
- **Advanced Simulation Runner**: Complete brain-world-agent simulation with statistics
- **Ensemble Support**: Multiple brains for comparative experiments
- **Health Diagnostics**: Comprehensive brain health monitoring
- **Configuration Validation**: Ensure proper setup before running simulations
- **Performance Optimizations**: Pre-configured settings for different use cases
- **Error Handling**: Better error messages and validation

This makes NLM more accessible to beginners while providing powerful tools for advanced researchers.
