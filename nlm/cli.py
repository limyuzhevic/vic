#!/usr/bin/env python3
"""
NLM Command Line Interface

This module provides command-line interfaces for various NLM operations,
including package management, example execution, and system configuration.
"""

import argparse
import sys
import os
import time
from pathlib import Path
import pynlm


def main():
    """Main CLI entry point"""
    parser = argparse.ArgumentParser(
        description="NLM - Neural Learning Machine Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-cli --version                    Show version information
  nlm-cli --info                       Show system information
  nlm-cli run-examples                 Run all example scripts
  nlm-cli demo --steps 100             Run a quick demo
  nlm-cli test                        Run the test suite
  nlm-cli build                       Build the package
        """
    )
    
    parser.add_argument(
        '--version',
        action='store_true',
        help='Show version information and exit'
    )
    
    parser.add_argument(
        '--info',
        action='store_true',
        help='Show system information and exit'
    )
    
    parser.add_argument(
        '--examples',
        action='store_true',
        help='Run all example scripts'
    )
    
    parser.add_argument(
        'command',
        nargs='?',
        default='help',
        help='Command to execute (help, run-examples, demo, test, build)'
    )
    
    parser.add_argument(
        '--steps',
        type=int,
        default=100,
        help='Number of simulation steps for demo (default: 100)'
    )
    
    args = parser.parse_args()
    
    if args.version:
        show_version()
        return
        
    if args.info:
        show_info()
        return
        
    if args.examples:
        run_all_examples()
        return
        
    # Execute command
    if args.command == 'help':
        parser.print_help()
    elif args.command == 'run-examples':
        run_all_examples()
    elif args.command == 'demo':
        run_demo(args.steps)
    elif args.command == 'test':
        run_tests()
    elif args.command == 'build':
        build_package()
    else:
        print(f"Unknown command: {args.command}")
        parser.print_help()
        sys.exit(1)


def show_version():
    """Show version information"""
    print(f"NLM Python Package: {pynlm.__version__}")
    print(f"Author: {pynlm.__author__}")
    print(f"Email: {pynlm.__email__}")
    print(f"License: {pynlm.__license__}")
    
    # Get build info
    build_info = pynlm.get_build_info()
    print(f"\nBuild Information:")
    for key, value in build_info.items():
        print(f"  {key}: {value}")


def show_info():
    """Show system information"""
    print("=== NLM System Information ===")
    
    # Get build info
    build_info = pynlm.get_build_info()
    print(f"Platform: {build_info['platform']}")
    print(f"Architecture: {build_info['architecture']}")
    print(f"Python: {build_info['python_version']}")
    print(f"Python implementation: {build_info['python_implementation']}")
    
    # Show available modules
    print(f"\n=== Available Modules ===")
    modules = [
        'brain',
        'world', 
        'agent',
        'config',
        'exceptions',
        'utils',
        'types',
    ]
    
    for module in modules:
        try:
            __import__(f'pynlm.{module}')
            print(f"  ✓ {module}")
        except ImportError:
            print(f"  ✗ {module}")
    
    # Show example scripts
    print(f"\n=== Example Scripts ===")
    examples_dir = Path('examples')
    if examples_dir.exists():
        example_files = list(examples_dir.glob('*.py'))
        for example_file in example_files:
            print(f"  • {example_file.name}")
    else:
        print("  No examples directory found")
    
    # Show configuration validation
    print(f"\n=== Configuration Validation ===")
    from pynlm import validate_config, create_default_config
    
    config = create_default_config()
    config_dict = {
        'neuron_count': config.neuron_count,
        'simulation_timestep': config.simulation_timestep,
        'connection_probability': config.connection_probability,
    }
    
    errors = validate_config(config_dict)
    if errors:
        print(f"  Configuration errors: {len(errors)}")
        for error in errors:
            print(f"    - {error}")
    else:
        print(f"  Configuration: Valid")
        print(f"    Neuron count: {config.neuron_count}")
        print(f"    Simulation timestep: {config.simulation_timestep}s")
        print(f"    Connection probability: {config.connection_probability}")


def run_all_examples():
    """Run all example scripts"""
    print("=== Running All Examples ===")
    
    examples_dir = Path('examples')
    if not examples_dir.exists():
        print("Examples directory not found!")
        return
    
    example_files = sorted(examples_dir.glob('*.py'))
    
    for example_file in example_files:
        print(f"\n{'='*60}")
        print(f"Running {example_file.name}")
        print(f"{'='*60}")
        
        try:
            # Execute the example script
            with open(example_file, 'r') as f:
                code = f.read()
            
            # Create namespace for execution
            namespace = {
                '__name__': '__main__',
                '__file__': str(example_file),
                'pynlm': pynlm,
                'time': time,
                'os': os,
            }
            
            # Execute the code
            exec(code, namespace)
            
            print(f"✓ {example_file.name} completed successfully")
            
        except Exception as e:
            print(f"✗ Error running {example_file.name}: {e}")
            import traceback
            traceback.print_exc()
    
    print(f"\n{'='*60}")
    print(f"All examples completed!")
    print(f"{'='*60}")


def run_demo(steps=100):
    """Run a quick demo"""
    print("=== NLM Quick Demo ===")
    print(f"Running simulation for {steps} steps...")
    
    try:
        # Create a simple brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Run simulation
        for step in range(steps):
            brain.step(step)
            
            # Print progress every 25 steps
            if (step + 1) % 25 == 0:
                print(f"Step {step + 1:4d}/{steps}: "
                      f"Firing rate: {brain.getAverageFiringRate():6.2f} Hz, "
                      f"Total spikes: {brain.getTotalSpikeCount():6d}")
        
        # Print final results
        print(f"\n=== Demo Results ===")
        print(f"Total simulation steps: {steps}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Active neurons: {brain.getActiveNeuronCount()}")
        print(f"Firing neurons: {brain.getFiringNeuronCount()}")
        
        print(f"\n✓ Demo completed successfully!")
        
    except Exception as e:
        print(f"✗ Demo failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


def run_tests():
    """Run the test suite"""
    print("=== Running Test Suite ===")
    
    try:
        import pytest
        
        # Run pytest with default options
        result = pytest.main([
            'tests/',
            '-v',
            '--tb=short',
            '--cov=pynlm',
            '--cov-report=term-missing'
        ])
        
        if result == 0:
            print("\n✓ All tests passed!")
        else:
            print(f"\n✗ Tests failed with exit code: {result}")
            sys.exit(1)
            
    except ImportError:
        print("pytest not installed. Installing...")
        import subprocess
        subprocess.check_call([sys.executable, '-m', 'pip', 'install', 'pytest'])
        run_tests()


def build_package():
    """Build the package"""
    print("=== Building NLM Package ===")
    
    try:
        import subprocess
        
        # Build the package
        print("Running setup.py sdist bdist_wheel...")
        
        # Build source distribution
        print("Building source distribution...")
        subprocess.check_call([sys.executable, 'setup.py', 'sdist'])
        
        # Build wheel
        print("Building wheel...")
        subprocess.check_call([sys.executable, 'setup.py', 'bdist_wheel'])
        
        print(f"\n✓ Package built successfully!")
        print(f"  Source distribution: dist/pynlm-*.tar.gz")
        print(f"  Wheel: dist/pynlm-*.whl")
        
    except Exception as e:
        print(f"✗ Build failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == '__main__':
    main()
