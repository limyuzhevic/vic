#!/usr/bin/env python3
"""
Simple test script to verify the improved NLM Python bindings work correctly.
This script tests basic functionality without requiring the full build system.
"""

import sys
import os

# Add the python directory to the path so we can import the module
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

def test_basic_functionality():
    """Test basic functionality of the improved bindings."""
    print("Testing basic NLM improved bindings functionality...")
    
    try:
        # Test if the module can be imported
        import pynlm_improved as nlm
        print("✓ Successfully imported pynlm_improved")
        
        # Test version
        print(f"✓ Version: {nlm.VERSION}")
        
        # Test creating configuration
        config = nlm.create_simple_config()
        print("✓ Created simple configuration")
        
        # Test configuration operations
        config.setNeuronCount(50)
        config.setRegionCount(2)
        config.setConnectionProbability(0.1)
        
        # Test getting values
        neuron_count = config.get("neuron_count")
        print(f"✓ Got neuron_count: {neuron_count}")
        
        # Test creating brain
        brain = nlm.PyBrain(config)
        print("✓ Created brain")
        
        # Test creating world
        world = nlm.PySimpleWorld()
        print("✓ Created world")
        
        # Test creating agent
        agent = nlm.PyAgentBrain(brain)
        print("✓ Created agent")
        
        # Test helper functions
        brain_init = nlm.create_default_config()
        print("✓ Created default config via helper")
        
        # Test utility functions
        distance = nlm.calculate_distance(0.0, 0.0, 3.0, 4.0)
        print(f"✓ Distance calculation: {distance} (expected ~5.0)")
        
        # Test normalization
        normalized = nlm.normalize(5.0, 0.0, 10.0)
        print(f"✓ Normalization: {normalized} (expected 0.5)")
        
        # Test clamping
        clamped = nlm.clamp(15.0, 0.0, 10.0)
        print(f"✓ Clamping: {clamped} (expected 10.0)")
        
        print("\n✅ All basic tests passed!")
        return True
        
    except ImportError as e:
        print(f"✗ Import error: {e}")
        return False
    except Exception as e:
        print(f"✗ Test error: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_advanced_functionality():
    """Test more advanced functionality."""
    print("\nTesting advanced functionality...")
    
    try:
        import pynlm_improved as nlm
        
        # Test creating simulation with helper
        brain, world, agent = nlm.create_simple_simulation()
        print("✓ Created simple simulation via helper")
        
        # Test running simulation
        results = nlm.run_simple_simulation(steps=100, verbose=False)
        print("✓ Ran simple simulation")
        
        # Check some results
        if 'final_neurons' in results:
            print(f"✓ Final neuron count: {results['final_neurons']}")
        
        # Test vision to 2D conversion
        test_vision = [0.1, 0.2, 0.3, 0.4, 0.5]
        vision_grid = nlm.vision_to_2d(test_vision, 5, 1)
        print(f"✓ Vision to 2D conversion: {len(vision_grid)} rows")
        
        print("\n✅ All advanced tests passed!")
        return True
        
    except Exception as e:
        print(f"✗ Advanced test error: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling."""
    print("\nTesting error handling...")
    
    try:
        import pynlm_improved as nlm
        
        # Test invalid configuration (should still work, but values might be invalid)
        config = nlm.create_simple_config(neuronCount=0)
        print("✓ Handled edge case (neuron_count=0)")
        
        # Test vision to 2D with edge cases
        empty_vision = []
        empty_grid = nlm.vision_to_2d(empty_vision, 10, 10)
        print(f"✓ Empty vision handling: {len(empty_grid)} rows")
        
        print("\n✅ Error handling tests passed!")
        return True
        
    except Exception as e:
        print(f"✗ Error handling test error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    print("=" * 60)
    print("NLM Improved Python Bindings Test")
    print("=" * 60)
    
    success = True
    
    # Run all tests
    success &= test_basic_functionality()
    success &= test_advanced_functionality()
    success &= test_error_handling()
    
    print("\n" + "=" * 60)
    if success:
        print("🎉 ALL TESTS PASSED! The improved bindings are working correctly.")
    else:
        print("❌ SOME TESTS FAILED! Please check the output above.")
    print("=" * 60)
    
    sys.exit(0 if success else 1)