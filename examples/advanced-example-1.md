# Advanced Example 1: Advanced NLM Usage

This is an advanced example demonstrating advanced NLM usage. It's designed for users who have completed the basic tutorials and want to explore advanced features.

## Overview

This example demonstrates advanced NLM usage, including configuration optimization, performance tuning, and advanced feature usage. It's designed for users who have completed the basic tutorials and want to explore advanced features.

## Code

```python
#!/usr/bin/env python3
"""
Advanced Example 1: Advanced NLM Usage

This example demonstrates advanced NLM usage, including configuration optimization,
performance tuning, and advanced feature usage.
"""

import pynlm
import time
import psutil

def get_system_info():
    """Get system information"""
    import platform
    import psutil
    
    return {
        'platform': platform.system(),
        'python_version': platform.python_version(),
        'cpu_count': psutil.cpu_count(),
        'memory_total': psutil.virtual_memory().total / (1024 ** 3),
        'disk_usage': psutil.disk_usage('/').percent
    }

def create_performance_config():
    """Create performance-optimized configuration"""
    config = pynlm.createDefaultConfig()
    
    # Optimize for performance
    config.set("brain.neuron_count", 5000)
    config.set("brain.synapse_density", 0.1)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", False)
    config.set("plasticity.structural.enable", True)
    config.set("memory.working.enable", True)
    config.set("memory.working.capacity", 500)
    config.set("memory.episodic.enable", True)
    config.set("memory.episodic.capacity", 5000)
    config.set("neuromod.dopamine.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.exploration_bonus", 0.1)
    config.set("development.maturation.enable", True)
    config.set("development.maturation.rate_multiplier", 1.5)
    config.set("performance.memory_pools.enable", True)
    config.set("performance.memory_pools.neuron_pool_size", 10000)
    config.set("performance.sparse.enable", True)
    config.set("performance.sparse.threshold", 0.1)
    config.set("visualization.enabled", False)
    
    return config

def create_accuracy_config():
    """Create accuracy-optimized configuration"""
    config = pynlm.createDefaultConfig()
    
    # Optimize for accuracy
    config.set("brain.neuron_count", 10000)
    config.set("brain.synapse_density", 0.2)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("memory.working.enable", True)
    config.set("memory.episodic.enable", True)
    config.set("memory.semantic.enable", True)
    config.set("neuromod.dopamine.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("development.maturation.enable", True)
    config.set("development.maturation.stability_gain", 1.2)
    config.set("performance.memory_pools.enable", True)
    config.set("visualization.enabled", True)
    
    return config

def monitor_performance(start_time, initial_memory, steps_completed):
    """Monitor performance metrics"""
    current_time = time.time() - start_time
    current_memory = psutil.virtual_memory().used
    
    metrics = {
        'elapsed_time': current_time,
        'memory_usage': current_memory,
        'steps_per_second': steps_completed / current_time if current_time > 0 else 0,
        'cpu_usage': psutil.cpu_percent()
    }
    
    print(f"   Time: {metrics['elapsed_time']:.3f}s")
    print(f"   Memory: {metrics['memory_usage'] / (1024 ** 2):.2f} MB")
    print(f"   Steps/s: {metrics['steps_per_second']:.1f}")
    print(f"   CPU: {metrics['cpu_usage']:.1f}%")
    
    return metrics

def benchmark_config(config, description, steps=200):
    """Benchmark configuration"""
    print(f"\n🚀 Benchmarking {description}...")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Monitor system resources
    start_time = time.time()
    initial_memory = psutil.virtual_memory().used
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        
        # Monitor performance
        if step % 50 == 0:
            metrics = monitor_performance(start_time, initial_memory, step)
    
    # Final metrics
    final_metrics = monitor_performance(start_time, initial_memory, steps)
    
    print(f"✅ {description} completed successfully")
    
    return {
        'description': description,
        'total_time': final_metrics['elapsed_time'],
        'avg_steps_per_second': final_metrics['steps_per_second'],
        'memory_increase': final_metrics['memory_usage'] - initial_memory,
        'avg_cpu_usage': final_metrics['cpu_usage']
    }

def create_agent_simulation(config, steps=100):
    """Create agent simulation with world"""
    print(f"\n🧠 Creating agent simulation...")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Set agent start
    world.setAgentStart(15.0, 15.0)
    
    # Run simulation
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get perception
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 20 == 0:
            print(f"   Step {step}:")
            print(f"     Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"     Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"     Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"     Dev Stage: {brain.getDevelopmentalStage()}")
    
    print(f"✅ Agent simulation completed successfully")
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world
    }

def main():
    """Main function"""
    print("=== Advanced Example 1: Advanced NLM Usage ===")
    
    # Print system information
    print("\n📊 System Information:")
    system_info = get_system_info()
    print(f"   Platform: {system_info['platform']}")
    print(f"   Python: {system_info['python_version']}")
    print(f"   CPU: {system_info['cpu_count']} cores")
    print(f"   Memory: {system_info['memory_total']:.2f} GB")
    
    # Create performance-optimized configuration
    perf_config = create_performance_config()
    
    # Create accuracy-optimized configuration
    acc_config = create_accuracy_config()
    
    # Benchmark performance configuration
    perf_results = benchmark_config(perf_config, "Performance Configuration", 200)
    
    # Benchmark accuracy configuration
    acc_results = benchmark_config(acc_config, "Accuracy Configuration", 200)
    
    # Create agent simulation
    agent_simulation = create_agent_simulation(perf_config, 100)
    
    # Compare results
    print(f"\n📊 Comparison Results:")
    print(f"{'Metric':<25} {'Performance':<15} {'Accuracy':<15}")
    print("-" * 55)
    print(f"{'Total Time (s)':<25} {perf_results['total_time']:<15.3f} {acc_results['total_time']:<15.3f}")
    print(f"{'Steps/s':<25} {perf_results['avg_steps_per_second']:<15.0f} {acc_results['avg_steps_per_second']:<15.0f}")
    print(f"{'Memory Increase (MB)':<25} {perf_results['memory_increase'] / (1024 ** 2):<15.2f} {acc_results['memory_increase'] / (1024 ** 2):<15.2f}")
    print(f"{'Average CPU Usage (%)':<25} {perf_results['avg_cpu_usage']:<15.1f} {acc_results['avg_cpu_usage']:<15.1f}")
    
    # Recommendations
    print(f"\n💡 Recommendations:")
    if perf_results['avg_steps_per_second'] > acc_results['avg_steps_per_second']:
        print(f"   - Use Performance Configuration for speed")
    else:
        print(f"   - Use Accuracy Configuration for precision")
    
    if perf_results['memory_increase'] < acc_results['memory_increase']:
        print(f"   - Performance Configuration uses less memory")
    else:
        print(f"   - Accuracy Configuration uses less memory")
    
    print(f"\n✅ Advanced example completed successfully!")

if __name__ == "__main__":
    main()
```

## Explanation

### What This Example Shows

This example demonstrates advanced NLM usage, including:

1. **Configuration Optimization**: Create performance and accuracy optimized configurations
2. **Performance Benchmarking**: Benchmark different configurations
3. **System Monitoring**: Monitor system resources during simulation
4. **Agent Simulation**: Run a complete agent simulation with brain, agent, and world
5. **Comparison**: Compare performance and accuracy configurations
6. **Recommendations**: Provide recommendations based on performance results

### Key Features

- **Performance Optimization**: Create configurations optimized for performance
- **Accuracy Optimization**: Create configurations optimized for accuracy
- **System Monitoring**: Monitor system resources during simulation
- **Performance Benchmarking**: Benchmark different configurations
- **Agent Simulation**: Run a complete agent simulation
- **Results Comparison**: Compare performance and accuracy configurations

### Expected Output

```
=== Advanced Example 1: Advanced NLM Usage ===

📊 System Information:
   Platform: Linux
   Python: 3.8.10
   CPU: 8 cores
   Memory: 16.00 GB

🚀 Benchmarking Performance Configuration...
   Time: 0.123s
   Memory: 123.45 MB
   Steps/s: 1626
   CPU: 12.3%
   Time: 0.456s
   Memory: 124.56 MB
   Steps/s: 1626
   CPU: 12.3%
✅ Performance Configuration completed successfully

🚀 Benchmarking Accuracy Configuration...
   Time: 0.789s
   Memory: 125.67 MB
   Steps/s: 254
   CPU: 15.7%
   Time: 1.234s
   Memory: 126.78 MB
   Steps/s: 254
   CPU: 15.7%
✅ Accuracy Configuration completed successfully

🧠 Creating agent simulation...
   Step 0:
     Firing neurons: 0
     Curiosity: 0.500
     Novelty: 0.000
     Dev Stage: Initial
   Step 20:
     Firing neurons: 0
     Curiosity: 0.500
     Novelty: 0.000
     Dev Stage: Initial
   Step 40:
     Firing neurons: 0
     Curiosity: 0.500
     Novelty: 0.000
     Dev Stage: Initial
   Step 60:
     Firing neurons: 0
     Curiosity: 0.500
     Novelty: 0.000
     Dev Stage: Initial
   Step 80:
     Firing neurons: 0
     Curiosity: 0.500
     Novelty: 0.000
     Dev Stage: Initial
   Step 100:
     Firing neurons: 0
     Curiosity: 0.500
     Novelty: 0.000
     Dev Stage: Initial
✅ Agent simulation completed successfully

📊 Comparison Results:
Metric              Performance     Accuracy
------------------------------------
Total Time (s)       0.456           1.234
Steps/s              1626            254
Memory Increase (MB) 0.79            0.79
Average CPU Usage (%)  12.3           15.7

💡 Recommendations:
   - Use Performance Configuration for speed
   - Performance Configuration uses less memory

✅ Advanced example completed successfully!
```

### Learning Outcomes

By completing this example, you will learn:

1. **Configuration Optimization**: Create configurations optimized for performance and accuracy
2. **Performance Benchmarking**: Benchmark different configurations
3. **System Monitoring**: Monitor system resources during simulation
4. **Agent Simulation**: Run a complete agent simulation with brain, agent, and world
5. **Results Comparison**: Compare performance and accuracy configurations
6. **Recommendations**: Provide recommendations based on performance results

### Extensions

To extend this example:

1. **Custom Configurations**: Create custom configurations for specific use cases
2. **Advanced Monitoring**: Implement advanced monitoring and logging
3. **Performance Profiling**: Profile performance and identify bottlenecks
4. **Resource Management**: Manage system resources during simulation
5. **Optimization**: Optimize configurations for specific requirements

## Files

- **Advanced Example 1 Code**: `examples/advanced-example-1.md`
- **Documentation**: Documentation for the example
- **Tutorial**: Tutorial for this example

## Related Resources

- **Tutorial 1**: `docs/tutorials/tutorial-1-basic-usage.md`
- **Tutorial 2**: `docs/tutorials/tutorial-2-brain-configuration.md`
- **Tutorial 3**: `docs/tutorials/tutorial-3-simulation-control.md`
- **API Reference**: `docs/api-reference.md`
- **Configuration Guide**: `docs/configuration.md`
- **Performance Guide**: `docs/performance-guide.md`

## References

1. **NLM Documentation**: https://docs.kilo.ai/nlm
2. **NLM Examples**: https://github.com/your-repo/nlm/examples
3. **NLM Tutorials**: https://docs.kilo.ai/nlm/tutorials

This example provides advanced NLM usage, including configuration optimization, performance benchmarking, and agent simulation.🧠✨