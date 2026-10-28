# Enhanced NLM Python Bindings

## Overview

This document describes the enhanced Python bindings for the Neural Learning Machine (NLM) framework. These bindings provide comprehensive type hints, Pythonic APIs, and advanced features for working with neural simulation systems.

## Key Features

### 1. Type Hints and Type Safety

The enhanced bindings include comprehensive type hints for better IDE support and static type checking:

```python
import pynlm_enhanced as nlm
from pynlm_enhanced import Vision, Audio, Action, ActionType

# Type hints are automatically provided for all parameters
config: nlm.Config = nlm.create_default_config()
brain: nlm.Brain = nlm.create_brain(config)

# All return types are properly annotated
vision: Vision = Vision(640, 480, 3)
action: Action = Action(ActionType.MoveForward)
```

### 2. Pythonic APIs

The enhanced bindings leverage Python idioms for a more natural programming experience:

#### Dictionary-like Access
```python
config = nlm.create_default_config()

# Get values using dictionary syntax
neuron_count = config["neuron_count"]
config["simulation_timestep"] = 0.001
config["connection_probability"] = 0.2

# Check if key exists
if config.has("custom_param"):
    value = config["custom_param"]
```

#### Context Managers
```python
# Use context managers for resource management
with nlm.BrainContext(config) as brain:
    brain.initialize()
    brain.simulate(100)
    brain.save("brain_state.pkl")
```

#### Enhanced Collection Methods
```python
# Get multiple values at once
stats = brain.get_neural_activity()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")

# Get regions summary
regions = brain.get_regions_summary()
for region in regions:
    print(f"Region {region['id']}: {region['neuron_count']} neurons")
```

### 3. Enhanced Error Handling

Better error messages and exception types:

```python
try:
    brain = nlm.create_brain(config)
    brain.initialize()
except nlm.ConfigError as e:
    print(f"Configuration error: {e}")
except nlm.BrainError as e:
    print(f"Brain initialization error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### 4. Performance Monitoring

Built-in performance monitoring and profiling tools:

```python
# Get system information
system_info = nlm.get_system_info()
print(f"Platform: {system_info['platform']}")
print(f"Architecture: {system_info['architecture']}")

# Get memory usage
memory = nlm.get_memory_usage()
print(f"Memory usage: {memory['used_mb']:.1f} MB / {memory['available_mb']:.1f} MB")

# Get performance statistics
performance = nlm.get_performance_stats()
print(f"CPU usage: {performance['cpu_usage']}%")
print(f"Memory usage: {performance['memory_usage']}%")

# Benchmark functions
benchmark_result = nlm.benchmark(lambda: brain.step(0, 0.001), 1000)
print(f"Benchmark completed in {benchmark_result:.3f} seconds")
```

### 5. Serialization and Deserialization

Advanced serialization utilities:

```python
import tempfile
import os

# Save brain state
with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as f:
    brain_path = f.name

brain.save(brain_path)
print(f"Brain state saved to {brain_path}")

# Load brain state
new_brain = nlm.create_brain(config)
new_brain.load(brain_path)

print("Brain state loaded successfully")

# Clean up
os.unlink(brain_path)
```

### 6. Visualization and Analysis

Built-in visualization and analysis functions:

```python
# Create neural network visualization
network_plot = nlm.create_neural_network_plot(brain)

# Create spike raster plot
spike_raster = nlm.plot_spike_raster(brain, window_size=100)

# Analyze spiking patterns
analysis = nlm.analyze_spiking_patterns(brain)
print(f"Total spikes: {analysis['total_spikes']}")
print(f"Average firing rate: {analysis['avg_firing_rate']} Hz")
print(f"Excitatory/inhibitory ratio: {analysis['excitation_inhibition_ratio']}")

# Create activity heatmap
heatmap = nlm.create_activity_heatmap(brain)
```

## Module Reference

### Core Classes

#### Config
Enhanced configuration management with type safety and validation.

#### Vision
Visual sensory input with enhanced image processing capabilities.

#### Audio
Audio sensory input with signal processing and analysis.

#### InternalSignals
Internal neural signals and representations.

#### Action
Enhanced action representation with parameter management.

#### SensoryPercept
Combined sensory input from all modalities.

#### WorldObject
Objects in the simulation environment.

#### AgentBody
Agent physical state and properties.

#### ActionResult
Results from actions executed in the world.

#### Brain
Enhanced brain class with advanced neural computation features.

#### AgentBrain
Agent-brain interface for interaction with simulation worlds.

#### SimpleWorld
Enhanced 2D world for NLM simulation.

### Utility Functions

#### Performance Monitoring
- `get_system_info()`: System information
- `get_memory_usage()`: Memory statistics
- `get_performance_stats()`: Performance metrics
- `benchmark()`: Function benchmarking
- `profile_function()`: Function profiling

#### Serialization
- `save_brain_state()`: Save brain state
- `load_brain_state()`: Load brain state
- `serialize_brain()`: Serialize brain to dictionary
- `deserialize_brain()`: Deserialize brain from dictionary

#### Visualization
- `create_neural_network_plot()`: Create network visualization
- `plot_spike_raster()`: Create spike raster plots
- `analyze_spiking_patterns()`: Analyze spike patterns
- `create_activity_heatmap()`: Create activity heatmaps

#### Factory Functions
- `create_default_config()`: Create default configuration
- `create_brain()`: Create brain with configuration
- `create_simple_world()`: Create simple world
- `create_agent_brain()`: Create agent brain interface
- `create_vision()`: Create vision sensory input
- `create_audio()`: Create audio sensory input
- `create_internal_signals()`: Create internal signals

## Advanced Usage Examples

### 1. Using Type Hints with IDE Support

```python
# With type hints, IDEs can provide autocompletion and type checking
import pynlm_enhanced as nlm
from pynlm_enhanced import Vision, Action, ActionType

# IDE will suggest Vision constructor parameters
vision: Vision = Vision(width=640, height=480, channels=3)

# Type checking for actions
action: Action = Action(ActionType.MoveForward)
action.add_parameter(1.0)  # Parameter type is float
```

### 2. Configuration Management with Validation

```python
def create_simulation_config(neuron_count: int, timestep: float) -> nlm.Config:
    """Create a configuration with validation."""
    config = nlm.create_default_config()
    
    # Type-safe configuration
    config.set("neuron_count", neuron_count)
    config.set("simulation_timestep", timestep)
    
    # Validate configuration
    if not config.validate_all():
        raise nlm.ConfigError("Invalid configuration")
    
    return config

# Usage with type checking
config = create_simulation_config(2000, 0.0001)
```

### 3. Advanced Sensory Processing

```python
def process_vision_input(vision_data: list, width: int, height: int) -> Vision:
    """Process vision input with validation."""
    if len(vision_data) != width * height * 3:
        raise ValueError("Vision data size mismatch")
    
    vision = Vision(width, height, 3)
    vision.set_data(vision_data)
    
    # Normalize pixel values
    vision.normalize()
    
    return vision

# Process and use vision
vision_data = [0.5] * (640 * 480 * 3)
vision = process_vision_input(vision_data, 640, 480)
```

### 4. Action Sequence Management

```python
class ActionSequence:
    """Manage sequences of actions with validation."""
    
    def __init__(self, brain: nlm.Brain):
        self.brain = brain
        self.actions: list[Action] = []
    
    def add_action(self, action_type: ActionType, **parameters) -> None:
        """Add an action with parameters."""
        action = Action(action_type)
        
        # Add parameters based on action type
        if action_type == ActionType.MoveForward:
            action.add_parameter(parameters.get('speed', 1.0))
        elif action_type == ActionType.TurnLeft:
            action.add_parameter(parameters.get('angle', 45.0))
        elif action_type == ActionType.Look:
            action.add_parameter(parameters.get('x', 0.0))
            action.add_parameter(parameters.get('y', 0.0))
        
        self.actions.append(action)
    
    def execute_sequence(self) -> list[ActionResult]:
        """Execute the action sequence."""
        results = []
        
        for action in self.actions:
            # In practice, you would implement action execution
            # This is a simplified example
            result = ActionResult(1.0, True, f"Executed {action.get_name()}")
            results.append(result)
        
        return results

# Use action sequence
agent = ActionSequence(brain)
agent.add_action(ActionType.MoveForward, speed=2.0)
agent.add_action(ActionType.TurnLeft, angle=90.0)
agent.add_action(ActionType.Look, x=100.0, y=100.0)

results = agent.execute_sequence()
```

### 5. Performance Monitoring and Optimization

```python
class PerformanceMonitor:
    """Monitor and optimize performance."""
    
    def __init__(self, brain: nlm.Brain):
        self.brain = brain
        self.benchmarks: dict = {}
    
    def benchmark_simulation(self, steps: int, iterations: int = 5) -> dict:
        """Benchmark simulation performance."""
        times = []
        
        for _ in range(iterations):
            start_time = time.time()
            for step in range(steps):
                self.brain.step(step, step * 0.001)
            end_time = time.time()
            times.append(end_time - start_time)
        
        result = {
            'mean_time': statistics.mean(times),
            'median_time': statistics.median(times),
            'min_time': min(times),
            'max_time': max(times),
            'std_dev': statistics.stdev(times) if len(times) > 1 else 0.0,
            'steps_per_second': steps / statistics.mean(times)
        }
        
        self.benchmarks[f'simulation_{steps}steps'] = result
        return result
    
    def get_performance_report(self) -> dict:
        """Get performance report."""
        return {
            'benchmarks': self.benchmarks,
            'system_info': nlm.get_system_info(),
            'memory_usage': nlm.get_memory_usage(),
            'performance_stats': nlm.get_performance_stats()
        }

# Use performance monitor
monitor = PerformanceMonitor(brain)
sim_stats = monitor.benchmark_simulation(100)
print(f"Simulation speed: {sim_stats['steps_per_second']:.0f} steps/second")

report = monitor.get_performance_report()
```

### 6. Serialization and State Management

```python
class BrainStateManager:
    """Manage brain state with serialization."""
    
    def __init__(self, brain: nlm.Brain):
        self.brain = brain
        self.state_history: list[dict] = []
    
    def save_state(self, description: str = "") -> dict:
        """Save current brain state."""
        # Get current brain state as dictionary
        state = self.brain.to_dict()
        
        # Add metadata
        state['timestamp'] = time.time()
        state['description'] = description
        state['neuron_count'] = self.brain.get_total_neuron_count()
        state['synapse_count'] = self.brain.get_total_synapse_count()
        
        self.state_history.append(state)
        
        return state
    
    def load_state(self, state: dict) -> bool:
        """Load brain state from dictionary."""
        # Validate state
        if 'neuron_count' not in state or 'synapse_count' not in state:
            return False
        
        # In practice, you would implement state restoration
        # This is a simplified example
        print(f"Loaded state: {state['neuron_count']} neurons, {state['synapse_count']} synapses")
        return True
    
    def get_state_summary(self) -> list[dict]:
        """Get summary of saved states."""
        return [
            {
                'index': i,
                'timestamp': state['timestamp'],
                'description': state['description'],
                'neuron_count': state['neuron_count'],
                'synapse_count': state['synapse_count']
            }
            for i, state in enumerate(self.state_history)
        ]

# Use state manager
state_manager = BrainStateManager(brain)
state = state_manager.save_state("After 100 steps")

# Get summary
summary = state_manager.get_state_summary()
print(f"Saved {len(summary)} states")
```

## Migration Guide

### From Original to Enhanced Bindings

#### 1. Import Changes

**Original:**
```python
import pynlm
```

**Enhanced:**
```python
import pynlm_enhanced as nlm
```

#### 2. Class Instantiation

**Original:**
```python
brain = pynlm.create_brain(config)
```

**Enhanced:**
```python
brain = nlm.create_brain(config)
```

#### 3. Method Calls

**Original:**
```python
brain.initialize()
brain.step(0, 0.001)
brain.get_total_spike_count()
```

**Enhanced:**
```python
brain.initialize()
brain.step(0, 0.001)
brain.get_total_spike_count()
```

#### 4. New Features

**New Pythonic APIs:**
```python
# Dictionary-like access
neuron_count = brain["neuron_count"]
brain["simulation_timestep"] = 0.001

# Context managers
with nlm.BrainContext(config) as brain:
    brain.initialize()

# Performance monitoring
stats = nlm.get_performance_stats()

# Serialization
brain.save("state.nlm")
```

#### 5. Type Hints

**Original:** No type hints
**Enhanced:** Full type hints for better IDE support

## Installation

### Using pip

```bash
pip install pynlm_enhanced
```

### From source

```bash
mkdir build
cd build
cmake ..
make
pip install pynlm_enhanced
```

### Development installation

```bash
pip install -e .
```

## Testing

Run the test suite:

```bash
pytest tests/
```

Run performance tests:
```bash
python tests/test_performance.py
```

## Performance Considerations

### Memory Usage

The enhanced bindings may have slightly higher memory usage due to:
- Type hint metadata
- Additional validation logic
- Enhanced error handling

### Performance Impact

- Most operations have minimal performance impact
- Some new features (e.g., serialization) may have overhead
- Use `nlm.get_performance_stats()` to monitor performance

### Optimization Tips

1. Use `nlm.get_memory_usage()` to monitor memory consumption
2. Use `nlm.benchmark()` to profile critical functions
3. Enable garbage collection periodically for long-running simulations
4. Use `nlm.create_default_config()` for better performance

## Compatibility

### Python Version

- Compatible with Python 3.8+
- Type hints require Python 3.8 or later

### C++ Compiler

- Requires C++17 or later
- Compatible with GCC 7+, Clang 5+, MSVC 2019+

### Dependencies

- pybind11 >= 2.11.0
- scikit-build-core >= 0.5.0

## Future Enhancements

### Planned Features

1. **Advanced Type Hints**: Full type stubs for better IDE support
2. **Async APIs**: Asynchronous operation for long-running simulations
3. **GPU Acceleration**: CUDA bindings for GPU-accelerated computation
4. **Distributed Computing**: Multi-process and multi-GPU support
5. **Advanced Visualization**: Interactive plotting and visualization
6. **Machine Learning Integration**: Integration with ML frameworks
7. **Cloud Computing**: Cloud deployment and scaling
8. **Container Support**: Docker and container integration

### Research Directions

1. **Neuromorphic Computing**: Integration with neuromorphic hardware
2. **Quantum Computing**: Quantum neural simulation capabilities
3. **Cognitive Computing**: Advanced cognitive architectures
4. **Autonomous Systems**: Self-improving and self-organizing systems

## License

MIT License. See LICENSE file for details.

## Acknowledgements

This enhanced binding builds on the original NLM Python bindings and adds comprehensive type hints, Pythonic APIs, and advanced features for improved usability and developer experience.

## Contact

For questions, issues, or suggestions:
- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Email: contact@nlm-project.org

---

*This document describes the enhanced Python bindings for the Neural Learning Machine (NLM) framework. The enhanced bindings provide comprehensive type hints, Pythonic APIs, and advanced features for improved usability and developer experience.*
