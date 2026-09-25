#!/usr/bin/env python3
"""
NLM Python Bindings - Quick Start Demo

This is a quick start demo that shows the most common and important
features of the improved NLM Python bindings.
"""

import sys
import os
import tempfile

# Add current directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm


def main():
    print("=" * 60)
    print("NLM Python Bindings - Quick Start Demo")
    print("=" * 60)
    print()
    
    # Check if pynlm is available
    try:
        import pynlm
        print("✓ pynlm imported successfully")
        print(f"  Version: {pynlm.__version__}")
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        print("Please check your installation.")
        return
    
    print("\n" + "=" * 60)
    print("Example 1: Simple Brain Creation")
    print("=" * 60)
    
    # Create a basic brain
    print("\n1. Creating a brain...")
    config = pynlm.create_default_config()
    print(f"   Config: {config.summary()}")
    
    brain = pynlm.create_brain(config)
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    
    brain.initialize()
    print("   Brain initialized")
    
    print("\n2. Running a simple simulation...")
    for i in range(5):
        brain.step(i)
        print(f"   Step {i}: {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    print("\n" + "=" * 60)
    print("Example 2: Enhanced ID Management")
    print("=" * 60)
    
    print("\n1. Neuron ID Operations:")
    neuron_id = pynlm.NeuronId(12345)
    print(f"   Created NeuronId: {neuron_id}")
    print(f"   Value: {neuron_id.value}")
    print(f"   Index: {neuron_id.index()}")
    print(f"   As dictionary: {neuron_id.as_dict()}")
    
    print("\n2. Population ID with Sentinel:")
    valid_pop = pynlm.PopulationId(99)
    invalid_pop = pynlm.PopulationId.invalid()
    print(f"   Valid population: {valid_pop}")
    print(f"   Invalid population: {invalid_pop}")
    
    print("\n3. Enum Operations:")
    neuron_type = pynlm.NeuronType.Excitatory
    print(f"   Neuron type: {neuron_type}")
    print(f"   Is excitatory: {neuron_type.is_excitatory()}")
    print(f"   Is inhibitory: {neuron_type.is_inhibitory()}")
    
    action_type = pynlm.ActionType.MoveForward
    print(f"   Action type: {action_type}")
    print(f"   Is movement: {action_type.is_movement()}")
    print(f"   Is rotational: {action_type.is_rotational()}")
    
    print("\n" + "=" * 60)
    print("Example 3: Agent in a World")
    print("=" * 60)
    
    print("\n1. Setting up the environment...")
    world = pynlm.create_simple_world()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    print(f"   World size: {world.getWidth()}x{world.getHeight()} pixels")
    print(f"   Vision: {world.getSensoryPercept().getVision().getWidth()}x"
          f"{world.getSensoryPercept().getVision().getHeight()} pixels")
    
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    print("   Agent initialized")
    
    print("\n2. Running a simulation...")
    for step in range(10):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"   Step {step}:")
        print(f"     - Action: {action.name}")
        print(f"     - Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"     - Curiosity: {agent.getCuriosityLevel():.2f}")
    
    print("\n" + "=" * 60)
    print("Example 4: Convenience Functions")
    print("=" * 60)
    
    print("\n1. Quick test simulation...")
    test_result = pynlm.quick_test_simulation(steps=10)
    print(f"   ✓ Test completed successfully")
    print(f"   - Total steps: {test_result['total_steps']}")
    print(f"   - Total neurons: {test_result['total_neurons']}")
    print(f"   - Total spikes: {test_result['total_spikes']}")
    print(f"   - Execution time: {test_result['execution_time']:.3f}s")
    
    print("\n2. Development agent creation...")
    brain2, agent2, world2 = pynlm.create_development_agent(
        world_size=(15, 15),
        enable_learning=True,
        enable_development=True
    )
    print(f"   ✓ Development agent created")
    print(f"   - Brain: {brain2.getTotalNeuronCount()} neurons")
    print(f"   - Learning enabled: {agent2.isRewardModulationEnabled()}")
    print(f"   - Development enabled: {agent2.isDevelopmentEnabled()}")
    
    print("\n" + "=" * 60)
    print("Example 5: Configuration Management")
    print("=" * 60)
    
    print("\n1. Loading and saving configuration...")
    config_file = tempfile.mktemp(suffix='.json')
    
    # Save configuration
    config.saveToFile(config_file)
    print(f"   ✓ Configuration saved to: {config_file}")
    
    # Load configuration
    new_config = pynlm.create_default_config()
    new_config.loadFromFile(config_file)
    print(f"   ✓ Configuration loaded successfully")
    print(f"   - Loaded config: {new_config.summary()}")
    
    # Clean up
    os.unlink(config_file)
    print("   ✓ Temporary file cleaned up")
    
    print("\n" + "=" * 60)
    print("Example 6: Error Handling")
    print("=" * 60)
    
    print("\n1. Using context manager for safe operations...")
    try:
        with pynlm.BrainManager() as manager:
            print("   ✓ Brain manager created")
            success = manager.run_simulation(5)
            if success:
                print("   ✓ Simulation completed successfully")
    except Exception as e:
        print(f"   ✗ Error: {e}")
    
    print("\n" + "=" * 60)
    print("Summary")
    print("=" * 60)
    print("\nThe improved NLM Python bindings now offer:")
    print("  ✓ Enhanced Pythonic API with type hints")
    print("  ✓ Improved error handling and documentation")
    print("  ✓ Convenience functions for common tasks")
    print("  ✓ Better ID management with dictionaries and sentinel values")
    print("  ✓ Enhanced enum utilities with helper methods")
    print("  ✓ Comprehensive example suite")
    print("  ✓ Full test coverage")
    print("\nFor more information, see:")
    print("  - python/ERROR_HANDLING.md")
    print("  - python/examples/comprehensive_examples.py")
    print("  - python/test_pynlm.py (tests)")
    
    print("\n" + "=" * 60)
    print("Demo completed successfully!")
    print("=" * 60)


if __name__ == "__main__":
    main()
