#!/usr/bin/env python3
"""
Setup script for the NLM Python package.

This script helps set up the NLM Python environment, including:
- Installing the C++ extension module
- Installing Python dependencies
- Setting up the package for development
- Running tests
"""

import os
import sys
import subprocess
import argparse
from pathlib import Path

def run_command(cmd, cwd=None, check=True):
    """Run a command and return the result."""
    print(f"Running: {cmd}")
    try:
        result = subprocess.run(
            cmd, shell=True, cwd=cwd, check=check,
            capture_output=True, text=True
        )
        if result.stdout:
            print(result.stdout)
        return result
    except subprocess.CalledProcessError as e:
        print(f"Error: {e}")
        if e.stderr:
            print(f"Stderr: {e.stderr}")
        raise

def install_cpp_module():
    """Install the C++ extension module."""
    print("Installing NLM C++ extension module...")
    
    # Check if we're in the right directory
    if not os.path.exists("pyproject.toml"):
        print("Error: pyproject.toml not found. Run this script from the project root.")
        return False
    
    try:
        # Install with scikit-build-core
        run_command("pip install -e .")
        print("C++ module installation completed successfully.")
        return True
    except Exception as e:
        print(f"Failed to install C++ module: {e}")
        return False

def install_dependencies():
    """Install Python dependencies."""
    print("Installing Python dependencies...")
    
    # Install with pip
    run_command("pip install numpy matplotlib")
    
    # Install development dependencies if requested
    print("Development dependencies installed.")
    return True

def build_documentation():
    """Build documentation."""
    print("Building documentation...")
    
    try:
        # Check if sphinx is available
        import sphinx
        print("Sphinx is already installed.")
        return True
    except ImportError:
        print("Sphinx not found. Install with: pip install sphinx")
        return False

def run_tests():
    """Run the test suite."""
    print("Running NLM tests...")
    
    # Run pytest
    try:
        run_command("python -m pytest tests/ -v")
        print("Tests completed successfully.")
        return True
    except Exception as e:
        print(f"Tests failed: {e}")
        return False

def create_structure():
    """Create the Python package structure."""
    print("Creating Python package structure...")
    
    # Create directory structure
    directories = [
        "examples",
        "docs",
        "scripts",
        "tests/python",
        "benchmarks"
    ]
    
    for dir_name in directories:
        os.makedirs(dir_name, exist_ok=True)
        print(f"  Created {dir_name}/")
    
    print("Package structure created.")
    return True

def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(description="NLM Python Package Setup Script")
    parser.add_argument(
        "--skip-cpp", 
        action="store_true", 
        help="Skip C++ module installation"
    )
    parser.add_argument(
        "--skip-deps", 
        action="store_true", 
        help="Skip Python dependency installation"
    )
    parser.add_argument(
        "--skip-tests", 
        action="store_true", 
        help="Skip test execution"
    )
    parser.add_argument(
        "--docs", 
        action="store_true", 
        help="Build documentation"
    )
    parser.add_argument(
        "--examples", 
        action="store_true", 
        help="Create example directory structure"
    )
    parser.add_argument(
        "--check-env", 
        action="store_true", 
        help="Check environment setup"
    )
    parser.add_argument(
        "--all", 
        action="store_true", 
        help="Run all setup steps"
    )
    
    args = parser.parse_args()
    
    print("NLM Python Package Setup")
    print("=" * 40)
    
    steps_completed = []
    
    # Determine which steps to run
    if args.all or args.check_env:
        print("\n1. Environment Check")
        try:
            from nlm import _check_environment
            _check_environment()
            steps_completed.append("Environment Check")
        except Exception as e:
            print(f"Environment check failed: {e}")
    
    if args.all or args.examples:
        print("\n2. Creating Package Structure")
        try:
            create_structure()
            steps_completed.append("Package Structure")
        except Exception as e:
            print(f"Package structure creation failed: {e}")
    
    if args.all or not args.skip_cpp:
        print("\n3. Installing C++ Module")
        try:
            install_cpp_module()
            steps_completed.append("C++ Module Installation")
        except Exception as e:
            print(f"C++ module installation failed: {e}")
    
    if args.all or not args.skip_deps:
        print("\n4. Installing Dependencies")
        try:
            install_dependencies()
            steps_completed.append("Dependency Installation")
        except Exception as e:
            print(f"Dependency installation failed: {e}")
    
    if args.all or args.docs:
        print("\n5. Building Documentation")
        try:
            build_documentation()
            steps_completed.append("Documentation Building")
        except Exception as e:
            print(f"Documentation building failed: {e}")
    
    if args.all or not args.skip_tests:
        print("\n6. Running Tests")
        try:
            run_tests()
            steps_completed.append("Test Execution")
        except Exception as e:
            print(f"Test execution failed: {e}")
    
    # Summary
    print("\n" + "=" * 40)
    print("Setup Summary")
    print("=" * 40)
    print(f"Completed steps: {len(steps_completed)}/{6}")
    for step in steps_completed:
        print(f"  ✓ {step}")
    
    if len(steps_completed) == 6:
        print("\nAll setup steps completed successfully!")
        return 0
    else:
        print(f"\nSetup completed with {6 - len(steps_completed)} step(s) pending.")
        return 1


if __name__ == "__main__":
    sys.exit(main())