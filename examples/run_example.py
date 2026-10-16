#!/usr/bin/env python3
"""Main entry point for NLM examples.

This script provides access to all NLM Python example demonstrations.
It allows users to run specific examples or see all available demonstrations.
"""

import pynlm
import sys
import os
import importlib.util

def list_examples():
    """List all available example demonstrations."""
    examples = [
        ("basic_usage.py", "Basic brain creation and usage"),
        ("plasticity_demo.py", "STDP (Spike-Timing-Dependent Plasticity) learning"),
        ("sensory_demo.py", "Working with different sensory inputs"),
        ("simulation_demo.py", "Complete simulation with world interaction"),
        ("model_persistence.py", "Saving and loading brain models"),
        ("custom_config.py", "Custom configuration setup"),
        ("memory_demo.py", "Working memory capabilities"),
        ("neuromodulation_demo.py", "Neuromodulation features (dopamine, curiosity, etc.)"),
        ("development_demo.py", "Developmental stages and maturation"),
    ]
    
    print("=== Available NLM Python Examples ===\n")
    
    for i, (filename, description) in enumerate(examples, 1):
        print(f"{i}. {filename}")
        print(f"   {description}")
        print()
    
    print("Usage: python run_example.py <example_number>|<example_name>")
    print("Example: python run_example.py plasticity_demo")
    print("Example: python run_example.py 3")

def run_example_by_name(example_name):
    """Run an example by its name."""
    examples_dir = os.path.join(os.path.dirname(__file__), 'examples')
    
    # Map example names to files
    example_files = {
        'basic_usage': 'basic_usage.py',
        'plasticity_demo': 'plasticity_demo.py',
        'sensory_demo': 'sensory_demo.py',
        'simulation_demo': 'simulation_demo.py',
        'model_persistence': 'model_persistence.py',
        'custom_config': 'custom_config.py',
        'memory_demo': 'memory_demo.py',
        'neuromodulation_demo': 'neuromodulation_demo.py',
        'development_demo': 'development_demo.py',
        'all': '__main__'  # Special case to run all
    }
    
    if example_name not in example_files:
        print(f"✗ Unknown example: {example_name}")
        print(f"Available examples: {', '.join(example_files.keys())}")
        return False
    
    filename = example_files[example_name]
    filepath = os.path.join(examples_dir, filename)
    
    if not os.path.exists(filepath):
        print(f"✗ Example file not found: {filepath}")
        return False
    
    print(f"\n=== Running {filename} ===\n")
    
    try:
        # Execute the example file
        with open(filepath, 'r') as f:
            code = f.read()
        
        # Create namespace and execute
        namespace = {
            '__name__': '__main__',
            '__file__': filepath,
            'pynlm': pynlm,
            'sys': sys,
            'os': os,
            'importlib': importlib,
            'math': __import__('math'),
            'random': __import__('random'),
        }
        
        exec(code, namespace)
        
        print(f"\n✓ Successfully completed {filename}")
        return True
        
    except Exception as e:
        print(f"✗ Error running {filename}: {e}")
        import traceback
        traceback.print_exc()
        return False

def run_example_by_number(example_number):
    """Run an example by its number."""
    examples = [
        ("basic_usage.py", "basic_usage"),
        ("plasticity_demo.py", "plasticity_demo"),
        ("sensory_demo.py", "sensory_demo"),
        ("simulation_demo.py", "simulation_demo"),
        ("model_persistence.py", "model_persistence"),
        ("custom_config.py", "custom_config"),
        ("memory_demo.py", "memory_demo"),
        ("neuromodulation_demo.py", "neuromodulation_demo"),
        ("development_demo.py", "development_demo"),
    ]
    
    if example_number < 1 or example_number > len(examples):
        print(f"✗ Invalid example number: {example_number}")
        print(f"Please choose a number between 1 and {len(examples)}")
        return False
    
    filename, example_name = examples[example_number - 1]
    return run_example_by_name(example_name)

def run_all_examples():
    """Run all examples in sequence."""
    examples_dir = os.path.join(os.path.dirname(__file__), 'examples')
    example_files = [f for f in os.listdir(examples_dir) if f.endswith('.py') and f != '__init__.py']
    
    print("=== Running All NLM Examples ===\n")
    
    success_count = 0
    total_count = len(example_files)
    
    for filename in sorted(example_files):
        if filename.startswith('__'):
            continue
            
        print(f"Running {filename}...")
        
        filepath = os.path.join(examples_dir, filename)
        
        try:
            with open(filepath, 'r') as f:
                code = f.read()
            
            namespace = {
                '__name__': '__main__',
                '__file__': filepath,
                'pynlm': pynlm,
                'sys': sys,
                'os': os,
                'importlib': importlib,
                'math': __import__('math'),
                'random': __import__('random'),
            }
            
            exec(code, namespace)
            print(f"✓ {filename} completed successfully\n")
            success_count += 1
            
        except Exception as e:
            print(f"✗ {filename} failed: {e}\n")
    
    print(f"=== Summary ===")
    print(f"Completed: {success_count}/{total_count} examples")
    
    if success_count == total_count:
        print("✓ All examples completed successfully!")
    else:
        print(f"⚠ {total_count - success_count} examples failed")
    
    return success_count == total_count

def main():
    """Main entry point."""
    # Check if NLM is available
    try:
        import pynlm
        print(f"NLM Python bindings version: {getattr(pynlm, '__version__', 'Unknown')}")
    except ImportError:
        print("✗ NLM Python bindings not available")
        print("Please install NLM: pip install pynlm")
        sys.exit(1)
    
    # Parse command line arguments
    if len(sys.argv) == 1:
        # No arguments: show help
        list_examples()
        
    elif len(sys.argv) == 2:
        # One argument: could be example name or number
        arg = sys.argv[1]
        
        # Check if it's a number
        if arg.isdigit():
            example_number = int(arg)
            success = run_example_by_number(example_number)
        else:
            # Treat as example name
            success = run_example_by_name(arg)
        
        sys.exit(0 if success else 1)
    
    elif sys.argv[1] == '--all':
        # Run all examples
        success = run_all_examples()
        sys.exit(0 if success else 1)
    
    else:
        print("Usage: python run_example.py [example_number|example_name| --all]")
        print("\nExamples:")
        print("  python run_example.py                    # Show all examples")
        print("  python run_example.py 3                    # Run example #3")
        print("  python run_example.py simulation_demo      # Run by name")
        print("  python run_example.py --all               # Run all examples")
        sys.exit(1)

if __name__ == "__main__":
    main()
