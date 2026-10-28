# NLM - Advanced Configuration Examples

This file demonstrates the advanced configuration capabilities introduced in Phase 6 of the NLM project.

## JSON Configuration File

```json
{
    "neuron_count": 5000,
    "region_count": 2,
    "connection_probability": 0.05,
    "initial_weight_mean": 0.5,
    "initial_weight_std": 0.1,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 15.0,
    "plasticity.stdp.enable": true,
    "plasticity.hebbian.enable": true,
    "plasticity.structural.enable": true,
    "neuromod.dopamine.enable": true,
    "neuromod.curiosity.enable": true,
    "neuromod.novelty.enable": true,
    "development.enable": true,
    "working_memory.capacity": 500,
    "episodic_memory.max_episodes": 2000,
    "simulation_timestep": 0.002,
    "simulation_duration": 100.0,
    "checkpoint_dir": "./checkpoints",
    "replay_interval": 200,
    "consolidation_interval": 5000,
    "random_seed": 42
}
```

## YAML Configuration File

```yaml
# NLM Advanced Configuration
neuron_count: 2000
region_count: 1
connection_probability: 0.1

initial_weight_mean: 0.5
initial_weight_std: 0.1

# STDP Configuration
stdp_ltp_weight: 0.03
stdp_ltd_weight: 0.025
stdp_tau: 25.0
plasticity:
  stdp:
    enable: true
  hebbian:
    enable: true
  structural:
    enable: true

# Neuromodulation
neuromod:
  dopamine:
    enable: true
  curiosity:
    enable: true
  novelty:
    enable: true

# Development
development:
  enable: true
  stages: 5

# Memory Systems
working_memory:
  capacity: 200
episodic_memory:
  max_episodes: 1000

# Simulation Settings
simulation:
  timestep: 0.001
  duration: 200

# Advanced Features
checkpoint_dir: ./checkpoints
replay_interval: 100
consolidation_interval: 2000
random_seed: 12345
```

## INI Configuration File (Backward Compatible)

```ini
# NLM Configuration File
# Neuron and Network Configuration
neuron_count = 3000
region_count = 2
connection_probability = 0.08

# Initial Conditions
initial_weight_mean = 0.6
initial_weight_std = 0.15

# STDP Parameters
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
stdp_tau = 20.0

# Plasticity Systems
plasticity.stdp.enable = true
plasticity.hebbian.enable = true
plasticity.structural.enable = true

# Neuromodulation Systems
neuromod.dopamine.enable = true
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Development
development.enable = true

# Memory Configuration
working_memory.capacity = 300
episodic_memory.max_episodes = 3000

# Simulation Settings
simulation_timestep = 0.001
random_seed = 999
```

## Advanced Configuration Builder Usage

### Standard Configuration

```python
import pynlm_advanced as nlm

# Create standard configuration
config = nlm.create_default_advanced_config()
brain = nlm.create_brain(config.get_config())
```

### High Performance Configuration

```python
import pynlm_advanced as nlm

# Create high performance configuration
config = nlm.create_high_performance_config(10000)
brain = nlm.create_brain(config.get_config())
```

### Learning Configuration

```python
import pynlm_advanced as nlm

# Create learning configuration
config = nlm.create_learning_config(2000)
brain = nlm.create_brain(config.get_config())
```

## Advanced Simulation Examples

### Basic Advanced Simulation

```python
import pynlm_advanced as nlm

# Setup
brain = nlm.create_brain(config.get_config())
world = nlm.create_simple_world()
agent = nlm.create_agent_brain(brain)
agent.initialize(world)

# Create advanced runner
runner = nlm.create_advanced_runner(brain, world, agent)

# Run advanced simulation
runner.run_advanced(5000, mode="balanced")

# Get statistics
stats = runner.get_advanced_statistics()
print(f"Neurons: {stats['neurons']}")
print(f"Spikes: {stats['total_spikes']}")
print(f"Curiosity: {stats['curiosity_level']}")
```

### Learning Experiment with Advanced Features

```python
import pynlm_advanced as nlm

# Setup with advanced config
config = nlm.create_learning_config()
brain = nlm.create_brain(config.get_config())
world = nlm.create_simple_world()
agent = nlm.create_agent_brain(brain)
agent.initialize(world)

# Create advanced runner
runner = nlm.create_advanced_runner(brain, world, agent)

# Run learning experiment
runner.run_learning_experiment(10000, reward_scaling=2.0)

# Save with timestamp
runner.save_with_timestamp("./experiments/learning_run")
```

### Multi-Episode Learning

```python
import pynlm_advanced as nlm

# Setup
brain = nlm.create_brain(config.get_config())
world = nlm.create_simple_world()
agent = nlm.create_agent_brain(brain)
agent.initialize(world)

# Create advanced runner
runner = nlm.create_advanced_runner(brain, world, agent)

# Run multiple episodes
runner.run_multi_episode(num_episodes=100, steps_per_episode=500)

# Get performance statistics
stats = runner.get_advanced_statistics()
print(f"Final performance after {100} episodes:")
print(f"  Average firing rate: {stats['avg_firing_rate']}")
print(f"  Total spikes: {stats['total_spikes']}")
```

## Performance Profiling

```python
import pynlm_advanced as nlm

# Create performance profiler
profiler = nlm.create_performance_profiler()

# Profile different operations
profiler.start_profile("brain_step")
brain->step(step);
profiler.end_profile()

profiler.start_profile("memory_update");
workingMemory->update(timestep);
profiler.end_profile()

profiler.start_profile("plasticity_update");
applyPlasticity();
profiler.end_profile()

# Get profiling results
summary = profiler.get_profile_summary()
for operation, data in summary.items():
    print(f"{operation}: avg {data['avg_time_ms']:.2f}ms")
```

## Experiment Runner Examples

### Ablation Study

```python
import pynlm_advanced as nlm

# Setup
brain = nlm.create_brain(config.get_config())
world = nlm.create_simple_world()
agent = nlm.create_agent_brain(brain)
agent.initialize(world)

# Create experiment runner
experiment_runner = nlm.create_advanced_experiment_runner(brain, world, agent)

# Run ablation study
system_disabling = {
    "reward_modulation": True,
    "curiosity": False,
    "structural_plasticity": True,
    "development": False
}

results = experiment_runner.run_ablation_study(system_disabling)

print("Ablation Study Results:")
for variant, result in results.items():
    print(f"\n{variant}:")
    for key, value in result.items():
        print(f"  {key}: {value}")
```

### Parameter Sweep

```python
import pynlm_advanced as nlm

# Setup
brain = nlm.create_brain(config.get_config())
world = nlm.create_simple_world()
agent = nlm.create_agent_brain(brain)
agent.initialize(world)

# Create experiment runner
experiment_runner = nlm.create_advanced_experiment_runner(brain, world, agent)

# Define parameter sweep
parameters = {
    "connection_probability": [0.05, 0.1, 0.15],
    "stdp_ltp_weight": [0.01, 0.02, 0.03],
    "neuron_count": [1000, 2000, 3000]
}

# Run parameter sweep
sweep_results = experiment_runner.run_parameter_sweep(parameters, steps=500)

print("Parameter Sweep Results:")
for exp_name, results in sweep_results.items():
    print(f"\n{exp_name}:")
    print(f"  Parameters: {results['parameters']}")
    print(f"  Spikes: {results['spikes']}")
    print(f"  Firing rate: {results['firing_rate']}")
```

### Scaling Experiment

```python
import pynlm_advanced as nlm

# Setup
brain = nlm.create_brain(config.get_config())
world = nlm.create_simple_world()
agent = nlm.create_agent_brain(brain)
agent.initialize(world)

# Create experiment runner
experiment_runner = nlm.create_advanced_experiment_runner(brain, world, agent)

# Define neuron counts for scaling experiment
neuron_counts = [500, 1000, 2000, 5000, 10000]

# Run scaling experiment
scaling_results = experiment_runner.run_scaling_experiment(neuron_counts, steps=200)

print("Scaling Experiment Results:")
for neuron_count_key, results in scaling_results.items():
    count = neuron_count_key.split('_')[1]
    print(f"\nNeurons: {count}")
    print(f"  Spikes: {results['spikes']}")
    print(f"  Firing rate: {results['firing_rate']}")
    print(f"  E/I ratio: {results['e_i_ratio']}")
```

## System Utilities

### Getting System Capabilities

```python
import pynlm_advanced as nlm

caps = nlm.getSystemCapabilities()
print("System Capabilities:")
print(f"  Max neurons: {caps.maxNeurons}")
print(f"  Max synapses: {caps.maxSynapses}")
print(f"  Max timestep: {caps.maxSimulationTimestep}s")
print(f"  JSON support: {caps.supportsJSON}")
print(f"  YAML support: {caps.supportsYAML}")
print(f"  Advanced API: {caps.supportsAdvancedAPI}")
```

### Getting Feature Availability

```python
import pynlm_advanced as nlm

features = nlm.getFeatureAvailability()
print("Available Features:")
for feature, available in features.__dict__.items():
    print(f"  {feature}: {'✓' if available else '✗'}")
```

### Getting Version Information

```python
import pynlm_advanced as nlm

version_info = nlm.getVersionInfo()
print(f"NLM Advanced Module v{version_info.version}")
print(f"Build Date: {version_info.buildDate}")
print(f"Git Commit: {version_info.gitCommit}")
```

### System Health Check

```python
import pynlm_advanced as nlm

health = nlm.performHealthCheck()
print(f"System Health: {'Healthy' if health.is_healthy else 'Unhealthy'}")
if health.errors:
    print("Errors:")
    for error in health.errors:
        print(f"  - {error}")
if health.warnings:
    print("Warnings:")
    for warning in health.warnings:
        print(f"  - {warning}")
```

## Configuration Validation

### Valid Configuration

```python
import pynlm_advanced as nlm

config = nlm.create_default_advanced_config()

# Validate configuration
validation_result = config.validate()
if validation_result.is_valid:
    print("Configuration is valid")
else:
    print("Configuration errors:")
    for error in validation_result.errors:
        print(f"  - {error}")
```

### Invalid Configuration

```python
import pynlm_advanced as nlm

# Create invalid configuration
config = nlm.create_default_advanced_config()
config.set("neuron_count", -100)  # Invalid value
config.set("connection_probability", 1.5)  # Invalid range

# Validate configuration
validation_result = config.validate()
if not validation_result.is_valid:
    print("Configuration validation failed (expected):")
    for error in validation_result.errors:
        print(f"  - {error}")
```

## Error Handling Examples

### Basic Error Handling

```python
import pynlm_advanced as nlm

# Enable error handling with logging
nlm.initialize_error_handling_system("nlm_errors.log")

try:
    # Create brain with invalid configuration
    config = nlm.create_default_advanced_config()
    config.set("invalid_key", "invalid_value")
    
    brain = nlm.create_brain(config.get_config())
    
    # This might raise an error
    brain.initialize()
    
except Exception as e:
    print(f"Error occurred: {e}")
```

### Advanced Error Handling with Different Handlers

```python
import pynlm_advanced as nlm

# Add custom error handler
class CustomErrorHandler:
    def handle_error(self, error):
        print(f"Custom error handler: {error}")
    
    def handle_warning(self, message):
        print(f"Custom warning: {message}")
    
    def handle_info(self, message):
        print(f"Custom info: {message}")

# Register custom handler
custom_handler = CustomErrorHandler()
nlm.ErrorManager.getInstance().addErrorHandler(custom_handler)

# Perform operations that might trigger errors
brain = nlm.create_brain(config.get_config())
brain.initialize()
```

## Configuration Builder Examples

### Custom Configuration

```python
import pynlm_advanced as nlm

# Create custom configuration
config = (nlm.AdvancedConfigBuilder()
          .set_neural_network(8000, 3)
          .set_connectivity(0.08, 0.4, 0.15)
          .set_stdp(0.025, 0.02, 30.0)
          .enable_plasticity(True)
          .enable_reward_modulation(True)
          .enable_curiosity(True)
          .enable_development(True)
          .set_memory_capacity(800, 5000)
          .set_simulation(0.001, 300.0)
          .enable_visualization(True))

print("Custom configuration created:")
print(config.export_to_dict())
```

### Configuration from Dictionary

```python
import pynlm_advanced as nlm

# Python dictionary for configuration
config_dict = {
    "neuron_count": 3000,
    "region_count": 2,
    "connection_probability": 0.12,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.018,
    "stdp_tau": 20.0,
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "plasticity.structural.enable": True,
    "development.enable": True,
    "simulation_timestep": 0.002,
    "random_seed": 12345
}

# Create config from dictionary
config = nlm.AdvancedConfigBuilder()
config.load_from_dict(config_dict)

print("Configuration loaded from dictionary:")
print(config.export_to_dict())
```

## Performance Optimization Examples

### Batch Processing

```python
import pynlm_advanced as nlm

# Setup
brain = nlm.create_brain(config.get_config())
world = nlm.create_simple_world()
agent = nlm.create_agent_brain(brain)
agent.initialize(world)

# Create advanced runner
runner = nlm.create_advanced_runner(brain, world, agent)

# Run batch processing
steps_per_batch = 100
num_batches = 50

for batch in range(num_batches):
    runner.run_advanced(steps_per_batch, mode="balanced")
    
    # Save checkpoint periodically
    if batch % 10 == 0:
        runner.save_with_timestamp(f"checkpoint_batch_{batch}")
    
    # Get performance statistics
    stats = runner.get_advanced_statistics()
    print(f"Batch {batch}: {stats['total_spikes']} spikes, " +
          f"{stats['avg_firing_rate']:.2f} avg firing rate")
```

### Configuration Benchmarking

```python
import pynlm_advanced as nlm

# Define configurations to benchmark
configs = [
    nlm.create_default_advanced_config(),
    nlm.create_high_performance_config(2000),
    nlm.create_learning_config(5000)
]

# Setup brains
brains = []
for config in configs:
    brain = nlm.create_brain(config.get_config())
    brains.append(brain)

# Run benchmarks
bench_results = nlm.PerformanceProfiler.benchmark_configurations(brains, steps=1000)

print("Configuration Benchmarking Results:")
for result in bench_results:
    print(f"\nConfig {result['config_id']}:")
    print(f"  Neurons: {result['neurons']}")
    print(f"  Synapses: {result['synapses']}")
    print(f"  Total spikes: {result['total_spikes']}")
    print(f"  Average firing rate: {result['avg_firing_rate']:.2f}")
    print(f"  E/I ratio: {result['e_i_ratio']:.2f}")
```

## Integration with Existing NLM Code

### Mixing Advanced and Standard APIs

```python
import pynlm
import pynlm_advanced as nlm

# Use standard NLM API
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)

brain = pynlm.createBrain(config)
brain.initialize()

# Use advanced NLM API for enhanced features
advanced_config = nlm.create_default_advanced_config()
# Merge configurations (simplified)

# Create advanced runner
runner = nlm.create_advanced_runner(brain, world, agent)

# Run with standard and advanced features combined
runner.run_advanced(1000, mode="balanced")

# Get comprehensive statistics
stats = runner.get_advanced_statistics()
print("Combined API Results:")
for key, value in stats.items():
    print(f"  {key}: {value}")
```

This documentation provides comprehensive examples of using the advanced features introduced in Phase 6 of the NLM project. The advanced configuration builder, enhanced simulation runner, performance profiling tools, and experiment runner capabilities significantly expand the usability and flexibility of the NLM framework while maintaining backward compatibility with existing code.
