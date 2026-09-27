# NLM Python Bindings API Reference

Comprehensive documentation for the NLM Python bindings, including both the original and enhanced APIs.

## Overview

The NLM Python bindings provide a Python interface to the NLM (Neural Learning Machine) C++ neural simulation framework. The bindings include both the original low-level API and enhanced high-level APIs with better Pythonic interfaces, error handling, and convenience features.

## Installation

```bash
pip install pynlm
```

## API Modules

### 1. Main Package: `pynlm`

#### Core Classes

**Config**
```python
class Config:
    """Configuration class for NLM simulation."""
    
    def __init__(self):
        # Create default configuration
    
    def loadFromFile(self, filepath: str) -> None:
        # Load configuration from JSON file
    
    def saveToFile(self, filepath: str) -> None:
        # Save configuration to JSON file
    
    def has(self, key: str) -> bool:
        # Check if configuration key exists
    
    def getKeys(self) -> List[str]:
        # Get all configuration keys
    
    def clear(self) -> None:
        # Clear all configuration entries
    
    def summary(self) -> str:
        # Get configuration summary
```

**EnhancedConfig**
```python
class EnhancedConfig(Config):
    """Enhanced Config class with Pythonic interface."""
    
    def __init__(self, config: Optional[Config] = None):
        # Initialize with optional existing config
    
    def set(self, key: str, value: Any, source: str = "runtime") -> None:
        # Set configuration value
    
    def get(self, key: str) -> Any:
        # Get configuration value
    
    def get_or(self, key: str, default: Any = None) -> Any:
        # Get configuration value with default
    
    def validate(self) -> List[str]:
        # Validate configuration
    
    def save_to_file(self, filepath: str) -> None:
        # Save configuration to JSON file
    
    def load_from_file(self, filepath: str) -> None:
        # Load configuration from JSON file
```

**Brain**
```python
class Brain:
    """Brain class for NLM simulation."""
    
    def __init__(self, config: Union[EnhancedConfig, Config]):
        # Initialize brain with configuration
    
    def initialize(self) -> bool:
        # Initialize the brain
    
    def step(self, step_number: int, time: Optional[float] = None) -> None:
        # Perform a simulation step
    
    def step_with_time(self, step_number: int, time: float) -> None:
        # Perform a simulation step with timestamp
    
    # ... (many other methods)
```

**EnhancedBrain**
```python
class EnhancedBrain(Brain):
    """Enhanced Brain class with Pythonic properties."""
    
    def __init__(self, config: Union[EnhancedConfig, Config, Any]):
        # Initialize brain with enhanced config
    
    # Pythonic properties for easier access
    @property
    def total_neuron_count(self) -> int:
        # Total number of neurons
    
    @property
    def firing_neuron_count(self) -> int:
        # Number of currently firing neurons
    
    @property
    def total_spike_count(self) -> int:
        # Total number of spikes
    
    @property
    def average_firing_rate(self) -> float:
        # Average firing rate across all neurons
    
    # ... (many other properties)
```

**AgentBrain**
```python
class AgentBrain:
    """Agent brain interface connecting NLM brain to world."""
    
    def __init__(self, brain: Union[Brain, EnhancedBrain]):
        # Initialize agent brain with brain instance
    
    def initialize(self, world: Any) -> bool:
        # Initialize with world
    
    def processSensoryInput(self, percept: Any) -> Any:
        # Process sensory percept and inject into brain
    
    def decodeMotorCommand(self) -> Any:
        # Decode brain motor activity into motor command
    
    # Learning system control methods
    def enable_reward_modulation(self, enable: bool) -> None:
        # Enable or disable reward modulation
    
    def enable_curiosity(self, enable: bool) -> None:
        # Enable or disable curiosity
    
    def enable_development(self, enable: bool) -> None:
        # Enable or disable development
    
    def enable_structural_plasticity(self, enable: bool) -> None:
        # Enable or disable structural plasticity
```

**SimpleWorld**
```python
class SimpleWorld:
    """Simple 2D world for NLM simulation."""
    
    def __init__(self):
        # Create new simple world
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int) -> None:
        # Configure world dimensions and vision
    
    def reset(self) -> None:
        # Reset world to initial state
    
    def update(self, timestep: float) -> None:
        # Update world simulation
    
    def applyMotorCommand(self, action: Any, current_time: float) -> Any:
        # Apply motor command to world
    
    def getSensoryPercept(self) -> SensoryPercept:
        # Get sensory percept from world
    
    def getAgentBody(self) -> AgentBody:
        # Get agent body state
```

**SensoryPercept**
```python
class SensoryPercept:
    """Sensory percept class for NLM."""
    
    def __init__(self):
        # Initialize sensory percept
    
    def getVision(self) -> List[List[float]]:
        # Get vision data
    
    def setVision(self, vision: List[List[float]]) -> None:
        # Set vision data
    
    def getTouch(self) -> List[float]:
        # Get touch data
    
    def setTouch(self, touch: List[float]) -> None:
        # Set touch data
    
    def getAudio(self) -> List[float]:
        # Get audio data
    
    def setAudio(self, audio: List[float]) -> None:
        # Set audio data
    
    def getAllSignals(self) -> Dict[str, List[float]]:
        # Get all sensory signals
```

**Action**
```python
class Action:
    """Action class for NLM."""
    
    def __init__(self, action_type: Optional[Any] = None, parameters: Optional[List[float]] = None):
        # Initialize action
    
    def getType(self) -> Any:
        # Get action type
    
    def getParameters(self) -> List[float]:
        # Get action parameters
    
    def getName(self) -> str:
        # Get action name
    
    def clone(self) -> 'Action':
        # Clone action
```

### 2. Factory Functions

```python
from pynlm import (
    createDefaultConfig,  # Create default configuration
    createBrain,          # Create brain with configuration
    createSimpleWorld,    # Create simple world
    createAgentBrain,     # Create agent brain interface
)
```

### 3. Enumerations

```python
from pynlm import (
    NeuronType,      # Excitatory, Inhibitory, Modulatory, etc.
    SynapseType,     # Excitatory, Inhibitory, Modulatory, etc.
    ActionType,      # MoveForward, MoveBackward, TurnLeft, etc.
    WorldObjectType, # Empty, Resource, Hazard, Wall, etc.
)
```

### 4. Custom Exceptions

```python
from pynlm import (
    NLMError,            # Base exception for NLM errors
    ConfigurationError,   # Error in configuration
    SimulationError,      # Error during simulation
    SerializationError,   # Error during file I/O
    ActionError,          # Error in action processing
    WorldError,           # Error in world processing
    AgentError,           # Error in agent processing
    SensoryError,         # Error in sensory processing
)
```

## Enhanced API Features

### 1. Pythonic Properties

The enhanced API provides Pythonic properties for easier access to common data:

```python
# Brain properties
brain.total_neuron_count      # Total number of neurons
brain.firing_neuron_count     # Number of firing neurons
brain.total_spike_count       # Total number of spikes
brain.average_firing_rate     # Average firing rate (Hz)
brain.developmental_stage     # Current developmental stage
brain.region_count            # Number of regions
brain.initialized             # Whether brain is initialized

# AgentBrain properties
agent.curiosity_level         # Current curiosity level
agent.novelty_level           # Current novelty level
agent.prediction_error        # Current prediction error
agent.neuromodulation_level   # Current neuromodulation level

# World properties
world.width                  # World width
world.height                 # World height
world.simulation_time        # Current simulation time
world.initialized            # Whether world is configured
```

### 2. Better Error Handling

The enhanced API provides custom exceptions for better error handling:

```python
try:
    brain.initialize()
    brain.step(10)
except SimulationError as e:
    print(f"Simulation error: {e}")
except ConfigurationError as e:
    print(f"Configuration error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### 3. Configuration Validation

The enhanced config provides validation capabilities:

```python
config = EnhancedConfig()
config.set("brain.neuron_count", 1000)
config.set("world.width", 20)

# Validate configuration
errors = config.validate()
if errors:
    print(f"Configuration errors: {errors}")
else:
    print("✓ Configuration is valid")
```

### 4. Convenience Methods

The enhanced API provides convenience methods for common operations:

```python
# Batch operations
config.batch_set({
    "brain.neuron_count": 1000,
    "world.width": 20,
    "world.height": 20,
})

# Factory functions
brain = createBrain(config)
world = createSimpleWorld()
agent = createAgentBrain(brain)

# Type-safe access
vision_data = brain.getVision()  # Automatically converts to numpy
```

### 5. Performance Enhancements

The performance enhancements module provides optimized functions:

```python
from pynlm import PerformanceMonitor, optimized_simulation

# Create performance monitor
monitor = PerformanceMonitor()

# Run optimized simulation
config = EnhancedConfig()
results = optimized_simulation(config, num_steps=1000, num_threads=2)

# Get performance statistics
stats = monitor.get_all_stats()
```

### 6. Memory Pool

The memory pool provides reusable objects for better performance:

```python
from pynlm import _memory_pool

# Get objects from pool
action = _memory_pool.get_action()
vision = _memory_pool.get_vision(32, 32, 3)

# Use objects...

# Return objects to pool
_memory_pool.return_action(action)
_memory_pool.return_vision(vision)
```

## Example Usage

### Basic Simulation

```python
import pynlm
from pynlm import EnhancedConfig, EnhancedBrain

# Create configuration
config = EnhancedConfig()
config.set("brain.neuron_count", 1000)

# Create brain
brain = EnhancedBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.firing_neuron_count} neurons firing")
```

### World-Agent Simulation

```python
from pynlm import EnhancedSimpleWorld, EnhancedAgentBrain

# Create world
world = EnhancedSimpleWorld()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Create brain and agent
config = EnhancedConfig()
brain = EnhancedBrain(config)
brain.initialize()

agent = EnhancedAgentBrain(brain)
agent.initialize(world)

# Enable learning systems
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_development(True)

# Run simulation
for step in range(50):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    action = agent.processSensoryInput(percept)
    world.applyMotorCommand(action, world.simulation_time)
```

### File I/O Operations

```python
from pynlm import EnhancedConfig

# Save configuration
config = EnhancedConfig()
config.set("brain.neuron_count", 1000)
config.save_to_file("my_config.json")

# Load configuration
loaded_config = EnhancedConfig()
loaded_config.load_from_file("my_config.json")

# Validate loaded config
errors = loaded_config.validate()
if not errors:
    print("✓ Configuration is valid")
```

### Sensory Data Processing

```python
from pynlm import SensoryPercept
import numpy as np

# Create percept
percept = SensoryPercept()

# Set vision data
vision_data = np.random.rand(32, 32, 3).astype(np.float32)
percept.setVision(vision_data.tolist())

# Set audio data
audio_data = np.random.randn(44100).astype(np.float32)
percept.setAudio(audio_data.tolist())

# Get all signals
signals = percept.getAllSignals()
for signal_type, signal_data in signals.items():
    print(f"{signal_type}: {len(signal_data)} values")
```

### Action System

```python
from pynlm import Action, createActionResult

# Create actions
actions = [
    Action(pynlm.ActionType.MoveForward),
    Action(pynlm.ActionType.TurnLeft, [0.5]),
    Action(pynlm.ActionType.Interact, [1.0, 0.0, 0.0]),
]

# Create action result
result = createActionResult(reward=10.0, success=True, message="Action completed")
print(f"Action result: {result.reward} reward, success={result.success}")
```

## Performance Optimization

### 1. Batched Operations

```python
from pynlm import BatchedBrain

brain = EnhancedBrain(config)
batched_brain = BatchedBrain(brain)

# Batch multiple steps
steps = list(range(100))
batched_brain.batch_step(steps)

# Batch current injections
injections = [(neuron_id, current) for neuron_id, current in zip(neuron_ids, currents)]
batched_brain.batch_inject_current(injections)
```

### 2. Vectorized Sensory Processing

```python
from pynlm import VectorizedSensory

# Process multiple vision inputs
vision_list = [np.random.rand(32, 32, 3).astype(np.float32) for _ in range(10)]
processed_visions = VectorizedSensory.batch_process_vision(vision_list)

# Process multiple audio inputs
audio_list = [np.random.randn(1024).astype(np.float32) for _ in range(5)]
processed_audios = VectorizedSensory.batch_process_audio(audio_list)
```

### 3. Caching

```python
from pynlm import create_vision_cached, create_audio_cached, create_action_cached

# Create cached objects (faster subsequent calls)
vision1 = create_vision_cached(32, 32, 3)
vision2 = create_vision_cached(32, 32, 3)  # Returns same cached object

audio1 = create_audio_cached(44100, 1024)
audio2 = create_audio_cached(44100, 1024)  # Returns same cached object
```

## Migration Guide

### From Original API to Enhanced API

#### Original API (Low-level)

```python
# Original API
config = Config()
config.loadFromFile("config.json")

brain = Brain(config)
brain.initialize()

for step in range(100):
    brain.step(step)

neurons = brain.getTotalNeuronCount()
spikes = brain.getTotalSpikeCount()
```

#### Enhanced API (High-level)

```python
# Enhanced API
config = EnhancedConfig()
config.set("brain.neuron_count", 1000)

brain = EnhancedBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.firing_neuron_count} neurons firing")

neurons = brain.total_neuron_count  # Pythonic property
spikes = brain.total_spike_count    # Pythonic property
```

### Key Differences

| Feature | Original API | Enhanced API |
|---------|-------------|--------------|
| Configuration | Direct C++ bindings | Pythonic interface with validation |
| Error Handling | Manual try-catch | Custom exceptions |
| Properties | Methods only | Pythonic properties |
| Factory Functions | None | Convenience factory functions |
| Type Hints | None | Full type hints |
| Documentation | Basic | Comprehensive docstrings |
| Examples | Limited | Extensive examples |

## Troubleshooting

### Common Issues

#### 1. Brain not initialized

**Problem:** Simulation step fails because brain is not initialized.

**Solution:**
```python
config = EnhancedConfig()
brain = EnhancedBrain(config)
if not brain.initialize():
    print("Brain initialization failed")
    return
```

#### 2. Invalid configuration

**Problem:** Configuration validation fails.

**Solution:**
```python
config = EnhancedConfig()
errors = config.validate()
if errors:
    print(f"Configuration errors: {errors}")
    # Fix configuration issues
    config.set("brain.neuron_count", 1000)  # Example fix
```

#### 3. File I/O errors

**Problem:** File operations fail.

**Solution:**
```python
try:
    config.save_to_file("config.json")
except SerializationError as e:
    print(f"Failed to save configuration: {e}")
```

#### 4. Sensory data mismatches

**Problem:** Sensory data dimensions don't match expected values.

**Solution:**
```python
from pynlm import SensoryError

try:
    percept = SensoryPercept()
    vision_data = [[0.1, 0.2, 0.3], [0.4, 0.5, 0.6]]
    percept.setVision(vision_data)
except SensoryError as e:
    print(f"Sensory error: {e}")
```

## Support and Resources

### Documentation

- **API Reference**: This document
- **Examples**: `examples.py`, `examples_advanced.py`, `usage_examples.py`
- **Performance**: `performance_enhancements.py`
- **Tests**: `test_pynlm.py`

### Getting Help

1. **Check examples**: Run the example scripts to see how to use the API
2. **Read documentation**: Review the docstrings in the source code
3. **Use type hints**: The enhanced API provides full type hints for better IDE support
4. **Error messages**: Custom exceptions provide detailed error information

### Contributing

1. **Report issues**: Submit bug reports at [NLM GitHub repository](https://github.com/nlm)
2. **Feature requests**: Suggest new features or improvements
3. **Examples**: Add new examples to demonstrate API usage
4. **Documentation**: Improve documentation and add examples

## Version Information

- **Current version**: 0.1.0
- **Python compatibility**: 3.8+
- **API compatibility**: Backwards compatible with original API
- **Dependencies**: None (pure Python wrappers)

## License

MIT License. See LICENSE file for details.

---

*This API reference was generated from the NLM Python bindings source code.*
