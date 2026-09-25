# Enhanced NLM Python Bindings

This document provides comprehensive documentation for the enhanced Python bindings of the NLM (Neural Learning Machine) neural simulation framework.

## Overview

The enhanced Python bindings (`enhanced_pynlm`) provide a comprehensive API for working with the NLM neural simulation framework. This enhanced version includes advanced functionality for simulation control, memory management, neuromodulation, performance monitoring, and debugging capabilities.

### Key Features

- **Enhanced Simulation Control**: Advanced step control, batch operations, and simulation management
- **Memory Management**: Working memory, episodic memory, and associative memory control
- **Neuromodulation**: Advanced dopamine, curiosity, and other neuromodulation systems
- **Performance Monitoring**: Comprehensive performance metrics and profiling
- **Debug and Development Tools**: Brain state inspection and debugging utilities
- **Experiment Configuration**: Phase 6 experiment templates and configuration
- **Batch Operations**: Efficient batch processing for multiple operations
- **Educational Support**: Comprehensive examples and tutorials

## Installation

Install the enhanced bindings using:

```bash
pip install -e .[dev]
```

## Basic Usage

### Create and Configure a Brain

```python
import enhanced_pynlm as nlm

# Create a default configuration
config = nlm.createEnhancedConfig()

# Set custom parameters
config.set("random_seed", 42)
config.set("neuron_count", 1000)
config.set("region_count", 1)
config.set("connection_probability", 0.1)

# Create brain with enhanced configuration
brain = nlm.createEnhancedBrain(config)

# Initialize the brain
brain.initialize()
```

### Run a Simulation

```python
# Run simulation with progress reporting
for step in range(1000):
    brain.step(step, step * 0.001)
    if step % 100 == 0:
        print(f"Step {step}, Spikes: {brain.getTotalSpikeCount()}")
```

### Get Brain Status

```python
# Get detailed brain status
status = brain.getDetailedStatus()
print(f"Neurons: {status['totalNeurons']}")
print(f"Spikes: {status['totalSpikes']}")
print(f"Active Neurons: {status['activeNeurons']}")
print(f"Developmental Stage: {status['developmentalStage']}")
```

## Advanced Usage

### Memory Management

```python
import enhanced_pynlm.memory as memory

# Configure memory systems
brain.getWorkingMemory().setCapacity(1000)
brain.getEpisodicMemory().setMaxEpisodes(1000)

# Get memory statistics
stats = memory.getWorkingMemoryStats(brain)
print(f"Working Memory Utilization: {stats['workingMemoryUtilization']}")

# Store and retrieve memory
brain.getWorkingMemory().storeToNeuron(neuron_id, activation)
memory = brain.getWorkingMemory().getMemoryTrace(neuron_id)

# Consolidate episodic memory
memory.consolidate(0.5)  # Keep memories with activation > 0.5
```

### Neuromodulation Control

```python
import enhanced_pynlm.neuromodulation as neuromodulation

# Set neuromodulation levels
neuromodulation.setDopamineLevel(brain, 0.5)
neuromodulation.setCuriosityLevel(brain, 0.3)
neuromodulation.setPredictionError(brain, 0.1)

# Get current neuromodulation levels
levels = brain.getAllNeuromodulationLevels()
print(f"Dopamine: {levels['dopamine']}")
print(f"Curiosity: {levels['curiosity']}")
print(f"Novelty: {levels['novelty']}")
print(f"Prediction Error: {levels['predictionError']}")

# Apply neuromodulation sequence
sequence = [
    ("dopamine", 0.2),
    ("curiosity", 0.5),
    ("prediction_error", 0.3)
]
neuromodulation.neuromodulationSequence(brain, sequence)
```

### Batch Operations

```python
import enhanced_pynlm.simulation as simulation

# Batch process sensory inputs
percepts = [create_vision_percept() for _ in range(10)]
actions = simulation.batchProduceActions(percepts, brain)

# Batch inject current
neuron_ids = [neuron_1_id, neuron_2_id, neuron_3_id]
simulation.batchInjectCurrent(brain, neuron_ids, 5.0)

# Batch enable plasticity
simulation.batchEnablePlasticity(brain, [neuron_1_id], [neuron_2_id], True, True)
```

### Performance Monitoring

```python
import enhanced_pynlm.performance as performance

# Get performance metrics
metrics = performance.getBrainPerformanceMetrics(brain)
print(f"Performance: {metrics['stepsPerSecond']} steps/sec")
print(f"Neurons processed: {metrics['neuronsProcessed']}")

# Memory performance
memory_perf = performance.getMemoryPerformance(brain)
print(f"Memory utilization: {memory_perf['workingMemoryUtilization']}")

# Neuromodulation performance
neuro_perf = performance.getNeuromodulationPerformance(brain)
print(f"Dopamine responsiveness: {neuro_perf['dopamineResponsiveness']}")

# Profile simulation
profile = performance.profileSimulation(brain, 1000, 0.001)
print(f"Average step time: {profile['avgStepTimeMs']} ms")
```

### Development Control

```python
# Create agent brain for development studies
agent_brain = nlm.createEnhancedAgentBrain(brain)
agent_brain.initialize(world)

# Configure development
agent_brain.configureProcessing(
    rewardModulation=True,
    structuralPlasticity=True,
    development=True,
    curiosity=True
)

# Advance development
agent_brain.advanceDevelopment(1.0)  # 1 second of development

# Get development status
dev_status = agent_brain.getDevelopmentStats()
print(f"Developmental Stage: {dev_status['developmentalStage']}")
print(f"Age: {dev_status['age']} seconds")
```

### Experiment Configuration

```python
import enhanced_pynlm.experiment as experiment

# Create Phase 6 experiment configuration
exp_config = experiment.createPhase6Config()

# Configure for learning
experiment.configureForLearning(exp_config,
                                enableLearning=True,
                                enableDevelopment=True,
                                enableMemory=True)

# Configure for development
experiment.configureForDevelopment(exp_config,
                                 enableDevelopment=True,
                                 enableStages=True,
                                 enablePlasticity=True)

# Configure for memory research
experiment.configureForMemoryResearch(exp_config,
                                     enableAllMemory=True,
                                     workingMemoryCapacity=1000,
                                     episodicMemoryCapacity=1000)

# Get experiment results
results = experiment.runPhase6(brain, exp_config)
analysis = experiment.analyzeExperimentResults(results)
print(f"Total Reward: {analysis['totalReward']}")
print(f"Memory Efficiency: {analysis['memoryEfficiency']}")
```

### Debug and Development

```python
import enhanced_pynlm.debug as debug

# Enable debug mode
nlm.enableDebugMode(True)

# Get brain state information
brain_info = debug.inspectBrainState(brain)
print(f"Neurons: {brain_info['neuronCount']}")
print(f"Synaptic connections: {brain_info['synapseCount']}")

# Validate brain state
validation = debug.validateBrainState(brain)
if not validation['valid']:
    print("Errors:", validation['errors'])

# Log debug information
nlm.logMessage("info", "Simulation step completed successfully")

# Set log level
nlm.setLogLevel("debug")
```

### Visualization

```python
import enhanced_pynlm as nlm

# Create visualization interface
viz = nlm.createVisualizationInterface()

# Configure visualization
viz.configureEnhanced(
    enable3D=True,
    neuronRadius=0.5,
    synapseThickness=0.2,
    memoryOpacity=0.7
)

# Create enhanced world
world = nlm.createEnhancedWorld()
world.configureEnhanced(100, 100, 20, 20, 100.0, 0.1)

# Set agent start
world.setAgentStart(50, 50)

# Render frame
viz.renderFrame(brain, world, 0.001)

# Save to file
viz.renderToFile("output.png", brain, world, 0.001, True)
```

## API Reference

### Core Classes

#### Config
- `createConfig()` - Create default configuration
- `createEnhancedConfig()` - Create enhanced configuration
- `set()` - Set configuration value
- `get()` - Get configuration value
- `has()` - Check if configuration exists
- `loadFromFile()` - Load configuration from file
- `saveToFile()` - Save configuration to file

#### Brain
- `createBrain()` - Create brain with configuration
- `createEnhancedBrain()` - Create enhanced brain
- `initialize()` - Initialize brain
- `step()` - Perform simulation step
- `stepMultiple()` - Perform multiple steps
- `receiveSensoryInput()` - Inject sensory input
- `produceAction()` - Produce motor action
- `reset()` - Reset brain state
- `save()` - Save brain state
- `load()` - Load brain state
- `getDetailedStatus()` - Get detailed brain status
- `getNeuronActivations()` - Get neuron activations

#### AgentBrain
- `createAgentBrain()` - Create agent brain interface
- `createEnhancedAgentBrain()` - Create enhanced agent brain
- `initialize()` - Initialize with world
- `processSensoryInput()` - Process sensory percept
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation()` - Apply reward modulation
- `updateDevelopment()` - Update development
- `configureProcessing()` - Configure processing subsystems
- `getAllNeuromodulationLevels()` - Get all neuromodulation levels

#### SimpleWorld
- `createWorld()` - Create world simulation
- `createEnhancedWorld()` - Create enhanced world
- `configure()` - Configure world
- `configureEnhanced()` - Configure enhanced world
- `update()` - Update world
- `updateEnhanced()` - Update enhanced world
- `applyMotorCommand()` - Apply motor command
- `getWorldStats()` - Get world statistics

#### Action
- `createAction()` - Create action
- `createActionWithParams()` - Create action with parameters

#### SensoryPercept
- `createSensoryPercept()` - Create sensory percept
- `getVision()`, `setVision()` - Vision methods
- `getTouch()`, `setTouch()` - Touch methods
- `getInternal()`, `setInternal()` - Internal methods

#### VisualizationInterface
- `createVisualizationInterface()` - Create visualization interface
- `createEnhancedVisualizationInterface()` - Create enhanced visualization
- `renderFrame()` - Render visualization frame
- `renderToFile()` - Render to file
- `configureEnhanced()` - Configure enhanced visualization

### Submodules

#### Core
- `core.createConfig()` - Create configuration
- `core.createEnhancedConfig()` - Create enhanced configuration
- `core.createBrain()` - Create brain
- `core.createEnhancedBrain()` - Create enhanced brain
- `core.createAgentBrain()` - Create agent brain
- `core.createEnhancedAgentBrain()` - Create enhanced agent brain

#### Simulation
- `simulation.step()` - Single simulation step
- `simulation.runSimulationLoop()` - Run simulation loop
- `simulation.saveSimulationState()` - Save simulation state
- `simulation.loadSimulationState()` - Load simulation state
- `simulation.getSimulationStats()` - Get simulation statistics
- `simulation.advanceTime()` - Advance simulation time
- `simulation.batchProcessSensory()` - Batch process sensory inputs
- `simulation.batchProduceActions()` - Batch produce actions
- `simulation.batchInjectCurrent()` - Batch inject current
- `simulation.batchEnablePlasticity()` - Batch enable plasticity

#### Memory
- `memory.getWorkingMemoryStats()` - Get working memory statistics
- `memory.clearWorkingMemory()` - Clear working memory
- `memory.storeWorkingMemory()` - Store working memory
- `memory.episodicMemoryStats()` - Get episodic memory statistics
- `memory.clearEpisodicMemory()` - Clear episodic memory
- `memory.consolidateEpisodicMemory()` - Consolidate episodic memory
- `memory.replayMemory()` - Replay memory
- `memory.pruneMemory()` - Prune memory
- `memory.getMemoryIntegrationScore()` - Get memory integration score

#### Neuromodulation
- `neuromodulation.setDopamineLevel()` - Set dopamine level
- `neuromodulation.setCuriosityLevel()` - Set curiosity level
- `neuromodulation.setPredictionError()` - Set prediction error
- `neuromodulation.neuromodulationSequence()` - Apply neuromodulation sequence
- `neuromodulation.neuromodulationInteraction()` - Model neuromodulation interactions

#### Development
- Development system utilities (placeholder)

#### Experiment
- `experiment.createPhase6Config()` - Create Phase 6 experiment configuration
- `experiment.configureForLearning()` - Configure for learning experiments
- `experiment.configureForDevelopment()` - Configure for development experiments
- `experiment.configureForMemoryResearch()` - Configure for memory research
- `experiment.analyzeExperimentResults()` - Analyze experiment results

#### Performance
- `performance.getBrainPerformanceMetrics()` - Get brain performance metrics
- `performance.getMemoryPerformance()` - Get memory performance metrics
- `performance.getNeuromodulationPerformance()` - Get neuromodulation performance metrics
- `performance.benchmarkSimulation()` - Benchmark simulation
- `performance.getResourceUsage()` - Get resource usage
- `performance.profileSimulation()` - Profile simulation

#### Debug
- `debug.enableDebugMode()` - Enable debug mode
- `debug.setLogLevel()` - Set log level
- `debug.inspectBrainState()` - Inspect brain state
- `debug.validateBrainState()` - Validate brain state
- `debug.logMessage()` - Log message
- `debug.handlePythonException()` - Handle Python exception

### Utility Functions

#### Core Utilities
- `nlm.formatSimulationStep()` - Format simulation step
- `nlm.getNeuronActivations()` - Get neuron activations
- `nlm.setDefaultConfigValues()` - Set default config values
- `nlm.createDefaultConfig()` - Create default config

#### Error Handling
- `RuntimeError` - Runtime error
- `InvalidArgumentError` - Invalid argument error
- `OutOfRangeError` - Out of range error

### Constants

- `DEFAULT_RANDOM_SEED` - Default random seed
- `DEFAULT_TIMESTEP` - Default timestep
- `DEFAULT_NEURON_COUNT` - Default neuron count
- `DEFAULT_REGION_COUNT` - Default region count
- `DEFAULT_CONNECTION_PROBABILITY` - Default connection probability

## Examples

### Example 1: Basic Neural Network

```python
import enhanced_pynlm as nlm

# Create and configure brain
config = nlm.createEnhancedConfig()
config.set("neuron_count", 500)
config.set("random_seed", 42)

brain = nlm.createEnhancedBrain(config)
brain.initialize()

# Run learning simulation
for step in range(1000):
    brain.step(step, step * 0.001)
    if step % 100 == 0:
        status = brain.getDetailedStatus()
        print(f"Step {step}: {status['totalSpikes']} spikes")

brain.reset()
```

### Example 2: Memory Research

```python
import enhanced_pynlm as nlm
import enhanced_pynlm.memory as memory
import enhanced_pynlm.experiment as experiment

# Create memory research configuration
exp_config = experiment.createPhase6Config()
experiment.configureForMemoryResearch(exp_config,
                                     enableAllMemory=True,
                                     workingMemoryCapacity=1000,
                                     episodicMemoryCapacity=1000)

# Create brain and run experiment
brain = nlm.createEnhancedBrain(exp_config)
brain.initialize()

# Run memory experiment
for step in range(2000):
    brain.step(step, step * 0.001)

# Get memory statistics
mem_stats = memory.getWorkingMemoryStats(brain)
print(f"Memory efficiency: {mem_stats['workingMemoryUtilization']}")

# Consolidate memory
brain.getEpisodicMemory().consolidate(0.5)
```

### Example 3: Neuromodulation Study

```python
import enhanced_pynlm as nlm
import enhanced_pynlm.neuromodulation as neuromodulation

# Create brain
brain = nlm.createEnhancedBrain(nlm.createEnhancedConfig())
brain.initialize()

# Run neuromodulation experiment
for step in range(500):
    # Apply neuromodulation
    neuromodulation.setDopamineLevel(brain, 0.5)
    neuromodulation.setCuriosityLevel(brain, 0.3)
    
    # Step brain
    brain.step(step, step * 0.001)
    
    # Record neuromodulation effects
    if step % 100 == 0:
        levels = brain.getAllNeuromodulationLevels()
        print(f"Step {step}: Dopamine={levels['dopamine']}, Curiosity={levels['curiosity']}")

brain.reset()
```

### Example 4: Performance Optimization

```python
import enhanced_pynlm as nlm
import enhanced_pynlm.performance as performance

# Create brain
brain = nlm.createEnhancedBrain(nlm.createEnhancedConfig())
brain.initialize()

# Profile performance
profile = performance.profileSimulation(brain, 1000, 0.001)
print(f"Performance: {profile['stepsPerSecond']} steps/sec")
print(f"Average step time: {profile['avgStepTimeMs']} ms")

# Get detailed metrics
metrics = performance.getBrainPerformanceMetrics(brain)
print(f"Neurons processed: {metrics['neuronsProcessed']}")
print(f"Memory usage: {metrics['memoryUsageMB']} MB")

brain.reset()
```

## Version Information

- **Enhanced Version**: 2.0.0
- **Basic Version**: 1.0.0
- **Author**: NLM Research Team
- **License**: MIT

## Dependencies

Required dependencies:
- Python >= 3.8
- C++17 compiler
- pybind11 >= 2.11.0
- scikit-build-core >= 0.5.0

Optional dependencies for development:
- pytest >= 7.0
- numpy >= 1.20
- matplotlib >= 3.0
- pandas >= 1.0
- scipy >= 1.0

## Changelog

### Version 2.0.0 (Enhanced)
- Added comprehensive simulation control methods
- Added advanced memory management
- Added neuromodulation utilities
- Added performance monitoring and profiling
- Added debug and development tools
- Added batch operations for better performance
- Added experiment configuration templates
- Enhanced error handling and validation
- Added educational examples and tutorials
- Improved API documentation

### Version 1.0.0 (Basic)
- Basic Python bindings for NLM
- Essential simulation functionality
- Basic memory systems
- Core neuromodulation support

## Contributing

To contribute to the enhanced Python bindings:

1. Fork the repository
2. Create a feature branch
3. Implement enhancements
4. Add tests and documentation
5. Submit a pull request

## Support

For support and issues, please:
- Report bugs through the issue tracker
- Ask questions on the NLM forum
- Check the documentation for examples
- Review the API reference

## Acknowledgments

This enhanced Python bindings project is part of the NLM (熙然) neural simulation framework, developed by the NLM Research Team.

We acknowledge the contributions of all developers and researchers who have worked on the NLM project.

## Copyright

Copyright (c) 2024 NLM Research Team

MIT License
