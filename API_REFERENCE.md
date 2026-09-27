# NLM API Reference Documentation
# ================================

Complete API reference for all NLM modules, classes, functions, and interfaces.

## Overview

This comprehensive API reference documents all NLM APIs, including:

1. **Low-Level API** (`pynlm` module): Direct C++ bindings for maximum performance
2. **High-Level API** (`nlm` module): Simplified interfaces for common operations
3. **Training Module** (`nlm.training`): Training, optimization, and evaluation tools
4. **Convenience Functions**: One-line utilities for rapid development

## Module Organization

The NLM Python API is organized into logical modules:

### Core Modules

#### 1. High-Level Classes (nlm module)
- **NLMBrain**: Simplified brain interface for common neural simulation operations
- **NLMExperiment**: High-level experiment runner for complete simulation setups
- **NLMVisualizer**: Backend-agnostic visualization interface
- **ExperimentResults**: Container for experiment results and metrics

#### 2. Training Module (nlm.training module)
- **TrainingCore**: Main training loop and evaluation controller
- **TrainingConfig**: Configuration for training runs and hyperparameters
- **TrainingCallback**: Event-driven callbacks for training monitoring
- **TrainingMonitor**: Performance monitoring and metrics collection
- **HyperparameterOptimizer**: Automated hyperparameter tuning
- **CommonTrainingPatterns**: Implementations of common training patterns (RL, SL, US, Hybrid)
- **TrainingPersistence**: Save/load functionality for training sessions

#### 3. Convenience Functions (nlm module)
- **create_simple_brain**: One-line brain creation with intelligent defaults
- **create_standard_experiment**: Complete experiment setup with brain, world, and agent
- **quick_test**: Rapid verification of NLM system functionality
- **train_agent**: High-level training loop with reward functions and early stopping
- **evaluate_performance**: Systematic performance assessment on test cases
- **optimize_hyperparameters**: Automated hyperparameter optimization using search strategies

#### 4. Error Handling (nlm module)
- **NLMError**: Base exception class for NLM Python API errors
- **ValidationError**: Exception raised for invalid input parameters
- **ConfigurationError**: Exception raised for invalid configuration values

## High-Level API Documentation

### NLMBrain Class

**Description**: A simplified brain interface that abstracts away the complexity of low-level NLM operations while maintaining full access to underlying functionality.

**Methods**:

#### Core Operations
```python
__init__(neuron_count=1000, complexity=0.5, config=None)
    Initialize NLMBrain with neuron count and complexity

def initialize(self)
    Initialize the brain for simulation. Must be called before any simulation steps.

def step(self, step_number, current_time=None)
    Perform one simulation step.

def reset(self)
    Reset the brain to its initial state.
```

#### Statistics and Monitoring
```python
def get_neuron_count(self) -> int
    Get the number of neurons in the brain.

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
```

#### Input/Output Operations
```python
def inject_current(self, neuron_id, current: float)
    Inject current into a specific neuron.

def inject_current_to_neurons(self, neuron_type: str, current: float)
    Inject current into all neurons of a specific type.

def produce_action(self)
    Produce motor action based on neural activity.

def receive_sensory_input(self, sensory_input)
    Inject sensory input into the brain.
```

#### State Management
```python
def save(self, filepath: str)
    Save brain state to file.

def load(self, filepath: str)
    Load brain state from file.
```

**Example Usage**:
```python
import pynlm as nlm

# Create a simple brain
brain = nlm.NLMBrain(neuron_count=1000, complexity=0.7)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    firing_rate = brain.get_average_firing_rate()
    active = brain.get_active_neurons()
    print(f"Step {step}: Firing rate={firing_rate:.3f}, Active neurons={active}")

# Save and load state
brain.save("brain_state.nlm")
# ... do some work ...
brain.load("brain_state.nlm")
```

### NLMExperiment Class

**Description**: High-level experiment runner for setting up and running complex experiments with brain, world, and agent components.

**Factory Methods**:

```python
@staticmethod
def create_simple(neuron_count=1000, world_size=(10, 10), config=None) -> 'NLMExperiment'
    Create a simple experiment with basic brain and world setup.

@staticmethod
def create_standard(world_size=(10, 10), config=None) -> 'NLMExperiment'
    Create a standard experiment with default settings.
```

**Instance Methods**:

```python
def __init__(self, neuron_count=1000, world_size=(10, 10), config=None)
    Initialize experiment.

def configure(self, **kwargs)
    Configure experiment parameters.

def set_visualizer(self, visualizer: NLMVisualizer)
    Set visualizer for the experiment.

def run(self, num_steps) -> ExperimentResults
    Run the experiment for a given number of steps.

def enable_features(self, reward_modulation=False, structural_plasticity=False,
                   development=False, curiosity: bool)
    Enable optional brain features.
```

**Example Usage**:
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

### NLMVisualizer Class

**Description**: Simple visualization interface for NLM simulations with multiple backend support.

**Backends**:
- `console`: Simple text-based output (default)
- `plotly`: Interactive web-based plots (if available)
- `matplotlib`: Static plots (if available)

**Methods**:
```python
def __init__(self, backend="console")
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

**Example Usage**:
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

**Description**: Create a simple brain with optimal default settings and parameter validation.

**Signature**:
```python
def create_simple_brain(neuron_count: int = None, complexity: float = None,
                       config: Optional[Dict[str, Any]] = None) -> Any:
```

**Parameters**:
- `neuron_count`: Number of neurons (defaults to global config)
- `complexity`: Brain complexity (0.0-1.0, defaults to global config)
- `config`: Optional additional configuration

**Returns**:
- pynlm Brain object or simplified NLMBrain wrapper

**Raises**:
- ValidationError: If parameters are invalid

**Example**:
```python
brain = create_simple_brain(neuron_count=500, complexity=0.7)
brain.initialize()
```

### create_standard_experiment

**Description**: Create a standard experiment setup with brain, world, and agent using standard parameters.

**Signature**:
```python
def create_standard_experiment(world_size: tuple = None) -> Dict[str, Any]:
```

**Parameters**:
- `world_size`: Tuple of (width, height) for the world (defaults to global config)

**Returns**:
- Dictionary containing 'brain', 'world', and 'agent' keys

**Example**:
```python
experiment = create_standard_experiment(world_size=(20, 20))
brain = experiment['brain']
world = experiment['world']
agent = experiment['agent']

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### quick_test

**Description**: Quick testing utility for evaluating NLM functionality in different modes.

**Signature**:
```python
def quick_test(mode: str = 'connectivity', steps: int = None,
              brain: Any = None, config: Optional[Dict] = None) -> Dict[str, Any]:
```

**Parameters**:
- `mode`: Test mode ('connectivity', 'firing', 'learning', 'all')
- `steps`: Number of simulation steps
- `brain`: Optional pre-created brain object
- `config`: Optional configuration for brain creation

**Returns**:
- Dictionary containing test results

**Raises**:
- ValidationError: If mode or parameters are invalid

**Example**:
```python
# Quick connectivity test
results = quick_test(mode='connectivity', steps=500)
print(f"Test passed: {results['passed']}")

# Quick firing rate test
results = quick_test(mode='firing', steps=200)
if results['passed']:
    print(f"Firing rate: {results['metrics'].get('firing_rate_mean', 0):.3f} Hz")
```

### train_agent

**Description**: High-level training loop for NLM agents with reward tracking, progress visualization, and early stopping.

**Signature**:
```python
def train_agent(world: Any, brain: Any, episodes: int,
               visualizer: Any = None,
               reward_function: Optional[Callable] = None,
               stop_criteria: Optional[Dict] = None) -> Dict[str, Any]:
```

**Parameters**:
- `world`: World object
- `brain`: Brain object
- `episodes`: Number of training episodes
- `visualizer`: Optional visualizer for progress tracking
- `reward_function`: Optional function to calculate reward
- `stop_criteria`: Optional early stopping criteria

**Returns**:
- Dictionary containing training results

**Example**:
```python
results = train_agent(world, brain, episodes=100)
print(f"Training completed: {results['episodes_completed']} episodes")
print(f"Average reward: {results['average_reward']:.3f}")
```

### evaluate_performance

**Description**: Evaluate brain performance on test cases with configurable metrics.

**Signature**:
```python
def evaluate_performance(brain: Any, test_cases: List[Any],
                       metrics: Optional[List[str]] = None) -> Dict[str, Any]:
```

**Parameters**:
- `brain`: Brain object to evaluate
- `test_cases`: List of test cases to run
- `metrics`: List of metrics to calculate (e.g., 'speed', 'accuracy',
            'stability', 'learning_rate')

**Returns**:
- Dictionary containing performance evaluation results

**Example**:
```python
test_cases = [
    {'type': 'connectivity', 'steps': 200, 'target_metrics': {'min_active': 10}},
    {'type': 'firing', 'steps': 100, 'target_metrics': {'min_firing_rate': 1.0}},
    {'type': 'learning', 'steps': 300, 'target_metrics': {'stage_change': True}},
]

results = evaluate_performance(brain, test_cases, metrics=['speed', 'stability'])
print(f"Performance score: {results['performance_score']}")
```

### optimize_hyperparameters

**Description**: Optimize brain hyperparameters using a search strategy.

**Signature**:
```python
def optimize_hyperparameters(brain: Any, param_space: Dict[str, List],
                            evaluation_func: Optional[Callable] = None,
                            iterations: int = 10) -> Dict[str, Any]:
```

**Parameters**:
- `brain`: Brain object to optimize (will be copied for each iteration)
- `param_space`: Dictionary mapping parameter names to lists of values
- `evaluation_func`: Function to evaluate hyperparameter combinations
- `iterations`: Number of optimization iterations to perform

**Returns**:
- Dictionary containing optimization results and best parameters

**Example**:
```python
param_space = {
    'neuron_count': [500, 1000, 2000],
    'complexity': [0.3, 0.5, 0.7],
    'learning_rate': [0.001, 0.01, 0.1],
}

results = optimize_hyperparameters(brain, param_space, iterations=30)
print(f"Best parameters: {results['best_parameters']}")
print(f"Best score: {results['best_score']:.4f}")
```

## Error Handling

### Custom Exception Hierarchy

```python
class NLMError(Exception):
    """Base exception class for NLM Python API errors."""

class ValidationError(NLMError):
    """Raised for invalid input parameters or configuration."""

class ConfigurationError(NLMError):
    """Raised for invalid configuration values."""
```

### Error Handling in Convenience Functions

All convenience functions perform input validation with descriptive error messages:

```python
# Invalid parameters trigger ValidationError
try:
    brain = create_simple_brain(neuron_count=0, complexity=1.5)  # Invalid values
except ValidationError as e:
    print(f"Validation error: {e}")

# Missing required parameters
try:
    results = evaluate_performance(brain, [])  # Empty test cases
except ValidationError as e:
    print(f"Configuration error: {e}")
```

### Error Wrapping

C++ exceptions are automatically wrapped in Python exceptions with helpful context.

## Configuration Management

### Global Configuration

```python
import pynlm as nlm

# Configure global settings
nlm.configure(
    default_neuron_count=1000,
    default_complexity=0.5,
    default_world_size=(10, 10),
    default_steps=1000,
    enable_warnings=True
)

# Create objects using defaults
brain = NLMBrain()  # Uses configured defaults
experiment = NLMExperiment.create_standard()  # Uses configured defaults

# Get configuration values
neuron_count = nlm.get_config('default_neuron_count')
```

### Per-Object Configuration

```python
brain = NLMBrain(
    neuron_count=2000,
    complexity=0.8,
    config={
        "brain": {
            "excitatory_neurons": 1600,
            "inhibitory_neurons": 400,
            "v_thresh": -45.0,
            "tau_mem": 25.0
        },
        "plasticity": {
            "stdp": {"learning_rate": 0.005},
            "hebbian": {"enable": True}
        }
    }
)
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
```python
# Reuse brain objects for multiple simulations
brain = NLMBrain(neuron_count=1000)

# Multiple simulation runs
for i in range(5):
    brain.initialize()
    for step in range(100):
        brain.step(step)
    brain.save(f"simulation_{i}.nlm")
    brain.reset()
```

### Simulation Efficiency
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

**No active neurons**:
- Ensure the brain is initialized and has sufficient time to warm up.
- Check neuron count and complexity parameters.

**Excessive firing rates**:
- Reduce brain complexity or check neuron parameters.
- Adjust firing thresholds and time constants.

**Memory issues**:
- Clear visualization history and reuse brain objects.
- Monitor memory usage during long simulations.

**Import errors**:
- Ensure the C++ extensions are built and in your Python path.
- Verify that all dependencies are installed.

### Error Messages

- `NLMError`: General NLM error
- `ValidationError`: Invalid input parameters
- `ConfigurationError`: Invalid configuration
- `ImportError`: Missing NLM bindings

## Migration Guide

### From Low-Level to High-Level API

**Before (Low-Level)**:
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

**After (High-Level)**:
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
| `pynlm.createAgentBrain(brain)` | `experiment.get_agent()` |

## License

See the LICENSE file in the root directory for full license information.

## Version

- Python API Version: 1.0.0
- Documentation Version: 1.0.0
- Last Updated: 2026-09-27

## Contact

For questions, issues, or feature requests, please refer to the NLM documentation or issue tracker.