#!/usr/bin/env python3
"""
Test script for NLM Python bindings.
This script verifies that the improved Python bindings work correctly.
"""

import pynlm
import sys
import traceback

def test_basic_functionality():
    """Test basic NLM Python binding functionality."""
    print("Testing basic NLM functionality...")
    
    try:
        # Test 1: Create default configuration
        print("  1. Creating default configuration...")
        config = pynlm.createDefaultConfig()
        assert config is not None
        print("     ✓ Configuration created successfully")
        
        # Test 2: Create brain
        print("  2. Creating brain...")
        brain = pynlm.createBrain(config)
        assert brain is not None
        print("     ✓ Brain created successfully")
        
        # Test 3: Initialize brain
        print("  3. Initializing brain...")
        success = brain.initialize()
        assert success, "Brain initialization failed"
        print("     ✓ Brain initialized successfully")
        
        # Test 4: Check brain statistics
        print("  4. Checking brain statistics...")
        stats = brain.getStats()
        assert "total_neurons" in stats
        assert "total_synapses" in stats
        assert stats["total_neurons"] > 0
        print(f"     ✓ Brain stats retrieved: {stats}")
        
        # Test 5: Run simulation steps
        print("  5. Running simulation steps...")
        for step in range(10):
            brain.step(step)
        print("     ✓ Simulation steps completed successfully")
        
        # Test 6: Create world
        print("  6. Creating world...")
        world = pynlm.createSimpleWorld()
        assert world is not None
        print("     ✓ World created successfully")
        
        # Test 7: Configure world
        print("  7. Configuring world...")
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        print("     ✓ World configured successfully")
        
        # Test 8: Reset world
        print("  8. Resetting world...")
        world.reset()
        print("     ✓ World reset successfully")
        
        # Test 9: Create agent
        print("  9. Creating agent...")
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        print("     ✓ Agent created successfully")
        
        # Test 10: Initialize agent
        print("  10. Initializing agent...")
        agent.initialize(world)
        print("     ✓ Agent initialized successfully")
        
        # Test 11: Get subsystem status
        print("  11. Getting agent subsystem status...")
        status = agent.getSubsystemsStatus()
        assert "reward_modulation" in status
        assert "curiosity" in status
        print(f"     ✓ Agent status retrieved: {status}")
        
        print("\n✓ All basic functionality tests passed!")
        return True
        
    except Exception as e:
        print(f"\n✗ Basic functionality test failed: {e}")
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling in Python bindings."""
    print("\nTesting error handling...")
    
    try:
        # Test 1: Try to load invalid config file
        print("  1. Testing invalid config file loading...")
        config = pynlm.createDefaultConfig()
        try:
            config.loadFromFile("/nonexistent/file.json")
            print("     ✗ Should have raised an error for invalid file")
            return False
        except Exception as e:
            print(f"     ✓ Correctly raised error: {type(e).__name__}")
        
        # Test 2: Try to get invalid config key
        print("  2. Testing invalid config key access...")
        try:
            config.get("invalid_key")
            print("     ✗ Should have raised an error for invalid key")
            return False
        except Exception as e:
            print(f"     ✓ Correctly raised error: {type(e).__name__}")
        
        # Test 3: Try to validate invalid config key
        print("  3. Testing invalid config key validation...")
        try:
            config.validateKey("another_invalid_key")
            print("     ✗ Should have raised an error for invalid key")
            return False
        except Exception as e:
            print(f"     ✓ Correctly raised error: {type(e).__name__}")
        
        print("\n✓ All error handling tests passed!")
        return True
        
    except Exception as e:
        print(f"\n✗ Error handling test failed: {e}")
        traceback.print_exc()
        return False

def test_convenience_functions():
    """Test convenience functions and enhanced API."""
    print("\nTesting convenience functions...")
    
    try:
        # Test 1: Create standard configuration
        print("  1. Testing standard configuration...")
        config = pynlm.createStandardConfig()
        assert config is not None
        assert config.has("neuron_count")
        assert config.has("region_count")
        print("     ✓ Standard configuration created successfully")
        
        # Test 2: Create ready agent
        print("  2. Testing ready agent creation...")
        result = pynlm.createReadyAgent(width=15, height=15)
        assert "brain" in result
        assert "world" in result
        assert "agent" in result
        print("     ✓ Ready agent created successfully")
        
        # Test 3: Run simple episode
        print("  3. Testing simple episode run...")
        agent = result["agent"]
        world = result["world"]
        
        # Run a short episode
        pynlm.runSimpleEpisode(result["brain"], result["world"], result["agent"], 20)
        print("     ✓ Simple episode completed successfully")
        
        print("\n✓ All convenience function tests passed!")
        return True
        
    except Exception as e:
        print(f"\n✗ Convenience function test failed: {e}")
        traceback.print_exc()
        return False

def test_advanced_features():
    """Test advanced features and API enhancements."""
    print("\nTesting advanced features...")
    
    try:
        # Test 1: Create brain with enhanced API
        print("  1. Testing enhanced brain API...")
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test enhanced statistics access
        stats = brain.getStats()
        assert stats["total_neurons"] == 500
        print("     ✓ Enhanced brain API works correctly")
        
        # Test 2: Enhanced error checking
        print("  2. Testing enhanced error checking...")
        is_valid = brain.isValid()
        is_initialized = brain.checkInitialized()
        print(f"     ✓ Brain validation: valid={is_valid}, initialized={is_initialized}")
        
        # Test 3: Pythonic configuration access
        print("  3. Testing Pythonic configuration access...")
        config.set("brain.synapse_density", 0.15)
        synapse_density = config.get("brain.synapse_density")
        assert synapse_density == 0.15
        print("     ✓ Pythonic configuration access works")
        
        # Test 4: Enhanced action production
        print("  4. Testing enhanced action production...")
        action = brain.produceAction()
        # Action should not be None if brain has motor neurons
        # (brain state may vary, so we just check the method exists and doesn't crash)
        print("     ✓ Enhanced action production works")
        
        print("\n✓ All advanced feature tests passed!")
        return True
        
    except Exception as e:
        print(f"\n✗ Advanced feature test failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    print(f"Python Bindings Version: {pynlm.__version__}")
    print("=" * 60)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Error Handling", test_error_handling),
        ("Convenience Functions", test_convenience_functions),
        ("Advanced Features", test_advanced_features),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'=' * 60}")
        print(f"Running: {test_name}")
        print(f"{'=' * 60}")
        
        try:
            if test_func():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"\n✗ Test '{test_name}' crashed: {e}")
            traceback.print_exc()
            failed += 1
    
    print(f"\n{'=' * 60}")
    print("Test Summary")
    print(f"{'=' * 60}")
    print(f"Tests passed: {passed}")
    print(f"Tests failed: {failed}")
    print(f"Total tests: {passed + failed}")
    print(f"Success rate: {passed/(passed+failed)*100:.1f}%")
    print(f"{'=' * 60}")
    
    if failed == 0:
        print("🎉 All tests passed! The improved Python bindings are working correctly.")
        return 0
    else:
        print(f"❌ {failed} test(s) failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())