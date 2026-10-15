# NLM - Comprehensive Python API Guide

## Overview of Enhanced Python API

This is the enhanced Python API for the NLM (Neural Learning Machine) neural simulation framework. This guide covers both the original API and the new enhancements.

## Quick Start (Copy & Paste)

### Example 1: Basic Enhanced Brain Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think for 100 steps
for i in range(100):
    brain.step(i)

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Complete Agent with Enhanced Features

```python
import pynlm

# 1. Setup with enhanced configuration
config = pynlm.createDevelopmentConfig()  # Enhanced preset
brain = pynlm.createBrain(config)
brain.initialize()

# 2. Create agent with enhanced capabilities
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# 3. Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# 4. Run simulation with enhanced statistics
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Enhanced simulation complete!")
```

### Example 3: Using Context Manager for Brain

```python
import pynlm

# Create brain with context manager for automatic reset
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    
    # Brain is automatically reset when exiting context
    for i in range(50):
        brain.step(i)
    
    print("Brain automatically reset!")
```

## API Reference

### Core Classes

#### Brain Class
```python
# Creation
brain = pynlm.createBrain(config)

# Context manager support
with brain:
    brain.initialize()
    # Your simulation code
    # Brain automatically resets on exit

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
stats = pynlm.getBrainStats(brain)      # Get comprehensive stats
print(stats.toString())                 # Pretty print stats

# Serialization
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state
brain.saveToFile("state.json")     # Save as JSON
brain.loadFromFile("state.json")   # Load from JSON

# Conversion
brain_dict = brain.toDict()        # Convert to dictionary
```

#### Config Class
```python
# Creation with presets
config = pynlm.createDefaultConfig()       # Simple setup
config = pynlm.createDevelopmentConfig()   # Enhanced
config = pynlm.createMinimalConfig()       # Lightweight

# JSON support
json_config = "{\"brain.neuronCount\": 1000}"
config = pynlm.loadConfigFromJSON(json_config)

# Enhanced getters with defaults
neuron_count = config.getInt("brain.neuronCount", 1000)
learning_rate = config.getDouble("brain.learningRate", 0.1)
enabled = config.getBool("brain.plasticityEnabled", True)

# Enhanced setters
config.setInt("brain.neuronCount", 1000)
config.setDouble("brain.learningRate", 0.1)
config.setBool("brain.plasticityEnabled", True)

# Validation
if config.validate():
    print("Configuration is valid!")
```

#### AgentBrain Class
```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Enhanced initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Enhanced neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Enhanced development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
stats = {
    'curiosity_level': agent.getCuriosityLevel(),
    'novelty_level': agent.getNoveltyLevel(),
    'neuromodulation_level': agent.getNeuromodulationLevel(),
    'prediction_error': agent.getPredictionError()
}
```

#### SimpleWorld Class
```python
# Creation
world = pynlm.createSimpleWorld()

# Enhanced configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
world.addObject(obj)

# Properties
properties = {
    'width': world.getWidth(),
    'height': world.getHeight(),
    'simulation_time': world.getSimulationTime(),
    'max_energy': world.getMaxEnergy()
}
```

### New Convenience Functions

#### Configuration Presets
```python
# Different configuration presets
config1 = pynlm.createDefaultConfig()       # 1000 neurons, basic setup
config2 = pynlm.createDevelopmentConfig()   # 2000 neurons, full features
config3 = pynlm.createMinimalConfig()       # 500 neurons, lightweight
```

#### Run Complete Episode
```python
# Run a complete simulation episode
def runEpisode(brain, world, agent, max_steps=1000):
    return pynlm.runCompleteEpisode(brain, world, agent, max_steps)

result = runEpisode(brain, world, agent, 500)
```

#### Statistical Aggregation
```python
# Aggregate statistics from multiple brains
brains = [brain1, brain2, brain3]
aggregated_stats = pynlm.aggregateBrainStats(brains)
print(aggregated_stats)
# Output: {'neuronCount': 1500.0, 'synapseCount': 3000.0, ...}
```

#### Visualization Utilities
```python
# Generate neural network layout visualization
layout = pynlm.generateNeuralNetworkLayout(brain)
print(layout)

# Generate brain activity heatmap
heatmap = pynlm.generateBrainActivityHeatmap(brain)

# Generate development timeline
tl = pynlm.generateDevelopmentTimeline(brain)
print(tl)
```

#### Performance Profiling
```python
# Profile simulation performance
profiler = pynlm.createPerformanceProfiler()

# Your simulation code
for step in range(100):
    brain.step(step)
    profiler.recordStepTime()

print(profiler.getPerformanceReport())
```

#### Experiment Setup
```python
# Setup experiment with metadata
experiment = pynlm.createExperimentSetup("neural_learning_test")
experiment.setExperimentName("Test Experiment")
experiment.setSeed(42)

# Setup environment
pynlm.setupExperimentEnvironment(experiment, brain, world, agent)

# Record metrics
experiment.recordMetric("success_rate", 0.85)
experiment.recordMetric("average_firing_rate", 12.5)
```

### Error Handling

The enhanced API provides Pythonic exception handling:

```python
import pynlm

try:
    # This will raise PyRuntimeError
    brain = pynlm.createBrain(None)
except pynlm.PyRuntimeError as e:
    print(f"Runtime error: {e}")

try:
    # This will raise PyValueError
    config = pynlm.loadConfigFromJSON("")
except pynlm.PyValueError as e:
    print(f"Value error: {e}")

try:
    # This will raise PyKeyError
    value = config.getInt("missing.key")
except pynlm.PyKeyError as e:
    print(f"Key error: {e}")
```

### Advanced Usage Patterns

#### Pattern 1: Batch Processing
```python
import pynlm

def processMultipleBrains(brain_configs, steps):
    brains = []
    for config in brain_configs:
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
    
    # Run simulations
    for i, brain in enumerate(brains):
        for step in range(steps):
            brain.step(step)
        
        # Get stats
        stats = pynlm.getBrainStats(brain)
        print(f"Brain {i+1}: {stats.summary()}")
    
    return brains
```

#### Pattern 2: Multi-threaded Processing
```python
import pynlm
from concurrent.futures import ThreadPoolExecutor

def processSingleBrain(config, steps, worker_id):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return worker_id, brain.getTotalSpikeCount()

# Process multiple brains in parallel
config = pynlm.createDefaultConfig()
futures = []
with ThreadPoolExecutor(max_workers=4) as executor:
    for i in range(4):
        future = executor.submit(processSingleBrain, config, 100, i)
        futures.append(future)
    
    for future in futures:
        worker_id, spike_count = future.result()
        print(f"Worker {worker_id}: {spike_count} spikes")
```

#### Pattern 3: Machine Learning Integration
```python
import pynlm
import numpy as np

def trainNeuralNetwork(num_trials=100, steps_per_trial=100):
    results = []
    
    for trial in range(num_trials):
        # Create new brain for each trial
        config = pynlm.createDevelopmentConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run trial
        for step in range(steps_per_trial):
            brain.step(step)
        
        # Get performance metrics
        stats = pynlm.getBrainStats(brain)
        results.append({
            'trial': trial,
            'spike_count': stats['totalSpikeCount'],
            'firing_rate': stats['averageFiringRate'],
            'neurons_active': stats['firingNeuronCount']
        })
        
        # Save best model
        if trial == num_trials - 1 or results[-1]['spike_count'] > max(r['spike_count'] for r in results):
            brain.save(f"best_model_trial_{trial}.bin")
    
    # Convert to numpy for analysis
    df = pd.DataFrame(results)
    return df
```

## Performance Tips

### 1. Configuration Optimization
```python
# Use appropriate configuration preset for your use case
config = pynlm.createMinimalConfig()  # Faster, less memory
# vs
config = pynlm.createDevelopmentConfig()  # Slower, more features
```

### 2. Memory Management
```python
# Use context managers to ensure cleanup
with pynlm.createBrain(config) as brain:
    brain.initialize()
    # ... simulation code
    # Brain automatically cleaned up
```

### 3. Parallel Processing
```python
# Process multiple brains in parallel using threading
# See Pattern 2 in Advanced Usage
```

### 4. Visualization for Analysis
```python
# Generate comprehensive visualizations
layout = pynlm.generateNeuralNetworkLayout(brain)
heatmap = pynlm.generateBrainActivityHeatmap(brain)
timeline = pynlm.generateDevelopmentTimeline(brain)

# Combine visualizations
visualization = layout + "\n" + heatmap + "\n" + timeline
print(visualization)
```

## Troubleshooting

### Common Issues

#### "My simulation is too slow"
- **Solution**: Use `createMinimalConfig()` for faster performance
- **Solution**: Reduce the number of steps
- **Solution**: Use parallel processing for batch simulations

#### "Memory usage is high"
- **Solution**: Use context managers to ensure cleanup
- **Solution**: Use `createMinimalConfig()` with fewer neurons
- **Solution**: Monitor memory with performance profiler

#### "Configuration errors"
- **Solution**: Use `config.validate()` before running simulation
- **Solution**: Use preset configurations like `createDefaultConfig()`
- **Solution**: Check error messages for specific validation failures

#### "Brain not learning"
- **Solution**: Enable learning subsystems with `agent.enableRewardModulation(True)`
- **Solution**: Ensure proper sensory input is connected
- **Solution**: Check developmental stage progression

### Performance Profiling
```python
# Profile your simulation to identify bottlenecks
profiler = pynlm.createPerformanceProfiler()

# Your simulation code here
for step in range(100):
    brain.step(step)
    profiler.recordStepTime()

# Get detailed performance report
report = profiler.getPerformanceReport()
print(report)
```

## Best Practices

### 1. Always Initialize Before Use
```python
brain = pynlm.createBrain(config)
brain.initialize()  # CRITICAL - never skip this!
```

### 2. Use Context Managers for Resource Management
```python
with pynlm.createBrain(config) as brain:
    brain.initialize()
    # Your simulation
    # Brain automatically cleaned up
```

### 3. Handle Exceptions Properly
```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(0)
except pynlm.PyRuntimeError as e:
    print(f"Simulation failed: {e}")
```

### 4. Validate Configurations
```python
if config.validate():
    # Safe to proceed with simulation
    pass
else:
    print("Configuration invalid - fixing...")
```

### 5. Use Appropriate Configuration Presets
```python
# Choose based on your needs
if debug_mode:
    config = pynlm.createMinimalConfig()  # Fast, small
elif training:
    config = pynlm.createDevelopmentConfig()  # Large, full features
else:
    config = pynlm.createDefaultConfig()  # Balanced
```

## Migration Guide

### From Old API to Enhanced API

#### Old Code
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)
```

#### New Enhanced Code
```python
import pynlm

# Option 1: Use enhanced convenience functions
brain = pynlm.createDefaultBrain()
brain.initialize()

for i in range(100):
    brain.step(i)

# Option 2: Full featured version
config = pynlm.createDevelopmentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)
    
    # Enhanced with statistics
    if i % 10 == 0:
        stats = pynlm.getBrainStats(brain)
        print(f"Step {i}: {stats['firingNeuronCount']} neurons firing")
```

## Next Steps

When you're comfortable with the enhanced API:

1. **Read `HOW_TO_USE.md`** for detailed C++ usage information
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Experiment with different configurations** to find the right balance for your use case
4. **Use the performance profiler** to optimize your simulations
5. **Create your own convenience functions** based on your specific needs

## New Performance-Optimized API Classes

In addition to the core Brain class, NLM now provides comprehensive Python bindings for performance-optimized classes that enable large-scale neural simulations with advanced optimizations:

### 1. OptimizedBrain - High-Performance Neural Simulation

The `OptimizedBrain` class provides significant performance improvements over the standard `Brain` class through several optimization techniques:

```python
import pynlm

# Create optimized brain with performance monitoring
brain = pynlm.createOptimizedBrain(config)
brain.initialize()

# Configure performance optimizations
brain.enableEventDriven(True)      # Only active neurons consume computation
brain.enableMultithreading(True)   # Parallel population updates
brain.enableSIMD(True)             # SIMD vectorization
brain.enableSparseConnectivity(True) # Efficient adjacency lists
brain.setThreadCount(4)            # Configure thread count

# Get comprehensive performance statistics
perf_stats = brain.getPerformanceStats()
print(f"Performance: {perf_stats['neurons_per_second']:.0f} neurons/sec")
print(f"Memory: {perf_stats['memory_usage_mb']:.1f} MB")
print(f"Real-time factor: {perf_stats['real_time_factor']:.2f}x")

# Pythonic properties for easy access
print(f"Event-driven: {brain.event_driven_enabled}")
print(f"Thread count: {brain.thread_count}")
print(f"Memory usage: {brain.memory_usage()} bytes")
```

**Key Features:**
- Event-driven processing for inactive neurons
- Multithreading with work-stealing load balancing
- SIMD vectorization for vector operations
- Sparse connectivity for memory efficiency
- Real-time performance monitoring
- Configurable thread count
- Memory usage reporting

### 2. MemoryPool - Lock-Free Memory Management

The `MemoryPool` classes provide efficient, lock-free memory allocation/deallocation for neural simulation components:

```python
import pynlm

# Create neuron pool for efficient neuron ID management
neuron_pool = nlm.createNeuronPool(block_size=1024, initial_blocks=4)

# Allocate and deallocate neurons efficiently
neuron_id1 = neuron_pool.allocate()
neuron_id2 = neuron_pool.allocate()
neuron_pool.deallocate(neuron_id1)

# Get pool statistics
stats = neuron_pool.stats()
print(f"Allocated: {stats['allocated_count']}")
print(f"Available: {stats['available_count']}")
print(f"Utilization: {stats['utilization_rate']:.1%}")

# Create synapse pool for synaptic connections
synapse_pool = nlm.createSynapsePool()
synapse_id = synapse_pool.allocate(source_neuron, destination_neuron)
stats = synapse_pool.stats()
print(f"Synapse pool size: {stats['size']}")
print(f"Memory usage: {stats['memory_usage_bytes']} bytes")
```

**Key Features:**
- Lock-free allocation/deallocation for thread safety
- Pre-allocated memory blocks for efficiency
- Memory usage monitoring and statistics
- Specialized pools for neurons and synapses
- Reduced memory allocation overhead

### 3. ParallelProcessing - Thread Pool and Algorithms

The `ParallelProcessing` module provides parallel processing infrastructure for large-scale simulations:

```python
import pynlm

# Create thread pool for parallel tasks
thread_pool = nlm.createThreadPool(num_threads=4)

# Submit tasks with futures
future1 = thread_pool.submit(process_data, data1)
future2 = thread_pool.submit(process_data, data2)
result1 = future1.get()
result2 = future2.get()

# Use ParallelFor for parallel loops
nlm.ParallelFor.range(0, 1000000, process_item)

# Use ParallelFor for parallel reductions
max_value = nlm.ParallelFor.reduce(
    0, len(data_array), 
    lambda i: data_array[i],
    nlm.ParallelReducers.max,
    -1
)

# Use ParallelNeuralProcessor for neural computations
processor = nlm.createParallelNeuralProcessor(4)
processor.processNeurons(num_neurons, neural_process_function)

# Get processing statistics
stats = processor.stats()
print(f"Total spikes processed: {stats['total_spike_count']}")
print(f"Total neurons processed: {stats['total_firing_count']}")
print(f"Threads used: {stats['num_threads']}")
```

**Key Features:**
- Thread pool management with work-stealing
- Parallel algorithms for loops, reductions, and scans
- Neural population processing
- Performance monitoring
- Thread identification and management

### 4. Event Queue - High-Performance Event Processing

Event queues provide lock-free, time-bucketed spike processing for event-driven simulations:

```python
import pynlm

# Create spike ring buffer for immediate spike processing
spike_queue = nlm.createSpikeRingBuffer(capacity=16384)

# Process spikes in parallel
spikes_processed = 0
while not spike_queue.empty():
    event = spike_queue.pop()
    process_spike(event)
    spikes_processed += 1

# Create delayed spike queue for synaptic delays
delayed_queue = nlm.createDelayedSpikeQueue()

# Schedule spikes with specific delays
for spike in spikes:
    delayed_queue.schedule(
        source_neuron=spike.source,
        dest_neuron=spike.dest,
        synapse_id=spike.synapse,
        weight=spike.weight,
        is_excitatory=spike.is_excitatory,
        delivery_step=current_step + spike.delay
    )

# Get all spikes due at current step
due_spikes = delayed_queue.getDueSpikes(current_step)
print(f"Processing {len(due_spikes)} due spikes")

# Get queue statistics
stats = delayed_queue.stats()
print(f"Pending spikes: {stats['pending_count']}")
print(f"Queue utilization: {len(due_spikes)/stats['pending_count']:.1%}")
```

**Key Features:**
- Lock-free single-producer single-consumer ring buffers
- Time-bucketed delayed spikes for efficient processing
- Cache-efficient circular buffer implementation
- Overflow handling and bounded capacity

### 5. SIMD Vectorization - Vectorized Neural Operations

SIMD vectorization accelerates vector operations using available SIMD instruction sets:

```python
import pynlm

# Get SIMD information
simd_info = nlm.get_simd_info()
print(f"SIMD Available: {simd_info['available']}")
print(f"SIMD Type: {simd_info['type']}")
print(f"SIMD Width: {simd_info['width']} floats per vector")

# Create and configure VectorizedLIF
lif = nlm.VectorizedLIF()
lif.set_parameters(
    v_rest=-70.0,
    v_threshold=-55.0,
    v_reset=-70.0,
    tau=20.0,
    C=1.0,
    dt=1.0,
    refractory_decay=1.0,
    refractory_period=5.0
)

# Get neuron model parameters
lif_params = lif.stats()
print(f"LIF Parameters: {lif_params}")

# Update neurons in batch using SIMD
V = np.array([...])  # Membrane potentials
I = np.array([...])  # Synaptic inputs
refractory = np.array([...])  # Refractory counters
fired = np.zeros_like(V, dtype=np.uint8)

num_spikes = lif.update(V, I, refractory, fired, len(V))
print(f"Batch update produced {num_spikes} spikes")
```

**Key Features:**
- Automatic detection of best SIMD width (AVX-512, AVX2, SSE, scalar)
- Portable high-level interface with scalar fallback
- Aligned memory operations for performance
- Targets membrane potential updates, synaptic integration, refractory calculations

### 6. Sparse Connectivity - Memory-Efficient Network Representation

Sparse connectivity provides memory-efficient adjacency list storage for large neural networks:

```python
import nlm

# Create sparse connectivity structure
connectivity = nlm.createSparseConnectivity()

# Reserve space for neurons
connectivity.reserveNeurons(100000)  # 100K neurons

# Add synapses efficiently
syn1 = connectivity.addSynapse(
    source_neuron=0,
    dest_neuron=42,
    weight=0.5,
    delay=1,
    synapse_type=pynlm.SYNAPSE_TYPE_EXCITATORY
)

syn2 = connectivity.addSynapse(
    source_neuron=1,
    dest_neuron=43,
    weight=-0.3,
    delay=2,
    synapse_type=pynlm.SYNAPSE_TYPE_INHIBITORY
)

# Get synapses from a source neuron
synapses = connectivity.getSynapsesFrom(0)
print(f"Neuron 0 has {len(synapses)} outgoing connections")

# Get synapses to a destination neuron
incoming = connectivity.getSynapsesTo(42)
print(f"Neuron 42 has {len(incoming)} incoming connections")

# Get connectivity statistics
stats = connectivity.getStats()
print(f"Network Statistics:")
print(f"  Total neurons: {stats['num_neurons']}")
print(f"  Total synapses: {stats['num_synapses']}")
print(f"  Avg synapses per neuron: {stats['avg_synapses_per_neuron']:.2f}")
print(f"  Connectivity probability: {stats['connectivity_probability']:.4f}")
print(f"  Memory usage: {stats['total_synapse_memory'] / (1024*1024):.2f} MB")
```

**Key Features:**
- Adjacency list representation (more memory efficient than dense matrices)
- Efficient synapse lookup by source or destination neuron
- Compact storage for large networks (millions of neurons)
- Cache-friendly iteration patterns
- Index-stability (free connections keep indices)

## Performance Configuration

### SIMD Configuration
```python
# Get current SIMD configuration
simd_config = nlm.get_simd_info()
print(f"SIMD information: {simd_config}")

# Performance is automatic based on CPU capabilities
# No manual configuration needed
```

### Thread Configuration
```python
# Configure thread counts for parallel processing
# Thread counts are globally configurable

# For OptimizedBrain
brain.setThreadCount(8)  # Use 8 threads for parallel processing

# For ParallelNeuralProcessor
processor = nlm.createParallelNeuralProcessor(8)

# For ThreadPool
thread_pool = nlm.createThreadPool(8)
```

### Memory Pool Configuration
```python
# Configure memory pools for optimal performance
neuron_pool = nlm.createNeuronPool(
    block_size=2048,    # Objects per block
    initial_blocks=8    # Pre-allocate 8 blocks
)

synapse_pool = nlm.createSynapsePool()
```

## Performance Monitoring

### Real-time Performance Statistics
All performance classes provide comprehensive real-time statistics:

```python
# OptimizedBrain statistics
brain_perf = brain.getPerformanceStats()
print(f"Optimization statistics:")
print(f"  Neurons per second: {brain_perf['neurons_per_second']:.0f}")
print(f"  Synaptic events per second: {brain_perf['synaptic_events_per_second']:.0f}")
print(f"  Real-time factor: {brain_perf['real_time_factor']:.2f}x")
print(f"  Memory usage: {brain_perf['memory_usage_mb']:.1f} MB")
print(f"  Pending spikes: {brain_perf['pending_spikes']}")
print(f"  Pending delayed spikes: {brain_perf['pending_delayed_spikes']}")

# MemoryPool statistics
pool_stats = neuron_pool.stats()
print(f"Memory pool statistics:")
print(f"  Allocated objects: {pool_stats['allocated_count']}")
print(f"  Available objects: {pool_stats['available_count']}")
print(f"  Memory usage: {pool_stats['memory_usage_bytes'] / 1024:.1f} KB")
print(f"  Utilization rate: {pool_stats['utilization_rate']:.1%}")

# ParallelProcessing statistics
proc_stats = processor.stats()
print(f"Processing statistics:")
print(f"  Total spike count: {proc_stats['total_spike_count']}")
print(f"  Total firing count: {proc_stats['total_firing_count']}")
print(f"  Threads used: {proc_stats['num_threads']}")
```

### Benchmarking Utilities
```python
# Run comprehensive performance benchmark
def benchmarkPerformance(brain, world, agent, steps=1000):
    return nlm.benchmarkPerformance(brain, world, agent, steps)

# Run optimized episode with monitoring
result = nlm.runOptimizedEpisode(
    brain, world, agent, 
    max_steps=500, 
    enable_learning=True
)

# Get performance report
report = nlm.benchmarkPerformance(brain, world, agent, 100)
print(report)
```

## Error Handling

The performance-optimized classes provide specialized error handling:

```python
try:
    # Create optimized brain with invalid configuration
    brain = nlm.createOptimizedBrain(invalid_config)
except nlm.PyValueError as e:
    print(f"Configuration error: {e}")
except nlm.PyRuntimeError as e:
    print(f"Runtime error: {e}")

# Memory pool errors
try:
    pool = nlm.createNeuronPool(-1, 0)  # Invalid parameters
except nlm.PyValueError as e:
    print(f"Pool creation error: {e}")

# Thread pool errors
try:
    thread_pool = nlm.createThreadPool(0)  # Zero threads invalid
except nlm.PyValueError as e:
    print(f"Thread pool error: {e}")
```

## Migration Guide

### From Standard Brain to OptimizedBrain

#### Old Code
```python
brain = nlm.createBrain(config)
brain.initialize()
for i in range(1000):
    brain.step(i)
```

#### New Enhanced Code
```python
# Option 1: Use enhanced convenience function
brain = nlm.createOptimizedBrain(config)
brain.enableEventDriven(True)
brain.enableMultithreading(True)
brain.enableSIMD(True)
brain.initialize()
for i in range(1000):
    brain.step(i)

# Option 2: Full featured version with performance monitoring
brain = nlm.createOptimizedBrain(config)
brain.enableEventDriven(True)
brain.enableMultithreading(True)
brain.enableSIMD(True)
brain.enableSparseConnectivity(True)
brain.setThreadCount(4)
brain.initialize()

for i in range(1000):
    brain.step(i)
    
    # Monitor performance every 100 steps
    if i % 100 == 0:
        stats = brain.getPerformanceStats()
        print(f"Step {i}: {stats['neurons_per_second']:.0f} neurons/sec")
```

### Performance Benefits

The `OptimizedBrain` provides several key performance advantages:

1. **Event-driven processing**: Only active neurons consume computation
2. **Multithreading**: Parallel population updates using work-stealing
3. **SIMD vectorization**: Vectorized operations for better CPU utilization
4. **Sparse connectivity**: Memory-efficient for large networks
5. **Real-time monitoring**: Performance statistics during simulation

## Advanced Usage

### Custom Performance Configuration
```python
def configurePerformance(config, enable_all=True, thread_count=None):
    """Custom performance configuration"""
    brain = nlm.createOptimizedBrain(config)
    
    if enable_all:
        brain.enableEventDriven(True)
        brain.enableMultithreading(True)
        brain.enableSIMD(True)
        brain.enableSparseConnectivity(True)
    
    if thread_count is not None:
        brain.setThreadCount(thread_count)
    
    return brain
```

### Performance Monitoring Wrapper
```python
def monitorPerformance(func, *args, **kwargs):
    """Decorator to monitor function performance"""
    def wrapper(*args, **kwargs):
        profiler = nlm.createPerformanceProfiler()
        profiler.startMeasurement()
        
        result = func(*args, **kwargs)
        
        # Your function call here
        # In practice, you'd measure the actual performance
        
        print(profiler.getPerformanceReport())
        return result
    
    return wrapper
```

### Batch Processing with Parallelization
```python
def processMultipleBrains(configs, steps, parallel=True):
    """Process multiple brains with optional parallelization"""
    if parallel:
        # Use thread pool for parallel processing
        thread_pool = nlm.createThreadPool()
        futures = []
        
        for config in configs:
            future = thread_pool.submit(processSingleBrain, config, steps)
            futures.append(future)
        
        # Collect results
        results = []
        for future in futures:
            results.append(future.get())
        
        return results
    else:
        # Sequential processing
        return [processSingleBrain(config, steps) for config in configs]

def processSingleBrain(config, steps):
    """Process single brain"""
    brain = nlm.createOptimizedBrain(config)
    brain.enableEventDriven(True)
    brain.enableMultithreading(True)
    brain.initialize()
    
    for i in range(steps):
        brain.step(i)
    
    stats = brain.getPerformanceStats()
    return {
        'config': config,
        'spike_count': stats['total_spike_count'],
        'firing_rate': stats['avg_firing_rate'],
        'performance': stats['neurons_per_second']
    }
```

## Performance Considerations

### Memory Usage
```python
# OptimizedBrain memory usage
brain = nlm.createOptimizedBrain(config)
brain.enableSparseConnectivity(True)  # Reduces memory for large networks
brain.enableEventDriven(True)        # Reduces memory for inactive neurons

# MemoryPool memory usage
neuron_pool = nlm.createNeuronPool(block_size=1024)
# Pre-allocated blocks reduce memory fragmentation
```

### CPU Performance
```python
# SIMD configuration (automatic based on CPU)
brain.enableSIMD(True)  # Uses available SIMD instructions

# Threading configuration
brain.enableMultithreading(True)
brain.setThreadCount(4)  # Match your CPU cores

# Event-driven processing
brain.enableEventDriven(True)  # Only process active neurons
```

### Scaling Considerations
```python
# Large networks (millions of neurons)
brain = nlm.createOptimizedBrain(config)
brain.enableSparseConnectivity(True)  # Essential for memory efficiency
brain.enableEventDriven(True)        # Essential for performance
brain.setThreadCount(8)               # Multiple cores

# Small networks (thousands of neurons)
brain = nlm.createOptimizedBrain(config)
brain.enableEventDriven(True)          # Still beneficial
brain.enableSIMD(True)                # Still beneficial
brain.setThreadCount(2)               # Fewer cores needed
```

## Backward Compatibility

All performance-optimized classes are **fully backward compatible** with the existing NLM API:

1. **Existing code continues to work unchanged**
2. **New classes are additive** - can be used alongside existing classes
3. **No breaking changes** to existing method signatures
4. **Performance is optional** - enable optimizations as needed

### Compatibility Checklist

- ✅ `createBrain()` still works (standard Brain)
- ✅ `createOptimizedBrain()` new performance-enhanced version
- ✅ All existing configuration presets supported
- ✅ All existing simulation methods work
- ✅ Performance optimizations are optional
- ✅ Performance classes can be used independently

## Next Steps

1. **Start with standard `Brain`** for simple simulations
2. **Migrate to `OptimizedBrain`** for larger simulations
3. **Use `MemoryPool`** for efficient object management
4. **Implement `ParallelProcessing`** for CPU-intensive tasks
5. **Add event queues** for spike processing optimization
6. **Enable SIMD** for vector operations
7. **Use sparse connectivity** for very large networks

The performance-optimized API provides a powerful, scalable solution for neural simulations while maintaining full backward compatibility with existing code.

That's it! The enhanced Python API now provides comprehensive support for performance-optimized neural simulations with all the tools needed for large-scale, high-performance neural network computations.

That's it! You're now ready to use the enhanced NLM Python API with all its new features and capabilities.
