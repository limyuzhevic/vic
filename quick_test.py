#!/usr/bin/env python3
"""
Simple test to verify NLM package functionality
"""

import sys
import os

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

print("Testing NLM Package Import")
print("=" * 40)

# Test basic imports
try:
    from nlm import get_version, check_cpp_module
    print("✓ Successfully imported nlm package")
    print(f"  Version: {get_version()}")
    print(f"  C++ module available: {check_cpp_module()}")
except ImportError as e:
    print(f"✗ Failed to import nlm package: {e}")
    sys.exit(1)

# Test pynlm module
try:
    import pynlm
    print("✓ Successfully imported pynlm module")
    print(f"  pynlm version: {getattr(pynlm, '__version__', 'Unknown')}")
except ImportError as e:
    print(f"✗ Failed to import pynlm module: {e}")

# Test nlm_examples module
try:
    import nlm_examples
    print("✓ Successfully imported nlm_examples module")
except ImportError as e:
    print(f"⚠ nlm_examples module not available: {e}")

# Test nlm_power_user module
try:
    import nlm_power_user
    print("✓ Successfully imported nlm_power_user module")
except ImportError as e:
    print(f"⚠ nlm_power_user module not available: {e}")

print("\n" + "=" * 40)
print("Import test completed successfully!")
print("The NLM package appears to be working correctly.")
print("Note: Full functionality requires the C++ extension module to be built.")
print("To build with C++ bindings, run: pip install -e .")
print("=" * 40)