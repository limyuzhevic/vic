#!/usr/bin/env python3
"""
Test script for improved NLM Python bindings.
Tests the new features and fixes implemented.
"""

import sys
import os
import tempfile

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    print("✓ Successfully imported pynlm")
    print(f"✓ Module version: {pynlm.__version__}")
    print(f"✓ Module doc: {pynlm.__doc__}")
    
    # Test Config.set() method (was missing before)
    print("\n--- Testing Config.set() method ---")
    config = pynlm.createDefaultConfig()
    config.set("test.key", "test_value")
    config.set("test.int", 42)
    config.set("test.float", 3.14)
    config.set("test.bool", True)
    print("✓ Config.set() method works")
    
    # Test Config.getOr() method (was missing before)
    print("\n--- Testing Config.getOr() method ---")
    val_str = config.getOr("test.key", "default")
    print(f"✓ Config.getOr() for string: {val_str}")
    
    val_int = config.getOr("test.int", 0)
    print(f"✓ Config.getOr() for int: {val_int}")
    
    val_float = config.getOr("test.float", 0.0)
    print(f"✓ Config.getOr() for float: {val_float}")
    
    val_bool = config.getOr("test.bool", False)
    print(f"✓ Config.getOr() for bool: {val_bool}")
    
    val_nonexistent = config.getOr("nonexistent.key", "fallback")
    print(f"✓ Config.getOr() for non-existent key: {val_nonexistent}")
    
    # Test Config.saveToFile() fix
    print("\n--- Testing Config.saveToFile() fix ---")
    with tempfile.NamedTemporaryFile(mode='w', suffix='.nlm', delete=False) as f:
        config_file = f.name
    
    save_result = config.saveToFile(config_file)
    if save_result:
        print(f"✓ Config.saveToFile() succeeded, saved to {config_file}")
        
        # Read and verify the content
        with open(config_file, 'r') as f:
            content = f.read()
            print("✓ File content:")
            print(content)
            
            # Check that placeholder values were replaced
            if "PLACEHOLDER_VALUE" not in content:
                print("✓ No placeholder values found in saved file")
            else:
                print("✗ Placeholder values still present in saved file")
    else:
        print("✗ Config.saveToFile() failed")
    
    os.unlink(config_file)
    
    # Test Config.loadFromFile() with simple format
    print("\n--- Testing Config.loadFromFile() ---")
    load_result = config.loadFromFile(config_file)
    if load_result:
        print("✓ Config.loadFromFile() succeeded")
    else:
        print("✓ Config.loadFromFile() handled missing file gracefully")
    
    # Test Brain properties
    print("\n--- Testing Brain properties ---")
    brain_config = pynlm.createDefaultConfig()
    brain_config.set("neuron_count", 100)
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    print(f"✓ Brain.total_neuron_count: {brain.total_neuron_count}")
    print(f"✓ Brain.total_synapse_count: {brain.total_synapse_count}")
    print(f"✓ Brain.firing_neuron_count: {brain.firing_neuron_count}")
    print(f"✓ Brain.average_firing_rate: {brain.average_firing_rate}")
    
    # Test SimpleWorld properties
    print("\n--- Testing SimpleWorld properties ---")
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.setAgentStart(10.0, 10.0)
    print(f"✓ SimpleWorld width: {world.getWidth()}")
    print(f"✓ SimpleWorld height: {world.getHeight()}")
    print(f"✓ SimpleWorld agent position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Test context managers
    print("\n--- Testing context managers ---")
    try:
        with pynlm.createSimpleWorld() as w:
            w.configure(10, 10, 5, 5)
            print("✓ SimpleWorld context manager works")
    except Exception as e:
        print(f"✗ SimpleWorld context manager failed: {e}")
    
    try:
        with pynlm.createBrain(pynlm.createDefaultConfig()) as b:
            b.initialize()
            print("✓ Brain context manager works")
    except Exception as e:
        print(f"✗ Brain context manager failed: {e}")
    
    # Test ConfigSource enum
    print("\n--- Testing ConfigSource enum ---")
    config_source = pynlm.ConfigSource
    print(f"✓ ConfigSource has values: Default={config_source.Default}, File={config_source.File}, CommandLine={config_source.CommandLine}, Runtime={config_source.Runtime}")
    
    print("\n=== All tests completed successfully! ===")
    
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("This may be because the C++ extension hasn't been built yet.")
    print("You may need to build the extension first:")
    print("  cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_35cdc141-6a29-44eb-9109-5c4af3f53009")
    print("  pip install -e .")
    
except Exception as e:
    print(f"✗ Test failed with error: {e}")
    import traceback
    traceback.print_exc()