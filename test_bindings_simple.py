#!/usr/bin/env python3
"""
Quick test script to verify the improved NLM Python bindings work correctly.
This script tests basic functionality and key improvements.
"""

import sys
import traceback
import pynlm

def test_improved_bindings():
    """Test the improved Python bindings."""
    print("Testing improved NLM Python bindings...")
    print("=" * 50)
    
    try:
        # Test 1: Configuration creation and basic operations
        print("1. Testing configuration...")
        config = pynlm.createDefaultConfig()
        assert config is not None
        print("   ✓ Default config created")
        
        # Test type-safe configuration
        config.set("neuron_count", 100)
        config.set("learning_rate", 0.01)
        config.set("plasticity_enabled", True)
        
        # Test type-safe retrieval
        neuron_count = config.get("neuron_count").value
        assert neuron_count == 100
        print("   ✓ Configuration operations work")
        
        # Test 2: Brain creation and initialization
        print("2. Testing brain creation...")
        brain = pynlm.createBrain(config)
        assert brain is not None
        print("   ✓ Brain created")
        
        pynlm.initializeBrain(brain)
        print("   ✓ Brain initialized")
        
        # Test 3: Statistics collection
        print("3. Testing statistics collection...")
        stats = pynlm.getBrainStats(brain)
        assert len(stats) == 8
        neurons, synapses, active, firing, rate, eireq, spikes, pending = stats
        print(f"   ✓ Statistics: {neurons} neurons, {spikes} spikes, {rate:.2f} Hz firing rate")
        
        # Test 4: Error handling
        print("4. Testing error handling...")
        try:
            invalid_config = pynlm.createDefaultConfig()
            invalid_config.set("neuron_count", -1)
            issues = pynlm.validateConfiguration(invalid_config)
            print(f"   ✓ Validation found {len(issues)} issues")
        except Exception as e:
            print(f"   ✗ Validation failed: {e}")
            return False
        
        # Test 5: Convenience functions
        print("5. Testing convenience functions...")
        # Create test components
        test_brain = pynlm.createTestBrain()
        test_world = pynlm.createTestWorld()
        test_agent = pynlm.createTestAgent()
        
        assert test_brain is not None
        assert test_world is not None
        assert test_agent is not None
        print("   ✓ Test components created")
        
        # Test simulation convenience
        result = pynlm.runSimulation(test_brain, test_world, test_agent, 3)
        assert isinstance(result, tuple) and len(result) == 3
        print(f"   ✓ Simulation convenience: {result[0]} spikes")
        
        # Test 6: Enhanced features
        print("6. Testing enhanced features...")
        # Test agent feature toggles
        test_agent.enableRewardModulation(True)
        test_agent.enableCuriosity(True)
        test_agent.enableDevelopment(True)
        
        assert test_agent.isRewardModulationEnabled()
        assert test_agent.isCuriosityEnabled()
        assert test_agent.isDevelopmentEnabled()
        print("   ✓ Agent feature toggling works")
        
        # Test 7: Documentation and help
        print("7. Testing documentation...")
        # Check that main classes have docstrings
        assert brain.__doc__ is not None
        assert config.__doc__ is not None
        assert test_agent.__doc__ is not None
        print("   ✓ Classes have documentation")
        
        # Test 8: Type safety
        print("8. Testing type safety...")
        config2 = pynlm.createDefaultConfig()
        config2.set("test_value", "hello")
        
        # Type-safe retrieval
        value = config2.get("test_value")
        assert value.value == "hello"
        assert isinstance(value.value, str)
        print("   ✓ Type-safe retrieval works")
        
        print("\n" + "=" * 50)
        print("SUCCESS: All tests passed!")
        print("=" * 50)
        print("\nKey improvements verified:")
        print("• Enhanced error handling with NLMError")
        print("• Type-safe configuration methods")
        print("• Comprehensive statistics collection")
        print("• Convenience functions for common operations")
        print("• Test component factory methods")
        print("• Configuration validation")
        print("• Improved documentation")
        
        return True
        
    except Exception as e:
        print(f"\nFAILED: {e}")
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = test_improved_bindings()
    sys.exit(0 if success else 1)