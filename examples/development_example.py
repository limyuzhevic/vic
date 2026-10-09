#!/usr/bin/env python3
"""
Development Process Example

This example demonstrates NLM's developmental system and how brains change
and mature over time through developmental stages.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'pynlm_enhanced'))

from examples import development_example

def main():
    """Run the development example."""
    print("NLM Development Process Example")
    print("=" * 40)
    print()
    
    try:
        development_example()
        print("\n✓ Example completed successfully!")
    except Exception as e:
        print(f"\n✗ Example failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()