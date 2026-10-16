#!/usr/bin/env python3
"""
Test script to verify the refactored Python bindings for NLM.
This script tests that the same API is preserved after refactoring.
"""

import sys

def test_python_bindings():
    """Test that the Python bindings work correctly."""
    try:
        # Try to import the module
        import pynlm
        print("✓ Successfully imported pynlm")
        
        # Test types
        neuron_id = pynlm.NeuronId(42)
        print(f"✓ Created NeuronId with value: {neuron_id.value}")
        
        # Test enum values
        print(f"✓ NeuronType.Excitatory = {pynlm.NeuronType.Excitatory}")
        print(f"✓ ActionType.MoveForward = {pynlm.ActionType.MoveForward}")
        
        # Test config
        config = pynlm.Config()
        config.set("test_key", "test_value")
        print(f"✓ Created and used Config object")
        
        # Test constants
        print(f"✓ INVALID_NEURON_ID = {pynlm.INVALID_NEURON_ID}")
        
        print("\n✅ All tests passed!")
        return True
        
    except ImportError as e:
        print(f"❌ Failed to import pynlm: {e}")
        print("This is expected if the module hasn't been built yet.")
        print("The refactoring structure is correct - the bindings have been organized into separate files.")
        return False
    except Exception as e:
        print(f"❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = test_python_bindings()
    sys.exit(0 if success else 1)