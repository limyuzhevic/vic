#!/usr/bin/env python3
"""
Basic Brain Simulation Example

This example demonstrates the simplest possible NLM usage:
creating a brain and running a basic simulation.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'pynlm_enhanced'))

from examples import basic_brain_example

def main():
    """Run the basic brain example."""
    print("NLM Basic Brain Example")
    print("=" * 40)
    print()
    
    try:
        basic_brain_example()
        print("\n✓ Example completed successfully!")
    except Exception as e:
        print(f"\n✗ Example failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()