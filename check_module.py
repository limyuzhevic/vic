#!/usr/bin/env python3
"""
Test script to check if pynlm module is properly built and working.
"""

import sys
import os

# Add the python directory to the Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

print("Testing pynlm module import...")

try:
    import pynlm
    print("✓ SUCCESS: pynlm module imported successfully")
    print(f"  Module file: {pynlm.__file__}")
    
    # Check basic functionality
    config = pynlm.createDefaultConfig()
    print(f"✓ Config created: {config}")
    
    # Test basic operations
    config.set("test_key", "test_value")
    print(f"✓ Config set/get: {config.get('test_key')}")
    
    # Test Pythonic features
    print(f"✓ Config __len__: {len(config)}")
    print(f"✓ Config __str__: {str(config)}")
    print(f"✓ Config __repr__: {repr(config)}")
    
    # Test ID classes
    neuron_id = pynlm.NeuronId(123)
    print(f"✓ NeuronId created: {neuron_id}, value: {neuron_id.value}")
    
    # Test other classes
    world = pynlm.createSimpleWorld()
    print(f"✓ SimpleWorld created: {world}")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"✓ Brain created and initialized: {brain}")
    
    agent = pynlm.createAgentBrain(brain)
    print(f"✓ AgentBrain created: {agent}")
    
    print("\n✅ All basic tests passed!")
    
except ImportError as e:
    print(f"❌ ERROR: Could not import pynlm: {e}")
    print("\nTrying to build pynlm module...")
    
    # Try to build the module
    try:
        # Check if we can run cmake
        import subprocess
        import tempfile
        
        # Create a build directory
        build_dir = os.path.join(os.path.dirname(__file__), "build")
        os.makedirs(build_dir, exist_ok=True)
        
        print(f"Creating build directory: {build_dir}")
        
        # Try to run cmake
        cmake_cmd = ["cmake", "..", "-DCMAKE_BUILD_TYPE=Release"]
        
        print(f"Running CMake: {' '.join(cmake_cmd)}")
        result = subprocess.run(cmake_cmd, cwd=build_dir, capture_output=True, text=True)
        
        if result.returncode == 0:
            print("✓ CMake completed successfully")
            
            # Try to run make
            make_cmd = ["make", "-j4"]
            print(f"Running Make: {' '.join(make_cmd)}")
            result = subprocess.run(make_cmd, cwd=build_dir, capture_output=True, text=True)
            
            if result.returncode == 0:
                print("✓ Make completed successfully")
                
                # Try to import again
                print("\nTrying to import pynlm again...")
                import importlib
                importlib.reload(pynlm)
                print("✓ pynlm module reloaded successfully")
            else:
                print(f"❌ Make failed: {result.stderr}")
        else:
            print(f"❌ CMake failed: {result.stderr}")
            
    except Exception as e:
        print(f"❌ Build failed: {e}")
        import traceback
        traceback.print_exc()
        
except Exception as e:
    print(f"❌ ERROR: {e}")
    import traceback
    traceback.print_exc()

print("\nTest completed.")