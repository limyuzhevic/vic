# Advanced Example 1: Large-Scale Neural Simulations (1M+ Neurons)

## Overview

This advanced example demonstrates how to create and run large-scale neural simulations with over 1 million neurons using the NLM framework. It showcases the performance optimizations and architectural features that enable NLM to scale to biologically realistic brain sizes.

## Learning Objectives

After completing this example, you will be able to:

1. **Configure large-scale brain architectures** with efficient memory management
2. **Implement performance-critical simulation parameters** for massive networks
3. **Monitor and optimize simulation performance** at scale
4. **Utilize memory-efficient data structures** for large neural populations
5. **Analyze scalability limits and bottlenecks** in large simulations

## What This Example Demonstrates

- **Massive neural populations**: Simulating networks with 1M+ neurons
- **Sparse connectivity**: Efficient representation of large brain connectivity
- **Memory optimization**: Using pre-allocated pools and SoA layouts
- **Performance profiling**: Identifying and optimizing bottlenecks
- **Scalability analysis**: Understanding how performance scales with network size

## System Requirements

For this example, you'll need:

- **Hardware**: High-performance CPU with multiple cores, 8GB+ RAM (more for 1M+ neurons)
- **Build**: NLM compiled with performance optimizations enabled
- **Environment**: Sufficient disk space for configuration and output files
- **Memory**: ~1GB+ RAM for 1M neurons with sparse connectivity

## Technical Architecture

### 1. Memory-Efficient Brain Configuration

```cpp
#include "NLM/Brain.hpp"
#include "NLM/Config.hpp"
#include "NLM/Types.hpp"

void createLargeScaleBrain(pynlm::Brain*& brain, int neuronCount) {
    // Create configuration optimized for large networks
    pynlm::Config config;
    
    // Memory-efficient neuron count (1M+)
    config.set("brain.neuron_count", neuronCount);
    
    // Sparse connectivity to avoid O(N²) memory
    config.set("brain.synapse_density", 0.05);  // Only 5% connectivity
    config.set("brain.connection_probability", 0.02);
    
    // Optimized for large-scale processing
    config.set("performance.enable_vectorization", true);
    config.set("performance.enable_parallel", true);
    config.set("performance.memory_pool_size", "auto");
    
    // Create and initialize brain
    brain = pynlm::createBrain(config);
    brain->initialize();
    
    // Pre-allocate event queues for high spike rates
    brain->configureEventSystem(65536, 1024);
}
```

### 2. Memory Pool Configuration

```python
import pynlm

def configureMemoryPools(neuron_count):
    """Configure memory pools for large-scale simulations"""
    
    config = pynlm.createDefaultConfig()
    
    # Neuron memory pools
    neuron_memory_mb = neuron_count * 64 / (1024 * 1024)  # 64 bytes per neuron
    config.set("performance.memory_pool.neurons", neuron_memory_mb * 1.5)  # 1.5x safety margin
    
    # Synapse memory pools (sparse)
    estimated_synapses = neuron_count * 0.05 * 4  # 5% density * 4 connections/neuron
    synapse_memory_mb = estimated_synapses * 32 / (1024 * 1024)  # 32 bytes per synapse
    config.set("performance.memory_pool.synapses", synapse_memory_mb * 1.2)
    
    # Event queue pools for high-frequency spikes
    config.set("performance.memory_pool.event_queues", 128)  # 128KB per queue
    
    # Cache-friendly data layout
    config.set("performance.data_layout", "structure_of_arrays")
    
    return config

def optimizeForScale(config, neuron_count):
    """Optimize configuration based on target scale"""
    
    if neuron_count >= 1000000:  # 1M+ neurons
        config.set("performance.parallel.threads", 8)
        config.set("performance.vectorization.simd_level", "avx512")
        config.set("performance.event_system.bucket_count", 2048)
        config.set("performance.spike_processing.batch_size", 1024)
        
    elif neuron_count >= 100000:  # 100k-1M neurons
        config.set("performance.parallel.threads", 4)
        config.set("performance.vectorization.simd_level", "avx2")
        config.set("performance.event_system.bucket_count", 1024)
        config.set("performance.spike_processing.batch_size", 512)
        
    else:  # Small scale (<100k neurons)
        config.set("performance.parallel.threads", 2)
        config.set("performance.vectorization.simd_level", "sse")
        config.set("performance.event_system.bucket_count", 256)
        config.set("performance.spike_processing.batch_size", 128)
```

### 3. Simulation Loop for Large Networks

```python
def runLargeScaleSimulation(brain, agent, world, num_steps=10000, checkpoint_interval=1000):
    """Run simulation optimized for large-scale networks"""
    
    print(f"=== Large-Scale Neural Simulation ({brain.getTotalNeuronCount():,} neurons) ===")
    print(f"Target steps: {num_steps:,}")
    print(f"Checkpoint interval: {checkpoint_interval:,} steps")
    
    # Pre-allocate monitoring arrays for performance
    spike_rates = []
    firing_patterns = []
    memory_usage = []
    
    # Track simulation statistics
    start_time = time.time()
    total_spikes = 0
    last_total_spikes = 0
    
    print(f"{'Step':>8} | {'Firing':>10} | {'Spikes/s':>10} | {'Memory':>12} | {'FPS':>8} | {'Status':>10}")
    print(f"{'':>8} | {'':>10} | {'':>10} | {'':>12} | {'':>8} | {'':>10}")
    
    for step in range(num_steps):
        # Update world environment
        world.update(0.001)  # Smaller time steps for accuracy
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step with profiling
        step_start = time.time()
        brain.step(step)
        step_time = time.time() - step_start
        
        # Monitor performance metrics
        current_firing = brain.getFiringNeuronCount()
        current_spikes = brain.getTotalSpikeCount()
        
        # Calculate spikes per second for this step
        spikes_this_step = current_spikes - last_total_spikes
        spikes_per_second = spikes_this_step / 0.001  # Convert to Hz
        last_total_spikes = current_spikes
        total_spikes += spikes_this_step
        
        # Memory usage monitoring
        mem_usage = getMemoryUsage()  # Custom function
        memory_usage.append(mem_usage)
        
        # Frame rate calculation
        fps = 1.0 / step_time if step_time > 0 else 0
        
        # Print progress every 1000 steps
        if step % checkpoint_interval == 0 and step > 0:
            avg_fps = (step / (time.time() - start_time))
            
            print(f"{step:>8,} | {current_firing:>10,} | {spikes_per_second:>10,.0f} | "
                  f"{mem_usage:>12,.0f} MB | {fps:>8,.1f} | "
                  f"FPS(avg):{avg_fps:>7,.1f}")
            
            # Save checkpoint if interval reached
            if step % checkpoint_interval == 0:
                checkpoint_file = f"large_scale_checkpoint_{step:08d}.bin"
                brain.save(checkpoint_file)
                print(f"   ✓ Checkpoint saved: {checkpoint_file}")
        
        # Adjust learning rates based on scale
        if step % 500 == 0 and step > 0:
            adjustLearningRatesForScale(brain, step, neuron_count)
    
    # Final statistics
    end_time = time.time()
    total_time = end_time - start_time
    actual_fps = num_steps / total_time
    
    print(f"\n=== Large-Scale Simulation Complete ===")
    print(f"Total simulation time: {total_time:.1f} seconds")
    print(f"Actual FPS: {actual_fps:.1f}")
    print(f"Total spikes: {total_spikes:,}")
    print(f"Average spikes per second: {total_spikes/total_time:.0f}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Peak memory usage: {max(memory_usage):.1f} MB")
    print(f"Scalability factor: {num_steps/(total_time*1000):.0f} steps/second")
    
    return {
        'total_time': total_time,
        'actual_fps': actual_fps,
        'total_spikes': total_spikes,
        'final_firing_rate': brain.getAverageFiringRate(),
        'peak_memory': max(memory_usage),
        'scalability': num_steps/(total_time*1000)
    }

def getMemoryUsage():
    """Get current memory usage in MB"""
    import psutil
    import os
    process = psutil.Process(os.getpid())
    return process.memory_info().rss / (1024 * 1024)

def adjustLearningRatesForScale(brain, step, neuron_count):
    """Adjust learning parameters based on simulation scale"""
    
    # Scale learning rates down for larger networks to prevent instability
    if neuron_count >= 1000000:
        # More conservative learning for very large networks
        stdp_rate = 0.0001 * (1.0 - step/10000)  # Gradually decrease
        hebbian_rate = 0.0002 * (1.0 - step/10000)
    elif neuron_count >= 100000:
        # Moderate learning for large networks
        stdp_rate = 0.0005 * (1.0 - step/5000)
        hebbian_rate = 0.001 * (1.0 - step/5000)
    else:
        # Normal learning for smaller networks
        stdp_rate = 0.001
        hebbian_rate = 0.002
    
    # This would require access to brain configuration
    # In practice, you'd use the agent interface to modify learning rates
    pass
```

### 4. Performance Monitoring and Analysis

```python
import matplotlib.pyplot as plt
import numpy as np

def analyzeLargeScalePerformance(metrics, neuron_count):
    """Analyze performance characteristics of large-scale simulation"""
    
    print(f"\n=== Large-Scale Performance Analysis ===")
    print(f"Network size: {neuron_count:,} neurons")
    
    # Performance metrics analysis
    print(f"\nPerformance Metrics:")
    print(f"   Total simulation time: {metrics['total_time']:.1f} seconds")
    print(f"   Actual FPS: {metrics['actual_fps']:.1f}")
    print(f"   Total spikes generated: {metrics['total_spikes']:,}")
    print(f"   Average firing rate: {metrics['final_firing_rate']:.2f} Hz")
    
    # Scalability analysis
    neurons_per_second = metrics['total_spikes'] / metrics['total_time']
    efficiency = metrics['total_spikes'] / (metrics['peak_memory'] * 1024 * 1024)
    
    print(f"\nScalability Analysis:")
    print(f"   Processing rate: {neurons_per_second:,.0f} neurons/second")
    print(f"   Memory efficiency: {efficiency:,.0f} spikes/byte")
    print(f"   Theoretical max (1M neurons @ 100Hz): 100,000,000 spikes/sec")
    
    # Performance bottleneck identification
    bottlenecks = identifyPerformanceBottlenecks(metrics)
    
    return bottlenecks

def identifyPerformanceBottlenecks(metrics):
    """Identify performance bottlenecks in large-scale simulation"""
    
    bottlenecks = []
    
    # Check memory bandwidth bottleneck
    if metrics['actual_fps'] < 50:
        bottlenecks.append({
            'type': 'Memory Bandwidth',
            'severity': 'High',
            'impact': 'Simulation running below optimal performance',
            'solution': 'Increase memory pool sizes, optimize data layout'
        })
    
    # Check spike processing bottleneck
    if metrics['actual_fps'] < 20:
        bottlenecks.append({
            'type': 'Spike Processing',
            'severity': 'Critical',
            'impact': 'Spike delivery is limiting performance',
            'solution': 'Increase event queue capacity, optimize spike delivery'
        })
    
    # Check thread synchronization bottleneck
    if metrics['actual_fps'] < 10:
        bottlenecks.append({
            'type': 'Parallel Processing',
            'severity': 'High',
            'impact': 'Thread synchronization limiting scalability',
            'solution': 'Reduce lock contention, improve work distribution'
        })
    
    return bottlenecks

def generateLargeScaleVisualization(metrics, output_file='large_scale_analysis.png'):
    """Generate visualization of large-scale simulation results"""
    
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle(f'Large-Scale Neural Simulation Analysis ({metrics["neuron_count"]:,} neurons)', fontsize=16)
    
    # Performance over time
    steps = np.arange(0, len(metrics['spike_history']), 100)
    ax1.plot(steps, metrics['spike_history'][::100], 'b-', alpha=0.7, linewidth=1)
    ax1.set_xlabel('Simulation Steps')
    ax1.set_ylabel('Cumulative Spikes')
    ax1.set_title('Spike Accumulation Over Time')
    ax1.grid(True, alpha=0.3)
    
    # Memory usage over time
    ax2.plot(steps, metrics['memory_usage'][::100], 'r-', alpha=0.7, linewidth=1)
    ax2.set_xlabel('Simulation Steps')
    ax2.set_ylabel('Memory Usage (MB)')
    ax2.set_title('Memory Usage Over Time')
    ax2.grid(True, alpha=0.3)
    
    # Performance metrics comparison
    performance_metrics = ['FPS', 'Spikes/sec', 'Efficiency']
    values = [metrics['actual_fps'], metrics['spikes_per_second'], metrics['efficiency']]
    
    colors = ['green' if v > 0.5 * max(values) else 'orange' if v > 0.2 * max(values) else 'red' for v in values]
    bars = ax3.bar(performance_metrics, values, color=colors, alpha=0.7)
    ax3.set_ylabel('Value')
    ax3.set_title('Performance Metrics Summary')
    ax3.set_ylim(0, max(values) * 1.2)
    
    # Add value labels on bars
    for bar, value in zip(bars, values):
        height = bar.get_height()
        ax3.text(bar.get_x() + bar.get_width()/2., height,
                f'{value:.1f}', ha='center', va='bottom')
    
    # Scalability analysis
    x_vals = [100000, 500000, 1000000]
    fps_vals = [metrics['actual_fps'] * (x / metrics['neuron_count'])**0.8 for x in x_vals]  # Approximate scaling
    
    ax4.plot(x_vals, fps_vals, 'bo-', linewidth=2, markersize=8)
    ax4.set_xlabel('Number of Neurons')
    ax4.set_ylabel('Frames Per Second')
    ax4.set_title('Estimated Performance Scaling')
    ax4.set_xscale('log')
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig(output_file, dpi=150, bbox_inches='tight')
    plt.show()
    
    print(f"\nVisualization saved to: {output_file}")
```

### 5. Complete Example Integration

```python
def main():
    """Complete large-scale simulation example"""
    
    print("=== NLM Large-Scale Neural Simulation Example ===")
    print("This example demonstrates simulation of neural networks with 1M+ neurons\n")
    
    # Configure for different scales
    scales = [
        (100000, "Small Scale"),
        (500000, "Medium Scale"),
        (1000000, "Large Scale"),
    ]
    
    all_results = {}
    
    for neuron_count, scale_name in scales:
        print(f"\n{'='*60}")
        print(f"Running {scale_name}: {neuron_count:,} neurons")
        print(f"{'='*60}")
        
        # Create configuration
        config = configureMemoryPools(neuron_count)
        
        # Create world for simulation
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, visionWidth=20, visionHeight=20)
        
        # Create brain with optimized configuration
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning for realistic simulation
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation
        metrics = runLargeScaleSimulation(
            brain, agent, world, 
            num_steps=5000 if neuron_count <= 500000 else 2000,
            checkpoint_interval=1000
        )
        metrics['neuron_count'] = neuron_count
        metrics['scale_name'] = scale_name
        metrics['spike_history'] = []
        metrics['memory_usage'] = []
        
        all_results[scale_name] = metrics
        
        # Save brain state
        brain.save(f"large_scale_brain_{scale_name}.bin")
        print(f"✓ Brain state saved: large_scale_brain_{scale_name}.bin")
    
    # Analyze results
    print(f"\n{'='*60}")
    print("COMPREHENSIVE RESULTS ANALYSIS")
    print(f"{'='*60}")
    
    for scale_name, metrics in all_results.items():
        bottlenecks = analyzeLargeScalePerformance(metrics)
        
        print(f"\n{scale_name} ({metrics['neuron_count']:,} neurons):")
        print(f"   Performance: {metrics['actual_fps']:.1f} FPS")
        print(f"   Spike rate: {metrics['spikes_per_second']:,.0f} spikes/second")
        print(f"   Memory efficiency: {metrics['efficiency']:,.0f} spikes/byte")
        
        if bottlenecks:
            print(f"   ⚠️  Bottlenecks identified:")
            for bottleneck in bottlenecks:
                print(f"      - {bottleneck['type']}: {bottleneck['severity']} severity")
        else:
            print(f"   ✓ No significant performance bottlenecks")
    
    # Generate comprehensive visualization
    print(f"\nGenerating performance analysis visualization...")
    generateLargeScaleVisualization(all_results['Large Scale'], 'comprehensive_large_scale_analysis.png')
    
    print(f"\n=== Large-Scale Simulation Example Complete ===")
    print(f"All simulations completed successfully!")
    print(f"Key insights:")
    print(f"- Scalability is achievable with proper memory management")
    print(f"- Performance bottlenecks can be systematically identified")
    print(f"- Large networks benefit from optimized configuration")
    print(f"- Memory efficiency is critical for scaling to 1M+ neurons")

if __name__ == "__main__":
    main()
```

## Best Practices for Large-Scale Simulations

1. **Start with smaller networks**: Begin with 100k neurons to validate configuration
2. **Monitor memory usage**: Use memory profiling tools to identify leaks
3. **Optimize connectivity**: Use sparse connectivity representations
4. **Parallelize appropriately**: Match thread count to hardware capabilities
5. **Use checkpointing**: Save intermediate states for long simulations
6. **Profile before optimizing**: Use profiling tools to identify bottlenecks
7. **Scale gradually**: Increase network size incrementally

## Extensions and Variations

1. **Different connectivity patterns**: Regular grids, small-world, scale-free networks
2. **Heterogeneous neuron populations**: Mix of different neuron types
3. **Dynamic network size**: Growth and pruning during simulation
4. **Multi-region architectures**: Specialized functional regions
5. **Distributed computing**: Multi-machine scaling

## Common Pitfalls and Solutions

| Pitfall | Solution |
|---------|----------|
| Memory overflow | Use memory pools and optimize data layout |
| Slow initialization | Pre-allocate all memory upfront |
| Poor scaling | Profile and optimize bottleneck areas |
| Numerical instability | Use smaller time steps, adaptive learning rates |
| Low throughput | Increase parallelism, optimize cache usage |

## Further Reading

- [Performance Documentation](./docs/PERFORMANCE.md)
- [Architecture Overview](./docs/ARCHITECTURE.md)
- [Phase 6 Integration](./docs/PHASE6_ARCHITECTURE.md)
- [Scaling Guide](./docs/SCALING.md)

This example provides a foundation for creating and running large-scale neural simulations that can handle biologically realistic network sizes while maintaining good performance and scalability.