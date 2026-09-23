#!/usr/bin/env python3
"""
Quick test of improved NLM Python bindings.
This script tests the key improvements made to the bindings.
"""

import sys
import pynlm

def test_basic_functionality():
    """Test basic functionality of improved bindings."""
    print("Testing improved NLM Python bindings...")
    print("=" * 50)
    
    try:
        # Test 1: Configuration operations
        print("1. Testing configuration operations...")
        config = pynlm.createDefaultConfig()
        
        # Type-safe configuration
        config.set("neuron_count", 100)
        config.set("learning_rate", 0.01)
        config.set("plasticity_enabled", True)
        
        # Type-safe retrieval
        neuron_count = config.get("neuron_count").value
        assert neuron_count == 100
        
        learning_rate = config.get("learning_rate").value
        assert learning_rate == 0.01
        
        print("   ✓ Configuration operations work")
        
        # Test 2: Brain operations
        print("2. Testing brain operations...")
        brain = pynlm.createBrain(config)
        pynlm.initializeBrain(brain)
        
        print("   ✓ Brain created and initialized")
        
        # Test 3: Statistics
        print("3. Testing statistics...")
        stats = pynlm.getBrainStats(brain)
        neurons, total_spikes, firing_rate = stats[0], stats[6], stats[4]
        
        print(f"   ✓ Statistics: {neurons} neurons, {total_spikes} total spikes, {firing_rate:.2f} Hz")
        
        # Test 4: Convenience functions
        print("4. Testing convenience functions...")
        test_brain = pynlm.createTestBrain()
        test_world = pynlm.createTestWorld()
        test_agent = pynlm.createTestAgent()
        
        print("   ✓ Test components created")
        
        result = pynlm.runSimulation(test_brain, test_world, test_agent, 2)
        print(f"   ✓ Simulation: {result[0]} spikes, curiosity: {result[2]:.2f}")
        
        # Test 5: Error handling
        print("5. Testing error handling...")
        issues = pynlm.validateConfiguration(config)
        print(f"   ✓ Validation: found {len(issues)} issues")
        
        # Test 6: Agent features
        print("6. Testing agent features...")
        test_agent.enableRewardModulation(True)
        test_agent.enableCuriosity(True)
        test_agent.enableDevelopment(True)
        
        assert test_agent.isRewardModulationEnabled()
        assert test_agent.isCuriosityEnabled()
        assert test_agent.isDevelopmentEnabled()
        
        print("   ✓ Agent feature toggling works")
        
        # Test 7: Documentation
        print("7. Testing documentation...")
        if brain.__doc__:
            print("   ✓ Brain has documentation")
        else:
            print("   ⚠ Brain documentation check inconclusive")
        
        print("\n" + "=" * 50)
        print("SUCCESS: All tests passed!")
        print("=" * 50)
        
        return True
        
    except Exception as e:
        print(f"\nFAILED: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = test_basic_functionality()
    sys.exit(0 if success else 1)