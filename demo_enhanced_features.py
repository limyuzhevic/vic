#!/usr/bin/env python3
"""
NLM Python Bindings - Enhanced Features Demonstration
===================================================

This script demonstrates the enhanced features of the NLM Python bindings,
including context managers, performance monitoring, advanced configurations,
and experimental features for advanced users.
"""

import pynlm
import time
import sys
from typing import Optional, List, Dict, Any

def demo_basic_usage():
    """Demonstrate basic NLM usage (legacy style)"""
    print("=== Basic Usage (Legacy Style) ===")
    
    # Create a simple brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for i in range(10):
        brain.step(i)
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print()

def demo_enhanced_configuration():
    """Demonstrate enhanced configuration presets"""
    print("=== Enhanced Configuration Presets ===")
    
    # Show available presets
    print("Available brain configurations:")
    print("  1. Mini Brain (100 neurons, fast simulation)")
    print("  2. Standard Brain (1000 neurons, balanced)")
    print("  3. Advanced Brain (5000 neurons, full features)")
    
    # Create mini brain
    config = pynlm.createMiniBrainConfig()
    print(f"\nMini brain config validation: {config}")
    
    # Create standard brain
    config = pynlm.createStandardBrainConfig()
    print(f"Standard brain config has {config.getKeys().size()} settings")
    
    # Create advanced brain
    config = pynlm.createAdvancedBrainConfig()
    print(f"Advanced brain config has {config.getKeys().size()} settings")
    print()

def demo_performance_monitoring():
    """Demonstrate performance monitoring capabilities"""
    print("=== Performance Monitoring ===")
    
    # Create brain and monitor
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    monitor = pynlm.PerformanceMonitor()
    
    print("Running simulation with performance monitoring...")
    for i in range(100):
        start_time = time.time()
        brain.step(i)
        elapsed = time.time() - start_time
        monitor.recordStep(elapsed)
        
        if (i + 1) % 25 == 0:
            stats = monitor.getStats()
            print(f"  Step {i+1}: {stats['average_step_time_ms']:.2f}ms avg, "
                  f"{stats['steps_per_second']:.1f} steps/sec")
    
    # Get final statistics
    stats = monitor.getStats()
    print(f"\nFinal performance statistics:")
    print(f"  Total steps: {stats['steps_completed']}")
    print(f"  Total time: {stats['total_time']:.2f}s")
    print(f"  Average step time: {stats['average_step_time_ms']:.2f}ms")
    print(f"  Steps per second: {stats['steps_per_second']:.1f}")
    print(f"  Performance stable: {stats['performance_stable']}")
    print()

def demo_context_manager():
    """Demonstrate the enhanced context manager"""
    print("=== Context Manager ===")
    
    # Create brain, agent, and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("Using SimulationContext for automatic resource management...")
    
    # Using the enhanced context manager
    context = pynlm.createSimulationContext(brain, agent, world, autoReset=True)
    
    # Run multiple iterations
    for iteration in range(5):
        context.simulate()
        stats = context.getStats()
        print(f"  Iteration {iteration + 1}:")
        print(f"    Neurons firing: {stats['firing_count']}")
        print(f"    Total spikes: {stats['spike_count']}")
        print(f"    Curiosity level: {stats['curiosity']:.3f}")
        print(f"    World time: {stats['world_time']:.2f}")
    
    print(f"\nContext manager automatically reset brain after use")
    print()

def demo_advanced_features():
    """Demonstrate experimental/advanced features"""
    print("=== Experimental Features ===")
    
    # Create a more complex setup
    config = pynlm.createStandardBrainConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Running simulation with advanced features enabled...")
    
    # Run simulation
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"  Step {step}:")
            print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"    Prediction error: {agent.getPredictionError():.3f}")
    
    print("\nAdvanced features demonstration complete")
    print()

def demo_network_analysis():
    """Demonstrate network analysis utilities"""
    print("=== Network Analysis ===")
    
    # Create a larger brain for analysis
    config = pynlm.createAdvancedBrainConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Analyze the network
    analysis = pynlm.VisualizationHelper.analyzeNetwork(brain)
    
    print("Network Analysis Results:")
    for key, value in analysis.items():
        if isinstance(value, (int, float)):
            print(f"  {key}: {value}")
        else:
            print(f"  {key}: [complex data]")
    
    print(f"\nEstimated memory usage: {analysis.get('estimated_memory_kb', 0):.2f} KB")
    print()

def demo_experimental_features():
    """Demonstrate experimental features for advanced users"""
    print("=== Experimental Features (Advanced Users) ===")
    
    print("Note: These features are experimental and may change or be removed in future versions.")
    print()
    
    # Create setup for experimental features
    config = pynlm.createStandardBrainConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Example 1: Synaptic manipulation (placeholder for actual implementation)
    print("1. Synaptic Manipulation (experimental):")
    print("   - strengthenConnections(): Modify synaptic weights")
    print("   - createPattern(): Create specific neural patterns")
    print()
    
    # Example 2: Custom plasticity (placeholder for actual implementation)
    print("2. Custom Plasticity (experimental):")
    print("   - applyCustomSTDP(): Apply custom STDP rules")
    print()
    
    # Example Tjeme 3: Advanced memory manipulation (placeholder for actual implementation)
    print("3. Advanced Memory Manipulation (experimental):")
    print("   - replayExperience(): Replay specific memory patterns")
    print()
    
    print("Experimental features are designed for advanced users who understand")
    print("the implications of directly manipulating neural connectivity and learning rules.")
    print()

def demo_error_handling():
    """Demonstrate enhanced error handling"""
    print("=== Enhanced Error Handling ===")
    
    try:
        # Create a configuration with invalid parameters
        config = pynlm.createDefaultConfig()
        
        # This would normally be done internally, but we show the concept
        print("Enhanced error handling provides:")
        print("  - Specific exception types (ConfigurationError, SimulationError)")
        print("  - Detailed error messages with context")
        print("  - Validation of configuration parameters")
        print("  - Graceful failure modes")
        print()
        
        print("Key exception types:")
        print("  - NLMError (base exception)")
        print("  - ConfigurationError (for configuration issues)")
        print("  - SimulationError (for simulation issues)")
        print()
        
    except Exception as e:
        print(f"Error in demo: {e}")
    
    print("Error handling demonstration complete")
    print()

def demo_integration_with_existing():
    """Show how new features integrate with existing code"""
    print("=== Integration with Existing Code ===")
    
    print("The enhanced bindings maintain full backward compatibility:")
    print()
    
    # Example 1: Legacy code still works
    print("1. Legacy code (unchanged):")
    print("   config = pynlm.createDefaultConfig()")
    print("   brain = pynlm.createBrain(config)")
    print("   brain.initialize()")
    print("   # ... existing simulation loop")
    print()
    
    # Example 2: Enhanced features for new code
    print("2. New enhanced code:")
    print("   config = pynlm.createStandardBrainConfig()")
    print("   brain = pynlm.createBrain(config)")
    print("   with pynlm.simulation_context(brain, agent, world):")
    print("       # automatic resource management")
    print("       for i in range(100):")
    print("           # enhanced simulation with monitoring")
    print()
    
    # Example 3: Gradual migration
    print("3. Gradual migration path:")
    print("   - Start with existing code (no changes needed)")
    print("   - Add monitoring: create PerformanceMonitor()")
    print("   - Add context management: createSimulationContext()")
    print("   - Add presets: createStandardBrainConfig()")
    print("   - Use experimental features when ready")
    print()

def main():
    """Run all demonstrations"""
    print("NLM Python Bindings - Enhanced Features Demo")
    print("=" * 50)
    print()
    
    try:
        demo_basic_usage()
        demo_enhanced_configuration()
        demo_performance_monitoring()
        demo_context_manager()
        demo_advanced_features()
        demo_network_analysis()
        demo_experimental_features()
        demo_error_handling()
        demo_integration_with_existing()
        
        print("=" * 50)
        print("All demonstrations completed successfully!")
        print()
        print("For more information:")
        print("- Read the enhanced documentation in the repo")
        print("- Check the experimental features in pynlm.experimental")
        print("- Review performance monitoring APIs")
        print("- Explore context manager capabilities")
        
    except Exception as e:
        print(f"\nError during demo: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()