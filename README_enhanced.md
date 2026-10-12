# NLM — 熙然 Enhanced Python Bindings

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Enhanced Python API Overview

This enhanced version provides Python bindings with:
- **Custom Python exceptions** for better error handling
- **Context managers** for automatic resource management
- **Enhanced configuration** with builder pattern and validation
- **High-level simulation utilities** for common workflows
- **Comprehensive metrics collection**
- **Factory functions** for complex object creation
- **Advanced data analysis and export capabilities**
- **Full iterator support** for collection-like interfaces

## Quick Start Examples

### 1. Basic Enhanced Setup

```python
import pynlm

# Create enhanced simulation ecosystem with validation
ecosystem = pynlm.Factory.createSimulationEcosystem({
    'neuron_count': 500,
    'region_count': 2,
    'curiosity_enabled': True,
    'reward_modulation_enabled': True
})

brain = ecosystem['brain']
world = ecosystem['world']
agent = ecosystem['agent']

print("✓ Enhanced simulation ecosystem created")
print(f"  - Neurons: {brain.getTotalNeuronCount()}")
print(f"  - Regions: {brain.getRegionCount()}")
```

### 2. Advanced Configuration with Builder Pattern

```python
import pynlm

# Create brain with enhanced configuration
from pynlm import Config

config = Config()
brain = pynlm.Brain(config.withTimestep(0.001)
                          .withRandomSeed(42)
                          .withNeuronCount(1000)
                          .withRegionCount(3))

# Configuration validation
if config.validate():
    print("✓ Configuration is valid")
else:
    print("✗ Configuration validation failed")

# Convert to dictionary and back
config_dict = config.to_dict()
config2 = Config.from_dict(config_dict)
```

### 3. Complete Simulation with Metrics

```python
import pynlm

# Create simulation ecosystem
ecosystem = pynlm.Factory.createSimulationEcosystem({
    'neuron_count': 800,
    'region_count': 2
})

brain = ecosystem['brain']
world = ecosystem['world']
agent = ecosystem['agent']

# Initialize brain
brain.initialize()
print("✓ Brain initialized")

# Run simulation with metrics
results = pynlm.SimulationManager.runCompleteSimulation(
    world, brain, agent, steps=1000
)

print(f"✓ Simulation completed in {results['simulation_time_ms']:.2f} ms")
print(f"✓ Performance: {results['brain_metrics']['steps_per_second']:.0f} steps/sec")
print(f"✓ Spikes generated: {results['brain_metrics']['total_spikes']}")
print(f"✓ Agent curiosity: {results['agent_metrics']['curiosity']:.3f}")
```

### 4. Experiment Runner

```python
import pynlm

# Create experiment setup
experiment_setup = pynlm.Factory.createExperimentSetup("curiosity_driven")

# Create world and brain
world = pynlm.Factory.createWorldTemplate("resource_harvest")
config = experiment_setup["config"]
brain = pynlm.Brain(config)
agent = pynlm.AgentBrain.createCuriosityDrivenAgent(brain)

# Run experiment
experiment_results = pynlm.SimulationManager.runExperiment(
    world, brain, agent,
    trials=5,
    stepsPerTrial=2000,
    randomizeSeeds=True
)

# Generate analysis report
report = pynlm.SimulationManager.generateAnalysisReport(experiment_results)
print("\n" + report)
```

### 5. Advanced Features

```python
import pynlm

# Create ecosystem
ecosystem = pynlm.Factory.createSimulationEcosystem({
    'neuron_count': 1000,
    'region_count': 2,
    'auto_initialize': True  # Auto-initialize on creation
})

brain = ecosystem['brain']

# Context manager for automatic cleanup
with pynlm.BrainContext(brain) as ctx:
    print("✓ Brain context created")
    
    # Run episode
    brain.runEpisode(500)
    
    # Get comprehensive diagnostics
    diagnostics = brain.getDiagnostics()
    print("\n=== Brain Diagnostics ===")
    print(diagnostics)
    
    # Get connectivity analysis
    connectivity = brain.analyzeConnectivity()
    print(f"\nConnectivity: {connectivity}")

# Brain is automatically reset after context exit
print("✓ Brain context exited (automatically reset)")

# Advanced benchmark
benchmark_results = pynlm.SimulationManager.runBenchmark(
    brain,
    step_counts=[100, 500, 1000],
    warmup_steps=[10, 50, 100]
)
print(f"\n✓ Benchmark completed: {benchmark_results['total_benchmarks']} tests")
```

### 6. Data Export and Import

```python
import pynlm

# Create brain and run some simulation
brain = pynlm.Brain()
brain.initialize()
brain.runEpisode(500)

# Export brain state for analysis
analysis_data = brain.exportForAnalysis()
print("Brain state exported for analysis")

# Create new brain and import state
new_brain = pynlm.Brain()
new_brain.importFromAnalysis(analysis_data)
print("✓ Brain state imported successfully")

# Export world configuration
world = pynlm.SimpleWorld()
config = world.exportConfig()
print(f"World configuration exported: {config}")
```

## High-Level API Examples

### Agent Types

```python
import pynlm

brain = pynlm.Brain()

# Create different agent types
agents = {
    'curiosity_driven': pynlm.AgentBrain.createCuriosityDrivenAgent(
        brain, curiosityThreshold=0.6, enableStructuralPlasticity=True
    ),
    'reward_based': pynlm.AgentBrain.createRewardBasedAgent(
        brain, enableRewardModulation=True, enableDevelopment=True
    ),
    'exploration': pynlm.AgentBrain.createExplorationAgent(
        brain, explorationRate=0.4
    )
}

for name, agent in agents.items():
    print(f"✓ Created {name} agent")
```

### World Templates

```python
import pynlm

# Create different world scenarios
worlds = {
    'empty': pynlm.Factory.createWorldTemplate("empty"),
    'maze': pynlm.Factory.createWorldTemplate("maze"),
    'resource_harvest': pynlm.Factory.createWorldTemplate("resource_harvest")
}

for name, world in worlds.items():
    analysis = world.analyzeEnvironment()
    print(f"{name}: {analysis['width']}x{analysis['height']} "
          f"with {analysis['total_objects']} objects")
```

## Error Handling

The enhanced bindings provide custom Python exceptions for better error handling:

```python
import pynlm

brain = pynlm.Brain()

try:
    brain.initialize()
    # Run some operations
    config = brain.getConfig()
    config.validate()
    
except pynlm.NLMBrainError as e:
    print(f"✗ Brain error: {e}")
except pynlm.NLMInvalidParameter as e:
    print(f"✗ Invalid parameter: {e}")
except pynlm.NLMConfigurationError as e:
    print(f"✗ Configuration error: {e}")
except Exception as e:
    print(f"✗ Unexpected error: {e}")
```

## Performance Monitoring

```python
import pynlm

brain = pynlm.Brain()

# Run benchmark
benchmark_results = brain.benchmark(steps=2000, warmupSteps=200)

print(f"✓ Benchmark completed:")
print(f"  - Steps: {benchmark_results['steps']}")
print(f"  - Total time: {benchmark_results['total_time_ms']:.2f} ms")
print(f"  - Steps/sec: {benchmark_results['steps_per_second']:.0f}")
print(f"  - Spikes generated: {benchmark_results['spikes_generated']}")
print(f"  - Final firing rate: {benchmark_results['final_firing_rate']:.3f}")

# Get memory information
memory_info = brain.getMemoryInfo()
print(f"\nMemory usage: ~{memory_info['memory_usage_estimate_kb']:.0f} KB")
```

## Data Analysis and Export

```python
import pynlm

# Create a brain with some simulation
brain = pynlm.Brain()
brain.initialize()
brain.runEpisode(1000)

# Get comprehensive analysis
metrics = brain.collectMetrics()
connectivity = brain.analyzeConnectivity()

diagnostics = brain.getDiagnostics()

# Export for analysis
export_data = brain.exportForAnalysis()

print("=== Brain Analysis Results ===")
print(f"Total neurons: {metrics['total_neurons']}")
print(f"Total synapses: {metrics['total_synapses']}")
print(f"Active neurons: {metrics['active_neurons']}")
print(f"Firing rate: {metrics['average_firing_rate']:.3f}")
print(f"Total spikes: {metrics['total_spikes']}")
print(f"Strong connections: {connectivity.get('strong_ratio', 0):.3f}")
```

## Integration Examples

### Multi-World Training

```python
import pynlm

brain = pynlm.Brain()
agent = pynlm.AgentBrain.createCuriosityDrivenAgent(brain)

# Create multiple worlds for training
worlds = [
    pynlm.Factory.createWorldTemplate("empty"),
    pynlm.Factory.createWorldTemplate("maze"),
    pynlm.Factory.createWorldTemplate("resource_harvest")
]

# Run batch training
batch_results = agent.runBatchEpisodes(worlds, stepsPerWorld=500)

print(f"✓ Batch training completed across {len(worlds)} worlds")
print(f"  - Final reward: {batch_results['world_results'][0]['final_reward']:.3f}")
print(f"  - Average curiosity: {sum(r['curiosity'] for r in batch_results['world_results']) / len(batch_results['world_results']):.3f}")
```

### Research Experiment

```python
import pynlm

# Setup experiment
world = pynlm.Factory.createWorldTemplate("resource_harvest")
brain = pynlm.Brain()
agent = pynlm.AgentBrain.createCuriosityDrivenAgent(brain)

# Run controlled experiment
experiment = pynlm.SimulationManager.runExperiment(
    world, brain, agent,
    trials=10,
    stepsPerTrial=3000,
    randomizeSeeds=True
)

# Generate research report
research_report = pynlm.SimulationManager.generateAnalysisReport(experiment)
print("=== Research Experiment Report ===")
print(research_report)
```

## API Reference

### Core Classes

- **Config**: Enhanced configuration with builder pattern and validation
- **Brain**: Central neural simulation brain with advanced features
- **AgentBrain**: Enhanced agent brain interface with Pythonic APIs
- **SimpleWorld**: Enhanced world simulation with analysis tools
- **SimulationManager**: High-level simulation utilities
- **Factory**: Factory module for creating complex objects

### Key Methods

#### Config Methods:
- `withTimestep(timestep)`: Set simulation timestep
- `withRandomSeed(seed)`: Set random seed
- `withNeuronCount(count)`: Set neuron count
- `withRegionCount(count)`: Set region count
- `validate()`: Validate configuration
- `to_dict()`: Export configuration as dictionary
- `from_dict(dict)`: Create configuration from dictionary

#### Brain Methods:
- `runEpisode(steps, world)`: Run simulation episode
- `collectMetrics(samples)`: Collect comprehensive metrics
- `analyzeConnectivity()`: Analyze network connectivity
- `benchmark(steps, warmup)`: Run performance benchmark
- `getMemoryInfo()`: Get memory usage information
- `findNeurons(type)`: Find neurons by type
- `exportForAnalysis()`: Export brain state for analysis
- `importFromAnalysis(data)`: Import brain state from analysis data
- `validateBrain()`: Validate brain state
- `getDiagnostics()`: Get comprehensive diagnostics

#### AgentBrain Methods:
- `createCuriosityDrivenAgent(brain, ...)`: Create curiosity-driven agent
- `createRewardBasedAgent(brain, ...)`: Create reward-based agent
- `createExplorationAgent(brain, ...)`: Create exploration agent
- `runEpisodeWithMetrics(world, steps, profiler)`: Run episode with metrics
- `runBatchEpisodes(worlds, steps)`: Run batch of episodes
- `analyzeSensoryInput(percept)`: Analyze sensory input

#### SimpleWorld Methods:
- `create(width, height, ...)`: Create world with parameters
- `generateFromTemplate(name)`: Generate world from template
- `analyzeEnvironment()`: Analyze world environment
- `exportWorldState()`: Export world state
- `importWorldState(state)`: Import world state
- `validateWorld()`: Validate world state

### Exceptions

- **NLMException**: Base exception for all NLM errors
- **NLMBrainError**: Brain-specific errors
- **NLMInvalidParameter**: Invalid parameter errors
- **NLMFileNotFound**: File not found errors
- **NLMConfigurationError**: Configuration errors

### Modules

- **pynlm.Factory**: Factory functions for object creation
- **pynlm.SimulationManager**: High-level simulation utilities
- **pynlm.Config**: Configuration class with builder pattern

## Installation

```bash
# Install the enhanced bindings
pip install pynlm

# Or build from source
python -m pip install scikit-build-core pybind11
mkdir build && cd build
cmake ..
make -j4
pip install .
```

## Requirements

- Python >= 3.8
- C++20 compiler
- pybind11 >= 2.11.0

## Testing

```bash
# Run Python tests
pytest

# Or run specific tests
test_path = "tests/test_brain.py"
pytest test_path -v
```

## Version

Enhanced NLM Python Bindings v2.0.0

## License

MIT

## Authors

Research project — See docs for scientific background.