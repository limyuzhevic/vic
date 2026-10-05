#!/usr/bin/env python3
"""
Advanced Neural Network Simulation with Monitoring
Example 2: Advanced NLM usage with performance monitoring and analytics

This example demonstrates advanced features including performance monitoring,
statistics collection, and comprehensive analytics for neural simulations.
"""

import sys
sys.path.insert(0, '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_0a0a4035-c27c-4d0f-b5e0-c56865fd20dd/python')

import nlm_py
import time
from nlm_utils import SimulationRunner, ConfigBuilder, MetricsCollector, PerformanceProfiler, PluginLoader

def main():
    print("=== Advanced NLM Simulation with Monitoring ===")
    
    # Create research configuration
    config = ConfigBuilder.create_research_config()
    config.max_steps = 500
    config.enable_performance_monitoring = True
    config.enable_profiling = True
    
    print(f"Configuration:")
    print(f"  Max steps: {config.max_steps}")
    print(f"  STDP learning rate: {config.stdp_learning_rate}")
    print(f"  Hebbian learning rate: {config.hebbian_learning_rate}")
    print(f"  Development enabled: {config.enable_development}")
    print(f"  Working memory enabled: {config.enable_working_memory}")
    print(f"  Episodic memory enabled: {config.enable_episodic_memory}")
    
    # Create brain and world
    brain = nlm_py.createBrain(config)
    world = nlm_py.createSimpleWorld()
    
    # Initialize world
    world.configure(width=150.0, height=150.0, visionWidth=30, visionHeight=30)
    world.setAgentStart(75.0, 75.0)
    
    # Initialize brain
    brain.initialize()
    
    # Create monitoring tools
    metrics_collector = MetricsCollector()
    performance_profiler = PerformanceProfiler("advanced_simulation")
    performance_monitor = nlm_py.createPerformanceMonitor(brain)
    
    print("\nStarting advanced simulation with monitoring...")
    start_time = time.time()
    
    # Start performance profiling
    performance_profiler.start()
    
    # Run simulation with comprehensive monitoring
    for step in range(500):
        # Record profiling checkpoint
        if step % 100 == 0:
            performance_profiler.checkpoint(f"step_{step}")
        
        # Step the brain
        brain.step(step)
        
        # Update world
        world.update(step * 0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        brain.receiveSensoryInput(percept)
        
        # Collect metrics
        metrics_collector.collect_metrics(brain, step)
        
        # Record performance metrics
        performance_monitor.record_frame()
        
        # Print periodic status
        if step % 50 == 0:
            current_metrics = metrics_collector.get_latest_metrics()
            perf_metrics = performance_monitor.get_metrics()
            
            print(f"  Step {step}:")
            print(f"    Active neurons: {brain.getActiveNeuronCount()}")
            print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"    Total spikes: {brain.getTotalSpikeCount()}")
            print(f"    FPS: {perf_metrics.fps:.1f}")
            print(f"    Simulation time: {perf_metrics.simulation_time:.2f}s")
    
    # Stop performance profiling
    performance_profiler.stop()
    
    end_time = time.time()
    total_time = end_time - start_time
    
    print(f"\nAdvanced simulation completed!")
    print(f"Total time: {total_time:.2f} seconds")
    
    # Get comprehensive performance metrics
    perf_metrics = performance_monitor.get_metrics()
    
    print(f"\nComprehensive Performance Metrics:")
    print(f"  FPS: {perf_metrics.fps:.2f}")
    print(f"  Simulation time: {perf_metrics.simulation_time:.2f}s")
    print(f"  Memory usage: {perf_metrics.memory_usage} MB")
    print(f"  CPU utilization: {perf_metrics.cpu_utilization}%")
    print(f"  Spike count: {perf_metrics.spike_count}")
    print(f"  Neuron count: {perf_metrics.neuron_count}")
    print(f"  Synapse count: {perf_metrics.synapse_count}")
    print(f"  Average firing rate: {perf_metrics.average_firing_rate:.2f} Hz")
    print(f"  Active neurons: {perf_metrics.active_neurons}")
    print(f"  Firing neurons: {perf_metrics.firing_neurons}")
    print(f"  Energy usage: {perf_metrics.energy_usage}")
    print(f"  Steps completed: {perf_metrics.steps_completed}")
    
    # Get simulation statistics
    all_metrics = metrics_collector.get_all_metrics()
    print(f"\nCollected {len(all_metrics)} metric data points")
    
    # Print profiling results
    print(f"\nProfiling Results:")
    performance_profiler.printResults()
    
    # Export performance data
    print(f"\nExporting performance data...")
    performance_monitor.exportToCSV("advanced_simulation_performance.csv")
    performance_monitor.exportToJSON("advanced_simulation_performance.json")
    metrics_collector.export_to_csv("advanced_simulation_metrics.csv")
    metrics_collector.export_to_json("advanced_simulation_metrics.json")
    
    # Export profiling data
    performance_profiler.exportToFile("advanced_simulation_profile.json")
    
    print(f"Performance data exported successfully")
    
    # Generate summary report
    print(f"\n=== Simulation Summary ===")
    print(f"Simulation completed successfully with comprehensive monitoring!")
    print(f"Key insights:")
    print(f"  - Average firing rate: {perf_metrics.average_firing_rate:.2f} Hz")
    print(f"  - Neuron efficiency: {perf_metrics.average_firing_rate/perf_metrics.neuron_count:.4f} Hz/neuron")
    print(f"  - Performance consistency: FPS {perf_metrics.fps:.1f}")
    
    print("\n=== Advanced example completed successfully ===")

if __name__ == "__main__":
    main()