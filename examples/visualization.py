"""
Visualization and performance NLM examples.

This example demonstrates:
- Advanced visualization capabilities
- Performance monitoring
- Batch processing
- Memory-efficient simulation
"""

import pynlm
from pynlm import NLMVisualizer, NLMPerformanceMonitor, NLMUtils, BrainSimulator
import matplotlib.pyplot as plt

def performance_monitoring_example():
    """Example demonstrating performance monitoring."""
    print("=== Performance Monitoring Example ===")
    
    # Create simulator with performance monitoring
    monitor = NLMPerformanceMonitor()
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation with monitoring
    monitor.start_monitoring()
    
    for step in range(100):
        brain.step(step)
        metrics = monitor.end_step('simulation_step')
        
        if step % 50 == 0:
            print(f"Step {step}: {metrics['steps_per_second']:.1f} steps/sec")
    
    # Get performance summary
    summary = monitor.get_summary()
    print(f"\nPerformance summary:")
    print(f"  Total steps: {summary['total_steps']}")
    print(f"  Total time: {summary['total_time']:.2f}s")
    print(f"  Average step time: {summary['average_step_time']*1000:.1f}ms")
    print(f"  Steps per second: {summary['steps_per_second']:.1f}")
    
    return brain

def visualization_example():
    """Example demonstrating visualization capabilities."""
    print("\n=== Visualization Example ===")
    
    # Create brain simulator
    simulator = BrainSimulator()
    
    # Run simulation with stats collection
    for step in range(100):
        simulator.step(step)
    
    stats = simulator.get_stats_history()
    
    # Create various visualizations
    fig1, plt1 = NLMVisualizer.plot_brain_activity(stats)
    if fig1:
        plt1.savefig('brain_activity.png', dpi=150, bbox_inches='tight')
        print("Brain activity plot saved to 'brain_activity.png'")
        plt1.close()
    
    fig2, plt2 = NLMVisualizer.plot_performance_metrics(stats)
    if fig2:
        plt2.savefig('performance_metrics.png', dpi=150, bbox_inches='tight')
        print("Performance metrics plot saved to 'performance_metrics.png'")
        plt2.close()
    
    return simulator

def batch_processing_example():
    """Example demonstrating batch processing."""
    print("\n=== Batch Processing Example ===")
    
    # Create multiple brains for batch processing
    brains = []
    configs = []
    
    print("Creating batch of brains...")
    for i in range(5):
        config = NLMUtils.generate_random_config(
            neuron_range=(500, 2000),
            synapse_range=(0.05, 0.3)
        )
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        brains.append(brain)
        configs.append(config)
    
    # Run batch simulation
    batch_results = []
    for i, brain in enumerate(brains):
        print(f"Running brain {i+1}/5...")
        
        for step in range(100):
            brain.step(step)
        
        batch_results.append({
            'brain_id': i,
            'neurons': brain.getTotalNeuronCount(),
            'firing': brain.getFiringNeuronCount(),
            'spikes': brain.getTotalSpikeCount(),
            'rate': brain.getAverageFiringRate()
        })
    
    # Analyze batch results
    print(f"\nBatch processing results:")
    for result in batch_results:
        print(f"  Brain {result['brain_id']}: "
              f"{result['neurons']} neurons, "
              f"{result['rate']:.2f} Hz average firing rate")
    
    return brains, batch_results

def memory_efficient_example():
    """Example demonstrating memory-efficient simulation."""
    print("\n=== Memory-Efficient Simulation Example ===")
    
    import gc
    import time
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Simulate with periodic checkpointing
    checkpoint_interval = 1000
    num_steps = 10000
    
    print(f"Running memory-efficient simulation: {num_steps} steps")
    print(f"Checkpointing every {checkpoint_interval} steps")
    
    start_time = time.time()
    
    for step in range(num_steps):
        brain.step(step)
        
        # Checkpoint periodically
        if step > 0 and step % checkpoint_interval == 0:
            checkpoint_path = f"checkpoint_{step}.pkl"
            brain.save(checkpoint_path)
            print(f"  Checkpoint saved: {checkpoint_path}")
            
            # Force garbage collection
            gc.collect()
    
    end_time = time.time()
    
    print(f"\nMemory-efficient simulation completed:")
    print(f"  Duration: {end_time - start_time:.2f} seconds")
    print(f"  Average speed: {(num_steps / (end_time - start_time)):.1f} steps/sec")
    
    # Load last checkpoint
    if num_steps > checkpoint_interval:
        brain.load(f"checkpoint_{num_steps - checkpoint_interval}.pkl")
        print(f"Loaded last checkpoint successfully")
    
    return brain

if __name__ == "__main__":
    # Run examples
    brain1 = performance_monitoring_example()
    
    sim1 = visualization_example()
    
    brains, results = batch_processing_example()
    
    brain2 = memory_efficient_example()
    
    print("\n=== Visualization and performance examples completed successfully! ===")
