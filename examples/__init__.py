#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Advanced Examples

This module provides comprehensive examples for using the NLM Python API.
It includes simple tutorials, advanced demonstrations, and real-world use cases.

Usage:
    python -m examples.advanced [example_name] [options]
    python -m examples.basic [example_name]
    python -m examples.custom [example_file.py]

Examples:
    python -m examples.advanced simple           # Run simple brain example
    python -m examples.advanced agent           # Run agent-world example
    python -m examples.advanced learning         # Run learning session example
    python -m examples.advanced development     # Run development stages example
    python -m examples.advanced config          # Run configuration comparison
    python -m examples.advanced all             # Run all examples
    python -m examples.basic hello              # Run hello world example
    python -m examples.basic visualization      # Run visualization example
"""

import sys
import os
from pathlib import Path
import importlib.util
import argparse

# Add the parent directory to the path so we can import the advanced examples
sys.path.insert(0, str(Path(__file__).parent.parent))

# Try to import the advanced examples
try:
    from advanced.examples import AdvancedNLMExamples
    ADVANCED_AVAILABLE = True
except ImportError:
    ADVANCED_AVAILABLE = False
    print("Warning: Could not import advanced examples. They may not be available.")

def run_basic_examples(example_name):
    """Run basic examples."""
    print(f"Running basic example: {example_name}")
    
    if example_name == "hello":
        print("=== Basic NLM Example: Hello World ===")
        import pynlm
        
        # Create a simple brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Run a simple simulation
        for i in range(10):
            brain.step(i)
            print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")
        
        print("Hello world example completed!\n")
        
    elif example_name == "visualization":
        print("=== Basic NLM Example: Visualization ===")
        import pynlm
        
        # Create a brain with some structure
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.05)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("Brain visualization example:")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
        print(f"  Region count: {brain.getRegionCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
        
        # Run a simulation
        for i in range(50):
            brain.step(i)
            if i % 10 == 0:
                print(f"  Step {i}: {brain.getFiringNeuronCount()} firing neurons")
        
        print("Visualization example completed!\n")
        
    elif example_name == "quick_test":
        print("=== Basic NLM Example: Quick Test ===")
        import pynlm
        
        # Create minimal brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Quick test
        for i in range(20):
            brain.step(i)
        
        print(f"Quick test completed: {brain.getTotalSpikeCount()} total spikes")
        print("Quick test example completed!\n")
        
    else:
        print(f"Unknown basic example: {example_name}")
        print("Available basic examples: hello, visualization, quick_test")
        return False
    
    return True

def run_advanced_examples(example_name):
    """Run advanced examples."""
    if not ADVANCED_AVAILABLE:
        print("Advanced examples are not available.")
        return False
    
    print(f"Running advanced example: {example_name}")
    
    examples = AdvancedNLMExamples()
    
    if example_name == "simple":
        examples.example_simple_brain()
    
    elif example_name == "agent":
        examples.example_agent_world()
    
    elif example_name == "learning":
        examples.example_learning_session()
    
    elif example_name == "development":
        examples.example_development_stages()
    
    elif example_name == "config":
        examples.example_configuration_comparison()
    
    elif example_name == "all":
        examples.run_all_examples()
    
    else:
        print(f"Unknown advanced example: {example_name}")
        print("Available advanced examples: simple, agent, learning, development, config, all")
        return False
    
    return True

def run_custom_example(custom_file):
    """Run a custom example from a file."""
    print(f"Running custom example: {custom_file}")
    
    try:
        # Load and execute the custom example file
        spec = importlib.util.spec_from_file_location("custom_example", custom_file)
        custom_module = importlib.util.module_from_spec(spec)
        
        # Add necessary imports to the module
        custom_module.pynlm = __import__('pynlm')
        custom_module.json = __import__('json')
        custom_module.os = __import__('os')
        
        # Execute the module
        spec.loader.exec_module(custom_module)
        
        print(f"Custom example completed!\n")
        return True
        
    except Exception as e:
        print(f"Error running custom example: {e}")
        return False

def main():
    """Main function to parse arguments and run examples."""
    parser = argparse.ArgumentParser(
        description="NLM Advanced Examples",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        'module',
        choices=['basic', 'advanced', 'custom'],
        help='Example module to run'
    )
    
    parser.add_argument(
        'example',
        nargs='?',
        default='',
        help='Specific example to run'
    )
    
    parser.add_argument(
        '--list',
        action='store_true',
        help='List available examples'
    )
    
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Enable verbose output'
    )
    
    args = parser.parse_args()
    
    if args.list:
        print("Available examples:")
        print("\nBasic examples:")
        print("  hello          - Simple "Hello World" example")
        print("  visualization  - Brain visualization and statistics")
        print("  quick_test     - Quick performance test")
        
        if ADVANCED_AVAILABLE:
            print("\nAdvanced examples:")
            print("  simple         - Simple brain with custom configuration")
            print("  agent          - Complete agent-world interaction")
            print("  learning       - Learning session with checkpointing")
            print("  development    - Development stages and maturation")
            print("  config         - Configuration comparison and optimization")
            print("  all            - Run all advanced examples")
        
        print("\nCustom examples:")
        print("  (provide path to custom Python file)")
        return
    
    if args.module == 'basic':
        if not args.example:
            print("Please specify a basic example to run.")
            print("Use --list to see available examples.")
            return
        success = run_basic_examples(args.example)
    
    elif args.module == 'advanced':
        if not args.example:
            print("Please specify an advanced example to run.")
            print("Use --list to see available examples.")
            return
        success = run_advanced_examples(args.example)
    
    elif args.module == 'custom':
        if not args.example:
            print("Please specify a custom example file.")
            print("Use --list to see available examples.")
            return
        success = run_custom_example(args.example)
    
    if not success:
        sys.exit(1)

if __name__ == "__main__":
    main()