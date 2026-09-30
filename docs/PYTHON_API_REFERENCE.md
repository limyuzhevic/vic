# NLM Python API Reference

## Overview

This document provides comprehensive reference documentation for the NLM (Neural Learning Machine) Python API. It includes:

- **API Structure**: Overview of the Python API modules and their components
- **Class Reference**: Detailed documentation for all Python classes
- **Function Reference**: Documentation for all functions and methods
- **Type Definitions**: Complete type information for all data structures
- **Usage Examples**: Code examples for common usage patterns

## Table of Contents

1. [API Structure](#api-structure)
2. [Core Classes](#core-classes)
3. [Configuration Classes](#configuration-classes)
4. [Performance Classes](#performance-classes)
5. [Experiment Classes](#experiment-classes)
6. [Utility Classes](#utility-classes)
7. [Function Reference](#function-reference)
8. [Type Definitions](#type-definitions)
9. [Migration Guide](#migration-guide)

## API Structure

The NLM Python API consists of several modules:

### Main Module

```python
from nlm import (
    Config,
    Brain,
    SimpleWorld,
    AgentBrain,
    Action,
    SensoryPercept,
    Vision,
    Audio,
    InternalSignals,
    ActionType,
    NeuronType,
    DevelopmentalStage,
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId
)
```

### Advanced Modules

```python
from nlm.advanced import (
    AdvancedExperiment,
    CustomEnvironment
)

from nlm.profiling import (
    PerformanceProfiler,
    MemoryMonitor,
    CPUMonitor,
    SimulationProfiler
)

from nlm.experiments import (
    Experiment,
    ExperimentSuite,
    ExperimentConfig,
    ExperimentResult,
    ExperimentType,
    ExperimentStatus
)
```

## Core Classes

### Config

#### Overview
The `Config` class provides configuration management for NLM systems.

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `loadFromFile(filepath)` | Load configuration from JSON file | `filepath: str` | `bool` |
| `loadFromArgs(argc, argv)` | Load configuration from command line | `argc: int, argv: List[str]` | `bool` |
| `saveToFile(filepath)` | Save configuration to JSON file | `filepath: str` | `bool` |
| `getInt(key, defaultValue)` | Get integer value | `key: str, defaultValue: int = 0` | `int` |
| `getFloat(key, defaultValue)` | Get float value | `key: str, defaultValue: float = 0.0` | `float` |
| `getBool(key, defaultValue)` | Get boolean value | `key: str, defaultValue: bool = false` | `bool` |
| `getString(key, defaultValue)` | Get string value | `key: str, defaultValue: str = ""` | `str` |
| `setInt(key, value)` | Set integer value | `key: str, value: int` | `void` |
| `setFloat(key, value)` | Set float value | `key: str, value: float` | `void` |
| `setBool(key, value)` | Set boolean value | `key: str, value: bool` | `void` |
| `setString(key, value)` | Set string value | `key: str, value: str` | `void` |
| `has(key)` | Check if key exists | `key: str` | `bool` |
| `getKeys()` | Get all configuration keys | - | `List[str]` |
| `clear()` | Clear all configuration entries | - | `void` |
| `summary()` | Get configuration summary | - | `str` |

#### Example

```python
config = Config()
config.setInt("world.width", 200)
config.setFloat("energy.decay_rate", 0.01)

width = config.getInt("world.width", 100)
decay_rate = config.getFloat("energy.decay_rate", 0.05)

config.saveToFile("config.json")
```

### Brain

#### Overview
The `Brain` class represents the central neural simulation brain in NLM.

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize()` | Initialize brain with configuration | - | `bool` |
| `step(currentStep)` | Perform simulation step | `currentStep: SimulationStep` | `void` |
| `step(currentStep, currentTime)` | Perform simulation step with timestamp | `currentStep: SimulationStep, currentTime: Timestamp` | `void` |
| `receiveSensoryInput(input)` | Inject sensory input | `input: SensoryInput` | `void` |
| `injectCurrent(neuron, current)` | Inject current into neuron | `neuron: NeuronId, current: MembranePotential` | `void` |
| `injectCurrentToNeurons(type, current)` | Inject current to neurons of type | `type: NeuronType, current: MembranePotential` | `void` |
| `produceAction()` | Produce motor action | - | `unique_ptr<Action>` |
| `reset()` | Reset brain state | - | `void` |
| `save(filepath)` | Save brain state | `filepath: str` | `bool` |
| `load(filepath)` | Load brain state | `filepath: str` | `bool` |
| `addRegion(name)` | Add new neural region | `name: str = ""` | `RegionId` |
| `getRegion(id)` | Get region by ID | `id: RegionId` | `NeuralRegion*` |
| `getRegionCount()` | Get number of regions | - | `size_t` |
| `getRegionIds()` | Get all region IDs | - | `vector<RegionId>` |
| `getRegions()` | Get all regions | - | `const vector<unique_ptr<NeuralRegion>>&` |
| `getTotalNeuronCount()` | Get total neuron count | - | `size_t` |
| `getTotalSynapseCount()` | Get total synapse count | - | `size_t` |
| `getActiveNeuronCount()` | Get active neuron count | - | `size_t` |
| `getFiringNeuronCount()` | Get firing neuron count | - | `size_t` |
| `getAverageFiringRate()` | Get average firing rate | - | `float` |
| `getExcitationInhibitionRatio()` | Get E/I balance ratio | - | `float` |
| `getTotalSpikeCount()` | Get total spike count | - | `size_t` |
| `getDevelopmentalStage()` | Get developmental stage | - | `DevelopmentalStage` |
| `setDevelopmentalStage(stage)` | Set developmental stage | `stage: DevelopmentalStage` | `void` |
| `getConfig()` | Get configuration | - | `shared_ptr<const Config>` |
| `logStatus()` | Log brain status | - | `void` |

#### Example

```python
brain = Brain(config)

if brain.initialize():
    print("Brain initialized successfully")

# Get brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### SimpleWorld

#### Overview
The `SimpleWorld` class represents a 2D world for NLM simulation.

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `configure(width, height, visionWidth, visionHeight)` | Configure world | `width: int, height: int, visionWidth: int, visionHeight: int` | `void` |
| `reset()` | Reset world | - | `void` |
| `setAgentStart(x, y)` | Set agent starting position | `x: float, y: float` | `void` |
| `update(timestep)` | Update world | `timestep: double` | `void` |
| `applyMotorCommand(cmd, currentTime)` | Apply motor command | `cmd: MotorCommand, currentTime: double` | `void` |
| `getSensoryPercept()` | Get sensory percept | - | `SensoryPercept` |
| `getAgentBody()` | Get agent body | - | `AgentBody` |
| `addObject(obj)` | Add world object | `obj: WorldObject` | `void` |
| `removeObject(x, y)` | Remove object at position | `x: float, y: float` | `void` |
| `isValidPosition(x, y)` | Check position validity | `x: float, y: float` | `bool` |
| `getWidth()` | Get world width | - | `int` |
| `getHeight()` | Get world height | - | `int` |
| `getMaxEnergy()` | Get max energy | - | `float` |
| `setMaxEnergy(e)` | Set max energy | `e: float` | `void` |
| `getEnergyDecayRate()` | Get energy decay rate | - | `float` |
| `setEnergyDecayRate(r)` | Set energy decay rate | `r: float` | `void` |
| `getSimulationTime()` | Get simulation time | - | `double` |
| `setRandomSeed(seed)` | Set random seed | `seed: unsigned int` | `void` |
| `getRandomSeed()` | Get random seed | - | `unsigned int` |

#### Example

```python
world = SimpleWorld()
world.configure(100, 100, 10, 10)
world.setAgentStart(50, 50)

for step in range(100):
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Apply action
    world.applyMotorCommand(MotorCommand.MoveForward, world.getSimulationTime())
    
    # Update world
    world.update(0.01)
```

### AgentBrain

#### Overview
The `AgentBrain` class connects NLM brain to world simulation.

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize(world)` | Initialize with world | `world: SimpleWorld` | `void` |
| `getSensoryInputSize()` | Get expected sensory input size | - | `size_t` |
| `getMotorOutputSize()` | Get expected motor output size | - | `size_t` |
| `processSensoryInput(percept)` | Process sensory percept | `percept: SensoryPercept` | `void` |
| `decodeMotorCommand()` | Decode motor command | - | `Action` |
| `applyRewardModulation(reward, predictedReward)` | Apply reward modulation | `reward: float, predictedReward: float` | `void` |
| `updateDevelopment(timestep)` | Update development system | `timestep: double` | `void` |
| `getDevelopmentalStage()` | Get developmental stage | - | `DevelopmentalStage` |
| `getNeuromodulationLevel()` | Get neuromodulation level | - | `float` |
| `getCuriosityLevel()` | Get curiosity level | - | `float` |
| `getNoveltyLevel()` | Get novelty level | - | `float` |
| `getPredictionError()` | Get prediction error | - | `float` |
| `reset()` | Reset agent | - | `void` |
| `getBrain()` | Get underlying brain | - | `Brain*` |
| `enableRewardModulation(enable)` | Enable/disable reward modulation | `enable: bool` | `void` |
| `enableStructuralPlasticity(enable)` | Enable/disable structural plasticity | `enable: bool` | `void` |
| `enableDevelopment(enable)` | Enable/disable development | `enable: bool` | `void` |
| `enableCuriosity(enable)` | Enable/disable curiosity | `enable: bool` | `void` |
| `isRewardModulationEnabled()` | Check if reward modulation enabled | - | `bool` |
| `isStructuralPlasticityEnabled()` | Check if structural plasticity enabled | - | `bool` |
| `isDevelopmentEnabled()` | Check if development enabled | - | `bool` |
| `isCuriosityEnabled()` | Check if curiosity enabled | - | `bool` |

## Configuration Classes

### PyConfig

#### Overview
The `PyConfig` class provides enhanced configuration with JSON/YAML support.

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `loadFromFile(filepath)` | Load from JSON file | `filepath: str` | `bool` |
| `loadFromYAML(filepath)` | Load from YAML file | `filepath: str` | `bool` |
| `loadFromArgs(argc, argv)` | Load from command line | `argc: int, argv: List[str]` | `bool` |
| `saveToFile(filepath)` | Save to JSON file | `filepath: str` | `bool` |
| `getInt(key, defaultValue)` | Get integer | `key: str, defaultValue: int = 0` | `int` |
| `getFloat(key, defaultValue)` | Get float | `key: str, defaultValue: float = 0.0` | `float` |
| `getBool(key, defaultValue)` | Get boolean | `key: str, defaultValue: bool = false` | `bool` |
| `getString(key, defaultValue)` | Get string | `key: str, defaultValue: str = ""` | `str` |
| `setInt(key, value)` | Set integer | `key: str, value: int` | `void` |
| `setFloat(key, value)` | Set float | `key: str, value: float` | `void` |
| `setBool(key, value)` | Set boolean | `key: str, value: bool` | `void` |
| `setString(key, value)` | Set string | `key: str, value: str` | `void` |
| `has(key)` | Check key exists | `key: str` | `bool` |
| `getKeys()` | Get all keys | - | `List[str]` |
| `clear()` | Clear all entries | - | `void` |
| `summary()` | Get summary | - | `str` |
| `toString()` | String representation | - | `str` |
| `_PyConfig__enter__()` | Context manager enter | - | `void` |
| `_PyConfig__exit__()` | Context manager exit | - | `void` |

### EnvironmentConfig

#### Overview
The `EnvironmentConfig` class handles environment variable overrides.

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `loadFromEnvironment(config)` | Load from environment | `config: PyConfig` | `bool` |
| `getEnv(name, defaultValue)` | Get environment variable | `name: str, defaultValue: str = ""` | `static str` |
| `hasEnv(name)` | Check environment variable exists | `name: str` | `static bool` |

### ValidatedConfig

#### Overview
The `ValidatedConfig` class provides configuration validation.

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `addValidationRule(key, validator, errorMessage)` | Add validation rule | `key: str, validator: function, errorMessage: str = ""` | `void` |
| `validate()` | Validate configuration | - | `List[str]` |
| `getValidationErrors()` | Get validation errors | - | `str` |
| `isValid()` | Check if valid | - | `bool` |
| `_ValidatedConfig__getConfig()` | Get config reference | - | `PyConfig` |

## Performance Classes

### PerformanceProfiler

#### Overview
The `PerformanceProfiler` class provides comprehensive performance profiling.

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `start_profiling()` | Start profiling | - | `void` |
| `stop_profiling()` | Stop profiling | - | `void` |
| `benchmark_function(func, *args, **kwargs)` | Benchmark function | `func: callable, *args, **kwargs` | `BenchmarkResult` |
| `benchmark_simulation(brain, world, num_steps, num_runs)` | Benchmark simulation | `brain: Brain, world: SimpleWorld, num_steps: int, num_runs: int = 10` | `List[BenchmarkResult]` |
| `run_suite(test_functions, suite_name)` | Run benchmark suite | `test_functions: List[callable], suite_name: str = "default"` | `Dict` |
| `compare_results(result1, result2)` | Compare results | `result1: BenchmarkResult, result2: BenchmarkResult` | `Dict` |
| `get_summary()` | Get summary | - | `Dict` |
| `save_results(filepath)` | Save results | `filepath: str` | `void` |
| `load_results(filepath)` | Load results | `filepath: str` | `void` |
| `generate_report()` | Generate report | - | `str` |

### MemoryMonitor

#### Overview
The `MemoryMonitor` class provides real-time memory monitoring.

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `get_current_memory()` | Get current memory | - | `MemoryMetrics` |
| `start_monitoring()` | Start monitoring | - | `void` |
| `stop_monitoring()` | Stop monitoring | - | `void` |
| `get_average_memory()` | Get average memory | - | `MemoryMetrics` |
| `get_peak_memory()` | Get peak memory | - | `MemoryMetrics` |
| `clear_history()` | Clear history | - | `void` |
| `get_history_size()` | Get history size | - | `int` |
| `save_history(filepath)` | Save history | `filepath: str` | `void` |
| `load_history(filepath)` | Load history | `filepath: str` | `void` |

### CPUMonitor

#### Overview
The `CPUMonitor` class provides real-time CPU monitoring.

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `get_current_cpu()` | Get current CPU | - | `CPUMetrics` |
| `start_monitoring()` | Start monitoring | - | `void` |
| `stop_monitoring()` | Stop monitoring | - | `void` |
| `get_average_cpu()` | Get average CPU | - | `CPUMetrics` |
| `clear_history()` | Clear history | - | `void` |

### SimulationProfiler

#### Overview
The `SimulationProfiler` class provides neural simulation profiling.

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `get_current_simulation_metrics(brain, world)` | Get simulation metrics | `brain: Brain, world: SimpleWorld = None` | `SimulationMetrics` |
| `start_profiling()` | Start profiling | - | `void` |
| `stop_profiling()` | Stop profiling | - | `void` |
| `profile_step(brain, step_func, *args, **kwargs)` | Profile step | `brain: Brain, step_func: callable, *args, **kwargs` | `Dict` |
| `profile_simulation(brain, world, num_steps)` | Profile simulation | `brain: Brain, world: SimpleWorld, num_steps: int` | `List[Dict]` |
| `clear_history()` | Clear history | - | `void` |
| `get_average_metrics()` | Get average metrics | - | `SimulationMetrics` |
| `save_results(filepath)` | Save results | `filepath: str` | `void` |
| `load_results(filepath)` | Load results | `filepath: str` | `void` |

## Experiment Classes

### AdvancedExperiment

#### Overview
The `AdvancedExperiment` class provides comprehensive experiment management.

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `run_parameter_sweep(param_grid, brain_factory)` | Run parameter sweep | `param_grid: Dict[str, List], brain_factory: callable` | `Dict` |
| `run_multi_environment(environments, brain_factory)` | Run multi-environment | `environments: List[Dict], brain_factory: callable` | `Dict` |
| `run_custom_agent(agent_factory, num_episodes)` | Run custom agent | `agent_factory: callable, num_episodes: int = 100` | `Dict` |
| `save_results(filepath)` | Save results | `filepath: str` | `void` |
| `load_results(filepath)` | Load results | `filepath: str` | `void` |
| `analyze_results()` | Analyze results | - | `Dict` |
| `plot_results(output_dir)` | Plot results | `output_dir: str = "."` | `void` |
| `run()` | Run experiment | - | `Dict` |

### Experiment

#### Overview
The `Experiment` class provides base experiment functionality.

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `load_config(filepath)` | Load configuration | `filepath: str` | `void` |
| `save_config(filepath)` | Save configuration | `filepath: str` | `void` |
| `run()` | Run experiment | - | `ExperimentResult` |

### ExperimentSuite

#### Overview
The `ExperimentSuite` class provides experiment suite management.

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `add_experiment(experiment)` | Add experiment | `experiment: Experiment` | `void` |
| `remove_experiment(experiment_name)` | Remove experiment | `experiment_name: str` | `void` |
| `run_all()` | Run all experiments | - | `Dict[str, ExperimentResult]` |
| `run_selected(experiment_names)` | Run selected | `experiment_names: List[str]` | `Dict[str, ExperimentResult]` |
| `get_suite_summary()` | Get summary | - | `Dict` |
| `save_suite_results(filepath)` | Save suite results | `filepath: str` | `void` |
| `load_suite_results(filepath)` | Load suite results | `filepath: str` | `void` |

## Utility Classes

### NeuronPopulation

#### Overview
The `NeuronPopulation` class provides custom neuron population creation.

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------er-------------|------------|---------|
| `createExcitatoryPopulation(count, connectionProbability)` | Create excitatory population | `count: size_t, connectionProbability: float = 0.1` | `vector<NeuronId>` |
| `createRecurrentNetwork(numInput, numHidden, numOutput)` | Create recurrent network | `numInput: size_t, numHidden: size_t, numOutput: size_t` | `vector<NeuronId>` |
| `createCustomConnectivity(sourceNeurons, targetNeurons, weights)` | Create custom connectivity | `sourceNeurons: vector<NeuronId>, targetNeurons: vector<NeuronId>, weights: vector<float>` | `void` |

### BatchProcessor

#### Overview
The `BatchProcessor` class provides batch processing utilities.

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `processBatch(percepts)` | Process batch | `percepts: vector<SensoryPercept>` | `vector<Action>` |
| `processSingle(percept)` | Process single | `percept: SensoryPercept` | `Action` |
| `simulateBatch(numSteps, batchSize)` | Simulate batch | `numSteps: int, batchSize: int` | `vector<vector<Action>>` |
| `saveBatchResults(results, filepath)` | Save batch results | `results: vector<vector<Action>>, filepath: str` | `bool` |

## Function Reference

### Global Functions

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `createDefaultConfig()` | Create default configuration | - | `shared_ptr<Config>` |
| `createBrain(config)` | Create brain with configuration | `config: shared_ptr<Config>` | `shared_ptr<Brain>` |
| `createSimpleWorld()` | Create simple world | - | `shared_ptr<SimpleWorld>` |
| `createAgentBrain(brain)` | Create agent brain interface | `brain: shared_ptr<Brain>` | `shared_ptr<AgentBrain>` |

### Factory Functions

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `createEnhancedBrain(configFile = "")` | Create enhanced brain | `configFile: str = ""` | `shared_ptr<PyBrain>` |
| `createEnhancedAgent(configFile = "")` | Create enhanced agent | `configFile: str = ""` | `shared_ptr<PyAgent>` |
| `quickStart()` | Quick start with minimal configuration | - | `shared_ptr<PyAgent>` |

## Type Definitions

### Core Types

| Type | Description |
|------|-------------|
| `NeuronId` | Unique identifier for a neuron |
| `SynapseId` | Unique identifier for a synapse |
| `RegionId` | Unique identifier for a brain region |
| `PopulationId` | Unique identifier for a neuron population |
| `NeuronType` | Neuron type enumeration |
| `SynapseType` | Synapse type enumeration |
| `DevelopmentalStage` | Developmental stage enumeration |
| `FiringState` | Neuron firing state enumeration |
| `ActionType` | Action type enumeration |
| `MotorCommand` | Low-level motor command enumeration |
| `WorldObjectType` | World object type enumeration |
| `SimulationStep` | Simulation step type |
| `Timestamp` | Timestamp type |
| `MembranePotential` | Membrane potential type |

### Configuration Types

| Type | Description |
|------|-------------|
| `ConfigValue` | Configuration value type (variant) |
| `ConfigSource` | Configuration source enumeration |
| `ConfigEntry` | Configuration entry structure |

### Performance Types

| Type | Description |
|------|-------------|
| `MemoryMetrics` | Memory usage metrics |
| `CPUMetrics` | CPU usage metrics |
| `SimulationMetrics` | Neural simulation metrics |
| `BenchmarkResult` | Benchmark test result |

### Experiment Types

| Type | Description |
|------|-------------|
| `ExperimentConfig` | Experiment configuration |
| `ExperimentResult` | Experiment result data |
| `ExperimentStatus` | Experiment status enumeration |
| `ExperimentType` | Experiment type enumeration |

## Migration Guide

### Migration from Legacy API to Enhanced API

This section provides guidance for migrating from the legacy NLM Python API to the enhanced API.

#### Key Changes

1. **EnhancedBrain vs Brain**: Replace `Brain` with `EnhancedBrain` for Pythonic interface
2. **PyConfig vs Config**: Replace `Config` with `PyConfig` for enhanced functionality
3. **World vs SimpleWorld**: Replace `SimpleWorld` with `World` for convenience methods
4. **Agent vs PyAgent**: Replace `Agent` with `PyAgent` for high-level abstractions
5. **MemoryManager**: Use `PyMemoryManager` for memory management

#### Migration Examples

##### Legacy to Enhanced Configuration

```python
# Legacy code
config = Config()
config.loadFromFile("config.json")
width = config.getOr<int>("world.width", 100)
config.set<int>("world.width", 200)

# Enhanced code
config = PyConfig()
config.loadFromFile("config.json")
width = config.getInt("world.width", 100)
config.setInt("world.width", 200)
```

##### Legacy to Enhanced Brain

```python
# Legacy code
brain = Brain(config)
brain.initialize()
brain.step(step)
action = brain.produceAction()

# Enhanced code
brain = EnhancedBrain()
brain.initialize()
brain.step(step)
action = brain.produceAction()
```

##### Legacy to Enhanced World

```python
# Legacy code
world = SimpleWorld()
world.configure(100, 100, 10, 10)
world.setAgentStart(50, 50)
world.update(0.01)

# Enhanced code
world = World()
world.configure(100, 100, 10, 10)
world.setAgentStart(50, 50)
world.step(0.01)
```

#### Backward Compatibility

The enhanced API maintains backward compatibility through:

1. **Legacy Aliases**: `Config` is available as `ConfigLegacy`
2. **Original Factory Functions**: Original `createDefaultConfig`, `createBrain`, etc. remain available
3. **Original ID Types**: Original `NeuronId`, `SynapseId`, etc. remain available
4. **Original Enums**: Original enumerations remain available

#### Example: Backward Compatible Usage

```python
# Use enhanced API
from nlm import PyConfig, EnhancedBrain, World

# Or use legacy API (still available)
from nlm import Config, Brain, SimpleWorld

# Both work
config = PyConfig()  # Enhanced
config = Config()    # Legacy

brain = EnhancedBrain()  # Enhanced
brain = Brain()          # Legacy

world = World()          # Enhanced
world = SimpleWorld()     # Legacy
```

## Conclusion

This document provides comprehensive reference documentation for the enhanced NLM Python API. The API provides:

- **Pythonic interfaces** for intuitive usage
- **Advanced configuration systems** with JSON/YAML support
- **Performance monitoring** tools for optimization
- **Experiment management** for systematic research
- **Custom neuron population** creation for specialized research
- **Batch processing** utilities for efficient simulation

The enhanced API maintains backward compatibility while providing powerful new features for neural simulation research and experimentation.
