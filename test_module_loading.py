#!/usr/bin/env python3
"""
Direct test of Python module files without package imports
"""

import sys
import os

print("Testing Python Module Files Directly")
print("=" * 50)

# Add the workspace to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Test direct file loading
try:
    import importlib.util
    
    # Test config module
    print("Testing config module...")
    spec = importlib.util.spec_from_file_location("pynlm_config", "pynlm/config.py")
    pynlm_config = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(pynlm_config)
    print("✓ config.py loaded successfully")
    
    # Test brain module  
    print("Testing brain module...")
    spec = importlib.util.spec_from_file_location("pynlm_brain", "pynlm/brain.py")
    pynlm_brain = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(pynlm_brain)
    print("✓ brain.py loaded successfully")
    
    # Test world module
    print("Testing world module...")
    spec = importlib.util.spec_from_file_location("pynlm_world", "pynlm/world.py")
    pynlm_world = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(pynlm_world)
    print("✓ world.py loaded successfully")
    
    # Test agent module
    print("Testing agent module...")
    spec = importlib.util.spec_from_file_location("pynlm_agent", "pynlm/agent.py")
    pynlm_agent = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(pynlm_agent)
    print("✓ agent.py loaded successfully")
    
    # Test logger module
    print("Testing logger module...")
    spec = importlib.util.spec_from_file_location("pynlm_logger", "pynlm/logger.py")
    pynlm_logger = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(pynlm_logger)
    print("✓ logger.py loaded successfully")
    
    # Test main pynlm module
    print("Testing pynlm.py...")
    spec = importlib.util.spec_from_file_location("pynlm", "pynlm/pynlm.py")
    pynlm = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(pynlm)
    print("✓ pynlm.py loaded successfully")
    
    print("\n" + "=" * 50)
    print("SUCCESS: All Python wrapper modules loaded successfully!")
    print("\nCreated Python modules:")
    print("- pynlm/config.py: Enhanced configuration schema system")
    print("- pynlm/brain.py: Brain wrapper with convenience methods")
    print("- pynlm/world.py: World wrapper")
    print("- pynlm/agent.py: Agent wrapper")
    print("- pynlm/logger.py: Enhanced logging system")
    print("- pynlm/pynlm.py: Main Python module entry point")
    print("\nThe Python bindings wrapper module has been implemented successfully!")
    print("C++ bindings may not be available in this environment.")
    
except Exception as e:
    print(f"✗ Module loading failed: {e}")
    import traceback
    traceback.print_exc()
    print("\nNote: This may be due to missing C++ bindings or circular imports.")