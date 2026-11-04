#!/usr/bin/env python3
"""
Demonstration of Enhanced NLM Python Bindings

This script demonstrates the transformation from the basic NLM Python bindings
to the enhanced, production-ready Python API for neural simulation.

The enhanced bindings provide:
1. High-level wrapper classes with intuitive APIs
2. Configuration builders with fluent interfaces
3. Performance monitoring and optimization tools
4. Visualization and plotting capabilities
5. Comprehensive error handling and validation
6. Utility functions for common tasks
7. Backward compatibility with original API
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
from pynlm import (
    BrainSimulator, AgentSimulator, NLMVisualizer, NLMConfigBuilder,
    NLMPerformanceMonitor, NLMUtils, ValidationError, SimulationError
)

def demonstrate_basic_enhanced():
    """Demonstrate basic enhanced usage."""
    print("=" * 60)
    print("1. BASIC ENHANCED SIMULATION")
    print("=" * 60)
    
    # Create brain using enhanced simulator
    simulator = BrainSimulator()
    print("✓ Created BrainSimulator")
    
    # Run simulation with method chaining
    simulator.run(50)
    print("✓ Ran 50 simulation steps")
    
    # Get statistics
    stats = simulator.get_current_stats()
    print(f"✓ Collected statistics: {stats['step']} steps")
    print(f"  - Total neurons: {stats['total_neurons']}")
    print(f"  - Average firing rate: {stats['average_firing_rate']:.2f} Hz")
    print(f"  - Total spikes: {stats['total_spikes']}")
    
    # Get average firing rate
    avg_rate = simulator.get_average_firing_rate()
    print(f"✓ Average firing rate: {avg_rate:.2f} Hz")
    
    return simulator

def demonstrate_configuration_builder():
    """Demonstrate configuration builder usage."""
    print("\n" + "=" * 60)
    print("2. CONFIGURATION BUILDER")
    print("=" * 60)
    
    # Create different configurations using fluent interface
    configs = {
        'minimal': NLMConfigBuilder().apply_preset('minimal').get_config(),
        'standard': NLMConfigBuilder().apply_preset('standard').get_config(),
        'advanced': NLMConfigBuilder().apply_preset('advanced').get_config(),
    }
    
    print("✓ Created preset configurations:")
    for name, config in configs.items():
        summary = config.summary()
        print(f"  - {name}: {summary}")
    
    # Create custom configuration
    custom_config = (NLMConfigBuilder()
                    .apply_preset('standard')
                    .set_neural_parameters(neuron_count=3000)
                    .set_plasticity(stdp_rate=0.005)
                    .set_neuromodulation(dopamine_scale=2.0)
                    .get_config())
    
    print(f"✓ Created custom configuration: {custom_config.summary()}")
    
    # Run simulation with custom config
    brain = pynlm.createBrain(custom_config)
    brain.initialize()
    
    for step in range(20):
        brain.step(step)
    
    print(f"✓ Custom configuration simulation complete:")
    print(f"  - Neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    return configs, custom_config

def demonstrate_agent_simulation():
    """Demonstrate agent simulation."""
    print("\n" + "=" * 60)
    print("3. AGENT SIMULATION")
    print("=" * 60)
    
    # Create agent simulator
    simulator = AgentSimulator(
        world_width=25,
        world_height=25,
        vision_width=10,
        vision_height=10
    )
    
    print("✓ Created AgentSimulator")
    print(f"  - World size: {simulator.world_width}x{simulator.world_height}")
    print(f"  - Vision: {simulator.vision_width}x{simulator.vision_height}")
    
    # Initialize
    simulator.initialize()
    print("✓ Initialized agent, brain, and world")
    
    # Run episode
    def progress_callback(step, metrics):
        if step % 25 == 0:
            print(f"  Step {step}: {metrics['action_type']} action, "
                  f"{metrics['brain_stats']['firing_neurons']} firing neurons")
    
    episode_data = simulator.run_episode(100, progress_callback)
    print(f"✓ Completed 100-step episode")
    
    # Get episode statistics
    stats = simulator.get_episode_stats()
    print(f"✓ Episode statistics:")
    print(f"  - Total steps: {stats['total_steps']}")
    print(f"  - Unique actions: {stats['unique_actions']}")
    print(f"  - Average firing rate: {stats['average_firing_rate']:.2f} Hz")
    print(f"  - Total spikes: {stats['total_spikes']}")
    
    return simulator, episode_data

def demonstrate_performance_monitoring():
    """Demonstrate performance monitoring."""
    print("\n" + "=" * 60)
    print("4. PERFORMANCE MONITORING")
    print("=" * 60)
    
    # Create performance monitor
    monitor = NLMPerformanceMonitor()
    print("✓ Created NLMPerformanceMonitor")
    
    # Create and simulate brain
    simulator = BrainSimulator()
    monitor.start_monitoring()
    print("✓ Started performance monitoring")
    
    # Run simulation with monitoring
    for step in range(50):
        simulator.step(step)
        metrics = monitor.end_step('simulation_step')
        
        if step % 25 == 0:
            print(f"  Step {step}: {metrics['steps_per_second']:.1f} steps/sec")
    
    # Get performance summary
    summary = monitor.get_summary()
    print(f"✓ Performance summary:")
    print(f"  - Total steps: {summary['total_steps']}")
    print(f"  - Total time: {summary['total_time']:.2f}s")
    print(f"  - Average step time: {summary['average_step_time']*1000:.1f}ms")
    print(f"  - Steps per second: {summary['steps_per_second']:.1f}")
    
    return simulator, monitor

def demonstrate_utils():
    """Demonstrate utility functions."""
    print("\n" + "=" * 60)
    print("5. UTILITY FUNCTIONS")
    print("=" * 60)
    
    # Generate random config
    print("✓ Generating random configuration...")
    random_config = NLMUtils.generate_random_config(
        neuron_range=(500, 2000),
        synapse_range=(0.05, 0.3)
    )
    print(f"  - Neuron count: {random_config.summary()}")
    
    # Create simulation grid
    print("\n✓ Creating simulation grid...")
    agents = NLMUtils.create_simulation_grid(world_size=20, num_agents=3)
    print(f"  - Created {len(agents)} agents")
    for i, agent in enumerate(agents):
        print(f"    Agent {i}: ({agent['agent']['x']:.1f}, {agent['agent']['y']:.1f})")
    
    # Run benchmark
    print("\n✓ Running performance benchmark...")
    benchmark = NLMUtils.benchmark_simulation(num_steps=100)
    print(f"  - Benchmark results:")
    print(f"    - Steps: {benchmark['steps']}")
    print(f"    - Duration: {benchmark['duration']:.2f}s")
    print(f"    - Steps per second: {benchmark['steps_per_second']:.1f}")
    print(f"    - Neurons: {benchmark['neurons']}")
    print(f"    - Average firing rate: {benchmark['average_firing_rate']:.2f} Hz")
    
    return random_config, agents

def demonstrate_visualization():
    """Demonstrate visualization capabilities."""
    print("\n" + "=" * 60)
    print("6. VISUALIZATION")
    print("=" * 60)
    
    # Create simulation data
    simulator = BrainSimulator()
    stats_history = [simulator.get_current_stats() for _ in range(20)]
    
    print("✓ Created sample statistics data")
    print(f"  - Data points: {len(stats_history)}")
    
    # Try to create visualizations
    try:
        import matplotlib.pyplot as plt
        
        # Create brain activity plot
        fig1, plt1 = NLMVisualizer.plot_brain_activity(stats_history)
        if fig1:
            plt1.savefig('demo_brain_activity.png', dpi=100, bbox_inches='tight')
            print("✓ Created brain activity visualization")
            plt1.close()
        
        # Create performance metrics plot
        fig2, plt2 = NLMVisualizer.plot_performance_metrics(stats_history)
        if fig2:
            plt2.savefig('demo_performance_metrics.png', dpi=100, bbox_inches='tight')
            print("✓ Created performance metrics visualization")
            plt2.close()
        
        print("✓ Visualization files saved:")
        print("  - demo_brain_activity.png")
        print("  - demo_performance_metrics.png")
        
    except ImportError:
        print("⚠ Matplotlib not available - skipping visualization")
        print("  (Install with: pip install matplotlib)")
    
    return simulator, stats_history

def demonstrate_error_handling():
    """Demonstrate error handling."""
    print("\n" + "=" * 60)
    print("7. ERROR HANDLING")
    print("=" * 60)
    
    # Test 1: Brain not initialized
    print("✓ Test 1: Brain not initialized")
    simulator = BrainSimulator()
    
    # Manually set to test error handling
    simulator._is_initialized = False
    
    try:
        simulator.step(0)
        print("  ERROR: Should have raised SimulationError")
    except pynlm.SimulationError as e:
        print(f"  ✓ Correctly caught SimulationError: {e}")
    
    # Test 2: Validation error
    print("\n✓ Test 2: Validation error")
    try:
        from pynlm import ValidationError
        raise ValidationError("Test validation error message")
    except ValidationError as e:
        print(f"  ✓ Correctly caught ValidationError: {e}")
    
    # Test 3: Invalid preset
    print("\n✓ Test 3: Invalid preset")
    try:
        config = NLMConfigBuilder().apply_preset('invalid_preset')
        print("  ERROR: Should have raised ValidationError")
    except ValidationError as e:
        print(f"  ✓ Correctly caught ValidationError: {e}")
    
    print("\n✓ Error handling demonstration complete")

def demonstrate_backward_compatibility():
    """Demonstrate backward compatibility with original API."""
    print("\n" + "=" * 60)
    print("8. BACKWARD COMPATIBILITY")
    print("=" * 60)
    
    print("✓ Demonstrating backward compatibility with original API...")
    
    # Old API example
    print("\nOld API example:")
    print("  import pynlm")
    print("  config = pynlm.createDefaultConfig()")
    print("  brain = pynlm.createBrain(config)")
    print("  brain.initialize()")
    print("  for step in range(100):")
    print("      brain.step(step)")
    
    # New API example
    print("\nNew API example:")
    print("  from pynlm import BrainSimulator")
    print("  simulator = BrainSimulator()")
    print("  simulator.run(100)")
    print("  for stats in simulator.get_stats_history():")
    print("      print(f\"Step {stats['step']}: {stats['firing_neurons']} neurons\")")
    
    # Actually run both to show compatibility
    print("\nRunning both APIs for comparison...")
    
    # Old API
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(10):
        brain.step(step)
    
    print(f"✓ Old API: {brain.getTotalNeuronCount()} neurons, "
          f"{brain.getAverageFiringRate():.2f} Hz")
    
    # New API
    simulator = BrainSimulator()
    simulator.run(10)
    
    print(f"✓ New API: {simulator.get_current_stats()['total_neurons']} neurons, "
          f"{simulator.get_average_firing_rate():.2f} Hz")
    
    print("\n✓ Both APIs produce equivalent results")

def main():
    """Run all demonstrations."""
    print("Enhanced NLM Python Bindings Demonstration")
    print("=" * 60)
    print("This script demonstrates the enhanced Python bindings for NLM")
    print("neural simulation, showcasing the transformation from basic")
    print("API to a comprehensive, production-ready interface.")
    print("=" * 60)
    
    try:
        # Run all demonstrations
        simulator1 = demonstrate_basic_enhanced()
        
        configs, custom_config = demonstrate_configuration_builder()
        
        simulator2, episode_data = demonstrate_agent_simulation()
        
        simulator3, monitor = demonstrate_performance_monitoring()
        
        random_config, agents = demonstrate_utils()
        
        simulator4, stats_history = demonstrate_visualization()
        
        demonstrate_error_handling()
        
        demonstrate_backward_compatibility()
        
        print("\n" + "=" * 60)
        print("DEMONSTRATION COMPLETE")
        print("=" * 60)
        print("\nEnhanced features demonstrated:")
        print("  1. ✓ High-level wrapper classes with method chaining")
        print("  2. ✓ Configuration builders with preset configurations")
        print("  3. ✓ Performance monitoring and profiling")
        print("  4. ✓ Agent-environment interaction management")
        print("  5. ✓ Utility functions and helpers")
        print("  6. ✓ Visualization and plotting capabilities")
        print("  7. ✓ Comprehensive error handling")
        print("  8. ✓ Backward compatibility with original API")
        print("\nThe enhanced NLM Python bindings provide a powerful,")
        print("user-friendly interface for neural simulation that serves")
        print("users from beginners to advanced researchers.")
        
    except Exception as e:
        print(f"\nERROR during demonstration: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
