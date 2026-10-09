#!/usr/bin/env python3
"""
Performance Monitoring Example

This example demonstrates NLM's performance monitoring capabilities,
showing how to track and analyze brain performance metrics.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'pynlm_enhanced'))

from examples import performance_monitoring_example

def main():
    """Run the performance monitoring example."""
    print("NLM Performance Monitoring Example")
    print("=" * 40)
    print()
    
    try:
        performance_monitoring_example()
        print("\n✓ Example completed successfully!")
    except Exception as e:
        print(f"\n✗ Example failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()