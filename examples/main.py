#!/usr/bin/env python3
"""
Main example script demonstrating the improved NLM Python bindings.

This script demonstrates the enhanced features of the NLM Python bindings:
1. Convenience factory functions and presets
2. Context managers for automatic resource cleanup
3. Enhanced error handling
4. Pythonic interfaces with property access
5. Advanced simulation patterns
"""

import sys
import traceback

try:
    import pynlm
    PYNL_AVAILABLE = True
except ImportError:
    PYNL_AVAILABLE = False
    print("Error: pynlm module not available.")
    print("Please install the package using: pip install pynlm")
    sys.exit(1)

def print_header(title: str) -> None:
    """Print formatted header."""
    print(f"\n{'='*60}")
    print(f"{title}")
    print(f"{'='*60}")

def example_1_basic_usage() -> None:
    """Example 1: Basic brain usage."""
    print_header("Example 1: Basic Brain Usage")
    
    # Create a simple brain using the convenience function
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run a simple simulation
    for step in range(50):
        brain.step(step)
        
    print(f"✓ Brain created and simulated successfully")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")

def example_2_convenience_functions() -> None:
    """Example 2: Using convenience factory functions."""
    print_header("Example 2: Convenience Factory Functions")
    
    # Use the quick simulation factory function
    brain, config = pynlm.createQuickSimulation(
        neuronCount=300,
        simulationTime=100.0
    )
    
    print(f"✓ Quick simulation created with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation using utility function
    pynlm.runSimulationSteps(brain, 75, 0.1)
    
    print(f"  Simulation completed with {brain.getTotalSpikeCount()} spikes")

def example_3_preset_configurations() -> None:
    """Example 3: Using preset configurations."""
    print_header("Example 3: Preset Configurations")
    
    # Demonstrate different preset configurations
    presets = [
        ("Balanced", pynlm.createBalancedConfig),
        ("High-Performance", pynlm.createHighPerformanceConfig),
        ("Learning-Focused", pynlm.createLearningFocusedConfig),
    ]
    
    for name, config_func in presets:
        config = config_func()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"✓ {name} Configuration:")
        print(f"  - Neurons: {brain.getTotalNeuronCount()}")
        print(f"  - E/I Ratio: {brain.getExcitationInhibitionRatio():.3f}")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")

def example_4_context_managers() -> None:
    """Example 4: Context manager usage."""
    print_header("Example 4: Context Managers")
    
    # Create complete setup
    brain, world, agent = pynlm.createCompleteAgentSetup()
    
    print("✓ Complete setup created")
    
    # Use context manager for automatic cleanup
    print("\nUsing context manager for brain:")
    with brain as managed_brain:
        managed_brain.initialize()
        for step in range(20):
            managed_brain.step(step)
            if step % 5 == 0:
                print(f"  Step {step}: {managed_brain.getFiringNeuronCount()} neurons firing")
    
    print("✓ Brain context manager completed (automatic reset)")
    
    print("\nUsing context manager for agent:")
    with agent as managed_agent:
        managed_agent.initialize(world)
        for step in range(10):
            action = managed_agent.decodeMotorCommand()
            print(f"  Step {step}: Action = {action.getType()}")
    
    print("✓ Agent context manager completed (automatic reset)")

def example_5_agent_simulation() -> None:
    """Example 5: Complete agent simulation."""
    print_header("Example 5: Complete Agent Simulation")
    
    # Create complete agent setup
    brain, world, agent = pynlm.createCompleteAgentSetup()
    
    print("✓ Agent setup created")
    print("  Running simulation with agent-world interaction...")
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run complete agent simulation using utility function
    pynlm.runAgentEpisode(agent, world, 50)
    
    print(f"✓ Simulation completed!")
    print(f"  Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty level: {agent.getNoveltyLevel():.3f}")

def example_6_error_handling() -> None:
    """Example 6: Enhanced error handling."""
    print_header("Example 6: Enhanced Error Handling")
    
    try:
        # Create brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Attempt to load non-existent file
        result = brain.load("non_existent_file.bin")
        print(f"✓ File loading result: {'Success' if result else 'Failed (expected)'}")
        
        # Attempt to save to invalid path (may raise exception)
        try:
            brain.save("/invalid/path/brain.bin")
        except pynlm.NLMError as e:
            print(f"✓ Caught NLMError: {e}")
            
    except Exception as e:
        print(f"✓ Exception handling working: {type(e).__name__}: {e}")

def example_7_world_configurations() -> None:
    """Example 7: World configuration presets."""
    print_header("Example 7: World Configuration Presets")
    
    worlds = [
        ("Standard", pynlm.createStandardWorld),
        ("Large", pynlm.createLargeWorld),
        ("Small", pynlm.createSmallWorld),
    ]
    
    for name, world_func in worlds:
        world = world_func()
        print(f"✓ {name} World: {world.getWidth()}x{world.getHeight()} pixels")
        print(f"  Vision sensor: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")

def example_8_performance_optimizations() -> None:
    """Example 8: Performance optimization functions."""
    print_header("Example 8: Performance Optimization")
    
    # Create brain and apply optimizations
    brain, world, agent = pynlm.createCompleteAgentSetup()
    
    print("✓ Initial setup created")
    
    # Apply performance optimizations
    pynlm.optimizeForSpeed(brain)
    print("✓ Speed optimization enabled")
    
    pynlm.optimizeForAccuracy(brain)
    print("✓ Accuracy optimization enabled")
    
    # Verify optimizations are applied
    config = brain.getConfig()
    print(f"  Optimization settings check:")
    print(f"    - Performance optimization: {config.get('performance.optimization.enabled')}")
    print(f"    - SIMD acceleration: {config.get('performance.simulation.useSIMD')}")
    print(f"    - High precision: {config.get('performance.simulation.useHighPrecision')}")

def example_9_utilities() -> None:
    """Example 9: Utility functions."""
    print_header("Example 9: Utility Functions")
    
    # Create visualization configuration
    viz_config = pynlm.createVisualizationConfig()
    print(f"✓ Visualization config: FPS={viz_config.get('visualization.fps')}, Output={viz_config.get('visualization.output')}")
    
    # Create debug configuration
    debug_config = pynlm.createDebugConfig()
    print(f"✓ Debug config: Enabled={debug_config.get('debug.enabled')}, Log level={debug_config.get('debug.log_level')}")

def example_10_advanced_patterns() -> None:
    """Example 10: Advanced simulation patterns."""
    print_header("Example 10: Advanced Simulation Patterns")
    
    # Create multiple brain configurations
    configs = {
        "balanced": pynlm.createBalancedConfig(),
        "fast": pynlm.createHighPerformanceConfig(),
        "adaptive": pynlm.createLearningFocusedConfig(),
    }
    
    brains = {}
    for name, config in configs.items():
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains[name] = brain
        print(f"✓ Created {name} brain: {brain.getTotalNeuronCount()} neurons")
    
    # Compare performances
    print("\nPerformance comparison after 30 steps:")
    for name, brain in brains.items():
        pynlm.runSimulationSteps(brain, 30, 0.1)
        print(f"  {name.capitalize()}: {brain.getTotalSpikeCount()} spikes")

def main() -> int:
    """Run all examples."""
    print("NLM Python Bindings - Enhanced Examples")
    print("This demonstrates the improved features of the pynlm module")
    print()
    
    examples = [
        example_1_basic_usage,
        example_2_convenience_functions,
        example_3_preset_configurations,
        example_4_context_managers,
        example_5_agent_simulation,
        example_6_error_handling,
        example_7_world_configurations,
        example_8_performance_optimizations,
        example_9_utilities,
        example_10_advanced_patterns,
    ]
    
    completed = 0
    failed = 0
    
    for example_func in examples:
        try:
            example_func()
            completed += 1
            print(f"✓ {example_func.__name__} completed successfully")
        except Exception as e:
            failed += 1
            print(f"✗ {example_func.__name__} failed: {e}")
            traceback.print_exc()
        
        print()  # Add spacing between examples
    
    print_header("Summary")
    print(f"✓ Examples completed: {completed}")
    print(f"✗ Examples failed: {failed}")
    print(f"  Total: {completed + failed}")
    
    if failed > 0:
        print(f"\n⚠️  {failed} example(s) failed. Please check the output above.")
        return 1
    else:
        print(f"\n🎉 All {completed} examples completed successfully!")
        return 0

if __name__ == "__main__":
    sys.exit(main())