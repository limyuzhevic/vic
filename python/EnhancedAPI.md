# NLM Python Enhanced API Design

This document provides a comprehensive guide for the enhanced Python bindings of the NLM (Neural Learning Machine) system.

## Overview

The enhanced pynlm Python bindings provide:

- **User-friendly wrapper classes** with Pythonic APIs
- **Comprehensive documentation** with type hints
- **Factory functions** for easy object creation
- **Advanced features** for power users
- **Python-specific utilities** and tools
- **Robust error handling** with custom exceptions
- **Performance monitoring** and profiling capabilities

## Quick Start Guide

### Basic Usage

```python
from pynlm import (
    # Core components
    Brain, Config, SimpleWorld, AgentBrain,
    Vision, Audio, InternalSignals,
    Action, WorldObject, SensoryPercept,
    
    # Factory functions
    create_config, create_brain, create_world, create_agent_brain,
    
    # Constants and utilities
    NeuronType, SynapseType, ActionType,
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID,
    
    # Monitoring and utilities
    SimulationMonitor, profiling, benchmark,
)

# Create configuration with ease
config = create_config({
    "random_seed": 42,
    "neuron_count": 1000,
    "simulation_timestep": 0.001
})

# Create brain with configuration
brain = create_brain(config)
brain.initialize()

# Create world
world = create_world({
    "width": 10, "height": 10,
    "vision_width": 5, "vision_height": 5
})

# Create agent brain
agent_brain = create_agent_brain(brain, world)
agent_brain.initialize()
```

### Simulation Loop Example

```python
import time
from pynlm import create_config, create_brain, create_world, create_agent_brain

# Initialize simulation
config = create_config({"random_seed": 42})
brain = create_brain(config)
world = create_world()
agent_brain = create_agent_brain(brain, world)

brain.initialize()
agent_brain.initialize()

# Main simulation loop
for step in range(1000):
    # Get sensory percept from world
    percept = world.get_sensory_percept()
    
    # Process input through agent brain
    action = agent_brain.process_sensory_input(percept)
    
    # Apply action in world
    reward, success = world.apply_action(action)
    
    # Get reward prediction error
    prediction_error = agent_brain.get_prediction_error()
    
    # Apply neuromodulation
    agent_brain.apply_reward_modulation(reward, prediction_error)
    
    # Update development
    agent_brain.update_development(1)

brain.reset()
world.reset()
```

## Core API Classes

### 1. Config - Configuration Management

```python
class Config:
    def __init__(self, filepath: str = None, config_dict: dict = None):
        """Initialize configuration from file or dictionary."""
    
    @classmethod
    def from_file(cls, filepath: str) -> 'Config':
        """Create configuration from file."""
    
    @classmethod  
    def from_dict(cls, config_dict: dict) -> 'Config':
        """Create configuration from dictionary."""
    
    @classmethod
    def from_defaults(cls) -> 'Config':
        """Create configuration with sensible defaults."""
    
    def get(self, key: str, default=None):
        """Get configuration value with default."""
    
    def set(self, key: str, value, data_type: type = None):
        """Set configuration value with type validation."""
    
    def load_from_file(self, filepath: str):
        """Load configuration from file."""
    
    def save_to_file(self, filepath: str):
        """Save configuration to file."""
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys."""
    
    def clear(self):
        """Clear all configuration entries."""
    
    def summary(self) -> str:
        """Get configuration summary."""
    
    def validate(self) -> bool:
        """Validate configuration values."""
    
    def __repr__(self) -> str:
        """String representation."""
```

### 2. Vision - Visual Sensory Input

```python
class Vision:
    def __init__(self, width: int = 84, height: int = 84, channels: int = 3):
        """Initialize vision input with optional resolution."""
    
    def set_data(self, data: Union[List[float], np.ndarray]):
        """Set visual data with automatic shape validation."""
    
    def get_data(self) -> np.ndarray:
        """Get visual data as numpy array."""
    
    def get_width(self) -> int:
        """Get vision width."""
    
    def get_height(self) -> int:
        """Get vision height."""
    
    def get_channels(self) -> int:
        """Get number of channels."""
    
    @property
    def shape(self) -> Tuple[int, int, int]:
        """Get data shape."""
    
    @property
    def normalized(self) -> 'Vision':
        """Get normalized copy of vision data."""
    
    def __repr__(self) -> str:
        """String representation."""
```

### 3. Audio - Audio Sensory Input

```python
class Audio:
    def __init__(self, sample_rate: int = 22050, num_samples: int = 16384):
        """Initialize audio input with sample rate and duration."""
    
    def set_data(self, data: Union[List[float], np.ndarray]):
        """Set audio data with automatic validation."""
    
    def get_data(self) -> np.ndarray:
        """Get audio data as numpy array."""
    
    def set_sample_rate(self, rate: int):
        """Set sample rate."""
    
    def get_sample_rate(self) -> int:
        """Get sample rate."""
    
    def get_num_samples(self) -> int:
        """Get number of samples."""
    
    def get_duration(self) -> float:
        """Get audio duration in seconds."""
    
    def analyze_frequency(self) -> Dict[str, float]:
        """Analyze audio frequency content."""
```

### 4. InternalSignals - Internal State Input

```python
class InternalSignals:
    def __init__(self):
        """Initialize internal signals container."""
    
    def add_signal(self, value: float):
        """Add a signal value."""
    
    def add_signals(self, values: List[float]):
        """Add multiple signal values."""
    
    def clear_signals(self):
        """Clear all signals."""
    
    def get_signals(self) -> List[float]:
        """Get all signals."""
    
    def get_mean(self) -> float:
        """Get mean of signals."""
    
    def get_std(self) -> float:
        """Get standard deviation of signals."""
    
    def get_count(self) -> int:
        """Get number of signals."""
```

### 5. Action - Motor Output

```python
class Action:
    def __init__(self, action_type: ActionType, parameters: List[float] = None):
        """Initialize action with type and optional parameters."""
    
    @classmethod
    def move_forward(cls, speed: float = 1.0) -> 'Action':
        """Create forward movement action."""
    
    @classmethod
    def turn_left(cls, angle: float = 0.5) -> 'Action':
        """Create left turn action."""
    
    @classmethod
    def look_at(cls, x: float, y: float) -> 'Action':
        """Create look-at action."""
    
    @classmethod
    def custom(cls, action_type: str, parameters: List[float] = None) -> 'Action':
        """Create custom action."""
    
    def get_type(self) -> ActionType:
        """Get action type."""
    
    def set_type(self, action_type: ActionType):
        """Set action type."""
    
    def get_parameters(self) -> List[float]:
        """Get action parameters."""
    
    def set_parameters(self, parameters: List[float]):
        """Set action parameters."""
    
    def get_name(self) -> str:
        """Get human-readable action name."""
    
    def clone(self) -> 'Action':
        """Create a copy of this action."""
    
    def __repr__(self) -> str:
        """String representation."""
    
    def to_dict(self) -> Dict:
        """Convert to dictionary."""
    
    @classmethod
    def from_dict(cls, data: Dict) -> 'Action':
        """Create from dictionary."""
```

### 6. World - Environment Interface

```python
class World:
    def __init__(self, width: int = 20, height: int = 20):
        """Initialize world with dimensions."""
    
    def configure(self, width: int, height: int, vision_width: int = 5, vision_height: int = 5):
        """Configure world dimensions and vision."""
    
    def reset(self):
        """Reset world to initial state."""
    
    def set_agent_start(self, x: float, y: float, orientation: float = 0.0):
        """Set agent starting position."""
    
    def update(self, timestep: float = 1.0):
        """Update world physics."""
    
    def apply_action(self, action: Action) -> Tuple[float, bool]:
        """Apply action to world, return (reward, success)."""
    
    def get_sensory_percept(self) -> SensoryPercept:
        """Get current sensory percept from world."""
    
    def get_agent_body(self) -> AgentBody:
        """Get current agent body state."""
    
    def add_object(self, obj: WorldObject):
        """Add object to world."""
    
    def remove_object(self, x: float, y: float):
        """Remove object at position."""
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid."""
    
    # Getters
    def get_width(self) -> int:
        """Get world width."""
    
    def get_height(self) -> int:
        """Get world height."""
    
    def get_max_energy(self) -> float:
        """Get maximum agent energy."""
    
    def set_max_energy(self, energy: float):
        """Set maximum agent energy."""
    
    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate."""
    
    def set_energy_decay_rate(self, rate: float):
        """Set energy decay rate."""
    
    def get_simulation_time(self) -> float:
        """Get simulation time."""
    
    def set_random_seed(self, seed: int):
        """Set random seed."""
    
    def get_random_seed(self) -> int:
        """Get random seed."""
```

### 7. Brain - Neural Processing Core

```python
class Brain:
    def __init__(self, config: Config = None):
        """Initialize brain with optional configuration."""
    
    @classmethod
    def create_with_config(cls, config_dict: dict) -> 'Brain':
        """Create brain from configuration dictionary."""
    
    @classmethod
    def create_default(cls) -> 'Brain':
        """Create brain with default configuration."""
    
    def initialize(self) -> bool:
        """Initialize brain with configuration."""
    
    def step(self, timestep: float = 1.0, current_time: float = None):
        """Perform simulation step."""
    
    def step_with_time(self, current_step: int, current_time: float):
        """Perform simulation step with explicit time."""
    
    def receive_sensory_input(self, input_data: SensoryInput):
        """Receive sensory input from environment."""
    
    def inject_current(self, neuron_id: NeuronId, current: float):
        """Inject current into specific neuron."""
    
    def inject_current_to_type(self, neuron_type: NeuronType, current: float):
        """Inject current into all neurons of type."""
    
    def produce_action(self) -> Action:
        """Produce motor action based on neural activity."""
    
    def reset(self):
        """Reset brain state."""
    
    def save(self, filepath: str) -> bool:
        """Save brain state to file."""
    
    def load(self, filepath: str) -> bool:
        """Load brain state from file."""
    
    # Region management
    def add_region(self, name: str = "") -> RegionId:
        """Add a new neural region."""
    
    def get_region(self, region_id: RegionId) -> 'NeuralRegion':
        """Get region by ID."""
    
    def get_region_count(self) -> int:
        """Get number of regions."""
    
    def get_region_ids(self) -> List[RegionId]:
        """Get all region IDs."""
    
    def get_total_neuron_count(self) -> int:
        """Get total neuron count."""
    
    def get_total_synapse_count(self) -> int:
        """Get total synapse count."""
    
    # Statistics
    def get_active_neuron_count(self) -> int:
        """Get count of active neurons."""
    
    def get_firing_neuron_count(self) -> int:
        """Get count of firing neurons."""
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate."""
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition ratio."""
    
    def get_total_spike_count(self) -> int:
        """Get total spike count."""
    
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get developmental stage."""
    
    def set_developmental_stage(self, stage: DevelopmentalStage):
        """Set developmental stage."""
    
    # Memory systems
    def get_working_memory(self) -> 'NeuralWorkingMemory':
        """Get working memory."""
    
    def get_episodic_memory(self) -> 'NeuralEpisodicMemory':
        """Get episodic memory."""
    
    def get_associative_memory(self) -> 'NeuralAssociativeMemory':
        """Get associative memory."""
    
    # Prediction system
    def get_prediction_system(self) -> 'PredictionSystem':
        """Get prediction system."""
    
    # Cognition systems
    def get_planner(self) -> 'NeuralPlanner':
        """Get neural planner."""
    
    def get_concept_formation(self) -> 'ConceptFormation':
        """Get concept formation."""
    
    def get_attention(self) -> 'AttentionalSelection':
        """Get attentional selection."""
    
    # Development system
    def get_development_system(self) -> 'DevelopmentSystem':
        """Get development system."""
    
    # Neuromodulation systems
    def get_dopamine(self) -> 'Dopamine':
        """Get dopamine system."""
    
    def get_curiosity(self) -> 'Curiosity':
        """Get curiosity system."""
    
    def get_novelty(self) -> 'Novelty':
        """Get novelty detector."""
    
    # Get configuration
    def get_config(self) -> Config:
        """Get configuration."""
    
    # Logging
    def log_status(self):
        """Log brain status."""
```

### 8. AgentBrain - Brain-Agent Interface

```python
class AgentBrain:
    def __init__(self, brain: Brain):
        """Initialize agent brain interface."""
    
    def initialize(self, world: World):
        """Initialize with world."""
    
    def get_sensory_input_size(self) -> int:
        """Get expected sensory input size."""
    
    def get_motor_output_size(self) -> int:
        """Get expected motor output size."""
    
    def process_sensory_input(self, percept: SensoryPercept) -> Action:
        """Process sensory percept and produce action."""
    
    def decode_motor_command(self, brain_activity: List[float]) -> Action:
        """Decode brain activity into motor command."""
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float):
        """Apply reward-based neuromodulation."""
    
    def update_development(self, timestep: float):
        """Update development system."""
    
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
    
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level."""
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
    
    def get_novelty_level(self) -> float:
        """Get novelty level."""
    
    def get_prediction_error(self) -> float:
        """Get prediction error."""
    
    def reset(self):
        """Reset agent for new episode."""
    
    def get_brain(self) -> Brain:
        """Get underlying brain."""
    
    # Enable/disable systems
    def enable_reward_modulation(self, enable: bool):
        """Enable/disable reward modulation."""
    
    def enable_structural_plasticity(self, enable: bool):
        """Enable/disable structural plasticity."""
    
    def enable_development(self, enable: bool):
        """Enable/disable development."""
    
    def enable_curiosity(self, enable: bool):
        """Enable/disable curiosity."""
    
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled."""
    
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled."""
    
    def is_development_enabled(self) -> bool:
        """Check if development is enabled."""
    
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled."""
```

## Factory Functions

### Basic Factory Functions

```python
# Create configuration
def create_config(config_dict: dict = None, config_type: str = "default") -> Config:
    """Create configuration with optional presets."""

# Create brain
def create_brain(config: Config = None, brain_type: str = "default") -> Brain:
    """Create brain with optional configuration."""

# Create world
def create_world(width: int = 20, height: int = 20, world_type: str = "simple") -> World:
    """Create world with dimensions."""

# Create agent brain
def create_agent_brain(brain: Brain, world: World = None) -> AgentBrain:
    """Create agent brain interface."""

# Create sensory inputs
def create_vision(width: int = 84, height: int = 84, channels: int = 3) -> Vision:
    """Create vision input."""

def create_audio(sample_rate: int = 22050, num_samples: int = 16384) -> Audio:
    """Create audio input."""

def create_internal_signals() -> InternalSignals:
    """Create internal signals."""

# Create actions
def create_action(action_type: ActionType, parameters: List[float] = None) -> Action:
    """Create action."""

def create_movement_action(movement_type: str, magnitude: float = 1.0) -> Action:
    """Create movement action."""

def create_look_action(x: float, y: float) -> Action:
    """Create look-at action."""
```

### Advanced Factory Functions

```python
# Preset configurations
def create_research_config() -> Config:
    """Create configuration for research."""

def create_training_config(episode_limit: int = 1000) -> Config:
    """Create configuration for training."""

def create_simulation_config(time_steps: int = 10000) -> Config:
    """Create configuration for simulation."""

# Preset brains
def create_simple_brain() -> Brain:
    """Create minimal brain for testing."""

def create_development_brain() -> Brain:
    """Create brain with development enabled."""

def create_learning_brain(learning_rate: float = 0.01) -> Brain:
    """Create brain with specific learning rate."""

# Preset worlds
def create_grid_world(width: int = 10, height: int = 10) -> World:
    """Create grid-based world."""

def create_arena_world(radius: float = 20.0) -> World:
    """Create arena-shaped world."""

def create_maze_world(seed: int = 42) -> World:
    """Create procedural maze world."""
```

## Advanced Features

### 1. Simulation Monitoring

```python
class SimulationMonitor:
    def __init__(self, brain: Brain, world: World, agent_brain: AgentBrain = None):
        """Monitor simulation performance."""
    
    def start_monitoring(self):
        """Start monitoring."""
    
    def stop_monitoring(self):
        """Stop monitoring."""
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get monitoring statistics."""
    
    def get_performance_metrics(self) -> Dict[str, float]:
        """Get performance metrics."""
    
    def get_stability_metrics(self) -> Dict[str, float]:
        """Get stability metrics."""
    
    def plot_metrics(self, output_file: str = None):
        """Plot metrics to file."""
```

### 2. Performance Profiling

```python
import time
import cProfile
import pstats

# Profile simulation
profiler = Profiler()
profiler.enable()

# Run simulation
brain.step()
world.update()

profiler.disable()
stats = pstats.Stats(profiler).sort_stats('cumulative')
stats.print_stats()

# Profile memory usage
import tracemalloc
tracemalloc.start()

# Run simulation
brain.step()
world.update()

current, peak = tracemalloc.get_traced_memory()
tracemalloc.stop()

print(f"Current memory usage: {current / 10**6:.2f} MB")
print(f"Peak memory usage: {peak / 10**6:.2f} MB")
```

### 3. Benchmarking

```python
class Benchmark:
    def __init__(self, brain: Brain, world: World, agent_brain: AgentBrain = None):
        """Benchmark simulation performance."""
    
    def benchmark_step(self, num_steps: int = 100) -> Dict[str, float]:
        """Benchmark single step performance."""
    
    def benchmark_simulation(self, num_steps: int = 1000) -> Dict[str, float]:
        """Benchmark full simulation."""
    
    def benchmark_memory(self) -> Dict[str, float]:
        """Benchmark memory usage."""
    
    def run_comparison(self, configurations: List[Config]) -> Dict[str, Any]:
        """Compare different configurations."""
    
    def generate_report(self, output_file: str = None):
        """Generate benchmark report."""
```

### 4. Visualization Tools

```python
class BrainVisualizer:
    def __init__(self, brain: Brain):
        """Initialize brain visualizer."""
    
    def plot_neural_activity(self, output_file: str = None):
        """Plot neural activity over time."""
    
    def plot_spike_raster(self, output_file: str = None):
        """Plot spike raster plot."""
    
    def plot_region_connections(self, output_file: str = None):
        """Plot region connectivity."""
    
    def plot_development(self, output_file: str = None):
        """Plot developmental progression."""

class WorldVisualizer:
    def __init__(self, world: World):
        """Initialize world visualizer."""
    
    def plot_world(self, output_file: str = None):
        """Plot world state."""
    
    def animate_simulation(self, num_steps: int, output_file: str = None):
        """Animate simulation."""
```

## Python-Specific Utilities

### 1. Type Conversion Utilities

```python
class TypeConverter:
    @staticmethod
    def to_numpy(array_like) -> np.ndarray:
        """Convert to numpy array."""
    
    @staticmethod
    def to_list(array_like) -> List:
        """Convert to Python list."""
    
    @staticmethod
    def to_tensor(data) -> torch.Tensor:
        """Convert to PyTorch tensor if available."""
    
    @staticmethod
    def from_dict(data: dict, data_type: type) -> Any:
        """Create object from dictionary."""
    
    @staticmethod
    def to_dict(obj) -> dict:
        """Convert object to dictionary."""
```

### 2. Serialization Utilities

```python
class Serializer:
    def __init__(self, filepath: str):
        """Initialize serializer."""
    
    def save_config(self, config: Config):
        """Save configuration."""
    
    def load_config(self) -> Config:
        """Load configuration."""
    
    def save_brain(self, brain: Brain):
        """Save brain state."""
    
    def load_brain(self) -> Brain:
        """Load brain state."""
    
    def export_json(self, obj) -> str:
        """Export object to JSON."""
    
    def import_json(self, json_str: str, obj_type: type) -> Any:
        """Import object from JSON."""
```

### 3. Experiment Management

```python
class ExperimentManager:
    def __init__(self, experiment_id: str = None):
        """Initialize experiment manager."""
    
    def start_experiment(self, config: Config):
        """Start new experiment."""
    
    def log_step(self, step: int, data: Dict[str, Any]):
        """Log step data."""
    
    def save_checkpoint(self, brain: Brain, world: World, step: int):
        """Save experiment checkpoint."""
    
    def load_checkpoint(self, brain: Brain, world: World) -> int:
        """Load experiment checkpoint."""
    
    def get_results(self) -> Dict[str, Any]:
        """Get experiment results."""
    
    def generate_report(self, output_file: str = None):
        """Generate experiment report."""
```

## Error Handling

### 1. Custom Exceptions

```python
class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigError(NLMError):
    """Exception for configuration errors."""
    pass

class BrainError(NLMError):
    """Exception for brain errors."""
    pass

class WorldError(NLMError):
    """Exception for world errors."""
    pass

class SensorError(NLMError):
    """Exception for sensor errors."""
    pass

class ActionError(NLMError):
    """Exception for action errors."""
    pass

# Usage example
try:
    brain.initialize()
except BrainError as e:
    print(f"Brain initialization failed: {e}")
except ConfigError as e:
    print(f"Configuration error: {e}")
```

### 2. Validation Utilities

```python
class Validator:
    @staticmethod
    def validate_config(config: Config) -> List[str]:
        """Validate configuration, return list of errors."""
    
    @staticmethod
    def validate_brain(brain: Brain) -> List[str]:
        """Validate brain state."""
    
    @staticmethod
    def validate_world(world: World) -> List[str]:
        """Validate world state."""
    
    @staticmethod
    def validate_action(action: Action) -> List[str]:
        """Validate action."""
    
    @staticmethod
    def validate_data(data, expected_type: type, expected_shape: Tuple = None):
        """Validate data type and shape."""
```

## Documentation Strategy

### 1. API Documentation

This API follows NumPy-style documentation:

```python
def method_name(self, param1, param2):
    """
    Describe method purpose.

    Parameters
    ----------
    param1 : type
        Description of param1.
    param2 : type
        Description of param2.

    Returns
    -------
    type
        Description of return value.

    Raises
    ------
    NLMError
        Description of errors raised.

    See Also
    --------
    Related methods.

    Notes
    -----
    Additional notes.

    Examples
    --------
    >>> example usage
    """
```

### 2. Type Hints

All functions include comprehensive type hints for better IDE support and static analysis:

```python
from typing import List, Dict, Tuple, Optional, Union
import numpy as np

def process_data(
    input_data: Union[np.ndarray, List[float]],
    expected_shape: Optional[Tuple[int, ...]] = None,
    normalize: bool = False
) -> Tuple[np.ndarray, Dict[str, float]]:
    """Process input data with validation."""
    ...
```

### 3. Jupyter Notebooks

Create example notebooks for:
- Basic usage and getting started
- Advanced simulation techniques
- Custom component development
- Performance optimization
- Experiment design

### 4. API Reference

Generate API documentation using Sphinx or similar tools:

```python
# conf.py
sphinx_extensions = ['sphinx.ext.autodoc', 'sphinx.ext.napoleon']

# index.rst
.. toctree::
   api/config
   api/brain
   api/world
   api/sensors
   api/actions
   api/advanced
```

## Migration Guide

### From Old API to New API

#### Config

Old:
```python
config = pybind11.cast("Config")
config.loadFromFile("config.cfg")
config.get("random_seed")
```

New:
```python
config = create_config({"random_seed": 42})
config.load_from_file("config.cfg")
config.get("random_seed")
```

#### Brain

Old:
```python
brain = pybind11.cast("Brain", config)
brain.initialize()
brain.step(current_step)
```

New:
```python
brain = create_brain(config)
brain.initialize()
brain.step()
```

#### Action

Old:
```python
action = Action()
action.setType(ActionType.MoveForward)
action.getParameters()
```

New:
```python
action = create_action(ActionType.MoveForward)
action.get_type()
action.get_parameters()
```

## Performance Considerations

### 1. Memory Management

```python
# Use context managers for efficient resource management
with SimulationSession(config) as session:
    brain = session.create_brain()
    world = session.create_world()
    # Automatic cleanup on exit
```

### 2. Vectorized Operations

```python
# Use numpy arrays for batch operations
vision_data = np.array([0.1, 0.2, 0.3, ...]).reshape(84, 84, 3)
world.set_sensory_input(Vision.from_numpy(vision_data))
```

### 3. Caching

```python
# Cache frequently used objects
cached_actions = {
    "move_forward": Action.move_forward(1.0),
    "turn_left": Action.turn_left(0.5),
    "look_at": Action.look_at(0.0, 0.0)
}
```

## Testing and Validation

### 1. Unit Tests

```python
def test_config_creation():
    config = create_config({"random_seed": 42})
    assert config.get("random_seed") == 42

def test_brain_initialization():
    brain = create_brain()
    assert brain.initialize() is True

def test_action_creation():
    action = create_action(ActionType.MoveForward)
    assert action.get_type() == ActionType.MoveForward
```

### 2. Integration Tests

```python
def test_simulation_loop():
    config = create_config({"random_seed": 42})
    brain = create_brain(config)
    world = create_world()
    
    assert brain.initialize()
    assert world.reset()
    
    # Run simulation
    for step in range(100):
        percept = world.get_sensory_percept()
        action = brain.process_sensory_input(percept)
        brain.apply_action(action)
    
    assert brain.reset()
```

## Conclusion

The enhanced pynlm Python bindings provide a comprehensive, user-friendly interface to the NLM neural simulation framework. The API is designed for both beginners and advanced users, with clear documentation, robust error handling, and powerful tools for simulation monitoring and analysis.