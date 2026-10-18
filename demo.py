#!/usr/bin/env python3
"""
Quick demonstration of the enhanced NLM Python bindings features.

This script showcases the new functionality added to the NLM Python bindings:
1. Context managers
2. Convenience factory functions
3. Enhanced error handling
4. Pythonic interfaces
5. Preset configurations

To run this demo, ensure the pynlm module is properly installed.
"""

import sys

try:
    import pynlm
    PYNL_AVAILABLE = True
except ImportError:
    print("Error: pynlm module not found.")
    print("Please install the package using: pip install pynlm")
    sys.exit(1)

def demo_basic_usage():
    """Demonstrate basic brain usage."""
    print("1. Basic Brain Usage")
    print("-" * 30)
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for step in range(20):
        brain.step(step)
        
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print()

def demo_convenience_factory():
    """Demonstrate convenience factory function."""
    print("2. Convenience Factory Function")
    print("-" * 30)
    
    brain, config = pynlm.createQuickSimulation(
        neuronCount=200,
        simulationTime=50.0
    )
    
    print(f"   Created simulation with {brain.getTotalNeuronCount()} neurons")
    print(f"   Configuration keys: {len(brain.getConfig().getKeys())}")
    print()

def demo_preset_configs():
    """Demonstrate preset configurations."""
    print("3. Preset Configurations")
    print("-" * 30)
    
    configs = [
        ("Balanced", pynlm.createBalancedConfig),
        ("High-Performance", pynlm.createHighPerformanceConfig),
        ("Learning-Focused", pynlm.createLearningFocusedConfig),
    ]
    
    for name, config_func in configs:
        config = config_func()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"   {name}:")
        print(f"     Neurons: {brain.getTotalNeuronCount()}")
        print(f"     E/I Ratio: {brain.getExcitationInhibitionRatio():.3f}")
        print()

def demo_context_manager():
    """Demonstrate context manager usage."""
    print("4. Context Manager")
    print("-" * 30)
    
    brain, world, agent = pynlm.createCompleteAgentSetup()
    
    print("   Using context manager for brain:")
    with brain as managed_brain:
        managed_brain.initialize()
        for step in range(10):
            managed_brain.step(step)
            print(f"     Step {step}: {managed_brain.getFiringNeuronCount()} neurons firing")
    
    print("   ✓ Brain automatically reset after context")
    print()

def demo_agent_simulation():
    """Demonstrate agent simulation."""
    print("5. Agent Simulation")
    print("-" * 30)
    
    brain, world, agent = pynlm.createCompleteAgentSetup()
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(15):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"     Step {step}: Action={action.getType()}, "
              f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    print(f"   Final developmental stage: {brain.getDevelopmentalStage()}")
    print()

def demo_world_configs():
    """Demonstrate world configuration presets."""
    print("6. World Configuration Presets")
    print("-" * 30)
    
    worlds = [
        ("Standard", pynlm.createStandardWorld),
        ("Large", pynlm.createLargeWorld),
        ("Small", pynlm.createSmallWorld),
    ]
    
    for name, world_func in worlds:
        world = world_func()
        print(f"   {name} World: {world.getWidth()}x{world.getHeight()}")
    
    print()

def demo_utility_functions():
    """Demonstrate utility functions."""
    print("7. Utility Functions")
    print("-" * 30)
    
    # Run simulation steps
    brain, config = pynlm.createQuickSimulation()
    pynlm.runSimulationSteps(brain, 30, 0.1)
    print(f"   Simulation completed: {brain.getTotalSpikeCount()} spikes")
    
    # Create visualization config
    viz_config = pynlm.createVisualizationConfig()
    print(f"   Visualization config: FPS={viz_config.get('visualization.fps')}")
    
    # Create debug config
    debug_config = pynlm.createDebugConfig()
    print(f"   Debug config: Enabled={debug_config.get('debug.enabled')}")
    print()

def main():
    """Run the demonstration."""
    print("NLM Python Bindings - Feature Demonstration")
    print("=" * 50)
    print()
    
    try:
        demo_basic_usage()
        demo_convenience_factory()
        demo_preset_configs()
        demo_context_manager()
        demo_agent_simulation()
        demo_world_configs()
        demo_utility_functions()
        
        print("=" * 50)
        print("✅ All demonstrations completed successfully!")
        print()
        print("The NLM Python bindings now offer:")
        print("  • Enhanced documentation and type hints")
        print("  • Convenience factory functions")
        print("  • Preset configurations for different use cases")
        print("  • Context managers for safe resource management")
        print("  • Improved error handling")
        print("  • Pythonic interfaces with property access")
        print("  • Utility functions for common tasks")
        print("  • Performance optimization options")
        
    except Exception as e:
        print(f"❌ Error during demonstration: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())