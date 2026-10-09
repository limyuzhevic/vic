#!/usr/bin/env python3
"""
Batch Processing Example

This example demonstrates how to run multiple simulations in parallel
using NLM's batch processing capabilities.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'pynlm_enhanced'))

from examples import batch_processing_example

def main():
    """Run the batch processing example."""
    print("NLM Batch Processing Example")
    print("=" * 40)
    print()
    
    try:
        batch_processing_example()
        print("\n✓ Example completed successfully!")
    except Exception as e:
        print(f"\n✗ Example failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()