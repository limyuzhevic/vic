# NLM Python API Documentation
# ================================

This document provides comprehensive documentation for the NLM Python API, including high-level classes, convenience functions, and usage examples.

## Overview

The NLM Python API provides two interfaces:

1. **Low-level API** (via `pynlm` module): Direct bindings to C++ NLM classes
2. **High-level API** (via `pynlm` module): Simplified classes and convenience functions

The high-level API is recommended for most users as it provides better documentation, error handling, and ease of use.

## High-Level Classes

### NLMBrain

A simplified brain interface for common neural simulation operations.

#### Methods

```python
def __init__(self, neuron_count: int = 1000, complexity: float = 0.5, 
             config: Optional[Dict[str, Any]] = None)
    Initialize NLMBrain with neuron count and complexity

    Args:
        neuron_count: Number of neurons in the brain (default: 1000)
        complexity: Brain complexity (0.0-1.0, default: 0.5)
        config: Optional configuration dict

def initialize(self)
    Initialize the brain for simulation. Must be called before any simulation steps.

def step(self, step_number: int, current_time: Optional[float] = None)
    Perform one simulation step.
    
    Args:
        step_number: The step number (used for learning rules)
        current_time: Optional current time in simulation

def reset(self)
    Reset the brain to its initial state.

def get_neuron_count(self) -> int
    Get the number of neurons in the brain.

def get_complexity(self) -> float
    Get the brain complexity (0.0-1.0).

def get_total_neurons(self) -> int
    Get total neuron count.

def get_total_synapses(self) -> int
    Get total synapse count.

def get_active_neurons(self) -> int
    Get count of currently active neurons.

def get_firing_neurons(self) -> int
    Get count of currently firing neurons.

def get_average_firing_rate(self) -> float
    Get average firing rate across all neurons.

def get_total_spikes(self) -> int
    Get total spike count.

def get_developmental_stage(self)
    Get current developmental stage.

def set_developmental_stage(self, stage)
    Set developmental stage.

def inject_current(self, neuron_id, current: float)
    Inject current into a specific neuron.
    
    Args:
        neuron_id: Neuron ID to inject current into
        current: Current value to inject

def inject_current_to_neurons(self, neuron_type: str, current: float)
    Inject current into all neurons of a specific type.
    
    Args:
        neuron_type: Type of neurons (e.g., 'Excitatory', 'Inhibitory')
        current: Current value to inject

def produce_action(self)
    Produce motor action based on neural activity.

def receive_sensory_input(self, sensory_input)
    Inject sensory input into the brain.
    
    Args:
        sensory_input: Sensory input object

def save(self, filepath: str)
    Save brain state to file.
    
    Args:
        filepath: Path to save file

def load(self, filepath: str)
    Load brain state from file.
    
    Args:
        filepath: Path to load file
```

#### Example Usage

```python
import pynlm as nlm

# Create a simple brain
brain = nlm.NLMBrain(neuron_count=1000, complexity=0.7)

# Initialize and run simulation
brain.initialize()

for step in range(100):
    brain.step(step)
    firing_rate = brain.get_average_firing_rate()
    active = brain.get_active_neurons()
    print(f"Step {step}: Firing rate={firing_rate:.3f}, Active neurons={active}")
```

### NLMExperiment

High-level experiment runner for setting up and running complex experiments.

#### Class Methods

```python
@staticmethod
def create_simple(neuron_count: int = 1000, world_size: tuple = (10, 10),
                  config: Optional[Dict[str, Any]] = None) -> 'NLMExperiment'
    Create a simple experiment with basic brain and world setup.

@staticmethod
def create_standard(world_size: tuple = (10, 10), 
                    config: Optional[Dict[str, Any]] = None) -> 'NLMExperiment'
    Create a standard experiment with default settings.
```

#### Instance Methods

```python
def __init__(self, neuron_count: int = 1000, world_size: tuple = (10, 10),
             config: Optional[Dict[str, Any]] = None)
    Initialize experiment.

def configure(self, **kwargs)
    Configure experiment parameters.

def set_visualizer(self, visualizer: NLMVisualizer)
    Set visualizer for the experiment.

def run(self, num_steps: int) -> ExperimentResults
    Run the experiment for a given number of steps.

def enable_features(self, reward_modulation: bool = False,
                    structural_plasticity: bool = False,
                    development: bool = False,
                    curiosity: bool = bool)
    Enable optional brain features.
```

#### ExperimentResults

Container for experiment results.

```python
@dataclass
class ExperimentResults:
    metrics: Dict[str, float]
    timestamps: List[float]
    data_points: List[Dict[str, Any]]
    
    def to_dict(self) -> Dict[str, Any]
        Convert results to dictionary.
```

#### Example Usage

```python
import pynlm as nlm

# Create experiment
experiment = nlm.NLMExperiment.create_standard(world_size=(20, 20))

# Run experiment
results = experiment.run(num_steps=1000)

# Access results
print(f"Final firing rate: {results.metrics['final_firing_rate']:.3f}")
print(f"Total spikes: {results.metrics['final_spikes']}")

# Save results to file
with open('experiment_results.json', 'w') as f:
    json.dump(results.to_dict(), f, indent=2)
```

### NLMVisualizer

Simple visualization interface for NLM simulations.

#### Methods

```python
def __init__(self, backend: str = "console")
    Initialize visualizer.
    
    Args:
        backend: Visualization backend ('console', 'plotly', 'matplotlib')

def enable(self)
    Enable visualization output.

def disable(self)
    Disable visualization output.

def show_firing_rate(self, firing_rate: float, step: int = 0)
    Display firing rate information.

def show_neuron_stats(self, brain)
    Display brain statistics.

def show_simulation_progress(self, current_step: int, total_steps: int,
                             metrics: Optional[Dict] = None)
    Display simulation progress.

def show(self)
    Display accumulated visualizations.
```

#### Example Usage

```python
import pynlm as nlm

visualizer = nlm.NLMVisualizer(backend="console")

# Run simulation with visualization
for step in range(100):
    brain.step(step)
    visualizer.show_firing_rate(brain.get_average_firing_rate(), step)

visualizer.show()
```

## Convenience Functions

### create_simple_brain

Create a simple brain with optimal default settings.

```python
def create_simple_brain(neuron_count: int = None, complexity: float = None,
                       config: Optional[Dict[str, Any]] = None) -> Any:
    """
    Create a simple brain with specified parameters.
    
    Args:
        neuron_count: Number of neurons
        complexity: Brain complexity (0.0-1.0)
        config: Optional additional configuration
        
    Returns:
        Brain object
    """
```

### create_standard_experiment

Create a standard experiment setup.

```python
def create_standard_experiment(world_size: tuple = None) -> Dict[str, Any]:
    """
    Create a standard experiment setup with brain, world, and agent.
    
    Args:
        world_size: Tuple of (width, height) for the world
        
    Returns:
        Dictionary containing 'brain', 'world', and 'agent'
    """
```

### quick_test

Quick testing utility for evaluating NLM functionality.

```python
def quick_test(mode: str = 'connectivity', steps: int = None,
              brain: Any = None, config: Optional[Dict] = None) -> Dict[str, Any]:
    """
    Run a quick test in a specified mode.
    
    Args:
        mode: Test mode ('connectivity', 'firing', 'learning', 'all')
        steps: Number of simulation steps
        brain: Optional pre-created brain object
        config: Optional configuration
        
    Returns:
        Dictionary containing test results
    """
```

### train_agent

High-level training loop for NLM agents.

```python
def train_agent(world: Any, brain: Any, episodes: int,
               visualizer: Any = None,
               reward_function: Optional[Callable] = None,
               stop_criteria: Optional[Dict] = None) -> Dict[str, Any]:
    """
    Train an agent in a world environment.
    
    Args:
        world: World object
        brain: Brain object
        episodes: Number of training episodes
        visualizer: Optional visualizer for progress tracking
        reward_function: Optional function to calculate reward
        stop_criteria: Optional early stopping criteria
        
    Returns:
        Dictionary containing training results
    """
```

### evaluate_performance

Evaluate brain performance on test cases.

```python
def evaluate_performance(brain: Any, test_cases: List[Any],
                       metrics: Optional[List[str]] = None) -> Dict[str, Any]:
    """
    Evaluate brain performance on test cases.
    
    Args:
        brain: Brain object to evaluate
        test_cases: List of test cases to run
        metrics: List of metrics to calculate
        
    Returns:
        Dictionary containing performance evaluation results
    """
```

### optimize_hyperparameters

Optimize brain hyperparameters using a search strategy.

```python
def optimize_hyperparameters(brain: Any, param_space: Dict[str, List],
                           evaluation_func: Optional[Callable] = None,
                           iterations: int = 10) -> Dict[str, Any]:
    """
    Optimize brain hyperparameters.
    
    Args:
        brain: Brain object to optimize
        param_space: Dictionary mapping parameter names to lists of values
        evaluation_func: Function to evaluate hyperparameter combinations
        iterations: Number of optimization iterations
        
    Returns:
        Dictionary containing optimization results
    """
```

## Error Handling

The NLM Python API provides comprehensive error handling:

### Custom Exceptions

```python
class NLMError(Exception):
    """Base exception class for NLM Python API errors."""

class ValidationError(NLMError):
    """Exception raised for input validation errors."""

class ConfigurationError(NLMError):
    """Exception raised for configuration errors."""
```

### Validation

All functions perform input validation with helpful error messages:

```python
def validate_params(neuronCount: int, complexity: float, worldSize: tuple)
    Validate NLM parameters
```

### Error Wrapping

C++ exceptions are automatically wrapped in Python exceptions.

## Configuration

### Global Configuration

The NLM Python API provides global configuration options:

```python
# Configure global settings
nlm.configure(
    default_neuron_count=1000,
    default_complexity=0.5,
    default_world_size=(10, 10),
    default_steps=1000,
    enable_warnings=True
)

# Get configuration values
neuron_count = nlm.get_config('default_neuron_count')
```

## Advanced Usage

### Custom Reward Functions

```python
def custom_reward_function(world, brain, action, step):
    # Custom reward logic based on world state, brain activity, etc.
    reward = 0.0
    
    # Example: Positive reward for high firing rates
    if brain.get_average_firing_rate() > 10.0:
        reward += 1.0
    
    # Example: Negative reward for low firing rates
    if brain.get_average_firing_rate() < 0.1:
        reward -= 0.5
    
    # Example: Reward for actions that move toward center
    if hasattr(world, 'getAgentBody'):
        body = world.getAgentBody()
        distance = (body.x - world.getWidth()/2)**2 + (body.y - world.getHeight()/2)**2
        reward += 0.1 * max(0, 1.0 - distance)
    
    return reward

# Use in training
train_agent(world, brain, episodes=100, reward_function=custom_reward_function)
```

### Custom Test Cases

```python
@dataclass
class TestCase:
    """Custom test case for evaluation."""
    type: str  # 'connectivity', 'firing', 'learning', etc.
    steps: int
    target_metrics: Dict[str, float]  # Target metric values
    weight: float = 1.0  # Weight in overall evaluation

# Create test cases
test_cases = [
    TestCase(type='connectivity', steps=200, target_metrics={'min_active': 10}),
    TestCase(type='firing', steps=100, target_metrics={'min_firing_rate': 1.0}),
    TestCase(type='learning', steps=300, target_metrics={'stage_change': True}),
]

# Evaluate performance
results = evaluate_performance(brain, test_cases, metrics=['speed', 'stability'])
```

### Visualization Backends

Different visualization backends are available:

```python
# Console backend (default)
visualizer = nlm.NLMVisualizer(backend="console")

# Plotly backend (if available)
# visualizer = nlm.NLMVisualizer(backend="plotly")

# Matplotlib backend (if available)
# visualizer = nlm.NLMVisualizer(backend="matplotlib")
```

## Integration with Low-Level API

The high-level API integrates seamlessly with the low-level `pynlm` module:

```python
import pynlm
import pynlm as nlm  # Also imports high-level API

# Access low-level classes
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access high-level classes
simple_brain = nlm.NLMBrain(neuron_count=500)
simple_brain.initialize()

# Both interfaces work with the same underlying C++ objects
```

## Performance Tips

### Memory Management

- Reuse brain objects for multiple simulations
- Use `brain.reset()` instead of creating new brains
- Clear visualization history with `visualizer.clear_history()`

### Simulation Efficiency

- Use appropriate `steps` parameter for your use case
- Configure brain complexity based on required detail
- Enable only necessary features to reduce overhead

### Batch Operations

```python
# Process multiple steps efficiently
brain.initialize()
for step in range(1000):
    brain.step(step)

# Process in batches
batch_size = 100
for batch_start in range(0, 1000, batch_size):
    batch_end = min(batch_start + batch_size, 1000)
    for step in range(batch_start, batch_end):
        brain.step(step)
    # Optional: Check performance and save if needed
```

## Troubleshooting

### Common Issues

1. **No active neurons**: Ensure the brain is initialized and has sufficient time to warm up.
2. **Excessive firing rates**: Reduce brain complexity or check neuron parameters.
3. **Memory issues**: Clear visualization history and reuse brain objects.
4. **Import errors**: Ensure the C++ extensions are built and in your Python path.

### Error Messages

- `NLMError`: General NLM error
- `ValidationError`: Invalid input parameters
- `ConfigurationError`: Invalid configuration
- `ImportError`: Missing NLM bindings

## Migration Guide

### From Low-Level to High-Level API

#### Before (Low-Level)

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Get stats
spikes = brain.getTotalSpikeCount()
rate = brain.getAverageFiringRate()
```

#### After (High-Level)

```python
import pynlm as nlm

# Create brain (simplified)
brain = nlm.NLMBrain(neuron_count=1000, complexity=0.7)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Get stats (simplified interface)
spikes = brain.get_total_spikes()
rate = brain.get_average_firing_rate()
```

### Feature Mapping

| Low-Level Feature | High-Level Equivalent |
|-------------------|----------------------|
| `pynlm.createBrain(config)` | `nlm.NLMBrain()` |
| `brain.initialize()` | `brain.initialize()` |
| `brain.step(i)` | `brain.step(i)` |
| `brain.getTotalSpikeCount()` | `brain.get_total_spikes()` |
| `pynlm.createSimpleWorld()` | `nlm.NLMExperiment.create_standard()` |
| `world.update(0.1)` | `experiment.run(num_steps)` |

## License

See the LICENSE file in the root directory for full license information.

## Acknowledgments

- Based on the NLM neural simulation framework
- Special thanks to the NLM development team
- Documentation generated from source code comments