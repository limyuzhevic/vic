#!/usr/bin/env python3
"""
Configuration Management Example

This example demonstrates NLM's advanced configuration system,
showing how to create custom configurations and presets.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'pynlm_enhanced'))

from examples import configuration_example

def main():
    """Run the configuration example."""
    print("NLM Configuration Management Example")
    print("=" * 40)
    print()
    
    try:
        configuration_example()
        print("\n✓ Example completed successfully!")
    except Exception as e:
        print(f"\n✗ Example failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()