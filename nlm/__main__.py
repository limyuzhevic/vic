#!/usr/bin/env python3
"""
NLM Command Line Interface
===========================

Command-line interface for the NLM (Neural Learning Machine) Python package.
Provides various commands for running examples, checking the environment,
and managing the NLM simulation framework.
"""

import sys
import argparse
import subprocess
from pathlib import Path

def run_example(command_args):
    """Run an example simulation."""
    try:
        # Import and run the example
        from nlm_examples import (
            run_silent_brain_example,
            run_agent_world_example,
            run_learning_demonstration,
            run_curiosity_example,
            run_plasticity_demo
        )
        
        example_functions = {
            'silent': run_silent_brain_example,
            'agent-world': run_agent_world_example,
            'learning': run_learning_demonstration,
            'curiosity': run_curiosity_example,
            'plasticity': run_plasticity_demo,
            'all': None  # Special case for all examples
        }
        
        if command_args.example == 'all':
            print("Running all examples...")
            from nlm_examples import run_all_examples
            results = run_all_examples()
            print("\nAll examples completed!")
            return results
        else:
            func = example_functions[command_args.example]
            print(f"Running {command_args.example} example...")
            results = func()
            print(f"\n{example_args.example.title()} example completed successfully!")
            return results
            
    except ImportError as e:
        print(f"Error importing example modules: {e}")
        return None
    except Exception as e:
        print(f"Error running example: {e}")
        return None

def check_environment():
    """Check the NLM environment setup."""
    print("Checking NLM Environment Setup")
    print("=" * 40)
    
    checks = []
    
    # Check if NLM module is available
    try:
        import nlm
        print("✓ NLM package imported successfully")
        checks.append(True)
        
        # Check if C++ module is available
        from nlm import check_cpp_module
        if check_cpp_module():
            print("✓ C++ extension module available")
            checks.append(True)
        else:
            print("✗ C++ extension module not available")
            checks.append(False)
            
        # Check version
        version = nlm.get_version()
        print(f"✓ NLM version: {version}")
        checks.append(True)
        
    except ImportError as e:
        print(f"✗ NLM package import failed: {e}")
        checks.append(False)
    
    # Check dependencies
    deps_to_check = ['numpy', 'matplotlib']
    for dep in deps_to_check:
        try:
            __import__(dep)
            print(f"✓ {dep} available")
            checks.append(True)
        except ImportError:
            print(f"✗ {dep} not available")
            checks.append(False)
    
    # Summary
    print("\n" + "=" * 40)
    passed = sum(checks)
    total = len(checks)
    print(f"Environment Check: {passed}/{total} checks passed")
    
    if passed == total:
        print("✓ All checks passed!")
        return True
    else:
        print("✗ Some checks failed. Please fix the issues above.")
        return False

def run_demo():
    """Run a quick demo of NLM capabilities."""
    print("Running NLM Demo")
    print("=" * 40)
    
    try:
        import nlm
        
        # Create a simple demonstration
        print("Creating a simple neural network...")
        brain = nlm.create_brain()
        brain.initialize()
        
        print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
        print(f"Brain has {brain.getTotalSynapseCount()} synapses")
        
        # Run a few steps
        print("\nRunning simulation for 10 steps...")
        for i in range(10):
            brain.step(i)
            if i % 5 == 0:
                print(f"  Step {i}: {brain.getFiringNeuronCount()} firing neurons")
        
        print(f"\nFinal statistics:")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
        
        print("\nDemo completed successfully!")
        return True
        
    except Exception as e:
        print(f"Demo failed: {e}")
        return False

def build_documentation():
    """Build documentation."""
    print("Building Documentation")
    print("=" * 40)
    
    try:
        # Check if sphinx is available
        import sphinx
        
        print("Sphinx is available. Building documentation...")
        # In a real implementation, we would run sphinx-build here
        print("Documentation building would be run here in a full implementation.")
        print("To build documentation, run: sphinx-build docs/ docs/_build/")
        
        return True
        
    except ImportError:
        print("Sphinx not available. Install with: pip install sphinx")
        return False

def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm check    # Check environment setup
  nlm demo     # Run a quick demo
  nlm example silent    # Run silent brain example
  nlm example all      # Run all examples
  nlm docs     # Build documentation
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Check command
    check_parser = subparsers.add_parser("check", help="Check environment setup")
    check_parser.set_defaults(func=check_environment)
    
    # Demo command
    demo_parser = subparsers.add_parser("demo", help="Run a quick demo")
    demo_parser.set_defaults(func=run_demo)
    
    # Documentation command
    docs_parser = subparsers.add_parser("docs", help="Build documentation")
    docs_parser.set_defaults(func=build_documentation)
    
    # Examples command
    examples_parser = subparsers.add_parser(
        "example", 
        help="Run examples",
        description="Run example simulations"
    )
    examples_parser.add_argument(
        "example",
        choices=['silent', 'agent-world', 'learning', 'curiosity', 'plasticity', 'all'],
        help="Example to run"
    )
    examples_parser.set_defaults(func=run_example)
    
    # Parse arguments
    args = parser.parse_args()
    
    if not hasattr(args, 'func'):
        parser.print_help()
        return 1
    
    # Run the requested command
    try:
        result = args.func(args)
        if result is not None:
            return 0 if result else 1
        return 0
    except KeyboardInterrupt:
        print("\nOperation cancelled by user.")
        return 1
    except Exception as e:
        print(f"Error: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())