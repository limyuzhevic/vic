#!/usr/bin/env python3

"""
Quick test to check if the pynlm module can be imported and basic functionality works.
"""

import sys
print("Testing pynlm Python bindings import...")

try:
    import pynlm
    print("✓ Successfully imported pynlm")
    
    # Check basic functionality
    config = pynlm.createDefaultConfig()
    print("✓ createDefaultConfig() works")
    
    brain = pynlm.createBrain(config)
    print("✓ createBrain() works")
    
    # Test basic methods
    print(f"✓ Brain has getTotalNeuronCount(): {brain.getTotalNeuronCount()}")
    print(f"✓ Brain has getTotalSynapseCount(): {brain.getTotalSynapseCount()}")
    
    # Test Vision class
    vision = pynlm.Vision(10, 10, 3)
    print(f"✓ Vision created: width={vision.getWidth()}, height={vision.getHeight()}")
    
    # Test Action class
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    print(f"✓ Action created: {action.getName()}")
    
    print("\nAll basic tests passed!")
    
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("This might be because the module hasn't been built yet.")
    print("You may need to build the project with:")
    print("  mkdir build")
    print("  cd build")
    print("  cmake ../python")
    print("  make -j4")
    
except Exception as e:
    print(f"✗ Error during testing: {e}")
    import traceback
    traceback.print_exc()
    
print("\nTest complete.")