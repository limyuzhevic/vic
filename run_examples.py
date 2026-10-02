#!/usr/bin/env python3
"""
Quick start script to run all NLM examples.
Run with: python run_examples.py
"""

import subprocess
import sys
import os

def run_example(script_path, description):
    """Run a single example script."""
    print(f"\n{'='*60}")
    print(f"Running: {description}")
    print(f"Script: {script_path}")
    print(f"{'='*60}")
    
    try:
        result = subprocess.run([sys.executable, script_path], 
                              capture_output=True, text=True, timeout=60)
        
        if result.returncode == 0:
            print("SUCCESS")
            print(result.stdout)
        else:
            print("FAILED")
            print("STDOUT:", result.stdout)
            print("STDERR:", result.stderr)
            
    except subprocess.TimeoutExpired:
        print("TIMEOUT (example took too long)")
    except FileNotFoundError:
        print(f"ERROR: Script not found: {script_path}")
    except Exception as e:
        print(f"ERROR: {e}")

def main():
    """Run all examples in the examples directory."""
    examples_dir = os.path.join(os.path.dirname(__file__), "examples")
    
    if not os.path.exists(examples_dir):
        print(f"ERROR: Examples directory not found: {examples_dir}")
        return
    
    print("NLM Examples Quick Runner")
    print("=" * 40)
    print(f"Found examples directory: {examples_dir}")
    
    # Run basic examples
    basic_dir = os.path.join(examples_dir, "basic")
    if os.path.exists(basic_dir):
        print(f"\nRunning basic examples from {basic_dir}")
        for file in sorted(os.listdir(basic_dir)):
            if file.endswith(".py"):
                run_example(os.path.join(basic_dir, file), f"Basic Example: {file}")
    
    # Run intermediate examples
    intermediate_dir = os.path.join(examples_dir, "intermediate")
    if os.path.exists(intermediate_dir):
        print(f"\nRunning intermediate examples from {intermediate_dir}")
        for file in sorted(os.listdir(intermediate_dir)):
            if file.endswith(".py"):
                run_example(os.path.join(intermediate_dir, file), f"Intermediate Example: {file}")
    
    # Run advanced examples
    advanced_dir = os.path.join(examples_dir, "advanced")
    if os.path.exists(advanced_dir):
        print(f"\nRunning advanced examples from {advanced_dir}")
        for file in sorted(os.listdir(advanced_dir)):
            if file.endswith(".py"):
                run_example(os.path.join(advanced_dir, file), f"Advanced Example: {file}")
    
    print(f"\n{'='*60}")
    print("Example runner completed!")
    print(f"{'='*60}")

if __name__ == "__main__":
    main()