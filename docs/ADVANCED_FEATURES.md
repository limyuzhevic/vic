# NLM Advanced Features and API Documentation

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) advanced features, including enhanced Python utilities and API extensions.

## Quick Start

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run basic simulation
for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Advanced Usage

```python
import pynlm
from advanced_nlm_tools import (
    NLMExperimentRunner, 
    ExperimentConfig, 
    NLMConfigAnalyzer,
    NLMAnalysisTools
)

# Create advanced configuration
brain_config = {
    'brain.neuron_count': 2000,
    'brain.synapse_density': 0.15,
    'plasticity.stdp.enable': True,
    'neuromod.curiosity.enable': True,
    'neuromod.novelty.enable': True
}

# Create components
brain = pynlm.createBrain(brain_config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create experiment configuration
experiment_config = ExperimentConfig(
    name="Advanced Learning Experiment",
    max_steps=5000,
    timestep=0.1,
    save_checkpoints=True,
    verbose=True
)

# Create experiment runner
runner = NLMExperimentRunner(brain, world, agent)

# Run experiment
result = runner.run_experiment(experiment_config)
print(f"Experiment completed in {result.duration:.2f}s")
print(f"Final firing rate: {result.final_firing_rate:.3f}")
```

## Enhanced API Features

### 1. Experiment Runner (NLMExperimentRunner)

The `NLMExperimentRunner` class provides comprehensive experiment management with advanced control features.

#### Key Features:

- **Simulation Control**: Pause, resume, step modes
- **Callback System**: Event-driven callbacks for monitoring
- **Checkpoint Management**: Automatic and manual checkpointing
- **Performance Metrics**: Real-time and post-experiment analysis
- **Progress Tracking**: Detailed logging and progress monitoring

#### Methods:

```python
# Create experiment runner
runner = NLMExperimentRunner(brain, world, agent)

# Add callbacks
def step_callback(runner, metrics, event_type):
    if metrics['firing_rate'] > 10.0:
        print(f"High activity at step {metrics['step']}")

runner.add_callback(step_callback)

# Run experiment
config = ExperimentConfig(name="MyExperiment", max_steps=1000)
result = runner.run_experiment(config)

# Control simulation
runner.pause()      # Pause simulation
runner.resume()     # Resume simulation
runner.step_mode()  # Step mode (one step at a time)

# Checkpoints
checkpoint = runner.save_checkpoint("experiment_step_100")
runner.load_checkpoint("experiment_step_100")
```

### 2. Configuration Analyzer (NLMConfigAnalyzer)

The `NLMConfigAnalyzer` class provides configuration validation and optimization.

#### Key Features:

- **Configuration Validation**: Check for errors and warnings
- **Parameter Optimization**: Suggest better parameter values
- **Performance Estimation**: Predict experiment performance
- **Configuration Comparison**: Compare multiple configurations

#### Methods:

```python
analyzer = NLMConfigAnalyzer()

# Validate configuration
validation = analyzer.validate_config(config)
print(f"Validation score: {validation['score']}/100")
print(f"Warnings: {validation['warnings']}")
print(f"Suggestions: {validation['suggestions']}")

# Compare configurations
configs = [config1, config2, config3]
comparison = analyzer.compare_configs(configs)
print(f"Best configuration: {comparison['recommendations'][0]}")
```

### 3. Analysis Tools (NLMAnalysisTools)

The `NLMAnalysisTools` class provides advanced analysis capabilities for simulation data.

#### Key Features:

- **Firing Pattern Analysis**: Analyze neural spike patterns
- **Learning Curve Analysis**: Track learning progress
- **Batch Analysis**: Analyze multiple experiments
- **Statistical Analysis**: Statistical comparisons and significance testing

#### Methods:

```python
analyzer = NLMAnalysisTools()

# Analyze firing patterns
spike_times = [0.1, 0.3, 0.5, 0.7, 1.0, 1.2]
pattern_analysis = analyzer.analyze_firing_patterns(spike_times)
print(f"Firing rate: {pattern_analysis['firing_rate']:.3f}")
print(f"Regularity: {pattern_analysis['regularity']:.3f}")

# Analyze learning curves
steps = list(range(0, 1000, 100))
values = [0.1, 0.3, 0.5, 0.7, 0.8, 0.85, 0.9, 0.92, 0.94, 0.95]
learning_curve = analyzer.calculate_learning_curve(steps, values)
print(f"Learning rate: {learning_curve['learning_rate']:.6f}")
print(f"Final improvement: {learning_curve['improvement_percentage']:.2f}%")

# Batch analysis
experiments = [exp1, exp2, exp3]
batch_analysis = analyzer.batch_analyze_experiments(experiments)
print(f"Success rate: {batch_analysis['successful_experiments']/batch_analysis['experiment_count']*100:.1f}%")
```

## Experiment Configuration

### ExperimentConfig Dataclass

```python
@dataclass
class ExperimentConfig:
    name: str
    description: str = ""
    max_steps: int = 1000
    timestep: float = 0.1
    random_seed: int = 42
    verbose: bool = True
    save_checkpoints: bool = False
    checkpoint_interval: int = 100
    callbacks: List[Callable] = field(default_factory=list)
    metadata: Dict[str, Any] = field(default_factory=dict)
```

### Configuration Options:

- **name**: Unique identifier for the experiment
- **description**: Human-readable description
- **max_steps**: Maximum number of simulation steps
- **timestep**: Simulation timestep in seconds
- **random_seed**: Random seed for reproducibility
- **verbose**: Enable detailed logging
- **save_checkpoints**: Automatically save simulation state
- **checkpoint_interval**: Steps between checkpoints
- **callbacks**: Functions to call during simulation
- **metadata**: Additional experiment metadata

## Simulation Control

### States

```python
from advanced_nlm_tools import SimulationState

# Available states
- SimulationState.RUNNING: Simulation is running
- SimulationState.PAUSED: Simulation is paused
- SimulationState.STOPPED: Simulation is stopped
- SimulationState.STEP_MODE: Simulation runs one step at a time
```

### Control Methods

```python
# Check current state
print(f"Current state: {runner.state}")

# State transitions
runner.pause()      # Pause current simulation
runner.resume()     # Resume paused simulation
runner.step_mode()  # Switch to step mode

# Step mode usage
runner.step_mode()
for i in range(100):
    if not runner.step():  # Returns False when simulation should stop
        break
```

## Configuration Analysis

### Validation

The configuration analyzer validates configurations and provides suggestions:

```python
validation = analyzer.validate_config(config)

# Check if configuration is valid
if validation['valid']:
    print("Configuration is valid!")
else:
    print(f"Errors: {validation['errors']}")

# Get performance score
print(f"Performance score: {validation['score']}/100")

# Get suggestions
if validation['suggestions']:
    print("Suggestions:")
    for suggestion in validation['suggestions']:
        print(f"  - {suggestion}")
```

### Comparison

Compare multiple configurations to find the best one:

```python
configs = [
    create_config_1(),
    create_config_2(),
    create_config_3()
]

comparison = analyzer.compare_configs(configs)
best_config = comparison['performance_estimates'][best_key]
print(f"Best configuration has score: {best_config['estimated_performance']}")
print(f"Expected firing rate: {best_config['expected_firing_rate']}")
```

## Analysis Tools

### Firing Pattern Analysis

Analyze neural firing patterns to understand neural dynamics:

```python
spike_times = [0.1, 0.3, 0.5, 0.7, 1.0, 1.2, 1.5, 1.8]
analysis = analyzer.analyze_firing_patterns(spike_times)

print("Firing Pattern Analysis:")
print(f"  Firing Rate: {analysis['firing_rate']:.3f} Hz")
print(f"  Mean ISI: {analysis['mean_isi']:.3f} s")
print(f"  Regularity: {analysis['regularity']:.3f}")
print(f"  Burst Ratio: {analysis['burst_ratio']:.3f}")
print(f"  Peak Frequency: {analysis['peak_frequency']:.1f} Hz")
```

### Learning Curve Analysis

Track learning progress over time:

```python
steps = list(range(0, 1000, 50))
values = [0.1, 0.2, 0.35, 0.45, 0.55, 0.65, 0.72, 0.78, 0.82, 0.85]

learning_curve = analyzer.calculate_learning_curve(steps, values)

print("Learning Curve Analysis:")
print(f"  Initial value: {learning_curve['initial']:.3f}")
print(f"  Final value: {learning_curve['final']:.3f}")
print(f"  Improvement: {learning_curve['improvement_percentage']:.2f}%")
print(f"  Learning rate: {learning_curve['learning_rate']:.6f}")
print(f"  Convergence step: {learning_curve['convergence_step']}")

if learning_curve['plateaus']:
    print(f"  Number of plateaus: {len(learning_curve['plateaus'])}")
    for i, plateau in enumerate(learning_curve['plateaus'][:3]):
        print(f"    Plateau {i+1}: {plateau['value']:.3f} (length: {plateau['length']})")
```

### Batch Analysis

Analyze multiple experiments simultaneously:

```python
experiments = [
    {'name': 'Exp1', 'duration': 10.5, 'success': True, 'config': config1},
    {'name': 'Exp2', 'duration': 15.2, 'success': True, 'config': config2},
    {'name': 'Exp3', 'duration': 8.7, 'success': False, 'config': config3},
    {'name': 'Exp4', 'duration': 12.1, 'success': True, 'config': config4}
]

batch_analysis = analyzer.batch_analyze_experiments(experiments)

print("Batch Analysis Results:")
print(f"  Total experiments: {batch_analysis['experiment_count']}")
print(f"  Successful: {batch_analysis['successful_experiments']}")
print(f"  Success rate: {batch_analysis['successful_experiments']/batch_analysis['experiment_count']*100:.1f}%")
print(f"  Average duration: {batch_analysis['average_duration']:.2f}s")
print(f"  Duration std: {batch_analysis['duration_std']:.2f}s")

if batch_analysis['recommendations']:
    print("\nRecommendations:")
    for recommendation in batch_analysis['recommendations']:
        print(f"  - {recommendation}")
```

## Performance Profiling

### Benchmarking

The advanced tools include benchmarking capabilities:

```python
from advanced_nlm_tools import get_analysis_tools

analyzer = get_analysis_tools()

# Benchmark configuration analysis
import time
start_time = time.time()
for _ in range(100):
    validation = analyzer.validate_config(config)
end_time = time.time()

print(f"Configuration analysis: 100 validations in {end_time - start_time:.3f}s")
print(f"Average time per validation: {(end_time - start_time)/100*1000:.2f}ms")
```

## Integration with Existing API

The advanced tools integrate seamlessly with the existing NLM Python API:

```python
import pynlm
from advanced_nlm_tools import (
    NLMExperimentRunner, 
    ExperimentConfig, 
    NLMConfigAnalyzer,
    NLMAnalysisTools,
    create_advanced_brain_config,
    run_advanced_simulation
)

# Use the enhanced API
def example_advanced_usage():
    # Create advanced configuration
    config = create_advanced_brain_config(
        neuron_count=3000,
        enable_learning=True,
        enable_development=True,
        enable_curiosity=True
    )
    
    # Run advanced simulation
    result = run_advanced_simulation(
        brain_config=config,
        world_config={'width': 50, 'height': 50},
        num_steps=2000
    )
    
    # Analyze results
    analyzer = NLMAnalysisTools()
    learning_curve = analyzer.calculate_learning_curve(
        list(range(0, 2000, 100)),
        [result['experiment_result'].final_firing_rate] * 20
    )
    
    return result

# Run example
experiment_result = example_advanced_usage()
print(f"Experiment completed successfully!")
```

## Error Handling and Debugging

### Error Handling

The advanced tools include comprehensive error handling:

```python
try:
    result = runner.run_experiment(config)
    if not result.success:
        print(f"Experiment failed: {result.error}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Debugging

Enable detailed debugging information:

```python
import logging
logging.basicConfig(level=logging.DEBUG)

# Run experiment with debug logging
result = runner.run_experiment(config)

# Check experiment result
if result.error:
    print(f"Experiment error: {result.error}")
else:
    print(f"Experiment completed successfully!")
    print(f"Checkpoints saved: {len(result.checkpoint_files)}")
```

## Migration Guide

### From Basic to Advanced API

If you're currently using the basic NLM API, here's how to migrate to the advanced features:

#### Basic API

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)
```

#### Advanced API

```python
import pynlm
from advanced_nlm_tools import NLMExperimentRunner, ExperimentConfig

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create experiment configuration
experiment_config = ExperimentConfig(
    name="Advanced Experiment",
    max_steps=100,
    verbose=True
)

# Create experiment runner
runner = NLMExperimentRunner(brain, world, agent)

# Run experiment with advanced features
result = runner.run_experiment(experiment_config)
```

### Key Benefits of Advanced API:

- **Better Control**: Pause, resume, step modes
- **Monitoring**: Real-time metrics and callbacks
- **Persistence**: Checkpoint management
- **Analysis**: Built-in analysis tools
- **Validation**: Configuration validation and optimization
- **Debugging**: Enhanced error handling and logging

## Performance Considerations

### Memory Usage

The advanced tools use additional memory for:
- Metrics history
- Callback management
- Checkpoint data
- Analysis buffers

### CPU Usage

The advanced tools add CPU overhead for:
- Metrics collection and analysis
- Callback execution
- Checkpoint serialization
- Performance profiling

### Recommendations

- Use checkpoints only when necessary
- Limit callback complexity
- Monitor memory usage in long-running experiments
- Use profiling to identify performance bottlenecks

## Troubleshooting

### Common Issues

1. **"Configuration validation failed"**
   ```python
   # Solution: Check validation warnings and fix issues
   validation = analyzer.validate_config(config)
   print(f"Warnings: {validation['warnings']}")
   ```

2. **"Experiment paused indefinitely"**
   ```python
   # Solution: Resume or continue to step mode
   runner.resume()
   # or
   runner.step_mode()
   ```

3. **"Memory usage too high"**
   ```python
   # Solution: Reduce checkpoint frequency or disable verbose logging
   config = ExperimentConfig(max_steps=1000, save_checkpoints=False, verbose=False)
   ```

4. **"No firing neurons detected"**
   ```python
   # Solution: Check brain initialization and parameters
   print(f"Total neurons: {brain.getTotalNeuronCount()}")
   print(f"Injection current: Check injection parameters")
   ```

### Getting Help

- **Documentation**: See this document for detailed API reference
- **Examples**: Check the example scripts in the repository
- **GitHub Issues**: Report bugs and feature requests
- **Community**: Join the NLM discussion forums

## Future Enhancements

The advanced tools are designed to be extensible. Future enhancements may include:

- **Machine Learning Integration**: Automated hyperparameter optimization
- **Real-time Visualization**: Interactive plots and dashboards
- **Cloud Integration**: Distributed computing and cloud deployment
- **Model Zoo**: Pre-trained models and architectures
- **Advanced Analytics**: Machine learning for pattern detection
- **Simulation Orchestration**: Multi-agent system management

## Conclusion

The NLM advanced tools provide a comprehensive suite of features for working with neural simulations, including:

- **Enhanced Control**: Pause, resume, step modes
- **Advanced Analysis**: Configuration validation, firing pattern analysis, learning curve analysis
- **Experiment Management**: Checkpoints, callbacks, metrics collection
- **Performance Optimization**: Profiling, benchmarking, memory management
- **Integration**: Seamless integration with existing NLM API

These tools make it easier to run complex neural simulations, analyze results, and optimize configurations for better performance.

For the latest updates and documentation, visit the NLM project repository.
