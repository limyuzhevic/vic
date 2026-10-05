#!/usr/bin/env python3
# NLM Complete Example Suite
# Comprehensive examples for NLM (Neural Learning Machine) Phase 2
# This script provides a complete suite of examples for getting started with NLM

"""
NLM Complete Example Suite
==========================

This is the main entry point for NLM examples. It provides a comprehensive
tutorial on using the NLM framework for neural network simulation.

The examples cover:
1. Basic Usage - Introduction to NLM
2. Environment Integration - Complete agent-world simulation
3. Advanced Features - All NLM subsystems
4. Configuration Management - Saving and loading configurations
5. Performance Testing - Benchmarking NLM
6. Experimentation - Creating and running experiments
7. Visualization - Visualizing NLM simulations
8. Real-world Applications - Practical use cases

Each example is self-contained and can be run independently.
"""

import sys
import os
import argparse
import time
from typing import Dict, Any

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def create_argument_parser():
    """Create and configure the argument parser for the example suite."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Example Suite",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python nlm_examples.py                          # Run all examples
  python nlm_examples.py --example basic          # Run only basic example
  python nlm_examples.py --example advanced --config myconfig.cfg
  python nlm_examples.py --list                  # List all available examples
  python nlm_examples.py --help                  # Show this help message

For more detailed information about NLM, visit:
https://github.com/nlm-project/nlm
"""
    )
    
    parser.add_argument(
        '--example',
        choices=['all', 'basic', 'world', 'advanced', 'performance', 
                'configuration', 'experiments', 'visualization', 'applications'],
        default='all',
        help='Which example to run (default: all)'
    )
    
    parser.add_argument(
        '--config',
        default='configs/default.cfg',
        help='Configuration file to use (default: configs/default.cfg)'
    )
    
    parser.add_argument(
        '--world-width',
        type=int,
        default=20,
        help='Width of the simulated world (default: 20)'
    )
    
    parser.add_argument(
        '--world-height',
        type=int,
        default=20,
        help='Height of the simulated world (default: 20)'
    )
    
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Enable verbose output'
    )
    
    return parser

def log_message(message: str, verbose: bool = False):
    """Log a message with optional verbose flag."""
    if verbose:
        print(f"[NLM Examples] {message}")
    else:
        print(message)

# Import example functions from separate modules
from examples.example_basic import (
    example_basic_usage as run_basic_example,
    example_brain_in_world as run_world_example,
    example_advanced_features as run_advanced_example,
    example_performance_test as run_performance_example,
    example_configuration_export as run_configuration_example,
    example_reinforcement_learning as run_rl_example
)

def run_all_examples(args):
    """Run all examples in the suite."""
    print("=" * 70)
    print("NLM (Neural Learning Machine) Example Suite")
    print("Phase 2: Real Neural Computation")
    print("=" * 70)
    print()
    
    examples_to_run = [
        ("Basic Usage", run_basic_example),
        ("Brain in a Simple World", run_world_example),
        ("Advanced Features", run_advanced_example),
        ("Performance Test", run_performance_example),
        ("Configuration Export/Import", run_configuration_example),
        ("Reinforcement Learning Integration", run_rl_example),
    ]
    
    success_count = 0
    failed_count = 0
    
    for example_name, example_func in examples_to_run:
        print(f"\n{'='*70}")
        print(f"Running: {example_name}")
        print(f"{'='*70}")
        
        try:
            example_func()
            success_count += 1
            print(f"✓ {example_name} completed successfully")
        except Exception as e:
            failed_count += 1
            print(f"✗ {example_name} failed: {e}")
            import traceback
            traceback.print_exc()
    
    print(f"\n{'='*70}")
    print(f"Example Suite Summary:")
    print(f"  Successfully completed: {success_count}")
    print(f"  Failed: {failed_count}")
    print(f"  Total: {success_count + failed_count}")
    print(f"{'='*70}")

def create_example_summary(args):
    """Display a summary of available examples."""
    print("NLM Example Suite - Available Examples")
    print("=" * 70)
    print()
    
    examples = [
        ("basic", "Basic Brain Setup", "Create and initialize a basic NLM brain"),
        ("world", "Brain in a Simple World", "Complete agent-world interaction simulation"),
        ("advanced", "Advanced Features", "Demonstrates all NLM subsystems"),
        ("performance", "Performance Test", "Benchmarks NLM with different configurations"),
        ("configuration", "Configuration Export/Import", "Save and load NLM configurations"),
        ("rl", "Reinforcement Learning Integration", "Integrates NLM with RL concepts"),
    ]
    
    print(f"{'Example Name':<20} {'Description':<50} {'Purpose':<30}")
    print("-" * 100)
    
    for example_key, example_name, example_desc in examples:
        print(f"{example_key:<20} {example_name:<50} {example_desc:<30}")
    
    print("\nTo run a specific example, use:")
    print("  python nlm_examples.py --example <example_name>")
    print("\nFor more information about a specific example, check the individual script.")

def create_custom_config(args):
    """Create a custom configuration based on command line arguments."""
    print(f"\nCreating custom configuration...")
    print(f"  World size: {args.world_width}x{args.world_height}")
    print(f"  Config file: {args.config}")
    print(f"  Verbose: {args.verbose}")
    
    # Load base config
    config = pynlm.createDefaultConfig()
    
    # Customize configuration
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    # Save custom config
    custom_config_file = "custom_config.cfg"
    config.saveToFile(custom_config_file)
    
    print(f"Custom configuration saved to: {custom_config_file}")
    return config

def main():
    """Main function to run the NLM example suite."""
    parser = create_argument_parser()
    args = parser.parse_args()
    
    try:
        if args.example == 'all':
            run_all_examples(args)
        elif args.example == 'list':
            create_example_summary(args)
        else:
            print(f"Running example: {args.example}")
            print("Note: Individual example execution requires the specific example script.")
            print(f"Run: python examples/example_{args.example}.py")
    except KeyboardInterrupt:
        print("\nExecution interrupted by user.")
        sys.exit(1)
    except Exception as e:
        print(f"Error running NLM examples: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()