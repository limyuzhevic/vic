#!/usr/bin/env python3
"""
Demonstration of NLM Python Bindings Improvements

This script demonstrates the key improvements made to the NLM Python bindings,
showing how they provide a more Pythonic, user-friendly, and powerful API.
"""

import pynlm
import json

def demonstrate_pythonic_api():
    """Demonstrate Pythonic API patterns"""
    print("=" * 60)
    print("DEMONSTRATION 1: Pythonic API Patterns")
    print("=" * 60)
    
    # Before: Manual setup
    print("\n1.1 Before: Manual setup")
    print("```python")
    print("action = pynlm.Action(pynlm.ActionType.MoveForward)")
    print("params = action.getParameters()")
    print("params.append(0.5)")
    print("action.setParameters(params)")
    print("```")
    
    # After: Pythonic chaining
    print("\n1.2 After: Pythonic method chaining")
    action = pynlm.Action().withType(pynlm.ActionType.MoveForward).withParams([0.5])
    print(f"Created action: {action}")
    
    # Before: Manual iteration
    print("\n1.3 Before: Manual iteration")
    print("```python")
    print("signals = percept.getInternal()")
    print("for i in range(len(signals)):")
    print("    signal = signals[i]")
    print("```")
    
    # After: Pythonic iteration
    print("\n1.4 After: Pythonic iteration")
    signals = pynlm.InternalSignals()
    signals.addSignal(0.1)
    signals.addSignal(0.5)
    signals.addSignal(0.9)
    print(f"Signals: {list(signals)}")
    print(f"Length: {len(signals)}")
    print(f"First signal: {next(iter(signals))}")
    
    print("\n" + "=" * 60)

def demonstrate_factory_methods():
    """Demonstrate factory methods"""
    print("DEMONSTRATION 2: Factory Methods")
    print("=" * 60)
    
    # Different factory methods
    print("\n2.1 Configuration factory methods:")
    
    # From dictionary
    config = pynlm.createConfigFromDict({
        "brain.neuron_count": "1000",
        "brain.simulation.steps": "500"
    })
    print(f"Created from dict: {config}")
    
    # From JSON string
    config_json = '{"brain.region_count": "2", "brain.learning.enabled": "true"}'
    config2 = pynlm.createConfigFromString(config_json)
    print(f"Created from JSON: {config2}")
    
    # Test environment factory
    print("\n2.2 Environment factory methods:")
    world1 = pynlm.createSimpleWorld()
    print(f"Simple world: {world1}")
    
    world2 = pynlm.createArenaWorld(50, 50)
    print(f"Arena world: {world2}")
    
    # Test agent factory
    print("\n2.3 Agent factory methods:")
    world = pynlm.createTestEnvironment()
    brain = pynlm.createBrainWithDefaults()
    agent = pynlm.createTestAgent(brain, world)
    print(f"Test agent: {agent}")
    print(f"Agent learning enabled: Reward={agent.isRewardModulationEnabled()}, "
          f"Development={agent.isDevelopmentEnabled()}, "
          f"Curiosity={agent.isCuriosityEnabled()}")
    
    print("\n" + "=" * 60)

def demonstrate_serialization():
    """Demonstrate serialization capabilities"""
    print("DEMONSTRATION 3: Serialization")
    print("=" * 60)
    
    # Action serialization
    print("\n3.1 Action serialization:")
    action = pynlm.Action(pynlm.ActionType.TurnLeft, [0.3, 0.7])
    print(f"Original action: {action}")
    
    action_dict = action.toDict()
    print(f"As dictionary: {json.dumps(action_dict, indent=2)}")
    
    # Deserialize
    action2 = pynlm.Action()
    action2.fromDict(action_dict)
    print(f"Deserialized action: {action2}")
    
    # SensoryPercept serialization
    print("\n3.2 SensoryPercept serialization:")
    percept = pynlm.SensoryPercept()
    # In real usage, this would have vision/touch/audio data
    print(f"Percept as dict: {percept.toDict()}")
    
    # Config serialization
    print("\n3.3 Config serialization:")
    config = pynlm.createDefaultConfig()
    config.set("brain.learning.rate", "0.01")
    config_dict = config.to_dict()
    print(f"Config keys: {list(config_dict.keys())}")
    
    # Update from dict
    config2 = pynlm.createConfigFromDict(config_dict)
    print(f"Updated config: {config2}")
    
    print("\n" + "=" * 60)

def demonstrate_convenience_methods():
    """Demonstrate convenience methods"""
    print("DEMONSTRATION 4: Convenience Methods")
    print("=" * 60)
    
    # AgentBrain convenience
    print("\n4.1 AgentBrain convenience methods:")
    world = pynlm.createTestEnvironment()
    brain = pynlm.createBrain(pynlm.createTestConfig())
    agent = pynlm.createTestAgent(brain, world)
    
    print(f"Agent episode state: {agent.getEpisodeState()}")
    
    # Toggle learning
    print("\n4.2 Learning subsystem toggles:")
    print(f"Initial reward modulation: {agent.isRewardModulationEnabled()}")
    
    agent.toggleRewardModulation()
    print(f"After toggle: {agent.isRewardModulationEnabled()}")
    
    agent.toggleDevelopment()
    print(f"Development after toggle: {agent.isDevelopmentEnabled()}")
    
    # Update episode
    print("\n4.3 Episode processing:")
    percept = world.getSensoryPercept()
    agent.updateEpisode(percept, 10.0, 9.0, 1.0)
    print(f"Episode processed. New state: {agent.getEpisodeState()}")
    
    print("\n" + "=" * 60)

def demonstrate_error_handling():
    """Demonstrate error handling"""
    print("DEMONSTRATION 5: Error Handling")
    print("=" * 60)
    
    # Factory method validation
    print("\n5.1 Factory method validation:")
    try:
        pynlm.createAgentBrain(None)
        print("ERROR: Should have thrown exception")
    except Exception as e:
        print(f"✓ Correctly caught exception: {e}")
    
    # Input validation
    print("\n5.2 Input validation:")
    world = pynlm.createSimpleWorld()
    world.configure(50, 50, 20, 20)
    
    # Add object and check validity
    world.addObject(pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Resource, 10.0f, 0.5f))
    
    obj = world.getObjectAt(10, 10)
    if obj:
        print(f"✓ Object found: {obj}")
        distance = obj.distanceTo(15, 15)
        print(f"Distance to (15,15): {distance:.2f}")
        print(f"Is near (12,12) within 3.0: {obj.isNear(12, 12, 3.0)}")
    
    # Configuration validation
    print("\n5.3 Configuration validation:")
    config = pynlm.createDefaultConfig()
    errors = pynlm.validateConfig(config)
    print(f"Validation errors: {list(errors)}")
    
    print("\n" + "=" * 60)

def demonstrate_debugging_tools():
    """Demonstrate debugging tools"""
    print("DEMONSTRATION 6: Debugging Tools")
    print("=" * 60)
    
    # Debug information
    print("\n6.1 Debug information:")
    debug_data = {
        "simulation_step": 42,
        "total_spikes": 156,
        "firing_rate": 12.3,
        "brain_neurons": 1000
    }
    pynlm.debugInfo(debug_data)
    
    # Version information
    print("\n6.2 Version information:")
    print(f"NLM Python bindings version: {pynlm.getVersion()}")
    print(f"Project version: {pynlm.__version__}")
    
    # Brain statistics
    print("\n6.3 Brain statistics:")
    brain = pynlm.createBrain(pynlm.createTestConfig())
    stats = brain.getStatistics()
    print("Brain statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    print("\n" + "=" * 60)

def demonstrate_constants_and_helpers():
    """Demonstrate constants and helper functions"""
    print("DEMONSTRATION 7: Constants and Helpers")
    print("=" * 60)
    
    print("\n7.1 Available constants:")
    print(f"DEFAULT_WORLD_WIDTH: {pynlm.DEFAULT_WORLD_WIDTH}")
    print(f"DEFAULT_WORLD_HEIGHT: {pynlm.DEFAULT_WORLD_HEIGHT}")
    print(f"DEFAULT_VISION_SIZE: {pynlm.DEFAULT_VISION_SIZE}")
    print(f"DEFAULT_NEURON_COUNT: {pynlm.DEFAULT_NEURON_COUNT}")
    print(f"DEFAULT_REGION_COUNT: {pynlm.DEFAULT_REGION_COUNT}")
    
    print("\n7.2 Factory constants:")
    test_config = pynlm.createTestConfig()
    print(f"Test config neuron count: {test_config.get('brain.neuron_count')}")
    
    test_env = pynlm.createTestEnvironment()
    print(f"Test environment dimensions: {test_env.getWidth()}x{test_env.getHeight()}")
    
    print("\n" + "=" * 60)

def demonstrate_simulation_utilities():
    """Demonstrate simulation utilities"""
    print("DEMONSTRATION 8: Simulation Utilities")
    print("=" * 60)
    
    # Create complete test setup
    print("\n8.1 Creating complete test setup:")
    world = pynlm.createTestEnvironment()
    brain = pynlm.createBrain(pynlm.createTestConfig())
    agent = pynlm.createTestAgent(brain, world)
    
    print(f"Created world: {world}")
    print(f"Created brain: {brain}")
    print(f"Created agent: {agent}")
    
    # Run simulation
    print("\n8.2 Running simulation:")
    results = pynlm.runSimpleExperiment(brain, world, 20)
    print(f"Experiment results: {json.dumps(results, indent=2)}")
    
    # Monitor simulation
    print("\n8.3 Monitoring simulation:")
    print("Collecting metrics for 10 steps...")
    metrics = pynlm.monitorSimulation(brain, 10, 2.0)
    print(f"Collected {len(metrics)} metric points")
    print("Sample metrics:")
    for i, metric in enumerate(metrics[:3]):
        print(f"  Step {metric['step']}: {metric['firing_rate']} firing rate, "
              f"{metric['spikes']} total spikes")
    
    print("\n" + "=" * 60)

def main():
    """Run all demonstrations"""
    print("NLM Python Bindings - Improvements Demonstration")
    print("This script showcases the new Pythonic features and improvements")
    print("made to the NLM Python bindings.\n")
    
    demonstrate_pythonic_api()
    demonstrate_factory_methods()
    demonstrate_serialization()
    demonstrate_convenience_methods()
    demonstrate_error_handling()
    demonstrate_debugging_tools()
    demonstrate_constants_and_helpers()
    demonstrate_simulation_utilities()
    
    print("\n" + "=" * 60)
    print("DEMONSTRATION COMPLETE!")
    print("=" * 60)
    print("\nKey improvements demonstrated:")
    print("✓ Pythonic API with method chaining and iteration")
    print("✓ Factory methods for quick setup")
    print("✓ Serialization to/from dictionaries")
    print("✓ Convenience methods for common operations")
    print("✓ Robust error handling and validation")
    print("✓ Debugging and monitoring tools")
    print("✓ Useful constants and helper functions")
    print("✓ Simulation utilities for experiments")
    
    print("\nThe NLM Python bindings now provide:")
    print("• A Pythonic interface that's intuitive for Python developers")
    print("• Comprehensive error handling and input validation")
    print("• Factory methods for rapid prototyping")
    print("• Serialization for complex data management")
    print("• Debugging and monitoring tools for development")
    print("• Documentation and examples for easy adoption")
    
    print("\nTo run the comprehensive test suite:")
    print("python test_bindings_improvements.py")

if __name__ == "__main__":
    main()
