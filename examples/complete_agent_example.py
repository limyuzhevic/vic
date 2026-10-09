#!/usr/bin/env python3
"""
Complete Agent Example

This example demonstrates a complete agent setup using all advanced
NLM features including neuromodulation, memory, and prediction.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'pynlm_enhanced'))

from examples import complete_agent_example

def main():
    """Run the complete agent example."""
    print("NLM Complete Agent Example")
    print("=" * 40)
    print()
    
    try:
        complete_agent_example()
        print("\n✓ Example completed successfully!")
    except Exception as e:
        print(f"\n✗ Example failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()