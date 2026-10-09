# NLM API Reference

## Overview

This document provides complete API documentation for the enhanced NLM bindings. It includes detailed information about all classes, methods, functions, and their parameters.

## Table of Contents

### Core Classes
- [Brain](API_REFERENCE.md#brain)
- [Config](API_REFERENCE.md#config)
- [AgentBrain](API_REFERENCE.md#agentbrain)
- [World](API_REFERENCE.md#world)

### Helper Functions
- [pynlm Module](API_REFERENCE.md#pynlm-module)
- [Convenience Functions](API_REFERENCE.md#convenience-functions)
- [Analysis Functions](API_REFERENCE.md#analysis-functions)
- [Training Functions](API_REFERENCE.md#training-functions)

### Error Handling
- [Exceptions](API_REFERENCE.md#exceptions)
- [Error Functions](API_REFERENCE.md#error-functions)

---

## Brain

### Class Definition

```python
class Brain:
    """Central neural simulation brain class.
    
    This class implements real spiking neural computation with event-driven
    dynamics and integrated memory, prediction, cognition, and neuromodulation systems.
    """
```

### Constructor

```python
def __init__(self, config: Config) -> None:
    """Create brain with configuration.
    
    Args:
        config: Configuration object for brain settings
    """
```

### Methods

#### Core Methods

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `initialize()` | Initialize the brain with configuration | `() -> None` | `brain.initialize()` |
| `step(currentStep)` | Perform a simulation step | `(currentStep: int) -> None` | `brain.step(0)` |
| `step(currentStep, currentTime)` | Perform a simulation step with timestamp | `(currentStep: int, currentTime: float) -> None` | `brain.step(0, 0.001)` |
| `reset()` | Reset brain state | `() -> None` | `brain.reset()` |

#### Sensory Processing

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `receiveSensoryInput(input)` | Inject sensory input into the brain | `(input: SensoryInput) -> None` | `brain.receiveSensoryInput(percept)` |
| `injectCurrent(neuron, current)` | Inject current into a specific neuron | `(neuron: NeuronId, current: float) -> None` | `brain.injectCurrent(neuron_id, 1.0)` |
| `injectCurrentToNeurons(type, current)` | Inject current into all neurons of a specific type | `(type: NeuronType, current: float) -> None` | `brain.injectCurrentToNeurons(NeuronType.Sensory, 1.0)` |

#### Motor Processing

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `produceAction()` | Produce motor action based on neural activity | `() -> Action` | `action = brain.produceAction()` |
| `getRegion(id)` | Get a region by ID | `(id: RegionId) -> NeuralRegion` | `region = brain.getRegion(region_id)` |

#### Statistics and Information

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `getTotalNeuronCount()` | Get total neuron count across all regions | `() -> int` | `neurons = brain.getTotalNeuronCount()` |
| `getTotalSynapseCount()` | Get total synapse count across all regions | `() -> int` | `synapses = brain.getTotalSynapseCount()` |
| `getActiveNeuronCount()` | Get count of active neurons | `() -> int` | `active = brain.getActiveNeuronCount()` |
| `getFiringNeuronCount()` | Get count of currently firing neurons | `() -> int` | `firing = brain.getFiringNeuronCount()` |
| `getAverageFiringRate()` | Get average firing rate across all neurons | `() -> float` | `rate = brain.getAverageFiringRate()` |
| `getExcitationInhibitionRatio()` | Get excitation/inhibition balance ratio | `() -> float` | `ratio = brain.getExcitationInhibitionRatio()` |
| `getTotalSpikeCount()` | Get total spike count | `() -> int` | `spikes = brain.getTotalSpikeCount()` |
| `getTotalSpikeEventCount()` | Get pending spike event count | `() -> int` | `pending = brain.getTotalSpikeEventCount()` |

#### Development and Plasticity

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `getDevelopmentalStage()` | Get current developmental stage | `() -> DevelopmentalStage` | `stage = brain.getDevelopmentalStage()` |
| `setDevelopmentalStage(stage)` | Set developmental stage | `(stage: DevelopmentalStage) -> None` | `brain.setDevelopmentalStage(DevelopmentalStage.Maturation)` |
| `getConfig()` | Get the configuration | `() -> Config` | `config = brain.getConfig()` |

#### Memory Systems

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `getWorkingMemory()` | Get working memory | `() -> NeuralWorkingMemory` | `memory = brain.getWorkingMemory()` |
| `getEpisodicMemory()` | Get episodic memory | `() -> NeuralEpisodicMemory` | `episodic = brain.getEpisodicMemory()` |
| `getAssociativeMemory()` | Get associative memory | `() -> NeuralAssociativeMemory` | `associative = brain.getAssociativeMemory()` |

#### Prediction System

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `getPredictionSystem()` | Get prediction system | `() -> PredictionSystem` | `prediction = brain.getPredictionSystem()` |

#### Cognition Systems

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `getPlanner()` | Get neural planner | `() -> NeuralPlanner` | `planner = brain.getPlanner()` |
| `getConceptFormation()` | Get concept formation | `() -> ConceptFormation` | `concept = brain.getConceptFormation()` |
| `getAttention()` | Get attentional selection | `() -> AttentionalSelection` | `attention = brain.getAttention()` |

#### Development System

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `getDevelopmentSystem()` | Get development system | `() -> DevelopmentSystem` | `development = brain.getDevelopmentSystem()` |

#### Neuromodulation Systems

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `getDopamine()` | Get dopamine system | `() -> Dopamine` | `dopamine = brain.getDopamine()` |
| `getCuriosity()` | Get curiosity system | `() -> Curiosity` | `curiosity = brain.getCuriosity()` |
| `getNovelty()` | Get novelty detection | `() -> Novelty` | `novelty = brain.getNovelty()` |
| `getPredictionErrorSignal()` | Get prediction error signal | `() -> PredictionError` | `prediction_error = brain.getPredictionErrorSignal()` |

#### Plasticity System

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `getSTDP()` | Get STDP plasticity system | `() -> STDP*` | `stdp = brain.getSTDP()` |
| `getHebbian()` | Get Hebbian plasticity system | `() -> Hebbian*` | `hebbian = brain.getHebbian()` |
| `getStructuralPlasticity()` | Get structural plasticity system | `() -> StructuralPlasticity*` | `structural = brain.getStructuralPlasticity()` |

#### Spike System

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `getSpikeSystem()` | Get spike system | `() -> SpikeSystem*` | `spike_system = brain.getSpikeSystem()` |

#### Utility Methods

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `addRegion(name)` | Add a new neural region | `(name: str = "") -> RegionId` | `region_id = brain.addRegion("cortex")` |
| `getRegionCount()` | Get the number of regions | `() -> int` | `count = brain.getRegionCount()` |
| `getRegionIds()` | Get all region IDs | `() -> List[RegionId]` | `ids = brain.getRegionIds()` |
| `getRegions()` | Get all regions | `() -> List[NeuralRegion]` | `regions = brain.getRegions()` |
| `addInterRegionConnection(source, target, weight, delay)` | Add inter-region connection | `(source: RegionId, target: RegionId, weight: float = 0.0, delay: int = 1) -> None` | `brain.addInterRegionConnection(source, target, 0.5, 5)` |
| `removeInterRegionConnection(source, target)` | Remove inter-region connection | `(source: RegionId, target: RegionId) -> None` | `brain.removeInterRegionConnection(source, target)` |
| `logStatus()` | Log brain status | `() -> None` | `brain.logStatus()` |

---

## Config

### Class Definition

```python
class Config:
    """Configuration class for NLM system.
    
    This class provides configuration management for NLM operations,
    including loading, saving, and validation of configuration files.
    """
```

### Constructor

```python
def __init__(self) -> None:
    """Create new configuration."""
```

### Methods

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `loadFromFile(filepath)` | Load configuration from a JSON file | `(filepath: str) -> bool` | `config.loadFromFile("config.json")` |
| `loadFromArgs(argc, argv)` | Load configuration from command line arguments | `(argc: int, argv: List[str]) -> bool` | `config.loadFromArgs(len(sys.argv), sys.argv)` |
| `saveToFile(filepath)` | Save configuration to a JSON file | `(filepath: str) -> bool` | `config.saveToFile("config.json")` |
| `has(key)` | Check if a configuration key exists | `(key: str) -> bool` | `config.has("neuron_count")` |
| `getKeys()` | Get all configuration keys | `() -> List[str]` | `keys = config.getKeys()` |
| `clear()` | Clear all configuration entries | `() -> None` | `config.clear()` |
| `summary()` | Get a summary string of the configuration | `() -> str` | `summary = config.summary()` |

#### Configuration Access

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `getOr(key, default_value)` | Get configuration value with Python-style default | `(key: str, default_value: Any) -> Any` | `value = config.getOr("neuron_count", 1000)` |

#### Configuration Setting

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `set(key, value)` | Set configuration value (string type) | `(key: str, value: str) -> None` | `config.set("brain_name", "research_brain")` |
| `set(key, value)` | Set configuration value (integer type) | `(key: str, value: int) -> None` | `config.set("neuron_count", 1000)` |
| `set(key, value)` | Set configuration value (float type) | `(key: str, value: float) -> None` | `config.set("plasticity_learning_rate", 0.01)` |
| `set(key, value)` | Set configuration value (boolean type) | `(key: str, value: bool) -> None` | `config.set("visualization_enabled", False)` |

#### Configuration Validation

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `validate()` | Validate configuration and return results | `() -> Dict[str, Any]` | `results = config.validate()` |
| `fixDuplicates()` | Fix duplicate configuration entries | `() -> bool` | `fixed = config.fixDuplicates()` |

#### Configuration Export/Import

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `to_dict()` | Export as dictionary | `() -> Dict[str, str]` | `dict_config = config.to_dict()` |
| `from_dict(config_dict)` | Import from dictionary | `(config_dict: Dict[str, str]) -> None` | `config.from_dict({"neuron_count": "1000"})` |
| `copy()` | Copy configuration | `() -> Config` | `new_config = config.copy()` |

---

## AgentBrain

### Class Definition

```python
class AgentBrain:
    """Agent brain interface connecting NLM brain to world.
    
    This class provides an interface for connecting the NLM brain to the world,
    enabling sensory processing, motor control, and learning.
    """
```

### Constructor

```python
def __init__(self, brain: Brain) -> None:
    """Create agent for brain.
    
    Args:
        brain: The NLM brain to connect to
    """
```

### Methods

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `initialize(world)` | Initialize with world | `(world: World) -> None` | `agent.initialize(world)` |
| `getSensoryInputSize()` | Get expected sensory input size | `() -> int` | `size = agent.getSensoryInputSize()` |
| `getMotorOutputSize()` | Get expected motor output size | `() -> int` | `size = agent.getMotorOutputSize()` |
| `processSensoryInput(percept)` | Process sensory percept and inject into brain | `(percept: SensoryPercept) -> None` | `agent.processSensoryInput(percept)` |
| `decodeMotorCommand()` | Decode brain motor activity into motor command | `() -> MotorCommand` | `command = agent.decodeMotorCommand()` |
| `applyRewardModulation(reward, predicted_reward)` | Apply reward-based neuromodulation | `(reward: float, predicted_reward: float) -> None` | `agent.applyRewardModulation(1.0, 0.8)` |
| `updateDevelopment(timestep)` | Update development system | `(timestep: float) -> None` | `agent.updateDevelopment(0.1)` |
| `getDevelopmentalStage()` | Get current developmental stage | `() -> DevelopmentalStage` | `stage = agent.getDevelopmentalStage()` |
| `getNeuromodulationLevel()` | Get current neuromodulation level | `() -> float` | `level = agent.getNeuromodulationLevel()` |
| `getCuriosityLevel()` | Get curiosity level | `() -> float` | `curiosity = agent.getCuriosityLevel()` |
| `getNoveltyLevel()` | Get novelty level | `() -> float` | `novelty = agent.getNoveltyLevel()` |
| `getPredictionError()` | Get prediction error | `() -> float` | `error = agent.getPredictionError()` |
| `reset()` | Reset agent for new episode | `() -> None` | `agent.reset()` |
| `getBrain()` | Get the underlying brain | `() -> Brain` | `brain = agent.getBrain()` |

#### Agent Configuration

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `enableRewardModulation(enable)` | Enable or disable reward modulation | `(enable: bool) -> None` | `agent.enableRewardModulation(True)` |
| `enableStructuralPlasticity(enable)` | Enable or disable structural plasticity | `(enable: bool) -> None` | `agent.enableStructuralPlasticity(True)` |
| `enableDevelopment(enable)` | Enable or disable development | `(enable: bool) -> None` | `agent.enableDevelopment(True)` |
| `enableCuriosity(enable)` | Enable or disable curiosity-driven behavior | `(enable: bool) -> None` | `agent.enableCuriosity(True)` |
| `isRewardModulationEnabled()` | Check if reward modulation is enabled | `() -> bool` | `enabled = agent.isRewardModulationEnabled()` |
| `isStructuralPlasticityEnabled()` | Check if structural plasticity is enabled | `() -> bool` | `enabled = agent.isStructuralPlasticityEnabled()` |
| `isDevelopmentEnabled()` | Check if development is enabled | `() -> bool` | `enabled = agent.isDevelopmentEnabled()` |
| `isCuriosityEnabled()` | Check if curiosity is enabled | `() -> bool` | `enabled = agent.isCuriosityEnabled()` |

#### Agent Analysis

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `getAgentStats()` | Get comprehensive agent statistics | `() -> Dict[str, Any]` | `stats = agent.getAgentStats()` |
| `trainEpisode(brain, world, steps)` | Train agent for one episode | `(brain: Brain, world: World, steps: int) -> Dict` | `result = agent.trainEpisode(brain, world, 100)` |

---

## World

### Class Definition

```python
class World:
    """Simple 2D world for NLM simulation.
    
    This class provides a 2D world environment for NLM brain simulation,
    including sensory processing, motor control, and object management.
    """
```

### Constructor

```python
def __init__(self) -> None:
    """Create new simple world."""
```

### Methods

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `configure(width, height, visionWidth, visionHeight)` | Configure world dimensions | `(width: int, height: int, visionWidth: int, visionHeight: int) -> None` | `world.configure(10, 10, 8, 8)` |
| `reset()` | Reset world state | `() -> None` | `world.reset()` |
| `setAgentStart(x, y)` | Set agent start position | `(x: float, y: float) -> None` | `world.setAgentStart(0, 0)` |
| `update(timestep)` | Update world by timestep | `(timestep: float) -> None` | `world.update(0.1)` |
| `applyMotorCommand(cmd, currentTime)` | Apply motor command | `(cmd: MotorCommand, currentTime: float) -> ActionResult` | `result = world.applyMotorCommand(cmd, 0.0)` |
| `getSensoryPercept()` | Get sensory percept | `() -> SensoryPercept` | `percept = world.getSensoryPercept()` |
| `getAgentBody()` | Get agent body | `() -> AgentBody` | `body = world.getAgentBody()` |
| `addObject(obj)` | Add world object | `(obj: WorldObject) -> None` | `world.addObject(obj)` |
| `removeObject(x, y)` | Remove world object | `(x: float, y: float) -> None` | `world.removeObject(5, 5)` |
| `isValidPosition(x, y)` | Check if position is valid | `(x: float, y: float) -> bool` | `valid = world.isValidPosition(5, 5)` |

#### World Information

| Method | Description | Signature | Example |
|--------|-------------|-----------|---------|
| `getWidth()` | Get world width | `() -> int` | `width = world.getWidth()` |
| `getHeight()` | Get world height | `() -> int` | `height = world.getHeight()` |
| `getMaxEnergy()` | Get maximum energy | `() -> float` | `max_energy = world.getMaxEnergy()` |
| `setMaxEnergy(e)` | Set maximum energy | `(e: float) -> None` | `world.setMaxEnergy(100.0)` |
| `getEnergyDecayRate()` | Get energy decay rate | `() -> float` | `decay = world.getEnergyDecayRate()` |
| `setEnergyDecayRate(r)` | Set energy decay rate | `(r: float) -> None` | `world.setEnergyDecayRate(0.1)` |
| `getSimulationTime()` | Get simulation time | `() -> float` | `time = world.getSimulationTime()` |
| `setRandomSeed(seed)` | Set random seed | `(seed: int) -> None` | `world.setRandomSeed(42)` |
| `getRandomSeed()` | Get random seed | `() -> int` | `seed = world.getRandomSeed()` |

---

## pynlm Module

### Module Overview

The `pynlm` module provides Python bindings for the NLM system. It includes:

- Core classes: `Brain`, `Config`, `AgentBrain`, `World`
- Helper functions for common operations
- Convenience functions for rapid development
- Advanced analysis tools
- Training and monitoring utilities

### Module Functions

| Function | Description | Signature | Example |
|----------|-------------|-----------|---------|
| `createDefaultConfig()` | Create a default configuration | `() -> Config` | `config = pynlm.createDefaultConfig()` |
| `createBrain(config)` | Create a new brain with configuration | `(config: Config) -> Brain` | `brain = pynlm.createBrain(config)` |
| `createSimpleWorld()` | Create a new simple world | `() -> World` | `world = pynlm.createSimpleWorld()` |
| `createAgentBrain(brain)` | Create a new agent brain interface | `(brain: Brain) -> AgentBrain` | `agent = pynlm.createAgentBrain(brain)` |

### Convenience Functions

| Function | Description | Signature | Example |
|----------|-------------|-----------|---------|
| `quickStart()` | Quick start example - create and initialize a basic brain | `() -> Tuple[Brain, World, AgentBrain]` | `brain, world, agent = pynlm.quickStart()` |
| `demoMode(mode)` | Run demo mode with predefined scenarios | `(mode: str) -> Dict` | `result = pynlm.demoMode("learning")` |
| `benchmark(brain_type, steps)` | Benchmark brain performance | `(brain_type: str, steps: int) -> Dict` | `results = pynlm.benchmark("standard", 1000)` |

### Error Handling Functions

| Function | Description | Signature | Example |
|----------|-------------|-----------|---------|
| `validateConfig(config)` | Validate configuration and return results | `(config: Config) -> Dict[str, Any]` | `results = pynlm.validateConfig(config)` |
| `fixConfigDuplicates(config)` | Fix duplicate configuration entries | `(config: Config) -> bool` | `fixed = pynlm.fixConfigDuplicates(config)` |

### Configuration Functions

| Function | Description | Signature | Example |
|----------|-------------|-----------|---------|
| `mergeConfigs(base, override)` | Merge two configuration objects | `(base: Config, override: Config) -> Config` | `merged = pynlm.mergeConfigs(base, override)` |
| `exportConfig(config, format)` | Export configuration to different format | `(config: Config, format: str) -> str` | `json_config = pynlm.exportConfig(config, "json")` |
| `importConfig(data, format)` | Import configuration from different format | `(data: str, format: str) -> Config` | `config = pynlm.importConfig(json_data, "json")` |

### Analysis Functions

| Function | Description | Signature | Example |
|----------|-------------|-----------|---------|
| `analyzeBrainActivity(brain)` | Analyze brain activity patterns | `(brain: Brain) -> Dict[str, Any]` | `analysis = pynlm.analyzeBrainActivity(brain)` |
| `compareBrainStates(brain1, brain2)` | Compare brain states | `(brain1: Brain, brain2: Brain) -> float` | `similarity = pynlm.compareBrainStates(brain1, brain2)` |
| `getBrainStatistics(brain)` | Get comprehensive brain statistics | `(brain: Brain) -> Dict[str, Any]` | `stats = pynlm.getBrainStatistics(brain)` |
| `getAgentStatistics(agent)` | Get comprehensive agent statistics | `(agent: AgentBrain) -> Dict[str, Any]` | `stats = pynlm.getAgentStatistics(agent)` |
| `getBrainActivityHeatmap(brain)` | Get activity heatmap of the brain | `(brain: Brain) -> List[List[float]]` | `heatmap = pynlm.getBrainActivityHeatmap(brain)` |
| `getBrainPerformanceMetrics(brain)` | Get performance metrics | `(brain: Brain) -> Dict[str, Any]` | `metrics = pynlm.getBrainPerformanceMetrics(brain)` |
| `cloneBrain(brain)` | Clone brain state | `(brain: Brain) -> Brain` | `brain2 = pynlm.cloneBrain(brain)` |

### Training Functions

| Function | Description | Signature | Example |
|----------|-------------|-----------|---------|
| `trainAgentEpisode(agent, brain, world, steps)` | Train agent for one episode | `(agent: AgentBrain, brain: Brain, world: World, steps: int) -> Dict` | `result = pynlm.trainAgentEpisode(agent, brain, world, 100)` |

### Utility Functions

| Function | Description | Signature | Example |
|----------|-------------|-----------|---------|
| `createTutorialBrain()` | Create a brain configured for tutorial/demo purposes | `() -> Brain` | `brain = pynlm.createTutorialBrain()` |
| `createBenchmarkBrain(neuron_count)` | Create a brain configured for benchmarking | `(neuron_count: int) -> Brain` | `brain = pynlm.createBenchmarkBrain(1000)` |
| `createResearchBrain()` | Create a brain configured for research purposes | `() -> Brain` | `brain = pynlm.createResearchBrain()` |

### Error Handling Functions

| Function | Description | Signature | Example |
|----------|-------------|-----------|---------|
| `getLastError()` | Get the last error message | `() -> str` | `error = pynlm.getLastError()` |
| `clearErrors()` | Clear error history | `() -> None` | `pynlm.clearErrors()` |
| `getErrorCount()` | Get error count | `() -> int` | `count = pynlm.getErrorCount()` |

### Performance Functions

| Function | Description | Signature | Example |
|----------|-------------|-----------|---------|
| `optimizeBrainForPerformance(brain)` | Optimize brain for performance | `(brain: Brain) -> None` | `pynlm.optimizeBrainForPerformance(brain)` |
| `profileBrainPerformance(brain, duration)` | Profile brain performance | `(brain: Brain, duration: int) -> Dict` | `profile = pynlm.profileBrainPerformance(brain, 60)` |

### Monitoring Functions

| Function | Description | Signature | Example |
|----------|-------------|-----------|---------|
| `startMonitoring(brain, interval)` | Start monitoring brain | `(brain: Brain, interval: float) -> None` | `pynlm.startMonitoring(brain, 1.0)` |
| `stopMonitoring()` | Stop all monitoring | `() -> None` | `pynlm.stopMonitoring()` |
| `getMonitoringData()` | Get current monitoring data | `() -> Dict[str, Any]` | `data = pynlm.getMonitoringData()` |

### Batch Processing Functions

| Function | Description | Signature | Example |
|----------|-------------|-----------|---------|
| `batchProcessBrains(brains)` | Process multiple brains in batch | `(brains: List[Brain]) -> List[Dict]` | `results = pynlm.batchProcessBrains(brains)` |
| `compareBrainSets(brains)` | Compare multiple brain sets | `(brains: List[Brain]) -> Dict[str, Any]` | `comparison = pynlm.compareBrainSets(brains)` |

### Advanced Features

| Function | Description | Signature | Example |
|----------|-------------|-----------|---------|
| `enableAdvancedLearning(brain)` | Enable advanced learning features | `(brain: Brain) -> None` | `pynlm.enableAdvancedLearning(brain)` |
| `enableNeuromodulation(brain, enable)` | Enable or disable neuromodulation | `(brain: Brain, enable: bool) -> None` | `pynlm.enableNeuromodulation(brain, True)` |
| `enableSelfOrganization(brain)` | Enable self-organization features | `(brain: Brain) -> None` | `pynlm.enableSelfOrganization(brain)` |

---

## Exceptions

### Custom Exceptions

| Exception | Description | When raised |
|-----------|-------------|-------------|
| `NLMError` | General NLM error | Any NLM operation failure |
| `InvalidArgumentError` | Invalid argument provided | Invalid parameters |
| `OutOfRangeError` | Value out of valid range | Invalid configuration values |

### Exception Usage

```python
import pynlm

try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brain.step(0)
except pynlm.NLMError as e:
    print(f"NLM Error: {e}")
    # Handle NLM-specific errors
except pynlm.InvalidArgumentError as e:
    print(f"Invalid argument: {e}")
    # Handle invalid argument errors
except pynlm.OutOfRangeError as e:
    print(f"Out of range: {e}")
    # Handle out of range errors
except Exception as e:
    print(f"Unexpected error: {e}")
    # Handle any other unexpected errors
```

---

## Error Functions

| Function | Description | Example |
|----------|-------------|---------|
| `getLastError()` | Get the last error message | `error = pynlm.getLastError()` |
| `clearErrors()` | Clear error history | `pynlm.clearErrors()` |
| `getErrorCount()` | Get error count | `count = pynlm.getErrorCount()` |

---

## Conclusion

This API reference provides comprehensive documentation for the enhanced NLM Python bindings. It includes:

- **Complete class documentation** for all core classes
- **Detailed method descriptions** with examples
- **Parameter and return value information** for all functions
- **Error handling** with custom exceptions
- **Convenience functions** for rapid development
- **Advanced analysis tools** for research and development

The enhanced Python API is designed to be:

1. **Intuitive** for Python developers
2. **Comprehensive** for all use cases
3. **Flexible** for various applications
4. **Robust** with comprehensive error handling
5. **Performant** with optimization tools

**Start using NLM Python API today and experience the power of enhanced neural simulation!** 🚀