# Advanced NLM Python API Documentation

## Overview

The Advanced NLM Python API extends the core NLM functionality with sophisticated tools for complex experimentation, data analysis, and simulation control. This documentation provides comprehensive guidance for advanced users working with neural learning machine simulations.

## Key Features

### 1. Advanced Experiment Control (`NLMExperiment`)

#### Stimulus-Response Protocols
- **Multi-modal stimulus presentation**: Vision, touch, internal signals, and proprioception
- **Adaptive timing and duration control**: Precise scheduling of experimental events
- **Response data collection**: High-frequency recording of neural responses
- **Real-time stimulus application**: Dynamic stimulus presentation during simulation

#### Learning Curve Experiments
- **Adaptive difficulty adjustment**: Automatic difficulty scaling based on performance
- **Multi-trial experimental design**: Long-running learning experiments with statistical power
- **Reward function integration**: Custom reward structures for different task types
- **Performance tracking**: Detailed metrics collection for learning analysis

#### Developmental Trajectory Tracking
- **Age-stage simulation**: Tracking brain development across different life stages
- **Multi-metric analysis**: Concurrent measurement of multiple neural and behavioral metrics
- **Comparative developmental analysis**: Cross-age comparison of brain capabilities

#### Memory Replay Protocols
- **Consolidation and replay control**: Configurable replay intervals and strength
- **Multiple episode replay**: Simultaneous replay of multiple memory traces
- **Replay effect analysis**: Quantitative assessment of replay benefits

### 2. Neural Data Analysis (`NLMAnalyzer`)

#### Spike Dynamics Analysis
- **Firing rate calculations**: Population and single-neuron firing statistics
- **Burst pattern detection**: Identification of coordinated neural bursting
- **Synchrony measurements**: Quantification of neural population coordination
- **Temporal pattern analysis**: Detection of rhythmic and oscillatory patterns

#### Network Topology Analysis
- **Connectivity matrix construction**: Complete network connectivity mapping
- **Graph theory metrics**: Degree distribution, clustering, path length analysis
- **Community detection**: Identification of functionally segregated modules
- **Network robustness analysis**: Assessment of network resilience to perturbations

#### Learning Trajectory Modeling
- **Model fitting and validation**: Statistical comparison of different learning models
- **Parameter estimation**: Quantitative extraction of learning dynamics
- **Goodness-of-fit analysis**: Model selection and validation metrics
- **Cross-validation**: Robustness assessment of learning models

### 3. Simulation Control (`NLMSimulationController`)

#### Real-Time Monitoring
- **Progress tracking**: Step-by-step simulation monitoring
- **Performance metrics collection**: Computational resource usage and efficiency metrics
- **Checkpoint management**: Automated simulation state saving and recovery
- **Real-time visualization**: Live display of simulation progress and results

#### Advanced Control Features
- **Multi-threaded execution**: Parallel simulation and analysis
- **Speed control**: Adjustable simulation speed for different experimental needs
- **Pause and resume**: Flexible simulation interruption and continuation
- **Event-driven control**: Trigger-based simulation control

### 4. Experimental Setup Utilities

#### Advanced Configuration
- **Optimized parameter sets**: Pre-configured parameter sets for different experimental regimes
- **Dynamic configuration**: Runtime configuration modification
- **Parameter validation**: Automatic checking of parameter consistency
- **Configuration serialization**: Save and load experimental configurations

#### Integration with Core NLM
- **Seamless compatibility**: Full integration with existing NLM brain and world objects
- **Advanced API features**: Enhanced access to neural system capabilities
- **Experimental protocol templates**: Ready-to-use experimental templates
- **Reproducibility tools**: Version control and data management for reproducible research

## API Reference

### NLMExperiment Class

#### Methods

**`run_stimulus_response_protocol(stimuli, response_window, recording_interval)`**

Run a stimulus-response protocol with sophisticated control over stimulus presentation and response recording.

**Parameters**
- `stimuli`: List of stimulus dictionaries with keys: `type`, `data`, `duration`, `timing`
- `response_window`: Number of simulation steps to record after stimulus presentation
- `recording_interval`: Frequency of response recording (every N steps)

**Returns**: Dictionary containing response analysis and statistics

**`run_learning_curve_experiment(task_params, num_trials, recording_frequency)`**

Run a learning curve experiment with adaptive difficulty and comprehensive performance tracking.

**Parameters**
- `task_params`: Task configuration dictionary
- `num_trials`: Number of experimental trials to run
- `recording_frequency`: How often to record performance data

**Returns**: Dictionary containing learning progress and analysis

**`run_development_experiment(age_stages, metrics)`**

Track brain development across different age stages.

**Parameters**
- `age_stages`: List of developmental ages to simulate
- `metrics`: List of neural/behavioral metrics to track

**Returns**: Dictionary containing developmental trajectories

**`create_replay_protocol(replay_episodes, replay_interval, replay_strength)`**

Create and run a memory replay protocol.

**Parameters**
- `replay_episodes`: Number of memory episodes to replay
- `replay_interval`: How often to trigger replay
- `replay_strength`: Strength of replay effects

**Returns**: Dictionary containing replay analysis

**`save_experiment_data(filepath, format)`**
Load experiment data from file.

**Parameters**
- `filepath`: Path to save data file
- `format`: File format ('json' or 'npy')

**`load_experiment_data(filepath)`**
Load experiment data from file.

**Parameters**
- `filepath`: Path to load data from

### NLMAnalyzer Class

#### Static Methods

**`analyze_neural_dynamics(spike_data, window_size)`**

Analyze neural spike dynamics.

**Parameters**
- `spike_data`: List of spike event dictionaries
- `window_size`: Analysis window size in steps

**Returns**: Dictionary containing firing rate analysis, burst patterns, and synchrony measures

**`analyze_network_topology(neural_regions)`**

Analyze network connectivity and structure.

**Parameters**
- `neural_regions`: List of neural region objects

**Returns**: Dictionary containing connectivity matrix and network metrics

**`analyze_learning_trajectory(performance_data, learning_model)`**

Analyze learning curves with model fitting.

**Parameters**
- `performance_data`: List of performance measurements
- `learning_model`: Type of learning model to fit ('exponential', 'power_law', 'logarithmic')

**Returns**: Dictionary containing model parameters and goodness-of-fit metrics

### NLMSimulationController Class

#### Methods

**`start_simulation(max_steps, speed_factor, enable_monitoring)`**

Start a simulation with advanced control options.

**Parameters**
- `max_steps`: Maximum number of simulation steps
- `speed_factor`: Speed multiplier for visualization
- `enable_monitoring`: Enable real-time monitoring

**`pause_simulation()`**
Pause the current simulation.

**`resume_simulation()`**
Resume a paused simulation.

**`stop_simulation()`**
Stop the current simulation.

**`step_simulation(steps)`**
Advance simulation by specified steps.

**Parameters**
- `steps`: Number of steps to advance

**Returns**: Dictionary containing step results and statistics

### Convenience Functions

**`create_advanced_brain(config_dict)`**
Create an advanced NLM brain with optimized configuration.

**Parameters**
- `config_dict`: Configuration dictionary with optimization parameters

**`create_advanced_world(config_dict)`**
Create an advanced NLM world with enhanced features.

**Parameters**
- `config_dict`: Configuration dictionary for world parameters

**`create_experimental_setup(config_dict)`**
Create a complete experimental setup.

**Parameters**
- `config_dict`: Complete configuration dictionary

**Returns**: Tuple of (brain, world, experiment)

## Usage Examples

### Basic Advanced Experiment
```python
from advanced_nlm_utils import create_experimental_setup

# Create advanced experimental setup
config = {
    'neuron_count': 2000,
    'region_count': 3,
    'connection_probability': 0.15
}

brain, world, experiment = create_experimental_setup(config)

# Design sophisticated stimulus protocol
stimuli = [
    {'type': 'vision', 'data': np.random.rand(256), 'timing': 0},
    {'type': 'touch', 'data': np.random.rand(8), 'timing': 100}
]

# Run advanced experiment
results = experiment.run_stimulus_response_protocol(
    stimuli=stimuli, 
    response_window=200, 
    recording_interval=10
)
```

### Learning Curve Analysis
```python
from advanced_nlm_utils import create_experimental_setup

# Create experimental setup
brain, world, experiment = create_experimental_setup({})

# Run adaptive learning experiment
task_params = {
    'task_type': 'navigation',
    'reward_function': 'proportional'
}

learning_results = experiment.run_learning_curve_experiment(
    task_params=task_params,
    num_trials=50,
    recording_frequency=3
)
```

### Neural Data Analysis
```python
from advanced_nlm_utils import NLMAnalyzer

# Create sample spike data
spike_data = [
    {'neuron_id': 0, 'time': 0, 'amplitude': 0.5},
    {'neuron_id': 1, 'time': 10, 'amplitude': 0.8},
    # ... more data
]

# Analyze neural dynamics
dynamics = NLMAnalyzer.analyze_neural_dynamics(spike_data)

# Analyze network topology
class MockRegion:
    def __init__(self, id, neurons):
        self.id = id
        self.neuron_count = neurons

regions = [MockRegion(i, 100) for i in range(5)]
topology = NLMAnalyzer.analyze_network_topology(regions)
```

### Real-Time Simulation Control
```python
from advanced_nlm_utils import create_experimental_setup

# Create experimental setup
brain, world, experiment = create_experimental_setup({})

# Create simulation controller
from advanced_nlm_utils import NLMSimulationController
controller = NLMSimulationController(brain, world)

# Start simulation with monitoring
controller.start_simulation(max_steps=500, enable_monitoring=True)

# Run simulation with real-time monitoring
while controller.simulation_state == 'running':
    step_results = controller.step_simulation(steps=50)
    # ... process results in real-time

# Stop simulation
controller.stop_simulation()
```

## Performance Considerations

### Memory Management
- Large-scale simulations: Monitor memory usage and implement efficient checkpointing
- Data storage: Use compressed formats for large datasets
- Garbage collection: Implement periodic cleanup for long-running experiments

### Computational Efficiency
- Vectorization: Use numpy operations for efficient array processing
- Parallel processing: Leverage multi-threading for independent computations
- Caching: Implement result caching for repeated analyses
- Subset analysis: Analyze data subsets when full dataset is too large

### Storage and I/O
- Data compression: Use efficient formats for large datasets
- Streaming output: Implement incremental data writing for long experiments
- Database integration: Consider database solutions for complex experimental metadata
- Backup strategies: Implement automatic backup of experimental data

## Troubleshooting

### Common Issues

**Memory Usage Too High**
- Solution: Use more efficient data structures and implement checkpointing
- Solution: Process data in chunks rather than loading everything at once

**Simulation Running Too Slow**
- Solution: Reduce recording frequency and increase timestep
- Solution: Use more efficient configuration parameters
- Solution: Implement parallel processing where possible

**Data Analysis Taking Too Long**
- Solution: Use vectorized operations and efficient algorithms
- Solution: Implement subset analysis for large datasets
- Solution: Use approximate methods when exact results are not required

### Error Messages

- **File not found**: Check file paths and permissions
- **Memory error**: Reduce data size or increase system memory
- **Configuration error**: Check configuration parameters and their types
- **Simulation state error**: Check simulation state before performing operations

## Future Enhancements

### Planned Features
- **Machine Learning Integration**: Automated feature extraction and model training
- **Advanced Visualization**: Real-time neural activity visualization and animation
- **Cloud Computing**: Distributed simulation across multiple machines
- **Mobile Integration**: Portable experimental protocols for field work
- **Hardware Integration**: Direct control of real neural systems

### Research Integration
- **ReProducible Research**: Built-in version control and data management
- **Collaboration Tools**: Multi-user experimental sessions and shared analysis
- **Peer Review**: Automated validation of experimental methods and results
- **Data Sharing**: Standardized formats for data sharing across research groups

## Conclusion

The Advanced NLM Python API provides comprehensive tools for sophisticated neural learning machine research. From complex experimental design to advanced data analysis, these tools enable researchers to conduct cutting-edge experiments with unprecedented flexibility and power.

The API is designed to be both accessible for beginners and powerful for experts, with extensive documentation and examples to guide users through the capabilities of the system.

For the latest updates, examples, and community support, visit the NLM project documentation and forums.
