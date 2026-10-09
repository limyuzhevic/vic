#!/usr/bin/env python3
"""
Event-Driven Programming Example

This example demonstrates NLM's event-driven programming capabilities,
where you can register callbacks for specific neural events.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'pynlm_enhanced'))

from examples import event_driven_example

def main():
    """Run the event-driven example."""
    print("NLM Event-Driven Programming Example")
    print("=" * 40)
    print()
    
    try:
        event_driven_example()
        print("\n✓ Example completed successfully!")
    except Exception as e:
        print(f"\n✗ Example failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()