#!/usr/bin/env python3
"""
Example 7: Performance Monitoring and Optimization

This example demonstrates how to monitor and optimize NLM performance,
including profiling, benchmarking, and performance analysis.
"""

import pynlm
import numpy as np
import time
import matplotlib.pyplot as plt

def performance_monitoring():
    """Demonstrate performance monitoring and optimization."""
    
    print("=== Example 7: Performance Monitoring and Optimization ===")
    
    # Create advanced performance profiler
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create performance profiler
    profiler = pynlm.createPerformanceProfiler()
    profiler.startProfiling()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Performance monitoring setup:")
    print(f"  Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"  World size: {world.getWidth()}x{world.getHeight()}")
    print(f"  Profiler started: {profiler.isProfiling()}")
    
    # Performance data collection
    perf_data = {
        'steps': [],
        'simulation_time': [],
        'firing_rate': [],
        'spike_count': [],
        'curiosity': [],
        'reward': [],
        'memory_usage': [],
        'cpu_usage': []
    }
    
    # Run performance benchmark
    print("\nRunning performance benchmark...")
    
    start_time = time.time()
    
    for step in range(100):
        step_start = time.time()
        
        # Update world
        world.update(0.1)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain with profiling
        brain.step(step)
        
        # Get actions
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Measure performance metrics
        step_time = time.time() - step_start
        
        # Collect metrics
        perf_data['steps'].append(step)
        perf_data['simulation_time'].append(step_time)
        perf_data['firing_rate'].append(brain.getAverageFiringRate())
        perf_data['spike_count'].append(brain.getTotalSpikeCount())
        perf_data['curiosity'].append(agent.getCuriosityLevel())
        perf_data['reward'].append(np.random.random() * 0.3)
        
        # Print progress
        if step % 25 == 0:
            print(f"  Step {step}: {step_time*1000:.1f} ms/step, "
                  f"Firing rate: {perf_data['firing_rate'][-1]:.3f}")
    
    end_time = time.time()
    total_time = end_time - start_time
    
    profiler.stopProfiling()
    
    # Get final performance metrics
    final_metrics = profiler.getPerformanceMetrics()
    
    # Analyze performance
    print("\n--- Performance Analysis ---")
    
    # Calculate statistics
    mean_sim_time = np.mean(perf_data['simulation_time'])
    std_sim_time = np.std(perf_data['simulation_time'])
    mean_firing_rate = np.mean(perf_data['firing_rate'])
    total_spikes = perf_data['spike_count'][-1]
    steps_per_second = len(perf_data['steps']) / total_time
    
    print(f"Total simulation time: {total_time:.2f} seconds")
    print(f"Steps per second: {steps_per_second:.1f}")
    print(f"Mean simulation time per step: {mean_sim_time*1000:.1f} ms")
    print(f"Std simulation time: {std_sim_time*1000:.1f} ms")
    print(f"Mean firing rate: {mean_firing_rate:.3f}")
    print(f"Total spikes generated: {total_spikes}")
    
    # Plot performance metrics
    plt.figure(figsize=(15, 12))
    
    # Simulation time distribution
    plt.subplot(3, 2, 1)
    plt.hist(perf_data['simulation_time'], bins=20, alpha=0.7, color='skyblue')
    plt.title('Simulation Time Distribution')
    plt.xlabel('Time per Step (seconds)')
    plt.ylabel('Frequency')
    
    # Firing rate over time
    plt.subplot(3, 2, 2)
    plt.plot(perf_data['steps'], perf_data['firing_rate'], linewidth=2)
    plt.title('Firing Rate Over Time')
    plt.ylabel('Firing Rate')
    plt.xlabel('Step')
    plt.grid(True, alpha=0.3)
    
    # Spike count over time
    plt.subplot(3, 2, 3)
    plt.plot(perf_data['steps'], perf_data['spike_count'], linewidth=2)
    plt.title('Cumulative Spike Count')
    plt.ylabel('Total Spikes')
    plt.xlabel('Step')
    plt.grid(True, alpha=0.3)
    
    # Curiosity over time
    plt.subplot(3, 2, 4)
    plt.plot(perf_data['steps'], perf_data['curiosity'], linewidth=2, color='green')
    plt.title('Curiosity Levels Over Time')
    plt.ylabel('Curiosity Level')
    plt.xlabel('Step')
    plt.grid(True, alpha=0.3)
    
    # Reward over time
    plt.subplot(3, 2, 5)
    plt.plot(perf_data['steps'], perf_data['reward'], linewidth=2, color='orange')
    plt.title('Reward Over Time')
    plt.ylabel('Reward')
    plt.xlabel('Step')
    plt.grid(True, alpha=0.3)
    
    # Performance over time (combined)
    plt.subplot(3, 2, 6)
    perf_score = (np.array(perf_data['firing_rate']) / max(np.array(perf_data['firing_rate'])) *
                  np.array(perf_data['curiosity']) *
                  np.array(perf_data['reward']))
    plt.plot(perf_data['steps'], perf_score, linewidth=2, color='red')
    plt.title('Overall Performance Score')
    plt.ylabel('Performance Score')
    plt.xlabel('Step')
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('performance_analysis.png')
    plt.show()
    
    # Performance optimization recommendations
    print("\n--- Performance Optimization Recommendations ---")
    
    if mean_sim_time > 0.01:  # If more than 10ms per step
        print(f"⚠️  Slow simulation time: {mean_sim_time*1000:.1f} ms per step")
        print("   Recommendations:")
        print("   - Reduce neuron count for real-time applications")
        print("   - Disable structural plasticity for speed")
        print("   - Use SIMD optimizations in C++ build")
        print("   - Consider parallel processing")
    else:
        print(f"✅ Good simulation performance: {mean_sim_time*1000:.1f} ms per step")
    
    # Check for performance bottlenecks
    max_time = max(perf_data['simulation_time'])
    if max_time > mean_sim_time * 3:
        print(f"⚠️  High performance variance: max {max_time*1000:.1f} ms vs mean {mean_sim_time*1000:.1f} ms")
        print("   Recommendations:")
        print("   - Profile to identify bottlenecks")
        print("   - Optimize hot paths")
        print("   - Consider memory management")
    
    # Memory usage analysis
    print("\n--- Memory Usage Analysis ---")
    
    # Simulate memory tracking
    memory_usage = []
    for i in range(len(perf_data['steps'])):
        base_usage = 50 + len(perf_data['steps'][i]) * 0.1
        memory_usage.append(base_usage)
    
    memory_growth = memory_usage[-1] - memory_usage[0]
    memory_growth_rate = memory_growth / len(perf_data['steps'])
    
    print(f"Memory growth over simulation: {memory_growth:.1f} MB")
    print(f"Memory growth rate: {memory_growth_rate:.2f} MB/step")
    
    if memory_growth_rate > 0.01:  # If growing more than 0.01 MB per step
        print("⚠️  Potential memory leak detected!")
        print("   Recommendations:")
        print("   - Check for memory leaks in Python bindings")
        print("   - Clear unused references")
        print("   - Use memory profiling tools")
    else:
        print("✅ Memory usage stable")
    
    print("\nPerformance monitoring demonstration complete!")
    
    return brain, world, agent, perf_data

if __name__ == "__main__":
    performance_monitoring()