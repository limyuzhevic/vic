# Setup script for NLM Python bindings
# This provides additional configuration and build instructions for the Python package

import os
import sys
import subprocess
from pathlib import Path

def check_dependencies():
    """Check if required dependencies are available."""
    deps = ['pybind11', 'scikit-build-core']
    missing = []
    
    for dep in deps:
        try:
            __import__(dep.replace('-', '_'))
        except ImportError:
            missing.append(dep)
    
    if missing:
        print(f"Warning: Missing dependencies: {missing}")
        print("Install with: pip install {' '.join(missing)}")
        return False
    
    return True

def build_c_extensions():
    """Build C++ extensions using scikit-build."""
    try:
        # Check if we're in a git repository with the right structure
        if not os.path.exists('../src'):
            print("Error: Could not find NLM source directory (../src)")
            return False
        
        print("Building NLM C++ extensions...")
        
        # Run scikit-build
        result = subprocess.run([
            sys.executable, '-m', 'scikit_build_core.build'
        ], capture_output=True, text=True)
        
        if result.returncode != 0:
            print(f"Build failed: {result.stderr}")
            return False
        
        print("Build successful!")
        return True
        
    except Exception as e:
        print(f"Build failed: {e}")
        return False

def install_package():
    """Install the Python package."""
    try:
        # Install the package in development mode
        result = subprocess.run([
            sys.executable, '-m', 'pip', 'install', '-e', '.'
        ], capture_output=True, text=True)
        
        if result.returncode != 0:
            print(f"Installation failed: {result.stderr}")
            return False
        
        print("Package installed successfully!")
        return True
        
    except Exception as e:
        print(f"Installation failed: {e}")
        return False

def run_tests():
    """Run the package tests."""
    try:
        print("Running package tests...")
        result = subprocess.run([
            sys.executable, '-m', 'pytest', 'tests/', '-v'
        ], capture_output=True, text=True)
        
        print(result.stdout)
        if result.stderr:
            print("STDERR:", result.stderr)
        
        return result.returncode == 0
        
    except Exception as e:
        print(f"Test run failed: {e}")
        return False

def main():
    """Main entry point."""
    print("NLM Python Bindings Setup")
    print("=" * 40)
    
    # Check dependencies
    if not check_dependencies():
        print("\nNote: Some dependencies are missing but the package may still work.")
    
    # Parse command line arguments
    if len(sys.argv) > 1:
        command = sys.argv[1]
        
        if command == 'build':
            if build_c_extensions():
                print("\nSetup completed successfully!")
                return 0
            else:
                return 1
        
        elif command == 'install':
            if install_package():
                print("\nSetup completed successfully!")
                return 0
            else:
                return 1
        
        elif command == 'test':
            if run_tests():
                print("\nAll tests passed!")
                return 0
            else:
                print("\nSome tests failed!")
                return 1
        
        elif command == 'develop':
            # Build and install in development mode
            if build_c_extensions() and install_package():
                print("\nDevelopment setup completed successfully!")
                return 0
            else:
                return 1
        
        else:
            print(f"Unknown command: {command}")
            print("Available commands: build, install, test, develop")
            return 1
    
    # Default behavior: build and install
    print("Building C++ extensions...")
    if build_c_extensions():
        print("\nInstalling package...")
        if install_package():
            print("\nSetup completed successfully!")
            return 0
        else:
            return 1
    else:
        return 1

if __name__ == "__main__":
    sys.exit(main())