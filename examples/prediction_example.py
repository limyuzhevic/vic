#!/usr/bin/env python3
"""
Prediction System Example

This example demonstrates NLM's prediction system for anticipating
future events and reducing uncertainty.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'pynlm_enhanced'))

from examples import prediction_example

def main():
    """Run the prediction example."""
    print("NLM Prediction System Example")
    print("=" * 40)
    print()
    
    try:
        prediction_example()
        print("\n✓ Example completed successfully!")
    except Exception as e:
        print(f"\n✗ Example failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()