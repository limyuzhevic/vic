#!/usr/bin/env python3

"""
Quick verification test for NLM Python package structure.
This tests the basic package imports and functionality.
"""

import sys
import os

# Add the python directory to the path so we can import pynlm
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

def test_package_structure():
    """Test that the Python package structure is correct."""
    print("Testing NLM Python package structure...")
    
    # Test that we can import the module
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return False
    
    # Test that pynlm has expected attributes
    expected_attrs = [
        'createDefaultConfig',
        'createBrain',
        'createAgentBrain', 
        'createSimpleWorld',
        'NeuronType',
        'DevelopmentalStage',
        'ActionType',
        'MotorCommand'
    ]
    
    for attr in expected_attrs:
        if hasattr(pynlm, attr):
            print(f"✓ pynlm has attribute: {attr}")
        else:
            print(f"✗ pynlm missing attribute: {attr}")
            return False
    
    # Test basic functionality
    try:
        config = pynlm.createDefaultConfig()
        print("✓ createDefaultConfig() works")
        
        # Test config operations
        config.set("brain.neuron_count", 100)
        config.set("brain.synapse_density", 0.1)
        
        if config.has("brain.neuron_count") and config.getOr("brain.neuron_count", 0) == 100:
            print("✓ Config set/get operations work")
        else:
            print("✗ Config set/get operations failed")
            return False
            
    except Exception as e:
        print(f"✗ Basic config test failed: {e}")
        return False
    
    print("\n✓ All package structure tests passed!")
    return True
def test_python_bindings_availability():
    """Test that the Python bindings are available for compilation."""
    print("\nTesting Python bindings availability...")
    
    # Check if bindings.cpp exists
    bindings_path = os.path.join(os.path.dirname(__file__), 'python', 'bindings.cpp')
    if os.path.exists(bindings_path):
        print(f"✓ Bindings file exists: {bindings_path}")
        
        # Check file size (should not be empty)
        file_size = os.path.getsize(bindings_path)
        if file_size > 1000:  # Should be substantial
            print(f"✓ Bindings file is substantial ({file_size} bytes)")
        else:
            print(f"✗ Bindings file seems too small ({file_size} bytes)")
            return False
    else:
        print(f"✗ Bindings file not found: {bindings_path}")
        return False
    
    # Check if CMakeLists.txt exists for python module
    cmake_path = os.path.join(os.path.dirname(__file__), 'python', 'CMakeLists.txt')
    if os.path.exists(cmake_path):
        print(f"✓ CMakeLists.txt exists: {cmake_path}")
    else:
        print(f"✗ CMakeLists.txt not found: {cmake_path}")
        return False
    
    print("\n✓ All bindings availability tests passed!")
    return True
if __name__ == "__main__":
    print("=" * 60)
    print("NLM Python Package Structure Verification")
    print("=" * 60)
    
    success = True
    success &= test_package_structure()
    success &= test_python_bindings_availability()
    
    print("\n" + "=" * 60)
    if success:
        print("VERIFICATION SUCCESSFUL")
        print("The NLM Python package structure is correctly set up!")
        sys.exit(0)
    else:
        print("VERIFICATION FAILED")
        print("There are issues with the NLM Python package structure.")
        sys.exit(1)