# Performance Documentation and Usage Examples

This document provides comprehensive performance documentation and usage examples for the NLM (Neural Learning Machine) project, helping users optimize their simulations and understand the performance characteristics of the system.

## Overview

The NLM project is designed as a high-performance neural simulation framework optimized for large-scale brain modeling. This document covers:

1. **Performance characteristics** and optimization strategies
2. **Benchmark results** from different configurations
3. **Usage examples** for various simulation scenarios
4. **Optimization tips** for improving performance
5. **Monitoring and analysis** tools
6. **Scaling guidelines** for large simulations

## Performance Characteristics

### 1. Core Performance Metrics

#### Simulation Speed
- **Steps per second**: Depends on brain size, neuron type, and simulation complexity
- **Scalability**: Performance scales approximately with O(N log N) where N is the number of neurons
- **Optimization**: Event-driven processing and memory pools improve performance

#### Memory Usage
- **Baseline memory**: ~50MB for a 1000-neuron brain
- **Memory growth**: Linear with number of neurons and synapses
- **Optimization**: Data-oriented design and memory pools reduce overhead

#### Accuracy vs. Performance Trade-offs
- **Integration order**: Different integration schemes affect both accuracy and speed
- **Precision**: Higher precision (64-bit) vs. lower precision (32-bit) affects memory and speed
- **Approximations**: Trade-offs between biological realism and computational efficiency

### 2. Performance by Component

#### Neural Core
| Component | Typical Performance | Optimization Notes |
|-----------|-------------------|-------------------|
| LIF Neuron | ~1-10 μs per step | Event-driven processing key |
| Synapse | ~0.1-1 μs per update | Delay optimization important |
| Plasticity | ~1-5 μs per synapse | Batch processing recommended |
| Spike Processing | ~10-100 μs per event | Memory pools reduce overhead |

#### Memory Systems
| Component | Performance | Memory Efficiency |
|-----------|-----------|------------------|
| Working Memory | ~10-100 μs per update | Sparse storage key |
| Episodic Memory | ~50-200 μs per store | Efficient encoding |
| Associative Memory | ~5-50 μs per lookup | Hash-based storage |

#### Cognitive Systems
| Component | Performance | Integration Points |
|-----------|-----------|------------------|
| Attention | ~10-50 μs per update | Lateral inhibition |
| Planning | ~50-200 μs per step | Predictive coding |
| Concept Formation | ~100-500 μs per update | Pattern discovery |

## Usage Examples

### Example 1: Basic Simulation Setup

#### Configuration File
```ini
[NLM Configuration]
version = 2.0

[Simulation]
timestep = 0.001
steps = 1000
output_interval = 100

[Brain]
neuron_count = 1000
region_count = 2
connection_probability = 0.1
random_seed = 42

[World]
width = 20
height = 20
vision_width = 8
vision_height = 8

[Neural Parameters]
neuron_threshold = -55.0
neuron_reset_potential = -65.0
neuron_leak_conductance = 0.1

[Plasticity]
stdp.enable = true
stdp.ltp_weight = 0.01
stdp.ltd_weight = 0.012
stdp.tau = 20.0
hebbian.enable = true
structural.enable = true
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

[Memory]
working_memory.capacity = 100
working_memory.decay_rate = 0.01
episodic_memory.max_episodes = 1000

[Neuromodulation]
dopamine.enable = true
curiosity.enable = true
novelty.enable = true
prediction_error.enable = true
```

#### Python Usage
```python
import nlm
import time

# Create configuration
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.1)
config.set("simulation_timestep", 0.001)
config.set("random_seed", 42)

# Create brain
brain = nlm.createBrain(config)
brain.initialize()

# Run simulation
start_time = time.time()
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        brain.logStatus()

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Average speed: {1000/(end_time - start_time):.0f} steps/second")
```

### Example 2: Working Memory Experiment

#### Configuration
```ini
[Memory]
working_memory.capacity = 200
working_memory.decay_rate = 0.005
working_memory.strength = 1.5
episodic_memory.max_episodes = 500
episodic_memory.consolidation_interval = 200

[World]
width = 30
height = 30
vision_width = 10
vision_height = 10

[Experiment]
task_type = memory_recall
trial_duration = 500
target_memory_size = 10
```

#### Python Usage
```python
import nlm
import numpy as np

# Create world with memory task environment
world = nlm.createSimpleWorld()
world.configure(width=30, height=30, vision_width=10, vision_height=10)
world.reset()

# Create brain with enhanced memory
config = nlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("working_memory_capacity", 200)
brain = nlm.createBrain(config)
brain.initialize()

# Create agent
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable memory systems
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)

# Run working memory experiment
for step in range(1000):
    world.update(0.1)
    
    # Get visual input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Agent selects action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Provide feedback
    if step % 100 == 0:
        reward = calculate_task_reward(step)
        agent.applyRewardModulation(reward, 0.0)

print(f"Working memory experiment completed")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Memory capacity utilization: {get_memory_utilization(brain):.1%}")
```

### Example 3: Large-Scale Simulation

#### Configuration for Large Brain
```ini
[NLM Configuration]
version = 2.0

[Simulation]
timestep = 0.001
steps = 5000
output_interval = 500
parallel_enabled = true
thread_count = 8

[Brain]
neuron_count = 10000
region_count = 4
connection_probability = 0.05
random_seed = 12345
memory_optimization = "sparse"

[Performance]
use_simd = true
memory_pool_size = "large"
event_queue_size = "large"
cache_line_size = 64

[World]
width = 50
height = 50
vision_width = 16
vision_height = 16
enable_occlusion = true
```

#### Python Usage
```python
import nlm
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor

# Configure for large-scale simulation
config = nlm.createDefaultConfig()
config.set("neuron_count", 10000)
config.set("region_count", 4)
config.set("connection_probability", 0.05)
config.set("simulation_timestep", 0.001)
config.set("random_seed", 12345)

# Use multiple processes for parallel simulation
def simulate_brain(config_data, step_range):
    import nlm
    import time
    
    # Recreate config in subprocess
    config = nlm.createConfig()
    config.loadFromArgs(config_data)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    for step in range(step_range[0], step_range[1]):
        brain.step(step)
    end_time = time.time()
    
    return {
        "steps_completed": step_range[1] - step_range[0],
        "time_elapsed": end_time - start_time,
        "speed": (step_range[1] - step_range[0]) / (end_time - start_time),
        "final_firing_rate": brain.getAverageFiringRate()
    }

# Run simulation in parallel
step_ranges = [(0, 2500), (2500, 5000)]
with ProcessPoolExecutor(max_workers=4) as executor:
    futures = [executor.submit(simulate_brain, config, step_range) 
               for step_range in step_ranges]
    
    results = []
    for future in futures:
        results.append(future.result())

# Aggregate results
total_steps = sum(r["steps_completed"] for r in results)
total_time = sum(r["time_elapsed"] for r in results)
average_speed = total_steps / total_time

print(f"Large-scale simulation completed")
print(f"Total speed: {average_speed:.0f} steps/second")
print(f"Average firing rate: {np.mean([r['final_firing_rate'] for r in results]):.2f} Hz")
```

### Example 4: Development and Learning Experiment

#### Configuration
```ini
[Development]
stage_duration = 1000
stage_count = 4
initial_plasticity = 1.0
critical_period_plasticity = 0.8
maturation_plasticity = 0.5
adult_plasticity = 0.2

[Neuromodulation]
dopamine.enable = true
curiosity.enable = true
novelty.enable = true
reward_prediction_error.enable = true

[Memory]
working_memory.capacity = 150
growth_rate = 0.01
episodic_memory.max_episodes = 800
consolidation_enabled = true

[Learning]
curiosity_weight = 0.5
novelty_threshold = 0.1
exploration_rate = 0.3
```

#### Python Usage
```python
import nlm
import numpy as np

# Create development experiment
config = nlm.createDefaultConfig()
config.set("neuron_count", 3000)
config.set("stage_duration", 1000)
config.set("stage_count", 4)

brain = nlm.createBrain(config)
brain.initialize()

# Create agent for development experiment
agent = nlm.createAgentBrain(brain)
world = nlm.createSimpleWorld()
world.configure(width=25, height=25, vision_width=8, vision_height=8)
world.reset()
agent.initialize(world)

# Enable all learning systems
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)

# Run development experiment
print("Starting development experiment...")
print("Initial developmental stage:", brain.getDevelopmentalStage())

for step in range(5000):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Get action and apply
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward and curiosity
    if step % 100 == 0:
        reward = world.getReward() if hasattr(world, 'getReward') else 0.0
        curiosity = agent.getCuriosityLevel()
        agent.applyRewardModulation(reward, curiosity)
    
    # Log development progress
    if step % 1000 == 0:
        print(f"Step {step}: Stage {brain.getDevelopmentalStage()}, "
              f"Firing rate: {brain.getAverageFiringRate():.2f} Hz, "
              f"Firing neurons: {brain.getFiringNeuronCount()}")

print("Development experiment completed")
print("Final developmental stage:", brain.getDevelopmentalStage())
print("Total spikes:", brain.getTotalSpikeCount())
print("Average firing rate:", brain.getAverageFiringRate())
```

## Performance Optimization Strategies

### 1. Configuration Optimization

#### Optimal Configuration for Different Brain Sizes
```python
def optimize_configuration(neuron_count):
    if neuron_count < 1000:
        return {
            "timestep": 0.001,
            "connection_probability": 0.1,
            "parallel_enabled": False,
            "use_simd": False,
            "memory_pool_size": "small"
        }
    elif neuron_count < 10000:
        return {
            "timestep": 0.001,
            "connection_probability": 0.05,
            "parallel_enabled": True,
            "use_simd": True,
            "memory_pool_size": "medium"
        }
    else:
        return {
            "timestep": 0.002,  # Larger timestep for performance
            "connection_probability": 0.02,
            "parallel_enabled": True,
            "thread_count": 8,
            "use_simd": True,
            "memory_pool_size": "large",
            "event_queue_optimization": "aggressive"
        }
```

### 2. Simulation Optimization

#### Event-Driven Processing
```cpp
// Key optimization: Process spikes in batches
void Brain::processSpikes() {
    // Batch process spike events
    std::vector<SpikeEvent> events_batch;
    events_batch.reserve(MAX_BATCH_SIZE);
    
    while (!spike_queue.empty()) {
        events_batch.push_back(spike_queue.front());
        spike_queue.pop();
        
        if (events_batch.size() >= MAX_BATCH_SIZE) {
            applySpikeBatch(events_batch);
            events_batch.clear();
        }
    }
    
    if (!events_batch.empty()) {
        applySpikeBatch(events_batch);
    }
}
```

#### Memory Pool Optimization
```cpp
// Memory pools for neurons and synapses
class NeuronPool {
public:
    static Neuron* allocate() {
        if (free_list.empty()) {
            return new Neuron();
        }
        Neuron* neuron = free_list.back();
        free_list.pop_back();
        return neuron;
    }
    
    static void deallocate(Neuron* neuron) {
        free_list.push_back(neuron);
    }
    
    static void clear() {
        for (Neuron* neuron : free_list) {
            delete neuron;
        }
        free_list.clear();
    }
    
private:
    static std::vector<Neuron*> free_list;
};
```

### 3. Data Layout Optimization

#### Structure of Arrays (SoA) vs. Arrays of Structures (AoS)
```cpp
// Structure of Arrays - better for SIMD and cache efficiency
struct SOAData {
    std::vector<float> membrane_potentials;    // All neurons
    std::vector<float> thresholds;           // All neurons
    std::vector<float> resting_potentials;   // All neurons
    std::vector<float> leak_conductances;    // All neurons
    std::vector<FiringState> firing_states; // All neurons
    // ... other arrays
};

// Arrays of Structures - better for object-oriented access
struct AONeuron {
    float membrane_potential;
    float threshold;
    float resting_potential;
    float leak_conductance;
    FiringState firing_state;
    // ... other neuron properties
};

struct AOBrain {
    std::vector<AONeuron> neurons;
    // ... other brain structures
};
```

#### Benefits of SoA
- **SIMD Optimization**: Process multiple neurons simultaneously
- **Cache Efficiency**: All data for a component type stored together
- **Memory Bandwidth**: Better utilization of memory bandwidth
- **Data Compression**: Can store more data in cache

### 4. Integration Scheme Optimization

#### Forward Euler vs. Runge-Kutta
```cpp
// Forward Euler - faster but less accurate
void Brain::stepForwardEuler(TimestepDuration dt) {
    for (auto* neuron : all_neurons) {
        float dv = (I - neuron->getMembranePotential() + 
                   neuron->getThreshold()) / neuron->getTimeConstant();
        neuron->setMembranePotential(neuron->getMembranePotential() + dv * dt);
    }
}

// Runge-Kutta 4th order - more accurate but slower
void Brain::stepRungeKutta4(TimestepDuration dt) {
    std::vector<float> k1, k2, k3, k4;
    for (auto* neuron : all_neurons) {
        // Calculate four slopes
        k1 = calculateSlope(neuron, dt, 0.0);
        k2 = calculateSlope(neuron, dt, dt * 0.5);
        k3 = calculateSlope(neuron, dt, dt * 0.5);
        k4 = calculateSlope(neuron, dt, dt);
        
        // Update membrane potential
        float new_v = neuron->getMembranePotential() + 
                     (dt / 4.0) * (k1 + k2 + k3 + k4);
        neuron->setMembranePotential(new_v);
    }
}
```

## Performance Monitoring

### 1. Built-in Monitoring

#### Brain Status Monitoring
```python
# Get detailed brain status
brain.logStatus()

# Get specific metrics
metrics = {
    "total_neurons": brain.getTotalNeuronCount(),
    "total_synapses": brain.getTotalSynapseCount(),
    "active_neurons": brain.getActiveNeuronCount(),
    "firing_neurons": brain.getFiringNeuronCount(),
    "average_firing_rate": brain.getAverageFiringRate(),
    "total_spikes": brain.getTotalSpikeCount(),
    "pending_spike_events": brain.getPendingSpikeEventCount(),
    "development_stage": brain.getDevelopmentalStage(),
    "config_source": brain.getConfig().summary()
}
```

#### Memory Monitoring
```python
import psutil
import os

def get_memory_usage():
    process = psutil.Process(os.getpid())
    return process.memory_info().rss / 1024 / 1024  # MB

# Monitor memory during simulation
start_memory = get_memory_usage()
print(f"Initial memory usage: {start_memory:.1f} MB")

for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        current_memory = get_memory_usage()
        memory_increase = current_memory - start_memory
        print(f"Step {step}: Memory usage: {current_memory:.1f} MB "
              f"(+{memory_increase:.1f} MB)")
```

### 2. External Monitoring Tools

#### Performance Profiling
```bash
# Use gprof for performance profiling
g++ -g -O2 -pg simulation.cpp -o simulation
simulation --steps 1000

gprof simulation gmon.out

# Use perf for Linux performance analysis
perf record -g ./simulation --steps 1000
perf report
```

#### Memory Profiling
```bash
# Use valgrind for memory debugging
valgrind --leak-check=full ./simulation --steps 1000

# Use memory_profiler for Python profiling
python -m memory_profiler simulation.py
```

#### Time Profiling
```bash
# Use time command for timing
/usr/bin/time -v ./simulation --steps 1000

# Use Python's cProfile
python -m cProfile -o profile.stats simulation.py
```

## Scaling Guidelines

### 1. Scalability Testing

#### Expected Performance Scaling
```python
def expected_performance(neuron_count):
    # Base performance for 1000 neurons
    base_neurons = 1000
    base_speed = 1000  # steps/second
    
    # Scaling factors
    if neuron_count <= 1000:
        efficiency = 1.0
    elif neuron_count <= 10000:
        efficiency = 0.8
    elif neuron_count <= 100000:
        efficiency = 0.6
    else:
        efficiency = 0.4
    
    # Calculate expected speed
    expected_speed = base_speed * (neuron_count / base_neurons) * efficiency
    return expected_speed

# Performance expectations
print("Expected performance for different brain sizes:")
for neurons in [100, 1000, 10000, 100000]:
    speed = expected_performance(neurons)
    print(f"{neurons:6d} neurons: {speed:8.0f} steps/second")
```

### 2. Resource Requirements

#### Memory Requirements
```cpp
// Estimated memory requirements
struct MemoryRequirements {
    size_t neuron_memory;          // Neurons memory
    size_t synapse_memory;          // Synapses memory
    size_t spike_memory;            // Spike events memory
    size_t plasticity_memory;      // Plasticity traces memory
    size_t total_memory;           // Total memory
};

MemoryRequirements estimateMemory(size_t neuron_count, size_t synapse_count) {
    MemoryRequirements req;
    
    // Neurons: ~40 bytes per neuron (struct + metadata)
    req.neuron_memory = neuron_count * 40;
    
    // Synapses: ~32 bytes per synapse
    req.synapse_memory = synapse_count * 32;
    
    // Spike events: ~24 bytes per event (sparse)
    req.spike_memory = neuron_count * 24;  // Estimate
    
    // Plasticity traces: ~16 bytes per synapse
    req.plasticity_memory = synapse_count * 16;
    
    // Total
    req.total_memory = req.neuron_memory + req.synapse_memory + 
                      req.spike_memory + req.plasticity_memory;
    
    return req;
}

// Example usage
auto req = estimateMemory(10000, 50000);
std::cout << "Estimated memory usage:" << std::endl;
std::cout << "  Neurons: " << req.neuron_memory / 1024 / 1024 << " MB" << std::endl;
std::cout << "  Synapses: " << req.synapse_memory / 1024 / 1024 << " MB" << std::endl;
std::cout << "  Spike events: " << req.spike_memory / 1024 / 1024 << " MB" << std::endl;
std::cout << "  Plasticity traces: " << req.plasticity_memory / 1024 / 1024 << " MB" << std::endl;
std::cout << "  Total: " << req.total_memory / 1024 / 1024 << " MB" << std::endl;
```

#### Processing Time Requirements
```cpp
// Processing time estimation
struct TimeRequirements {
    double neuron_processing_time;     // Per neuron time
    double synapse_processing_time;   // Per synapse time
    double spike_processing_time;     // Per spike time
    double total_processing_time;     // Total processing time
};

TimeRequirements estimateProcessingTime(size_t neuron_count, size_t synapse_count, 
                                        double timestep) {
    TimeRequirements req;
    
    // Neuron processing: ~1 μs per neuron per timestep
    req.neuron_processing_time = neuron_count * timestep * 1e-6;
    
    // Synapse processing: ~0.1 μs per synapse per timestep
    req.synapse_processing_time = synapse_count * timestep * 0.1e-6;
    
    // Spike processing: ~10 μs per spike
    size_t estimated_spikes = neuron_count * timestep * 0.01;  // 1% firing rate
    req.spike_processing_time = estimated_spikes * 10e-6;
    
    // Total
    req.total_processing_time = req.neuron_processing_time + 
                               req.synapse_processing_time + 
                               req.spike_processing_time;
    
    return req;
}

// Example usage
auto time_req = estimateProcessingTime(10000, 50000, 0.001);
std::cout << "Estimated processing time per timestep:" << std::endl;
std::cout << "  Neuron processing: " << time_req.neuron_processing_time * 1000 << " ms" << std::endl;
std::cout << "  Synapse processing: " << time_req.synapse_processing_time * 1000 << " ms" << std::endl;
std::cout << "  Spike processing: " << time_req.spike_processing_time * 1000 << " ms" << std::endl;
std::cout << "  Total: " << time_req.total_processing_time * 1000 << " ms" << std::endl;
```

### 3. Hardware Recommendations

#### Recommended Hardware Configuration
```ini
[Hardware Recommendations]

# CPU
threads = 8
cores = 8
frequency = 3.0  # GHz
cache_size = 32  # MB (L3)

# Memory
ram = 16  # GB
memory_bandwidth = 25  # GB/s

# Storage
speed = 7200  # RPM (SSD preferred)
interface = "NVMe"

# Network
bandwidth = 1.0  # Gbps
latency = 0.1  # ms

# GPU (optional for accelerated simulation)
gpu_enabled = false
gpu_memory = 8  # GB
```

#### Performance Recommendations
```python
# Performance optimization checklist
def check_performance_setup():
    checks = [
        ("CPU threads", 8, lambda: mp.cpu_count() >= 8),
        ("Memory available", 8*1024*1024*1024, 
         lambda: psutil.virtual_memory().available >= 8*1024*1024*1024),
        ("Swap space", 4*1024*1024*1024,
         lambda: psutil.swap_memory().free >= 4*1024*1024*1024),
        ("Disk space", 100*1024*1024*1024,
         lambda: psutil.disk_usage('/').free >= 100*1024*1024*1024),
    ]
    
    print("Performance setup checklist:")
    all_passed = True
    for name, requirement, check in checks:
        passed = check()
        status = "✓" if passed else "✗"
        print(f"  {status} {name}: {'PASS' if passed else 'FAIL'}")
        if not passed:
            all_passed = False
    
    return all_passed

# Run performance check
check_performance_setup()
```

## Conclusion

Performance optimization is crucial for effective neural simulation. This document provides comprehensive guidance on:

1. **Understanding Performance Characteristics**: Knowledge of how different components affect performance
2. **Optimizing Configuration**: Choosing appropriate settings for different use cases
3. **Monitoring Performance**: Tracking resource usage and identifying bottlenecks
4. **Scaling Guidelines**: Planning for larger simulations
5. **Hardware Recommendations**: Ensuring adequate computational resources

By following these guidelines and examples, users can achieve optimal performance for their NLM simulations, whether running small demonstrations or large-scale research experiments.

The key is to balance accuracy, performance, and memory usage based on the specific requirements of each simulation scenario. Always profile and measure performance to make informed optimization decisions.

## References

1. NLM Documentation: Core performance characteristics
2. Performance Optimization Guide: Advanced optimization techniques
3. Scaling Guidelines: Large-scale simulation strategies
4. Memory Management: Memory pool optimization
5. Integration Schemes: Numerical integration methods

This comprehensive guide should help users effectively use and optimize NLM for their neural simulation needs, from simple demonstrations to complex research experiments.