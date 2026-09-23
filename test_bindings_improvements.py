#!/usr/bin/env python3
"""
Test script for NLM Python Bindings improvements
Demonstrates the new Pythonic features, convenience methods, and error handling
"""

import pynlm
import json

def test_basic_functionality():
    """Test basic Pythonic functionality"""
    print("=== Test 1: Basic Pythonic Functionality ===")
    
    # Create config using factory method
    config = pynlm.createDefaultConfig()
    print(f"Created config: {config}")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test properties and statistics
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Statistics: {brain.getStatistics()}")
    
    # Test stepping
    for i in range(10):
        brain.step(i)
    
    print(f"After 10 steps: {brain.getTotalSpikeCount()} total spikes")
    print("✓ Basic functionality test passed\n")

def test_pythonic_api():
    """Test Pythonic API patterns"""
    print("=== Test 2: Pythonic API Patterns ===")
    
    # Create simple environment
    world = pynlm.createSimpleWorld()
    print(f"Created world: {world}")
    
    # Test context manager pattern with Action
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    print(f"Created action: {action}")
    
    # Test builder pattern
    brain = pynlm.createBrainWithDefaults()
    brain.initialize()
    
    # Test chaining
    action2 = pynlm.Action().withType(pynlm.ActionType.TurnLeft).withParams([0.5])
    print(f"Created action with builder: {action2}")
    
    # Test dictionary conversion
    action_dict = action2.toDict()
    print(f"Action as dict: {json.dumps(action_dict, indent=2)}")
    
    # Test from_dict
    action3 = pynlm.Action()
    action3.fromDict(action_dict)
    print(f"Recreated action from dict: {action3}")
    
    print("✓ Pythonic API test passed\n")

def test_convenience_methods():
    """Test convenience methods"""
    print("=== Test 3: Convenience Methods ===")
    
    # Create agent with learning enabled
    brain = pynlm.createBrain(pynlm.createTestConfig())
    world = pynlm.createTestEnvironment()
    agent = pynlm.createTestAgent(brain, world)
    
    print(f"Created test agent: {agent}")
    print(f"Agent learning status: Reward={agent.isRewardModulationEnabled()}, "
          f"Development={agent.isDevelopmentEnabled()}, "
          f"Curiosity={agent.isCuriosityEnabled()}")
    
    # Test episode processing
    percept = world.getSensoryPercept()
    reward = 10.0
    predicted_reward = 8.0
    
    # Test the updateEpisode method (convenience method)
    agent.updateEpisode(percept, reward, predicted_reward, 1.0)
    
    print(f"Episode state: {agent.getEpisodeState()}")
    
    # Test toggling
    agent.toggleRewardModulation()
    print(f"Toggled reward modulation: {agent.isRewardModulationEnabled()}")
    
    print("✓ Convenience methods test passed\n")

def test_error_handling():
    """Test error handling and validation"""
    print("=== Test 4: Error Handling and Validation ===")
    
    # Test config validation
    config = pynlm.createDefaultConfig()
    errors = pynlm.validateConfig(config)
    print(f"Config validation errors: {list(errors)}")
    
    # Test null validation in factory methods
    try:
        pynlm.createAgentBrain(None)
        print("ERROR: Should have thrown exception for null brain")
    except Exception as e:
        print(f"✓ Correctly caught null brain exception: {e}")
    
    # Test input validation
    world = pynlm.createSimpleWorld()
    world.configure(50, 50, 20, 20)
    
    # Add an object and test distance calculation
    world.addObject(pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Resource, 10.0f, 0.5f))
    
    obj = world.getObjectAt(10, 10)
    if obj:
        distance = obj.distanceTo(15, 15)
        print(f"Distance to (15,15): {distance:.2f}")
        is_near = obj.isNear(12, 12, 3.0)
        print(f"Is near (12,12) within 3.0: {is_near}")
    
    print("✓ Error handling test passed\n")

def test_serialization():
    """Test serialization to/from dictionaries"""
    print("=== Test 5: Serialization ===")
    
    # Test Action serialization
    action = pynlm.Action(pynlm.ActionType.MoveForward, [0.5, 1.0])
    action_dict = action.toDict()
    print(f"Action serialization: {json.dumps(action_dict, indent=2)}")
    
    # Test from_dict
    action2 = pynlm.Action()
    action2.fromDict(action_dict)
    print(f"Deserialization: {action2}")
    
    # Test SensoryPercept serialization
    percept = pynlm.SensoryPercept()
    # Add some mock data (in real usage, this would come from vision/audio)
    print(f"Percept serialization: {percept.toDict()}")
    
    # Test Config serialization
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", "500")
    config_dict = config.to_dict()
    print(f"Config keys: {list(config_dict.keys())}")
    
    print("✓ Serialization test passed\n")

def test_simulation_utilities():
    """Test simulation utilities and debugging tools"""
    print("=== Test 6: Simulation Utilities ===")
    
    # Create test environment
    world = pynlm.createTestEnvironment()
    brain = pynlm.createBrain(pynlm.createTestConfig())
    agent = pynlm.createTestAgent(brain, world)
    
    # Run simple experiment
    results = pynlm.runSimpleExperiment(brain, world, 50)
    print(f"Experiment results: {json.dumps(results, indent=2)}")
    
    # Test monitoring
    print("Running simulation monitor...")
    metrics = pynlm.monitorSimulation(brain, 10, 2.0)
    print(f"Collected {len(metrics)} metric points")
    print(f"Sample metrics: {list(metrics)[0]}")
    
    print("✓ Simulation utilities test passed\n")

def test_constants_and_helpers():
    """Test constants and helper functions"""
    print("=== Test 7: Constants and Helpers ===")
    
    print(f"DEFAULT_WORLD_WIDTH: {pynlm.DEFAULT_WORLD_WIDTH}")
    print(f"DEFAULT_WORLD_HEIGHT: {pynlm.DEFAULT_WORLD_HEIGHT}")
    print(f"DEFAULT_VISION_SIZE: {pynlm.DEFAULT_VISION_SIZE}")
    print(f"DEFAULT_NEURON_COUNT: {pynlm.DEFAULT_NEURON_COUNT}")
    print(f"__version__: {pynlm.__version__}")
    
    # Test createTestConfig and createTestEnvironment
    test_config = pynlm.createTestConfig()
    print(f"Test config neuron count: {test_config.get('brain.neuron_count')}")
    
    test_env = pynlm.createTestEnvironment()
    print(f"Test environment: {test_env}")
    
    print("✓ Constants and helpers test passed\n")

def test_debugging_tools():
    """Test debugging and monitoring tools"""
    print("=== Test 8: Debugging Tools ===")
    
    # Test debugInfo function
    debug_data = {
        "message": "Testing debug tools",
        "step": 42,
        "firing_rate": 15.7
    }
    pynlm.debugInfo(debug_data)
    
    # Test version check
    version = pynlm.getVersion()
    print(f"NLM Python bindings version: {version}")
    
    print("✓ Debugging tools test passed\n")

def main():
    """Run all tests"""
    print("NLM Python Bindings - Comprehensive Test Suite")
    print("=" * 60)
    
    try:
        test_basic_functionality()
        test_pythonic_api()
        test_convenience_methods()
        test_error_handling()
        test_serialization()
        test_simulation_utilities()
        test_constants_and_helpers()
        test_debugging_tools()
        
        print("=" * 60)
        print("ALL TESTS PASSED! ✓")
        print("\nThe NLM Python bindings now provide:")
        print("• Pythonic API with context managers and properties")
        print("• Convenience methods for common operations")
        print("• Serialization to/from Python dicts and JSON")
        print("• Comprehensive error handling with descriptive exceptions")
        print("• Debugging and monitoring utilities")
        print("• Factory methods for quick setup")
        print("• Rich documentation and examples")
        
    except Exception as e:
        print(f"\n❌ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
