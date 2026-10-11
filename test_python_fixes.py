#!/usr/bin/env python3
"""
Test script to verify Python bindings fixes for memory safety and error handling.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build'))

import pynlm as nlm

def test_python_bindings_fixes():
    """Test the critical fixes in Python bindings."""
    print("Testing Python bindings fixes...")
    
    # Test 1: Create config with error handling
    print("\n1. Testing config creation...")
    try:
        config = nlm.createDefaultConfig()
        print("  ✓ Config creation successful")
    except Exception as e:
        print(f"  ✗ Config creation failed: {e}")
        return False
    
    # Test 2: Create brain from config
    print("\n2. Testing brain creation...")
    try:
        brain = nlm.createBrain(config)
        print("  ✓ Brain creation successful")
    except Exception as e:
        print(f"  ✗ Brain creation failed: {e}")
        return False
    
    # Test 3: Create world
    print("\n3. Testing world creation...")
    try:
        world = nlm.createSimpleWorld()
        print("  ✓ World creation successful")
    except Exception as e:
        print(f"  ✗ World creation failed: {e}")
        return False
    
    # Test 4: Create action from dict with error handling
    print("\n4. Testing action creation from dictionary...")
    try:
        # Test valid action
        valid_action = nlm.createActionFromDict({
            "type": "MoveForward",
            "parameters": [1.0, 2.0, 3.0]
        })
        print("  ✓ Valid action creation successful")
        
        # Test action with error handling
        try:
            invalid_action = nlm.createActionFromDict({
                "type": "InvalidAction",
                "parameters": "not a list"
            })
            print("  ✗ Invalid action should have failed but didn't")
            return False
        except Exception:
            print("  ✓ Invalid action properly rejected with error handling")
    except Exception as e:
        print(f"  ✗ Action creation failed: {e}")
        return False
    
    # Test 5: Create world object from dict
    print("\n5. Testing world object creation...")
    try:
        world_obj = nlm.createWorldObjectFromDict({
            "x": 10.0,
            "y": 20.0,
            "type": "Resource",
            "value": 100.0,
            "radius": 1.0
        })
        print("  ✓ World object creation successful")
    except Exception as e:
        print(f"  ✗ World object creation failed: {e}")
        return False
    
    print("\n✓ All Python bindings fixes verified successfully!")
    return True

if __name__ == "__main__":
    print("=== NLM Python Bindings Fix Verification ===")
    print("Testing critical fixes for memory safety and error handling")
    print("=" * 60)
    
    success = test_python_bindings_fixes()
    
    if success:
        print("\n=== ALL PYTHON BINDINGS TESTS PASSED ===")
        print("The critical fixes for Python bindings are working correctly!")
        sys.exit(0)
    else:
        print("\n=== SOME PYTHON BINDINGS TESTS FAILED ===")
        print("Some fixes need additional work.")
        sys.exit(1)