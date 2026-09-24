#!/usr/bin/env python3
"""
Main entry point for running NLM examples.

This module provides a unified interface to run all NLM example scripts
in sequence or individually.
"""

import sys
import os
import argparse
import importlib

# Add the examples directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def load_example_module(example_name):
    """Load an example module dynamically."""
    try:
        if example_name == 'basic_brain':
            from examples.example1_basic_brain import Example1_BasicBrain
            return Example1_BasicBrain
        elif example_name == 'agent_world':
            from examples.example2_agent_in_world import Example2_AgentInWorld
            return Example2_AgentInWorld
        elif example_name == 'learning_benchmarks':
            from examples.example3_learning_benchmarks import Example3_LearningBenchmarks
            return Example3_LearningBenchmarks
        elif example_name == 'episodic_memory':
            from examples.example4_episodic_memory import Example4_EpisodicMemory
            return Example4_EpisodicMemory
        elif example_name == 'development':
            from examples.example5_development_and_adaptation import Example5_DevelopmentAndAdaptation
            return Example5_DevelopmentAndAdaptation
        elif example_name == 'configuration':
            from examples.example6_configuration_management import Example6_ConfigurationManagement
            return Example6_ConfigurationManagement
        else:
            raise ValueError(f"Unknown example: {example_name}")
    except ImportError as e:
        raise ImportError(f"Failed to load example '{example_name}': {e}")

def run_single_example(example_name):
    """Run a single example by name."""
    try:
        example_class = load_example_module(example_name)
        
        # Find the run method
        run_method_name = f"run_{example_name.replace('-', '_')}_example"
        if hasattr(example_class, run_method_name):
            run_method = getattr(example_class, run_method_name)
        else:
            # Try alternative naming conventions
            if hasattr(example_class, 'run_example'):
                run_method = example_class.run_example
            elif hasattr(example_class, 'main'):
                run_method = example_class.main
            else:
                raise AttributeError(f"Example {example_name} has no run method")
        
        print(f"Running example: {example_name.replace('-', ' ').title()}")
        print("=" * 60)
        
        result = run_method()
        
        print("=" * 60)
        print(f"Example '{example_name}' completed successfully")
        return True
        
    except Exception as e:
        print(f"\nError running example '{example_name}': {e}")
        import traceback
        traceback.print_exc()
        return False

def run_all_examples():
    """Run all examples in sequence."""
    print("Running all NLM examples in sequence...")
    print("=" * 60)
    
    examples = [
        'basic_brain',
        'agent_world',
        'learning_benchmarks',
        'episodic_memory',
        'development',
        'configuration'
    ]
    
    results = {}
    
    for example_name in examples:
        print(f"\n{'='*60}")
        print(f"Example {example_name.upper()}")
        print(f"{'='*60}")
        
        try:
            result = run_single_example(example_name)
            results[example_name] = result
            
            if result:
                print(f"\n{'='*60}")
                print(f"✓ Example {example_name} completed successfully")
            else:
                print(f"\n{'='*60}")
                print(f"✗ Example {example_name} failed")
                
        except KeyboardInterrupt:
            print(f"\n{'='*60}")
            print(f"⚠ Example {example_name} interrupted by user")
            results[example_name] = False
        except Exception as e:
            print(f"\n{'='*60}")
            print(f"✗ Example {example_name} failed with error: {e}")
            results[example_name] = False
    
    print("\n" + "=" * 60)
    print("SUMMARY")
    print("=" * 60)
    
    success_count = sum(1 for result in results.values() if result)
    total_count = len(results)
    
    print(f"Examples completed: {success_count}/{total_count}")
    
    for example_name, result in results.items():
        status = "✓ SUCCESS" if result else "✗ FAILED"
        print(f"  {example_name}: {status}")
    
    success = success_count == total_count
    if success:
        print(f"\n🎉 All examples completed successfully!")
    else:
        print(f"\n⚠️  Some examples failed. Check output above for details.")
    
    return success

def list_examples():
    """List all available examples with descriptions."""
    examples_info = {
        'basic_brain': {
            'name': 'Basic Brain Simulation',
            'description': 'Simple brain creation and basic simulation without external world',
            'duration': 'Quick (1-2 minutes)',
            'complexity': 'Beginner'
        },
        'agent_world': {
            'name': 'Agent in a 2D World',
            'description': 'Complete agent simulation with world interaction and learning',
            'duration': 'Medium (3-5 minutes)',
            'complexity': 'Intermediate'
        },
        'learning_benchmarks': {
            'name': 'Learning Benchmarks',
            'description': 'Comparative performance testing of different brain configurations',
            'duration': 'Medium (5-7 minutes)',
            'complexity': 'Intermediate'
        },
        'episodic_memory': {
            'name': 'Episodic Memory and Replay',
            'description': 'Demonstration of memory storage, consolidation, and replay',
            'duration': 'Medium (4-6 minutes)',
            'complexity': 'Intermediate'
        },
        'development': {
            'name': 'Development and Adaptation',
            'description': 'Monitoring brain development over time through multiple stages',
            'duration': 'Long (8-10 minutes)',
            'complexity': 'Advanced'
        },
        'configuration': {
            'name': 'Configuration Management',
            'description': 'Experimentation with different brain configurations and parameters',
            'duration': 'Short (2-3 minutes)',
            'complexity': 'Beginner'
        }
    }
    
    print("Available NLM Examples")
    print("=" * 70)
    
    for key, info in examples_info.items():
        print(f"\n{key.upper()}:")
        print(f"  Name: {info['name']}")
        print(f"  Description: {info['description']}")
        print(f"  Duration: {info['duration']}")
        print(f"  Complexity: {info['complexity']}")
    
    print("\n" + "=" * 70)
    print("Usage Examples:")
    print(f"  python examples/examples.py --run basic_brain")
    print(f"  python examples/examples.py --run agent_world")
    print(f"  python examples/examples.py --run all")
    print(f"  python examples/examples.py --list")

def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Examples - Complete Collection of Advanced Usage Examples",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
This module provides a unified interface to run all NLM example scripts
for learning and demonstrating the capabilities of the NLM Python API.

The examples cover the full range of NLM functionality:
- Basic brain simulation and neural computation
- Agent-world interaction and learning
- Performance benchmarking and optimization
- Memory systems and episodic learning
- Brain development and adaptation
- Configuration management and experimentation

Each example is designed to be educational, practical, and self-contained.
""")
    
    parser.add_argument('--list', action='store_true',
                       help='List all available examples with descriptions')
    
    parser.add_argument('--run', type=str, default='all',
                       help='Run specific example (default: all). '
                            'Available: basic_brain, agent_world, learning_benchmarks, '
                            'episodic_memory, development, configuration')
    
    parser.add_argument('--help-all', action='store_true',
                       help='Show help for all examples')
    
    args = parser.parse_args()
    
    if args.list:
        list_examples()
        return 0
    
    if args.help_all:
        print("Help for each example is contained within the individual scripts.")
        print("Run each example individually for detailed documentation.")
        return 0
    
    if not args.run:
        parser.print_help()
        return 1
    
    if args.run == 'all':
        success = run_all_examples()
        return 0 if success else 1
    else:
        success = run_single_example(args.run)
        return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())
