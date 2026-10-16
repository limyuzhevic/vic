# NLM (Neural Learning Machine) - Advanced Usage Guide

**Target Audience:** Power users, researchers, and developers who want to get the most out of NLM with custom configurations and advanced features.

---

## Advanced Configuration

### Custom Configuration Files

Create custom configuration files for specific use cases:

```bash
# Create a high-performance configuration
cat > config/high_performance.cfg << EOF
# High-performance NLM configuration
random_seed = 12345
simulation_timestep = 0.0005
neuron_count = 2000
region_count = 4
connection_probability = 0.2
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.018
stdp_time_constant = 25.0
development_synaptogenesis_rate = 0.0002
development_pruning_rate = 0.00005
synaptogenesis_rate = 0.0005
pruning_rate = 0.00002
dopamine_baseline = 0.2
reward_discount_factor = 0.995
log_level = INFO
log_to_file = true
log_filename = nlm_high_perf.log
max_simulation_steps = 50000
simulation_time_limit = 0.0
visualization_enabled = false
EOF
```

### Command Line Configuration

```bash
# Run with custom configuration and verbose logging
./nlm --config=config/high_performance.cfg --verbose --log=debug

# Override specific parameters
./nlm --config=config/base.cfg \
     --neuron_count=3000 \
     --connection_probability=0.15 \
     --stdp_ltp_weight=0.03
```

### Configuration Validation Script

```python
# validate_config.py - Validate NLM configuration files
def validate_config(config_file):
    """Validate NLM configuration file for required parameters and valid ranges."""
    required_params = [
        "random_seed", "simulation_timestep", "neuron_count", 
        "region_count", "connection_probability"
    ]
    
    valid_ranges = {
        "neuron_count": (100, 10000),
        "region_count": (1, 10),
        "connection_probability": (0.01, 1.0),
        "simulation_timestep": (0.0001, 0.1),
        "stdp_ltp_weight": (0.001, 0.1),
        "stdp_ltd_weight": (0.001, 0.1),
        "stdp_time_constant": (1.0, 100.0),
        "reward_discount_factor": (0.9, 0.999)
    }
    
    config = pynlm.createDefaultConfig()
    config.loadFromFile(config_file)
    
    errors = []
    warnings = []
    
    for param in required_params:
        if not config.has(param):
            errors.append(f"Missing required parameter: {param}")
    
    for param, (min_val, max_val) in valid_ranges.items():
        if config.has(param):
            value = config.get(param)
            if value < min_val or value > max_val:
                warnings.append(f"Parameter {param} = {value} outside recommended range [{min_val}, {max_val}]")
    
    return errors, warnings

# Usage
errors, warnings = validate_config("config/my_config.cfg")
if errors:
    print("ERRORS:", errors)
if warnings:
    print("WARNINGS:", warnings)
if not errors and not warnings:
    print("Configuration is valid!")
```

---

## Performance Tuning

### Optimize for Speed

```bash
# Release build with optimizations
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native"
make -j$(nproc)
```

### Memory Optimization

```python
# Create memory-efficient brain configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)  # Smaller for lower memory usage
config.set("simulation_timestep", 0.001)
config.set("log_level", "WARNING")  # Reduce logging overhead

brain = pynlm.createBrain(config)
brain.initialize()
```

### Large-Scale Simulation Script

```python
# batch_simulation.py - Run multiple simulations efficiently
def run_batch_simulation(config_template, num_simulations, output_prefix):
    """Run multiple simulations with the same configuration."""
    for i in range(num_simulations):
        config = pynlm.createDefaultConfig()
        
        # Copy settings from template
        for key in config_template.getKeys():
            config.set(key, config_template.get(key))
        
        # Set simulation-specific seed
        config.set("random_seed", 1000 + i)
        
        # Run simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(config.getOr("max_simulation_steps", 1000)):
            brain.step(step)
        
        # Save results
        brain.save(f"{output_prefix}_sim{i}.bin")
        
        print(f"Completed simulation {i+1}/{num_simulations}")

# Usage
batch_sim = pynlm.BatchSimulation()
batch_sim.run_batch_simulation(config_template, 100, "experiment_results")
```

---

## Advanced Debugging and Validation

### Debug Mode

```python
# Debug brain with custom logging
config = pynlm.createDefaultConfig()
config.set("log_level", "DEBUG")

brain = pynlm.createBrain(config)
brain.initialize()

# Enable detailed debug logging
brain.enableDebugMode(True)

# Run simulation with debug output
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: Firing neurons={brain.getFiringNeuronCount()}, Total spikes={brain.getTotalSpikeCount()}")
```

### Neuron-level Debugging

```python
# Debug specific neuron
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Find and debug a specific neuron
neuron_id = 42
if brain.hasNeuron(neuron_id):
    neuron = brain.getNeuron(neuron_id)
    print(f"Neuron {neuron_id}: Type={neuron.getType()}, "
          f"Membrane potential={neuron.getMembranePotential()}, "
          f"Last spike time={neuron.getLastSpikeTime()}")
```

### Configuration Validation

```python
# config_validator.py - Comprehensive configuration validation
def validate_neural_parameters(config):
    """Validate neural network parameters for stability and performance."""
    issues = []
    
    # Check neuron parameters
    if config.has("brain.v_thresh"):
        thresh = config.get("brain.v_thresh")
        if thresh >= 0:
            issues.append(f"Threshold should be negative, got {thresh}")
    
    # Check connection density
    if config.has("brain.connection_probability"):
        prob = config.get("brain.connection_probability")
        if prob > 0.5:
            issues.append(f"High connection probability ({prob}) may cause instability")
    
    # Check plasticity parameters
    if config.has("brain.plasticity.hebbian.learning_rate"):
        rate = config.get("brain.plasticity.hebbian.learning_rate")
        if rate > 0.1:
            issues.append(f"High Hebbian learning rate ({rate}) may cause weight saturation")
    
    return issues

# Usage
validator = ConfigurationValidator()
config_issues = validator.validate_neural_parameters(config)
if config_issues:
    print("Configuration issues found:")
    for issue in config_issues:
        print(f"  - {issue}")
```

---

## Custom Configuration Templates

### Development Configuration

```bash
cat > config/dev.cfg << EOF
# Development configuration with detailed logging and smaller network
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
dopamine_baseline = 0.0
reward_discount_factor = 0.99
log_level = DEBUG
log_to_file = true
log_filename = nlm_dev.log
max_simulation_steps = 5000
simulation_time_limit = 0.0
EOF
```

### Research Configuration

```bash
cat > config/research.cfg << EOF
# Research configuration for reproducible experiments
random_seed = 12345678  # Large prime for good randomness
simulation_timestep = 0.0001  # Higher temporal resolution
neuron_count = 5000
region_count = 8
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 30.0
development_synaptogenesis_rate = 0.0002
development_pruning_rate = 0.00001
synaptogenesis_rate = 0.0005
pruning_rate = 0.00001
dopamine_baseline = 0.1
reward_discount_factor = 0.999
log_level = INFO
log_to_file = true
log_filename = nlm_research_12345678.log
max_simulation_steps = 20000
simulation_time_limit = 0.0
EOF
```

---

## Advanced Python API Usage

### Brain Manipulation

```python
import pynlm

# Create and configure brain with advanced settings
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.15)
config.set("brain.initial_weight_mean", 0.3)
config.set("brain.initial_weight_std", 0.05)

brain = pynlm.createBrain(config)
brain.initialize()

# Access and modify brain regions
regions = brain.getRegions()
for region in regions:
    print(f"Region {region.getId()}: {region.getNeuronCount()} neurons")
    
    # Modify region properties
    region.setActivityThreshold(0.5)
    region.enablePlasticity(True)

# Advanced synapse manipulation
synapses = brain.getSynapses()
for synapse in synapses:
    if synapse.getWeight() < 0.1:
        # Strengthen weak synapses
        synapse.setWeight(synapse.getWeight() * 1.5)
    elif synapse.getWeight() > 0.9:
        # Weaken strong synapses
        synapse.setWeight(synapse.getWeight() * 0.8)
```

### Memory Management

```python
# Advanced memory management
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create custom memory system
working_memory_config = {
    "capacity": 1000,
    "decay_rate": 0.01,
    "competition": True,
    "lateral_inhibition": True
}

brain.configureWorkingMemory(working_memory_config)

# Store and retrieve experiences
brain.storeExperience(
    input_pattern=[0.1, 0.2, 0.3],
    action=[1, 0, 0],
    reward=1.0,
    context="exploration"
)

# Replay specific experiences
replay_result = brain.replayExperience(
    pattern=[0.1, 0.2, 0.3],
    replay_count=5,
    strength=1.2
)
```

### Performance Monitoring

```python
# Performance monitoring for research
import time

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Monitor performance metrics
start_time = time.time()
step_times = []
memory_usage = []
firing_rates = []

for step in range(1000):
    step_start = time.time()
    brain.step(step)
    step_time = time.time() - step_start
    step_times.append(step_time)
    
    # Record metrics
    firing_rates.append(brain.getFiringNeuronCount() / brain.getTotalNeuronCount())
    memory_usage.append(brain.getMemoryUsage())

end_time = time.time()
total_time = end_time - start_time

# Performance analysis
print(f"Total simulation time: {total_time:.2f}s")
print(f"Average step time: {sum(step_times)/len(step_times)*1000:.2f}ms")
print(f"Average firing rate: {sum(firing_rates)/len(firing_rates)*100:.2f}%")
print(f"Max memory usage: {max(memory_usage):.2f} MB")

# Performance report
performance_report = {
    "total_time": total_time,
    "steps_per_second": 1000 / total_time,
    "avg_step_time_ms": sum(step_times)/len(step_times)*1000,
    "firing_rate_percent": sum(firing_rates)/len(firing_rates)*100,
    "memory_usage_mb": max(memory_usage)
}
```

---

## Troubleshooting

### Common Issues and Solutions

#### "Simulation hangs or stops responding"
```python
# Solution: Enable performance monitoring and reduce load
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)  # Reduce neuron count
config.set("max_simulation_steps", 1000)  # Reduce steps
config.set("log_level", "WARNING")  # Reduce logging overhead

brain = pynlm.createBrain(config)
brain.initialize()

# Monitor simulation progress
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Progress: {step}/1000 steps")
```

#### "Memory usage too high"
```python
# Solution: Optimize configuration for memory usage
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 200)  # Fewer neurons
config.set("brain.synapse_density", 0.05)  # Sparse connections
config.set("brain.v_thresh", -50.0)  # Standard threshold
config.set("brain.v_reset", -65.0)  # Standard reset

brain = pynlm.createBrain(config)
brain.initialize()

# Monitor memory
import psutil
process = psutil.Process()
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")
```

#### "Configuration file not loading"
```python
# Solution: Check file format and permissions
import os
config_file = "configs/my_config.cfg"

if not os.path.exists(config_file):
    print(f"Configuration file not found: {config_file}")
else:
    # Try to parse the file
    config = pynlm.createDefaultConfig()
    try:
        config.loadFromFile(config_file)
        print("Configuration loaded successfully")
        print(f"Keys: {config.getKeys()}")
    except Exception as e:
        print(f"Error loading configuration: {e}")
```

---

## Batch Processing and Automation

### Automated Experiment Runner

```python
# experiment_runner.py - Automated experiment management
import multiprocessing
import json
import time
from datetime import datetime

class ExperimentManager:
    def __init__(self, config_template):
        self.config_template = config_template
        self.results = []
    
    def run_experiment(self, experiment_id, config_overrides):
        """Run a single experiment with custom configuration."""
        config = pynlm.createDefaultConfig()
        
        # Copy template settings
        for key in self.config_template.getKeys():
            config.set(key, self.config_template.get(key))
        
        # Apply experiment-specific overrides
        for key, value in config_overrides.items():
            config.set(key, value)
        
        # Add experiment-specific seed for reproducibility
        config.set("random_seed", 10000 + experiment_id)
        
        # Run experiment
        start_time = time.time()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(config.getOr("max_simulation_steps", 1000)):
            brain.step(step)
        
        end_time = time.time()
        
        # Save results
        result = {
            "experiment_id": experiment_id,
            "duration": end_time - start_time,
            "final_spikes": brain.getTotalSpikeCount(),
            "firing_rate": brain.getAverageFiringRate(),
            "config": {k: config.get(k) for k in config.getKeys()}
        }
        
        self.results.append(result)
        return result
    
    def run_batch(self, experiments):
        """Run multiple experiments in parallel."""
        with multiprocessing.Pool() as pool:
            results = pool.starmap(
                self.run_experiment,
                [(exp['id'], exp.get('config_overrides', {})) for exp in experiments]
            )
        
        self.results.extend(results)
        return results
    
    def save_results(self, output_file):
        """Save experiment results to JSON file."""
        with open(output_file, 'w') as f:
            json.dump({
                "timestamp": datetime.now().isoformat(),
                "results": self.results
            }, f, indent=2)

# Usage example
template_config = pynlm.createDefaultConfig()
template_config.set("neuron_count", 1000)
template_config.set("max_simulation_steps", 500)

manager = ExperimentManager(template_config)

experiments = [
    {"id": 1, "config_overrides": {"connection_probability": 0.1}},
    {"id": 2, "config_overrides": {"connection_probability": 0.15}},
    {"id": 3, "config_overrides": {"connection_probability": 0.2}},
]

results = manager.run_batch(experiments)
manager.save_results("experiment_results.json")
```

---

## Advanced Features Checklist

### [ ] Configuration Validation
- [x] Parameter range checking
- [x] Required parameter validation
- [ ] Schema validation
- [ ] Type checking

### [ ] Performance Monitoring
- [x] Step timing
- [x] Memory usage tracking
- [x] Firing rate monitoring
- [ ] CPU profiling

### [ ] Advanced Debugging
- [x] Verbose mode
- [x] Custom logging levels
- [ ] Debug breakpoints
- [ ] Memory leak detection

### [ ] Batch Processing
- [x] Parallel experiment execution
- [x] Result aggregation
- [x] Automated parameter sweeps
- [ ] Experiment comparison

### [ ] Custom Configuration
- [x] Template management
- [x] Configuration validation
- [ ] Parameter search optimization
- [ ] Experiment reproducibility

### [ ] Advanced API
- [x] Brain manipulation
- [x] Memory management
- [x] Synapse control
- [ ] Neural recording

---

## Next Steps

After mastering the basics, consider these advanced topics:

1. **Neural Network Architecture Design**: Create custom brain topologies
2. **Learning Algorithm Optimization**: Modify plasticity rules for specific applications
3. **Multi-objective Optimization**: Balance competing objectives during learning
4. **Transfer Learning**: Apply knowledge from one task to another
5. **Meta-learning**: Learn how to learn more efficiently
6. **Hardware Acceleration**: Optimize for GPU/CPU performance
7. **Distributed Computing**: Run large simulations across multiple machines

The NLM framework provides a solid foundation for implementing advanced neural network research and applications.

---

*This Advanced Usage Guide is continuously updated. Check the NLM documentation for the latest features and improvements.*