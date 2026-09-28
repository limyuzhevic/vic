# NLM Performance Guide - Parameter Tuning and Optimization

## Overview

This guide provides comprehensive instructions for tuning NLM (Neural Learning Machine) parameters to achieve optimal performance, balance between speed and accuracy, and efficient resource usage.

## Performance Considerations

### 1. Neural Parameters

#### Neuron Configuration

| Parameter | Default | Range | Effect | Tuning Recommendation |
|-----------|---------|--------|--------|----------------------|
| `neuron_count` | 1000 | 100-10000 | Network capacity | Start with 500, increase 100 at a time |
| `v_thresh` | -50.0 | -60 to -40 mV | Excitability | Lower = more firing, Higher = more selective |
| `v_rest` | -70.0 | -75 to -65 mV | Baseline potential | Adjust based on desired firing rate |
| `v_reset` | -75.0 | -80 to -70 mV | Post-spike reset | Keep same as v_rest for stability |
| `tau_mem` | 20.0 | 10-40 ms | Membrane time constant | Lower = faster response, Higher = smoother |
| `tau_ref` | 2.0 | 1-5 ms | Refractory period | Keep 1-2 ms for biological realism |

#### Synaptic Parameters

| Parameter | Default | Range | Effect | Tuning Recommendation |
|-----------|---------|--------|--------|----------------------|
| `synapse_density` | 0.1 | 0.01-0.3 | Connection probability | Start 0.05, increase if learning slow |
| `connection_probability` | 0.05 | 0.01-0.1 | Specific connectivity | Lower = sparser, Higher = denser |
| `initial_weight_mean` | 0.5 | 0.1-1.0 | Initial connection strength | 0.5 = balanced excitation/inhibition |
| `initial_weight_std` | 0.1 | 0.01-0.2 | Weight variability | Higher = more diverse dynamics |

### 2. Plasticity Parameters

#### STDP Parameters

| Parameter | Default | Range | Effect | Tuning Recommendation |
|-----------|---------|--------|--------|----------------------|
| `plasticity.stdp.learning_rate` | 0.001 | 0.0001-0.01 | Learning magnitude | Start 0.0001, increase for faster learning |
| `plasticity.stdp.tau_plus` | 20.0 | 10-40 ms | LTP time constant | Longer = more temporal precision |
| `plasticity.stdp.tau_minus` | 20.0 | 10-40 ms | LTD time constant | Match tau_plus for symmetry |

#### Hebbian Parameters

| Parameter | Default | Range | Effect | Tuning Recommendation |
|-----------|---------|--------|--------|----------------------|
| `plasticity.hebbian.enable` | true | true/false | Enable Hebbian learning | Enable for pattern completion |

#### Structural Plasticity

| Parameter | Default | Range | Effect | Tuning Recommendation |
|-----------|---------|--------|--------|----------------------|
| `plasticity.structural.enable` | true | true/false | Enable synapse growth | Enable for network development |
| Synaptogenesis rate | Variable | 0.01-0.1/step | New synapse creation | Start 0.01, increase gradually |
| Pruning rate | Variable | 0.01-0.1/step | Synapse removal | Start 0.01, adjust based on density |

### 3. Neuromodulation Parameters

#### Dopamine System

| Parameter | Default | Range | Effect | Tuning Recommendation |
|-----------|---------|--------|--------|----------------------|
| `neuromod.dopamine.scale` | 1.0 | 0.5-2.0 | Reward modulation strength | 1.0 = balanced learning |

#### Curiosity System

| Parameter | Default | Range | Effect | Tuning Recommendation |
|-----------|---------|--------|--------|----------------------|
| `neuromod.curiosity.enable` | true | true/false | Enable exploration | Enable for novel environments |
| Novelty threshold | Variable | 0.1-0.5 | What counts as novel | Lower = more exploration |

#### Novelty Detection

| Parameter | Default | Range | Effect | Tuning Recommendation |
|-----------|---------|--------|--------|----------------------|
| `neuromod.novelty.enable` | true | true/false | Enable novelty detection | Enable for adaptive behavior |
| Novelty sensitivity | Variable | 0.1-1.0 | Response to novelty | Adjust based on environment complexity |

### 4. Memory System Parameters

#### Working Memory

| Parameter | Default | Range | Effect | Tuning Recommendation |
|-----------|---------|--------|--------|----------------------|
| Working memory capacity | 100 | 10-500 | Number of stored patterns | Increase with neuron count |
| Decay rate | 0.01/step | 0.001-0.1/step | Pattern retention | Faster decay = shorter memory |
| Competition strength | 0.5 | 0.1-1.0 | Memory access priority | Higher = more competitive |

#### Episodic Memory

| Parameter | Default | Range | Effect | Tuning Recommendation |
|-----------|---------|--------|--------|----------------------|
| Replay frequency | Every 100 steps | Variable | Memory consolidation | Adjust based on learning speed |
| Consolidation threshold | 0.5 | 0.1-1.0 | What gets consolidated | Higher = only strong memories |

## Tuning Strategy

### Phase 1: Baseline Configuration

1. **Start with defaults** - Use default parameters as baseline
2. **Simple test** - Run minimal simulation (100 neurons, 100 steps)
3. **Observe behavior** - Check firing rates, learning progress

```python
import pynlm

# Baseline configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.learning_rate", 0.0001)

brain = pynlm.createBrain(config)
brain.initialize()

# Quick test
for i in range(100):
    brain.step(i)
```

### Phase 2: Parameter Exploration

#### Speed vs Accuracy Trade-off

**For Fast Performance (Real-time applications)**:
```python
# Faster, simpler dynamics
config.set("brain.tau_mem", 10.0)           # Faster response
config.set("brain.tau_ref", 1.0)           # Shorter refractory
config.set("plasticity.stdp.learning_rate", 0.00001)  # Smaller updates
config.set("brain.neuron_count", 200)      # Fewer neurons
```

**For High Accuracy (Research applications)**:
```python
# Slower, more precise dynamics
config.set("brain.tau_mem", 40.0)          # Slower integration
config.set("brain.tau_ref", 5.0)           # Longer refractory
config.set("plasticity.stdp.learning_rate", 0.001)      # Larger updates
config.set("brain.neuron_count", 2000)     # More neurons
config.set("brain.synapse_density", 0.15)   # More connections
```

#### Exploration vs Exploitation

**High Exploration** (curiosity-driven):
```python
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.noise_level", 0.5)
config.set("neuromod.novelty.threshold", 0.2)
```

**High Exploitation** (learning-focused):
```python
config.set("neuromod.curiosity.enable", False)
config.set("neuromod.novelty.threshold", 0.8)
```

### Phase 3: Performance Optimization

#### Vectorization and SIMD

For C++ builds, enable performance optimizations:

```bash
# Release build with optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake .. -DCMAKE_CXX_FLAGS="-O3 -march=native -ftree-vectorize"

# Enable SIMD for neural computations
make -j$(nproc)
```

#### Memory Optimization

```python
# For large networks, use memory-efficient configuration
config.set("brain.neuron_count", 1000)  # Manageable size
config.set("brain.synapse_density", 0.05)  # Sparse connections

# Use checkpoint system for long simulations
checkpoint_manager = pynlm.createCheckpointManager()
checkpoint_manager.setAutoSaveInterval(1000)
```

#### Parallel Processing

If using multi-core systems:

```cpp
// In CMakeLists.txt or build configuration
add_definitions(-DNLM_PARALLEL -DNLM_NUM_THREADS=4)
```

## Advanced Tuning Techniques

### 1. Dynamic Parameter Adjustment

```python
class AdaptiveBrain:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.performance_history = []
        self.adaptation_rate = 0.01
    
    def adapt_parameters(self, simulation_results):
        # Adjust based on learning performance
        if simulation_results['learning_speed'] < target_speed:
            # Increase learning rate
            current_lr = self.config.get("plasticity.stdp.learning_rate")
            self.config.set("plasticity.stdp.learning_rate", 
                          current_lr * 1.1)
        
        # Reduce exploration if over-exploiting
        if simulation_results['exploration_rate'] > 0.8:
            current_threshold = self.config.get("neuromod.novelty.threshold")
            self.config.set("neuromod.novelty.threshold",
                          current_threshold * 0.9)
    
    def run_simulation(self, duration):
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        for step in range(duration):
            # Run simulation step
            brain.step(step)
            
            # Periodically adapt parameters
            if step % 1000 == 0:
                results = self.get_performance_metrics(brain)
                self.adapt_parameters(results)
```

### 2. Parameter Search

```python
import numpy as np
from scipy import optimize

def performance_metric(params):
    """Objective function for parameter optimization."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", int(params[0]))
    config.set("plasticity.stdp.learning_rate", params[1])
    config.set("neuromod.novelty.threshold", params[2])
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)
    
    # Return performance score (higher is better)
    return brain.getAverageFiringRate() * brain.getTotalSpikeCount()

# Parameter bounds
bounds = [(100, 5000),    # neuron_count
          (0.00001, 0.01),  # learning_rate
          (0.1, 1.0)]       # novelty_threshold

# Run optimization
result = optimize.minimize(
    performance_metric,
    x0=[1000, 0.001, 0.5],
    bounds=bounds,
    method='L-BFGS-B'
)

print(f"Optimal parameters: {result.x}")
print(f"Best performance: {result.fun}")
```

### 3. Temperature Scaling

```python
class TemperatureController:
    """Controls learning temperature for exploration-exploitation balance."""
    
    def __init__(self, initial_temp=1.0):
        self.temperature = initial_temp
        self.adaptation_schedule = 'exponential'
    
    def update_temperature(self, step, total_steps):
        if self.adaptation_schedule == 'exponential':
            # Exponential decay
            self.temperature = max(0.01, 
                                 self.temperature * (0.999 ** step))
        elif self.adaptation_schedule == 'linear':
            # Linear decay
            self.temperature = max(0.01,
                                  initial_temp * (1.0 - step / total_steps))
    
    def apply_temperature(self, config):
        """Apply temperature to learning parameters."""
        # Scale learning rate with temperature
        base_lr = 0.001
        config.set("plasticity.stdp.learning_rate",
                 base_lr * self.temperature)
        
        # Scale novelty threshold with temperature
        base_threshold = 0.5
        config.set("neuromod.novelty.threshold",
                 base_threshold * (1.0 / self.temperature))
```

## Performance Monitoring

### 1. Basic Metrics

```python
def monitor_performance(brain, world, agent, duration):
    metrics = {
        'firing_rate': [],
        'spike_count': [],
        'learning_progress': [],
        'exploration_rate': [],
        'memory_usage': [],
        'execution_time': []
    }
    
    for step in range(duration):
        import time
        step_start = time.time()
        
        # Run simulation step
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record metrics
        metrics['firing_rate'].append(brain.getFiringNeuronCount())
        metrics['spike_count'].append(brain.getTotalSpikeCount())
        metrics['learning_progress'].append(
            brain.getNeuromodulationLevel())
        metrics['exploration_rate'].append(agent.getCuriosityLevel())
        
        execution_time = time.time() - step_start
        metrics['execution_time'].append(execution_time)
    
    return metrics
```

### 2. Performance Profiling

```python
class PerformanceProfiler:
    """Profile simulation performance and identify bottlenecks."""
    
    def __init__(self):
        self.timings = {}
        self.memory_usage = {}
    
    def profile_neural_computation(self, brain, steps):
        import time
        
        # Time neural step execution
        start_time = time.time()
        for step in range(steps):
            brain.step(step)
        end_time = time.time()
        
        self.timings['neural_step'] = (end_time - start_time) / steps
        self.timings['spike_processing'] = self.measure_spike_processing(brain, steps)
        self.timings['plasticity_application'] = self.measure_plasticity(brain, steps)
    
    def profile_memory_usage(self, brain):
        import tracemalloc
        
        tracemalloc.start()
        # Run simulation
        for i in range(100):
            brain.step(i)
        
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        self.memory_usage['neural_memory'] = peak / 1024 / 1024  # MB
    
    def profile_scaling(self, neuron_counts):
        """Profile how performance scales with network size."""
        results = {}
        
        for neuron_count in neuron_counts:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", neuron_count)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Time simulation
            import time
            start_time = time.time()
            for i in range(100):
                brain.step(i)
            end_time = time.time()
            
            results[neuron_count] = {
                'time_per_step': (end_time - start_time) / 100,
                'spikes_per_step': brain.getTotalSpikeCount() / 100,
                'memory_per_step': self.measure_step_memory(brain)
            }
        
        return results
    
    def generate_report(self):
        """Generate performance analysis report."""
        report = f"Performance Analysis Report\n"
        report += f"==========================\n\n"
        
        report += f"Neural Computation:\n"
        for operation, time in self.timings.items():
            report += f"  {operation}: {time*1000:.2f} ms per step\n"
        
        report += f"\nMemory Usage:\n"
        for usage_type, memory in self.memory_usage.items():
            report += f"  {usage_type}: {memory:.1f} MB\n"
        
        return report
```

## Validation and Testing

### 1. Performance Validation

```python
def validate_performance(baseline_config, optimized_config):
    """Compare performance of baseline vs optimized configuration."""
    
    # Test baseline
    baseline_brain = pynlm.createBrain(baseline_config)
    baseline_results = run_performance_test(baseline_brain)
    
    # Test optimized
    optimized_brain = pynlm.createBrain(optimized_config)
    optimized_results = run_performance_test(optimized_brain)
    
    # Compare results
    comparison = {}
    for metric in baseline_results:
        baseline_val = baseline_results[metric]
        optimized_val = optimized_results[metric]
        
        if baseline_val != 0:
            improvement = (optimized_val - baseline_val) / baseline_val * 100
        else:
            improvement = float('inf') if optimized_val > 0 else 0
        
        comparison[metric] = {
            'baseline': baseline_val,
            'optimized': optimized_val,
            'improvement_percent': improvement
        }
    
    return comparison
```

### 2. Consistency Testing

```python
def test_consistency(brain_configs, num_runs=10):
    """Test that parameter choices produce consistent results."""
    
    all_results = []
    
    for config in brain_configs:
        run_results = []
        for run in range(num_runs):
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for i in range(100):
                brain.step(i)
            
            run_results.append({
                'firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'final_neuron_count': brain.getFiringNeuronCount()
            })
        
        all_results.append((config, run_results))
    
    # Calculate consistency metrics
    consistency_report = {}
    for config, results in all_results:
        config_id = str(config.getKeys())
        
        for metric in results[0].keys():
            values = [r[metric] for r in results]
            mean_val = np.mean(values)
            std_val = np.std(values)
            cv = std_val / mean_val if mean_val > 0 else 0
            
            consistency_report[f"{config_id}_{metric}"] = {
                'mean': mean_val,
                'std': std_val,
                'coefficient_of_variation': cv,
                'consistency': 'high' if cv < 0.1 else 'medium' if cv < 0.3 else 'low'
            }
    
    return consistency_report
```

## Configuration Templates

### Template 1: Exploration-Focused

```python
exploration_config = pynlm.createDefaultConfig()
exploration_config.set("brain.neuron_count", 1500)
exploration_config.set("brain.synapse_density", 0.08)
exploration_config.set("plasticity.stdp.learning_rate", 0.0005)
exploration_config.set("neuromod.curiosity.enable", True)
exploration_config.set("neuromod.curiosity.noise_level", 0.3)
exploration_config.set("neuromod.novelty.threshold", 0.3)
exploration_config.set("working.memory.capacity", 200)
```

### Template 2: Exploitation-Focused

```python
exploitation_config = pynlm.createDefaultConfig()
exploitation_config.set("brain.neuron_count", 800)
exploitation_config.set("brain.synapse_density", 0.04)
exploitation_config.set("plasticity.stdp.learning_rate", 0.002)
exploitation_config.set("neuromod.curiosity.enable", False)
exploitation_config.set("neuromod.novelty.threshold", 0.7)
exploitation_config.set("working.memory.capacity", 100)
```

### Template 3: Balanced

```python
balanced_config = pynlm.createDefaultConfig()
balanced_config.set("brain.neuron_count", 1000)
balanced_config.set("brain.synapse_density", 0.06)
balanced_config.set("plasticity.stdp.learning_rate", 0.001)
balanced_config.set("neuromod.curiosity.enable", True)
balanced_config.set("neuromod.curiosity.noise_level", 0.2)
balanced_config.set("neuromod.novelty.threshold", 0.5)
balanced_config.set("working.memory.capacity", 150)
```

This performance guide provides comprehensive instructions for tuning NLM parameters to achieve optimal results in various applications. Use these techniques to balance exploration vs exploitation, speed vs accuracy, and consistency vs variability according to your specific requirements.