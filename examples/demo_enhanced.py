#!/usr/bin/env python3
"""
Example script demonstrating the enhanced pynlm Python bindings.

This script showcases the new features and improvements made to the
NLM Python bindings, including high-level wrapper classes, configuration
builders, performance monitoring, and visualization tools.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
from pynlm import BrainSimulator, AgentSimulator, NLMVisualizer, NLMConfigBuilder
from pynlm import NLMPerformanceMonitor, NLMUtils

def example_basic_enhanced():
    """Demonstrate basic enhanced usage."""
    print("=== Basic Enhanced Example ===")
    
    # Create a brain using the enhanced simulator
    simulator = BrainSimulator()
    simulator.step(0)
    simulator.step(1)
    simulator.step(2)
    
    print(f"Simulation complete: {simulator._simulation_step} steps")
    print(f"Statistics: {simulator.get_current_stats()}")
    
    # Get average firing rate
    print(f"Average firing rate: {simulator.get_average_firing_rate():.2f} Hz")
    
    return simulator

def example_agent_enhanced():
    """Demonstrate enhanced agent usage."""
    print("\n=== Agent Enhanced Example ===")
    
    # Create agent simulator
    simulator = AgentSimulator(world_width=25, world_height=25)
    simulator.initialize()
    
    # Run a few steps
    for i in range(5):
        action, metrics = simulator.run_step()
        print(f"Step {i}: Action {metrics['action_type']}, "
              f"Firing: {metrics['brain_stats']['firing_neurons']}")
    
    # Get episode statistics
    stats = simulator.get_episode_stats()
    print(f"Episode stats: {stats}")
    
    return simulator

def example_config_builder():
    """Demonstrate configuration builder usage."""
    print("\n=== Configuration Builder Example ===")
    
    # Create different configurations using fluent interface
    configs = {
        'minimal': NLMConfigBuilder().apply_preset('minimal').get_config(),
        'standard': NLMConfigBuilder().apply_preset('standard').get_config(),
        'advanced': NLMConfigBuilder().apply_preset('advanced').get_config(),
    }
    
    for name, config in configs.items():
        print(f"{name} config: {config.summary()}")
    
    # Create custom configuration
    custom_config = (NLMConfigBuilder()
                    .apply_preset('standard')
                    .set_neural_parameters(neuron_count=3000)
                    .set_plasticity(stdp_rate=0.005)
                    .set_neuromodulation(dopamine_scale=2.0)
                    .get_config())
    
    print(f"\nCustom config: {custom_config.summary()}")
    
    return configs, custom_config

def example_performance_monitoring():
    """Demonstrate performance monitoring."""
    print("\n=== Performance Monitoring Example ===")
    
    # Create performance monitor
    monitor = NLMPerformanceMonitor()
    
    # Create and simulate brain
    simulator = BrainSimulator()
    monitor.start_monitoring()
    
    for i in range(10):
        simulator.step(i)
        monitor.end_step('simulation_step')
    
    # Get performance summary
    summary = monitor.get_summary()
    print(f"Performance summary: {summary}")
    
    return simulator, monitor

def example_utils():
    """Demonstrate utility functions."""
    print("\n=== Utility Functions Example ===")
    
    # Generate random config
    random_config = NLMUtils.generate_random_config()
    print(f"Random config: {random_config.summary()}")
    
    # Create simulation grid
    agents = NLMUtils.create_simulation_grid(world_size=10, num_agents=3)
    print(f"Created simulation grid with {len(agents)} agents")
    for i, agent in enumerate(agents):
        print(f"  Agent {i}: ({agent['agent']['x']:.1f}, {agent['agent']['y']:.1f})")
    
    # Run benchmark
    print("\nRunning benchmark...")
    benchmark_results = NLMUtils.benchmark_simulation(num_steps=50)
    print(f"Benchmark results: {benchmark_results}")
    
    return random_config, agents

def example_visualization():
    """Demonstrate visualization capabilities."""
    print("\n=== Visualization Example ===")
    
    # Create simulation data
    simulator = BrainSimulator()
    stats_history = [simulator.get_current_stats() for _ in range(10)]
    
    # Note: This would require matplotlib to actually create plots
    # For now, we just demonstrate the API
    try:
        import matplotlib.pyplot as plt
        fig, plt_obj = NLMVisualizer.plot_brain_activity(stats_history)
        if fig:
            plt_obj.savefig('demo_visualization.png', dpi=100, bbox_inches='tight')
            print("Created visualization: demo_visualization.png")
            plt_obj.close()
        return True
    except ImportError:
        print("Matplotlib not available - skipping visualization example")
        return False

def example_error_handling():
    """Demonstrate error handling."""
    print("\n=== Error Handling Example ===")
    
    # Create uninitialized simulator
    simulator = BrainSimulator()
    
    # Manually set to test error handling
    simulator._is_initialized = False
    
    try:
        simulator.step(0)
        print("ERROR: Should have raised SimulationError")
    except pynlm.SimulationError as e:
        print(f"Correctly caught error: {e}")
    
    # Test validation error
    try:
        from pynlm import ValidationError
        raise ValidationError("Test validation error")
    except ValidationError as e:
        print(f"Correctly caught validation error: {e}")

def main():
    """Run all examples."""
    print("Enhanced pynlm Examples")
    print("=" * 50)
    
    try:
        # Run all examples
        example_basic_enhanced()
        example_agent_enhanced()
        example_config_builder()
        example_performance_monitoring()
        example_utils()
        example_visualization()
        example_error_handling()
        
        print("\n" + "=" * 50)
        print("All examples completed successfully!")
        print("\nEnhanced features demonstrated:")
        print("  - BrainSimulator with improved error handling")
        print("  - AgentSimulator with episode management")
        print("  - NLMConfigBuilder with fluent interface")
        print("  - NLMPerformanceMonitor for performance tracking")
        print("  - NLMUtils with utility functions")
        print("  - Enhanced error handling and validation")
        
    except Exception as e:
        print(f"\nError during execution: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
