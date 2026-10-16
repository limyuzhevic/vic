#!/usr/bin/env python3
"""
Advanced NLM Python API Examples

This script demonstrates the advanced features of the NLM Python API,
including batch processing, performance monitoring, and advanced configuration.
"""

import pynlm
import time
import json
from pathlib import Path

def example_advanced_configuration():
    """Example 1: Creating advanced configurations"""
    print("=== Advanced Configuration Examples ===")
    
    # 1. Development-focused configuration
    dev_config = pynlm.createDevelopmentConfig(plasticityModifier=2.0)
    print(f"Development config neurons: {dev_config.getOr('brain.neuron_count', 0)}")
    print(f"Development config plasticity: {dev_config.getOr('plasticity.learning_rate', 0.0)}")
    
    # 2. Performance-optimized configuration  
    perf_config = pynlm.createOptimizedConfig(neuronCount=1500, connectionProb=0.08)
    print(f"Performance config neurons: {perf_config.getOr('brain.neuron_count', 0)}")
    print(f"Performance config Hebbian: {perf_config.getOr('plasticity.hebbian.enable', false)}")
    
    # 3. Quick test configuration
    quick_config = pynlm.createQuickTestConfig()
    print(f"Quick test config neurons: {quick_config.getOr('brain.neuron_count', 0)}")
    
    return dev_config, perf_config, quick_config

def example_batch_processing():
    """Example 2: Batch processing with multiple configurations"""
    print("\n=== Batch Processing Example ===")
    
    # Create multiple configurations
    configs = [
        pynlm.createQuickTestConfig(),
        pynlm.createDevelopmentConfig(),
        pynlm.createOptimizedConfig(neuronCount=800, connectionProb=0.12)
    ]
    
    # Define simulation steps for each config
    steps = [100, 500, 200]
    
    print("Running batch simulation...")
    start_time = time.time()
    
    # Run batch simulation (sequential)
    results = pynlm.runBatchSimulation(configs, steps, parallel=False)
    
    end_time = time.time()
    print(f"Batch simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Results: {len(results)} simulations completed")
    
    for i, result in enumerate(results):
        print(f"  Config {i}: Reward = {result.totalReward:.2f}")
    
    return results

def example_config_comparison():
    """Example 3: Comparing multiple configurations"""
    print("\n=== Configuration Comparison Example ===")
    
    # Create different types of configurations to compare
    configs = [
        pynlm.createQuickTestConfig(),
        pynlm.createDevelopmentConfig(),
        pynlm.createOptimizedConfig(neuronCount=1200, connectionProb=0.15)
    ]
    
    print("Comparing configurations...")
    comparison = pynlm.compareConfigs(configs)
    
    print("Comparison Results:")
    for config_name, metrics in comparison.items():
        print(f"  {config_name}:")
        print(f"    Neurons: {metrics.get('total_neurons', 0)}")
        print(f"    Synapses: {metrics.get('total_synapses', 0)}")
        print(f"    Firing Rate: {metrics.get('firing_rate', 0):.4f}")
        print(f"    Total Spikes: {metrics.get('total_spikes', 0)}")
        print(f"    E/I Ratio: {metrics.get('excitatory_inhibition_ratio', 0):.4f}")
    
    return comparison

def example_advanced_agent_simulation():
    """Example 4: Advanced agent simulation with monitoring"""
    print("\n=== Advanced Agent Simulation Example ===")
    
    # Create brain with development configuration
    config = pynlm.createDevelopmentConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems for advanced behavior
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Running advanced agent simulation...")
    print("Agent enabled: Reward Modulation, Structural Plasticity, Development, Curiosity")
    
    # Run simulation
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print status every 50 steps
        if step % 50 == 0:
            print(f"  Step {step}: Firing neurons: {brain.getFiringNeuronCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    print("Advanced simulation completed!")
    print(f"Final stats:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.4f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

def example_performance_analysis():
    """Example 5: Performance analysis and optimization"""
    print("\n=== Performance Analysis Example ===")
    
    # Create performance monitor
    monitor = pynlm.createPerformanceMonitor()
    
    # Test different configuration sizes
    config_sizes = [50, 100, 200, 500]
    times = []
    memory_usage = []
    
    print("Testing performance with different configuration sizes:")
    
    for size in config_sizes:
        config = pynlm.createOptimizedConfig(neuronCount=size, connectionProb=0.1)
        
        start_time = time.time()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        end_time = time.time()
        
        times.append(end_time - start_time)
        memory_usage.append(brain.getTotalNeuronCount() * sizeof(float))  # Approximate memory
        
        print(f"  Size {size:3d}: Time = {times[-1]:.3f}s, Neurons = {brain.getTotalNeuronCount()}")
    
    print(f"\nPerformance analysis:")
    print(f"  Time scaling: {times[0]:.3f}s -> {times[-1]:.3f}s (factor: {times[-1]/times[0]:.2f})")
    print(f"  Neurons scaling: {config_sizes[0]} -> {config_sizes[-1]} (factor: {config_sizes[-1]/config_sizes[0]:.1f})")
    
    return config_sizes, times, memory_usage

def example_checkpoints_and_recovery():
    """Example 6: Checkpoint management and recovery"""
    print("\n=== Checkpoint Management Example ===")
    
    # Create checkpoint manager
    checkpoint_dir = "./nlm_checkpoints"
    manager = pynlm.checkpointManager(checkpoint_dir, maxCheckpoints=5)
    
    # Create and run simulation
    config = pynlm.createDevelopmentConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Running simulation with checkpointing...")
    
    # Run some steps and save checkpoints
    for step in range(150):
        brain.step(step)
        
        # Save checkpoint every 30 steps
        if step % 30 == 0 and step > 0:
            checkpoint_path = f"{checkpoint_dir}/checkpoint_step_{step}.bin"
            brain.save(checkpoint_path)
            print(f"  Saved checkpoint at step {step}")
    
    # Demonstrate loading checkpoint
    if Path(checkpoint_dir).exists():
        latest_checkpoint = f"{checkpoint_dir}/checkpoint_step_{140}.bin"
        if Path(latest_checkpoint).exists():
            print(f"Loading checkpoint from {latest_checkpoint}")
            new_brain = pynlm.createBrain(config)
            new_brain.initialize()
            new_brain.load(latest_checkpoint)
            print(f"  Loaded brain: {new_brain.getTotalNeuronCount()} neurons, "
                  f"{new_brain.getTotalSynapseCount()} synapses")
    
    return brain

def example_memory_profiling():
    """Example 7: Memory profiling"""
    print("\n=== Memory Profiling Example ===")
    
    # Create memory profiler
    profiler = pynlm.createMemoryProfiler()
    
    print("Profiling memory usage with different configurations:")
    
    configs = [
        pynlm.createQuickTestConfig(),
        pynlm.createDevelopmentConfig(),
        pynlm.createOptimizedConfig(neuronCount=1000)
    ]
    
    for i, config in enumerate(configs):
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        # Get memory stats
        memory_stats = profiler.getMemoryStats(brain)
        
        print(f"  Config {i+1}:")
        print(f"    Total neurons: {memory_stats.get('neurons', 0)}")
        print(f"    Total synapses: {memory_stats.get('synapses', 0)}")
        print(f"    Memory usage: {memory_stats.get('memory_mb', 0):.2f} MB")
        print(f"    Peak memory: {memory_stats.get('peak_memory_mb', 0):.2f} MB")
    
    return profiler

def main():
    """Run all examples"""
    print("NLM Advanced Python API Examples")
    print("=" * 50)
    
    try:
        # Example 1: Advanced configuration
        dev_config, perf_config, quick_config = example_advanced_configuration()
        
        # Example 2: Batch processing
        batch_results = example_batch_processing()
        
        # Example 3: Configuration comparison
        comparison = example_config_comparison()
        
        # Example 4: Advanced agent simulation
        brain, agent, world = example_advanced_agent_simulation()
        
        # Example 5: Performance analysis
        config_sizes, times, memory_usage = example_performance_analysis()
        
        # Example 6: Checkpoints and recovery
        checkpoint_brain = example_checkpoints_and_recovery()
        
        # Example 7: Memory profiling
        profiler = example_memory_profiling()
        
        print("\n" + "=" * 50)
        print("All examples completed successfully!")
        print("\nKey features demonstrated:")
        print("  1. Advanced configuration management")
        print("  2. Batch processing and simulation")
        print("  3. Configuration comparison and analysis")
        print("  4. Advanced agent behavior with full subsystems")
        print("  5. Performance monitoring and optimization")
        print("  6. Checkpoint management and recovery")
        print("  7. Memory profiling and analysis")
        
    except Exception as e:
        print(f"\nError running examples: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
