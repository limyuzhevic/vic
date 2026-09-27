# Enhanced NLM Python Bindings

## Overview

This document describes the enhanced Python bindings for the NLM (Neural Learning Machine) framework. The enhanced API provides significant improvements over the original bindings, including:

- **Enhanced Configuration API** with YAML support, validation, and environment variables
- **Advanced Brain Interface** with visualization, diagnostics, and learning tracking
- **Enhanced Agent Interface** with advanced sensory processing and reward shaping
- **World and Environment Tools** with enhanced configuration and management
- **Memory and Learning Tools** with advanced visualization and analysis
- **Performance and Monitoring Tools** with real-time metrics and profiling
- **Educational and Debugging Tools** with step-by-step execution and visualization
- **Advanced Scripting and Automation** with batch operations and experiment scripting

## Installation

To use the enhanced Python bindings, you need to:

1. **Compile the enhanced bindings**
   ```bash
   cd /path/to/nlm/source
   g++ -std=c++17 -I/usr/include/python3.10 -I./include -I./external/pybind11 -DNLM_PERFORMANCE_ENABLED -DNLM_USE_MEMORY_POOL -DNLM_USE_SOA -o python_bindings.so python/bindings.cpp -lpython3.10
   ```

2. **Install the module**
   ```bash
   pip install pynlm
   ```

## Key Features

### 1. Enhanced Configuration API

#### Configuration Class
```python
import pynlm

# Create configuration from environment variables
config = pynlm.createConfigFromEnvironment()

# Create default configuration template
default_config = pynlm.createDefaultConfig()

# Load from YAML file (new feature)
config.loadFromFile("config.yaml")

# Validate configuration
validation_result = config.validate()
if validation_result.valid:
    print("Configuration is valid")
    if validation_result.warnings:
        for warning in validation_result.warnings:
            print(f"WARNING: {warning}")
else:
    print(f"ERROR: {validation_result.errorMessage}")

# Set values with validation
config.set("brain.neuronCount", 2000)

# Get values with defaults
neuron_count = config.getOr("brain.neuronCount", 1000)

# Export/import configuration
config_data = config.exportConfig()
config2 = pynlm.EnhancedConfig()
config2.importConfig(config_data)
```

#### Environment Variable Support
```python
# Set environment variables
export NLM_BRAIN_NEURON_COUNT=2000
export NLM_WORLD_WIDTH=150.0
export NLM_WORLD_HEIGHT=120.0

# Create config from environment
config = pynlm.createConfigFromEnvironment()
```

#### Configuration Templates
```python
# Create specialized configuration templates
# For reinforcement learning
rl_config = pynlm.createDefaultConfig()
rl_config.set("brain.rewardModulation.enable", True)
rl_config.set("brain.curiosity.enable", True)
rl_config.set("brain.novelty.enable", True)

# For developmental learning
dev_config = pynlm.createDefaultConfig()
dev_config.set("brain.development.enable", True)
dev_config.set("brain.synaptogenesis.enable", True)
dev_config.set("brain.pruning.enable", True)
```

### 2. Advanced Brain Interface

#### Brain Class
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create enhanced brain
brain = pynlm.createEnhancedBrain(config)

# Initialize with diagnostics
if brain.initializeWithDiagnostics():
    print("Brain initialized successfully")
else:
    print("Brain initialization failed")

# Enhanced simulation step
brain.stepAdvanced(timestep=0.01, collectStats=True)

# Get debug information
debug_info = brain.getDebugInfo()
print(f"Active neurons: {debug_info.activeNeurons}")
print(f"Firing neurons: {debug_info.firingNeurons}")
print(f"Average firing rate: {debug_info.averageFiringRate:.2f} Hz")

# Enable visualization
brain.enableVisualization(True)

# Update visualization
brain.updateVisualization()

# Get learning progress
learning_progress = brain.getLearningProgress()
print(f"Learning score: {learning_progress.convergenceScore:.3f}")

# Advanced neuron manipulation
from pynlm import NeuronId
neurons = [NeuronId(1), NeuronId(2), NeuronId(3)]
brain.injectCurrentToPattern(neurons, 0.5)

# Long-range connections
from pynlm import RegionId
brain.createLongRangeConnection(RegionId(100), RegionId(200), weight=0.8)

# Access to internal systems
stdp = brain.getSTDP()
hebbian = brain.getHebbian()
curiosity = brain.getCuriosity()
```

### 3. Enhanced Agent Interface

#### AgentBrain Class
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain and agent
brain = pynlm.createEnhancedBrain(config)
agent = pynlm.createEnhancedAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=200.0, height=150.0, visionWidth=20, visionHeight=20)

# Initialize agent with enhanced features
agent.initializeWithWorld(world, enableLearning=True)

# Process sensory input with advanced features
from pynlm import Vision, SensoryPercept
vision = Vision(width=20, height=20, channels=3)
percept = SensoryPercept()
percept.setVision(vision)

agent.processSensoryInputEnhanced(
    percept,
    applySmoothing=True,
    detectNovelty=True,
    updateWorkingMemory=True
)

# Analyze sensory input
analysis = agent.analyzeSensoryInput(percept)
print(f"Novelty score: {analysis.noveltyScore:.3f}")
print(f"Complexity: {analysis.complexity:.3f}")

# Enhanced action selection
motor_cmd = agent.decodeMotorCommandEnhanced(
    applyRewardShaping=True,
    considerExploration=True,
    temperature=1.0,
    useActionValueEstimation=True
)

# Enhanced reward modulation
agent.applyRewardModulationEnhanced(
    reward=1.0,
    predictedReward=0.8,
    useTDLearning=True,
    usePolicyGradient=False,
    useActorCritic=True
)

# Enhanced development
agent.updateDevelopmentEnhanced(
    timestep=0.01,
    enableStructuralPlasticity=True,
    enableSynaptogenesis=True,
    enablePruning=True
)

# Get learning statistics
stats = agent.getLearningStatistics()
print(f"Average reward: {stats.averageReward:.3f}")
print(f"Convergence rate: {stats.convergenceRate:.3f}")
```

### 4. World and Environment Tools

#### World Class
```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=200.0, height=150.0, visionWidth=20, visionHeight=20)

# Set agent starting position
world.setAgentStart(50.0, 50.0)

# Add objects to world
from pynlm import WorldObject, WorldObjectType

# Add resource
resource = WorldObject(x=100.0, y=100.0, type=WorldObjectType.Resource, value=10.0)
world.addObject(resource)

# Add hazard
hazard = WorldObject(x=150.0, y=80.0, type=WorldObjectType.Hazard, value=-5.0)
world.addObject(hazard)

# Add wall
wall = WorldObject(x=180.0, y=50.0, type=WorldObjectType.Wall, radius=2.0)
world.addObject(wall)

# Simulate world update
for step in range(10):
    world.update(timestep=0.1)
    percept = world.getSensoryPercept()
    
    # Check for objects nearby
    body = world.getAgentBody()
    if world.isValidPosition(body.x, body.y):
        obj = world.getObjectAt(body.x, body.y)
        if obj:
            print(f"Found object: type={obj.type}, value={obj.value}")

print(f"Simulation time: {world.getSimulationTime():.2f}")
```

### 5. Memory and Learning Tools

#### Memory Systems
```python
import pynlm

# Create memory systems
working_memory = pynlm.createWorkingMemory()
episodic_memory = pynlm.createEpisodicMemory()
semantic_memory = pynlm.createSemanticMemory()
procedural_memory = pynlm.createProceduralMemory()
associative_memory = pynlm.createAssociativeMemory()

# Working memory
from pynlm import NeuronId
neuron_id = NeuronId(123)
working_memory.store(neuron_id, 0.85f)
retrieved = working_memory.retrieve(neuron_id)
print(f"Working memory: {retrieved}")

# Semantic memory
semantic_memory.storeFact("temperature", "warm")
fact = semantic_memory.retrieveFact("temperature")
print(f"Semantic memory: {fact}")

# Procedural memory
pattern = [NeuronId(1), NeuronId(2), NeuronId(3)]
procedural_memory.learnSkill("walk_forward", pattern)
skill = procedural_memory.getSkill("walk_forward")
print(f"Procedural memory: learned skill '{skill.name}'")

# Associative memory
associative_memory.associate(NeuronId(100), NeuronId(200), 0.9f)
associations = associative_memory.getAssociations(NeuronId(100))
print(f"Associative memory: {len(associations)} associations")

# Visualization interface
viz = pynlm.createVisualizationInterface()
if viz.initialize():
    print("Visualization interface initialized")
    viz.close()
```

### 6. Performance and Monitoring Tools

#### Performance Monitoring
```python
import pynlm

# Create configuration with performance settings
config = pynlm.createDefaultConfig()
config.set("brain.performance.enabled", True)

# Create brain with performance monitoring
brain = pynlm.createEnhancedBrain(config)

# Monitor performance
for i in range(5):
    brain.stepAdvanced(timestep=0.1, collectStats=True)
    
    perf_metrics = brain.getPerformanceMetrics()
    print(f"Step {i+1}:")
    print(f"  CPU usage: {perf_metrics.cpuUsage:.1f}%")
    print(f"  Memory usage: {perf_metrics.memoryUsage:.1f}%")
    print(f"  Simulation speed: {perf_metrics.simulationSpeed:.1f}x")
    print(f"  Network efficiency: {perf_metrics.networkEfficiency:.3f}")
```

### 7. Educational and Debugging Tools

#### Educational Tools
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createEnhancedBrain(config)

# Step-by-step execution
print("Step-by-step execution:")
for step in range(3):
    print(f"Step {step + 1}:")
    brain.stepAdvanced(timestep=0.01, collectStats=True)
    
    debug_info = brain.getDebugInfo()
    print(f"  Neurons: {debug_info.activeNeurons} active, {debug_info.firingNeurons} firing")
    print(f"  Firing rate: {debug_info.averageFiringRate:.2f} Hz")

# Learning progress
learning_progress = brain.getLearningProgress()
print(f"Learning score: {learning_progress.convergenceScore:.3f}")
print(f"Total spikes: {learning_progress.totalSpikes}")
```

### 8. Advanced Scripting and Automation

#### Batch Operations
```python
import pynlm

# Create multiple configurations
configs = []
for i in range(3):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuronCount", 1000 + i * 500)
    config.set("world.width", 100.0 + i * 20)
    configs.append(config)

# Create multiple brains
brains = []
for config in configs:
    brain = pynlm.createEnhancedBrain(config)
    brains.append(brain)
```

#### Experiment Scripting
```python
import pynlm

def run_experiment(config, experiment_name):
    """Run a single experiment"""
    brain = pynlm.createEnhancedBrain(config)
    
    if not brain.initializeWithDiagnostics():
        return {'success': False, 'error': 'Initialization failed'}
    
    # Run simulation
    for step in range(100):
        brain.stepAdvanced(timestep=0.1, collectStats=True)
    
    # Collect results
    debug_info = brain.getDebugInfo()
    learning_progress = brain.getLearningProgress()
    
    return {
        'success': True,
        'final_neurons': debug_info.activeNeurons,
        'final_firing': debug_info.firingNeurons,
        'learning_score': learning_progress.convergenceScore
    }

# Run multiple experiments
experiments = []
for i in range(3):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuronCount", 1000 + i * 1000)
    experiment = run_experiment(config, f"exp_{i}")
    experiments.append(experiment)
```

## Backward Compatibility

The enhanced bindings maintain full backward compatibility with the original API:

```python
import pynlm

# Original classes still work
config = pynlm.Config()
brain = pynlm.Brain(config)
world = pynlm.SimpleWorld()
agent = pynlm.AgentBrain(brain)

# Original methods still work
brain.initialize()
brain.step(0)
brain.step(0, 0)
agent.initialize(world)
agent.processSensoryInput(percept)
```

## Additional Utilities

### Debugging Structures
```python
# Create debugging structures
debug_info = pynlm.createDebugInfo()
learning_progress = pynlm.createLearningProgress()
sensory_analysis = pynlm.createSensoryAnalysis()
performance_metrics = pynlm.createPerformanceMetrics()
```

### Configuration Validation
```python
# Validate configuration
validation_result = pynlm.validateConfig(config)
if validation_result.valid:
    print("Configuration is valid")
else:
    print(f"Validation failed: {validation_result.errorMessage}")
```

### Configuration Utilities
```python
# Format configuration
config_str = pynlm.formatConfig(config)

# Export/import configuration
config_data = pynlm.exportConfig(config)
pynlm.importConfig(config, config_data)
```

## Best Practices

1. **Configuration Management**: Use `createFromEnvironment()` for production deployments
2. **Error Handling**: Always check validation results before using configuration
3. **Performance Monitoring**: Enable performance monitoring for large-scale simulations
4. **Visualization**: Use `enableVisualization(True)` for debugging and research
5. **Memory Management**: Be mindful of memory usage in long-running simulations
6. **Batch Operations**: Use batch configuration loading for multiple experiments

## Examples Directory

For complete, runnable examples, see the `examples/` directory:

- `enhanced_bindings_example.py`: Complete usage examples for all features
- `experimental_examples.py`: Examples of advanced experimental setups
- `production_examples.py`: Examples for production deployment

## Further Reading

For more information about NLM, visit the official documentation at:
https://nlm.readthedocs.io

For C++ API documentation, see the source headers in `src/` directory.

## License

This enhanced API is distributed under the same license as the original NLM framework.
