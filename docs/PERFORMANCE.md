# NLM Performance Documentation

## Phase 5 Performance Infrastructure

### Overview

Phase 5 introduces comprehensive performance optimizations enabling NLM to scale from thousands to millions of neurons. The performance infrastructure is designed with the following principles:

1. **Event-driven processing**: Only active neurons consume computation
2. **Memory efficiency**: Pre-allocated pools and SoA layouts reduce overhead
3. **Parallelism**: Multithreading for multi-core CPUs
4. **SIMD vectorization**: Accelerate neural computations
5. **Sparse connectivity**: Avoid O(N²) memory for large networks

### Performance Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Performance Infrastructure                │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐ │
│  │  Event Queues  │  │  Memory Pools  │  │ SIMD Ops   │ │
│  │  Ring buffers  │  │  Pre-allocated │  │ Vectorized │ │
│  │  Time-bucketed │  │  Cache-aligned │  │ LIF update │ │
│  └─────────────────┘  └─────────────────┘  └─────────────┘ │
│                                                              │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐ │
│  │     Sparse      │  │   Parallel      │  │  Benchmark  │ │
│  │  Connectivity   │  │   Processing   │  │  Framework  │ │
│  │  Adjacency list │  │  Thread pools  │  │  Profiling  │ │
│  └─────────────────┘  └─────────────────┘  └─────────────┘ │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

## Components

### 1. Event Queues (EventQueue.hpp)

Lock-free SPSC ring buffer for immediate spikes:
- Capacity: 16,384 spikes (configurable, power of 2)
- Cache-aligned event structures (16 bytes)
- O(1) push/pop operations

Time-bucketed delayed spike queue:
- 1,024 time buckets (configurable)
- Maximum delay: 100 steps
- Efficient batch delivery

### 2. Memory Pools (MemoryPool.hpp)

**NeuronPool**: Pre-allocated neuron storage with SoA layout
- Membrane potentials: Contiguous float array
- Refractory counters: Contiguous uint32 array
- Firing states: Contiguous uint8 array
- Cache line aligned (64 bytes)

**SynapsePool**: Compact synapse storage
- Synapse records: Contiguous struct arrays
- Adjacency lists for sparse connectivity
- Free list for O(1) allocation/deallocation

### 3. Sparse Connectivity (SparseConnectivity.hpp)

Adjacency list representation:
- Outgoing: source neuron → synapse indices
- Incoming: destination neuron → synapse indices
- Synapse records: Contiguous array

Benefits:
- Avoids dense N×N matrix
- Efficient O(k) lookup (k = synapses per neuron)
- Scales to millions of neurons

### 4. Parallel Processing (ParallelProcessing.hpp)

ThreadPool: Lock-free task queue
- Configurable thread count
- Work-stealing support
- Low overhead task submission

ParallelFor: Parallel loop execution
- Static chunk partitioning
- Grain size control
- Reduction operations

ParallelNeuralProcessor: Neural-specific parallelization
- Thread-local state accumulation
- Synchronized aggregation
- Cache-friendly data access

### 5. SIMD Vectorization (SIMDVectorization.hpp)

VectorizedLIF: Batch LIF neuron updates
- AVX-512: 16 neurons per iteration
- AVX2: 8 neurons per iteration
- SSE: 4 neurons per iteration
- Scalar fallback

VectorizedSTDP: Batch plasticity updates
- Vectorized weight calculations
- Reduced memory access

## Benchmarking

### Standard Benchmarks

| Scale | Neurons | Synapses | Target Steps/sec |
|-------|---------|----------|-----------------|
| Micro | 1,000 | ~10,000 | 10,000+ |
| Small | 10,000 | ~100,000 | 5,000+ |
| Medium | 100,000 | ~1,000,000 | 1,000+ |
| Large | 1,000,000 | ~10,000,000 | 100+ |

### Performance Metrics

**Simulation Performance**:
- Neurons per second: Total neurons / wall-clock time
- Spikes per second: Total spikes / wall-clock time
- Real-time factor: simulated_time / wall_clock_time

**Memory Usage**:
- Neuron memory: sizeof(NeuronState) × neuron_count
- Synapse memory: sizeof(SynapseRecord) × synapse_count
- Event queue memory: Queue capacity × event_size

### Measurement Commands

```bash
# Build with performance flags
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..
make -j$(nproc)

# Run benchmark
./build/nlm --benchmark --neurons=10000 --steps=10000

# Run scaling experiment
./build/nlm --scaling-benchmark
```

## Scaling Results

### Phase 5 Target Scales

```
Scale    Neurons    Synapses    Memory    Steps/sec
──────────────────────────────────────────────────
10k      10,000     ~100,000    ~10 MB    5,000+
100k     100,000    ~1,000,000  ~100 MB   1,000+
1M       1,000,000  ~10,000,000 ~1 GB     100+
```

### Scaling Behavior

**Ideal scaling**: Double neurons → Half performance per neuron
**Actual scaling**: Depends on:
- Memory bandwidth saturation
- Cache miss rates
- Synchronization overhead
- Event queue contention

## Performance Optimization Guidelines

### DO

1. **Profile first**: Use perf/valgrind before optimizing
2. **Measure impact**: Compare before/after benchmarks
3. **Maintain correctness**: Verify numerical results
4. **Document tradeoffs**: Note speed vs. precision costs
5. **Keep it modular**: Easy to disable optimizations

### DON'T

1. **Optimize blindly**: Measure first, optimize second
2. **Sacrifice correctness**: Numerical stability matters
3. **Over-engineer**: Simple optimizations first
4. **Ignore limits**: Some code is inherently serial
5. **Assume scaling**: Test at target scale

## Hardware Considerations

### CPU Scaling

- Single core: Baseline performance
- Multi-core: Near-linear up to ~8 cores
- NUMA: Local memory affinity matters
- AVX-512: Significant boost for vectorized ops

### Memory Scaling

- L3 cache: Critical for neural state access
- Memory bandwidth: Limits update throughput
- NUMA topology: Affects multi-socket scaling

### GPU (Future)

Potential GPU targets:
- Large-scale neuron updates
- Synaptic plasticity
- Event processing
- Population statistics

Not GPU candidates:
- Small networks (< 10k neurons)
- Sparse irregular updates
- Complex branching logic

## Performance Optimization Examples

### 1. Configuration Profile Selection

```bash
# Fast prototyping (small brain, quick tests)
./nlm --profile=fast --neuron_count=100 --max_simulation_steps=500

# Detailed simulation (high precision, research)
./nlm --profile=detailed --neuron_count=50000 --visualization=true

# Adaptive learning (balanced, production)
./nlm --profile=adaptive --neuron_count=2000 --development=true

# Use your own config file
./nlm --config=my_research_config.cfg
```

### 2. Performance Benchmarking

```bash
# Standard benchmark command
./nlm --benchmark --neurons=10000 --steps=10000

# Scaling benchmark for different configurations
./nlm --scaling-benchmark --profile=fast --profile=detailed --profile=adaptive

# Quick test configuration
./nlm --config=test_config.cfg --benchmark
```

### 3. Python Performance Examples

```python
import pynlm
import time

# Profile-based configuration
config = pynlm.createAdvancedConfig("fast")
brain = pynlm.createBrain(config)
brain.initialize()

# Performance metrics analysis
metrics = pynlm.analyzeBrainMetrics(brain)
print(f"Performance metrics: {metrics}")

# Quick test configuration
quick_config = pynlm.createTestConfig()
brain = pynlm.createBrain(quick_config)
brain.initialize()
```

### 4. Memory Optimization Examples

```cpp
// Use memory pools for large-scale simulations
// Configure in config file:
simulation.use_pools = true
brain.enable_memory_pools = true
world.use_memory_pools = true

// SIMD configuration
performance.simd_level = "AVX512"
performance.vectorization = "full"
```

### 5. Parallel Processing Examples

```bash
# Configure for multi-core systems
./nlm --threads=8 --neuron_count=100000

# Enable parallel processing
./nlm --parallel=true --simd=avx2

# Performance monitoring
./nlm --benchmark --monitor --output=perf_results.json
```

### 6. Configuration Validation Examples

```python
import pynlm

# Validate configuration before running
config = pynlm.createAdvancedConfig("detailed")

# Check for common issues
if not config.has("brain.neuron_count"):
    print("Warning: neuron_count not set")
    
if config.getOr("simulation_timestep", 0.001) > 0.01:
    print("Warning: large timestep may affect accuracy")

# Validate with custom rules
errors = config.validate()
if errors:
    print("Configuration errors:", errors)
```

### 7. Error Handling for Performance

```python
def run_performance_test(config_profile="fast", num_steps=1000):
    """Run a performance test with comprehensive error handling."""
    try:
        # Create brain with validation
        config = pynlm.createAdvancedConfig(config_profile)
        
        # Validate configuration
        if not config.validateConfig():
            errors = config.getValidationErrors()
            raise ValueError(f"Invalid configuration: {errors}")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Start performance monitoring
        start_time = time.time()
        start_spikes = brain.getTotalSpikeCount()
        
        # Run simulation with error handling at each step
        for step in range(num_steps):
            try:
                brain.step(step)
                
                # Log performance every 100 steps
                if step % 100 == 0:
                    elapsed = time.time() - start_time
                    spikes = brain.getTotalSpikeCount() - start_spikes
                    print(f"Step {step}: {spikes} spikes, {elapsed:.2f}s elapsed")
                    
            except Exception as e:
                print(f"Error at step {step}: {e}")
                continue
        
        # Final performance metrics
        elapsed = time.time() - start_time
        metrics = pynlm.analyzeBrainMetrics(brain)
        
        return {
            "success": True,
            "total_time": elapsed,
            "steps_per_second": num_steps / elapsed,
            "spikes": brain.getTotalSpikeCount() - start_spikes,
            "metrics": metrics
        }
        
    except Exception as e:
        print(f"Performance test failed: {e}")
        return {
            "success": False,
            "error": str(e)
        }

# Run performance tests with different profiles
results_fast = run_performance_test("fast", 1000)
results_detailed = run_performance_test("detailed", 1000)
results_adaptive = run_performance_test("adaptive", 1000)
```

### 8. Production Deployment Examples

```bash
# Production configuration for stable operation
./nlm \
  --config=production.cfg \
  --threads=4 \
  --log_level=INFO \
  --enable_monitoring=true \
  --performance_profile=balanced

# High-performance research configuration
./nlm \
  --profile=detailed \
  --threads=16 \
  --simd=avx512 \
  --neuron_count=500000 \
  --max_simulation_steps=100000 \
  --enable_profiling=true
```

### 9. Best Practices for Performance

```python
# Always validate configuration
config = pynlm.createAdvancedConfig("production")
assert config.validateConfig(), "Configuration validation failed"

# Use performance profiles appropriately
profiles = {
    "fast": "Prototyping and quick tests",
    "detailed": "Research and high precision",
    "adaptive": "Production and simulation"
}

# Monitor resource usage
import psutil
process = psutil.Process()
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")
print(f"CPU usage: {process.cpu_percent()}%")

# Proper cleanup
def cleanup_brain(brain):
    """Clean up brain resources properly."""
    try:
        brain.reset()
        brain.save("cleanup_checkpoint.bin")
    except Exception as e:
        print(f"Cleanup warning: {e}")
```

## Quick Reference

| Task | Command | Description |
|------|---------|-------------|
| Quick test | `./nlm --profile=fast --neuron_count=100` | Small brain for testing |
| Large simulation | `./nlm --profile=detailed --neuron_count=100000` | Research-scale simulation |
| Production run | `./nlm --profile=adaptive --threads=8` | Balanced production config |
| Performance analysis | `./nlm --benchmark --output=perf.json` | Generate performance metrics |
| Configuration validation | `./nlm --validate --config=my_config.cfg` | Validate configuration |
| Help | `./nlm --help` | Show all options |