# NLM — 熙然: Enhanced Python Usage Guide

**Phase 6: Final Integration - Enhanced Python Bindings**

This guide documents the enhanced Python bindings for the NLM neural simulation framework, providing:
- Installation and setup instructions
- Comprehensive API documentation
- Code examples for different use cases
- Best practices and troubleshooting

## Table of Contents

1. [Quick Start](#1-quick-start)
2. [Installation](#2-installation)
3. [Configuration](#3-configuration)
4. [Basic Usage](#4-basic-usage)
5. [Advanced Features](#5-advanced-features)
6. [Error Handling](#6-error-handling)
7. [Performance Optimization](#7-performance-optimization)
8. [Data Analysis and Export](#8-data-analysis-and-export)
9. [Best Practices](#9-best-practices)
10. [Troubleshooting](#10-troubleshooting)
11. [API Reference](#11-api-reference)

## 1. Quick Start

### Prerequisites

- Python 3.8 or higher
- C++20 compatible compiler
- pybind11 >= 2.11.0 (will be installed automatically)

### Installation

```bash
# Install via pip (recommended)
pip install pynlm

# Or build from source
python -m pip install scikit-build-core pybind11
mkdir build && cd build
cmake ..
make -j4
pip install .
```

### First Program

```python
import pynlm

# Create enhanced simulation ecosystem
ecosystem = pynlm.Factory.createSimulationEcosystem({
    'neuron_count': 100,
    'region_count': 1,
    'curiosity_enabled': True
})

brain = ecosystem['brain']
world = ecosystem['world']
agent = ecosystem['agent']

print("Hello NLM!")
print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"World is {world.getWidth()}x{world.getHeight()} pixels")
```

## 2. Installation

### System Dependencies

```bash
# Ubuntu/Debian
apt-get update
apt-get install -y g++ cmake python3-dev python3-pip

# macOS
brew install cmake python3

# Windows
# Download and install from:
# - https://cmake.org/download/
# - https://www.python.org/downloads/
```

### Python Package Installation

#### Method 1: Using pip (Recommended)

```bash
# Install the package
pip install pynlm

# Verify installation
python -c "import pynlm; print(pynlm.get_version())"
```

#### Method 2: Building from Source

```bash
# Clone the repository
cd ~/projects
git clone https://github.com/nlm-project/nlm.git

cd nlm

# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build (this may take several minutes)
make -j4

# Install Python bindings
pip install .
```

### Verification

```python
import pynlm

# Check version
print(f"NLM Python Bindings: {pynlm.get_version()}")

# Test basic functionality
ecosystem = pynlm.Factory.createSimulationEcosystem()
print("✓ Enhanced simulation ecosystem created successfully")

# Test configuration
config = pynlm.Config()
config.withTimestep(0.001)
config.withRandomSeed(42)
print("✓ Configuration builder pattern works")

# Test brain creation
brain = pynlm.Brain(config)
print("✓ Brain creation successful")
```

## 3. Configuration

### Configuration Classes

#### Basic Configuration

```python
import pynlm

# Create basic configuration
config = pynlm.Config()
print(config.getSummary())
```

#### Enhanced Configuration Builder

```python
import pynlm

# Configuration builder pattern (fluent interface)
config = (pynlm.Config()
         .withTimestep(0.001)
         .withRandomSeed(42)
         .withNeuronCount(1000)
         .withRegionCount(2))

# Configuration validation
if config.validate():
    print("✓ Configuration is valid")
else:
    print("✗ Configuration validation failed")
```

#### Configuration from Dictionary

```python
import pynlm

# Create configuration from dictionary
config_dict = {
    'simulation_timestep': 0.001,
    'random_seed': 42,
    'neuron_count': 500,
    'region_count': 1,
    'connection_probability': 0.15
}

config = pynlm.Config.from_dict(config_dict)
brain = pynlm.Brain(config)
```

#### Configuration Export/Import

```python
import pynlm

# Export configuration to dictionary
config = pynlm.Config().withNeuronCount(800)
config_dict = config.to_dict()

# Save configuration to file
config.saveWithBackup("my_config.cfg")

# Load configuration from file
config2 = pynlm.Config()
config2.loadFromFile("my_config.cfg")
```

## 4. Basic Usage

### Brain Simulation

```python
import pynlm

# Create brain with configuration
config = pynlm.Config().withNeuronCount(200)
brain = pynlm.Brain(config)

# Initialize brain
brain.initialize()
print("✓ Brain initialized")

# Run simulation steps
for step in range(10):
    brain.step(step, step * 0.001)

# Get brain status
brain.logStatus()
```

### World Simulation

```python
import pynlm

# Create world
world = pynlm.SimpleWorld.create(320, 240, 16, 16)

# Configure world
world.configure(400, 300, 20, 20)
print(f"World: {world.getWidth()}x{world.getHeight()} pixels")
print(f"Vision: {world.getVisionWidth()}x{world.getVisionHeight()} pixels")

# Add objects to world
agent_body = world.getAgentBody()
world.addObjectWithValidation(50, 50, pynlm.WorldObjectType.Resource, 1.0)
world.addObjectWithValidation(200, 150, pynlm.WorldObjectType.Hazard, -1.0)

# Run world simulation
results = world.runEpisode(steps=100, recordHistory=True)
print(f"World simulation completed in {results['total_time_ms']:.2f} ms")
```

### Agent-Brain Integration

```python
import pynlm

# Create brain and agent
brain = pynlm.Brain()
agent = pynlm.AgentBrain.createCuriosityDrivenAgent(brain)

# Create world
world = pynlm.Factory.createWorldTemplate("resource_harvest")

# Initialize agent
agent.initialize(*world)

# Run agent episode
results = agent.runEpisodeWithMetrics(world, steps=500, enableProfiler=True)

print(f"✓ Agent episode completed")
print(f"  - Steps: {results['steps']}")
print(f"  - Final reward: {results['final_reward']:.3f}")
print(f"  - Curiosity level: {results['curiosity']:.3f}")
print(f"  - Novelty level: {results['novelty']:.3f}")
```

## 5. Advanced Features

### Context Manager Support

```python
import pynlm

# Create brain
brain = pynlm.Brain().withNeuronCount(300)

# Use context manager for automatic cleanup
with pynlm.BrainContext(brain) as ctx:
    print("✓ Brain context created")
    
    # Run simulation
    brain.runEpisode(100)
    
    # Get diagnostics
    diagnostics = brain.getDiagnostics()
    print("=== Diagnostics ===")
    print(diagnostics)

# Brain is automatically reset after context exit
print("✓ Brain context exited (automatically reset)")
```

### Performance Benchmarking

```python
import pynlm

# Create brain
brain = pynlm.Brain().withNeuronCount(500)

# Run comprehensive benchmark
benchmark_results = brain.benchmark(steps=2000, warmupSteps=200)

print("=== Performance Benchmark ===")
print(f"✓ Benchmark completed:")
print(f"  - Steps: {benchmark_results['steps']}")
print(f"  - Warmup: {benchmark_results['warmup_steps']}")
print(f"  - Total time: {benchmark_results['total_time_ms']:.2f} ms")
print(f"  - Steps/sec: {benchmark_results['steps_per_second']:.0f}")
print(f"  - Spikes generated: {benchmark_results['spikes_generated']}")
print(f"  - Final firing rate: {benchmark_results['final_firing_rate']:.3f}")

# Get memory information
memory_info = brain.getMemoryInfo()
print(f"\nMemory usage: ~{memory_info['memory_usage_estimate_kb']:.0f} KB")
```

### Data Analysis and Export

```python
import pynlm

# Create and run brain
brain = pynlm.Brain()
brain.initialize()
brain.runEpisode(1000)

# Collect metrics
metrics = brain.collectMetrics()
connectivity = brain.analyzeConnectivity()

print("=== Data Analysis ===")
print(f"Neurons: {metrics['total_neurons']}")
print(f"Synapses: {metrics['total_synapses']}")
print(f"Active neurons: {metrics['active_neurons']}")
print(f"Firing rate: {metrics['average_firing_rate']:.3f}")
print(f"Total spikes: {metrics['total_spikes']}")
print(f"Strong connections: {connectivity.get('strong_ratio', 0):.3f}")

# Export for analysis
export_data = brain.exportForAnalysis()
import json
print(f"\nExported data keys: {list(export_data.keys())}")
```

### Factory Functions

```python
import pynlm

# Create complete simulation ecosystem
ecosystem = pynlm.Factory.createSimulationEcosystem({
    'neuron_count': 600,
    'region_count': 2,
    'auto_initialize': True
})

brain = ecosystem['brain']
world = ecosystem['world']
agent = ecosystem['agent']

print("✓ Simulation ecosystem created")
print(f"  - Brain: {brain.getTotalNeuronCount()} neurons")
print(f"  - World: {world.getWidth()}x{world.getHeight()}")
print(f"  - Agent: curiosity enabled={agent.isCuriosityEnabled()}")
```

### Experiment Management

```python
import pynlm

# Create world
world = pynlm.Factory.createWorldTemplate("maze")

# Create brain and agent
brain = pynlm.Brain()
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
print("=== Experiment Report ===")
print(report)
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
          f"with {analysis['total_objects']} objects, "
          f"{analysis['resources']} resources, "
          f"{analysis['hazards']} hazards")
```

### Batch Processing

```python
import pynlm

# Create multiple worlds
worlds = [
    pynlm.Factory.createWorldTemplate("empty"),
    pynlm.Factory.createWorldTemplate("resource_harvest"),
    pynlm.Factory.createWorldTemplate("maze")
]

brain = pynlm.Brain()
agent = pynlm.AgentBrain.createCuriosityDrivenAgent(brain)

# Run batch of episodes across all worlds
batch_results = agent.runBatchEpisodes(worlds, stepsPerWorld=300)

print("=== Batch Processing Results ===")
print(f"✓ Processed {batch_results['total_worlds']} worlds")
print(f"  - Average final reward: {sum(r['final_reward'] for r in batch_results['world_results']) / len(batch_results['world_results']):.3f}")
print(f"  - Average curiosity: {sum(r['curiosity'] for r in batch_results['world_results']) / len(batch_results['world_results']):.3f}")
```

## 6. Error Handling

### Custom Exceptions

```python
import pynlm

brain = pynlm.Brain()

# Exception handling
try:
    # Create and initialize brain
    brain.initialize()
    
    # Run simulation
    brain.runEpisode(100)
    
    # Validate configuration
    config = brain.getConfig()
    config.validate()
    
    # Save brain state
    brain.saveWithValidation("brain_state.pkl")
    
except pynlm.NLMBrainError as e:
    print(f"✗ Brain error: {e}")
except pynlm.NLMInvalidParameter as e:
    print(f"✗ Invalid parameter: {e}")
except pynlm.NLMConfigurationError as e:
    print(f"✗ Configuration error: {e}")
except pynlm.NLMFileNotFound as e:
    print(f"✗ File not found: {e}")
except Exception as e:
    print(f"✗ Unexpected error: {e}")
```

### Input Validation

```python
import pynlm

# Test configuration validation
invalid_configs = [
    {"simulation_timestep": -1.0},  # Invalid: negative timestep
    {"random_seed": "invalid"},     # Invalid: string seed
    {"neuron_count": 0},            # Invalid: zero neurons
]

for config_dict in invalid_configs:
    try:
        config = pynlm.Config.from_dict(config_dict)
        if config.validate():
            print(f"✗ Expected validation failure for {config_dict}")
        else:
            print(f"✓ Correctly rejected invalid config: {config_dict}")
    except pynlm.NLMConfigurationError as e:
        print(f"✓ Validation error: {e}")
```

## 7. Performance Optimization

### Memory Management

```python
import pynlm

# Create brain with performance monitoring
brain = pynlm.Brain()

# Get memory information before and after operations
print("=== Memory Management ===")
memory_before = brain.getMemoryInfo()
print(f"Initial memory estimate: ~{memory_before['memory_usage_estimate_kb']} KB")

# Run simulation
brain.runEpisode(500)

memory_after = brain.getMemoryInfo()
print(f"After simulation: ~{memory_after['memory_usage_estimate_kb']} KB")

# Benchmark different configurations
configs = [
    {"neuron_count": 200, "description": "Small"},
    {"neuron_count": 500, "description": "Medium"},
    {"neuron_count": 1000, "description": "Large"}
]

for config_dict in configs:
    brain = pynlm.Brain(config_dict)
    results = brain.benchmark(steps=1000, warmupSteps=100)
    print(f"{config_dict['description']}: {results['steps_per_second']:.0f} steps/sec")
```

### Efficient Data Processing

```python
import pynlm

# Create brain with efficient configuration
brain = pynlm.Brain().withNeuronCount(300)

# Collect metrics efficiently
metrics = brain.collectMetrics(sampleSteps=50)
connectivity = brain.analyzeConnectivity()

print("=== Efficient Data Processing ===")
print(f"Collected metrics in one operation:")
print(f"  - {len(metrics)} different metrics")
print(f"  - Connectivity: {connectivity}")

# Batch processing for multiple runs
print("\n=== Batch Processing ===")
step_counts = [100, 500, 1000, 2000]
benchmark_results = pynlm.SimulationManager.runBenchmark(
    brain,
    step_counts=step_counts,
    warmup_steps=[10, 50, 100, 200]
)

for i, bench in enumerate(benchmark_results['benchmarks']):
    if bench['success']:
        results = bench['results']
        print(f"  {step_counts[i]} steps: {results['steps_per_second']:.0f} steps/sec")
```

## 8. Data Analysis and Export

### Brain State Export

```python
import pynlm

# Create and run brain
brain = pynlm.Brain()
brain.initialize()
brain.runEpisode(1000)

# Export comprehensive analysis
analysis_data = brain.exportForAnalysis()

print("=== Brain State Export ===")
print(f"Total spikes: {analysis_data['total_spikes']}")
print(f"Number of regions: {len(analysis_data['regions'])}")

# Save analysis data
import json
import os

if not os.path.exists('exports'):
    os.makedirs('exports')

with open('exports/brain_analysis.json', 'w') as f:
    json.dump(analysis_data, f, indent=2)

print("✓ Analysis data saved to exports/brain_analysis.json")
```

### World State Export

```python
import pynlm

# Create and run world
world = pynlm.SimpleWorld.create(200, 200, 10, 10)

# Add some objects
world.addObjectWithValidation(50, 50, pynlm.WorldObjectType.Resource, 1.0)
world.addObjectWithValidation(150, 150, pynlm.WorldObjectType.Marker, 0.0)

# Export world state
world_state = world.exportWorldState()

print("=== World State Export ===")
print(f"World dimensions: {world_state['width']}x{world_state['height']}")
print(f"Agent position: ({world_state['agent_x']}, {world_state['agent_y']})")
print(f"Number of objects: {len(world_state['objects'])}")

# Import world state into new world
new_world = pynlm.SimpleWorld()
new_world.importWorldState(world_state)

print("✓ World state imported into new world")
```

### Research Data Export

```python
import pynlm

# Run experiment
world = pynlm.Factory.createWorldTemplate("resource_harvest")
brain = pynlm.Brain()
agent = pynlm.AgentBrain.createCuriosityDrivenAgent(brain)

experiment_results = pynlm.SimulationManager.runExperiment(
    world, brain, agent,
    trials=3,
    stepsPerTrial=1000
)

# Export experiment data
import json

experiment_data = {
    'world_template': 'resource_harvest',
    'trials': experiment_results['trials'],
    'config': brain.getConfig().to_dict(),
    'timestamp': pynlm.SimulationManager.getCurrentTimestamp()
}

with open('exports/experiment_results.json', 'w') as f:
    json.dump(experiment_data, f, indent=2)

print("✓ Experiment data exported to exports/experiment_results.json")
```

## 9. Best Practices

### 1. Configuration Management

```python
import pynlm

# Best practice: Always validate configuration
config = pynlm.Config()
config.withTimestep(0.001)
config.withNeuronCount(1000)

if not config.validate():
    raise pynlm.NLMConfigurationError("Invalid configuration")

brain = pynlm.Brain(config)
```

### 2. Error Handling

```python
import pynlm

# Best practice: Always handle errors
brain = pynlm.Brain()

try:
    brain.initialize()
    brain.runEpisode(500)
    brain.saveWithValidation("results/brain.pkl")
    
except pynlm.NLMBrainError as e:
    # Handle brain-specific errors
    print(f"Brain error: {e}")
except pynlm.NLMInvalidParameter as e:
    # Handle invalid parameters
    print(f"Invalid parameter: {e}")
except Exception as e:
    # Handle unexpected errors
    print(f"Unexpected error: {e}")
```

### 3. Resource Management

```python
import pynlm

# Best practice: Use context managers for resource management
brain = pynlm.Brain()

# Old way (manual cleanup)
brain.initialize()
brain.runEpisode(100)
brain.reset()

# New way (automatic cleanup)
with pynlm.BrainContext(brain) as ctx:
    ctx.brain.initialize()
    ctx.brain.runEpisode(100)
# Brain is automatically reset when context exits
```

### 4. Performance Monitoring

```python
import pynlm

# Best practice: Always monitor performance
brain = pynlm.Brain()

# Run benchmark to establish performance baseline
benchmark_results = brain.benchmark(steps=1000, warmupSteps=100)

print(f"Performance baseline: {benchmark_results['steps_per_second']:.0f} steps/sec")

# Monitor memory usage
memory_info = brain.getMemoryInfo()
print(f"Memory usage: ~{memory_info['memory_usage_estimate_kb']} KB")

# Collect metrics regularly
metrics = brain.collectMetrics(sampleSteps=100)
print(f"Current firing rate: {metrics['average_firing_rate']:.3f}")
```

### 5. Data Validation

```python
import pynlm

# Best practice: Always validate data
brain = pynlm.Brain()
brain.initialize()

# Validate brain before using it
if not brain.validateBrain():
    raise pynlm.NLMBrainError("Brain validation failed")

# Validate world if available
if hasattr(brain, 'world'):
    validation = brain.world.validateWorld()
    if not validation['valid']:
        print(f"World validation errors: {validation['errors']}")
```

## 10. Troubleshooting

### Common Issues and Solutions

#### Issue 1: Brain initialization fails

**Problem:** Brain initialization fails with error "Failed to initialize brain!"

**Solution:**
```python
import pynlm

# Check configuration
config = pynlm.Config()
print("Configuration:", config.getSummary())

# Check if configuration is valid
if not config.validate():
    print("✗ Configuration validation failed")
    # Fix configuration
    config.withTimestep(0.001)
    config.withNeuronCount(100)

# Create brain and try again
brain = pynlm.Brain(config)
try:
    brain.initialize()
    print("✓ Brain initialized successfully")
except pynlm.NLMBrainError as e:
    print(f"✗ Brain initialization failed: {e}")
```

#### Issue 2: High memory usage

**Problem:** Memory usage is higher than expected

**Solution:**
```python
import pynlm

brain = pynlm.Brain().withNeuronCount(500)
memory_info = brain.getMemoryInfo()

print(f"Memory usage estimate: ~{memory_info['memory_usage_estimate_kb']} KB")
print(f"Active neurons: {memory_info['active_neurons']}")
print(f"Total neurons: {memory_info['total_neurons']}")

# Try with smaller network
brain_small = pynlm.Brain().withNeuronCount(100)
results = brain_small.benchmark(steps=500, warmupSteps=50)
print(f"Small network: {results['steps_per_second']:.0f} steps/sec")
```

#### Issue 3: Performance degradation

**Problem:** Simulation performance degrades over time

**Solution:**
```python
import pynlm

brain = pynlm.Brain()

# Run periodic benchmarks
for run in range(5):
    results = brain.benchmark(steps=1000, warmupSteps=100)
    print(f"Run {run + 1}: {results['steps_per_second']:.0f} steps/sec")
    
    # Reset brain for next run
    brain.reset()
    brain.initialize()
```

### Debugging Tips

#### Enable verbose logging

```python
import pynlm

# Enable debug logging
brain = pynlm.Brain()
brain.getConfig().set("log_level", "Debug", pynlm.ConfigSource::User)

# Initialize with logging
brain.initialize()
brain.logStatus()  # Shows detailed brain status
```

#### Check system resources

```python
import pynlm
import psutil
import time

# Check memory and CPU usage
print("=== System Resources ===")
print(f"Memory: {psutil.virtual_memory().percent}% used")
print(f"CPU: {psutil.cpu_percent()}%")

# Create brain and monitor resources
brain = pynlm.Brain()

# Run simulation while monitoring
start_time = time.time()
start_memory = psutil.virtual_memory().used

brain.runEpisode(1000)

end_time = time.time()
end_memory = psutil.virtual_memory().used

print(f"Simulation time: {end_time - start_time:.2f} seconds")
print(f"Memory delta: {(end_memory - start_memory) / 1024 / 1024:.2f} MB")
```

## 11. API Reference

### Core Classes

#### Config
```python
class Config:
    def __init__(self)
    def withTimestep(self, timestep) -> Config
    def withRandomSeed(self, seed) -> Config
    def withNeuronCount(self, count) -> Config
    def withRegionCount(self, count) -> Config
    def loadFromFile(self, filepath)
    def saveToFile(self, filepath)
    def has(self, key)
    def getKeys(self)
    def validate() -> bool
    def to_dict(self) -> dict
    def from_dict(cls, config_dict) -> Config
    def getSummary(self) -> str
```

#### Brain
```python
class Brain:
    def __init__(self, config) -> Config
    def initialize(self)
    def step(self, currentStep, currentTime)
    def receiveSensoryInput(self, input)
    def produceAction(self) -> Action
    def reset(self)
    def save(self, filepath)
    def load(self, filepath)
    def runEpisode(self, steps, world=None)
    def collectMetrics(self, sampleSteps=100) -> dict
    def analyzeConnectivity(self) -> dict
    def benchmark(self, steps=1000, warmupSteps=100) -> dict
    def getMemoryInfo(self) -> dict
    def findNeurons(self, type) -> list
    def exportForAnalysis(self) -> dict
    def importFromAnalysis(self, data)
    def validateBrain(self) -> bool
    def getDiagnostics(self) -> str
```

#### AgentBrain
```python
class AgentBrain:
    def __init__(self, brain) -> Brain
    def createCuriosityDrivenAgent(cls, brain, curiosityThreshold=0.5, enableStructuralPlasticity=True) -> AgentBrain
    def createRewardBasedAgent(cls, brain, enableRewardModulation=True, enableDevelopment=True) -> AgentBrain
    def createExplorationAgent(cls, brain, explorationRate=0.3) -> AgentBrain
    def runEpisodeWithMetrics(self, world, steps, enableProfiler=False) -> dict
    def runBatchEpisodes(self, worlds, stepsPerWorld) -> dict
    def analyzeSensoryInput(self, percept) -> dict
```

#### SimpleWorld
```python
class SimpleWorld:
    def __init__(self)
    def create(cls, width=320, height=240, visionWidth=16, visionHeight=16) -> SimpleWorld
    def configure(self, width, height, visionWidth, visionHeight)
    def runEpisode(self, steps, recordHistory=False) -> dict
    def analyzeEnvironment(self) -> dict
    def exportWorldState(self) -> dict
    def importWorldState(self, state)
    def generateFromTemplate(cls, templateName) -> SimpleWorld
    def validateWorld(self) -> dict
```

#### SimulationManager
```python
class SimulationManager:
    @staticmethod
    def createSimulation(world, brain, agent=None) -> dict
    
    @staticmethod
    def runCompleteSimulation(world, brain, agent, steps) -> dict
    
    @staticmethod
    def runExperiment(world, brain, agent, trials, stepsPerTrial, randomizeSeeds=False) -> dict
    
    @staticmethod
    def runBenchmark(brain, step_counts, warmup_steps) -> dict
    
    @staticmethod
    def generateAnalysisReport(experiment_results) -> str
```

#### Factory
```python
class Factory:
    @staticmethod
    def createSimulationEcosystem(config_dict=None) -> dict
    
    @staticmethod
    def createExperimentSetup(experimentType="default") -> dict
    
    @staticmethod
    def createWorldTemplate(templateName) -> SimpleWorld
    
    @staticmethod
    def createBenchmarkConfig(benchmarkType="standard") -> dict
```

### Exceptions

```python
class NLMException(Exception)
class NLMBrainError(NLMException)
class NLMInvalidParameter(NLMException)
class NLMFileNotFound(NLMException)
class NLMConfigurationError(NLMException)
```

### Utility Functions

```python
def get_version() -> str
```

### Constants

```python
WorldObjectType: Enum
  - Empty
  - Resource
  - Hazard
  - Wall
  - Marker

NeuronType: Enum
  - Excitatory
  - Inhibitory
  - Modulatory
  - Sensory
  - Motor
  - Internal

ActionType: Enum
  - MoveForward
  - MoveBackward
  - TurnLeft
  - TurnRight
  - Look
  - LookUp
  - LookDown
  - Interact
  - Eat
  - Drink
  - Rest
  - Wait
  - Custom
```

## Installation and Setup

### Prerequisites

- Python 3.8 or higher
- C++20 compatible compiler (g++ >= 9, clang >= 10, or MSVC >= 19.14)
- pybind11 >= 2.11.0 (will be installed automatically)
- scikit-build-core >= 0.5.0

### Installation Steps

1. **Install system dependencies**
```bash
# Ubuntu/Debian
apt-get update
apt-get install -y g++ cmake python3-dev python3-pip

# macOS
brew install cmake python3
```

2. **Install Python package**
```bash
pip install pynlm
```

3. **Verify installation**
```python
import pynlm
print(f"NLM Python Bindings: {pynlm.get_version()}")
ecosystem = pynlm.Factory.createSimulationEcosystem()
print("✓ Installation successful!")
```

### Development Setup

For development, you may want to clone the repository and build from source:

```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install -e .
```

## Changes and Contributions

### Enhancing the Enhanced API

The enhanced bindings provide many advanced features. You can contribute by:

1. **Adding new convenience methods** to existing classes
2. **Implementing data analysis tools** for specific research needs
3. **Creating new world templates** for different experiment scenarios
4. **Adding performance monitoring** and profiling tools
5. **Improving error messages** and diagnostics

### Example: Adding a Convenience Method

```python
# Example of adding a convenience method to Brain class
class PyBrain:
    def runLearningExperiment(self, pattern_length=10, trials=5):
        """Run a learning experiment with repeated input patterns."""
        results = []
        for trial in range(trials):
            # Reset and run learning trial
            self.reset()
            self.initialize()
            
            # Apply repeated input pattern
            for step in range(pattern_length):
                # Inject specific pattern
                self.injectCurrentToNeurons(NeuronType::Sensory, 30.0f)
                self.step(step, step * self.getTimestep())
            
            # Collect metrics
            metrics = self.collectMetrics()
            results.append(metrics)
        
        return results
```

## Credits

The enhanced Python bindings build upon the original NLM project:
- **Original Authors:** NLM Authors
- **Enhanced by:** Kilo (AI Assistant)
- **Based on:** Phase 6: Final Integration of NLM Neural Learning Machine

## License

MIT License

---

*For more information, visit the [NLM Documentation](https://nlm.readthedocs.io)*