#!/usr/bin/env python3
"""
Verification script for NeuralWorkingMemory Python bindings.

This script checks that all required components are present in the bindings.
Note: This should be run after the Python bindings are built and installed.
"""

import sys
import importlib.util

def check_bindings():
    """Check if pynlm module can be imported and has required components."""
    try:
        import pynlm
        print("✓ pynlm module imported successfully")
        
        # Check for NeuralWorkingMemory class
        if hasattr(pynlm, 'NeuralWorkingMemory'):
            print("✓ NeuralWorkingMemory class found")
            
            # Check key methods
            required_methods = [
                '__init__', 'initialize', 'store', 'storeToNeuron',
                'retrieve', 'contains', 'getNeuronActivation',
                'update', 'clear', 'getActiveTraces', 'getCapacity',
                'setCapacity', 'getDecayRate', 'setDecayRate',
                'getMemoryNeurons', 'strengthenMemory', 'runCompetition',
                'isWinning', 'getMemoryActivity'
            ]
            
            missing_methods = []
            for method in required_methods:
                if not hasattr(pynlm.NeuralWorkingMemory, method):
                    missing_methods.append(method)
            
            if not missing_methods:
                print("✓ All required methods present in NeuralWorkingMemory")
            else:
                print(f"✗ Missing methods: {missing_methods}")
                return False
                
        else:
            print("✗ NeuralWorkingMemory class not found")
            return False
            
        # Check helper functions
        required_helpers = [
            'getWorkingMemoryFromBrain',
            'createWorkingMemory', 
            'createWorkingMemoryConfig'
        ]
        
        missing_helpers = []
        for helper in required_helpers:
            if not hasattr(pynlm, helper):
                missing_helpers.append(helper)
        
        if not missing_helpers:
            print("✓ All helper functions found")
        else:
            print(f"✗ Missing helper functions: {missing_helpers}")
            return False
            
        # Check Brain class has getWorkingMemory
        if hasattr(pynlm, 'Brain') and hasattr(pynlm.Brain, 'getWorkingMemory'):
            print("✓ Brain.getWorkingMemory() method found")
        else:
            print("✗ Brain.getWorkingMemory() method missing")
            return False
            
        print("\n" + "="*60)
        print("BINDINGS VERIFICATION SUCCESSFUL")
        print("="*60)
        print("\nSummary:")
        print("- NeuralWorkingMemory class with all methods: ✓")
        print("- Helper functions: ✓")
        print("- Brain integration: ✓")
        print("\nThe Python bindings for NeuralWorkingMemory are ready for use!")
        print("\nTo use the bindings:")
        print("1. Import pynlm")
        print("2. Create brain or working memory instances")
        print("3. Use the exposed methods for working memory operations")
        
        return True
        
    except ImportError as e:
        print(f"✗ Import error: {e}")
        print("\nThis script should be run after building and installing the Python bindings:")
        print("1. Build with CMake: mkdir build && cd build && cmake .. && make")
        print("2. Install with pip: pip install .")
        return False
    except Exception as e:
        print(f"✗ Error: {e}")
        return False

if __name__ == "__main__":
    success = check_bindings()
    sys.exit(0 if success else 1)