#!/usr/bin/env python3
"""
Memory System Example

This example demonstrates NLM's memory systems (working memory and episodic memory)
for storing and retrieving information during simulation.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'pynlm_enhanced'))

from examples import memory_system_example

def main():
    """Run the memory system example."""
    print("NLM Memory System Example")
    print("=" * 40)
    print()
    
    try:
        memory_system_example()
        print("\n✓ Example completed successfully!")
    except Exception as e:
        print(f"\n✗ Example failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()