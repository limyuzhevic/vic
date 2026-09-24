#!/usr/bin/env python3
"""
NLM Advanced Example Scripts

This directory contains comprehensive example scripts for using the NLM Python API,
demonstrating various use cases from simple to advanced.

Each example is designed to showcase different capabilities:
1. Basic brain simulation
2. Learning agents in environments
3. Advanced neural processing
4. Performance and benchmarking
5. Integration with external systems
"""

import os
import sys

# Version and metadata
__version__ = "1.0.0"
__author__ = "NLM Project"
__license__ = "MIT"

# Make examples easy to discover
EXAMPLES_DIR = os.path.dirname(os.path.abspath(__file__))

# Common utilities that all examples can use
sys.path.insert(0, EXAMPLES_DIR)

# Example entry point
def run_example(example_name):
    """Run a specific example by name."""
    examples = {
        'basic_brain': 'examples.example1_basic_brain:Example1_BasicBrain',
        'agent_world': 'examples.example2_agent_in_world:Example2_AgentInWorld',
        'learning_benchmarks': 'examples.example3_learning_benchmarks:Example3_LearningBenchmarks',
        'episodic_memory': 'examples.example4_episodic_memory:Example4_EpisodicMemory',
        'development': 'examples.example5_development_and_adaptation:Example5_DevelopmentAndAdaptation',
        'configuration': 'examples.example6_configuration_management:Example6_ConfigurationManagement',
        'all': 'examples.examples_main:run_all_examples',
    }
    
    if example_name not in examples:
        print(f"Unknown example: {example_name}")
        print("Available examples:", list(examples.keys()))
        return False
    
    try:
        module_path, class_function = examples[example_name].split(':')
        module = __import__(module_path, fromlist=[''])
        
        if '.' in class_function:
            module_class = getattr(module, class_function.split('.')[0])
            func = getattr(module_class, class_function.split('.')[1])
        else:
            func = getattr(module, class_function)
        
        print(f"Running example: {example_name}")
        print("=" * 60)
        
        result = func()
        
        print("=" * 60)
        print(f"Example '{example_name}' completed successfully")
        return True
        
    except Exception as e:
        print(f"Error running example '{example_name}': {e}")
        import traceback
        traceback.print_exc()
        return False

def list_examples():
    """List all available examples with descriptions."""
    examples_info = {
        'basic_brain': {
            'name': 'Basic Brain Simulation',
            'description': 'Simple brain creation and basic simulation without external world',
            'duration': 'Quick (1-2 minutes)'
        },
        'agent_world': {
            'name': 'Agent in a 2D World',
            'description': 'Complete agent simulation with world interaction and learning',
            'duration': 'Medium (3-5 minutes)'
        },
        'learning_benchmarks': {
            'name': 'Learning Benchmarks',
            'description': 'Comparative performance testing of different brain configurations',
            'duration': 'Medium (5-7 minutes)'
        },
        'episodic_memory': {
            'name': 'Episodic Memory and Replay',
            'description': 'Demonstration of memory storage, consolidation, and replay',
            'duration': 'Medium (4-6 minutes)'
        },
        'development': {
            'name': 'Development and Adaptation',
            'description': 'Monitoring brain development over time through multiple stages',
            'duration': 'Long (8-10 minutes)'
        },
        'configuration': {
            'name': 'Configuration Management',
            'description': 'Experimentation with different brain configurations and parameters',
            'duration': 'Short (2-3 minutes)'
        },
        'all': {
            'name': 'All Examples',
            'description': 'Run every example in sequence with comprehensive output',
            'duration': 'Long (20-25 minutes)'
        }
    }
    
    print("Available NLM Examples")
    print("=" * 60)
    
    for key, info in examples_info.items():
        print(f"\n{key}:")
        print(f"  Name: {info['name']}")
        print(f"  Description: {info['description']}")
        print(f"  Duration: {info['duration']}")
    
    print("\n" + "=" * 60)
    print("Usage Examples:")
    print(f"  python examples/examples.py --run basic_brain")
    print(f"  python examples/examples.py --list")
    print(f"  python examples/examples.py --run all")

def main():
    """Main entry point for the examples module."""
    import argparse
    
    parser = argparse.ArgumentParser(
        description="NLM Advanced Example Scripts",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
This module provides comprehensive example scripts for learning and 
using the NLM Python API. Each example demonstrates different aspects 
of the neural simulation framework.

The examples are designed to be:
- Educational: Teaching key concepts and usage patterns
- Practical: Providing working code for real applications
- Benchmark: Testing performance and capabilities
- Comprehensive: Covering the full range of NLM features
        """)
    
    parser.add_argument('--list', action='store_true',
                       help='List all available examples')
    
    parser.add_argument('--run', type=str, default='all',
                       help='Run specific example (default: all)')
    
    parser.add_argument('--version', action='version',
                       version=f'NLM Examples {__version__}')
    
    args = parser.parse_args()
    
    if args.list:
        list_examples()
        return 0
    
    if not args.run:
        parser.print_help()
        return 1
    
    return 0 if run_example(args.run) else 1

if __name__ == "__main__":
    import sys
    sys.exit(main())
