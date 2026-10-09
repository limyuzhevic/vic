#!/usr/bin/env python3
"""
NLM Examples Runner

Utility script to run specific examples or all examples.
"""

import argparse
import sys
import os

def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description="Run NLM example scripts",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python run_examples.py                    Run all examples
  python run_examples.py basic_brain_example      Run basic brain example
  python run_examples.py agent_world_example      Run agent-world example
  python run_examples.py memory_system_example    Run memory system example
  python run_examples.py development_example      Run development example
  python run_examples.py prediction_example       Run prediction example
  python run_examples.py complete_agent_example   Run complete agent example
  python run_examples.py batch_processing_example Run batch processing example
  python run_examples.py configuration_example    Run configuration example
  python run_examples.py event_driven_example     Run event-driven example
  python run_examples.py performance_monitoring_example Run performance monitoring example

All examples are located in the examples/ directory.
        """
    )
    
    parser.add_argument(
        'example',
        nargs='?',
        default='all',
        help='Example to run (default: all)'
    )
    
    args = parser.parse_args()
    
    # Add examples directory to path
    examples_dir = os.path.dirname(os.path.abspath(__file__))
    sys.path.insert(0, examples_dir)
    
    try:
        if args.example == 'all':
            # Run all examples
            from examples import run_all_examples
            run_all_examples()
        else:
            # Run specific example
            example_file = f"{args.example}.py"
            example_path = os.path.join(examples_dir, example_file)
            
            if os.path.exists(example_path):
                # Execute the example file
                with open(example_path, 'r') as f:
                    exec(f.read())
            else:
                print(f"Error: Example '{args.example}' not found")
                print(f"Looking for: {example_path}")
                sys.exit(1)
    
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()