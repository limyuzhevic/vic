# NLM — Comprehensive Documentation

This documentation provides comprehensive information about the Neural Learning Machine (NLM) system,
covering all aspects from installation and configuration to advanced usage and API reference.

## Table of Contents

1. [Overview](#overview)
2. [Quick Start](#quick-start)
3. [Installation and Setup](#installation-and-setup)
4. [Configuration Management](#configuration-management)
5. [Command Line Interface (CLI)](#command-line-interface)
6. [Python API Reference](#python-api-reference)
7. [Advanced Features](#advanced-features)
8. [Experiments and Examples](#experiments-and-examples)
9. [Performance and Optimization](#performance-and-optimization)
10. [Troubleshooting](#troubleshooting)
11. [API Reference](#api-reference)
12. [Changelog](#changelog)

## Overview

NLM (Neural Learning Machine) is a brain-inspired spiking neural network simulator that implements
Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays,
and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

### Key Features

- **Phase 6 Integration**: All systems integrated into a coherent artificial brain loop
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction System**: Neural prediction and prediction error
- **Cognition**: Planning, attention, concept formation
- **Development**: Synaptogenesis, pruning, maturation
- **Plasticity**: STDP, Hebbian, structural plasticity
- **Replay and Consolidation**: Episodic memory replay during sleep/rest
- **Python Bindings**: Full Python API with enhanced error handling and advanced features

## Quick Start

### Basic Usage (Python)

```python
import pynlm

# 1. Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# 2. Run a simple simulation
for i in range(1000):
    brain.step(i)
    if i % 100 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")

# 3. Get results
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Basic Usage (CLI)

```bash
# Basic simulation
./nlm --mode basic

# Advanced simulation with custom settings
./nlm --mode advanced --steps 5000 --benchmark

# Debug simulation with detailed logging
./nlm --mode debug --log-level DEBUG --log-file debug.log
```

### Configuration File

Create a configuration file (`config.json`):

```json
{
    "description": "NLM Configuration",
    "version": "1.0",
    "entries": [
        {
            "key": "random_seed",
            "value": 42,
            "source": 0,
            "description": "Random seed for reproducible experiments"
        },
        {
            "key": "simulation_timestep",
            "value": 0.001,
            "source": 0,
            "description": "Simulation timestep in seconds"
        },
        {
            "key": "neuron_count",
            "value": 1000,
            "source": 0,
            "description": "Number of neurons in the brain"
        },
        {
            "key": "connection_probability",
            "value": 0.1,
            "source": 0,
            "description": "Connection probability for random connectivity"
        }
    ]
}
```

## Installation and Setup

### System Requirements

- **Operating System**: Linux, macOS, or Windows
- **Compiler**: C++20 compatible compiler (GCC 9+, Clang 12+, MSVC 2019+)
- **CMake**: Version 3.16 or higher
- **Python**: 3.8 or higher
- **Optional**: Python packages for enhanced features

### Building from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Install Python bindings
pip install -e .
```

### Python Package Installation

```bash
# Install from source (recommended for development)
pip install -e .

# Or install from PyPI (if available)
pip install pynlm
```

### Development Installation

```bash
# Install development dependencies
pip install scikit-build-core pybind11 pytest numpy

# Install in development mode
pip install -e .[dev]
```

## Configuration Management

### Configuration File Formats

NLM supports both JSON and YAML configuration files:

#### JSON Configuration

```json
{
    "description": "NLM Configuration",
    "version": "1.0",
    "entries": [
        {
            "key": "brain.neuron_count",
            "value": 1000,
            "description": "Number of neurons",
            "source": "default"
        },
        {
            "key": "plasticity.stdp.enable",
            "value": true,
            "description": "Enable STDP plasticity",
            "source": "default"
        }
    ]
}
```

#### YAML Configuration

```yaml
description: "NLM Configuration"
version: "1.0"
entries:
  - key: "brain.neuron_count"
    value: 1000
    description: "Number of neurons"
    source: "default"
  - key: "plasticity.stdp.enable"
    value: true
    description: "Enable STDP plasticity"
    source: "default"
```

### Configuration Schema

NLM uses a flexible configuration system with the following parameter types:

#### Basic Types

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `random_seed` | int | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | float | 0.001 | Simulation timestep in seconds |
| `neuron_count` | int | 1000 | Number of neurons in the brain |
| `region_count` | int | 1 | Number of brain regions |

#### Range Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| `connection_probability` | [0.0, 1.0] | 0.1 | Probability of connection between neurons |
| `stdp_ltp_weight` | [0.0, 1.0] | 0.01 | STDP LTP weight |
| `stdp_ltd_weight` | [0.0, 1.0] | 0.012 | STDP LTD weight |
| `stdp_tau` | [1.0, 100.0] | 20.0 | STDP time constant |

#### Boolean Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| `enable_reward_modulation` | true | Enable reward prediction error |
| `enable_structural_plasticity` | true | Enable structural plasticity |
| `enable_development` | true | Enable developmental stages |
| `enable_curiosity` | true | Enable curiosity-driven exploration |
| `enable_novelty` | true | Enable novelty detection |

### Configuration Examples

#### Minimal Configuration

```python
import pynlm

# Create brain with minimal configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("simulation_timestep", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()
```

#### Advanced Configuration

```python
import pynlm

# Create advanced configuration with multiple subsystems
config = pynlm.createDefaultConfig()

# Memory systems
config.set("working_memory.capacity", 5000)
config.set("episodic_memory.max_episodes", 10000)

# Neuromodulation
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.threshold", 0.3)
config.set("neuromod.novelty.decay", 0.99)

# Plasticity
config.set("plasticity.stdp.enable", true)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", true)
config.set("plasticity.structural.enable", true)

# Cognition
config.set("cognition.planning.depth", 5)
config.set("cognition.attention.inhibition_strength", 0.5)

brain = pynlm.createBrain(config)
brain.initialize()
```

## Command Line Interface (CLI)

### Overview

The NLM CLI provides comprehensive control over simulations, including multiple execution modes,
configuration management, and advanced debugging capabilities.

### Basic Usage

```bash
# Show help
./nlm --help

# Show version information
./nlm --version

# Show build information
./nlm --build-info

# Basic simulation
./nlm --mode basic

# Advanced simulation
./nlm --mode advanced --steps 5000

# Debug simulation
./nlm --mode debug --log-level DEBUG
```

### CLI Options

#### Execution Modes

| Option | Description |
|--------|-------------|
| `--mode basic` | Basic demonstration with connectivity tests |
| `--mode advanced` | Full integration experiment with all features |
| `--mode debug` | Debug mode with detailed logging and analysis |
| `--mode interactive` | Interactive mode with user prompts |

#### Configuration Options

| Option | Description |
|--------|-------------|
| `--config <file>` | Configuration file (JSON/YAML, default: configs/default.cfg) |
| `--validate-config` | Validate configuration file and exit |
| `--save-config` | Save current config to file |
| `--random-seed <n>` | Set random seed (default: 42) |

#### Execution Control

| Option | Description |
|--------|-------------|
| `--steps <n>` | Number of simulation steps (default: 1000) |
| `--timestep <t>` | Simulation timestep in seconds (default: 0.001) |

#### Output and Logging

| Option | Description |
|--------|-------------|
| `--output <file>` | Output file for results (default: nlm_output.txt) |
| `--log-level <L>` | Log level: DEBUG, INFO, WARN, ERROR, CRITICAL |
| `--log-file <file>` | Log output to file |
| `--verbose` | Enable verbose output |
| `--quiet` | Suppress non-error output |

#### Checkpointing

| Option | Description |
|--------|-------------|
| `--checkpoint <file>` | Save/load checkpoint file |
| `--benchmark` | Run performance benchmark |

### Configuration Validation Tool

NLM includes a configuration validation tool (`nlm-config-validate`) that validates
configuration files against NLM schema:

```bash
# Basic validation
./nlm-config-validate config.json

# Validate with schema
./nlm-config-validate --schema nlm_schema.json config.json

# Validate with verbose output
./nlm-config-validate --verbose --strict config.yaml

# Validate with errors only
./nlm-config-validate --errors-only config.json

# Write validation results to file
./nlm-config-validate --output validation_report.json config.json
```

### Configuration Validation Output

The configuration validation tool provides detailed output including:

- Validation status (OK, WARNING, CRITICAL)
- Total parameters and validation statistics
- List of errors and warnings
- Detailed parameter information
- Performance metrics

## Python API Reference

### Core Classes

#### Brain Class

The `Brain` class is the main interface for neural network simulation.

**Methods:**

```python
class Brain:
    def __init__(self, config: Config):
        """Create a new brain with the given configuration."""
        pass
    
    def initialize(self) -> bool:
        """Initialize the brain. Returns True on success."""
        pass
    
    def step(self, step_number: int, time: Optional[float] = None) -> None:
        """Run one simulation step."""
        pass
    
    def reset(self) -> None:
        """Reset the brain to its initial state."""
        pass
    
    # Accessors
    def getTotalNeuronCount(self) -> int
    def getTotalSynapseCount(self) -> int
    def getFiringNeuronCount(self) -> int
    def getAverageFiringRate(self) -> float
    def getExcitationInhibitionRatio(self) -> float
    def getTotalSpikeCount(self) -> int
    def getPendingSpikeEventCount(self) -> int
    
    # Memory systems
    def getWorkingMemory(self) -> WorkingMemory
    def getEpisodicMemory(self) -> EpisodicMemory
    def getAssociativeMemory(self) -> AssociativeMemory
    
    # Neuromodulation
    def getDopamine(self) -> Dopamine
    def getCuriosity(self) -> Curiosity
    def getNovelty(self) -> Novelty
    
    # Cognition systems
    def getPlanner(self) -> NeuralPlanner
    def getConceptFormation(self) -> ConceptFormation
    def getAttention(self) -> AttentionalSelection
    
    # Development
    def getDevelopmentSystem(self) -> DevelopmentSystem
    def getDevelopmentalStage(self) -> DevelopmentalStage
    
    # Plasticity
    def getSTDP(self) -> STDP
    def getHebbian(self) -> Hebbian
    def getStructuralPlasticity(self) -> StructuralPlasticity
    
    # State management
    def save(self, filepath: str) -> bool
    def load(self, filepath: str) -> bool
    def logStatus(self) -> None
```

#### Config Class

The `Config` class manages configuration parameters.

**Methods:**

```python
class Config:
    def __init__(self):
        """Create a new empty configuration."""
        pass
    
    @classmethod
    def createDefaultConfig(cls) -> 'Config':
        """Create a configuration with default values."""
        pass
    
    def loadFromFile(self, filepath: str) -> bool:
        """Load configuration from file (JSON/YAML)."""
        pass
    
    def saveToFile(self, filepath: str) -> bool:
        """Save configuration to file (JSON/YAML)."""
        pass
    
    def has(self, key: str) -> bool:
        """Check if configuration contains the key."""
        pass
    
    def getKeys(self) -> List[str]:
        """Get all configuration keys."""
        pass
    
    def set(self, key: str, value: Any, source: ConfigSource = ConfigSource.RUNTIME) -> None:
        """Set a configuration value."""
        pass
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value."""
        pass
    
    def summary(self) -> str:
        """Get configuration summary as string."""
        pass
```

#### SimpleWorld Class

The `SimpleWorld` class represents the environment in which the brain operates.

**Methods:**

```python
class SimpleWorld:
    def __init__(self):
        """Create a new empty world."""
        pass
    
    def configure(self, width: int, height: int, visionWidth: int, visionHeight: int) -> None:
        """Configure world dimensions."""
        pass
    
    def reset(self) -> None:
        """Reset world to initial state."""
        pass
    
    def update(self, timestep: float) -> None:
        """Update world state."""
        pass
    
    def setAgentStart(self, x: float, y: float) -> None:
        """Set agent starting position."""
        pass
    
    def applyMotorCommand(self, command: MotorCommand, time: float) -> None:
        """Apply motor command to world."""
        pass
    
    def getSensoryPercept(self) -> SensoryPercept:
        """Get sensory percept from world."""
        pass
    
    def getAgentBody(self) -> Body:
        """Get agent body state."""
        pass
```

#### AgentBrain Class

The `AgentBrain` class bridges the brain and world, processing sensory input and producing motor commands.

**Methods:**

```python
class AgentBrain:
    def __init__(self, brain: Brain):
        """Create an agent brain interface."""
        pass
    
    def initialize(self, world: SimpleWorld) -> None:
        """Initialize agent with world."""
        pass
    
    def processSensoryInput(self, percept: SensoryPercept) -> None:
        """Process sensory input from world."""
        pass
    
    def decodeMotorCommand(self) -> MotorCommand:
        """Decode motor command from brain activity."""
        pass
    
    # Subsystem enable/disable
    def enableRewardModulation(self, enabled: bool) -> None
    def enableStructuralPlasticity(self, enabled: bool) -> None
    def enableDevelopment(self, enabled: bool) -> None
    def enableCuriosity(self, enabled: bool) -> None
    
    # Statistics
    def getCuriosityLevel(self) -> float
    def getNoveltyLevel(self) -> float
    def getNeuromodulationLevel(self) -> float
    def getPredictionError(self) -> float
    def getDevelopmentalStage(self) -> DevelopmentalStage
```

### Enumerations

#### NeuronType

```python
class NeuronType:
    EXCITATORY = 0
    INHIBITORY = 1
    SENSORY = 2
    MOTOR = 3
    MODULATORY = 4
    INTERNAL = 5
```

#### DevelopmentalStage

```python
class DevelopmentalStage:
    INITIAL = 0
    CRITICAL_PERIOD = 1
    MATURATION = 2
    ADULT = 3
    AGING = 4
```

#### ActionType

```python
class ActionType:
    MOVE_FORWARD = 0
    MOVE_BACKWARD = 1
    TURN_LEFT = 2
    TURN_RIGHT = 3
    LOOK_LEFT = 4
    LOOK_RIGHT = 5
    INTERACT = 6
    WAIT = 7
```

#### WorldObjectType

```python
class WorldObjectType:
    EMPTY = 0
    RESOURCE = 1
    HAZARD = 2
    WALL = 3
    MARKER = 4
```

## Advanced Features

### Enhanced Configuration Management

The enhanced configuration system provides advanced features:

1. **Multi-source loading**: Load configuration from multiple sources (JSON, YAML, environment variables)
2. **Schema validation**: Validate configuration against NLM schema
3. **Configuration inheritance**: Merge configurations with inheritance
4. **Environment variable integration**: Support for environment variable configuration
5. **Configuration snapshots**: Save and restore configuration snapshots

### Advanced Brain Features

1. **Memory system optimization**: Automatic optimization of memory systems
2. **Performance monitoring**: Real-time performance monitoring and optimization
3. **Advanced plasticity**: Custom plasticity rules and learning algorithms
4. **Neuromodulation control**: Fine-grained control over neuromodulation systems
5. **Development stages**: Biologically inspired developmental stages

### Advanced Experiment Framework

1. **Parameter sweeps**: Systematically vary parameters to explore behavior
2. **Cross-validation**: Validate models on different datasets
3. **Meta-learning**: Learn from learning experiences
4. **Experiment tracking**: Comprehensive experiment documentation

### Integration with Python Ecosystem

1. **NumPy integration**: NumPy-enhanced data structures and operations
2. **Matplotlib integration**: Visualization and plotting capabilities
3. **Jupyter notebook support**: Interactive documentation and examples
4. **Data analysis**: Integration with data analysis libraries

## Experiments and Examples

### Example 1: Simple Brain Simulation

```python
import pynlm
import time

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run simulation
start_time = time.time()
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Example 2: Brain in a World

```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent in world
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Agent simulation completed")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
```

### Example 3: Advanced Experiment

```python
import pynlm
from typing import Dict, Any

class AdvancedExperiment:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Configure world
        self.world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        
        # Configure agent
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableNovelty(True)
    
    def run_parameter_sweep(self, parameters: List[Dict[str, Any]], steps: int = 1000):
        """Run parameter sweep over configuration parameters."""
        results = []
        
        for param_set in parameters:
            print(f"Running experiment with parameters: {param_set}")
            
            # Create new brain with parameter set
            config = pynlm.createDefaultConfig()
            for key, value in param_set.items():
                config.set(key, value)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run experiment
            for step in range(steps):
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                brain.step(step)
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Collect results
            result = {
                'parameters': param_set,
                'total_reward': self.agent.getNeuromodulationLevel(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'memory_episodes': brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0,
                'curiosity_level': self.agent.getCuriosityLevel(),
                'novelty_level': self.agent.getNoveltyLevel()
            }
            
            results.append(result)
            
            # Reset for next experiment
            self.world.reset()
            self.agent.reset()
        
        return results

# Example usage
experiment = AdvancedExperiment()

# Define parameter sweep
parameters = [
    {'neuron_count': 500, 'connection_probability': 0.1},
    {'neuron_count': 1000, 'connection_probability': 0.15},
    {'neuron_count': 1500, 'connection_probability': 0.2},
]

# Run parameter sweep
results = experiment.run_parameter_sweep(parameters, steps=500)

# Analyze results
for result in results:
    print(f"Parameters: {result['parameters']}")
    print(f"  Reward: {result['total_reward']:.3f}")
    print(f"  Firing rate: {result['avg_firing_rate']:.3f}")
    print(f"  Memory episodes: {result['memory_episodes']}")
    print(f"  Curiosity: {result['curiosity_level']:.3f}")
    print(f"  Novelty: {result['novelty_level']:.3f}")
    print()
```

## Performance and Optimization

### Performance Monitoring

NLM provides comprehensive performance monitoring:

1. **Real-time monitoring**: Monitor performance during simulation
2. **Benchmarking**: Compare performance across configurations
3. **Optimization suggestions**: Automatically suggest optimizations
4. **Resource usage**: Monitor memory and CPU usage

### Optimization Features

1. **Memory optimization**: Automatically optimize memory usage
2. **Computation optimization**: Optimize computational performance
3. **Simulation optimization**: Optimize simulation parameters
4. **Hardware adaptation**: Adapt simulation to hardware capabilities

### Performance Benchmark

```bash
# Run performance benchmark
./nlm --benchmark --mode advanced --steps 1000

# Compare configurations
./nlm --benchmark --mode advanced --steps 1000 --checkpoint config1.json
./nlm --benchmark --mode advanced --steps 1000 --checkpoint config2.json
```

## Troubleshooting

### Common Issues and Solutions

#### "Brain initialization failed"

**Problem**: The brain fails to initialize.

**Solution**:
```python
# Check if configuration is valid
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Valid value
brain = pynlm.createBrain(config)
if not brain.initialize():
    print("Brain initialization failed")
    sys.exit(1)
```

#### "Configuration file not found"

**Problem**: Configuration file cannot be loaded.

**Solution**:
```bash
# Create directory if it doesn't exist
mkdir -p configs

# Create configuration file
./nlm --config configs/default.cfg

# Or use command line arguments
./nlm --config ./my_config.json
```

#### "Simulation taking too long"

**Problem**: Simulation is taking too long to complete.

**Solutions**:
1. **Reduce simulation steps**: Decrease the number of simulation steps
   ```bash
   ./nlm --mode basic --steps 100
   ```

2. **Reduce neuron count**: Use fewer neurons
   ```python
   config.set("neuron_count", 100)
   ```

3. **Increase timestep**: Use larger simulation timestep
   ```python
   config.set("simulation_timestep", 0.01)
   ```

#### "Memory usage too high"

**Problem**: Memory usage is too high.

**Solutions**:
1. **Reduce memory systems**: Disable memory systems
   ```python
   # Disable working memory
   brain.getWorkingMemory().clear()
   ```

2. **Reduce memory capacity**: Reduce memory capacity
   ```python
   brain.getWorkingMemory().setCapacity(1000)
   ```

3. **Optimize memory**: Run memory optimization
   ```python
   brain.optimize_memory()
   ```

### Error Codes

| Code | Description |
|------|-------------|
| 0 | Success |
| 1 | Validation failed (errors found) |
| 2 | Configuration file not found or cannot be read |
| 3 | Schema file not found or cannot be read |
| 4 | Invalid command line arguments |
| 5 | Internal error during validation |

## API Reference

This section provides detailed API reference for all NLM classes and methods.

### Brain Class Reference

#### Constructor
```python
Brain(config: Config) -> Brain
```

#### Methods
```python
initialize() -> bool
step(step_number: int, time: Optional[float] = None) -> None
reset() -> None
getTotalNeuronCount() -> int
getTotalSynapseCount() -> int
getFiringNeuronCount() -> int
getAverageFiringRate() -> float
getExcitationInhibitionRatio() -> float
getTotalSpikeCount() -> int
getPendingSpikeEventCount() -> int
getWorkingMemory() -> WorkingMemory
getEpisodicMemory() -> EpisodicMemory
getAssociativeMemory() -> AssociativeMemory
getDopamine() -> Dopamine
getCuriosity() -> Curiosity
getNovelty() -> Novelty
getPlanner() -> NeuralPlanner
getConceptFormation() -> ConceptFormation
getAttention() -> AttentionalSelection
getDevelopmentSystem() -> DevelopmentSystem
getDevelopmentalStage() -> DevelopmentalStage
getSTDP() -> STDP
getHebbian() -> Hebbian
getStructuralPlasticity() -> StructuralPlasticity
save(filepath: str) -> bool
load(filepath: str) -> bool
logStatus() -> None
```

### Config Class Reference

#### Constructor
```python
Config() -> Config
```

#### Class Methods
```python
Config.createDefaultConfig() -> Config
```

#### Methods
```python
loadFromFile(filepath: str) -> bool
saveToFile(filepath: str) -> bool
has(key: str) -> bool
getKeys() -> List[str]
set(key: str, value: Any, source: ConfigSource = ConfigSource.RUNTIME) -> None
get(key: str, default: Any = None) -> Any
summary() -> str
```

### SimpleWorld Class Reference

#### Constructor
```python
SimpleWorld() -> SimpleWorld
```

#### Methods
```python
configure(width: int, height: int, visionWidth: int, visionHeight: int) -> None
reset() -> None
update(timestep: float) -> None
setAgentStart(x: float, y: float) -> None
applyMotorCommand(command: MotorCommand, time: float) -> None
getSensoryPercept() -> SensoryPercept
getAgentBody() -> Body
```

### AgentBrain Class Reference

#### Constructor
```python
AgentBrain(brain: Brain) -> AgentBrain
```

#### Methods
```python
initialize(world: SimpleWorld) -> None
processSensoryInput(percept: SensoryPercept) -> None
decodeMotorCommand() -> MotorCommand
enableRewardModulation(enabled: bool) -> None
enableStructuralPlasticity(enabled: bool) -> None
enableDevelopment(enabled: bool) -> None
enableCuriosity(enabled: bool) -> None
getCuriosityLevel() -> float
getNoveltyLevel() -> float
getNeuromodulationLevel() -> float
getPredictionError() -> float
getDevelopmentalStage() -> DevelopmentalStage
```

## Changelog

### Version 0.1.0 (Phase 6: Final Integration)

#### New Features
- Complete integration of all brain systems
- Enhanced Python bindings with advanced error handling
- Configuration validation tool
- Comprehensive CLI interface with multiple execution modes
- Performance monitoring and benchmarking
- Advanced configuration management (JSON/YAML)

#### Breaking Changes
- None (maintains backward compatibility)

#### Bug Fixes
- Configuration loading and validation
- Error handling and exception management
- Performance and memory management

#### Known Issues
- Some experimental features may have performance limitations
- Advanced features require additional system resources

### Future Plans

#### Phase 7: Advanced Learning
- Implement reinforcement learning algorithms
- Add advanced cognitive architectures
- Enhance social learning capabilities

#### Phase 8: Distributed Computing
- Implement multi-process simulation
- Add distributed memory management
- Support for cloud-based simulation

#### Phase 9: Applications
- Domain-specific applications (neuroscience, AI, etc.)
- Educational tools and demonstrations
- Research tools and analysis

### Contact and Support

For questions, issues, or feature requests:

- **Documentation**: https://nlm.readthedocs.io
- **Repository**: https://github.com/nlm-project/nlm
- **Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions

### Acknowledgments

The NLM project acknowledges the contributions of:

- **Research Community**: For inspiration and collaboration
- **Open Source Projects**: For libraries and tools
- **Contributors**: For code, documentation, and support
- **Users**: For feedback and usage

---

**License**: MIT License

**Copyright**: 2026 NLM Authors
