# NLM - API Reference

This document provides comprehensive API reference for the NLM (Neural Learning Machine) Python bindings. It includes all available classes, methods, and their parameters.

## Module Overview

The NLM Python API provides the following main components:

- **Brain** - Central neural simulation brain class
- **AgentBrain** - Agent brain interface connecting NLM brain to world
- **SimpleWorld** - Simple 2D world for NLM simulation
- **Config** - Configuration system for NLM settings
- **SensoryPercept** - Sensory percept data structure
- **Action** - Action representation for motor output
- **ActionResult** - Action result from world
- **Vision, Audio, InternalSignals** - Sensory input classes

## Core Classes

### Brain

#### Constructors
```python
Brain(config: Config) -> Brain
```

Creates a new brain with the specified configuration.

**Parameters:**
- `config` - Configuration object for the brain

#### Methods

##### Initialization and Configuration
```python
Brain.initialize() -> None
```

Initializes the brain with its configuration.

```python
Brain.logStatus() -> None
```

Logs the current status of the brain.

```python
Brain.getConfig() -> Config
```

Returns the configuration used to create the brain.

##### Simulation Control
```python
Brain.step(step: int, time: float = None) -> None
```

Performs a simulation step.

**Parameters:**
- `step` - Step number
- `time` - Optional simulation time (defaults to step * timestep)

```python
Brain.reset() -> None
```

Resets the brain to its initial state.

##### Neural Activity Control
```python
Brain.receiveSensoryInput(input: SensoryPercept) -> None
```

Injects sensory input into the brain.

**Parameters:**
- `input` - Sensory percept to inject

```python
Brain.injectCurrent(neuron: NeuronId, current: float) -> None
```

Injects current into a specific neuron.

**Parameters:**
- `neuron` - Target neuron ID
- `current` - Current to inject

```python
Brain.injectCurrentToNeurons(type: NeuronType, current: float) -> None
```

Injects current into all neurons of a specific type.

**Parameters:**
- `type` - Type of neurons to target
- `current` - Current to inject

```python
Brain.produceAction() -> Action
```

Produces motor action based on neural activity.

Returns:
- Generated motor action

##### State Management
```python
Brain.save(filepath: str) -> bool
```

Saves brain state to file.

**Parameters:**
- `filepath` - Path to save the brain state

**Returns:**
- True if save successful, False otherwise

```python
Brain.load(filepath: str) -> bool
```

Loads brain state from file.

**Parameters:**
- `filepath` - Path to load the brain state from

**Returns:**
- True if load successful, False otherwise

##### Region Management
```python
Brain.addRegion(name: str = "") -> RegionId
```

Adds a new neural region.

**Parameters:**
- `name` - Optional name for the region

Returns:
- ID of the new region

```python
Brain.getRegion(id: RegionId) -> Region
```

Gets a region by ID.

**Parameters:**
- `id` - Region ID to retrieve

Returns:
- The requested region, or None if not found

```python
Brain.getRegionCount() -> int
```

Returns the number of regions in the brain.

Returns:
- Number of regions

```python
Brain.getRegionIds() -> List[RegionId]
```

Returns all region IDs.

Returns:
- List of all region IDs

```python
Brain.getRegions() -> List[Region]
```

Returns all regions.

Returns:
- List of all regions

```python
Brain.getTotalNeuronCount() -> int
```

Returns total neuron count across all regions.

Returns:
- Total neuron count

```python
Brain.getTotalSynapseCount() -> int
```

Returns total synapse count across all regions.

Returns:
- Total synapse count

```python
Brain.getActiveNeuronCount() -> int
```

Returns count of active neurons.

Returns:
- Active neuron count

```python
Brain.getFiringNeuronCount() -> int
```

Returns count of currently firing neurons.

Returns:
- Firing neuron count

```python
Brain.getAverageFiringRate() -> float
```

Returns average firing rate across all neurons.

Returns:
- Average firing rate (Hz)

```python
Brain.getExcitationInhibitionRatio() -> float
```

Returns excitation/inhibition balance ratio.

Returns:
- E/I ratio

```python
Brain.getTotalSpikeCount() -> int
```

Returns total spike count.

Returns:
- Total spike count

```python
Brain.getDevelopmentalStage() -> DevelopmentalStage
```

Returns current developmental stage.

Returns:
- Current developmental stage

```python
Brain.setDevelopmentalStage(stage: DevelopmentalStage) -> None
```

Sets developmental stage.

**Parameters:**
- `stage` - Developmental stage to set

##### Performance Monitoring
```python
Brain.getPerformanceMetrics() -> PerformanceMetrics
```

Returns performance metrics dashboard.

Returns:
- Performance metrics object

```python
Brain.getMemoryUsage() -> int
```

Returns current memory usage in bytes.

Returns:
- Memory usage in bytes

```python
Brain.getLearningProgress() -> float
```

Returns learning progress metrics.

Returns:
- Learning progress (0.0 to 1.0)

### AgentBrain

#### Constructors
```python
AgentBrain(brain: Brain) -> AgentBrain
```

Creates a new agent brain interface.

**Parameters:**
- `brain` - Brain object to interface with

#### Methods

##### Initialization and Configuration
```python
AgentBrain.initialize(world: SimpleWorld) -> None
```

Initializes the agent brain with the given world.

**Parameters:**
- `world` - World object to initialize with

```python
AgentBrain.configure(config: Config) -> None
```

Configures the agent with advanced settings.

**Parameters:**
- `config` - Advanced configuration

```python
AgentBrain.validateConfiguration() -> bool
```

Validates current configuration.

Returns:
- True if configuration is valid, False otherwise

```python
AgentBrain.getConfiguration() -> Config
```

Returns current configuration.

Returns:
- Current configuration object

##### Sensory and Motor Processing
```python
AgentBrain.getSensoryInputSize() -> int
```

Returns expected sensory input size.

Returns:
- Expected sensory input size

```python
AgentBrain.getMotorOutputSize() -> int
```

Returns expected motor output size.

Returns:
- Expected motor output size

```python
AgentBrain.processSensoryInput(percept: SensoryPercept) -> None
```

Processes sensory percept and injects into brain.

**Parameters:**
- `percept` - Sensory percept to process

```python
AgentBrain.decodeMotorCommand() -> MotorCommand
```

Decodes brain motor activity into motor command.

Returns:
- Decoded motor command

```python
AgentBrain.decodeFromMotorNeurons() -> MotorCommand
```

Calculates motor command from motor neuron activity.

Returns:
- Motor command

```python
AgentBrain.selectWithCuriosity(defaultCmd: MotorCommand) -> MotorCommand
```

Selects motor command with curiosity-based exploration.

**Parameters:**
- `defaultCmd` - Default motor command

Returns:
- Motor command with curiosity consideration

##### Neuromodulation
```python
AgentBrain.applyRewardModulation(reward: float, predictedReward: float) -> None
```

Applies reward-based neuromodulation.

**Parameters:**
- `reward` - Actual reward received
- `predictedReward` - Predicted reward

```python
AgentBrain.updateDevelopment(timestep: float) -> None
```

Updates development system.

**Parameters:**
- `timestep` - Time step for development update

```python
AgentBrain.getNeuromodulationLevel() -> float
```

Returns current neuromodulation level.

Returns:
- Neuromodulation level (-1.0 to 1.0)

```python
AgentBrain.getCuriosityLevel() -> float
```

Returns curiosity level.

Returns:
- Curiosity level (0.0 to 1.0)

```python
AgentBrain.getNoveltyLevel() -> float
```

Returns novelty level.

Returns:
- Novelty level (0.0 to 1.0)

```python
AgentBrain.getPredictionError() -> float
```

Returns prediction error.

Returns:
- Prediction error

##### Feature Control
```python
AgentBrain.enableRewardModulation(enable: bool) -> None
```

Enables or disables reward modulation.

**Parameters:**
- `enable` - Whether to enable reward modulation

```python
AgentBrain.enableStructuralPlasticity(enable: bool) -> None
```

Enables or disables structural plasticity.

**Parameters:**
- `enable` - Whether to enable structural plasticity

```python
AgentBrain.enableDevelopment(enable: bool) -> None
```

Enables or disables development.

**Parameters:**
- `enable` - Whether to enable development

```python
AgentBrain.enableCuriosity(enable: bool) -> None
```

Enables or disables curiosity.

**Parameters:**
- `enable` - Whether to enable curiosity

```python
AgentBrain.isRewardModulationEnabled() -> bool
```

Checks if reward modulation is enabled.

Returns:
- True if reward modulation enabled, False otherwise

```python
AgentBrain.isStructuralPlasticityEnabled() -> bool
```

Checks if structural plasticity is enabled.

Returns:
- True if structural plasticity enabled, False otherwise

```python
AgentBrain.isDevelopmentEnabled() -> bool
```

Checks if development is enabled.

Returns:
- True if development enabled, False otherwise

```python
AgentBrain.isCuriosityEnabled() -> bool
```

Checks if curiosity is enabled.

Returns:
- True if curiosity enabled, False otherwise

##### Advanced Features
```python
AgentBrain.setLearningRule(rule: LearningRule, parameters: dict) -> None
```

Sets custom learning rule configuration.

**Parameters:**
- `rule` - Learning rule configuration
- `parameters` - Rule parameters

```python
AgentBrain.setLearningRate(rate: float) -> None
```

Sets learning rate for plasticity.

**Parameters:**
- `rate` - Learning rate value

```python
AgentBrain.setArchitecture(architecture: ArchitectureConfig) -> None
```

Customizes brain architecture.

**Parameters:**
- `architecture` - Architecture configuration

```python
AgentBrain.createExperiment(name: str, config: Config) -> ExperimentConfig
```

Creates a new experiment configuration.

**Parameters:**
- `name` - Experiment name
- `config` - Experiment configuration

Returns:
- Experiment configuration

```python
AgentBrain.runBatchSimulation(inputs: List[SensoryPercept], rewards: List[ActionResult], timestep: float, steps: int) -> List[dict]
```

Runs batch simulation with multiple sensory inputs.

**Parameters:**
- `inputs` - List of sensory inputs
- `rewards` - List of expected rewards
- `timestep` - Simulation timestep
- `steps` - Number of steps to simulate

Returns:
- List of batch simulation results

```python
AgentBrain.saveBrainState(filepath: str) -> bool
```

Saves brain state to file (checkpoint).

**Parameters:**
- `filepath` - Path to save brain state

Returns:
- True if save successful, False otherwise

```python
AgentBrain.loadBrainState(filepath: str) -> bool
```

Loads brain state from file (checkpoint).

**Parameters:**
- `filepath` - Path to load brain state from

Returns:
- True if load successful, False otherwise

```python
AgentBrain.getPerformanceMetrics() -> PerformanceMetrics
```

Returns performance metrics dashboard.

Returns:
- Performance metrics object

```python
AgentBrain.getMemoryUsage() -> int
```

Returns current memory usage.

Returns:
- Memory usage in bytes

```python
AgentBrain.getLearningProgress() -> float
```

Returns learning progress metrics.

Returns:
- Learning progress (0.0 to 1.0)

### SimpleWorld

#### Constructors
```python
SimpleWorld() -> SimpleWorld
```

Creates a new simple world.

#### Methods

```python
SimpleWorld.configure(width: int, height: int, visionWidth: int, visionHeight: int) -> None
```

Configures the world dimensions.

**Parameters:**
- `width` - World width
- `height` - World height
- `visionWidth` - Vision width
- `visionHeight` - Vision height

```python
SimpleWorld.reset() -> None
```

Resets the world to initial state.

```python
SimpleWorld.setAgentStart(x: float, y: float) -> None
```

Sets agent starting position.

**Parameters:**
- `x` - X coordinate
- `y` - Y coordinate

```python
SimpleWorld.update(timestep: float) -> None
```

Updates world by timestep.

**Parameters:**
- `timestep` - Time step to update

```python
SimpleWorld.applyMotorCommand(cmd: MotorCommand, currentTime: float) -> ActionResult
```

Applies motor command to world.

**Parameters:**
- `cmd` - Motor command to apply
- `currentTime` - Current simulation time

Returns:
- Action result

```python
SimpleWorld.getSensoryPercept() -> SensoryPercept
```

Gets sensory percept from world.

Returns:
- Current sensory percept

```python
SimpleWorld.getAgentBody() -> AgentBody
```

Gets agent body state.

Returns:
- Agent body object

```python
SimpleWorld.addObject(obj: WorldObject) -> None
```

Adds world object.

**Parameters:**
- `obj` - World object to add

```python
SimpleWorld.removeObject(x: float, y: float) -> None
```

Removes world object at position.

**Parameters:**
- `x` - X coordinate
- `y` - Y coordinate

```python
SimpleWorld.isValidPosition(x: float, y: float) -> bool
```

Checks if position is valid.

**Parameters:**
- `x` - X coordinate
- `y` - Y coordinate

Returns:
- True if position is valid, False otherwise

```python
SimpleWorld.getWidth() -> int
```

Gets world width.

Returns:
- World width

```python
SimpleWorld.getHeight() -> int
```

Gets world height.

Returns:
- World height

```python
SimpleWorld.getMaxEnergy() -> float
```

Gets maximum energy.

Returns:
- Maximum energy

```python
SimpleWorld.setMaxEnergy(e: float) -> None
```

Sets maximum energy.

**Parameters:**
- `e` - Maximum energy

```python
SimpleWorld.getEnergyDecayRate() -> float
```

Gets energy decay rate.

Returns:
- Energy decay rate

```python
SimpleWorld.setEnergyDecayRate(r: float) -> None
```

Sets energy decay rate.

**Parameters:**
- `r` - Energy decay rate

```python
SimpleWorld.getSimulationTime() -> float
```

Gets simulation time.

Returns:
- Current simulation time

```python
SimpleWorld.setRandomSeed(seed: int) -> None
```

Sets random seed.

**Parameters:**
- `seed` - Random seed

```python
SimpleWorld.getRandomSeed() -> int
```

Gets random seed.

Returns:
- Random seed

### Config

#### Constructors
```python
Config() -> Config
```

Creates an empty configuration.

#### Methods

```python
Config.loadFromFile(filepath: str) -> bool
```

Loads configuration from file.

**Parameters:**
- `filepath` - Path to configuration file

Returns:
- True if load successful, False otherwise

```python
Config.loadFromArgs(argc: int, argv: List[str]) -> bool
```

Loads configuration from command line arguments.

**Parameters:**
- `argc` - Number of arguments
- `argv` - List of arguments

Returns:
- True if load successful, False otherwise

```python
Config.saveToFile(filepath: str) -> bool
```

Saves configuration to file.

**Parameters:**
- `filepath` - Path to save configuration

Returns:
- True if save successful, False otherwise

```python
Config.has(key: str) -> bool
```

Checks if configuration key exists.

**Parameters:**
- `key` - Configuration key

Returns:
- True if key exists, False otherwise

```python
Config.getKeys() -> List[str]
```

Gets all configuration keys.

Returns:
- List of all configuration keys

```python
Config.clear() -> None
```

Clears all configuration entries.

```python
Config.summary() -> str
```

Gets configuration summary.

Returns:
- Configuration summary string

```python
Config.get(key: str) -> Optional[Any]
```

Gets configuration value by key.

**Parameters:**
- `key` - Configuration key

Returns:
- Configuration value, or None if key doesn't exist

```python
Config.getOr(key: str, defaultValue: Any) -> Any
```

Gets configuration value or default if key doesn't exist.

**Parameters:**
- `key` - Configuration key
- `defaultValue` - Default value

Returns:
- Configuration value or default

```python
Config.set(key: str, value: Any, source: ConfigSource = ConfigSource.Runtime) -> None
```

Sets configuration value.

**Parameters:**
- `key` - Configuration key
- `value` - Value to set
- `source` - Configuration source (default: Runtime)

```python
Config.remove(key: str) -> None
```

Removes configuration entry.

**Parameters:**
- `key` - Configuration key to remove

```python
Config.validate() -> bool
```

Validates configuration.

Returns:
- True if configuration is valid, False otherwise

### Helper Functions

```python
def createDefaultConfig() -> Config
```

Creates a default configuration.

Returns:
- Default configuration

```python
def createBrain(config: Config) -> Brain
```

Creates a brain with configuration.

**Parameters:**
- `config` - Configuration for the brain

Returns:
- Brain object

```python
from .types import *
```

Imports all type definitions.

### Exception Classes

```python
class RuntimeError(Exception):
    """Runtime error in NLM operations"""
```

### Enums

#### NeuronType
```python
NeuronType.Excitatory
NeuronType.Inhibitory
NeuronType.Modulatory
NeuronType.Sensory
NeuronType.Motor
NeuronType.Internal
```

#### SynapseType
```python
SynapseType.Excitatory
SynapseType.Inhibitory
SynapseType.Modulatory
SynapseType.Electrical
SynapseType.GapJunction
```

#### DevelopmentalStage
```python
DevelopmentalStage.Initial
DevelopmentalStage.CriticalPeriod
DevelopmentalStage.Maturation
DevelopmentalStage.Adult
DevelopmentalStage.Aging
```

#### FiringState
```python
FiringState.Resting
FiringState.Active
FiringState.Refractory
FiringState.Inhibited
```

#### ActionType
```python
ActionType.MoveForward
ActionType.MoveBackward
ActionType.MoveLeft
ActionType.MoveRight
ActionType.TurnLeft
ActionType.TurnRight
ActionType.Look
ActionType.LookUp
ActionType.LookDown
ActionType.Interact
ActionType.Eat
ActionType.Drink
ActionType.Rest
ActionType.Wait
ActionType.Custom
```

#### MotorCommand
```python
MotorCommand.MoveForward
MotorCommand.MoveBackward
MotorCommand.TurnLeft
MotorCommand.TurnRight
MotorCommand.LookLeft
MotorCommand.LookRight
MotorCommand.Interact
MotorCommand.Wait
```

#### WorldObjectType
```python
WorldObjectType.Empty
WorldObjectType.Resource
WorldObjectType.Hazard
WorldObjectType.Wall
WorldObjectType.Marker
```

#### ConfigSource
```python
ConfigSource.Default
ConfigSource.File
ConfigSource.CommandLine
ConfigSource.Runtime
```

## Type Definitions

```python
NeuronId
SynapseId
RegionId
PopulationId
ConfigValue
ActionResult
SensoryPercept
Vision
Audio
InternalSignals
WorldObject
AgentBody
Action
WorldObject
NeuronType
SynapseType
DevelopmentalStage
FiringState
ActionType
MotorCommand
WorldObjectType
ConfigSource
PerformanceMetrics
LearningRule
ArchitectureConfig
ExperimentConfig
EnvironmentConfig
```

## Performance Metrics Structure

```python
class PerformanceMetrics:
    def __init__(self):
        self.spikeCount: int = 0
        self.firingRate: float = 0.0
        self.memoryUsage: int = 0
        self.simulationTime: float = 0.0
        self.learningProgress: float = 0.0
        self.cpuUsage: float = 0.0
        self.gpuMemory: float = 0.0
        
    def update(self, step: int, time: float) -> None:
        """Update performance metrics with new data"""
        
    def reset(self) -> None:
        """Reset all metrics"""
        
    def getSummary(self) -> str:
        """Get formatted summary of performance metrics"""
```

## Learning Rule Structure

```python
class LearningRule:
    def __init__(self):
        self.type: str = ""
        self.parameters: dict = {}
        
    def addParameter(self, name: str, value: Any) -> None:
        """Add a parameter to the learning rule"""
        
    def getParameter(self, name: str) -> Any:
        """Get a parameter value"""
        
    def validate(self) -> bool:
        """Validate learning rule configuration"""
```

## Architecture Configuration

```python
class ArchitectureConfig:
    def __init__(self):
        self.neuronCount: int = 0
        self.regionCount: int = 0
        self.connectionProbability: float = 0.0
        self.regionTypes: List[str] = []
        
    def addRegion(self, type: str, size: int) -> None:
        """Add a brain region with specific type and size"""
        
    def validate(self) -> bool:
        """Validate architecture configuration"""
```

## Experiment Configuration

```python
class ExperimentConfig:
    def __init__(self):
        self.name: str = ""
        self.description: str = ""
        self.parameters: dict = {}
        
    def addParameter(self, name: str, value: Any) -> None:
        """Add a parameter to the experiment"""
        
    def getParameter(self, name: str) -> Any:
        """Get a parameter value"""
        
    def run(self) -> dict:
        """Run the experiment with current configuration"""
        
    def stop(self) -> None:
        """Stop the running experiment"""
        
    def isRunning(self) -> bool:
        """Check if experiment is running"""
```

## Environment Configuration

```python
class EnvironmentConfig:
    def __init__(self):
        self.variables: dict = {}
        
    def loadFromEnvironment(self) -> bool:
        """Load configuration from environment variables"""
        
    def loadFromFile(self, filepath: str) -> bool:
        """Load configuration from file"""
        
    def validate(self) -> bool:
        """Validate environment configuration"""
        
    def getVariable(self, name: str) -> Any:
        """Get environment variable value"""
        
    def setVariable(self, name: str, value: Any) -> None:
        """Set environment variable value"""
```

## Batch Processing Functions

```python
def runBatchSimulation(brain: Brain, inputs: List[SensoryPercept], rewards: List[ActionResult], timestep: float, steps: int) -> List[dict]:
    """Run batch simulation and return results"""
```

```python
def runParallelSimulation(brain: Brain, steps: List[Tuple[int, float]]) -> None:
    """Run parallel simulation with multiple steps"""
```

## Checkpoint System

```python
def saveCheckpoint(filepath: str, brain: Brain) -> None:
    """Save brain checkpoint to file"""
```

```python
def loadCheckpoint(filepath: str, config: Config) -> Brain:
    """Load brain checkpoint from file"""
```

## Experiment Management

```python
def createExperiment(name: str, config: Config) -> ExperimentConfig:
    """Create a new experiment"""
```

```python
def runExperiment(experiment: ExperimentConfig, steps: int) -> None:
    """Run an experiment for specified steps"""
```

## Performance Utilities

```python
def getSystemInfo() -> dict:
    """Get system information"""
```

```python
def getMemoryStats() -> dict:
    """Get memory statistics"""
```

```python
def getPerformanceBenchmark() -> dict:
    """Get performance benchmark data"""
```

## Example Usage

### Basic Brain Usage
```python
import pynlm

# Create brain with default config
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Complete Agent Simulation
```python
import pynlm

# Create agent with world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Advanced Configuration
```python
import pynlm

# Create advanced configuration
config = pynlm.createAdvancedConfig()
config.set("neuron_count", 2000)
config.set("plasticity_learning_rate", 0.02)
config.set("stdp_ltp_weight", 0.025)
config.set("stdp_ltd_weight", 0.024)
config.set("environment_name", "GridWorld")
config.set("log_level", "DEBUG")

# Validate configuration
if config.validate():
    print("Configuration is valid")
    
    # Create brain with advanced config
    brain = pynlm.createBrain(config)
    brain.initialize()
else:
    print("Configuration has errors")
```

### Performance Monitoring
```python
import pynlm

# Create brain and monitor
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

# Create performance metrics
metrics = pynlm.createPerformanceMetrics()

# Run simulation with monitoring
for step in range(1000):
    brain.step(step)
    metrics.update(step, step * 0.001)
    
    if step % 100 == 0:
        print(f"Step {step}: {metrics.getSummary()}")
```

### Serialization
```python
import pynlm
import os

# Create and run brain
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

for step in range(1000):
    brain.step(step)

# Save checkpoint
checkpoint_file = "brain_checkpoint.npkl"
brain.save(checkpoint_file)
print(f"Brain saved to {checkpoint_file}")

# Load checkpoint
if os.path.exists(checkpoint_file):
    new_brain = pynlm.createBrain(pynlm.createAdvancedConfig())
    new_brain.initialize()
    new_brain.load(checkpoint_file)
    print("Brain loaded from checkpoint")
```

This API reference provides comprehensive documentation for all NLM Python bindings. For detailed examples and usage patterns, refer to the `easy_usage.md` and `advanced_usage.md` documentation files.