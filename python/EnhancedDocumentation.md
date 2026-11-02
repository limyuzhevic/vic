# Enhanced NLM Python Bindings - Documentation

## Overview

This documentation covers the enhanced NLM (Neural Learning Machine) Python bindings with significant improvements over the original implementation.

### Key Enhancements

1. **Pythonic API Design**: All classes now follow Python conventions with snake_case method names, context managers, and operator overloads
2. **Better Error Handling**: Custom exception hierarchy for more robust error management
3. **Configuration Builder**: Fluent API for easier configuration setup
4. **Memory Management**: Improved pybind11 usage with consistent reference policies
5. **Iterator Support**: Iteration over Config and InternalSignals objects
6. **Utility Functions**: Common operations like normalization and array formatting
7. **Type Hints**: Better documentation and type support for enhanced IDE experience

## Quick Start Examples

### Example 1: Simplest Possible Usage

```python
import pynlm

# Create a virtual brain with default configuration
config, brain, world, agent_brain = pynlm.quick_start()

# Use context managers for automatic resource cleanup
with brain as brain_context:
    brain_context.initialize()
    brain_context.step(0)

print(f"Brain created with {brain_context.getTotalNeuronCount()} neurons")
```

### Example 2: Custom Configuration with ConfigBuilder

```python
import pynlm

# Create custom configuration using fluent API
config = (pynlm.ConfigBuilder()
    .set_neural_parameters(
        region_count=10,
        neurons_per_region=500,
        connection_density=0.05,
        learning_rate=0.001,
        decay_rate=0.99
    )
    .set_world_parameters(
        width=200,
        height=200,
        vision_width=30,
        vision_height=30,
        max_energy=150.0
    )
    .set_agent_parameters(
        max_energy=150.0,
        health=100.0,
        energy_decay_rate=0.05
    )
    .create())

brain = pynlm.create_brain(config)
brain.initialize()
print(f"Custom brain initialized with {brain.getTotalNeuronCount()} neurons")
```

### Example 3: Complete Agent Simulation

```python
import pynlm

def run_complete_simulation(steps=1000):
    # Initialize all components
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    world = pynlm.create_simple_world()
    agent = pynlm.create_agent_brain(brain)
    
    # Configure world
    world.configure(width=50, height=50, vision_width=15, vision_height=15)
    world.reset()
    world.setAgentStart(25.0, 25.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable learning features
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Run simulation with context managers
    with world as world_context:
        with brain as brain_context:
            with agent as agent_context:
                for step in range(steps):
                    # Update world
                    world.update(0.1)
                    
                    # Get perception
                    percept = world.get_sensory_percept()
                    
                    # Process sensory input
                    agent_context.process_sensory_input(percept)
                    
                    # Brain thinks
                    brain_context.step(step)
                    
                    # Get action
                    action = agent_context.decode_motor_command()
                    
                    # Apply action
                    world.apply_motor_command(action, world_context.get_simulation_time())
                    
                    # Print progress
                    if step % 100 == 0:
                        print(f"Step {step}:")
                        print(f"  Neurons firing: {brain_context.getFiringNeuronCount()}")
                        print(f"  Curiosity level: {agent_context.getCuriosityLevel():.3f}")
                        print(f"  Development stage: {brain_context.getDevelopmentalStage()}")
    
    return brain, world, agent

# Run the simulation
simulation_results = run_complete_simulation(1000)
print("Simulation completed successfully!")
```

### Example 4: Advanced Usage with Vision Processing

```python
import pynlm

# Create a vision system
vision = pynlm.Vision(width=64, height=64, channels=3)

# Generate test image data
import random
image_data = [random.uniform(0, 1) for _ in range(64 * 64 * 3)]
vision.set_data(image_data)

# Apply various image processing operations
print("Original vision data:", vision.get_data()[:5])  # First 5 values

# Normalize the image
vision.normalize(0.0, 1.0)
print("After normalization:", vision.get_data()[:5])

# Scale the image
vision.scale(2.0)
print("After scaling by 2:", vision.get_data()[:5])

# Create multiple vision objects and add them
vision2 = pynlm.Vision(width=64, height=64, channels=3)
vision2.set_data([x * 0.5 for x in image_data])

# Add two vision objects
combined_vision = vision + vision2
print("Combined vision size:", len(combined_vision.get_data()))
```

### Example 5: Configuration Iteration and Manipulation

```python
import pynlm

# Create a configuration
config = pynlm.create_default_config()

# Add custom settings
config.set("custom.agent_name", "TestAgent")
config.set("custom.max_steps", 1000)
config.set("custom.learning_rate", 0.01)

# Use dictionary-style access
print(f"Agent name: {config['custom.agent_name']}")
print(f"Max steps: {config['custom.max_steps']}")

# Use has() and get() methods
if config.has("custom.agent_name"):
    print("Configuration has agent name")

# Iterate over configuration keys
print("Configuration keys:")
for key in config:
    value = config.get(key)
    print(f"  {key}: {value}")

# Remove a key
config.remove("custom.agent_name")
print("After removing agent_name, keys:", list(config))
```

## API Reference

### Core Classes

#### Config
```python
class Config:
    def __init__():
        # Create empty configuration
    
    def load_from_file(filepath: str) -> bool:
        # Load configuration from file
    
    def load_from_args(args: List[str]) -> bool:
        # Load configuration from command line arguments
    
    def save_to_file(filepath: str) -> bool:
        # Save configuration to file
    
    def has(key: str) -> bool:
        # Check if key exists
    
    def get_keys() -> List[str]:
        # Get all configuration keys
    
    def clear():
        # Clear all configuration entries
    
    def summary() -> str:
        # Get configuration summary
    
    def get(key: str) -> Optional[str]:
        # Get configuration value
    
    def set(key: str, value: str):
        # Set configuration value
    
    def validate():
        # Validate configuration
    
    def __contains__(key: str) -> bool:
        # Dictionary-like membership test
    
    def __getitem__(key: str) -> Any:
        # Dictionary-like item access
    
    def __setitem__(key: str, value: Any):
        # Dictionary-like item assignment
    
    def __len__() -> int:
        # Number of configuration entries
    
    def __iter__():
        # Iterator over configuration keys
    
    def __repr__() -> str:
        # String representation
```

#### ConfigBuilder
```python
class ConfigBuilder:
    def __init__():
        # Create a new builder
    
    def set(key: str, value: str) -> 'ConfigBuilder':
        # Set a configuration key-value pair
    
    def set_int(key: str, value: int) -> 'ConfigBuilder':
        # Set a configuration key-value pair as int
    
    def set_float(key: str, value: float) -> 'ConfigBuilder':
        # Set a configuration key-value pair as float
    
    def set_bool(key: str, value: bool) -> 'ConfigBuilder':
        # Set a configuration key-value pair as bool
    
    def set_neural_parameters(...) -> 'ConfigBuilder':
        # Set neural network parameters
    
    def set_world_parameters(...) -> 'ConfigBuilder':
        # Set world parameters
    
    def set_agent_parameters(...) -> 'ConfigBuilder':
        # Set agent parameters
    
    def validate() -> 'ConfigBuilder':
        # Validate the configuration
    
    def build() -> Config:
        # Build and return the configuration
    
    def create() -> Config:
        # Build and return the configuration (alias)
```

#### Brain
```python
class Brain:
    def __init__(config: Config):
        # Create brain with configuration
    
    def initialize():
        # Initialize the brain
    
    def step(step: int, time: float = None):
        # Perform a simulation step
    
    def receive_sensory_input(input: SensoryInput):
        # Inject sensory input into the brain
    
    def inject_current(neuron: NeuronId, current: float):
        # Inject current into a specific neuron
    
    def inject_current_to_neurons(neuron_type: NeuronType, current: float):
        # Inject current into all neurons of a specific type
    
    def produce_action() -> Action:
        # Get motor action based on neural activity
    
    def reset():
        # Reset brain state
    
    def save(filepath: str):
        # Save brain state to file
    
    def load(filepath: str):
        # Load brain state from file
    
    def add_region(name: str = "") -> RegionId:
        # Add a new neural region
    
    def get_region(region_id: RegionId) -> NeuralRegion:
        # Get a region by ID
    
    def get_region_count() -> int:
        # Get the number of regions
    
    def get_region_ids() -> List[RegionId]:
        # Get all region IDs
    
    def get_regions() -> List[NeuralRegion]:
        # Get all regions
    
    def get_total_neuron_count() -> int:
        # Get total neuron count
    
    def get_total_synapse_count() -> int:
        # Get total synapse count
    
    def get_active_neuron_count() -> int:
        # Get count of active neurons
    
    def get_firing_neuron_count() -> int:
        # Get count of currently firing neurons
    
    def get_average_firing_rate() -> float:
        # Get average firing rate
    
    def get_excitation_inhibition_ratio() -> float:
        # Get excitation/inhibition balance
    
    def get_total_spike_count() -> int:
        # Get total spike count
    
    def get_developmental_stage() -> DevelopmentalStage:
        # Get current developmental stage
    
    def set_developmental_stage(stage: DevelopmentalStage):
        # Set developmental stage
    
    def get_config() -> Config:
        # Get the configuration
    
    def log_status():
        # Log brain status
    
    # Context manager support
    def __enter__():
        # Context manager entry
    
    def __exit__():
        # Context manager exit
```

#### Vision
```python
class Vision(SensoryInput):
    def __init__(width: int = None, height: int = None, channels: int = 3):
        # Create vision object
    
    def set_data(data: List[float]):
        # Set vision data
    
    def get_data() -> List[float]:
        # Get vision data
    
    def get_width() -> int:
        # Get image width
    
    def get_height() -> int:
        # Get image height
    
    def get_channels() -> int:
        # Get number of channels
    
    def add_data(data: List[float]):
        # Add data to existing vision data
    
    def scale(factor: float):
        # Scale all vision data
    
    def normalize(min: float = 0.0, max: float = 1.0):
        # Normalize vision data
    
    # Operator overloads
    def __add__(other: Vision) -> Vision:
        # Add two Vision objects
    
    def __mul__(scalar: float) -> Vision:
        # Multiply Vision by scalar
```

#### Audio
```python
class Audio(SensoryInput):
    def __init__(sample_rate: int = None, num_samples: int = None):
        # Create audio object
    
    def set_data(data: List[float]):
        # Set audio data
    
    def get_data() -> List[float]:
        # Get audio data
    
    def set_sample_rate(sample_rate: int):
        # Set sample rate
    
    def get_sample_rate() -> int:
        # Get sample rate
    
    def get_num_samples() -> int:
        # Get number of samples
    
    def add_silence(duration: float):
        # Add silence to audio
    
    def normalize(min: float = -1.0, max: float = 1.0):
        # Normalize audio data
```

#### InternalSignals
```python
class InternalSignals(SensoryInput):
    def __init__():
        # Create internal signals
    
    def add_signal(value: float):
        # Add a signal value
    
    def get_signals() -> List[float]:
        # Get all signal values
    
    def get_signal_count() -> int:
        # Get number of signals
    
    def clear_signals():
        # Clear all signals
    
    def get_mean() -> float:
        # Get mean signal value
    
    def get_stddev() -> float:
        # Get standard deviation
    
    # Iterator support
    def __len__() -> int:
        # Number of signals
    
    def __iter__():
        # Iterator over signals
```

#### Action
```python
class Action:
    def __init__(action_type: ActionType = None, parameters: List[float] = None):
        # Create action
    
    def get_type() -> ActionType:
        # Get action type
    
    def set_type(action_type: ActionType):
        # Set action type
    
    def get_parameters() -> List[float]:
        # Get action parameters
    
    def set_parameters(parameters: List[float]):
        # Set action parameters
    
    def get_name() -> str:
        # Get action name
    
    def clone() -> Action:
        # Clone action
    
    def add_parameter(value: float):
        # Add a parameter
    
    def multiply_parameters(factor: float):
        # Multiply all parameters
    
    def normalize_parameters(min: float = 0.0, max: float = 1.0):
        # Normalize parameter values
    
    def is_valid() -> bool:
        # Check if action is valid
    
    # Context manager support
    def __enter__():
        # Context manager entry
    
    def __exit__():
        # Context manager exit
```

#### WorldObject
```python
class WorldObject:
    def __init__(x: float = 0.0, y: float = 0.0, type: WorldObjectType = WorldObjectType.Empty,
                 value: float = 0.0, radius: float = 0.5):
        # Create world object
    
    # Position properties
    x: float
    y: float
    radius: float
    type: WorldObjectType
    value: float
    active: bool
    
    def distance_to(other: WorldObject) -> float:
        # Calculate distance to another object
    
    def is_near(other: WorldObject, threshold: float) -> bool:
        # Check if near another object
    
    def move_to(new_x: float, new_y: float):
        # Move object to new position
    
    def move(dx: float, dy: float):
        # Move object by offset
    
    def copy() -> WorldObject:
        # Copy world object
```

#### AgentBody
```python
class AgentBody:
    def __init__():
        # Create agent body
    
    # State properties
    x: float
    y: float
    orientation: float
    velocity_x: float
    velocity_y: float
    angular_velocity: float
    energy: float
    health: float
    age: float
    is_moving: bool
    is_turning: bool
    last_action_time: float
    
    def move_to(x: float, y: float):
        # Move agent to position
    
    def rotate_to(orientation: float):
        # Rotate agent to orientation
    
    def set_velocity(velocity_x: float, velocity_y: float):
        # Set agent velocity
    
    def update_position(dt: float):
        # Update position based on velocity
    
    def damage(amount: float):
        # Damage agent
    
    def heal(amount: float):
        # Heal agent
    
    def reset():
        # Reset agent body state
    
    # Context manager support
    def __enter__():
        # Context manager entry
    
    def __exit__():
        # Context manager exit
```

### Module-Level Functions

#### Core Creation Functions
```python
# Create a default configuration
config = pynlm.create_default_config()

# Create a brain with configuration
brain = pynlm.create_brain(config)

# Create a simple world
world = pynlm.create_simple_world()

# Create an agent brain interface
agent = pynlm.create_agent_brain(brain)
```

#### Utility Functions
```python
# Quick start helper
config, brain, world, agent = pynlm.quick_start()

# Version information
version = pynlm.version()

# System information
system_info = pynlm.get_system_info()

# Current time
current_time = pynlm.current_time()

# Array operations
normalized_data = pynlm.normalize(data, min=0.0, max=1.0)
array_string = pynlm.array_to_string(data)

# Tuple creation
component_tuple = pynlm.make_tuple(config, brain, world, agent)
```

#### Constants
```python
# Invalid ID constants
INVALID_NEURON_ID = pynlm.INVALID_NEURON_ID
INVALID_SYNAPSE_ID = pynlm.INVALID_SYNAPSE_ID
INVALID_REGION_ID = pynlm.INVALID_REGION_ID
INVALID_POPULATION_ID = pynlm.INVALID_POPULATION_ID
```

### Exception Classes

#### NLMException
Base exception class for all NLM errors.

#### ConfigError
Configuration-specific errors.

#### InitializationError
Brain initialization errors.

#### ResourceError
Resource management errors.

#### ValidationError
Configuration validation errors.

## Migration Guide

### From Original to Enhanced API

The enhanced bindings maintain full backward compatibility. Here's what changed:

#### Method Naming
- Original: `getType()`, `getData()`, `getWidth()`
- Enhanced: `get_type()`, `get_data()`, `get_width()`

#### Configuration Access
- Original: `config.has("key")`, `config.get("key")`
- Enhanced: `config.has("key")`, `config["key"]` (dictionary-style)

#### Context Management
- All major classes now support context managers:
  ```python
  # Before
  brain.initialize()
  brain.step(0)
  
  # After (with automatic cleanup)
  with brain:
      brain.initialize()
      brain.step(0)
  ```

#### Configuration Builder
- Original: Manual configuration setting
- Enhanced: Fluent API with ConfigBuilder

#### Vision Operations
- Original: Limited vision operations
- Enhanced: Operator overloads, image processing

## Performance Considerations

### Memory Management
- Context managers ensure proper resource cleanup
- Consistent use of reference_internal policies
- Shared_ptr for shared ownership

### Configuration Validation
- Early validation prevents invalid configurations
- ConfigBuilder provides validation during construction

### Brain Simulation
- Context managers ensure proper brain reset
- Efficient memory management for large simulations

## Testing

### Basic Tests
```python
import pynlm

# Test basic creation
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()
brain.step(0)

# Test context managers
with brain:
    brain.step(1)

# Test configuration
config.set("test.key", "test_value")
assert config.has("test.key")

# Test utility functions
data = [1.0, 2.0, 3.0]
norm_data = pynlm.normalize(data)
```

### Advanced Tests
```python
import pynlm

# Test ConfigBuilder
config = (pynlm.ConfigBuilder()
    .set_neural_parameters(region_count=5, neurons_per_region=100)
    .set_world_parameters(width=100, height=100)
    .create())

# Test Vision operations
vision = pynlm.Vision(64, 64, 3)
vision.set_data(list(range(64*64*3)))
vision.normalize(0.0, 1.0)
vision.scale(0.5)

# Test operator overloads
vision2 = pynlm.Vision(64, 64, 3)
vision2.set_data([x * 0.5 for x in range(64*64*3)])
combined = vision + vision2

# Test agent simulation
def run_advanced_test():
    config, brain, world, agent = pynlm.quick_start()
    
    agent.initialize(world)
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    with world, brain, agent:
        for step in range(100):
            world.update(0.1)
            agent.process_sensory_input(world.get_sensory_percept())
            brain.step(step)
            world.apply_motor_command(agent.decode_motor_command(), world.get_simulation_time())
    
    return True

assert run_advanced_test()
```

## Troubleshooting

### Common Issues

#### "ImportError: No module named 'pynlm'"
```bash
pip install --force-reinstall .
```

#### "Brain not responding or stuck"
- Ensure `brain.initialize()` is called before `brain.step()`
- Check that configuration has required parameters
- Use context managers for automatic cleanup

#### "Configuration validation errors"
- Use `ConfigBuilder` with validation
- Check required parameters: `neural.region_count`, `world.width`, `world.height`
- Set parameters before validation

#### "Memory issues or segmentation faults"
- Use context managers for resource cleanup
- Ensure proper configuration setup
- Monitor system resources during large simulations

#### "Vision processing errors"
- Ensure vision data dimensions match
- Use proper normalization before processing
- Check that images have same dimensions for addition

### Debugging Tips

1. **Add logging**:
```python
import logging
logging.basicConfig(level=logging.DEBUG)
```

2. **Use quick start**:
```python
config, brain, world, agent = pynlm.quick_start()
```

3. **Check configuration**:
```python
print(brain.get_config().summary())
```

4. **Monitor brain state**:
```python
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing: {brain.getFiringNeuronCount()}")
```

## Conclusion

The enhanced NLM Python bindings provide a significantly more Pythonic interface while maintaining full backward compatibility and C++ performance. Key improvements include:

- **Context managers** for automatic resource cleanup
- **ConfigBuilder** for easier configuration setup
- **Operator overloads** for intuitive operations
- **Custom exceptions** for better error handling
- **Iterator support** for Pythonic iteration
- **Utility functions** for common operations
- **Type hints and documentation** for better IDE support

These enhancements make the NLM Python bindings more accessible to Python developers while preserving the powerful neural simulation capabilities of the C++ backend.