# Improved NLM Python API - Enhanced Bindings

This is an enhanced version of the NLM Python bindings with improved convenience functions, error handling, and Pythonic API patterns.

## Usage Examples

### Basic Usage (Unchanged)
```python
import pynlm

# Create brain with default config
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
```

### New High-Level Convenience Functions
```python
import pynlm

# One-line brain creation with preset configurations
brain = pynlm.create_brain_with_config('explorer')
brain = pynlm.create_brain_with_config('learner', neuron_count=2000)
brain = pynlm.create_brain_with_config('balanced')

# Complete simulation with monitoring
result = pynlm.run_simulation_with_monitoring(brain, world, agent, max_steps=1000)
print(f"Simulation completed with {result['spikes']} spikes")

# Batch operations for multiple brains
brains = pynlm.create_multiple_brains(count=5, neuron_count=1000)
results = pynlm.run_batch_simulation(brains, world, agent)
```

### Advanced Agent Functions
```python
import pynlm

# Create goal-directed agent
agent = pynlm.create_goal_directed_agent(brain, goals=['explore', 'learn', 'consolidate'])

# Memory-based agent with experience collection
memory_agent = pynlm.create_memory_based_agent(brain)
experience = memory_agent.collect_experience(world, agent, steps=100)

# Advanced neuromodulation
agent.enable_curiosity_driven_exploration(threshold=0.3)
agent.setup_meta_learning(meta_rate=0.01, meta_horizon=100)
```

### NumPy Integration
```python
import pynlm
import numpy as np

# Convert brain state to NumPy arrays
brain_state = pynlm.get_all_synaptic_weights(brain)
print(f"Weights shape: {brain_state.shape}")

# Inject currents using NumPy arrays
currents = np.random.normal(0, 1, 100)
neuron_indices = np.arange(100)
brain.inject_current_batch(currents, neuron_indices)

# Performance monitoring with NumPy arrays
stats = pynlm.compute_brain_statistics(brain, world, agent)
print(f"Firing rate: {stats['firing_rate']:.2f} Hz")
```

### Debugging and Monitoring
```python
import pynlm

# Real-time monitoring
monitor = pynlm.start_brain_monitoring(brain, interval=0.1)
while monitor.is_active():
    status = monitor.get_status()
    print(f"Active neurons: {status['firing_count']}")
    if monitor.should_stop():
        break

# Network analysis
analysis = pynlm.analyze_brain_network(brain)
print(f"Connectivity: {analysis['density']:.3f}")
print(f"Communities: {analysis['num_communities']}")
print(f"Hub neurons: {analysis['num_hubs']}")

# Validation
validation = pynlm.validate_brain_health(brain)
if not validation['healthy']:
    print("Issues found:", validation['issues'])
```

### Advanced Serialization
```python
import pynlm

# Create incremental checkpoint
session_id = pynlm.create_simulation_session(world, agent, brain)
checkpoint_name = pynlm.save_incremental_checkpoint(brain, world, agent, session_id)

# Load from checkpoint
pynlm.load_from_checkpoint(checkpoint_name, brain, world, agent)

# List and manage sessions
available_sessions = pynlm.list_simulation_sessions()
for session in available_sessions:
    print(f"Session: {session['name']}, Size: {session['size']} bytes")
```

### Threading and Parallelization
```python
import pynlm
import threading

# Parallel episode runner
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

def run_episode(agent_id):
    brain = pynlm.create_brain_with_config('explorer')
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    return pynlm.run_single_episode(agent, world, max_steps=500)

# Run multiple episodes in parallel
results = pynlm.run_parallel_episodes(num_episodes=10, agent_id_range=(0, 10))
print(f"Completed {len(results)} parallel episodes")
```

### Debugging and Performance Tools
```python
import pynlm

# Detailed synaptic weight analysis
weight_analysis = pynlm.analyze_synaptic_dynamics(brain, window=100)
print(f"STDP efficacy: {weight_analysis['stdp_efficacy']:.4f}")
print(f"Hebbian strength: {weight_analysis['hebbian_strength']:.4f}")

# Neuron activity debugging
neuron_debug = pynlm.debug_neuron_activity(brain, neuron_indices=[0, 1, 2, 3, 4])
print(f"Firing patterns: {neuron_debug['patterns']}")
print(f"Coherence: {neuron_debug['coherence']:.3f}")

# Performance profiling
profiling = pynlm.profile_simulation_performance(brain, world, agent, steps=1000)
print(f"FPS: {profiling['frames_per_second']:.1f}")
print(f"Memory usage: {profiling['memory_mb']:.1f} MB")
```

### Configuration Management
```python
import pynlm

# Configuration presets
configs = pynlm.list_config_presets()
for name, description in configs.items():
    print(f"{name}: {description}")

# Create custom configuration
custom_config = pynlm.create_custom_config({
    'brain': {
        'neuron_count': 5000,
        'connection_probability': 0.2,
        'plasticity': {'stdp': {'learning_rate': 0.005}}
    },
    'neuromodulation': {
        'curiosity': {'enabled': True, 'scale': 2.0}
    }
})

# Validate configuration
validation = pynlm.validate_config(custom_config)
if validation['valid']:
    brain = pynlm.createBrain(custom_config)
else:
    print("Invalid config:", validation['errors'])
```

### Visualization and Analysis
```python
import pynlm

# Comprehensive brain analysis
analysis = pynlm.comprehensive_brain_analysis(brain, world, agent)

# Save results to file
pynlm.save_analysis_results(analysis, 'brain_analysis.json')

# Generate visualizations (if matplotlib available)
try:
    pynlm.plot_brain_connectivity(brain)
    pynlm.plot_firing_patterns(brain)
    pynlm.plot_development_trajectory(brain)
except ImportError:
    print("Install matplotlib for visualization: pip install matplotlib")
```

## New Functions API

### Simulation Control
- `create_brain_with_config(config_name, **kwargs)` - Create brain with preset configuration
- `run_simulation_with_monitoring(brain, world, agent, max_steps, interval)` - Run with monitoring
- `run_single_episode(agent, world, max_steps)` - Run one complete episode
- `run_parallel_episodes(num_episodes, agent_id_range)` - Run multiple episodes in parallel
- `monitor_brain_activity(brain, interval)` - Start real-time monitoring

### Convenience Functions
- `create_multiple_brains(count, **kwargs)` - Create multiple brains
- `run_batch_simulation(brains, world, agent)` - Run simulation on multiple brains
- `reset_brain(brain)` - Clean brain reset with validation
- `save_simulation_state(brain, world, agent, label)` - Advanced checkpointing
- `load_simulation_state(label, brain, world, agent)` - Flexible loading

### NumPy Integration
- `get_all_synaptic_weights(brain)` - Get all weights as NumPy array
- `inject_current_batch(brain, currents, neuron_indices)` - Batch current injection
- `as_numpy_array(brain_state)` - Convert to NumPy array
- `from_numpy_array(data)` - Import from NumPy

### Agent Enhancements
- `create_goal_directed_agent(brain, goals)` - Goal-based agent
- `create_memory_based_agent(brain, memory_system)` - Memory integration
- `create_hierarchical_agent(brain, controllers)` - Multi-level control
- `create_reinforcement_agent(brain, reward_function)` - RL integration

### Advanced Neuromodulation
- `enable_curiosity_driven_exploration(agent, threshold)` - Exploration optimization
- `create_bayesian_curiosity(agent, model)` - Probabilistic curiosity
- `implement_hedging_behavior(agent, uncertainty)` - Risk management
- `setup_meta_learning(agent, meta_params)` - Learning to learn

### Configuration and Validation
- `create_custom_config(settings)` - Create custom configuration
- `validate_config(config)` - Schema validation
- `config_summary(config)` - Human-readable summary
- `config_compare(config1, config2)` - Differences analysis
- `list_config_presets()` - List available configuration presets

### Debugging and Analysis
- `analyze_brain_network(brain)` - Network topology analysis
- `analyze_synaptic_dynamics(brain, window)` - Synaptic change analysis
- `analyze_neural_coherence(brain)` - Coherence analysis
- `analyze_development_trajectory(brain)` - Development tracking
- `validate_brain_health(brain)` - Health validation
- `profile_simulation_performance(brain, world, agent, steps)` - Performance profiling

### Serialization and Persistence
- `create_simulation_session(world, agent, brain)` - Create simulation session
- `save_incremental_checkpoint(brain, world, agent, session_id)` - Incremental checkpointing
- `load_from_checkpoint(checkpoint_name, brain, world, agent)` - Load from checkpoint
- `list_simulation_sessions()` - List available sessions
- `compress_brain_state(brain)` - Memory-efficient serialization
- `decompress_brain_state(data)` - Efficient loading

### Threading and Parallelization
- `run_parallel_episodes(num_episodes, agent_id_range)` - Parallel episode runner
- `concurrent_brain_training(brains, worlds, agents)` - Concurrent training
- `thread_safe_simulation_manager()` - Thread-safe coordinator

### Analysis and Visualization
- `comprehensive_brain_analysis(brain, world, agent)` - Complete analysis
- `save_analysis_results(analysis, filename)` - Save analysis results
- `plot_brain_connectivity(brain)` - Network visualization
- `plot_firing_patterns(brain)` - Activity visualization
- `plot_development_trajectory(brain)` - Development visualization

### Error Handling
- `NLMError` - Base exception class
- `BrainInitializationError` - Brain setup failures
- `SimulationError` - Runtime simulation issues
- `ConfigurationError` - Invalid configuration
- `MemoryError` - Resource allocation problems

## Design Principles

1. **Pythonic API**: Use Python conventions (snake_case, generators, context managers)
2. **Backward Compatibility**: Maintain existing API while adding new features
3. **Performance**: Optimize for common use cases and batch operations
4. **Error Handling**: Comprehensive, user-friendly exceptions with recovery suggestions
5. **Documentation**: Clear, comprehensive docstrings with examples
6. **Testing**: Comprehensive test coverage for all new functionality

## Benefits

1. **Improved Usability**: Lower barrier to entry with high-level convenience functions
2. **Enhanced Performance**: NumPy integration and batch operations
3. **Better Debugging**: Comprehensive monitoring and analysis tools
4. **Advanced Features**: Goal-directed behaviors and meta-learning capabilities
5. **Production Ready**: Thread-safe, scalable, and persistent simulations
6. **Research Grade**: Network analysis and advanced visualization

## Installation

```bash
# Install with enhanced features
pip install pynlm[all]

# Install with core features only
pip install pynlm[core]

# Install development version
pip install -e .
```

## Dependencies

Required:
- Python 3.8+
- NumPy (optional, for enhanced features)
- Matplotlib (optional, for visualization)

Optional:
- Scipy (for advanced analysis)
- Pandas (for data handling)
- TensorFlow/PyTorch (for advanced integration)

This enhanced API maintains full backward compatibility while providing powerful new features for both beginners and advanced users.