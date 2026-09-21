#!/usr/bin/env python3
"""
Simple test to verify NLM Python bindings work correctly after our improvements.
This tests the new factory methods and convenience methods.
"""

import sys
import os

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("\n=== Testing Basic Functionality ===")
    
    # Test 1: Create default config
    try:
        config = pynlm.createDefaultConfig()
        print("✓ Created default config")
    except Exception as e:
        print(f"✗ Failed to create default config: {e}")
        return False
    
    # Test 2: Create brain
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✓ Created and initialized brain")
        print(f"  - Neurons: {brain.getTotalNeuronCount()}")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    except Exception as e:
        print(f"✗ Failed to create/initialize brain: {e}")
        return False
    
    # Test 3: Run a few steps
    try:
        for i in range(10):
            brain.step(i)
        print(f"✓ Ran 10 simulation steps")
        print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
    except Exception as e:
        print(f"✗ Failed to run simulation steps: {e}")
        return False
    
    return True

def test_factory_methods():
    """Test new factory methods."""
    print("\n=== Testing Factory Methods ===")
    
    # Test 1: createVision
    try:
        vision1 = pynlm.createVision()
        print("✓ Created empty Vision")
        
        vision2 = pynlm.createVision(8, 8, 3)
        print("✓ Created Vision with dimensions (8, 8, 3)")
    except Exception as e:
        print(f"✗ Failed to create Vision: {e}")
        return False
    
    # Test 2: createAudio
    try:
        audio1 = pynlm.createAudio()
        print("✓ Created empty Audio")
        
        audio2 = pynlm.createAudio(44100, 1024)
        print("✓ Created Audio with sampleRate=44100, numSamples=1024")
    except Exception as e:
        print(f"✗ Failed to create Audio: {e}")
        return False
    
    # Test 3: createInternalSignals
    try:
        internal = pynlm.createInternalSignals()
        print("✓ Created empty InternalSignals")
    except Exception as e:
        print(f"✗ Failed to create InternalSignals: {e}")
        return False
    
    # Test 4: createAction
    try:
        action1 = pynlm.createAction()
        print("✓ Created empty Action")
        
        action2 = pynlm.createAction(pynlm.ActionType.MoveForward)
        print("✓ Created Action with MoveForward type")
        
        action3 = pynlm.createAction(pynlm.ActionType.MoveForward, [0.5, 0.5, 0.5])
        print("✓ Created Action with MoveForward type and parameters")
    except Exception as e:
        print(f"✗ Failed to create Action: {e}")
        return False
    
    # Test 5: createWorldObject
    try:
        obj1 = pynlm.createWorldObject()
        print("✓ Created empty WorldObject")
        
        obj2 = pynlm.createWorldObject(10.0, 10.0, pynlm.WorldObjectType.Resource, 1.0)
        print("✓ Created WorldObject at (10,10) with Resource type")
        
        obj3 = pynlm.createWorldObject(5.0, 5.0, pynlm.WorldObjectType.Hazard, 5.0, 0.3)
        print("✓ Created WorldObject with all parameters")
    except Exception as e:
        print(f"✗ Failed to create WorldObject: {e}")
        return False
    
    return True

def test_config_convenience_methods():
    """Test new Config convenience methods."""
    print("\n=== Testing Config Convenience Methods ===")
    
    try:
        config = pynlm.createDefaultConfig()
        
        # Test setting values with convenience methods
        config.set("int_value", 42)
        config.set("float_value", 3.14)
        config.set("bool_value", True)
        config.set("string_value", "hello")
        
        print("✓ Set values using convenience methods")
        
        # Test getting values
        int_val = config.get("int_value")
        print(f"✓ Got int value: {int_val}")
        
        # Test getOr method
        missing_val = config.getOr("missing_key", "default_value")
        print(f"✓ Got missing value with default: {missing_val}")
        
        # Test has method
        has_int = config.has("int_value")
        has_missing = config.has("missing_key")
        print(f"✓ Config has int_value: {has_int}")
        print(f"✓ Config has missing_key: {has_missing}")
        
    except Exception as e:
        print(f"✗ Failed Config convenience methods test: {e}")
        return False
    
    return True

def test_agent_functionality():
    """Test AgentBrain functionality."""
    print("\n=== Testing AgentBrain Functionality ===")
    
    try:
        # Create brain and world
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        print("✓ Created AgentBrain")
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent.initialize(world)
        print("✓ Initialized agent with world")
        
        # Enable features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        print("✓ Enabled reward modulation and curiosity")
        
        # Check status
        curiosity_level = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        print(f"✓ Curiosity level: {curiosity_level:.3f}")
        print(f"✓ Novelty level: {novelty_level:.3f}")
        
    except Exception as e:
        print(f"✗ Failed AgentBrain test: {e}")
        return False
    
    return True

def test_world_functionality():
    """Test world functionality."""
    print("\n=== Testing World Functionality ===")
    
    try:
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        world.reset()
        
        print("✓ Created and configured world")
        
        # Set agent position
        world.setAgentStart(5.0, 5.0)
        print("✓ Set agent start position")
        
        # Get world properties
        width = world.getWidth()
        height = world.getHeight()
        time = world.getSimulationTime()
        print(f"✓ World properties - width={width}, height={height}, time={time}")
        
        # Test object management
        obj = pynlm.createWorldObject(2.0, 3.0, pynlm.WorldObjectType.Resource, 1.0)
        world.addObject(obj)
        print("✓ Added world object")
        
    except Exception as e:
        print(f"✗ Failed World test: {e}")
        return False
    
    return True

def main():
    """Run all tests."""
    print("NLM Python Bindings Test Suite")
    print("=" * 50)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Factory Methods", test_factory_methods),
        ("Config Convenience Methods", test_config_convenience_methods),
        ("Agent Functionality", test_agent_functionality),
        ("World Functionality", test_world_functionality),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'='*50}")
        print(f"Running: {test_name}")
        print(f"{'='*50}")
        
        try:
            result = test_func()
            results.append((test_name, result))
            status = "PASSED" if result else "FAILED"
            print(f"\n{test_name}: {status}")
        except Exception as e:
            print(f"\n{test_name}: FAILED with exception: {e}")
            results.append((test_name, False))
    
    # Summary
    print(f"\n{'='*50}")
    print("TEST SUMMARY")
    print(f"{'='*50}")
    
    passed = 0
    total = len(results)
    
    for test_name, result in results:
        status = "PASS" if result else "FAIL"
        print(f"{test_name}: {status}")
        if result:
            passed += 1
    
    print(f"\nTotal: {passed}/{total} tests passed")
    
    if passed == total:
        print("\n🎉 All tests passed! NLM Python bindings are working correctly.")
        return 0
    else:
        print(f"\n❌ {total - passed} test(s) failed.")
        return 1

if __name__ == "__main__":
    sys.exit(main())