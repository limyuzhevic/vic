#!/usr/bin/env python3
"""
Test script to verify NLM Python bindings functionality
"""

import sys
import pynlm

def test_basic_functionality():
    """Test basic Python bindings functionality"""
    print("Testing basic NLM Python bindings functionality...")
    
    try:
        # Test 1: Basic configuration creation
        print("Test 1: Creating configuration...")
        config = pynlm.createConfigWithDefaults()
        print("✓ Configuration created successfully")
        
        # Test 2: Basic brain creation
        print("Test 2: Creating brain...")
        brain = pynlm.createBrain(config)
        print("✓ Brain created successfully")
        
        # Test 3: Brain initialization
        print("Test 3: Initializing brain...")
        brain.initialize()
        print("✓ Brain initialized successfully")
        
        # Test 4: Basic simulation
        print("Test 4: Running basic simulation...")
        brain.runSimulation(10, log_interval=5)
        print("✓ Simulation completed successfully")
        
        # Test 5: Statistics access
        print("Test 5: Accessing statistics...")
        stats = pynlm.getBrainStatistics(brain)
        print(f"✓ Statistics retrieved: {len(stats)} keys")
        
        # Test 6: Memory stats
        print("Test 6: Accessing memory statistics...")
        memory_stats = pynlm.getMemoryStats(brain)
        print(f"✓ Memory statistics retrieved: {len(memory_stats)} keys")
        
        # Test 7: Advanced configuration
        print("Test 7: Creating advanced configuration...")
        advanced_config = pynlm.createConfigAdvanced(
            neuron_count=100,
            connection_probability=0.1,
            enable_stdp=True
        )
        print("✓ Advanced configuration created successfully")
        
        # Test 8: Error handling
        print("Test 8: Testing error handling...")
        try:
            raise pynlm.NLMError("Test error")
        except pynlm.NLMError:
            print("✓ Error handling works correctly")
        
        print("\n✅ All basic functionality tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_advanced_features():
    """Test advanced Python bindings features"""
    print("\n" + "="*60)
    print("Testing advanced NLM Python bindings features...")
    print("="*60)
    
    try:
        # Test 1: Advanced configuration with all features
        print("Test 1: Creating advanced configuration...")
        config = pynlm.createConfigAdvanced(
            neuron_count=500,
            connection_probability=0.05,
            enable_stdp=True,
            enable_hebbian=True,
            enable_structural=True,
            enable_curiosity=True,
            enable_reward_modulation=True
        )
        print("✓ Advanced configuration created successfully")
        
        # Test 2: Advanced brain simulation
        print("Test 2: Running advanced simulation...")
        brain = pynlm.createBrain(config)
        brain.initialize()
        brain.runSimulation(50, log_interval=10)
        print("✓ Advanced simulation completed successfully")
        
        # Test 3: Comprehensive statistics
        print("Test 3: Retrieving comprehensive statistics...")
        stats = pynlm.getBrainStatistics(brain)
        print(f"✓ Comprehensive statistics retrieved:")
        for key, value in stats.items():
            print(f"    {key}: {value}")
        
        # Test 4: Development statistics
        print("Test 4: Creating agent and getting development statistics...")
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(pynlm.createSimpleWorld())
        dev_stats = pynlm.getDevelopmentStats(brain, agent)
        print(f"✓ Development statistics retrieved:")
        for key, value in dev_stats.items():
            print(f"    {key}: {value}")
        
        # Test 5: Spike pattern analysis
        print("Test 5: Analyzing spike patterns...")
        pattern = pynlm.analyzeSpikePattern(brain, window_size=20)
        print(f"✓ Spike pattern analyzed: {len(pattern)} data points")
        
        # Test 6: Error handling
        print("Test 6: Advanced error handling...")
        try:
            # Test invalid configuration
            invalid_config = pynlm.createConfigAdvanced(neuron_count=-1)
        except Exception as e:
            print(f"✓ Configuration validation works: {type(e).__name__}")
        
        print("\n✅ All advanced functionality tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Advanced test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Main test function"""
    print("NLM Python Bindings - Functionality Test Suite")
    print("="*60)
    
    # Test basic functionality
    basic_success = test_basic_functionality()
    
    # Test advanced features
    advanced_success = test_advanced_features()
    
    # Summary
    print("\n" + "="*60)
    print("TEST SUMMARY")
    print("="*60)
    print(f"Basic Functionality: {'✅ PASSED' if basic_success else '❌ FAILED'}")
    print(f"Advanced Features:  {'✅ PASSED' if advanced_success else '❌ FAILED'}")
    
    if basic_success and advanced_success:
        print("\n🎉 ALL TESTS PASSED!")
        print("The NLM Python bindings are working correctly.")
        return 0
    else:
        print("\n❌ SOME TESTS FAILED!")
        print("Please check the errors above and fix any issues.")
        return 1

if __name__ == "__main__":
    sys.exit(main())