# NLM Python API Guide
# ===================

This comprehensive guide documents the NLM Python API, providing detailed information on both the low-level and high-level interfaces, including convenience functions, training utilities, and advanced usage patterns.

## Overview

The NLM (Neural Learning Machine) Python API provides comprehensive access to the NLM neural simulation framework, which implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms.

### API Architecture

The NLM Python API is organized into three main layers:

1. **C++ Bindings** (`pynlm` module): Direct bindings to C++ classes for maximum performance and access to all features
2. **High-Level Classes** (`NLMBrain`, `NLMExperiment`, `NLMVisualizer`): Simplified interfaces for common operations
3. **Convenience Functions**: One-line functions for rapid development

### Key Classes and Functions

#### High-Level Classes

##### NLMBrain
A simplified brain interface that abstracts away the complexity of low-level NLM operations while maintaining full access to underlying functionality.

**Core Methods:**
- `__init__(neuron_count=1000, complexity=0.5, config=None)`: Create brain with optimal defaults
- `initialize()`: Initialize brain for simulation
- `step(step_number, current_time=None)`: Execute one simulation step
- `reset()`: Reset brain to initial state
- `get_neuron_count()`: Get total neuron count
- `get_total_synapses()`: Get total synapse count
- `get_active_neurons()`: Get currently active neurons
- `get_firing_neurons()`: Get currently firing neurons
- `get_average_firing_rate()`: Get average firing rate
- `get_total_spikes()`: Get total spike count
- `get_developmental_stage()`: Get current developmental stage
- `inject_current(neuron_id, current)`: Inject current into specific neuron
- `produce_action()`: Generate motor action from neural activity
- `receive_sensory_input(sensory_input)`: Process sensory input
- `save(filepath)`: Save brain state to file
- `load(filepath)`: Load brain state from file

##### NLMExperiment
High-level experiment runner that manages complete simulation environments with brain, world, and agent components.

**Factory Methods:**
- `create_simple(neuron_count=1000, world_size=(10, 10), config=None)`: Create basic experiment
- `create_standard(world_size=(10, 10), config=None)`: Create standard experiment with defaults

**Core Methods:**
- `configure(**kwargs)`: Configure experiment parameters
- `set_visualizer(visualizer)`: Attach visualizer for progress tracking
- `run(num_steps)`: Execute experiment and return results
- `enable_features(reward_modulation, structural_plasticity, development, curiosity)`: Enable optional features

##### NLMVisualizer
Backend-agnostic visualization interface for monitoring brain states and simulation progress.

**Backends:**
- `console`: Simple text-based output (default)
- `plotly`: Interactive web-based plots (if available)
- `matplotlib`: Static plots (if available)

**Core Methods:**
- `show_firing_rate(firing_rate, step=0)`: Display firing rate information
- `show_neuron_stats(brain)`: Show brain statistics
- `show_simulation_progress(current_step, total_steps, metrics=None)`: Show progress bar

#### Convenience Functions

##### create_simple_brain
```python
create_simple_brain(neuron_count=None, complexity=None, config=None)
```
One-line brain creation with intelligent defaults and parameter validation.

##### create_standard_experiment
```python
create_standard_experiment(world_size=None)
```
Complete experiment setup with optimized brain, world, and agent configuration.

##### quick_test
```python
quick_test(mode='connectivity', steps=None, brain=None, config=None)
```
Rapid verification of NLM system functionality in different modes.

##### train_agent
```python
train_agent(world, brain, episodes, visualizer=None, reward_function=None, stop_criteria=None)
```
Complete training loop with reward tracking, progress visualization, and early stopping.

##### evaluate_performance
```python
evaluate_performance(brain, test_cases, metrics=None)
```
Systematic performance assessment on test cases with configurable metrics.

##### optimize_hyperparameters
```python
optimize_hyperparameters(brain, param_space, evaluation_func=None, iterations=10)
```
Automated hyperparameter optimization using search strategies.

## Advanced Usage Patterns

### Custom Reward Functions
```python
def custom_reward_function(world, brain, action, step):
    reward = 0.0
    
    # Reward for high firing rates (learning activity)
    if brain.get_average_firing_rate() > 10.0:
        reward += 1.0
    
    # Penalty for low firing rates
    if brain.get_average_firing_rate() < 0.1:
        reward -= 0.5
    
    # Reward for agent proximity to goal
    if hasattr(world, 'getAgentBody'):
        body = world.getAgentBody()
        center_x = world.getWidth() / 2
        center_y = world.getHeight() / 2
        distance = ((body.x - center_x) ** 2 + (body.y - center_y) ** 2) ** 0.5
        reward += 0.1 * max(0, 1.0 - distance / max(world.getWidth(), world.getHeight()))
    
    return reward
```

### Custom Test Cases
```python
@dataclass
class TestCase:
    type: str  # 'connectivity', 'firing', 'learning', etc.
    steps: int
    target_metrics: Dict[str, float]
    weight: float = 1.0

# Create comprehensive test suite
test_cases = [
    TestCase(type='connectivity', steps=200, target_metrics={'min_active': 10}),
    TestCase(type='firing', steps=100, target_metrics={'min_firing_rate': 1.0}),
    TestCase(type='learning', steps=300, target_metrics={'stage_change': True}),
    TestCase(type='memory', steps=400, target_metrics={'stability': 0.8}),
]

results = evaluate_performance(brain, test_cases, metrics=['speed', 'stability', 'accuracy'])
```

### Batch Processing
```python
# Process multiple simulation steps efficiently
brain = NLMBrain(neuron_count=500, complexity=0.7)
brain.initialize()

# Large-scale simulation
batch_size = 1000
for batch_start in range(0, 100000, batch_size):
    batch_end = min(batch_start + batch_size, 100000)
    
    for step in range(batch_start, batch_end):
        brain.step(step)
        
        # Periodic checkpoint and performance check
        if step % 10000 == 0:
            brain.save(f"checkpoint_step_{step}.nlm")
            firing_rate = brain.get_average_firing_rate()
            print(f"Step {step}: Firing rate = {firing_rate:.3f}")
```

## Error Handling

The NLM Python API provides comprehensive error handling:

### Custom Exception Hierarchy
```python
class NLMError(Exception):
    """Base exception class for all NLM Python API errors."""

class ValidationError(NLMError):
    """Raised for invalid input parameters or configuration."""

class ConfigurationError(NLMError):
    """Raised for invalid configuration values."""

class ImportError(NLMError):
    """Raised when NLM C++ bindings are not available."""
```

### Input Validation
All functions perform automatic validation with descriptive error messages:

```python
# Invalid parameters trigger ValidationError
try:
    brain = NLMBrain(neuron_count=0, complexity=1.5)  # Invalid values
except ValidationError as e:
    print(f"Validation error: {e}")

# Missing required parameters
try:
    results = evaluate_performance(brain, [])  # Empty test cases
except ValidationError as e:
    print(f"Configuration error: {e}")
```

### Error Recovery
```python
def safe_simulation(brain, steps, max_retries=3):
    for attempt in range(max_retries):
        try:
            brain.initialize()
            for step in range(steps):
                brain.step(step)
            return True, "Simulation successful"
        except Exception as e:
            print(f"Attempt {attempt + 1} failed: {e}")
            if attempt == max_retries - 1:
                return False, f"Failed after {max_retries} attempts: {e}"
            brain.reset()
    return False, "Unknown error"
```

## Configuration Management

### Global Configuration
```python
import pynlm as nlm

# Set global defaults
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
```

### Per-Object Configuration
```python
brain = NLMBrain(
    neuron_count=2000,
    complexity=0.7,
    config={
        "brain": {
            "excitatory_neurons": 1600,
            "inhibitory_neurons": 400,
            "v_thresh": -50.0,
            "tau_mem": 20.0
        },
        "plasticity": {
            "stdp": {"learning_rate": 0.001},
            "hebbian": {"enable": True}
        }
    }
)
```

### Configuration Validation
```python
def validate_config(config):
    """Validate configuration parameters."""
    required_keys = ['brain', 'plasticity']
    for key in required_keys:
        if key not in config:
            raise ConfigurationError(f"Missing required config section: {key}")
    
    if 'neuron_count' in config.get('brain', {}):
        count = config['brain']['neuron_count']
        if count <= 0:
            raise ConfigurationError(f"Neuron count must be positive: {count}")
    
    complexity = config.get('brain', {}).get('complexity', 0.5)
    if not 0.0 <= complexity <= 1.0:
        raise ConfigurationError(f"Complexity must be in [0.0, 1.0]: {complexity}")
```

## Performance Optimization

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

# Clear visualization history
visualizer = NLMVisualizer(backend="console")
for step in range(100):
    brain.step(step)
    visualizer.show_firing_rate(brain.get_average_firing_rate(), step)

visualizer.clear_history()  # Free memory
```

### Efficient Batch Operations
```python
class BatchProcessor:
    def __init__(self, brain):
        self.brain = brain
        self.batch_size = 1000
        self.checkpoint_interval = 10000
        self.step_count = 0
    
    def process_batch(self, total_steps):
        self.brain.initialize()
        
        for batch_start in range(0, total_steps, self.batch_size):
            batch_end = min(batch_start + self.batch_size, total_steps)
            
            for step in range(batch_start, batch_end):
                self.brain.step(step)
                self.step_count += 1
                
                if self.step_count % self.checkpoint_interval == 0:
                    self.brain.save(f"checkpoint_step_{step}.nlm")
        
        return self.step_count
```

### Profiler Integration
```python
import time
import pynlm as nlm

class SimulationProfiler:
    def __init__(self):
        self.timings = []
        self.memory_usage = []
    
    def run_profiled(self, brain, steps):
        start_time = time.time()
        
        brain.initialize()
        for step in range(steps):
            step_start = time.time()
            brain.step(step)
            step_time = time.time() - step_start
            
            self.timings.append(step_time)
            
            if step % 100 == 0:
                print(f"Step {step}: {step_time*1000:.2f}ms")
        
        total_time = time.time() - start_time
        avg_step_time = sum(self.timings) / len(self.timings)
        
        return {
            'total_time': total_time,
            'avg_step_time': avg_step_time,
            'steps_per_second': steps / total_time,
            'timings': self.timings
        }
```

## Integration Patterns

### Low-Level to High-Level Migration
```python
# Before: Low-level API
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

spikes = brain.getTotalSpikeCount()
rate = brain.getAverageFiringRate()

# After: High-level API
import pynlm as nlm

brain = nlm.NLMBrain(neuron_count=1000, complexity=0.7)
brain.initialize()

for i in range(100):
    brain.step(i)

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
| `agent.initialize(world)` | Part of experiment setup |
| `agent.processSensoryInput(percept)` | Handled by experiment loop |
| `agent.decodeMotorCommand()` | Handled by experiment loop |

### Advanced Integration
```python
class IntegratedExperiment:
    def __init__(self):
        self.brain = None
        self.world = None
        self.agent = None
        self.visualizer = None
        self.results = {}
    
    def setup(self, brain_config=None, world_config=None):
        # Create brain using high-level API
        self.brain = NLMBrain(**brain_config) if brain_config else NLMBrain()
        
        # Create experiment with standard setup
        experiment = NLMExperiment.create_standard(
            world_size=world_config.get('world_size', (10, 10)) if world_config else None
        )
        
        self.brain = experiment['brain']
        self.world = experiment['world']
        self.agent = experiment['agent']
        
        # Setup visualization
        self.visualizer = NLMVisualizer(backend="console")
        experiment.set_visualizer(self.visualizer)
        
        return self
    
    def run(self, num_steps, reward_function=None):
        self.brain.initialize()
        self.world.reset()
        self.agent.initialize(self.world)
        
        for step in range(num_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward
            if reward_function:
                reward = reward_function(self.world, self.brain, action, step)
                self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Update visualization
            if self.visualizer:
                self.visualizer.show_simulation_progress(
                    step + 1, num_steps,
                    {'firing_rate': self.brain.get_average_firing_rate()}
                )
        
        # Collect results
        self.results = {
            'final_firing_rate': self.brain.get_average_firing_rate(),
            'total_spikes': self.brain.get_total_spikes(),
            'active_neurons': self.brain.get_active_neurons(),
            'steps_completed': num_steps
        }
        
        return self.results
```

## License

See the LICENSE file in the root directory for full license information.

## Version

- Python API Version: 1.0.0
- Documentation Version: 1.0.0
- Last Updated: 2026-09-27

## Contact

For questions, issues, or feature requests, please refer to the NLM documentation or issue tracker.