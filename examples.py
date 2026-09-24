#!/usr/bin/env python3
"""
NLM Examples - Complete Collection of Advanced Usage Examples

This file provides access to all NLM example scripts for learning and
benchmarking purposes.

Usage:
    python examples.py                    # Run all examples
    python examples.py example1              # Run only example 1
    python examples.py --list                # List all available examples
"""

import sys
import os
import argparse

# Add the parent directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import all example classes
from examples.example1_basic_brain import Example1_BasicBrain
from examples.example2_agent_in_world import Example2_AgentInWorld
from examples.example3_learning_benchmarks import Example3_LearningBenchmarks
from examples.example4_episodic_memory import Example4_EpisodicMemory
from examples.example5_development_and_adaptation import Example5_DevelopmentAndAdaptation
from examples.example6_configuration_management import Example6_ConfigurationManagement

# Map example names to functions
EXAMPLES = {
    'example1': ('Basic Brain Simulation', Example1_BasicBrain.run_example),
    'example2': ('Agent in a 2D World', Example2_AgentInWorld.run_example),
    'example3': ('Learning Benchmarks', Example3_LearningBenchmarks.run_learning_benchmarks),
    'example4': ('Episodic Memory', Example4_EpisodicMemory.run_episodic_memory_example),
    'example5': ('Development and Adaptation', Example5_DevelopmentAndAdaptation.run_development_example),
    'example6': ('Configuration Management', Example6_ConfigurationManagement.run_configuration_example),
}

def list_examples():
    """List all available examples."""
    print("Available NLM Examples:")
    print("=" * 60)
    for key, (name, _) in EXAMPLES.items():
        print(f"{key}: {name}")
    print("=" * 60)
    print("\nUsage: python examples.py example_key")

def run_example(key):
    """Run a specific example."""
    if key not in EXAMPLES:
        print(f"Error: Unknown example '{key}'")
        print("\nUse --list to see all available examples")
        return False
    
    name, example_func = EXAMPLES[key]
    print(f"\n{'='*60}")
    print(f"Running: {name}")
    print(f"{'='*60}")
    
    try:
        example_func()
        print(f"\n{'='*60}")
        print(f"Example completed: {name}")
        print(f"{'='*60}")
        return True
    except Exception as e:
        print(f"\nError running example '{name}': {e}")
        import traceback
        traceback.print_exc()
        return False

def run_all_examples():
    """Run all examples in sequence."""
    print("Running all NLM examples in sequence...")
    print("=" * 60)
    
    results = {}
    
    for key, (name, example_func) in EXAMPLES.items():
        print(f"\n{'='*60}")
        print(f"Example {key}: {name}")
        print(f"{'='*60}")
        
        try:
            result = example_func()
            results[key] = True if result is None else result
            print(f"\n{'='*60}")
            print(f"Example {key} completed successfully")
            print(f"{'='*60}")
        except Exception as e:
            print(f"\n{'='*60}")
            print(f"Error in example {key}: {e}")
            print(f"{'='*60}")
            results[key] = False
    
    print("\n" + "=" * 60)
    print("SUMMARY")
    print("=" * 60)
    
    success_count = sum(1 for result in results.values() if result)
    total_count = len(results)
    
    print(f"Examples completed: {success_count}/{total_count}")
    
    for key, result in results.items():
        status = "SUCCESS" if result else "FAILED"
        name = EXAMPLES[key][0]
        print(f"  {key}: {name} - {status}")
    
    return success_count == total_count

def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Examples - Advanced Usage Examples",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python examples.py                    # Run all examples
  python examples.py example1              # Run only example 1
  python examples.py --list                # List all available examples
  
The examples showcase various aspects of the NLM Python API:
  - Basic brain simulation and configuration
  - Agent-world interaction and learning
  - Performance benchmarking
  - Memory systems and consolidation
  - Brain development and adaptation
  - Configuration management and experimentation
        """)
    
    parser.add_argument('example', nargs='?', default='all',
                       help='Example to run (default: all)')
    
    parser.add_argument('--list', action='store_true',
                       help='List all available examples')
    
    args = parser.parse_args()
    
    if args.list:
        list_examples()
        return 0
    
    if args.example == 'all':
        return 0 if run_all_examples() else 1
    else:
        success = run_example(args.example)
        return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())
