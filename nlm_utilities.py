# NLM Python Utilities for Advanced Users

Advanced utilities for working with NLM simulations, providing enhanced control, analysis, and debugging capabilities for experienced users.

## Overview

The NLM Python Utilities extend the basic `pynlm` interface with:
- Advanced brain manipulation and introspection
- Memory system control and analysis
- Plasticity rule customization and monitoring
- Simulation state capture and analysis
- Performance monitoring and optimization
- Debug and visualization tools
- Export/import utilities for data analysis

## Installation

```bash
pip install nlm-utilities
```

## Core Utilities

### Brain Manipulation Tools

```python
from nlm_utilities import BrainUtils, MemoryUtils, PlasticityUtils, SimulationUtils

# Create and configure brain with advanced settings
brain = BrainUtils.create_advanced_brain(
    neuron_count=5000,
    region_count=10,
    connection_probability=0.15,
    enable_advanced_plasticity=True,
    enable_neuromodulation=True,
    enable_memory_systems=True
)

# Access and manipulate neural components
utils = BrainUtils(brain)
utils.inject_pattern_to_region("sensory", [0.5, 0.8, 0.3, 0.9, 0.2])
utils.set_neuron_properties(0, {
    'type': 'sensory',
    'threshold': 1.0,
    'refractory_period': 20,
    'learning_rate': 0.02
})
```

### Memory System Control

```python
from nlm_utilities import MemoryUtils

# Memory system control
memory_utils = MemoryUtils(brain)

# Working memory control
memory_utils.enable_working_memory(True)
memory_utils.set_working_memory_capacity(100)
memory_utils.set_trace_decay_rate(0.95)
memory_utils.set_competition_strength(0.7)

# Episodic memory control
memory_utils.enable_episodic_memory(True)
memory_utils.set_storage_capacity(10000)
memory_utils.set_encoding_frequency(5)
memory_utils.set_replay_enabled(True)
memory_utils.set_replay_interval(25)

# Memory introspection
working_memory_traces = memory_utils.get_working_memory_traces()
episodic_episodes = memory_utils.get_episodic_episodes()
memory_utilization = memory_utils.get_memory_utilization()
memory_efficiency = memory_utils.calculate_memory_efficiency()
```

### Plasticity Control

```python
from nlm_utilities import PlasticityUtils

# Plasticity system control
plasticity_utils = PlasticityUtils(brain)

# STDP control
plasticity_utils.enable_stdp(True)
plasticity_utils.set_stdp_parameters(
    ltp_weight=0.025,
    ltd_weight=0.018,
    time_constant=25.0,
    neuromodulation_sensitive=True
)

# Hebbian control
plasticity_utils.enable_hebbian(True)
plasticity_utils.set_hebbian_parameters(
    learning_rate=0.015,
    eligibility_trace_duration=50,
    presynaptic_modulation=True
)

# Structural plasticity control
plasticity_utils.enable_structural_plasticity(True)
plasticity_utils.set_synaptogenesis_rate(0.0003)
plasticity_utils.set_pruning_rate(0.00007)

# Plasticity analysis
plasticity_stats = plasticity_utils.get_plasticity_statistics()
learning_curve = plasticity_utils.get_learning_curve(steps=1000)
synapse_heatmap = plasticity_utils.get_synapse_weight_distribution()
```

### Advanced Simulation Control

```python
from nlm_utilities import SimulationUtils

# Advanced simulation control
sim_utils = SimulationUtils(brain)

# Simulation timing and control
sim_utils.set_timestep(0.001)
sim_utils.set_max_steps(10000)
sim_utils.set_automatic_checkpoints(True)
sim_utils.set_checkpoint_interval(1000)

# Stimulus control
sim_utils.clear_all_inputs()
sim_utils.inject_custom_stimulus([
    {'type': 'sensory', 'neuron': 0, 'value': 0.8},
    {'type': 'motor', 'neuron': 1, 'value': 0.5}
])
sim_utils.schedule_stimulus(
    time=1.0,
    stimulus={'type': 'pattern', 'neurons': [2, 5, 8], 'values': [0.9, 0.7, 0.8]}
)

# Performance monitoring
sim_utils.enable_performance_monitoring(True)
sim_utils.set_monitoring_interval(100)
performance_data = sim_utils.get_performance_metrics()
simulation_efficiency = sim_utils.calculate_efficiency()
```

## Analysis Tools

### Neural Activity Analysis

```python
from nlm_utilities import AnalysisUtils

# Neural activity analysis
analysis_utils = AnalysisUtils(brain)

# Spike analysis
spike_statistics = analysis_utils.analyze_spikes()
spike_patterns = analysis_utils.detect_spike_patterns()
spike_coherence = analysis_utils.calculate_spike_coherence()

# Neuronal dynamics analysis
neuron_dynamics = analysis_utils.analyze_neuron_dynamics()
network_activity = analysis_utils.analyze_network_activity()
functional_connectivity = analysis_utils.calculate_functional_connectivity()

# Temporal dynamics analysis
temporal_dynamics = analysis_utils.analyze_temporal_dynamics()
responsiveness = analysis_utils.calculate_neuron_responsiveness()
adaptation = analysis_utils.calculate_adaptation_rates()
```

### Memory Analysis

```python
# Memory analysis
memory_analysis = analysis_utils.analyze_memory_systems()
working_memory_performance = analysis_utils.analyze_working_memory_performance()
episodic_memory_quality = analysis_utils.analyze_episodic_memory_quality()
consolidation_effectiveness = analysis_utils.analyze_consolidation_effectiveness()
```

### Plasticity Analysis

```python
# Plasticity analysis
plasticity_analysis = analysis_utils.analyze_plasticity()
learning_efficiency = analysis_utils.analyze_learning_efficiency()
plasticity_stability = analysis_utils.analyze_plasticity_stability()
pattern_specificity = analysis_utils.analyze_pattern_specificity()
```

## Brain State Management

### State Capture and Export

```python
from nlm_utilities import StateUtils

# Brain state management
state_utils = StateUtils(brain)

# Capture brain state at specific timepoints
brain_state = state_utils.capture_brain_state(step=500)
state_utils.capture_state_sequence(steps=[100, 500, 1000, 2000])

# Export brain state
state_utils.export_brain_state(brain_state, 'brain_state_500.pickle')
state_utils.export_neural_state(brain_state, 'neural_state.hdf5')
state_utils.export_plasticity_state(brain_state, 'plasticity_state.json')

# Import brain state
imported_state = state_utils.import_brain_state('brain_state_500.pickle')
imported_brain = state_utils.restore_brain_state(imported_state)
```

### State Comparison

```python
# Compare brain states
state_comparison = state_utils.compare_brain_states(
    state1='brain_state_100.pickle',
    state2='brain_state_500.pickle'
)

# Analyze state changes
state_changes = state_utils.analyze_state_changes(state_comparison)
state_similarity = state_utils.calculate_state_similarity(state1, state2)
state_divergence = state_utils.calculate_state_divergence(state1, state2)
```

## Debug and Visualization Tools

### Debug Utilities

```python
from nlm_utilities import DebugUtils

# Debug utilities
debug_utils = DebugUtils(brain)

# Brain consistency check
consistency_report = debug_utils.check_brain_consistency()

# Parameter validation
validation_report = debug_utils.validate_parameters()

# Memory integrity check
memory_integrity = debug_utils.check_memory_integrity()

# Plasticity consistency check
plasticity_consistency = debug_utils.check_plasticity_consistency()

# Performance profiling
profile_data = debug_utils.run_performance_profile(steps=1000)
```

### Visualization Utilities

```python
from nlm_utilities import VisualizationUtils

# Visualization utilities
viz_utils = VisualizationUtils(brain)

# Neural network visualization
network_graph = viz_utils.create_neural_network_graph()
network_metrics = viz_utils.calculate_network_metrics()

# Activity visualization
activity_heatmap = viz_utils.create_activity_heatmap()
spike_raster = viz_utils.create_spike_raster_plot()
temporal_activity = viz_utils.create_temporal_activity_plot()

# Memory visualization
memory_heatmap = viz_utils.create_memory_heatmap()
episodic_trace = viz_utils.create_episodic_trace_plot()
working_memory_plot = viz_utils.create_working_memory_plot()

# Plasticity visualization
plasticity_dynamics = viz_utils.create_plasticity_dynamics_plot()
learning_curves = viz_utils.create_learning_curves()
weight_distributions = viz_utils.create_weight_distributions()
```

## Performance Optimization

### Performance Monitoring

```python
from nlm_utilities import PerformanceUtils

# Performance monitoring
perf_utils = PerformanceUtils(brain)

# Real-time performance monitoring
perf_utils.start_real_time_monitoring()
perf_utils.set_monitoring_thresholds(cpu=80.0, memory=2.0, time=60.0)
performance_alerts = perf_utils.get_performance_alerts()

# Performance profiling
profile_data = perf_utils.profile_simulation(steps=5000)
performance_report = perf_utils.generate_performance_report()
bottleneck_analysis = perf_utils.analyze_bottlenecks()

# Optimization recommendations
optimization_suggestions = perf_utils.get_optimization_suggestions()
perf_utils.apply_optimization_suggestions(optimization_suggestions)
```

### Resource Management

```python
# Resource management
resource_utils = ResourceUtils(brain)

# Memory management
memory_stats = resource_utils.get_memory_statistics()
memory_recommendations = resource_utils.get_memory_optimization_recommendations()
resource_utils.optimize_memory_usage()

# CPU optimization
cpu_stats = resource_utils.get_cpu_statistics()
cpu_recommendations = resource_utils.get_cpu_optimization_recommendations()
resource_utils.optimize_cpu_usage()

# Network optimization (for distributed simulations)
if resource_utils.is_distributed_simulation():
    network_stats = resource_utils.get_network_statistics()
    network_optimization = resource_utils.optimize_network_performance()
```

## Export/Import Tools

### Data Export

```python
from nlm_utilities import ExportUtils

# Data export
export_utils = ExportUtils(brain)

# Export neural data
export_utils.export_neural_data('neural_data.csv')
export_utils.export_spike_data('spike_data.npz')
export_utils.export_connectivity_matrix('connectivity_matrix.npy')

# Export memory data
export_utils.export_working_memory_data('working_memory.csv')
export_utils.export_episodic_memory_data('episodic_memory.json')
export_utils.export_memory_traces('memory_traces.hdf5')

# Export plasticity data
export_utils.export_plasticity_data('plasticity_data.csv')
export_utils.export_synapse_weights('synapse_weights.hdf5')
export_utils.export_stdp_history('stdp_history.json')

# Export analysis results
export_utils.export_analysis_results('analysis_results.json')
export_utils.export_performance_metrics('performance_metrics.csv')
export_utils.export_visualization_data('visualization_data.mat')
```

### Data Import

```python
# Data import
import_utils = ImportUtils()

# Import neural data
neural_data = import_utils.import_neural_data('neural_data.csv')
spike_data = import_utils.import_spike_data('spike_data.npz')
connectivity_matrix = import_utils.import_connectivity_matrix('connectivity_matrix.npy')

# Import memory data
working_memory_data = import_utils.import_working_memory_data('working_memory.csv')
episodic_memory_data = import_utils.import_episodic_memory_data('episodic_memory.json')
memory_traces = import_utils.import_memory_traces('memory_traces.hdf5')

# Import plasticity data
plasticity_data = import_utils.import_plasticity_data('plasticity_data.csv')
synapse_weights = import_utils.import_synapse_weights('synapse_weights.hdf5')
stdp_history = import_utils.import_stdp_history('stdp_history.json')
```

## Command Line Tools

### Utility Command Line Interface

```bash
# Brain utilities
./nlm_brain_utils --create-advanced --neurons 5000 --regions 10
./nlm_brain_utils --validate-configuration config.cfg
./nlm_brain_utils --export-brain-state brain_state.pickle
./nlm_brain_utils --import-brain-state brain_state.pickle

# Memory utilities
./nlm_memory_utils --enable-working-memory --capacity 100
./nlm_memory_utils --enable-episodic-memory --storage 10000
./nlm_memory_utils --analyze-memory-performance
./nlm_memory_utils --export-memory-data memory_data.csv

# Plasticity utilities
./nlm_plasticity_utils --enable-stdp --ltp-weight 0.025 --ltd-weight 0.018
./nlm_plasticity_utils --enable-hebbian --learning-rate 0.015
./nlm_plasticity_utils --analyze-plasticity --output plasticity_report.json
./nlm_plasticity_utils --export-plasticity-data plasticity_data.csv

# Simulation utilities
./nlm_simulation_utils --set-timestep 0.001 --max-steps 10000
./nlm_simulation_utils --inject-stimulus --pattern sensory [0.5,0.8,0.3] --time 1.0
./nlm_simulation_utils --monitor-performance --interval 100
./nlm_simulation_utils --profile-simulation --steps 5000

# Analysis utilities
./nlm_analysis_utils --analyze-spikes --output spike_analysis.json
./nlm_analysis_utils --analyze-neural-dynamics --output dynamics_analysis.csv
./nlm_analysis_utils --analyze-memory --output memory_analysis.json
./nlm_analysis_utils --analyze-plasticity --output plasticity_analysis.json

# Visualization utilities
./nlm_visualization_utils --create-network-graph --output network.png
./nlm_visualization_utils --create-activity-heatmap --output activity_heatmap.png
./nlm_visualization_utils --create-learning-curves --output learning_curves.png
./nlm_visualization_utils --create-plasticity-dynamics --output plasticity_dynamics.png

# Export utilities
./nlm_export_utils --export-all --output-dir simulation_results/
./nlm_export_utils --export-neural-data --format csv --output neural_data.csv
./nlm_export_utils --export-memory-data --format json --output memory_data.json
./nlm_export_utils --export-plasticity-data --format hdf5 --output plasticity_data.hdf5
```

## Python API Reference

### Core Classes

#### BrainUtils
Brain manipulation and configuration utilities.

#### MemoryUtils
Memory system control and analysis utilities.

#### PlasticityUtils
Plasticity system control and analysis utilities.

#### SimulationUtils
Simulation control and performance monitoring utilities.

#### AnalysisUtils
Neural, memory, and plasticity analysis utilities.

#### StateUtils
Brain state capture, export, and import utilities.

#### DebugUtils
Brain debugging and validation utilities.

#### VisualizationUtils
Brain visualization utilities.

#### PerformanceUtils
Performance monitoring and optimization utilities.

#### ResourceUtils
Resource management utilities.

#### ExportUtils
Data export utilities.

#### ImportUtils
Data import utilities.

### Core Methods

#### BrainUtils
- `create_advanced_brain(neuron_count, region_count, connection_probability, ...)` - Create advanced brain
- `create_research_brain(config)` - Create research brain from configuration
- `create_real_time_brain(config)` - Create real-time brain from configuration
- `create_large_scale_brain(config)` - Create large-scale brain from configuration
- `inject_pattern_to_region(region_type, pattern)` - Inject pattern to region
- `set_neuron_properties(neuron_id, properties)` - Set neuron properties
- `configure_plasticity(stdp_config, hebbian_config, structural_config)` - Configure plasticity
- `configure_neuromodulation(neuromodulation_config)` - Configure neuromodulation
- `configure_memory_systems(memory_config)` - Configure memory systems

#### MemoryUtils
- `enable_working_memory(enabled)` - Enable working memory
- `set_working_memory_capacity(capacity)` - Set working memory capacity
- `set_trace_decay_rate(decay_rate)` - Set trace decay rate
- `set_competition_strength(strength)` - Set competition strength
- `enable_episodic_memory(enabled)` - Enable episodic memory
- `set_storage_capacity(capacity)` - Set storage capacity
- `set_encoding_frequency(frequency)` - Set encoding frequency
- `set_replay_enabled(enabled)` - Enable replay
- `set_replay_interval(interval)` - Set replay interval
- `get_working_memory_traces()` - Get working memory traces
- `get_episodic_episodes()` - Get episodic episodes
- `get_memory_utilization()` - Get memory utilization
- `calculate_memory_efficiency()` - Calculate memory efficiency

#### PlasticityUtils
- `enable_stdp(enabled)` - Enable STDP
- `set_stdp_parameters(**kwargs)` - Set STDP parameters
- `enable_hebbian(enabled)` - Enable Hebbian
- `set_hebbian_parameters(**kwargs)` - Set Hebbian parameters
- `enable_structural_plasticity(enabled)` - Enable structural plasticity
- `set_synaptogenesis_rate(rate)` - Set synaptogenesis rate
- `set_pruning_rate(rate)` - Set pruning rate
- `get_plasticity_statistics()` - Get plasticity statistics
- `get_learning_curve(steps)` - Get learning curve
- `get_synapse_weight_distribution()` - Get synapse weight distribution

#### SimulationUtils
- `set_timestep(timestep)` - Set timestep
- `set_max_steps(max_steps)` - Set max steps
- `set_automatic_checkpoints(enabled)` - Set automatic checkpoints
- `set_checkpoint_interval(interval)` - Set checkpoint interval
- `clear_all_inputs()` - Clear all inputs
- `inject_custom_stimulus(stimuli)` - Inject custom stimulus
- `schedule_stimulus(time, stimulus)` - Schedule stimulus
- `enable_performance_monitoring(enabled)` - Enable performance monitoring
- `set_monitoring_interval(interval)` - Set monitoring interval
- `get_performance_metrics()` - Get performance metrics
- `calculate_efficiency()` - Calculate efficiency

#### AnalysisUtils
- `analyze_spikes()` - Analyze spikes
- `detect_spike_patterns()` - Detect spike patterns
- `calculate_spike_coherence()` - Calculate spike coherence
- `analyze_neuron_dynamics()` - Analyze neuron dynamics
- `analyze_network_activity()` - Analyze network activity
- `calculate_functional_connectivity()` - Calculate functional connectivity
- `analyze_temporal_dynamics()` - Analyze temporal dynamics
- `calculate_neuron_responsiveness()` - Calculate neuron responsiveness
- `calculate_adaptation_rates()` - Calculate adaptation rates
- `analyze_memory_systems()` - Analyze memory systems
- `analyze_working_memory_performance()` - Analyze working memory performance
- `analyze_episodic_memory_quality()` - Analyze episodic memory quality
- `analyze_consolidation_effectiveness()` - Analyze consolidation effectiveness
- `analyze_plasticity()` - Analyze plasticity
- `analyze_learning_efficiency()` - Analyze learning efficiency
- `analyze_plasticity_stability()` - Analyze plasticity stability
- `analyze_pattern_specificity()` - Analyze pattern specificity

#### StateUtils
- `capture_brain_state(step)` - Capture brain state
- `capture_state_sequence(steps)` - Capture state sequence
- `export_brain_state(state, filename)` - Export brain state
- `export_neural_state(state, filename)` - Export neural state
- `export_plasticity_state(state, filename)` - Export plasticity state
- `import_brain_state(filename)` - Import brain state
- `restore_brain_state(state)` - Restore brain state
- `compare_brain_states(state1, state2)` - Compare brain states
- `analyze_state_changes(comparison)` - Analyze state changes
- `calculate_state_similarity(state1, state2)` - Calculate state similarity
- `calculate_state_divergence(state1, state2)` - Calculate state divergence

#### DebugUtils
- `check_brain_consistency()` - Check brain consistency
- `validate_parameters()` - Validate parameters
- `check_memory_integrity()` - Check memory integrity
- `check_plasticity_consistency()` - Check plasticity consistency
- `run_performance_profile(steps)` - Run performance profile

#### VisualizationUtils
- `create_neural_network_graph()` - Create neural network graph
- `calculate_network_metrics()` - Calculate network metrics
- `create_activity_heatmap()` - Create activity heatmap
- `create_spike_raster_plot()` - Create spike raster plot
- `create_temporal_activity_plot()` - Create temporal activity plot
- `create_memory_heatmap()` - Create memory heatmap
- `create_episodic_trace_plot()` - Create episodic trace plot
- `create_working_memory_plot()` - Create working memory plot
- `create_plasticity_dynamics_plot()` - Create plasticity dynamics plot
- `create_learning_curves()` - Create learning curves
- `create_weight_distributions()` - Create weight distributions

#### PerformanceUtils
- `start_real_time_monitoring()` - Start real-time monitoring
- `set_monitoring_thresholds(cpu, memory, time)` - Set monitoring thresholds
- `get_performance_alerts()` - Get performance alerts
- `profile_simulation(steps)` - Profile simulation
- `generate_performance_report()` - Generate performance report
- `analyze_bottlenecks()` - Analyze bottlenecks
- `get_optimization_suggestions()` - Get optimization suggestions
- `apply_optimization_suggestions(suggestions)` - Apply optimization suggestions

#### ResourceUtils
- `get_memory_statistics()` - Get memory statistics
- `get_memory_optimization_recommendations()` - Get memory optimization recommendations
- `optimize_memory_usage()` - Optimize memory usage
- `get_cpu_statistics()` - Get CPU statistics
- `get_cpu_optimization_recommendations()` - Get CPU optimization recommendations
- `optimize_cpu_usage()` - Optimize CPU usage
- `is_distributed_simulation()` - Check if distributed simulation
- `get_network_statistics()` - Get network statistics
- `optimize_network_performance()` - Optimize network performance

#### ExportUtils
- `export_neural_data(filename)` - Export neural data
- `export_spike_data(filename)` - Export spike data
- `export_connectivity_matrix(filename)` - Export connectivity matrix
- `export_working_memory_data(filename)` - Export working memory data
- `export_episodic_memory_data(filename)` - Export episodic memory data
- `export_memory_traces(filename)` - Export memory traces
- `export_plasticity_data(filename)` - Export plasticity data
- `export_synapse_weights(filename)` - Export synapse weights
- `export_stdp_history(filename)` - Export STDP history
- `export_analysis_results(filename)` - Export analysis results
- `export_performance_metrics(filename)` - Export performance metrics
- `export_visualization_data(filename)` - Export visualization data

#### ImportUtils
- `import_neural_data(filename)` - Import neural data
- `import_spike_data(filename)` - Import spike data
- `import_connectivity_matrix(filename)` - Import connectivity matrix
- `import_working_memory_data(filename)` - Import working memory data
- `import_episodic_memory_data(filename)` - Import episodic memory data
- `import_memory_traces(filename)` - Import memory traces
- `import_plasticity_data(filename)` - Import plasticity data
- `import_synapse_weights(filename)` - Import synapse weights
- `import_stdp_history(filename)` - Import STDP history

## Configuration Examples

### Basic Configuration

```python
from nlm_utilities import BrainUtils

brain = BrainUtils.create_advanced_brain(
    neuron_count=1000,
    region_count=3,
    connection_probability=0.1,
    enable_advanced_plasticity=False,
    enable_neuromodulation=False,
    enable_memory_systems=False
)
```

### Research Configuration

```python
from nlm_utilities import BrainUtils, MemoryUtils, PlasticityUtils

brain = BrainUtils.create_advanced_brain(
    neuron_count=5000,
    region_count=10,
    connection_probability=0.15,
    enable_advanced_plasticity=True,
    enable_neuromodulation=True,
    enable_memory_systems=True
)

# Configure advanced plasticity
plasticity_utils = PlasticityUtils(brain)
plasticity_utils.enable_stdp(True)
plasticity_utils.set_stdp_parameters(ltp_weight=0.025, ltd_weight=0.018, time_constant=25.0)
plasticity_utils.enable_hebbian(True)
plasticity_utils.set_hebbian_parameters(learning_rate=0.015, eligibility_trace_duration=50)

# Configure memory systems
memory_utils = MemoryUtils(brain)
memory_utils.enable_working_memory(True)
memory_utils.set_working_memory_capacity(100)
memory_utils.set_trace_decay_rate(0.95)
memory_utils.enable_episodic_memory(True)
memory_utils.set_storage_capacity(10000)
memory_utils.set_encoding_frequency(5)

# Configure neuromodulation
brain.configure_neuromodulation({
    'dopamine': {'enabled': True, 'baseline': 0.1, 'reward_sensitivity': 2.0},
    'curiosity': {'enabled': True, 'novelty_threshold': 0.5}
})
```

### Real-time Configuration

```python
from nlm_utilities import BrainUtils

brain = BrainUtils.create_advanced_brain(
    neuron_count=1000,
    region_count=3,
    connection_probability=0.1,
    enable_advanced_plasticity=True,
    enable_neuromodulation=True,
    enable_memory_systems=True
)

# Configure for real-time performance
sim_utils = SimulationUtils(brain)
sim_utils.set_timestep(0.01)
sim_utils.set_max_steps(5000)
sim_utils.set_automatic_checkpoints(True)
sim_utils.set_checkpoint_interval(500)

# Set performance thresholds
perf_utils = PerformanceUtils(brain)
perf_utils.set_monitoring_thresholds(cpu=90.0, memory=1.0, time=30.0)
```

### Large-scale Configuration

```python
from nlm_utilities import BrainUtils

brain = BrainUtils.create_advanced_brain(
    neuron_count=20000,
    region_count=50,
    connection_probability=0.05,
    enable_advanced_plasticity=True,
    enable_neuromodulation=True,
    enable_memory_systems=True
)

# Configure for large-scale performance
resource_utils = ResourceUtils(brain)
resource_utils.optimize_memory_usage()
resource_utils.optimize_cpu_usage()

# Set monitoring for large-scale
perf_utils = PerformanceUtils(brain)
perf_utils.set_monitoring_thresholds(cpu=95.0, memory=4.0, time=120.0)
```

## Advanced Usage Examples

### Complex Brain Configuration

```python
# Create a complex research brain configuration
brain = BrainUtils.create_advanced_brain(
    neuron_count=10000,
    region_count=20,
    connection_probability=0.12,
    enable_advanced_plasticity=True,
    enable_neuromodulation=True,
    enable_memory_systems=True
)

# Configure sophisticated plasticity
plasticity_utils = PlasticityUtils(brain)
plasticity_utils.enable_stdp(True)
plasticity_utils.set_stdp_parameters(
    ltp_weight=0.03,
    ltd_weight=0.022,
    time_constant=30.0,
    neuromodulation_sensitive=True,
    weight_dependent=True
)

plasticity_utils.enable_hebbian(True)
plasticity_utils.set_hebbian_parameters(
    learning_rate=0.02,
    eligibility_trace_duration=75,
    presynaptic_modulation=True,
    neuromodulation_modulated=True
)

plasticity_utils.enable_structural_plasticity(True)
plasticity_utils.set_synaptogenesis_rate(0.0004)
plasticity_utils.set_pruning_rate(0.00008)

# Configure complex memory systems
memory_utils = MemoryUtils(brain)
memory_utils.enable_working_memory(True)
memory_utils.set_working_memory_capacity(200)
memory_utils.set_trace_decay_rate(0.97)
memory_utils.set_competition_strength(0.8)
memory_utils.set_reinstatement_probability(0.9)

memory_utils.enable_episodic_memory(True)
memory_utils.set_storage_capacity(50000)
memory_utils.set_encoding_frequency(3)
memory_utils.set_replay_enabled(True)
memory_utils.set_replay_interval(30)
memory_utils.set_consolidation_threshold(5)
memory_utils.set_pattern_compression(True)

# Configure advanced neuromodulation
brain.configure_neuromodulation({
    'dopamine': {
        'enabled': True,
        'baseline': 0.15,
        'reward_sensitivity': 3.0,
        'prediction_error_weight': 2.0,
        'learning_modulation': True,
        'temporal_dynamics': True
    },
    'curiosity': {
        'enabled': True,
        'novelty_threshold': 0.6,
        'exploration_bonus': 0.7,
        'policy_gradient_weight': 0.4,
        'novelty_decay_rate': 0.98
    },
    'novelty': {
        'enabled': True,
        'detection_sensitivity': 0.9,
        'pattern_change_threshold': 0.4,
        'decay_rate': 0.99,
        'history_size': 1000
    }
})

# Setup advanced simulation
from nlm_utilities import SimulationUtils
sim_utils = SimulationUtils(brain)
sim_utils.set_timestep(0.001)
sim_utils.set_max_steps(20000)
sim_utils.set_automatic_checkpoints(True)
sim_utils.set_checkpoint_interval(1000)

# Inject complex stimulus
sim_utils.inject_custom_stimulus([
    {'type': 'sensory', 'neuron': 0, 'value': 0.9},
    {'type': 'sensory', 'neuron': 1, 'value': 0.7},
    {'type': 'sensory', 'neuron': 2, 'value': 0.8},
    {'type': 'motor', 'neuron': 10, 'value': 0.6},
    {'type': 'motor', 'neuron': 11, 'value': 0.5}
])

# Start advanced simulation
brain.initialize()
for step in range(5000):
    sim_utils.inject_custom_stimulus([
        {'type': 'sensory', 'neuron': 0, 'value': 0.9 * (1 + 0.1 * (step % 100 / 100.0))}
    ])
    brain.step(step, step * 0.001)
    if step % 100 == 0:
        performance_metrics = sim_utils.get_performance_metrics()
        print(f"Step {step}: Performance = {performance_metrics}")
```

### Analysis and Visualization

```python
from nlm_utilities import AnalysisUtils, VisualizationUtils

# Create analysis utilities
analysis_utils = AnalysisUtils(brain)
visualization_utils = VisualizationUtils(brain)

# Perform comprehensive analysis
spike_analysis = analysis_utils.analyze_spikes()
neural_dynamics = analysis_utils.analyze_neuron_dynamics()
network_activity = analysis_utils.analyze_network_activity()
memory_analysis = analysis_utils.analyze_memory_systems()
plasticity_analysis = analysis_utils.analyze_plasticity()
performance_analysis = analysis_utils.analyze_learning_efficiency()

# Create visualizations
network_graph = visualization_utils.create_neural_network_graph()
activity_heatmap = visualization_utils.create_activity_heatmap()
spike_raster = visualization_utils.create_spike_raster_plot()
learning_curves = visualization_utils.create_learning_curves()
plasticity_dynamics = visualization_utils.create_plasticity_dynamics_plot()
memory_heatmap = visualization_utils.create_memory_heatmap()

# Save visualizations
import matplotlib.pyplot as plt
plt.figure(figsize=(15, 10))

plt.subplot(2, 3, 1)
plt.imshow(network_graph)
plt.title('Neural Network')

plt.subplot(2, 3, 2)
plt.imshow(activity_heatmap)
plt.title('Activity Heatmap')

plt.subplot(2, 3, 3)
plt.plot(spike_raster)
plt.title('Spike Raster')

plt.subplot(2, 3, 4)
plt.plot(learning_curves)
plt.title('Learning Curves')

plt.subplot(2, 3, 5)
plt.imshow(plasticity_dynamics)
plt.title('Plasticity Dynamics')

plt.subplot(2, 3, 6)
plt.imshow(memory_heatmap)
plt.title('Memory Heatmap')

plt.tight_layout()
plt.savefig('brain_simulation_analysis.png')
plt.show()
```

### Performance Optimization

```python
from nlm_utilities import PerformanceUtils, ResourceUtils

# Performance optimization
perf_utils = PerformanceUtils(brain)
resource_utils = ResourceUtils(brain)

# Monitor and optimize performance
perf_utils.start_real_time_monitoring()
perf_utils.set_monitoring_thresholds(cpu=85.0, memory=3.0, time=60.0)

# Get performance alerts
performance_alerts = perf_utils.get_performance_alerts()
if performance_alerts:
    print("Performance alerts:", performance_alerts)

# Profile simulation
profile_data = perf_utils.profile_simulation(steps=1000)
performance_report = perf_utils.generate_performance_report()

# Analyze bottlenecks
bottleneck_analysis = perf_utils.analyze_bottlenecks()
print("Bottleneck analysis:", bottleneck_analysis)

# Get optimization suggestions
optimization_suggestions = perf_utils.get_optimization_suggestions()
print("Optimization suggestions:", optimization_suggestions)

# Apply optimizations
perf_utils.apply_optimization_suggestions(optimization_suggestions)

# Resource management
memory_stats = resource_utils.get_memory_statistics()
cpu_stats = resource_utils.get_cpu_statistics()
print("Memory stats:", memory_stats)
print("CPU stats:", cpu_stats)

# Memory optimization
memory_recommendations = resource_utils.get_memory_optimization_recommendations()
resource_utils.optimize_memory_usage()

# CPU optimization
cpu_recommendations = resource_utils.get_cpu_optimization_recommendations()
resource_utils.optimize_cpu_usage()
```

### State Management

```python
from nlm_utilities import StateUtils

# State management
state_utils = StateUtils(brain)

# Capture brain state at key timepoints
key_timepoints = [100, 500, 1000, 2000, 5000]
for step in key_timepoints:
    brain_state = state_utils.capture_brain_state(step)
    state_utils.export_brain_state(brain_state, f'brain_state_step_{step}.pickle')
    print(f"Captured brain state at step {step}")

# Compare states
if len(key_timepoints) >= 2:
    state_comparison = state_utils.compare_brain_states(
        state_utils.import_brain_state(f'brain_state_step_{key_timepoints[0]}.pickle'),
        state_utils.import_brain_state(f'brain_state_step_{key_timepoints[-1]}.pickle')
    )
    state_changes = state_utils.analyze_state_changes(state_comparison)
    state_similarity = state_utils.calculate_state_similarity(
        state_utils.import_brain_state(f'brain_state_step_{key_timepoints[0]}.pickle'),
        state_utils.import_brain_state(f'brain_state_step_{key_timepoints[-1]}.pickle')
    )
    print(f"State similarity: {state_similarity}")
    print(f"State changes: {state_changes}")
```

## Installation

```bash
pip install nlm-utilities
```

## Quick Start

```bash
# Install
pip install nlm-utilities

# Create basic advanced brain
python -c "
from nlm_utilities import BrainUtils
brain = BrainUtils.create_advanced_brain(
    neuron_count=1000,
    region_count=3,
    connection_probability=0.1
)
print('Advanced brain created successfully!')
print(f'Neurons: {brain.getTotalNeuronCount()}')
print(f'Regions: {brain.getTotalRegionCount()}')
"

# Run simulation with analysis
python -c "
from nlm_utilities import BrainUtils, SimulationUtils, AnalysisUtils
brain = BrainUtils.create_advanced_brain(neuron_count=500)
sim_utils = SimulationUtils(brain)
analysis_utils = AnalysisUtils(brain)

brain.initialize()
for step in range(1000):
    brain.step(step, step * 0.001)

spike_stats = analysis_utils.analyze_spikes()
neural_dynamics = analysis_utils.analyze_neuron_dynamics()
print('Simulation complete!')
print(f'Spike statistics: {spike_stats}')
print(f'Neural dynamics: {neural_dynamics}')
"
```

## Support

For support with NLM Python Utilities:
- GitHub Issues: https://github.com/nlm-project/nlm-utilities/issues
- Documentation: https://docs.nlm-project.org/python-utilities
- Community Forum: https://forum.nlm-project.org/discussions

## License

MIT License - See LICENSE file for details.

---

This NLM Python Utilities library provides advanced tools for working with NLM simulations, enabling researchers and developers to create, configure, analyze, and optimize complex neural simulations with enhanced control and insights.