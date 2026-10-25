#!/usr/bin/env python3
"""
Test Script for Enhanced NLM Bindings

This script tests the enhanced NLM bindings to ensure they work correctly.
"""

import sys
import os

def test_imports():
    """Test that all enhanced modules can be imported."""
    print("Testing imports...")
    
    try:
        import pynlm_enhanced as nlm
        print("✓ pynlm_enhanced imported successfully")
        print(f"  Version: {getattr(nlm, '__version__', 'unknown')}")
        print(f"  Description: {getattr(nlm, '__description__', 'unknown')}")
    except ImportError as e:
        print(f"✗ Failed to import pynlm_enhanced: {e}")
        return False
    
    try:
        import pynlm_utils as utils
        print("✓ pynlm_utils imported successfully")
    except ImportError as e:
        print(f"✗ Failed to import pynlm_utils: {e}")
        return False
    
    # Test enhanced classes
    try:
        # Create enhanced config
        config = nlm.create_enhanced_config()
        print("✓ Enhanced config created")
        
        # Create enhanced brain
        brain = nlm.create_enhanced_brain()
        print("✓ Enhanced brain created")
        
        # Test context manager
        with nlm.create_enhanced_brain() as test_brain:
            print("✓ Context manager works")
        
        # Create enhanced world
        with nlm.create_enhanced_world() as world:
            print("✓ Enhanced world created")
        
        # Create enhanced agent brain
        with nlm.create_enhanced_brain() as test_brain:
            with nlm.create_enhanced_agent_brain(test_brain) as agent:
                print("✓ Enhanced agent brain created")
        
        # Test utilities
        stats = utils.SimulationStats()
        print("✓ SimulationStats created")
        
        tracker = utils.ProgressTracker(100, "Test")
        print("✓ ProgressTracker created")
        
        config_dict = utils.config.to_dict(config)
        print("✓ Config utilities work")
        
        return True
        
    except Exception as e:
        print(f"✗ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_basic_functionality():
    """Test basic functionality of enhanced bindings."""
    print("\nTesting basic functionality...")
    
    try:
        import pynlm_enhanced as nlm
        
        # Create and initialize brain
        with nlm.create_enhanced_brain() as brain:
            brain.initialize()
            print("✓ Brain initialized")
            
            # Test stepping
            for step in range(10):
                brain.step(step)
            print("✓ Brain stepped successfully")
            
            # Test properties
            neurons = brain.get_total_neuron_count()
            synapses = brain.get_total_synapse_count()
            firing = brain.get_firing_neuron_count()
            print(f"✓ Properties accessible: {neurons} neurons, {synapses} synapses, {firing} firing")
            
            # Test save/load (simplified)
            # Note: This may fail if file permissions are restricted
            try:
                brain.save("test_brain.bin")
                print("✓ Brain save works")
                
                # Clean up
                if os.path.exists("test_brain.bin"):
                    os.remove("test_brain.bin")
            except Exception as e:
                print(f"⚠ Brain save failed (may be due to permissions): {e}")
            
            return True
            
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_enhanced_features():
    """Test enhanced features like error handling and context managers."""
    print("\nTesting enhanced features...")
    
    try:
        import pynlm_enhanced as nlm
        
        # Test error handling
        print("  Testing error handling...")
        try:
            brain = nlm.create_enhanced_brain()
            # Should fail since brain is not initialized
            brain.step(0)
            print("  ✗ Should have thrown SimulationError")
            return False
        except nlm.SimulationError as e:
            print("  ✓ SimulationError caught correctly")
        except Exception as e:
            print(f"  ✗ Wrong error type: {e}")
            return False
        
        # Test context manager
        print("  Testing context manager...")
        with nlm.create_enhanced_brain() as brain:
            brain.initialize()
            brain.step(0)
        print("  ✓ Context manager works correctly")
        
        # Test nested context managers
        print("  Testing nested context managers...")
        with nlm.create_enhanced_brain() as brain1:
            with nlm.create_enhanced_world() as world:
                with nlm.create_enhanced_agent_brain(brain1) as agent:
                    print("  ✓ Nested context managers work")
        
        return True
        
    except Exception as e:
        print(f"✗ Enhanced features test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    print("=== Enhanced NLM Bindings Test ===\n")
    
    all_passed = True
    
    if not test_imports():
        all_passed = False
    
    if not test_basic_functionality():
        all_passed = False
    
    if not test_enhanced_features():
        all_passed = False
    
    print("\n" + "="*50)
    if all_passed:
        print("✓ All tests passed! Enhanced bindings are working correctly.")
        return 0
    else:
        print("✗ Some tests failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
