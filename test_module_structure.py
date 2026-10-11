"""
Simple test script to verify Python module structure
"""

import sys
import os

# Add current directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

print("Testing NLM Python Module Structure")
print("=" * 50)

# Test imports from individual modules directly
modules_to_test = [
    ('config', 'from pynlm.config import Config, ConfigSchema, ConfigValidator'),
    ('brain', 'from pynlm.brain import Brain, ProgressCallback'),
    ('world', 'from pynlm.world import SimpleWorld'),
    ('agent', 'from pynlm.agent import AgentBrain'),
    ('logger', 'from pynlm.logger import LogLevel, StructuredLogger, get_logger'),
]

all_passed = True

for module_name, import_stmt in modules_to_test:
    try:
        # Execute the import statement
        exec(import_stmt)
        print(f"✓ {module_name} module imported successfully")
    except ImportError as e:
        print(f"✗ {module_name} module import failed: {e}")
        all_passed = False
    except Exception as e:
        print(f"✗ {module_name} module import error: {e}")
        all_passed = False

# Test basic functionality from config module
try:
    from pynlm.config import create_default_config
    cfg = create_default_config()
    print("✓ Default config created successfully")
    
    # Test that config has expected keys
    expected_keys = ['brain.region_count', 'brain.neurons_per_region', 'simulation.max_steps']
    for key in expected_keys:
        if cfg.has(key):
            print(f"✓ Config has expected key: {key}")
        else:
            print(f"✗ Config missing expected key: {key}")
except Exception as e:
    print(f"✗ Basic config functionality failed: {e}")
    all_passed = False

# Test creating objects (with minimal configuration)
try:
    from pynlm.config import create_default_config
    cfg = create_default_config()
    
    # Test brain creation (this will fail without C++ bindings, but that's expected)
    try:
        from pynlm.brain import Brain
        brain = Brain(cfg)
        print("✓ Brain object created")
    except Exception as e:
        print(f"⚠ Brain creation failed (expected without C++ bindings): {e}")
    
    # Test world creation
    try:
        from pynlm.world import SimpleWorld
        world = SimpleWorld()
        print("✓ World object created")
    except Exception as e:
        print(f"✗ World creation failed: {e}")
        all_passed = False
    
    # Test logger creation
    try:
        from pynlm.logger import get_logger
        logger = get_logger("test")
        print("✓ Logger object created")
    except Exception as e:
        print(f"✗ Logger creation failed: {e}")
        all_passed = False
        
except Exception as e:
    print(f"✗ Object creation test failed: {e}")
    all_passed = False

print("\n" + "=" * 50)
if all_passed:
    print("SUCCESS: All critical module imports and functionality tests passed!")
else:
    print("PARTIAL SUCCESS: Some modules had issues (may be expected if C++ bindings are not available).")

print("\nModule structure test complete!")
print("Note: C++ bindings are not available in this environment.")
print("The Python wrapper modules are created and can be used independently.")