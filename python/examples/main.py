#!/usr/bin/env python3
"""
Command-line interface for NLM Python bindings examples.
"""

import argparse
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from examples.agent_example import main as agent_examples_main
from examples.advanced_examples import main as advanced_examples_main

def main():
    parser = argparse.ArgumentParser(description="NLM Python Bindings Examples")
    parser.add_argument('type', choices=['agent', 'advanced', 'all'],
                       help='Type of examples to run')
    
    args = parser.parse_args()
    
    try:
        if args.type == 'agent':
            agent_examples_main()
        elif args.type == 'advanced':
            advanced_examples_main()
        elif args.type == 'all':
            print("Running all examples...")
            agent_examples_main()
            print("\n" + "="*50 + "\n")
            advanced_examples_main()
        
        print("\nAll examples completed successfully!")
        
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
