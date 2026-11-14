#!/usr/bin/env python3
"""
Test script to explore current Python bindings and identify missing features
"""

import sys
import os
import subprocess
import json

# First, let's check if we can import pynlm
try:
    import pynlm
    print("✓ Successfully imported pynlm")
    
    # Test basic functionality
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    print("✓ Basic API works")
    
    # Check what's available in the module
    print("\nAvailable in pynlm module:")
    attrs = dir(pynlm)
    for attr in sorted(attrs):
        if not attr.startswith('_'):
            print(f"  - {attr}")
    
    # Check classes
    print("\nAvailable classes:")
    classes = ['Brain', 'Config', 'SimpleWorld', 'AgentBrain', 'Vision', 'Audio', 
               'InternalSignals', 'Action', 'WorldObject', 'SensoryPercept',
               'AgentBody', 'ActionResult']
    for cls in classes:
        if hasattr(pynlm, cls):
            print(f"  ✓ {cls}")
        else:
            print(f"  ✗ {cls} - MISSING")
    
    # Check enumerations
    print("\nAvailable enumerations:")
    enums = ['NeuronType', 'ActionType', 'MotorCommand', 'WorldObjectType', 
             'DevelopmentalStage', 'FiringState', 'SynapseType']
    for enum in enums:
        if hasattr(pynlm, enum):
            print(f"  ✓ {enum}")
        else:
            print(f"  ✗ {enum} - MISSING")
    
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("Trying to build from source...")
    
    # Try to build the project
    if os.path.exists("build"):
        print("Build directory exists, checking if library is built...")
        
        # Check for shared library
        lib_names = ["libpynlm.so", "libpynlm.dylib", "pynlm.dll"]
        for lib_name in lib_names:
            lib_path = f"build/{lib_name}"
            if os.path.exists(lib_path):
                print(f"Found library: {lib_path}")
                break
    else:
        print("Building project...")
        try:
            # Try to build with CMake
            subprocess.run(["cmake", "-B", "build", "-D", "NLM_BUILD_PYTHON_BINDINGS=ON", "."], 
                         capture_output=True, text=True)
            subprocess.run(["cmake", "--build", "build"], 
                         capture_output=True, text=True)
        except subprocess.CalledProcessError as e:
            print(f"Build failed: {e}")