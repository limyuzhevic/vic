#!/usr/bin/env python3
"""
Setup script for NLM Python package.
This script helps prepare the environment for building and testing Python bindings.
"""
import subprocess
import sys
import os
from pathlib import Path

def run_command(cmd, cwd=None):
    """Run a command and return (success, output)"""
    print(f"Running: {cmd}")
    try:
        result = subprocess.run(
            cmd, shell=True, cwd=cwd, capture_output=True, text=True
        )
        if result.returncode != 0:
            print(f"Error: {result.stderr}")
            return False, result.stderr
        return True, result.stdout
    except Exception as e:
        return False, str(e)

def check_python_dependencies():
    """Check if required Python packages are installed"""
    print("Checking Python dependencies...")
    
    required = ["pybind11", "scikit-build-core", "numpy", "pytest"]
    missing = []
    
    for package in required:
        try:
            __import__(package.replace("-", "_").replace(".", ""))
            print(f"  ✓ {package} is installed")
        except ImportError:
            print(f"  ✗ {package} is missing")
            missing.append(package)
    
    return missing

def check_cmake():
    """Check if CMake is available"""
    print("Checking CMake availability...")
    
    success, output = run_command("cmake --version")
    if success:
        print("  ✓ CMake is available")
        # Extract version
        version_line = output.split('\n')[0]
        print(f"  {version_line}")
        return True
    else:
        print("  ✗ CMake is not available")
        return False

def check_compiler():
    """Check for C++ compiler"""
    print("Checking C++ compiler...")
    
    # Try g++
    success, output = run_command("g++ --version")
    if success:
        print("  ✓ g++ compiler available")
        return True
    
    # Try clang++
    success, output = run_command("clang++ --version")
    if success:
        print("  ✓ clang++ compiler available")
        return True
    
    print("  ✗ No C++ compiler found")
    return False

def create_build_structure():
    """Create build directory structure"""
    print("Creating build directory structure...")
    
    # Create build directory
    build_dir = Path("build")
    if not build_dir.exists():
        build_dir.mkdir()
        print(f"  Created: {build_dir}")
    
    # Create build subdirectory for Python
    python_build = build_dir / "python"
    if not python_build.exists():
        python_build.mkdir()
        print(f"  Created: {python_build}")
    
    return build_dir

def install_dependencies():
    """Install Python dependencies"""
    print("Installing Python dependencies...")
    
    # Install packages
    packages = ["pybind11", "scikit-build-core", "numpy", "pytest"]
    success, output = run_command(f"pip install {' '.join(packages)}")
    
    if success:
        print("  ✓ Dependencies installed successfully")
        return True
    else:
        print("  ✗ Failed to install dependencies")
        return False

def build_python_bindings(build_dir):
    """Build Python bindings"""
    print("Building Python bindings...")
    
    # Set environment variables for building
    env = os.environ.copy()
    env["PYTHONPATH"] = str(build_dir / "python")
    
    # Build with scikit-build-core
    cmd = "pip install -e . --no-build-isolation"
    success, output = run_command(cmd, cwd=build_dir.parent)
    
    if success:
        print("  ✓ Python bindings built successfully")
        return True
    else:
        print("  ✗ Failed to build Python bindings")
        return False

def run_tests():
    """Run NLM tests"""
    print("Running NLM tests...")
    
    # Check if we have a build directory
    build_dir = Path("build")
    if not build_dir.exists():
        print("  ✗ Build directory not found")
        return False
    
    # Try to run the test executable
    test_exe = build_dir / "nlm_test"
    if test_exe.exists():
        print(f"  Running: {test_exe}")
        success, output = run_command(str(test_exe))
        
        if success:
            print("  ✓ Tests passed")
            return True
        else:
            print("  ✗ Tests failed")
            return False
    else:
        print(f"  ✗ Test executable not found: {test_exe}")
        return False

def print_help():
    """Print help information"""
    print("""
NLM Python Setup Script
=======================

This script helps prepare the environment for building and testing
NLM Python bindings.

Usage:
    python setup.py [options]

Options:
    --check-only      Only check dependencies, don't install/build
    --install-only   Only install Python dependencies
    --build-only     Only build Python bindings
    --test-only      Only run tests (assumes bindings are built)
    --help, -h       Show this help message

Examples:
    python setup.py                    # Full setup
    python setup.py --check-only        # Check only
    python setup.py --install-only      # Install only
    python setup.py --build-only        # Build only

Environment:
    This project uses scikit-build-core to build Python bindings
    for the NLM C++ neural simulation framework.
""")

def main():
    """Main setup function"""
    import argparse
    
    parser = argparse.ArgumentParser(description="Setup NLM Python bindings")
    parser.add_argument("--check-only", action="store_true",
                       help="Only check dependencies, don't install/build")
    parser.add_argument("--install-only", action="store_true",
                       help="Only install Python dependencies")
    parser.add_argument("--build-only", action="store_true",
                       help="Only build Python bindings")
    parser.add_argument("--test-only", action="store_true",
                       help="Only run tests (assumes bindings are built)")
    parser.add_argument("--help", "-h", action="store_true",
                       help="Show this help message")
    
    args = parser.parse_args()
    
    if args.help or len(sys.argv) == 1:
        print_help()
        return 0
    
    all_passed = True
    
    # Step 1: Check system requirements
    print("\n" + "="*60)
    print("NLM Python Setup")
    print("="*60)
    
    # Check Python dependencies
    missing_deps = check_python_dependencies()
    if missing_deps:
        print(f"  ✗ Missing dependencies: {missing_deps}")
        all_passed = False
    
    # Check CMake
    if not check_cmake():
        all_passed = False
    
    # Check compiler
    if not check_compiler():
        all_passed = False
    
    # Step 2: Install dependencies (if requested)
    if args.check_only or not missing_deps:
        print("\nAll system checks passed!")
        if args.check_only:
            print("Use --install-only, --build-only, or --test-only for specific steps")
        return 0 if all_passed else 1
    
    if not args.install_only and not args.build_only:
        if not install_dependencies():
            all_passed = False
    
    # Step 3: Create build structure
    if not args.build_only and not args.test_only:
        build_dir = create_build_structure()
    
    # Step 4: Build Python bindings
    if not args.test_only:
        if not build_python_bindings(build_dir):
            all_passed = False
    
    # Step 5: Run tests
    if not args.build_only:
        if not run_tests():
            all_passed = False
    
    # Final status
    print("\n" + "="*60)
    if all_passed:
        print("Setup completed successfully!")
        print("\nNext steps:")
        print("1. Python bindings are installed and ready to use")
        print("2. Run 'python -c \"import pynlm; print(pynlm.__version__)\"' to verify")
        print("3. Try the examples in the 'examples/' directory")
        return 0
    else:
        print("Setup completed with errors!")
        print("Please check the output above for details.")
        return 1

if __name__ == "__main__":
    sys.exit(main())