# NLM Python API Documentation

## Overview

The NLM (熙然) Python API provides comprehensive bindings to the NLM neural simulation framework. This documentation covers:

- Core Python bindings for C++ classes
- Utility functions and helpers
- Advanced usage patterns
- Best practices and examples

## Installation

The Python bindings are built as part of the NLM C++ project. To install:

```bash
# Build the project (includes Python bindings)
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install (if needed)
make install
```

The Python package `pynlm` will be installed in your Python environment.

## Basic API Reference

### Module Structure

The main NLM module provides the following components:

```python
import nlm

# Core classes
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()
agent_brain = nlm.createAgentBrain(brain)

# Utility modules
from nlm import nlm_utils
from nlm.nlm_utils import NLMValidator, NLMConvenience, NLMBenchmark
```

### Core Classes

#### Config

The `Config` class manages neural simulation parameters.

**Methods:**

- `createDefaultConfig()` → Config
  Create a new configuration with default values.

- `config.loadFromFile(filepath: str)` → bool
  Load configuration from a file.

- `config.loadFromArgs(argc: int, argv: str[])` → bool
  Load configuration from command line arguments.

- `config.saveToFile(filepath: str)` → bool
  Save configuration to a file.

- `config.set(key: str, value: Any, source: ConfigSource)`
  Set a configuration parameter.

- `config.get(key: str, default: Any = None)` → Any
  Get a configuration parameter.

- `config.has(key: str)` → bool
  Check if a configuration key exists.

- `config.getKeys()` → List[str]
  Get all configuration keys.

- `config.summary()` → str
  Get a human-readable summary of the configuration.

**Example:**

```python
import nlm

# Create and configure brain
config = nlm.createDefaultConfig()
config.set('neuron_count', 500)
config.set('region_count', 2)
config.set('connection_probability', 0.15)
config.set('simulation_timestep', 0.001)

brain = nlm.createBrain(config)
brain.initialize()
```

#### Brain

The `Brain` class represents the central neural simulation brain.

**Key Methods:**

- `brain.initialize()` → bool
  Initialize the brain with current configuration.

- `brain.step(current_step: int, current_time: float = 0.0)`
  Perform a simulation step.

- `brain.receiveSensoryInput(input: SensoryInput)`
  Inject sensory input into the brain.

- `brain.produceAction()` → Action
  Generate motor action based on neural activity.

- `brain.reset()`
  Reset brain state for a new episode.

- `brain.save(filepath: str)` → bool
  Save brain state to file.

- `brain.load(filepath: str)` → bool
  Load brain state from file.

- `brain.getTotalNeuronCount()` → int
  Get total number of neurons.

- `brain.getTotalSynapseCount()` → int
  Get total number of synapses.

- `brain.getActiveNeuronCount()` → int
  Get number of currently active neurons.

- `brain.getFiringNeuronCount()` → int
  Get number of neurons currently firing.

- `brain.getAverageFiringRate()` → float
  Get average firing rate across all neurons.

- `brain.getExcitationInhibitionRatio()` → float
  Get excitation/inhibition balance ratio.

- `brain.getTotalSpikeCount()` → int
  Get total spike count.

- `brain.getDevelopmentalStage()` → DevelopmentalStage
  Get current developmental stage.

**Example:**

```python
import nlm

config = nlm.createDefaultConfig()
config.set('neuron_count', 100)
config.set('region_count', 1)

brain = nlm.createBrain(config)
if brain.initialize():
    print("Brain initialized successfully")
    print(f"Neuron count: {brain.getTotalNeuronCount()}")
    
    # Run a few simulation steps
    for step in range(10):
        brain.step(step)
        print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")
```

#### SimpleWorld

The `SimpleWorld` class represents the simulated environment.

**Key Methods:**

- `world.configure(width: int, height: int, vision_width: int, vision_height: int)`
  Configure world dimensions and sensor capabilities.

- `world.update(timestep: float)`
  Update world state.

- `world.applyMotorCommand(command: MotorCommand, current_time: float)`
  Apply a motor command to the world.

- `world.getSensoryPercept()` → SensoryPercept
  Get current sensory percept.

- `world.getAgentBody()` → AgentBody
  Get agent body state.

- `world.addObject(object: WorldObject)`
  Add an object to the world.

- `world.isValidPosition(x: float, y: float)` → bool
  Check if a position is valid.

- `world.reset()`
  Reset world to initial state.

**Example:**

```python
import nlm

world = nlm.createSimpleWorld()
world.configure(100, 100, 20, 20)  # Width, height, vision sensor dimensions

# Set agent start position
world.setAgentStart(50, 50)

# Run simulation
for step in range(100):
    world.update(0.001)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process in brain
    brain.receiveSensoryInput(percept)
    
    # Get action
    action = brain.produceAction()
    
    # Apply to world
    if hasattr(world, 'applyAction'):
        world.applyAction(action)
```

#### AgentBrain

The `AgentBrain` class connects the NLM brain to the world environment.

**Key Methods:**

- `agent_brain.initialize(world: SimpleWorld)`
  Initialize agent brain with world.

- `agent_brain.getSensoryInputSize()` → int
  Get expected sensory input size.

- `agent_brain.getMotorOutputSize()` → int
  Get expected motor output size.

- `agent_brain.processSensoryInput(percept: SensoryPercept)`
  Process sensory percept and inject into brain.

- `agent_brain.decodeMotorCommand()` → MotorCommand
  Decode brain motor activity into motor command.

- `agent_brain.applyRewardModulation(reward: float, predicted_reward: float)`
  Apply reward-based neuromodulation.

- `agent_brain.updateDevelopment(timestep: float)`
  Update development system.

- `agent_brain.getDevelopmentalStage()` → DevelopmentalStage
  Get current developmental stage.

- `agent_brain.reset()`
  Reset agent for new episode.

- Configuration methods: `enableRewardModulation()`, `enableDevelopment()`, etc.

**Example:**

```python
import nlm

# Create brain and world
config = nlm.createDefaultConfig()
config.set('neuron_count', 200)
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(100, 100, 20, 20)

# Create agent brain
agent_brain = nlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Main simulation loop
for step in range(100):
    # Get sensory input from world
    percept = world.getSensoryPercept()
    
    # Process input through agent brain
    agent_brain.processSensoryInput(percept)
    
    # Generate motor command
    motor_command = agent_brain.decodeMotorCommand()
    
    # Apply to world
    world.applyMotorCommand(motor_command, step * 0.001)
    
    # Update brain
    brain.step(step)
    
    # Update development
    agent_brain.updateDevelopment(0.001)
```

### Utility Functions

#### NLMValidator

Utility class for validating NLM parameters and configurations.

**Key Methods:**

- `validate_config(config: Config, strict: bool = False)` → bool
  Validate a configuration object.

- `validate_neuron_count(count: int)` → bool
  Validate neuron count parameter.

- `validate_step_count(step: int)` → bool
  Validate simulation step parameter.

- `validate_array_shape(data: Any, expected_shape: tuple)` → bool
  Validate array shape and type.

**Example:**

```python
from nlm.nlm_utils import NLMValidator

validator = NLMValidator()

# Validate neuron count
try:
    validator.validate_neuron_count(500)
    print("Neuron count validation passed")
except NLMValidationError as e:
    print(f"Validation failed: {e}")
```

#### NLMConvenience

Convenience class providing high-level methods for common NLM operations.

**Key Methods:**

- `run_episode(steps: int = 100, verbose: bool = False)` → dict
  Run a complete simulation episode.

- `train_model(episodes: int = 10, steps_per_episode: int = 100)` → list
  Train a model using reinforcement learning.

- `create_experiment_suite(experiment_configs: list)` → list
  Create multiple experiments with different configurations.

**Example:**

```python
from nlm.nlm_utils import NLMConvenience

import nlm

# Create brain and world
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(100, 100, 20, 20)

# Create convenience wrapper
convention = NLMConvenience(brain, world)

# Run episode
results = convention.run_episode(steps=50, verbose=True)
print(f"Episode completed: {results['spike_delta']} spikes change")
```

#### NLMBenchmark

Utility class for benchmarking NLM performance.

**Key Methods:**

- `time_operation(name: str, func, *args)` → Any
  Time an NLM operation.

- `get_timing_stats(name: str)` → dict
  Get timing statistics for an operation.

- `print_summary()`
  Print benchmark summary.

**Example:**

```python
from nlm.nlm_utils import NLMBenchmark

benchmark = NLMBenchmark()

# Time brain initialization
brain = benchmark.time_operation("brain_init", nlm.createBrain, config)

# Time simulation steps
for step in range(100):
    benchmark.time_operation("brain_step", brain.step, step)

# Print results
benchmark.print_summary()
```

### High-Level API Functions

#### create_brain_from_dict()

Create a brain from a configuration dictionary.

```python
from nlm.nlm_utils import create_brain_from_dict

config_dict = {
    'neuron_count': 500,
    'region_count': 2,
    'connection_probability': 0.15,
    'simulation_timestep': 0.001
}

brain = create_brain_from_dict(config_dict)
brain.initialize()
```

#### create_world_from_spec()

Create a world with specified dimensions.

```python
from nlm.nlm_utils import create_world_from_spec

world = create_world_from_spec(
    width=100,
    height=100,
    vision_width=20,
    vision_height=20
)
```

#### run_training_session()

Run a complete training session with configuration dictionaries.

```python
from nlm.nlm_utils import run_training_session

brain_config = {
    'neuron_count': 200,
    'region_count': 1,
    'connection_probability': 0.2
}

world_config = {
    'width': 100,
    'height': 100,
    'vision_width': 20,
    'vision_height': 20
}

results = run_training_session(
    brain_config=brain_config,
    world_config=world_config,
    episodes=20,
    steps_per_episode=100,
    learning_rate=0.01
)
```

### Error Handling

NLM provides custom exception classes for better error handling:

```python
import nlm
from nlm.nlm_utils import NLMError, NLMValidationError, NLMConfigurationError

try:
    brain = nlm.createBrain(config)
    brain.initialize()
except NLMConfigurationError as e:
    print(f"Configuration error: {e}")
except NLMError as e:
    print(f"NLM error: {e}")
```

### Module Information

The NLM module provides version and author information:

```python
import nlm

print(f"NLM version: {nlm.__version__}")
print(f"Author: {nlm.__author__}")
print(f"Description: {nlm.__description__}")
```

## Advanced Usage

### Working with Multiple Brains

Create multiple independent brain instances for parallel processing:

```python
def create_brain_cluster(config_templates):
    """Create a cluster of brains with different configurations."""
    brains = []
    
    for template in config_templates:
        config = nlm.createDefaultConfig()
        
        # Apply template configuration
        for key, value in template.items():
            config.set(key, value)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
    
    return brains
```

### Custom Sensory Inputs

Create custom sensory input processing:

```python
def create_custom_sensory_handler():
    """Create custom sensory processing pipeline."""
    def process_vision(percept):
        # Extract vision data
        vision_data = percept.getVision()
        
        # Apply custom processing
        processed = [max(0, v * 0.5) for v in vision_data]
        
        # Create new percept with processed data
        new_percept = nlm.SensoryPercept()
        new_percept.setVision(processed)
        
        return new_percept
    
    return process_vision
```

### Experiment Management

Manage complex experiment setups:

```python
class NLMDataset:
    """Manage NLM experiment datasets."""
    
    def __init__(self):
        self.experiments = []
        self.metadata = {}
    
    def add_experiment(self, name, config, results):
        """Add experiment to dataset."""
        self.experiments.append({
            'name': name,
            'config': config,
            'results': results,
            'timestamp': datetime.now()
        })
    
    def get_experiment(self, name):
        """Get experiment by name."""
        for exp in self.experiments:
            if exp['name'] == name:
                return exp
        return None
    
    def compare_experiments(self, experiment_names):
        """Compare multiple experiments."""
        experiments = [self.get_experiment(name) for name in experiment_names]
        
        comparison = {}
        for exp in experiments:
            comparison[exp['name']] = {
                'spike_count': exp['results']['spike_delta'],
                'weight_change': exp['results']['weight_change'],
                'learning_detected': exp['results']['learning_detected']
            }
        
        return comparison
```

### Performance Profiling

Profile NLM performance with detailed metrics:

```python
def profile_performance(brain, world, steps=100):
    """Profile NLM performance with detailed metrics."""
    from nlm.nlm_utils import NLMBenchmark
    import time
    
    benchmark = NLMBenchmark()
    
    # Profile brain initialization
    benchmark.time_operation("init", brain.initialize)
    
    # Profile simulation steps
    start_time = time.time()
    for step in range(steps):
        benchmark.time_operation("step", brain.step, step)
        benchmark.time_operation("update", world.update, 0.001)
    
    # Get performance summary
    print("\nPerformance Profile:")
    benchmark.print_summary()
    
    # Calculate metrics
    total_time = time.time() - start_time
    steps_per_second = steps / total_time
    
    return {
        'total_time': total_time,
        'steps_per_second': steps_per_second,
        'timing_stats': {
            'brain_step': benchmark.get_timing_stats('brain_step'),
            'world_update': benchmark.get_timing_stats('world_update'),
            'brain_init': benchmark.get_timing_stats('brain_init')
        }
    }
```

## Best Practices

### 1. Configuration Management

Use configuration dictionaries for better reproducibility:

```python
# Good: Use configuration dictionaries
brain_config = {
    'neuron_count': 500,
    'region_count': 2,
    'connection_probability': 0.15,
    'simulation_timestep': 0.001,
    'random_seed': 42
}

# Apply to brain
brain = create_brain_from_dict(brain_config)

# Bad: Multiple small configuration changes
config = nlm.createDefaultConfig()
config.set('neuron_count', 500)
config.set('region_count', 2)
# ... many more lines
```

### 2. Error Handling

Always handle errors gracefully:

```python
import nlm
from nlm.nlm_utils import NLMError, NLMValidationError

try:
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
except NLMValidationError as e:
    print(f"Invalid configuration: {e}")
except NLMError as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
    # Log error and attempt recovery
```

### 3. Resource Management

Clean up resources properly:

```python
class NLMEnvironment:
    """Context manager for NLM resources."""
    
    def __enter__(self):
        self.brain = nlm.createBrain(nlm.createDefaultConfig())
        self.brain.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        # Clean up resources
        if hasattr(self, 'brain'):
            # Reset brain state
            self.brain.reset()
        
        # Clear references
        self.brain = None
```

### 4. Validation

Validate inputs before use:

```python
from nlm.nlm_utils import NLMValidator

validator = NLMValidator()

# Validate before creating brain
try:
    validator.validate_neuron_count(1000)
    validator.validate_step_count(100)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
except NLMValidationError as e:
    print(f"Validation failed: {e}")
```

### 5. Testing

Use the test framework for comprehensive testing:

```python
# Run basic tests
import subprocess
result = subprocess.run(['python', 'tests/test_basic.py'], 
                       capture_output=True, text=True)

if result.returncode == 0:
    print("All tests passed!")
else:
    print("Tests failed:")
    print(result.stdout)
    print(result.stderr)
```

## Examples

### Example 1: Simple Simulation

```python
import nlm
from nlm.nlm_utils import NLMConvenience

# Create brain and world
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(100, 100, 20, 20)

# Create convenience wrapper
convention = NLMConvenience(brain, world)

# Run episode
results = convention.run_episode(steps=100, verbose=True)

print(f"Simulation completed:")
print(f"  - Spike change: {results['spike_delta']}")
print(f"  - Weight change: {results['weight_change']:.6f}")
print(f"  - Learning detected: {results['learning_detected']}")
```

### Example 2: Training Session

```python
import nlm
from nlm.nlm_utils import run_training_session

# Configure training
brain_config = {
    'neuron_count': 300,
    'region_count': 2,
    'connection_probability': 0.2,
    'simulation_timestep': 0.001
}

world_config = {
    'width': 100,
    'height': 100,
    'vision_width': 20,
    'vision_height': 20
}

# Run training
results = run_training_session(
    brain_config=brain_config,
    world_config=world_config,
    episodes=10,
    steps_per_episode=50,
    learning_rate=0.05
)

print(f"\nTraining completed:")
print(f"  - Total episodes: {results['total_episodes']}")
print(f"  - Final spike change: {results['final_result']['spike_delta']}")
print(f"  - Learning episodes: {results['learning_episodes']}")
print(f"  - Average spike rate: {results['average_spike_rate']:.2f}")
```

### Example 3: Experiment Suite

```python
import nlm
from nlm.nlm_utils import NLMConvenience

# Define experiment configurations
experiment_configs = [
    {
        'name': 'baseline',
        'neuron_count': 100,
        'region_count': 1,
        'connection_probability': 0.1
    },
    {
        'name': 'high_connectivity',
        'neuron_count': 200,
        'region_count': 2,
        'connection_probability': 0.3
    },
    {
        'name': 'sparse_neurons',
        'neuron_count': 50,
        'region_count': 1,
        'connection_probability': 0.05
    }
]

# Create convenience wrapper
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(100, 100, 20, 20)

convention = NLMConvenience(brain, world)

# Run experiment suite
experiments = convention.create_experiment_suite(experiment_configs)

# Analyze results
print("\nExperiment Results:")
for exp in experiments:
    print(f"\nExperiment: {exp['config']['name']}")
    print(f"  - Neurons: {exp['config']['neuron_count']}")
    print(f"  - Regions: {exp['config']['region_count']}")
    print(f"  - Connectivity: {exp['config']['connection_probability']}")
```

## Troubleshooting

### Common Issues

#### Import Error

**Problem:** `ImportError: cannot import name 'nlm'`

**Solution:**

1. Ensure the NLM Python module is installed:

```bash
pip install pynlm
```

2. Check that the module is in your Python path:

```bash
python -c "import sys; print(sys.path)"
```

3. Rebuild the Python bindings:

```bash
cd build
make -j4
```

#### Configuration Errors

**Problem:** Invalid configuration values

**Solution:** Use the validation utilities:

```python
from nlm.nlm_utils import NLMValidator

validator = NLMValidator()
try:
    validator.validate_neuron_count(1000)
    print("Neuron count valid")
except NLMValidationError as e:
    print(f"Invalid neuron count: {e}")
```

#### Performance Issues

**Problem:** Slow simulation performance

**Solution:**

1. Optimize configuration for performance:

```python
config = nlm.createDefaultConfig()
config.set('neuron_count', 100)  # Smaller network
config.set('region_count', 1)    # Fewer regions
```

2. Use proper simulation steps:

```python
for step in range(100):
    brain.step(step)  # Use actual step count
    world.update(0.001)
```

3. Profile performance:

```python
from nlm.nlm_utils import NLMBenchmark

benchmark = NLMBenchmark()
# ... run simulation ...
benchmark.print_summary()
```

#### Memory Issues

**Problem:** Memory usage grows too large

**Solution:**

1. Use checkpoint system:

```python
brain.save("checkpoint.nlm")
# Later... brain.load("checkpoint.nlm")
```

2. Clean up references:

```python
# Clear references to large objects
brain = None
world = None
```

3. Monitor memory usage:

```python
from nlm.nlm_utils import NLMBenchmark

benchmark = NLMBenchmark()
# ... run simulation ...
# Monitor memory
```

### Getting Help

For additional help:

1. **Documentation:** Check the online documentation at https://nlm.readthedocs.io
2. **Examples:** Look at the examples in the `examples/` directory
3. **Issues:** Report issues at https://github.com/nlm-project/nlm/issues
4. **Discussions:** Join the NLM discussion forum

## Version Information

```python
import nlm
print(f"NLM Python Bindings Version: {nlm.__version__}")
print(f"Author: {nlm.__author__}")
print(f"Description: {nlm.__description__}")
```

This documentation is continuously updated. For the latest information, visit the official NLM documentation.
