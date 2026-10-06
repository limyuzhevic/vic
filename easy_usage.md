---

## NEW: Advanced Python API (v3.0)

### New Convenience Functions

```python
# Run simple simulation with automatic stats
stats = pynlm.run_simulation(brain, 1000, verbose=True)
# Returns: (firing_neurons, total_spikes, average_firing_rate)

# Run complete agent simulation
sim_results = pynlm.run_agent_simulation(brain, world, agent, 1000)
# Returns: (firing_neurons, total_spikes, avg_firing_rate, curiosity, novelty)

# Get comprehensive brain stats
brain_json = pynlm.get_brain_stats(brain)
# Returns: JSON string with complete brain information

# Create experiment configurations
config = pynlm.create_experiment_config("behavioral_learning")

# Benchmark performance
benchmark = pynlm.benchmark_performance(brain, 1000)
# Returns: JSON with performance metrics

# Advanced research configurations
research_suite = pynlm.create_research_suite()
```

### New Classes and Methods

```python
# Enhanced Config class
config.loadFromJsonString(json_string)
config.saveToJsonString()  # Returns JSON as string
config.has("key")
config.getInt("key", default)
config.getFloat("key", default)
config.setString("key", value)
config.setInt("key", value)

# Enhanced Action class
action.getName()  # Returns descriptive name like "MoveForward"
action.clone()    # Create a copy

# Enhanced AgentBrain methods
agent.enableRewardModulation(True/False)
agent.isRewardModulationEnabled()  # Check if enabled
agent.enableStructuralPlasticity(True/False)
agent.isStructuralPlasticityEnabled()
agent.enableDevelopment(True/False)
agent.isDevelopmentEnabled()
agent.enableCuriosity(True/False)
agent.isCuriosityEnabled()

# Enhanced WorldObject
obj.getTypeName()  # Get string name of object type
obj.isActive()     # Check if object is active

# Enhanced SensoryPercept
percept.getAllSignals()  # Get all sensory signals
percept.getFormattedData()  # Get formatted string representation
```

### New Advanced Module

```python
# Advanced features (requires separate import)
import pynlm_advanced

# Batch processing
results = pynlm_advanced.batch_run_simulations([config1, config2], [1000, 2000])

# Progress tracking
pynlm_advanced.run_with_progress(brain, 5000, lambda progress: print(f"Progress: {progress*100:.1f}%")

# Data analysis
analysis = pynlm_advanced.analyze_simulation_data(results)

# Experiment management
pynlm_advanced.export_experiment_results("experiment.json", results, "Behavioral learning experiment")

# Performance profiling
profile = pynlm_advanced.profile_simulation(brain, 1000)

# Network analysis
connectivity = pynlm_advanced.analyze_connectivity(brain)

# Time series analysis
time_series_analysis = pynlm_advanced.analyze_time_series(time_series)
```

### Version Information

```python
info = pynlm.get_version_info()
print(info)  # JSON with version, phase, systems, etc.
```

### Configuration Validation

```python
validation = pynlm.validate_config(config)
print(validation)  # JSON with validation results
```