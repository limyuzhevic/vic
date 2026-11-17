# Improved NLM Python Bindings with Modern Python Features

This document outlines the improvements made to the NLM (Neural Learning Machine) Python bindings to enhance usability, add modern Python features, and improve the overall user experience.

## Key Improvements

### 1. Simplified loadFromArgs API

**Before:** Complex C++-style argument parsing
```cpp
.def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
    std::vector<char*> argv;
    argv.reserve(args.size());
    for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
    return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
}, py::arg("args"),
   "Load configuration from command line arguments")
```

**After:** Intuitive Python dictionary-based configuration
```python
# New simplified Python API
config.load_from_dict({
    "random_seed": 42,
    "neuron_count": 500,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015
})

# Still supports traditional args for compatibility
config.load_from_args(["--random_seed", "42", "--neuron_count", "500"])
```

### 2. Modern Python Features Added

#### Type Hints and Documentation
```python
class Brain:
    """Central neural simulation brain class.
    
    Args:
        config: Configuration object for brain initialization
    """
    
    def step(self, current_step: int, current_time: float = None) -> None:
        """Perform a simulation step."""
        ...
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate across all neurons."""
        ...
```

#### Context Managers for Resource Management
```python
# Context manager for automatic brain cleanup
with Brain(config) as brain:
    brain.initialize()
    brain.step(step, time)
    # Brain automatically reset on exit

# World context manager for environment management
with World() as world:
    world.configure(100, 100, 20, 20)
    world.update(timestep)
```

#### Fluent Interface Design
```python
# Chain multiple operations
brain = (Brain(config)
         .initialize()
         .set_development_stage(DevelopmentalStage.Maturation)
         .add_region("visual_cortex"))

# Configuration chaining
config = (Config()
          .load_from_file("config.json")
          .set("random_seed", 42)
          .set("neuron_count", 1000))
```

### 3. Enhanced Error Handling

#### Pythonic Exceptions
```python
class NLMError(Exception):
    """Base exception for all NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Raised when configuration is invalid."""
    pass

class NeuralError(NLMError):
    """Raised when neural operations fail."""
    pass
```

#### Better Error Messages
```python
# Clear error messages
try:
    brain.step(step, time)
except NeuralError as e:
    print(f"Neural error: {e}")
    print(f"Suggestion: Check neuron connectivity or parameters")
```

### 4. Advanced Neural Models and Utilities

#### High-Level Neural Models
```python
# Predefined brain architectures
class FeedForwardBrain(Brain):
    """Simple feedforward neural network."""
    
    def __init__(self, config=None):
        super().__init__(config or self.get_default_config())
        self._setup_feedforward_layers()

class RecurrentBrain(Brain):
    """Recurrent neural network with short-term memory."""
    
    def __init__(self, config=None):
        super().__init__(config or self.get_default_config())
        self._setup_recurrent_connections()

class HierarchicalBrain(Brain):
    """Multi-layer hierarchical brain with attention."""
    
    def __init__(self, config=None):
        super().__init__(config or self.get_default_config())
        self._setup_hierarchical_structure()
```

#### Advanced Utilities
```python
# Neural visualization utilities
class NeuralVisualizer:
    """Utilities for visualizing neural activity and connectivity."""
    
    @staticmethod
    def plot_connectivity(brain, output_file="connectivity.png"):
        """Plot brain connectivity matrix."""
        ...
    
    @staticmethod
    def animate_spikes(brain, steps=100, interval=50):
        """Animate neural spike patterns."""
        ...

# Learning and analysis utilities
class LearningAnalyzer:
    """Analyze neural learning patterns and performance."""
    
    @staticmethod
    def analyze_plasticity(brain, window=1000):
        """Analyze synaptic plasticity over time."""
        ...
    
    @staticmethod
    def detect_patterns(activity_data):
        """Detect patterns in neural activity."""
        ...
```

### 5. Configuration Management Improvements

#### Modern Configuration API
```python
class NLMConfig:
    """Enhanced configuration management with validation."""
    
    def __init__(self):
        self._config = {}
        self._validators = {}
    
    def add_parameter(self, name, value, param_type, validation=None):
        """Add a parameter with type validation."""
        if validation and not validation(value):
            raise ValueError(f"Invalid value for {name}: {value}")
        self._config[name] = value
    
    def get_optimized_config(self, task_type="general"):
        """Get pre-optimized configuration for common tasks."""
        presets = {
            "general": self._get_general_preset(),
            "fast_simulation": self._get_fast_preset(),
            "high_precision": self._get_high_precision_preset(),
            "research": self._get_research_preset()
        }
        return presets.get(task_type, self._config)
```

#### Preset Configurations
```python
# Quick presets for common use cases
configs = {
    "toy_brain": {
        "neuron_count": 100,
        "simulation_timestep": 0.01,
        "random_seed": 42
    },
    "research_brain": {
        "neuron_count": 10000,
        "simulation_timestep": 0.001,
        "stdp_ltp_weight": 0.05,
        "stdp_ltd_weight": 0.04,
        "plasticity_modifier": 1.2
    },
    "production_brain": {
        "neuron_count": 5000,
        "simulation_timestep": 0.001,
        "enable_checkpointing": True,
        "checkpoint_interval": 1000
    }
}
```

### 6. Advanced Neural Models

#### Learning Model Extensions
```python
class LearningBrain(Brain):
    """Brain with enhanced learning capabilities."""
    
    def __init__(self, config=None):
        super().__init__(config)
        self._learning_rates = {}
        self._adaptation_history = []
    
    def enable_adaptive_learning(self, base_rate=0.01, decay_rate=0.001):
        """Enable neurons to adapt their learning rates."""
        self._adaptation_history.clear()
        for neuron in self._get_all_neurons():
            neuron.set_learning_rate(base_rate)
        self._decay_rate = decay_rate
    
    def apply_stable_plasticity(self, epochs=100):
        """Apply plasticity with stability constraints."""
        self._learning_rates.clear()
        for neuron in self._get_all_neurons():
            self._learning_rates[neuron.get_id()] = 0.01 * neuron.get_excitability()
    
    def learn_from_experience(self, experience_data):
        """Learn from accumulated experience data."""
        for pattern, outcome in experience_data:
            self._update_weights_based_on_pattern(pattern, outcome)

class DevelopmentalBrain(Brain):
    """Brain that develops and matures over time."""
    
    def __init__(self, config=None):
        super().__init__(config)
        self._development_stage = DevelopmentalStage.Initial
        self._plasticity_schedule = {}
    
    def develop_stage(self, stage, duration):
        """Advance to a new developmental stage."""
        self._development_stage = stage
        self._plasticity_schedule[stage] = duration
        self._apply_stage_specific_changes()
    
    def enable_critical_period(self, neuron_types=None):
        """Enable critical period for specific neuron types."""
        if neuron_types is None:
            neuron_types = [NeuronType.Sensory, NeuronType.Motor]
        # Enhanced plasticity for critical period
        for neuron in self._get_neurons_by_type(neuron_types):
            neuron.set_plasticity_multiplier(2.0)
```

### 7. Performance Optimizations

#### Memory Management
```python
@contextmanager
def brain_session(config):
    """Context manager for efficient brain session management."""
    brain = Brain(config)
    try:
        brain.initialize()
        yield brain
    finally:
        brain.reset()

# Use instead of:
# brain = Brain(config)
# brain.initialize()
# try:
#     # use brain
# finally:
#     brain.reset()
```

#### Batch Operations
```python
class BatchProcessor:
    """Process multiple simulation steps efficiently."""
    
    def __init__(self, brain, batch_size=100):
        self.brain = brain
        self.batch_size = batch_size
        self.results = []
    
    def process_batch(self, steps):
        """Process a batch of steps efficiently."""
        initial_spikes = self.brain.get_total_spike_count()
        for step in range(steps):
            self.brain.step(step, step * self.brain.get_config("simulation_timestep"))
        final_spikes = self.brain.get_total_spike_count()
        self.results.append({
            "steps": steps,
            "spikes": final_spikes - initial_spikes,
            "step_size": steps / (final_spikes - initial_spikes) if final_spikes > initial_spikes else 0
        })
        return self.results[-1]
```

### 8. Example Usage

#### Basic Example
```python
#!/usr/bin/env python3
"""Example: Simple neural simulation with NLM."""

import nlm
from nlm import Brain, World, Config

def main():
    # Create configuration
    config = Config().load_from_dict({
        "random_seed": 42,
        "neuron_count": 500,
        "simulation_timestep": 0.001,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015
    })
    
    # Create and initialize brain
    brain = Brain(config)
    brain.initialize()
    
    # Create world
    world = World()
    world.configure(100, 100, 20, 20)
    
    # Run simulation
    print("Starting neural simulation...")
    for step in range(1000):
        # Get sensory input from world
        percept = world.get_sensory_percept()
        
        # Process input
        brain.process_sensory_input(percept)
        
        # Step brain
        brain.step(step, step * config.get_or("simulation_timestep", 0.001))
        
        # Get motor command
        action = brain.produce_action()
        
        # Apply action to world
        world.apply_motor_command(action, step * config.get_or("simulation_timestep", 0.001))
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}: Spikes={brain.get_total_spike_count()}, "
                  f"Firing rate={brain.get_average_firing_rate():.2f} Hz")
    
    # Analyze results
    print("\nSimulation Results:")
    print(f"Total spikes: {brain.get_total_spike_count()}")
    print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
    print(f"Learning detected: {'Yes' if brain.is_learning_occurred() else 'No'}")
    
    # Save brain state
    brain.save("final_brain_state.npz")
    
    print("Simulation completed successfully!")

if __name__ == "__main__":
    main()
```

#### Advanced Example
```python
#!/usr/bin/env python3
"""Advanced example: Developmental learning with curiosity."""

import nlm
from nlm import Brain, World, Config, DevelopmentalStage
import matplotlib.pyplot as plt

def developmental_learning_example():
    """Example of a brain that develops and learns throughout its lifetime."""
    
    # Create specialized configuration for developmental learning
    config = Config()
    config.load_from_dict({
        "random_seed": 42,
        "neuron_count": 1000,
        "simulation_timestep": 0.001,
        "enable_development": True,
        "enable_curiosity": True,
        "enable_reward_modulation": True,
        "enable_structural_plasticity": True
    })
    
    # Create brain and world
    brain = Brain(config)
    world = World()
    world.configure(200, 200, 30, 30)
    
    # Initialize with developmental stages
    brain.set_development_stage(DevelopmentalStage.CriticalPeriod)
    
    # Run developmental learning
    developmental_phases = [
        ("Initial", 0, 1000),
        ("Critical Period", 1000, 2000),
        ("Maturation", 2000, 4000),
        ("Adult", 4000, 6000),
        ("Aging", 6000, 8000)
    ]
    
    learning_data = []
    curiosity_levels = []
    
    for phase_name, start_step, end_step in developmental_phases:
        print(f"\n--- {phase_name} Phase ({start_step}-{end_step} steps) ---")
        brain.develop_stage(developmental_phases.index((phase_name, start_step, end_step)))
        
        for step in range(start_step, end_step):
            # Get sensory input
            percept = world.get_sensory_percept()
            
            # Process input with brain
            brain.process_sensory_input(percept)
            
            # Step brain
            brain.step(step, step * config.get_or("simulation_timestep", 0.001))
            
            # Get action
            action = brain.produce_action()
            
            # Apply action and get reward
            reward = world.apply_motor_command(action, step * config.get_or("simulation_timestep", 0.001))
            
            # Apply reward modulation
            brain.apply_reward_modulation(reward, brain.get_prediction_error())
            
            # Record learning metrics
            if step % 100 == 0:
                learning_data.append({
                    "step": step,
                    "total_spikes": brain.get_total_spike_count(),
                    "firing_rate": brain.get_average_firing_rate(),
                    "curiosity": brain.get_curiosity_level(),
                    "novelty": brain.get_novelty_level()
                })
                curiosity_levels.append(brain.get_curiosity_level())
    
    # Analyze developmental trajectory
    print("\n--- Developmental Analysis ---")
    initial_firing = learning_data[0]["firing_rate"]
    final_firing = learning_data[-1]["firing_rate"]
    print(f"Firing rate trajectory: {initial_firing:.2f} -> {final_firing:.2f} Hz")
    print(f"Average curiosity level: {sum(curiosity_levels)/len(curiosity_levels):.3f}")
    
    # Plot learning curve
    steps = [d["step"] for d in learning_data]
    firing_rates = [d["firing_rate"] for d in learning_data]
    
    plt.figure(figsize=(12, 5))
    
    plt.subplot(1, 2, 1)
    plt.plot(steps, firing_rates)
    plt.title("Firing Rate Development")
    plt.xlabel("Simulation Step")
    plt.ylabel("Firing Rate (Hz)")
    
    plt.subplot(1, 2, 2)
    plt.plot(steps, [d["curiosity"] for d in learning_data])
    plt.title("Curiosity Level")
    plt.xlabel("Simulation Step")
    plt.ylabel("Curiosity Level")
    
    plt.tight_layout()
    plt.savefig("developmental_learning.png")
    print("Developmental learning plot saved as 'developmental_learning.png'")
    
    return learning_data

if __name__ == "__main__":
    result = developmental_learning_example()
```

## Installation and Usage

### Quick Start
```bash
# Install NLM
pip install nlm

# Basic usage
python -c "import nlm; brain = nlm.Brain(); brain.initialize(); print('Brain ready!')"

# Run examples
python examples/basic_example.py
python examples/advanced_example.py
```

### Building from Source
```bash
# Clone the repository
https://github.com/nlm-project/nlm.git

cd nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install .
```

### Python API Reference

#### Core Classes
- `Brain`: Central neural simulation engine
- `World`: 2D simulation environment
- `Config`: Configuration management
- `NeuronId`, `SynapseId`, `RegionId`: Strongly-typed identifiers
- `Vision`, `Audio`, `InternalSignals`: Sensory input types
- `Action`, `MotorCommand`: Motor output types

#### Key Methods
- `brain.step()`: Advance simulation by one timestep
- `brain.process_sensory_input()`: Inject sensory data
- `brain.produce_action()`: Generate motor command
- `brain.apply_reward_modulation()`: Apply learning signals
- `brain.save()`: Save brain state to file
- `brain.load()`: Load brain state from file

#### Constants and Enums
- `NeuronType`: Excitatory, Inhibitory, Modulatory, etc.
- `SynapseType`: Excitatory, Inhibitory, Modulatory, etc.
- `DevelopmentalStage`: Initial, CriticalPeriod, Maturation, Adult, Aging
- `ActionType`: MoveForward, MoveBackward, TurnLeft, etc.

## Testing

### Running Tests
```bash
# Run C++ tests
cd build
ctest

# Run Python tests
pytest python/tests/

# Run integration tests
pytest integration/
```

### Test Coverage
- Unit tests for all C++ components
- Python binding tests
- Integration tests for brain-world interaction
- Performance benchmarks
- Stress tests

## Contributing

### Code Style
- Follow C++14/17 best practices
- Use pybind11 idiomatic bindings
- Write comprehensive unit tests
- Maintain backward compatibility

### Documentation
- Update docstrings for all Python bindings
- Add examples to the documentation
- Create API reference
- Write user tutorials

### Performance
- Profile and optimize critical paths
- Implement efficient memory management
- Add performance benchmarks
- Optimize for both accuracy and speed

## License

MIT License. See LICENSE file for details.

## Acknowledgments

Special thanks to:
- The NLM research team for their contributions
- Contributors who have improved the codebase
- Users who have provided feedback and reported issues
- Open source projects that NLM depends on

## Future Improvements

1. **Advanced Python Interfaces**: Add high-level neural network APIs
2. **GPU Acceleration**: Enable GPU-accelerated neural simulation
3. **Distributed Computing**: Support for multi-machine simulations
4. **Visualization Tools**: Enhanced 3D and interactive visualization
5. **Cloud Integration**: AWS/GCP integration for cloud-based simulations
6. **Machine Learning Integration**: Combine neural simulation with ML

The NLM Python bindings are continuously evolving to provide the most powerful and user-friendly neural simulation framework available. We welcome contributions and feedback from the community!
"""
import nlm

__version__ = "0.1.0"
__author__ = "NLM Authors"
__all__ = [
    # Core classes
    'Brain', 'World', 'Config', 'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    # Neural types
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    # Sensory and motor
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'MotorCommand', 'WorldObject', 'AgentBody', 'ActionResult',
    # Agent systems
    'AgentBrain', 'SensoryPercept',
    # Configuration
    'ConfigSource',
    # Error handling
    'NLMError', 'ConfigurationError', 'NeuralError',
    # Advanced utilities
    'NeuralVisualizer', 'LearningAnalyzer', 'BatchProcessor',
    'FeedForwardBrain', 'RecurrentBrain', 'HierarchicalBrain',
    'LearningBrain', 'DevelopmentalBrain',
    # Presets
    'configs', 'NeuralModelPresets'
]

# Provide backward compatibility
from nlm.bindings import (
    # Original bindings for backward compatibility
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID
)

# Export enhanced versions
from nlm.enhanced_bindings import (
    # Enhanced bindings with modern Python features
    Brain, World, Config, ConfigManager, SessionManager,
    NeuralVisualizer, LearningAnalyzer, BatchProcessor
)

# Export model presets
from nlm.models import (
    FeedForwardBrain, RecurrentBrain, HierarchicalBrain,
    LearningBrain, DevelopmentalBrain
)

# Export configuration presets
from nlm.presets import (
    get_preset_config, save_preset, load_preset,
    configs, NeuralModelPresets
)

print("NLM Python bindings loaded successfully!")
print(f"Version: {__version__}")
print(f"Author: {__author__}")
print("Use 'help(nlm)' for more information.")

# Set up logging
import logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Module-level initialization
import os
import sys

# Configure module paths
MODULE_DIR = os.path.dirname(os.path.abspath(__file__))
if MODULE_DIR not in sys.path:
    sys.path.insert(0, MODULE_DIR)

# Initialize default configuration
_default_config = None

def get_default_config():
    """Get default configuration for quick usage."""
    global _default_config
    if _default_config is None:
        _default_config = Config()
        _default_config.load_from_dict({
            "random_seed": 42,
            "neuron_count": 500,
            "simulation_timestep": 0.001
        })
    return _default_config

# Quick access functions
def quick_brain(neuron_count=500, seed=42):
    """Create a brain with default configuration."""
    config = get_default_config()
    config.set("neuron_count", neuron_count)
    config.set("random_seed", seed)
    return Brain(config)

def quick_simulation(steps=100):
    """Run a quick simulation with default brain."""
    brain = quick_brain()
    world = World()
    world.configure(50, 50, 10, 10)
    
    for step in range(steps):
        percept = world.get_sensory_percept()
        brain.process_sensory_input(percept)
        brain.step(step, step * 0.001)
        action = brain.produce_action()
        world.apply_motor_command(action, step * 0.001)
    
    return brain, world

# Performance monitoring
class PerformanceMonitor:
    """Monitor and log performance metrics."""
    
    def __init__(self):
        self.metrics = {}
        self.start_time = None
        self.end_time = None
    
    def start(self):
        """Start performance monitoring."""
        import time
        self.start_time = time.time()
        self.metrics = {'steps': 0, 'spikes': 0}
    
    def step(self, brain, world):
        """Record metrics for a step."""
        self.metrics['steps'] += 1
        self.metrics['spikes'] += brain.get_total_spike_count() - self.metrics.get('previous_spikes', 0)
        self.metrics['previous_spikes'] = brain.get_total_spike_count()
    
    def end(self):
        """End performance monitoring and return results."""
        import time
        self.end_time = time.time()
        
        if self.start_time and self.end_time:
            self.metrics['elapsed_time'] = self.end_time - self.start_time
            self.metrics['steps_per_second'] = self.metrics['steps'] / self.metrics['elapsed_time']
            self.metrics['spikes_per_second'] = self.metrics['spikes'] / self.metrics['elapsed_time']
        
        return self.metrics

# Create global performance monitor
performance_monitor = PerformanceMonitor()

# Example usage function
def example_usage():
    """Show example usage of the NLM Python bindings."""
    print("=== NLM Python Bindings Example Usage ===\n")
    
    print("1. Create a brain with default configuration:")
    print("   >>> brain = nlm.Brain()")
    print("   >>> brain.initialize()")
    print()
    
    print("2. Run a simulation step:")
    print("   >>> for step in range(100):")
    print("   ...     brain.step(step, step * 0.001)")
    print()
    
    print("3. Get neural activity:")
    print("   >>> print(f'Total spikes: {brain.get_total_spike_count()}')")
    print("   >>> print(f'Average firing rate: {brain.get_average_firing_rate():.2f} Hz')")
    print()
    
    print("4. Save and load brain state:")
    print("   >>> brain.save('my_brain.npz')")
    print("   >>> brain.load('my_brain.npz')")
    print()
    
    print("5. Advanced usage with world interaction:")
    print("   >>> world = nlm.World()")
    print("   >>> world.configure(100, 100, 20, 20)")
    print("   >>> percept = world.get_sensory_percept()")
    print("   >>> brain.process_sensory_input(percept)")
    print("   >>> action = brain.produce_action()")
    print("   >>> world.apply_motor_command(action, step * 0.001)")

# Set up module-level examples
__all__.append('example_usage')

# Register example usage
if 'example_usage' not in globals():
    globals()['example_usage'] = example_usage

# Module metadata
__metadata__ = {
    'version': __version__,
    'author': __author__
}

# Initialize module
if __name__ != '__main__':
    # Module initialization
    pass

print(__doc__)
