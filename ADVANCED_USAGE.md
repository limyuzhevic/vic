# Advanced NLM - Advanced Usage Guide for Expert Users

## Introduction

This guide covers advanced features and usage patterns for the NLM (Neural Learning Machine) Python API. It assumes familiarity with the basic NLM concepts and API documented in `easy_usage.md` and `HOW_TO_USE.md`.

## Advanced API Features

### 1. Enhanced Core Classes

The enhanced Python API now exposes low-level neural components for advanced users:

```python
import pynlm

# Create a brain with enhanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
brain = pynlm.createBrain(config)
brain.initialize()

# Access and manipulate individual neurons
neurons = []
for region in brain.getRegions():
    for population in region.getAllPopulations():
        neurons.extend(population.getAllNeurons())

# Modify neuron properties
for i, neuron in enumerate(neurons[:10]):
    neuron.setThreshold(-45.0 - i * 2.0)  # Adjust thresholds
    neuron.setType(pynlm.NeuronType.Modulatory)  # Change type

# Create custom synapses
synapse = pynlm.createSynapse(
    pynlm.SynapseId(0),
    neurons[0].getId(),
    neurons[1].getId()
)
synapse.setWeight(1.5)
synapse.setDelay(5)
```

### 2. Memory Systems

The enhanced API provides full access to memory systems:

```python
import pynlm

# Create advanced memory systems
working_memory = pynlm.createWorkingMemory()
working_memory.setCapacity(100)  # Increase capacity

episodic_memory = pynlm.createEpisodicMemory()

# Store and retrieve experiences
episode_id = 12345
episodic_memory.storeExperience(
    content=[1.0, 0.5, 0.8, 0.3, 0.9],
    importance=0.8,
    timestamp=time.time()
)

# Replay specific episodes
episodic_memory.replayEpisode(episode_id)

# Analyze memory effectiveness
memory_stats = {
    'episode_count': episodic_memory.getEpisodeCount(),
    'total_importance': episodic_memory.getTotalImportance(),
    'average_importance': episodic_memory.getAverageImportance(),
    'memory_load': episodic_memory.getMemoryLoad()
}
```

### 3. Neuromodulation Systems

Access and control advanced neuromodulation:

```python
import pynlm
import time

# Create and configure neuromodulators
reward = pynlm.createReward()
reward.setLearningRate(0.01)

curiosity = pynlm.createCuriosity()
curiosity.setLevel(0.7)

novelty = pynlm.createNovelty()
novelty.setSensitivity(0.8)

prediction_error = pynlm.createPredictionError()
prediction_error.setLearningRate(0.05)

# Apply neuromodulation to brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Advanced reward processing with prediction error
reward_value = 1.0
predicted_reward = 0.8
reward.setPredictionError(predicted_reward - reward_value)
reward.applyToBrain(brain)

# Trigger curiosity-driven exploration
curiosity.triggerExploration()
```

### 4. Plasticity Systems

Configure and monitor advanced plasticity rules:

```python
import pynlm

# Create and configure plasticity rules
stdp = pynlm.createSTDP()
stdp.setLearningRate(0.001)

hebbian = pynlm.createHebbian()
hebbian.setLearningRate(0.005)

structural = pynlm.createStructuralPlasticity()
structural.setSynaptogenesisRate(0.0002)
structural.setPruningRate(0.00001)

# Enable plasticity in neurons
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable different plasticity mechanisms
brain.enableHebbianPlasticity(True)
brain.enableSTDPPlasticity(True)
brain.enableStructuralPlasticity(True)
brain.enableRewardModulatedPlasticity(True)

# Monitor plasticity effects
stats = brain.getPlasticityStatistics()
print(f"Total weight changes: {stats['total_weight_change']}")
print(f"STDP LTP events: {stats['stdp_ltp_events']}")
print(f"STDP LTD events: {stats['stdp_ltd_events']}")
```

### 5. Developmental System

Control advanced development and maturation:

```python
import pynlm

# Create development system
development = pynlm.createDevelopmentSystem()
development.setAge(10.0)  # Set developmental age

# Control developmental stage
development.setStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Monitor development progress
print(f"Current stage: {development.getCurrentStage()}")
print(f"Plasticity rate: {development.getPlasticityRate()}")
print(f"Maturation progress: {development.getMaturationProgress()}")
print(f"Critical period active: {development.getCriticalPeriodActive()}")

# Advance maturation
if not development.isMature():
    development.advanceMaturation(1.0)

# Configure age-dependent parameters
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.setDevelopmentSystem(development)
```

### 6. Advanced Cognition Systems

Implement advanced cognitive capabilities:

```python
import pynlm

# Create cognition systems
planner = pynlm.createNeuralPlanner()
planner.setPlanningHorizon(100)
planner.setExplorationExploitationRatio(0.3)

concept_formation = pynlm.createConceptFormation()
concept_formation.setLearningRate(0.02)

# Use cognition in agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.setCognitiveSystem(planner)
agent.setConceptFormation(concept_formation)

# Process sensory input and form concepts
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Form concepts from patterns
concept_id = concept_formation.processPattern(percept.getVision())
```

### 7. Dynamic Command System

The enhanced API includes a dynamic command system for runtime feature addition:

```python
import pynlm

# Create command registry for runtime commands
command_registry = pynlm.createCommandRegistry()

# Define custom commands
def custom_benchmark(command_args):
    brain_name = command_args[0] if len(command_args) > 0 else "default"
    neurons = int(command_args[1]) if len(command_args) > 1 else 1000
    steps = int(command_args[2]) if len(command_args) > 2 else 100
    
    # Run benchmark
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neurons)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    for step in range(steps):
        brain.step(step)
    
    duration = time.time() - start_time
    print(f"Benchmark for {brain_name}: {steps} steps in {duration:.2f}s")
    return True

# Register custom command
command_registry.registerCommand(
    name="custom_benchmark",
    description="Run custom benchmark with specified parameters",
    handler=custom_benchmark,
    parameters=["brain_name", "neuron_count", "step_count"]
)

# Execute command
command_registry.executeCommand("custom_benchmark", ["test_brain", "2000", "500"])
```

### 8. Performance Monitoring

Monitor and optimize NLM performance:

```python
import pynlm
import time
import threading

# Create performance monitor
performance_monitor = pynlm.createPerformanceMonitor()

# Start monitoring
subsystems = ["brain", "memory", "plasticity", "neuromodulation"]
performance_monitor.startMonitoring(subsystems, interval=1.0)

# Run simulation with monitoring
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Collect performance metrics during simulation
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        metrics = performance_monitor.getCurrentMetrics()
        print(f"Step {step}: Firing rate {brain.getFiringNeuronCount()}, "
              f"Spike rate {brain.getAverageFiringRate():.2f} Hz")

# Get performance statistics
stats = performance_monitor.getPerformanceStatistics()
print(f"Average CPU usage: {stats['cpu_usage']:.1f}%")
print(f"Memory efficiency: {stats['memory_efficiency']:.2f}")
print(f"Simulation speed: {stats['simulation_speed']} steps/sec")

# Stop monitoring
performance_monitor.stopMonitoring()
```

### 9. Advanced Experimentation

Run sophisticated experiments:

```python
import pynlm

# Create advanced experiment runner
experiment_runner = pynlm.createExperimentRunner()

# Create phase 6 integration experiment
phase6_experiment = experiment_runner.createExperiment("phase6_integration")

# Configure experiment
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("simulation_timestep", 0.001)

phase6_config = pynlm.createPhase6Config()
phase6_config.maxSteps = 5000
phase6_config.neuronCount = 1000
phase6_config.enableDevelopment = True
phase6_config.enableCheckpointing = True
phase6_config.enableReplay = True

# Run experiment
brain = pynlm.createBrain(config)
brain.initialize()

result = phase6_experiment.run(phase6_config)

# Analyze results
print(f"Integration successful: {result.memoryWorkingMemoryIntegrated and result.neuromodulationIntegrated}")
print(f"Total reward: {result.totalReward}")
print(f"Average firing rate: {result.avgFiringRate}")
print(f"Memory episodes stored: {result.memoryEpisodesStored}")
```

### 10. Data Management and Persistence

Advanced data handling and persistence:

```python
import pynlm
import json
import numpy as np

# Save brain state with metadata
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Configure checkpoint system
checkpoint_config = pynlm.createCheckpointConfig()
checkpoint_config.autoSave = True
checkpoint_config.saveInterval = 100
checkpoint_config.compress = True

# Save brain state
brain.save("advanced_checkpoint.bin", checkpoint_config)

# Export data in various formats
export_config = pynlm.createExportConfig()
export_config.format = "csv"
export_config.includeNeurons = True
export_config.includeSynapses = True
export_config.includeMemory = True

brain.export("brain_data.csv", export_config)

# Import data from different sources
import_config = pynlm.createImportConfig()
import_config.format = "json"
import_config.dataSource = "experiment_data.json"

brain.importFrom("imported_brain.bin", import_config)

# Advanced data analysis
analysis_config = pynlm.createAnalysisConfig()
analysis_config.type = "pattern_recognition"
analysis_config.threshold = 0.7
analysis_config.minSupport = 0.5

results = brain.analyze(analysis_config)
print(f"Pattern clusters found: {results['cluster_count']}")
print(f"Dominant patterns: {results['dominant_patterns']}")
```

## Performance Optimization

### 1. Memory Optimization

```python
import pynlm

# Configure memory pools for large networks
memory_config = pynlm.createMemoryPoolConfig()
memory_config.blockSize = 4096
memory_config.numBlocks = 256
memory_config.enablePreallocation = True

# Set up memory management
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize(memory_config)

# Monitor memory usage
memory_stats = brain.getMemoryStatistics()
print(f"Peak memory usage: {memory_stats['peak_memory']} MB")
print(f"Memory efficiency: {memory_stats['efficiency']:.2%}")
```

### 2. Parallel Processing

```python
import pynlm
import threading

# Configure parallel processing
events = pynlm.createEventQueue()
events.setMaxWorkers(4)
events.setMaxQueueSize(1000)

# Process events in parallel
for i in range(100):
    events.addEvent(lambda step=i: processStep(brain, step))

events.processAll()

# Configure SIMD vectorization
simd_config = pynlm.createSIMDConfig()
simd_config.enableVectorization = True
simd_config.vectorWidth = 512

brain.setSIMDConfiguration(simd_config)
```

## Debugging and Monitoring

### 1. Advanced Logging

```python
import pynlm

# Create advanced logging system
logger = pynlm.createAdvancedLogger()
logger.setLogLevel(pynlm.LogLevel.Debug)
logger.setFileOutput("advanced_simulation.log")
logger.setConsoleOutput(True)
logger.setLogFormat("%(timestamp)s [%(level)s] %(message)s")

# Enable detailed logging for specific components
logger.enableComponentLogging("brain", True)
logger.enableComponentLogging("memory", True)
logger.enableComponentLogging("plasticity", True)

# Log simulation progress
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        logger.logInfo(f"Simulation step {step} completed")

# Generate performance report
report = logger.generatePerformanceReport()
print(f"Simulation performance: {report['performance_score']}/100")
```

### 2. State Validation

```python
import pynlm

# Create validation system
validator = pynlm.createStateValidator()
validator.setValidationLevel(pynlm.ValidationLevel.Full)

# Validate brain state
validation_result = validator.validate(brain)
if validation_result.isValid:
    print("Brain state is valid")
else:
    print(f"Validation errors: {validation_result.errors}")

# Validate consistency
consistency_result = validator.validateConsistency(brain)
if consistency_result.isConsistent:
    print("Brain state is consistent")
else:
    print(f"Consistency issues: {consistency_result.issues}")
```

## Migration Guide

### From Basic to Advanced API

1. **Add dependencies**: Update `pyproject.toml` to include optional dependencies
2. **Update imports**: Import additional classes and functions
3. **Configure components**: Set up enhanced subsystems with custom parameters
4. **Use advanced features**: Leverage dynamic commands and monitoring
5. **Optimize performance**: Configure memory pools and parallel processing

```python
# Basic API usage
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)

# Advanced API usage
import pynlm
from pynlm import createAdvancedBrain, createWorkingMemory, createNeuromodulator

# Create enhanced components
config = pynlm.createDefaultConfig()
brain = createAdvancedBrain(config)

# Configure memory
working_memory = createWorkingMemory()
working_memory.setCapacity(1000)

# Configure neuromodulation
reward_system = createNeuromodulator()
reward_system.setLearningRate(0.01)

# Enable advanced features
brain.enableAdvancedPlasticity(True)
brain.enableWorkingMemory(True)
brain.enableNeuromodulation(True)
```

## Troubleshooting

### Common Issues and Solutions

1. **Memory Issues with Large Networks**
   - Solution: Configure memory pools and enable preallocation

2. **Performance Degradation**
   - Solution: Use parallel processing and SIMD vectorization

3. **Simulation Instability**
   - Solution: Adjust neuron parameters and enable stability checks

4. **Memory Leaks**
   - Solution: Use proper cleanup and enable automatic garbage collection

5. **Command Execution Errors**
   - Solution: Register commands properly and validate parameters

### Debugging Techniques

```python
import pynlm

# Enable debug mode
debugger = pynlm.createDebugger()
debugger.enableBreakOnError(True)
debugger.enablePerformanceProfiling(True)

# Run simulation with debugging
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Set up breakpoints and monitoring
debugger.addBreakpoint("plasticity_change", threshold=0.1)
debugger.addMonitor("neuron_firing", callback=logFiringRate)

# Run simulation
for step in range(1000):
    try:
        brain.step(step)
    except Exception as e:
        debugger.handleError(e, step)

# Analyze debug output
debug_report = debugger.generateReport()
print(f"Errors found: {debug_report['error_count']}")
print(f"Performance issues: {debug_report['performance_issues']}")
```

## Future Enhancement Roadmap

The advanced API is designed to support future enhancements:

1. **Machine Learning Integration**: Connect NLM with traditional ML algorithms
2. **Multi-Agent Systems**: Enable communication between multiple NLM instances
3. **Hardware Acceleration**: Support GPU and specialized hardware
4. **Real-time Interfaces**: Enable live interaction and control
5. **Cloud Deployment**: Support for distributed computing and cloud deployment

## Conclusion

The enhanced NLM Python API provides advanced users with comprehensive control over neural simulation components, memory systems, neuromodulation, plasticity, and cognition. The dynamic command system, performance monitoring, and advanced debugging tools make it suitable for research, development, and production deployment.

For the most up-to-date features and documentation, refer to the main `README.md` and `docs/` directory.
