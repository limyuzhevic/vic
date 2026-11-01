#!/usr/bin/env python3
"""
Setup script for NLM Python package
Handles installation, compilation, and setup for development
"""

import os
import sys
import subprocess
import argparse
from pathlib import Path

def run_command(cmd, cwd=None, check=True):
    """Run a command and return the result"""
    print(f"Running: {' '.join(cmd)}")
    try:
        result = subprocess.run(cmd, cwd=cwd, check=check, 
                              capture_output=True, text=True)
        if result.stdout:
            print(result.stdout)
        return result
    except subprocess.CalledProcessError as e:
        print(f"Error: {e}")
        print(f"stderr: {e.stderr}")
        raise

def build_python_module():
    """Build the pynlm Python module"""
    print("Building pynlm Python module...")
    
    # Check if scikit-build is available
    try:
        import scikit_build_core
        print("scikit-build-core is available")
    except ImportError:
        print("Installing scikit-build-core...")
        run_command([sys.executable, "-m", "pip", "install", "scikit-build-core>=0.5.0"])
    
    # Check if pybind11 is available
    try:
        import pybind11
        print("pybind11 is available")
    except ImportError:
        print("Installing pybind11...")
        run_command([sys.executable, "-m", "pip", "install", "pybind11>=2.11.0"])
    
    # Build the Python module
    print("Building Python module using pyproject.toml...")
    run_command([sys.executable, "-m", "build"])
    
    print("Python module build complete!")

def install_dependencies():
    """Install Python dependencies"""
    print("Installing Python dependencies...")
    
    # Install main dependencies
    deps = [
        "numpy>=1.20",
        "scipy>=1.8.0", 
        "pandas>=1.5.0",
        "matplotlib>=3.5.0",
        "jupyter>=1.0.0",
    ]
    
    for dep in deps:
        print(f"Installing {dep}...")
        run_command([sys.executable, "-m", "pip", "install", dep])
    
    # Install development dependencies
    print("Installing development dependencies...")
    dev_deps = [
        "pytest>=7.0",
        "black>=22.0",
        "flake8>=5.0",
        "mypy>=0.991",
    ]
    
    for dep in dev_deps:
        print(f"Installing {dep}...")
        run_command([sys.executable, "-m", "pip", "install", dep])

def install_package():
    """Install NLM as a Python package"""
    print("Installing NLM as a Python package...")
    
    # Check if we're in development mode
    if "--dev" in sys.argv:
        print("Installing in development mode...")
        run_command([sys.executable, "-m", "pip", "install", "-e", "."])
    else:
        print("Installing in production mode...")
        run_command([sys.executable, "-m", "pip", "install", "."])
    
    print("NLM package installation complete!")

def create_virtual_environment():
    """Create a virtual environment for NLM"""
    print("Creating NLM virtual environment...")
    
    venv_name = "nlm_env"
    if os.path.exists(venv_name):
        print(f"Virtual environment '{venv_name}' already exists")
        response = input("Do you want to remove it and recreate? (y/n): ").strip().lower()
        if response != 'y':
            print("Keeping existing virtual environment")
            return
        else:
            print(f"Removing existing virtual environment '{venv_name}'...")
            run_command([sys.executable, "-m", "venv", "--clear", venv_name], check=False)
    
    # Create virtual environment
    run_command([sys.executable, "-m", "venv", venv_name])
    
    # Determine pip path
    if os.name == 'nt':  # Windows
        pip_path = os.path.join(venv_name, "Scripts", "pip")
    else:  # Unix-like
        pip_path = os.path.join(venv_name, "bin", "pip")
    
    # Activate virtual environment and install dependencies
    if os.name == 'nt':
        activate_script = os.path.join(venv_name, "Scripts", "activate.bat")
        pip_script = os.path.join(venv_name, "Scripts", "pip")
        python_script = os.path.join(venv_name, "Scripts", "python")
    else:
        activate_script = os.path.join(venv_name, "bin", "activate")
        pip_script = os.path.join(venv_name, "bin", "pip")
        python_script = os.path.join(venv_name, "bin", "python")
    
    print(f"Virtual environment created at {venv_name}")
    print(f"To activate: source {activate_script}")
    print(f"Or run: {pip_script} install -e .")

def build_native_library():
    """Build the native C++ library"""
    print("Building native C++ library...")
    
    # Create build directory
    build_dir = "build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
    
    # Configure CMake
    run_command(["cmake", ".."], cwd=build_dir)
    
    # Build the library
    run_command(["make", "-j4"], cwd=build_dir)
    
    print("Native library build complete!")

def test_installation():
    """Test the installation"""
    print("Testing NLM installation...")
    
    try:
        import pynlm
        print("✓ pynlm module imported successfully")
        
        # Test basic functionality
        config = pynlm.createDefaultConfig()
        print("✓ Default config created")
        
        brain = pynlm.createBrain(config)
        print("✓ Brain created")
        
        brain.initialize()
        print("✓ Brain initialized")n        
        for i in range(10):
            brain.step(i)
        
        print("✓ Brain simulation completed")
        print(f"✓ Total spikes: {brain.getTotalSpikeCount()}")
        
        # Test with world
        world = pynlm.createSimpleWorld()
        print("✓ World created")
        
        # Test with agent
        agent = pynlm.createAgentBrain(brain)
        print("✓ Agent created")
        
        print("\nAll tests passed! ✓")
        return True
        
    except Exception as e:
        print(f"✗ Installation test failed: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(description="NLM Setup Script")
    parser.add_argument("--skip-deps", action="store_true", help="Skip dependency installation")
    parser.add_argument("--skip-build", action="store_true", help="Skip building Python module")
    parser.add_argument("--skip-test", action="store_true", help="Skip installation test")
    parser.add_argument("--virtual-env", action="store_true", help="Create virtual environment")
    parser.add_argument("--build-native", action="store_true", help="Build native C++ library")
    parser.add_argument("--all", action="store_true", help="Do everything")
    parser.add_argument("--mode", choices=['dev', 'prod'], default='prod', help="Installation mode")
    
    args = parser.parse_args()
    
    print("=== NLM Setup Script ===")
    
    # Determine what to do based on arguments
    do_all = args.all
    skip_deps = args.skip_deps
    skip_build = args.skip_build
    skip_test = args.skip_test
    create_venv = args.virtual_env
    build_native = args.build_native
    
    if do_all:
        print("Performing complete setup...")
        if not skip_deps:
            install_dependencies()
        if not skip_build:
            build_python_module()
        if not skip_test:
            test_installation()
        if create_venv:
            create_virtual_environment()
        if build_native:
            build_native_library()
        install_package()
    else:
        # Do only what's explicitly requested
        if create_venv:
            create_virtual_environment()
        
        if not skip_deps:
            install_dependencies()
        
        if not skip_build:
            build_python_module()
        
        if build_native:
            build_native_library()
        
        install_package()
        
        if not skip_test:
            test_installation()
    
    print("\n=== Setup Complete ===")
    print("\nNext steps:")
    print("  1. Activate your environment (if using venv)")
    print("  2. Run 'python -c \"import pynlm; print(\"NLM loaded successfully!\")\"'")
    print("  3. Try one of the example scripts:")
    print("     - python scripts/demo.py      # Basic demo")
    print("     - python scripts/explore.py    # Explore features")
    print("     - python scripts/visualizer.py # Visualize simulations")
    print("     - python scripts/monitor.py   # Monitor performance")

if __name__ == "__main__":
    main()
