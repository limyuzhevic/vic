#!/usr/bin/env python3
"""
NLM Basic Examples - Getting Started with Neural Learning Machine

This file contains essential examples for users to understand and use NLM:
- 01_hello_world.py: Basic NLM setup and simulation
- 02_create_brain.py: Brain creation and configuration
- 03_basic_simulation.py: Fundamental simulation concepts
- 04_plasticity_learning.py: Learning mechanisms
- 05_multi_region.py: Multi-region brain organization
- 06_visualization.py: Basic visualization and monitoring
- 07_benchmarking.py: Performance testing
- 08_snake_game.py: Real-world application

This README provides quick access to all examples and explains how to use them.
"""

import os
import subprocess
import sys
from pathlib import Path

def run_example(example_name):
    """Run a specific example"""
    try:
        example_path = Path(f"basic/{example_name}")
        if example_path.exists():
            print(f"Running {example_name}...")
            subprocess.run([sys.executable, str(example_path)], check=True)
        else:
            print(f"Example {example_name} not found at {example_path}")
    except subprocess.CalledProcessError as e:
        print(f"Error running {example_name}: {e}")
    except FileNotFoundError:
        print(f"Python executable not found")

def list_examples():
    """List all available examples"""
    examples_dir = Path("basic")
    if examples_dir.exists():
        print("Available Basic Examples:")
        print("=" * 40)
        for example_file in examples_dir.glob("*.py"):
            example_name = example_file.stem
            print(f"  {example_name}")
        print()
    else:
        print("Basic examples directory not found")

def main():
    """Main function to handle command line arguments"""
    if len(sys.argv) > 1:
        # Run specific example
        example_name = sys.argv[1]
        run_example(example_name)
    else:
        # List all examples
        list_examples()
        
        print("To run an example, use:")
        print(f"  python -m examples.basic.<example_name>")
        print()
        print("Example:")
        print("  python -m examples.basic.01_hello_world")
        print()
        print("All examples demonstrate different aspects of NLM:")
        print("  - Basic simulation and monitoring")
        print("  - Brain creation and configuration")
        print("  - Learning and plasticity mechanisms")
        print("  - Multi-region organization")
        print("  - Visualization and analysis")
        print("  - Performance benchmarking")
        print("  - Real-world applications")

if __name__ == "__main__":
    main()
